#include "fine_lock.h"
#include "btree.h"
#include <cstring>
#include <assert.h>
#include <list>

fine_btree new_fine_node(bool leaf, bool root) {
  fine_btree n = new fine_node();
  for (int i = 0; i < 2*ORDER; i++) {
    n->keys[i] = 0;
  }
  n->is_leaf = leaf;
  n->is_root = root;
  n->num_keys = 0;
  return n;
}


btree to_btree(fine_btree t) {
  btree node = new_node(t->is_leaf, t->is_root);
  node->num_keys = t->num_keys;
  for(int i = 0; i < t->num_keys; i++) {
    node->keys[i] = t->keys[i];
    if (!t->is_leaf) {
      node->children[i] = to_btree(t->children[i]);
    }
  }
  if (!t->is_leaf) {
    node->children[t->num_keys] = to_btree(t->children[t->num_keys]);
  }
  return node;
}
void print_tree(fine_btree t) {
  print_tree(to_btree(t), 0);
}

bool node_contains_key(fine_btree t, int key) {
  for (int i = 0; i < t->num_keys; i++) {
    if (t->keys[i] == key) {
      return true;
    }
  }
  return false;
}
/*
 * will insert the new key, and the surrounding pointers to children in the
 * correct positions. Obviously, it has to preserve the order
 */
void insert_key_into_node(fine_btree node, int key, fine_btree left, fine_btree right) {
  int* new_keys = new int[2*ORDER];
  fine_btree* new_children = new fine_btree[2*ORDER + 1];
  int key_count = 0;
  int child_count = 0;
  for (int i = 0; i < node->num_keys; i++) {
    if ((key_count > 0 && node->keys[key_count-1] < key && key <= node->keys[i]) ||
        (key_count == 0 && key < node->keys[0])) {
      /* the key goes in between these elements, so add it there, pointing
       * to the new children */
      new_keys[key_count] = key;
      key_count++;
      new_children[child_count] = left;
      left->parent = node;
      child_count++;
      new_children[child_count] = right;
      right->parent = node;
      child_count++;
    } else {
      /* else here b/c we want to overwrite the old value if it's been split */
      new_children[child_count] = node->children[i];
      child_count++;
    }
    new_keys[key_count] = node->keys[i];
    key_count++;
  }
  if (key_count < node->num_keys+ 1) {
    /* we haven't inserted it yet! So put it at the end */
    new_keys[key_count] = key;
    key_count++;
    new_children[child_count] = left;
    left->parent = node;
    child_count++;
    new_children[child_count] = right;
    child_count++;
    right->parent = node;
  } else {
    /* otherwise, we still need to add the last child pointer */
    new_children[child_count] = node->children[node->num_keys];
    child_count++;
  }
  assert(child_count == key_count + 1);
  node->num_keys++;
  memcpy(node->keys, new_keys, sizeof(int)*node->num_keys);
  memcpy(node->children, new_children, sizeof(fine_btree)*(node->num_keys+1));
}

/*
 * when splitting a node, decide which child should point to this child.
 */
void add_child(fine_btree* children, int child_count, fine_btree child, fine_btree new_left, fine_btree new_right) {
  if (child_count < ORDER + 1) {
    child->parent = new_left;
  } else {
    child->parent = new_right;
  }
  children[child_count] = child;
}

/*
 * in the case where we have to split a node, determine where to split,
 * and properly distribute the children pointers between the two new nodes
 */
void split_node(int* old_keys, int* keys,
                fine_btree* old_children, fine_btree* children,
                int val, fine_btree left_child, fine_btree right_child,
                fine_btree new_left, fine_btree new_right) {
  int key_count = 0;
  int child_count = 0;
  for (int i = 0; i < 2*ORDER; i++) {
    if ((key_count > 0 && old_keys[key_count-1] < val && val <= old_keys[i]) ||
        (key_count == 0 && val < old_keys[0])) {
      /* this is where the key goes, so add it and the two child pointers here */
      keys[key_count] = val;
      key_count++;
      add_child(children, child_count, left_child, new_left, new_right);
      child_count++;
      add_child(children, child_count, right_child, new_left, new_right);
      child_count++;
    } else {
      add_child(children, child_count, old_children[i], new_left, new_right);
      child_count++;
    }
    keys[key_count] = old_keys[i];
    key_count++;
  }
  if (key_count < 2*ORDER + 1) {
    keys[key_count] = val;
    key_count++;
    add_child(children, child_count, left_child, new_left, new_right);
    child_count++;
    add_child(children, child_count, right_child, new_left, new_right);
    child_count++;
  } else if (child_count != key_count + 1) {
    // skipped over one, need to add the last one
    add_child(children, child_count, old_children[2*ORDER], new_left, new_right);
    child_count++;
  }
  assert(key_count + 1 == child_count);
}

/*
 * the main tricky part of insertion. node has been passed a key to insert,
 * either insert it if there's room, or split and percolate it up
 */
void percolate_up(fine_btree node, int key, fine_btree left_child, fine_btree right_child) {
  if (node->is_root) {
    if (node->num_keys == 2*ORDER) {
      /* full root! Gotta split! */
      fine_btree left = new_fine_node(node->is_leaf, false);
      fine_btree right = new_fine_node(node->is_leaf, false);
      int* all_keys = new int[2*ORDER + 1];
      fine_btree* all_children = new fine_btree[2*ORDER + 2];
      split_node(node->keys, all_keys, node->children, all_children,
                              key, left_child, right_child, left, right);
      left->parent = node;
      right->parent = node;
      memcpy(left->keys, all_keys, sizeof(int)*ORDER);
      memcpy(left->children, all_children, sizeof(fine_btree)*(ORDER+1));
      left->num_keys = ORDER;
      memcpy(right->keys, &all_keys[ORDER + 1], sizeof(int)*ORDER);
      memcpy(right->children, &all_children[ORDER + 1], sizeof(fine_btree)*(ORDER + 1));
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
      fine_btree* all_children = new fine_btree[2*ORDER + 2];
      fine_btree left = new_fine_node(node->is_leaf, false);
      fine_btree right = new_fine_node(node->is_leaf, false);
      split_node(node->keys, all_keys, node->children, all_children,
                              key, left_child, right_child, left, right);
      memcpy(left->keys, all_keys, sizeof(int)*ORDER);
      memcpy(left->children, all_children, sizeof(fine_btree)*(ORDER+1));
      left->num_keys = ORDER;
      memcpy(right->keys, &all_keys[ORDER + 1], sizeof(int)*ORDER);
      memcpy(right->children, &all_children[ORDER + 1], sizeof(fine_btree)*(ORDER + 1));
      right->num_keys = ORDER;
      percolate_up(node->parent, all_keys[ORDER], left, right);
    } else {
      /* not full! Just add it */
      insert_key_into_node(node, key, left_child, right_child);
    }
  }
}

/* distribute keys evenly between two new nodes, and percolate the middle
 * element upstairs
 */
void split(fine_btree node, int key) {
  /* here we assume node->num_keys == 2*ORDER, and that node is a leaf */
  fine_btree left = new_fine_node(node->is_leaf, false);
  fine_btree right = new_fine_node(node->is_leaf, false);
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

void unlock_all(std::list<fine_btree> locked) {
  for (std::list<fine_btree>::iterator cur = locked.begin(); cur != locked.end(); cur++) {
    //printf("unlocked!\n");
    (*cur)->lock.unlock();
  }
}

void insert_key(fine_btree t, int key, std::list<fine_btree> locked) {
  //assume you're locked if not the root
  if (t->is_root) {
    //printf("1\n");
    t->lock.lock();
    locked.push_front(t);
    //printf("2\n");
  }
  if (t->is_leaf && !t->is_root) {
    if (node_contains_key(t, key)) {
      unlock_all(locked);
      return;
    }
    if (t->num_keys < 2*ORDER) {
      /* there's room for another key here */
      insert_key_into_node(t, key, new_fine_node(true, false), new_fine_node(true, false));
      unlock_all(locked);
      return;
    } else { /* we have to split */
      split(t, key);
      unlock_all(locked);
      return;
    }
  } else if (t->is_leaf && t->is_root) {
    if (node_contains_key(t, key)) {
      unlock_all(locked);
      return;
    }
    if (t->num_keys < 2*ORDER) {
      /* there's room for another key here */
      insert_key_into_node(t, key, new_fine_node(true, false), new_fine_node(true, false));
      unlock_all(locked);
      return;
    } else { /* we have to split */
      fine_btree fake = new_fine_node(true, false);
      fake->parent = t;
      memcpy(fake->keys, t->keys, sizeof(int)*t->num_keys);
      memcpy(fake->children, t->children, sizeof(fine_btree)*(t->num_keys+1));
      fake->num_keys = t->num_keys;
      t->num_keys = 0;
      t->is_leaf = false;
      split(fake, key);
      unlock_all(locked);
      return;
    }
  } else {
    int i = 0;
    while (i < t->num_keys && t->keys[i]<key) {
      i++;
    }
    if (i < t->num_keys && t->keys[i] == key) {
      /* the thing we're trying to insert is already here */
      unlock_all(locked);
      return;
    }
    if (t->children[i]->num_keys < 2*ORDER) {
      /* my child has room, so won't overflow to me */
      //printf("3\n");
      t->children[i]->lock.lock();
      //printf("4\n");
      unlock_all(locked);
      locked.clear();
      locked.push_front(t->children[i]);
      insert_key(t->children[i], key);
      unlock_all(locked);
      return;
    } else {
      //printf("5\n");
      t->children[i]->lock.lock();
      //printf("6\n");
      locked.push_front(t->children[i]);
      insert_key(t->children[i], key);
      unlock_all(locked);
      return;
    }
  }
}

void insert_key(fine_btree t, int key) {
  std::list<fine_btree> locked;
  insert_key(t, key, locked);
}

bool contains_key(fine_btree t, int key) {
  if (t->is_root) {
    //printf("9\n");
    t->lock.lock();
    //printf("10\n");
  }
  int i = 0;
  while (i < t->num_keys && key > t->keys[i]) {
    i++;
  }
  /* here i == t->num_keys || key <= keys[i] */
  if (i == t->num_keys) {
    if (!t->is_leaf) {
      /* there are children, search them */
      t->children[t->num_keys]->lock.lock();
      t->lock.unlock();
      return contains_key(t->children[t->num_keys], key);
    } else {
      /* got to the end of a leaf, and didn't find it */
      t->lock.unlock();
      return false;
    }
  } else {
    if (key == t->keys[i]) {
      t->lock.unlock();
      return true;
    } else {
      /* keys[i-1] < key < keys[i] */
      if(!t->is_leaf) {
        /* there are children, search them */
        //printf("7\n");
        t->children[i]->lock.lock();
        //printf("8\n");
        t->lock.unlock();
        return contains_key(t->children[i], key);
      } else {
        /* this is a leaf, and it's not here :( */
        t->lock.unlock();
        return false;
      }
    }
  }
}
