#include <cstdio>
#include <cstring>
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

void insert_key(btree t, int key);
void split(btree node, int key);

btree new_node(bool leaf, bool root) {
  btree n = new node();
  for (int i = 0; i < 2*ORDER; i++) {
    n->keys[i] = 0;
  }
  n->is_leaf = leaf;
  n->is_root = root;
  n->num_keys = 0;
  return n;
}

void print_node(btree node) {
  printf("node %d:[", node);
  for(int i = 0; i < node->num_keys - 1; i++) {
    printf("%d,", node->keys[i]);
  }
  printf("%d]\n", node->keys[node->num_keys-1]);
}

void print_array(int* A, int N) {
  printf("[");
  for(int i = 0; i < N-1; i++) {
    printf("%d,", A[i]);
  }
  printf("%d]\n", A[N-1]);
}

void print_array(btree* A, int N) {
  printf("[");
  for(int i = 0; i < N-1; i++) {
    printf("%d,", A[i]);
  }
  printf("%d]\n", A[N-1]);
}
void print_children(btree t) {
  printf("[");
  for(int i = 0; i < t->num_keys; i++) {
    printf("%d,", t->children[i]);
  }
  printf("%d]\n", t->children[t->num_keys]);
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

void insert_just_key(int* old_keys, int* keys, int val) {
  int key_count = 0;
  for (int i = 0; i < 2*ORDER; i++) {
    if (i > 0 && old_keys[i-1] < val && val <= old_keys[i]) {
      keys[key_count] = val;
      key_count++;
    } else if (i == 0 && val < old_keys[0]) {
      keys[0] = val;
      key_count++;
    }
    keys[key_count] = old_keys[i];
    key_count++;
  }
  if (key_count < 2*ORDER + 1) {
    keys[key_count] = val;
  }
}

void add_child(btree* children, int child_count, btree child, btree new_left, btree new_right) {
  if (child_count < ORDER + 1) {
    child->parent = new_left;
  } else {
    child->parent = new_right;
  }
  children[child_count] = child;
}

void insert_key_and_children(int* old_keys, int* keys,
                             btree* old_children, btree* children,
                             int val, btree left_child, btree right_child,
                             btree new_left, btree new_right) {
  int key_count = 0;
  int child_count = 0;
  for (int i = 0; i < 2*ORDER; i++) {
    if ((i > 0 && old_keys[i-1] < val && val <= old_keys[i]) ||
        (i == 0 && val < old_keys[0])) {
      keys[key_count] = val;
      key_count++;
      add_child(children, child_count, left_child, new_left, new_right);
      child_count++;
      add_child(children, child_count, right_child, new_left, new_right);
      child_count++;
    }
    keys[key_count] = old_keys[i];
    key_count++;
    add_child(children, child_count, old_children[i], new_left, new_right);
    child_count++;
  }
  if (key_count < 2*ORDER + 1) {
    keys[key_count] = val;
    add_child(children, child_count, left_child, new_left, new_right);
    child_count++;
    add_child(children, child_count, right_child, new_left, new_right);
  }
}

void percolate_up(btree node, int key, btree left_child, btree right_child) {
  if (node->is_root) {
    if (node->num_keys == 2*ORDER) {
      /* full root! Gotta split! */
      btree left = new_node(node->is_leaf, false);
      btree right = new_node(node->is_leaf, false);
      int* all_keys = new int[2*ORDER + 1];
      btree* all_children = new btree[2*ORDER + 2];
      insert_key_and_children(node->keys, all_keys, node->children, all_children,
                              key, left_child, right_child, left, right);
      left->parent = node;
      right->parent = node;
      memcpy(left->keys, all_keys, sizeof(int)*ORDER);
      memcpy(left->children, all_children, sizeof(btree)*(ORDER+1));
      left->num_keys = ORDER;
      memcpy(right->keys, &all_keys[ORDER + 1], sizeof(int)*ORDER);
      memcpy(right->children, &all_children[ORDER + 1], sizeof(btree)*(ORDER + 1));
      right->num_keys = ORDER;
      node->keys[0] = all_keys[ORDER];
      node->children[0] = left;
      node->children[1] = right;
      node->num_keys = 1;
    } else {
      /* at the root, but not full */
      node->keys[node->num_keys] = key;
      node->children[node->num_keys] = left_child;
      node->children[node->num_keys + 1] = right_child;
      node->num_keys++;
    }
  } else {
    /* not the root */
    if (node->num_keys == 2*ORDER) {
      /* we're full, gotta split and send it upstairs! */
      int* all_keys = new int[2*ORDER + 1];
      btree* all_children = new btree[2*ORDER + 2];
      btree left = new_node(node->is_leaf, false);
      btree right = new_node(node->is_leaf, false);
      insert_key_and_children(node->keys, all_keys, node->children, all_children,
                              key, left_child, right_child, left, right);
      memcpy(left->keys, all_keys, ORDER);
      memcpy(left->children, all_children, ORDER+1);
      left->num_keys = ORDER;
      memcpy(right->keys, &all_keys[ORDER + 1], ORDER);
      memcpy(right->children, &all_children[ORDER + 1], ORDER + 1);
      right->num_keys = ORDER;
      percolate_up(node->parent, all_keys[ORDER], left, right);
    } else {
      /* not full! Just add it */
      node->keys[node->num_keys] = key;
      node->children[node->num_keys] = left_child;
      node->children[node->num_keys + 1] = right_child;
      node->num_keys++;
    }
  }
}

/* distribute keys evenly between two new nodes, and percolate the middle
 * element upstairs
 */
void split(btree node, int key) {
  /* here we assume node->num_keys == 2*ORDER, and that node is a leaf */
  btree left = new_node(node->is_leaf, false);
  btree right = new_node(node->is_leaf, false);
  int* all_keys = new int[2*ORDER + 1];
  insert_just_key(node->keys, all_keys, key);
  /* now we have all of them in order */
  for (int j = 0; j < ORDER; j++) {
    left->keys[j] = all_keys[j];
    left->num_keys++;
  }
  for (int j = ORDER + 1; j < 2*ORDER + 1; j++) {
    right->keys[j - ORDER - 1] = all_keys[j];
    right->num_keys++;
  }
  percolate_up(node->parent, all_keys[ORDER], left, right);
}

void insert_key(btree t, int key) {
  if (t->is_leaf && !t->is_root) {
    if (t->num_keys < 2*ORDER) {
      /* there's room for another key here */
      t->keys[t->num_keys] = key;
      t->num_keys++;
    } else { /* we have to split */
      split(t, key);
    }
  } else {
    int i = 0;
    while (i < t->num_keys && t->keys[i]<key) {
      i++;
    }
    insert_key(t->children[i], key);
  }
}

btree create_test_tree() {
  btree root = new_node(false, true);
  root->keys[0] = 6;
  root->keys[1] = 9;
  root->num_keys = 2;
  btree left = new_node(true, false);
  left->keys[0] = 1;
  left->keys[1] = 3;
  left->num_keys = 2;
  left->parent = root;
  btree middle = new_node(true, false);
  middle->keys[0] = 7;
  middle->num_keys = 1;
  middle->parent = root;
  btree right = new_node(true, false);
  right->keys[0] = 13;
  right->num_keys = 1;
  right->parent = root;
  root->children[0] = left;
  root->children[1] = middle;
  root->children[2] = right;
  return root;
}

int main() {
  btree root = create_test_tree();
  print_tree(root, 0);
  insert_key(root, 14);
  print_tree(root, 0);
  insert_key(root, 15);
  print_tree(root, 0);
}
