#ifndef DRAT_STRING_BTREE_H
#define DRAT_STRING_BTREE_H

#include <apfs/btree.h>

char* get_btn_flags_string(btree_node_phys_t* btn);
char* get_bt_info_flags_string(btree_info_t* bt_info);

void print_btree_info(btree_info_t* bt_info);
void print_btree_node_phys(btree_node_phys_t* btn);

#endif // DRAT_STRING_BTREE_H
