#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

struct Vector {
    size_t size;
    void **array;
    size_t element_size;
};

extern struct Vector *create_vector(size_t element_size);

extern void free_vector(struct Vector *vector);

extern void push_back_vector(struct Vector *vector, void *element);

extern void pop_vector(struct Vector *vector, size_t element);

extern void clear_vector(struct Vector* vector);

#endif // VECTOR_H