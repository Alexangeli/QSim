#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "parser.h"


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
    if (!s) { fprintf(stderr, "Errore parsing stato iniziale\n"); exit(EXIT_FAILURE); }

    /* parse circuit */
    circuit_t *c = parse_circuit(circuit_file, s->n);
    if (!c) { fprintf(stderr, "Errore parsing circuito\n"); exit(EXIT_FAILURE); }


    printf("qubits: %d\n", s->n);
    printf("state vector (%d elements):\n", 1 << s->n);
    /*for (int i = 0; i < (1 << s->n); i++)
        printf("  [%d] %f + i%f\n", i, s->state[i].real, s->state[i].imag);*/

    printf("\ngates defined: %d\n", c->num_gates);
    for (int i = 0; i < c->num_gates; i++) {
        printf("  gate[%d]: %s, size %dx%d\n", i,
               c->gates[i]->name,
               c->gates[i]->matrix->size,
               c->gates[i]->matrix->size);

    /*for (int r = 0; r < sz; r++)
        for (int cl = 0; cl < sz; cl++)
            printf("    [%d][%d] = %f + i%f\n", r, cl,
                   c->gates[i]->matrix->data[r][cl].real,
                   c->gates[i]->matrix->data[r][cl].imag);*/
}

    printf("circuit length: %d\n", c->circ_len);
    for (int i = 0; i < c->circ_len; i++)
        printf("  step[%d]: %s\n", i, c->gates[c->circ[i]]->name);

    printf("\nmeasure: %d\n", c->measure);

    free_init_state(s);
    free_circuit(c);

    return 0;

}
