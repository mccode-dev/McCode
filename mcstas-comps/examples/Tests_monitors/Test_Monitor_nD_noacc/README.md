# The `Test_Monitor_nD_noacc` Instrument

*McStas: compare ordinary event-list storage in `Monitor_nD` and `Monitor_nD_noacc`*

## Description

The instrument sends the same source rays through matching OpenACC-capable
and non-OpenACC monitor implementations. Each pair exercises a normal list
and a fixed-capacity overflowing list.

With `NCount=200`, `ND_acc` and `ND_noacc` each contain 200 rows, while
`ND_acc_ovf` and `ND_noacc_ovf` each contain the first 5 accepted rows. The
numeric rows are identical for each pair on the CPU path, and the associated
histograms still include all 200 events.

## Examples

- **Test: NCount=200 Detector: FlexRef_I=200**

## Links

- [Source code](Test_Monitor_nD_noacc.instr) for `Test_Monitor_nD_noacc.instr`.
