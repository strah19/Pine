#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

struct Vector
{
    size_t size;
    void **array;
};

extern struct Vector *CreateVector(size_t element_size);

extern void FreeVector(struct Vector *vector);

extern void VectorPushBack(struct Vector *vector, void *element);

extern void VectorErase(struct Vector *vector, size_t element, int element_size);

#endif // VECTOR_H