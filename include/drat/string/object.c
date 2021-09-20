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

#include <drat/asize.h>

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
    enum_string_mapping_t flags[] = {
        { OBJ_NOHEADER,         "No-header" },
        { OBJ_ENCRYPTED,        "Encrypted" },
        { OBJ_NONPERSISTENT,    "Non-persistent (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)" },
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), o_type & OBJECT_TYPE_FLAGS_MASK, true);
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
 *      with "Unknown value", i.e. given the return value `return_value`, the
 *      expression `strstr(return_value, "Unknown value") == return_value` will
 *      evaluate to `true`.
 */
char* get_o_type_string(uint32_t o_type) {
    enum_string_mapping_t types[] = {
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
        
        // The following object type values extend out of the object type mask
        // area, so a quick workaround which allows us to identify these with a
        // single call to `get_single_enum_string()` is to mask the real values.
        { OBJECT_TYPE_CONTAINER_KEYBAG  & OBJECT_TYPE_MASK,     "Container keybag" },
        { OBJECT_TYPE_VOLUME_KEYBAG     & OBJECT_TYPE_MASK,     "Volume keybag" },
        { OBJECT_TYPE_MEDIA_KEYBAG      & OBJECT_TYPE_MASK,     "Media keybag" },
    };

    return get_single_enum_string(types, ARRAY_SIZE(types), o_type & OBJECT_TYPE_MASK);
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
    // begins with "Unknown value".
    char* result_string = get_o_type_string(o_subtype);
    if (strstr(result_string, "Unknown value") != result_string) {
        return result_string;
    }
    free(result_string);

    enum_string_mapping_t subtypes[] = {
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

    return get_single_enum_string(subtypes, ARRAY_SIZE(subtypes), o_subtype & OBJECT_TYPE_MASK);
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
    printf("Stored checksum:    %#016"PRIx64"\n",   *(uint64_t*)obj);
    printf("OID:                %#"PRIx64"\n",      obj->o_oid);
    printf("XID:                %#"PRIx64"\n",      obj->o_xid);
    printf("Storage type:       %s\n",              o_storage_type_to_string(obj->o_type));
    printf("Type flags:         %s\n",              type_flags_string);
    printf("Type:               %s\n",              type_string);
    printf("Subtype:            %s\n",              subtype_string);

    free(type_flags_string);
    free(type_string);
    free(subtype_string);
}
