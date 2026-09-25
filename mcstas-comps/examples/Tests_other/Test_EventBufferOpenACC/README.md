# Test_EventBufferOpenACC

This fixture appends three-column rows from an OpenACC `TRACE` kernel to a
fixed-capacity `MC_EVENT_BUFFER`. With the default `NCount=64`, eight rows are
accepted and 56 reservations are dropped. Device row order is intentionally
not part of the assertion; the output must contain exactly eight complete
rows and no out-of-bounds data.

Run it on a host with an OpenACC compiler and GPU, for example:

```sh
mcrun -c --no-mpi --openacc -n 64 -s 42 \
  -I . -d /tmp/event_buffer_openacc Test_EventBufferOpenACC.instr
```
