# Wall-clock timing of INITIALIZE, TRACE and FINALLY in mccode.sim

## Status

*Proposed*.

## Context

A run records no timing, so users cannot see where time goes without profiling by hand.

## Decision

* `mccode_time_ns()` in `mccode-r.c` returns the monotonic clock in ns: `clock_gettime(CLOCK_MONOTONIC)` on POSIX, `mach_absolute_time()` on macOS (CLOCK_MONOTONIC has 1 µs resolution there), `QueryPerformanceCounter` on Windows. Not `timespec_get`, which is not monotonic.
* `mccode_main()` times `init()`, `raytrace_all()`/`raytrace_all_funnel()` and the start of `finally()`.
* `siminfo_close()` writes a block to `mccode.sim` only when the normal end was reached:

```
begin timing
  Init: 0.01 [s]
  Trace: 9.88 [s]
  Finally: 0.02 [s]
end timing
```

## Consequences

* No numerical change: no RNG, physics or ordering is touched.
* Values are the master's wall-clock times. With MPI there is no reduction across nodes.
* Finally is measured up to the closing of `mccode.sim`, not to the end of `finally()`.
* NeXus: `timing_init`, `timing_trace` and `timing_finally` are written to the NXentry next to `end_time` and `duration`.
* A run stopped by a signal has no timing block.
* Not tested: MSVC, Linux/gcc, OpenACC, MPI.
