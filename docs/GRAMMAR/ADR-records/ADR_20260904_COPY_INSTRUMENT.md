# INSTRUMENT keyword COPY(instance) - only copy instance type and paramters

## Status

*Proposed* and prototype *implemented* 

## Context

As demonstrated by @g5t in https://github.com/mccode-dev/McCode/issues/2621, a bug was found in relation to the `COPY` keyword and `EXTEND` blocks, leading to a  confusing warning of the form  

```c
WARNING: Existing (COPY) EXTEND block in COMPONENT image_plate_scattered:
  EXTEND %{
    /* take into account ZnS scintillator efficiency */
    double eff=0.5;
    double v=sqrt(vx*vx+vy*vy+vz*vz);
    p *= 1.0-exp(3960.0/v*log(1.0-eff/100.0));
  %}

is overwritten by:
  EXTEND %{
  %}
```
even in the case of an empty / non-existent EXTEND %{%} block on the copy.

## Decision

For sake of simplicity and to ensure instrument-readability @willend, @g5t and @mads-bertelsen have agreed to no longer include an `EXTEND` from `instance` when `COPY(instance)`. Adding an explicit `EXTEND` block (even if duplicated) is much more explicit.

By symmetry, `METADATA`, `GROUP`, `WHEN`, `JUMP`, and `SPLIT` are from now on also not copied - only `instamnce` and its `instance->actual` are copied.

[`mccode-antlr`](https://github.com/mccode-dev/mccode-antlr/) already implements this behaviour for `EXTEND` since [this commit](https://github.com/mccode-dev/mccode-antlr/commit/7786fd31efde941a16e028f6a616f5b8284c0b50)

## Consequences

* A statistic om the combined use of COPY with the affected keywords will be provided soon
* CHANGELOG should be crystal clear on this change / potential incompatibility

