# Wall-clock timing of INITIALIZE, TRACE and FINALLY in mccode.sim

## Status

*Proposed*. Implemented in Lomholy/McCode#70. Needs a matching change in mccode-antlr (`translators/c_raytrace.py` emits its own copy of the `raytrace_all` loop).

## Context

A run records no timing. `mcstartdate` has 1 s resolution, so it cannot give the duration of a section. Users who want to know where time goes must profile by hand.

This ADR adds:

* wall-clock time of INITIALIZE, total TRACE and FINALLY, in integer nanoseconds, written to `mccode.sim` on every run;
* an opt-in per-ray timer (`MCCODE_TIME_RAY`) that adds count, mean, standard deviation, min and max.

## Decision

### Clock

`mccode_time_ns()` in `mccode-r.c` returns the monotonic time in ns.

* Linux and other POSIX: `clock_gettime(CLOCK_MONOTONIC, &ts)`, returning `ts.tv_sec*1000000000LL + ts.tv_nsec`.
* macOS: `mach_absolute_time()` scaled by `mach_timebase_info()`. `CLOCK_MONOTONIC` has only 1 µs resolution there, and the generated code's `_POSIX_C_SOURCE` hides `CLOCK_MONOTONIC_RAW`. On Apple silicon one tick is 41.67 ns.
* Windows: `QueryPerformanceCounter` and `QueryPerformanceFrequency`, converted as `(c/f)*1000000000 + (c%f)*1000000000/f`. This avoids overflow and keeps full precision.
* Not `timespec_get(TIME_UTC)`. It is not monotonic, so an NTP step would corrupt a duration.

### Runtime macros

In `mccode-r.h.in`:

```c
long long mccode_time_ns(void);
#ifdef MCCODE_TIME_RAY
#if defined(OPENACC) || defined(FUNNEL)
#error "MCCODE_TIME_RAY is supported only for CPU, non-FUNNEL builds"
#endif
void mcray_time_add(long long dt);
#define MCRAY_TIMER_START long long _mcray_t0 = mccode_time_ns();
#define MCRAY_TIMER_STOP  mcray_time_add(mccode_time_ns() - _mcray_t0);
#else
#define MCRAY_TIMER_START
#define MCRAY_TIMER_STOP
#endif
```

The section timers need no code-generator change. `mccode_main.c` already calls `init()`, `raytrace_all()` or `raytrace_all_funnel()`, and `finally()`, and both generators embed these files. Only the per-ray timer needs two emitted lines around `raytrace(_particle);` in `raytrace_all`.

### Block format in `mccode.sim`

```
begin timing
  Clock: monotonic_ns
  Nodes: 4
  Reduced: yes
  Init: 1234567 +- 20311 (min 1200345, max 1260001) [ns]
  Trace: 987654321 +- 4100221 (min 980001234, max 993000111) [ns]
  Finally: 2345678 [ns]
  Rays: 1000000
  Ray: 812 +- 95 (min 410, max 90211) [ns]
end timing
```

* Each statistics row is `<key>: <mean> +- <std> (min <min>, max <max>) [ns]`. `Finally` is `<value> [ns]`.
* With `Reduced: no` (one process, or the signal path), `Init` and `Trace` are single values, e.g. `Init: 1234567 [ns]`. `Ray` always has the full row.
* Values are integer ns. Mean and std are rounded to the nearest ns.
* `Finally` is a single value.
* `Rays` and `Ray` appear only with `MCCODE_TIME_RAY`.
* The block has no `filename:` key, so the 0D-monitor "No file" case in `mcplotloader.py` is unaffected.

### MPI semantics

* Init and Trace are reduced over ranks to min, mean, population std and max.
* Per-ray statistics are combined over all rays of all ranks.
* Finally is master-only. `siminfo_close()` does work only on the master, and a collective call there would deadlock in the signal path. The value is the master's.
* Without MPI, or with one rank, there is nothing to reduce, so `Init` and `Trace` are written as single values.

### Signal path

On SIG_TERM, before `finally()`, the handler sets `mctime_trace_ns` and `mctime_finally_t0`. No reduction runs on this path, since only one rank gets the signal. The block holds local values and `Reduced: no`.

### Flag

`mcrun --time-rays` (and `mxrun --time-rays`) appends `-DMCCODE_TIME_RAY` to the cflags and implies `-c`. The workaround `mcrun --D1=-DMCCODE_TIME_RAY -c ...` works now without a code change.

## Consequences

* **No numerical change.** No RNG, physics or ordering is touched. Monitor output is the same as without the flag for the same `--seed`.
* Section timers make 6 clock reads per run. This cost is negligible.
* Per-ray timer makes two clock reads per ray, about 20 to 60 ns. This skews the timing of very cheap instruments.
* `MCCODE_TIME_RAY` with OpenACC or an active FUNNEL fails at compile time through the `#error`. On CPU, `-DFUNNEL` is only active together with `-DCPUFUNNEL`; otherwise the runtime drops it and the per-ray timer works.
* Per-ray values below one clock tick read as 0 (min 0 is common on macOS).
* mccode-antlr must emit the two macros to get per-ray statistics. It must only do so with a runtime that defines them, otherwise its generated C does not compile. The section timers work with mccode-antlr as soon as it embeds this runtime.
* A scan's top-level `mccode.sim`, written by mcrun, has no timing block. Each step's `mccode.sim` has one.
* FINALLY is master-only under MPI.

## Behaviour

Measured on macOS arm64, clang, CPU, with this branch's `mcstas` and `mcxtrace` code generators (`template_simple`, `-n 1e5`):

| case | result |
|---|---|
| plain build, McStas and McXtrace | block present, `Nodes: 1`, `Reduced: no`, no `Rays`/`Ray` lines |
| `-DMCCODE_TIME_RAY` | `Rays: 100000` (= ncount), `Ray: 35 +- 18 (min 0, max 1459)` |
| per-ray overhead | TRACE 2.8 ms to 5.4 ms for 1e5 rays, about 25 ns per ray |
| monitors with and without `MCCODE_TIME_RAY`, same seed | bit-identical |
| `mpirun -np 2` and `-np 4` | `Reduced: yes`, min <= mean <= max, summed `Rays` = ncount |
| SIGTERM during TRACE | block written, `Reduced: no` |
| `-DFUNNEL -DCPUFUNNEL` | section timers written; with `MCCODE_TIME_RAY` the `#error` fires |
| `mcrun --time-rays` | recompiles with `-DMCCODE_TIME_RAY` |

Not tested locally: MSVC (`QueryPerformanceCounter`), Linux/gcc, nvc/OpenACC, NeXus output.
