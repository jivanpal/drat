#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "apfs/struct/nx.h"
#include "misc/func.h"

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
    printf("Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "\nABORT: main: ");
        report_fopen_error();
        printf("\n");
        return -errno;
    }
    printf("OK.\n\n");

    char* block_buf = malloc(nx_block_size);
    read_blocks(block_buf, nx_block_addr, 1);
    printf("- Details for block 0x%016llx\n", nx_block_addr);
    print_obj_hdr_info(block_buf);
    printf("\n");

    fclose(nx);
    return 0;
}
