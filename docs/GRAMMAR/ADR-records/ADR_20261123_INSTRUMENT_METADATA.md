# Instrument-level METADATA

## Status

proposed

## Context

`METADATA type name %{ ... %}` can be attached to a component definition and to a component instance. The grammar documentation also lists it for an instrument, but `instrument.y` never accepted it there: a `METADATA` line in the instrument header is a syntax error.

mccode-antlr has accepted instrument `METADATA` since its first release, in the instrument header after `SHELL`/`SEARCH` and before `DEPENDENCY`. Instruments written with it, e.g. by niess for the ESS BIFROST model, cannot be translated by `mcstas`.

The header is the only unambiguous place for it. After `TRACE`, a `METADATA` line could belong either to the last component instance or to the instrument (bison reports 2 shift/reduce conflicts).

A quoted metadata name was stored with its quotes: `METADATA text "notes"` is named `"notes"`, so `--meta-data=comp:notes` never matches it. mccode-antlr removes the quotes, and puts them back only for a name with spaces, so that it stays one entry in the space-separated `--meta-defined` output. No `.comp` or `.instr` file in McCode uses a quoted name.

While testing, two bugs in the runtime metadata accessors (`metadata-r.c`) showed up. They affect existing component metadata too:

- `--meta-list` aborts with `double free` for every instrument that has metadata, and would otherwise print only the last source.
- The key `::` crashes `--meta-defined`, `--meta-type` and `--meta-data` with a NULL `strcmp`, and `File(metadatakey="::")` crashes in INITIALIZE.

## Decision

In `instrument.y`, accept `metadata` (zero or more `METADATA` blocks) in the instrument rule between `SHELL` and `DEPENDENCY`, as in mccode-antlr.

In `metadata.c`, `metadata_construct_table` also adds the instrument blocks, after all component blocks. Each block has the instrument that defined it as its source, also when that instrument is `%include`d. Only a block with the same source and name replaces another, i.e. a repeated name in one instrument header.

mccode-antlr kept the source, but replaced instrument blocks by name alone. As the table is looked up by source and name, a `%include`d instrument silently removed the blocks of the same name from the instruments included before it. mccode-antlr now also replaces by source and name (mccode-antlr branch `metadata-source-name-key`).

For all METADATA (definition, instance, instrument), store a quoted name without its quotes, as a quoted type already was. A name with spaces keeps them, as in mccode-antlr. The four `metadatum` alternatives (identifier or string for each) are now one rule. mccode-antlr did not escape these quotes in the generated C, which then did not compile; it now does.

In `pygen.c.in`, write instrument blocks with `instr.add_METADATA(name=..., type=..., value=...)`, guarded by `hasattr`: McStasScript has no instrument METADATA yet, so without it the script prints a warning and continues. All METADATA text is now written as a raw Python string, so escapes such as `\"` in JSON reach McStasScript unchanged.

In `metadata-r.c`, build the `--meta-list` line in place, and let `metadata_table_key_component` return `""` instead of `NULL` when the key has no source part.

## Consequences

The same instrument files are accepted by `mcstas` and `mcstas-antlr`, and, with the mccode-antlr change below, give the same metadata table.

`METADATA text "notes"` and `METADATA text notes` now define the same name. A user who looked up the quoted form, `comp:"notes"`, must drop the quotes. A name with spaces is looked up with its quotes, as before: `comp:"two words"`.

Every block can be looked up under the instrument that defined it, e.g. `--meta-data=included_instr:name`, whatever the order of `%include`s. Component instances cannot clash in the same way: an included instance with the name of an existing one is an error in both translators, unless it is `REMOVABLE`, in which case it is dropped with its metadata.

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
- `METADATA text "notes" %{ ... %}` is found with `--meta-data=my_instr:notes`, `METADATA text "two words" %{ ... %}` with `--meta-data='my_instr:"two words"'`.
- If `my_instr` `%include`s `inc` and both define `dup`, `my_instr:dup` and `inc:dup` are both found.
- `--meta-defined=::` and `--meta-type=::` exit 1 instead of crashing.
- Tested on Linux x86_64 with gcc and bison 3.8.2: no grammar conflicts. Generated C was compared before/after for the 375 McStas example instruments: all identical. A test instrument with instrument, instance, quoted-name and nested `%include`d instrument metadata (repeated names in each) gives the same metadata table as mccode-antlr with the change above, with no AddressSanitizer or UBSan errors in the accessors. McXtrace shares the parser and runtime; it was not built. macOS and Windows were not tested.
