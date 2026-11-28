# Trace each SPLIT branch as its own ray

## Status

proposed

## Context

In trace mode (`--trace`, used by `mcdisplay`), a SPLIT restores the saved particle state for each branch inside one `ENTER:` ... `LEAVE:` block. The trace parser therefore treats all branches as a single ray, and `mcdisplay` draws a "return line" from the end of each branch back to the SPLIT point.

## Decision

In the SPLIT loop (`cogen.c.in`), before restoring the particle for the second and later branches, emit `DEBUG_LEAVE(); DEBUG_STATE(); DEBUG_ENTER(); DEBUG_STATE();`. This closes the current ray at its end point and opens a new one, in the line order the existing trace parser expects.

## Consequences

Each SPLIT branch is drawn as a separate ray starting at the SPLIT component. No parser or frontend changes are needed. Only trace output changes; simulation results are unaffected, and the `DEBUG_*` macros compile out without trace support.

## Behaviour

- No return lines; absorbed branches and nested SPLITs are handled the same way.
- Branches count as separate rays, so a display with SPLIT holds more rays per traced particle and reaches the `mcdisplay` ray limit sooner.
- Tested with McStas on macOS arm64: a SPLIT 3 test instrument (raw trace) and `PSI_DMC` in `mcdisplay-webgl-classic`. McXtrace shares the code generator and was not tested.
