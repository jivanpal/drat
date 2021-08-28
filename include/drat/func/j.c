#include "j.h"

#include <stdint.h>
#include <stdio.h>

#include <apfs/xf.h>
#include <apfs/dstream.h>
#include <apfs/j.h>

#include <drat/func/xf.h>

uint64_t get_file_size(j_inode_val_t* inode, uint16_t inode_len) {
    if (inode->internal_flags & INODE_HAS_UNCOMPRESSED_SIZE) {
        return inode->uncompressed_size;
    }

    // If inode has xfields, get file size from there
    if (inode_len != sizeof(j_inode_val_t)) {

        xf_pair_t** xf_pairs = get_xf_pairs_array(inode->xfields);
        if (!xf_pairs) {
            // TODO: Error handling using `errno`?
            fprintf("\nERROR: %s: Call to `get_xf_pairs_array()` failed.\n", __func__);
            return 0;
        }

        for (xf_pair_t** cursor = xf_pairs; *cursor; cursor++) {
            xf_pair_t* xf_pair = *cursor;
            if (xf_pair->key.x_type == INO_EXT_TYPE_DSTREAM) {
                j_dstream_t* dstream = &(xf_pair->value);
                return dstream->size;
            }
        }
    }

    // TODO: Error handling using `errno`?
    fprintf("\nERROR: %s: No file size found.\n", __func__);
    return 0;
}
