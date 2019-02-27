#include "bst.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct bst_node node_t;
typedef struct bst_node_data node_data_t;

// -------------------
// ------helpers------
// -------------------
static int num_nodes(node_t* node) {
    int n = 0;
    if (node) {
        n = 1 + num_nodes(node->l) + num_nodes(node->r);
    }
    return n;
}

static const node_t* find_node(const node_t* root, const int key) {
    if (!root || root->key.data == key) {
        return root;
    } else if (key < root->key.data) {
        return find_node(root->l, key);
    } else if (key > root->key.data) {
        return find_node(root->r, key);
    }
    assert(0);
}

// ----------------
// ------impl------
// ----------------
node_t* bst_alloc(const int root_key) {
    node_t* new_node = malloc(sizeof(node_t));
    new_node->key.data = root_key;
    new_node->key.is_valid = 1;
    new_node->l = NULL;
    new_node->r = NULL;
    return new_node;
}

void bst_free(node_t* root) {
    if (root) {
        bst_free(root->l);
        bst_free(root->r);
        free(root);
    }
}

int bst_has_value(const node_t* root, const int key) {
    return !!find_node(root, key);
}

void bst_insert(node_t* root, const int key) {
    assert(root && root->key.data != key);
    if (key < root->key.data) {
        if (root->l) {
            bst_insert(root->l, key);
        } else {
            root->l = bst_alloc(key);
        }
    } else {
        if (root->r) {
            bst_insert(root->r, key);
        } else {
            root->r = bst_alloc(key);
        }
    }
}

void bst_remove(node_t* root, const int key) {
    node_t* n = (node_t*)find_node(root, key);
    if (n) { n->key.is_valid = 0; }
}

/*
incoming node is node d.
returns new root node b.

     d           b
    / \         / \
   b   (e) => (a)  d
  / \             / \
(a) (c)         (c) (e)
*/
static void right_rotate(node_t* node) {
    if (node && node->l) {
        node_t* a = node->l->l;
        node_t* b = node->l;
        node_t* c = node->l->r;
        node_t* d = node;
        node_t* e = node->r;

        { // swap b/d keys. can't swap nodes since they may have a parent.
            node_data_t tmp = b->key;
            node_t* tmp_n = b;
            b->key = d->key, b = d;
            d->key = tmp,    d = tmp_n;
        }

        b->l = a, b->r = d;
        d->l = c, d->r = e;
    }
}

/*
incoming node is node b.
returns new root node d.

     b           d
    / \         / \
  (a)  d   =>  b  (e)
      / \     / \
    (c) (e) (a) (c)
*/
static void left_rotate(node_t* node) {
    if (node && node->r) {
        node_t* a = node->l;
        node_t* b = node;
        node_t* c = node->r->l;
        node_t* d = node->r;
        node_t* e = node->r->r;

        {
            node_data_t tmp = b->key;
            node_t* tmp_n = b;
            b->key = d->key, b = d;
            d->key = tmp,    d = tmp_n;
        }

        b->l = a, b->r = c;
        d->l = b, d->r = e;
    }
}

// create a "backbone" (no left children)
static void make_backbone(node_t* node) {
    while(node) {
        while(node->l) {
            right_rotate(node);
        }
        node = node->r;
    }
}

// remove and free any nodes with null keys.
// ... can change the root node, which is why it returns.
// ... very branchy (2 per node), yuck.
// ... could be part of building the backbone?
static node_t* remove_invalid_nodes_from_backbone(node_t* root) {
    node_t* parent = NULL;
    node_t* child = root;
    root = NULL;

    while (child) {
        assert(!child->l);
        if (!child->key.is_valid) {
            node_t* node_to_free = child;
            child = child->r;
            if (parent) { parent->r = child; }
            free(node_to_free);
        } else {
            parent = child;
            child = child->r;
            if (!root) { root = parent; }
        }
    }
    return root;
}

// return the new root
node_t* bst_balance_a(node_t* root) {
    // Day-Stout-Warren algorithm, see
    // http://courses.cs.vt.edu/
    //      cs2604/spring05/mcpherson/note/BalancingTrees.pdf
    make_backbone(root);
    root = remove_invalid_nodes_from_backbone(root);

    const int n_nodes = num_nodes(root);
    int m = (int)pow(2.0, floor(log2(n_nodes + 1.0))) - 1;

    node_t* n = root;
    for (int i = 0; i < (n_nodes - m); ++i) {
        left_rotate(n);
        n = n->r;
    }

    while (m > 1) {
        m /= 2, n = root;
        for (int i = 0; i < m; ++i) {
            left_rotate(n);
            n = n->r;
        }
    }
    return root;
}

void bst_balance_b(node_t** root) {
    assert(root);
    *root = bst_balance_a(*root);
}

// -----------------
// ------tests------
// -----------------
// helper for making a tree with a char at each node
// requires the input to be zero-terminated.
static node_t* make_alnum_tree(const char* str) {
    node_t* root = bst_alloc(str[0]);
    for (int i = 1; str[i] != '\0'; ++i) {
        bst_insert(root, str[i]);
    }
    return root;
}

static void test_rotate() {
    node_t* root = make_alnum_tree("dbeac");

    right_rotate(root);
    assert(root->key.data == 'b');
    assert(root->l->key.data == 'a');
    assert(root->r->key.data == 'd');
    assert(root->r->l->key.data == 'c');
    assert(root->r->r->key.data == 'e');

    left_rotate(root);
    assert(root->key.data == 'd');
    assert(root->l->key.data == 'b');
    assert(root->r->key.data == 'e');
    assert(root->l->l->key.data == 'a');
    assert(root->l->r->key.data == 'c');

    bst_free(root);
}

static int backbone_node_value(node_t* node, int depth) {
    node_t* n = node;
    for (int i = 0; i < depth; ++i) { n = n->r; }
    return n->key.data;
}

static void test_backbone() {
    node_t* root = make_alnum_tree("dhbfeigjac");
    assert(num_nodes(root) == 10);

    make_backbone(root);
    assert(root->key.data == 'a');
    assert(root->r->key.data == 'b');
    assert(root->r->r->key.data == 'c');
    assert(root->r->r->r->key.data == 'd');
    assert(root->r->r->r->r->key.data == 'e');

    bst_remove(root, 'a'); assert(!find_node(root, 'a')->key.is_valid);
    bst_remove(root, 'b'); assert(!find_node(root, 'b')->key.is_valid);
    bst_remove(root, 'e'); assert(!find_node(root, 'e')->key.is_valid);
    bst_remove(root, 'f'); assert(!find_node(root, 'f')->key.is_valid);
    bst_remove(root, 'j'); assert(!find_node(root, 'j')->key.is_valid);
    root = remove_invalid_nodes_from_backbone(root);
    assert(backbone_node_value(root, 0) == 'c');
    assert(backbone_node_value(root, 1) == 'd');
    assert(backbone_node_value(root, 2) == 'g');
    assert(backbone_node_value(root, 3) == 'h');
    assert(backbone_node_value(root, 4) == 'i');

    bst_free(root);
}

static void test_balance() {
    assert(bst_balance_a(NULL) == NULL);

    { // simple case: 7 nodes, perfect balance.
        node_t* root = make_alnum_tree("dbeacfg");
        bst_balance_b(&root);

        assert(root->key.data == 'd');
        assert(root->l->key.data == 'b');
        assert(root->r->key.data == 'f');
        assert(root->l->l->key.data == 'a');
        assert(root->l->r->key.data == 'c');
        assert(root->r->l->key.data == 'e');
        assert(root->r->r->key.data == 'g');

        bst_free(root);
    }
    { // more complex case: 9 nodes
        node_t* root = make_alnum_tree("dbeaihcfg");
        bst_balance_b(&root);

        assert(root->key.data == 'f');
        assert(root->l->key.data == 'd');
        assert(root->r->key.data == 'h');
        assert(root->l->l->key.data == 'b');
        assert(root->l->r->key.data == 'e');
        assert(root->r->l->key.data == 'g');
        assert(root->r->r->key.data == 'i');
        assert(root->l->l->l->key.data == 'a');
        assert(root->l->l->r->key.data == 'c');

        bst_free(root);
    }
    { // more complex case: 8 nodes after deletions
        node_t* root = make_alnum_tree("dbeoaixhcfg");
        bst_remove(root, 'a');
        bst_remove(root, 'x');
        bst_remove(root, 'o');
        bst_balance_b(&root);

        assert(root->key.data == 'f');
        assert(root->l->key.data == 'd');
        assert(root->r->key.data == 'h');
        assert(root->l->l->key.data == 'c');
        assert(root->l->r->key.data == 'e');
        assert(root->r->l->key.data == 'g');
        assert(root->r->r->key.data == 'i');
        assert(root->l->l->l->key.data == 'b');

        bst_free(root);
    }
}

void bst_test() {
    test_rotate();
    test_backbone();
    test_balance();
}
