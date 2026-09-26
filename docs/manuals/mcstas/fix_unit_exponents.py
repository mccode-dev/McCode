#!/usr/bin/env python3
r"""
fix_unit_exponents.py

One-off maintenance script: normalizes implicit unit exponents in McStas
component and instrument documentation headers (the %P / %PARAMETERS
section) to explicit caret notation, e.g.:

    cm2      -> cm^2
    AA-1     -> AA^-1
    Angs-1   -> AA^-1
    m3       -> m^3
    cm-2 s-1 -> cm^-2 s^-1

This matches the LaTeX generator (_format_unit() in
tools/Python/mcdoc/mcdoc.py), which turns "word^exponent" into a proper
math-mode superscript when building the Component/User manuals -- but can
only do so if the source .comp/.instr headers actually use the "^"
notation in the first place.

Along the way, it also canonicalizes Angstrom shorthand spelling to "AA"
(the dominant convention in this codebase): "Angs", "Ang", "Angstrom" and
"Angstroms" all become "AA". This is a source-level complement to
_tex_with_angstrom()/_ANGSTROM_RE in mcdoc.py, which already recognize
all of these spellings when rendering and turn them into the LaTeX \AA{}
(ring-A, i.e. \AA{}) symbol regardless -- but keeping the source spelling
consistent is worth doing too, rather than accumulating several synonyms
for the same thing across the library.

Scope: deliberately narrow, matching exactly what _format_unit() acts on:
  - Only lines inside a %P / %PARAMETERS McDoc section (tracked by
    scanning for %-tag lines; any %-tag whose first letter isn't P exits
    parameter mode). %D/%I/%L prose is never touched, since converting a
    bracketed mention there to caret notation wouldn't render as a
    superscript anyway (only the dedicated Unit table column does).
  - Only within "[...]" bracket groups on those lines (a parameter's unit
    annotation may appear right after "name:", or at the end of the line
    after the description -- both McDoc-supported forms are handled).
  - Only "word" + optional "-" + digits tokens not already following a
    "^" (so "AA^3" and "cm^-2" are left alone), and only where the digit
    run ends at a word boundary (so e.g. "H2O" is correctly left alone --
    the "O" right after "2" means there's no exponent there to convert).
  - The Angstrom-spelling canonicalization only matches whole "Angs"/
    "Ang"/"Angstrom"/"Angstroms" tokens (word-bounded), so it cannot
    touch part of a longer identifier or word.

Usage:
    # Preview changes as a unified diff; changes nothing on disk:
    python3 fix_unit_exponents.py --dry-run /path/to/mcstas-comps

    # Apply changes in place:
    python3 fix_unit_exponents.py /path/to/mcstas-comps

    # Restrict to one file or a narrower subtree:
    python3 fix_unit_exponents.py --dry-run /path/to/mcstas-comps/contrib/NMO.comp

Please review the diff (or `git diff` after applying) before committing --
tested against real headers (PowderN.comp, NMO.comp, GISANS_sample.comp)
plus a battery of synthetic edge cases (compound units, decimal exponents,
already-correct carets, chemical-formula-shaped false positives, prose
outside %P), but a human pass over a mechanical, repo-wide edit is always
worthwhile. Two files (contrib/NMO.comp, samples/PowderN.comp) have
already been fixed by hand as a spot-check, using "AA" throughout; running
this script over them again should report zero further changes.
"""
import argparse
import difflib
import os
import re
import sys

# A %-tag line, e.g. "* %P", "* %PARAMETERS", "* %D", "* %E", "%I".
TAG_RE = re.compile(r'^\s*\*?\s*%([A-Za-z]+)')

# A McDoc parameter-doc line: "* name: rest-of-line" (tolerant of the
# leading '*' comment marker, dotted names like line_info.dq, and of the
# ':' possibly being followed directly by '[' with no space).
PARAM_LINE_RE = re.compile(r'^(\s*\*\s*[\w.]+\s*:)(.*)$')

# Angstrom shorthand spellings other than the canonical "AA", matched as
# whole words only.
ANGSTROM_SYNONYM_RE = re.compile(r'\b(?:Angstroms?|Angs?)\b')

# A "word immediately followed by an exponent" token, e.g. cm2, AA-1,
# m-2.5 -- but not one already preceded by '^' (e.g. the "3" in "AA^3").
EXPONENT_RE = re.compile(r'(?<!\^)\b([A-Za-z]+)(-?\d+(?:\.\d+)?)\b')


def normalize_unit(unit):
    ''' Canonicalizes Angstrom spelling to "AA", then converts cmN ->
    cm^N / AA-1 -> AA^-1 style tokens within one [unit] bracket's
    content; anything else (units with no numeric suffix, or already
    using '^') is left untouched. '''
    unit = ANGSTROM_SYNONYM_RE.sub('AA', unit)
    return EXPONENT_RE.sub(r'\1^\2', unit)


def process_param_line(line):
    ''' Normalizes every [unit] bracket found anywhere on a parameter-doc
    line (a unit may appear right after "name:", or at the end of the
    line, per the two McDoc-supported forms). Returns (new_line, n). '''
    m = PARAM_LINE_RE.match(line)
    if not m:
        return line, 0
    prefix, rest = m.group(1), m.group(2)
    changed = [0]

    def repl(bm):
        old = bm.group(1)
        new = normalize_unit(old)
        if new != old:
            changed[0] += 1
        return '[%s]' % new

    new_rest = re.sub(r'\[([^\]]*)\]', repl, rest)
    return prefix + new_rest, changed[0]


def process_text(text):
    ''' Returns (new_text, num_changes). '''
    out_lines = []
    changed = 0
    in_params = False
    for line in text.split('\n'):
        tag_m = TAG_RE.match(line)
        if tag_m:
            in_params = tag_m.group(1).upper().startswith('P')
            out_lines.append(line)
            continue
        if in_params:
            line, n = process_param_line(line)
            changed += n
        out_lines.append(line)
    return '\n'.join(out_lines), changed


def iter_files(root):
    if os.path.isfile(root):
        yield root
        return
    for dirpath, _dirnames, filenames in os.walk(root):
        for fn in filenames:
            if fn.endswith('.comp') or fn.endswith('.instr'):
                yield os.path.join(dirpath, fn)


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('path', help='.comp/.instr file, or a directory to scan recursively')
    ap.add_argument('--dry-run', action='store_true',
                     help='Print a diff of would-be changes; do not modify files')
    args = ap.parse_args()

    total_files = 0
    total_lines = 0
    for path in sorted(iter_files(args.path)):
        with open(path, encoding='utf-8') as f:
            text = f.read()
        new_text, n = process_text(text)
        if n == 0:
            continue
        total_files += 1
        total_lines += n
        if args.dry_run:
            diff = difflib.unified_diff(
                text.splitlines(keepends=True),
                new_text.splitlines(keepends=True),
                fromfile=path, tofile=path)
            sys.stdout.writelines(diff)
        else:
            with open(path, 'w', encoding='utf-8') as f:
                f.write(new_text)
            print('updated %s (%d line%s)' % (path, n, '' if n == 1 else 's'))

    print('\n%d file(s), %d line(s) %s.' % (
        total_files, total_lines, 'would change' if args.dry_run else 'changed'))


if __name__ == '__main__':
    main()
