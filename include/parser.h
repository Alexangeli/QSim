#ifndef PARSER_H
#define PARSER_H

#include "matrix.h"
#include "complex.h"

/* Structure for a quantum gate with name and matrix.
name:   gate name, dynamically allocated
matrix: associated matrix
*/

typedef struct {
    char     *name;
    matrix_t *matrix;
} gate_t;

/* Structure representing the full parsed circuit. 
 * gates:     array of defined gates 
 * num_gates: number of defined gates 
 * circ:      array of gate indices in circuit order 
 * circ_len:  number of gates in the circuit 
 * measure:   number of measurements, 0 if absent */

typedef struct {
    gate_t  **gates;
    int      num_gates;
    int     *circ;
    int      circ_len;
    int      measure;
} circuit_t;


/*
 * Structure representing the initial quantum state.
 *
 * n:     number of qubits 
 * state: state vector of size 2^n
 */

typedef struct {
    int        n;
    complex_t *state;
} init_state_t;


/********** Function prototypes **********/

/*
 * Reads and parses the initial state file.
 * Input:  filename - path to the init file
 * Output: pointer to init_state_t, NULL on error
 */
init_state_t *parse_init_state(const char *filename);

/*
 * Reads and parses the circuit file.
 * Input:  filename - path to the circuit file
           num_qubits - number of qubits in the circuit
 * Output: pointer to circuit_t, NULL on error
 */
circuit_t *parse_circuit(const char *filename, int num_qubits);

/*
 * Frees memory allocated for a initial state.
 * Input:  init_state - pointer to the initial state to free
 */
void free_init_state(init_state_t *init_state);

/*
 * Frees memory allocated for a circuit_t structure.
 * Input:  circuit - pointer to the circuit to free
 */
void free_circuit(circuit_t *circuit);

#endif