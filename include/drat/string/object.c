/**
 * Functions that generate nicely formatted strings
 * representing data found in APFS objects.
 */

#include "object.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drat/string/common.h>

/**
 * Get a human-readable string describing the object storage type for a given
 * type bitfield.
 * 
 * o_type:  A 32-bit bitfield whose upper 16 bits represent an APFS storage
 *          type. Examples include the `o_type` field of `obj_phys_t`,
 *          and the `om_tree_type` field of `omap_phys_t`.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. This pointer needn't be
 *      freed by the caller.
 */
char* o_storage_type_to_string(uint32_t o_type) {
    switch (o_type & OBJ_STORAGETYPE_MASK) {
        case OBJ_VIRTUAL:   return "Virtual";
        case OBJ_EPHEMERAL: return "Ephemeral";
        case OBJ_PHYSICAL:  return "Physical";
        default:            return "(invalid type)";
    }
}

/**
 * Get a human-readable string that lists the type flags that are set on a given
 * object. This list does not include storage types; namely, it does not specify
 * whether the object is Physical, Virtual, or Ephemeral.
 * 
 * o_type:     A 32-bit bitfield whose lower 16 bits represent an APFS object type.
 *          Examples include the `o_type` field of `obj_phys_t`,
 *          and the `cpm_type` field of `checkpoint_mapping_t`.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_o_type_flags_string(uint32_t o_type) {
    struct u64_string_mapping flags[] = {
        { OBJ_NOHEADER,         "No-header" },
        { OBJ_ENCRYPTED,        "Encrypted" },
        { OBJ_NONPERSISTENT,    "Non-persistent (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)" },
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
        if (o_type & flags[i].value) {
            bytes_written += snprintf(
                result_string + bytes_written,
                bufsize - bytes_written,
                "%s, ",
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
        snprintf(result_string, bufsize, "(none)");
    } else {
        // Trim trailing comma and space
        result_string[strlen(result_string) - 2] = '\0';
    }

    // Truncate buffer
    result_string = realloc(result_string, strlen(result_string) + 1);

    return result_string;
}

/**
 * Get a human-readable string describing a given `o_type` value.
 * 
 * o_type:  A 32-bit bitfield whose lower 16 bits represent an APFS object type.
 *          Examples include the `o_type` field of `obj_phys_t`,
 *          and the `cpm_type` field of `checkpoint_mapping_t`.
 * 
 * RETURN VALUE:
 *      A pointer to the first character in the string. This pointer must be
 *      freed when it is no longer needed.
 * 
 *      If the specified type is not recognised, the string returned will begin
 *      with "Unknown type", i.e. given the return value `return_value`, the
 *      expression `strstr(return_value, "Unknown type") == return_value` will
 *      evaluate to `true`.
 */
char* get_o_type_string(uint32_t o_type) {
    struct u64_string_mapping types[] = {
        { OBJECT_TYPE_NX_SUPERBLOCK,        "Container superblock" },
        { OBJECT_TYPE_BTREE,                "B-tree (root node)" },
        { OBJECT_TYPE_BTREE_NODE,           "B-tree (non-root) node" },
        { OBJECT_TYPE_SPACEMAN,             "Space manager" },
        { OBJECT_TYPE_SPACEMAN_CAB,         "Space manager chunk-info address block" },
        { OBJECT_TYPE_SPACEMAN_CIB,         "Space manager chunk-info block" },
        { OBJECT_TYPE_SPACEMAN_BITMAP,      "Space manager free-space bitmap" },
        { OBJECT_TYPE_OMAP,                 "Object map" },
        { OBJECT_TYPE_CHECKPOINT_MAP,       "Checkpoint map" },
        { OBJECT_TYPE_FS,                   "APFS volume" },
        { OBJECT_TYPE_NX_REAPER,            "Container reaper" },
        { OBJECT_TYPE_NX_REAP_LIST,         "Container reaper list" },
        { OBJECT_TYPE_EFI_JUMPSTART,        "EFI jumpstart boot info" },
        { OBJECT_TYPE_NX_FUSION_WBC,        "Fusion device write-back cache state" },
        { OBJECT_TYPE_NX_FUSION_WBC_LIST,   "Fusion device write-back cache list" },
        { OBJECT_TYPE_ER_STATE,             "Encryption-rolling state" },
        { OBJECT_TYPE_GBITMAP,              "General-purpose bitmap" },
        { OBJECT_TYPE_GBITMAP_BLOCK,        "General purpose bitmap block" },
        { OBJECT_TYPE_ER_RECOVERY_BLOCK,    "Encryption-rolling recovery block" },
        { OBJECT_TYPE_SNAP_META_EXT,        "Additional snapshot metadata" },
        { OBJECT_TYPE_INTEGRITY_META,       "Integrity metadata" },
        { OBJECT_TYPE_RESERVED_20,          "Reserved type/subtype (0x20)" },
        { OBJECT_TYPE_INVALID,              "(invalid type / no subtype)" },
        { OBJECT_TYPE_TEST,                 "A type reserved for testing (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)" },
        { OBJECT_TYPE_CONTAINER_KEYBAG,     "Container keybag" },
        { OBJECT_TYPE_VOLUME_KEYBAG,        "Volume keybag" },
    };

    char* result_string = NULL;

    uint32_t masked_o_type = o_type & OBJECT_TYPE_MASK;
    for (size_t i = 0; i < ARRAY_SIZE(types); i++) {
        if (masked_o_type == types[i].value) {
            if (asprintf(&result_string, "%s", types[i].string) == -1) {
                fprintf(stderr, "\nERROR: %s: Couldn't allocate sufficient memory for `result_string`.\n", __func__);
                return NULL;
            }
        }
        break;
    }

    if (!result_string) {
        // No role matched; use default string.
        if (
            asprintf(
                &result_string,
                "Unknown type (%#08"PRIx32") --- perhaps this type was introduced in"
                " a later version of APFS than that published on 2019-02-27.",
                masked_o_type
            ) == -1
        ) {
            fprintf(stderr, "%s: Couldn't allocate sufficient memory for `result_string` when returning default string.\n", __func__);
            return NULL;
        }
    }

    return result_string;
}

/**
 * Get a human-readable string describing a given `o_subtype` value.
 * 
 * o_subtype:   A 32-bit field that represents an APFS object subtype.
 *              Examples include the `o_subtype` field of `obj_phys_t`,
 *              and the `cpm_subtype` field of `checkpoint_mapping_t`.
 * 
 * RETURN VALUE:
 *      A pointer to the first character in the string. This pointer must be
 *      freed when it is no longer needed.
 * 
 *      If the specified subtype is not recognised, the string returned will
 *      begin with "Unknown subtype", i.e. given the return value `return_value`,
 *      the expression `strstr(return_value, "Unknown subtype") == return_value`
 *      will evaluate to `true`.
 */
char* get_o_subtype_string(uint32_t o_subtype) {
    // Check if `o_subtype` is a value representing a regular type.
    // That is, return the result of `get_o_type_string()` unless that result
    // begins with "Unknown type".
    char* result_string = get_o_type_string(o_subtype);
    if (strstr(result_string, "Unknown type") != result_string) {
        return result_string;
    }
    free(result_string);
    result_string = NULL;

    struct u64_string_mapping subtypes[] = {
        { OBJECT_TYPE_SPACEMAN_FREE_QUEUE,  "Space manager free-space queue" },
        { OBJECT_TYPE_EXTENT_LIST_TREE,     "Extents-list tree" },
        { OBJECT_TYPE_FSTREE,               "File-system records tree" },
        { OBJECT_TYPE_BLOCKREFTREE,         "Extent references tree" },
        { OBJECT_TYPE_SNAPMETATREE,         "Volume snapshot metadata tree" },
        { OBJECT_TYPE_OMAP_SNAPSHOT,        "Object map snapshots tree" },
        { OBJECT_TYPE_FUSION_MIDDLE_TREE,   "Fusion middle tree" },
        { OBJECT_TYPE_GBITMAP_TREE,         "B-tree of general-purpose bitmaps" },
        { OBJECT_TYPE_FEXT_TREE,            "B-tree of file extents" },
    };

    uint32_t masked_o_subtype = o_subtype & OBJECT_TYPE_MASK;
    for (size_t i = 0; i < ARRAY_SIZE(subtypes); i++) {
        if (masked_o_subtype == subtypes[i].value) {
            if (asprintf(&result_string, "%s", subtypes[i].string) == -1) {
                fprintf(stderr, "\nERROR: %s: Couldn't allocate sufficient memory for `result_string`.\n", __func__);
                return NULL;
            }
        }
        break;
    }

    if (!result_string) {
        // No role matched; use default string.
        if (
            asprintf(
                &result_string,
                "Unknown subtype (%#08"PRIx32") --- perhaps this subtype was introduced in"
                " a later version of APFS than that published on 2019-02-27.",
                masked_o_subtype
            ) == -1
        ) {
            fprintf(stderr, "%s: Couldn't allocate sufficient memory for `result_string` when returning default string.\n", __func__);
            return NULL;
        }
    }

    return result_string;
}

/**
 * Print a nicely formatted string describing the data contained in the header
 * of an APFS object.
 */
void print_obj_phys(obj_phys_t* obj) {
    char* type_flags_string = get_o_type_flags_string(obj->o_type);
    char* type_string       = get_o_type_string(obj->o_type);
    char* subtype_string    = get_o_subtype_string(obj->o_subtype);

    // Print the info
    printf("Stored checksum:    0x%016" PRIx64 "\n",   *(uint64_t*)obj);
    printf("OID:                0x%" PRIx64 "\n",      obj->o_oid);
    printf("XID:                0x%" PRIx64 "\n",      obj->o_xid);
    printf("Storage type:       %s\n",          o_storage_type_to_string(obj->o_type));
    printf("Type flags:         %s\n",          type_flags_string);
    printf("Type:               %s\n",          type_string);
    printf("Subtype:            %s\n",          subtype_string);

    free(type_flags_string);
    free(type_string);
    free(subtype_string);
}
