#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rbTree.h"

/**
 * Constants used for accessing left and right child of red black tree
 */
#define LEFT_CHILD 0
#define RIGHT_CHILD 1

// The global root of the red black tree!
rbtree_node *root;

/**
 * Returns pointer to uncle
 */
rbtree_node* uncle_node(rbtree_node *node) {
	// If no parent or grandparent, then no uncle
	if (node->parent == NULL || node->parent->parent == NULL)
		return NULL;
	
	// If the current node is a left chld, then uncle is on right, else left
	if (node->parent == node->parent->parent->children[LEFT_CHILD])
		return node->parent->parent->children[RIGHT_CHILD];
	else
		return node->parent->parent->children[LEFT_CHILD];
}

/**
 * Returns pointer to sibling
 */
rbtree_node* sibling_node(rbtree_node* node) {
	// No sibling if no parent
	if (node == NULL || node->parent == NULL)
		return NULL;
	
	if (node == node->parent->children[LEFT_CHILD])
		return node->parent->children[RIGHT_CHILD];
	else
		return node->parent->children[LEFT_CHILD];
}

/**
 * Moves node down and moves given node in its place
 */
void move_node_down(rbtree_node* node, rbtree_node* new_parent) {
  	if (node->parent != NULL) {
		if (node == node->parent->children[LEFT_CHILD])
			node->parent->children[LEFT_CHILD] = new_parent;
		else
			node->parent->children[RIGHT_CHILD] = new_parent;
	}
	new_parent->parent = node->parent;
	node->parent = new_parent;
}


int node_has_red_child(rbtree_node* node) {
	return ((node->children[LEFT_CHILD] != NULL && node->children[LEFT_CHILD]->red) || (node->children[RIGHT_CHILD] != NULL && node->children[RIGHT_CHILD]->red));
}

/**
 * Red black tree rotation function.
 * :param node: The reference node from which the rotation is to be made
 * :param direction: LEFT_CHILD or RIGHT_CHILD. Rotates in the corresponding direction
 */
void rotate_rbtree_nodes(rbtree_node* node, int direction) {
	rbtree_node *temp_node = node->children[(direction + 1) % 2];
	
	// Update root if current node is the root
	if (node == root)
		root = temp_node;

	move_node_down(node, temp_node);
	
	node->children[(direction + 1) % 2] = temp_node->children[direction];
	
	if (temp_node->children[direction] != NULL)
		temp_node->children[direction]->parent = node;

	temp_node->children[direction] = node;
}

void swap_colours_nodes(rbtree_node* node1, rbtree_node* node2) {
	int temp_red = node1->red;
	node1->red = node2->red;
	node2->red = temp_red;
}

void swap_values_nodes(rbtree_node* node1, rbtree_node* node2) {
	void *temp_ptr = node1->ptr;
	node1->ptr = node2->ptr;
	node2->ptr = temp_ptr;

	size_t temp_size = node1->size;
	node1->size = node2->size;
	node2->size = temp_size;

	int temp_free = node1->free;
	node1->free = node2->free;
	node2->free = temp_free;
}

/**
 * Fix red red at given node
 */
void fix_red_red_node(rbtree_node* node) {
	// If node is root, color it black and return
	if (node == root) {
		node->red = 0;
		return;
	}
	
	// Initialize parent, grandparent and uncle
	rbtree_node *parent = node->parent;
	rbtree_node *grand_parent = parent->parent;
	rbtree_node *uncle = uncle_node(node);

	if (parent->red != 0) {
		if (uncle != NULL && uncle->red == 1) {
			// Uncle red, perform recoloring and recurse
			parent->red = 0;
			uncle->red = 0;
			grand_parent->red = 1;
			fix_red_red_node(grand_parent);
		} 
		else {
			// else perform LR, LL, RL, RR
			if (parent == parent->parent->children[LEFT_CHILD]) {
				if (node == node->parent->children[LEFT_CHILD]) {
					// for left right
					swap_colours_nodes(parent, grand_parent);
				}
				else {
					rotate_rbtree_nodes(parent, LEFT_CHILD);
					swap_colours_nodes(node,grand_parent);
				}
				// for left left and left right
				rotate_rbtree_nodes(grand_parent, RIGHT_CHILD);
			}
			else {
				if (node == node->parent->children[LEFT_CHILD]) {
					// for right left
					rotate_rbtree_nodes(parent, RIGHT_CHILD);
					swap_colours_nodes(node, grand_parent);
				}
				else {
					swap_colours_nodes(parent, grand_parent);
				}
				// for right right and right left
				rotate_rbtree_nodes(grand_parent, LEFT_CHILD);
			}
		}
	}
}

/**
 * Find node without a left child in the subtree of the given node
 */
rbtree_node* successor_node(rbtree_node* node) {
	rbtree_node* temp_node = node;
	
	while(temp_node->children[LEFT_CHILD] != NULL)
		temp_node = temp_node->children[LEFT_CHILD];
 
	return temp_node;
}

/**
 * Find node that replaces a deleted node in BST
 */
rbtree_node* replace_BST_node(rbtree_node* node) {
	// When node has 2 children
	if (node->children[LEFT_CHILD] != NULL && node->children[RIGHT_CHILD] != NULL)
		return successor_node(node->children[RIGHT_CHILD]);

	// When leaf
	if (node->children[LEFT_CHILD] == NULL && node->children[RIGHT_CHILD] == NULL)
		return NULL;

	// When single child
	if (node->children[LEFT_CHILD] != NULL)
		return node->children[LEFT_CHILD];
	else
		return node->children[RIGHT_CHILD];
}

/**
 * Fix a double black for a given node
 */
void fix_rbtree_double_black(rbtree_node* node) {
	if (node == NULL)
		return;
	
	// return if reached root
	if (node == root)
		return;

	rbtree_node* sibling = sibling_node(node);
	rbtree_node* parent = node->parent;
	
	// If no sibling, double black pushed up, recurse for parent
	if (sibling == NULL)
		fix_rbtree_double_black(parent);
	else {
		// Red sibling
		if (sibling->red == 1) {
			parent->red = 1;
			sibling->red = 0;
			if (sibling == sibling->parent->children[LEFT_CHILD])
				rotate_rbtree_nodes(parent, RIGHT_CHILD);
			else
				rotate_rbtree_nodes(parent, LEFT_CHILD);
			fix_rbtree_double_black(node);
		}
		else {
			// If at least 1 red child
			if ((sibling->children[LEFT_CHILD] != NULL && sibling->children[LEFT_CHILD]->red == 1) || (sibling->children[RIGHT_CHILD] != NULL && sibling->children[RIGHT_CHILD]->red == 1)) {
				if (sibling->children[LEFT_CHILD] != NULL && sibling->children[LEFT_CHILD]->red == 1) {
					// If sibling is left child
					if (sibling == sibling->parent->children[LEFT_CHILD]) {
						// left left
			    		sibling->children[LEFT_CHILD]->red = sibling->red;
						sibling->red = parent->red;
						rotate_rbtree_nodes(parent, RIGHT_CHILD);
					}
					// If sibling is right child
					else {
						// right left
						sibling->children[LEFT_CHILD]->red = parent->red;
						rotate_rbtree_nodes(sibling, RIGHT_CHILD);
						rotate_rbtree_nodes(parent, LEFT_CHILD);
					}
				} else {
					if (sibling == sibling->parent->children[LEFT_CHILD]) {
						// left right
						sibling->children[RIGHT_CHILD]->red = parent->red;
						rotate_rbtree_nodes(sibling, LEFT_CHILD);
						rotate_rbtree_nodes(parent, RIGHT_CHILD);
					} else {
						// right right
						sibling->children[RIGHT_CHILD]->red = sibling->red;
						sibling->red = parent->red;
						rotate_rbtree_nodes(parent, LEFT_CHILD);
					}
				}
				parent->red = 0;
			} 
			// 2 black children
			else {
  				sibling->red = 1;
				if (parent->red == 0)
					fix_rbtree_double_black(parent);
				else
					parent->red = 0;
			}
		}
	}
}

/**
 * Deletes the given node from the red black tree
 */
void delete_rbtree_node(rbtree_node *node) {
	if (node == NULL) {
		fprintf(stderr, "Cannot delete a NULL node! Exiting...\n");
		exit(EXIT_FAILURE);
	}

	rbtree_node *temp_node = replace_BST_node(node);
	int both_black_nodes = 0;
	
	if ((temp_node == NULL || temp_node->red == 0) && (node->red == 0))
		both_black_nodes = 1;
	
	rbtree_node *parent = node->parent;

	if (temp_node == NULL) {
		// temp_node is NULL, therefore node is a leaf
		if (node == root) {
			// If the node to be deleted is root, make root NULL
			root = NULL;
		}
		else {
			if (both_black_nodes) {
				fix_rbtree_double_black(node);
			}
			else {
				rbtree_node* sibling = sibling_node(node);
				if (sibling != NULL)
					sibling->red = 1;
			}
			
			if (node->parent != NULL && parent != NULL) {
				if (node == node->parent->children[LEFT_CHILD])
					parent->children[LEFT_CHILD] = NULL;
				else
					parent->children[RIGHT_CHILD] = NULL;
			}
		}
		free(node);
		return;
	}
	
	if (node->children[LEFT_CHILD] == NULL || node->children[RIGHT_CHILD] == NULL) {
		// node has only 1 child
		if (node == root) {
			node->ptr = temp_node->ptr;
			node->size = temp_node->size;
			node->free = temp_node->free;
			node->children[LEFT_CHILD] = node->children[RIGHT_CHILD] = NULL;
			free(temp_node);
		}
		else {
			if (node->parent != NULL && parent != NULL) {
				if (node == node->parent->children[LEFT_CHILD])
					parent->children[LEFT_CHILD] = temp_node;
				else
					parent->children[RIGHT_CHILD] = temp_node;
			}
		
			free(node);
			temp_node->parent = parent;
			if (both_black_nodes)
				fix_rbtree_double_black(temp_node);
			else
				temp_node->red = 0;
		}
		return;
	}

	// If node has 2 children, swap values with successor and recurse
  	swap_values_nodes(temp_node, node);
	delete_rbtree_node(temp_node);
}

/**
 * Deletes a node which starts at ptr and fixes the tree for red black properties
 * :param ptr: The starting address of the node that is to be deleted
 */
void rbtree_delete_node(void *ptr) {
	rbtree_node* node_to_delete = rbtree_node_search(ptr);
	if (node_to_delete == NULL) {
		fprintf(stderr, "Trying to delete a node that doesn't exist! Exiting...\n");
		exit(EXIT_FAILURE);
	}

	delete_rbtree_node(node_to_delete);
}

rbtree_node* get_rbtree_root() {
	return root;
}

/**
 * The helper method used by the rbtree_point_search() method
 * :param ptr: The address that is to be searched
 * :param node: The root of the red black tree in which ptr is to be searched
 * :return: The RBNode that starts with ptr in the tree
 */
rbtree_node *node_search_helper(void *ptr, rbtree_node *node) {
	if (node == NULL) {
		return NULL;
	}
	else if (node->ptr == NULL) {
		fprintf(stderr, "Encountered a bad node\n");
		return NULL;
	}
	else if ((node->ptr == ptr)) {
		return node;
	}
	else if (node->ptr > ptr) {
		if (node->children[LEFT_CHILD] != NULL) {
			return node_search_helper(ptr, node->children[LEFT_CHILD]);
		}
	}
	else {
		if (node->children[RIGHT_CHILD] != NULL) {
			return node_search_helper(ptr, node->children[RIGHT_CHILD]);
		}
	}
	return NULL;
}

/**
 * Searches a node in the red black tree whose ptr 
 * 	matches the argument ptr provided.
 * :param ptr: The address that is to be searched
 * :return: The red black tree node that starts with ptr in the tree
 * 				or NULL if not found
 */
rbtree_node *rbtree_node_search(void *ptr) {
	if (root == NULL) {
		return NULL;
	}

	return node_search_helper(ptr, root);
}

/**
 * Creates a new red black tree node.
 * Defaults: 
 * 	free: 0, color: red
 * :param ptr: The starting address of the memory location allocated
 * :param size: The size of memory allocated
 * :return: A new RBNode
 */
rbtree_node* create_rbtree_node(void *ptr, size_t size) {
	rbtree_node *new_node = (rbtree_node *) malloc(sizeof(rbtree_node));
	
	new_node->ptr = ptr;
	new_node->size = size;
	new_node->free = 0;
	new_node->red = 1;
	new_node->parent = NULL;
	new_node->children[LEFT_CHILD] = NULL;
	new_node->children[RIGHT_CHILD] = NULL;

	return new_node;
}

rbtree_node *search_node(void *ptr) {
	rbtree_node *temp_node = root;
	while (temp_node != NULL) 
		if (ptr < temp_node->ptr)
			if (temp_node->children[LEFT_CHILD] == NULL)
				break;
			else
				temp_node = temp_node->children[LEFT_CHILD];
		else if (ptr == temp_node->ptr)
			break;
		else
			if (temp_node->children[RIGHT_CHILD] == NULL)
				break;
			else
				temp_node = temp_node->children[RIGHT_CHILD];

	return temp_node;
}

/**
 * Creates a new node for ptr and size and inserts it into the red black tree.
 * The tree follows all the red black properties and hence remains balanced.
 * The node is created only if no node pointing ptr in red black tree exists.
 * If such a node exists, but has been freed, it is just updated.
 * If a node which hasn't been freed exists, an error is returned.
 * :param ptr: The starting address of the memory allocated to be inserted in the tree
 * :param size: The size of the memory allocated
 */
void rbtree_insert(void *ptr, size_t size) {
	if (root == NULL) {
		rbtree_node *new_node = create_rbtree_node(ptr, size);
		new_node->red = 0;
		root = new_node;
	}
	else {
		rbtree_node* temp_node = search_node(ptr);
		if (temp_node->ptr == ptr)
			return;

		rbtree_node *new_node = create_rbtree_node(ptr, size);
		new_node->parent = temp_node;

		if (ptr < temp_node->ptr)
			temp_node->children[LEFT_CHILD] = new_node;
		else
			temp_node->children[RIGHT_CHILD] = new_node;

		fix_red_red_node(new_node);
	}
}

/**
 * The helper method used by the rbtree_interval_search() method
 * 		The inteval is defined by (ptr, ptr + size)
 * :param ptr: The address that is to be searched for in an interval
 * :param parent: The root of the red black tree
 * :return: The RBNode in the red black tree whose interval contains ptr
 */
rbtree_node* interval_search_helper(void *ptr, rbtree_node *root, int free) {
	if (root == NULL) {
		return NULL;
	}

	if (ptr >= root->ptr && (size_t)ptr <= ((size_t)root->ptr + root->size)) {
		if (root->free == free)
			return root;
		else
			return NULL;
	}
	else if (ptr < root->ptr) {
		return interval_search_helper(ptr, root->children[LEFT_CHILD], free);
	}
	else if ((size_t)ptr > ((size_t)root->ptr + root->size)) {
		return interval_search_helper(ptr, root->children[RIGHT_CHILD], free);
	}
	
	return NULL;
}

/**
 * Searches an allocated node in the red black tree whose interval 
 * 	contains the given ptr. Ignores the freed nodes.
 * 		The inteval is defined by (ptr, ptr + size)	// ptr refers to node->ptr
 * :param ptr: The address that is to be searched for in an interval
 * :return: The RBNode in the red black tree whose interval contains ptr
 */
rbtree_node *rbtree_interval_search(void *ptr, int free) {
	return interval_search_helper(ptr, root, free);
}

/**
 * The helper method used by the rbtree_range_search() method
 * 		The inteval is defined by (ptr, ptr + size)
 * :param ptr: The address that is to be searched for in an interval
 * :param size: The size of the interval pointed by ptr
 * :param root: The root of the red black tree
 * :return: The RBNode in the red black tree whose interval contains the interval (ptr, ptr + size)
 */
rbtree_node* range_search_helper(void *ptr, size_t size, rbtree_node *root) {
	if (root == NULL)
		return NULL;
	
	if (ptr < root->ptr && (size_t)root->ptr <= ((size_t)ptr + size) && root->free == 1) {
		return root;
	}
	else if (ptr < root->ptr) {
		return range_search_helper(ptr, size, root->children[LEFT_CHILD]);
	}
	else if (ptr > root->ptr) {
		return range_search_helper(ptr, size, root->children[RIGHT_CHILD]);
	}
	
	return NULL;
}

/**
 * Searches for a FREE node in the red black tree whose interval 
 * 	contains the interval (ptr, ptr + size).
 * 		The inteval is defined by (ptr, ptr + size)	// ptr refers to node->ptr
 * :param ptr: The starting address of the interval
 * :param size: The size of the interval pointed by ptr
 * :return: The RBNode in the red black tree whose interval contains the interval (ptr, ptr + size)
 */
rbtree_node *rbtree_range_search(void *ptr, size_t size) {
	return range_search_helper(ptr, size, root);
}

void rbtree_delete_in_range_helper(rbtree_node *node, void *ptr, size_t size) {
	if (node == NULL)
		return;
	
	if (ptr < node->ptr)
		rbtree_delete_in_range_helper(node->children[LEFT_CHILD], ptr, size);
	
	// Determine if node is right child or left
	int node_is_right = -1;		// If node is indeed a right child, this will be set to 1. if left, 0. if no parent, -1
	rbtree_node *parent_node = node->parent;
	if (parent_node != NULL) {
		if (node == parent_node->children[RIGHT_CHILD]) {
			node_is_right = 1;
		}
		else {
			node_is_right = 0;
		}
	}

	int deleted = 0;
	if (ptr < node->ptr && ((size_t)ptr + size) >= (size_t)node->ptr && node->free == 1) {
		deleted = 1;
		delete_rbtree_node(node);

		if (parent_node != NULL) {
			rbtree_delete_in_range_helper(parent_node->children[node_is_right], ptr, size);
		}
		else if (root != NULL) {
			rbtree_delete_in_range_helper(root, ptr, size);
		}
	}
	
	if (deleted == 0 && node != NULL && ((size_t)ptr + size) > (size_t)node->ptr)
		rbtree_delete_in_range_helper(node->children[RIGHT_CHILD], ptr, size);
}

void rbtree_delete_in_range(void *ptr, size_t size) {
	rbtree_delete_in_range_helper(root, ptr, size);
}

/**
 * The helper method used by rbtree_print
 * :param root: The root of the red black tree to be printed
 * :param depth: The recursive depth parameter to print the .'s
 */
void print_helper(rbtree_node *root, int depth, rbtree_node *gl_root) {
	if (root != NULL) {
		print_helper(root->children[LEFT_CHILD], depth + 1, gl_root);

		int curr_depth = depth;
		// printf("Current Depth is %d\n", curr_depth);
		while (curr_depth--) {
			printf(".");
		}
		// printf("red = %d, free = %d node at %p with ptr %p size %i, parent %p, and children %p %p\n", root->red, root->free, (void *)root, root->ptr, (int)root->size, (void *)root->parent, (void *)root->children[LEFT_CHILD], (void *)root->children[RIGHT_CHILD]);
		printf("%p - ptr: %ld R: %d, F: %d Children: %p %p\n", (void *)root, ((long)root->ptr - (long)gl_root->ptr) / 4, root->red, root->free, (void *)root->children[LEFT_CHILD], (void *)root->children[RIGHT_CHILD]);

		print_helper(root->children[RIGHT_CHILD], depth + 1, gl_root);
	}
}

/**
 * This method prints the red black tree rooted at global "root" param
 */
void rbtree_print() {
	// Get the left most node so that the addresses printed are relative and easier to make sense
	rbtree_node *temp = root;
	while (temp->children[LEFT_CHILD] != NULL) {
		temp = temp->children[LEFT_CHILD];
	}

	print_helper(root, 0, temp);
}

// Returns the number of black nodes in a subtree of the given node
// or -1 if it is not a red black tree.
int compute_black_height(rbtree_node* curr_node) {
    // For an empty subtree the answer is obvious
    if (curr_node == NULL)
        return 0; 
    // Computes the height for the left and right child recursively
    int leftHeight = compute_black_height(curr_node->children[LEFT_CHILD]);
    int rightHeight = compute_black_height(curr_node->children[RIGHT_CHILD]);
    int add = curr_node->red == 0 ? 1 : 0;
    // The current subtree is not a red black tree if and only if
    // one or more of current node's children is a root of an invalid tree
    // or they contain different number of black nodes on a path to a null node.
    if (leftHeight == -1 || rightHeight == -1 || leftHeight != rightHeight)
        return -1; 
    else
        return leftHeight + add;
}

/**
 * Checks the red black tree property of no red node having a red child/parent
 */
int check_no_two_adj_red_nodes(rbtree_node *curr_node) {
	if (curr_node == NULL) {
		return 1;
	}

	if (curr_node->red == 1) {
		if (curr_node->children[LEFT_CHILD] != NULL && curr_node->children[LEFT_CHILD]->red == 1) {
			return 0;
		}
		if (curr_node->children[RIGHT_CHILD] != NULL && curr_node->children[RIGHT_CHILD]->red == 1) {
			return 0;
		}
	}

	return check_no_two_adj_red_nodes(curr_node->children[LEFT_CHILD]) && check_no_two_adj_red_nodes(curr_node->children[RIGHT_CHILD]);
}

/**
 * Checks if the tree rooted at curr_node is a binary search tree
 */
int is_bst(rbtree_node *curr_node, rbtree_node *left_node, rbtree_node *right_node) {
	// Base condition 
    if (curr_node == NULL) 
        return 1; 
  
    // if left node exist then check it has 
    // correct data or not i.e. left node's data 
    // should be less than root's data 
    if (left_node != NULL && curr_node->ptr < left_node->ptr) 
        return 0; 
  
    // if right node exist then check it has 
    // correct data or not i.e. right node's data 
    // should be greater than root's data 
    if (right_node != NULL && curr_node->ptr > right_node->ptr) 
        return 0; 
  
    // check recursively for every node. 
    return is_bst(curr_node->children[LEFT_CHILD], left_node, curr_node) && 
           is_bst(curr_node->children[RIGHT_CHILD], curr_node, right_node);
}

/**
 * Returns 1 if red black tree properties are followed by the tree rooted at root, else 0
 * Rules:
 * 1) Every node has a color either red or black.
 * 2) Root of tree is always black.
 * 3) There are no two adjacent red nodes (A red node cannot have a red parent or red child).
 * 4) Every path from a node (including root) to any of its descendant NULL node has the same number of black nodes.
 */
int is_red_black_tree() {
	// Check BST
	if (is_bst(root, NULL, NULL) == 0) {
		fprintf(stderr, "The red-black tree is not a binary SEARCH tree.\n");
		return 0;
	}

	// Rule 2
	if (root->red == 1) {
		fprintf(stderr, "Root of a red-black tree must be Black.\n");
		return 0;
	}

	// Rule 3
	if (check_no_two_adj_red_nodes(root) == 0) {
		fprintf(stderr, "A red node cannot have a red child in a red-black tree.\n");
		return 0;
	}

	// Rule 4
	if (compute_black_height(root) == -1) {
		fprintf(stderr, "The black height of left subtree and right subtree must be the same in a red-black tree.\n");
		return 0;
	}

	return 1;
}
