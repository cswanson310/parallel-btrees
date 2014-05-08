#include <cstdio>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>
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

/************************ PRINTING + HELPER FUNCTIONS ************************/

void print_node(btree node) {
  if (node->num_keys == 0) {
    /* printf("node %d:[]", node); */
    return;
  }
  /* printf("node %d:[", node); */
  for(int i = 0; i < node->num_keys - 1; i++) {
    printf("%d,", node->keys[i]);
  }
  printf("%d]\n", node->keys[node->num_keys-1]);
}

void print_array(int* a, int n) {
  if (n == 0) {
    printf("[]\n");
    return;
  }
  printf("[");
  for(int i = 0; i < n-1; i++) {
    printf("%d,", a[i]);
  }
  printf("%d]\n", a[n-1]);
}

void print_array(btree* a, int n) {
  if (n == 0) {
    printf("[]\n");
    return;
  }
  printf("[");
  for(int i = 0; i < n-1; i++) {
    //printf("%d,", a[i]);
  }
  /* printf("%d]\n", a[n-1]); */
}
void print_children(btree t) {
  if (t->num_keys == 0) {
    printf("[]\n");
    return;
  }
  printf("[");
  for(int i = 0; i < t->num_keys; i++) {
    //printf("%d,", t->children[i]);
  }
  //printf("%d]\n", t->children[t->num_keys]);
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

/************************ INSERTING + HELPER FUNCTIONS ************************/
/*
 * for a leaf node, insert just the key value into the keys. Don't worry about
 * the children
 */
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

/*
 * will insert the new key, and the surrounding pointers to children in the
 * correct positions. Obviously, it has to preserve the order
 */
void insert_key_into_node(btree node, int key, btree left, btree right) {
  int* new_keys = new int[2*ORDER];
  btree* new_children = new btree[2*ORDER + 1];
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
  memcpy(node->children, new_children, sizeof(btree)*(node->num_keys+1));
}

/*
 * when splitting a node, decide which child should point to this child.
 */
void add_child(btree* children, int child_count, btree child, btree new_left, btree new_right) {
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
                btree* old_children, btree* children,
                int val, btree left_child, btree right_child,
                btree new_left, btree new_right) {
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
void percolate_up(btree node, int key, btree left_child, btree right_child) {
  if (node->is_root) {
    if (node->num_keys == 2*ORDER) {
      /* full root! Gotta split! */
      btree left = new_node(node->is_leaf, false);
      btree right = new_node(node->is_leaf, false);
      int* all_keys = new int[2*ORDER + 1];
      btree* all_children = new btree[2*ORDER + 2];
      split_node(node->keys, all_keys, node->children, all_children,
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
      split_node(node->keys, all_keys, node->children, all_children,
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

/*
 * the main function for insertion. Find the place we need to insert it,
 * then either put it in, or do a split
 */
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

/******************************* SEARCHING *******************************/

/*
 * the main function for deletion. If the key is in the tree, delete it
 * returns true for success, false if the key was not there
 */
bool contains_key(btree t, int key) {
  int i = 0;
  while (i < t->num_keys && key > t->keys[i]) {
    i++;
  }
  /* here i == t->num_keys || key <= keys[i] */
  if (i == t->num_keys) {
    if (!t->is_leaf) {
      /* there are children, search them */
      return contains_key(t->children[t->num_keys], key);
    } else {
      /* got to the end of a leaf, and didn't find it */
      return false;
    }
  } else {
    if (key == t->keys[i]) {
      return true;
    } else {
      /* keys[i-1] < key < keys[i] */
      if(!t->is_leaf) {
        /* there are children, search them */
        return contains_key(t->children[i], key);
      } else {
        /* this is a leaf, and it's not here :( */
        return false;
      }
    }
  }
}

/****************************** COMPARING (EQ) *******************************/

/*
 * return true if t1 is a subset of t2, and false otherwise
 */
bool tree_subset(btree t1, btree t2) {
  for (int i = 0; i < t1->num_keys; i++) {
    if (!contains_key(t2, t1->keys[i])) {
      return false;
    }
    if (!t1->is_leaf && !tree_subset(t1->children[i], t2)) {
      return false;
    }
  }
  if (!t1->is_leaf && !tree_subset(t1->children[t1->num_keys], t2)) {
    return false;
  }
  return true;
}

/*
 * takes two trees, and returns true if they are equal, false otherwise
 */
bool tree_eq(btree t1, btree t2) {
  return tree_subset(t1, t2) && tree_subset(t2, t1);
}
/*
 * creates this tree:
 *   1
 *   3
 * 6
 *   7
 * 9
 *   13
 */
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

void test_eq(int trials) {
  btree t1 = new_node(true, true);
  btree t2 = new_node(true, true);
  int key;
  for (int i = 0; i < trials; i++) {
    key = rand() % 1000;
    insert_key(t1, key);
    assert(contains_key(t1, key));
    insert_key(t2, key);
    assert(contains_key(t2, key));
  }
  assert(tree_eq(t1, t2));
  insert_key(t1, 1001);
  assert(!tree_eq(t1, t2));
}

/*
 * simply return the number of search requests, so we can know the number to
 * expect, to test if the results are the same
 */
int num_searches(std::string filename) {
  std::ifstream file;
  std::string line;
  file.open(filename);
  int count = 0;
  if (file.is_open()){
    while (getline(file, line)) {
      if (line[0] == 's') {
        count++;
      }
    }
    file.close();
  } else {
    printf("Something went wrong... try again\n");
    assert(false);
  }
  return count;
}

void process_file(std::string filename, btree t, bool* results) {
  std::ifstream file;
  std::string line;
  file.open(filename);
  int searches = 0;
  bool result;
  if (file.is_open()){
    while (getline(file, line)) {
      switch(line[0]) {
        case 'i':
          insert_key(t, std::stoi(&line[2]));
          break;
        case 's':
          result = contains_key(t, std::stoi(&line[2]));
          results[searches] = result;
          searches++;
          break;
        default:
          printf("whaaat?\n");
      }
    }
    file.close();
  } else {
    printf("Something went wrong... try again\n");
  }
}

bool test_from_file(std::string filename) {
  btree t1 = new_node(true, true);
  btree t2 = new_node(true, true);
  int searches = num_searches(filename);
  bool* results1 = new bool[searches];
  bool* results2 = new bool[searches];
  printf("searching %d times...\n", searches);
  process_file(filename, t1, results1);
  process_file(filename, t2, results2);
  for (int i = 0; i < searches; i++) {
    if (results1[i] != results2[i]) {
      printf("results differ!\n");
      return false;
    }
  }
  return tree_eq(t1, t2);
}

int main() {
  test_eq(1000);
  char cmd[MAXWORDSIZE];		/* string to hold a command */
  char filename[MAXWORDSIZE];   /* string to hold a filename */
  std::ifstream file;
  int key;
  bool result;
  bool goOn = true;
  btree t = new_node(true, true);
  while(goOn) {
    printf("\n\t*** These are your commands .........\n");
    //printf("\t\"C\" to scan the tree\n");
    printf("\t\"i\" to insert\n");
    //printf("\t\"gte\" to search and print elements greater than or equal to the key\n");
    //printf("\t\"lte\" to search and print elements less than or equal to the key\n");
    printf("\t\"s\" to search, and print the key\n");
    printf("\t\"T\" to print the btree in inorder format\n");
    printf("\t\"t\" to test, reading input data from a file\n");
    printf("\t\"x\" to exit\n");
    scanf("%s", cmd);
    assert( strlen(cmd) < MAXWORDSIZE);
    switch( cmd[0]) {
    /*case 'C':
        printf("\n*** Scanning... \n");
        scanTree(printOcc);
        break;*/
    case 'i':
      printf("\nEnter key for insertion\n");
      scanf("%d", &key);
      printf("\n*** Inserting %d\n", key);
      insert_key(t,key);
      break;
    case 's':
      printf("enter search-key: ");
      scanf("%d", &key);
      printf("\n*** Searching for %d...", key);
      result = contains_key(t, key);
      printf("%d\n", result);
      break;
    /*case 'S':
        printf("enter search-word: ");
        scanf("%s", word);
        assert( strlen(word) < MAXWORDSIZE);
        printf("\n*** Searching for word %s \n", word);
        search(word, TRUE);
        printf("%d pages read\n", btReadCount);
        btReadCount = 0;
        break;
    case 'P':
        printf("enter search-prefix: ");
        scanf("%s", word);
        assert( strlen(word) < MAXWORDSIZE);
        printf("\n*** Searching for prefix %s \n", word);
        prefix_search(word, TRUE);
        printf("%d pages read\n", btReadCount);
        btReadCount = 0;
        break;
    case 'M':
        printf("enter search-substring: ");
        scanf("%s", word);
        assert( strlen(word) < MAXWORDSIZE);
        printf("\n*** Searching for substring %s \n", word);
        ss_search(word, TRUE);
        printf("%d pages read\n", btReadCount);
        btReadCount = 0;
        break;
    case 'p':
        printf("pagenumber=?\n");
        scanf("%s", cmd);
        assert( strlen(cmd) < MAXWORDSIZE);
        i = (PAGENO) atoi(cmd);
        printPage(i,fpbtree);
        break;
    */
    case 'T':
      printf("\n*** Printing tree in order .........\n");
      print_tree(t,0);
      break;
    case 't':
      printf("enter test file: ");
      scanf("%s", filename);
      assert(test_from_file(filename));
      printf("results matched!\n");
      break;
    case 'x':
      printf("\n*** Exiting .........\n");
      goOn = false;
      break;
    default:
      printf("\n*** Illegal command \"%s\"\n", cmd);
      break;
    }
  }
}
