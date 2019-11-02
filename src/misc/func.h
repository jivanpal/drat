#include <stdio.h>
#include <sys/errno.h>

#define MALLOC_INCREMENT     64

char*   nx_path;
FILE*   nx;
size_t  nx_block_size = 4096;

#include "../apfs/func/cksum.h"
#include "../apfs/func/boolean.h"
#include "../apfs/func/string.h"

void report_fopen_error() {
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
            fprintf(stderr, "Unknown error.\n");
            break;
    }
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
