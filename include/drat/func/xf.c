#include "xf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/**
 * Given an instance of `xf_blob_t` (such as `j_drec_val_t::xfields` or
 * `j_inode_val_t::xfields`), get an array of instances of `xf_pair_t`, which
 * allows you to easily access the xfield value for a given xfield key/type,
 * since they are then stored directly alongside each other.
 * 
 * xfields:
 *      A pointer to an instance of `xf_blob_t`, including the data in its
 *      `xf_data` field.
 * 
 * RETURN VALUE:
 *      A pointer to the head of an array of pointers to instances of
 *      `xf_pair_t`. The length of this array is not returned to the caller,
 *      but the last element of this array will be a NULL pointer. If an error
 *      occurs, NULL is returned.
 * 
 *      When the data in the array is no longer needed, it is the caller's
 *      responsibility to free the associated memory by passing the pointer
 *      that was returned by this function to `free_xf_pairs_array()`.
 */
xf_pair_t** get_xf_pairs_array(xf_blob_t* xfields) {
    x_field_t* xf_keys = xfields->xf_data;
    
    // The array to return
    xf_pair_t** xf_pairs_array = malloc(xfields->xf_num_exts * sizeof(xf_pair_t*));
    if (!xf_pairs_array) {
        fprintf(stderr, "\nERROR: %s: Couldn't create `xf_pairs_array`.\n", __func__);
        return NULL;
    }

    // The start of the xfield data/values array
    uint8_t* xf_values = xfields->xf_data + xfields->xf_num_exts * sizeof(x_field_t);
    
    /*
     * Use this value to keep track of how many bytes into `xf_values` the
     * current xfield's value starts. This will always be a multiple of 8 before
     * we copy a value, since the values are 8-byte aligned.
     */
    uint16_t xf_value_cursor_index = 0;

    // Populate the array
    for (uint16_t i = 0; i < xfields->xf_num_exts; i++) {
        // Move to the start of the next 8-byte aligned segment.
        uint16_t remainder = xf_value_cursor_index % 8;
        if (remainder != 0) {
            xf_value_cursor_index += 8 - remainder;
        }

        xf_pairs_array[i] = malloc(sizeof(xf_pair_t) + xf_keys[i].x_size);
        if (!xf_pairs_array[i]) {
            fprintf(stderr, "\nERROR: %s: Couldn't create `xf_pairs_array[%"PRIu16"].`\n", __func__, i);
            free_xf_pairs_array(xf_pairs_array);
            return NULL;
        }

        memcpy(xf_pairs_array[i],           xf_keys + i,    sizeof(x_field_t));
        memcpy(&(xf_pairs_array[i]->value), xf_values + i,  xf_keys[i].x_size);

        xf_value_cursor_index += xf_keys[i].x_size;
    }
    xf_pairs_array[xfields->xf_num_exts] = NULL;

    return xf_pairs_array;
}

/**
 * Free memory allocated for a file-system records array that
 * was created by a call to `get_xf_pairs_array()`.
 * 
 * xf_pairs_array:
 *      A pointer to an array of pointers to instances of `xf_pair_t`, as
 *      returned by a call to `get_xfields_array()`.
 */
void free_xf_pairs_array(xf_pair_t** xf_pairs_array) {
    if (!xf_pairs_array) {
        return;
    }

    for (xf_pair_t** cursor = xf_pairs_array; *cursor; cursor++) {
        free(*cursor);
    }
    free(xf_pairs_array);
}
