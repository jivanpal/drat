#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "../apfs/io.h"
#include "../apfs/struct/general.h"
#include "../apfs/struct/j.h"
#include "../apfs/struct/jconst.h"
#include "../apfs/struct/dstream.h"
#include "../apfs/struct/sibling.h"
#include "../apfs/struct/snap.h"

#include "../apfs/func/boolean.h"
#include "../apfs/func/cksum.h"
#include "../apfs/func/btree.h"

#include "../apfs/string/object.h"
#include "../apfs/string/nx.h"
#include "../apfs/string/btree.h"
#include "../apfs/string/omap.h"
#include "../apfs/string/fs.h"
#include "../apfs/string/j.h"

static void print_usage(int argc, char** argv) {
    fprintf(
        argc == 1 ? stdout : stderr,
        
        "Usage:   %s <container> <fs tree root node address> <omap tree root node address>\n"
        "Example: %s /dev/disk0s2 0xd02a4 0x3af2\n",
        
        argv[0],
        argv[0]
    );
}

int cmd_explore_fs_tree(int argc, char** argv) {
    if (argc == 1) {
        print_usage(argc, argv);
        return 0;
    }

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 4) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }

    char* nx_path = argv[1];
    
    // Capture <fs tree root node address>
    paddr_t fs_root_addr;
    bool parse_success = sscanf(argv[2], "0x%" PRIx64 "", &fs_root_addr);
    if (!parse_success) {
        parse_success = sscanf(argv[2], "%" PRIu64, &fs_root_addr);
    }
    if (!parse_success) {
        fprintf(stderr, "%s is not a valid block address.\n", argv[2]);
        print_usage(argc, argv);
        return 1;
    }

    // Capture <omap tree root node address>
    paddr_t omap_root_addr;
    parse_success = sscanf(argv[3], "0x%" PRIx64 "", &omap_root_addr);
    if (!parse_success) {
        parse_success = sscanf(argv[3], "%" PRIu64, &omap_root_addr);
    }
    if (!parse_success) {
        fprintf(stderr, "%s is not a valid block address.\n", argv[3]);
        print_usage(argc, argv);
        return 1;
    }
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "\nABORT: main: ");
        report_fopen_error();
        printf("\n");
        return -errno;
    }
    printf("OK.\n\n");

    // Read the specified root nodes
    printf("Reading the file-system tree root node (block 0x%" PRIx64 ") ... ", fs_root_addr);
    btree_node_phys_t* fs_root_node = malloc(nx_block_size);
    if (!fs_root_node) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_root_node`.\n");
        return -1;
    }

    if (read_blocks(fs_root_node, fs_root_addr, 1) == 0) {
        printf("\nEND: Block index %s does not exist in `%s`.\n", argv[2], nx_path);
        return 0;
    }

    printf("validating ... ");
    if (is_cksum_valid(fs_root_node)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\n");
    }
    printf("\n");

    printf("Reading the object map root node (block 0x%" PRIx64 ") ... ", omap_root_addr);
    btree_node_phys_t* omap_root_node = malloc(nx_block_size);
    if (!omap_root_node) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `omap_root_node`.\n");
        return -1;
    }

    if (read_blocks(omap_root_node, omap_root_addr, 1) == 0) {
        printf("\nEND: Block index %s does not exist in `%s`.\n", argv[2], nx_path);
        return 0;
    }

    printf("validating ... ");
    if (is_cksum_valid(omap_root_node)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\n");
    }
    printf("\n");

    // Allocate space for the current working node in the file-system tree,
    // then copy the root node to this space.
    btree_node_phys_t* node = malloc(nx_block_size);
    if (!node) {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
        return -1;
    }
    memcpy(node, fs_root_node, nx_block_size);

    // Pointers to areas of the node
    char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
    char* key_start = toc_start + node->btn_table_space.len;
    char* val_end   = (char*)node + nx_block_size;
    if (node->btn_flags & BTNODE_ROOT) {
        val_end -= sizeof(btree_info_t);
    }

    // // Copy the B-tree info somewhere else for easy referencing
    // btree_info_t* bt_info = malloc(sizeof(btree_info_t));
    // if (!bt_info) {
    //     fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `bt_info`.\n");
    //     return -1;
    // }
    // memcpy(bt_info, val_end, sizeof(btree_info_t));

    // Descend the tree
    while (true) {
        if (node->btn_flags & BTNODE_FIXED_KV_SIZE) {
            fprintf(stderr, "\nFile-system trees don't have fixed size keys and values ... do they?\n");
            return 0;
        }

        printf("\nNode details:\n");
        printf("--------------------------------------------------------------------------------\n");
        print_btree_node_phys(node);
        printf("--------------------------------------------------------------------------------\n");
        printf("\n");

        printf("\nNode has %u entries, as follows:\n", node->btn_nkeys);
        assert(node->btn_nkeys > 0);

        kvloc_t* toc_entry = toc_start;
        for (uint32_t i = 0;    i < node->btn_nkeys;    i++, toc_entry++) {
            j_key_t* hdr = key_start + toc_entry->k.off;
            uint8_t type = (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT;

            printf(
                "- %3u:  %#15llx = Virtual OID   ||   %2u = %#1x = Type",
                i, hdr->obj_id_and_type & OBJ_ID_MASK, type, type
            );

            if (node->btn_flags & BTNODE_LEAF) {
                if ( (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT  ==  APFS_TYPE_DIR_REC ) {
                    j_drec_hashed_key_t* key = hdr;
                    j_drec_val_t* val = val_end - toc_entry->v.off;

                    printf(" = `dentry`   ||   Dentry Virtual OID = %#16" PRIx64 "   ||   Dentry name = %s", val->file_id, key->name);
                }
            } else {
                oid_t* child_node_virt_oid = val_end - toc_entry->v.off;
                printf("   ||   Target child node Virtual OID = %#16" PRIx64 "", *child_node_virt_oid);
                omap_entry_t* child_node_omap_entry = get_btree_phys_omap_entry(omap_root_node, *child_node_virt_oid, (xid_t)(~0));
                if (!child_node_omap_entry) {
                    printf("  ||  UNRESOLVABLE");
                } else {
                    btree_node_phys_t* child_node = malloc(nx_block_size);
                    if (!child_node) {
                        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `child_node`.\n");
                        return -1;
                    }
                    if (read_blocks(child_node, child_node_omap_entry->val.ov_paddr, 1) != 1) {
                        fprintf(stderr, "\nABORT: Failed to read block %#" PRIx64 ".\n", child_node_omap_entry->val.ov_paddr);
                        return -1;
                    }

                    if (*((uint64_t*)child_node) == 0) {
                        printf("  ||  ZEROED OUT");
                    }

                    free (child_node);
                }
            }

            printf("\n");
        }
        
        uint32_t entry_index;
        printf("Choose an entry [0-%u]: ", node->btn_nkeys - 1);
        scanf("%u", &entry_index);
        while (entry_index >= node->btn_nkeys) {
            printf("Invalid choice; choose an entry [0-%u]: ", node->btn_nkeys - 1);
            scanf("%u", &entry_index);
        }

        toc_entry = (kvloc_t*)toc_start + entry_index;

        // If this is a leaf node, output the file-system record details
        if (node->btn_flags & BTNODE_LEAF) {
            j_rec_t* fs_rec = malloc(sizeof(j_rec_t) + toc_entry->k.len + toc_entry->v.len);
            if (!fs_rec) {
                fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_rec`.\n");
                return -1;
            }

            fs_rec->key_len = toc_entry->k.len;
            fs_rec->val_len = toc_entry->v.len;
            memcpy(
                fs_rec->data,
                key_start + toc_entry->k.off,
                fs_rec->key_len
            );
            memcpy(
                fs_rec->data + fs_rec->key_len,
                val_end - toc_entry->v.off,
                fs_rec->val_len
            );

            j_key_t* hdr = fs_rec->data;
            printf("Key size:           %u bytes\n",    fs_rec->key_len);
            printf("Value size:         %u bytes\n",    fs_rec->val_len);
            printf("ID and type field:  0x%016" PRIx64 "\n",   hdr->obj_id_and_type);
            printf("\n");

            switch ( (hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT ) {
                // NOTE: Need to enclose each case in a block `{}` since the
                // names `key` and `val` are potentially declared multiple times
                // in this switch-statement (though in practice it is not a
                // concern since every `case` here ends in a `break`.)
                case APFS_TYPE_SNAP_METADATA: {
                    j_snap_metadata_key_t* key = fs_rec->data;
                    j_snap_metadata_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_EXTENT: {
                    j_phys_ext_key_t* key = fs_rec->data;
                    j_phys_ext_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_INODE: {
                    j_inode_key_t* key = fs_rec->data;
                    j_inode_val_t* val = fs_rec->data + fs_rec->key_len;
                    print_j_inode_key(key);
                    print_j_inode_val(val, fs_rec->val_len == sizeof(j_inode_val_t));
                } break;
                case APFS_TYPE_XATTR: {
                    j_xattr_key_t* key = fs_rec->data;
                    j_xattr_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_SIBLING_LINK: {
                    j_sibling_key_t* key = fs_rec->data;
                    j_sibling_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_DSTREAM_ID: {
                    j_dstream_id_key_t* key = fs_rec->data;
                    j_dstream_id_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_CRYPTO_STATE: {
                    j_crypto_key_t* key = fs_rec->data;
                    j_crypto_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_FILE_EXTENT: {
                    j_file_extent_key_t* key = fs_rec->data;
                    j_file_extent_val_t* val = fs_rec->data + fs_rec->key_len;
                    print_j_file_extent_key(key);
                    print_j_file_extent_val(val);
                } break;
                case APFS_TYPE_DIR_REC: {
                    // Spec inorrectly says to use `j_drec_key_t`; see NOTE in `apfs/struct/j.h`
                    j_drec_hashed_key_t*    key = fs_rec->data;
                    j_drec_val_t*           val = fs_rec->data + fs_rec->key_len;
                    print_j_drec_hashed_key(key);
                    print_j_drec_val(val, fs_rec->val_len == sizeof(j_drec_val_t));
                } break;
                case APFS_TYPE_DIR_STATS: {
                    j_dir_stats_key_t* key = fs_rec->data;
                    // Spec incorrectly says to use `j_drec_val_t`; we use `j_dir_stats_val_t`
                    j_dir_stats_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_SNAP_NAME: {
                    j_snap_name_key_t* key = fs_rec->data;
                    j_snap_name_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_SIBLING_MAP: {
                    j_sibling_map_key_t* key = fs_rec->data;
                    j_sibling_map_val_t* val = fs_rec->data + fs_rec->key_len;
                } break;
                case APFS_TYPE_INVALID:
                    fprintf(stderr, "The record has an invalid type.\n");
                    break;
                default:
                    fprintf(stderr, "The record has an unknown type.\n");
                    break;
            }

            return 0;
        }

        // Else, read the corresponding child node into `node` and loop
        oid_t* child_node_virt_oid = val_end - toc_entry->v.off;
        printf("Child node has Virtual OID 0x%" PRIx64 ".\n", *child_node_virt_oid);
        
        omap_entry_t* child_node_omap_entry = get_btree_phys_omap_entry(omap_root_node, *child_node_virt_oid, fs_root_node->btn_o.o_xid);
        if (!child_node_omap_entry) {
            printf("Need to descend to node with Virtual OID %#" PRIx64 " and maximum XID %#" PRIx64 ", but the object map lists no objects with this Virtual OID.\n", *child_node_virt_oid, fs_root_node->btn_o.o_xid);
            return 0;
        }

        printf("The object map resolved this Virtual OID to block address %#" PRIx64 ". Reading ... ", child_node_omap_entry->val.ov_paddr);
        if (read_blocks(node, child_node_omap_entry->val.ov_paddr, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", child_node_omap_entry->val.ov_paddr);
            return -1;
        }

        printf("validating ... ");
        if (is_cksum_valid(node)) {
            printf("OK.\n");
        } else {
            printf("FAILED.\n");
        }

        toc_start = (char*)(node->btn_data) + node->btn_table_space.off;
        key_start = toc_start + node->btn_table_space.len;
        val_end   = (char*)node + nx_block_size;    // Always dealing with non-root node here

        free(child_node_omap_entry);
    }
    
    return 0;
}
