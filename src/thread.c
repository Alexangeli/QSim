#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "thread.h"

/*
 * Thread function that multiplies two matrices a and b, storing result.
 *
 * Input:  arg - pointer to thread_arg_t 
 * Output: NULL
 */

void *thread_matrix_mul(void *arg) {
    thread_arg_t *t = (thread_arg_t*)arg;

    /*multiply a and b, and store in result*/
    t->result = matrix_multiply(t->a, t->b);
    return NULL;
}

/*
 * Computes the final unitary matrix by multiplying all gates using tree reduction.
 *
 * Uses up to n_threads threads at each level of the reduction.
 *
 * Input:  c           - parsed circuit 
 *         n_threads - max number of threads 
 * Output: pointer to the final matrix 
 */

matrix_t *compute_unitary(circuit_t *c, int n_threads) {
    int k = c -> circ_len;
    
    /* allocate array of matrix pointers, one per gate */
    matrix_t **matrices = malloc(k * sizeof(matrix_t *));
    for (int i = 0; i < k; i++)
        matrices[i] = c->gates[c->circ[i]]->matrix;

    /* current number of matrices at this level */
    int count = k;

    while (count > 1) {
        /* number of pairs to multiply */
        int pairs = count/2;

        /* number of threads to use at this level */
        int nthreads = pairs < n_threads ? pairs : n_threads;

        /*allocate threads and their arguments */
        pthread_t *threads = malloc(nthreads * sizeof(pthread_t));
        thread_arg_t * args = malloc(pairs * sizeof(thread_arg_t));

        /* allocate result array for this level */
        int new_count = pairs + (count % 2);
        matrix_t **new_matrices = malloc(new_count * sizeof(matrix_t *));

        /* assign pairs to args  */
        for (int i = 0; i < pairs; i++) {
            /* gates applied right to left: G1·G0, G3·G2 */
            args[i].a      = matrices[2*i + 1];
            args[i].b      = matrices[2*i];
            args[i].result = NULL;
        }
        /* launch threads in batches of nthreads */
        int i = 0;
        while(i < pairs) {
            /* number of threads to launch in this batch */
            int batch = (pairs - i) < nthreads ? (pairs - i) : nthreads;

            for (int j = 0; j < batch; j++) {
                pthread_create(&threads[j], NULL, thread_matrix_mul, &args[i + j]);
            }

            /* wait for batch to finish  */
            for (int j = 0; j < batch; j++) {
                pthread_join(threads[j], NULL);
            }

            /* store results */
            for (int j = 0; j < batch; j++){
                new_matrices[i + j] = args[i + j].result;
            }

            i += batch;
        }

        /* free old array but not the matrices themselves */
        free(matrices);
        matrices = new_matrices;
        count    = new_count;

        free(threads);
        free(args);
    }

    /* matrices[0] is the result matrix */
    matrix_t *result = matrices[0];
    free(matrices);
    return result;
}

/*
 * Applies the final unitary matrix to the initial state vector.
 *
 * Input:  m     - final matrix 
 *         state - initial state vector 
 *         n     - number of qubits 
 * Output: pointer to the final state vector 
 */
complex_t *apply_unitary(matrix_t *m, complex_t *state) {
    /* matrix_vec_mul already allocates the result */
    return matrix_vec_mul(m, state);
}
