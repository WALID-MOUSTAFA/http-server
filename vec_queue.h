#ifndef _VEC_QUEUE_H
#define _VEC_QUEUE_H

/********************** dynamic vector / dynamic queue *************/

/************* declatations ***************/

typedef struct {
    void **items;
    int capacity;
    int total;
} my_vec_t;

void my_vec_init(my_vec_t *);

static void my_vec_realocate(my_vec_t *, int capacity);

void my_vec_add(my_vec_t *v, void *);

void my_vec_set(my_vec_t *v, int, void *);

void *my_vec_get(my_vec_t *v, int);

int my_vec_delete(my_vec_t *v, int);

void my_vec_free(my_vec_t *v);

void* my_vec_pop(my_vec_t *v);

void* my_vec_get_tail(my_vec_t *v);

/********** implmentations ***************/

#endif
