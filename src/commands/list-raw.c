#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "../apfs/io.h"
#include "../apfs/func/boolean.h"
#include "../apfs/func/cksum.h"
#include "../apfs/func/btree.h"

#include "../apfs/struct/object.h"
#include "../apfs/struct/nx.h"
#include "../apfs/struct/omap.h"
#include "../apfs/struct/fs.h"

#include "../apfs/struct/j.h"
#include "../apfs/struct/dstream.h"
#include "../apfs/struct/sibling.h"
#include "../apfs/struct/snap.h"

#include "../apfs/string/object.h"
#include "../apfs/string/nx.h"
#include "../apfs/string/omap.h"
#include "../apfs/string/btree.h"
#include "../apfs/string/fs.h"
#include "../apfs/string/j.h"

#include "../apfs/print-fs-records.h"

/**
 * Print usage info for this program.
 */
static void print_usage(int argc, char** argv) {
    fprintf(
        argc == 1 ? stdout: stderr,
        
        "Usage:   %s <container> <volume ID> <file-system object ID in volume>\n"
        "Example: %s /dev/disk0s2  0  0xd4a7f\n",
        
        argv[0],
        argv[0]    
    );
}

int cmd_list_raw(int argc, char** argv) {
    if (argc == 1) {
        print_usage(argc, argv);
        return 0;
    }
    
    setbuf(stdout, NULL);

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 4) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }
    
    nx_path = argv[1];

    uint32_t volume_id;
    bool parse_success = sscanf(argv[2], "%u", &volume_id);
    if (!parse_success) {
        fprintf(stderr, "%s is not a valid volume ID.\n", argv[2]);
        print_usage(argc, argv);
        return 1;
    }

    oid_t fs_oid;
    parse_success = sscanf(argv[3], "0x%" PRIx64 "", &fs_oid);
    if (!parse_success) {
        parse_success = sscanf(argv[3], "%" PRIu64, &fs_oid);
    }
    if (!parse_success) {
        fprintf(stderr, "%s is not a valid block address.\n", argv[3]);
        print_usage(argc, argv);
        return 1;
    }
    
    // Open (device special) file corresponding to an APFS container, read-only
    fprintf(stderr, "Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "\nABORT: ");
        report_fopen_error();
        return -errno;
    }
    fprintf(stderr, "OK.\nSimulating a mount of the APFS container.\n");
    
    // Using `nx_superblock_t*`, but allocating a whole block of memory.
    // This way, we can read the entire block and validate its checksum,
    // but still have direct access to the fields in `nx_superblock_t`
    // without needing to epxlicitly cast to that datatype.
    nx_superblock_t* nxsb = malloc(nx_block_size);
    if (!nxsb) {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory to create `nxsb`.\n");
        return -1;
    }

    if (read_blocks(nxsb, 0x0, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to successfully read block 0x0.\n");
        return -1;
    }

    fprintf(stderr, "Validating checksum of block 0x0 ... ");
    if (!is_cksum_valid(nxsb)) {
        fprintf(stderr, "FAILED.\n!! APFS ERROR !! Checksum of block 0x0 should validate, but it doesn't. Proceeding as if it does.\n");
    }
    fprintf(stderr, "OK.\n");

    if (!is_nx_superblock(nxsb)) {
        fprintf(stderr, "\nABORT: Block 0x0 isn't a container superblock.\n\n");
    }
    if (nxsb->nx_magic != NX_MAGIC) {
        fprintf(stderr, "!! APFS ERROR !! Container superblock at 0x0 doesn't have the correct magic number. Proceeding as if it does.\n");
    }

    fprintf(stderr, "Locating the checkpoint descriptor area:\n");
    
    uint32_t xp_desc_blocks = nxsb->nx_xp_desc_blocks & ~(1 << 31);
    fprintf(stderr, "- Its length is %u blocks.\n", xp_desc_blocks);

    char (*xp_desc)[nx_block_size] = malloc(xp_desc_blocks * nx_block_size);
    if (!xp_desc) {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory for %u blocks.\n", xp_desc_blocks);
        return -1;
    }

    if (nxsb->nx_xp_desc_blocks >> 31) {
        fprintf(stderr, "- It is not contiguous.\n");
        fprintf(stderr, "- The Physical OID of the B-tree representing it is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);
        fprintf(stderr, "END: The ability to handle this case has not yet been implemented.\n\n");   // TODO: implement case when xp_desc area is not contiguous
        return -1;
    } else {
        fprintf(stderr, "- It is contiguous.\n");
        fprintf(stderr, "- The address of its first block is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);

        fprintf(stderr, "Loading the checkpoint descriptor area into memory ... ");
        if (read_blocks(xp_desc, nxsb->nx_xp_desc_base, xp_desc_blocks) != xp_desc_blocks) {
            fprintf(stderr, "\nABORT: Failed to read all blocks in the checkpoint descriptor area.\n");
            return -1;
        }
        fprintf(stderr, "OK.\n");
    }

    fprintf(stderr, "Locating the most recent well-formed container superblock in the checkpoint descriptor area:\n");
    
    uint32_t i_latest_nx = 0;
    xid_t xid_latest_nx = 0;

    xid_t max_xid = ~0;     // `~0` is the highest possible XID

    for (uint32_t i = 0; i < xp_desc_blocks; i++) {
        if (!is_cksum_valid(xp_desc[i])) {
            fprintf(stderr, "- Block at index %u within this area failed checksum validation. Skipping it.\n", i);
            continue;
        }
        
        if (is_nx_superblock(xp_desc[i])) {
            if ( ((nx_superblock_t*)xp_desc[i])->nx_magic  !=  NX_MAGIC ) {
                fprintf(stderr, "- Container superblock at index %u within this area is malformed; incorrect magic number. Skipping it.\n", i);
                continue;
            }

            if (
                    ( ((nx_superblock_t*)xp_desc[i])->nx_o.o_xid  >  xid_latest_nx )
                    && ( ((nx_superblock_t*)xp_desc[i])->nx_o.o_xid  <= max_xid  )
            ) {
                i_latest_nx = i;
                xid_latest_nx = ((nx_superblock_t*)xp_desc[i])->nx_o.o_xid;
            }
        } else if (!is_checkpoint_map_phys(xp_desc[i])) {
            fprintf(stderr, "- Block at index %u within this area is not a container superblock or checkpoint map. Skipping it.\n", i);
            continue;
        }
    }

    if (xid_latest_nx == 0) {
        fprintf(stderr, "No container superblock with an XID that doesn't exceed 0x%" PRIx64 " exists in the checkpoint descriptor area.\n", max_xid);
        return -1;
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
    char (*xp)[nx_block_size] = malloc(nxsb->nx_xp_desc_len * nx_block_size);
    if (!xp) {
        fprintf(stderr, "\nABORT: Couldn't allocate sufficient memory.\n");
        return -1;
    }

    if (nxsb->nx_xp_desc_index + nxsb->nx_xp_desc_len <= xp_desc_blocks) {
        // The simple case: the checkpoint is already contiguous in `xp_desc`.
        memcpy(xp, xp_desc[nxsb->nx_xp_desc_index], nxsb->nx_xp_desc_len * nx_block_size);
    } else {
        // The case where the checkpoint wraps around from the end of the
        // checkpoint descriptor area to the start.
        uint32_t segment_1_len = xp_desc_blocks - nxsb->nx_xp_desc_index;
        uint32_t segment_2_len = nxsb->nx_xp_desc_len - segment_1_len;
        memcpy(xp,                 xp_desc + nxsb->nx_xp_desc_index, segment_1_len * nx_block_size);
        memcpy(xp + segment_1_len, xp_desc,                          segment_2_len * nx_block_size);
    }
    fprintf(stderr, "OK.\n");
    
    // We could `free(xp_desc)` at this point, but instead, we retain our copy
    // of the checkpoint descriptor area in case any of the Ephemeral objects
    // referenced by the current checkpoint are malformed; then, we can
    // retrieve an older checkpoint without having to read the checkpoint
    // descriptor area again.

    uint32_t xp_obj_len = 0;    // This variable will equal the number of
    // checkpoint-mappings = no. of Ephemeral objects used by this checkpoint.
    for (uint32_t i = 0; i < nxsb->nx_xp_desc_len; i++) {
        if (is_checkpoint_map_phys(xp[i])) {
            xp_obj_len += ((checkpoint_map_phys_t*)xp[i])->cpm_count;
        }
    }
    fprintf(stderr, "- There are %u checkpoint-mappings in this checkpoint.\n\n", xp_obj_len);

    fprintf(stderr, "Reading the Ephemeral objects used by this checkpoint ... ");
    char (*xp_obj)[nx_block_size] = malloc(xp_obj_len * nx_block_size);
    if (!xp_obj) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `xp_obj`.\n");
        return -1;
    }
    uint32_t num_read = 0;
    for (uint32_t i = 0; i < nxsb->nx_xp_desc_len; i++) {
        if (is_checkpoint_map_phys(xp[i])) {
            checkpoint_map_phys_t* xp_map = xp[i];  // Avoid lots of casting
            for (uint32_t j = 0; j < xp_map->cpm_count; j++) {
                if (read_blocks(xp_obj[num_read], xp_map->cpm_map[j].cpm_paddr, 1) != 1) {
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
    for (uint32_t i = 0; i < xp_obj_len; i++) {
        if (!is_cksum_valid(xp_obj[i])) {
            fprintf(stderr, "FAILED.\n");
            fprintf(stderr, "An Ephemeral object used by this checkpoint is malformed. Going back to look at the previous checkpoint instead.\n");
            
            // TODO: Handle case where data for a given checkpoint is malformed
            fprintf(stderr, "END: Handling of this case has not yet been implemented.\n");
            return -1;
        }
    }
    fprintf(stderr, "OK.\n");

    free(xp);
    free(xp_desc);

    fprintf(stderr, "The container superblock states that the container object map has Physical OID 0x%" PRIx64 ".\n", nxsb->nx_omap_oid);

    fprintf(stderr, "Loading the container object map ... ");
    omap_phys_t* nx_omap = malloc(nx_block_size);
    if (read_blocks(nx_omap, nxsb->nx_omap_oid, 1) != 1) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap`.\n");
        return -1;
    }
    fprintf(stderr, "OK.\n");
    
    fprintf(stderr, "Validating the container object map ... ");
    if (!is_cksum_valid(nx_omap)) {
        fprintf(stderr, "FAILED.\n");
        return -1;
    }
    fprintf(stderr, "OK.\n");

    if ((nx_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
        fprintf(stderr, "END: The container object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return -1;
    }

    fprintf(stderr, "Reading the root node of the container object map B-tree ... ");
    btree_node_phys_t* nx_omap_btree = malloc(nx_block_size);
    if (!nx_omap_btree) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap_btree`.\n");
        return -1;
    }
    if (read_blocks(nx_omap_btree, nx_omap->om_tree_oid, 1) != 1) {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", nx_omap->om_tree_oid);
        return -1;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the root node of the container object map B-tree ... ");
    if (!is_cksum_valid(nx_omap_btree)) {
        fprintf(stderr, "FAILED.\n");
    } else {
        fprintf(stderr, "OK.\n");
    }

    uint32_t num_file_systems = 0;
    for (uint32_t i = 0; i < NX_MAX_FILE_SYSTEMS; i++) {
        if (nxsb->nx_fs_oid[i] == 0) {
            break;
        }
        num_file_systems++;
    }
    fprintf(stderr, "The container superblock lists %u APFS volumes, whose superblocks have the following Virtual OIDs:\n", num_file_systems);
    for (uint32_t i = 0; i < num_file_systems; i++) {
        fprintf(stderr, "- 0x%" PRIx64 "\n", nxsb->nx_fs_oid[i]);
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "Reading the APFS volume superblocks ... ");
    char (*apsbs)[nx_block_size] = malloc(nx_block_size * num_file_systems);
    if (!apsbs) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `apsbs`.\n");
        return -1;
    }
    for (uint32_t i = 0; i < num_file_systems; i++) {
        omap_entry_t* fs_entry = get_btree_phys_omap_entry(nx_omap_btree, nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
        if (!fs_entry) {
            fprintf(stderr, "\nABORT: No objects with Virtual OID %#" PRIx64 " and maximum XID %#" PRIx64 " exist in `nx_omap_btree`.\n", nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
            return -1;
        }
        if (read_blocks(apsbs + i, fs_entry->val.ov_paddr, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_entry->val.ov_paddr);
            return -1;
        }
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the APFS volume superblocks ... ");
    for (uint32_t i = 0; i < num_file_systems; i++) {
        if (!is_cksum_valid(apsbs + i)) {
            fprintf(stderr, "FAILED.\n- The checksum of the APFS volume with OID 0x%" PRIx64 " did not validate.\n- Going back to look at the previous checkpoint instead.\n", nxsb->nx_fs_oid[i]);

            // TODO: Handle case where data for a given checkpoint is malformed
            fprintf(stderr, "END: Handling of this case has not yet been implemented.\n");
            return -1;
        }

        if ( ((apfs_superblock_t*)(apsbs + i))->apfs_magic  !=  APFS_MAGIC ) {
            fprintf(stderr, "FAILED.\n- The magic string of the APFS volume with OID 0x%" PRIx64 " did not validate.\n- Going back to look at the previous checkpoint instead.\n", nxsb->nx_fs_oid[i]);

            // TODO: Handle case where data for a given checkpoint is malformed
            fprintf(stderr, "END: Handling of this case has not yet been implemented.\n");
            return -1;
        }
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "\n Volume list\n================\n");
    for (uint32_t i = 0; i < num_file_systems; i++) {
        fprintf(stderr, "%2u: %s\n", i, ((apfs_superblock_t*)(apsbs + i))->apfs_volname);
    }

    if (volume_id >= num_file_systems) {
        fprintf(stderr, "The specified volume ID (%u) does not exist in the list above. Exiting.\n", volume_id);
        return -1;
    }
    apfs_superblock_t* apsb = apsbs + volume_id;

    fprintf(stderr, "The volume object map has Physical OID 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);

    fprintf(stderr, "Reading the volume object map ... ");
    omap_phys_t* fs_omap = malloc(nx_block_size);
    if (!fs_omap) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap`.\n");
        return -1;
    }
    if (read_blocks(fs_omap, apsb->apfs_omap_oid, 1) != 1) {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);
        return -1;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the volume object map ... ");
    if (!is_cksum_valid(fs_omap)) {
        fprintf(stderr, "\nFAILED. The checksum did not validate.");
        return -1;
    }
    fprintf(stderr, "OK.\n");

    if ((fs_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
        fprintf(stderr, "END: The volume object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return -1;
    }

    fprintf(stderr, "Reading the root node of the volume object map B-tree ... ");
    btree_node_phys_t* fs_omap_btree = malloc(nx_block_size);
    if (!fs_omap_btree) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap_btree`.\n");
        return -1;
    }
    if (read_blocks(fs_omap_btree, fs_omap->om_tree_oid, 1) != 1) {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_omap->om_tree_oid);
        return -1;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the root node of the volume object map B-tree ... ");
    if (!is_cksum_valid(fs_omap_btree)) {
        fprintf(stderr, "FAILED.\n");
    } else {
        fprintf(stderr, "OK.\n");
    }

    fprintf(stderr, "The file-system tree root for this volume has Virtual OID 0x%" PRIx64 ".\n", apsb->apfs_root_tree_oid);
    fprintf(stderr, "Looking up this Virtual OID in the volume object map ... ");
    omap_entry_t* fs_root_entry = get_btree_phys_omap_entry(fs_omap_btree, apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
    if (!fs_root_entry) {
        fprintf(stderr, "\nABORT: No objects with Virtual OID %#" PRIx64 " and maximum XID %#" PRIx64 " exist in `fs_omap_btree`.\n", apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
        return -1;
    }
    fprintf(stderr, "corresponding block address is 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);

    fprintf(stderr, "Reading ... ");
    btree_node_phys_t* fs_root_btree = malloc(nx_block_size);
    if (!fs_root_btree) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_root_btree`.\n");
        return -1;
    }
    if (read_blocks(fs_root_btree, fs_root_entry->val.ov_paddr, 1) != 1) {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);
        return -1;
    }
    free(fs_root_entry);  // No longer need the block address of the file-system root.

    fprintf(stderr, "validating ... ");
    if (!is_cksum_valid(fs_root_btree)) {
        fprintf(stderr, "FAILED.\n");
        return -1;
    }
    fprintf(stderr, "OK.\n");

    j_rec_t** fs_records = get_fs_records(fs_omap_btree, fs_root_btree, fs_oid, (xid_t)(~0) );
    if (!fs_records) {
        fprintf(stderr, "No records found with OID 0x%" PRIx64 ".\n", fs_oid);
        return -1;
    }

    fprintf(stderr, "\nRecords for file-system object %#" PRIx64 "\n", fs_oid);
    // `fs_records` now contains the records for the item at the specified path
    print_fs_records(fs_records);

    free_j_rec_array(fs_records);
    
    // TODO: RESUME HERE
    
    free(fs_omap_btree);
    free(fs_omap);

    // Closing statements; de-allocate all memory, close all file descriptors.
    free(apsbs);
    free(nx_omap_btree);
    free(nx_omap);
    free(xp_obj);
    free(nxsb);
    fclose(nx);
    fprintf(stderr, "END: All done.\n");
    return 0;
}
