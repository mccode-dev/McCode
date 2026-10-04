# The `Test_Monitor_nD_list_MPI` Instrument

*McStas: MPI regression test for event-list output (Monitor_nD "list all").*

## Identification

- **Site:** Tests_monitors
- **Author:** P. Willendrup
- **Origin:** DTU
- **Date:** October 2026

## Description

```text
Event lists recorded on several MPI nodes are gathered and written by the
MPI master. This instrument controls which nodes record events, to check
that list output neither hangs nor loses events when some nodes have no
events, when the master has none, or when nodes flush their list buffers
during TRACE a different number of times.

Rays leave the source towards the monitors; the "gate" Arm absorbs them
depending on the node rank and on the 'mode' parameter:
mode=0: only the MPI master records events (the other nodes have none)
mode=1: only the last MPI node records events (the master has none)
mode=2: all nodes record all their events
mode=3: no node records any event
mode=4: master records all its events, node 1 every second one, other nodes
one event each. Run with --bufsiz=1000000 and ncount=2.2e6 per node,
the master flushes its list buffer twice during TRACE, node 1 once
and the others never (a 'list all' Monitor_nD flushes its buffer
when it holds --bufsiz events, if that is at least 1e6).
mode=5: every node records exactly one event (single-row lists)

The "events" monitor writes the list; the "count" monitor is a plain
histogram (MPI_Reduce'd) interleaved with the list output.

Without MPI, all modes except 3 and 5 record all events.

The companion script test_mpi_lists.py runs all modes with mcrun --mpi
in McCode and NeXus formats, and checks the number of listed events.
```

## Examples

- **Test: mode=2 Detector: count_I=6.28235e+06**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| mode | 1 | Which MPI nodes record events, see description. | 2 |

## Links

- [Source code](Test_Monitor_nD_list_MPI.instr) for `Test_Monitor_nD_list_MPI.instr`.

---
