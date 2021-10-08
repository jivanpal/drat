#ifndef APFS_CRYPTO_H
#define APFS_CRYPTO_H

/**
 * Structures and related items as defined in
 * §15 Encryption
 */

#include <stdint.h>
#include <apfs/cryptotypes.h>
#include <apfs/general.h>       // uuid_t
#include <apfs/j.h>             // j_key_t
#include <apfs/object.h>        // obj_phys_t

/** `j_crypto_key_t` **/

typedef struct {
    j_key_t     hdr;
} __attribute__((packed))   j_crypto_key_t;

/** `wrapped_crypto_state_t` --- forward declared for `j_crypto_val_t` **/

typedef struct {
    uint16_t            major_version;
    uint16_t            minor_version;
    crypto_flags_t      cpflags;
    cp_key_class_t      persistent_class;
    cp_key_os_version_t key_os_version;
    cp_key_revision_t   key_revision;
    uint16_t            key_len;
    uint8_t             persistent_key[0];
} __attribute__((aligned(2), packed))   wrapped_crypto_state_t;

#define CP_MAX_WRAPPEDKEYSIZE   128

/** `j_crypto_val_t` **/

typedef struct {
    uint32_t                refcnt;
    wrapped_crypto_state_t  state;
} __attribute__((aligned(4), packed))   j_crypto_val_t;

/** `wrapped_meta_crypto_state_t` **/

typedef struct {
    uint16_t            major_version;
    uint16_t            minor_version;
    crypto_flags_t      cpflags;
    cp_key_class_t      persistent_class;
    cp_key_os_version_t key_os_version;
    cp_key_revision_t   key_revision;
} __attribute__((aligned(2), packed))     wrapped_meta_crypto_state_t;

/** Protection Classes **/

#define PROTECTION_CLASS_DIR_NONE    0
#define PROTECTION_CLASS_A           1
#define PROTECTION_CLASS_B           2
#define PROTECTION_CLASS_C           3
#define PROTECTION_CLASS_D           4
#define PROTECTION_CLASS_F           6
#define PROTECTION_CLASS_M          14

#define CP_EFFECTIVE_CLASSMASK      0x0000001f

/** Encryption Identifiers **/

#define CRYPTO_SW_ID                4
#define CRYPTO_RESERVED_5           5

#define APFS_UNASSIGNED_CRYPTO_ID   (~0ULL)

/** `keybag_entry_t` --- forward declared for `kb_locker_t` **/

typedef struct {
    uuid_t      ke_uuid;
    uint16_t    ke_tag;
    uint16_t    ke_keylen;
    uint8_t     padding[4];
    uint8_t     ke_keydata[];
} keybag_entry_t;

#define APFS_VOL_KEYBAG_ENTRY_MAX_SIZE      512
#define APFS_FV_PERSONAL_RECOVERY_KEY_UUID  "EBC6C064-0000-11AA-AA11-00306543ECAC"

/** `kb_locker_t` **/

typedef struct {
    uint16_t        kl_version;
    uint16_t        kl_nkeys;
    uint32_t        kl_nbytes;
    uint8_t         padding[8];
    keybag_entry_t  kl_entries[];
} kb_locker_t;

#define APFS_KEYBAG_VERSION     2

/** `media_keybag_t` **/

typedef struct {
    obj_phys_t  mk_obj;
    kb_locker_t mk_locker;
} media_keybag_t;

/** Keybag Tags **/

enum {
    KB_TAG_UNKNOWN                  = 0,
    KB_TAG_RESERVED_1               = 1,
    
    KB_TAG_VOLUME_KEY               = 2,
    KB_TAG_VOLUME_UNLOCK_RECORDS    = 3,
    KB_TAG_VOLUME_PASSPHRASE_HINT   = 4,

    KB_TAG_WRAPPING_M_KEY           = 5,
    KB_TAG_VOLUME_M_KEY             = 6,
    
    KB_TAG_RESERVED_F8              = 0xf8,
};

#endif // APFS_CRYPTO_H
