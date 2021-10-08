#include <drat/utilities.h>

const char* nth_strchr(const char* s, char c, int n) {
    int c_count;
    char* nth_ptr;
    for (c_count=1,nth_ptr=strchr(s,c); nth_ptr != NULL && c_count < n && c!=0; c_count++) {
         nth_ptr = strchr(nth_ptr+1, c);
    }
    return nth_ptr;
}

// /private/var/etc -> /var/etc -> /etc -> null
char* get_remaining_url(char *url, char token) {
    const char *position_ptr = nth_strchr(url, token, 2);
    int position = (position_ptr == NULL ? -1 : position_ptr - url);

    if (position > 0) {
        char *newurl = malloc(strlen(url) - position);
        strncpy(newurl, url+position, strlen(url) - position + 1);
        return newurl;
    }
    return NULL;
}

// /private/var/etc -> private
char* get_first_entry(char *url, char token) {
    char *left_position_ptr = strchr(url, token);
    if (left_position_ptr == NULL) {
        return NULL;
    }
    
    int left_position = left_position_ptr - url;
    const char *right_position_ptr = nth_strchr(url, token, 2);
    if (right_position_ptr == NULL) {
        char *entry = malloc(strlen(url));
        strncpy(entry, url + left_position + 1, strlen(url));
        return entry;
    }
    int right_position = right_position_ptr - url;
    char *entry = malloc(right_position - left_position - 1);
    strncpy(entry, url+left_position+1, right_position - left_position - 1);
    return entry;
}

struct fs_browse_info* get_root_fs_tree(apfs_superblock_t* apsb, uint64_t nx_block_size) {
    omap_phys_t *fs_omap = malloc(nx_block_size);
    if (!fs_omap)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap`.\n");
        return NULL;
    }
    if (read_blocks(fs_omap, apsb->apfs_omap_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", apsb->apfs_omap_oid);
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the volume object map ... ");
    if (!is_cksum_valid(fs_omap))
    {
        fprintf(stderr, "\nFAILED. The checksum did not validate.");
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    if ((fs_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL)
    {
        fprintf(stderr, "END: The volume object map B-tree is not of the Physical storage type, and therefore it cannot be located.\n");
        return NULL;
    }

    fprintf(stderr, "Reading the root node of the volume object map B-tree ... ");
    btree_node_phys_t *fs_omap_btree = malloc(nx_block_size);
    if (!fs_omap_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_omap_btree`.\n");
        return NULL;
    }
    if (read_blocks(fs_omap_btree, fs_omap->om_tree_oid, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_omap->om_tree_oid);
        return NULL;
    }
    fprintf(stderr, "OK.\n");

    fprintf(stderr, "Validating the root node of the volume object map B-tree ... ");
    if (!is_cksum_valid(fs_omap_btree))
    {
        fprintf(stderr, "FAILED.\n");
    }
    else
    {
        fprintf(stderr, "OK.\n");
    }

    fprintf(stderr, "The file-system tree root for this volume has Virtual OID 0x%" PRIx64 ".\n", apsb->apfs_root_tree_oid);
    fprintf(stderr, "Looking up this Virtual OID in the volume object map ... ");
    omap_entry_t *fs_root_entry = get_btree_phys_omap_entry(fs_omap_btree, apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
    if (!fs_root_entry)
    {
        fprintf(stderr, "\nABORT: No objects with Virtual OID 0x%" PRIx64 " and maximum XID 0x%" PRIx64 " exist in `fs_omap_btree`.\n", apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
        return NULL;
    }
    fprintf(stderr, "corresponding block address is 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);

    fprintf(stderr, "Reading ... ");
    btree_node_phys_t *fs_root_btree = malloc(nx_block_size);
    if (!fs_root_btree)
    {
        fprintf(stderr, "\nABORT: Could not allocate sufficient memory for `fs_root_btree`.\n");
        return NULL;
    }
    if (read_blocks(fs_root_btree, fs_root_entry->val.ov_paddr, 1) != 1)
    {
        fprintf(stderr, "\nABORT: Failed to read block 0x%" PRIx64 ".\n", fs_root_entry->val.ov_paddr);
        return NULL;
    }
    free(fs_root_entry); // No longer need the block address of the file-system root.

    fprintf(stderr, "validating ... ");
    if (!is_cksum_valid(fs_root_btree))
    {
        fprintf(stderr, "FAILED.\nGoing back to look at the previous checkpoint instead.\n");
        return NULL;
    }
    fprintf(stderr, "OK.\n");
    struct fs_browse_info* return_info = malloc(sizeof(struct fs_browse_info));
    return_info->fs_omap_btree = fs_omap_btree;
    return_info->fs_root_btree = fs_root_btree;
    free(fs_omap);
    return return_info;
}
