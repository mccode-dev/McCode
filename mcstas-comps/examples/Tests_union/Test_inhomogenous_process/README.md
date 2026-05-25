# The `Test_inhomogenous_process` Instrument

*McStas: Test of inhomogenous incoherent union component,
and comparison to incoherent process, and incoherent component.*

## Identification

- **Site:** Tests_union
- **Author:** Daniel Lomholt Christensen
- **Origin:** UCPH@NBI
- **Date:** 18/03/2026

## Description

```text
Test of different setups with the Inhomogenous_incoherent_process.
For all setups, the idea is the same. A source shines neutrons on the box,
and a detector placed right up on the side of the box, measures the
incoherently scattered neutrons.
The first three have a constant scattering cross section throughout their
depths:
1 is the normal incoherent component,
1 is a union box with an incoherent process
1 is a union box with the inhomogenous incoherent process
Then the next three setups showcase a linearly rising scattering cross section
in the first example, this linearly increasing cross section is implemented
by 10 boxes utilizing the original inocherent process union component.

Then the inhomogenous incoherent component implements it with its simple syntax

Then a union box that uses the inhomogenous process
and the original incoherent process is implemented.


The 7th example examines the if the component works when a phonon component
is present. It is a lot slower than the others, and is therefore run with
fewer neutrons

The 8th and 9th examples show an intuitive image of the transmitted beam.
Here again the scattering cross section scales linearly, but due to the
nature of the two different processes, the one process curve becomes
continous (Inhomogenous_incoherent_process),
and the other discrete (Incoherent_process).

The 10th example plays with some more complex tiny expressions using the
gaussian function and the heavyside function.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| thick | m | :           Thickness of box samples | 0.01 |
| width | m | :           Width of the box samples | 0.03 |
| sample | string | :      Parameter choosing the specific sample. see %Examples for the options | "thin" |

## Links

- [Source code](Test_inhomogenous_process.instr) for `Test_inhomogenous_process.instr`.
- [Additional information](Test_inhomogenous_process.md)

---
