# The `Unittest_SEARCH_Monitor` Component

*McStas: Simple single detector/monitor.

Copy of the Monitor component, used by the Unittest_SEARCH instrument to test
that components are found via a SEARCH statement.*

## Identification

- **Author:** Kim Lefmann
- **Origin:** Risoe
- **Date:** October 4, 1997

## Description

```text
Sums neutrons (0th, 1st, and 2nd moment of p) flying through
the rectangular monitor opening. May also be used as detector.

Example: Unittest_SEARCH_Monitor(xmin=-0.1, xmax=0.1, ymin=-0.1, ymax=0.1)
```


## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| xmin | m | Lower x bound of opening | -0.05 |
| xmax | m | Upper x bound of opening | 0.05 |
| ymin | m | Lower y bound of opening | -0.05 |
| ymax | m | Upper y bound of opening | 0.05 |
| xwidth | m | Width of detector. Overrides xmin,xmax. | 0 |
| yheight | m | Height of detector. Overrides ymin,ymax. | 0 |
| restore_neutron | 1 | If set, the monitor does not influence the neutron state | 0 |

## Links

- [Source code](Unittest_SEARCH_Monitor.comp) for `Unittest_SEARCH_Monitor.comp`.

---
