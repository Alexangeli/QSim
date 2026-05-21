#include <stdlib.h>
#include "matrix.h"

/* Allocate memory for a matrix of given size */
matrix_t *matrix_alloc(int size) {
    matrix_t *m = malloc(sizeof(matrix_t));
    m->size = size;
    m->data = malloc(size * sizeof(complex_t *));
    for (int i = 0; i < size; i++) {
        m->data[i] = calloc(size * sizeof(complex_t));
    }
    return m;
}

/* Free the memory allocated for a matrix */
void matrix_free(matrix_t *m) {
    for (int i = 0; i < m->size; i++) {
        free(m->data[i]);
    }
    free(m->data);
    free(m);
}

/* Multiply rows for columns between two matrices */
matrix_t *matrix_multiply(matrix_t *a, matrix_t *b) {
    matrix_t *res = matrix_alloc(a->size);
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
    complex_t *res = calloc(m->size * sizeof(complex_t));
    for (int i = 0; i < m->size; i++) {
        for (int j = 0; j < m->size; j++) {
            res[i] = complex_add(res[i], complex_mul(m->data[i][j], v[j]));
        }
    }
    return res;
}
  