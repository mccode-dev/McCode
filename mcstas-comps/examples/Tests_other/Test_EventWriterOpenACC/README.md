# Test_EventWriterOpenACC

This fixture appends one three-column row per ray from an OpenACC `TRACE`
kernel. The generated ray loop calls the host event-writer service after each
GPU batch; the service synchronizes the bounded buffer, spools one chunk, and
resets the buffer before the next batch. Final output still uses the normal
ASCII or NeXus writer and the MPI root-drained session.

Use `NCount` divisible by `Chunk`, and set `--gpu_innerloop=Chunk` so a run has
multiple batches without padded histories. For `NCount=16` and `Chunk=4`, the
ASCII output contains rows `0 1 2` through `15 16 17`, with one row per event.

Example serial ASCII run:

```sh
mcrun -c --no-mpi --openacc --gpu_innerloop=4 -n 16 -I . \
  -d /tmp/event-writer-openacc Test_EventWriterOpenACC.instr
```

The same instrument can be run with two MPI ranks. Each rank contributes its
local rows to the common root-drained event session; NeXus uses the same chunk
transport and produces one `(16, 3)` event dataset.
