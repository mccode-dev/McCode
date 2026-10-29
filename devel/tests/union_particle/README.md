# Union Particle Regression

Run the custom McStas and McXtrace ABI/state tests with the freshly built
generators:

```sh
mamba run -n mcstas-dev python devel/tests/union_particle/run_union_particle.py \
  --compiler clang --workdir ../local_work

mamba run -n mcstas-dev python devel/tests/union_particle/run_union_particle.py \
  --mcxtrace mcxtrace --compiler clang --workdir ../local_work
```

The default McStas run also builds four isolated copies of
`Test_logger_2DQ`, `Test_abs_logger_nD`, `Test_inhomogenous_process`, and
`Mirror_validation`, injecting `safe_mode=0` or `safe_mode=1` into CPU
`Union_master` calls only, with 5 million particles per run. Use
`--examples NAME ...` to select examples,
`--no-examples` to skip them, and `--example-ncount N` to change their count.

Coverage includes native particle state and RNG dumps, forbidden process and
cross-section writes, rotated/focused nested boxes, upstream pure absorption,
before/after logger snapshots, full versus absorbed event weights, conditional
logger temporary storage, inhomogeneous scattering, and Mirror surface plus
refraction paths. Detector headers and numeric payloads are compared exactly;
dates, paths, and parameter-header lines are ignored. All builds and runs use
temporary directories, so the source examples are never modified or populated.

The custom contract test uses 64 particles and exact native-state/RNG dumps, not
statistical reference values. The example tests also require exact equality, not
a numerical tolerance. For reference-value validation, run `mctest` with at least
`-n 5E6` and keep the seed and MPI rank count fixed between comparisons.

## Callback Migration

The Union callback API is now particle-based, including CPU fallback callbacks
and process/surface templates. External components must update their signatures
to match `union-lib.h`; the former parallel position, wavevector, time and weight
arguments are no longer passed. Process particles are in the process frame;
surface and logger particles are in the master frame. Absorption event particles
retain their full pre-attenuation weight, with absorbed weight passed separately.

`safe_mode=1` preserves only outgoing velocity/wavevector, weight and advanced RNG
state after scattering. Queries use isolated snapshots, and safe-mode observers
receive fresh copies per invocation. These are shallow copies, not protection
against writes through pointers or modifications to shared process storage.

Compared with the previous implementation, explicit particle RNG forwarding in
focus and sampling helpers changes seeded trajectories. Reduced representation
conversions can also change rounding. McXtrace propagation now updates phase via
`PROP_DL`, and its previously inactive absorption-event bookkeeping is connected
to the particle-based absorption logger API. Equality between normal and safe
modes is tested separately from agreement with historical simulation references.
