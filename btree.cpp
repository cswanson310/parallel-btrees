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
  if (node->num_keys == 0) {
    printf("node %d:[]", node);
    return;
  }
  printf("node %d:[", node);
  for(int i = 0; i < node->num_keys - 1; i++) {
    printf("%d,", node->keys[i]);
  }
  printf("%d]\n", node->keys[node->num_keys-1]);
}

void print_array(int* A, int N) {
  if (N == 0) {
    printf("[]\n");
    return;
  }
  printf("[");
  for(int i = 0; i < N-1; i++) {
    printf("%d,", A[i]);
  }
  printf("%d]\n", A[N-1]);
}

void print_array(btree* A, int N) {
  if (N == 0) {
    printf("[]\n");
    return;
  }
  printf("[");
  for(int i = 0; i < N-1; i++) {
    printf("%d,", A[i]);
  }
  printf("%d]\n", A[N-1]);
}
void print_children(btree t) {
  if (t->num_keys == 0) {
    printf("[]\n");
    return;
  }
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
  if(depth == 0) {
    printf("\n");
  }
}

void insert_just_key(int* old_keys, int* keys, int val) {
  int key_count = 0;
  for (int i = 0; i < 2*ORDER; i++) {
    if (key_count > 0 && old_keys[key_count-1] < val && val <= old_keys[key_count]) {
      keys[key_count] = val;
      key_count++;
    } else if (key_count == 0 && val < old_keys[0]) {
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

void insert_key_into_node(btree node, int key, btree left, btree right) {
  //printf("inserting key (%d) into node: ", key);
  //print_node(node);
  int* new_keys = new int[2*ORDER];
  btree* new_children = new btree[2*ORDER + 1];
  int key_count = 0;
  int child_count = 0;
  for (int i = 0; i < node->num_keys; i++) {
    if ((key_count > 0 && node->keys[key_count-1] < key && key <= node->keys[i]) ||
        (key_count == 0 && key < node->keys[0])) {
      //printf("1\n");
      new_keys[key_count] = key;
      key_count++;
      new_children[child_count] = left;
      left->parent = node;
      child_count++;
      new_children[child_count] = right;
      right->parent = node;
      child_count++;
    } else {
      //printf("2\n");
      new_children[child_count] = node->children[i];
      child_count++;
    }
    //printf("3\n");
    new_keys[key_count] = node->keys[i];
    key_count++;
  }
  if (key_count < node->num_keys+ 1) {
    //printf("4\n");
    new_keys[key_count] = key;
    new_children[child_count] = left;
    left->parent = node;
    child_count++;
    new_children[child_count] = right;
    right->parent = node;
  } else {
    //printf("5\n");
    new_children[child_count] = node->children[node->num_keys];
    child_count++;
  }
  node->num_keys++;
  memcpy(node->keys, new_keys, sizeof(int)*node->num_keys);
  memcpy(node->children, new_children, sizeof(btree)*(node->num_keys+1));
  //printf("done inserting!\n");
  //print_tree(node, 0);
}

void insert_key_and_children(int* old_keys, int* keys,
                             btree* old_children, btree* children,
                             int val, btree left_child, btree right_child,
                             btree new_left, btree new_right) {
  int key_count = 0;
  int child_count = 0;
  for (int i = 0; i < 2*ORDER; i++) {
    if ((key_count > 0 && old_keys[key_count-1] < val && val <= old_keys[i]) ||
        (key_count == 0 && val < old_keys[0])) {
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
      insert_key_into_node(node, key, left_child, right_child);
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
      memcpy(left->keys, all_keys, sizeof(int)*ORDER);
      memcpy(left->children, all_children, sizeof(btree)*(ORDER+1));
      left->num_keys = ORDER;
      memcpy(right->keys, &all_keys[ORDER + 1], sizeof(int)*ORDER);
      memcpy(right->children, &all_children[ORDER + 1], sizeof(btree)*(ORDER + 1));
      right->num_keys = ORDER;
      percolate_up(node->parent, all_keys[ORDER], left, right);
    } else {
      /* not full! Just add it */
      /* actually need to find the right position for it... */
      insert_key_into_node(node, key, left_child, right_child);
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
      insert_key_into_node(t, key, new_node(true, false), new_node(true, false));
    } else { /* we have to split */
      split(t, key);
    }
  } else if (t->is_leaf && t->is_root) {
    if (t->num_keys < 2*ORDER) {
      /* there's room for another key here */
      insert_key_into_node(t, key, new_node(true, false), new_node(true, false));
    } else { /* we have to split */
      btree fake = new_node(true, false);
      fake->parent = t;
      memcpy(fake->keys, t->keys, sizeof(int)*t->num_keys);
      memcpy(fake->children, t->children, sizeof(btree)*(t->num_keys+1));
      fake->num_keys = t->num_keys;
      t->num_keys = 0;
      t->is_leaf = false;
      split(fake, key);
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
  /*print_tree(root, 0);
  insert_key(root, 14);
  print_tree(root, 0);
  insert_key(root, 15);
  print_tree(root, 0);
  insert_key(root, 0);
  print_tree(root, 0);
  insert_key(root, 8);
  print_tree(root, 0);
  insert_key(root, 8);
  print_tree(root, 0);*/
  root = new_node(true, true);
  for (int i=0; i < 16;i++) {
    insert_key(root, i);
    printf("in main: \n");
    print_tree(root, 0);
  }
}
