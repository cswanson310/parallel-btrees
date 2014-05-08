#include <mutex>

struct root_lock {
  std::mutex lock;
  btree root;
};

typedef struct root_lock* lockable_tree;

lockable_tree new_tree();

void insert_key(lockable_tree t, int key);
bool contains_key(lockable_tree t, int key);

