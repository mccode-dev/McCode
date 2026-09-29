# The `Test_EventWriter` Fixture

This fixture exercises the host-side bounded generic event writer from the
common runtime. The writer keeps only one configured chunk in event-data
memory; completed chunks are spooled temporarily and replayed at `end` through
the normal ASCII, NeXus, and MPI output paths.

The append instance writes 11 rows with a chunk capacity of 3 and calls an
explicit flush after row 4. The write instance uses the bulk-row API with 8
rows and a capacity of 2. Both files contain rows in the deterministic form
`row[r][c] = r*10 + c`:

```
0 1 2
10 11 12
20 21 22
...
100 101 102
```

`WriterDirect.dat` uses `mc_event_writer_begin_direct` with the known final
count of 11. Its three-row chunks are written directly through the serial
McCode backend; the normal `mc_event_writer_begin` path remains the choice for
unknown counts and MPI.

`WriterTrace.dat` starts the normal writer in `INITIALIZE`, appends one row per
ray in `TRACE`, and lets the bounded buffer flush chunks while the simulation is
running. The final `SAVE` call only performs the collective serialization.

`WriterStream.dat` uses the unknown-count serial stream sink. It opens the
normal output format when the first chunk is ready, writes subsequent chunks
directly during tracing, and patches the reserved row-count fields in the
ASCII header (or rewrites the NeXus detector metadata) at `SAVE`. The logical
header format is unchanged; the fixed-width fields only exist while the
stream is incomplete. This mode requires a new output file and is currently
serial; MPI uses the bounded spool path below until its host service protocol
is added.

`WriterEmpty` is a zero-event no-op. The companion
`Test_EventWriter_mpi.instr` puts rows on only one rank in each direction and
then gives the ranks different positive row/chunk counts. Those three writers
append and flush during `TRACE`; `WriterAllEmpty` checks that every rank can
enter an empty session. With `--mpi=2`, all four writers complete even when a
rank has no local chunks. NeXus event datasets for the serial fixture have
shapes `(11, 3)`, `(8, 3)`, `(11, 3)`, `(11, 3)`, and `(11, 3)` for the
append, bulk-write, direct, staged-trace, and unknown-count stream writers.
The MPI fixture also gives `WriterMetadataMismatch` different component
positions on the two ranks; the collective rejects the save without hanging
instead of silently accepting root metadata.

`WriterInvalidBegin` verifies that a rejected `chunk=0` begin leaves the
writer safely inactive, so the matching `end` call is harmless. `WriterInvalidEnd`
uses a path below a directory that does not exist. In ASCII mode the failed
open is reported as an invalid detector when `end` enters the output session,
after which the writer still releases its spool and buffer; this ASCII open
failure is the asserted case. NeXus instead treats the same value as a dataset
name; it sanitizes the path separator, so the save still produces a valid
`(1, 3)` events dataset. The fixture does not assert that NeXus case, keeping
the component focused on the ASCII open-failure path.

Run from this directory with:

```sh
mcrun -n 1000 -I . Test_EventWriter.instr
```

The McXtrace companion is
`mcxtrace-comps/examples/Tests_other/Test_EventWriter/Test_EventWriter_mcxtrace.instr`;
it also exercises the serial unknown-count stream sink.
