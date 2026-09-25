# The `Test_EventBuffer` Fixture

*McStas: deterministic fixture for the fixed-capacity `MC_EVENT_BUFFER` API*

## Identification

- **Site:** Tests_other
- **Origin:** DTU
- **Date:** 18 September 2026

## Description

A small, self-contained fixture that exercises the fixed-capacity event
buffer `MC_EVENT_BUFFER`, its lifecycle functions
`mc_event_buffer_init` / `mc_event_buffer_free` / `mc_event_buffer_append`
(Step 3) and the `mc_event_buffer_save` helper (Step 4) added to the common
runtime (`common/lib/share/mccode-r.{h.in,c}`). It is isolated from the
Step 1/2 `Test_EventOutList` fixture and does not depend on it.

The `EventBuffer` fixture component initializes a buffer in its `SAVE`
section, appends `rows` deterministic rows (`row[r][c] = r*10 + c`,
independent of `ncount` and seed), saves only the accepted rows (through the
explicit `DETECTOR_OUT_LIST` call or the `mc_event_buffer_save` helper,
selected by `save_helper`), prints deterministic diagnostics
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
| `evSave`       | `cap=4, ncols=3, nrows=6, save_helper=1` | `BufSave.dat` byte-identical to `BufFill.dat` plus a one-time `WARNING: mc_event_buffer_save: 2 events dropped ...` |
| `evSaveEmpty`  | `cap=2, ncols=3, nrows=0, save_helper=1` | documented zero-event no-op through the helper: no file, no warning |

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

Overflow is reported twice by design: the fixture's own stderr diagnostics
(always) and, when saved through `mc_event_buffer_save`, a one-time non-fatal
`WARNING` from the helper (Step 4).

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

Expected stderr diagnostics (order follows the component instances; the
save-helper warning precedes the instance diagnostics for `evSave`):

```
EventBuffer[evFill]:      capacity=4 width=3 rows=6 accepted=4 count=4 next=6 dropped=2
EventBuffer[evZeroCap]:   capacity=0 width=3 rows=3 accepted=0 count=0 next=3 dropped=3
EventBuffer[evFreeTwice]: capacity=4 width=3 rows=2 accepted=2 count=2 next=2 dropped=0
WARNING: mcevent_out_list: width=0 with count=2; no list written
EventBuffer[evZeroWid]:   capacity=3 width=0 rows=2 accepted=2 count=2 next=2 dropped=0
WARNING: mc_event_buffer_save: 2 events dropped (capacity exceeded), saving 4 rows to 'BufSave'
EventBuffer[evSave]:      capacity=4 width=3 rows=6 accepted=4 count=4 next=6 dropped=2
EventBuffer[evSaveEmpty]: capacity=2 width=3 rows=0 accepted=0 count=0 next=0 dropped=0
```

## Save helper (Step 4)

`mc_event_buffer_save` saves exactly `count` accepted rows (never `capacity`)
through `mcevent_out_list`, reports `dropped` once through a non-fatal
`WARNING` when it is nonzero, and leaves the buffer unmodified. A NULL buffer
is a documented no-op. The `evSave` instance (same parameters as `evFill`,
`save_helper=1`) verifies that the helper path produces output identical to
the explicit `DETECTOR_OUT_LIST` path plus the one-time dropped warning, and
`evSaveEmpty` verifies the documented zero-event no-op (no file, no warning).
A McXtrace companion
(`mcxtrace-comps/examples/Tests_other/Test_EventBuffer/Test_EventBuffer_mcxtrace.instr`,
flavor-neutral `EventBuffer.comp` plus `Source_pt`) exercises the same two
paths on the McXtrace runtime.

## OpenACC

`mc_event_buffer_append` is annotated for OpenACC device code (`acc routine`
prototype, atomic reservation of `next`, atomic `dropped`/`count` updates)
and performs no allocation, I/O or `printf`. Device execution requires the
buffer struct and its `data` array to be in device memory; the caller owns
that transfer, and this CPU fixture does not exercise the device path.

## Links

- [Source code](Test_EventBuffer.instr) for `Test_EventBuffer.instr`.
- [Component](EventBuffer.comp) for `EventBuffer.comp`.
- [McXtrace companion](../../../../mcxtrace-comps/examples/Tests_other/Test_EventBuffer/Test_EventBuffer_mcxtrace.instr).

---
