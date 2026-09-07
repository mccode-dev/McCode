# INSTRUMENT keyword COPY(instance) - only copy instance type and paramters

## Status

Change *accepted* and prototype *implemented*

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

By symmetry, `METADATA`, `GROUP`, `WHEN`, `JUMP`, and `SPLIT` are from now on also not copied - only `instance` and its `instance->actual` are copied.

> [!NOTE]  
> This behavior is implemented in [`mccode-antlr`](https://github.com/mccode-dev/mccode-antlr/)`>=0.28.0`

## Consequences

* Relatively mild:
  * Before change: Using `COPY` from an instance including e.g. a
    `WHEN` statement, this `WHEN` statement would be carried to the copy instance.
  * After change: Neither `METADATA`, `GROUP`, `WHEN`, `JUMP`, or  `SPLIT` are copied. If required, manually copy these properties to the copy.
  * Using AI methods, the full instrument suite for both of McStas and
    McXtrace was surveyed for combinations of `COPY` with the other
    keywords. [See PR comments](https://github.com/mccode-dev/McCode/pull/2627) for detailed information.
  * The survey conclusion was that only `ILL_H5` and `ILL_H5_new` are
    affected by each including a `COPY`'d monitor with an active `WHEN`
    clause.
  * A full statistic with all instrument combined use of COPY with the affected keywords
    is provided in [this csv file](ADR_20260904_COPY_INSTRUMENT_data/copy_survey.csv)
* `CHANGELOG`s should be crystal clear on this change / potential incompatibility

