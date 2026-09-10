# McStas / McXtrace INSTRUMENT grammar

This is one of two companion cheat sheets: this file covers `.instr` **instrument**
files; the companion [`COMPONENT grammar`](../Component/COMPONENT_Grammar.md) covers `.comp` **component**
files. Each is written to stand alone — a little material (the reserved-keyword table,
the particle-terminology note, the shared-grammar note) is intentionally duplicated in
both. Keywords are case-insensitive but conventionally written in UPPERCASE; identifiers
are case-sensitive C identifiers. Sources are listed at the end of the document (§18).

**On "McStas" vs "McXtrace" vs "particle":** the instrument/component grammar itself —
every keyword and syntax rule in this document — is identical between the two codes; it's
one shared parser. What differs is the physics carried by the traced entity: McStas
traces neutrons, McXtrace traces X-ray photons. This cheat sheet uses the neutral term
**"particle"** throughout for the thing being traced through the instrument, and only
calls out "neutron" or "photon" specifically where the two codes' *behaviour* (not
grammar) actually differs — chiefly, the fields making up the particle's state:

| | McStas (neutron) | McXtrace (photon) |
|---|---|---|
| Position | `x, y, z` [m] | `x, y, z` [m] |
| Direction/momentum | velocity `vx, vy, vz` [m/s] | wavevector `kx, ky, kz` [Å⁻¹] |
| Extra kinematic field | — | phase `phi` [rad] |
| Polarization | spin `sx, sy, sz` | polarization vector `Ex, Ey, Ez` |
| Time | `t` [s] | `t` [s] |
| Weight | `p` | `p` |

`USERVARS` fields (§4 below) are appended to whichever of these two per-particle state
structs applies for the code you're using — the grammar for declaring them is identical.

---

**Notation used for grammar skeletons in this document:** `[ ... ]` marks an element (or
whole clause) as *optional*; `( a | b | c )` marks a *required choice* among alternatives.
Both are meta-notation and never appear literally in a `.instr`/`.comp` file. Everything
else — including bare `{ }` and, especially, `%{ ... %}` — is **literal syntax**: `%{ %}`
is the actual embedded-C-code delimiter, and bare `{ }` shows up as-is in real code (e.g.
a `vector` default like `axis={0,1,0}`). The two never mean "optional" in this document.

## 1. Instrument file skeleton

```c
DEFINE INSTRUMENT name(parameters)

[SHELL "command"]                    // rare, pre-parse shell command(s)
[DEPENDENCY "linker/compiler flags"] // linker-instructions

DECLARE
%{
  // global C declarations
%}

USERVARS
%{
  double my_per_particle_flag;   // fields appended to every particle's state
%}

INITIALIZE
%{
  // C code run once at simulation start
%}

TRACE
  [SEARCH "extra/search/dir"]    // optional, only valid here (see §3)
  COMPONENT ... AT ...
  COMPONENT ... AT ...
  ...

[SAVE
%{
  // C code run whenever data is (re)saved
%}]

[FINALLY
%{
  // C code run once at simulation end
%}]

END
```

`DECLARE`, `USERVARS`, `INITIALIZE`, `SAVE`, and `FINALLY` are all optional. `TRACE` and the
final `END` are mandatory.

## 2. `DEFINE INSTRUMENT` and its parameters

```c
DEFINE INSTRUMENT test(d1, double d2, int i, char *s1, string s2,
                        d3 = 1, string s4 = "hello",
                        double lambda/"AA" = 4.0, double L2/"m")
```

* Default parameter type is `double`. Explicit types: `double`, `int`, `string` (or
  `char *`, a synonym), and `vector` (written `double *`, e.g. `double *axis`).
  **Unlike component parameters (see the [`COMPONENT grammar`](../Component/COMPONENT_Grammar.md)), instrument parameters do not
  support the `symbol` type** — per the grammar, `instr_formal` only accepts
  double/int/string/vector.
* `param = value` makes a parameter optional; omitted values fall back to the default at
  run time (or the user is prompted interactively).
* `param/"unit"` attaches a **documentation-only** unit string picked up by `mcdoc`/`mcgui` —
  it is never converted or checked at run time. Can be combined with a default:
  `double L2/"m" = 3.0`.
* Instrument parameters are visible everywhere in `INITIALIZE`, `TRACE`/`EXTEND`, `SAVE`,
  `FINALLY` — but *not* automatically inside component `EXTEND` blocks' DECLARE-level
  globals; copy them into a `DECLARE` global in `INITIALIZE` if a component's C code needs
  a value that isn't a component parameter.

## 3. Top-level instrument keywords (before `DECLARE`)

```c
SHELL "some command run prior to code generation"
DEPENDENCY " -DSOME_DEFINE -lsome -L/some/lib -I/some/include "
```

* `SHELL "cmd"` runs an arbitrary shell command while `mcstas`/`mcxtrace` parses the file
  (before code generation). The compiler aborts if the command's exit status is non-zero.
  Rare in the shipped examples (only one hit across the whole `mcstas-comps/examples` tree);
  **never `%include` an untrusted `.instr` file that contains a `SHELL` line** — it executes
  arbitrary code on the machine running the translator.
* `DEPENDENCY "..."` appends compiler/linker flags (e.g. `-lgsl -lgslcblas`, `-DUSE_OFF`,
  `-DMCPLPATH=GETPATH(data)`). Flags from the instrument and from every used component are
  concatenated and printed as `CFLAGS=...`; `mcrun` picks this up automatically. Real
  examples: `DEPENDENCY " -DLOADPATH=GETPATH(data) "`, `DEPENDENCY "-DMCPLPATH=GETPATH(data)"`.
  (A component definition may also carry its own `DEPENDENCY` line — see the companion
  component-grammar file.)
* `SEARCH` can only appear
  **inside `TRACE`**, interleaved with `COMPONENT` lines (typically placed first, right after
  `TRACE`):

  ```c
  TRACE
  SEARCH "/add/a/component/search/directory/"
  SEARCH SHELL "wget https://some/component/file.comp"
  COMPONENT Origin = Progress_bar() AT (0,0,0) ABSOLUTE
  ...
  ```
  `SEARCH SHELL "cmd"` runs `cmd` and adds each line of its stdout as a search directory
  (e.g. `pkg-config --variable=compdir my-neutron-lib`). This mechanism is used mainly by
  Greg Tucker's `mcstas-antlr` infrastructure for BIFROST-style external component repos —
  it does not appear anywhere in the shipped `mcstas-comps`/`mcxtrace-comps` examples trees.

## 4. `DECLARE` / `USERVARS` / `INITIALIZE`

```c
DECLARE
%{
  double some_global_var;
%}

USERVARS
%{
  double myvar;   // "type varname;" ONLY — no initializers, no multi-declarations
%}

INITIALIZE
%{
  myvar = sqrt(PI*input_var)*rand01();
%}
```

* `DECLARE` globals are ordinary global C variables — shared, mutable state across *all*
  in-flight particles. Standard K&R/ISO-C syntax; `math.h` functions like `sin`/`cos`
  use **radians**, even though McStas/McXtrace's own `ROTATED` angles are in **degrees**.
* `USERVARS` appends fields to each **particle's own
  state** (see the state-field table above), so they travel per-particle. This is what
  makes them safe under `SPLIT`, MPI, and GPU/OpenACC execution, where a shared `DECLARE`
  global would be read/written by multiple particles concurrently. `USERVARS` may be
  declared at instrument level and/or inside individual component definitions (see the
  companion component-grammar file) — instrument- and component-level `USERVARS` fields all
  land in the same per-particle struct and are visible from any `TRACE`/`EXTEND`/`WHEN`/
  `MCDISPLAY` code, referenced directly by name like the built-in state fields. Declarations
  must be exactly `type varname;` — no default initializers.
* `INITIALIZE` (alias `INITIALISE`) code runs once, at start-up.

## 5. `TRACE` — the full component-instance grammar

```c
[REMOVABLE] [CPU] [SPLIT [n]] COMPONENT name = comp(parameters) [WHEN condition]
  AT (x,y,z) (RELATIVE (reference | PREVIOUS[(n)]) | ABSOLUTE)
  [ROTATED (rx,ry,rz) (RELATIVE (reference | PREVIOUS[(n)]) | ABSOLUTE)]
  [GROUP group_name]
  [EXTEND C_code]
  [JUMP (reference | PREVIOUS[(n)] | MYSELF | NEXT[(n)]) (ITERATE n | WHEN condition)]
  [METADATA type name C_code]
```

Every clause other than `COMPONENT name = comp(...)` and `AT (...)` is optional; a
component may carry more than one `JUMP` clause. Order matters and must follow the layout
above. Concrete example:

```c
COMPONENT Mono1 = Monochromator_curved(zwidth=0.025, yheight=0.025, gap=0.0005,
    NH=15, NV=1, mosaich=44, mosaicv=44, r0=1, Q=1.8734)
  AT (0, 0, -LMM) RELATIVE Cradle
  ROTATED (0, A1/2, 0) RELATIVE Cradle
  GROUP IN6Monoks
  EXTEND
  %{
    if (SCATTERED) myvar = 1;
  %}
```

### Placement and references
* `AT (x,y,z) RELATIVE ref` places the component's local origin relative to a previously
  declared instance. `AT (x,y,z) RELATIVE ABSOLUTE` (or a lone `AT (x,y,z) ABSOLUTE`) is the global frame. `ROTATED` works identically, and is independent
  of `AT`'s reference — you may position `RELATIVE A` and rotate `RELATIVE B`. If `ROTATED`
  is omitted, the default is `(0,0,0)` in the *same* reference as `AT`. Rotation order:
  first `rx` around x, then `ry` around y, then `rz` around z — all in **degrees**. (Hint: Use a stack of `Arm` components to decouple rotations and perform them in wanted order.) 
* `PREVIOUS` refers to the immediately preceding instance; `PREVIOUS(n)` walks back `n`
  instances. An invalid `PREVIOUS` reference degrades to `ABSOLUTE` with a warning.
* Best practice from the manual: do all `ROTATED` work on `Arm()`-class "goniometer"
  components, and position optics relative to those arms.
* `name`, `comp`, and every parameter/`AT`/`ROTATED` expression above are as declared by a
  *component definition* — see the companion component-grammar file for how `comp` itself
  is written.

## 6. `SPLIT` — boost statistics past a lossy component

```c
SPLIT COMPONENT mono = NCrystal_sample(...) AT (...) ...
SPLIT 100 COMPONENT Sample = Single_crystal(...) AT (...) ...
```

* `SPLIT` (bare) repeats the incoming particle event **10×** (the default) from that
  component onward, each copy carrying `1/10` of the original weight. `SPLIT n` sets an
  explicit repeat count. Only useful if downstream components make further random-number
  (Monte Carlo) choices, otherwise the copies are identical and the extra statistics are
  wasted. Real efficiency scales roughly as `r^(number of SPLIT points)`, so two `SPLIT`s
  at the default `r=10` compound to ~100× more downstream events (execution time grows
  accordingly). Can be used inside a `GROUP`, but only the group's *first* component may
  carry the `SPLIT` (the parser errors out otherwise); best practice is to move `SPLIT` to
  an earlier, non-grouped component.
* If your instrument uses global (`DECLARE`) flags together with `WHEN`/`Monitor_nD`
  user variables, make sure those flags are reset *inside* the `SPLIT`'ed section on every
  loop iteration.

## 7. `GROUP` — mutually-exclusive component sets ("XOR")

```c
COMPONENT Mono1 = Monochromator_curved(...)
  AT (0,0,-LMM) RELATIVE Cradle  ROTATED (0,A1/2,0) RELATIVE Cradle
  GROUP IN6Monoks

COMPONENT Mono2 = Monochromator_curved(...)
  AT (0,0,0) RELATIVE Cradle  ROTATED (0,A2/2,0) RELATIVE Cradle
  GROUP IN6Monoks
```

* All components sharing the same `GROUP name` are tried in sequence until one of them
  calls the `SCATTER` macro internally; that one "wins" and the rest of the group is
  skipped for that particle. Grouped components must be **consecutive** in `TRACE`, and
  no non-member component may sit between them.
* `SCATTERED` (0 on group entry, incremented on `SCATTER`) is the usual way to test, in an
  `EXTEND` block, whether *this* member of the group fired.
* **Warning** (from the manual, and worth repeating): combining `WHEN` + `EXTEND` + `GROUP`
  is easy to get wrong — if every member's `WHEN` is false, *none* scatters and the particle
  is silently `ABSORB`ed. A common fix is to add an `Arm` to the group with the *opposite*
  `WHEN` condition plus an explicit `SCATTER` in its `EXTEND`, so the group always has a
  fallback member.
* Real-world example: `SNS_BASIS.instr` groups three analyzer crystals under
  `GROUP ANALYZERS`; `RTP_DIF.instr` groups reflect/transmit monitors under `GROUP mono_rt`.

## 8. `WHEN` — conditional component activation

```c
COMPONENT Sample = V_sample(...) AT (...)
  EXTEND
  %{ if (SCATTERED) flag=1; else flag=0; %}

COMPONENT MyMon = PSD_monitor(...) WHEN (flag==1) AT (0,0,0) RELATIVE Sample
```

* `WHEN condition` (any C expression, same scope as `EXTEND`) sits between the component's
  parameter list and its `AT` clause, and only gates the `TRACE`/`EXTEND` behaviour — not
  `INITIALIZE`, `SAVE`, `MCDISPLAY`, or `FINALLY`, and `mcdisplay` draws the component as if
  the condition were always true.
* `WHEN` also gates membership inside a `GROUP` — a false `WHEN` means that instance is
  skipped as if absent (see the `GROUP` warning above).
* Prefer `USERVARS` over a global `DECLARE` flag for this "set-in-one-EXTEND,
  read-in-a-later-WHEN" pattern, since it stays correct under `SPLIT`/GPU/MPI.

## 9. `EXTEND` — appending C code after a component's TRACE

```c
COMPONENT Mono2 = Monochromator_curved(...) AT (...) ROTATED (...) GROUP IN6Monoks
EXTEND
%{
  if (SCATTERED) myvar = 2;
%}
```

* Injects C code executed immediately after that one instance's own `TRACE` section runs;
  it can read/write the component's own parameters, `USERVARS`, and instrument `DECLARE`
  globals (but not instrument *parameters* directly — copy those into a `DECLARE` global
  first, in `INITIALIZE`, if needed). This is the standard, file-free way to tweak an
  existing library component's behaviour per-instance.

## 10. `JUMP` — loops and non-sequential propagation

```c
COMPONENT Jmp = Arm() AT (0,0,0.1) ABSOLUTE
JUMP Return WHEN (doJump)          // conditional jump

COMPONENT CG_2 = Guide_gravity(l=L/n, m=1, ...)
  AT (0,0,0) RELATIVE PREVIOUS  ROTATED (0,(L/n+d)/R*180/PI,0) RELATIVE PREVIOUS
  JUMP CG_2_Position ITERATE n   // repeat a loop body n times
```

* Two exclusive forms: `JUMP target WHEN condition` (jump once, conditionally) or
  `JUMP target ITERATE n` (loop the section back to `target` exactly `n` times — the
  standard way to build a long curved guide from a handful of repeated `Arm`/element
  pairs, e.g. `JUMP CG_2_Position ITERATE numel`).
* `target` may be a named instance, `PREVIOUS`/`PREVIOUS(n)`, `MYSELF` (re-enter the same
  component — used for multiple-scattering-style loops), or `NEXT`/`NEXT(n)`.
* **The jump lands directly inside the target's local coordinate frame — `AT`/`ROTATED`
  are *not* re-applied.** For anything but a `MYSELF` loop, the target should therefore be
  an `Arm`, so its position/orientation is well defined. Get the geometry of the *arm*
  right; the jump itself does no further transform.
* A component may carry more than one `JUMP` clause. `JUMP` (like `WHEN`) only affects
  `TRACE`; `mcdisplay` shows the instrument as if no jump existed (so a 500-element curved
  guide built with `JUMP ... ITERATE` renders as a single element in 3D view).
* Not recommended inside a `GROUP` (the jump counter/condition applies to the instance
  *within* its group, which is easy to reason about incorrectly). Use sparingly and
  document heavily — nested `JUMP`s are a real goto and can be hard to follow.
* Real examples: `Unittest_JUMP_ITERATE.instr`, `Unittest_JUMP_WHEN.instr`,
  `Test_Jump_Iterate.instr` in `Tests_grammar/`.

## 11. `COPY` — duplicating a component instance

```c
COMPONENT COPY(H25_1) = COPY(H25_1)
  AT (0,0,L_H25_1+gGap) RELATIVE PREVIOUS
  ROTATED (0,Rh_H25_1,0) RELATIVE PREVIOUS

COMPONENT COPY(H25_1) = COPY(H25_1)(W=2*gW)     // override one parameter
  AT (0,0,L_H25_1+gGap) RELATIVE PREVIOUS
  ROTATED (0,Rh_H25_1,0) RELATIVE PREVIOUS
```

* `COMPONENT name = COPY(instance)` duplicates a **preceding** instance's full parameter
  set (definition + setting parameters); `instance` may also be `PREVIOUS`.
* `COPY(instance)(par=val, ...)` overrides just the listed parameters — handy for
  concentric-geometry samples (`Isotropic_Sqw`/`PowderN`-style: first instance
  `concentric=1`, the copy `concentric=0`).
* `COMPONENT COPY(root) = COPY(instance)` (same name on both sides) auto-generates a
  unique instance name from `root` — convenient for long chains of elements you'll never
  need to reference again (e.g. `COMPONENT COPY(Origin) = COPY(Origin)` repeated many
  times). Because the actual name is McStas/McXtrace-generated, referring back to such an
  instance later (other than via `PREVIOUS`) is awkward — reserve this form for elements
  nobody needs to address individually.
* You must still give each copy its own `AT`/`ROTATED` — `COPY` does not clone placement.
  If a copied instance re-declares `EXTEND`/`GROUP`/`JUMP`/`WHEN`, those override (not
  merge with) the source instance's settings.
* Real examples: `ILL_H25.instr` (`COPY(H25_1)`), `Test_FZP_simple.instr`
  (`COPY(psd_monitor)` repeated with different `filename=`).

## 12. `METADATA` — attaching arbitrary side-data

```c
COMPONENT first_file = File(filename="first.txt", metadatakey="stored", keep=1) AT (0,0,0) ABSOLUTE
METADATA txt stored %{
Store text to write in first.txt
%}

METADATA "mimetype/text" origin_info %{
# Generated file
Here is some data to go into a file.
%}
```

* `METADATA type name %{ ...text... %}` attaches a named, typed block of arbitrary text to
  a component **instance** (as shown above, placed after the instance's
  positioning/`EXTEND`/`JUMP` clauses in `TRACE`) or to an **instrument**. It has zero
  effect on the simulated physics — it's purely for downstream tools (IDF fragments, JSON
  geometry blobs, CIF snippets, free-form notes) to retrieve without parsing generated C.
  `type`/`name` are bare identifiers or quoted strings (quote for MIME-style types like
  `"application/json"`).
* `METADATA` may also be attached to a component **definition** (inherited by every
  instance of that component) — see the companion component-grammar file. An
  instance-level block, as shown here, overrides a definition-level block of the same
  `name` for that one instance.
* Inspect from the command line via `mcrun --meta-list`, `--meta-defined=comp[:name]`,
  `--meta-type=comp:name`, `--meta-data=comp:name`.

## 13. `CPU` — force one instance to run on CPU

```c
CPU COMPONENT slow_bit = SomeLib(...) AT (...) ...
```

* `CPU` is a per-instance flag (see the full grammar in §5) forcing one otherwise
  GPU-capable instance to run on CPU only under `--openacc` (e.g. for debugging), without
  marking the whole component class CPU-only. The complementary, component-**definition**
  level flag is `NOACC` — described in the companion component-grammar file. Neither `CPU`
  nor `NOACC` appears anywhere in the shipped `mcstas-comps`/`mcxtrace-comps` example
  trees — they're a newer, more specialized addition (postdates the slide deck this cheat
  sheet was cross-checked against).

## 14. `%include` and instrument concatenation

```c
%include "monitor_nd-lib"          // C library, from within a %{ %} block
%include "ILL_H16.instr"           // whole instrument or component file
```

* Outside a `%{ %}` C block, `%include "file"` reads another `.instr`/`.comp` file as
  meta-language (component definitions, or — inside `TRACE` — whole instruments to
  concatenate; see below). It's resolved at translation time by `mcstas`/`mcxtrace`, not by
  the C compiler — don't confuse it with C's `#include`.
* Inside a `%{ ... %}` C code block, `%include "name"` instead pulls in a runtime C
  library from the component search path (`.h`/`.c` pair, included once no matter how many
  components ask for it) — e.g. `%include "read_table-lib"`. The runtime library is
  included by default, equivalent to an implicit `%include "mcstas-r"` (or `"mcxtrace-r"`).
* **Instrument concatenation**: `%include` inside `TRACE` splices a whole second
  instrument's `DECLARE`/`INITIALIZE`/`SAVE`/`FINALLY` blocks and parameter list into the
  including instrument, and inlines its `TRACE` component list:
  ```c
  DEFINE INSTRUMENT concatenated()
  TRACE
  %include "part1.instr"
  %include "part2.instr"
  END
  ```
  Mark components that shouldn't be duplicated when included this way (typically sources)
  as `REMOVABLE COMPONENT ... = ...` — a `REMOVABLE` component is simply skipped when the
  file is `%include`'d into another instrument (but present as normal when run standalone).
  Real examples: `templateTAS.instr` (`REMOVABLE COMPONENT Origin`, `REMOVABLE COMPONENT
  Source`), `ILL_IN5_Mantid.instr`; concatenation itself: `ILL_H16_IN5_Mantid.instr`
  (`%include "ILL_H16_Mantid.instr"` + `%include "ILL_IN5_Mantid.instr"`).

## 15. `SAVE` / `FINALLY` / `END` (instrument level)

```c
SAVE
%{ // executed whenever a (partial) save is triggered, and at simulation end %}

FINALLY
%{ // executed once, at simulation end, after SAVE %}

END
```

* Both optional; `SAVE` fires on e.g. a `USR2` signal or a `Progress_bar(flag_save=1)`
  instance, and always once more at the very end (before `FINALLY`). `FINALLY` also runs on
  early termination (`Ctrl-C`, `TERM`/`INT`). `END` is mandatory and closes the instrument.

---

## 16. Reserved keyword reference

All tokens below are reserved and case-insensitive; do not reuse them as C identifiers.
"Scope": **I** = valid in instrument definitions, **C** = valid in component definitions
(the companion component-grammar file covers the **C**-scope keywords in more depth).

| Keyword | Scope | Meaning |
|---|---|---|
| `ABSOLUTE` | I | Global coordinate frame for `AT`/`ROTATED`. |
| `AT` | I | Component position. |
| `COMPONENT` | I | Declares a component instance. |
| `COPY` | I | Duplicate a preceding instance (§11). |
| `CPU` | I | Force one instance to run on CPU only (§13). |
| `DECLARE` | I, C | C global declarations. |
| `DEFINE` | I, C | Starts an `INSTRUMENT` or `COMPONENT` definition. |
| `DEFINITION` | C | No longer supported — raises a compile error if used ([`COMPONENT grammar`](../Component/COMPONENT_Grammar.md), §3). |
| `DEPENDENCY` | I, C | Compiler/linker flags (§3). |
| `DISPLAY` | C | Alias for `MCDISPLAY`. |
| `END` | I, C | Ends the definition. |
| `EXTEND` | I, C | Appends C code after a `TRACE`, or after an `INHERIT`ed section. |
| `FINALLY` | I, C | C code run at simulation end. |
| `GROUP` | I | Exclusive ("XOR") component group (§7). |
| `%include` | I, C | Import a file — component/instrument, or (inside `%{ %}`) a C library (§14). |
| `INHERIT` | C | Derive a component (whole or per-section) from a parent. |
| `INITIALIZE` / `INITIALISE` | I, C | C code run once at start-up. |
| `ITERATE` | I | `JUMP` loop-count clause (§10). |
| `JUMP` | I | Conditional/iterative non-sequential propagation (§10). |
| `MCDISPLAY` | C | C code to draw component geometry (alias `DISPLAY`). |
| `METADATA` | I, C | Attach a named, typed text block (§12). |
| `MYSELF` | I | Self-reference for `COPY`/`JUMP`. |
| `NEXT` / `NEXT(n)` | I | Forward instance reference, for `JUMP`. |
| `NOACC` | C | Marks a component class GPU-incompatible. |
| `OUTPUT` | C | Alias for `PRIVATE PARAMETERS`; parses but no longer has any effect ([`COMPONENT grammar`](../Component/COMPONENT_Grammar.md), §3). |
| `PARAMETERS` | C | Qualifier after `SETTING`/`OUTPUT`/`PRIVATE` (and the rejected `DEFINITION`). |
| `PREVIOUS` / `PREVIOUS(n)` | I, C | Backward instance reference. |
| `PRIVATE` | C | Alias for `OUTPUT PARAMETERS`; parses but no longer has any effect ([`COMPONENT grammar`](../Component/COMPONENT_Grammar.md), §3). |
| `RELATIVE` | I | Relative coordinate frame for `AT`/`ROTATED`. |
| `REMOVABLE` | I | Skip this component when the file is `%include`'d (§14). |
| `ROTATED` | I | Component orientation. |
| `SAVE` | I, C | C code run on (partial) data save. |
| `SEARCH` | I | Extra component/instrument search directory — **TRACE-section only** (§3). |
| `SETTING` | C | Component parameters compiled as ordinary C variables. |
| `SHARE` | C | C code emitted once, shared across all instances. |
| `SHELL` | I, C | Run a shell command at parse time (§3). |
| `SPLIT` | I | Boost downstream statistics by event repetition (§6). |
| `TRACE` | I, C | The particle-propagation / component-interaction section. |
| `USERVARS` | I, C | Per-particle custom fields (§4). |
| `WHEN` | I | Conditional component activation, or `JUMP` condition (§8, §10). |

### Obsolete / rejected keywords
`STATE PARAMETERS`, `POLARISATION PARAMETERS`, and `DEFINITION PARAMETERS` (a
component-definition keyword — see the [`COMPONENT grammar`](../Component/COMPONENT_Grammar.md), §3) are all still recognised by
the grammar but **immediately raise a compile error if used** — leftovers from older
grammar versions. Separately, `OUTPUT PARAMETERS`/`PRIVATE PARAMETERS` are **not**
rejected — they still parse without error — but no longer have any effect; they're inert
rather than erroring. All four are usually a sign the file, or the documentation it was
written against, predates the current McStas/McXtrace.


---

## 17. Source-material for this reference
* `mccode/src/instrument.y`, `mccode/src/instrument.l` — the Bison/Flex grammar itself
  (authoritative; shared verbatim between McStas and McXtrace).
* `docpkg/manuals/mcstas/manual.pdf`, chapter 4, "The McStas kernel and meta-language".
* `docpkg/manuals/mcxtrace/manual.pdf`, chapter 6 (equivalent kernel/meta-language chapter)
  and section 6.1 (notational conventions / particle state fields).
* `mcstas-comps/examples/` and `mcxtrace-comps/examples/`, especially the
  `Tests_grammar/` folder (`Unittest_JUMP_ITERATE`, `Unittest_JUMP_WHEN`,
  `Test_Jump_Iterate`, `Test_GROUP`, `Unittest_SPLIT`), plus `ILL_H25.instr`,
  `SNS_BASIS.instr`, `templateTAS.instr`, `Test_FZP_simple.instr`, and `test_File.instr`.
* `01_Advanced_grammar.pptx` (uploaded slide deck) — cross-checked, gaps noted in §17.

See also: `McStas_McXtrace_COMPONENT_Grammar.md` ([`COMPONENT grammar`](../Component/COMPONENT_Grammar.md), `.comp` grammar).
