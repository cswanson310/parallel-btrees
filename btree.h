#include "def.h"

struct node {
  bool is_leaf;
  bool is_root;
  int keys[2*ORDER];
  struct node * children[2*ORDER + 1];
  struct node * parent;
  int num_keys;
};

typedef struct node* btree;
btree new_node(bool leaf, bool root);

bool tree_eq(btree t1, btree t2);
void print_tree(btree t, int depth);

void insert_key(btree t, int key);
bool contains_key(btree t, int key);
