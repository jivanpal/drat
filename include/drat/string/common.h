#ifndef DRAT_STRING_COMMON_H
#define DRAT_STRING_COMMON_H

/**
 * Collection of common routines and structures used by `get_*_string()`
 * functions.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Data structure that encodes an enum member's value along with a string
 * describing what that enum member represents. This is used to create an array
 * of such strctures for a particular enum type, which thus collectively
 * describes all of an enum's members, providing an enum-to-string mapping.
 */
typedef struct enum_string_pair {
    uint64_t value;
    char* string;
} enum_string_mapping_t;

char* get_single_enum_string(
    const enum_string_mapping_t* mapping,
    size_t mapping_length,
    uint64_t enum_field
);

char* get_flags_enum_string(
    const enum_string_mapping_t* mapping,
    size_t mapping_length,
    uint64_t flags_field,
    bool single_line
);

#endif // DRAT_STRING_COMMON_H
