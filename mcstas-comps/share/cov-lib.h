/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2020, All rights reserved
*         DTU Physics, Lyngby, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Library: share/cov-lib.h
*
* %Identification
* Written by: Tobias Weber <tweber@ill.fr>
* Date: Nov 20, 2020
* Origin: Institut Laue Langevin
* Release: McStas 3.0
* Version: 0.1
*
* This file is used for resolution calculations, it was taken from "tlibs2" and "matrix_calc":
*   https://code.ill.fr/scientific-software/takin/tlibs2/-/blob/master/libs/mathlib.h
*   https://github.com/t-weber/matrix_calc/blob/master/src/libs/runtime.c
*
* Usage: within SHARE
* %include "cov-lib"
*
*******************************************************************************/

#ifndef __TLIBS2_C_MATHLIB_H__
#define __TLIBS2_C_MATHLIB_H__


/* ---------------------------------------------------------------------------- */
/* linalg functions */
/* ---------------------------------------------------------------------------- */
/**
 * set float epsilon
 */
extern void tl2_set_eps(double eps);

/**
 * get float epsilon
 */
extern double tl2_get_eps();

/**
 * tests equality of floating point numbers
 */
extern int tl2_flt_equals(double x, double y, double eps);

/**
 * set matrix elements to zero
 */
extern void tl2_mat_zero(double* M, int I, int J);

/**
 * set vector elements to zero
 */
extern void tl2_vec_zero(double* vec, int N);

/**
 * copy a vector
 */
extern void tl2_vec_cpy(double* dst, const double* src, int N);

/**
 * copy a matrix
 */
extern void tl2_mat_cpy(double* DST, const double* SRC, int I, int J);

/**
 * removes a given row and column of a square matrix
 */
extern void tl2_submat(const double* M, int N, double* M_new, int iremove, int jremove);

/**
 * calculates the determinant
 */
extern double tl2_determinant(const double* M, int N);

/**
 * inverted matrix
 */
extern int tl2_inverse(const double* M, double* I, int N);

/**
 * matrix-matrix product
 */
extern void tl2_matmat_mul(const double* M1, const double* M2,
	double *RES, int I, int J, int K);

/**
 * matrix-vector product
 */
extern void tl2_matvec_mul(const double* M, const double* v,
	double *res, int I, int J);

/**
 * transposed matrix
 */
extern void tl2_transpose(const double* M, double* T, int rows, int cols);

/**
 * vector inner product
 */
extern double tl2_inner(const double* v0, const double* v1, int N);

/**
 * vector outer product
 */
extern void tl2_outer(const double* v0, const double* v1, double *M, int N);

/**
 * 3-vector cross product
 */
extern void tl2_cross(const double* v0, const double* v1, double *res);

/**
 * vector length
 */
extern double tl2_vec_len(const double* vec, int N);

/**
 * vector addition
 */
extern void tl2_vec_add(const double* v0, const double* v1, double *res, int N);

/**
 * vector subtraction
 */
extern void tl2_vec_sub(const double* v0, const double* v1, double *res, int N);

/**
 * negative vector
 */
extern void tl2_vec_neg(const double* vec, double *res, int N);

/**
 * vector-scalar multiplication
 */
extern void tl2_vec_mul(const double* v, double s, double *res, int N);

/**
 * vector-scalar division
 */
extern void tl2_vec_div(const double* v, double s, double *res, int N);

/**
 * matrix addition
 */
extern void tl2_mat_add(const double* M0, const double* M1,
	double *RES, int I, int J);

/**
 * matrix subtraction
 */
extern void tl2_mat_sub(const double* M0, const double* M1,
	double *RES, int I, int J);

/**
 * negative matrix
 */
extern void tl2_mat_neg(const double* M, double *RES, int I, int J);

/**
 * matrix-scalar multiplication
 */
extern void tl2_mat_mul(const double* M, double s,
	double *RES, int I, int J);

/**
 * matrix-scalar division
 */
extern void tl2_mat_div(const double* M, double s,
	double *RES, int I, int J);

/**
 * mean vector
 */
extern void tl2_vec_mean(const double* vecs, const double* probs,
	double* mean, int N, unsigned int EVTS);

/**
 * matrix trafo
 */
extern void tl2_mat_trafo(const double* M, const double* T,
	double* RES, int N, int ortho);

/**
 * covariance matrix
 */
extern int tl2_covariance(const double* vecs, const double* probs,
	double* COV, double* mean, int N, unsigned int EVTS);

/**
 * resolution matrix
 */
extern int tl2_reso(const double* vecs, const double* probs,
	double* COV, double* RESO, unsigned int EVTS);
/* ---------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------- */
/* Helper functions */
/* ----------------------------------------------------------------------------- */
#pragma acc routine
extern double tl2_k_to_E(double kix, double kiy, double kiz, double kfx, double kfy, double kfz);

extern void tl2_print_vec(const double* vec, const char* title, int I);
extern void tl2_print_mat(const double* mat, const char* title, int I, int J);

/**
 * save neutron events
 */
extern int tl2_save_events(const double* vecs, const double* probs,
	const char* filename, unsigned int EVTS);
/* ----------------------------------------------------------------------------- */


#endif
