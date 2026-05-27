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
 *  owned: set to 1 if the returned matrix must be freed by caller,
 *                0 if it belongs to the circuit.
 * Input:  c           - circuit 
 *         n_threads - number of threads 
 * Output: pointer to the final matrix 
 */
matrix_t *compute_unitary(circuit_t *c, int n_threads, int *owned);

/*
 * Applies the final matrix to the initial state vector.
 *
 * Input:  m     - final matrix 
 *         state - initial state vector 
 *         n     - number of qubits 
 * Output: pointer to the final state vector 
 */
complex_t *apply_unitary(matrix_t *m, complex_t *state);


/*
 * Structure containing arguments for a measurement thread.
 *
 * v_fin:    final state vector 
 * size:     size of the state vector (2^n) 
 * counts:   array of counts, one per state
 * n_shots:  number of samples this thread must take 
 * seed:     random seed for this thread 
 */
typedef struct {
    complex_t *v_fin;
    int        size;
    int       *counts;
    int        n_shots;
    unsigned int seed;
} measure_arg_t;

/*
 * Performs the measurement of the final state vector N times using multiple threads.
 *
 * Input:  v_fin       - final state vector 
 *         n           - number of qubits 
 *         n_shots     - number of measurements 
 *         num_threads - number of threads 
 * Output: array of counts of size 2^n 
 */
int *measure(complex_t *v_fin, int n, int n_shots, int num_threads);

# endif