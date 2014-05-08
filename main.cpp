/*
 * main.c
 * the file to run tests/gather performance data
 */

#include <iostream>
#include <fstream>
#include <cstring>
#include <assert.h>
#include "btree.h"
#include "global_locked.h"
#include "def.h"
#include <sys/types.h>
#include <pthread.h>

/****************************** HELPERS *******************************/

typedef struct {
  int proc_id;
  int num_procs;
  lockable_tree t;
  std::string filename;
} proc_args;

/*
 * simple, calculate the number of lines in a file
 */
int num_lines(std::string filename) {
  std::ifstream file;
  std::string line;
  file.open(filename);
  int count = 0;
  if (file.is_open()){
    while (getline(file, line)) {
      count++;
    }
    file.close();
  } else {
    printf("Something went wrong... try again\n");
  }
  return count;
}

/*
 * basically just case on insert or search, and do the appropriate op
 */
void process_line(lockable_tree t, std::string line) {
  switch (line[0]) {
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

/*
 * for each pthread, process the part of the file that has to do with them
 */
void* process_file(void* args) {
  proc_args* p_args = (proc_args *)args;
  std::string filename = p_args->filename;
  lockable_tree t = p_args->t;
  int num_processors = p_args->num_procs;
  int proc_id = p_args->proc_id;
  std::ifstream file;
  std::string line;
  file.open(filename);
  int i = 0;
  if (file.is_open()){
    while (getline(file, line)) {
      if (i%num_processors == proc_id) {
        process_line(t, line);
      }
      i++;
    }
    file.close();
  } else {
    printf("Something went wrong... try again\n");
  }

}

/*
 * process the file with multiple processors, where each processor takes
 * the portion of lines that pertain to them
 */
void multi_process_file(std::string filename, lockable_tree t, int processors) {
  proc_args* args = new proc_args[processors];
  pthread_t* threads = new pthread_t[processors];
  pthread_attr_t pthread_custom_attr;

  for (int i = 0; i < processors; i++) {
    args[i].proc_id = i;
    args[i].num_procs = processors;
    args[i].t = t;
    args[i].filename = filename;
  }
  pthread_attr_init(&pthread_custom_attr);

  for (int i = 1; i < processors; i++) {
    pthread_create(&threads[i], &pthread_custom_attr, process_file, (void *)(args+i));
  }

  process_file((void *)(args));

  for (int i = 1; i < processors; i++) {
    pthread_join(threads[i], 0);
  }
}

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
  lockable_tree t2 = new_tree();
  process_file(filename, t1);
  multi_process_file(filename, t2, 4);
  return tree_eq(t1, t2->root);
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
