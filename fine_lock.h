#include <mutex>
#include "def.h"

fine_btree new_fine_node(bool leaf, bool root);

btree to_btree(fine_btree t);
void print_tree(fine_btree t);

void insert_key(fine_btree t, int key);
bool contains_key(fine_btree t, int key);
