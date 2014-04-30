#include <cstdio>
#include "def.h"

struct node {
  bool is_leaf;
  int keys[2*ORDER];
  struct node * children[2*ORDER + 1];
  int num_keys;
};

typedef struct node* btree;

btree new_node(bool leaf) {
  btree n = new node();
  for (int i = 0; i < 2*ORDER; i++) {
    n->keys[i] = 0;
  }
  n->is_leaf = leaf;
  n->num_keys = 0;
  return n;
}

void print_tree(btree t, int depth) {
  for (int i = 0; i < t->num_keys; i++) {
    if (!t->is_leaf) {
      print_tree(t->children[i], depth + 1);
    }
    for (int j = 0; j < depth; j++)
      printf("   ");
    printf("%d\n", t->keys[i]);
  }
  if (!t->is_leaf)
    print_tree(t->children[t->num_keys], depth + 1);
}

btree create_test_tree() {
  btree root = new_node(false);
  root->keys[0] = 6;
  root->keys[1] = 9;
  root->num_keys = 2;
  btree left = new_node(true);
  left->keys[0] = 1;
  left->keys[1] = 3;
  left->num_keys = 2;
  btree middle = new_node(true);
  middle->keys[0] = 7;
  middle->num_keys = 1;
  btree right = new_node(true);
  right->keys[0] = 13;
  right->num_keys = 1;
  root->children[0] = left;
  root->children[1] = middle;
  root->children[2] = right;
  return root;
}

int main() {
  btree root = create_test_tree();
  print_tree(root, 0);
}
