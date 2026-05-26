# The `Pump_probe_solvent` Instrument

*McXtrace: Design study of a pump and probe type instrument. The sample is an excitable molecule*

## Identification

- **Site:** DTU
- **Author:** Erik B Knudsen
- **Origin:** DTU Fysik
- **Date:** \today

## Description

```text
This is an example instrument for simulating time resolved scattering from
optically excitable, disordered molecules in solution, including scattering
signal from the solvent. By FSOLV it is possible to change the concentration of
solute.

This is to be considered a concept instrument model, i.e. it is _not_ a model of
an existing real instrument. A simple flat source emitting photons impinge on a chopper,
defining an X-ray pulse. Timing jitter in the chopper may be included through the JITTER
parameter which introduces a random uncertainty in the timing of the pulse.

The X-ray pulse is allowed to propagate to the sample which is excited by an instantaneous laser
pulse at t=0. The whole sample is assumed to be excited uniformly.
A Monte Carlo choice between interacting with solvent or solute is performed at the sample position.
The chopper is set such that the X-ray pulse front will arive at the sample at t=Dt. Thus,
setting Dt<-tau (chopper opening time) will simulate the "laser-off" setting.

The sampling of the scattered signal may be controlled by PSIMAX,PSIMIN and ETAMAX,ETAMIN
parameters. Please be aware the by restricting the azimuthal range it is not uncommon to underfill
the acceptance area of subsequent components. This is particularly the case for small psi.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| excitation_prob | 1 | Fraction of the molecules in solution that are excited at t=0. | 0.2 |
| Dt | 1 | Time delay between laser pulse exciting the sample and the x-ray pulse. | 100e-9 |
| PSIMIN | deg. | Minimum scattering angle to be simulated. | 0 |
| PSIMAX | deg. | Maximum scattering angle to be simulated. | 50 |
| FSOLV |  | Volume fraction of solvent to solute. ( =1 means only solvent, =0 only solute) | 0 |
| JITTER | s | Jitter in chopper. | 0 |
| ETAMIN | deg. | Minimum azimuthal angle for scattered photons. | -180 |
| ETAMAX | deg. | Maximum azimuthal angle for scattered photons. | 180 |
| NOSOLVENT |  | If nonzero - ignore scattering from solvent. | 0 |
| NOSOLUTE |  | If nonzeor - ignore scattering from solute. | 0 |

## Links

- [Source code](Pump_probe_solvent.instr) for `Pump_probe_solvent.instr`.

---
