/**
 * Functions that generate nicely formatted strings representing data found in
 * the checkpoint area. This includes instances of `nx_superblock_t`,
 * `checkpoint_map_phys_t`, and `checkpoint_mapping_t`.
 */

#include "omap.h"

#include <inttypes.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "object.h"

/**
 * Get a human-readable string that lists the flags that are set on a given
 * object map.
 * 
 * omap:    A pointer to the object map in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_om_flags_string(omap_phys_t* omap) {
    // String to use if no flags are set    
    char* no_flags_string = "- No flags are set.\n";
    size_t no_flags_string_len = strlen(no_flags_string);
    
    const int NUM_FLAGS = 4;
    
    uint64_t flag_constants[] = {
        OMAP_MANUALLY_MANAGED,
        OMAP_ENCRYPTING,
        OMAP_DECRYPTING,
        OMAP_KEYROLLING,
        /* 
         * The following constant does not convey any useful info about the
         * object map itself, but the objects contained within an object map
         * refer to this field to convey info about their encryption state.
         * This constant is listed here for completeness.
         */
        // OMAP_CRYPTO_GENERATION,
    };

    char* flag_strings[] = {
        "No snapshot support",
        "Transitioning to encrypted state",
        "Transitioning to decrypted state",
        "Transitioning from on old encryption key to a new encryption key"
    };

    // Allocate sufficient memory for the result string
    size_t max_mem_required = 0;
    for (int i = 0; i < NUM_FLAGS; i++) {
        max_mem_required += strlen(flag_strings[i]) + 3;
        // `+ 3` accounts for prepending "- " and appending "\n" to each string
    }
    if (max_mem_required < no_flags_string_len) {
        max_mem_required = no_flags_string_len;
    }
    max_mem_required++; // Make room for terminating NULL byte

    char* result_string = malloc(max_mem_required);
    if (!result_string) {
        fprintf(stderr, "\nABORT: get_om_flags_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (omap->om_flags & flag_constants[i]) {
            *cursor++ = '-';
            *cursor++ = ' ';
            memcpy(cursor, flag_strings[i], strlen(flag_strings[i]));
            cursor += strlen(flag_strings[i]);
            *cursor++ = '\n';
        }
    }

    if (cursor == result_string) {
        // No strings were added, so it must be that no flags are set.
        memcpy(cursor, no_flags_string, no_flags_string_len);
        cursor += no_flags_string_len;
    }

    *cursor = '\0';

    // Free up excess allocated memory.
    result_string = realloc(result_string, strlen(result_string) + 1);
    return result_string;
}

/**
 * Print a nicely formatted string describing the data contained in an object
 * map, including the data in its header.
 */
void print_omap_phys(omap_phys_t* omap) {
    print_obj_phys(omap);

    char* flags_string = get_om_flags_string(omap);
    printf("Flags:\n%s", flags_string);
    free(flags_string);

    char* type_flags_string = get_o_type_flags_string(omap->om_tree_type);
    char* type_string       = get_o_type_string(omap->om_tree_type);
    printf("Object mappings tree:\n");
    printf("- Storage type:         %s\n",          o_storage_type_to_string(omap->om_tree_type));
    printf("- Type flags:           %s\n",          type_flags_string);
    printf("- Type:                 %s\n",          type_string);
    printf("- Object ID:            0x%" PRIx64 "\n",      omap->om_tree_oid);
    free(type_flags_string);
    free(type_string);

    type_flags_string   = get_o_type_flags_string(omap->om_snapshot_tree_type);
    type_string         = get_o_type_string(omap->om_snapshot_tree_type);
    printf("Snapshots tree:\n");
    printf("- Storage type:         %s\n",          o_storage_type_to_string(omap->om_snapshot_tree_type));
    printf("- Type flags:           %s\n",          type_flags_string);
    printf("- Type:                 %s\n",          type_string);
    printf("- Object ID:            0x%" PRIx64 "\n",      omap->om_snapshot_tree_oid);
    free(type_flags_string);
    free(type_string);

    printf("- Number of snapshots:  %u snapshots\n",    omap->om_snap_count);
    printf("- Latest snapshot XID:  0x%" PRIx64 "\n",          omap->om_most_recent_snap);
    printf("In-progress revert:\n");
    printf("- Minimum XID:          0x%" PRIx64 "\n",      omap->om_pending_revert_min);
    printf("- Maximum XID:          0x%" PRIx64 "\n",      omap->om_pending_revert_max);
}

/**
 * Print a nicely formatted string describing the data contained in an object
 * map key.
 */
void print_omap_key(omap_key_t* omap_key) {
    printf("  - OID:                            0x%" PRIx64 "\n",      omap_key->ok_oid);
    printf("  - XID:                            0x%" PRIx64 "\n",      omap_key->ok_xid);
}

/**
 * Print a nicely formatted string describing the data contained in an object
 * map value.
 */
void print_omap_val(omap_val_t* omap_val) {
    // TODO: Print flags
    printf("  - Object size:                    %u bytes\n",    omap_val->ov_size);
    printf("  - Object address in container:    0x%" PRIx64 "\n",      omap_val->ov_paddr);
}
