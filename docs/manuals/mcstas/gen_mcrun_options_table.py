#!/usr/bin/env python3
"""
gen_mcrun_options_table.py

Generates LaTeX longtable(s) of the mcrun/mcxrun command-line options,
by statically parsing tools/Python/mcrun/mcrun.py's OptionParser setup
(add_mcrun_options / add_mcstas_options). This avoids hand-maintaining
option tables in the manual that silently drift out of sync with the
actual Python front-end (see running.tex, tables f:mcrunoptions and
f:simoptions).

Usage:
    python3 gen_mcrun_options_table.py ../../../tools/Python/mcrun/mcrun.py \
        > mcrun_options_table.tex

Intended to be called from the doc build (see buildscripts/build_manuals_mcstas
and CMakeLists.txt in this directory) each time the manual is rebuilt, so the
option list can never silently go stale again. The output file
mcrun_options_table.tex is \\input from running.tex.

Limitations: this is a best-effort static AST walk, not a full Python
interpreter -- option help strings that are built from data not visible
at parse time (e.g. runtime-computed lists) are rendered with a
'[...]' placeholder. Check the output after regenerating.
"""
import ast, sys, re

def get_str(node):
    """Best-effort extraction of a string value from an AST node,
    resolving simple '%' formatting and '+' concatenation against
    a couple of well-known mccode_config lookups."""
    if isinstance(node, ast.Constant) and isinstance(node.value, str):
        return node.value
    if isinstance(node, ast.BinOp) and isinstance(node.op, ast.Mod):
        left = get_str(node.left)
        if left is None:
            return None
        repl = 'particle'
        right = node.right
        keymap = {'PARTICLE': 'particle', 'EXESUFFIX': 'out'}
        if isinstance(right, ast.Subscript):
            try:
                key = right.slice.value if hasattr(right.slice, 'value') else None
                if key in keymap:
                    repl = keymap[key]
            except Exception:
                pass
        return re.sub(r'%[sd]', repl, left)
    if isinstance(node, ast.BinOp) and isinstance(node.op, ast.Add):
        left = get_str(node.left)
        right = get_str(node.right)
        if left is not None or right is not None:
            return (left or '') + (right or ' [...]')
    if isinstance(node, ast.JoinedStr):
        parts = []
        for v in node.values:
            if isinstance(v, ast.Constant):
                parts.append(str(v.value))
            else:
                parts.append('')
        return ''.join(parts)
    return None

def esc(s):
    if s is None:
        return ''
    s = s.replace('\\', r'\textbackslash{}')
    s = s.replace('&', r'\&').replace('%', r'\%').replace('_', r'\_')
    s = s.replace('#', r'\#').replace('$', r'\$')
    s = s.replace('~', r'\textasciitilde{}')
    s = s.replace('^', r'\textasciicircum{}')
    s = s.replace('{', r'\{').replace('}', r'\}')
    s = re.sub(r'\s+', ' ', s).strip()
    return s

def extract_options(funcname, tree):
    """Walk a given add_*_options() function body and pull out every
    opt.add_option(...)/add(...) call's flags, metavar, help and default."""
    options = []
    for node in ast.walk(tree):
        if isinstance(node, ast.FunctionDef) and node.name == funcname:
            for call in ast.walk(node):
                if isinstance(call, ast.Call):
                    fname = None
                    if isinstance(call.func, ast.Name):
                        fname = call.func.id
                    elif isinstance(call.func, ast.Attribute):
                        fname = call.func.attr
                    if fname != 'add':
                        continue
                    flags = []
                    for a in call.args:
                        s = get_str(a)
                        if s and s.startswith('-'):
                            flags.append(s)
                    help_txt = None
                    metavar = None
                    default = None
                    for kw in call.keywords:
                        if kw.arg == 'help':
                            help_txt = get_str(kw.value)
                        if kw.arg == 'metavar':
                            metavar = get_str(kw.value)
                        if kw.arg == 'default':
                            if isinstance(kw.value, ast.Constant):
                                default = kw.value.value
                    if flags:
                        options.append((flags, metavar, help_txt, default))
    return options

def fmt_table(options, label, caption):
    lines = []
    lines.append(r'\begin{longtable}{|p{0.32\textwidth}|p{0.58\textwidth}|}')
    lines.append(r'\caption{%s} \label{%s} \\' % (caption, label))
    lines.append(r'\hline')
    lines.append(r'\textbf{Option} & \textbf{Description} \\ \hline')
    lines.append(r'\endfirsthead')
    lines.append(r'\hline \textbf{Option} & \textbf{Description} \\ \hline')
    lines.append(r'\endhead')
    for flags, metavar, help_txt, default in options:
        flagstr = r' \newline '.join(
            r'\texttt{%s}' % esc(f) + (r'~\textit{%s}' % esc(metavar) if metavar and f.startswith('--') else '')
            for f in flags)
        desc = esc(help_txt) if help_txt else ''
        if default is not None and default != '' and default is not False:
            desc += r' \textit{(default: %s)}' % esc(str(default))
        lines.append(r'%s & %s \\ \hline' % (flagstr, desc))
    lines.append(r'\end{longtable}')
    return '\n'.join(lines)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('Usage: %s /path/to/mcrun.py > mcrun_options_table.tex' % sys.argv[0])
    srcfile = sys.argv[1]
    tree = ast.parse(open(srcfile).read())
    mcrun_opts = extract_options('add_mcrun_options', tree)
    instr_opts = extract_options('add_mcstas_options', tree)
    out = []
    out.append('%% AUTO-GENERATED by doc/manuals/mcstas/gen_mcrun_options_table.py')
    out.append('%% from tools/Python/mcrun/mcrun.py -- DO NOT EDIT BY HAND.')
    out.append('%% Regenerate with:')
    out.append('%%   python3 gen_mcrun_options_table.py ../../../tools/Python/mcrun/mcrun.py > mcrun_options_table.tex')
    out.append(fmt_table(mcrun_opts, 'f:mcrunoptions',
        r'\texttt{mcrun}-specific options (compilation, scanning, optimisation, MPI/OpenACC).'))
    out.append('')
    out.append(fmt_table(instr_opts, 'f:simoptions',
        r'Instrument/simulation options (also accepted directly by the generated \texttt{<instr>.out} executable).'))
    print('\n\n'.join(out))
