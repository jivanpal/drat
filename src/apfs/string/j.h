/**
 * Functions that generate nicely formatted strings representing data found in
 * File-system-related objects, such as `apfs_superblock_t`.
 */

#ifndef APFS_STRING_J_H
#define APFS_STRING_J_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>

#include "../struct/j.h"
#include "../struct/xf.h"

char* j_key_type_to_string(uint8_t j_key_type) {
    switch (j_key_type) {
        case APFS_TYPE_SNAP_METADATA:
            return "Snapshot metadata";
        case APFS_TYPE_EXTENT:
            return "Physical extent record";
        case APFS_TYPE_INODE:
            return "Inode";
        case APFS_TYPE_XATTR:
            return "Extended attribute";
        case APFS_TYPE_SIBLING_LINK:
            return "Sibling link (mapping from an inode to hard links that target it)";
        case APFS_TYPE_DSTREAM_ID:
            return "Data stream";
        case APFS_TYPE_CRYPTO_STATE:
            return "Per-file encryption state";
        case APFS_TYPE_FILE_EXTENT:
            return "Physical extent record for a file";
        case APFS_TYPE_DIR_REC:
            return "Directory entry";
        case APFS_TYPE_DIR_STATS:
            return "Directory information/statistics";
        case APFS_TYPE_SNAP_NAME:
            return "Snapshot name";
        case APFS_TYPE_SIBLING_MAP:
            return "Sibling map (mapping from hard link to inode)";
        case APFS_TYPE_INVALID:
            return "Invalid type";
        default:
            return "Unknown type";
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

char* j_inode_mode_to_string(mode_t mode) {
    switch (mode & S_IFMT) {
        case S_IFIFO:
            return "Named pipe (FIFO / queue)";
        case S_IFCHR:
            return "Character-special file";
        case S_IFDIR:
            return "Directory";
        case S_IFBLK:
            return "Block-special file";
        case S_IFREG:
            return "Regular file";
        case S_IFLNK:
            return "Symbolic link";
        case S_IFSOCK:
            return "Socket";
        case S_IFWHT:
            return "Whiteout";
        default:
            return "(unrecognised mode)";
    }
}

char* get_j_inode_internal_flags_string(uint64_t internal_flags) {
    // String to use if no flags are set    
    char* default_string = "- No internal flags are set.\n";
    size_t default_string_len = strlen(default_string);
    
    const int NUM_FLAGS = 17;
    
    uint64_t flag_constants[] = {
        INODE_IS_APFS_PRIVATE,
        INODE_MAINTAIN_DIR_STATS,
        INODE_DIR_STATS_ORIGIN,
        INODE_PROT_CLASS_EXPLICIT,
        INODE_WAS_CLONED,
        INODE_FLAG_UNUSED,
        INODE_HAS_SECURITY_EA,
        INODE_BEING_TRUNCATED,
        INODE_HAS_FINDER_INFO,
        INODE_IS_SPARSE,
        INODE_WAS_EVER_CLONED,
        INODE_ACTIVE_FILE_TRIMMED,
        INODE_PINNED_TO_MAIN,
        INODE_PINNED_TO_TIER2,
        INODE_HAS_RSRC_FORK,
        INODE_NO_RSRC_FORK,
        INODE_ALLOCATION_SPILLEDOVER,
    };

    char* flag_strings[] = {
        "Private flag (0x1) used by an APFS implementation --- this inode is not considered part of the file-system",
        "MAINTAIN_DIR_STATS: Tracks the size of all its children",
        "The MAINTAIN_DIR_STATS flag is set explicitly (not due to inheritance)",
        "Protection class was explicitly set on creation",
        "Created by cloning another inode",
        "Reserved/unused (0x20)",
        "Has an ACL (access control list, i.e. a security-based extended attribute)",
        "Truncation was in progress, but a crash occurred",
        "Has a 'Finder info' extended field/attribute",
        "Is sparse (i.e. has a sparse byte count extended field/attribute)",
        "Cloned at least once",
        "Is a trimmed overprovisioning file",
        "Fusion drive: file content is pinned to main storage device",
        "Fusion drive: file content is pinned to secondary storage device",
        "Has a resource fork",
        "Has no resource fork",
        "Fusion drive: file content spilled over from preferred storage tier/device",
    };

    // Allocate sufficient memory for the result string
    size_t max_mem_required = 0;
    for (int i = 0; i < NUM_FLAGS; i++) {
        max_mem_required += strlen(flag_strings[i]) + 3;
        // `+ 3` accounts for prepending "- " and appending "\n" to each string
    }
    if (max_mem_required < default_string_len) {
        max_mem_required = default_string_len;
    }
    max_mem_required++; // Account for terminating NULL byte

    char* result_string = malloc(max_mem_required);
    if (!result_string) {
        fprintf(stderr, "\nABORT: get_j_inode_internal_flags_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (internal_flags & flag_constants[i]) {
            *cursor++ = '-';
            *cursor++ = ' ';
            memcpy(cursor, flag_strings[i], strlen(flag_strings[i]));
            cursor += strlen(flag_strings[i]);
            *cursor++ = '\n';
        }
    }

    if (cursor == result_string) {
        // No strings were added, so it must be that no flags are set.
        memcpy(cursor, default_string, default_string_len);
        cursor += default_string_len;
    }

    *cursor = '\0';

    // Free up excess allocated memory.
    result_string = realloc(result_string, strlen(result_string) + 1);
    return result_string;
}

char* get_j_inode_bsd_flags_string(uint32_t bsd_flags) {
    // String to use if no flags are set    
    char* no_flags_string = "- No internal flags are set.\n";
    size_t no_flags_string_len = strlen(no_flags_string);
    
    const int NUM_FLAGS = 9;
    
    uint64_t flag_constants[] = {
        UF_NODUMP,
        UF_IMMUTABLE,
        UF_APPEND,
        UF_OPAQUE,
        UF_HIDDEN,
        SF_ARCHIVED,
        SF_IMMUTABLE,
        SF_APPEND,
        SF_DATALESS,
    };

    char* flag_strings[] = {
        "Do not dump the file",
        "File may not be changed",
        "File may only be appended to",
        "Directory is opaque when viewd through a union stack",
        "File/directory is not intended to be displayed to the user",
        "File has been archived",
        "File may not be changed",
        "File may only be appended to",
        "SF_DATALESSFAULT: Dataless placeholder --- see chflags(2) and getiopolicy_np(3)"
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
        fprintf(stderr, "\nABORT: get_j_inode_bsd_flags_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (bsd_flags & flag_constants[i]) {
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

void print_j_inode_val(j_inode_val_t* val, bool has_xfields) {
    printf("Parent ID:      0x%llx\n",  val->parent_id);
    printf("Private ID:     0x%llx\n",  val->private_id);
    printf("\n");

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

    char* tmp_string = get_j_inode_internal_flags_string(val->internal_flags);
    printf("Internal flags:\n%s", tmp_string);
    printf("\n");
    free(tmp_string);

    tmp_string = get_j_inode_bsd_flags_string(val->bsd_flags);
    printf("BSD flags:\n%s", tmp_string);
    printf("\n");
    free(tmp_string);

    printf("No. extended fields:    ");
    if (!has_xfields) {
        printf("0\n");
        return;
    }
    printf("%u\n", ((xf_blob_t*)(val->xfields))->xf_num_exts);
    
    // TODO: Print actual details of extended fields/attributes
}

void print_j_drec_hashed_key(j_drec_hashed_key_t* key) {
    print_j_key(key);   // `key` equals `&(key->hdr)`
    printf("\n");

    // 10-bit value; next smallest datatype is 16 bits
    uint16_t name_len = key->name_len_and_hash & J_DREC_LEN_MASK;
    // 22-bit value; next smallest datatype is 32 bits
    uint32_t name_hash = (key->name_len_and_hash & J_DREC_HASH_MASK) >> J_DREC_HASH_SHIFT;
    // TODO: validate the hash?
    
    printf("Directory name length:  %u UTF-8 bytes (including terminating NULL (U+0000) byte)\n",  name_len);
    printf("Directory name hash:    0x%06x\n",          name_hash);
    printf("Directory name:         ### %s ####\n",     key->name);
}

char* drec_val_to_type_string(j_drec_val_t* val) {
    switch (val->flags & DREC_TYPE_MASK) {
        case DT_UNKNOWN:
            return "Unknown";
        case DT_FIFO:
            return "Named pipe (FIFO / queue)";
        case DT_CHR:
            return "Character-special file";
        case DT_DIR:
            return "Directory";
        case DT_BLK:
            return "Block-special file";
        case DT_REG:
            return "Regular file";
        case DT_LNK:
            return "Symbolic link";
        case DT_SOCK:
            return "Socket";
        case DT_WHT:
            return "Whiteout";
        default:
            return "Unrecognised type";
    }
}

void print_j_drec_val(j_drec_val_t* val, bool has_xfields) {
    printf("Diretcory Virtual OID:  0x%llx\n", val->file_id);

    // timestamp converted from nanoseconds since
    // Unix epoch to seconds since Unix epoch.
    // Newline '\n' is added by result of `ctime()`.
    time_t timestamp = val->date_added / 1000000000;
    printf("Time added:             %s",    ctime(&timestamp));

    printf("Dentry type:            %s\n",  drec_val_to_type_string(val));

    printf("No. extended fields:    ");
    if (!has_xfields) {
        printf("0\n");
        return;
    }
    printf("%u\n", ((xf_blob_t*)(val->xfields))->xf_num_exts);
    
    // TODO: Print actual details of extended fields/attributes
}

#endif // APFS_STRING_J_H
