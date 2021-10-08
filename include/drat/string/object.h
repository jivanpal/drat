#ifndef DRAT_STRING_OBJECT_H
#define DRAT_STRING_OBJECT_H

#include <apfs/object.h>

char* o_storage_type_to_string(uint32_t o_type);
char* get_o_type_flags_string(uint32_t o_type);
char* get_o_type_string(uint32_t o_type);
char* get_o_subtype_string(uint32_t o_subtype);
void print_obj_phys(obj_phys_t* obj);

#endif // DRAT_STRING_OBJECT_H
