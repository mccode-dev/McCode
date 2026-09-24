# The `Test_abs_logger_nD_scintillator` Instrument

*McStas: Test of the scintillator variant of the monitor_nD like absorption logger*

## Identification

- **Site:** Tests_union
- **Author:** Milán Klausz
- **Origin:** ESS
- **Date:** September 2026

## Description

```text
Test of the scintillator variant of the monitor_nD like absorption logger.

Four GS20 converter tiles are illuminated by a near parallel beam, each with
its own Union_abs_logger_nD_scintillator attached. That logger is written for
one specific position sensitive detector, a GS20 converter layer read out by a
multi-anode photomultiplier tube (MAPMT), and represents the physics from the
neutron conversion up to a detection event being triggered in one of the MAPMT
pixels. Instead of logging each absorption where it happened, it distributes it
over the MAPMT pixel facing that position and its four neighbours, using
tabulated detection efficiencies from a separate Geant4 simulation.

The four loggers cover histogram and event list output, the two MAPMT pixel
sizes, and the use of a conditional:

abs_logger_nD_scintillator                  histogram, 6 mm pixels
abs_logger_nD_scintillator_con              histogram, 6 mm pixels, conditional
abs_logger_nD_scintillator_high_resolution  histogram, 3 mm pixels
abs_logger_nD_scintillator_list             event list, 6 mm pixels

The high resolution logger sets is_high_resolution=1, which selects 3 mm
pixels and the corresponding set of pixel hit efficiency tables, and is binned
twice as finely so that one bin again corresponds to one pixel. It records a lower
intensity than the 6 mm loggers because a detection threshold suppresses
absorptions whose light is shared between neighbouring pixels. With 3 mm
pixels a larger fraction of the area lies close to a pixel boundary, and as
the light spread is a fixed physical width, even the pixel centre is within
its range, so a smaller fraction of absorptions yields an accepted detection.

The conditional is a Union_conditional_standard with an energy window wide
enough to always evaluate true. It therefore exercises the record_to_temp
and write_temp_to_perm code path of the logger without changing which events
end up being recorded, so abs_logger_nD_scintillator_con should agree with
abs_logger_nD_scintillator.

The list mode logger reports zero integrated intensity, since in list mode
the signal is written to the event file rather than accumulated into a
histogram. It therefore has no example line of its own, but is still run as
part of every test to exercise the event list output path.

The Union_master is placed at an odd position and rotation relative to the
detector, which should leave all results unchanged, as the loggers record in
the coordinate system of the geometry they are attached to.
```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|

## Links

- [Source code](Test_abs_logger_nD_scintillator.instr) for `Test_abs_logger_nD_scintillator.instr`.

---
