#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

struct Vector
{
    size_t size;
    void **array;
    size_t element_size;
};

extern struct Vector *CreateVector(size_t element_size);

extern void FreeVector(struct Vector *vector);

extern void VectorPushBack(struct Vector *vector, void *element);

extern void PopVector(struct Vector *vector, size_t element);

extern void ClearVector(struct Vector* vector);

#endif // VECTOR_H