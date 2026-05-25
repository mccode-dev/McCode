# The `Test_Incoherent_MS` Instrument

*McStas: Test instrument for validation of magnitude of multiple scattering in Incoherent.comp.*

## Identification

- **Site:** Tests_samples
- **Author:** Daniel Lomholt Christensen and Peter Willendrup
- **Origin:** NBI
- **Date:** 20250603

## Description

```text
Test instrument for validation of magnitude of multiple scattering in Incoherent.comp.
The instrument considers "thin" and "thick" sample conditions.


For the thin sample conditions, the following analytical calulations find the
Neutron intensity at the detector to be 27.12:
<div class="latex">
$$
\frac{dN_{\rm real}}{dt}= \Psi A \rho_{\rm c}\sigma_{\rm inc}
\frac{\Delta \Omega}{4 \pi} a_{\rm lin} l_{unscattered}.
$$
Where $\Psi=1e7$, $A=1$, $\rho=0.0723$, $\sigma_{\rm inc}=\sigma_{\rm abs} = 5.08$, $\frac{\Delta \Omega}{4 \pi} = 10/99.95^2$
and $a_{\rm lin}= \exp(-(\sigma_{\rm inc} + \sigma_{\rm abs}\lambda/1.7982)\rho l_{avg})$.
$l_{unscattered}=0.1cm$ and is the path through the sample eg. unscattered path length.
</div>


For the thick sample condition, the general formula is the same,
But the attenuation is calculated using the effective length, instead of the actual
length:
<div class="latex">
\begin{equation} \label{eq:inc_layer_l}
l_{\rm app} = \int _{0}^{l_{\rm max}} a_{\rm lin}(z) dz
=  \int _{0}^{l_{\rm max}} e^{-2\mu z}dz
=  \frac{1-e^{-2\mu l_{\rm max}}}{2\mu },
\end{equation}
</div>

By scanning across thickness values, one then finds a curve, that should fit the
analytical calculations.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| flux_mult | 1/(s*cm**2*st*meV) | Source flux x 1e14 multiplier for source | 0.5512 |
| thick | m | Sample thickness | 0.001 |
| total_scattering | 1 | Flag to indicate if sample focuses (=0) or not (=1) | 0 |
| sample | str | Sample state "thick", "thin", "none" supported | "thin" |
| order | 1 | Maximal order of multiple scattering | 1 |

## Links

- [Source code](Test_Incoherent_MS.instr) for `Test_Incoherent_MS.instr`.
- A reference/HTML link for more information

---
