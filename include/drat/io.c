/**
 * Functions, structures, and global variables related to I/O involving the
 * APFS container that the user specifies.
 */

#include "io.h"

#include <sys/types.h>
#include <inttypes.h>

#include <apfs/nx.h>    // for NX_DEFAULT_BLOCK_SIZE
#include <drat/globals.h>

static FILE* nx;

char* get_fopen_error_msg() {
    switch (errno) {
        case EACCES:        return "You do not have sufficient privileges to read this file.";
        case EAGAIN:        return "open(2): the specified path is the slave side of a locked pseudo-terminal device.";
        case EFAULT:        return "Segmentation fault; the specified path points outside our allocated address space.";
        case EINTR:         return "The process was interrupted.";
        case EILSEQ:        return "The specified filepath does not match the encoding rules.";
        case EINVAL:        return "The `mode` argument to `fopen()` is invalid. This error should never occur in practice, please file a bug at: <https://dratapp.com/>";
        case ELOOP:         return "Too many symlinks were encountered; perhaps you specified a looping symlink.";
        case EMFILE:        return "This process is forbidden from opening any more file descriptors.";
        case ENAMETOOLONG:  return "The specified filepath or one of its componenets is too long.";
        case ENFILE:        return "The system file table is full.";
        case ENOENT:        return "The specified file does not exist.";
        case ENOMEM:        return "Unable to allocate sufficient memory to buffer file.";
        case ENOTDIR:       return "A component of the specified filepath is not a directory.";
        case ENXIO:         return "The device associated with the specified file does not exist.";
        case EOPNOTSUPP:    return "You tried to open a socket, but your system doesn't support that.";
        case EOVERFLOW:     return "The specified file is a regular file whose size in bytes exceeds the maximum size of `off_t`, so it is too large to be handled.";
        default:            return "Unknown error.";
    }
}

char* get_fseeko_error_msg() {
    switch (errno) {
        case EBADF:     return "The container is not an open stream or cannot be seeked through.";
        case EINVAL:    return "Block address lies outside the valid range.";
        case EOVERFLOW: return "Block address would cause overflow in `fseeko()` because the corresponding byte address exceeds the maximum value of `off_t`.";
        case ESPIPE:    return "The container is a pipe or queue, and thus cannot be seeked through.";
        case EIO:       return "I/O error during read attempt.";
        case ENOMEM:    return "`fseeko()` couldn't allocate enough memory.";
        default:        return "Unknown error.";
    }
}

/**
 * Open the APFS container with path specified by the global option "container"
 * in read-only mode, and automatically detect its block size if
 * `globals.block_size` is set to 0. If opening the container is successful,
 * you can use `read_block()` to read data from it, and `close_container()` to
 * clean up associated memory when you no longer need access to it.
 * 
 * The APFS container's block size is determined by the global "block size"
 * option. In particular, if `globals.block_size` is set to 0 when this function
 * is called, the block size will be automatically determined from the value
 * stored in block 0, and this value will be stored in `globals.block_size`.
 * 
 * Informational messages are printed on `stdout`.
 * 
 * RETURN VALUE:
 *  On success, returns 0. On failure, returns a non-zero value: either an
 *  `fopen(3)` error code (positive value) if there was an error opening the
 *  container, or -1 if there was a read error in trying to determine the block
 *  size, or -2 if `globals.container_path` is not set; and prints an error
 *  message describing the particular error on `stderr`.
 */
int open_container() {
    if (!globals.container_path) {
        fprintf(stderr, "%s: ERROR: `globals.container_path` is not set.\n", __func__);
        return -2;
    }

    printf("Opening `%s` in read-only mode ... ", globals.container_path);
    nx = fopen(globals.container_path, "rb");
    if (!nx) {
        fprintf(stderr, "%s: ERROR: %s\n", __func__, get_fopen_error_msg());
        return errno;
    }
    printf("OK.\n");

    if (globals.block_size == 0) {
        printf("Determining block size ... ");
        
        globals.block_size = NX_DEFAULT_BLOCK_SIZE;
        char buffer[globals.block_size];
        nx_superblock_t* superblock = &buffer;  // Avoid needing to use `malloc()`
        if (read_block(superblock, 0)) {
            fprintf(stderr, "%s: ERROR: Couldn't read block 0 in order to determine block size.\n", __func__);
            return -1;
        }

        if (superblock->nx_block_size != 0) {
            globals.block_size = superblock->nx_block_size;
            printf("%"PRId64" bytes.\n", globals.block_size);
        } else {
            printf("field says 0 bytes, using default size of %"PRIu64" bytes instead.\n", globals.block_size);
        }
    }

    return 0;
}

int close_container() {
    return fclose(nx);
}

int end_of_container() {
    return feof(nx);
}

/**
 * Read a block from the APFS container.
 * 
 * buffer:
 *  The location where data that is read will be stored. It is the caller's
 *  responsibility to ensure that sufficient memory is allocated to read a
 *  block.
 * 
 * block_address:
 *  The block address to read.
 * 
 * RETURN VALUE:
 *  On success, returns 0. On failure, returns a non-zero value: either an
 *  `fseeko(3)` error code (positive value) if seeking failed, or -1 if reading
 *  failed; and prints an error message describing the particular error on
 *  `stderr`.
 */
int read_block(void* buffer, uintmax_t block_address) {
    if (fseeko(nx, block_address * globals.block_size, SEEK_SET) == -1) {
        fprintf(stderr, "%s: ERROR: %s\n", __func__, get_fseeko_error_msg());
        return errno;
    }

    if (fread(buffer, globals.block_size, 1, nx) != 1) {
        /*
         * An error occurred. This cannot be EOF, since we never read more than
         * 1 block, so if it were EOF, this should've been detected as `EINVAL`
         * in the earlier call to `fseeko()`.
         */
        assert(!feof(nx) && ferror(nx));
        
        fprintf(stderr, "%s: ERROR: `fread()` encountered an unknown error whilst reading from the stream.\n", __func__);
        clearerr(nx);
        return -1;
    }

    return 0;
}

/**
 * Read a given number of blocks from the APFS container.
 * 
 * buffer:
 *  The location where data that is read will be stored. It is the caller's
 *  responsibility to ensure that sufficient memory is allocated to read the
 *  desired number of blocks.
 * 
 * start_block:
 *  The block address of the first block to read.
 * 
 * num_blocks:
 *  The total number of blocks to read.
 * 
 * RETURN VALUE:
 *  The number of blocks read successfully. Thus, returns `num_blocks` on
 *  complete success, and 0 on complete failure.
 */
size_t read_blocks(void* buffer, uintmax_t start_block, size_t num_blocks) {
    size_t num_blocks_read = 0;

    while (num_blocks_read < num_blocks) {
        if (read_block(buffer, start_block)) {
            break;
        }
        num_blocks_read++;        
        buffer = (char*)buffer + globals.block_size;
        start_block++;
    }

    return num_blocks_read;
}

/**
 * Write given data to the APFS container at a given address.
 * 
 * buffer:
 *  The location of the data to be written to the container.
 * 
 * start_block:
 *  Within the container, the block address to start writing to.
 * 
 * num_blocks:
 *  The total number of blocks to write from `buffer` to the container.
 * 
 * RETURN VALUE:
 *  The number of blocks written successfully. Thus, returns `num_blocks` on
 *  complete success, and 0 on complete failure. If complete success does not
 *  occur, and error message describing the paticular error is printed on
 *  `stderr`.
 */
size_t write_blocks(void* buffer, uintmax_t start_block, size_t num_blocks) {
    if (fseeko(nx, start_block * globals.block_size, SEEK_SET) == -1) {
        fprintf(stderr, "%s: ERROR: %s\n", __func__, get_fseeko_error_msg());
        return 0;
    }

    size_t num_blocks_written = fwrite(buffer, globals.block_size, num_blocks, nx);
    if (num_blocks_written != num_blocks) {
        fprintf(stderr, "%s: ERROR: Wrote %zu blocks, then failed.\n", __func__, num_blocks_written);
    }
    return num_blocks_written;
}
