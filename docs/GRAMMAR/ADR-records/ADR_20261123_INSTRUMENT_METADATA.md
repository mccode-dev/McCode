# Instrument-level METADATA

## Status

proposed

## Context

`METADATA type name %{ ... %}` can be attached to a component definition and to a component instance. The grammar documentation also lists it for an instrument, but `instrument.y` never accepted it there: a `METADATA` line in the instrument header is a syntax error.

mccode-antlr has accepted instrument `METADATA` since its first release, in the instrument header after `SHELL`/`SEARCH` and before `DEPENDENCY`. Instruments written with it, e.g. by niess for the ESS BIFROST model, cannot be translated by `mcstas`.

The header is the only unambiguous place for it. After `TRACE`, a `METADATA` line could belong either to the last component instance or to the instrument (bison reports 2 shift/reduce conflicts).

A quoted metadata name was stored with its quotes: `METADATA text "notes"` is named `"notes"`, so `--meta-data=comp:notes` never matches it. mccode-antlr stores `notes`. No `.comp` or `.instr` file in McCode uses a quoted name.

While testing, two bugs in the runtime metadata accessors (`metadata-r.c`) showed up. They affect existing component metadata too:

- `--meta-list` aborts with `double free` for every instrument that has metadata, and would otherwise print only the last source.
- A key without a source part (`::`, `:name`) crashes `--meta-defined`, `--meta-type` and `--meta-data` with a NULL `strcmp`.

## Decision

In `instrument.y`, accept `metadata` (zero or more `METADATA` blocks) in the instrument rule between `SHELL` and `DEPENDENCY`, as in mccode-antlr.

In `metadata.c`, `metadata_construct_table` also adds the instrument blocks, with the instrument name as their source, after all component blocks. `%include`d instruments add their blocks under the master instrument name. On a duplicate name the included one wins, as in mccode-antlr.

For all METADATA (definition, instance, instrument), store a quoted name without its quotes, as a quoted type already was, and as mccode-antlr does. The four `metadatum` alternatives (identifier or string for each) are now one rule.

In `metadata-r.c`, build the `--meta-list` line in place, and let `metadata_table_key_component` return `""` instead of `NULL` when the key has no source part.

## Consequences

The same instrument files are accepted by `mcstas` and `mcstas-antlr`, and give the same metadata table.

`METADATA text "notes"` and `METADATA text notes` now define the same name. A user who looked up the quoted form, `comp:"notes"`, must drop the quotes.

Instrument metadata is looked up like component metadata, with the instrument name as the source, e.g. `--meta-data=my_instr:config`, or `File(metadatakey="my_instr:config")`. A component instance with the same name as the instrument would share that namespace.

No change for instruments without instrument-level metadata: generated C is identical for all McStas examples.

## Behaviour

```c
DEFINE INSTRUMENT my_instr(a=1)
METADATA "application/json" config %{
{"x": 1}
%}
DEPENDENCY "..."
DECLARE
...
```

- `--meta-list` prints `origin last my_instr` (every source, space separated) and exits 0.
- `--meta-defined=my_instr` prints the instrument's metadata names. `--meta-type=my_instr:config` prints `application/json`, `--meta-data=my_instr:config` prints `{"x": 1}`.
- `METADATA text "notes" %{ ... %}` is found with `--meta-data=my_instr:notes`.
- `--meta-defined=::` and `--meta-type=::` exit 1 instead of crashing.
- Tested on Linux x86_64 with gcc and bison 3.8.2: no grammar conflicts. Generated C was compared before/after for the 375 McStas example instruments: all identical. A test instrument with instrument, instance, quoted-name and `%include`d instrument metadata gave the table above, with no AddressSanitizer or UBSan errors in the accessors. McXtrace shares the parser and runtime; it was not built. macOS and Windows were not tested.
