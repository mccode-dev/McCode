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

`WriterEmpty` is a zero-event no-op. The companion
`Test_EventWriter_mpi.instr` puts rows on only one rank in each direction and
then gives the ranks different positive row/chunk counts. With `--mpi=2`, all
three writers complete even when a rank has no local chunks. NeXus event
datasets for the serial fixture have shapes `(11, 3)` and `(8, 3)`.

Run from this directory with:

```sh
mcrun -n 1000 -I . Test_EventWriter.instr
```

The McXtrace companion is
`mcxtrace-comps/examples/Tests_other/Test_EventWriter/Test_EventWriter_mcxtrace.instr`.
