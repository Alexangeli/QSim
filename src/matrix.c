#include <stdlib.h>
#include <stdio.h>
#include "matrix.h"

/* Allocate memory for a matrix of given size */
matrix_t *matrix_alloc(int size) {
    matrix_t *m = malloc(sizeof(matrix_t));
    m->size = size;
    m->data = malloc(size * sizeof(complex_t *));
    for (int i = 0; i < size; i++) {
        m->data[i] = calloc(size, sizeof(complex_t));   // initialize all values to zero (0 + 0i)
    }
    return m;
}

/* Free the memory allocated for a matrix */
void matrix_free(matrix_t *m) {
    if (!m) return; // if m is NULL, nothing to free
    for (int i = 0; i < m->size; i++) {
        free(m->data[i]);   // free the array of complex numbers for each row
    }
    free(m->data); // free the array of pointers to rows
    free(m);    // free the matrix structure itself
}

/* Multiply rows for columns between two matrices 
   Standard row-by-column product: res[i][j] = sum_k( a[i][k] * b[k][j] )*/
matrix_t *matrix_multiply(matrix_t *a, matrix_t *b) {
    matrix_t *res = matrix_alloc(a->size);  // allocate result matrix (zero-initialized)
    for (int i = 0; i < a->size; i++) {
        for (int j = 0; j < a->size; j++) {
            for (int k = 0; k < a->size; k++) {
                res -> data[i][j] = complex_add(res->data[i][j], complex_mul(a->data[i][k], b->data[k][j]));
            }
        }
    }    

    return res;
}

/* Multiply a matrix by a vector */
complex_t *matrix_vec_mul(matrix_t *m, complex_t *v) {
    complex_t *res = calloc(m->size, sizeof(complex_t));
    for (int i = 0; i < m->size; i++) {
        for (int j = 0; j < m->size; j++) {
            // this calculates the dot product of the i-th row of the matrix with the vector v
            res[i] = complex_add(res[i], complex_mul(m->data[i][j], v[j]));
        }
    }
    return res;
}
  