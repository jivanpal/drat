#include "time.h"

#include <time.h>

/**
 * Get a human-readable timestamp from an APFS timestamp (Unix timestamp in
 * nanoseconds). The return value is a newline-terminated C-string stored in
 * the static buffer provided by the `ctime` API, so it must not be freed, and
 * this function is not thread-safe.
 */
char* apfs_timestamp_to_string(uint64_t apfs_timestamp) {
    // Dividing timestamps by 10^9 to convert APFS timestamps (Unix timestamps
    // in nanoseconds) to Unix timestamps (in seconds).
    // Trailing '\n' is provided by the result of `ctime()`.
    time_t timestamp = apfs_timestamp / 1000000000;
    return ctime(&timestamp);
}
