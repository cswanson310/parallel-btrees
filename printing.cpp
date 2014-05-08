#include <cstdio>
#include "def.h"
#include "btree.h"

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

