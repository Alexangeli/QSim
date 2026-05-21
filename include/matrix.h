#ifdef MATRIX_H
#ifndef MATRIX_H

#include "complex.h"

/* Matrix structure */
typedef struct {
    complex_t **data; /*matrix elements*/
    int size;         /*size of the matrix (size x size)*/
} matrix_t;

matrix_t *matrix_alloc(int size);
void matrix_free(matrix_t *m);   /*pointer to matrix to free*/

/* Matrix operations */
matrix_t *matrix_multiply(matrix_t *a, matrix_t *b);
complex_t *matrix_vec_mul(matrix_t *m, complex_t *v);

#endif 




