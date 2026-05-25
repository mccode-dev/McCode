# The `SE_example` Instrument

*McStas: Mockup of transmission Spin-Echo, written for PNCMI 2010 school in Delft.*

## Identification

- **Site:** Tests_polarization
- **Author:** Erik Knudsen, Peter Willendrup
- **Origin:** Risoe
- **Date:** June 2010

## Description

```text
This instrument shows the use of essential McStas polarisation components, including Pol_mirror, Pol_Bfield and MeanPolLambda_monitor.

Pol_mirror is used to polarize and analyze the incoming and scattered beams.
Pol_Bfield is used to define guidefields and flippers.
MeanPolLambda_montior is used to monitor beam polarisation.

Example: mcrun SE_example.instr dBz=-0.0001,0.0001 -N41 -n1e5
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| POL_ANGLE | deg | Reflection angle of polarizer/analyzer | 2 |
| SAMPLE | 1 | Flag to include (0) or exclude incoherent scatterer | 0 |
| Bguide | T | Field magnitude in guide fields | 0.1 |
| Bflip | T | Magnitude of flipper fields, | 3e-4 |
| dBz | T | Magnitude field perturbation in first guide field | 0 |
| Lam | Angstrom | Mean wavelength of neutrons emitted from source | 8 |
| dLam | Angstrom | Wavelength spread of neutrons emitted from source | 0.8 |

## Links

- [Source code](SE_example.instr) for `SE_example.instr`.
- Requires mcstas 1.12b (or beta). PolLambda_monitor has been modified to work (nL->nlam)

---
