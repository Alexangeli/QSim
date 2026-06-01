#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"
#include "thread.h"


/*
 * Prints an integer index as a binary string of n bits.
 *
 * Input:  j - index to print 
 *         n - number of bits 
 */
static void print_binary(int j, int n) {
    for (int bit = n - 1; bit >= 0; bit--)
        printf("%d", (j >> bit) & 1);
}

int main(int argc, char *argv[]) {

    char *state_file = NULL;
    char *circuit_file = NULL;
    int n_threads = 1;
    int opt;

    while ((opt = getopt(argc, argv, "s:c:t:")) != -1) {
        switch (opt) {
            case 's': state_file = optarg; break;
            case 'c': circuit_file = optarg; break;
            case 't': n_threads = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s -s <state_file> -c <circuit_file> -t <n_threads>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!state_file || !circuit_file || n_threads < 1) {
        fprintf(stderr, "Error: missing arguments or not valid.\n");
        exit(EXIT_FAILURE);
    }


    /* parse initial state  */
    init_state_t *s = parse_init_state(state_file);
    if (!s) {
        fprintf(stderr, "Error parsing initial state file: %s\n", state_file);
        exit(EXIT_FAILURE);
    }


    /* parse circuit */
    circuit_t *c = parse_circuit(circuit_file, s->n);
    if (!c) {
        fprintf(stderr, "Error parsing circuit file: %s\n", circuit_file);
        free_init_state(s);
        exit(EXIT_FAILURE);
    }

    int owned = 0;
    /* compute final unitary matrix */
    matrix_t *unitary = compute_unitary(c, n_threads, &owned);


    /* apply unitary to initial state */
    complex_t *v_fin = apply_unitary(unitary, s->state);

    int size = 1 << s->n;

    if (c->measure == 0) {
        /* no measurement: print final state */
        printf("[ ");
        for (int i = 0; i < size; i++) {
            printf("%.5f+i%.5f", v_fin[i].real, v_fin[i].imag);
            if (i < size - 1) printf(",  ");
        }
        printf(" ]\n");
    } else {
        /* measurement: sample N times and print distribution */

        int *counts = measure(v_fin, s->n, c->measure, n_threads);

        for (int i = 0; i < size; i++) {
            print_binary(i, s->n);
            printf(" @ %.5f\n", (double)counts[i] / c->measure);
        }
        free(counts);
    }
    

    free(v_fin);
    if (owned) matrix_free(unitary);
    free_init_state(s);
    free_circuit(c);
    return 0;

}
