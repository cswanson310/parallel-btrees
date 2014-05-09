/*
 * Defines all the constants I'll need
 */


#define ORDER 1

#define MAXWORDSIZE 100

#ifndef NODE_STRUCTS
#define NODE_STRUCTS
#include <mutex>

struct node {
  bool is_leaf;
  bool is_root;
  int keys[2*ORDER];
  struct node * children[2*ORDER + 1];
  struct node * parent;
  int num_keys;
};

typedef struct node* btree;

struct fine_node {
  std::mutex lock;
  bool is_leaf;
  bool is_root;
  int keys[2*ORDER];
  struct fine_node * children[2*ORDER + 1];
  struct fine_node * parent;
  int num_keys;
};

typedef struct fine_node* fine_btree;

struct root_lock {
  std::mutex lock;
  btree root;
};

typedef struct root_lock* lockable_tree;

#endif
