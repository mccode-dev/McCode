# Event_monitor_simple test

This fixture verifies that `Event_monitor_simple` uses the common event-list
writer instead of a private `.log` file. The McStas output is `events.dat` with
three rows and twelve columns:

```text
id x y z vx vy vz t sx sy sz p
```

The McXtrace companion writes the corresponding photon columns:

```text
id x y z kx ky kz t Ex Ey Ez p
```

Run the McStas fixture from this directory after rebuilding the McStas
installation:

```sh
mcrun -c -n 3 -I . -d /tmp/event_monitor_simple Test_EventMonitorSimple.instr
```

Run the McXtrace companion from the repository root after rebuilding the
McXtrace installation:

```sh
mxrun -c -n 3 -I . -d /tmp/event_monitor_simple_mcxtrace \
  mcxtrace-comps/examples/Tests_other/Test_EventMonitorSimple/Test_EventMonitorSimple_mcxtrace.instr
```

Both outputs should have `type: list(12, 3)`, a `variables:` header matching
the columns above, and component metadata identifying `events`.
