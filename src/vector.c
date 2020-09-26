#include "../include/vector.h"

struct Vector*
CreateVector ( size_t item_size ) {
    struct Vector* vector;
    vector = malloc( sizeof( struct Vector ) );
    vector->size = 0;
    vector->array = calloc( 0, item_size );
}

void
FreeVector ( struct Vector* vector ) {
    for ( size_t i = 0; i < vector->size; i++ ) {
        free( vector->array[i] );
    }

    free( vector->array );
    free( vector );
}

void
VectorPushBack ( struct Vector* vector, void* element ) {
    vector->size++;
    vector->array = realloc( vector->array, vector->size * sizeof( element ) );
    vector->array[vector->size - 1] = element;
}


void
VectorErase ( struct Vector* vector, size_t element, int element_size ) {
    vector->size--;
    vector->array = realloc( vector->array, vector->size * element_size );
}