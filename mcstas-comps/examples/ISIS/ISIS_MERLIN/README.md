# The `ISIS_MERLIN` Instrument

*McStas: MERLIN: High count rate, medium energy resolution, direct geometry chopper spectrometer.*

## Identification

- **Site:** ISIS
- **Author:** Rob Bewley
- **Origin:** <a href="http://www.isis.stfc.ac.uk">ISIS (UK)</a>
- **Date:** May 2017.

## Description

```text
Merlin has been in user operation since 2008. It was designed to be a high intensity, medium energy
resolution spectrometer that would complement the high-resolution MAPS spectrometer, and replace
the HET spectrometer. Merlin's design exploited recent advances in technology with a supermirror
guide to enhance flux, as well as 3 m long position-sensitive detectors in a vacuum, making it
ideal for the study of single crystals. The detector bank covers a massive \pi steradians of solid
angle with an angular range from -45 to 135 degrees in the horizontal plane, and \pm 30 degrees in
the vertical plane. This allows large swathes of Q,\omega space to be accessed in a single run.

Since 2014, Merlin has been running in event mode, and by using the new Gd chopper combined with a
disk chopper, it is able to run in repetition-rate multiplication (RRM) mode, allowing users to
simultaneously measure with several incident energies.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E_min | meV | lowest energy sampled      [meV] | 1.0 |
| E_max | meV | highest energy sampled     [meV] | 2000.0 |
| m | float | m value of guides | 3.0 |
| E_foc | meV | energy selected by chopper [meV] | 80.0 |
| nu_hz | Hz | frequency of chopper rotation [Hz] | 200.0 |
| type | int | chopper package selected sloppy chopper = 1, B chopper = 2, Gd chopper = 3 | 3 |

## Links

- [Source code](ISIS_MERLIN.instr) for `ISIS_MERLIN.instr`.
- <a href="http://www.isis.stfc.ac.uk/instruments/merlin">http://www.isis.stfc.ac.uk/instruments/merlin</a>

---
