'''
Shared logic for computing the difference between two McCode simulation
results, i.e. "diff.monN = a.monN - b.monN" for every monitor present, with
matching binning, in both simulation results 'a' and 'b'.

This is used by the various mcplot-diff frontends (mcplot-diff-html,
mcplot-diff-pyqtgraph, ...) and builds directly on the same
McCodeDataLoader / plot graph machinery used by the ordinary mcplot
frontends (see mcplotloader.py and plotgraph.py). It only adds the
"load two simulations, match up monitors by output filename, subtract"
step, plus a helper to assemble a diff plot graph that can be fed straight
into any existing plot-graph-based frontend (e.g. the interactive
pyqtgraph frontend in pqtgfrontend.py).
'''
import os
import re
import string
import datetime

import numpy as np

from .mcplotloader import McCodeDataLoader, Data1D, Data2D
from .plotgraph import PNSingle, PNMultiple, DataHandle


# ---------------------------------------------------------------------------
# small path/string helpers
# ---------------------------------------------------------------------------

def file_base_name(file_name):
    if '.' in file_name:
        separator_index = file_name.index('.')
        return file_name[:separator_index]
    return file_name


def path_base_name(path):
    return file_base_name(os.path.basename(path))


def _fallback_letters(n):
    """ 'A', 'B', ..., 'Z', then 'AA', 'AB', ... for n > 26 (extremely
        unlikely for a co-plot, but avoids an IndexError rather than
        capping silently). """
    letters = string.ascii_uppercase
    if n <= len(letters):
        return list(letters[:n])
    out = []
    i = 0
    while len(out) < n:
        label = ''
        k = i
        while True:
            label = letters[k % 26] + label
            k = k // 26 - 1
            if k < 0:
                break
        out.append(label)
        i += 1
    return out


def resolve_labels(paths, labels=None):
    """ Resolves user-facing short labels for N (>= 2) simulation paths,
        falling back to the basename of each path for any entry not given
        explicitly. Generalizes the original two-dataset default_labels()
        (still available below, as a thin wrapper) to any number of
        datasets, for the multi-way co-plot tools.

        A basename-only fallback collides for a common layout: results
        stored as .../<instrument>/<testnb>/, where testnb is frequently
        "1" across many runs (e.g. differing only in an MPI parameter
        earlier in the path) - two or more labels would silently come out
        identical. When that happens - and only when *every* label was
        auto-derived, an explicitly given label is never overridden - every
        label falls back to its own full input path instead (not just the
        colliding ones, to avoid a confusing half-basename half-path
        result), since that's always unambiguous and, unlike a bare
        positional letter, still tells the reader which run is which
        without needing to look anywhere else.

        Returns (labels, used_fallback) - used_fallback is True when the
        full-path fallback above was applied, i.e. the returned labels are
        the raw input paths rather than short human-chosen or
        basename-derived names; callers that want a guaranteed-compact
        label regardless (e.g. for a legend swatch) may still want to use
        positional letters purely for display in that case, while using
        these labels themselves wherever the fuller identification is
        useful (titles, filenames, etc). """
    n = len(paths)
    if labels is None:
        labels = [None] * n
    if len(labels) != n:
        raise ValueError("labels must be the same length as paths (%d vs %d)" % (len(labels), n))

    auto_flags = [not l for l in labels]
    resolved = list(labels)
    for i, p in enumerate(paths):
        if auto_flags[i]:
            resolved[i] = path_base_name(os.path.basename(os.path.abspath(p.rstrip('/'))))

    used_fallback = False
    if all(auto_flags) and len(set(resolved)) < len(resolved):
        resolved = [p.rstrip('/\\') for p in paths]
        used_fallback = True

    return resolved, used_fallback


def default_labels(path_a, path_b, label_a=None, label_b=None):
    """ Two-dataset convenience wrapper around resolve_labels(). Kept for
        backward compatibility (used by the mcplotdiff-* tools, which are
        deliberately staying two-way only) and because its signature/
        return shape (label_a, label_b, used_fallback) is a bit more
        convenient than unpacking a 2-element list at every call site.

        Returns (label_a, label_b, used_fallback) - see resolve_labels(). """
    resolved, used_fallback = resolve_labels([path_a, path_b], [label_a, label_b])
    return resolved[0], resolved[1], used_fallback


# Default overlay colour palette (colourblind-friendly, tab10-derived):
# blue and red first, matching the two-dataset tools' existing default
# colours exactly, so a 2-dataset co-plot looks identical to before.
DEFAULT_PALETTE = [
    '#1f77b4',  # blue    (A)
    '#d62728',  # red     (B)
    '#2ca02c',  # green
    '#ff7f0e',  # orange
    '#9467bd',  # purple
    '#8c564b',  # brown
    '#e377c2',  # pink
    '#7f7f7f',  # grey
    '#bcbd22',  # olive
    '#17becf',  # cyan
]


def resolve_colours(n, given=None):
    """ Resolves a list of n overlay colours: given[i] where explicitly
        supplied, else the next unused colour from DEFAULT_PALETTE
        (cycling if n exceeds the palette length - vanishingly unlikely
        for a co-plot, but avoids an IndexError). """
    colours = list(given) if given else []
    while len(colours) < n:
        colours.append(DEFAULT_PALETTE[len(colours) % len(DEFAULT_PALETTE)])
    return colours[:n]


def dirsafe_name(path):
    """ Filesystem-safe slug of a full input path, for use in *output
        directory/file* names.

        This is deliberately separate from default_labels()'s short
        display labels: those may legitimately collapse to bare "A"/"B"
        when the basenames of two input paths collide (e.g. both
        ".../<instrument>/1/") - fine for an on-screen legend, but if the
        *output directory* name were built from those same collapsed
        labels, every such comparison would land in the same
        "..._A_vs_B" folder, clobbering previous runs. This uses the
        input path as actually given (not just its basename), so distinct
        source paths always produce distinct output directories - the
        common case when batch/CI tooling runs many comparisons out of
        the same working directory, each against a differently-timestamped
        or differently-parameterised run.

        Keeps the path as given (relative or absolute, whichever the
        caller passed in) rather than resolving to an absolute path -
        that matches what a human actually typed/expects to see, and
        avoids dragging environment-specific absolute-path noise (e.g.
        a CI runner's long checkout path) into every folder name. """
    p = path.rstrip('/\\')
    if p in ('', '.', '..'):
        # degenerate case (e.g. someone ran the tool with '.' as one side) -
        # fall back to the resolved directory's own basename instead of a
        # bare, unhelpful "."
        p = os.path.basename(os.path.abspath(path))
    p = p.replace('\\', '/').replace('/', '_')
    # collapse any remaining filesystem-unsafe characters
    p = re.sub(r'[^\w\-.]', '_', p)
    # tidy up repeated/leading/trailing underscores left by the above
    p = re.sub(r'_+', '_', p).strip('_')
    return p or 'unnamed'


# ---------------------------------------------------------------------------
# loading two simulations and matching up their monitors
# ---------------------------------------------------------------------------

def resolve_simfile(path):
    """ Given a user-supplied path (file or directory), return (simfile, simdir)
        the way McCodeDataLoader / mcplot.py expects it. """
    if os.path.isfile(path):
        simfile = path
        simdir = os.path.dirname(path)
        if simdir == '':
            simdir = '.'
    elif os.path.isdir(path):
        simdir = path
        simfile = os.path.join(simdir, 'mccode.sim')
    else:
        raise Exception(path + " is neither a file or a directory")
    return simfile, simdir


def load_monitors(path):
    """ Loads a simulation (folder or single monitor file) and returns
        (monitors, directory), where monitors is a dict mapping a monitor
        'key' (its output filename, e.g. 'PSD.dat') to a Data1D/Data2D
        object. """
    simfile, simdir = resolve_simfile(path)

    loader = McCodeDataLoader(simfile=simfile)
    loader.load()

    root = loader.plot_graph

    if isinstance(root, PNSingle):
        data_lst = [root.getdata_idx(0)]
    elif isinstance(root, PNMultiple):
        data_lst = root.getdata_lst()
    else:
        raise Exception('unsupported/empty simulation result in %s' % path)

    monitors = {}
    for data in data_lst:
        if not isinstance(data, (Data1D, Data2D)):
            # skip 0D / event-list / unknown monitors: nothing sensible to subtract
            continue
        key = data.filename if data.filename else data.component
        if key in monitors:
            # extremely unlikely (duplicate filenames), disambiguate by component
            key = '%s__%s' % (key, data.component)
        monitors[key] = data

    return monitors, getattr(loader, 'directory', simdir)


# ---------------------------------------------------------------------------
# computing the difference datasets
# ---------------------------------------------------------------------------

def _pct_diff_str(Ia, Ib):
    """ Percent difference in total intensity, A relative to B (matching
        the "Diff = A - B" convention already used throughout this file),
        formatted with an explicit sign, e.g. "+3.42%" or "-12.05%".

        Relative to B specifically (not a symmetric A/B average) since B
        is the natural reference point for a two-run comparison (e.g. 'is
        my new run within X% of the old/reference one'), and that's
        already how the diff itself (a - b) is signed. Falls back to a
        plain absolute difference when B's total is zero, since a percent
        change relative to zero isn't a meaningful number. """
    try:
        Ia = float(Ia)
        Ib = float(Ib)
    except Exception:
        return 'N/A'
    if Ib == 0:
        if Ia == 0:
            return '0% (both totals zero)'
        return 'N/A (B total is zero; A-B = %s)' % _fmt(Ia - Ib)
    return '%+.2f%%' % (100.0 * (Ia - Ib) / Ib)


def diff_1d(key, a, b, label_a, label_b):
    if len(a.xvals) != len(b.xvals):
        print("Warning: skipping '%s' - differing number of bins (%d vs %d)"
              % (key, len(a.xvals), len(b.xvals)))
        return None

    ay = np.array(a.yvals, dtype=float)
    by = np.array(b.yvals, dtype=float)
    aerr = np.array(a.y_err_vals, dtype=float)
    berr = np.array(b.y_err_vals, dtype=float)

    dy = ay - by
    derr = np.sqrt(aerr**2 + berr**2)

    d = Data1D()
    d.filepath = a.filepath
    d.component = a.component
    d.filename = a.filename
    d.xlabel = a.xlabel
    d.ylabel = a.ylabel
    d.xvar = a.xvar
    d.xlimits = a.xlimits
    d.variables = a.variables
    d.yvar = a.yvar

    d.xvals = a.xvals
    d.yvals = dy.tolist()
    d.y_err_vals = derr.tolist()
    d.Nvals = a.Nvals

    I = float(np.sum(dy))
    Ierr = float(np.sqrt(np.sum(derr**2)))
    try:
        N = int(min(a.values[2], b.values[2]))
    except Exception:
        N = 0
    d.values = (I, Ierr, N)
    d.statistics = '%s: %s\n%s: %s' % (label_a, a.statistics, label_b, b.statistics)
    pct_str = _pct_diff_str(a.values[0], b.values[0])
    d.diff_pct_str = pct_str
    d.title = ' - Diff (A - B), with:\n \nA=%s\nB=%s\nDiff: %s\n \n%s' % (label_a, label_b, pct_str, a.title)

    return d


def diff_2d(key, a, b, label_a, label_b):
    za = np.array(a.zvals, dtype=float)
    zb = np.array(b.zvals, dtype=float)

    if za.shape != zb.shape:
        print("Warning: skipping '%s' - differing array shapes (%s vs %s)"
              % (key, str(za.shape), str(zb.shape)))
        return None

    zdiff = za - zb

    d = Data2D()
    d.filepath = a.filepath
    d.component = a.component
    d.filename = a.filename
    d.xlabel = a.xlabel
    d.ylabel = a.ylabel
    d.xvar = a.xvar
    d.yvar = a.yvar
    d.zvar = a.zvar
    d.xlimits = a.xlimits

    d.zvals = zdiff.tolist()
    d.counts = a.counts

    I = float(np.sum(zdiff))
    try:
        Ierr = float(np.sqrt(float(a.values[1])**2 + float(b.values[1])**2))
        N = int(min(a.values[2], b.values[2]))
    except Exception:
        Ierr = 0.0
        N = 0
    d.values = (I, Ierr, N)
    d.statistics = '%s: %s\n%s: %s' % (label_a, a.statistics, label_b, b.statistics)
    pct_str = _pct_diff_str(a.values[0], b.values[0])
    d.diff_pct_str = pct_str
    d.title = ' - Diff (A - B), with:\n \nA=%s\nB=%s\nDiff: %s\n \n%s' % (label_a, label_b, pct_str, a.title)

    return d


def compute_diffs(monitors_a, monitors_b, label_a, label_b):
    """ Matches monitors present in both simulations by output filename and
        subtracts them. Returns an ordered list of diff Data1D/Data2D objects. """
    keys_a = set(monitors_a.keys())
    keys_b = set(monitors_b.keys())

    only_a = keys_a - keys_b
    only_b = keys_b - keys_a
    for k in sorted(only_a):
        print("Warning: monitor '%s' present in '%s' only, skipping" % (k, label_a))
    for k in sorted(only_b):
        print("Warning: monitor '%s' present in '%s' only, skipping" % (k, label_b))

    diffs = []
    # keep the ordering from simulation a where possible
    ordered_keys = [k for k in monitors_a.keys() if k in keys_b]
    for key in ordered_keys:
        a = monitors_a[key]
        b = monitors_b[key]

        if a.component != b.component:
            print("Warning: '%s' component name differs ('%s' vs '%s'), diffing anyway"
                  % (key, a.component, b.component))

        if isinstance(a, Data1D) and isinstance(b, Data1D):
            d = diff_1d(key, a, b, label_a, label_b)
        elif isinstance(a, Data2D) and isinstance(b, Data2D):
            d = diff_2d(key, a, b, label_a, label_b)
        else:
            print("Warning: skipping '%s' - mismatched or unsupported monitor types" % key)
            d = None

        if d is not None:
            diffs.append(d)

    return diffs


def match_monitors_multi(monitors_list, labels):
    """ Matches monitors present in EVERY dataset in monitors_list (each a
        {filename: Data1D/Data2D} dict, as returned by load_monitors()) by
        output filename, generalizing match_1d_monitors() (still available
        below, as a thin 2-dataset wrapper) to any number (>= 2) of
        datasets, for the multi-way co-plot tools.

        Only 1D monitors with identical binning across *all* datasets are
        kept; anything else (a monitor missing from at least one dataset,
        a 2D monitor, a type mismatch, or mismatched binning) is skipped
        with a warning - overlaying 2D images doesn't have an equally
        natural single-plot representation for N > 2, so that case is left
        to the two-dataset diff tools' diverging-colourmap image instead.

        Ordering follows monitors_list[0]'s own key order, not sorted
        alphabetically - matches how the underlying simulation loader
        orders monitors, so panels come out in a sensible, predictable
        order rather than shuffled.

        Returns an ordered list of (key, [data_0, data_1, ..., data_N-1]). """
    if len(monitors_list) < 2:
        raise ValueError("match_monitors_multi needs at least 2 datasets")

    key_sets = [set(m.keys()) for m in monitors_list]
    common_keys = set.intersection(*key_sets)
    all_keys = set.union(*key_sets)

    for k in sorted(all_keys - common_keys):
        missing_from = [labels[i] for i, ks in enumerate(key_sets) if k not in ks]
        print("Warning: monitor '%s' missing from %s, skipping" % (k, ', '.join(missing_from)))

    ordered_keys = [k for k in monitors_list[0].keys() if k in common_keys]

    pairs = []
    for key in ordered_keys:
        datas = [m[key] for m in monitors_list]

        if not all(isinstance(d, Data1D) for d in datas):
            if any(isinstance(d, Data2D) for d in datas):
                print("Warning: skipping '%s' - co-plotting only supports 1D monitors "
                      "(got types: %s); try a diff tool for 2D comparisons"
                      % (key, ', '.join(sorted(set(type(d).__name__ for d in datas)))))
            else:
                print("Warning: skipping '%s' - mismatched or unsupported monitor types" % key)
            continue

        nbins = set(len(d.xvals) for d in datas)
        if len(nbins) > 1:
            print("Warning: skipping '%s' - differing number of bins (%s)"
                  % (key, ', '.join(str(n) for n in sorted(nbins))))
            continue

        components = set(d.component for d in datas)
        if len(components) > 1:
            print("Warning: '%s' component name differs (%s), co-plotting anyway"
                  % (key, ', '.join(sorted(components))))

        pairs.append((key, datas))

    return pairs


def match_1d_monitors(monitors_a, monitors_b, label_a, label_b):
    """ Two-dataset convenience wrapper around match_monitors_multi().
        Kept for backward compatibility - returns an ordered list of
        (key, data_a, data_b) rather than (key, [data_a, data_b]). """
    pairs = match_monitors_multi([monitors_a, monitors_b], [label_a, label_b])
    return [(key, datas[0], datas[1]) for key, datas in pairs]


def load_and_diff(path_a, path_b, label_a=None, label_b=None):
    """ Convenience one-shot: loads both simulations, resolves labels, and
        returns (diffs, dir_a, dir_b, label_a, label_b, used_fallback). """
    label_a, label_b, used_fallback = default_labels(path_a, path_b, label_a, label_b)
    monitors_a, dir_a = load_monitors(path_a)
    monitors_b, dir_b = load_monitors(path_b)
    diffs = compute_diffs(monitors_a, monitors_b, label_a, label_b)
    return diffs, dir_a, dir_b, label_a, label_b, used_fallback


# ---------------------------------------------------------------------------
# locating pre-existing mcplot-html output for the two original simulations
# ---------------------------------------------------------------------------

def find_original_plot(directory, monitor_filename):
    """ Looks for the mcplot-html page(s) previously generated (by
        mcplot.py / mcplot-html) for a given monitor output file in
        `directory`, e.g. 'PSD.dat' -> 'PSD.dat.html' / 'PSD.dat_log.html'.
        Returns (linear_path_or_None, log_path_or_None); either may be None
        if the corresponding page hasn't been generated (yet). """
    if not directory or not monitor_filename:
        return None, None
    base = os.path.join(directory, monitor_filename)
    linear = base + ".html"
    log = base + "_log.html"
    linear = linear if os.path.isfile(linear) else None
    log = log if os.path.isfile(log) else None
    return linear, log


# ---------------------------------------------------------------------------
# assembling a plot graph from diff data, for plot-graph based frontends
# ---------------------------------------------------------------------------

def build_diff_plotgraph(diffs):
    """ Wraps a list of diff Data1D/Data2D objects (as returned by
        compute_diffs()) into a two-level plot graph (PNMultiple root with
        PNSingle primaries/secondaries), exactly the way
        mcplotloader.load_simulation() does for an ordinary simulation
        folder. This lets diff data be fed directly into any existing plot
        graph frontend, e.g. mccodelib.pqtgfrontend.McPyqtgraphPlotter. """
    handles = [DataHandle(load_fct=None, data=d) for d in diffs]
    root = PNMultiple(handles)
    primnodes = [PNSingle(h) for h in handles]
    root.set_primaries(primnodes)
    root.set_secondaries(primnodes)  # only one way to click here, as in load_simulation()
    return root


# ---------------------------------------------------------------------------
# writing diff datasets back out in McCode ASCII format
# ---------------------------------------------------------------------------

def _fmt(x):
    """ Standard number formatting for McCode-format file bodies/headers. """
    return '%.10g' % float(x)


def _sanitize(s):
    """ Collapses a string to something safe for a single-line "# field:
        value" header (diff titles in particular start with a leading
        newline, intended for on-screen display, which would otherwise
        split the header across two physical lines - the second of which
        has no leading "#" and so corrupts the file for any reader). """
    return ' '.join(str(s).split())


def _common_header_lines(data):
    return [
        '# Format: McCode with text headers',
        '# URL: http://www.mccode.org',
        '# Creator: mcplotdiff (McCode difference tool)',
        '# component: %s' % _sanitize(data.component),
        '# filename: %s' % _sanitize(data.filename),
        '# title: %s' % _sanitize(data.title),
    ]


def _write_1d_dat(data, outdir, prefix):
    filename = prefix + data.filename
    filepath = os.path.join(outdir, filename)

    lines = _common_header_lines(data)
    # Required: mcplotloader.py's _load_monitor() reads this line to decide
    # which parser to dispatch to (_parse_1D_monitor vs _parse_2D_monitor)
    # *before* either parser ever runs - without it, loading fails at the
    # dispatch step itself, not inside the parser.
    lines.insert(3, '# type: array_1d(%d)' % len(data.xvals))
    lines.append('# xlabel: %s' % _sanitize(data.xlabel))
    lines.append('# ylabel: %s' % _sanitize(data.ylabel))
    lines.append('# xvar: %s' % _sanitize(data.xvar))
    lines.append('# yvar: (%s,%s)' % (data.yvar[0], data.yvar[1]))
    lines.append('# xlimits: %s %s' % (_fmt(data.xlimits[0]), _fmt(data.xlimits[1])))
    lines.append('# variables: %s %s %s N' % (data.xvar, data.yvar[0], data.yvar[1]))
    lines.append('# values: %s %s %s' % (_fmt(data.values[0]), _fmt(data.values[1]), _fmt(data.values[2])))
    # The standard "# statistics: X0=...; dX=...;" field is a
    # weighted centroid/width, which assumes a non-negative intensity
    # distribution - not generally meaningful for signed difference data,
    # so it's written as zero here rather than a misleading number. The
    # real per-source statistics (from the original a/b datasets) are kept
    # as an additional, non-standard comment line for human reference;
    # readers (including mcplotloader.py's own parser) that only look for
    # the standard fields simply ignore it.
    lines.append('# statistics: X0=0; dX=0;')
    lines.append('# Diff-statistics: %s' % _sanitize(data.statistics.replace('\n', ' | ')))

    with open(filepath, 'w') as f:
        f.write('\n'.join(lines) + '\n')
        for x, y, yerr, n in zip(data.xvals, data.yvals, data.y_err_vals, data.Nvals):
            f.write('%s %s %s %s\n' % (_fmt(x), _fmt(y), _fmt(yerr), _fmt(n)))

    return filepath


def _write_2d_dat(data, outdir, prefix):
    filename = prefix + data.filename
    filepath = os.path.join(outdir, filename)

    lines = _common_header_lines(data)
    zshape = np.shape(data.zvals) if data.zvals else (0, 0)
    lines.insert(3, '# type: array_2d(%d, %d)' % (zshape[0], zshape[1] if len(zshape) > 1 else 0))
    lines.append('# xlabel: %s' % _sanitize(data.xlabel))
    lines.append('# ylabel: %s' % _sanitize(data.ylabel))
    lines.append('# xvar: %s' % _sanitize(data.xvar))
    lines.append('# yvar: %s' % _sanitize(data.yvar))
    lines.append('# zvar: %s' % _sanitize(data.zvar))
    lines.append('# xylimits: %s %s %s %s' % tuple(_fmt(v) for v in data.xlimits))
    lines.append('# values: %s %s %s' % (_fmt(data.values[0]), _fmt(data.values[1]), _fmt(data.values[2])))
    zarr = np.array(data.zvals, dtype=float) if data.zvals else np.zeros((1, 1))
    lines.append('# signal: Min=%s; Max=%s; Mean=%s;' % (_fmt(zarr.min()), _fmt(zarr.max()), _fmt(zarr.mean())))
    # see _write_1d_dat() for why this is zeroed rather than reusing
    # data.statistics directly
    lines.append('# statistics: X0=0; dX=0; Y0=0; dY=0;')
    lines.append('# Diff-statistics: %s' % _sanitize(data.statistics.replace('\n', ' | ')))

    with open(filepath, 'w') as f:
        f.write('\n'.join(lines) + '\n')
        f.write('# Data [%s/%s] %s:\n' % (_sanitize(data.component), _sanitize(data.filename), _sanitize(data.zvar)))
        for row in data.zvals:
            f.write(' '.join(_fmt(v) for v in row) + '\n')
        if data.counts:
            f.write('# Events [%s/%s] N:\n' % (_sanitize(data.component), _sanitize(data.filename)))
            for row in data.counts:
                f.write(' '.join(_fmt(v) for v in row) + '\n')

    return filepath


def write_mccode_dat(data, outdir, prefix='diff_'):
    """ Writes a single diff Data1D/Data2D object out in the same ASCII
        "# comment header + data body" format ordinary McCode monitor
        output files use - readable back in by mcplotloader.py itself (and,
        since it follows the same conventions, by any other tool that
        already reads McCode monitor output, e.g. Mantid's loaders).

        Returns the path written, or None if `data` isn't a supported type. """
    if isinstance(data, Data1D):
        return _write_1d_dat(data, outdir, prefix)
    elif isinstance(data, Data2D):
        return _write_2d_dat(data, outdir, prefix)
    return None


def write_all_mccode_dat(diffs, outdir, prefix='diff_'):
    """ Convenience: write_mccode_dat() for every diff dataset in `diffs`,
        into `outdir` (created if missing). Returns the list of paths
        written. """
    os.makedirs(outdir, exist_ok=True)
    paths = []
    for d in diffs:
        p = write_mccode_dat(d, outdir, prefix=prefix)
        if p:
            paths.append(p)
    return paths


# ---------------------------------------------------------------------------
# writing a mccode.sim index alongside the diff .dat files
# ---------------------------------------------------------------------------

def write_mccode_sim(diffs, outdir, label_a=None, label_b=None, instrument='diff',
                      prefix='diff_', filename='mccode.sim'):
    """ Writes a mccode.sim index file summarizing a set of diff datasets
        (as returned by compute_diffs()), in the same style a real McStas/
        McXtrace simulation directory uses, so `outdir` can be opened as a
        genuine simulation directory (e.g. `mcplot-html <outdir>/`) via the
        standard "mccode.sim + monitors" loading path
        (mcplotloader.is_mccodesim_w_monitors() -> load_simulation()),
        rather than falling back to the "folder full of loose .dat files"
        path (which also works, but is the less standard route, and
        wouldn't be recognised the same way by other McCode-aware tools
        that expect a simulation directory to have an index).

        Must be called *after* write_mccode_dat()/write_all_mccode_dat()
        have already written each diff monitor's own .dat file into
        `outdir` with the same `prefix` - this only writes the index, not
        the monitor data itself, and the "filename:" lines it writes must
        match the files actually on disk.

        The only thing mcplotloader.py's own parser
        (_get_filenames_from_mccodesim) actually requires is one
        "begin data ... filename: <name> ... end data" block per monitor -
        the surrounding instrument/simulation header blocks aren't parsed
        by mcplotloader.py at all, and are included purely so the file
        reads like (and is compatible with expectations set by) a genuine
        mccode.sim, for humans and any other McCode-aware tooling.

        Returns the path written. """
    filepath = os.path.join(outdir, filename)
    now = datetime.datetime.now().strftime('%a %b %d %H:%M:%S %Y')
    label_a = label_a or 'A'
    label_b = label_b or 'B'

    lines = []
    lines.append('McCode diff simulation description file for %s.' % instrument)
    lines.append('Date:    %s' % now)
    lines.append('Program: mcplotdiff (McCode difference tool)')
    lines.append('')
    lines.append('begin instrument: %s' % instrument)
    lines.append('  File: %s' % os.path.join(outdir, instrument))
    lines.append('  Source: diff(%s, %s)' % (_sanitize(label_a), _sanitize(label_b)))
    lines.append('  Trace_enabled: no')
    lines.append('  Default_main: yes')
    lines.append('  Embedded_runtime: yes')
    lines.append('end instrument')
    lines.append('')
    lines.append('begin simulation: %s' % outdir)
    lines.append('  Format: McCode with text headers')
    lines.append('  URL: http://www.mccode.org')
    lines.append('  Creator: mcplotdiff (McCode difference tool)')
    lines.append('  Instrument: diff(%s, %s)' % (_sanitize(label_a), _sanitize(label_b)))
    lines.append('  Ncount: 0')
    lines.append('  Trace: no')
    lines.append('  Param: a=%s b=%s' % (_sanitize(label_a), _sanitize(label_b)))
    lines.append('end simulation')
    lines.append('')

    for data in diffs:
        if isinstance(data, Data1D):
            type_line = '  type: array_1d(%d)' % len(data.xvals)
        elif isinstance(data, Data2D):
            zshape = np.shape(data.zvals) if data.zvals else (0, 0)
            type_line = '  type: array_2d(%d, %d)' % (zshape[0], zshape[1] if len(zshape) > 1 else 0)
        else:
            continue

        lines.append('begin data')
        lines.append('  Date: %s' % now)
        lines.append(type_line)
        lines.append('  component: %s' % _sanitize(data.component))
        lines.append('  title: %s' % _sanitize(data.title))
        try:
            lines.append('  values: %s %s %s' % (_fmt(data.values[0]), _fmt(data.values[1]), _fmt(data.values[2])))
        except Exception:
            pass
        # This is the one line mcplotloader.py's _get_filenames_from_mccodesim()
        # actually looks for - it must match the real file written by
        # write_mccode_dat()/write_all_mccode_dat() with the same prefix.
        lines.append('  filename: %s' % (prefix + data.filename))
        lines.append('end data')
        lines.append('')

    with open(filepath, 'w') as f:
        f.write('\n'.join(lines) + '\n')

    return filepath
