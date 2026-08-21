#!/usr/bin/env python3
"""
generate_tool_docs.py

Regenerates the option tables in the McStas/McXtrace Python-tool cheat
sheet directly from the actual argument-parser definitions in the repo, so
the docs can't silently drift out of sync with the code 

Usage:
    python3 generate_tool_docs.py --repo-root /path/to/McCode --outdir ./out

Scope - "python-tool-related" only, i.e. tools whose CLI is defined in
Python under tools/Python/...:
    mcrun (optparse, not argparse - see below),
    mcplot / mcplotdiff / mccoplot (html/matplotlib/pyqtgraph, argparse),
    mcdisplay (pyqtgraph default + cad, argparse),
    mctest, mcviewtest, mcdoc (argparse).

NOT covered (no Python parser object to introspect, so these stay as
static text in the page templates below - edit by hand if they change):
    - the C code generators (mcstas/mcxtrace/-pygen) - see codegen.md
    - mcgui - takes at most one positional arg, not worth a generator
    - mcdisplay-matplotlib/webgl/webgl-classic - bash wrappers that parse
      their own flags directly in shell
    - everything under tools/matlab (mcplot-matlab, mcdisplay-matlab)
    - README.md's PROSE (title, naming-convention paragraph) is
      hand-composed text baked into build_readme_md() below, since
      there's nothing to extract it from - but its two link-index
      tables ARE regenerated, specifically to keep their link-label
      style (see build_readme_md()'s own docstring) consistent and
      easy to update in one place.

Design notes:
  - This script parses each target file's AST rather than importing it.
    These tools have real import-time side effects (argument parsing
    against live sys.argv, GUI toolkit imports, etc.), so importing them
    to introspect their parser objects would require their full runtime
    dependency stack (PyQt, matplotlib, a display server for some of
    them...) just to regenerate documentation. Static analysis avoids all
    of that and cannot accidentally execute anything from the target repo.
  - Matching is by METHOD NAME (add_argument for argparse, add_option for
    optparse), not by a guessed parser variable name: mcrun.py in
    particular builds its optparse OptionGroups as `opt = OptionGroup(...)`
    then rebinds `add = opt.add_option` and calls the bare alias `add(...)`
    throughout, rather than `opt.add_option(...)` directly. A first pass
    finds such aliases (`X = Y.method_name`); a second pass matches both
    the direct attribute-call form and calls to a discovered alias.
  - mcrun.py wraps a few McStas-only options (e.g. `-g`/`--gravitation`)
    in `if mccode_config.configuration["MCCODE"] == 'mcstas':`. The
    extractor tracks this nesting while walking (rather than using a flat
    ast.walk(), which would find the calls but lose the surrounding
    conditional) and tags those entries so the rendered table can note
    "(McStas only)"/"(McXtrace only)" instead of silently presenting them
    as available on both flavours.
  - A few help= strings are built as
    'some text %s' % mccode_config.configuration["MCRUN"] rather than
    plain literals. Since we don't execute the target file, these are
    resolved against a small local stand-in for mccode_config (the
    standard McStas-flavour values).
  - A few real options are not expressed as add_argument()/add_option()
    calls at all (mcrun's "param=val" / "param=min,max" positional-
    parameter syntax is hand-parsed from optparse's leftover `args`, not a
    parser option) - these are re-inserted as fixed extra rows rather than
    invented out of nothing.
"""
import argparse
import ast
import os
import sys


# ---------------------------------------------------------------------------
# Static stand-in for mccode_config.configuration, for resolving help=
# strings that build themselves from it (see module docstring). Values are
# the standard McStas flavour.
# ---------------------------------------------------------------------------
class _FakeConfigModule:
    configuration = {
        "MCCODE": "mcstas",
        "MCRUN": "mcrun",
        "MCPLOT": "mcplot-pyqtgraph",
        "MCDISPLAY": "mcdisplay",
        "MCGUI": "mcgui",
        "MCDOC": "mcdoc",
        "PARTICLE": "neutron",
        "MCCODE_VERSION": "<version>",
        "NDBUFFERSIZE": "1e7",
    }
    platform = {
        "EXESUFFIX": "out",
    }


# A small, deliberately-limited allowlist of builtins - just enough to
# resolve the plain string-building patterns actually used in help=
# expressions (e.g. `str(MINIMIZE_METHODS)`), not a general-purpose eval
# sandbox. No I/O, no imports, no attribute access beyond what's already
# on _FakeConfigModule.
_SAFE_BUILTINS = {"str": str, "repr": repr, "len": len, "int": int, "float": float}
SAFE_EVAL_GLOBALS = {"mccode_config": _FakeConfigModule, "__builtins__": _SAFE_BUILTINS}

FLAVOR_ANNOTATIONS = {
    "mcstas": "*(McStas only)*",
    "mcxtrace": "*(McXtrace only)*",
}


# ---------------------------------------------------------------------------
# AST-based extraction
# ---------------------------------------------------------------------------

def _module_level_constants(tree):
    """ Extracts simple module-scope `NAME = <literal>` assignments (e.g.
        mcrun.py's `MINIMIZE_METHODS = ['powell', 'nelder-mead', ...]`), so
        help= expressions that reference such a constant directly (not
        through mccode_config) can still be resolved instead of falling
        back to raw source text. Deliberately shallow - only top-level
        Assign nodes with a literal-evaluable value, not anything that
        would require executing real code. """
    consts = {}
    for node in tree.body:
        if isinstance(node, ast.Assign) and len(node.targets) == 1 and isinstance(node.targets[0], ast.Name):
            try:
                consts[node.targets[0].id] = ast.literal_eval(node.value)
            except Exception:
                continue
    return consts


def _resolve_expr(node, extra_globals=None):
    """ Best-effort resolution of a keyword argument's AST value to a plain
        value. Literal strings/numbers resolve directly; the
        mccode_config.configuration[...]-based expressions actually used
        in this codebase evaluate against _FakeConfigModule (optionally
        extended with extra_globals - simple module-level constants
        discovered in the same source file, see _module_level_constants);
        anything else falls back to the raw source text of the expression,
        clearly flagged, so the generator degrades gracefully instead of
        silently guessing or crashing on a pattern it doesn't recognise
        (e.g. a value computed from other local variables at runtime,
        which is genuinely not statically resolvable). """
    if node is None:
        return ""
    try:
        return ast.literal_eval(node)
    except Exception:
        pass
    try:
        expr = ast.Expression(node)
        ast.fix_missing_locations(expr)
        code = compile(expr, "<help-string>", "eval")
        eval_globals = dict(SAFE_EVAL_GLOBALS)
        if extra_globals:
            eval_globals.update(extra_globals)
        return eval(code, eval_globals)
    except Exception:
        try:
            return "`%s` *(dynamic expression - verify by hand)*" % ast.unparse(node)
        except Exception:
            return "*(could not resolve help text)*"


def _kwarg(call_node, name):
    for kw in call_node.keywords:
        if kw.arg == name:
            return kw.value
    return None


def _find_method_aliases(tree, method_names):
    """ Finds simple aliases of the form `X = Y.method_name` (mcrun.py's
        `add = opt.add_option` pattern) so calls to the bare name `X(...)`
        can be recognised as equivalent to `Y.method_name(...)`. """
    aliases = set()
    for node in ast.walk(tree):
        if isinstance(node, ast.Assign) and isinstance(node.value, ast.Attribute):
            if node.value.attr in method_names:
                for target in node.targets:
                    if isinstance(target, ast.Name):
                        aliases.add(target.id)
    return aliases


def _is_flavor_check(test_node, flavor):
    """ Detects `mccode_config.configuration["MCCODE"] == 'mcstas'` (or the
        McXtrace/'mcxtrace' equivalent), in either comparison order. """
    if not isinstance(test_node, ast.Compare) or len(test_node.ops) != 1:
        return False
    if not isinstance(test_node.ops[0], ast.Eq):
        return False
    for side in (test_node.left, test_node.comparators[0]):
        try:
            if ast.literal_eval(side) == flavor:
                return True
        except Exception:
            continue
    return False


def _build_entry(call_node, flavor_only, extra_globals=None):
    flags = []
    for a in call_node.args:
        try:
            flags.append(ast.literal_eval(a))
        except Exception:
            pass
    if not flags:
        return None

    help_text = _resolve_expr(_kwarg(call_node, "help"), extra_globals=extra_globals)

    action_node = _kwarg(call_node, "action")
    action = None
    if action_node is not None:
        try:
            action = ast.literal_eval(action_node)
        except Exception:
            action = None

    nargs_present = _kwarg(call_node, "nargs") is not None

    metavar = None
    metavar_node = _kwarg(call_node, "metavar")
    if metavar_node is not None:
        try:
            metavar = ast.literal_eval(metavar_node)
        except Exception:
            pass

    is_positional = not flags[0].startswith("-")
    takes_value = action not in ("store_true", "store_false", "help", "version") and (
        is_positional or nargs_present or action in (None, "store", "append", "callback")
    )

    longest = max(flags, key=len)
    dest_guess = longest.lstrip("-").replace("-", "_").upper()

    return {
        "flags": flags,
        "is_positional": is_positional,
        "help": help_text,
        "takes_value": takes_value,
        "metavar": metavar,
        "dest_guess": dest_guess,
        "flavor_only": flavor_only,
    }


def _walk_with_flavor_context(node, method_names, aliases, flavor, results, extra_globals=None):
    """ Recursive descent (rather than a flat ast.walk()) so that calls
        nested inside `if mccode_config.configuration["MCCODE"] == 'mcstas':`
        can be tagged with which flavour they're actually available on. """
    if isinstance(node, ast.If):
        this_flavor = flavor
        if _is_flavor_check(node.test, "mcstas"):
            this_flavor = "mcstas"
        elif _is_flavor_check(node.test, "mcxtrace"):
            this_flavor = "mcxtrace"
        for child in node.body:
            _walk_with_flavor_context(child, method_names, aliases, this_flavor, results, extra_globals)
        for child in node.orelse:
            _walk_with_flavor_context(child, method_names, aliases, flavor, results, extra_globals)
        return

    if isinstance(node, ast.Call):
        func = node.func
        is_match = (isinstance(func, ast.Attribute) and func.attr in method_names) or \
                   (isinstance(func, ast.Name) and func.id in aliases)
        if is_match:
            entry = _build_entry(node, flavor, extra_globals=extra_globals)
            if entry:
                results.append(entry)

    for child in ast.iter_child_nodes(node):
        _walk_with_flavor_context(child, method_names, aliases, flavor, results, extra_globals)


def extract_add_arguments(filepath, method_names=("add_argument",), imported_globals=None):
    """ Statically scans a Python source file for calls to any of
        method_names (e.g. 'add_argument' for argparse, 'add_option' for
        optparse), on any object, plus calls to any local alias of that
        method (see _find_method_aliases). Returns a list of dicts, in
        source order:
            flags:         option strings, e.g. ['-n', '--ncount'], or a
                           single positional name, e.g. ['INSTR']
            is_positional: True if flags[0] doesn't start with '-'
            help:          resolved help text (see _resolve_expr)
            takes_value:   whether this option consumes a value
            metavar:       explicit metavar=, if given
            dest_guess:    fallback value-placeholder derived from the
                           longest flag, used when no metavar is set
            flavor_only:   'mcstas', 'mcxtrace', or None - set when the
                           call is nested inside an `if .../mcxtrace)/=='
                           check for that flavour (see _is_flavor_check)

        imported_globals: optional extra {name: value} dict merged in
        alongside this file's own module-level constants, for help=
        expressions that reference a constant defined in a DIFFERENT,
        imported module (e.g. mccoplot.py's help text builds itself from
        mccodelib.mcplotdiffloader.DEFAULT_PALETTE, not a local constant -
        callers that know to expect this can pre-resolve that file's
        constants with this same function and pass them through here). """
    source = open(filepath, encoding="utf-8").read()
    tree = ast.parse(source, filename=filepath)
    aliases = _find_method_aliases(tree, method_names)
    extra_globals = _module_level_constants(tree)
    if imported_globals:
        # local names win on clash, matching normal Python name resolution
        # (a local reassignment would shadow an imported one)
        extra_globals = {**imported_globals, **extra_globals}
    results = []
    _walk_with_flavor_context(tree, method_names, aliases, None, results, extra_globals)
    return results


def _extract_module_constants_only(filepath):
    """ Like extract_add_arguments()'s internal constant-scan, but exposed
        standalone for pulling constants out of a file that isn't itself
        an argparse/optparse target (e.g. mccodelib/mcplotdiffloader.py,
        which several mcplot-family tools import DEFAULT_PALETTE from). """
    source = open(filepath, encoding="utf-8").read()
    tree = ast.parse(source, filename=filepath)
    return _module_level_constants(tree)


def format_option_cell(entry):
    if entry["is_positional"]:
        return "`%s`" % entry["flags"][0]
    value = entry["metavar"] or entry["dest_guess"]
    parts = []
    for f in entry["flags"]:
        parts.append("`%s %s`" % (f, value) if entry["takes_value"] else "`%s`" % f)
    return ", ".join(parts)


def format_description(entry):
    desc = str(entry["help"]).replace("\n", " ").strip()
    if entry.get("flavor_only"):
        note = FLAVOR_ANNOTATIONS.get(entry["flavor_only"], "*(%s only)*" % entry["flavor_only"])
        desc = "%s %s" % (note, desc)
    return desc


def render_table(entries, extra_rows_first=None, extra_rows_last=None):
    """ extra_rows_first/last: list of (option_cell_markdown, description)
        tuples for real options that aren't expressed as parser-method
        calls at all (see module docstring) - inserted verbatim. """
    lines = ["| Option | Description |", "|---|---|"]
    for cell, desc in (extra_rows_first or []):
        lines.append("| %s | %s |" % (cell, desc))
    for e in entries:
        lines.append("| %s | %s |" % (format_option_cell(e), format_description(e)))
    for cell, desc in (extra_rows_last or []):
        lines.append("| %s | %s |" % (cell, desc))
    return "\n".join(lines)


def filter_by_flags(entries, wanted_flags):
    """ Keep only entries where at least one flag is in wanted_flags -
        used to split mcrun's option definitions into the same 3 topic
        tables the hand-written docs used. """
    wanted = set(wanted_flags)
    return [e for e in entries if wanted & set(e["flags"])]


# ---------------------------------------------------------------------------
# mcrun's 3-way topic split (mcrun.py's options aren't grouped this way in
# the source - "McRun options" and "Instrument options" are the only two
# native groups, and don't line up with the docs' topic split - so this
# mapping is maintained here by hand; update it if new options need
# re-categorising)
# ---------------------------------------------------------------------------
MCRUN_GENERAL_FLAGS = [
    "-c", "--force-compile", "--cogen", "-C", "--c-lint", "-I",
    "--D1", "--D2", "--D3", "--no-cflags", "--no-main", "--embed",
    "--verbose", "--showcfg", "--write-user-config", "--edit-user-config",
    "--override-config",
]
MCRUN_SCAN_FLAGS = [
    "-p", "--param", "-N", "--numpoints", "-L", "--list", "-M", "--multi",
    "--seeds", "--scan_split", "--optimize", "--optimize-monitor",
    "--optimize-eval", "--optimize-minimize", "--optimize-method",
    "--optimize-maxiter", "--optimize-tol", "--optimise-file",
]
# everything else in mcrun.py's parser falls into "simulation & instrument
# options" by elimination (see build_mcrun_md() below)


# ---------------------------------------------------------------------------
# Page builders
# ---------------------------------------------------------------------------

def build_readme_md(repo_root):
    """ README.md's two tables are hand-composed link indexes, not
        extracted from argparse (there's no per-option content here to
        drift out of sync) - but the LINK LABEL STYLE is kept here as the
        single source of truth so it can't drift from the per-page anchor
        text either. Style, established by hand-editing an earlier
        generated copy:
          - every tool name mentioned anywhere (including inside a
            comma-separated "Covers" list) spells out both `mcXXX` and
            `mxXXX` forms, not just the page title
          - for a `{variant,variant,...}`-set entry, both forms are shown
            stacked with a literal `<br>` INSIDE the same link, e.g.
            [`mcplot-{html,...}` <br> `mxplot-{html,...}`](...)
          - for two single-name tools mapping to the same "job" but
            different pages (the matlab row), two separate links are
            stacked with `<br>` BETWEEN them instead """
    return """# McStas / McXtrace User Tools Cheat Sheet

Source: `McCode/tools/Python` (plus `McCode/tools/matlab` for the legacy Matlab/Octave/iFit variants)

**Naming convention:** almost everything named `mc<something>` on the McStas
side has a direct `mx<something>` counterpart on the McXtrace side, built
from the *same* source, with the neutron/photon ("particle") wording
and a few McStas-only options (e.g. `--gravity`, time-of-flight mode)
adjusted accordingly. Where a tool has several rendering **variants**
(html / matplotlib / pyqtgraph / webgl / ...), the variant is appended after
a dash, e.g. `mcplot-html` / `mxplot-html`.

## Tool-class pages

| Page | Covers |
|---|---|
| [Code generators](codegen.md) | `mcstas`/`mcxtrace`, `mcstas-pygen`/`mcxtrace-pygen`, `mcstas-jupylab`/`mcxtrace-jupylab` |
| [mcgui](mcgui.md) | `mcgui` / `mxgui` |
| [mcrun](mcrun.md) | `mcrun` / `mxrun` |
| [mcplot family](mcplot.md) | `mcplot` / `mxplot`, `mcplotdiff` / `mxplotdiff`, `mccoplot` / `mxcoplot` — html / matplotlib / pyqtgraph / matlab variants |
| [mcdisplay family](mcdisplay.md) | `mcdisplay` / `mxdisplay` — pyqtgraph (default) / matplotlib / webgl / webgl-classic / cad / matlab variants |
| [mctest family](mctest.md) | `mctest` / `mxtest`, `mcviewtest` / `mxviewtest` |
| [mcdoc](mcdoc.md) | `mcdoc` / `mxdoc` |

## Quick reference: which tool for which job?

| I want to... | Use |
|---|---|
| Compile an `.instr` file to C directly (rarely needed by hand) | [`mcstas` / `mcxtrace`](codegen.md#mcstas-mcxtrace) |
| Convert an `.instr` model to Python for use with McStasScript | [`mcstas-pygen` / `mcxtrace-pygen`](codegen.md#mcstas-pygen-mcxtrace-pygen-convert-an-instrument-to-python) |
| Explore an instrument interactively in Jupyter via McStasScript | [`mcstas-jupylab` / `mcxtrace-jupylab`](codegen.md#mcstas-jupylab-mcxtrace-jupylab-explore-an-instrument-in-jupyter) |
| Edit/compile/run instruments interactively | [`mcgui` / `mxgui`](mcgui.md) |
| Run a simulation from the command line, or a parameter scan/optimisation | [`mcrun` / `mxrun`](mcrun.md) |
| Plot one simulation's results | [`mcplot-{html,matplotlib,pyqtgraph}` <br> `mxplot-{html,matplotlib,pyqtgraph}`](mcplot.md#mcplot-mxplot) |
| Compare exactly two simulations (`a - b`) | [`mcplotdiff-{html,matplotlib,pyqtgraph}` <br> `mxplotdiff-{html,matplotlib,pyqtgraph}`](mcplot.md#mcplotdiff-mxplotdiff) |
| Overlay 2+ simulations for direct comparison | [`mccoplot-{html,matplotlib,pyqtgraph}` <br> `mxcoplot-{html,matplotlib,pyqtgraph}`](mcplot.md#mccoplot-mxcoplot) |
| Visualise instrument geometry + particle trajectories | [`mcdisplay-{pyqtgraph,matplotlib,webgl,webgl-classic}` <br> `mxdisplay-{pyqtgraph,matplotlib,webgl,webgl-classic}`](mcdisplay.md) |
| Export instrument geometry to a CAD file | [`mcdisplay-cad` <br> `mxdisplay-cad`](mcdisplay.md#mcdisplay-cad-mxdisplay-cad-bonus-export-instrument-geometry-to-cad) |
| Plot/display results in Matlab, Octave, or iFit instead | [`mcplot-matlab` / `mxplot-matlab`](mcplot.md#mcplot-matlab-mxplot-matlab-legacy-matlaboctaveifit-variant) <br> [`mcdisplay-matlab` / `mxdisplay-matlab`](mcdisplay.md#mcdisplay-matlab-mxdisplay-matlab-legacy-matlaboctaveifit-variant) |
| Test/benchmark the whole instrument library against a McCode installation | [`mctest` / `mxtest`](mctest.md#mctest-mxtest) |
| Build a browsable pass/fail + diff report from `mctest` results | [`mcviewtest` / `mxviewtest`](mctest.md#mcviewtest-mxviewtest) |
| Generate/browse instrument & component documentation | [`mcdoc` / `mxdoc`](mcdoc.md) |
"""


def build_mcrun_md(repo_root):
    path = os.path.join(repo_root, "tools", "Python", "mcrun", "mcrun.py")
    # mcrun.py uses optparse (add_option), not argparse (add_argument) -
    # see module docstring.
    entries = extract_add_arguments(path, method_names=("add_option",))
    if not entries:
        raise RuntimeError(
            "found zero add_option()/add_argument() calls in %s - has its "
            "parser library or calling convention changed again? See this "
            "script's module docstring." % path)

    general = filter_by_flags(entries, MCRUN_GENERAL_FLAGS)
    scan = filter_by_flags(entries, MCRUN_SCAN_FLAGS)
    used_flags = set(MCRUN_GENERAL_FLAGS) | set(MCRUN_SCAN_FLAGS)
    simulation = [e for e in entries if not (used_flags & set(e["flags"]))]

    scan_extra_first = [
        ("`param=val`, `param=min,max`", "fixed parameter, or scan interval (comma-separated) "
                                          "*(hand-parsed from optparse's leftover positional args, not a real option)*"),
    ]

    return """[&larr; back to overview](README.md)

# mcrun / mxrun

Options are identical between McStas and McXtrace (only the particle name
in help text, and the McStas-only `-g`/`--gravitation` flag, differ).

*(This page is auto-generated by `generate_tool_docs.py` from `tools/Python/mcrun/mcrun.py` - do not hand-edit the tables below. Note: mcrun.py uses the older `optparse` module, not `argparse`.)*

## mcrun / mxrun \u2014 general & compile options

%s

## mcrun / mxrun \u2014 parameters, scanning & optimisation

%s

## mcrun / mxrun \u2014 simulation & instrument options

%s

---
[&larr; back to overview](README.md)
""" % (
        render_table(general),
        render_table(scan, extra_rows_first=scan_extra_first),
        render_table(simulation),
    )


def build_mcplot_md(repo_root):
    base = os.path.join(repo_root, "tools", "Python", "mcplot")
    variants = ["html", "matplotlib", "pyqtgraph"]
    tools = ["mcplot", "mcplotdiff", "mccoplot"]

    # mccoplot.py's --colours help text builds itself from
    # mccodelib.mcplotdiffloader.DEFAULT_PALETTE, a constant in a different,
    # imported file - resolve it there and pass it through, rather than
    # leaving it as an unresolved dynamic expression (see
    # extract_add_arguments()'s imported_globals parameter).
    diffloader_path = os.path.join(repo_root, "tools", "Python", "mccodelib", "mcplotdiffloader.py")
    diffloader_consts = _extract_module_constants_only(diffloader_path) if os.path.exists(diffloader_path) else {}
    # Exposed two ways: as bare names (mcplot-html imports DEFAULT_PALETTE
    # directly), and as attributes of a stand-in "diffloader" object
    # (mcplot-matplotlib/pyqtgraph instead do
    # `from mccodelib import mcplotdiffloader as diffloader` and reference
    # `diffloader.DEFAULT_PALETTE`).
    imported_globals = dict(diffloader_consts)
    imported_globals["diffloader"] = argparse.Namespace(**diffloader_consts)

    tables = {}
    for tool in tools:
        for variant in variants:
            toolpath = os.path.join(base, variant, tool + ".py")
            tables[(tool, variant)] = render_table(extract_add_arguments(toolpath, imported_globals=imported_globals))

    return """[&larr; back to overview](README.md)

# mcplot / mxplot family

Three related tool groups live under `tools/Python/mcplot`, sharing a
loader/matching layer:

- **mcplot / mxplot** \u2014 plot one simulation's monitors
- **mcplotdiff / mxplotdiff** \u2014 plot the *difference* `a - b` between two simulations (2 datasets only)
- **mccoplot / mxcoplot** \u2014 *overlay* 2 or more simulations' monitors on the same axes

Each has three rendering variants: **html**, **matplotlib**, **pyqtgraph**. Plain `mcplot` additionally has a fourth, legacy **matlab** variant (see bottom of this page) - not auto-generated, since it's a bash/`.m` tool with no Python argparse to introspect.

*(The html/matplotlib/pyqtgraph tables below are auto-generated by `generate_tool_docs.py` from the corresponding `tools/Python/mcplot/{variant}/{mcplot,mcplotdiff,mccoplot}.py` - do not hand-edit them. The matlab section at the bottom is hand-maintained.)*

## mcplot / mxplot

### mcplot-html / mxplot-html

%s

### mcplot-matplotlib / mxplot-matplotlib

%s

### mcplot-pyqtgraph / mxplot-pyqtgraph

%s

## mcplotdiff / mxplotdiff

Compares exactly two datasets: `diff = a - b`.

### mcplotdiff-html / mxplotdiff-html

%s

### mcplotdiff-matplotlib / mxplotdiff-matplotlib

%s

### mcplotdiff-pyqtgraph / mxplotdiff-pyqtgraph

%s

*(2D diff monitors use a diverging blue/white/red colour map; press `c` to cycle colour maps as usual.)*

## mccoplot / mxcoplot

Overlays **2 or more** datasets (unlike mcplotdiff, not limited to two).

### mccoplot-html / mxcoplot-html

%s

### mccoplot-matplotlib / mxcoplot-matplotlib

%s

### mccoplot-pyqtgraph / mxcoplot-pyqtgraph

%s

*(Legends always show compact letters `A`, `B`, `C`, ...; the full dataset identity is shown separately in the title/header.)*

## mcplot-matlab / mxplot-matlab *(legacy Matlab/Octave/iFit variant, hand-maintained)*

A second, older implementation of plain `mcplot` (single-simulation plotting only \u2014 no `mcplotdiff`/`mccoplot` equivalent exists for this variant), living under `tools/matlab` as a bash wrapper around a `.m` script rather than Python. Runs under **Matlab**, **Octave**, or **iFit** (a Matlab-compiler-based standalone runtime, see <https://ifit.mccode.org>) \u2014 the wrapper auto-detects whichever is available, preferring Matlab, then Octave, then iFit, and falls back to the plain Python `mcplot` if none of the three are found.

| Option | Description |
|---|---|
| `FILE`\\|`DIR` | monitor file or simulation directory to plot (default: current directory) |
| `-m` | explicitly request Matlab (skip auto-detection) |
| `-o` | explicitly request Octave |
| `-i` | explicitly request iFit |
| `-h` | show wrapper usage and exit |
| `-png`, `-jpg`, `-fig`, `-eps`, `-pdf` | *(parsed by `mcplot.m` itself, not the wrapper)* export each plotted monitor to the given format instead of just displaying it |

*(`-m`/`-o`/`-i`/`-h` are wrapper-level flags and must come first; the export-format flags are forwarded as-is to the underlying `.m` script.)*

---
[&larr; back to overview](README.md)
""" % (
        tables[("mcplot", "html")], tables[("mcplot", "matplotlib")], tables[("mcplot", "pyqtgraph")],
        tables[("mcplotdiff", "html")], tables[("mcplotdiff", "matplotlib")], tables[("mcplotdiff", "pyqtgraph")],
        tables[("mccoplot", "html")], tables[("mccoplot", "matplotlib")], tables[("mccoplot", "pyqtgraph")],
    )


def _rename_positional(entries, old_name, new_display):
    """ Overrides the displayed name for a specific positional argument -
        used for mcdisplayutils.make_common_parser()'s `options` positional
        (nargs='*'), which is genuinely how `name=value ...` simulation
        parameters are captured, but reads as meaningless out of context
        without knowing that. The underlying data (help text etc.) is
        real and unchanged; only the rendered flags/name are overridden. """
    for e in entries:
        if e["is_positional"] and e["flags"] == [old_name]:
            e["flags"] = [new_display]


def build_mcdisplay_md(repo_root):
    common_path = os.path.join(repo_root, "tools", "Python", "mccodelib", "mcdisplayutils.py")
    pyqtgraph_path = os.path.join(repo_root, "tools", "Python", "mcdisplay", "pyqtgraph", "mcdisplay.py")
    cad_path = os.path.join(repo_root, "tools", "Python", "mcdisplay", "cad", "mcdisplay.py")

    # mcdisplay-pyqtgraph's flags come from a shared "common parser" builder
    # in mccodelib.mcdisplayutils plus variant-specific additions in its own
    # mcdisplay.py - concatenate both, common options first, matching the
    # order they're actually added at runtime.
    common_entries = extract_add_arguments(common_path) if os.path.exists(common_path) else []
    _rename_positional(common_entries, "options", "name=value ...")
    pyqtgraph_entries = common_entries + extract_add_arguments(pyqtgraph_path)

    # mcdisplay-cad does NOT use the shared common parser above - it builds
    # its own small ArgumentParser and reads `instr`/`name=value ...` from
    # parser.parse_known_args()'s leftover "unknown" list rather than a
    # real add_argument() call, so they're genuinely invisible to static
    # analysis. Verified by hand against the actual source; re-verify this
    # note if mcdisplay/cad/mcdisplay.py's argument handling changes.
    cad_extra_first = [
        ("`INSTR`", "instrument file *(read from `parse_known_args()`'s leftover args, not a real add_argument() option)*"),
        ("`name=value ...`", "simulation parameters *(same leftover-args mechanism)*"),
    ]
    cad_entries = extract_add_arguments(cad_path)

    return """[&larr; back to overview](README.md)

# mcdisplay family

All variants wrap `mcrun --trace` under the hood; `INSTR` plus any
`name=value` instrument parameters are forwarded straight to it. The
**default** is `mcdisplay`/`mxdisplay` \u2192 pyqtgraph.

*(The pyqtgraph and cad tables below are auto-generated by `generate_tool_docs.py` from their actual argparse definitions - do not hand-edit them. matplotlib/webgl/webgl-classic/matlab are bash wrappers with no Python argparse to introspect, and stay hand-maintained.)*

## mcdisplay / mxdisplay *(default = pyqtgraph)*

%s

**Interactive keys:** `q` quit &middot; `p` save png &middot; `s` save svg (not on Windows) &middot; `space`/`F5` next ray &middot; click a subplot to zoom, right-click to exit zoom &middot; `h`/`F1` component list.

## mcdisplay-matplotlib / mxdisplay-matplotlib *(hand-maintained - bash wrapper, no argparse)*

Thin bash wrapper piping `mcrun --trace` output into a matplotlib 3D viewer \u2014 options are parsed by the wrapper script itself, not Python argparse.

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `-n N`, `--ncount N`, `--ncount=N` | number of particles to trace (capped at 1e2 by the wrapper) |
| `--trace={1,2}` | classic (1) or new (2, default) visualisation mode |
| `--backend=NAME` | matplotlib backend; `pdf`/`pgf`/`ps`/`svg` save a hardcopy instead of showing a window |
| `--help` | show wrapper usage and exit |

## mcdisplay-webgl / mxdisplay-webgl *(hand-maintained - NodeJS/vite-based, no Python argparse)*

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `--default` | automatically use instrument default parameter values |
| `-n`, `--ncount N` | number of particles to trace (default: 300) |
| `-t`, `--trace N` | visualisation mode (default: 2) |
| `-d`, `--dirname DIR` | override output directory name |
| `--inspect COMP` | show only rays reaching component `COMP` |
| `--first COMP` / `--last COMP` | zoom range start/end component |
| `--invcanvas` | invert canvas background |
| `--nobrowse` | do not open a web browser |
| `--timeout SEC` | shutdown time of the npm/vite dev server (default: 300) |

*(First run performs a one-time `npm`/`vite` module install, which needs internet access and can take minutes.)*

## mcdisplay-webgl-classic / mxdisplay-webgl-classic *(hand-maintained)*

| Option | Description |
|---|---|
| `INSTR` | instrument file |
| `name=value ...` | simulation parameters |
| `--default` | automatically use instrument default parameter values |
| `-n`, `--ncount N` | number of particles to trace (default: 300) |
| `-t`, `--trace N` | visualisation mode (default: 1) |
| `-d`, `--dirname DIR` | override output directory name |
| `--inspect COMP` | show only rays reaching component `COMP` |
| `--first COMP` / `--last COMP` | zoom range start/end component |
| `--invcanvas` | invert canvas background |
| `--nobrowse` | do not open a web browser |

## mcdisplay-cad / mxdisplay-cad *(bonus: export instrument geometry to CAD)*

Requires the `cadquery` Python package.

%s

## mcdisplay-matlab / mxdisplay-matlab *(legacy Matlab/Octave/iFit variant, hand-maintained)*

A second, older implementation living under `tools/matlab` as a bash wrapper around a `.m` script rather than Python. Runs under **Matlab** or **Octave** \u2014 the wrapper auto-detects whichever is available, preferring Matlab, then Octave, and falls back to the plain Python `mcdisplay` if neither is found.

| Option | Description |
|---|---|
| `INSTR` | instrument file (`.instr` or compiled binary) |
| `name=value ...` | simulation parameters, forwarded to `mcrun` |
| `-m` | explicitly request Matlab |
| `-o` | explicitly request Octave |
| `-h` | show wrapper usage and exit |
| `-n N`, `--ncount N` | number of particles to simulate |
| `--inspect=COMP` | only plot components matching `COMP` \u2014 a partial name (e.g. `Monitor`), or an interval (`Monok:Sample`, `2:10`, `2:end`) |
| `-png`, `-jpg`, `-fig`, `-eps`, `-pdf`, `-tif` | *(parsed by `mcdisplay.m` itself, not the wrapper)* export the 3D view to the given format |

*(`-m`/`-o`/`-h` are wrapper-level flags and must come first; the export-format flags and `--inspect` are forwarded as-is to the underlying `.m` script.)*

---
[&larr; back to overview](README.md)
""" % (
        render_table(pyqtgraph_entries),
        render_table(cad_entries, extra_rows_first=cad_extra_first),
    )


def build_mctest_md(repo_root):
    mctest_path = os.path.join(repo_root, "tools", "Python", "mctest", "mctest.py")
    mcviewtest_path = os.path.join(repo_root, "tools", "Python", "mctest", "mcviewtest.py")

    return """[&larr; back to overview](README.md)

# mctest family

Installation testing / benchmarking tools, under `tools/Python/mctest`.

*(This page is auto-generated by `generate_tool_docs.py` from `mctest.py`/`mcviewtest.py` - do not hand-edit the tables below.)*

## mctest / mxtest

Runs every `%%Example` test embedded in the instrument library, compiling,
displaying (single-particle), and running each one, then compares the
result against the target value recorded in the instrument header.

%s

## mcviewtest / mxviewtest

Builds a single browsable HTML comparison report from one or more `mctest`
result sets (as written by `mctest --testdir`), diffing/co-plotting each
row against a chosen reference column via `mcplotdiff-html`/`mccoplot-html`.

%s

---
[&larr; back to overview](README.md)
""" % (
        render_table(extract_add_arguments(mctest_path)),
        render_table(extract_add_arguments(mcviewtest_path)),
    )


def build_mcdoc_md(repo_root):
    path = os.path.join(repo_root, "tools", "Python", "mcdoc", "mcdoc.py")

    return """[&larr; back to overview](README.md)

# mcdoc / mxdoc

Generates browsable documentation for installed and local instrument/component files, under `tools/Python/mcdoc`.

*(This page is auto-generated by `generate_tool_docs.py` from `mcdoc.py` - do not hand-edit the table below.)*

%s

*(No `searchterm`/`--install`/`--manual`/`--comps`/`--web` at all &rarr; browse the existing installed docs directly.)*

---
[&larr; back to overview](README.md)
""" % render_table(extract_add_arguments(path))


PAGE_BUILDERS = {
    "README.md": build_readme_md,
    "mcrun.md": build_mcrun_md,
    "mcplot.md": build_mcplot_md,
    "mcdisplay.md": build_mcdisplay_md,
    "mctest.md": build_mctest_md,
    "mcdoc.md": build_mcdoc_md,
}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--repo-root", required=True, help="path to the McCode repository root (contains tools/Python)")
    ap.add_argument("--outdir", required=True, help="directory to write the generated .md pages into")
    ap.add_argument("--only", nargs="*", choices=list(PAGE_BUILDERS.keys()),
                     help="regenerate only these pages (default: all)")
    args = ap.parse_args()

    repo_root = os.path.abspath(args.repo_root)
    if not os.path.isdir(os.path.join(repo_root, "tools", "Python")):
        print("error: %s does not look like a McCode repo root (no tools/Python found)" % repo_root, file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.outdir, exist_ok=True)

    targets = args.only or list(PAGE_BUILDERS.keys())
    had_failure = False
    for name in targets:
        builder = PAGE_BUILDERS[name]
        try:
            content = builder(repo_root)
        except Exception as e:
            print("FAILED to generate %s: %s" % (name, e), file=sys.stderr)
            had_failure = True
            continue
        outpath = os.path.join(args.outdir, name)
        open(outpath, "w", encoding="utf-8").write(content)
        print("wrote %s" % outpath)

    print("\nNote: codegen.md and mcgui.md are still not regenerated by this "
          "script (no Python argparse/optparse object to introspect - "
          "mostly hand-written prose/links) - edit those by hand as needed. "
          "README.md IS now regenerated (its link-label style is the single "
          "source of truth in build_readme_md() - edit that function, not "
          "the generated file, if the style needs to change again).")

    if had_failure:
        sys.exit(1)


if __name__ == "__main__":
    main()
