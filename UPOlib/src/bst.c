/* vim: set tabstop=4 expandtab shiftwidth=4 softtabstop=4: */

/*
 * Copyright 2015 University of Piemonte Orientale, Computer Science Institute
 *
 * This file is part of UPOalglib.
 *
 * UPOalglib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * UPOalglib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with UPOalglib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bst_private.h"

/**** EXERCISE #1 - BEGIN of FUNDAMENTAL OPERATIONS ****/

upo_bst_t upo_bst_create(upo_bst_comparator_t key_cmp) {
    upo_bst_t tree = malloc(sizeof(struct upo_bst_s));
    if (tree == NULL) {
        perror("Unable to create a binary search tree");
        abort();
    }

    tree->root = NULL;
    tree->key_cmp = key_cmp;

    return tree;
}

void upo_bst_destroy(upo_bst_t tree, int destroy_data) {
    if (tree != NULL) {
        upo_bst_clear(tree, destroy_data);
        free(tree);
    }
}

void upo_bst_clear_impl(upo_bst_node_t *node, int destroy_data) {
    if (node != NULL) {
        upo_bst_clear_impl(node->left, destroy_data);
        upo_bst_clear_impl(node->right, destroy_data);

        if (destroy_data) {
            free(node->key);
            free(node->value);
        }

        free(node);
    }
}

void upo_bst_clear(upo_bst_t tree, int destroy_data) {
    if (tree != NULL) {
        upo_bst_clear_impl(tree->root, destroy_data);
        tree->root = NULL;
    }
}

upo_bst_node_t *upo_bst_put_impl(upo_bst_node_t *node, void *key, void *value, void **v_old, upo_bst_comparator_t key_cmp) {
    *v_old = NULL;

    if (node == NULL) {
        upo_bst_node_t *new_node = malloc(sizeof(struct upo_bst_node_s));

        if (new_node == NULL) {
            perror("Unable to create new node for binary search tree");
            exit(1);
        } else {
            new_node->key = key;
            new_node->value = value;
            new_node->left = NULL;
            new_node->right = NULL;
            node = new_node;
            return node;
        }
    } else {
        int cmp = key_cmp(key, node->key);
        if (cmp < 0) 
            node->left = upo_bst_put_impl(node->left, key, value, v_old, key_cmp);
        else if (cmp > 0)
            node->right = upo_bst_put_impl(node->right, key, value, v_old, key_cmp);
        else {
            *v_old = node->value;
            node->value = value;
        }

        return node;
    }
}

void *upo_bst_put(upo_bst_t tree, void *key, void *value) {
    if (tree == NULL || key == NULL || value == NULL) return NULL;

    void *v_old = NULL;
    tree->root = upo_bst_put_impl(tree->root, key, value, &v_old, tree->key_cmp);
    return v_old;
}

upo_bst_node_t *upo_bst_insert_impl(upo_bst_node_t *node, void *key, void *value, upo_bst_comparator_t key_cmp) {
    if (node == NULL) {
        upo_bst_node_t *new_node = malloc(sizeof(struct upo_bst_node_s));

        if (new_node == NULL) {
            perror("Unable to create new node for binary search tree");
            exit(1);
        } else {
            new_node->key = key;
            new_node->value = value;
            new_node->left = NULL;
            new_node->right = NULL;
            node = new_node;
            return node;
        }
    } else {
        int cmp = key_cmp(key, node->key);
        if (cmp < 0) node->left = upo_bst_insert_impl(node->left, key, value, key_cmp);
        else if (cmp > 0) node->right = upo_bst_insert_impl(node->right, key, value, key_cmp);

        return node;
    }
}

void upo_bst_insert(upo_bst_t tree, void *key, void *value) {
    if (tree != NULL)
        tree->root = upo_bst_insert_impl(tree->root, key, value, tree->key_cmp);
}

upo_bst_node_t *upo_bst_get_impl(upo_bst_node_t *node, const void *key, upo_bst_comparator_t key_cmp) {
    if (node == NULL) return NULL;

    int cmp = key_cmp(key, node->key);
    if (cmp < 0) 
        return upo_bst_get_impl(node->left, key, key_cmp);
    else if (cmp > 0) 
        return upo_bst_get_impl(node->right, key, key_cmp);
    else
        return node;
}

void *upo_bst_get(const upo_bst_t tree, const void *key) {
    if (tree == NULL || tree->root == NULL)
        return NULL;

    upo_bst_node_t *new_node = upo_bst_get_impl(tree->root, key, tree->key_cmp);
    if (tree->root != NULL) return new_node->value;
    else return NULL;
}

int upo_bst_contains(const upo_bst_t tree, const void *key) {
    if (tree == NULL) return 0;
    if (upo_bst_get_impl(tree->root, key, tree->key_cmp) != NULL) return 1;
    else return 0;
}

upo_bst_node_t *upo_bst_max_impl(upo_bst_node_t *node) {
    if (node == NULL) return NULL;
    else if (node->right != NULL)
        return upo_bst_max_impl(node->right);
    else return node;
}

upo_bst_node_t *upo_bst_delete_impl(upo_bst_node_t *node, const void *key, int destroy_data, upo_bst_comparator_t key_cmp) {
    if (node == NULL) return NULL;

    int cmp = key_cmp(key, node->key);
    if (cmp < 0) 
        node->left = upo_bst_delete_impl(node->left, key, destroy_data, key_cmp);
    else if (cmp > 0)
        node->right = upo_bst_delete_impl(node->right, key, destroy_data, key_cmp);
    else if (node->left != NULL && node->right != NULL) {
        upo_bst_node_t *m = upo_bst_max_impl(node->left);

        if (destroy_data) {
            free(node->key);
            free(node->value);
        }

        node->key = m->key;
        node->value = m->value;
        node->left = upo_bst_delete_impl(node->left, m->key, destroy_data, key_cmp);
        return node;
    } else {
        upo_bst_node_t *x = node;

        if (node->left != NULL)
            node = node->left;
        else
            node = node->right;

        if (destroy_data) {
            free(x->key);
            free(x->value);
        }
        free(x);
        return node;
    }

    return node;
}

void upo_bst_delete(upo_bst_t tree, const void *key, int destroy_data) {
    tree->root = upo_bst_delete_impl(tree->root, key, destroy_data, tree->key_cmp);
}

size_t upo_bst_size_impl(upo_bst_node_t *node) {
    if (node == NULL) return 0;
    return 1 + upo_bst_size_impl(node->left) + upo_bst_size_impl(node->right);
}

size_t upo_bst_size(const upo_bst_t tree) {
    if (tree == NULL) return 0;
    return upo_bst_size_impl(tree->root);
}

int upo_bst_height_isLeaf_impl(upo_bst_node_t *node) {
    if (node->left == NULL && node->right == NULL)
        return 1;
    return 0;
}

size_t upo_bst_height_impl(upo_bst_node_t *node) {
    if (node == NULL || upo_bst_height_isLeaf_impl(node)) return 0;
    int height_left = upo_bst_height_impl(node->left);
    int height_right = upo_bst_height_impl(node->right);
    return 1 + (height_left > height_right ? height_left : height_right);
}

size_t upo_bst_height(const upo_bst_t tree) {
    return upo_bst_height_impl(tree->root);
}

void upo_bst_traverse_in_order_impl(upo_bst_node_t *node, upo_bst_visitor_t visit, void *visit_context) {
    if (node != NULL) {
        upo_bst_traverse_in_order_impl(node->left, visit, visit_context);
        visit(node->key, node->value, visit_context);
        upo_bst_traverse_in_order_impl(node->right, visit, visit_context);
    }
}

void upo_bst_traverse_in_order(const upo_bst_t tree, upo_bst_visitor_t visit, void *visit_context) {
    if (tree != NULL)
        upo_bst_traverse_in_order_impl(tree->root, visit, visit_context);
}

int upo_bst_is_empty(const upo_bst_t tree) {
    if (tree == NULL || tree->root == NULL) return 1;
    else return 0;
}

/**** EXERCISE #1 - END of FUNDAMENTAL OPERATIONS ****/

/**** EXERCISE #2 - BEGIN of EXTRA OPERATIONS ****/

/**
 * @brief Due to how a tree works, the minimum value will always
 *        reside in the most left side of the tree.
 * 
 * @param tree 
 * @return void* The smallest key
 */
void *upo_bst_min(const upo_bst_t tree) {
    if (tree == NULL || tree->root == NULL)
        return NULL;

    upo_bst_node_t *current = tree->root;
    while (current->left != NULL)
        current = current->left;

    return current->key;
}

/**
 * @brief Due to how a tree works, the maximum value will always
 *        reside in the most right side of the tree.
 *  
 * @param tree 
 * @return void* The biggest key
 */
void *upo_bst_max(const upo_bst_t tree) {
    if (tree == NULL || tree->root == NULL)
        return NULL;

    upo_bst_node_t *current = tree->root;
    while (current->right != NULL)
        current = current->right;

    return current->key;
}

/**
 * @brief Removes the node with the smallest key
 *        Since you want the smallest key, you have to iterate
 *        through all the nodes on the left. Once reached the
 *        smallest node or leaf, you make the right node (since
 *        after deletion it's now the smallest) the left node
 * 
 * @param tree 
 * @param destroy_data 
 */
void upo_bst_delete_min(upo_bst_t tree, int destroy_data) {
    if (tree != NULL && tree->root != NULL) {
        upo_bst_node_t *current = tree->root;
        upo_bst_node_t *parent = NULL;

        while (current->left != NULL) {
            parent = current;
            current = current->left;
        }

        if (parent != NULL) 
            parent->left = current->right;
        else
            tree->root = current->right;

        if (destroy_data) {
            free(current->key);
            free(current->value);
        }
        free(current);
    }
}

/**
 * @brief Removes the node with the biggest key
 *        Since you want the biggest key, you have to iterate
 *        through all the nodes on the right. Once reached the
 *        biggest node or leaf, you make the left node (since
 *        after deletion it's now the biggest) the right node
 * 
 * @param tree 
 * @param destroy_data 
 */
void upo_bst_delete_max(upo_bst_t tree, int destroy_data) {
    if (tree != NULL && tree->root != NULL) {
        upo_bst_node_t *current = tree->root;
        upo_bst_node_t *parent = NULL;

        while (current->right != NULL) {
            parent = current;
            current = current->right;
        }

        if (parent != NULL) 
            parent->right = current->left;
        else
            tree->root = current->left;

        if (destroy_data) {
            free(current->value);
        }
        free(current);
    }
}

/**
 * @param tree 
 * @param key 
 * @return void*    The biggest value <= key
 */
void *upo_bst_floor(const upo_bst_t tree, const void *key) {
    if (tree == NULL || tree->root == NULL || key == NULL)
        return NULL;

    upo_bst_node_t *current = tree->root;
    upo_bst_node_t *floor_node = NULL;

    while (current != NULL) {
        int cmp = tree->key_cmp(key, current->key);
        if (cmp == 0)
            return current->key;
        else if (cmp < 0)
            current = current->left;
        else {
            floor_node = current;
            current = current->right;
        }
    }

    if (floor_node != NULL)
        return floor_node->key;
    else
        return NULL;
}

/**
 * @param tree 
 * @param key 
 * @return void*  The smallest value >= k
 */
void *upo_bst_ceiling(const upo_bst_t tree, const void *key) {
    if (tree == NULL || tree->root == NULL || key == NULL)
        return NULL;

    upo_bst_node_t *current = tree->root;
    upo_bst_node_t *ceiling_node = NULL;

    while (current != NULL) {
        int cmp = tree->key_cmp(key, current->key);
        if (cmp == 0)
            return current->key;
        else if (cmp > 0)
            current = current->right;
        else {
            ceiling_node = current;
            current = current->left;
        }
    }

    if (ceiling_node != NULL)
        return ceiling_node->key;
    else
        return NULL;
}

/**
 * @brief Recursive call where the first left node gets visited,
 *        then compared to the low and high key. If it is included
 *        in the interval, create a new list node and add it to the list.
 *        Visits afterwards the right node.
 * 
 * @param node 
 * @param low_key 
 * @param high_key 
 * @param list 
 * @param key_cmp 
 */
void upo_bst_keys_range_impl(const upo_bst_node_t *node, const void *low_key, const void *high_key, upo_bst_key_list_t *list, upo_bst_comparator_t key_cmp) {
    if (node != NULL) {
        upo_bst_keys_range_impl(node->left, low_key, high_key, list, key_cmp);

        if (key_cmp(node->key, low_key) >= 0 && key_cmp(node->key, high_key) <= 0) {
            upo_bst_key_list_node_t *new_node = malloc(sizeof(struct upo_bst_key_list_node_s));
            new_node->key = node->key;
            new_node->next = *list;
            *list = new_node;
        }

        upo_bst_keys_range_impl(node->right, low_key, high_key, list, key_cmp);
    }
}

upo_bst_key_list_t upo_bst_keys_range(const upo_bst_t tree, const void *low_key, const void *high_key) {
    if (tree != NULL) {
        upo_bst_key_list_t list = NULL;
        upo_bst_keys_range_impl(tree->root, low_key, high_key, &list, tree->key_cmp);
        return list;
    }

    return NULL;
}

void upo_bst_keys_impl(const upo_bst_node_t *node, upo_bst_comparator_t key_cmp, upo_bst_key_list_t *list) {
    if (node != NULL) {
        upo_bst_keys_impl(node->left, key_cmp, list);

        upo_bst_key_list_node_t *list_node = malloc(sizeof(struct upo_bst_key_list_node_s));
        if (list_node == NULL) {
            perror("Unable to allocate memory for a new node of the key list");
            abort();
        }

        list_node->key = node->key;
        list_node->next = *list;
        *list = list_node;

        upo_bst_keys_impl(node->right, key_cmp, list);
    }
}

upo_bst_key_list_t upo_bst_keys(const upo_bst_t tree) {
    if (tree != NULL) {
        upo_bst_key_list_t list = NULL;
        upo_bst_keys_impl(tree->root, tree->key_cmp, &list);
        return list;
    }
    
    return NULL; 
}

int upo_bst_is_bst_impl(
    const upo_bst_node_t *node, 
    const void *min_key, 
    int is_min_key_changed, 
    const void *max_key, 
    int is_max_key_changed, 
    upo_bst_comparator_t key_cmp
) {
    if (node == NULL) return 1;

    if (
        key_cmp(node->key, min_key) < 0 ||
        (is_min_key_changed && key_cmp(node->key, min_key) == 0) ||
        key_cmp(node->key, max_key) > 0 ||
        (is_max_key_changed && key_cmp(node->key, max_key) == 0)
    ) return 0;

    return upo_bst_is_bst_impl(node->left, min_key, is_min_key_changed, node->key, 1, key_cmp) &&
            upo_bst_is_bst_impl(node->right, node->key, 1, max_key, is_max_key_changed, key_cmp);
}

int upo_bst_is_bst(const upo_bst_t tree, const void *min_key, const void *max_key) {
    if (upo_bst_is_empty(tree)) return 1;
    return upo_bst_is_bst_impl(tree->root, min_key, 0, max_key, 0, tree->key_cmp);
}

/**** EXERCISE #2 - END of EXTRA OPERATIONS ****/

upo_bst_comparator_t upo_bst_get_comparator(const upo_bst_t tree) {
    if (tree == NULL) {
        return NULL;
    }

    return tree->key_cmp;
}

size_t upo_bst_subtree_size_impl(const upo_bst_node_t *node, const void *key, int is_subtree, upo_bst_comparator_t cmp) {
    if(node == NULL) return 0;

    if(cmp(key, node->key) == 0) {
        is_subtree = 1;
        return is_subtree + 
            upo_bst_subtree_size_impl(node->left, key, is_subtree, cmp) + 
            upo_bst_subtree_size_impl(node->right, key, is_subtree, cmp);
    } else if(cmp(key, node->key) > 0 && !is_subtree) {
        return is_subtree + 
            upo_bst_subtree_size_impl(node->right, key, is_subtree, cmp);
    } else if(cmp(key, node->key) < 0 && !is_subtree) {
        return is_subtree + 
            upo_bst_subtree_size_impl(node->left, key, is_subtree, cmp);
    } else {
        return is_subtree + 
            upo_bst_subtree_size_impl(node->left, key, is_subtree, cmp) + 
            upo_bst_subtree_size_impl(node->right, key, is_subtree, cmp);
    }
    
    return 0;
}

size_t upo_bst_subtree_size(const upo_bst_t tree, const void *key) {
    if(tree == NULL) return 0;
    if(tree->root == NULL) return 0;

    return upo_bst_subtree_size_impl(tree->root, key, 0, tree->key_cmp);
}

/*** START of MORE EXERCISES ***/

size_t upo_bst_rank_impl(const upo_bst_node_t *node, const void *key, upo_bst_comparator_t key_cmp) {
    if (node == NULL) return 0;

    if (key_cmp(node->key, key) < 0)
        return 1 + upo_bst_rank_impl(node->left, key, key_cmp) + upo_bst_rank_impl(node->right, key, key_cmp);

    return upo_bst_rank_impl(node->left, key, key_cmp) + upo_bst_rank_impl(node->right, key, key_cmp);
}

size_t upo_bst_rank(const upo_bst_t bst, const void *key) {
    return upo_bst_rank_impl(bst->root, key, bst->key_cmp);
}

static void *upo_bst_predeccessor_impl(const upo_bst_node_t *node, const void *key, upo_bst_comparator_t cmp) {
    if (!node) return NULL; 
    if (cmp(key, node->key) <= 0) return upo_bst_predeccessor_impl(node->left, key, cmp);
    else {
        const void *right = upo_bst_predeccessor_impl(node->right, key, cmp);
        return right ? right : node->key;
    }
}

const void *upo_bst_predecessor(const upo_bst_t bst, const void *key) {
    if (bst == NULL || bst->root == NULL) return NULL;
    return upo_bst_predeccessor_impl(bst->root, key, upo_bst_get_comparator(bst));
}

void *upo_bst_get_value_depth_impl(const upo_bst_node_t *node, const void *key, long *depth, upo_bst_comparator_t key_cmp) {
    if (node == NULL) {
        *depth = -1;
        return NULL;
    }

    if (key_cmp(node->key, key) > 0) {
        (*depth)++;
        return upo_bst_get_value_depth_impl(node->left, key, depth, key_cmp);
    } else if (key_cmp(node->key, key) < 0) {
        (*depth)++;
        return upo_bst_get_value_depth_impl(node->right, key, depth, key_cmp);
    } else
        return node->key;
}

void *upo_bst_get_value_depth(const upo_bst_t bst, const void *key, long *depth) {
    if (bst == NULL) {
        *depth = -1;
        return NULL;
    }

    *depth = 0;
    return upo_bst_get_value_depth_impl(bst->root, key, depth, bst->key_cmp);
}

void upo_bst_keys_le_impl(upo_bst_node_t *node, const void *key, upo_bst_key_list_t *list, upo_bst_comparator_t key_cmp) {
    if (node != NULL) {
        if (key_cmp(node->key, key) <= 0) {
            upo_bst_key_list_node_t *list_node = malloc(sizeof(upo_bst_key_list_node_t));
            list_node->key = node->key;
            list_node->next = *list;
            *list = list_node;
            upo_bst_keys_le_impl(node->right, key, list, key_cmp);
        }

        upo_bst_keys_le_impl(node->left, key, list, key_cmp);
    }
}

upo_bst_key_list_t upo_bst_keys_le(const upo_bst_t bst, const void *key) {
    upo_bst_key_list_t list = NULL;
    upo_bst_keys_le_impl(bst->root, key, &list, bst->key_cmp);
    return list;
}

size_t upo_bst_subtree_count_leaves_depth(const upo_bst_t bst, const void *key, size_t d) {
    
}

