#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "parser.h"

// utility functions
/*
 * Reads the entire file into a null-terminated string.
 *
 * Input:  filename - path to the file
 * Output: pointer to the string, NULL on error
 */
char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {perror("fopen"); return NULL;}

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);

    char *buf = malloc(file_size + 1);
    fread(buf, 1, file_size, f);
    buf[file_size] = '\0';
    fclose(f);
    return buf;
}

/*
 * Skips whitespace and commas in a string.
 *
 * Input:  p - pointer to current position
 * Output: pointer to first non-whitespace char 
 */
static const char *skip_whitespace(const char *p) {
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',')
        p++;
    return p;
}

/*---------------- PARSER -----------------------*/

/*              COMPLEX PARSER                   */

/*
 * Parses a complex number from a string pointer, advancing the pointer.
 *
 * Handles formats:
 *   (a, b)        e.g. (0, 1) or (0.5, -0.5)
 *   a+ib          e.g. 0.70711+i0.00000
 *   a-ib          e.g. 0.70711-i0.00000
 *   -a+ib         e.g. -0.70711+i0.00000
 *   -a-ib         e.g. -0.70711-i0.00000
 *   i / +i        pure positive imaginary 
 *   -i            pure negative imaginary 
 *   iN / +iN      e.g. i0.00000
 *   -iN           e.g. -i0.00000
 *   a             real only
 * Input:  p   - pointer to current position 
 *         out - pointer to store the result 
 * Output: pointer after the parsed number 
 */

static const char *parse_complex(const char *p, complex_t *out) {
    out -> real = 0.0;
    out -> imag = 0.0;
    int chars = 0;

    p = skip_whitespace(p);


    /* ----- format (a, b) ----- */
    if (*p == '(') {
        p++; // skip
        p = skip_whitespace(p);

        /* read real part*/
        sscanf(p, "%lf%n", &out->real, &chars);
        p += chars;
        p = skip_whitespace(p);

        /* read imaginary part*/
        sscanf(p, "%lf%n", &out->imag, &chars);
        p += chars;
        p = skip_whitespace(p);

        if (*p == ')') p++; /* skip ')' */
        return p;
    }

    /* ----- pure imaginary -i or -iN ----- */
    if (*p == '-' && *(p+1) == 'i') {
        p += 2; //skip
        double val = 1.0;
        chars = 0;
        sscanf(p, "%lf%n", &val, &chars);
        p += chars;
        out -> imag = -val;
        return p;
    }

    /* ----- pure imaginary +i or +iN -----*/
    if (*p == 'i' || (*p == '+' && *(p+1) == 'i')) {
        if (*p == '+') p++; /* skip '+' */
        p++;        
        double val = 1.0;
        chars = 0;
        sscanf(p, "%lf%n", &val, &chars);
        p += chars;
        out -> imag = val;
        return p;
    }

    /* ------ real part (handles even negatives) ----- */

    chars = 0;
    sscanf(p, "%lf%n", &out->real, &chars);
    p += chars;

    /* ----- optional imaginary part +ib ----- */
    if (*p == '+' && *(p+1) == 'i') {
        p += 2; /* skip '+i' */
        chars = 0;
        double val = 1.0; /* default to 1 in case of bare +i */
        sscanf(p, "%lf%n", &val, &chars);
        p += chars;
        out->imag = val;
        return p;
    }

    /* ----- optional imaginary part -ib -----*/
    if (*p == '-' && *(p+1) == 'i') {
        p += 2; /* skip '-i' / salta '-i' */
        chars = 0;
        double val = 1.0; /* default to 1 in case of bare -i */
        sscanf(p, "%lf%n", &val, &chars);
        p += chars;
        out->imag = -val;
        return p;
    }

    return p;
}


/*             INIT PARSER           */

/*
 * Reads and parses the initial state file.
 *
 * Input:  filename - path to the init file
 * Output: pointer to init_state_t, NULL on error
 */

init_state_t *parse_init(const char *filename) {
    char *buf = read_file(filename);
    if (!buf) return NULL;

    init_state_t *s = malloc(sizeof(init_state_t));
    s -> n = 0;
    s -> state = NULL;

    const char *p = buf;

    while(*p){
        p = skip_whitespace(p);
        if (!*p) break;
        
        if (strncmp(p, "#qubits", 7) == 0) {
            p += 7;
            p = skip_whitespace(p);
            /*read number of qubits*/
            int chars = 0;
            sscanf(p, "%d%n", &s->n, &chars);
            p += chars;

        } else if (strncmp(p, "init", 5) == 0) {
            p += 5;
            p = skip_whitespace(p);
            
            if (*p == '[') p++; // skip '['
            
            int size = 1 << s->n;
            s->state = malloc(size * sizeof(complex_t));

            /* parse each complex number */
            for (int i = 0; i < size; i++) {
                p = skip_whitespace(p);
                p = parse_complex(p, &s->state[i]);
            }

            p = skip_whitespace(p);
            if (*p == ']') p++; /* skip ']' */
        } else {
            /* skip unknown line */
            while (*p && *p != '\n') p++;
        }
    }

    free(buf);
    return s;
}

/*              CIRCUIT PARSER             */

/*
 * Reads and parses the circuit file.
 *
 * Input:  filename - path to the circuit file 
 *         n        - number of qubits 
 * Output: pointer to circuit_t, NULL on error
 */

circuit_t *parse_circuit(const char *filename, int n) {
    char *buf = read_file(filename);
    if (!buf) return NULL;

    circuit_t *c = malloc(sizeof(circuit_t));
    c -> gates = NULL;
    c -> num_gates = 0;
    c -> circ = NULL;
    c -> circ_len = 0;
    c -> measure = 0;

    int size = 1 << n; /* matrix dimension 2^n */
    const char *p = buf;

    while (*p) {
        p = skip_whitespace(p);
        if (!*p) break;

        if (strncmp(p, "#define", 7) == 0) {
            p += 7;
            p = skip_whitespace(p);

            /*read gate name*/
            char name[256];
            int chars = 0;
            sscanf(p, "%255s%n", name, &chars);
            p += chars;
            p = skip_whitespace(p); 

            if (*p == '[') p++;

            /* allocate and fill matrix */
            matrix_t *mat = matrix_alloc(size);
            for (int i = 0; i < size; i++){
                for (int j = 0; j < size; j++){
                    p = skip_whitespace(p);
                    p = parse_complex(p, &mat->data[i][j]);
                }
            }
            p = skip_whitespace(p);
            if (*p == ']') p++; /* skip ']'*/

            /* store gate dinamically */
            c -> gates = realloc(c->gates, (c-> num_gates + 1) * sizeof(gate_t*));
            c->gates[c->num_gates] = malloc(sizeof(gate_t));
            c->gates[c->num_gates]->name   = strdup(name);
            c->gates[c->num_gates]->matrix = mat;
            c->num_gates++;

        } else if (strncmp(p, "#circ", 5) == 0) {
            p += 5;

            /* read gate names until end of line */

            while (*p && *p != '\n') {
                p = skip_whitespace(p);
                if (!*p || *p == '\n') break;

                char name[256];
                int chars = 0;
                sscanf(p, "%255s%n", name, &chars);
                p += chars;

                /* check for measure keyword */
                if (strcmp(name, "measure") == 0) {
                    sscanf(p, "%d", &c->measure);
                    while (*p && *p != '\n') p++;
                    break;
                }

                /* find gate index by name */
                int idx = -1;
                for (int i = 0; i < c->num_gates; i++)
                    if (strcmp(c->gates[i]->name, name) == 0) { idx = i; break; }

                if (idx == -1) {
                    fprintf(stderr, "Gate '%s' not defined\n", name);
                    exit(EXIT_FAILURE);
                }

                /* store index */
                c->circ = realloc(c->circ, (c->circ_len + 1) * sizeof(int));
                c->circ[c->circ_len++] = idx;
            } 
        } else {
            /* skip unknown line */
            while (*p && *p != '\n') p++;
        }
    }
    
    free(buf);
    return c;
}



// free functions

/*
 * Frees the memory of the initial state.
 */
void init_state_free(init_state_t *s) {
    if (!s) return;
    free(s->state);
    free(s);
}

/*
 * Frees the memory of the circuit.
 */

void circuit_free(circuit_t *c) {
    if (!c) return;
    for (int i = 0; i < c->num_gates; i++) {
        free(c->gates[i]->name);
        matrix_free(c->gates[i]->matrix);
        free(c->gates[i]);
    }
    free(c->gates);
    free(c->circ);
    free(c);
}
