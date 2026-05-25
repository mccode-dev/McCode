# The `Histogrammer` Instrument

*McStas: Takes eventfile input (Virtual_input/Vitess/MCNP/Tripoli4/MCPL formats) and applies Monitor_nD to generate
histograms. Histograms can be chosen freely using the options string, see mcdoc Monitor_nD.comp*

## Identification

- **Site:** Tools
- **Author:** Peter Willendrup (peter.willendrup@risoe.dk)
- **Origin:** Risoe
- **Date:** 2007-03-19

## Description

```text
Takes any possible  McStas eventfile inputs (Virtual_input/Vitess/MCNP/Tripoli4 formats) and applies
Monitor_nD to generate user-selectable neutron histograms.

The 'options' parameter allows to customize the type of histogram to generate.
We suggest:
"sphere theta phi outgoing previous" (this is the default)
"previous, multiple, auto, x, y, z, vx, vy, vz, hdiv, vdiv, tof, lambda"
"previous, auto, x, y"
"previous, auto, tof, lambda"

Example: mcrun Histogrammer.instr MODE=0
- Gives information on the input parameters

Example: mcrun Histogrammer.instr filename="events.dat" MODE=1 options="sphere theta phi outgoing previous"
- Reads a Virtual_output generated event file and applies a spherical PSD
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| filename | string | Specifies input event file | 0 |
| MODE | int | Input file mode/format - 0 for help on usage 1=McStas,2=Vitess,3=MCNP,4=Tripoli4,5=MCPL | 0 |
| options | string | Specifies the histogramming rules used by Monitor_nD. | "sphere theta phi outgoing previous" |
| bufsize | int | Vitess_input 'buffersize' parameter - see mcdoc Vitess_input | 10000 |
| xwidth | m | Horizontal width of detector, or diameter for banana,cylinder and shpere geometry | 0.1 |
| yheight | m | Vertical height of detector, for plate, cylinder, banana shape | 0.1 |

## Links

- [Source code](Histogrammer.instr) for `Histogrammer.instr`.
- <a href="../sources/Virtual_input.html">Virtual_input</a> component (McStas event file)
- <a href="../misc/Vitess_input.html">Vitess_input</a> component (Vitess event file)
- <a href="../contrib/Virtual_mcnp_input.html">Virtual_mcnp_input</a> component (MCNP PTRAC event file)
- <a href="../contrib/Virtual_tripoli4_input.html">Virtual_tripoli4_input</a> component (Tripoli4 BATCH event file)
- <a href="../monitors/Monitor_nD.html">Monitor_nD</a> component (detector/histogrammer)
- <a href="../misc/MCPL_input.html">MCPL_input</a> component (MCPL event file)

---
