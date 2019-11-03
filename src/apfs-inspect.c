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
    if (argc != 2) {
        printf("Incorrect number of arguments.\n");
        print_usage(argv[0]);
        return 1;
    }
    nx_path = argv[1];
    
    // Open (device special) file corresponding to an APFS container, read-only
    printf("Opening file at `%s`.\n", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "ABORT: main: ");
        report_fopen_error();
        printf("\n");
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

    printf("\nReading block 0x0 into buffer ... ");
    read_blocks(block_buf, 0x0, 1);
    printf("OK.\n");

    printf("Validating block checksum ... ");

    if (!is_cksum_valid(block_buf)) {
        printf("FAILED.\nEND: Checksum of block 0x0 is invalid.\n\n");
        return 0;
    }
    printf("OK.\n");

    printf("\n- Details of block 0x0:\n");
    print_obj_hdr_info(block_buf);
    printf("\n");

    if (!is_nx_superblock(block_buf)) {
        printf("END: Block 0x0 is not a container superblock.\n\n");
        return 0;
    }

    nx_superblock_t* nxsb = malloc(sizeof(nx_superblock_t));
    if (!nxsb) {
        fprintf(stderr, "ABORT: main: Could not allocate sufficient memory for `nxsb`.\n");
        return -1;
    }
    memcpy(nxsb, block_buf, sizeof(*nxsb));

    printf("Preparing to read container superblocks from the checkpoint descriptor area:\n");
    printf("- Allocating memory for array of container superblocks ... ");
    
    size_t num_read = 0;
    size_t num_alloced = MALLOC_INCREMENT;

    /**
     * Simple struct used only in this scope. Allows us to keep track of the
     * physical block address of the ephemeral NXSBs along with each NXSB.
     */
    typedef struct paddr_block {
        paddr_t         paddr;
        char            block[nx_block_size];
    } paddr_block_t;

    paddr_block_t* nx_blocks = malloc(num_alloced * sizeof(paddr_block_t));
    if (!nx_blocks) {
        fprintf(stderr, "\nABORT: main: Could not allocate sufficient memory for %lu container superblocks.\n", num_alloced);
        return -1;
    }
    printf("OK.\n");

    printf("Locating the checkpoint descriptor area:\n");
    if (nxsb->nx_xp_desc_blocks >> 31 == 0) {
        printf("- The area is contiguous.\n");
        printf("- The address of its first block is 0x%016llx.\n", nxsb->nx_xp_desc_base);

        printf("Reading blocks from the checkpoint descriptor area into memory:\n");
        paddr_t current_block_addr = nxsb->nx_xp_desc_base;

        while (1) {
            read_blocks(block_buf, current_block_addr, 1);
            if (!is_cksum_valid(block_buf)) {
                printf("- Block 0x%llx failed checksum validation. Reading it anyway.\n", current_block_addr);
            }

            if (!is_nx_superblock(block_buf) && !is_checkpoint_map_phys(block_buf)) {
                // We have reached the end of the checkpoint descriptor area.
                break;
            }
            
            if (is_nx_superblock(block_buf)) {
                if (num_read >= num_alloced) {
                    // Need more memory
                    num_alloced += MALLOC_INCREMENT;
                    printf("- Read %lu container superblocks into memory so far. Allocating more memory for more container superblocks ... ", num_read);
                    nx_blocks = realloc(nx_blocks, num_alloced * sizeof(paddr_block_t));
                    if (!nx_blocks) {
                        fprintf(stderr, "\nABORT: main: Could not allocate sufficient memory for %lu instances of `nx_superblock_t` in `xp_desc_nx_blocks`.\n", num_alloced);
                        return -1;
                    }
                    printf("OK.\n");
                }

                // Copy the buffered NXSB into the array
                nx_blocks[num_read].paddr = current_block_addr;
                memcpy(nx_blocks[num_read].block, block_buf, nx_block_size);
                num_read++;
            }

            current_block_addr++;
        }
        // De-allocate excess memory.
        nx_blocks = realloc(nx_blocks, num_read * sizeof(paddr_block_t));

        printf("- Reached the end of the checkpoint descriptor area. Found %lu container superblocks and successfully read them into memory.\n", num_read);
    } else {
        printf("- The area is not contiguous.\n");
        printf("- The physical OID of the B-tree representing the area is 0x%016llx.\n", nxsb->nx_xp_desc_base);
        printf("END: The ability to handle this case has not yet been implemented.\n\n");   // TODO: implement case when xp_desc area is not contiguous
        return 0;
    }

    printf("Searching array for the container superblock with the highest XID:\n");
    // We find the latest NXSB from array `nx_blocks`, pointing `nx_latest` to it.
    paddr_block_t* nx_end           = nx_blocks + num_read;
    paddr_block_t* nx_latest        = nx_blocks;
    paddr_block_t* nx_cursor        = nx_latest + 1;
    
    // Don't need the block zero NXSB anymore, which is stored in `nxsb`;
    // use it for something else. These pointers are being used to avoid lots
    // of messy casting to `nx_superblock_t*`.
    nxsb = nx_latest->block;
    nx_superblock_t* nxsb_cursor;

    while (nx_cursor < nx_end) {
        if (!is_cksum_valid(nx_cursor->block)) {
            printf("- Block 0x%llx is malformed; failed checksum validation. Ignoring that block.\n", nx_cursor->paddr);
        } else {
            nxsb_cursor = nx_cursor->block;
            if (nxsb_cursor->nx_magic != NX_MAGIC) {
                printf("- Block 0x%llx is malformed; magic value is not as expected. Ignoring that block.\n", nx_cursor->paddr);
            } else if (nxsb_cursor->nx_o.o_xid > nxsb->nx_o.o_xid) {
                nx_latest = nx_cursor;
                nxsb = nx_latest->block;
            }
        }

        nx_cursor++;
    }
    printf("- Block 0x%llx has the highest XID.\n", nx_latest->paddr);
    printf("\n- Details of block 0x%llx:\n", nx_latest->paddr);
    print_obj_hdr_info(nxsb);
    printf("----\n");
    printf("Number of blocks in corresponding checkpoint:   %u\n", nxsb->nx_xp_desc_len);
    printf("desc_index:     0x%x\n", nxsb->nx_xp_desc_index);
    printf("\n");
}
