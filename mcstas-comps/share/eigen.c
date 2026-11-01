/************************************************************
 * McStas Eigensolver library                               *
 * Contributed by James Emil Avery, 202x                    *
 * Department of Computer Science, University of Copenhagen *
 ************************************************************/

/* Original file "eigen.c": */

#ifndef EIGEN_LIB
#define EIGEN_LIB

#include <stddef.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <complex.h>
#include <time.h>

/* TODO: Tridiagonal input/outpout in (3,m) storage. */
/* TODO: Symmetric tridiagonal input/outpout in (2,m) storage. */
/* TODO: Add pivot to handle ill-conditioned matrices */
// #include "eigen.h" 

#define G PRINTF_G
#define ROWS 0
#define COLS 1

/* TOOD: Generic matrix functions to separate file */
INLINE real_t vector_norm2(const cdouble*x, int n){
  real_t sum = 0;
  for(int i=0;i<n;i++) sum += cabs(cmul(x[i],conj(x[i])));
  return sum;
}

INLINE real_t vector_norm(const cdouble *x, int n){
  return sqrt(vector_norm2(x,n));
}

INLINE void extract_region(cdouble *S, int N,
			   int i0, int j0, int m, int n,
			   cdouble *D)
{
  for(int i=0;i<m;i++)
    for(int j=0;j<n;j++)
      D[i*n+j] = S[(i+i0)*N+(j+j0)];
}

INLINE void identity_matrix(cdouble *Q, int n)
{
  memset(Q,0,n*n*sizeof(cdouble));
  for(int i=0;i<n;i++) Q[i*n+i] = cplx(1,0);
}

INLINE void real_identity_matrix(real_t *Q, int n)
{
  memset(Q,0,n*n*sizeof(real_t));
  for(int i=0;i<n;i++) Q[i*n+i] = 1;
}

/* TODO: Make this work with both real and complex cdoubles (cabs should be replaced by a macro) */
INLINE real_t max_norm(cdouble *A, int m, int n)
{
  real_t mx = 0;
  for(int i=0;i<m;i++){ 	
    real_t row_norm = 0;
    for(int j=0;j<n;j++) row_norm += cabs(A[i*n+j]);
    mx = fmax(mx,row_norm);
  }
  return mx;  
}

INLINE void matrix_inplace_multiply(cdouble *A, cdouble *B, int m, int n, int q)
{
  for(int i=0;i<m;i++){
    cdouble* row=malloc(q*sizeof(cdouble));
    for(int j=0;j<q;j++){
      cdouble sum = cplx(0,0);
      for(int k=0;k<n;k++) sum = cadd(sum,cmul(A[i*n+k],B[k*q+j]));
      row[j] = sum;
    }
    for(int j=0;j<q;j++) A[i*n+j] = row[j];
  }
}


/* TODO: Document */
INLINE void apply_reflection(/*in/out*/cdouble* A, const cdouble *v, int m, int n, cdouble sigma, int transpose)
{
  cdouble* vHA=malloc(n*sizeof(cdouble));
  cdouble conj_sigma = conj(sigma);
  int stride[2] = {n*(!transpose) + 1*transpose, n*transpose + 1*(!transpose)};
  
  for(int j=0;j<n;j++){		
    cdouble sum = cplx(0,0);
    for(int k=0;k<m;k++) sum = cadd(sum,cmul(conj(v[k]),A[k*stride[0]+j*stride[1]]));
    vHA[j] = sum;
  }

  for(size_t i=0;i<m;i++)       /* A += -2*outer(v,vTA) */
    for(size_t j=0;j<n;j++){
      A[i*stride[0]+j*stride[1]] = csub(A[i*stride[0]+j*stride[1]],cmul(cmul(conj_sigma,v[i]),vHA[j])); 
    }  
}

/* A: NxN
   Transform mxn region starting at (i0,j0)
 */
INLINE void reflect_region(/*in/out*/cdouble* A, int N, int i0, int j0, int m, int n, const cdouble *v, cdouble sigma, int cols)
{
  cdouble* vHA=malloc(n*sizeof(cdouble));
  cdouble conj_sigma = conj(sigma);
  int stride[2] = {N*(!cols) + 1*cols, N*cols + 1*(!cols)};

  /* printf("reflect_region(A,%d,\n"
  	 "                 %d,%d\n"
  	 "                 %d,%d)\n",N,i0,j0,m,n);
  printf("stride = [%d,%d]\n",stride[0],stride[1]);
   */
  for(int j=0;j<n;j++){		
    cdouble sum = cplx(0,0);
    for(int i=0;i<m;i++){
      size_t IJ = (i+i0)*stride[0] + (j+j0)*stride[1];
      sum = cadd(sum,cmul(conj(v[i]),A[IJ]));
    }
    vHA[j] = sum;
  }

  for(size_t i=0;i<m;i++)       /* A += -2*outer(v,vTA) */
    for(size_t j=0;j<n;j++){
      size_t IJ = (i+i0)*stride[0] + (j+j0)*stride[1];
      A[IJ] = csub(A[IJ],cmul(conj_sigma,cmul(v[i],vHA[j]))); 
    }  
}

/* Apply n 2D reflections to an mxm matrix Q */
/* TODO: Transform rows instead (after this works) */
/* TODO: Since we no longer have views, pass view as function parameter */
void apply_real_reflections(real_t* V, int n, real_t* Q, int m)
{
  if(Q != 0){       // Do we want eigenvectors?
    for(int k=0;k<n;k++){
      real_t v0 = V[2*k], v1 = V[2*k+1];      
      // Udrullet:
      //       apply_reflection(Q({k,k+2},{0,m}), v);
      for(int l=0;l<m;l++){
	real_t q0 = Q[k*m+l], q1 = Q[(k+1)*m+l]; /* cdouble *q = &Q[l*m+k] */
	real_t vTA = q0*v0 + q1*v1;
	Q[k*m+l]     -= 2*v0*vTA;
	Q[(k+1)*m+l] -= 2*v1*vTA;
      }      
    }  
  }  
}

/* INLINE real_t COPYSIGN(real_t to, real_t from) */
/* { */
/*   if(fabs(from/to)<1e-14) return to; */
/*   else return copysign(to,from); */
/* } */

INLINE void reflection_vector(/*in*/cdouble* a, const real_t anorm,
			      /*out*/cdouble* v, cdouble* sigma, int n)
{ // Reflection vector that eliminates *row* a (as opposed to *column*)

  for(int i=0;i<n;i++) v[i] = a[i];

  // In the complex case, straightforward Householder reflection can only transform a Hermitian matrix
  // to a complex Hermitian tridiagonal matrix.
  // The following transform reduces to Householder reflection for real matrices, but
  // in the complex case, it produces a real symmetric tridiagonal matrix.
  
  // Using the nomenclature from [LAWN72], Pages 4-5 and setting xi_i = a[i-1]:
  cdouble x1 = a[0];
  real_t nu = copysign(anorm,creal(x1)); 
  cdouble norm_inv = cdiv(cplx(1.0,0),radd(nu,x1));
  *sigma = rmul(1/nu,radd(nu,x1));
  v[0] = radd(nu,v[0]);
  
  for(size_t i=0;i<n;i++) v[i] = cmul(v[i],norm_inv);
}



// Decompose a matrix (complex or real) into A = Q H Q.H(), where H is upper Hessenberg.
// If A is Hermitian (symmetric in real case), then H is tridiagonal.
// TODO: Row pivot
void print_vector(const char* name, cdouble* a, int l)
{
  printf("%s = array([",name); for(int i=0;i<l;i++) printf("%.3g + %.3gj%s",creal(a[i]), cimag(a[i]), i+1<l?", ":"])\n");
}

void print_matrix(const char *name, cdouble *A, int m, int n)
{
  printf("%s = array([\n",name);
  for(int i=0;i<m;i++){
    printf("\t[");
    for(int j=0;j<n;j++)
      printf("%.3g + %.3gj%s",creal(A[i*n+j]), cimag(A[i*n+j]), j+1<n?", ":"]");
    printf("%s",i+1<m?",\n":"\n])\n");
  }
}

/* TODO: Kan jeg håndtere a[0] =~= 0 mere robust? Kan jeg inkludere pivots? */
void QHQ(/*in/out*/cdouble* A, int n, cdouble* Q)
{
  cdouble  sigma;	        // Elementary operation scale (2 for reflection)
  cdouble* v=malloc(n*sizeof(cdouble));
  cdouble* vc=malloc(n*sizeof(cdouble));
  cdouble* a=malloc(n*sizeof(cdouble));    // Reflection vector

  real_t numerical_zero = max_norm(A,n,n)*10*machine_precision;
  
  for(int k=0;k<n-1;k++){
    int l = n-(k+1);		     /* length of kth subdiagonal column */
    extract_region(A,n,k+1,k,l,1,a); /* a = A[k:(k+1),(k+1):n], kth row postdiagonal. */    
    real_t anorm = vector_norm(a,l);
/*     printf("\n\n%d: ",k); print_vector("a",a,l);
    printf("%d: ",k); print_matrix("A before",A,n,n);    
 */
    if(anorm < numerical_zero) continue; /* Already eliminated, don't divide by 0 */
    
    reflection_vector(a,anorm,v,&sigma,n);    /* Vector definining elimination operations */
    for(int i=0;i<l;i++) vc[i] = conj(v[i]);  /* ...and its conjugate. */
    
    reflect_region(A,n,/* reg. start:*/k+1,k, /* reg. size:*/l,l+1, v,       sigma,  ROWS);/* TODO: fix */
    reflect_region(A,n,/* reg. start:*/k+1,k, /* reg. size:*/l,l+1, vc,conj(sigma),  COLS);
    // printf("%d: ",k); print_matrix("A after",A,n,n);

    if(Q != 0) reflect_region(Q,n, k+1,0, l,n, v, sigma, ROWS); 
  }
  free(v);free(vc);free(a);
}

// TODO: T_QTQ based on Givens rotations (should be possible to do with fewer operations)
//size_t QTQ_calls = 0;
void T_QTQ(const int n, const real_t *Din, const real_t *Lin, real_t *Dout, real_t *Lout, real_t *Vout, real_t shift)
{
  // Unrolled
  //  real_t numerical_zero = T.max_norm()*10*machine_precision;
  // specialized max_norm = max(sum(abs(A),axis=1)) for tridiagonal matrix. 
  real_t max_norm = 0, numerical_zero = 0;
  //  for(int i=0;i<n;i++) max_norm = max(max_norm, fabs(Din[i]) + 2*fabs(Lin[i]));
  //  numerical_zero = 10*max_norm*machine_precision;//TODO: max_norm for symmetric tridiagonal
  numerical_zero = 100*machine_precision;
  
  real_t a[2], v[2];
  real_t *D=malloc((n+1)*sizeof(real_t));
  real_t *L=malloc((n+1)*sizeof(real_t));
  real_t *U=malloc((2*(n+1))*sizeof(real_t));

  for(int i=0;i<n+1;i++){
    D[i] = Din[i]-shift;		// Diagonal
    L[i] = 0;			// Zero padding to avoid branching in inner loop
    U[i] = 0;                   // Zero padding to avoid branching in inner loop
    U[(n+1)+i] = 0;		// Second upper diagonal for fill-in. U[n+k] = T(k,k+2) is the element two rows above (k+2)st diagonal element.
    if(i<n-1){
      L[ i ] = Lin[i];	// First lower subdiagonal. L[k] = T(k+1,k) is element below kth diagonal element.
      U[ i ] = Lin[i];	// First upper subdiagonal. U[k] = T(k,k+1) is element above (k+1)st diagonal element.
      Vout[2*i] = 0; Vout[2*i+1] = 0;	// i'th reflection vector. (0,0) yields "no reflection". Must be initialized due to skipped steps.          
    }
  }

  for(int k=0;k<n-1;k++)
    if(fabs(L[k]) > numerical_zero)  // Only process if subdiagonal element is not already zero.
    {
      a[0] = D[k]; a[1] = L[k];       // a = T[k:k+2,k] is the vector of nonzeros in kth subdiagonal column.
      
      real_t anorm = sqrt(a[0]*a[0] + a[1]*a[1]); 

      // // Udrullet
      // //    reflection_vector(a,anorm,v);
      v[0] = D[k]; v[1] = L[k];
      real_t alpha = -copysign(anorm,a[0]); // Koster ingenting
      v[0] -= alpha;

      real_t vnorm = sqrt(v[0]*v[0]+v[1]*v[1]);
      real_t norm_inv = 1/vnorm;               /* Normalize */
      v[0] *= norm_inv;  v[1] *= norm_inv;

      Vout[2*k] = v[0]; Vout[2*k+1] = v[1];
      
      // // Udrullet 
      // //    apply_reflection(T({k,k+2},{k,k+3}),v);
      // //      if(k+1<n){			// k=n-1 case handled by padding with zeros
      real_t   vTA[3] = {D[ k ]*v[0] + L[ k ]*v[1],  // T(k,k  )*v[0] + T(k+1,k  )*v[1]
      			 U[ k ]*v[0] + D[k+1]*v[1],  // T(k,k+1)*v[0] + T(k+1,k+1)*v[1]
      			 U[(n+1)+k]*v[0] + U[k+1]*v[1]}; // T(k,k+2)*v[0] + T(k+1,k+2)*v[1]

      D[ k ]     -= 2*v[0]*vTA[0];
      L[ k ]     -= 2*v[1]*vTA[0];
      U[ k ]     -= 2*v[0]*vTA[1];
      D[k+1]     -= 2*v[1]*vTA[1];
      U[(n+1)+k] -= 2*v[0]*vTA[2];
      U[k+1]     -= 2*v[1]*vTA[2];
    }

  // Transform from the right = transform columns of the transpose.
  {
    int k = 0;
    const real_t *v = &Vout[0];
    real_t   vTA[2] = {D[ k ]*v[0] + U[  k  ]*v[1],  // T(k,k  )*v[0] + T(k,  k+1)*v[1]
  		          0        + D[ k+1 ]*v[1]}; // T(k+1,k)*v[0] + T(k+1,k+1)*v[1]. Lower subdiagonal is zero at this stage.
    
    D[k]       -= 2*v[0]*vTA[0]; // T(k,k)     -= 2*v[0]*vTA[0]
    U[k]       -= 2*v[1]*vTA[0]; // T(k,k+1)   -= 2*v[1]*vTA[0]
    L[k]       -= 2*v[0]*vTA[1]; // T(k+1,k)   -= 2*v[0]*vTA[1]
    D[k+1]     -= 2*v[1]*vTA[1]; // T(k+1,k+1) -= 2*v[1]*vTA[1]        
  }    

  for(int k=1;k<n-1;k++){
    const real_t *v = &Vout[2*k];

    real_t   vTA[3] = {U[k-1]*v[0] + U[(n+1)+k-1]*v[1], // T(k-1,k)*v[0] + T(k-1,k+1)*v[1]  
  		       D[ k ]*v[0] + U[  k  ]*v[1],     // T(k,k  )*v[0] + T(k,  k+1)*v[1]
  		       L[ k ]*v[0] + D[ k+1 ]*v[1]};    // T(k+1,k)*v[0] + T(k+1,k+1)*v[1]. Lower subdiagonal is zero at this stage

    U[k-1]     -= 2*v[0]*vTA[0];     // T(k-1,k)   -= 2*v[0]*vTA[0]
    U[(n+1)+(k-1)] -= 2*v[1]*vTA[0]; // T(k-1,k+1) -= 2*v[1]*vTA[0]
    D[k]       -= 2*v[0]*vTA[1];     // T(k,  k)     -= 2*v[0]*vTA[1]
    U[k]       -= 2*v[1]*vTA[1];     // T(k,  k+1)   -= 2*v[1]*vTA[1]
    L[k]       -= 2*v[0]*vTA[2];     // T(k+1,k)   -= 2*v[0]*vTA[2]
    D[k+1]     -= 2*v[1]*vTA[2];     // T(k+1,k+1) -= 2*v[1]*vTA[2]        
  } 
  
  // Copy working diagonals to output
  for(int i=0;i<n;i++){
    Dout[i] = D[i] + shift;	  // Diagonal
    if(i<n-1){
      Lout[i] = U[i];  // First lower subdiagonal. L[k] = T(k+1,k) is element below kth diagonal element.
    }
  }
  free(D); free(L); free(U);
}


INLINE real_pair eigvalsh2x2(real_t a, real_t b, real_t c, real_t d){
  real_pair result;
  real_t D = sqrt(4*b*c+(a-d)*(a-d));

  result.value[0] = (a+d-D)/2;
  result.value[1] = (a+d+D)/2;

  return result;
}


int    nth_time = 0;
real_t eigensystem_cputime = 0;
#define gershgorin_tolerance 1e4*machine_precision
#define max_iterations       30

// TODO: Til tridiagonale matricer er Givens-rotation nemmere/hurtigere (kun een sqrt)
// TODO: Assumes all different eigenvalues. Does this break with multiples?
// TODO: Stop after max_steps for fixed k. Return max Gershgorin radius as convergence -- or max Rayleigh quotient residual?
// TODO: Implement implicit QR iteration using Francis' Q theorem/bulge chasing
real_pair eigensystem_hermitian(const cdouble *A,
				const int n,
				real_t *lambdas, cdouble* Q)
{
  cdouble *T=malloc(n*n*sizeof(cdouble));	         /* T: Tridiagonal transform, dense representation */
  real_t *V=malloc((2*(n-1))*sizeof(real_t));
  real_t *Qhat=malloc(n*n*sizeof(real_t));	 /* V: Reflection vectors, Qhat: Inner transform */
  memcpy(T, A, n*n*sizeof(cdouble));

  real_t max_error    = gershgorin_tolerance;
  size_t n_iterations = 0;

#if TIME_EIGENSYSTEMS
  clock_t start_time = clock();
  if(((++nth_time) % 1000)==0) printf("%dth call to eigensystem_hermitian, cpu-time used: %gs, on average %gs per call.\n",
				      nth_time,eigensystem_cputime, eigensystem_cputime/nth_time);  
#endif  
  if(Q !=0){ // Do we want to compute eigenvectors?
    identity_matrix(Q,   n);       // Yes, so initialize Q and Qhat to identity    
    real_identity_matrix(Qhat,n);  // After initial transform, everything becomes real
  }
  
  // 1. Initial O(n^3) decomposition A = Q T Q.T to tridiagonal form
  QHQ(T,n,Q);

  /* Copy diagonal + first off-diagonal from dense T. */
  real_t *D=malloc(n*sizeof(real_t));
  real_t *L=malloc(n*sizeof(real_t));
  for(int i=0;i<n;i++){
    D[i] = creal(T[i*n+i]);
    L[i] = (i+1<n)? creal(T[i*n+i+1]) : 0;
  }

  
  // 2. After tridiagonal decomposition, we can do an eigenvalue
  //    QR-iteration step in O(n), and an eigenvector QR-iteration
  //    step in O(n^2).
  for(int k=n-1;k>=0;k--){
    // We start by targeting the (n,n)-eigenvalue, and gradually
    // deflate, working on smaller and smaller submatrices.
    real_t d  = D[k];		// d = T(k,k)
    real_t shift = d;

    // The Gershgorin disk radius is defined by just the row-sums of
    // absolute off-diagonal elements, since T is symmetric. As T is
    // tridiagonal, only T(k,k-1),T(k,k), and T(k,k+1) are nonzero.
    // Thus, the k'th Gershgorin radius is just |T(k,k-1)| +
    // |T(k,k+1)| = |T(k,k-1)| + |T(k+1,k)| = |L[k-1]|+|L[k]|.
    int i=0;

    real_t GR = (k>0?fabs(L[k-1]):0)+fabs(L[k]); // Initial Gershgorin radius
    int not_done = 2;
    while(not_done > 0){
      i++;   
      T_QTQ(k+1, D,L, D,L, V, shift);
      if(Q!=0) apply_real_reflections(V,k,Qhat,n);
      
      GR = (k>0?fabs(L[k-1]):0)+(k+1<n?fabs(L[k]):0); // Gershgorin radius

      // Best guess to eigenvalue in position n-1,n-1.
      if(k>0){
	real_pair l  = eigvalsh2x2(D[k-1],L[k-1], /* T[(k-1):k, (k-1):k] */
				   L[k-1],D[k]  );
	real_t l0 = l.value[0], l1 = l.value[1];
	
	shift    = fabs(l0-d) < fabs(l1-d)? l0 : l1; // Pick closest eigenvalue
      } else
	shift    = D[k];

      if(GR <= gershgorin_tolerance) not_done--;      
      if(i>max_iterations && GR > gershgorin_tolerance){
	fprintf(stderr,"Cannot converge eigenvalue %d to tolerance " G 
	       " in %d iterations using machine precision " G " (d=" G ", shift=" G ", GR=" G ")\n"
	       "D[k] = " G ", L[k-1] = " G ", L[k] = " G "\n",
	       k,gershgorin_tolerance,i,
	       machine_precision,d,shift,GR,
	       D[k], (k>0)?L[k-1]:0, (k+1<n)?L[k]:0);
	
	max_error = fmax(max_error,GR);
	break;
      }
      n_iterations++;
    }
  }
  for(int k=0;k<n;k++) lambdas[k] = D[k];

  if(Q != 0){
    cdouble *Q_tmp=malloc(n*n*sizeof(cdouble));
    for(int i=0;i<n;i++){
      cdouble *col=malloc(n*sizeof(cdouble));
      for(int j=0;j<n;j++){
	cdouble sum = cplx(0,0);
	for(int k=0;k<n;k++) sum = cadd(sum,rmul(Qhat[i*n+k],Q[k*n+j]));
	col[j] = sum;
      }
      for(int j=0;j<n;j++) Q_tmp[i*n+j] = col[j];
    }
    memcpy(Q,Q_tmp,n*n*sizeof(cdouble));
  }

#if TIME_EIGENSYSTEMS
  clock_t end_time = clock();
  eigensystem_cputime += (end_time-start_time)/(real_t)CLOCKS_PER_SEC;
#endif
  
  real_pair result;
  result.value[0] = max_error;
  result.value[1] = n_iterations;
  
  free(T); free(V); free(Qhat); free(D); free(L);
  return result;
}


#endif
