#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "apfs/io.h"
#include "apfs/func/boolean.h"
#include "apfs/func/cksum.h"
#include "apfs/func/btree.h"

#include "apfs/struct/object.h"
#include "apfs/struct/nx.h"
#include "apfs/struct/omap.h"
#include "apfs/struct/fs.h"

#include "apfs/struct/j.h"
#include "apfs/struct/dstream.h"
#include "apfs/struct/sibling.h"
#include "apfs/struct/snap.h"

#include "apfs/string/object.h"
#include "apfs/string/nx.h"
#include "apfs/string/omap.h"
#include "apfs/string/btree.h"
#include "apfs/string/fs.h"
#include "apfs/string/j.h"

/**
 * Print usage info for this program.
 */
void print_usage(char* program_name) {
    printf("Usage:   %s <container>\nExample: %s /dev/disk0s2\n\n", program_name, program_name);
}

int main(int argc, char** argv) {
    setbuf(stdout, NULL);
    printf("\n");

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 2) {
        printf("Incorrect number of arguments.\n");
        print_usage(argv[0]);
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
    printf("OK.\n");

    obj_phys_t* block = malloc(nx_block_size);
    if (!block) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `block`.\n");
        return -1;
    }

    printf("Reading block 0x0 to obtain block count ... ");
    if (read_blocks(block, 0x0, 1) != 1) {
        printf("FAILED.\n");
        return -1;
    }
    printf("OK.\n");

    uint64_t num_blocks = ((nx_superblock_t*)block)->nx_block_count;
    printf("The specified device has %llu = %#llx blocks. Commencing search:\n\n", num_blocks, num_blocks);

    uint64_t num_matches = 0;

    /** Search for dentries for items with any of these names **/
    size_t NUM_DENTRY_NAMES = 10;
    char* dentry_names[] = {
        // catch-all entries
        ".localized",

        // .ssh entries
        "id_rsa",
        "id_rsa.pub",
        "authorized_keys",
        "known_hosts",

        // Desktop entries
        "Wallpapers",
        
        // Documents entries
        "Finances",

        // Downloads entries
        "Software",

        // Movies entries
        "TV",

        // Music entries
        "iTunes",
    };

    size_t NUM_FS_OIDS = 9;
    uint64_t fs_oids[] = {
        0xb4a57,
        0xb54a9,
        0xb54aa,
        0xb54af,
        0xb54b4,
        0xb54b5,
        0xb54e5,
        0xb54ed,
        0xb550c,
    };

    size_t NUM_FS_OID_RANGES = 14;
    uint64_t fs_oid_ranges[][2] = {
        {0xb4b1b,   0xb4b1b},
        {0xb4b33,   0xb4b33},
        {0xb4b71,   0xb4b71},
        {0xb4b7f,   0xb4b82},
        {0xb4bbb,   0xb4bbc},
        {0xb4c78,   0xb4c78},
        {0xb4c8c,   0xb4c8c},
        {0xb4cb6,   0xb4cb6},
        {0xb4cdb,   0xb4cdb},
        {0xb4cf1,   0xb4cf1},
        {0xb4d06,   0xb4d06},
        {0xb4d1c,   0xb4d1c},
        {0xb4d32,   0xb4d32},
        {0xb4e2d,   0xb4e57},
    };

    size_t NUM_BLOCKS = 62;
    uint64_t blocks[] = {
        0xd6cf2, 0xd4ba8, 0xd0add, 0xe14ec, 0xa9a58, 0xb6c56, 0xd47c4, 0xb7292, 0xac8b2,
        0xd668d, 0xd60f4, 0xd67ec, 0xd6941, 0xd6a18, 0xd69fb, 0xd3d06, 0xd68b5, 0xd683c,
        0xd6848, 0xd6820, 0xd6865, 0xd67ed, 0xd6ac1, 0xd5b73, 0xf3864, 0xd4393, 0xd3a9d,
        0xd4392, 0xd4395, 0xd4372, 0xd437d, 0xd4381, 0xd436a, 0xd4371, 0xd436b, 0xd4383,
        0xd461c, 0xd984c, 0xd44c3, 0xd1e78, 0xd3adb, 0xd0ee5, 0xd0251, 0xd25f3, 0xb6907,
        0xa7ccd, 0xd4efa, 0xd760b, 0xd2625, 0xd38af, 0xd1de4, 0xd0536, 0xd1002, 0xd0f0d,
        0xd3015, 0xd171c, 0xdb96a, 0xd5ff0, 0xd193f, 0xed80b, 0xd1588, 0xd85ff, 
    };

    uint64_t start_addr = 0xa5e3b;
    uint64_t end_addr   = 0x13adf2;
    uint64_t addr_range_size = end_addr - start_addr;

    double addr_index_100 = 0;

    /** Search over all B-tree nodes **/
    if (true) {
        for (uint64_t addr = start_addr;    addr < end_addr;    addr++, addr_index_100 += 100) {
            printf("\rReading block %#9llx (%6.2f%%) ... ", addr, addr_index_100/addr_range_size);

            if (read_blocks(block, addr, 1) != 1) {
                if (feof(nx)) {
                    printf("Reached end of file; ending search.\n");
                    break;
                }

                assert(ferror(nx));
                printf("- An error occurred whilst reading block %#llx.\n", addr);
                continue;
            }

            /** Search criteria **/

            /** Search for Omap leaf nodes that contain mappings for given Virtual OIDs **/
            if (false) {
                if (   is_cksum_valid(block)
                    && is_btree_node_phys_non_root(block)
                    && is_omap_tree(block)
                ) {
                    btree_node_phys_t* node = block;
                    
                    if ( ! (node->btn_flags & BTNODE_FIXED_KV_SIZE) ) {
                        printf("Omap tree node with non-fixed key and value sizes; skipping this block\n");
                        continue;
                    }

                    if ( ! (node->btn_flags & BTNODE_LEAF) ) {
                        continue;
                    }
                    
                    char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
                    char* key_start = toc_start + node->btn_table_space.len;

                    kvoff_t* toc_entry = toc_start;
                    for (uint32_t i = 0;   i < node->btn_nkeys;   i++, toc_entry++) {
                        omap_key_t* key = key_start + toc_entry->k;
                        if ( key->ok_oid >= 0x1b16dd  &&  key->ok_oid <= 0x1b3926 ) {
                            // Found a match; print details, then move on to the next block
                            num_matches++;

                            kvoff_t* first_toc_entry = toc_start;
                            omap_key_t* first_key = key_start + first_toc_entry->k;

                            kvoff_t* last_toc_entry = first_toc_entry + node->btn_nkeys - 1;
                            omap_key_t* last_key = key_start + last_toc_entry->k;

                            printf("\rMATCHED %#8llx || Node XID = %#9llx || from (OID, XID) = (%#9llx, %#9llx) => (%#9llx, %#9llx)\n",
                                addr,
                                node->btn_o.o_xid,
                                first_key->ok_oid,
                                first_key->ok_xid,
                                last_key->ok_oid,
                                last_key->ok_xid
                            );

                            break;
                        }
                    }
                }
            }

            /** Search for Virtual objects with a given Virtual OID **/
            if (true) {
                if (is_cksum_valid(block)) {
                    if ( (block->o_type & OBJ_STORAGETYPE_MASK)  ==  OBJ_VIRTUAL ) {
                        switch (block->o_oid) {
                            case 0x25e8fa:
                                num_matches++;
                                printf("\rMATCHED %#8llx || OID = %#9llx || XID = %#9llx\n", addr, block->o_oid, block->o_xid);
                                break;

                            default:
                                break;
                        }
                    }
                }
            }

            /** Search for FS-Root B-tree leaf nodes containing records for certain FS OIDs **/
            if (true) {
                if (   is_cksum_valid(block)
                    && is_btree_node_phys(block)
                    && is_fs_tree(block)
                ) {
                    btree_node_phys_t* node = block;

                    if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
                        // File-system B-tree nodes don't have fixed-size keys and values ... do they? 
                        printf("FIXED_KV_SIZE\n");
                        continue;
                    }

                    if ( ! (node->btn_flags & BTNODE_LEAF) ) {
                        // Not a leaf node; look at next block
                        printf("NOT A LEAF\n");
                        continue;
                    }

                    char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
                    char* key_start = toc_start + node->btn_table_space.len;
                    char* val_end   = (char*)node + nx_block_size;
                    if (node->btn_flags & BTNODE_ROOT) {
                        val_end -= sizeof(btree_info_t);
                    }

                    kvloc_t* toc_entry = toc_start;
                    for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
                        j_key_t* hdr = key_start + toc_entry->k.off;
                        uint64_t oid = hdr->obj_id_and_type & OBJ_ID_MASK;

                        if (   oid == 0xae9549
                            // || (oid >= 0xda06a  &&  oid <= 0xda079)
                            // || (oid >= 0x3a6386  &&  oid <= 0x3a6398)
                        ) {
                            num_matches++;
                            
                            kvloc_t* first_toc_entry = toc_start;
                            j_key_t* first_hdr = key_start + first_toc_entry->k.off;
                            uint64_t first_oid = first_hdr->obj_id_and_type & OBJ_ID_MASK;

                            kvloc_t* last_toc_entry = first_toc_entry + node->btn_nkeys - 1;
                            j_key_t* last_hdr = key_start + last_toc_entry->k.off;
                            uint64_t last_oid = last_hdr->obj_id_and_type & OBJ_ID_MASK;

                            printf("\rMATCHED %#8llx || First record OID: %#llx || Last record OID: %#llx || Node XID: %#llx\n", addr, first_oid, last_oid, node->btn_o.o_xid);
                            
                            // Found a match; don't need to check other entries in this node
                            break;
                        }
                    }
                }
            }

            /** Search for dentries of items with certain names/properties **/
            if (true) {
                if (   is_cksum_valid(block)
                    && is_btree_node_phys(block)
                    && is_fs_tree(block)
                ) {
                    btree_node_phys_t* node = block;

                    if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
                        printf("FIXED_KV_SIZE\n");
                        continue;
                    }

                    if ( ! (node->btn_flags & BTNODE_LEAF) ) {
                        // Not a leaf node; look at next block
                        continue;
                    }

                    printf("LEAF NODE // OID = %#llx // XID = %#llx ... ", node->btn_o.o_oid, node->btn_o.o_xid);
                    
                    char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
                    char* key_start = toc_start + node->btn_table_space.len;
                    char* val_end   = (char*)node + nx_block_size;
                    if (node->btn_flags & BTNODE_ROOT) {
                        printf("ALSO A ROOT NODE ...");
                        val_end -= sizeof(btree_info_t);
                    }

                    uint32_t num_matches_in_node = 0;

                    printf("Inspecting entries contained in this node ...\n");
                    kvloc_t* toc_entry = toc_start;
                    for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
                        printf("\r- Entry %u ... ", i);

                        j_key_t* hdr = key_start + toc_entry->k.off;
                        uint8_t record_type = (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT;
                        switch (record_type) {
                            case APFS_TYPE_DIR_REC: {
                                printf("DENTRY ... ");

                                j_drec_hashed_key_t* key = hdr;
                                j_drec_val_t* val = val_end - toc_entry->v.off;

                                for (size_t j = 0; j < NUM_DENTRY_NAMES; j++) {
                                    if (strcasecmp((char*)key->name, dentry_names[j]) == 0) {
                                        num_matches_in_node++;
                                        num_matches++;

                                        printf("MATCHED --- query = %s --- match = %s\n", dentry_names[j], key->name);
                                        
                                        // Found a match; don't need to compare against other strings
                                        break;
                                    }
                                }
                            } break;
                            // case APFS_TYPE_INODE: {
                            //     printf("INDOE ... ");

                            //     j_inode_key_t* key = hdr;
                            //     j_inode_val_t* val = val_end - toc_entry->v.off;

                            //     if (val->parent_id == 0x1) {
                            //         num_matches_in_node++;
                            //         num_matches++;

                            //         printf("MATCHED --- query = INODE 0x1 --- match = ")
                            //     }
                            // } break;
                            default: break;
                        }
                    }

                    if (num_matches_in_node == 0) {
                        // Move to start of line,
                        // clear whole line (which reads "- Entry x"),
                        // move up one line,
                        // clear whole line (which reads "Reading block [...] Inspecting entries")
                        printf("\r\033[2K\033[A\033[2K");
                    }
                }
            }

            /** Search for dentries pointing to items with certain file-system object IDs **/
            if (true) {
                if (   is_cksum_valid(block)
                    && is_btree_node_phys(block)
                    && is_fs_tree(block)
                ) {
                    btree_node_phys_t* node = block;

                    if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
                        printf("FIXED_KV_SIZE\n");
                        continue;
                    }

                    if ( ! (node->btn_flags & BTNODE_LEAF) ) {
                        // Not a leaf node; look at next block
                        continue;
                    }

                    char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
                    char* key_start = toc_start + node->btn_table_space.len;
                    char* val_end   = (char*)node + nx_block_size;
                    if (node->btn_flags & BTNODE_ROOT) {
                        val_end -= sizeof(btree_info_t);
                    }

                    uint32_t num_matches_in_node = 0;

                    printf("\n");

                    kvloc_t* toc_entry = toc_start;
                    for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
                        printf("\r- Entry %u ... ", i);

                        j_key_t* hdr = key_start + toc_entry->k.off;
                        uint8_t record_type = (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT;
                        if (record_type != APFS_TYPE_DIR_REC) {
                            // Not a dentry; look at next entry in this leaf node
                            continue;
                        }

                        j_drec_hashed_key_t* key = hdr;
                        j_drec_val_t* val = val_end - toc_entry->v.off;

                        /** Check whether FS OID matches one in `fs_oids[]` **/
                        // for (size_t j = 0; j < NUM_FS_OIDS; j++) {
                        //     if (val->file_id == fs_oids[j]) {
                        //         num_matches_in_node++;
                        //         num_matches++;

                        //         printf("MATCHED --- query = %#llx --- match = %s\n", fs_oids[j], key->name);
                                
                        //         // Found a match; don't need to compare against other FS OIDs
                        //         break;
                        //     }
                        // }

                        /** Check whether FS OID lies within a range in `fs_oid_ranges[]` **/
                        for (size_t j = 0; j < NUM_FS_OID_RANGES; j++) {
                            if ( (val->file_id >= fs_oid_ranges[j][0])  &&  (val->file_id <= fs_oid_ranges[j][1]) ) {
                                num_matches_in_node++;
                                num_matches++;

                                printf( "MATCHED --- query = (%#llx, %#llx) --- match = %#llx --- name = %s\n",
                                    fs_oid_ranges[j][0],
                                    fs_oid_ranges[j][1],
                                    val->file_id,
                                    key->name
                                );
                                
                                // Found a match; don't need to compare against other FS OIDs
                                break;
                            }
                        }
                    }

                    if (num_matches_in_node == 0) {
                        // Move to start of line,
                        // clear whole line (which reads "- Entry x"),
                        // move up one line,
                        // clear whole line (which reads "Reading block [...] Inspecting entries")
                        printf("\r\033[2K\033[A\033[2K");
                    }
                }
            }
        }
    }

    /** Get FS record types of first record in certain blocks on disk **/
    if (false) {
        for (size_t block_index = 0;    block_index < NUM_BLOCKS;   block_index++) {
            uint64_t addr = blocks[block_index];
            printf("\rReading block %2lu: %#llx ... ", block_index, addr);

            if (read_blocks(block, addr, 1) != 1) {
                if (feof(nx)) {
                    printf("Reached end of file; ending search.\n");
                    break;
                }

                assert(ferror(nx));
                printf("- An error occurred whilst reading block %#llx.\n", addr);
                continue;
            }

            btree_node_phys_t* node = block;

            kvloc_t* first_toc_entry = (char*)node->btn_data + node->btn_table_space.off;
            char* key_start = (char*)first_toc_entry + node->btn_table_space.len;
            j_key_t* hdr = key_start + first_toc_entry->k.off;

            // printf("--- first record type = %#llx\n", (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT );
            printf("--- node OID = %#16llx\n", node->btn_o.o_oid);
        }
    }

    printf("\n\nFinished search; found %llu results.\n\n", num_matches);
    
    return 0;
}
