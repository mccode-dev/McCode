#ifndef MX_CRYSTALS_C
#define MX_CRYSTALS_C

void Mx_CubicCrystalChi(cdouble *chi0, cdouble *chih, double *k0mag, double *hscale, double *thetaB,
                         double f00, double f0h, double fp, double fpp, double V, int h, int k, int l,
                         double debye_waller_B, double E,
                         int crystal_type, double fscaler, double fscalei)
{
    double lambda, a, d;
    
    lambda = 2*PI/(E2K*E);  			/* wavelength in Å, E in keV, using built-in constants for safety    */
    a = cbrt(V); 				/* side length of unit cubic cell (Å)*/
    d = a/sqrt(h*h + k*k + l*l); 		/* d-spacing (Å)*/

    if (lambda>2*d){
      /*cannot close scattering triangle, set all returns to 0 and exit*/
      *thetaB=0;
      *k0mag=0;
      *hscale=0;
      *chi0=cplx(0.0,0.0);
      *chih=cplx(0.0,0.0);
      return;
    }

    *thetaB = asin(lambda/(2*d));  		/* kinematical bragg angle (rad) */
    *k0mag=2.0*M_PI/lambda; /* magnitude of k0 consistent with energy scale */
    *hscale=2.0*M_PI/d; /* minus sign to make it point into the crystal */
    
    /* structure factor rules from:
     https://en.wikipedia.org/wiki/Structure_factor section on diamond cubic crystals
     */
    
    cdouble fscaleh;
    double fscale0;
    
    switch(crystal_type) {
        case Mx_crystal_explicit:
            /* use explicitly provided structure factor scale factor */
            break;
        case Mx_crystal_diamond: /* diamond lattice rules */
            if (((h+k+l)%2) != 0){ 		/* (111) etc. odd sum eflection */
                fscaleh=cplx(4.0,4.0);
                fscale0=8;
            }
            else if (((h+k+l)%4)==0){ 		/* (400) etc. h+k+l=4n reflection */
                fscaleh=cplx(8.0,0.0);
                fscale0=8;
            } else {
                /* any other reflection is forbidden, will get a divide-by-zero somewhere, but user is
                 responsible for only using allowed reflections */
                fscaleh=cplx(0.0,0.0);
                fscale0=0;
            }
            break;
        case Mx_crystal_fcc: /* fcc lattice rules */
        {
            int hpar=h%2, kpar=k%2, lpar=l%2;
            if ( hpar==kpar && kpar==lpar ) { /* all parities the same */
                fscaleh=cplx(4.0,0.0);
                fscale0=4;
            }
            else { 		/* mixed parity forbidden */
                fscaleh=cplx(0.0,0.0);
                fscale0=0;
            }
        }
            break;
        case Mx_crystal_bcc: /* bcc lattice rules */
            if ( ((h+k+l)%2) == 0 ) { /* h+k+l even */
                fscaleh=cplx(2.0,0.0);
                fscale0=2;
            }
            else { 		/* otherwise forbidden */
                fscaleh=cplx(0.0,0.0);
                fscale0=0;
            }
            break;
        default:
            fscale0=0;
            fscaleh=cplx(0.0,0.0); /* fail later if unknown crystal type */
            break;
    }
    
    cdouble F0=rmul(fscale0, cplx(f00+fp, fpp)); // NEVER CHECKED for crystals other than diamond structure
    cdouble Fh=rmul(cabs(fscaleh), cplx(f0h+fp, fpp));
    
    double GAMMA=RE*lambda*lambda/(PI*V);
    double M=debye_waller_B*SQR(sin(*thetaB)/lambda)*(2./3.); /* isotropic temperature factor */
    *chi0 = rmul(GAMMA, cneg(F0));
    *chih = rmul(GAMMA*exp(-M), cneg(Fh));
}

int Mx_DarwinReflectivityBC(double *Rsig, double *Rpi, double kh[3],
                         const double k0hat[3], const double nhat[3],
                         const double alpha[3],
                         cdouble chi0, cdouble chih, cdouble chihbar,
                         double k0mag, double hscale, double thetaB
                         )
{
    double k0[3]={k0hat[0]*k0mag, k0hat[1]*k0mag, k0hat[2]*k0mag}; /* actual incoming k vector */
    double H[3]={alpha[0]*hscale, alpha[1]*hscale, alpha[2]*hscale};
    
    /* now we have to solve for the free-space kh vector outside the crystal.
     The requirement is that kh = Kh + q2 * nhat, and is pure real and has the same magnitude as k0.
     Thus, kh = k0 + q1 * nhat + H + q2 * nhat, |kh| = |k0|, and (q1 + q2) must be real to make kh real.
     if (q1 + q2) == q,
     q^2 + H^2 + 2 q (k0+H).nhat + 2 k0.H = 0
     Note that this is independent of the messy dispersion relationship, and the same for both polarizations.
     */
    double k0plusH[3];
    vplus(k0plusH, k0, H); // temporary k0+H as needed in second poly term above
    double bb=2*vdot(k0plusH, nhat);
    double cc = vdot(H,H) +2*vdot(k0,H);
    double root1, root2;
    qsolve_real(root1, root2, 1., bb, cc, sqrt); //root2 is the small root
    vplus(kh, k0plusH, root2*nhat);
    
    for (int i=0; i<2; i++) { // do reflectivity using shared geometry for both polarizations
        double C=(i==0)?fabs(cos(2*thetaB)) : 1; // polarization factor
        cdouble xi0;
        double K0[3], Kh[3];
        cdouble kqvals[4], xi0vals[4], xihvals[4];

        int fail=Mx_DiffractionDispersion(kqvals, xi0vals, xihvals,
                   k0, nhat, H, chi0, cmul(chih,chihbar), C, 1); // get first (interesting) root only

        cdouble kq=kqvals[0];
        xi0=xi0vals[0];
        vplus(K0, k0, creal(kq)*nhat);
        vplus(Kh, K0, H);

        /* compute the asymmetry b which is the ratio of the sizes of the footprint of the beam on the crystal */
        double b = (vdot(K0, nhat)/sqrt(vdot(K0,K0)))/(vdot(Kh, nhat)/sqrt(vdot(Kh, Kh)));
        
        if(fail) {
            *Rsig=0;
            *Rpi=0;
            return 1;
        }
        
        cdouble eq24=cdiv(rmul(2.0,xi0), rmul(k0mag*C,chih)); // B&C equation 24
        double eratio=cabs(eq24);
        
#ifdef MCDEBUG
#ifndef OPENACC
        fprintf(stderr,
                "Bragg Geometry results: k0=(%.3f %.3f %.3f), kh=(%.3f %.3f %.3f) "
                "xi0 = (%.3e + %.3e i) "
                "b0=%.4f "
                "eq24 = (%.3e + %.3e i) "
                "R = %.3e "
                "\n",
                k0[0], k0[1], k0[2], kh[0], kh[1], kh[2],
                creal(xi0), cimag(xi0),
                b,
                creal(eq24), cimag(eq24), eratio*eratio/fabs(b));
#endif
#endif
        
        if(i==0)  *Rpi=eratio*eratio/fabs(b); // the fabs(b0) is the footprint correction
        else      *Rsig=eratio*eratio/fabs(b);
    }
    return 0;
}

void cross(double res[3], const double v1[3], const double v2[3], int unitize)
{   // use our own cross product which takes arrays directly to reduce pointer shuffling
    res[0]=v1[1]*v2[2]-v1[2]*v2[1];
    res[1]=v1[2]*v2[0]-v1[0]*v2[2];
    res[2]=v1[0]*v2[1]-v1[1]*v2[0];
    if(unitize) {
        double l=sqrt(res[0]*res[0]+res[1]*res[1]+res[2]*res[2]);
        res[0]/=l;
        res[1]/=l;
        res[2]/=l;
    }
}
    
int Mx_DiffractionDispersion(cdouble kqvals[4], cdouble xi0[4], cdouble xih[4],
                          const double k0[3], const double nhat[3],
                          const double H[3],
                          cdouble chi0, cdouble chih_chihbar, double C, int nroots){
    /* compute the Batterman & Cole eq. 17 dispersion relation, and associated quantities.
       nroots sets how many roots to find.  They are sorted, with the first root being the attenuated incoming ray,
       then the amplified incoming ray, then the two 'big roots'.
       Normally only nroots=1 is interesting for Bragg diffraction.
     */

    double k0mag=sqrt(vdot(k0,k0));
    
    /* B&C equation 17 is ( |k0 + kq nhat|^2 - k0^2(1+chi0) ) ( |k0 + kq nhat + H|^2 - k0^2(1+chi0) ) = k^4 C^2 chih chihbar */
    double k0plusH[3];
    vplus(k0plusH, k0, H); // temporary k0+H as needed in second poly term above
    cdouble poly1[3], poly2[3], poly[5], deriv[4];
    poly1[2]=cplx(1.0,0.0);
    poly1[1]=cplx(2*vdot(k0,nhat), 0.0);
    poly1[0]=rmul(-k0mag*k0mag, chi0);
    poly2[2]=cplx(1.0,0.0);
    poly2[1]=cplx(2*vdot(k0plusH,nhat), 0.0);
    poly2[0]=radd(2*vdot(k0,H), radd(vdot(H,H), rmul(-k0mag*k0mag, chi0)));
    /* for multiplying these polys, just write it out explicitly */
    poly[0]=cmul(poly1[0],poly2[0]);
    poly[1]=cadd(cmul(poly1[0],poly2[1]), cmul(poly1[1],poly2[0]));
    poly[2]=cadd(cadd(cmul(poly1[0],poly2[2]), cmul(poly1[1],poly2[1])), cmul(poly1[2],poly2[0]));
    poly[3]=cadd(cmul(poly1[1],poly2[2]), cmul(poly1[2],poly2[1]));
    poly[4]=cmul(poly1[2],poly2[2]);
    cdouble epsilon=rmul(k0mag*k0mag*k0mag*k0mag*C*C, chih_chihbar); // right-hand side of dispersion
    poly[0]=csub(poly[0],epsilon);
    deriv[3]=rmul(4.0,poly[4]);
    deriv[2]=rmul(3.0,poly[3]);
    deriv[1]=rmul(2.0,poly[2]);
    deriv[0]=rmul(1.0,poly[1]);
    
    /* the dispersion polynomial is p1*p2-epsilon = 0; first, find big roots and factor out to leave only a quadratic with small (interesting) roots */
    /* big roots will be _close_ to big roots of p1 and p2, use these as guesses */
    cdouble p1b, p1s, p2b, p2s;
    // p1b and p2b are always larger root in magnitude
    qsolve(p1b, p1s, poly1[2], poly1[1], poly1[0], csqrt);
    qsolve(p2b, p2s, poly2[2], poly2[1], poly2[0], csqrt);
    
    /* now, (x-p1b)(x-p2b)(x-p1s)(x-p2s)-epsilon is the full poly, but near the interesting region around p1s, p2s, flatten first two terms */
    /* so x0 = (p1s+p2s)*0.5, this is (x-p1s)(x-p2s)-epsilon/((x0-p1b)*(x0-p2b)) = 0 to get approximate roots of full poly */
    cdouble x0=rmul(0.5, cadd(p1s,p2s));
    cdouble fp[3] = { csub(cmul(p1s,p2s), cdiv(epsilon, cmul(csub(x0,p1b), csub(x0,p2b)))),
                      cneg(cadd(p1s,p2s)),
                      cplx(1.0,0.0) };
    // factored poly roots, should be very close to exact roots
    // practical note:  looking at convergence, these roots are so close, one could probably drop the Newton's method refinement
    // completely.  However, since this code is intended to be pedantically correct, I will do the refinement.
    cdouble initroots[4];
    qsolve(initroots[0], initroots[1], fp[2], fp[1], fp[0], csqrt); // solution to factored polynomial is starting point for newton's method
    
    int rootloops, fail=0;
    // make sure the first root has im(k).re(k) < 0 so beam is attenuated. Since re(k) is nearly k0 and im(k) is im(kq)*nhat,
    // we need k0.nhat * im(k) < 0
    double kdotnhat=vdot(k0, nhat);
    if(cimag(initroots[0])*kdotnhat > 0) {
        if(cimag(initroots[1])*kdotnhat < 0) {
            cdouble swap;
            swap=initroots[0]; initroots[0]=initroots[1]; initroots[1]=swap;
        } else {
            // neither root has im(k) < 0, no physically possible solution.  Should never happen.
#ifndef OPENACC
            fprintf(stderr, "PerfectCrystal: No attenuating solution for incoming vector found, r1=(%.3e + %.3e i) 	r2=(%.3e + %.3e i) \n",
                    creal(initroots[0]), cimag(initroots[0]), creal(initroots[1]), cimag(initroots[1]));
#endif
            return -1;
        }
    }
    initroots[2]=p1b;
    initroots[3]=p2b; // the two big roots are last in the list

    for(rootloops=0; rootloops < nroots; rootloops++) {
        cdouble dd, kq, pv, dv;
        kq=initroots[rootloops];
#if MCDEBUG
#ifndef OPENACC
        fprintf(stderr,"Batterman Cole dispersion Newton Iterations, starting at root =(%.3e + %.3e i) \n", creal(kq), cimag(kq));
#endif
#endif
        int stepcount=0;
        do {
            pv=poly[4]; // poly value, evaluated via Horner's method
            pv=cadd(cmul(pv,kq),poly[3]);
            pv=cadd(cmul(pv,kq),poly[2]);
            pv=cadd(cmul(pv,kq),poly[1]);
            pv=cadd(cmul(pv,kq),poly[0]);
            dv=deriv[3]; // derivative value, evaluated via Horner's method
            dv=cadd(cmul(dv,kq),deriv[2]);
            dv=cadd(cmul(dv,kq),deriv[1]);
            dv=cadd(cmul(dv,kq),deriv[0]);
            dd=cdiv(cneg(pv),dv); // Newton's method step
            kq=cadd(kq,dd);
            stepcount++;
#ifdef MCDEBUG_EXTRA
#ifndef OPENACC
        fprintf(stderr,"Batterman Cole dispersion Newton Iterations, starting at root =(%.3e + %.3e i) \n",creal(kq), cimag(kq));
	fprintf(stderr,"Batterman Cole dispersion Newton Iterations, step count=%d, pv=%.3e dv=%.3e "
		"kq=(%.3e + %.3e I) shift=%.3e\n",
		stepcount, cabs(pv), cabs(dv), creal(kq), cimag(kq), cabs(dd) );
#endif
#endif
        } while ( ((cabs(dd) > 1e-15) && (stepcount < 20)) );
        kqvals[rootloops]=kq;

        fail= (stepcount == 20);
        if(fail) { // should never happen. always converges in about 3 steps!
#ifndef OPENACC
            fprintf(stderr,"****Newton's method convergence failure in Bragg_Geometry, killing particle!\n");
#endif
            kq=initroots[rootloops]; // leave offset plausible, close to quadratic start
        }


        // batterman and cole eq. 18, exact definitions of xi0 and xih
        // but adjust for numerical stability.
        // The smaller of xi0, xih depends on delicate cancellation of the polynomial.  The larger doesn't.
        // Thus, replace the small polynomial with epsilon / (large one)
        // This is equivalent to the Numerical Recipes trick for stable computation of quadratic roots
        cdouble poly1v=poly1[2];
        poly1v=cadd(cmul(poly1v,kq),poly1[1]);
        poly1v=cadd(cmul(poly1v,kq),poly1[0]);
        cdouble poly2v=poly2[2];
        poly2v=cadd(cmul(poly2v,kq),poly2[1]);
        poly2v=cadd(cmul(poly2v,kq),poly2[0]);
        if( fabs(creal(poly1v)) < fabs(creal(poly2v)) ) {
            poly1v=cdiv(epsilon,poly2v);
        } else {
            poly2v=cdiv(epsilon,poly1v);
        }
        xi0[rootloops]=rmul(1.0/(2*k0mag), poly1v);
        xih[rootloops]=rmul(1.0/(2*k0mag), poly2v);
        
    }
    return fail;
}

int Mx_LaueReflectivityBC(double *Rsig, double *Rpi, double *Tsig, double *Tpi,
                         double *Asig, double *Api, // primary attenuation
                         double kh[3],
                         const double k0hat[3], const double nhat[3],
                         const double alpha[3],
                         cdouble chi0, cdouble chih, cdouble chihbar,
                         double k0mag, double hscale, double thetaB, double thickness)
{
    double k0[3]={k0hat[0]*k0mag, k0hat[1]*k0mag, k0hat[2]*k0mag}; /* actual incoming k vector */
    double H[3]={alpha[0]*hscale, alpha[1]*hscale, alpha[2]*hscale};

    /* a bunch of precomputed dot products we will use over and over.*/
    double k0dotnhat=vdot(k0,nhat);
    double Hdotnhat=vdot(H, nhat);
    double k0dotH=vdot(k0,H);
    double HdotH=vdot(H,H);
    
    /* now we have to solve for the free-space kh vector outside the crystal.
     The requirement is that kh = Kh + e * nhat, and is pure real and has the same magnitude as k0.
     Thus, kh = k0 + kq*nhat + H + e*nhat, |kh| = |k0|, and kq+e must be real to make kh real.
     if kq+e == q,
     q^2 + H^2 + 2 q (k0+H).nhat + 2 k0.H = 0
     Note that this is independent of the messy dispersion relationship, and the same for both polarizations.
     */
    double k0plusH[3];
    vplus(k0plusH, k0, H);
    double bb=2*(k0dotnhat+Hdotnhat);
    double cc = HdotH +2*k0dotH;
    double root1, root2;
    qsolve_real(root1, root2, 1., bb, cc, sqrt); //root2 is the small root
    vplus(kh, k0plusH, root2*nhat);

    for (int i=0; i<2; i++) { // do reflectivity using shared geometry for both polarizations
        double C=(i==0)?fabs(cos(2*thetaB)) : 1; // polarization factor
        cdouble kqvals[4], xi0vals[4], xihvals[4];

        int fail=Mx_DiffractionDispersion(kqvals, xi0vals, xihvals,
                   k0, nhat, H, chi0, cmul(chih,chihbar), C, 2); // get first 2 (alpha and beta) roots only

        cdouble a1=cdiv(rmul(2.0,xi0vals[0]), rmul(k0mag*C,chih));  // complex Eha/E0a from B&C eq. 24
        cdouble a2=cdiv(rmul(2.0,xi0vals[1]), rmul(k0mag*C,chih));  // complex Ehb/E0a from B&C eq. 24

        // compute amplitudes from solving B&C eqns. 40
        cdouble i1=cdiv(a2,csub(a2,a1)); // e0a/e0i
        cdouble i2=cdiv(cneg(a1),csub(a2,a1)); // e0a/e0i
        cdouble ix=cplx(0.0,-thickness*1e10); // convert thickness to angstroms for absorption

        // factor out main K vector and difference explicitly,
        // so that dphi, the relative phase & amplitude of the alpha and beta
        // rays can be calculated to high accuracy
        // note: K0=k0 + r nhat, so K0.nhat = k0.nhat + r
        // similarly for Kh=k0 + H + r nhat so Kh.nhat = k0.nhat + H.nhat + r

        // we don't actually use the main phase (yet), so only compute real part
        // of transport exponentials
        // double complex phi0t=cexp((k0dotnhat+kqvals[0])*ix) #alpha transmitted wave factor
        // double complex phi0r=cexp((k0dotnhat+Hdotnhat+kqvals[0])*ix) #alpha reflected factor
        double phi0t=exp(creal(cmul(radd(k0dotnhat,kqvals[0]),ix))); // alpha transmitted wave factor
        double phi0r=exp(creal(cmul(radd(k0dotnhat+Hdotnhat,kqvals[0]),ix))); // alpha reflected factor
        cdouble dphi=cexp(cmul(csub(kqvals[1],kqvals[0]),ix)); // beta-alpha transmission ratio

        cdouble t0zz=rmul(phi0t, cadd(i1,cmul(i2,dphi))); // transmitted complex field amplitude at exit
        cdouble thzz=rmul(phi0r, cadd(cmul(a1,i1),cmul(cmul(a2,i2),dphi))); // reflected complex field amplitude at exit
#ifdef MCDEBUG
#ifndef OPENACC
        fprintf(stderr, "LAUE: "
        " k0 = (%.3f %.3f %.3f) thickness=%.3e"
        " a1=(%.3f + %.3fj) "
        " a2=(%.3f + %.3fj) "
        " i1=(%.3f + %.3fj) "
        " i2=(%.3f + %.3fj) "
        " phi0t = %.3e phi0r = %.3e "
        " dphi=(%.3f + %.3fj) "
        " t0zz=(%.3f + %.3fj) "
        " thzz=(%.3f + %.3fj) "
        " \n",
        k0[0], k0[1], k0[2], thickness,
        creal(a1), cimag(a1),
        creal(a2), cimag(a2),
        creal(i1), cimag(i1),
        creal(i2), cimag(i2),
        phi0t, phi0r,
        creal(dphi), cimag(dphi),
        creal(t0zz), cimag(t0zz),
        creal(thzz), cimag(thzz)
        );
#endif
#endif

        /* compute the asymmetry b which is the ratio of the sizes of the footprint of the beam on the crystal.
        Assume it is the same for alpha and beta branches!
        */
        double K0[3], Kh[3];
        vplus(K0, k0, creal(kqvals[0])*nhat);
        vplus(Kh, K0, H);
        double b = (vdot(K0, nhat)/sqrt(vdot(K0,K0)))/(vdot(Kh, nhat)/sqrt(vdot(Kh, Kh)));

        double R=cabs(cmul(thzz,thzz))/fabs(b); // the fabs(b) is the footprint correction
        double T=cabs(cmul(t0zz,t0zz))/fabs(b);

        if(i==0) {
            *Rpi= R; *Tpi= T; *Api=phi0t*phi0t;
        } else {
            *Rsig=R; *Tsig=T; *Asig=phi0t*phi0t;
        }
    }
    return 0;
}

/*This is the old Darwin function*/ 
void Mx_DarwinReflectivity(double *R, double *Thetah, double *Theta0, double *DeltaTheta0,
                             double f00, double f0h, double fp, double fpp, double V, double alpha, int h, int k, int l,
                             double debye_waller_B, double E, double Thetain, int pol,
                            int crystal_type, double fscaler, double fscalei
                             )
{

    double lambda,theta,theta0,DeltaThetas,a,d,b,C,W,kappa,g,L;
    double F0r,F0i,Fhr,Fhi,psi0r,psi0i,psihr,psihi;

    lambda = 2*PI/(E2K*E);  			/* wavelength in Å, E in keV, using built-in constants for safety    */
    a = cbrt(V); 				/* side length of unit cubic cell (Å)*/
    d = a/sqrt(h*h + k*k + l*l); 		/* d-spacing (Å)*/
    theta = asin(lambda/(2*d));  		/* kinematical bragg angle (rad) */
    b = sin(theta + alpha)/sin(theta - alpha);  /* asymmetry factor */

    *Theta0 = Thetain - alpha; 			/* (rad) angle between Bragg planes and incident ray */
    *Thetah = b*(*Theta0 - theta) + theta;   	/* (rad) Angle betweeb Bragg planes and reflected ray */
    /*check if Bragg angle is less than alpha. If so return 0 reflectivity*/
    if (theta<alpha) {
        *R=0;
        *DeltaTheta0 = -1; /*to mark it irrelevant*/
    }

    /* Define polarization factor: */
    switch(pol){
        case 0:
            C = (1 + fabs(cos(2*theta)))/2;         	/* unpolarized */
            break;
        case 1:
            C = fabs(cos(2*theta));  		/* polarization in the scattering plane */
            break;
        case 2:
            C = 1;                          	/* polarization perpendicular to the scattering plane*/
            break;
    }

    /* structure factor rules from:
https://en.wikipedia.org/wiki/Structure_factor section on diamond cubic crystals
     */

    switch(crystal_type) {
        case Mx_crystal_explicit:
            /* use explicitly provided structure factor scale factor */
            break;
        case Mx_crystal_diamond: /* diamond lattice rules */
            if (((h+k+l)%2) != 0){ 		/* (111) etc. odd sum eflection */
                fscaler=fscalei=4.0;
            }
            else if (((h+k+l)%4)==0){ 		/* (400) etc. h+k+l=4n reflection */
                fscaler=8; fscalei=0;
            } else {
                /* any other reflection is forbidden, will get a divide-by-zero somewhere, but user is
                   responsible for only using allowed reflections */
                fscaler=0; fscalei=0;
            }
            break;
        case Mx_crystal_fcc: /* fcc lattice rules */
            {
                int hpar=h%2, kpar=k%2, lpar=l%2;
                if ( hpar==kpar && kpar==lpar ) { /* all parities the same */
                    fscaler=4.0; fscalei=0.0;
                }
                else { 		/* mixed parity forbidden */
                    fscaler=0; fscalei=0;
                }
            }
            break;
        case Mx_crystal_bcc: /* bcc lattice rules */
            if ( ((h+k+l)%2) == 0 ) { /* h+k+l even */
                fscaler=2.0; fscalei=0.0;
            }
            else { 		/* otherwise forbidden */
                fscaler=0; fscalei=0;
            }
            break;
        default:
            fscaler=0; fscalei=0; /* fail later if unknown crystal type */
            break;
    }

    F0r=8*(f00+fp);
    F0i=8*fpp;
    double scalemag=sqrt(fscaler*fscaler+fscalei*fscalei);
    Fhr=scalemag*(f0h+fp);
    Fhi=scalemag*fpp;

    double main_scale=RE*lambda*lambda/(PI*V);
    double M=debye_waller_B*SQR(sin(Thetain)/lambda)*(2./3.); /* temperature factor */
    psi0r = F0r*main_scale;
    psi0i = F0i*main_scale;
    psihr = Fhr*main_scale*exp(-M); /* Eq 23*/
    psihi = Fhi*main_scale*exp(-M); /* only angle-dependent part gets scaled by temp factor */

    W = 0.5 * (sqrt(b) + 1/sqrt(b)) * psi0r/(C * psihr) +  sqrt(b)*sin(2*theta)*(theta - *Theta0)/(C * psihr); /* eq 28*/
    kappa = psihi/psihr;                                              	/* eq 22 */
    g = 0.5*(sqrt(b) + 1/sqrt(b))*psi0i/(C*psihr);               	/* eq 21 */
    L = (1/(1 + kappa*kappa))*( W*W + g*g + sqrt(SQR(W*W - g*g - 1 + kappa*kappa) + 4*SQR(g*W - kappa)));

    /* *R = L - sqrt(L*L - 1); */
    /* replace x-sqrt(x^2-1) with exactly equal 1/(x+sqrt(x^2-1)) to avoid roundoff when x is large ... MHM */
    *R = 1/(L + sqrt(L*L - 1));

    DeltaThetas = psi0r/sin(2*theta);               	/* eq 32 */
#ifdef MCDEBUG
#ifndef OPENACC
    printf("E,lambda= %f , %f \n",E,lambda);
    printf("theta= %f \n",theta*180/PI);
    printf("Theta0= %f \n",*Theta0*180/PI);
    printf("theta = %g rad, alpha=%g rad.\n",theta,alpha);
    printf("b,sqrt(b)= %f %f\n",b,sqrt(b));
    printf("1/sqrt(b)= %f \n",1/sqrt(b));
    printf("Fhr, Fhi, F0r, F0i= %g %g %g %g\n",Fhr, Fhi, F0r, F0i);
    printf("psihr, psihi, psi0r, psi0i= %g %g %g %g\n",psihr, psihi, psi0r, psi0i);
    printf("sqrt(b)*sin(2*theta)= %g \n",sqrt(b)*sin(2*theta));
    printf("C, pis0r,C * psihr= %g %g %g\n",C, psi0r,C * psihr);
    printf("W= %f \n",W);
    printf("kappa= %f \n",kappa);
    printf("g= %f \n",g);
    printf("L= %f \n",L);
    printf("R= %f \n",*R);
    printf("DeltaThetas %f \n",3600*DeltaThetas*180/PI);
#endif
#endif
    *DeltaTheta0 = 0.5*(1 + 1/b)*DeltaThetas;                        	/* center of reflectivity curve is at theta + DeltaTheta0 eq 31 */
}


#endif /* MX_CRYSTALS_C */
