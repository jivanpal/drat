#include "general.h"

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

void print_uuid(uuid_t uuid) {
    for (int i = 0; i < 16; i++) {
        switch (i) {
            case 4: case 6: case 8: case 10:
                printf("-");
                // fall through
            default:
                printf("%02X", uuid[i]);
                break;
        }
    }
}
