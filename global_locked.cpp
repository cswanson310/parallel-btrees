#include "btree.h"
#include "global_locked.h"

lockable_tree new_tree() {
  lockable_tree tree = new root_lock();
  btree root = new_node(true, true);
  tree->root = root;
  return tree;
}

void insert_key(lockable_tree t, int key) {
  t->lock.lock();
  insert_key(t->root, key);
  t->lock.unlock();
}
bool contains_key(lockable_tree t, int key) {
  bool result;
  t->lock.lock();
  result = contains_key(t->root, key);
  t->lock.unlock();
  return result;
}

