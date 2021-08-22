#ifndef APFS_CRYPTOTYPES_H
#define APFS_CRYPTOTYPES_H

/**
 * Definitions of types used for encryption-related structures, as defined in
 * §15.6 Encryption Types.
 * 
 * These types are defined in their own header file rather than being included
 * in `crypto.h`, else there would be a dependency cycle
 * (`j.h` ==> `crypto.h` ==> `j.h`).
 */

#include <stdint.h>

/** Encryption Types **/

typedef uint32_t    cp_key_class_t;
typedef uint32_t    cp_key_os_version_t;
typedef uint16_t    cp_key_revision_t;
typedef uint32_t    crypto_flags_t;

#endif // APFS_CRYPTOTYPES_H
