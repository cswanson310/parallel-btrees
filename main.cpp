/*
 * main.c
 * the file to run tests/gather performance data
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>
#include "btree.h"
#include "def.h"

/****************************** HELPERS *******************************/

/*
 * takes a filename, reads the file and processes it line by line
 */
void process_file(std::string filename, btree t) {
  std::ifstream file;
  std::string line;
  file.open(filename);
  if (file.is_open()){
    while (getline(file, line)) {
      switch(line[0]) {
        case 'i':
          insert_key(t, std::stoi(&line[2]));
          break;
        case 's':
          contains_key(t, std::stoi(&line[2]));
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
  process_file(filename, t1);
  process_file(filename, t2);
  return tree_eq(t1, t2);
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
