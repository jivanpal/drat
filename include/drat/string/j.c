/**
 * Functions that generate nicely formatted strings representing data found in
 * File-system-related objects, such as `apfs_superblock_t`.
 */

#include "j.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(__APPLE__) || defined(__BSD__)
    #include <sys/stat.h>
    /**
     * Resolve a missing <sys/stat.h> definition in Xcode Command Line Tools
     * for macOS Mojave. See: https://github.com/jivanpal/apfs-tools/issues/1
     **/
    #ifndef SF_DATALESS
    #define SF_DATALESS 0x40000000
    #endif
#else // ! __APPLE__
    /**
     * Define BSD flag constants for portability on non-BSD platforms. These
     * definitions are borrowed from <sys/stat.h> provided by Xcode Command
     * Line Tools for macOS Catalina.
     */
    #define UF_NODUMP       0x00000001
    #define UF_IMMUTABLE    0x00000002
    #define UF_APPEND       0x00000004
    #define UF_OPAQUE       0x00000008
    #define UF_HIDDEN       0x00008000
    #define SF_ARCHIVED     0x00010000
    #define SF_IMMUTABLE    0x00020000
    #define SF_APPEND       0x00040000
    #define SF_DATALESS     0x40000000
#endif // __APPLE__

#include <apfs/xf.h>

#include <drat/asize.h>
#include <drat/time.h>

#include <drat/string/common.h>

char* j_key_type_to_string(uint8_t j_key_type) {
    switch (j_key_type) {
        case APFS_TYPE_SNAP_METADATA:   return "Snapshot metadata";
        case APFS_TYPE_EXTENT:          return "Physical extent record";
        case APFS_TYPE_INODE:           return "Inode";
        case APFS_TYPE_XATTR:           return "Extended attribute";
        case APFS_TYPE_SIBLING_LINK:    return "Sibling link (mapping from an inode to hard links that target it)";
        case APFS_TYPE_DSTREAM_ID:      return "Data stream";
        case APFS_TYPE_CRYPTO_STATE:    return "Per-file encryption state";
        case APFS_TYPE_FILE_EXTENT:     return "Physical extent record for a file";
        case APFS_TYPE_DIR_REC:         return "Directory entry";
        case APFS_TYPE_DIR_STATS:       return "Directory information/statistics";
        case APFS_TYPE_SNAP_NAME:       return "Snapshot name";
        case APFS_TYPE_SIBLING_MAP:     return "Sibling map (mapping from hard link to inode)";
        case APFS_TYPE_INVALID:         return "Invalid type";
        default:                        return "Unknown type";
    }
}

void print_j_key(j_key_t* key) {
    printf("Virtual OID:    0x%llx\n",  key->obj_id_and_type & OBJ_ID_MASK);
    printf("Object type:    %s\n",      j_key_type_to_string(
            (key->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT
    ));
}

void print_j_inode_key(j_inode_key_t* key) {
    print_j_key(key);
}

char* j_inode_mode_to_string(apfs_mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFIFO:   return "Named pipe (FIFO / queue)";
        case S_IFCHR:   return "Character-special file";
        case S_IFDIR:   return "Directory";
        case S_IFBLK:   return "Block-special file";
        case S_IFREG:   return "Regular file";
        case S_IFLNK:   return "Symbolic link";
        case S_IFSOCK:  return "Socket";
        case S_IFWHT:   return "Whiteout";
        default:        return "(unrecognised mode)";
    }
}

char* get_j_inode_internal_flags_string(uint64_t internal_flags) {
    enum_string_mapping_t flags[] = {
        { INODE_IS_APFS_PRIVATE,            "Private flag (0x1) used by an APFS implementation --- this inode is not considered part of the file-system" },
        { INODE_MAINTAIN_DIR_STATS,         "MAINTAIN_DIR_STATS: Tracks the size of all its children" },
        { INODE_DIR_STATS_ORIGIN,           "The MAINTAIN_DIR_STATS flag is set explicitly (not due to inheritance)" },
        { INODE_PROT_CLASS_EXPLICIT,        "Protection class was explicitly set on creation" },
        { INODE_WAS_CLONED,                 "Created by cloning another inode" },
        { INODE_FLAG_UNUSED,                "Reserved/unused (0x20)" },
        { INODE_HAS_SECURITY_EA,            "Has an ACL (access control list, i.e. a security-based extended attribute)" },
        { INODE_BEING_TRUNCATED,            "Truncation was in progress, but a crash occurred" },
        { INODE_HAS_FINDER_INFO,            "Has a 'Finder info' extended field/attribute" },
        { INODE_IS_SPARSE,                  "Is sparse (i.e. has a sparse byte count extended field/attribute)" },
        { INODE_WAS_EVER_CLONED,            "Cloned at least once" },
        { INODE_ACTIVE_FILE_TRIMMED,        "Is a trimmed overprovisioning file" },
        { INODE_PINNED_TO_MAIN,             "Fusion drive: file content is pinned to main storage device" },
        { INODE_PINNED_TO_TIER2,            "Fusion drive: file content is pinned to secondary storage device" },
        { INODE_HAS_RSRC_FORK,              "Has a resource fork" },
        { INODE_NO_RSRC_FORK,               "Has no resource fork" },
        { INODE_ALLOCATION_SPILLEDOVER,     "Fusion drive: file content spilled over from preferred storage tier/device" },
        { INODE_FAST_PROMOTE,               "Scheduled for promotion from slow storage to fast storage" },
        { INODE_HAS_UNCOMPRESSED_SIZE,      "Uncompressed size is stored in the inode" },
        { INODE_IS_PURGEABLE,               "Will be deleted at the next purge" },
        { INODE_WANTS_TO_BE_PURGEABLE,      "Should become purgeable when its link count drops to 1" },
        { INODE_IS_SYNC_ROOT,               "Is the root of a sync hierarchy for `fileproviderd`" },
        { INODE_SNAPSHOT_COW_EXEMPTION,     "Exempt from copy-on-write behavior if the data is part of a snapshot" },
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), internal_flags, false);
}

char* get_j_inode_bsd_flags_string(uint32_t bsd_flags) {
    enum_string_mapping_t flags[] = {
        { UF_NODUMP,        "Do not dump the file" },
        { UF_IMMUTABLE,     "File may not be changed" },
        { UF_APPEND,        "File may only be appended to" },
        { UF_OPAQUE,        "Directory is opaque when viewd through a union stack" },
        { UF_HIDDEN,        "File/directory is not intended to be displayed to the user" },
        { SF_ARCHIVED,      "File has been archived" },
        { SF_IMMUTABLE,     "File may not be changed" },
        { SF_APPEND,        "File may only be appended to" },
        { SF_DATALESS,      "SF_DATALESSFAULT: Dataless placeholder --- see chflags(2) and getiopolicy_np(3)" },
    };

    return get_flags_enum_string(flags, ARRAY_SIZE(flags), bsd_flags, false);
}

void print_j_inode_val(j_inode_val_t* val, bool has_xfields) {
    printf("Parent ID:      0x%" PRIx64 "\n",  val->parent_id);
    printf("Private ID:     0x%" PRIx64 "\n",  val->private_id);
    printf("\n");

    char* tmp_string = NULL;
    if (val->internal_flags & INODE_HAS_UNCOMPRESSED_SIZE) {
        if (asprintf(tmp_string, "%" PRIu64 " bytes", val->uncompressed_size) < 0) {
            fprintf(stderr, "ABORT: %s:%d: call to asprintf() couldn't allocate sufficient memory", __func__, __LINE__);
            exit(-1);
        }
    } else {
        if (asprintf(tmp_string, "(unknown)") < 0) {
            fprintf(stderr, "ABORT: %s:%d: call to asprintf() couldn't allocate sufficient memory", __func__, __LINE__);
            exit(-1);
        }
    }
    printf("Uncompressed size:      %s\n", tmp_string);
    free(tmp_string);

    time_t timestamp = val->create_time / 1000000000;
    printf("Creation time:          %s",    ctime(&timestamp));
    timestamp = val->mod_time / 1000000000;
    printf("Last modification time: %s",    ctime(&timestamp));
    timestamp = val->change_time / 1000000000;
    printf("Last access time:       %s",    ctime(&timestamp));
    printf("\n");

    printf("Number of children / hard links:    %u\n", val->nchildren);
    printf("\n");
    
    printf("Owner UID:  %u\n",  val->owner);
    printf("Group GID:  %u\n",  val->group);
    printf("\n");

    printf("Mode:   %s\n",  j_inode_mode_to_string(val->mode));
    printf("\n");

    tmp_string = get_j_inode_internal_flags_string(val->internal_flags);
    printf("Internal flags:\n%s", tmp_string);
    printf("\n");
    free(tmp_string);

    tmp_string = get_j_inode_bsd_flags_string(val->bsd_flags);
    printf("BSD flags:\n%s", tmp_string);
    printf("\n");
    free(tmp_string);

    printf("Number of extended fields:          %u\n",
        has_xfields ? ((xf_blob_t*)(val->xfields))->xf_num_exts : 0
    );
    
    // TODO: Print actual details of extended fields/attributes
}

void print_j_file_extent_key(j_file_extent_key_t* key) {
    print_j_key(key);   // `key` equals `&(key->hdr)`
    printf("\n");
    printf("Extent offset within file:  %#" PRIx64 "\n", key->logical_addr);
}

void print_j_file_extent_val(j_file_extent_val_t* val) {
    // TODO: Print flags
    // TODO: Print crypto ID

    printf("Length (bytes): %llu\n",    val->len_and_flags & J_FILE_EXTENT_LEN_MASK);
    printf("Start block:    %#" PRIx64 "\n",   val->phys_block_num);
}

void print_j_drec_hashed_key(j_drec_hashed_key_t* key) {
    print_j_key(key);   // `key` equals `&(key->hdr)`
    printf("\n");

    // 10-bit value; next smallest datatype is 16 bits
    uint16_t name_len = key->name_len_and_hash & J_DREC_LEN_MASK;
    // 22-bit value; next smallest datatype is 32 bits
    uint32_t name_hash = (key->name_len_and_hash & J_DREC_HASH_MASK) >> J_DREC_HASH_SHIFT;
    // TODO: validate the hash?
    
    printf("Dentry name length:     %u UTF-8 bytes (including terminating NULL (U+0000) byte)\n",  name_len);
    printf("Dentry name hash:       0x%06x\n",          name_hash);
    printf("Dentry name:            ### %s ####\n",     key->name);
}

char* drec_val_to_type_string(j_drec_val_t* val) {
    switch (val->flags & DREC_TYPE_MASK) {
        case DT_UNKNOWN:    return "Unknown";
        case DT_FIFO:       return "Named pipe (FIFO / queue)";
        case DT_CHR:        return "Character-special file";
        case DT_DIR:        return "Directory";
        case DT_BLK:        return "Block-special file";
        case DT_REG:        return "Regular file";
        case DT_LNK:        return "Symbolic link";
        case DT_SOCK:       return "Socket";
        case DT_WHT:        return "Whiteout";
        default:            return "Unrecognised type";
    }
}

// TODO: Maybe delete function, might be unused as of v0.2
char* drec_val_to_short_type_string(j_drec_val_t* val) {
    switch (val->flags & DREC_TYPE_MASK) {
        case DT_UNKNOWN:    return "Unknown";
        case DT_FIFO:       return "FIFO---";
        case DT_CHR:        return "ChrSpcl";
        case DT_DIR:        return "Dirctry";
        case DT_BLK:        return "BlkSpcl";
        case DT_REG:        return "RegFile";
        case DT_LNK:        return "Symlink";
        case DT_SOCK:       return "Socket-";
        case DT_WHT:        return "Whteout";
        default:            return "Unrecog";
    }
}

void print_j_drec_val(j_drec_val_t* val, bool has_xfields) {
    printf("Dentry Virtual OID:     0x%" PRIx64 "\n", val->file_id);
    printf("Time added:             %s",    apfs_timestamp_to_string(val->date_added));
    printf("Dentry type:            %s\n",  drec_val_to_type_string(val));

    printf("No. extended fields:    ");
    if (!has_xfields) {
        printf("0\n");
        return;
    }
    printf("%u\n", ((xf_blob_t*)(val->xfields))->xf_num_exts);
    
    // TODO: Print actual details of extended fields/attributes
}

void print_j_dir_stats_val(j_dir_stats_val_t* val) {
    printf(
        "Number of children:                %"PRIu64" items\n"
        "Total size:                        %"PRIu64" bytes\n"
        "Chained key (parent dir's FSOID):  %#"PRIx64"\n"
        "Generation count:                  %"PRIu64" (%#"PRIx64")\n",

        val->num_children,
        val->total_size,
        val->chained_key,
        val->gen_count, val->gen_count
    );
}
