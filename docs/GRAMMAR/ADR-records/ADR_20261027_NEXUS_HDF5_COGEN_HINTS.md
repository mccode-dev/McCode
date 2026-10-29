# NeXus on native HDF5: code generator link hints say `-lhdf5`

## Status

Accepted, with [#2895](https://github.com/mccode-dev/McCode/pull/2895) (NeXus output on native HDF5, replacing the frozen napi / libNeXus).

## Context

The code generator (`mccode/src/cogen.c.in`) tells the user how to enable NeXus output in two places:

- with `--verbose`, after embedding the runtime: `Compile '<file> -DUSE_NEXUS -lNeXus' to enable NeXus support`;
- with `--no-runtime`, in the dependency list: `Dependency: '-DUSE_NEXUS -lNeXus' to enable NeXus support`.

\#2895 replaces the napi calls in `mccode-r.c` with about 300 lines of native HDF5 code that keep the `NX*` names. `mccode-r.h.in` no longer includes `napi.h`, and NeXus output links only HDF5. Linking `-lNeXus` is no longer needed. On systems without libNeXus it fails, so the hints would send users to a library they don't need.

## Decision

Both hint strings in `cogen.c.in` now say `-DUSE_NEXUS -lhdf5` instead of `-DUSE_NEXUS -lNeXus`. Nothing else in the code generator changes.

The real link flags still come from `NEXUSFLAGS` (`PlatformDefaults.cmake`, toolchains, `mccode_config.json`). They add the HDF5 include and library paths, and `-lhdf5_serial` on Debian/Ubuntu. The hint gives the minimal form, as it did before.

## Consequences

- The generated C code is unchanged. Only messages on stderr change, so there is no grammar or numerical change.
- Users following the hint on a system with HDF5 but without libNeXus now get a working link line.
- An existing `mccode_config.json` that still passes `-lNeXus` keeps working where libNeXus is installed: the runtime's own `NX*` functions take precedence (see #2895).
- mccode-antlr has the same hint, in `translators/c.py`, and defaults its `nexus` flags to `-lNeXus` in `config/platforms.yaml`. It embeds the runtime of the McCode release it resolves, so its defaults can only switch to HDF5 for releases that contain #2895. Tracked in [mccode-antlr#405](https://github.com/mccode-dev/mccode-antlr/issues/405); [mccode-antlr#406](https://github.com/mccode-dev/mccode-antlr/pull/406) makes its hint version-neutral.

## Behaviour

`mcstas --verbose foo.instr` (or `mcxtrace`) prints:

    Compile            'foo.c -DUSE_NEXUS -lhdf5' to enable NeXus support

and `mcstas --no-runtime foo.instr` lists:

    Dependency: '-DUSE_NEXUS -lhdf5' to enable NeXus support
