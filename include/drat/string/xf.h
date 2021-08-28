#ifndef DRAT_STRING_XF_H
#define DRAT_STRING_XF_H

#include <drat/func/xf.h>

char* x_type_to_string(uint8_t x_type);
char* get_x_flags_string(uint8_t x_flags);
void print_x_field(x_field_t* xfield);
void print_xf_pair(xf_pair_t* xf_pair);
void print_xf_pairs_array(xf_pair_t** xf_pairs_array);

#endif // DRAT_STRING_XF_H
