# Keep the last line of a C code file that lacks a final newline

## Status

proposed

## Context

Inside `%{ ... %}` blocks, and in files embedded there with `%include`, the lexer (`instrument.l`, state `<ccode>`) only knew newline-terminated lines. A last line without a newline matched no rule, so flex echoed it to stdout and it was missing from the generated C.

Seen with the ESS BIFROST model, where `rfc-lib.h` and `rfc-lib.c` (mcstas-radial-filter-collimator) end in `#endif // RFC_LIB_H` and `}` without a newline. McStas printed `#endif // RFC_LIB_H}` and the compiler reported `function definition is not allowed here` for everything that followed.

mccode-antlr accepts such files, so they are easily produced and go unnoticed.

## Decision

Add one `<ccode>` rule matching a line without a newline. It can only be the longest match on the last line of a file. The rule pushes the line back with a newline appended, so it is rescanned by the existing rules (`%}`, `%include`, plain code line).

## Consequences

Library and code files no longer need to end with a newline. No new syntax, and nothing changes for files that do end with one.

## Behaviour

- An unterminated last line is kept, and gets a newline in the generated C.
- Generated C was compared before/after for the 369 McStas example instruments: 365 identical, 2 fail to generate both before and after. 2 differ by one restored line (`Supermirror_thin_substrate`, `Test_lib_supermirror_lib`): the closing comment of `supermirror-lib.h`, which also lacks a final newline and was silently dropped until now.
- The lexer is shared with McXtrace; only McStas was built and tested (macOS arm64).
