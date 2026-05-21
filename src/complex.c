#include <math.h>
#include "complex.h"

/* Add two complex numbers */
complex_t complex_add(complex_t a, complex_t b) {
    complex_t res;
    res.real = a.real + b.real;
    res.imag = a.imag + b.imag;
    return res;
}

/* Multiply two complex numbers */

complex_t complex_mul(complex_t a, complex_t b) {
    complex_t res;
    res.real = a.real * b.real - a.imag * b.imag;
    res.imag = a.real * b.imag + a.imag * b.real;
    return res;
}

/* Calculate the modulus of a complex number */

double complex_mod(complex_t a) {
    return sqrt(a.real * a.real + a.imag * a.imag);
}