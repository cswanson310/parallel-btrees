#include <mutex>
#include "def.h"

lockable_tree new_tree();

void insert_key(lockable_tree t, int key);
bool contains_key(lockable_tree t, int key);

