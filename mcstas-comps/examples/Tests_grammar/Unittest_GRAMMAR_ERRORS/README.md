# Grammar error cases (must FAIL code generation)

Each `*.instr.fail` file here is an instrument that the code generator must
**reject with an error**. They belong with the positive tests
`Unittest_JUMP_RELATIVE`, `Unittest_INHERIT_DIAPHRAGM` and
`Unittest_USERVARS_TYPES`. All of them document the decisions in
[ADR_20261007_GRAMMAR_FIXES](../../../../docs/GRAMMAR/ADR-records/ADR_20261007_GRAMMAR_FIXES.md).
They are mainly meant as reference cases for other implementations of the
grammar, such as mccode-antlr.

The files use the `.instr.fail` extension so that test and documentation
tools, which look for `*.instr`, do not try to build them. To check one,
copy it to `*.instr` and run the code generator:

```bash
cp Err_MYSELF_in_parameters.instr.fail /tmp/Err_MYSELF_in_parameters.instr
mcstas /tmp/Err_MYSELF_in_parameters.instr   # must exit non-zero
```

| File | Case | Before the fix | Now |
|---|---|---|---|
| `Err_MYSELF_in_parameters` | `MYSELF` in the instance's own parameters | accepted, meant the previous instance | `ERROR: MYSELF can not be used here ...` |
| `Err_JUMP_unknown_target` | `JUMP` to a name that is no component | accepted, silently `JUMP MYSELF` | `JUMP at component b: target nosuch is not a component ...` |
| `Err_JUMP_out_of_range` | `JUMP NEXT(2)` past the last component | accepted, jumped to component 2 | `JUMP at component b: target NEXT_2 is not a component ...` |
| `Err_COPY_undefined` | `COPY(PREVIOUS)` on the first instance | code generator segfault | `ERROR: COPY of an undefined component instance ...` |
| `Err_unknown_component` | unknown component class | error message, then segfault | error message, exit code 1 |
| `Err_illegal_pointer_type` | `int *x` instrument parameter | garbled message (`$s*`, wrong line) | `ERROR: Illegal type int* for instrument parameter x ...` |

`MYSELF` remains valid from `WHEN` onwards (`WHEN`, `AT`, `ROTATED`, `JUMP`, ...).
