# Grammar consistency fixes from code-generator review

## Status

*Proposed* and *implemented* on branch `cogen-improve-comments-review`
(follows the comment-only commit b478b4e32).

## Context

A review of the code generator (`mccode/src/instrument.l`,
`instrument.y`, `cogen.c.in`) found places where accepted syntax did
not behave as documented or intended. A few crashed the generator; others
silently produced wrong simulations. None of these is a new feature: each
change makes the existing grammar do what it already claims to do, or
turns silent misbehaviour into an error.

The most serious one concerns the `INHERIT` keyword
(see [ADR_20250612_INHERIT_COMP](ADR_20250612_INHERIT_COMP.md)):
`DEFINE COMPONENT X INHERIT Y` with no sections of its own produced a
component *without any code*. The shipped `Diaphragm` (`INHERIT Slit`)
and `Place` (`INHERIT Arm`) were therefore no-ops in both McStas and
McXtrace. A Diaphragm let the full beam through: a test monitor behind
it read about 16x the intensity measured behind the equivalent Slit.

## Decision

Fix the following in the generator. No new keywords are introduced.

1. **Whole-component `INHERIT`** (`DEFINE COMPONENT X INHERIT Y`): a code
   section that `X` does not write is taken from `Y`. A section that `X`
   writes, even an empty `%{ %}` block, replaces `Y`'s. This is the rule
   mccode-antlr implements (mccode-dev/mccode-antlr#321, #325).
   Parameter lists keep being concatenated (unchanged). `SHARE` is emitted
   once per code block, so `X` and `Y` in the same instrument do not
   duplicate the shared code.
   Within one section, the parts are concatenated in the order given:
   `SECTION [%{..%}] (INHERIT Z | EXTEND %{..%})*`. `EXTEND` never pulls in
   `Y`'s code implicitly; write `INHERIT Y` inside the section for that.
   `USERVARS` now has the same form as the other sections (`INHERIT` allowed,
   the repeated `USERVARS` keyword is gone; no shipped file used it).
   Unchanged: `COPY(instance)` never copies `EXTEND`
   (mccode-dev/mccode-antlr#330).
2. **`JUMP PREVIOUS[(n)]` / `NEXT[(n)]`** are resolved relative to the
   jumping component, as documented. A target that does not exist (an
   unknown name, or out of range) is an error.
3. **`MYSELF` in the actual parameters** of a component instance is an
   error. It remains valid from `WHEN` onwards (`WHEN`, `AT`, `ROTATED`,
   `JUMP`, ...), where the instance is known.
4. **Component `USERVARS ... EXTEND %{ %}`**: the `EXTEND` blocks are
   kept, as for all other sections.
5. **`COPY(...)` of an undefined instance**, and an **unknown component
   class**, are reported as errors (both previously crashed the generator).
6. **`%include "file.ext"` inside C blocks** accepts any alphanumeric
   extension (e.g. `.hpp`), not only one-character ones. Names without an
   extension are still treated as libraries (`lib.h` + `lib.c`).
7. **`DECLARE` / `USERVARS` scanning** recognises several declarations
   on one line (`double a; double b;`).
8. **`USERVARS` read through `particle_getvar()`** (e.g. Monitor_nD
   `user1="var"`): numeric scalars are converted to `double` instead of
   having their memory read as a `double`. A USERVAR counts as numeric when
   it is not an array or pointer and every word of its type is one of
   `char short int long float double signed unsigned _Bool bool const
   volatile MCNUM size_t` or a `<stdint.h>` integer name
   (`u?int(_least|_fast)?N_t`), the same rule as mccode-antlr's
   `is_numeric_scalar`. Others (arrays, pointers, structs, and typedefs
   such as `typedef double real;`) are not readable this way and report
   failure. The same rule decides which USERVARS `particle_uservar_init()`
   zeroes. USERVARS used with Monitor_nD should still be declared `double`.
9. **Vector parameters given as `{...}`** are split at top-level commas,
   and each element is written into the generated C as the expression it
   is. Before, the elements were parsed as plain numbers (`strtod`), so
   an expression became several wrong elements: `{1, 3.2*0.0219, ...}`
   gave `{1, 3.2, 0, 0.0219, ...}`, one element too many and shifted.
   Literals were also rounded to 6 significant digits (`%g`); they are now
   copied exactly as written. Instrument parameters and function calls can
   be used in such vectors. The vector length is still fixed at code
   generation time.

## Consequences

* Instruments using `Diaphragm` or `Place`, or user components built as
  `DEFINE COMPONENT X INHERIT Y`, **change results**: they now behave like
  their parent. This is the intended behaviour, but CHANGELOG should say
  so clearly. No shipped example instrument uses Diaphragm.
* `StatisticalChopper_Monitor` now also works in instruments that do not
  contain a `Monitor_nD`.
* Instruments that used `JUMP PREVIOUS` (generator crash) or `JUMP NEXT`
  (jumped to the first component) now work. Instruments that used
  `MYSELF` in component parameters, or `JUMP` to a non-existent target,
  now fail to generate with an error message. No shipped instrument does
  either.
* `ILL_H5` and `ILL_H5_new` **change results**: their `IN15_Vpolariser` /
  `WASP_Vpolariser` (`Pol_guide_vmirror`) use expressions such as
  `3.2*0.0219` in `rPar`/`rUpPar`/`rDownPar` and now get the intended
  reflectivity parameters. Intensity downstream of these polarisers rises
  by factors of about 3 to 12 (e.g. `H511_IN15_Detector` from 0 to
  1.6e4); `H5_I` and the other detectors are unchanged. Their `%Example`
  values may need updating.
* Vector literals with more than 6 significant digits are now exact; the
  old rounding was at most 5e-6 relative. A worst-case test (all
  `Pol_mirror` reflectivity parameters given 8 digits) showed no change in
  monitor output.
* All other shipped instruments are unaffected. All 332 McStas and 112
  McXtrace examples generate the same code as before, apart from
  cosmetic lines and the `particle_getvar()` casts. Representative
  JUMP/SPLIT/GROUP/USERVARS/Union/xraylib test instruments give identical
  results.

## Behaviour

Relative JUMP (`c` is component 3 of `a, b, c, d`):

```
COMPONENT c = Arm() AT (0,0,1) RELATIVE b
  JUMP PREVIOUS WHEN (cond)   /* goes to b (before: generator crash)  */
  JUMP NEXT WHEN (cond)       /* goes to d (before: went to a)        */
  JUMP NEXT(2) WHEN (cond)    /* error: no component 5                */
```

`MYSELF` only after the parameter list:

```
COMPONENT s = Slit(xwidth=MYSELF)              /* error                  */
COMPONENT s = Slit(xwidth=0.1) WHEN (MYSELF)   /* ok, MYSELF -> s        */
```

Vector parameters with expressions (as in ILL_H5):

```
rUpPar={1, 3.2*0.0219, 4.07, 1, 0.003}
/* now:    5 elements, rUpPar[1] = 3.2 * 0.0219                        */
/* before: {1, 3.2, 0, 0.0219, 4.07, 1, 0.003}: 7 elements, wrong values */
```

Instrument parameters can now be used too, e.g. `rUpPar={1, 0.0219, 4.07, 2*m, 0.003}`.

Whole-component inheritance:

```
DEFINE COMPONENT Diaphragm INHERIT Slit
END
/* now has Slit's INITIALIZE/TRACE/DISPLAY; before it had none */

DEFINE COMPONENT X INHERIT Y
TRACE
%{
%}
END
/* X has all of Y's sections except TRACE, which is empty (before: an
   empty block counted as not written, so Y's TRACE was used) */

DEFINE COMPONENT X INHERIT Y
INITIALIZE INHERIT Y EXTEND
%{
  more();
%}
END
/* Y's INITIALIZE followed by more(); all other sections from Y.
   INITIALIZE EXTEND %{ more(); %} alone would give just more(). */
```
