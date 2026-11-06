// Bessel function of the first kind of order one, J1(x), usable on CPU and GPU (OpenACC).
// Replaces gsl_sf_bessel_J1 in the SANS contrib components.
// Double-precision Cephes j1 as used by sasmodels (lib/sas_J1.c), with names prefixed
// sans_ so it can be combined with the SasView components in one instrument.
// Absolute error 2.6e-16 (IEEE, 0 < x < 30).
// Cephes Math Library Release 2.8: June, 2000
// Copyright 1984, 1987, 1989, 2000 by Stephen L. Moshier
#ifndef SANS_J1_H
#define SANS_J1_H

static const double sans_RPJ1[4] = { -8.99971225705559398224E8, 4.52228297998194034323E11, -7.27494245221818276015E13, 3.68295732863852883286E15 };
static const double sans_RQJ1[8] = { 6.20836478118054335476E2,  2.56987256757748830383E5,  8.35146791431949253037E7,  2.21511595479792499675E10,
                                     4.74914122079991414898E12, 7.84369607876235854894E14, 8.95222336184627338078E16, 5.32278620332680085395E18 };
static const double sans_PPJ1[7] = { 7.62125616208173112003E-4, 7.31397056940917570436E-2, 1.12719608129684925192E0,  5.11207951146807644818E0,
                                     8.42404590141772420927E0,  5.21451598682361504063E0,  1.00000000000000000254E0 };
static const double sans_PQJ1[7] = { 5.71323128072548699714E-4, 6.88455908754495404082E-2, 1.10514232634061696926E0, 5.07386386128601488557E0,
                                     8.39985554327604159757E0,  5.20982848682361821619E0,  9.99999999999999997461E-1 };
static const double sans_QPJ1[8] = { 5.10862594750176621635E-2, 4.98213872951233449420E0, 7.58238284132545283818E1, 3.66779609360150777800E2,
                                     7.10856304998926107277E2,  5.97489612400613639965E2, 2.11688757100572135698E2, 2.52070205858023719784E1 };
static const double sans_QQJ1[7] = { 7.42373277035675149943E1, 1.05644886038262816351E3, 4.98641058337653607651E3, 9.56231892404756170795E3,
                                     7.99704160447350683650E3, 2.82619278517639096600E3, 3.36093607810698293419E2 };
#pragma acc declare copyin(sans_RPJ1[0:4], sans_RQJ1[0:8], sans_PPJ1[0:7], sans_PQJ1[0:7], sans_QPJ1[0:8], sans_QQJ1[0:7])

// Polynomial c[0]*x^N + ... + c[N]
#pragma acc routine seq
static double
sans_polevl (double x, const double* c, int N) {
  double ans = c[0];
  for (int i = 1; i <= N; i++)
    ans = ans * x + c[i];
  return ans;
}

// Polynomial x^N + c[0]*x^(N-1) + ... + c[N-1]
#pragma acc routine seq
static double
sans_p1evl (double x, const double* c, int N) {
  double ans = x + c[0];
  for (int i = 1; i < N; i++)
    ans = ans * x + c[i];
  return ans;
}

#pragma acc routine seq
static double
sans_J1 (double x) {
  const double Z1 = 1.46819706421238932572E1;
  const double Z2 = 4.92184563216946036703E1;
  const double SQRT1_PI = 0.56418958354775628;
  double ax = fabs (x), sign = x < 0 ? -1.0 : 1.0;
  double w, z, p, q;

  if (ax <= 5.0) {
    z = ax * ax;
    w = sans_polevl (z, sans_RPJ1, 3) / sans_p1evl (z, sans_RQJ1, 8);
    return sign * w * ax * (z - Z1) * (z - Z2);
  }
  w = 5.0 / ax;
  z = w * w;
  p = sans_polevl (z, sans_PPJ1, 6) / sans_polevl (z, sans_PQJ1, 6);
  q = sans_polevl (z, sans_QPJ1, 7) / sans_p1evl (z, sans_QQJ1, 7);
  p = p * (sin (ax) - cos (ax)) + w * q * (sin (ax) + cos (ax));
  return sign * p * SQRT1_PI / sqrt (ax);
}

#endif
