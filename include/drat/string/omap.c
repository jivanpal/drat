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

#include <drat/string/common.h>
#include <drat/string/object.h>

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
    struct u64_string_mapping flags[] = {
        { OMAP_MANUALLY_MANAGED,    "No snapshot support" },
        { OMAP_ENCRYPTING,          "Transitioning to encrypted state" },
        { OMAP_DECRYPTING,          "Transitioning to decrypted state" },
        { OMAP_KEYROLLING,          "Transitioning from on old encryption key to a new encryption key" },

        /* 
         * The following constant does not convey any useful info about the
         * object map itself, but the objects contained within an object map
         * refer to this field to convey info about their encryption state.
         * This constant is listed here for completeness.
         */
        // OMAP_CRYPTO_GENERATION
    };

    // Initialise result buffer as empty string
    const size_t bufsize = 2048;
    char* result_string = malloc(bufsize);
    if (!result_string) {
        fprintf(stderr, "\nERROR: %s: Couldn't create buffer `result_string`.\n", __func__);
        return NULL;
    }
    *result_string = '\0';

    size_t bytes_written = 0;
    for (size_t i = 0; i < ARRAY_SIZE(flags); i++) {
        if (omap->om_flags & flags[i].value) {
            bytes_written += snprintf(
                result_string + bytes_written,
                bufsize - bytes_written,
                "- %s\n",
                flags[i].string
            );
            
            if (bytes_written > bufsize - 1) {
                // Exhausted buffer; return early.
                fprintf(stderr, "\nERROR: %s: Buffer `result_string` too small for entire result.\n", __func__);
                return result_string;
            }
        }
    }

    if (bytes_written == 0) {
        // No flags set; use default string.
        snprintf(result_string, bufsize, "- No flags are set.\n");
    }
    
    // Truncate buffer
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
