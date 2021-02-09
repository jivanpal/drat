#ifndef DRAT_INSPECT_H
#define DRAT_INSPECT_H

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

static void print_usage(int argc, char** argv) {
    fprintf(
        argc == 1 ? stdout : stderr,
        
        "Usage:   %s <container>\n"
        "Example: %s /dev/disk0s2\n",
        
        argv[0],
        argv[0]    
    );
}

int cmd_inspect(int argc, char** argv) {
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
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `nx_omap`.\n");
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
            fprintf(stderr, "\nABORT: No objects with Virtual OID %#" PRIx64 " and maximum XID %#" PRIx64 " exist in `nx_omap_btree`.\n", nxsb->nx_fs_oid[i], nxsb->nx_o.o_xid);
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
    for (uint32_t i = 0; i < num_file_systems; i++) {
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

        printf("The file-system tree root for this volume has Virtual OID 0x%" PRIx64 ".\n", apsb->apfs_root_tree_oid);
        printf("Looking up this Virtual OID in the volume object map ... ");
        omap_entry_t* fs_root_entry = get_btree_phys_omap_entry(fs_omap_btree, apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
        if (!fs_root_entry) {
            fprintf(stderr, "\nABORT: No objects with Virtual OID %#" PRIx64 " and maximum XID %#" PRIx64 " exist in `fs_omap_btree`.\n", apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
            return -1;
        }
        printf("corresponding block address is 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);

        printf("Reading ... ");
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
        printf("validating ... ");
        if (!is_cksum_valid(fs_root_btree)) {
            printf("FAILED.\nGoing back to look at the previous checkpoint instead.\n");

            // TODO: Handle case where data for a given checkpoint is malformed
            printf("END: Handling of this case has not yet been implemented.\n");
            return 0;
        }
        printf("OK.\n");

        printf("\nDetails of the file-system B-tree root node:\n");
        printf("--------------------------------------------------------------------------------\n");
        print_btree_node_phys(fs_root_btree);
        printf("--------------------------------------------------------------------------------\n");
        printf("\n");

        size_t NUM_QUERIES = 6;
        oid_t fs_oid_queries[] = {
            0xa64b2,
            0xa64b3,
            0xd491a,
            0xd491d,
            0xd491e,
            0xd491f,
        };

        for (size_t j = 0; j < NUM_QUERIES; j++) {
            oid_t fs_oid = fs_oid_queries[j];

            // printf("Search results for file-system records with Virtual OID 0x%" PRIx64 ":\n", fs_oid);
            // printf("--------------------------------------------------------------------------------\n");

            printf("\n%#" PRIx64 ":\n", fs_oid);

            j_rec_t** fs_records = get_fs_records(fs_omap_btree, fs_root_btree, fs_oid, nxsb->nx_o.o_xid);
            if (!fs_records) {
                printf("No records found with OID 0x%" PRIx64 ".\n", fs_oid);
                return -1;
            }

            size_t num_records = 0;

            for (j_rec_t** fs_rec_cursor = fs_records; *fs_rec_cursor; fs_rec_cursor++) {
                num_records++;
                j_rec_t* fs_rec = *fs_rec_cursor;

                j_key_t* hdr = fs_rec->data;
                // printf("Key size:           %u bytes\n",    fs_rec->key_len);
                // printf("Value size:         %u bytes\n",    fs_rec->val_len);
                // printf("ID and type field:  0x%016" PRIx64 "\n",   hdr->obj_id_and_type);
                // printf("\n");
                printf("- ");

                switch ( (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT ) {
                    // NOTE: Need to enclose each case in a block `{}` since the
                    // names `key` and `val` are potentially declared multiple times
                    // in this switch-statement (though in practice it is not a
                    // concern since every `case` here ends in a `break`.)
                    case APFS_TYPE_SNAP_METADATA: {
                        j_snap_metadata_key_t* key = fs_rec->data;
                        j_snap_metadata_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("SNAP METADATA");
                    } break;
                    case APFS_TYPE_EXTENT: {
                        j_phys_ext_key_t* key = fs_rec->data;
                        j_phys_ext_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("EXTENT");
                    } break;
                    case APFS_TYPE_INODE: {
                        j_inode_key_t* key = fs_rec->data;
                        j_inode_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("INODE");
                        // print_j_inode_key(key);
                        // print_j_inode_val(val, fs_rec->val_len == sizeof(j_inode_val_t));
                    } break;
                    case APFS_TYPE_XATTR: {
                        j_xattr_key_t* key = fs_rec->data;
                        j_xattr_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("XATTR");
                    } break;
                    case APFS_TYPE_SIBLING_LINK: {
                        j_sibling_key_t* key = fs_rec->data;
                        j_sibling_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("SIBLING LINK");
                    } break;
                    case APFS_TYPE_DSTREAM_ID: {
                        j_dstream_id_key_t* key = fs_rec->data;
                        j_dstream_id_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("DSTREAM ID "
                            " || file ID = %#8llx"
                            " || ref. count = %u",

                            key->hdr.obj_id_and_type & OBJ_ID_MASK,
                            val->refcnt
                        );
                    } break;
                    case APFS_TYPE_CRYPTO_STATE: {
                        j_crypto_key_t* key = fs_rec->data;
                        j_crypto_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("CRYPTO STATE");
                    } break;
                    case APFS_TYPE_FILE_EXTENT: {
                        j_file_extent_key_t* key = fs_rec->data;
                        j_file_extent_val_t* val = fs_rec->data + fs_rec->key_len;

                        uint64_t extent_length_bytes = val->len_and_flags & J_FILE_EXTENT_LEN_MASK;
                        uint64_t extent_length_blocks = extent_length_bytes / nx_block_size;

                        printf( "FILE EXTENT"
                            " || file ID = %#8llx"
                            " || log. addr. = %#10" PRIx64 ""
                            " || length = %8" PRIu64 " B = %#10" PRIx64 " B = %5" PRIu64 " blocks = %#7" PRIx64 " blocks"
                            " || phys. block = %#10" PRIx64 "",

                            key->hdr.obj_id_and_type & OBJ_ID_MASK,
                            key->logical_addr,
                            extent_length_bytes, extent_length_bytes, extent_length_blocks, extent_length_blocks,
                            val->phys_block_num
                        );
                    } break;
                    case APFS_TYPE_DIR_REC: {
                        // Spec inorrectly says to use `j_drec_key_t`; see NOTE in `apfs/struct/j.h`
                        j_drec_hashed_key_t*    key = fs_rec->data;
                        j_drec_val_t*           val = fs_rec->data + fs_rec->key_len;
                        printf("DIR REC"
                            " || target ID = %#8" PRIx64 ""
                            " || name = %s",

                            val->file_id,
                            key->name
                        );
                        // print_j_drec_hashed_key(key);
                        // print_j_drec_val(val, fs_rec->val_len == sizeof(j_drec_val_t));
                    } break;
                    case APFS_TYPE_DIR_STATS: {
                        j_dir_stats_key_t* key = fs_rec->data;
                        // Spec incorrectly says to use `j_drec_val_t`; we use `j_dir_stats_val_t`
                        j_dir_stats_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("DIR STATS");
                    } break;
                    case APFS_TYPE_SNAP_NAME: {
                        j_snap_name_key_t* key = fs_rec->data;
                        j_snap_name_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("SNAP NAME");
                    } break;
                    case APFS_TYPE_SIBLING_MAP: {
                        j_sibling_map_key_t* key = fs_rec->data;
                        j_sibling_map_val_t* val = fs_rec->data + fs_rec->key_len;
                        printf("SIBLING MAP");
                    } break;
                    case APFS_TYPE_INVALID:
                        printf("INVALID");
                        // fprintf(stderr, "- A record with OID 0x%" PRIx64 " has an invalid type.\n", fs_oid);
                        break;
                    default:
                        printf("(unknown)");
                        fprintf(stderr, "- A record with OID 0x%" PRIx64 " has an unknown type.\n", fs_oid);
                        break;
                }

                // printf("--------------------------------------------------------------------------------\n");
                printf("\n");
            }

            // printf("- Found %lu records with Virtual OID 0x%" PRIx64 ".\n", num_records, fs_oid);
            free_j_rec_array(fs_records);
        }
        
        // TODO: RESUME HERE

        printf("\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("\n");

        
        free(fs_omap_btree);
        free(fs_omap);
    }

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

#endif // DRAT_INSPECT_H
