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

/**
 * Print usage info for this program.
 */
static void print_usage(int argc, char** argv) {
    fprintf(
        argc == 1 ? stdout : stderr,
        
        "Usage:   %s <container>\n"
        "Example: %s /dev/disk0s2\n",

        argv[0],
        argv[0]
    );
}

int cmd_resolver(int argc, char** argv) {
    if (argc == 1) {
        print_usage(argc, argv);
        return 0;
    }
    
    setbuf(stdout, NULL);

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }
    nx_path = argv[1];
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "\nABORT: ");
        report_fopen_error();
        printf("\n");
        return -errno;
    }
    printf("OK.\nSimulating a mount of the APFS container.\n");
    
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

    printf("Validating checksum of block 0x0 ... ");
    if (is_cksum_valid(nxsb)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\n!! APFS ERROR !! Checksum of block 0x0 should validate, but it doesn't. Proceeding as if it does.\n");
    }

    printf("\nDetails of block 0x0:\n");
    printf("--------------------------------------------------------------------------------\n");
    print_nx_superblock(nxsb);
    printf("--------------------------------------------------------------------------------\n");
    printf("\n");

    if (!is_nx_superblock(nxsb)) {
        printf("!! APFS ERROR !! Block 0x0 should be a container superblock, but it isn't. Proceeding as if it is.\n\n");
    }
    if (nxsb->nx_magic != NX_MAGIC) {
        printf("!! APFS ERROR !! Container superblock at 0x0 doesn't have the correct magic number. Proceeding as if it does.\n");
    }

    printf("Locating the checkpoint descriptor area:\n");
    
    uint32_t xp_desc_blocks = nxsb->nx_xp_desc_blocks & ~(1 << 31);
    printf("- Its length is %u blocks.\n", xp_desc_blocks);

    char (*xp_desc)[nx_block_size] = malloc(xp_desc_blocks * nx_block_size);
    if (!xp_desc) {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory for %u blocks.\n", xp_desc_blocks);
        return -1;
    }

    if (nxsb->nx_xp_desc_blocks >> 31) {
        printf("- It is not contiguous.\n");
        printf("- The Physical OID of the B-tree representing it is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);
        printf("END: The ability to handle this case has not yet been implemented.\n\n");   // TODO: implement case when xp_desc area is not contiguous
        return 0;
    } else {
        printf("- It is contiguous.\n");
        printf("- The address of its first block is 0x%" PRIx64 ".\n", nxsb->nx_xp_desc_base);

        printf("Loading the checkpoint descriptor area into memory ... ");
        if (read_blocks(xp_desc, nxsb->nx_xp_desc_base, xp_desc_blocks) != xp_desc_blocks) {
            fprintf(stderr, "\nABORT: Failed to read all blocks in the checkpoint descriptor area.\n");
            return -1;
        }
        printf("OK.\n");
    }

    printf("Locating the most recent well-formed container superblock in the checkpoint descriptor area:\n");
    
    uint32_t i_latest_nx = 0;
    xid_t xid_latest_nx = 0;

    xid_t max_xid = ~0;     // `~0` is the highest possible XID

    for (uint32_t i = 0; i < xp_desc_blocks; i++) {
        if (!is_cksum_valid(xp_desc[i])) {
            printf("- !! APFS WARNING !! Block at index %u within this area failed checksum validation. Skipping it.\n", i);
            continue;
        }
        
        if (is_nx_superblock(xp_desc[i])) {
            if ( ((nx_superblock_t*)xp_desc[i])->nx_magic  !=  NX_MAGIC ) {
                printf("- !! APFS WARNING !! Container superblock at index %u within this area is malformed; incorrect magic number. Skipping it.\n", i);
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
            printf("- !! APFS ERROR !! Block at index %u within this area is not a container superblock or checkpoint map. Skipping it.\n", i);
            continue;
        }
    }

    if (xid_latest_nx == 0) {
        printf("No container superblock with an XID that doesn't exceed 0x%" PRIx64 " exists in the checkpoint descriptor area.\n", max_xid);
        return 0;
    }

    // Don't need a copy of the block 0x0 NXSB which is stored in `nxsb`
    // anymore; replace that data with the latest NXSB.
    // This also lets us avoid repeatedly casting to `nx_superblock_t*`.
    memcpy(nxsb, xp_desc[i_latest_nx], sizeof(nx_superblock_t));

    printf("- It lies at index %u within the checkpoint descriptor area.\n", i_latest_nx);

    printf("\nDetails of this container superblock:\n");
    printf("--------------------------------------------------------------------------------\n");
    print_nx_superblock(nxsb);
    printf("--------------------------------------------------------------------------------\n");
    printf("- The corresponding checkpoint starts at index %u within the checkpoint descriptor area, and spans %u blocks.\n\n", nxsb->nx_xp_desc_index, nxsb->nx_xp_desc_len);

    // Copy the contents of the checkpoint we are currently considering to its
    // own array for easy access. The checkpoint descriptor area is a ring
    // buffer stored as an array, so doing this also allows us to handle the
    // case where the checkpoint we're considering wraps around the ring buffer.
    printf("Loading the corresponding checkpoint ... ");
    
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
    printf("OK.\n");
    
    // We could `free(xp_desc)` at this point, but instead, we retain our copy
    // of the checkpoint descriptor area in case any of the Ephemeral objects
    // referenced by the current checkpoint are malformed; then, we can
    // retrieve an older checkpoint without having to read the checkpoint
    // descriptor area again.

    printf("\nDetails of each block in this checkpoint:\n");
    printf("--------------------------------------------------------------------------------\n");
    for (uint32_t i = 0; i < nxsb->nx_xp_desc_len; i++) {
        if (is_nx_superblock(xp[i])) {
            print_nx_superblock(xp[i]);
        } else {
            assert(is_checkpoint_map_phys(xp[i]));
            print_checkpoint_map_phys(xp[i]);
        }
        printf("--------------------------------------------------------------------------------\n");
    }

    uint32_t xp_obj_len = 0;    // This variable will equal the number of
    // checkpoint-mappings = no. of Ephemeral objects used by this checkpoint.
    printf("\nDetails of each checkpoint-mapping in this checkpoint:\n");
    printf("--------------------------------------------------------------------------------\n");
    for (uint32_t i = 0; i < nxsb->nx_xp_desc_len; i++) {
        if (is_checkpoint_map_phys(xp[i])) {
            print_checkpoint_map_phys_mappings(xp[i]);
            xp_obj_len += ((checkpoint_map_phys_t*)xp[i])->cpm_count;
        }
    }
    printf("- There are %u checkpoint-mappings in this checkpoint.\n\n", xp_obj_len);

    printf("Reading the Ephemeral objects used by this checkpoint ... ");
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
    printf("OK.\n");
    assert(num_read = xp_obj_len);

    printf("Validating the Ephemeral objects ... ");
    for (uint32_t i = 0; i < xp_obj_len; i++) {
        if (!is_cksum_valid(xp_obj[i])) {
            printf("FAILED.\n");
            printf("An Ephemeral object used by this checkpoint is malformed. Going back to look at the previous checkpoint instead.\n");
            
            // TODO: Handle case where data for a given checkpoint is malformed
            printf("END: Handling of this case has not yet been implemented.\n");
            return 0;
        }
    }
    printf("OK.\n");

    free(xp);
    free(xp_desc);

    printf("\nDetails of the Ephemeral objects:\n");
    printf("--------------------------------------------------------------------------------\n");
    for (uint32_t i = 0; i < xp_obj_len; i++) {
        print_obj_phys(xp_obj[i]);
        printf("--------------------------------------------------------------------------------\n");
    }
    printf("\n");

    printf("The container superblock states that the container object map has Physical OID 0x%" PRIx64 ".\n", nxsb->nx_omap_oid);

    printf("Loading the container object map ... ");
    omap_phys_t* nx_omap = malloc(nx_block_size);
    if (read_blocks(nx_omap, nxsb->nx_omap_oid, 1) != 1) {
        printf("\nABORT: Could not allocate sufficient memory for `nx_omap`.\n");
        return -1;
    }
    printf("OK.\n");
    
    printf("Validating the container object map ... ");
    if (!is_cksum_valid(nx_omap)) {
        printf("FAILED.\n");
        printf("This container object map is malformed. Going back to look at the previous checkpoint instead.\n");
        
        // TODO: Handle case where a given container object map is malformed
        printf("END: Handling of this case has not yet been implemented.\n");
        return 0;
    }
    printf("OK.\n");

    printf("\nDetails of the container object map:\n");
    printf("--------------------------------------------------------------------------------\n");
    print_omap_phys(nx_omap);
    printf("--------------------------------------------------------------------------------\n");
    printf("\n");

    if ((nx_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
        printf("END: The container object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return 0;
    }

    printf("Reading the root node of the container object map B-tree ... ");
    btree_node_phys_t* nx_omap_btree = malloc(nx_block_size);
    if (!nx_omap_btree) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap_btree`.\n");
        return -1;
    }
    if (read_blocks(nx_omap_btree, nx_omap->om_tree_oid, 1) != 1) {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", nx_omap->om_tree_oid);
        return -1;
    }
    printf("OK.\n");

    printf("Validating the root node of the container object map B-tree ... ");
    if (!is_cksum_valid(nx_omap_btree)) {
        printf("FAILED.\n");
    } else {
        printf("OK.\n");
    }

    printf("\nDetails of the container object map B-tree:\n");
    printf("--------------------------------------------------------------------------------\n");
    print_btree_node_phys(nx_omap_btree);
    printf("--------------------------------------------------------------------------------\n");
    printf("\n");

    uint32_t num_file_systems = 0;
    for (uint32_t i = 0; i < NX_MAX_FILE_SYSTEMS; i++) {
        if (nxsb->nx_fs_oid[i] == 0) {
            break;
        }
        num_file_systems++;
    }
    printf("The container superblock lists %u APFS volumes, whose superblocks have the following Virtual OIDs:\n", num_file_systems);
    for (uint32_t i = 0; i < num_file_systems; i++) {
        printf("- 0x%" PRIx64 "\n", nxsb->nx_fs_oid[i]);
    }
    printf("\n");

    printf("Reading the APFS volume superblocks ... ");
    char (*apsbs)[nx_block_size] = malloc(nx_block_size * num_file_systems);
    if (!apsbs) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `apsbs`.\n");
        return -1;
    }
    for (uint32_t i = 0; i < num_file_systems; i++) {
        omap_entry_t* fs_entry = get_btree_phys_omap_entry(nx_omap_btree, nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
        if (!fs_entry) {
            fprintf(stderr, "\nABORT: No objects with Virtual OID 0x%" PRIx64 " and maximum XID 0x%" PRIx64 " exist in `nx_omap_btree`.\n", nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
            return -1;
        }
        if (read_blocks(apsbs + i, fs_entry->val.ov_paddr, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_entry->val.ov_paddr);
            return -1;
        }
    }
    printf("OK.\n");

    printf("Validating the APFS volume superblocks ... ");
    for (uint32_t i = 0; i < num_file_systems; i++) {
        if (!is_cksum_valid(apsbs + i)) {
            printf("FAILED.\n- The checksum of the APFS volume with OID 0x%" PRIx64 " did not validate.\n- Going back to look at the previous checkpoint instead.\n", nxsb->nx_fs_oid[i]);

            // TODO: Handle case where data for a given checkpoint is malformed
            printf("END: Handling of this case has not yet been implemented.\n");
            return 0;
        }

        if ( ((apfs_superblock_t*)(apsbs + i))->apfs_magic  !=  APFS_MAGIC ) {
            printf("FAILED.\n- The magic string of the APFS volume with OID 0x%" PRIx64 " did not validate.\n- Going back to look at the previous checkpoint instead.\n", nxsb->nx_fs_oid[i]);

            // TODO: Handle case where data for a given checkpoint is malformed
            printf("END: Handling of this case has not yet been implemented.\n");
            return 0;
        }
    }
    printf("OK.\n");

    printf("\nDetails of these volume superblocks:\n");
    printf("--------------------------------------------------------------------------------\n");
    for (uint32_t i = 0; i < num_file_systems; i++) {
        print_apfs_superblock(apsbs + i);
        printf("--------------------------------------------------------------------------------\n");
    }
    printf("\n");

    printf("\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("\n");
    
    uint32_t i = 0;
    
    apfs_superblock_t* apsb = apsbs + i;
    printf("Simulating a mount of volume %u (%s).\n", i, apsb->apfs_volname);
    printf("\n");

    printf("The volume object map has Physical OID 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);

    printf("Reading the volume object map ... ");
    omap_phys_t* fs_omap = malloc(nx_block_size);
    if (!fs_omap) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap`.\n");
        return -1;
    }
    if (read_blocks(fs_omap, apsb->apfs_omap_oid, 1) != 1) {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);
        return -1;
    }
    printf("OK.\n");

    printf("Validating the volume object map ... ");
    if (!is_cksum_valid(fs_omap)) {
        printf("FAILED.\n- The checksum did not validate.\n- Going back to look at the previous checkpoint instead.\n");

        // TODO: Handle case where data for a given checkpoint is malformed
        printf("END: Handling of this case has not yet been implemented.\n");
        return 0;
    }
    printf("OK.\n");

    printf("\nDetails of the volume object map:\n");
    printf("--------------------------------------------------------------------------------\n");
    print_omap_phys(fs_omap);
    printf("--------------------------------------------------------------------------------\n");
    printf("\n");

    if ((fs_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
        printf("END: The volume object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return 0;
    }

    printf("Reading the root node of the volume object map B-tree ... ");
    btree_node_phys_t* fs_omap_btree = malloc(nx_block_size);
    if (!fs_omap_btree) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap_btree`.\n");
        return -1;
    }
    if (read_blocks(fs_omap_btree, fs_omap->om_tree_oid, 1) != 1) {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_omap->om_tree_oid);
        return -1;
    }
    printf("OK.\n");

    printf("Validating the root node of the volume object map B-tree ... ");
    if (!is_cksum_valid(fs_omap_btree)) {
        printf("FAILED.\n");
    } else {
        printf("OK.\n");
    }

    printf("\nDetails of the volume object map B-tree:\n");
    printf("--------------------------------------------------------------------------------\n");
    print_btree_node_phys(fs_omap_btree);
    printf("--------------------------------------------------------------------------------\n");
    printf("\n");

    size_t NUM_RECORDS = 62;
    uint64_t record_data[][4] =  {
        {   0x8,    0xb46a8,    0xd6cf2,          0xa3bc },
        {   0x9,    0xb4703,    0xd4ba8,         0x77e69 },
        {   0x9,    0xb471d,    0xd0add,          0xa3c0 },
        {   0x4,    0xb4746,    0xe14ec,          0xa3c2 },
        {   0x6,    0xb4784,    0xa9a58,          0xa3c5 },
        {   0x6,    0xb4908,    0xb6c56,          0xa3c6 },
        {   0x3,    0xb494c,    0xd47c4,          0xa3c6 },
        {   0x3,    0xb4999,    0xb7292,          0xa3db },
        {   0x3,    0xb49a6,    0xac8b2,          0xa3dc },
        {   0x3,    0xb49aa,    0xd668d,        0x12394a },
        {   0x8,    0xb4a57,    0xd60f4,          0xa3e0 },
        {   0x6,    0xb4b0d,    0xd67ec,          0xa3ee },
        {   0x3,    0xb4b1c,    0xd6941,          0xa3ef },
        {   0x3,    0xb4b24,    0xd6a18,          0xa3f0 },
        {   0x3,    0xb4b34,    0xd69fb,          0xa3f1 },
        {   0x3,    0xb4b43,    0xd3d06,          0xa3f2 },
        {   0x8,    0xb4b52,    0xd68b5,          0xa3f3 },
        {   0x3,    0xb4b64,    0xd683c,          0xa3f4 },
        {   0x3,    0xb4b72,    0xd6848,          0xa3f6 },
        {   0x3,    0xb4b83,    0xd6820,          0xa3f7 },
        {   0x9,    0xb4b9f,    0xd6865,          0xa3f9 },
        {   0x9,    0xb4bae,    0xd67ed,          0xa3fa },
        {   0x3,    0xb4bbd,    0xd6ac1,          0xa3fb },
        {   0x9,    0xb4bea,    0xd5b73,          0xa3fd },
        {   0x8,    0xb4c50,    0xf3864,          0xa3ff },
        {   0x4,    0xb4c63,    0xd4393,          0xa403 },
        {   0x3,    0xb4c79,    0xd3a9d,          0xa405 },
        {   0x3,    0xb4c8d,    0xd4392,          0xa406 },
        {   0x3,    0xb4ca1,    0xd4395,          0xa407 },
        {   0x3,    0xb4cb7,    0xd4372,          0xa409 },
        {   0x3,    0xb4cc9,    0xd437d,          0xa40a },
        {   0x3,    0xb4cdc,    0xd4381,          0xa40b },
        {   0x3,    0xb4cf2,    0xd436a,          0xa40c },
        {   0x3,    0xb4d07,    0xd4371,          0xa40d },
        {   0x3,    0xb4d1d,    0xd436b,          0xa410 },
        {   0x3,    0xb4d33,    0xd4383,          0xa411 },
        {   0x3,    0xb4d47,    0xd461c,          0xa412 },
        {   0x3,    0xb4d55,    0xd984c,          0xa413 },
        {   0x6,    0xb4da0,    0xd44c3,          0xa416 },
        {   0x3,    0xb4e58,    0xd1e78,          0xa41e },
        {   0x3,    0xb4ec9,    0xd3adb,         0xb7e3d },
        {   0x4,    0xb4ee9,    0xd0ee5,          0xa428 },
        {   0x4,    0xb4eff,    0xd0251,          0xa42d },
        {   0x4,    0xb4f29,    0xd25f3,          0xa431 },
        {   0x9,    0xb540e,    0xb6907,          0xa495 },
        {   0x8,    0xb547d,    0xa7ccd,          0xa49e },
        {   0x9,    0xb54a9,    0xd4efa,          0xa4a2 },
        {   0x9,    0xb54aa,    0xd760b,        0x1e6456 },
        {   0x9,    0xb54af,    0xd2625,        0x18f420 },
        {   0x9,    0xb54b4,    0xd38af,         0x16951 },
        {   0x9,    0xb54b5,    0xd1de4,          0xd4a9 },
        {   0x9,    0xb54b7,    0xd0536,          0xa51f },
        {   0x3,    0xb54bf,    0xd1002,          0xd4ab },
        {   0x9,    0xb54c1,    0xd0f0d,        0x1f37be },
        {   0x9,    0xb54c3,    0xd3015,          0xa51a },
        {   0x9,    0xb54d2,    0xd171c,          0xa4a3 },
        {   0x3,    0xb54e6,    0xdb96a,          0xa4a4 },
        {   0x9,    0xb54ed,    0xd5ff0,          0xd7bb },
        {   0x9,    0xb54f5,    0xd193f,          0xa4a5 },
        {   0x3,    0xb5509,    0xed80b,         0x151a0 },
        {   0x9,    0xb550a,    0xd1588,          0xa4a6 },
        {   0x9,    0xb550c,    0xd85ff,          0xd784 },
    };

    printf("\n\nHELLO, IS IT ME YOU'RE LOOKING FOR?\n\n");
    for (size_t j = 0; j < NUM_RECORDS; j++) {
        printf("%2lu -- ", j);
        
        omap_entry_t* omap_entry = get_btree_phys_omap_entry(fs_omap_btree, record_data[j][3], (xid_t)(~0) );
        if (!omap_entry) {
            printf("Could not resolve %#" PRIx64 " --- no such entry in omap tree\n", record_data[j][3]);
            continue;
        }

        if ( (uint64_t)(omap_entry->val.ov_paddr)  ==  record_data[j][2] ) {
            printf("OK.\n");
        } else {
            printf( "Failed to resolve %#" PRIx64 " to %#" PRIx64 " --- it resolved to %#" PRIx64 " instead\n",
                record_data[j][3],
                record_data[j][2],
                omap_entry->val.ov_paddr
            );
        }

        free(omap_entry);
    }
    printf("\n\nDONE DONE\n\n");

    free(fs_omap_btree);
    free(fs_omap);

    // Closing statements; de-allocate all memory, close all file descriptors.
    free(apsbs);
    free(nx_omap_btree);
    free(nx_omap);
    free(xp_obj);
    free(nxsb);
    fclose(nx);
    printf("END: All done.\n");
    return 0;
}
