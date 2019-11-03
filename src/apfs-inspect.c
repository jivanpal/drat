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
    printf("Opening file at `%s` in read-only mode ... ", nx_path);
    nx = fopen(nx_path, "rb");
    if (!nx) {
        fprintf(stderr, "\nABORT: ");
        report_fopen_error();
        printf("\n");
        return -errno;
    }
    printf("OK.\nSimulating a mount of the APFS container.\n");
    
    char* block_buf = malloc(nx_block_size);
    if (!block_buf) {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory to create `block_buf`.\n");
        return -1;
    }

    if (read_blocks(block_buf, 0x0, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to successfully read block 0x0.\n");
        return -1;
    }

    printf("Validating checksum of block 0x0 ... ");
    if (is_cksum_valid(block_buf)) {
        printf("OK.\n");
    } else {
        printf("FAILED.\n!! APFS ERROR !! Checksum of block 0x0 should validate, but it doesn't. Proceeding as if it does.\n");
    }

    printf("\n- Details of block 0x0:\n");
    print_obj_hdr_info(block_buf);
    printf("\n");

    if (!is_nx_superblock(block_buf)) {
        printf("!! APFS ERROR !! Block 0x0 should be a container superblock, but it isn't. Proceeding as if it is.\n\n");
    }
    if (((nx_superblock_t*)block_buf)->nx_magic != NX_MAGIC) {
        printf("!! APFS ERROR !! Container superblock at 0x0 doesn't have the correct magic number. Proceeding as if it does.\n");
    }

    nx_superblock_t* nxsb = malloc(sizeof(nx_superblock_t));
    if (!nxsb) {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory for `nxsb`.\n");
        return -1;
    }
    memcpy(nxsb, block_buf, sizeof(*nxsb));

    printf("Locating the checkpoint descriptor area:\n");
    
    uint32_t xp_desc_blocks = nxsb->nx_xp_desc_blocks & ~(1 << 31);
    printf("- Its length is %u blocks.\n", xp_desc_blocks);

    char (*xp_desc)[nx_block_size] = malloc(xp_desc_blocks * nx_block_size);
    if (!xp_desc) {
        fprintf(stderr, "ABORT: Could not allocate sufficient memory for %u blocks.\n", xp_desc_blocks);
        return -1;
    }

    if (nxsb->nx_xp_desc_blocks >> 31) {
        printf("- It is not contiguous.\n");
        printf("- The physical OID of the B-tree representing it is 0x%016llx.\n", nxsb->nx_xp_desc_base);
        printf("END: The ability to handle this case has not yet been implemented.\n\n");   // TODO: implement case when xp_desc area is not contiguous
        return 0;
    } else {
        printf("- It is contiguous.\n");
        printf("- The address of its first block is 0x%llx.\n", nxsb->nx_xp_desc_base);

        paddr_t xp_desc_end = nxsb->nx_xp_desc_base + xp_desc_blocks;

        printf("Loading the checkpoint descriptor area into memory ... ");
        paddr_t addr;
        uint32_t i;
        for (addr = nxsb->nx_xp_desc_base, i = 0; addr < xp_desc_end; addr++, i++) {
            if (read_blocks(xp_desc[i], addr, 1) != 1) {
                fprintf(stderr, "\nABORT: Failed to read block 0x%llx.\n", addr);
                return -1;
            }
        }
        printf("OK.\n");
    }

    printf("Locating the most recent well-formed container superblock in the checkpoint descriptor area:\n");
    uint32_t i_latest_nx = 0;

    for (uint32_t i = 0; i < xp_desc_blocks; i++) {
        if (!is_cksum_valid(xp_desc[i])) {
            printf("- !! APFS WARNING !! Block at index %u within this area failed checksum validation. Skipping it.\n", i);
            continue;
        }
        
        if (is_nx_superblock(xp_desc[i])) {
            if ( ((nx_superblock_t*)xp_desc[i])->nx_magic  !=  NX_MAGIC ) {
                printf("- !! APFS WARNING !! Container superblock at index %u within this area is malformed; incorrect magic number. Skipping it.\n", i);
                continue;
            }

            if ( ((nx_superblock_t*)xp_desc[i])->nx_o.o_xid  >  ((nx_superblock_t*)xp_desc[i_latest_nx])->nx_o.o_xid ) {
                i_latest_nx = i;
            }
        } else if (!is_checkpoint_map_phys(xp_desc[i])) {
            printf("- !! APFS ERROR !! Block at index %u within this area is not a container superblock or checkpoint map. Skipping it.\n", i);
            continue;
        }
    }

    // Don't need a copy of the block 0x0 NXSB anymore; replace our copy with
    // the latest NXSB (partially so that we don't need to keep casting
    // to `nx_superblock_t*`).
    memcpy(nxsb, xp_desc[i_latest_nx], sizeof(nx_superblock_t));

    printf("- It lies at index %u within the checkpoint descriptor area.\n", i_latest_nx);
    printf("- The corresponding checkpoint starts at index %u within the checkpoint descriptor area, and spans %u blocks.\n", nxsb->nx_xp_desc_index, nxsb->nx_xp_desc_len);

    printf("\n- Details of this container superblock:\n\n");
    print_obj_hdr_info(nxsb);
    printf("\n");

    // Copy the contents of the corresponding checkpoint somewhere else so that
    // we can discard the rest of the checkpoint descriptor area.
    // 
    // The checkpoint descriptor area is a ring buffer stored as an array, so
    // this copying process also allows us to store the latest checkpoint
    // contiguously in memory in the event that the checkpoint wraps around the
    // from the end of the array to the start.
    printf("Loading the corresponding checkpoint:\n");
    
    char (*xp)[nx_block_size] = malloc(nxsb->nx_xp_desc_len * nx_block_size);
    if (!xp) {
        fprintf(stderr, "ABORT: Couldn't allocate sufficient memory.\n");
        return -1;
    }

    if (nxsb->nx_xp_desc_index + nxsb->nx_xp_desc_len <= xp_desc_blocks) {
        // The simple case: the checkpoint is already contiguous in `xp_desc`.
        memcpy(xp, xp_desc[nxsb->nx_xp_desc_index], nxsb->nx_xp_desc_len * nx_block_size);
    } else {
        // The case where the checkpoint wraps around the array.
        uint32_t segment_1_len = xp_desc_blocks - nxsb->nx_xp_desc_index;
        uint32_t segment_2_len = nxsb->nx_xp_desc_len - segment_1_len;
        memcpy(xp,                 xp_desc + nxsb->nx_xp_desc_index, segment_1_len * nx_block_size);
        memcpy(xp + segment_1_len, xp_desc,                          segment_2_len * nx_block_size);
    }
    
    free(xp_desc);

    printf("\n- Details for each block in this checkpoint:\n\n");
    for (uint32_t i = 0; i < nxsb->nx_xp_desc_len; i++) {
        print_obj_hdr_info(xp[i]);
        printf("\n");
    }

    free(nxsb);
    free(block_buf);
    fclose(nx);
    printf("END: All done.\n");
    return 0;
}
