/************************************************************
 * McStas Eigensolver library                               *
 * Contributed by James Emil Avery, 202x                    *
 * Department of Computer Science, University of Copenhagen *
 ************************************************************/

#ifndef EIGEN_LIB_H
#define EIGEN_LIB_H

/* Original file "types.h": */
#include <float.h>

// PW: Non-float64 logic suppressed.
  #define real_t double
  #define scalar cdouble
  #define machine_precision DBL_EPSILON
 #define PRINTF_G "%g"

#ifndef  _MSC_EXTENSIONS
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

typedef struct {
  real_t value[2];
} real_pair;


/* End of "types.h"         */
/* ------------------------ */
/* Original file "eigen.h": */

void print_vector(const char *name, cdouble *a, int l);
void print_matrix(const char *name, cdouble *A, int m, int n);

real_t vector_norm(const cdouble *x, int n);
void   extract_region(cdouble *S, int N,
		    int i0, int j0, int m, int n,
		    cdouble *D);
double max_norm(cdouble* A, int m, int n);
void identity_matrix(cdouble *Q, int n);
void real_identity_matrix(double *Q, int n);
void matrix_inplace_multiply(cdouble *A, cdouble *B, 
			     int m, int n, int q);

void reflection_vector(/*in*/cdouble *a, double anorm,
		       /*out*/cdouble *v, cdouble *sigma, int n);
void apply_reflection(/*in/out*/cdouble *A, const cdouble *v,
		      int m, int n, cdouble sigma, int transpose);

void reflect_region(/*in/out*/cdouble *A, int N,
		    int i0, int j0, int m, int n,
		    const cdouble *v, cdouble sigma, int cols);

void apply_real_reflections(double *V, int n, double *Q, int m);


void QHQ(/*in/out*/cdouble *A, int n, cdouble *Q);
void T_QTQ(const int n,
	   const real_t *Din, const real_t *Lin,
	   real_t *Dout, real_t *Lout, real_t *Vout,
	   real_t shift);

real_pair eigvalsh2x2(real_t a, real_t b, real_t c, real_t d);

real_pair eigensystem_hermitian(const cdouble *A, int n,
				real_t *lambdas, cdouble *Q);

/* End of eigen.h */
#endif
