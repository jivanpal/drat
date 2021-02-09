/**
 * Functions that generate nicely formatted strings representing data found in
 * the checkpoint area. This includes instances of `nx_superblock_t`,
 * `checkpoint_map_phys_t`, and `checkpoint_mapping_t`.
 */

#include "nx.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"

/**
 * Get a human-readable string that lists the optional feature flags that are
 * set on a given container superblock.
 * 
 * nxsb:    A pointer to the container superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_nx_features_string(nx_superblock_t* nxsb) {
    // String to use if no flags are set
    char* no_flags_string = "- No feature flags are set.\n";
    size_t no_flags_string_len = strlen(no_flags_string);
    
    const int NUM_FLAGS = 2;
    
    uint64_t flag_constants[] = {
        NX_FEATURE_DEFRAG,
        NX_FEATURE_LCFD,
    };

    char* flag_strings[] = {
        "The volumes in this container support defragmentation.",
        "This container is using low-capacity Fusion Drive mode.",
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
        fprintf(stderr, "\nABORT: get_nx_features_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (nxsb->nx_features & flag_constants[i]) {
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
 * Get a human-readable string that lists the read-only-compatible feature
 * flags that are set on a given container superblock.
 * 
 * nxsb:    A pointer to the container superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_nx_readonly_compatible_features_string(nx_superblock_t* nxsb) {
    // String to use if no flags are set
    char* no_flags_string = "- No read-only compatible feature flags are set.\n";
    size_t no_flags_string_len = strlen(no_flags_string);
    
    const int NUM_FLAGS = 0;
    uint64_t flag_constants[] = {
        // empty
    };
    char* flag_strings[] = {
        // empty
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
        fprintf(stderr, "\nABORT: get_nx_readonly_compatible_features_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (nxsb->nx_readonly_compatible_features & flag_constants[i]) {
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
 * Get a human-readable string that lists the backward-incompatible feature
 * flags that are set on a given container superblock.
 * 
 * nxsb:    A pointer to the container superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_nx_incompatible_features_string(nx_superblock_t* nxsb) {
    // String to use if no flags are set    
    char* no_flags_string = "- No backward-incompatible feature flags are set.\n";
    size_t no_flags_string_len = strlen(no_flags_string);
    
    const int NUM_FLAGS = 3;
    
    uint64_t flag_constants[] = {
        NX_INCOMPAT_VERSION1,
        NX_INCOMPAT_VERSION2,
        NX_INCOMPAT_FUSION,
    };

    char* flag_strings[] = {
        "This container uses APFS version 1, as implemented in macOS 10.12.",
        "This container uses APFS version 2, as implemented in macOS 10.13 and iOS 10.3.",
        "This container supports Fusion Drives.",
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
        fprintf(stderr, "\nABORT: get_nx_incompatible_features_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (nxsb->nx_features & flag_constants[i]) {
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
 * Get a human-readable string that lists the non-feature flags that
 * are set on a given container superblock.
 * 
 * nxsb:    A pointer to the container superblock in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_nx_flags_string(nx_superblock_t* nxsb) {
    // String to use if no flags are set    
    char* no_flags_string = "- No other flags are set.\n";
    size_t no_flags_string_len = strlen(no_flags_string);
    
    const int NUM_FLAGS = 3;
    
    uint64_t flag_constants[] = {
        NX_RESERVED_1,
        NX_RESERVED_2,
        NX_CRYPTO_SW,
    };

    char* flag_strings[] = {
        "Reserved flag 1",
        "Reserved flag 2",
        "This container uses software cryptography.",
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
        fprintf(stderr, "\nABORT: get_nx_flags_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (nxsb->nx_features & flag_constants[i]) {
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
 * Print a nicely formatted string describing the data contained in a container
 * superblock, including the data in its header.
 */
void print_nx_superblock(nx_superblock_t* nxsb) {
    print_obj_phys(nxsb); // `nxsb` is equivalent to `&(nxsb->nx_o)`.

    printf("Keybag location: ");
    if (nxsb->nx_keylocker.pr_block_count == 0) {
        printf("none (spans 0 blocks)\n");
    } else {
        printf(
            "first block %#" PRIx64 ", spans %" PRIu64 " (%#" PRIx64 ") blocks (last block %#" PRIx64 ")\n",
            nxsb->nx_keylocker.pr_start_paddr,
            nxsb->nx_keylocker.pr_block_count,
            nxsb->nx_keylocker.pr_block_count,
            nxsb->nx_keylocker.pr_start_paddr + nxsb->nx_keylocker.pr_block_count - 1
        );
    }

    printf("Media keybag location: ");
    if (nxsb->nx_mkb_locker.pr_block_count == 0) {
        printf("none (spans 0 blocks)\n");
    } else {
        printf(
            "first block %#" PRIx64 ", spans %" PRIu64 " (%#" PRIx64 ") blocks (last block %#" PRIx64 ")\n",
            nxsb->nx_mkb_locker.pr_start_paddr,
            nxsb->nx_mkb_locker.pr_block_count,
            nxsb->nx_mkb_locker.pr_block_count,
            nxsb->nx_mkb_locker.pr_start_paddr + nxsb->nx_mkb_locker.pr_block_count - 1
        );
    }


    char magic_string[] = {
        (char)(nxsb->nx_magic),
        (char)(nxsb->nx_magic >> 8),
        (char)(nxsb->nx_magic >> 16),
        (char)(nxsb->nx_magic >> 24),
        '\0'
    };
    printf("Magic string:       %s\n",          magic_string);

    printf(
        "Latest version of Apple APFS software that mounted this container: "
        "%" PRIu64 ".%llu.%llu.%llu.%llu\n",

         nxsb->nx_newest_mounted_version >> 40,
        (nxsb->nx_newest_mounted_version >> 30) & ~(~0ULL << 10),
        (nxsb->nx_newest_mounted_version >> 20) & ~(~0ULL << 10),
        (nxsb->nx_newest_mounted_version >> 10) & ~(~0ULL << 10),
        (nxsb->nx_newest_mounted_version)       & ~(~0ULL << 10)
    );

    printf("Block size:         %u bytes\n",    nxsb->nx_block_size);
    printf("Block count:        %" PRIu64 " (last block %#" PRIx64 ")\n",
        nxsb->nx_block_count,
        nxsb->nx_block_count - 1
    );
    
    char* features_string = get_nx_features_string(nxsb);
    printf("Supported features:\n%s", features_string);
    free(features_string);

    char* readonly_compatible_features_string = get_nx_readonly_compatible_features_string(nxsb);
    printf("Supported read-only compatible features:\n%s", readonly_compatible_features_string);
    free(readonly_compatible_features_string);

    char* incompatible_features_string = get_nx_incompatible_features_string(nxsb);
    printf("Backward-incompatible features:\n%s", incompatible_features_string);
    free(incompatible_features_string);
    
    printf("UUID:       0x%016" PRIx64 "%016" PRIx64 "\n",
        *((uint64_t*)(nxsb->nx_uuid) + 1),
        * (uint64_t*)(nxsb->nx_uuid)
    );
    printf("Next OID:                       0x%" PRIx64 "\n",  nxsb->nx_next_oid);
    printf("Next XID:                       0x%" PRIx64 "\n",  nxsb->nx_next_xid);

    // TODO: Maybe print `xp_desc` and `xp_data` fields.

    printf("Space manager Ephemeral OID:    0x%" PRIx64 "\n",  nxsb->nx_spaceman_oid);
    printf("Object map Physical OID:        0x%" PRIx64 "\n",  nxsb->nx_omap_oid);
    printf("Reaper Ephemeral OID:           0x%" PRIx64 "\n",  nxsb->nx_reaper_oid);

    char* flags_string = get_nx_flags_string(nxsb);
    printf("Other flags:\n%s", flags_string);
    free(flags_string);
}

/**
 * Print a nicely formatted string describing the data contained in a
 * single checkpoint-mapping (that is, a single mapping within a
 * checkpoint-mapping block).
 * 
 * In particular, this function prints the object ID of the epehemeral object
 * that the checkpoint-mapping relates to, followed by a bulleted list
 * comprising the location and other details of that Ephemeral object.
 * 
 * cpm:     A pointer to the checkpoint-mapping in question.
 */
void print_checkpoint_mapping(checkpoint_mapping_t* cpm) {
    printf("Ephemeral OID:                      0x%" PRIx64 "\n",      cpm->cpm_oid);
    printf("Logical block address on disk:      0x%" PRIx64 "\n",      cpm->cpm_paddr);

    char* type_string = get_o_type_string(cpm->cpm_type);
    printf("Object type:                        %s\n",          type_string);
    free(type_string);

    char* subtype_string = get_o_subtype_string(cpm->cpm_subtype);
    printf("Object subtype:                     %s\n",          subtype_string);
    free(subtype_string);
    
    printf("Object size:                        %u bytes\n",    cpm->cpm_size);
    printf("Associated volume OID (virtual):    0x%" PRIx64 "\n",      cpm->cpm_fs_oid);
}

/**
 * Get a human-readable string that lists the flags that are set on a given
 * checkpoint-mapping block.
 * 
 * cpm:     A pointer to the checkpoint-mapping block in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_cpm_flags_string(checkpoint_map_phys_t* cpm) {
    // String to use if no flags are set    
    char* no_flags_string = "- No flags are set.\n";
    size_t no_flags_string_len = strlen(no_flags_string);
    
    const int NUM_FLAGS = 1;
    
    uint64_t flag_constants[] = {
        CHECKPOINT_MAP_LAST,
    };

    char* flag_strings[] = {
        "Last checkpoint-mapping block in the corresponding checkpoint.",
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
        fprintf(stderr, "\nABORT: get_cpm_flags_string: Could not allocate sufficient memory for `result_string`.\n");
        exit(-1);
    }

    char* cursor = result_string;

    // Go through possible flags, adding corresponding string to result if
    // that flag is set.
    for (int i = 0; i < NUM_FLAGS; i++) {
        if (cpm->cpm_flags & flag_constants[i]) {
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
 * Print a nicely formatted string describing the data contained in a
 * checkpoint-mapping block.
 * 
 * The details of each of the checkpoint-mappings that exists within the block
 * will not be printed. For that, see `print_checkpoint_map_phys_mappings()`.
 * 
 * cpm:     A pointer to the checkpoint-mapping block in question.
 */
void print_checkpoint_map_phys(checkpoint_map_phys_t* cpm) {
    print_obj_phys(cpm);    // `cpm` as the same as `&(cpm->cpm_o)`
    
    char* flags_string = get_cpm_flags_string(cpm);
    printf("Flags:\n%s",    flags_string);
    free(flags_string);

    printf("Number of mappings: %u\n",  cpm->cpm_count);
    
}

/**
 * Call `print_checkpoint_mapping()` once for each checkpoint-mapping
 * that exists in a given checkpoint-mapping block.
 */
void print_checkpoint_map_phys_mappings(checkpoint_map_phys_t* cpm) {
    checkpoint_mapping_t* cursor = cpm->cpm_map;
    checkpoint_mapping_t* end = cursor + cpm->cpm_count;

    while (cursor < end) {
        print_checkpoint_mapping(cursor);
        printf("--------------------------------------------------------------------------------\n");
        cursor++;
    }
}
