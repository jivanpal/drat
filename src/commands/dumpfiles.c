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

static int fs_file_index = 0;

struct fs_browse_info {
    btree_node_phys_t *fs_omap_btree;
    btree_node_phys_t *fs_root_btree;
};

/**
 * Print usage info for this program.
 */
static void print_usage(int argc, char **argv)
{
    fprintf(
        argc == 1 ? stdout : stderr,

        "Usage:   %s <container> <volume ID> <path in volume> <output directory with trailing />\n"
        "Example: %s /dev/disk0s2  0  /private/var/mobile /Users/john/Desktop/\n",
        argv[0],
        argv[0]);
}

char *gen_directory(char *base_dir, char *fn, int xid) {
    char xid_str[10];
    sprintf(xid_str, "%d", xid);
    char *out = malloc(strlen(base_dir) + strlen(xid_str) + strlen(fn) + 3);
     // cannot import sys/stat.h to mkdir because of constant redefinition :(
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
                    //exit(-1);
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
                        for (uint64_t i = 0;   i < extent_len_blocks;   i++, block_addr++) {
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

struct fs_browse_info* get_root_fs_tree(nx_superblock_t* nxsb, uint32_t volume_id, char(*xp_desc)[nx_block_size], uint32_t xp_desc_blocks) {
    // Copy the contents of the checkpoint we are currently considering to its
    // own array for easy access. The checkpoint descriptor area is a ring
    // buffer stored as an array, so doing this also allows us to handle the
    // case where the checkpoint we're considering wraps around the ring buffer.
    fprintf(stderr, "Loading the corresponding checkpoint ... ");

    // The array `xp` will comprise the blocks in the checkpoint, in order.
    char(*xp)[nx_block_size] = malloc(nxsb->nx_xp_desc_len * nx_block_size);
    if (!xp)
    {
        fprintf(stderr, "\nABORT: Couldn't allocate sufficient memory.\n");
        return NULL;
    }

    if (nxsb->nx_xp_desc_index + nxsb->nx_xp_desc_len <= xp_desc_blocks)
    {
        // The simple case: the checkpoint is already contiguous in `xp_desc`.
        memcpy(xp, xp_desc[nxsb->nx_xp_desc_index], nxsb->nx_xp_desc_len * nx_block_size);
    }
    else
    {
        // The case where the checkpoint wraps around from the end of the
        // checkpoint descriptor area to the start.
        uint32_t segment_1_len = xp_desc_blocks - nxsb->nx_xp_desc_index;
        uint32_t segment_2_len = nxsb->nx_xp_desc_len - segment_1_len;
        memcpy(xp, xp_desc + nxsb->nx_xp_desc_index, segment_1_len * nx_block_size);
        memcpy(xp + segment_1_len, xp_desc, segment_2_len * nx_block_size);
    }
    fprintf(stderr, "OK.\n");

    // We could `free(xp_desc)` at this point, but instead, we retain our copy
    // of the checkpoint descriptor area in case any of the Ephemeral objects
    // referenced by the current checkpoint are malformed; then, we can
    // retrieve an older checkpoint without having to read the checkpoint
    // descriptor area again.

    uint32_t xp_obj_len = 0; // This variable will equal the number of
    // checkpoint-mappings = no. of Ephemeral objects used by this checkpoint.
    for (uint32_t i = 0; i < nxsb->nx_xp_desc_len; i++)
    {
        if (is_checkpoint_map_phys(xp[i]))
        {
            xp_obj_len += ((checkpoint_map_phys_t *)xp[i])->cpm_count;
        }
    }
    fprintf(stderr, "- There are %u checkpoint-mappings in this checkpoint.\n\n", xp_obj_len);

    free(xp);
    fprintf(stderr, "The container superblock states that the container object map has Physical OID 0x%" PRIx64 ".\n", nxsb->nx_omap_oid);

    fprintf(stderr, "Loading the container object map ... ");
    omap_phys_t *nx_omap = malloc(nx_block_size);
    if (read_blocks(nx_omap, nxsb->nx_omap_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap`.\n");
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the container object map ... ");
    if (!is_cksum_valid(nx_omap))
    {
        fprintf(stderr, "FAILED.\n");
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    if ((nx_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL)
    {
        fprintf(stderr, "END: The container object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return NULL;
    }

    fprintf(stderr, "Reading the root node of the container object map B-tree ... ");
    btree_node_phys_t *nx_omap_btree = malloc(nx_block_size);
    if (!nx_omap_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap_btree`.\n");
        return NULL;
    }
    if (read_blocks(nx_omap_btree, nx_omap->om_tree_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", nx_omap->om_tree_oid);
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the root node of the container object map B-tree ... ");
    if (!is_cksum_valid(nx_omap_btree))
    {
        fprintf(stderr, "FAILED.\n");
    }
    else
    {
        fprintf(stderr, "OK.\n");
    }

    uint32_t num_file_systems = 0;
    for (uint32_t i = 0; i < NX_MAX_FILE_SYSTEMS; i++)
    {
        if (nxsb->nx_fs_oid[i] == 0)
        {
            break;
        }
        num_file_systems++;
    }
    fprintf(stderr, "The container superblock lists %u APFS volumes, whose superblocks have the following Virtual OIDs:\n", num_file_systems);
    for (uint32_t i = 0; i < num_file_systems; i++)
    {
        fprintf(stderr, "- 0x%" PRIx64 "\n", nxsb->nx_fs_oid[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "Reading the APFS volume superblocks ... ");
    char(*apsbs)[nx_block_size] = malloc(nx_block_size * num_file_systems);
    if (!apsbs)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `apsbs`.\n");
        return NULL;
    }
    for (uint32_t i = 0; i < num_file_systems; i++)
    {
        omap_entry_t *fs_entry = get_btree_phys_omap_entry(nx_omap_btree, nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
        if (!fs_entry)
        {
            fprintf(stderr, "\nABORT: No objects with Virtual OID 0x%" PRIx64 " and maximum XID 0x%" PRIx64 " exist in `nx_omap_btree`.\n", nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
            return NULL;
        }
        if (read_blocks(apsbs + i, fs_entry->val.ov_paddr, 1) != 1)
        {
            fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_entry->val.ov_paddr);
            return NULL;
        }
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the APFS volume superblocks ... ");
    for (uint32_t i = 0; i < num_file_systems; i++)
    {
        if (!is_cksum_valid(apsbs + i))
        {
            fprintf(stderr, "FAILED.\n- The checksum of the APFS volume with OID 0x%" PRIx64 " did not validate.\n- Going back to look at the previous checkpoint instead.\n", nxsb->nx_fs_oid[i]);

            // TODO: Handle case where data for a given checkpoint is malformed
            fprintf(stderr, "END: Handling of this case has not yet been implemented.\n");
            return NULL;
        }

        if (((apfs_superblock_t *)(apsbs + i))->apfs_magic != APFS_MAGIC)
        {
            fprintf(stderr, "FAILED.\n- The magic string of the APFS volume with OID 0x%" PRIx64 " did not validate.\n- Going back to look at the previous checkpoint instead.\n", nxsb->nx_fs_oid[i]);

            // TODO: Handle case where data for a given checkpoint is malformed
            fprintf(stderr, "END: Handling of this case has not yet been implemented.\n");
            return NULL;
        }
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "\n Volume list\n================\n");
    for (uint32_t i = 0; i < num_file_systems; i++)
    {
        fprintf(stderr, "%2u: %s\n", i, ((apfs_superblock_t *)(apsbs + i))->apfs_volname);
    }

    if (volume_id >= num_file_systems)
    {
        fprintf(stderr, "The specified volume ID (%u) does not exist in the list above. Exiting.\n", volume_id);
        return NULL;
    }
    apfs_superblock_t *apsb = apsbs + volume_id;

    fprintf(stderr, "The volume object map has Physical OID 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);

    fprintf(stderr, "Reading the volume object map ... ");
    omap_phys_t *fs_omap = malloc(nx_block_size);
    if (!fs_omap)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap`.\n");
        return NULL;
    }
    if (read_blocks(fs_omap, apsb->apfs_omap_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the volume object map ... ");
    if (!is_cksum_valid(fs_omap))
    {
        fprintf(stderr, "\nFAILED. The checksum did not validate.");
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    if ((fs_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL)
    {
        fprintf(stderr, "END: The volume object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return NULL;
    }

    fprintf(stderr, "Reading the root node of the volume object map B-tree ... ");
    btree_node_phys_t *fs_omap_btree = malloc(nx_block_size);
    if (!fs_omap_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap_btree`.\n");
        return NULL;
    }
    if (read_blocks(fs_omap_btree, fs_omap->om_tree_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_omap->om_tree_oid);
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the root node of the volume object map B-tree ... ");
    if (!is_cksum_valid(fs_omap_btree))
    {
        fprintf(stderr, "FAILED.\n");
    }
    else
    {
        fprintf(stderr, "OK.\n");
    }

    fprintf(stderr, "The file-system tree root for this volume has Virtual OID 0x%" PRIx64 ".\n", apsb->apfs_root_tree_oid);
    fprintf(stderr, "Looking up this Virtual OID in the volume object map ... ");
    omap_entry_t *fs_root_entry = get_btree_phys_omap_entry(fs_omap_btree, apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
    if (!fs_root_entry)
    {
        fprintf(stderr, "\nABORT: No objects with Virtual OID 0x%" PRIx64 " and maximum XID 0x%" PRIx64 " exist in `fs_omap_btree`.\n", apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
        return NULL;
    }
    fprintf(stderr, "corresponding block address is 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);

    fprintf(stderr, "Reading ... ");
    btree_node_phys_t *fs_root_btree = malloc(nx_block_size);
    if (!fs_root_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_root_btree`.\n");
        return NULL;
    }
    if (read_blocks(fs_root_btree, fs_root_entry->val.ov_paddr, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);
        return NULL;
    }
    free(fs_root_entry); // No longer need the block address of the file-system root.

    fprintf(stderr, "validating ... ");
    if (!is_cksum_valid(fs_root_btree))
    {
        fprintf(stderr, "FAILED.\nGoing back to look at the previous checkpoint instead.\n");
        return NULL;
    }
    fprintf(stderr, "OK.\n");
    struct fs_browse_info* return_info = malloc(sizeof(struct fs_browse_info));
    return_info->fs_omap_btree = fs_omap_btree;
    return_info->fs_root_btree = fs_root_btree;
    free(fs_omap);
    free(apsbs);
    free(nx_omap);
    return return_info;
}


int cmd_dumpfiles(int argc, char **argv)
{
    setbuf(stdout, NULL);

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 5)
    {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }

    nx_path = argv[1];

    uint32_t volume_id;
    bool parse_success = sscanf(argv[2], "%u", &volume_id);
    if (!parse_success)
    {
        fprintf(stderr, "%s is not a valid volume ID.\n", argv[2]);
        print_usage(argc, argv);
        return 1;
    }

    char *path_stack = argv[3];
    char *outdir = argv[4];
    // Open (device special) file corresponding to an APFS container, read-only
    fprintf(stderr, "Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx)
    {
        fprintf(stderr, "\nABORT: ");
        report_fopen_error();
        return -errno;
    }
    fprintf(stderr, "OK.\nSimulating a mount of the APFS container.\n");

    // Using `nx_superblock_t*`, but allocating a whole block of memory.
    // This way, we can read the entire block and validate its checksum,
    // but still have direct access to the fields in `nx_superblock_t`
    // without needing to epxlicitly cast to that datatype.
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

    fprintf(stderr, "Validating checksum of block 0x0 ... ");
    if (!is_cksum_valid(nxsb))
    {
        fprintf(stderr, "FAILED.\n!! APFS ERROR !! Checksum of block 0x0 should validate, but it doesn't. Proceeding as if it does.\n");
    }
    fprintf(stderr, "OK.\n");
    if (!is_nx_superblock(nxsb))
    {
        fprintf(stderr, "\nABORT: Block 0x0 isn't a container superblock.\n\n");
    }
    if (nxsb->nx_magic != NX_MAGIC)
    {
        fprintf(stderr, "!! APFS ERROR !! Container superblock at 0x0 doesn't have the correct magic number. Proceeding as if it does.\n");
    }

    fprintf(stderr, "Locating the checkpoint descriptor area:\n");

    uint32_t xp_desc_blocks = nxsb->nx_xp_desc_blocks & ~(1 << 31);
    fprintf(stderr, "- Its length is %u blocks.\n", xp_desc_blocks);

    char(*xp_desc)[nx_block_size] = malloc(xp_desc_blocks * nx_block_size);
    if (!xp_desc)
    {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory for %u blocks.\n", xp_desc_blocks);
        return 1;
    }

    if (nxsb->nx_xp_desc_blocks >> 31)
    {
        fprintf(stderr, "- It is not contiguous.\n");
        fprintf(stderr, "- The Physical OID of the B-tree representing it is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);
        fprintf(stderr, "END: The ability to handle this case has not yet been implemented.\n\n"); // TODO: implement case when xp_desc area is not contiguous
        return 1;
    }
    else
    {
        fprintf(stderr, "- It is contiguous.\n");
        fprintf(stderr, "- The address of its first block is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);

        fprintf(stderr, "Loading the checkpoint descriptor area into memory ... ");
        if (read_blocks(xp_desc, nxsb->nx_xp_desc_base, xp_desc_blocks) != xp_desc_blocks)
        {
            fprintf(stderr, "\nABORT: Failed to read all blocks in the checkpoint descriptor area.\n");
            return 1;
        }
        fprintf(stderr, "OK.\n");
    }

    fprintf(stderr, "Locating the requested container superblock in the checkpoint descriptor area:\n");
    uint32_t i_target_nx = 0;
    xid_t xid_latest_nx = 0;
    uint32_t nx_indexes[xp_desc_blocks]; // collect all the valid indexes
    xid_t nx_xids[xp_desc_blocks];

    xid_t max_xid = ~0; // `~0` is the highest possible XID
    uint32_t j = 0;
    for (uint32_t i = 0; i < xp_desc_blocks; i++)
    {
        if (!is_cksum_valid(xp_desc[i]))
        {
            fprintf(stderr, "- Block at index %u within this area failed checksum validation. Skipping it.\n", i);
            continue;
        }
        if (is_nx_superblock(xp_desc[i]))
        {
            if (((nx_superblock_t *)xp_desc[i])->nx_magic != NX_MAGIC)
            {
                fprintf(stderr, "- Container superblock at index %u within this area is malformed; incorrect magic number. Skipping it.\n", i);
                continue;
            }
            printf("Found nxsb block at index %d with XID: %" PRIu64 "\n", i, ((nx_superblock_t *)xp_desc[i])->nx_o.o_xid);
            nx_indexes[j] = i; 
            nx_xids[j++] = ((nx_superblock_t *)xp_desc[i])->nx_o.o_xid;
        }
        else if (!is_checkpoint_map_phys(xp_desc[i]))
        {
            fprintf(stderr, "- Block at index %u within this area is not a container superblock or checkpoint map. Skipping it.\n", i);
            continue;
        }
    }

    if (j == 0)
    {
        fprintf(stderr, "No container superblock with an XID that doesn't exceed 0x%" PRIx64 " exists in the checkpoint descriptor area.\n", max_xid);
        return 1;
    }
    for (uint32_t i = 0; i < j; i++) {
        memcpy(nxsb, xp_desc[nx_indexes[i]], sizeof(nx_superblock_t));
        fprintf(stderr, "- It lies at index %u within the checkpoint descriptor area.\n", nx_indexes[i]);
        fprintf(stderr, "- The corresponding checkpoint starts at index %u within the checkpoint descriptor area, and spans %u blocks.\n\n", nxsb->nx_xp_desc_index, nxsb->nx_xp_desc_len);
        struct fs_browse_info *fs_trees = get_root_fs_tree(nxsb, volume_id, xp_desc, xp_desc_blocks);
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

        char *path = malloc(strlen(path_stack) + 1);
        if (!path)
        {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `path`.\n");
            return -1;
        }
        memcpy(path, path_stack, strlen(path_stack) + 1);

        printf("Entering directory /\n");
        char *path_builder = "";
        parse_fs_and_write(fs_records, fs_trees->fs_omap_btree, fs_trees->fs_root_btree, path, path_builder, outdir, nx_xids[i]);
        free(fs_trees->fs_omap_btree);
        free(fs_trees->fs_root_btree);
        free(fs_trees);
    }
    free(xp_desc);
    fprintf(stderr, "END: All done.\n");
    return 0;
}
