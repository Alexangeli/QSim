#ifndef THREAD_H
#define THREAD_H

#include "matrix.h"
#include "complex.h"
#include "parser.h"

/*
 * Structure containing arguments for a matrix multiplication thread.
 *
 * a:      first matrix 
 * b:      second matrix 
 * result: result matrix, allocated by the thread 
 */
typedef struct {
    matrix_t *a;      /* first matrix */
    matrix_t *b;      /* second matrix */
    matrix_t *result; /* result matrix */
} thread_arg_t;

/*
 * Thread function that multiplies two matrices.
 *
 * Input:  arg - pointer to thread_arg_t 
 * Output: NULL
 */
void *thread_matrix_mul(void *arg);

/*
 * Computes the final matrix by multiplying all gates in parallel.
 *
 * Input:  c           - circuit 
 *         n_threads - number of threads 
 * Output: pointer to the final matrix 
 */
matrix_t *compute_unitary(circuit_t *c, int num_threads);

/*
 * Applies the final matrix to the initial state vector.
 *
 * Input:  m     - final matrix 
 *         state - initial state vector 
 *         n     - number of qubits 
 * Output: pointer to the final state vector 
 */
complex_t *apply_unitary(matrix_t *m, complex_t *state);


# endif