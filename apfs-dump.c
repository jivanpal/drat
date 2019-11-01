#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>

#include "nx.h"

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
 * Read a block from the APFS container.
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
size_t nx_read(void* restrict buffer, long start_block, size_t num_blocks) {
    if (fseek(nx, start_block * nx_block_size, SEEK_SET) == -1) {
        // An error occurred.
        printf("FAILED: nx_read: ");
        switch (errno) {
            case EBADF:
                printf("The file `%s` cannot be seeked through.\n", nx_path);
                break;
            case EINVAL:
                printf("The specified starting block address, 0x%x, is invalid, as it lies outside of the file `%s`.\n", start_block, nx_path);
                break;
            case EOVERFLOW:
                printf("The specified starting block address, 0x%x, exceeds %d bits in length, which would result in an overflow.\n", start_block, 8 * sizeof(long));
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
        assert(("ABORT: nx_read: Unexpected beaviour --- `fread` read fewer blocks than expected, but `feof` did not report EOF and `ferror` did not detect an error.", ferror(nx) || feof(nx)));
        
        if (ferror(nx)) {
            printf("FAILED: nx_read: An unknown error occurred whilst reading from the stream.\n");
            return -1;
        }

        assert(feof(nx));
        printf("nx_read: Reached end-of-file after reading %d blocks.\n", num_blocks_read);
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
 * Determine whether a given APFS block has a valid checksum.
 */
char cksum_valid(uint32_t* block) {
    int num_words = nx_block_size / 4;  // Using 32-bit words.
    uint32_t modulus = ~0;  // all ones; = 2^32 - 1

    // These are 32-bit values, but we need at least 33 bits of memory for each
    // of these in order to compute them, due to potential overflow. We use the
    // next best thing: 64 bits.
    uint64_t simple_sum = 0;
    uint64_t second_sum = 0;

    // NOTE: `i = 2` is used here since we use 0x0 as the checksum value
    // when calculating the checksum, so we need to skip 64 bits of data.
    for (int i = 2; i < num_words; i++) {
        simple_sum = (simple_sum + (uint64_t)block[i])    % modulus;
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
     * is the residue of  `- (simple_sum + c1)` modulo `modulus`. However, it
     * turns out that this `c2` value always just equals `second_sum` itself,
     * so there is no need to compute `c2`.
     * 
     * Here, we compute `c1` and just store it in the variable `simple_sum`.
     */
    simple_sum = modulus - ((simple_sum + second_sum) % modulus);

    uint64_t computed_cksum = (second_sum << 32) | simple_sum;
    
    // This is the checksum read from the block header.
    uint64_t stored_cksum = ((uint64_t)block[1] << 32) | (uint64_t)block[0];
}

int main(int argc, char** argv) {
    // Extrapolate CLI arguments, exit if invalid
    if (argc != 2) {
        printf("\nIncorrect number of arguments.\n");
        print_usage(argv[0]);
        return 1;
    }
    nx_path = argv[1];
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at %s ...\n", nx_path);
    nx = fopen(nx_path, "rb");

    // Error handling and reporting if file cannot be opened
    if (!nx) {
        printf("ABORT: main: ");
        switch (errno) {
            case ENOMEM:
                printf("Unable to allocate sufficient memory to buffer file.\n");
                break;
            case EACCES:
                printf("You do not have sufficient privileges to read this file.\n");
                break;
            case EFAULT:
                printf("Segmentation fault.\n");
                break;
            case EINTR:
                printf("The process was interrupted.\n");
                break;
            case ELOOP:
                printf("Too many symlinks were encountered; perhaps you specified a looping symlink.\n");
                break;
            case EMFILE:
                printf("This process is forbidden from opening any more file descriptors.\n");
                break;
            case ENAMETOOLONG:
                printf("The specified filepath or one of its componenets is too long.\n");
                break;
            case ENOENT:
                printf("The specified file does not exist.\n");
                break;
            case ENOTDIR:
                printf("A component of the specified filepath is not a directory.\n");
                break;
            case ENXIO:
                printf("The device associated with the specified file does not exist.\n");
                break;
            case EOVERFLOW:
                printf("The specified file is a regular file, but its size exceeds %d bytes, so is too large to be handled.\n", sizeof(off_t));
                break;
            case EILSEQ:
                printf("The specified filepath does not match the encoding rules.\n");
                break;
            default:
                printf("Unknown error.\n");
                break;
        }
        return errno;
    }

    // Is this actually an APFS container? Attempt to mount it.
    char* block_buf = malloc(nx_block_size);
    nx_read(block_buf, 0x0, 1);

    nx_superblock_t* nxsb = (void*)block_buf;

    if (!cksum_valid((void*)block_buf)) {
        printf("END: Checksum of block 0x0 is invalid.\n");
        return 0;
    }
    printf("Checksum of block 0x0 is valid.\n");
}
