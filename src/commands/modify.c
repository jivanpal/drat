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

int cmd_modify(int argc, char** argv) {
    if (argc == 1) {
        print_usage(argc, argv);
        return 0;
    }
    
    /** Setup **/

    setbuf(stdout, NULL);

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argc, argv);
        return 1;
    }
    nx_path = argv[1];
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at `%s` in read-and-write mode ... ", nx_path);
    nx = fopen(nx_path, "w+b");
    if (!nx) {
        fprintf(stderr, "\nABORT: ");
        report_fopen_error();
        printf("\n");
        return -errno;
    }
    printf("OK.\n");

    /** Copy one block to another block address **/
    if (true) {
        long read_from  = 0xe1b61;
        long write_to   = 0xd3793;

        char* block = malloc(nx_block_size);
        if (!block) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `block`.\n");
            return -1;
        }
        
        if (read_blocks(block, read_from, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block %#lx.\n", read_from);
            return -1;
        }

        if (write_blocks(block, write_to, 1) != 1) {
            fprintf(stderr, "\nABORT: An error occurred when writing to block %#lx.\n", write_to);
            return -1;
        }

        printf("Successfully copied block %#lx to %#lx.\n", read_from, write_to);
    }

    /** Create an Omap B-tree node and write it to disk **/
    if (false) {
        size_t NUM_RECORDS = 82;

        /** Create a B-tree node in memory **/

        btree_node_phys_t* node = calloc(1, nx_block_size); // Using `calloc()` to get zeroed out memory
        if (!node) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
            return -1;
        }

        /** Set node header values **/

        node->btn_o.o_oid       = 0xe4759;
        node->btn_o.o_xid       = 0x1bca0d;
        node->btn_o.o_type      = OBJ_PHYSICAL | OBJECT_TYPE_BTREE_NODE;
        node->btn_o.o_subtype   = OBJECT_TYPE_OMAP;

        /** Set B-trees structure values **/
        
        node->btn_flags = BTNODE_LEAF | BTNODE_FIXED_KV_SIZE;
        
        node->btn_level         = 0;
        
        node->btn_nkeys         = NUM_RECORDS;

        node->btn_table_space.off   = 0;
        node->btn_table_space.len   = node->btn_nkeys * sizeof(kvoff_t);

        node->btn_free_space.off    = 0;    // TODO: this value
        node->btn_free_space.len    = 0;    // TODO: this value
        
        // Empty free list for keys and values
        node->btn_key_free_list.off = 0;
        node->btn_key_free_list.len = 0;
        node->btn_val_free_list.off = 0;
        node->btn_val_free_list.len = 0;

        /** Add records to node **/

        char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
        char* key_start = toc_start + node->btn_table_space.len;
        char* val_end   = (char*)node + nx_block_size;

        /** Add records to node by copying them from other nodes **/

        btree_node_phys_t* ref_node = malloc(nx_block_size);
        if (!ref_node) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `ref_node`.\n");
            return -1;
        }

        /** Declare data needed to construct B-tree node **/

        /**
         * Array describing which entries of which existing nodes need to
         * be copied to the new node. Each array entry comprises:
         * 
         * - 0: The reference node's block address
         * - 1: The index of the first entry to copy
         * - 2: The index of the last entry to copy
         */
        size_t NUM_NODES = 2;
        uint64_t ref_nodes[][3] = {
            {0xe4759, 1, 58},
            {0xe1bc1, 0, 23},
        };

        kvoff_t* toc_entry = toc_start;
        omap_key_t* key = key_start;
        omap_val_t* val = (omap_val_t*)val_end - 1;
        
        for (size_t i = 0; i < NUM_NODES; i++) {
            if (read_blocks(ref_node, ref_nodes[i][0], 1) != 1) {
                fprintf(stderr, "\nABORT: Error reading block %#" PRIx64 ".\n", ref_nodes[i][0]);
                return -1;
            }

            char* ref_toc_start = (char*)ref_node->btn_data + ref_node->btn_table_space.off;
            char* ref_key_start = ref_toc_start + ref_node->btn_table_space.len;
            char* ref_val_end   = (char*)ref_node + nx_block_size;
            
            uint32_t NUM_ENTRIES = (ref_nodes[i][2] - ref_nodes[i][1]) + 1;

            kvoff_t* ref_toc_entry = (kvoff_t*)ref_toc_start + ref_nodes[i][1];

            for (uint32_t j = 0;   j < NUM_ENTRIES;   j++, ref_toc_entry++, toc_entry++, key++, val--) {
                toc_entry->k = (char*)key - key_start;
                toc_entry->v = val_end - (char*)val;

                omap_key_t* ref_key = ref_key_start + ref_toc_entry->k;
                omap_val_t* ref_val = ref_val_end   - ref_toc_entry->v;

                omap_key_t* key = key_start + toc_entry->k;
                omap_val_t* val = val_end   - toc_entry->v;

                memcpy(key, ref_key, sizeof(omap_key_t));
                memcpy(val, ref_val, sizeof(omap_val_t));
            }
        }
        assert(toc_entry - (kvoff_t*)toc_start == (long)NUM_RECORDS);

        free(ref_node);
        
        /** Now we can declare where the node's free space is **/
        
        node->btn_free_space.off = (char*)key - key_start;
        node->btn_free_space.len = (char*)(val - 1) - (char*)key;

        /** Set checksum **/

        uint64_t block_checksum = compute_block_cksum(node);
        *((uint64_t*)node) = block_checksum;

        /** Print out the contents of the B-tree node that is in memory **/
        
        print_btree_node_phys(node);

        /** Write the data to disk **/

        if (write_blocks(node, 0xd35a3, 1) != 1) {
            fprintf(stderr, "\nABORT: Error writing to 0xd35a3.\n");
            return -1;
        } else {
            printf("\nSuccessfully wrote new node to 0xd35a3.\n");
        }

        /** Cleanup **/

        free(node);
    }

    /** Add an entry to an Omap B-tree leaf node and write the new node to disk **/
    if (false) {
        long addr = 0xd350b;
        size_t NEW_ENTRY_INDEX = 24;

        /** Create a B-tree node in memory **/

        btree_node_phys_t* node = malloc(nx_block_size);
        if (!node) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
            return -1;
        }
        if (read_blocks(node, addr, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block %#lx.\n", addr);
            return -1;
        }
        
        node->btn_nkeys++;

        if ( node->btn_table_space.len  <  node->btn_nkeys * sizeof(kvoff_t) ) {
            fprintf(stderr, "\nABORT: Not enough space in TOC area for the new entry.\n");
            return -1;
        }

        char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
        char* key_start = toc_start + node->btn_table_space.len;
        char* val_end   = (char*)node + nx_block_size;

        omap_key_t* key;
        omap_val_t* val;

        if ( node->btn_free_space.len  >=  sizeof(omap_key_t) + sizeof(omap_val_t) ) {
            key = (omap_key_t*)( key_start + node->btn_free_space.off );
            val = (omap_val_t*)( (char*)key + node->btn_free_space.len - sizeof(omap_val_t) );

            node->btn_free_space.off += sizeof(omap_key_t);
            node->btn_free_space.len -= sizeof(omap_key_t) + sizeof(omap_val_t);
        } else {
            fprintf(stderr, "Not enough space in the shared free space area. Checking the key- and value-free lists.\n");
            
            /** Check overall free space in each list **/
            
            if (node->btn_key_free_list.len < sizeof(omap_key_t) ) {
                fprintf(stderr, "\nABORT: Not enough free space in the key free-list.\n");
                return -1;
            }
            uint16_t key_free_len_remaining_in_search = node->btn_key_free_list.len;

            if (node->btn_val_free_list.len < sizeof(omap_val_t) ) {
                fprintf(stderr, "\nABORT: Not enough free space in the value free-list.\n");
                return -1;
            }
            uint16_t val_free_len_remaining_in_search = node->btn_val_free_list.len;

            /** Now actually look for sufficient free blocks **/

            nloc_t* key_free_loc = key_start + node->btn_key_free_list.off;
            while (key_free_loc->len < sizeof(omap_key_t)) {
                if (key_free_len_remaining_in_search <= 0) {
                    fprintf(stderr, "\nABORT: No sufficiently sized free block referenced by the key free list.\n");
                    return -1;
                }
                key_free_len_remaining_in_search -= key_free_loc->len;
                key_free_loc = key_start + key_free_loc->off;
            }
            key = key_free_loc;

            nloc_t* val_free_loc = val_end - node->btn_val_free_list.off;
            while (val_free_loc->len < sizeof(omap_val_t)) {
                if (val_free_len_remaining_in_search <= 0) {
                    fprintf(stderr, "\nABORT: No sufficiently sized free block referenced by the value free list.\n");
                    return -1;
                }
                val_free_len_remaining_in_search -= val_free_loc->len;
                val_free_loc = val_end - val_free_loc->off;
            }
            val = val_free_loc;
        }

        /** Add new record to node **/

        key->ok_oid = 0x277afc;
        key->ok_xid = 0x1af045;

        val->ov_size = nx_block_size;
        val->ov_paddr = 0xe911f;

        // Shift the TOC entries as needed, then add the new entry
        kvoff_t* new_toc_entry = (kvoff_t*)toc_start + NEW_ENTRY_INDEX;
        memmove(new_toc_entry + 1,   new_toc_entry,   (node->btn_nkeys - NEW_ENTRY_INDEX) * sizeof(kvoff_t));

        new_toc_entry->k = (char*)key - key_start;
        new_toc_entry->v = val_end - (char*)val;
        
        /** Set checksum **/

        uint64_t block_checksum = compute_block_cksum(node);
        *((uint64_t*)node) = block_checksum;

        /** Print out the contents of the B-tree node that is in memory **/
        
        print_btree_node_phys(node);

        /** Write the data to disk **/

        if (write_blocks(node, addr, 1) != 1) {
            fprintf(stderr, "\nABORT: Error writing to %#lx.\n", addr);
            return -1;
        } else {
            printf("\nSuccessfully wrote new node to %#lx.\n", addr);
        }

        /** Cleanup **/

        free(node);
    }

    /** Create an FS-root B-tree node and write it to disk **/
    if (false) {
        /** Declare data needed to construct B-tree node **/

        /**
         * Array containing data for all of the file-system records which need to
         * be added to the node. Each entry comprises:
         * 
         * - 0: The record type,    as in `obj_id_and_type`
         * - 1: The record ID,      as in `obj_id_and_type`
         * - 2: The block address   of the target node
         * - 3: The Virtual OID     of the target node
         */
        size_t NUM_RECORDS = 61;
        uint64_t record_data[][4] =  {
            {   0x8,    0xb46a8,    0xd6cf2,          0xa3bc },
            {   0x9,    0xb4703,    0xd4ba8,         0x77e69 },
            {   0x9,    0xb471d,    0xd0add,          0xa3c0 },
            {   0x4,    0xb4746,    0xe14ec,          0xa3c2 },
            {   0x6,    0xb4784,    0xa9a58,          0xa3c5 },
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

        /** Create a B-tree node in memory **/

        btree_node_phys_t* node = calloc(1, nx_block_size); // Using `calloc()` to get zeroed out memory
        if (!node) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
            return -1;
        }

        /** Set node header values **/

        node->btn_o.o_oid       = 0xa450;
        node->btn_o.o_xid       = 0x1bca0d;
        node->btn_o.o_type      = OBJ_VIRTUAL | OBJECT_TYPE_BTREE_NODE;
        node->btn_o.o_subtype   = OBJECT_TYPE_FSTREE;

        /** Set B-trees structure values **/
        
        // node->btn_flags -- do nothing
        
        node->btn_level         = 1;
        
        node->btn_nkeys         = NUM_RECORDS;

        node->btn_table_space.off   = 0;
        node->btn_table_space.len   = node->btn_nkeys * sizeof(kvloc_t);

        node->btn_free_space.off    = 0;    // TODO: this value
        node->btn_free_space.len    = 0;    // TODO: this value
        
        // Empty free list for keys and values
        node->btn_key_free_list.off = 0;
        node->btn_key_free_list.len = 0;
        node->btn_val_free_list.off = 0;
        node->btn_val_free_list.len = 0;

        /** Add records to node **/

        char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
        char* key_start = toc_start + node->btn_table_space.len;
        char* val_end   = (char*)node + nx_block_size;

        kvloc_t* toc_entry  = toc_start;
        j_key_t* key        = key_start;
        oid_t*   val        = (oid_t*)val_end - 1;
        for (size_t i = 0;   i < NUM_RECORDS;   i++, toc_entry++, key++, val--) {
            toc_entry->k.off = (char*)key - key_start;
            toc_entry->k.len = sizeof(j_key_t);
            toc_entry->v.off = val_end - (char*)val;
            toc_entry->v.len = sizeof(oid_t);

            uint64_t obj_type   = record_data[i][0];
            uint64_t obj_id     = record_data[i][1];
            
            key->obj_id_and_type = (obj_type << OBJ_TYPE_SHIFT) | obj_id;

            *val = record_data[i][3];
        }
        
        /** Now we can delcare where the node's free space is **/
        
        node->btn_free_space.off = (char*)key - key_start;
        node->btn_free_space.len = (char*)(val - 1) - (char*)key;

        /** Set checksum **/

        uint64_t block_checksum = compute_block_cksum(node);
        *((uint64_t*)node) = block_checksum;

        /** Print out the contents of the B-tree node that is in memory **/
        
        print_btree_node_phys(node);

        /** Write the data to disk **/

        if (write_blocks(node, 0xd33ce, 1) != 1) {
            fprintf(stderr, "\nWRITE ERROR!\n");
        } else {
            printf("\nWRITE COMPLETED SUCCESSFULLY\n");
        }

        /** Cleanup **/

        free(node);
    }

    /** Modify an FS-root B-tree non-leaf node and write the new version to disk **/
    if (false) {
        long read_from  = 0xd3aef;
        long write_to   = 0xd3aef;

        /** Read the B-tree node to modify **/
        btree_node_phys_t* node = malloc(nx_block_size);
        if (!node) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
            return -1;
        }
        if (read_blocks(node, read_from, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block %#lx.\n", read_from);
            return -1;
        }

        /** Modify the `obj_id_and_type` field of given entries **/
        if (false) {
            /** Declare data needed to edit node **/

            /**
             * Array containing data for all of the OID mappings which need to
             * be edited. Each entry comprises:
             * 
             * - 0: The index of the entry within the node
             * - 1: The desired `obj_id_and_type` value
             */
            size_t NUM_RECORDS = 1; // The number of records that will be modified; the size of the array `record_data`
            uint64_t record_data[][2] = {
                { 59, ((uint64_t)0x4 << OBJ_TYPE_SHIFT) | 0xaea315 },
            };

            /** Edit records as specified in `entry_data` **/

            char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
            char* key_start = toc_start + node->btn_table_space.len;
            char* val_end   = (char*)node + nx_block_size;
            if (node->btn_flags & BTNODE_ROOT) {
                val_end -= sizeof(btree_info_t);
            }

            for (size_t i = 0; i < NUM_RECORDS; i++) {
                kvloc_t* toc_entry = (kvloc_t*)toc_start + record_data[i][0];
                
                j_key_t* key = key_start + toc_entry->k.off;
                key->obj_id_and_type = record_data[i][1];
            }
        }
        
        /** Modify the target Virtual OID of given entries **/
        if (false) {
            /** Declare data needed to edit node **/

            /**
             * Array containing data for all of the OID mappings which need to
             * be edited. Each entry comprises:
             * 
             * - 0: The index of the entry within the node
             * - 1: The target Virtual OID
             */
            size_t NUM_RECORDS = 1; // The number of records that will be modified; the size of the array `record_data`
            uint64_t record_data[][2] = {
                { 32, 0x20a49f },
            };

            /** Edit records as specified in `entry_data` **/

            char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
            char* key_start = toc_start + node->btn_table_space.len;
            char* val_end   = (char*)node + nx_block_size;
            if (node->btn_flags & BTNODE_ROOT) {
                val_end -= sizeof(btree_info_t);
            }

            for (size_t i = 0; i < NUM_RECORDS; i++) {
                kvloc_t* toc_entry = (kvloc_t*)toc_start + record_data[i][0];
                
                oid_t* target_virt_oid = val_end - toc_entry->v.off;
                *target_virt_oid = record_data[i][1];
            }
        }

        /** Set checksum **/

        uint64_t block_checksum = compute_block_cksum(node);
        *((uint64_t*)node) = block_checksum;

        /** Print out the contents of the B-tree node that is in memory **/
        
        print_btree_node_phys(node);

        /** Write the data to disk **/

        if (write_blocks(node, write_to, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to write to block %#lx.\n", write_to);
            return -1;
        }
        printf("Successfully wrote modified version of block %#lx to %#lx.\n", read_from, write_to);

        /** Cleanup **/

        free(node);
    }

    /** Modify an Omap B-tree node and write the new version to disk **/
    if (false) {
        long read_from  = 0xe98c5;
        long write_to   = 0xd3691;

        /** Declare data needed to edit node **/

        /**
         * Array containing data for all of the OID mappings which need to
         * be edited. Each entry comprises:
         * 
         * - 0: The index of the entry within the node
         * - 1: The target block address
         */
        size_t NUM_RECORDS = 6; // The number of records that will be modified; the size of the array `record_data`
        uint64_t record_data[][2] = {
            { 50, 0xd6e9d },
            { 66, 0xd2a9b },
            { 75, 0xd214b },
            { 76, 0xd2155 },
            { 78, 0xd271b },
            { 81, 0xd0fd1 },
        };

        /** Read the B-tree node to modify **/
        btree_node_phys_t* node = malloc(nx_block_size);
        if (!node) {
            fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `node`.\n");
            return -1;
        }
        if (read_blocks(node, read_from, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to read block %#lx.\n", read_from);
            return -1;
        }

        /** Edit records as specified in `entry_data` **/

        char* toc_start = (char*)node->btn_data + node->btn_table_space.off;
        char* key_start = toc_start + node->btn_table_space.len;
        char* val_end   = (char*)node + nx_block_size;
        if (node->btn_flags & BTNODE_ROOT) {
            val_end -= sizeof(btree_info_t);
        }

        for (size_t i = 0; i < NUM_RECORDS; i++) {
            kvoff_t* toc_entry = (kvoff_t*)toc_start + record_data[i][0];
            
            omap_val_t* val = val_end - toc_entry->v;
            val->ov_paddr = record_data[i][1];
        }

        /** Set checksum **/

        uint64_t block_checksum = compute_block_cksum(node);
        *((uint64_t*)node) = block_checksum;

        /** Print out the contents of the B-tree node that is in memory **/
        
        print_btree_node_phys(node);

        /** Write the data to disk **/

        if (write_blocks(node, write_to, 1) != 1) {
            fprintf(stderr, "\nABORT: Failed to write to block %#lx.\n", write_to);
            return -1;
        }
        printf("Successfully wrote modified version of block %#lx to %#lx.\n", read_from, write_to);

        /** Cleanup **/

        free(node);
    }

    fclose(nx);
    
    return 0;
}
