#include "dyn_array.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct dyn_array {
    int* data;
    int size;
    int capacity;
}; typedef struct dyn_array dyn_array_t;

dyn_array_t make_dyn_array();
void free_dyn_array(dyn_array_t a);

// insertion functions return non-zero if allocation occurred
int push_back(dyn_array_t* a, int x);
int insert_at(dyn_array_t* a, int index, int x);

// return last element and reduce size
int pop_back(dyn_array_t* a);
int at(dyn_array_t* a, int index);



dyn_array_t make_dyn_array() {
    dyn_array_t a;
    a.data = calloc(16, sizeof(int));
    a.size = 0;
    a.capacity = 16;
    return a;
}

void free_dyn_array(dyn_array_t a) {
    free(a.data);
}

static int check_growth(dyn_array_t* a) {
    const int need_realloc = a->size == a->capacity;
    if (need_realloc) {
        // see https://lokiastari.com/blog/2016/03/25/resizemaths/
        a->capacity *= 1.5;
        a->data = realloc(a->data, sizeof(int) * a->capacity);
    }
    return need_realloc;
}

int push_back(dyn_array_t* a, int x) {
    const int need_realloc = check_growth(a);
    a->data[a->size++] = x;
    return need_realloc;
}

int insert_at(dyn_array_t* a, int index, int x) {
    assert(index <= a->size);
    const int need_realloc = check_growth(a);
    memmove(a->data + index + 1, a->data + index, sizeof(int) * a->size - index);
    a->data[index] = x;
    a->size++;
    return need_realloc;
}

int pop_back(dyn_array_t* a) {
    assert(a->size > 0);
    return a->data[--a->size];
}

int at(dyn_array_t* a, int index) {
    assert(index < a->size && a->size > 0);
    return a->data[index];
}

void test_dyn_array() {
    dyn_array_t a = make_dyn_array();

    // simple push-pop
    assert(push_back(&a, 15) == 0);
    assert(a.size == 1);
    assert(at(&a, 0) == 15);
    assert(a.size == 1);
    assert(pop_back(&a) == 15);
    assert(a.size == 0);

    // insert in reverse order (always @ 0)
    assert(insert_at(&a, 0, 4) == 0);
    assert(insert_at(&a, 0, 3) == 0);
    assert(insert_at(&a, 0, 2) == 0);
    assert(insert_at(&a, 0, 1) == 0);
    assert(at(&a, 0) == 1);
    assert(at(&a, 1) == 2);
    assert(at(&a, 2) == 3);
    assert(at(&a, 3) == 4);

    free_dyn_array(a);
}
