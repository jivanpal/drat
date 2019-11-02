#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "nx.h"

#define MALLOC_INCREMENT     64

char*   nx_path;
FILE*   nx;
size_t  nx_block_size = 4096;

/**
 * Print usage info for this program.
 */
void print_usage(char* program_name) {
    printf("Usage: %s <path to APFS container>\nExample: %s /dev/disk0s2\n\n", program_name, program_name);
}

/**
 * Read given number of blocks from the APFS container.
 * 
 * - buffer:        The location where data that is read will be stored. It is
 *      the caller's responsibility to ensure that sufficient memory is
 *      allocated to read the desired number of blocks.
 * - start_block:   APFS physical block address to start reading from.
 * - num_blocks:    The number of APFS physical blocks to read into `buffer`.
 * 
 * RETURN VALUE:    On success or partial success, the number of blocks read
 *              (a non-negative value). On failure, a negative value.
 */
size_t read_blocks(void* restrict buffer, long start_block, size_t num_blocks) {
    if (fseek(nx, start_block * nx_block_size, SEEK_SET) == -1) {
        // An error occurred.
        printf("FAILED: read_blocks: ");
        switch (errno) {
            case EBADF:
                printf("The file `%s` cannot be seeked through.\n", nx_path);
                break;
            case EINVAL:
                printf("The specified starting block address, 0x%lx, is invalid, as it lies outside of the file `%s`.\n", start_block, nx_path);
                break;
            case EOVERFLOW:
                printf("The specified starting block address, 0x%lx, exceeds %lu bits in length, which would result in an overflow.\n", start_block, 8 * sizeof(long));
                break;
            case ESPIPE:
                printf("The data stream associated with the file `%s` is a pipe or FIFO, and thus cannot be seeked through.\n", nx_path);
                break;
            default:
                printf("Unknown error.\n");
                break;
        }
        return -1;
    }

    size_t num_blocks_read = fread(buffer, nx_block_size, num_blocks, nx);
    if (num_blocks_read != num_blocks) {
        // An error occured or we reached end-of-file.
        
        // NOTE: This is intended to act as an `assert` statement with a message
        if (!ferror(nx) && !feof(nx)) {
            fprintf(stderr, "ABORT: read_blocks: Unexpected beaviour --- `fread` read fewer blocks than expected, but `feof` did not report EOF and `ferror` did not detect an error.\n\n");
            exit(-1);
        }
        
        if (ferror(nx)) {
            printf("FAILED: read_blocks: An unknown error occurred whilst reading from the stream.\n");
            return -1;
        }

        assert(feof(nx));
        printf("read_blocks: Reached end-of-file after reading %lu blocks.\n", num_blocks_read);
    }
    return num_blocks_read;
}

/**
 * Get a string representation of a 32-bit magic number, e.g. for the value
 * 'BSXN' stored in the `nx_magic` field of an instance of `nx_superblock_t`,
 * get the string "NXSB".
 * 
 * RETURN VALUE:    A pointer to a C-string with the string representation of
 *      `magic`. It is the caller's responsibility to free this pointer with
 *      `free()` when it is no longer needed.
 */
char* magic_to_string(uint32_t magic) {
    char* string = malloc(5);

    for (int i = 0; i < 4; i++) {
        string[i] = (char)(magic >> (8 * i));
    }
    string[4] = '\0';

    return string;
}

/**
 * Compute or validate the checksum of a given APFS block. This is a helper
 * function for `compute_block_cksum()` and `is_cksum_valid()`.
 * 
 * block:       A pointer to the raw APFS block data. This pointer should point
 *          to at least `nx_block_size` bytes (typically 4096 bytes) of data.
 * compute:     Boolean value. If non-zero, then compute the checksum of the
 *          block, treating the first 64 bits of the block (where the checksum
 *          is stored) as zero. If zero, then validate checksum against the
 *          checksum stored in the block header.
 * 
 * RETURN VALUE:    If `compute` is true, return the computed checksum.
 *              If `compute` is false, return zero if the checksum validates
 *              successfully, and non-zero if it fails to do so.
 */
uint64_t fletcher_cksum(uint32_t* block, char compute) {
    int num_words = nx_block_size / 4;  // Using 32-bit words.
    uint32_t modulus = ~0;  // all ones; = 2^32 - 1

    // These are 32-bit values, but we need at least 33 bits of memory for each
    // of these in order to compute them, as values can overflow during
    // summation before the modulus is taken.
    // We use the next best thing: 64 bits.
    uint64_t simple_sum = 0;
    uint64_t second_sum = 0;

    // NOTE: When computing the checksum, `i = 2` is used here since we treat
    // the first 64 bits of the block as zero. When validating the checksum, we
    // compute the traditional Fletcher-64 checksum of the entire block.
    for (int i = (compute ? 2 : 0); i < num_words; i++) {
        simple_sum = (simple_sum + block[i])    % modulus;
        second_sum = (second_sum + simple_sum)  % modulus;
    }

    /**
     * APFS uses a variant of the traditional Flecther-64 checksum.
     * See: https://blog.cugu.eu/post/apfs/#checksum
     * 
     * In particular:
     * 
     * - Instead of `simple_sum`, APFS uses `c1`, which (in number theory lingo)
     * is the remainder of `- (simple_sum + second_sum)` modulo `modulus`.
     * 
     * - Instead of `second_sum`, APFS uses `c2`, which (in number theory lingo)
     * is the remainder of  `- (simple_sum + c1)` modulo `modulus`. However, it
     * turns out that this `c2` value always just equals `second_sum` itself,
     * so there is no need to compute `c2`.
     * 
     * This variant of Fletcher-64 means that, when validating against the
     * stored checksum, we just compute the traditional Fletcher-64 checksum of
     * the whole block (including the stored checksum) and the computed value
     * should be zero if the stored checksum is correct.
     * 
     * Here, if we are computing the checksum rather than validating the block
     * against the stored checksum, we compute `c1` and just store it in the
     * variable `simple_sum`.
     */
    // if (compute) {
        simple_sum = modulus - ((simple_sum + second_sum) % modulus);
    // }

    return (second_sum << 32) | simple_sum;
}

/**
 * Get/compute the checksum of a given APFS block, treating the first 64 bits
 * of the block (the location where the checksum is usually stored) as zero.
 * 
 * block:   A pointer to the raw APFS block data. This pointer should point to
 *      at least `nx_block_size` bytes (typically 4096 bytes) of data.
 * 
 * RETURN VALUE:    The computed checksum.
 */
uint64_t compute_block_cksum(uint32_t* block) {
    return fletcher_cksum(block, 1);
}

/**
 * Determine whether a given APFS block has a valid checksum.
 */
char is_cksum_valid(uint32_t* block) {
    // TODO: The "simple" implementation doesn't appear to work.
    // return fletcher_cksum(block, 0) == 0;

    // The following gives the correct result.
    return compute_block_cksum(block) == *(uint64_t*)block;
}

/**
 * Get a human-readable string with a given object's storage type.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. This pointer needn't be
 *      freed by the caller.
 */
char* get_obj_storage_type_string(obj_phys_t* obj) {
    switch (obj->o_type & OBJ_STORAGETYPE_MASK) {
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
 * whether the object is physical, virtual, or ephemeral.
 * 
 * obj:     A pointer to the object in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_obj_type_flags_string(obj_phys_t* obj) {
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
    }
    char* result_string = malloc(max_mem_required);
    
    char* cursor = result_string;

    // Go through possible flags. If a flag is set, append its corresponding
    // string to `result_string`. The flags are comma-delimited.
    for (int i = 0; i < 3; i++) {
        if (obj->o_type & flag_constants[i]) {
            if (cursor != result_string) {
                *cursor++ = ',';
                *cursor++ = ' ';
            }
            memcpy(cursor, flag_strings[i], strlen(flag_strings[i]));
            cursor += strlen(flag_strings[i]);
        }
    }

    if (cursor == result_string) {
        // No strings were added, so it must be that no flags were set.
        // As such, return "(none)".
        *cursor++ = '(';
        *cursor++ = 'n';
        *cursor++ = 'o';
        *cursor++ = 'n';
        *cursor++ = 'e';
        *cursor++ = ')';
    }

    *cursor = '\0';
    
    // Free up excess allocated memory.
    result_string = realloc(result_string, strlen(result_string) + 1);

    return result_string;
}

/**
 * Get a human-readable string describing a given `o_type` value. This is a
 * helper function for `get_obj_type_string()` and `get_obj_subtype_string()`.
 */
char* o_type_to_string(uint32_t o_type) {
    switch (o_type & OBJECT_TYPE_MASK) {
        case OBJECT_TYPE_NX_SUPERBLOCK:
            return "Container superblock";
        case OBJECT_TYPE_BTREE:
            return "B-tree (root node)";
        case OBJECT_TYPE_BTREE_NODE:
            return "B-tree (non-root) node";
        case OBJECT_TYPE_SPACEMAN:
            return "Space manager";
        case OBJECT_TYPE_SPACEMAN_CAB:
            return "Space manager chunk-info address block";
        case OBJECT_TYPE_SPACEMAN_CIB:
            return "Space manager chunk-info block";
        case OBJECT_TYPE_SPACEMAN_BITMAP:
            return "Space manager free-space bitmap";
        case OBJECT_TYPE_OMAP:
            return "Object map";
        case OBJECT_TYPE_CHECKPOINT_MAP:
            return "Checkpoint map";
        case OBJECT_TYPE_FS:
            return "APFS volume";
        case OBJECT_TYPE_NX_REAPER:
            return "Container reaper";
        case OBJECT_TYPE_NX_REAP_LIST:
            return "Container reaper list";
        case OBJECT_TYPE_EFI_JUMPSTART:
            return "EFI jumpstart boot info";
        case OBJECT_TYPE_NX_FUSION_WBC:
            return "Fusion device write-back cache state";
        case OBJECT_TYPE_NX_FUSION_WBC_LIST:
            return "Fusion device write-back cache list";
        case OBJECT_TYPE_ER_STATE:
            return "Encryption-rolling state";
        case OBJECT_TYPE_GBITMAP:
            return "General-purpose bitmap";
        case OBJECT_TYPE_GBITMAP_BLOCK:
            return "General purpose bitmap block";
        case OBJECT_TYPE_INVALID:
            return "Invalid type";
        case OBJECT_TYPE_TEST:
            return "A type reserved for testing (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)";
        case OBJECT_TYPE_CONTAINER_KEYBAG:
            return "Container keybag";
        case OBJECT_TYPE_VOLUME_KEYBAG:
            return "Volume keybag";
        default:
            return NULL;
    }
}

/**
 * Get a human-readable string with a given object's type.
 * 
 * obj:     A pointer to the object in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string; or `NULL` if the type
 *      is unrecognised. The caller needn't free this pointer.
 */
char* get_obj_type_string(obj_phys_t* obj) {
    return o_type_to_string(obj->o_type);
}

/**
 * Get a human-readable string with an object's subtype.
 * 
 * obj:     A pointer to the object in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string; or `NULL` if the subtype
 *      is unrecognised. This pointer needn't be freed by the caller.
 */
char* get_obj_subtype_string(obj_phys_t* obj) {
    // Type values are also used as subtype values; see if the subtype is one
    // of the valid regular types first.
    char* type_string = o_type_to_string(obj->o_subtype);

    if (type_string) {
        return type_string;
    }

    // If not, then go through subtype-exclusive cases.
    switch (obj->o_subtype & OBJECT_TYPE_MASK) {
        case OBJECT_TYPE_SPACEMAN_FREE_QUEUE:
            return "Space manager free-space queue";
        case OBJECT_TYPE_EXTENT_LIST_TREE:
            return "Extents-list tree";
        case OBJECT_TYPE_FSTREE:
            return "File-system records tree";
        case OBJECT_TYPE_BLOCKREFTREE:
            return "Extent references tree";
        case OBJECT_TYPE_SNAPMETATREE:
            return "Volume snapshot metadata tree";
        case OBJECT_TYPE_OMAP_SNAPSHOT:
            return "Object map snapshots tree";
        case OBJECT_TYPE_FUSION_MIDDLE_TREE:
            return "Fusion inter-drive block-mapping tree";
        case OBJECT_TYPE_GBITMAP_TREE:
            return "B-tree of general-purpose bitmaps";
        default:
            return NULL;
    }
}

/**
 * Print the information stored in an APFS object's
 * header in a human-readable form.
 */
void print_obj_hdr_info(obj_phys_t* obj) {
    char* type_flags_string = get_obj_type_flags_string(obj);
    
    char* type_string = get_obj_type_string(obj);
    if (!type_string) {
        char* format_string = "Unknown type (0x%08x) --- perhaps this type was introduced in a later version of APFS than that published on 2019-02-27.";
        size_t type_string_size = strlen(format_string) + 4; // Add 4 to account for `%08x` being replaced with a 16-nibble hex number, and necessary terminating null byte.
        
        type_string = malloc(type_string_size);
        if (!type_string) {
            fprintf(stderr, "ABORT: print_obj_hdr_info: Could not allocate sufficient memory when generating `type_string` in the case where the object type is not recognised.\n");
            exit(-1);
        }

        sprintf(type_string, format_string, obj->o_type & OBJECT_TYPE_MASK);
    }

    char* subtype_string = get_obj_subtype_string(obj);
    if (!subtype_string) {
        char* format_string = "Unknown subtype (0x%08x) --- perhaps this subtype was introduced in a later version of APFS than that published on 2019-02-27.";
        size_t subtype_string_size = strlen(format_string) + 4; // Add 4 to account for `%08x` being replaced with a 16-nibble hex number, and necessary terminating null byte.
        
        subtype_string = malloc(subtype_string_size);
        if (!subtype_string) {
            fprintf(stderr, "ABORT: print_obj_hdr_info: Could not allocate sufficient memory when generating `subtype_string` in the case where the object subtype is not recognised.\n");
            exit(-1);
        }

        sprintf(subtype_string, format_string, obj->o_subtype);
    }

    // Print the info
    printf("Stored checksum:        0x%016llx\n",   *(uint64_t*)obj);
    printf("OID (object ID):        0x%016llx\n",   obj->o_oid);
    printf("XID (transaction ID):   0x%016llx\n",   obj->o_xid);
    printf("Storage type:           %s\n",          get_obj_storage_type_string(obj));
    printf("Type flags:             %s\n",          type_flags_string);
    printf("Type:                   %s\n",          type_string);
    printf("Subtype:                %s\n",          subtype_string);

    free(type_flags_string);
}

/**
 * Determine whether a given APFS block is a container superblock.
 */
char is_nx_superblock(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_NX_SUPERBLOCK;
}

char is_checkpoint_map_phys(obj_phys_t* obj) {
    return (obj->o_type & OBJECT_TYPE_MASK) == OBJECT_TYPE_CHECKPOINT_MAP;
}

int main(int argc, char** argv) {
    printf("\n");

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 2) {
        printf("Incorrect number of arguments.\n");
        print_usage(argv[0]);
        return 1;
    }
    nx_path = argv[1];
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at `%s`.\n", nx_path);
    nx = fopen(nx_path, "rb");

    // Error handling and reporting if file cannot be opened
    if (!nx) {
        fprintf(stderr, "ABORT: main: ");
        switch (errno) {
            case ENOMEM:
                fprintf(stderr, "Unable to allocate sufficient memory to buffer file.\n");
                break;
            case EACCES:
                fprintf(stderr, "You do not have sufficient privileges to read this file.\n");
                break;
            case EFAULT:
                fprintf(stderr, "Segmentation fault.\n");
                break;
            case EINTR:
                fprintf(stderr, "The process was interrupted.\n");
                break;
            case ELOOP:
                fprintf(stderr, "Too many symlinks were encountered; perhaps you specified a looping symlink.\n");
                break;
            case EMFILE:
                fprintf(stderr, "This process is forbidden from opening any more file descriptors.\n");
                break;
            case ENAMETOOLONG:
                fprintf(stderr, "The specified filepath or one of its componenets is too long.\n");
                break;
            case ENOENT:
                fprintf(stderr, "The specified file does not exist.\n");
                break;
            case ENOTDIR:
                fprintf(stderr, "A component of the specified filepath is not a directory.\n");
                break;
            case ENXIO:
                fprintf(stderr, "The device associated with the specified file does not exist.\n");
                break;
            case EOVERFLOW:
                fprintf(stderr, "The specified file is a regular file, but its size exceeds %lu bytes, so is too large to be handled.\n", sizeof(off_t));
                break;
            case EILSEQ:
                fprintf(stderr, "The specified filepath does not match the encoding rules.\n");
                break;
            default:
                printf("Unknown error.\n");
                break;
        }
        return -errno;
    }
    printf("Opened file successfully.\n\n");

    // Is this actually an APFS container? Attempt to mount it.
    printf("Simulating a mount of the APFS container.\n");
    char* block_buf = malloc(nx_block_size);
    if (!block_buf) {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory to read block.\n");
        return -1;
    }

    printf("\nReading block 0x0 into buffer.\n");
    read_blocks(block_buf, 0x0, 1);

    printf("- Details of block 0x0:\n");
    print_obj_hdr_info(block_buf);
    printf("\n");

    if (!is_cksum_valid(block_buf)) {
        printf("END: Checksum of block 0x0 is invalid.\n\n");
        return 0;
    }
    printf("Checksum of block 0x0 is valid.\n");

    if (!is_nx_superblock(block_buf)) {
        printf("END: Block 0x0 is not a container superblock.\n\n");
        return 0;
    }
    printf("Block 0x0 is a container superblock.\n");

    nx_superblock_t* nxsb = malloc(sizeof(nx_superblock_t));
    if (!nxsb) {
        fprintf(stderr, "ABORT: main: Could not allocate sufficient memory for `nxsb`.\n");
        return -1;
    }
    memcpy(nxsb, block_buf, sizeof(*nxsb));

    printf("\nLocating the checkpoint descriptor area.\n");
    if (nxsb->nx_xp_desc_blocks >> 31 == 0) {
        printf("The area is contiguous.\n");
        printf("The address of its first block is 0x%016llx.\n", nxsb->nx_xp_desc_base);

        printf("Allocating memory for array of superblocks found in the checkpoint descriptor area.\n");
        size_t num_alloced = MALLOC_INCREMENT;
        nx_superblock_t* nx_blocks = malloc(num_alloced * sizeof(nx_superblock_t));
        if (!nx_blocks) {
            fprintf(stderr, "ABORT: main: Could not allocate sufficient memory for %lu instances of `nx_superblock_t` in `xp_desc_nx_blocks`.\n", num_alloced);
            return -1;
        }

        printf("Reading the blocks into memory.\n\n");
        paddr_t current_block_addr = nxsb->nx_xp_desc_base;
        size_t num_read = 0;

        while (1) {
            read_blocks(block_buf, current_block_addr, 1);

            printf("Read block 0x%llx into buffer.\n- Details of block 0x%llx:\n", current_block_addr, current_block_addr);
            print_obj_hdr_info(block_buf);
            printf("\n");

            if (!is_nx_superblock(block_buf) && !is_checkpoint_map_phys(block_buf)) {
                // We have reached the end of the checkpoint descriptor area.
                break;
            }
            
            if (is_nx_superblock(block_buf)) {
                if (num_read >= num_alloced) {
                    // Need more memory
                    num_alloced += MALLOC_INCREMENT;
                    printf("Allocating more memory for array ... ");
                    nx_blocks = realloc(nx_blocks, num_alloced * sizeof(nx_superblock_t));
                    if (!nx_blocks) {
                        fprintf(stderr, "\nABORT: main: Could not allocate sufficient memory for %lu instances of `nx_superblock_t` in `xp_desc_nx_blocks`.\n", num_alloced);
                        return -1;
                    }
                    printf("done.\n");
                }

                // Copy the buffered NXSB into the array
                memcpy(nx_blocks + num_read, block_buf, sizeof(nx_superblock_t));
                num_read++;
            }

            current_block_addr++;
        }
        // De-allocate excess memory.
        nx_blocks = realloc(nx_blocks, num_read * sizeof(nx_superblock_t));

        printf("Reached the end of the checkpoint descriptor area.\n");
        printf("Found %lu container superblocks and read them into memory.\n", num_read);

    } else {
        printf("The area is not contiguous.\n");
        printf("The OID of the B-tree representing the area if 0x%016llx.\n", nxsb->nx_xp_desc_base);
        printf("END: The ability to handle this case has not yet been implemented.\n\n");   // TODO: implement case when xp_desc area is not contiguous
    }
}
