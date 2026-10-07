# The `Test_EventWriter` Fixture

This fixture exercises the host-side generic event writer from the common
runtime. The writer keeps only one configured chunk in event-data memory;
bounded writers spool completed chunks, while unknown-count streams service
completed chunks during the simulation and finalize only their metadata at
`end`.

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
McCode backend; the normal `mc_event_writer_begin` path remains the bounded
spool choice when the final count is unknown.

`WriterTrace.dat` starts the normal writer in `INITIALIZE`, appends one row per
ray in `TRACE`, and lets the bounded buffer flush chunks while the simulation is
running. The final `SAVE` call only performs the collective serialization.

`WriterStream.dat` uses the unknown-count serial stream sink. It opens the
normal output format when the first chunk is ready, writes subsequent chunks
directly during tracing, and patches the reserved row-count fields in the
ASCII header (or rewrites the NeXus detector metadata) at `SAVE`. The logical
header format is unchanged; the fixed-width fields only exist while the
stream is incomplete. Completed unknown-count chunks are written before
`SAVE`; in host-serviced execution they are written at the next host service
boundary, while `SAVE` handles the final partial chunk and metadata. The MPI
variant keeps rank-local chunks in bounded spools and drains them at those
boundaries into a root-owned stream.

`WriterEmpty` is a zero-event no-op. The companion
`Test_EventWriter_mpi.instr` puts rows on only one rank in each direction and
then gives the ranks different positive row/chunk counts (`Rows=7` produces
seven rows on rank 0 and ten on rank 1 for the unequal writer). Those three writers
append and flush during `TRACE`; `WriterAllEmpty` checks that every rank can
enter an empty unknown-count stream. With `--mpi=2`, all regular writers
complete even when a rank has no local chunks. `WriterStreamMPI` is root-owned
and, for `Rows=7`,
contains 17 rows. Rows retain their sequence order within each rank, but the
two rank-local blocks may arrive in either cross-rank order. Its NeXus event
dataset has shape `(17, 3)`.
NeXus event datasets for the serial fixture have shapes `(11, 3)`, `(8, 3)`,
`(11, 3)`, `(11, 3)`, and `(11, 3)` for the append, bulk-write, direct,
staged-trace, and unknown-count stream writers. `WriterDirectMPIRejected` uses
positive row and chunk counts, but deliberately calls the serial-only
known-count direct constructor under MPI. Its begin must be rejected; its
matching `end` call must return an invalid detector and leave the writer
inactive. This assertion is independent of the selected ASCII or NeXus format,
and the rejected case does not enter a collective or create an output file.
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

`WriterInvalidDirectBegin` and `WriterInvalidStreamBegin` exercise the same
rejected-begin/end contract for the serial known-count and unknown-count
constructors. Neither case creates an output file.

The MPI fixture's final `WriterStreamFailure` uses a missing directory. The
root sink fails after local or remote chunks become available, but all ranks
still drain their in-flight chunks and complete the service acknowledgement
protocol. ASCII runs assert the propagated end failure; NeXus treats the path
as a dataset name and is not asserted for this case.

Run from this directory with:

```sh
mcrun -n 1000 -I . Test_EventWriter.instr
mcrun -n 20 --mpi=2 -I . Test_EventWriter_mpi.instr Rows=7
mcrun -n 20 --mpi=2 --format=NeXus -I . Test_EventWriter_mpi.instr Rows=7
# Asymmetric large-payload and many-rank any-source coverage:
mcrun -n 10000 --mpi=4 -I . Test_EventWriter_mpi.instr Rows=10000
mcrun -n 10000 --mpi=4 --format=NeXus -I . Test_EventWriter_mpi.instr Rows=10000
```

The McXtrace companion is
`mcxtrace-comps/examples/Tests_other/Test_EventWriter/Test_EventWriter_mcxtrace.instr`;
it also exercises the serial unknown-count stream sink.
