#include "def.h"

btree new_node(bool leaf, bool root);

bool tree_eq(btree t1, btree t2);
void print_tree(btree t, int depth);

void insert_key(btree t, int key);
bool contains_key(btree t, int key);

void insert_just_key(int* old_keys, int* keys, int val);
