#ifndef APFS_STRING_OMAP_H
#define APFS_STRING_OMAP_H

#include "../struct/omap.h"
#include "object.h"

char* get_om_flags_string(omap_phys_t* omap);
void print_omap_phys(omap_phys_t* omap);
void print_omap_key(omap_key_t* omap_key);
void print_omap_val(omap_val_t* omap_val);

#endif // APFS_STRING_OMAP_H
