#include <stdlib.h>
#include "vec_queue.h"


void my_vec_init(my_vec_t *v)
{

	//Note(walid): choosing init capacity value is very important, 
	v->capacity = 8;
	v->total = 0;
	v->items = (void**) malloc(sizeof(void *) * v->capacity);
}

static void my_vec_realocate(my_vec_t *v, int capacity)
{

	void **items = (void**) realloc(v->items, sizeof(void *) * capacity);
	if (items) {
		v->items = items;
		v->capacity = capacity;
	}
}

void
my_vec_add(my_vec_t *v, void *item)
{
	//Note: small realoc == more frequent reallocations == shitty performance  
	//Note: always even, no fractions  
	//Remember: doubling the capacity is recommended بالإجماع 
	if (v->capacity == v->total){
		my_vec_realocate(v, v->capacity * 2);
	}
	v->items[v->total++] = item;
}

void
my_vec_set(my_vec_t *v, int index, void *item)
{
	if (index >= 0 && index < v->total){
		v->items[index] = item;
	}
}

void*
my_vec_get(my_vec_t *v, int index)
{
	if (index >= 0 && index < v->total){
		return v->items[index];
	}
	
	return NULL;
}

int
my_vec_delete(my_vec_t *v, int index)
{
	if (index < 0 || index >= v->total){
		return 0;
	}
	
	v->items[index] = NULL;

	for (int i = index; i < v->total - 1; i++) {
		v->items[i] = v->items[i + 1];
		v->items[i + 1] = NULL;
	}
	
	v->total--;

	//Note: 2 -> 4 -> 6 -> 8 -> 10
	if (v->total > 0 && v->total == v->capacity / 4)
		my_vec_realocate(v, v->capacity / 2);

	return 1;
}

void*
my_vec_pop(my_vec_t *v)
{
	int last_index= v->total-1;
	void* last= my_vec_get(v ,v->total -1);
	if(my_vec_delete(v,last_index) == 0){
		return NULL;
	}
	
	return last;
}

void*
my_vec_get_tail(my_vec_t *v)
{
	void* tail;
	if((tail = my_vec_get(v, v->total-1)) == NULL) {
		return NULL;
	}

	return tail;
}



void*
my_vec_push(my_vec_t *v, void *item)
{
	my_vec_add(v, item);
	return item;
}


void
my_vec_free(my_vec_t *v)
{
    free(v->items);
}

