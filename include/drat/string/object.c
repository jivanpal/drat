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
        case OBJ_VIRTUAL:
            return "Virtual";
        case OBJ_EPHEMERAL:
            return "Ephemeral";
        case OBJ_PHYSICAL:
            return "Physical";
        default:
            return "(invalid type)";
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
    uint32_t flag_constants[] = {
        OBJ_NOHEADER,
        OBJ_ENCRYPTED,
        OBJ_NONPERSISTENT,
    };

    char* flag_strings[] = {
        "No-header",
        "Encrypted",
        "Non-persistent (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)",
    };

    // Allocate sufficient memory to store the longest possible string.
    size_t max_mem_required = 0;
    for (int i = 0; i < 3; i++) {
        max_mem_required += strlen(flag_strings[i]) + 2;
        // `+ 2` accounts for appending ", " to each string
    }
    char* result_string = malloc(max_mem_required);
    if (!result_string) {
        fprintf(stderr, "\nABORT: get_obj_type_flags_string: Could not allocate sufficient memory for `result_string`.\n");
    }
    
    char* cursor = result_string;

    // Go through possible flags. If a flag is set, append its corresponding
    // string to `result_string`. The flags are comma-delimited.
    for (int i = 0; i < 3; i++) {
        if (o_type & flag_constants[i]) {
            if (cursor != result_string) {
                *cursor++ = ',';
                *cursor++ = ' ';
            }
            size_t flag_string_len = strlen(flag_strings[i]);
            memcpy(cursor, flag_strings[i], flag_string_len);
            cursor += flag_string_len;
        }
    }

    if (cursor == result_string) {
        // No strings were added, so it must be that no flags are set.
        char* no_flags_string = "(none)";
        size_t no_flags_string_len = strlen(no_flags_string);
        memcpy(cursor, no_flags_string, no_flags_string_len);
        cursor += no_flags_string_len;
    }

    *cursor = '\0';
    
    // Free up excess allocated memory.
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
    // This function is implemented this way so that the caller receives a
    // pointer that is allocated using `malloc()` rather than a stack pointer.

    // This string is a legal `sprintf()` format string.
    char* default_string = "Unknown type (0x%08x) --- perhaps this type was introduced in a later version of APFS than that published on 2019-02-27.";

    size_t NUM_FLAGS = 26;
    uint32_t flag_constants[] = {
        OBJECT_TYPE_NX_SUPERBLOCK,
        OBJECT_TYPE_BTREE,
        OBJECT_TYPE_BTREE_NODE,
        OBJECT_TYPE_SPACEMAN,
        OBJECT_TYPE_SPACEMAN_CAB,
        OBJECT_TYPE_SPACEMAN_CIB,
        OBJECT_TYPE_SPACEMAN_BITMAP,
        OBJECT_TYPE_OMAP,
        OBJECT_TYPE_CHECKPOINT_MAP,
        OBJECT_TYPE_FS,
        OBJECT_TYPE_NX_REAPER,
        OBJECT_TYPE_NX_REAP_LIST,
        OBJECT_TYPE_EFI_JUMPSTART,
        OBJECT_TYPE_NX_FUSION_WBC,
        OBJECT_TYPE_NX_FUSION_WBC_LIST,
        OBJECT_TYPE_ER_STATE,
        OBJECT_TYPE_GBITMAP,
        OBJECT_TYPE_GBITMAP_BLOCK,
        OBJECT_TYPE_ER_RECOVERY_BLOCK,
        OBJECT_TYPE_SNAP_META_EXT,
        OBJECT_TYPE_INTEGRITY_META,
        OBJECT_TYPE_RESERVED_20,
        OBJECT_TYPE_INVALID,
        OBJECT_TYPE_TEST,
        OBJECT_TYPE_CONTAINER_KEYBAG,
        OBJECT_TYPE_VOLUME_KEYBAG,
    };
    char* flag_strings[] = {
        "Container superblock",
        "B-tree (root node)",
        "B-tree (non-root) node",
        "Space manager",
        "Space manager chunk-info address block",
        "Space manager chunk-info block",
        "Space manager free-space bitmap",
        "Object map",
        "Checkpoint map",
        "APFS volume",
        "Container reaper",
        "Container reaper list",
        "EFI jumpstart boot info",
        "Fusion device write-back cache state",
        "Fusion device write-back cache list",
        "Encryption-rolling state",
        "General-purpose bitmap",
        "General purpose bitmap block",
        "Encryption-rolling recovery block",
        "Additional snapshot metadata",
        "Integrity metadata",
        "Reserved type/subtype (0x20)",
        "(invalid type / no subtype)",
        "A type reserved for testing (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)",
        "Container keybag",
        "Volume keybag",
    };

    // Allocate sufficient memory to store the longest flag string.
    size_t max_mem_required = strlen(default_string) + 8;   // Add 8 to account for `%x` being replaced with up to 8 characters.
    for (uint32_t i = 0; i < NUM_FLAGS; i++) {
        size_t flag_string_length = strlen(flag_strings[i]);
        if (max_mem_required < flag_string_length) {
            max_mem_required = flag_string_length;
        }
    }
    max_mem_required++; // Account for terminating NULL byte

    char* result_string = malloc(max_mem_required);
    if (!result_string) {
        fprintf(stderr, "\nABORT: get_o_type_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }
    
    // Set the right string
    uint32_t masked_o_type = o_type & OBJECT_TYPE_MASK;
    snprintf(result_string, max_mem_required, default_string, masked_o_type);
    for (size_t i = 0; i < NUM_FLAGS; i++) {
        if (masked_o_type == flag_constants[i]) {
            memcpy(result_string, flag_strings[i], strlen(flag_strings[i]) + 1);
            break;
        }
    }

    // De-allocate excess memory
    result_string = realloc(result_string, strlen(result_string) + 1);

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
    // This function is implemented this way so that the caller receives a
    // pointer that is allocated using `malloc()` rather than a stack pointer.

    // Check if `o_subtype` is a value representing a regular type.
    // That is, return the result of `get_o_type_string()` unless that result
    // begins with "Unknown type".
    char* result_string = get_o_type_string(o_subtype);
    if (strstr(result_string, "Unknown type") != result_string) {
        return result_string;
    }

    // Set `result_string` to an empty string,
    // so that `strlen(result_string) == 0`.
    *result_string = '\0';

    // This string is a legal `sprintf()` format string.
    char* default_string = "Unknown subtype (0x%08x) --- perhaps this subtype was introduced in a later version of APFS than that published on 2019-02-27.";

    size_t NUM_FLAGS = 9;
    uint32_t flag_constants[] = {
        OBJECT_TYPE_SPACEMAN_FREE_QUEUE,
        OBJECT_TYPE_EXTENT_LIST_TREE,
        OBJECT_TYPE_FSTREE,
        OBJECT_TYPE_BLOCKREFTREE,
        OBJECT_TYPE_SNAPMETATREE,
        OBJECT_TYPE_OMAP_SNAPSHOT,
        OBJECT_TYPE_FUSION_MIDDLE_TREE,
        OBJECT_TYPE_GBITMAP_TREE,
        OBJECT_TYPE_FEXT_TREE,
    };
    char* flag_strings[] = {
        "Space manager free-space queue",
        "Extents-list tree",
        "File-system records tree",
        "Extent references tree",
        "Volume snapshot metadata tree",
        "Object map snapshots tree",
        "Fusion inter-drive block-mapping tree",
        "B-tree of general-purpose bitmaps",
        "B-tree of file extents",
    };

    // Allocate sufficient memory to store the longest flag string.
    size_t max_mem_required = strlen(default_string) + 8;   // Add 8 to account for `%x` being replaced with up to 8 characters.
    for (uint32_t i = 0; i < NUM_FLAGS; i++) {
        size_t flag_string_length = strlen(flag_strings[i]);
        if (max_mem_required < flag_string_length) {
            max_mem_required = flag_string_length;
        }
    }
    max_mem_required++; // Account for terminating NULL byte
    result_string = realloc(result_string, max_mem_required);
    if (!result_string) {
        fprintf(stderr, "\nABORT: get_o_subtype_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    // Set the right string
    uint32_t masked_o_subtype = o_subtype & OBJECT_TYPE_MASK;
    for (size_t i = 0; i < NUM_FLAGS; i++) {
        if (masked_o_subtype == flag_constants[i]) {
            memcpy(result_string, flag_strings[i], strlen(flag_strings[i]) + 1);
            break;
        }
    }

    // If no string set (due to no flags present), then report "Unknown type".
    if (strlen(result_string) == 0) {
        sprintf(result_string, default_string, masked_o_subtype);
    }

    // De-allocate excess memory
    result_string = realloc(result_string, strlen(result_string) + 1);

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
