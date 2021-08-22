#ifndef DRAT_STRING_COMMON_H
#define DRAT_STRING_COMMON_H

#include <stdint.h>

// TODO: Factor out macro (duplicated in `src/commands.h`)
#define ARRAY_SIZE(a)   (sizeof(a) / sizeof(*(a)))

struct u64_string_mapping {
    uint64_t value;
    char* string;
};

#endif // DRAT_STRING_COMMON_H
