# The `Test_EventBuffer` Fixture

*McStas: deterministic fixture for the fixed-capacity `MC_EVENT_BUFFER` API*

## Identification

- **Site:** Tests_other
- **Origin:** DTU
- **Date:** 18 September 2026

## Description

A small, self-contained fixture that exercises the fixed-capacity event
buffer `MC_EVENT_BUFFER` and its lifecycle functions
`mc_event_buffer_init` / `mc_event_buffer_free` / `mc_event_buffer_append`
added to the common runtime (`common/lib/share/mccode-r.{h.in,c}`), Step 3 of
the generic event functions plan. It is isolated from the Step 1/2
`Test_EventOutList` fixture and does not depend on it.

The `EventBuffer` fixture component initializes a buffer in its `SAVE`
section, appends `rows` deterministic rows (`row[r][c] = r*10 + c`,
independent of `ncount` and seed), saves only the accepted rows through
`DETECTOR_OUT_LIST`, prints deterministic diagnostics
(`accepted/count/next/dropped`) to stderr, and frees the buffer exactly once
(plus an optional safe second free). Neutron physics is a pass-through
no-op, so the behavior is fully deterministic in a single process.

One instrument run covers the case matrix:

| Instance       | Params                             | Expected                                                              |
|----------------|------------------------------------|-----------------------------------------------------------------------|
| `evFill`       | `cap=4, ncols=3, nrows=6`      | `BufFill.dat`: 4 rows in insertion order; 2 dropped; no capacity rows |
| `evZeroCap`    | `cap=0, ncols=3, nrows=3`      | no file (count=0 no-op); 3 dropped; no out-of-bounds write            |
| `evFreeTwice`  | `cap=4, ncols=3, nrows=2, free_twice=1` | `BufFreeTwice.dat`: 2 rows; second free is a safe no-op        |
| `evZeroWid`    | `cap=3, ncols=0, nrows=2`      | no file (width=0 rejected by `mcevent_out_list`); no storage allocated |

Documented buffer semantics exercised here:

- Fixed capacity; appending beyond capacity increments `dropped`, returns 0
  and never writes out of bounds.
- `count` equals the number of accepted rows and is what gets saved (never
  `capacity`), so no uninitialized capacity rows can appear in the output.
- CPU appends store rows in insertion order; `next` (reservations) is kept
  separate from `count` and may exceed `capacity`.
- Zero capacity is a valid safe buffer: appends are rejected and counted in
  `dropped`.
- `width==0` with `capacity>0` is an allowed degenerate case: init succeeds,
  no storage is allocated, appends are accepted while copying nothing, and
  saving is a `mcevent_out_list` no-op.
- `mc_event_buffer_free` is exactly-once and idempotent after initialization (a
  second call is a no-op, never a double-free); a zero-initialized empty buffer
  is also safe to free.
- A null row pointer is rejected without touching any counter.

Overflow is reported by the fixture's stderr diagnostics rather than by the
Step-4 save helper/warning machinery, which is deliberately not part of this
step.

## How to run

The component is not part of the installed library; pass its directory with
`-I`:

```sh
mcrun -n 1000 -I $(pwd) Test_EventBuffer.instr
```

Expected deterministic data for `BufFill.dat` (rows = events, in insertion
order, exactly `count=4` rows despite `capacity=4 < rows=6`):

```
0 1 2
10 11 12
20 21 22
30 31 32
```

Expected stderr diagnostics (order follows the component instances):

```
EventBuffer[evFill]:      capacity=4 width=3 rows=6 accepted=4 count=4 next=6 dropped=2
EventBuffer[evZeroCap]:   capacity=0 width=3 rows=3 accepted=0 count=0 next=3 dropped=3
EventBuffer[evFreeTwice]: capacity=4 width=3 rows=2 accepted=2 count=2 next=2 dropped=0
EventBuffer[evZeroWid]:   capacity=3 width=0 rows=2 accepted=2 count=2 next=2 dropped=0
```

## OpenACC

`mc_event_buffer_append` is annotated for OpenACC device code (`acc routine`
prototype, atomic reservation of `next`, atomic `dropped`/`count` updates)
and performs no allocation, I/O or `printf`. Device execution requires the
buffer struct and its `data` array to be in device memory; the caller owns
that transfer, and this CPU fixture does not exercise the device path.

## Links

- [Source code](Test_EventBuffer.instr) for `Test_EventBuffer.instr`.
- [Component](EventBuffer.comp) for `EventBuffer.comp`.

---
