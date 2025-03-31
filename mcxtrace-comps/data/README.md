# X-ray scattering datafiles

This directory contains data-files for McXtrace components, e.g. reflectivity, 
structure, dynamics, absorption, geometry, etc.

## Reflectivity

Use the given python script `reflec_xraydb` to generate reflectivity data vs energy,angle
for (nearly) any material.

For instance:
```
./reflec_xraydb.py Pt
```


## Absorption

Most `.txt` files were obtained from NIST: http://physics.nist.gov/cgi-bin/ffast/ffast.pl

1) Use the tool `get_xray_db_data` - requires bash and wget, e.g. `./get_xray_db_data Mo Mo.txt`

2) Edit `Mo.txt` using an editor and strip out any html tag.

3) "Massage" the header to make it similar to

```
#Be (Z 4) 
#Atomic weight: A[r] 9.012180
#Nominal density: rho 1.8450
#sigma[a](barns/atom) = [mu/rho](cm^2 g^-1)  ×  1.49651E+01
#E(eV) [mu/rho](cm^2 g^-1) = f[2](e atom^-1)  ×  4.66927E+06
#2 edges. Edge energies (keV):
#
#
# K      1.11000E-01  L I    8.42000E-03
#Relativistic correction estimate f[rel] (H82,3/5CL) = -1.7882E-03, -6.0000E-04 e atom^-1
#Nuclear Thomson correction f[NT] = -9.7394E-04 e atom^-1
#
#━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
#Form Factors, Attenuation and Scattering Cross-sections
#Z=4, E = 0.001 - 430 keV
#
#      E            f[1]          f[2]        [mu/rho]      [sigma/rho]      [mu/rho]      [mu/rho][K]      lambda
#                                      Photoelectric Coh+inc      Total
#     keV        e atom^-1      e atom^-1   cm^2 g^-1       cm^2 g^-1      cm^2 g^-1   cm^2 g^-1     nm
```

