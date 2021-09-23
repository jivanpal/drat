#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <drat/time.h>
#include <apfs/object.h>
#include <apfs/nx.h>
#include <apfs/omap.h>
#include <apfs/fs.h>
#include <apfs/j.h>
#include <apfs/dstream.h>
#include <apfs/sibling.h>
#include <apfs/snap.h>

#include <drat/io.h>
#include <drat/print-fs-records.h>

#include <drat/func/boolean.h>
#include <drat/func/cksum.h>
#include <drat/func/btree.h>
#include <drat/func/j.h>

#include <drat/string/object.h>
#include <drat/string/nx.h>
#include <drat/string/omap.h>
#include <drat/string/btree.h>
#include <drat/string/fs.h>
#include <drat/string/j.h>
#include <drat/utilities.h>
#include <unistd.h>

/**
 * Print usage info for this program.
 */
static void print_usage(int argc, char **argv)
{
    fprintf(
        argc == 1 ? stdout : stderr,
        "Usage:   %s <container> <path in volume> <output directory>\n"
        "Example: %s /dev/disk0s2 /private/var/mobile /Users/john/Desktop\n",
        argv[0],
        argv[0]);
}

char *gen_directory(char *base_dir, char *fn, int xid) {
    char xid_str[10];
    sprintf(xid_str, "%d", xid);
    if (base_dir[strlen(base_dir) - 1] != '/') {
        const char slash = '/';
        strncat(base_dir, &slash, 1);
    }
    if (access(base_dir, F_OK) != 0) {
        fprintf(stderr, "The chosen output directory does not exist\n");
        exit(1);
    }
    char* out = malloc(strlen(base_dir) + strlen(xid_str) + strlen(fn) + 3);
    // cannot import sys/stat.h to mkdir because of constant redefinition
    sprintf(out, "%s_%s_%s", base_dir, xid_str, fn);
    return out;
}

void parse_fs_and_write(j_rec_t **fs_records, btree_node_phys_t *fs_omap_btree, btree_node_phys_t *fs_root_btree, char *path_to_process, char *full_path, char *outdir, xid_t xid)
{
    if (fs_records == NULL || fs_records[0] == NULL) {
        return;
    }
    int res = -1;
    char *entry = NULL;
    char *rest = NULL;
    char *result;
    if (path_to_process != NULL) {
        entry = get_first_entry(path_to_process, '/');
        rest = get_remaining_url(path_to_process, '/');
    }
    for (j_rec_t **fs_rec_cursor = fs_records; *fs_rec_cursor; fs_rec_cursor++) // for each directory entry...
    {
        j_rec_t *fs_rec = *fs_rec_cursor;
        j_key_t *hdr = fs_rec->data;
        if (((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT) == APFS_TYPE_DIR_REC)
        {
            //j_drec_key_t *key = fs_rec->data;
            j_drec_hashed_key_t *key = fs_rec->data;
            if (path_to_process != NULL) {
                res = strcmp((char *)key->name, entry);
            }
            if (res == 0 || path_to_process == NULL || strcmp(path_to_process, "/") == 0) {
                printf("Analysing record: %s\n", (char *)key->name);
                // diff between found index and start index
                signed int matching_record_index = fs_rec_cursor - fs_records;
                // Get the file ID of the matching record's target
                j_rec_t *fs_rec = fs_records[matching_record_index];
                j_drec_val_t *val = fs_rec->data + fs_rec->key_len;

                // Get the records for the target
                oid_t dir_oid = val->file_id;
                char *new_full_path = malloc(strlen(full_path) + strlen((char *)key->name) + 2);
                strncpy(new_full_path, full_path, strlen(full_path));
                const char* separator = "/";
                strncpy(new_full_path + strlen(full_path), separator, 1);
                strncpy(new_full_path + strlen(full_path) + 1, (char *)key->name, strlen((char *)key->name) + 1);
                //printf("Entering absolute path: %s\n", new_full_path);
                parse_fs_and_write(get_fs_records(fs_omap_btree, fs_root_btree, dir_oid, (xid_t)(~0)), fs_omap_btree, fs_root_btree, rest, new_full_path, outdir, xid);
            }
        } else if (((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT) == APFS_TYPE_INODE) {
            oid_t file_oid = hdr->obj_id_and_type & OBJ_ID_MASK;
            j_rec_t **file_record = get_fs_records(fs_omap_btree, fs_root_btree, file_oid, (xid_t)(~0));
            print_fs_records(file_record);
            // get the inode value records
            j_rec_t* fs_rec = *file_record;
            j_inode_val_t* inode = fs_rec->data + fs_rec->key_len;
            printf("Inode record type: %s\n", get_j_inode_type(inode));
            char *fn = get_file_name(file_record);
            bool is_reg_file = strcmp(get_j_inode_type(inode), "Regular file") == 0;
            result = gen_directory(outdir, fn, xid);
            if (fn && is_reg_file) {
                // Output content from all matching file extents
                char* buffer = malloc(nx_block_size);
                if (!buffer) {
                    fprintf(stderr, "Could not allocate sufficient memory for `buffer`.\n");
                    return;
                }
                uint64_t file_size = get_file_size(inode, fs_rec->val_len);
                printf("File size: %" PRIu64 "\n", file_size);
                if (file_size == 0) {
                    // Not a file, or file size couldn't be found; skip it in this case.
                    continue;
                }

                // Keep track of how many more bytes we need to output
                uint64_t bytes_remaining = file_size;

                bool found_file_extent = false;
                // Go through all the fs records
                for (j_rec_t** fs_rec_cursor = file_record; *fs_rec_cursor; fs_rec_cursor++) {
                    j_rec_t* fs_rec = *fs_rec_cursor;
                    j_key_t* hdr = fs_rec->data;

                    // Go through all the extent records
                    if ( ((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT)  ==  APFS_TYPE_FILE_EXTENT ) {                
                        found_file_extent = true;
                        j_file_extent_val_t* val = fs_rec->data + fs_rec->key_len;
                    
                        // Output the content from this particular file extent
                        uint64_t block_addr = val->phys_block_num;

                        uint64_t extent_len_blocks = (val->len_and_flags & J_FILE_EXTENT_LEN_MASK) / nx_block_size;
                        for (uint64_t i = 0; i < extent_len_blocks; i++, block_addr++) {
                            if (read_blocks(buffer, block_addr, 1) != 1) {
                                fprintf(stderr, "\n\nEncountered an error reading block %#" PRIx64 " (block %" PRIu64 " of %" PRIu64 "). Exiting.\n\n", block_addr, i+1, extent_len_blocks);
                                exit(-1);
                            }
                           
                            uint64_t bytes_to_write = nx_block_size;
                            if (bytes_remaining < nx_block_size) {
                                bytes_to_write = bytes_remaining;
                            }
                            
                            FILE *fp = fopen(result, "ab");
                            if (fwrite(buffer, bytes_to_write, 1, fp) != 1) {
                                fprintf(stderr, "\n\nEncountered an error writing block %" PRIu64 " of %" PRIu64 " to `stdout`. Exiting.\n\n", i+1, extent_len_blocks);
                                exit(-1);
                            }
                            fclose(fp);
                        
                            bytes_remaining -= bytes_to_write;

                            if (bytes_remaining == 0) {
                                break;  // Exit extent loop
                            }
                        }
                    }

                    if (bytes_remaining == 0) {
                        break;  // Exit fs record loop
                    }
                }
                if (!found_file_extent) {
                    fprintf(stderr, "Could not find any file extents for the specified path.\n");
                }
                free_j_rec_array(file_record);
            } else {
                continue;
            }
        }
    }
    free_j_rec_array(fs_records);
    free(result);
}

int cmd_dumpfiles(int argc, char **argv)
{
    setbuf(stdout, NULL);

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 4)
    {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }

    nx_path = argv[1];

    char *path_stack = argv[2];
    char *outdir = argv[3];
    // Open (device special) file corresponding to an APFS container, read-only
    fprintf(stderr, "Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx)
    {
        fprintf(stderr, "\nABORT: ");
        report_fopen_error();
        return -errno;
    }
    fseek(nx, 0, SEEK_END);
    long size = ftell(nx);
    fseek(nx, 0, SEEK_SET);
    printf("Image File size: %ld\n", size);
    nx_superblock_t *nxsb = malloc(nx_block_size);
    if (!nxsb)
    {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory to create `nxsb`.\n");
        return -1;
    }

    if (read_blocks(nxsb, 0x0, 1) != 1)
    {
        fprintf(stderr, "ABORT: Failed to successfully read block 0x0.\n");
        return -1;
    }

    fprintf(stderr, "Validating checksum of block 0x0 ...");
    if (!is_cksum_valid(nxsb))
    {
        fprintf(stderr, "FAILED.\n!! APFS ERROR !! Checksum of block 0x0 should validate, but it doesn't. Proceeding as if it does.\n");
    }
    if (!is_nx_superblock(nxsb))
    {
        fprintf(stderr, "\nABORT: Block 0x0 isn't a container superblock.\n\n");
    }
    if (nxsb->nx_magic != NX_MAGIC)
    {
        fprintf(stderr, "!! APFS ERROR !! Container superblock at 0x0 doesn't have the correct magic number. Proceeding as if it does.\n");
    }
    printf("OK\n");
    uint64_t nx_block_size = nxsb->nx_block_size;
    
    for (uint32_t block_index = 0; block_index < size/nx_block_size; block_index++) {
        char *apsb = malloc(nx_block_size);
        if (!apsb) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `apsbs`.\n");
            return 1;
        }
        if (read_blocks(apsb, block_index, 1) != 1)
        {
            fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIu32 ".\n", block_index);
            continue;
        }
        if (is_cksum_valid(apsb))
        {
            if (((apfs_superblock_t *)apsb)->apfs_magic == APFS_MAGIC)
            {
                printf("Found valid APSB block at addr 0x%" PRIu32 "\n", block_index);
                struct fs_browse_info *fs_trees = get_root_fs_tree(apsb, nx_block_size);
                if (fs_trees == NULL) {
                    fprintf(stderr, "Error getting the fs tree\n");
                    continue;
                }
                oid_t fs_oid = 0x2;
                // Start with root directory oid
                j_rec_t **fs_records = get_fs_records(fs_trees->fs_omap_btree, fs_trees->fs_root_btree, fs_oid, (xid_t)(~0));
                if (!fs_records)
                {
                    fprintf(stderr, "No records found with OID 0x%" PRIx64 ".\n", fs_oid);
                    return -1;
                }
                if (path_stack[strlen(path_stack) - 1] == '/' && strcmp(path_stack, "/") != 0) {
                    path_stack[strlen(path_stack) - 1] = '\0';
                }
                char *path = malloc(strlen(path_stack) + 1);
                if (!path)
                {
                    fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `path`.\n");
                    return -1;
                }
                memcpy(path, path_stack, strlen(path_stack) + 1);

                printf("Entering directory /\n");
                char *path_builder = "";
                parse_fs_and_write(fs_records, fs_trees->fs_omap_btree, fs_trees->fs_root_btree, path, path_builder, outdir, ((apfs_superblock_t *)apsb)->apfs_o.o_xid);
                free(fs_trees->fs_omap_btree);
                free(fs_trees->fs_root_btree);
                free(fs_trees);
                free(apsb);
            }
        }
    }
    fprintf(stderr, "END: All done.\n");
    return 0;
}
