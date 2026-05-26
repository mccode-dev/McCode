# The `Test_Jump_Iterate` Instrument

*McStas: A test of the JUMP ITERATE keyword to describe a long curved guide in a concise way.*

## Identification

- **Site:** Tests_grammar
- **Author:** E. Farhi
- **Origin:** ILL
- **Date:** March 30, 2015

## Description

```text
A curved guide made of only two guide elements which are iterated with  slight
rotation in between, to describe a long curved guide.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L | m | total length of curved section | 60 |
| numel | 1 | number of guide elements, each of length L/numel | 30 |
| R | m | curvature radius | 1500 |
| width | m | width of guide element | 0.03 |
| m | 1 | super-mirror coating wrt Ni, e.g. m=2 | 2 |

## Links

- [Source code](Test_Jump_Iterate.instr) for `Test_Jump_Iterate.instr`.

---
