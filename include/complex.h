#ifndef COMPLEX_H
#define COMPLEX_H

/* Complex number representation -> a + ib */
typedef struct {
    double real;    // Real part
    double imag;    // Imaginary part
} complex_t;

// basic operations
complex_t complex_add(complex_t a, complex_t b);
complex_t complex_mul(complex_t a, complex_t b);
double    complex_mod(complex_t a);

#endif 