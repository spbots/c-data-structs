#ifndef bst_h
#define bst_h

/*
a very very naive bst.

uses is_valid flag to allow bulk insert and remove without rebalancing

a node that is "removed" is marked as invalid and only removed on the
next call to bst_balance.

design notes & questions:
- are there "standard" ways to name functions like _alloc/_free?
- since most of the data will be on the heap it made sense to me
    to allocate the root on the heap - i.e. not encourage the
    user to put the root node on the stack.
- is it important to handle something like "namespace"? i.e.
    avoiding naming collisions using some sensible prefix (bst_)
- in functions like bst_balance, where the root node could change,
    is it more conventional to return the new node or to pass a
    pointer-to-pointer as the function arg?
*/
struct bst_node {
    struct bst_node_data {
        int data;
        int is_valid;
    } key;
    struct bst_node* l;
    struct bst_node* r;
};

struct bst_node* bst_alloc(int root_key);
void bst_free(struct bst_node* root);

int bst_has_value(const struct bst_node* root, const int key);

void bst_insert(struct bst_node* root, const int key);
void bst_remove(struct bst_node* root, const int key);

struct bst_node* bst_balance_a(struct bst_node* root);
void bst_balance_b(struct bst_node** root);


void bst_test();

#endif // bst_h
