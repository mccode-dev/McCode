# McStas / McXtrace COMPONENT grammar

This is one of two companion cheat sheets: this file covers `.comp` **component**
files; the companion [`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md) covers `.instr`
**instrument** files. Each is written to stand alone — a little material (the
reserved-keyword table, the particle-terminology note, the shared-grammar note) is
intentionally duplicated in both. Keywords are case-insensitive but conventionally written in UPPERCASE; identifiers
are case-sensitive C identifiers. Sources are listed at the end of the document (§13).

**On "McStas" vs "McXtrace" vs "particle":** the instrument/component grammar itself —
every keyword and syntax rule in this document — is identical between the two codes; it's
one shared parser. What differs is the physics carried by the traced entity: McStas
traces neutrons, McXtrace traces X-ray photons. This cheat sheet uses the neutral term
**"particle"** throughout for the thing being traced through a component, and only calls
out "neutron" or "photon" specifically where the two codes' *behaviour* (not grammar)
actually differs — chiefly, the fields making up the particle's state, which every
component's `TRACE` code reads and writes:

| | McStas (neutron) | McXtrace (photon) |
|---|---|---|
| Position | `x, y, z` [m] | `x, y, z` [m] |
| Direction/momentum | velocity `vx, vy, vz` [m/s] | wavevector `kx, ky, kz` [Å⁻¹] |
| Extra kinematic field | — | phase `phi` [rad] |
| Polarization | spin `sx, sy, sz` | polarization vector `Ex, Ey, Ez` |
| Time | `t` [s] | `t` [s] |
| Weight | `p` | `p` |

A component's `TRACE` C code operates directly on these fields (by whichever names apply
to the code you're writing for). `USERVARS` fields (§7 below) are appended to the same
per-particle struct — the grammar for declaring them is identical in both codes.

---

**Notation used for grammar skeletons in this document:** `[ ... ]` marks an element (or
whole clause) as *optional*; `( a | b | c )` marks a *required choice* among alternatives.
Both are meta-notation and never appear literally in a `.instr`/`.comp` file. Everything
else — including bare `{ }` and, especially, `%{ ... %}` — is **literal syntax**: `%{ %}`
is the actual embedded-C-code delimiter, and bare `{ }` shows up as-is in real code (e.g.
a `vector` default like `axis={0,1,0}`). The two never mean "optional" in this document.

## 1. Component file skeleton

```c
DEFINE COMPONENT name [INHERIT parent]

SETTING PARAMETERS (s1, s2 = default, ...)
[OUTPUT PARAMETERS (o1, o2, ...)]      // legacy; parses but effect 

[DEPENDENCY "..."]
[NOACC] // marks that component TRACE code can not be offloaded to GPU via OpenACC

SHARE
%{ // C shared once across all instances of this component %}

DECLARE
%{ // becomes the per-instance member fields of this component's generated C struct, see §5 %}

USERVARS
%{ // per-particle fields, same mechanism as instrument-level USERVARS %}

INITIALIZE
%{ // run once per instance, at start %}

TRACE
%{ // the actual particle/component interaction %}

[SAVE %{ ... %}]
[FINALLY %{ ... %}]
[MCDISPLAY %{ ... %}]                  // alias: DISPLAY

END
```

Section order above is what the grammar enforces; every section except `DEFINE COMPONENT
name` and `TRACE`/`END` is optional. A component definition may also carry one or more
`METADATA` blocks (§8) and, in the `TRACE` section of an *instrument* that uses this
component, be positioned/duplicated/grouped — see the companion instrument-grammar file
for that side of things.

## 2. `DEFINE COMPONENT` header

```c
DEFINE COMPONENT Slit
DEFINE COMPONENT Place INHERIT Arm          // whole-component inheritance, see §9
DEFINE COMPONENT MyComp NOACC               // GPU-incompatible component class, see §6
```

This marks the beginning of the definition and names the component. `INHERIT parent`
(§9) and `NOACC` (§6) are both optional modifiers on this header line.

## 3. `SETTING PARAMETERS` 
```c
SETTING PARAMETERS (radius, height, pack = 1, string filename = "out.dat",
                     vector axis = {0,1,0}, symbol my_func)
```

* **`SETTING PARAMETERS`** is the *only* supported way to declare a component's
  parameters. They compile to ordinary (usually `double`-typed) C variables and accept
  arbitrary C expressions as actual arguments from the instrument file.
* Occasionally you will find components with an **`OUTPUT PARAMETERS`** list (alias `PRIVATE PARAMETERS`) which is silently ignored. Its former job was to trigger
  identifier-renaming so `DECLARE` variables from different instances of the same
  component wouldn't collide; that's now handled structurally instead (see §5). You may
  still see it in older `.comp` files; it's harmless to leave in place, and safe to delete.
* Parameter types, all under `SETTING PARAMETERS`:
  - `double` (the default if untyped), `int`, `string` (or `char *`)
  - `vector` (or `double *`) — a numeric array literal (`axis={0,1,0}`) or the name of a
    `double[]` declared in the instrument's `DECLARE`; defaults to `NULL` unless given an
    explicit default.
  - `symbol` — a bare C identifier (e.g. to pass a user function pointer or preprocessor
    symbol through unevaluated).
  **Note:** instrument parameters ([`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md), §2) support only
  double/int/string/vector — `symbol` is a *component*-parameter-only type.
* `name = value` marks a parameter optional, exactly as for instrument parameters. Don't
  give a natural-default-free parameter (guide length, slit size, ...) a default value
  just for convenience — it removes a useful "you forgot to set this" error.

## 4. Component-level `DEPENDENCY`

```c
DEPENDENCY "-lLIB1 -lLIB2 .."
```

Works exactly like the instrument-level line ([`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md), §3) — appends
compiler/linker flags needed by this component's C code (external library calls, etc.).
Must precede `SHARE`/`DECLARE`. Flags from every component actually used in an instrument
are concatenated with the instrument's own `DEPENDENCY` line into one `CFLAGS=...` string.

## 5. `SHARE` vs `DECLARE`

```c
SHARE
%{
  // emitted ONCE total, no matter how many instances exist
%}

DECLARE
%{
  // becomes the per-instance member fields of this component's generated C struct
%}
```

* **`SHARE`** code is emitted **once** in the generated simulation, regardless of how many
  instances of the component the instrument uses — put shared types, helper functions, and
  large static tables here. Located before `DECLARE`.
* **`DECLARE`**, in a fairly raw/literal sense, becomes the internals of a per-instance
  component struct in the running instrument. This means that every declaration in this block 
  should be of the form
  
```c
DECLARE %{
  double* myvar; // "type varname;" ONLY — no initializers, no multi-declarations
%}
```  
* Under the hood, each component *type* gets
  its own generated C struct, and every instance of that component in the instrument gets
  its own separately-instantiated variable of that struct type. The variables you declare
  in `DECLARE` are parsed straight out of your C code and become member fields of that
  struct.
* `%include "file"` inside a `%{ %}` block in either section pulls in a runtime C library
  from the component search path (see the [`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md)'s `%include` section).

## 6. `NOACC` — marking a component class GPU-incompatible

```c
DEFINE COMPONENT MyComp NOACC
```

* Declares that this component **class** can never be compiled for GPU/OpenACC execution
  (e.g. its `TRACE` section calls out to a C library with no GPU-compatible
  implementation). This automatically pulls in the `DEPENDENCY "-DFUNNEL"` flag, enabling
  McStas/McXtrace's CPU/GPU "funneling" mode, so that an instrument mixing GPU-friendly and
  `NOACC` components can still run under `--openacc` — at the cost of funneling particles
  back to the CPU just for that component's computation.
* The complementary, **per-instance** keyword is `CPU`, set in an instrument's `TRACE`
  section (not here) to force one specific instance of an otherwise GPU-capable component
  to run on CPU only — see the companion instrument-grammar file.


## 7. `USERVARS`, `INITIALIZE`, `TRACE`, `SAVE`, `FINALLY`, `MCDISPLAY`

```c
USERVARS
%{
  double myvar;   // "type varname;" ONLY — no initializers, no multi-declarations
%}

INITIALIZE
%{
  // C code run once per instance, at simulation start; may modify SETTING parameters
%}

TRACE
%{
  // the actual computation of the interaction between the particle and the component
%}

SAVE
%{
  // C code executed whenever a (partial) data save is triggered
%}

FINALLY
%{
  // C code executed once, at simulation end
%}

MCDISPLAY               // alias: DISPLAY
%{
  // C code to draw the component geometry for mcdisplay
%}
```

* **`USERVARS`** at component level uses the identical mechanism as instrument-level
  `USERVARS` ([`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md), §4): declarations of the exact form `type varname;`,
  appended to the same per-particle state struct as the built-in fields (see the state
  table above) and to any instrument-level `USERVARS` fields. Prefer these over `DECLARE`
  globals for any per-particle flag read across components/`EXTEND` blocks — they stay
  correct under `SPLIT`, MPI, and GPU/OpenACC, where a shared `DECLARE` global would not.
* **`TRACE`** is mandatory for components interacting with the particle and holds the 
  component'score physics: it should propagate the
  particle to the component (helper macros like `PROP_Z0`/`PROP_DT` are typically used),
  compute the interaction, and call the `SCATTER` macro whenever the particle's state is
  meaningfully changed/detected — this both drives correct 3D visualization and is what
  `GROUP` ([`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md), §7) tests to decide which group member "wins". `ABSORB`
  terminates tracing for that particle; `ALLOW_BACKPROP` (called before a `PROP_` call)
  permits negative-time backpropagation instead of the default absorb-on-negative-time
  behaviour.
* **`SAVE`**, **`FINALLY`**, and **`MCDISPLAY`**/`DISPLAY` are all optional, and behave
  analogously to their instrument-level namesakes ([`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md), §15) — `SAVE` on
  (partial) data save, `FINALLY` once at simulation end, `MCDISPLAY` to draw geometry.

## 8. `METADATA` at component-definition level

```c
DEFINE COMPONENT File
SETTING PARAMETERS (string filename, string metadatakey = "", int keep = 0)
METADATA "text/plain" default_note
%{
  Default per-definition metadata text.
%}
...
END
```

* `METADATA type name %{ ...text... %}` may appear inside a component **definition**
  (any number of blocks) — attaches a named, typed block of arbitrary text, with zero
  effect on the simulated physics, purely for downstream tools (IDF fragments, JSON
  geometry blobs, CIF snippets, notes) to retrieve without parsing generated C. `type`/
  `name` are bare identifiers or quoted strings (quote for MIME-style types like
  `"application/json"`).
* Definition-level metadata is inherited by **every instance** of the component, and (per
  the usual `INHERIT` rules, §9) by any component that `INHERIT`s from it. An
  instance-level `METADATA` block of the same `name`, attached in an instrument's `TRACE`
  section (see the companion instrument-grammar file, §12), overrides the
  definition-level one for that one instance.
* Inspect from the command line via `mcrun --meta-list`, `--meta-defined=comp[:name]`,
  `--meta-type=comp:name`, `--meta-data=comp:name`.

## 9. `INHERIT` — component heritage

Whole-component inheritance (copies every section from the parent, minus its doc header;
override/extend any section by re-declaring it):

```c
DEFINE COMPONENT Place INHERIT Arm
END
```
(real, minimal example from `mcstas-comps/optics/Place.comp`)

```c
DEFINE COMPONENT child_name INHERIT parent_name
  SETTING PARAMETERS (newpar1, newpar2)
  INITIALISE
    INHERIT parent_name
    EXTEND
    %{ // C code appended to parent_name's INITIALIZE %}
  SAVE
  %{ // C code that fully REPLACES parent_name's SAVE %}
END
```

Per-section `INHERIT parent_name` (optionally followed by `EXTEND %{ ... %}`) is written
**inside that section's own block**, right after the section keyword — not as a top-level
clause. A bare `INHERIT parent` for a section replaces it wholesale with the parent's code;
`INHERIT parent EXTEND %{ ... %}` instead appends the given code after the parent's.

You can also pull in individual sections from **any** component without inheriting the
whole definition (the top-level `DEFINE COMPONENT` line doesn't itself use `INHERIT`):

```c
DEFINE COMPONENT name(...)
SETTING PARAMETERS (...)
DECLARE
  INHERIT parent1
INITIALISE
  INHERIT parent2
  EXTEND %{ // appended to parent2's INITIALIZE %}
TRACE
  INHERIT parent3
END
```

Mixing sections from different sources this way needs care around variable naming
collisions. `EXTEND` used this way (attached to an `INHERIT`ed section, inside a
component definition) is a different mechanism from the instrument-level `EXTEND` modifier
on a `COMPONENT` instance ([`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md), §9) — same keyword, but one extends an
inherited *section of a component definition*, the other extends one *instance's* `TRACE`
from the instrument file.

## 10. `END` (component level)

```c
END
```

Mandatory; closes the component definition.

---

## 11. Reserved keyword reference

All tokens below are reserved and case-insensitive; do not reuse them as C identifiers.
"Scope": **I** = valid in instrument definitions ([`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md)), **C** = valid in
component definitions (this file).

| Keyword | Scope | Meaning |
|---|---|---|
| `ABSOLUTE` | I | Global coordinate frame for `AT`/`ROTATED`. |
| `AT` | I | Component position. |
| `COMPONENT` | I | Declares a component instance. |
| `COPY` | I | Duplicate a preceding instance. |
| `CPU` | I | Force one instance to run on CPU only. |
| `DECLARE` | I, C | C global declarations (§5). |
| `DEFINE` | I, C | Starts an `INSTRUMENT` or `COMPONENT` definition (§2). |
| `DEFINITION` | C | No longer supported — raises a compile error if used (§3). |
| `DEPENDENCY` | I, C | Compiler/linker flags (§4). |
| `DISPLAY` | C | Alias for `MCDISPLAY` (§7). |
| `END` | I, C | Ends the definition (§10). |
| `EXTEND` | I, C | Appends C code after a `TRACE`, or after an `INHERIT`ed section (§9). |
| `FINALLY` | I, C | C code run at simulation end (§7). |
| `GROUP` | I | Exclusive ("XOR") component group. |
| `%include` | I, C | Import a file — component/instrument, or (inside `%{ %}`) a C library. |
| `INHERIT` | C | Derive a component (whole or per-section) from a parent (§9). |
| `INITIALIZE` / `INITIALISE` | I, C | C code run once at start-up (§7). |
| `ITERATE` | I | `JUMP` loop-count clause. |
| `JUMP` | I | Conditional/iterative non-sequential propagation. |
| `MCDISPLAY` | C | C code to draw component geometry (§7, alias `DISPLAY`). |
| `METADATA` | I, C | Attach a named, typed text block (§8). |
| `MYSELF` | I | Self-reference for `COPY`/`JUMP`. |
| `NEXT` / `NEXT(n)` | I | Forward instance reference, for `JUMP`. |
| `NOACC` | C | Marks a component class GPU-incompatible (§6). |
| `OUTPUT` | C | Alias for `PRIVATE PARAMETERS`; parses but no longer has any effect (§3, §5). |
| `PARAMETERS` | C | Qualifier after `SETTING`/`OUTPUT`/`PRIVATE` (and the rejected `DEFINITION`) (§3). |
| `PREVIOUS` / `PREVIOUS(n)` | I, C | Backward instance reference. |
| `PRIVATE` | C | Alias for `OUTPUT PARAMETERS`; parses but no longer has any effect (§3, §5). |
| `RELATIVE` | I | Relative coordinate frame for `AT`/`ROTATED`. |
| `REMOVABLE` | I | Skip this component when the file is `%include`'d. |
| `ROTATED` | I | Component orientation. |
| `SAVE` | I, C | C code run on (partial) data save (§7). |
| `SEARCH` | I | Extra component/instrument search directory — TRACE-section only. |
| `SETTING` | C | Component parameters compiled as ordinary C variables (§3). |
| `SHARE` | C | C code emitted once, shared across all instances (§5). |
| `SHELL` | I, C | Run a shell command at parse time. |
| `SPLIT` | I | Boost downstream statistics by event repetition. |
| `TRACE` | I, C | The particle-propagation / component-interaction section (§7). |
| `USERVARS` | I, C | Per-particle custom fields (§7). |
| `WHEN` | I | Conditional component activation, or `JUMP` condition. |

### Obsolete / rejected keywords
`STATE PARAMETERS`, `POLARISATION PARAMETERS`, and `DEFINITION PARAMETERS` (§3) are all
still recognised by the grammar but **immediately raise a compile error if used** —
leftovers from older grammar versions (the first two long-standing, `DEFINITION
PARAMETERS` more recently). Remove them if you see them in an old `.instr`/`.comp` file.
Separately, `OUTPUT PARAMETERS`/`PRIVATE PARAMETERS` (§3, §5) are **not** rejected — they
still parse without error — but no longer have any effect; they're inert rather than
erroring. All four are usually a sign the file, or the documentation it was written
against, predates the current McStas/McXtrace.

---

## 12. Source-material for this reference

* `mccode/src/instrument.y`, `mccode/src/instrument.l` — the Bison/Flex grammar itself
  (authoritative; shared verbatim between McStas and McXtrace).
* `docpkg/manuals/mcstas/manual.pdf`, chapter 4, "The McStas kernel and meta-language",
  especially §4.5 ("Writing component definitions") and §4.6 ("Extending component
  definitions").
* `docpkg/manuals/mcxtrace/manual.pdf`, chapter 6 (equivalent kernel/meta-language chapter)
  and section 6.1 (notational conventions / particle state fields).
* `mcstas-comps/optics/Place.comp` (minimal `INHERIT` example), `mcstas-comps/optics/
  Diaphragm.comp`, `mcxtrace-comps/optics/Diaphragm.comp`, `mcxtrace-comps/optics/
  Place.comp` (further `INHERIT` users), and the `mcstas-comps/examples/Tests_other/
  test_File/test_File.instr` / `mcxtrace-comps/examples/Tests_other/test_File/
  test_File.instr` pair (`METADATA` usage against the `File` component).

See also: [`INSTRUMENT grammar`](../Instrument/INSTRUMENT_Grammar.md)
