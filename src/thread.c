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

matrix_t *compute_unitary(circuit_t *c, int n_threads, int *owned) {
    int k = c -> circ_len;
    
    /* allocate array of matrix pointers, one per gate */
    matrix_t **matrices = malloc(k * sizeof(matrix_t *));
    int *owned_arr = calloc(k, sizeof(int)); /* 1 = allocated by us, 0 = from circuit */

    for (int i = 0; i < k; i++) {
        matrices[i] = c->gates[c->circ[i]]->matrix;
        owned_arr[i] = 0; /* belongs to circuit */
    }

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
        int *new_owned = calloc(new_count, sizeof(int));

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
                new_owned[i+j] = 1; /* allocated by matrix_multiply */
            }

            i += batch;
        }

        /* carry over odd matrix */
        if (count % 2 == 1) {
            new_matrices[new_count - 1] = matrices[count - 1];
            new_owned[new_count - 1]    = owned[count - 1];
        }

        /*free intermediate matrices we own */
        for (int i = 0; i < count; i++) {
            if (owned_arr[i]) {
                matrix_free(matrices[i]);
            }
        }

        /* free old array but not the matrices themselves */
        free(matrices);
        free(owned_arr);
        matrices = new_matrices;
        owned_arr = new_owned;
        count    = new_count;

        free(threads);
        free(args);
    }

    /* matrices[0] is the result matrix */
    matrix_t *result = matrices[0];
    *owned = owned_arr[0];
    free(matrices);
    free(owned_arr);
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

/*
 * Thread function that performs n_shots measurements on the final state vector.
*
 * Input:  arg - pointer to measure_arg_t 
 * Output: NULL
 */

void *thread_measure(void *arg) {
    measure_arg_t *m = (measure_arg_t *)arg;

    /* compute probability distribution */
    double *probs = malloc(m->size * sizeof(double));
    for (int i = 0; i < m->size; i++) {
        probs[i] = m ->v_fin[i].real * m->v_fin[i].real +
                   m->v_fin[i].imag * m->v_fin[i].imag; 
    }

    /* perform n_shots samples */

    for (int s = 0; s < m -> n_shots; s++) {
        /*generate random number tween 0 and 1 */
        double r = (double)rand_r(&m->seed) / RAND_MAX;

        /* find which state r falls into */
        double cumulative = 0.0;
        for (int i = 0; m->n_shots; i++) {
            cumulative += probs[i];
            if (r <= cumulative) {
                m -> counts[i]++;
                break;
            }
        }
    }
    
    free(probs);
    return NULL;
}

/*
 * Performs N measurements on the final state vector using multiple threads.
 *
 * Input:  v_fin       - final state vector 
 *         n           - number of qubits 
 *         n_shots     - number of measurements 
 *         num_threads - number of threads 
 * Output: array of counts of size 2^n
 */

int *measure(complex_t *v_fin, int n, int n_shots, int n_threads) {
    int size = 1 << n;

    /* allocate global counts array */
    int *counts = calloc(size, sizeof(int));

    /* allocate per-thread counts arrays */
    int **thread_counts = malloc(n_threads * sizeof(int *));
    for (int i = 0; i < n_threads; i++) {
        thread_counts[i] = calloc(size, sizeof(int));
    }

    /*distribute n_shots among threads */
    int base_shots = n_shots / n_threads;
    int remainder = n_shots / n_threads;

    pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
    measure_arg_t *args = malloc(n_threads * sizeof(measure_arg_t));

    /* launch threads */
    for (int i = 0; i < n_threads; i++) {
        args[i].v_fin = v_fin;
        args[i].size = size;
        args[i].counts = thread_counts[i];
        args[i].n_shots = base_shots + (i < remainder ? 1 : 0); // to distribute the remainder
        args[i].seed = (unsigned int)(i + 1) * 12345; // to ensure the different seeds per thread
        pthread_create(&threads[i], NULL, thread_measure, &args[i]);
    }

    /* wait for all threads */
    for (int i = 0; i < n_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    /* sum counts from all threads */
    for (int i = 0; i < n_threads; i++) {
        for (int j = 0; j < size; j++){
            counts[j] += thread_counts[i][j];
        }
    }

    /* free arrays for every thread */
    for (int i = 0; i < n_threads; i++){
        free(thread_counts[i]);
    }

    free(thread_counts);
    free(threads);
    free(args);

    return counts;
}