#include <sys/types.h>

typedef struct rbtree_node {
	struct rbtree_node *parent;
	struct rbtree_node *children[2];
	void *ptr;
	size_t size;
	int free;
	int red;
} rbtree_node;

void rbtree_insert(void *ptr, size_t size);
void rbtree_delete_node(void *ptr);

void rbtree_delete_in_range(void *ptr, size_t size);

rbtree_node *rbtree_node_search(void *ptr);
rbtree_node *rbtree_interval_search(void *ptr, int free);
rbtree_node *rbtree_range_search(void *ptr, size_t size);

// These are used just in the script testing red black tree
void rbtree_print();
int is_red_black_tree();
