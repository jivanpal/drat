#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "apfs/nx.h"
#include "functions.h"

/**
 * Print usage info for this program.
 */
void print_usage(char* program_name) {
    printf("Usage: %s <path to APFS container>\nExample: %s /dev/disk0s2\n\n", program_name, program_name);
}

int main(int argc, char** argv) {
    printf("\n");

    // Extrapolate CLI arguments, exit if invalid
    if (argc != 3) {
        printf("Incorrect number of arguments.\n");
        print_usage(argv[0]);
        return 1;
    }
    char* nx_path = argv[1];
    paddr_t nx_block_addr = 0x0;
    char parse_success = sscanf(argv[2], "0x%llx", &nx_block_addr);
    if (!parse_success) {
        parse_success = sscanf(argv[2], "%llu", &nx_block_addr);
    }
    if (!parse_success) {
        printf("%s is not a valid block address.\n", argv[2]);
        print_usage(argv[0]);
        printf("\n");
    }
    
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

    char* block_buf = malloc(nx_block_size);
    read_blocks(block_buf, nx_block_addr, 1);
    printf("- Details for block 0x%016llx\n", nx_block_addr);
    print_obj_hdr_info(block_buf);

}
