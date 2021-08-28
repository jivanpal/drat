/**
 * Functions that generate nicely formatted strings representing data found in
 * File-system-related objects, such as `apfs_superblock_t`.
 */

#include "fs.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <drat/asize.h>
#include <drat/time.h>

#include <drat/string/common.h>
#include <drat/string/object.h>
#include <drat/string/general.h>

/**
 * Get a human-readable string that lists the optional feature flags that are
 * set on a given APFS volume superblock.
 * 
 * apsb:    A pointer to the APFS volume superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_apfs_features_string(apfs_superblock_t* apsb) {
    enum_string_mapping_t flags[] = {
        { APFS_FEATURE_DEFRAG_PRERELEASE,           "Reserved --- To avoid data corruption, this flag must not be set; this flag enabled a prerelease version of the defragmentation system in macOS 10.13 versions. Itʼs ignored by macOS 10.13.6 and later." },
        { APFS_FEATURE_HARDLINK_MAP_RECORDS,        "This volume has hardlink map records." },
        { APFS_FEATURE_DEFRAG,                      "Defragmentation is supported." },
        { APFS_FEATURE_STRICTATIME,                 "File access times are updated every time a file is read." },
        { APFS_FEATURE_VOLGRP_SYSTEM_INO_SPACE,     "This volume supports mounting a system and data volume as a single user-visible volume." },
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), apsb->apfs_features, false);
}

/**
 * Get a human-readable string that lists the optional feature flags that are
 * set on a given APFS volume superblock.
 * 
 * apsb:    A pointer to the APFS volume superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_apfs_readonly_compatible_features_string(apfs_superblock_t* apsb) {
    enum_string_mapping_t flags[] = {
        // empty
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), apsb->apfs_readonly_compatible_features, false);
}

/**
 * Get a human-readable string that lists the backward-incompatible feature
 * flags that are set on a given APFS volume superblock.
 * 
 * apsb:    A pointer to the APFS volume superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_apfs_incompatible_features_string(apfs_superblock_t* apsb) {
    enum_string_mapping_t flags[] = {
        { APFS_INCOMPAT_CASE_INSENSITIVE,           "Filenames on this volume are case-insensitive." },
        { APFS_INCOMPAT_DATALESS_SNAPS,             "At least one snapshot with no data exists for this volume." },
        { APFS_INCOMPAT_ENC_ROLLED,                 "This volume's encryption has changed keys at least once." },
        { APFS_INCOMPAT_NORMALIZATION_INSENSITIVE,  "Filenames on this volume are normalization insensitive." },
        { APFS_INCOMPAT_INCOMPLETE_RESTORE,         "This volume is being restored, or a restore operation to this volume was uncleanly aborted." },
        { APFS_INCOMPAT_SEALED_VOLUME,              "This volume is sealed (cannot be modified)." },
        { APFS_INCOMPAT_RESERVED_40,                "Reserved flag (0x40)." },
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), apsb->apfs_incompatible_features, false);
}

/**
 * Get a human-readable string that lists the volume flags that are
 * set on a given APFS volume superblock.
 * 
 * apsb:    A pointer to the APFS volume superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_apfs_fs_flags_string(apfs_superblock_t* apsb) {
    enum_string_mapping_t flags[] = {
        { APFS_FS_UNENCRYPTED,              "Volume is unencrypted." },
        { APFS_FS_RESERVED_2,               "Reserved flag (0x2)." },
        { APFS_FS_RESERVED_4,               "Reserved flag (0x4)." },
        { APFS_FS_ONEKEY,                   "Single VEK (volume encryption key) for all files in this volume." },
        { APFS_FS_SPILLEDOVER,              "Volume has run out of allocated space on SSD, so has spilled over to other drives." },
        { APFS_FS_RUN_SPILLOVER_CLEANER,    "Volume has spilled over and spillover cleaner must be run." },
        { APFS_FS_ALWAYS_CHECK_EXTENTREF,   "When deciding whether to overwrite a file extent, always consult the extent reference tree." },
        { APFS_FS_RESERVED_80,              "Reserved flag (0x80)." },
        { APFS_FS_RESERVED_100,             "Reserved flag (0x100)." },
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), apsb->apfs_fs_flags, false);
}

/**
 * Get a human-readable string describing the role of a given APFS volume
 * superblock.
 * 
 * apsb:    A pointer to the APFS volume superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_apfs_role_string(apfs_superblock_t* apsb) {
    enum_string_mapping_t roles[] = {
        { APFS_VOL_ROLE_NONE,           "(no role)" },
        { APFS_VOL_ROLE_SYSTEM,         "System (contains a root directory for the system)" },
        { APFS_VOL_ROLE_USER,           "User (contains users' home directories)" },
        { APFS_VOL_ROLE_RECOVERY,       "Recovery (contains a recovery system)" },
        { APFS_VOL_ROLE_VM,             "Virtual memory (used as swap space for virtual memory)" },
        { APFS_VOL_ROLE_PREBOOT,        "Preboot (contains files needed to boot from an encrypted volumes)" },
        { APFS_VOL_ROLE_INSTALLER,      "Installer (used by the OS installer)" },
        { APFS_VOL_ROLE_DATA,           "Data (contains mutable data)" },
        { APFS_VOL_ROLE_BASEBAND,       "Baseband (used by the radio firmware)" },
        { APFS_VOL_ROLE_UPDATE,         "Update (used by the software update mechanism)" },
        { APFS_VOL_ROLE_XART,           "xART (used to manage OS access to secure user data" },
        { APFS_VOL_ROLE_HARDWARE,       "Hardware (used for firmware data)" },
        { APFS_VOL_ROLE_BACKUP,         "Backup (used by Time Machine to store backups)" },
        { APFS_VOL_ROLE_RESERVED_7,     "Reserved role 7 (Sidecar?) (role field value 0x1c0)" },
        { APFS_VOL_ROLE_RESERVED_8,     "Reserved role 8 (role field value 0x200)" },
        { APFS_VOL_ROLE_ENTERPRISE,     "Enterprise (used to store enterprise-managed data)" },
        { APFS_VOL_ROLE_RESERVED_10,    "Reserved role 10 (role field value 0x280)" },
        { APFS_VOL_ROLE_PRELOGIN,       "Pre-login (used to store system data used before login)" },
    };

    return get_single_enum_string(roles, ARRAY_SIZE(roles), apsb->apfs_role);
}

void print_apfs_modified_by(apfs_modified_by_t* data) {
    printf("- ID: %s\n", data->id);
    printf("- Timestamp: %s", apfs_timestamp_to_string(data->timestamp));
    printf("- Last XID: %"PRIx64"\n", data->last_xid);
}

/**
 * Print a nicely formatted string describing the data contained in a given
 * APFS volume superblock.
 * 
 * apsb:    A pointer to the APFS volume superblock in question.
 */
void print_apfs_superblock(apfs_superblock_t* apsb) {
    print_obj_phys(apsb);   // `apsb` equals `&(apsb->apfs_o)`
    printf("\n");

    char magic_string[] = {
        (char)(apsb->apfs_magic),
        (char)(apsb->apfs_magic >> 8),
        (char)(apsb->apfs_magic >> 16),
        (char)(apsb->apfs_magic >> 24),
        '\0'
    };
    printf("Magic string:                           %s\n",  magic_string);
    printf("Index within container volume array:    %u\n",  apsb->apfs_fs_index);
    printf("\n");

    printf("Volume name:        ### %s ###\n",  apsb->apfs_volname);
    
    char* tmp_string = get_apfs_role_string(apsb);
    printf("Role:               %s", tmp_string);
    free(tmp_string);
    
    printf("\n");

    tmp_string = get_apfs_fs_flags_string(apsb);
    printf("Flags:\n%s", tmp_string);
    free(tmp_string);

    tmp_string = get_apfs_features_string(apsb);
    printf("Supported features:\n%s", tmp_string);
    free(tmp_string);

    tmp_string = get_apfs_readonly_compatible_features_string(apsb);
    printf("Supported read-only compatible features:\n%s", tmp_string);
    free(tmp_string);

    tmp_string = get_apfs_incompatible_features_string(apsb);
    printf("Backward-incompatible features:\n%s", tmp_string);
    free(tmp_string);
    
    printf("\n");

    printf("Last unmount time:                  %s",    apfs_timestamp_to_string(apsb->apfs_unmount_time));
    printf("Last modification time:             %s",    apfs_timestamp_to_string(apsb->apfs_last_mod_time));
    printf("\n");

    printf("Reserved blocks:                    %" PRIu64 " blocks\n", apsb->apfs_fs_reserve_block_count);
    printf("Block quota:                        %" PRIu64 " blocks\n", apsb->apfs_fs_quota_block_count);
    printf("Allocated blocks:                   %" PRIu64 " blocks\n", apsb->apfs_fs_alloc_count);
    printf("\n");

    printf("Volume object map Physical OID:     0x%" PRIx64 "\n",    apsb->apfs_omap_oid);
    printf("\n");
    
    printf("Root tree info:\n");
    printf("- OID:              0x%" PRIx64 "\n",  apsb->apfs_root_tree_oid);
    printf("- Storage type:     %s\n",      o_storage_type_to_string(apsb->apfs_root_tree_type));
    
    tmp_string = get_o_type_flags_string(apsb->apfs_root_tree_type);
    printf("- Type flags:       %s\n",  tmp_string);
    free(tmp_string);

    tmp_string = get_o_type_string(apsb->apfs_root_tree_type);
    printf("- Object type:      %s\n",  tmp_string);
    free(tmp_string);

    printf("\n");

    printf("Extent-reference tree info:\n");
    printf("- OID:              0x%" PRIx64 "\n",  apsb->apfs_extentref_tree_oid);
    printf("- Storage type:     %s\n",      o_storage_type_to_string(apsb->apfs_extentref_tree_type));
    
    tmp_string = get_o_type_flags_string(apsb->apfs_extentref_tree_type);
    printf("- Type flags:       %s\n",  tmp_string);
    free(tmp_string);

    tmp_string = get_o_type_string(apsb->apfs_extentref_tree_type);
    printf("- Object type:      %s\n",  tmp_string);
    free(tmp_string);

    printf("\n");

    printf("Snapshot metadata tree info:\n");
    printf("- OID:              0x%" PRIx64 "\n",  apsb->apfs_snap_meta_tree_oid);
    printf("- Storage type:     %s\n",      o_storage_type_to_string(apsb->apfs_snap_meta_tree_type));
    
    tmp_string = get_o_type_flags_string(apsb->apfs_snap_meta_tree_type);
    printf("- Type flags:       %s\n",  tmp_string);
    free(tmp_string);

    tmp_string = get_o_type_string(apsb->apfs_snap_meta_tree_type);
    printf("- Object type:      %s\n",  tmp_string);
    free(tmp_string);

    printf("\n");

    printf("On next mount, revert to:\n");
    printf("- snapshot with this XID:                           0x%" PRIx64 "\n",  apsb->apfs_revert_to_xid);
    printf("- APFS volume superblock with this Physical OID:    0x%" PRIx64 "\n",  apsb->apfs_revert_to_sblock_oid);
    printf("\n");

    printf("Next file-system object ID that will be assigned:   0x%" PRIx64 "\n",  apsb->apfs_next_obj_id);
    printf("Next document ID that will be assigned:             0x%x\n",    apsb->apfs_next_doc_id);
    printf("\n");

    printf("Number of:\n");
    printf("\n");
    printf("- regular files:                %" PRIu64 "\n",    apsb->apfs_num_files);
    printf("- directories:                  %" PRIu64 "\n",    apsb->apfs_num_directories);
    printf("- symbolic links:               %" PRIu64 "\n",    apsb->apfs_num_symlinks);
    printf("- other file-system objects:    %" PRIu64 "\n",    apsb->apfs_num_other_fsobjects);
    printf("\n");
    printf("- snapshots:                    %" PRIu64 "\n",    apsb->apfs_num_snapshots);
    printf("- block allocations ever made:  %" PRIu64 "\n",    apsb->apfs_total_block_alloced);
    printf("- block liberations ever made:  %" PRIu64 "\n",    apsb->apfs_total_blocks_freed);
    printf("\n");

    printf("UUID:   ");
    print_uuid(apsb->apfs_vol_uuid);
    printf("\n");

    printf("Formatted by:\n");
    print_apfs_modified_by(&(apsb->apfs_formatted_by));
    printf("\n");

    printf("Last modified by:\n");
    print_apfs_modified_by(&(apsb->apfs_modified_by));

    /*
     * TODO: Print the following fields:
     * - apfs_root_to_xid
     * - apfs_er_state_oid
     * - apfs_cloneinfo_id_epoch
     * - apfs_cloneinfo_xid
     * - apfs_snap_meta_ext_oid
     * - apfs_volume_group_id
     * - apfs_integrity_meta_oid
     * - apfs_fext_tree_oid
     * - apfs_fext_tree_type
     */
}
