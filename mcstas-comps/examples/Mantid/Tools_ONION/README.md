# The `Tools_ONION` Instrument

*McStas: Test case for an layered detector shell instrument to provide quick overview of resolution per pixel.*

## Identification

- **Site:** Mantid
- **Author:** Thomas Huegle
- **Origin:** ORNL
- **Date:** July 2019

## Description

```text
Test case for an layered detector shell instrument to provide quick overview of resolution per pixel.
Set up to be run in McStas -> Mantid mode, refer to manual for details on execution.
Simulates desired divergence by adjusting moderator size.
Considers only in-plane (xwidth) factors, sets yheight as 1 cm.
For an example on how to analyze the data, see the 'Onion_analyzerScript.py' available at
<a href="https://github.com/mccode-dev/McCode-contribution-support-docs/tree/master/McStas/2.6_Onion_tool_T_Huegle">https://github.com/mccode-dev/McCode-contribution-support-docs/tree/master/McStas/2.6_Onion_tool_T_Huegle</a>

Acceptable statistics can be achieved by running the simulation with 1E7 neutrons (output file: ~400MB)
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| divergence | degrees | angular divergence of neutrons reaching the sample | 0.4 |
| distance | m | distance between source and sample | 32 |
| filename |  |  | "source_sct521_bu_08_1.dat" |

## Links

- [Source code](Tools_ONION.instr) for `Tools_ONION.instr`.
- "Using an onion like neutron scattering instrument model to quickly optimize design parameters"
- T. Huegle, <a href="https://doi.org/10.1016/j.nima.2019.162711">Nuclear Instruments and Methods in Physics Research Section A, 2019<a>
- Python script available at <a href="https://github.com/mccode-dev/McCode-contribution-support-docs/tree/master/McStas/2.6_Onion_tool_T_Huegle">https://github.com/mccode-dev/McCode-contribution-support-docs/tree/master/McStas/2.6_Onion_tool_T_Huegle</a>

---
