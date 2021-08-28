#ifndef DRAT_FUNC_XF_H
#define DRAT_FUNC_XF_H

#include <apfs/xf.h>

/**
 * Data structure used to collect an xfield's key (an instance of `x_field_t`,
 * which contains its type, flags, and the size of its value) and its value
 * together. The function `get_xfields_array()`, which returns a pointer
 * to an array of instance of this data structure, allows us to more easily
 * access xfields without needing to do cumbersome arithmetic to find the xfield
 * value in an instance of `xf_blob_t`.
 */
typedef struct {
    x_field_t   key;
    uint8_t     value[];
} xf_pair_t;

xf_pair_t** get_xf_pairs_array(xf_blob_t* xfields);

void free_xf_pairs_array(xf_pair_t** xfields_array);

#endif // DRAT_FUNC_XF_H
