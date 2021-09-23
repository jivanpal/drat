#include <inttypes.h>
#include <apfs/btree.h>
#include <apfs/fs.h>
#include <drat/time.h>
#include <apfs/object.h>
#include <apfs/nx.h>
#include <apfs/omap.h>
#include <apfs/fs.h>
#include <apfs/j.h>
#include <apfs/dstream.h>
#include <apfs/sibling.h>
#include <apfs/snap.h>
#include <signal.h>

#include <drat/io.h>
#include <drat/print-fs-records.h>

#include <drat/func/boolean.h>
#include <drat/func/cksum.h>
#include <drat/func/btree.h>
#include <drat/func/j.h>

#include <drat/string/object.h>
#include <drat/string/nx.h>
#include <drat/string/omap.h>
#include <drat/string/btree.h>
#include <drat/string/fs.h>
#include <drat/string/j.h>
#include <string.h>

struct fs_browse_info {
    btree_node_phys_t *fs_omap_btree;
    btree_node_phys_t *fs_root_btree;
};

const char* nth_strchr(const char* s, char c, int n);
char* get_remaining_url(char *url, char token);
char* get_first_entry(char *url, char token);
struct fs_browse_info* get_root_fs_tree(apfs_superblock_t* apsb, uint64_t nx_block_size);