                        QSim - Quantum Circuit Simulator
----------------------------------------------------------------------------------------------

Project for the Operating Systems II Module course (a.y. 2025-26)
Channel A-L - Prof. Paolo Zuliani


## Included files

### Source files
- src/main.c       : entry point, CLI argument handling with getopt
- src/parser.c     : input file parsing (initial state and circuit)
- src/complex.c    : complex number operations (sum, product, modulus)
- src/matrix.c     : matrix operations (allocation, product, matrix x vector)
- src/thread.c     : multi-thread logic (unitary evolution and measurement)

### Header files
- include/parser.h  : structures and prototypes for the parser
- include/complex.h : complex_t structure and prototypes
- include/matrix.h  : matrix_t structure and prototypes
- include/thread.h  : structures and prototypes for threads

### Test files
- tests/EPR-init.q             : initial state for the EPR circuit (2 qubits)
- tests/EPR-circ.q             : EPR circuit
- tests/EPR-finalstate.q       : expected final state for EPR
- tests/H2-init.q              : initial state for Hadamard 2 qubits
- tests/H2-circ.q              : Hadamard 2 qubit circuit
- tests/H2-finalstate.q        : expected final state for H2
- tests/H10-init.q             : initial state for Hadamard 10 qubits
- tests/H10-circ.q             : Hadamard 10 qubit circuit
- tests/H10-circ-no-newlines.q : H10 circuit on a single line
- tests/init-ex.q              : example initial state (1 qubit)
- tests/circ-ex.q              : example circuit (1 qubit)
- tests/EPR-circ-measure.q     : EPR circuit with measurement

## Compilation

  make

To recompile from scratch:

  make clean && make

## Usage

  ./qsim -s <state_file> -c <circuit_file> -t <num_threads>

### Arguments
- -s <state_file>   : path to the initial state file
- -c <circuit_file> : path to the circuit file
- -t <num_threads>  : number of threads to use (>= 1)

## Input file format

### Initial state file
#qubits n
#init [α0, α1, ..., α2^n-1]
Complex numbers are in the format `a+ib` or `a-ib`.
The imaginary part may be absent.

Example:
#qubits 2
#init [1, 0, 0, 0]

### Circuit file
#define NAME [...]
#circ G0 G1 ... Gk
Optionally with measurement:
#circ G0 G1 ... Gk measure N

Example:
#define H [(0.5+i0.0, 0.5+i0.0) (0.5+i0.0, -0.5+i0.0)]
#circ H measure 1000

## Output format

### Without measurement
The final state of the circuit in the format:
[ α0+iβ0,  α1+iβ1,  ...,  α2^n-1+iβ2^n-1 ]

### With measurement
The estimated probability distribution:
00 @ 0.50100
01 @ 0.00000
10 @ 0.00000
11 @ 0.49900

## Examples

Simulation without measurement:
./qsim -s tests/EPR-init.q -c tests/EPR-circ.q -t 4

Simulation with measurement (1000 samples, 4 threads):
./qsim -s tests/EPR-init.q -c tests/EPR-circ-measure.q -t 4

Simulation with 10 qubits:
./qsim -s tests/H10-init.q -c tests/H10-circ.q -t 4