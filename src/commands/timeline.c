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

#include <drat/string/object.h>
#include <drat/string/nx.h>
#include <drat/string/omap.h>
#include <drat/string/btree.h>
#include <drat/string/fs.h>
#include <drat/string/j.h>
#include <drat/utilities.h>

typedef struct file_metadata {
    uint64_t        mod_time;
    uint64_t        access_time;
    uint64_t        change_time;
    uint64_t        create_time;
    cp_key_class_t  default_protection_class;
    char            *filename;
    // cprotect xattr ?
} file_metadata;

file_metadata** timeline; 

static int fs_file_index = 0;
static int total_files = 0;

int compare_create(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    if (ptr_to_first->create_time > ptr_to_second->create_time) return 1;
    else if (ptr_to_first->create_time < ptr_to_second->create_time) return -1;
    else return 0;
}

int compare_mod(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    if (ptr_to_first->mod_time > ptr_to_second->mod_time) return 1;
    else if (ptr_to_first->mod_time < ptr_to_second->mod_time) return -1;
    else return 0;
}

int compare_change(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    if (ptr_to_first->change_time > ptr_to_second->change_time) return 1;
    else if (ptr_to_first->change_time < ptr_to_second->change_time) return -1;
    else return 0;
}

int compare_access(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    if (ptr_to_first->access_time > ptr_to_second->access_time) return 1;
    else if (ptr_to_first->access_time < ptr_to_second->access_time) return -1;
    else return 0;
}

void merge_files(file_metadata **fs_files, int size) {
    if (total_files == 0) {
        timeline = malloc(sizeof(file_metadata *) * size);
        for (int i = 0; i < size; i++) {
            timeline[i] = malloc(sizeof(file_metadata));
            timeline[i]->filename = fs_files[i]->filename;
            timeline[i]->create_time = fs_files[i]->create_time;
            timeline[i]->mod_time = fs_files[i]->mod_time;
            timeline[i]->change_time = fs_files[i]->change_time;
            timeline[i]->access_time = fs_files[i]->access_time;
            timeline[i]->default_protection_class = fs_files[i]->default_protection_class;
        }
    } else {
        timeline = realloc(timeline, sizeof(file_metadata *) * (total_files + size));
        for (int i = total_files, j = 0; j < size; i++, j++) {
            timeline[i] = malloc(sizeof(file_metadata));
            timeline[i]->filename = fs_files[j]->filename;
            timeline[i]->create_time = fs_files[j]->create_time;
            timeline[i]->mod_time = fs_files[j]->mod_time;
            timeline[i]->change_time = fs_files[j]->change_time;
            timeline[i]->access_time = fs_files[j]->access_time;
            timeline[i]->default_protection_class = fs_files[j]->default_protection_class;
        }
    }
    total_files += size;
}

void parse_fs(j_rec_t **fs_records, btree_node_phys_t *fs_omap_btree, btree_node_phys_t *fs_root_btree, file_metadata **fs_files, char *path_to_process, char *full_path)
{
    if (fs_records == NULL || fs_records[0] == NULL) {
        return;
    }
    int res = -1;
    char *entry = NULL;
    char *rest = NULL;
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
                //printf("Entering absolute directory: %s\n", new_full_path);
                parse_fs(get_fs_records(fs_omap_btree, fs_root_btree, dir_oid, (xid_t)(~0)), fs_omap_btree, fs_root_btree, fs_files, rest, new_full_path);
            }
        } else if (((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT) == APFS_TYPE_INODE) {
            oid_t file_oid = hdr->obj_id_and_type & OBJ_ID_MASK;
            j_rec_t **file_record = get_fs_records(fs_omap_btree, fs_root_btree, file_oid, (xid_t)(~0));
            print_fs_records(file_record);
            // get the inode value records
            j_rec_t* fs_rec = *file_record;
            j_key_t* hdr = fs_rec->data;
            j_inode_val_t* val = fs_rec->data + fs_rec->key_len;
            printf("Inode record type: %s\n", get_j_inode_type(val));
            char *fn = get_file_name(file_record);
            bool is_reg_file = strcmp(get_j_inode_type(val), "Regular file") == 0;
            if (fn && is_reg_file) {
                struct file_metadata *fs_file = malloc(sizeof(struct file_metadata));
                fs_file->filename = full_path;
                fs_file->create_time = val->create_time; 
                fs_file->mod_time = val->mod_time; 
                fs_file->change_time = val->change_time; 
                fs_file->access_time = val->access_time;
                fs_file->default_protection_class = val->default_protection_class;
                fs_files[fs_file_index++] = fs_file;
            }
        } else {
            continue;
        }
    }
}

/**
 * Print usage info for this program.
 */
static void print_usage(int argc, char **argv)
{
    fprintf(
        argc == 1 ? stdout : stderr,

        "Usage:   %s <container> <path in volume> <timestamp>\n"
        "Example: %s /dev/disk0s2 /Users/john/Documents access\n"
        "Available timestamps: modify, access, change, birth\n",
        argv[0],
        argv[0]);
}

int cmd_timeline(int argc, char **argv)
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
    
    char* path_stack = argv[2];
    char* timestamp = argv[3];
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
    struct file_metadata **fs_files;

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

                if (path_stack[strlen(path_stack) - 1] == '/'  && strcmp(path_stack, "/") != 0) {
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
                fs_files = malloc(sizeof(struct file_metadata *) * ((apfs_superblock_t *)apsb)->apfs_num_files);

                parse_fs(fs_records, fs_trees->fs_omap_btree, fs_trees->fs_root_btree, fs_files, path, path_builder);

                if (fs_files == NULL) {
                    fprintf(stderr, "Error reading volume files\n");
                    exit(-1);
                }
                merge_files(fs_files, fs_file_index);
                fs_file_index = 0;
                free(fs_trees->fs_omap_btree);
                free(fs_trees->fs_root_btree);
                free(fs_trees);
                free_j_rec_array(fs_records);
                free(fs_files);
                free(apsb);
            }
        }

    }
    
    printf("Sorting %d elements by timestamp...\n", total_files);
    int (*comparePtr)(file_metadata *, file_metadata *);
    int offset;
    if (strcmp(timestamp, "modify") == 0) {
        comparePtr = &compare_mod;
        offset = 0;
    } else if (strcmp(timestamp, "access") == 0) {
        comparePtr = &compare_access;
        offset = 1;
    } else if (strcmp(timestamp, "change") == 0) {
        comparePtr = &compare_change;
        offset = 2;
    } else if (strcmp(timestamp, "birth") == 0) {
        comparePtr = &compare_create;
        offset = 3;
    } else {
        fprintf(stderr, "Wrong timestamp requested\n");
        return -1;
    }
    for (int i = 0; i < total_files; i++) {
        printf("Filename:      %s\n",   timeline[i]->filename);
        printf("%s timestamp:  %s\n",   timestamp, apfs_timestamp_to_string(*(((uint64_t *)timeline[i])+offset)));
        printf("\n");
    }
    qsort(timeline, total_files, sizeof(file_metadata *), comparePtr);
    printf("======================================================================\n");
    printf("Timeline:\n");
    printf("======================================================================\n");
    for (int i = 0; i < total_files; i++) {
        printf("Filename:      %s\n",   timeline[i]->filename);
        printf("%s timestamp:  %s\n",   timestamp, apfs_timestamp_to_string(*(((uint64_t *)timeline[i])+offset)));
        printf("\n");
    }

    free(nxsb);
    fclose(nx);
    fprintf(stderr, "END: All done.\n");
    return 0;
}