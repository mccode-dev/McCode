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

/* Largest matrix dimension n. Scratch arrays are fixed-size stack arrays of this
   size (no heap allocation per call, so the solver can run on an OpenACC device). */
#ifndef EIGEN_MAXN
#define EIGEN_MAXN 12
#endif


/* End of "types.h"         */
/* ------------------------ */
/* Original file "eigen.h": */

void print_vector(const char *name, cdouble *a, int l);
void print_matrix(const char *name, cdouble *A, int m, int n);

#pragma acc routine seq
real_t vector_norm(const cdouble *x, int n);
#pragma acc routine seq
void   extract_region(cdouble *S, int N,
		    int i0, int j0, int m, int n,
		    cdouble *D);
#pragma acc routine seq
double max_norm(cdouble* A, int m, int n);
#pragma acc routine seq
void identity_matrix(cdouble *Q, int n);
#pragma acc routine seq
void real_identity_matrix(double *Q, int n);
#pragma acc routine seq
void matrix_inplace_multiply(cdouble *A, cdouble *B, 
			     int m, int n, int q);

#pragma acc routine seq
void reflection_vector(/*in*/cdouble *a, double anorm,
		       /*out*/cdouble *v, cdouble *sigma, int n);
#pragma acc routine seq
void apply_reflection(/*in/out*/cdouble *A, const cdouble *v,
		      int m, int n, cdouble sigma, int transpose);

#pragma acc routine seq
void reflect_region(/*in/out*/cdouble *A, int N,
		    int i0, int j0, int m, int n,
		    const cdouble *v, cdouble sigma, int cols);

#pragma acc routine seq
void apply_real_reflections(double *V, int n, double *Q, int m);


#pragma acc routine seq
void QHQ(/*in/out*/cdouble *A, int n, cdouble *Q);
#pragma acc routine seq
void T_QTQ(const int n,
	   const real_t *Din, const real_t *Lin,
	   real_t *Dout, real_t *Lout, real_t *Vout,
	   real_t shift);

#pragma acc routine seq
real_pair eigvalsh2x2(real_t a, real_t b, real_t c, real_t d);

#pragma acc routine seq
real_pair eigensystem_hermitian(const cdouble *A, int n,
				real_t *lambdas, cdouble *Q);

/* End of eigen.h */
#endif
