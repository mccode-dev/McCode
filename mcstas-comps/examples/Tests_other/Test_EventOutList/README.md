# The `Test_EventOutList` Fixture

*McStas: deterministic fixture for the generic event-list output API*

## Identification

- **Site:** Tests_other
- **Origin:** DTU
- **Date:** 18 September 2026

## Description

A small, self-contained fixture that exercises the generic event-list output
API `mcevent_out_list` (and the `DETECTOR_OUT_LIST` component macro) added to
the common runtime (`common/lib/share/mccode-r.{h.in,c}`). It establishes a
baseline against the existing low-level `mcdetector_out_list` path.

The `EventOutList` fixture component builds a deterministic row-major buffer in
its `SAVE` section (`buf[r*ncols+c] = r*10 + c`, independent of `ncount` and
seed) and writes it through `DETECTOR_OUT_LIST`. Neutron physics is a
pass-through no-op, so the written list is fully deterministic.

One instrument run covers the whole case matrix:

| Instance   | Params                        | Expected                                            |
|------------|-------------------------------|-----------------------------------------------------|
| `evMulti`  | `nevents=3, ncols=3`          | `ListMulti.dat`: 3 rows x 3 cols, no extension added|
| `evOneCol` | `nevents=3, ncols=1`          | `ListOneCol.dat`: 3 values, one column              |
| `evOne`    | `nevents=1, ncols=3`          | `ListOneEv.dat`: 1 event x 3 columns                |
| `evZero`   | `nevents=0`                   | no-op, no file written                              |
| `evBadCnt` | `nevents=-1`                  | rejected with a warning, no file                    |
| `evBadWid` | `ncols=0`                     | rejected with a warning, no file                    |
| `evNull`   | `nullbuf=1`                   | rejected (null data), warning, no file              |
| `evEmpty`  | `columns=""`                 | `ListEmpty.dat`: safe output with unnamed columns   |

## How to run

The component is not part of the installed library; pass its directory with
`-I`:

```sh
mcrun -n 1000 -I $(pwd) Test_EventOutList.instr          # McCode ASCII
mcrun -n 1000 --format=NeXus -I $(pwd) Test_EventOutList.instr
```

Expected deterministic data (rows = events, columns as named):

```
0 1 2
10 11 12
20 21 22
```

The flavor-neutral component also has a minimal McXtrace companion instrument:

```sh
mxrun -n 1000 \
  -I mcstas-comps/examples/Tests_other/Test_EventOutList \
  mcxtrace-comps/examples/Tests_other/Test_EventOutList/Test_EventOutList_mcxtrace.instr
```

## Links

- [Source code](Test_EventOutList.instr) for `Test_EventOutList.instr`.
- [McXtrace source code](../../../../mcxtrace-comps/examples/Tests_other/Test_EventOutList/Test_EventOutList_mcxtrace.instr)
  for the companion `Test_EventOutList_mcxtrace.instr`.
- [Component](EventOutList.comp) for `EventOutList.comp`.

---
