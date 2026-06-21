#ifndef MX_CRYSTALS_H
#define MX_CRYSTALS_H

%include "mccode-complex-lib"

enum {Mx_crystal_explicit=0, Mx_crystal_diamond, Mx_crystal_fcc,Mx_crystal_bcc};

/* make proper function declaration to be standards-compliant */
#pragma acc routine
void Mx_CubicCrystalChi(cdouble *chi0, cdouble *chih, double *k0mag, double *hscale, double *thetaB,
                         double f00, double f0h, double fp, double fpp, double V, int h, int k, int l,
                         double debye_waller_B, double E,
                         int crystal_type, double fscaler, double fscalei);

#pragma acc routine
int Mx_DiffractionDispersion(cdouble kqvals[4], cdouble xi0[4], cdouble xih[4],
        const double k0[3], const double nhat[3],
        const double H[3],
        cdouble chi0, cdouble chih_chihbar, double C, int nroots);

#pragma acc routine
int Mx_DarwinReflectivityBC(double *Rsig, double *Rpi, double kh[3],
	const double k0hat[3], const double nhat[3],
	const double alpha[3],
	cdouble chi0, cdouble chih, cdouble chihbar,
	double k0mag, double hscale, double thetaB);

#pragma acc routine
int Mx_LaueReflectivityBC(double *Rsig, double *Rpi, double *Tsig, double *Tpi,
	double *Asig, double *Api, // primary attenuation
	double kh[3],
	const double k0hat[3], const double nhat[3],
	const double alpha[3],
	cdouble chi0, cdouble chih, cdouble chihbar,
	double k0mag, double hscale, double thetaB, double thickness);

/*This is the old Darwin function*/
#pragma acc routine
void Mx_DarwinReflectivity(double *R, double *Thetah, double *Theta0, double *DeltaTheta0,
        double f00, double f0h, double fp, double fpp, double V, double alpha, int h, int k, int l,
        double debye_waller_B, double E, double Thetain, int pol,
        int crystal_type, double fscaler, double fscalei);

#pragma acc routine
void cross(double res[3], const double v1[3], const double v2[3], int unitize);

%include "read_table-lib"

#define vdot(a,b) (a[0]*b[0]+a[1]*b[1]+a[2]*b[2])
#define vplus(res, a, b) { res[0]=a[0]+b[0]; res[1]=a[1]+b[1]; res[2]=a[2]+b[2]; }
// solve a quadratic a x^2 + b x + c safely, a la Numerical Recipes.  r1 always has the larger magnitude
// a, b, c, r1, r2 are cdouble; sqrtfn is expected to be a cdouble->cdouble square root (e.g. csqrt)
#define qsolve(r1, r2, a, b, c, sqrtfn) { \
    if(creal(b) > 0) { r1=cdiv(csub(cneg(b),sqrtfn(csub(cmul(b,b),cmul(rmul(4.0,a),c)))),rmul(2.0,a)); } \
    else { r1=cdiv(cadd(cneg(b),sqrtfn(csub(cmul(b,b),cmul(rmul(4.0,a),c)))),rmul(2.0,a)); } \
    r2=cdiv(cdiv(c,a),r1); }
// real-coefficient counterpart of qsolve: a, b, c, r1, r2 are plain double; sqrtfn is e.g. sqrt.
// Native +,-,*,/ are fine here on both platforms since plain double is never the MSVC complex problem.
#define qsolve_real(r1, r2, a, b, c, sqrtfn) { \
    if(b > 0) { r1=(-b-sqrtfn(b*b-4*a*c))/(2*a); } \
    else { r1=(-b+sqrtfn(b*b-4*a*c))/(2*a); } \
    r2=(c/a)/r1; }

#endif /* MX_CRYSTALS_H SHARE section */
