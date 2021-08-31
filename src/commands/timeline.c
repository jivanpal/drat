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

typedef struct file_metadata {
    char            *filename;
    uint64_t        create_time;
    uint64_t        mod_time;
    uint64_t        change_time;
    uint64_t        access_time;
    cp_key_class_t  default_protection_class;
    // cprotect xattr ?
} file_metadata;

static int fs_file_index = 0;

int compare_create(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    return ptr_to_first->create_time - ptr_to_second->create_time;
}

int compare_mod(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    return ptr_to_first->mod_time - ptr_to_second->mod_time;
}

int compare_change(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    return ptr_to_first->change_time - ptr_to_second->change_time;
}

int compare_access(file_metadata *first, file_metadata *second) {
    struct file_metadata *ptr_to_first = *(file_metadata **)first;
    struct file_metadata *ptr_to_second = *(file_metadata **)second;
    return ptr_to_first->access_time - ptr_to_second->access_time;
}

const char* nth_strchr(const char* s, char c, int n) {
    int c_count;
    char* nth_ptr;
    for (c_count=1,nth_ptr=strchr(s,c); nth_ptr != NULL && c_count < n && c!=0; c_count++) {
         nth_ptr = strchr(nth_ptr+1, c);
    }
    return nth_ptr;
}

// /private/var/etc -> /var/etc -> /etc -> null
char *getRemainingUrl(char *url, char token) {
    const char *position_ptr = nth_strchr(url, token, 2);
    int position = (position_ptr == NULL ? -1 : position_ptr - url);

    if (position > 0) {
        char *newurl = malloc(strlen(url) - position);
        strncpy(newurl, url+position, strlen(url) - position + 1);
        return newurl;
    }
    return NULL;
}

// /private/var/etc -> private
char *getFirstEntry(char *url, char token) {
    char *left_position_ptr = strchr(url, token);
    if (left_position_ptr == NULL) {
        return NULL;
    }
    
    int left_position = left_position_ptr - url;
    const char *right_position_ptr = nth_strchr(url, token, 2);
    if (right_position_ptr == NULL) {
        char *entry = malloc(strlen(url));
        strncpy(entry, url + left_position + 1, strlen(url));
        return entry;
    }
    int right_position = right_position_ptr - url;
    char *entry = malloc(right_position - left_position - 1);
    strncpy(entry, url+left_position+1, right_position - left_position - 1);
    return entry;
}


void parse_fs(j_rec_t **fs_records, btree_node_phys_t *fs_omap_btree, btree_node_phys_t *fs_root_btree, struct file_metadata **fs_files, char *path, char *full_path)
{
    if (fs_records == NULL || fs_records[0] == NULL) {
        return;
    }
    int res = -1;
    char *entry = NULL;
    char *rest = NULL;
    if (path != NULL) {
        entry = getFirstEntry(path, '/');
        rest = getRemainingUrl(path, '/');
    }
    for (j_rec_t **fs_rec_cursor = fs_records; *fs_rec_cursor; fs_rec_cursor++) // for each directory entry...
    {
        j_rec_t *fs_rec = *fs_rec_cursor;
        j_key_t *hdr = fs_rec->data;
        if (((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT) == APFS_TYPE_DIR_REC)
        {
            j_drec_key_t *key = fs_rec->data;
            if (path != NULL) {
                res = strcmp((char *)key->name, entry);
            }
            if (res == 0 || path == NULL || strcmp(path, "/") == 0) {
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
                strncpy(new_full_path + strlen(full_path), "/", 1);
                strncpy(new_full_path + strlen(full_path) + 1, (char *)key->name, strlen((char *)key->name) + 1);
                //printf("Entering absolute directory: %s\n", new_full_path);
                parse_fs(get_fs_records(fs_omap_btree, fs_root_btree, dir_oid, (xid_t)(~0)), fs_omap_btree, fs_root_btree, fs_files, rest, new_full_path);
            }
        } else if (((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT) == APFS_TYPE_INODE){
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
    free_j_rec_array(fs_records);
}

/**
 * Print usage info for this program.
 */
static void print_usage(int argc, char **argv)
{
    fprintf(
        argc == 1 ? stdout : stderr,

        "Usage:   %s <container> <volume ID> <path in volume>\n"
        "Example: %s /dev/disk0s2  0  /Users/john/Documents access\n"
        "Available timestamps: modify, access, change, birth\n",
        argv[0],
        argv[0]);
}

int cmd_timeline(int argc, char **argv)
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
    char* timestamp = argv[4];

    uint32_t volume_id;
    bool parse_success = sscanf(argv[2], "%u", &volume_id);
    if (!parse_success)
    {
        fprintf(stderr, "%s is not a valid volume ID.\n", argv[2]);
        print_usage(argc, argv);
        return 1;
    }

    char *path_stack = argv[3];

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
        return -1;
    }

    if (nxsb->nx_xp_desc_blocks >> 31)
    {
        fprintf(stderr, "- It is not contiguous.\n");
        fprintf(stderr, "- The Physical OID of the B-tree representing it is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);
        fprintf(stderr, "END: The ability to handle this case has not yet been implemented.\n\n"); // TODO: implement case when xp_desc area is not contiguous
        return 0;
    }
    else
    {
        fprintf(stderr, "- It is contiguous.\n");
        fprintf(stderr, "- The address of its first block is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);

        fprintf(stderr, "Loading the checkpoint descriptor area into memory ... ");
        if (read_blocks(xp_desc, nxsb->nx_xp_desc_base, xp_desc_blocks) != xp_desc_blocks)
        {
            fprintf(stderr, "\nABORT: Failed to read all blocks in the checkpoint descriptor area.\n");
            return -1;
        }
        fprintf(stderr, "OK.\n");
    }

    fprintf(stderr, "Locating the most recent well-formed container superblock in the checkpoint descriptor area:\n");

    uint32_t i_latest_nx = 0;
    xid_t xid_latest_nx = 0;

    xid_t max_xid = ~0; // `~0` is the highest possible XID

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

            if (
                (((nx_superblock_t *)xp_desc[i])->nx_o.o_xid > xid_latest_nx) && (((nx_superblock_t *)xp_desc[i])->nx_o.o_xid <= max_xid))
            {
                i_latest_nx = i;
                xid_latest_nx = ((nx_superblock_t *)xp_desc[i])->nx_o.o_xid;
            }
        }
        else if (!is_checkpoint_map_phys(xp_desc[i]))
        {
            fprintf(stderr, "- Block at index %u within this area is not a container superblock or checkpoint map. Skipping it.\n", i);
            continue;
        }
    }

    if (xid_latest_nx == 0)
    {
        fprintf(stderr, "No container superblock with an XID that doesn't exceed 0x%" PRIx64 " exists in the checkpoint descriptor area.\n", max_xid);
        return 0;
    }

    // Don't need a copy of the block 0x0 NXSB which is stored in `nxsb`
    // anymore; replace that data with the latest NXSB.
    // This also lets us avoid repeatedly casting to `nx_superblock_t*`.
    memcpy(nxsb, xp_desc[i_latest_nx], sizeof(nx_superblock_t));

    fprintf(stderr, "- It lies at index %u within the checkpoint descriptor area.\n", i_latest_nx);
    fprintf(stderr, "- The corresponding checkpoint starts at index %u within the checkpoint descriptor area, and spans %u blocks.\n\n", nxsb->nx_xp_desc_index, nxsb->nx_xp_desc_len);

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
        return -1;
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

    fprintf(stderr, "Reading the Ephemeral objects used by this checkpoint ... ");
    char(*xp_obj)[nx_block_size] = malloc(xp_obj_len * nx_block_size);
    if (!xp_obj)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `xp_obj`.\n");
        return -1;
    }
    uint32_t num_read = 0;
    for (uint32_t i = 0; i < nxsb->nx_xp_desc_len; i++)
    {
        if (is_checkpoint_map_phys(xp[i]))
        {
            checkpoint_map_phys_t *xp_map = xp[i]; // Avoid lots of casting
            for (uint32_t j = 0; j < xp_map->cpm_count; j++)
            {
                if (read_blocks(xp_obj[num_read], xp_map->cpm_map[j].cpm_paddr, 1) != 1)
                {
                    fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", xp_map->cpm_map[j].cpm_paddr);
                    return -1;
                }
                num_read++;
            }
        }
    }
    fprintf(stderr, "OK.\n");
    assert(num_read = xp_obj_len);

    fprintf(stderr, "Validating the Ephemeral objects ... ");
    for (uint32_t i = 0; i < xp_obj_len; i++)
    {
        if (!is_cksum_valid(xp_obj[i]))
        {
            fprintf(stderr, "FAILED.\n");
            fprintf(stderr, "An Ephemeral object used by this checkpoint is malformed. Going back to look at the previous checkpoint instead.\n");

            // TODO: Handle case where data for a given checkpoint is malformed
            fprintf(stderr, "END: Handling of this case has not yet been implemented.\n");
            return 0;
        }
    }
    fprintf(stderr, "OK.\n");

    free(xp);
    free(xp_desc);

    fprintf(stderr, "The container superblock states that the container object map has Physical OID 0x%" PRIx64 ".\n", nxsb->nx_omap_oid);

    fprintf(stderr, "Loading the container object map ... ");
    omap_phys_t *nx_omap = malloc(nx_block_size);
    if (read_blocks(nx_omap, nxsb->nx_omap_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap`.\n");
        return -1;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the container object map ... ");
    if (!is_cksum_valid(nx_omap))
    {
        fprintf(stderr, "FAILED.\n");
        return 0;
    }
    fprintf(stderr, "OK.\n");

    if ((nx_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL)
    {
        fprintf(stderr, "END: The container object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return 0;
    }

    fprintf(stderr, "Reading the root node of the container object map B-tree ... ");
    btree_node_phys_t *nx_omap_btree = malloc(nx_block_size);
    if (!nx_omap_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap_btree`.\n");
        return -1;
    }
    if (read_blocks(nx_omap_btree, nx_omap->om_tree_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", nx_omap->om_tree_oid);
        return -1;
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
        return -1;
    }
    for (uint32_t i = 0; i < num_file_systems; i++)
    {
        omap_entry_t *fs_entry = get_btree_phys_omap_entry(nx_omap_btree, nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
        if (!fs_entry)
        {
            fprintf(stderr, "\nABORT: No objects with Virtual OID 0x%" PRIx64 " and maximum XID 0x%" PRIx64 " exist in `nx_omap_btree`.\n", nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
            return -1;
        }
        if (read_blocks(apsbs + i, fs_entry->val.ov_paddr, 1) != 1)
        {
            fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_entry->val.ov_paddr);
            return -1;
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
            return 0;
        }

        if (((apfs_superblock_t *)(apsbs + i))->apfs_magic != APFS_MAGIC)
        {
            fprintf(stderr, "FAILED.\n- The magic string of the APFS volume with OID 0x%" PRIx64 " did not validate.\n- Going back to look at the previous checkpoint instead.\n", nxsb->nx_fs_oid[i]);

            // TODO: Handle case where data for a given checkpoint is malformed
            fprintf(stderr, "END: Handling of this case has not yet been implemented.\n");
            return 0;
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
        return 0;
    }
    apfs_superblock_t *apsb = apsbs + volume_id;

    fprintf(stderr, "The volume object map has Physical OID 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);

    fprintf(stderr, "Reading the volume object map ... ");
    omap_phys_t *fs_omap = malloc(nx_block_size);
    if (!fs_omap)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap`.\n");
        return -1;
    }
    if (read_blocks(fs_omap, apsb->apfs_omap_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);
        return -1;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the volume object map ... ");
    if (!is_cksum_valid(fs_omap))
    {
        fprintf(stderr, "\nFAILED. The checksum did not validate.");
        return 0;
    }
    fprintf(stderr, "OK.\n");

    if ((fs_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL)
    {
        fprintf(stderr, "END: The volume object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return 0;
    }

    fprintf(stderr, "Reading the root node of the volume object map B-tree ... ");
    btree_node_phys_t *fs_omap_btree = malloc(nx_block_size);
    if (!fs_omap_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap_btree`.\n");
        return -1;
    }
    if (read_blocks(fs_omap_btree, fs_omap->om_tree_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_omap->om_tree_oid);
        return -1;
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
        return -1;
    }
    fprintf(stderr, "corresponding block address is 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);

    fprintf(stderr, "Reading ... ");
    btree_node_phys_t *fs_root_btree = malloc(nx_block_size);
    if (!fs_root_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_root_btree`.\n");
        return -1;
    }
    if (read_blocks(fs_root_btree, fs_root_entry->val.ov_paddr, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);
        return -1;
    }
    free(fs_root_entry); // No longer need the block address of the file-system root.

    fprintf(stderr, "validating ... ");
    if (!is_cksum_valid(fs_root_btree))
    {
        fprintf(stderr, "FAILED.\nGoing back to look at the previous checkpoint instead.\n");
        return 0;
    }
    fprintf(stderr, "OK.\n");

    oid_t fs_oid = 0x2;
    // Start with root directory oid
    j_rec_t **fs_records = get_fs_records(fs_omap_btree, fs_root_btree, fs_oid, (xid_t)(~0));
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

    struct file_metadata **fs_files = malloc(sizeof(struct file_metadata *) * apsb->apfs_num_files);
    printf("Entering directory /\n");
    char *path_builder = "";
    parse_fs(fs_records, fs_omap_btree, fs_root_btree, fs_files, path, path_builder);
    printf("Sorting %d elements by timestamp...\n", fs_file_index);
    int (*comparePtr)(file_metadata *, file_metadata *);
    if (strcmp(timestamp, "modify")) {
        comparePtr = &compare_mod;
    } else if (strcmp(timestamp, "access")) {
        comparePtr = &compare_access;
    } else if (strcmp(timestamp, "change")) {
        comparePtr = &compare_change;
    } else if (strcmp(timestamp, "birth")) {
        comparePtr = &compare_create;
    } else {
        fprintf(stderr, "Wrong timestamp requested\n");
    }
    qsort(fs_files, fs_file_index, sizeof(file_metadata *), comparePtr);
    printf("======================================================================\n");
    printf("Timeline:\n");
    printf("======================================================================\n");
    for (int i = 0; i < fs_file_index; i++) {
        printf("Filename:      %s\n",   fs_files[i]->filename);
        printf("%s timestamp:  %s\n",   timestamp, apfs_timestamp_to_string(fs_files[i]->create_time));
        printf("\n");
    }

    free(fs_omap_btree);
    free(fs_omap);
    free(apsbs);
    free(nx_omap_btree);
    free(nx_omap);
    free(xp_obj);
    free(nxsb);
    fclose(nx);
    fprintf(stderr, "END: All done.\n");
    return 0;
}
