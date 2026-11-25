[&larr; back to overview](README.md)

# mcmerge / mxmerge

Merges 2 or more datasets (runs or scans, ASCII or NeXus) from separate runs of the same simulation, as if they were one longer run:

```
mcmerge -d OUT [--format McCode|NeXus] DATASET DATASET ...
```

| Option | Description |
|---|---|
| `input` | dataset directories, 2 or more |
| `-d OUT` | output directory, must not exist |
| `--format FMT` | `McCode` (ASCII) or `NeXus`, one of the input formats (default: NeXus if any input is) |

Requires `numpy` and `h5py`.

- Give each run its own seed, e.g. `mcrun -s 1000` and `mcrun -s 2000`. With MPI, the nodes use `Seed` to `Seed+Nodes-1`, so keep the seeds at least that far apart.
- Scans are merged point by point.
- Event lists (`Monitor_nD`) are joined, with `p` scaled by each run's share of `Ncount`. In NeXus, the output does not need the `mccode_node*.h5` files of MPI runs.
- The output is a copy of the first dataset in the output format, with new numbers. It lists its inputs as `Merged_from`, has `Seed: -1` and no `Nodes`, and can be merged again.
- Other files, e.g. MCPL, are not merged but listed on screen.

## What is refused

- inputs that differ: instrument, parameters, McCode version, monitors, limits or bins, number of scan points
- same or overlapping seeds
- unfinished runs and intermediate saves
- data without `mean: yes` or `mean: no` in the monitor headers, or event lists without `N` in `values`, i.e. from older McCode versions
- event lists without `p`, with a second weight `I`, with missing `mccode_node*.h5` files, or truncated, i.e. with fewer events than `N` in `values` (e.g. `list=n`, use `list all`)

When ASCII and NeXus are merged, what is only in the NeXus data gives a warning instead.

## Limitations

- Polarisation monitors are merged approximately: the mean is weighted with the number of rays `N`, not with their intensity.
- Neutron or photon IDs (`n`) in event lists are not unique, as with MPI.
- Runs that read the same MCPL file are not independent, however different the seeds. This is not detected.
- To ASCII: geometry, component parameters and `BINS` of the NeXus data are left out.

---
[&larr; back to overview](README.md)
