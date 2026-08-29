#!/usr/bin/env python3

"""
Co-plot (overlay) two or more McCode simulation results as HTML pages.

This is a companion to mcplotdiff.py (mcplot-diff-html), sharing part of
its command-line syntax and monitor-matching logic, but instead of
subtracting diff.monN = a.monN - b.monN, mccoplot.py overlays any number
(2 or more) of datasets' monN on the same axes for direct visual
comparison across all of them at once ("does the curve shape/position
still line up"), rather than the difference tool's pairwise "how big is
the gap". Unlike the diff tools (deliberately staying two-way only), this
is an interactive, end-user comparison tool - typically used with a
handful (2-8 or so) of related runs.

Only 1D monitors are supported: datasets are matched by output filename
(exactly like mcplotdiff.py), and any monitor that isn't a matching 1D
monitor across *every* dataset is skipped with a warning - overlaying more
than two 2D images doesn't have an equally natural single-plot
representation, so that case is intentionally left to mcplotdiff.py's
(two-way) diverging-colourmap image instead.

The monitor-loading/matching logic is shared with mcplotdiff.py via
mccodelib.mcplotdiffloader; this script reuses the same client-side d3-based
plotting code (plotfuncs.js) as mcplot.py / mcplot-html - specifically
Plot1D's built-in multi-series support - via a dedicated
template_1d_coplot.html.
"""
import argparse
import logging
import os
import sys
import json
import subprocess

from shutil import copyfile

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

from mccodelib.mcplotloader import Data1D, Data2D
from mccodelib import mccode_config
from mccodelib.mcplotdiffloader import (
    path_base_name, resolve_labels, resolve_colours, dirsafe_name,
    load_monitors, find_original_plot, match_monitors_multi, DEFAULT_PALETTE,
)

# The bare html-plotter (mcplot-html itself, or mxplot-html under McXtrace),
# imported directly rather than shelled out to, so we can guarantee each
# source dataset's own per-monitor pages (<monitor>.dat.html /
# <monitor>.dat_log.html) actually exist before find_original_plot() goes
# looking for them below - see _ensure_html_plots(). Bound to a common name
# (_baseplot) regardless of which flavour's module was actually importable,
# so _ensure_html_plots() doesn't need to know or care which one it is.
try:
    import mcplot as _baseplot
except ImportError:
    import mxplot as _baseplot

global WIDTH, HEIGHT
WIDTH = 1024
HEIGHT = 768


# ---------------------------------------------------------------------------
# legend letters (A, B, C, ... - always positional, independent of the
# actual resolved dataset labels - see _title_for()/write_index() for where
# the real labels go instead)
# ---------------------------------------------------------------------------

def _legend_letters(n):
    import string
    letters = string.ascii_uppercase
    if n <= len(letters):
        return list(letters[:n])
    # astronomically unlikely for a co-plot, but avoid an IndexError
    return ['S%d' % i for i in range(n)]


# ---------------------------------------------------------------------------
# html / json generation
# ---------------------------------------------------------------------------

def get_params_json(data, colour, title):
    p = {
        'w': WIDTH,
        'h': HEIGHT,
        'x': data.xvals,
        'y': data.yvals,
        'yerr': data.y_err_vals,
        'xlabel': data.xlabel,
        'ylabel': data.ylabel,
        'title': title,
        'colour': colour,
    }
    if autosize:
        p['autosize'] = True
    return json.dumps(p)


def _title_for(datas, identities):
    """ identities are what gets shown in the "A=.../B=.../..." block
        below: normally the same as the resolved display labels, but the
        actual source paths (args.datasets) when resolve_labels()
        collapsed the labels to bare letters (basenames collided) - bare
        letters alone would carry no identifying information there,
        unlike in the compact on-plot legend where a bare letter is
        exactly what's wanted. """
    d0 = datas[0]
    letters = _legend_letters(len(datas))
    try:
        lines = ['%s [%s]' % (d0.component, d0.filename), ' ']
        lines += ['%s=%s' % (letter, identity) for letter, identity in zip(letters, identities)]
        lines += [' ', d0.title]
        for letter, data in zip(letters, datas):
            lines.append('%s: I = %s Err = %s N = %s' % (letter, data.values[0], data.values[1], data.values[2]))
        title = '\n'.join(lines)
    except Exception:
        title = '%s\n[%s]' % (d0.component, d0.filename)
    return title


def _legend_rows_html(letters, colours, dat_links):
    rows = []
    for letter, colour, link in zip(letters, colours, dat_links):
        linktext = ('(<a href="%s" target=_blank>plot</a>)' % link) if link else ''
        rows.append('  <div><span class="swatch" style="background:%s"></span>%s %s</div>' % (colour, letter, linktext))
    return '\n'.join(rows)


def get_html(params_list_json, legend_rows_html):
    text = open(os.path.join(os.path.dirname(__file__), 'template_1d_coplot.html')).read()
    text = text.replace("@PARAMS_LIST@", params_list_json)
    text = text.replace("@LEGEND_ROWS@", legend_rows_html)
    logscalestr = "true" if logscale else "false"
    text = text.replace("@LOGSCALE@", logscalestr)
    text = text.replace("@LIBPATH@", libpath)
    return text


def _ensure_html_plots(path):
    """ Runs the bare mcplot-html plotter (_baseplot's own main()) on
        `path` with --nobrowse, so its per-monitor pages
        (<monitor>.dat.html / <monitor>.dat_log.html, next to the monitor's
        own .dat file) are guaranteed to exist before find_original_plot()
        looks for them - rather than depending on the user having already
        run mcplot-html on that source directory separately (which is what
        made the "A (plot)" / "B (plot)" links unreliable before this).

        Works for both a directory and a single monitor file, matching
        whatever the co-plotted paths themselves are - _baseplot's own
        main() already handles both cases the same way mccoplot.py's own
        inputs do.

        Deliberately non-fatal: the co-plot itself doesn't depend on these
        extra pages existing, only the "view original plot" links do, so
        any failure here (e.g. the path doesn't parse as a McCode result
        for some reason) is reported but doesn't abort the co-plot run -
        _baseplot's own main() calls quit() (SystemExit) on a loader
        failure rather than raising, so that's caught explicitly too. """
    mcplot_args = argparse.Namespace(
        simulation=[path], nobrowse=True, log=False, autosize=False,
        libpath=None, output=None, width=None, height=None,
    )
    try:
        _baseplot.main(mcplot_args)
    except SystemExit:
        print("Warning: could not generate mcplot-html pages for '%s' "
              "(used for the per-dataset 'plot' links)" % path)
    except Exception as e:
        print("Warning: could not generate mcplot-html pages for '%s': %s" % (path, e))


def browse(html_filepath):
    try:
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], html_filepath), shell=True)
    except Exception as e:
        raise Exception('Os-specific open browser: %s' % e.__str__())


# ---------------------------------------------------------------------------
# per-monitor plot writer
# ---------------------------------------------------------------------------

def coplot_single(key, datas, outdir, use_logscale, colours, dat_links, identities):
    """ Writes one co-plot (overlaid N-dataset) monitor page to outdir.
        Returns the file path written. """
    global logscale
    logscale = use_logscale

    basename = 'coplot_' + path_base_name(datas[0].filename)
    fname = basename + ('_log.html' if use_logscale else '.html')
    f = os.path.join(outdir, fname)

    if os.path.exists(f):
        os.remove(f)

    letters = _legend_letters(len(datas))
    title_0 = _title_for(datas, identities)

    params_list = [get_params_json(datas[0], colours[0], title_0)]
    for data, colour in zip(datas[1:], colours[1:]):
        params_list.append(get_params_json(data, colour, ""))  # only dataset 0's title is used (see template)
    params_list_json = '[\n' + ',\n'.join(params_list) + '\n]'

    legend_rows_html = _legend_rows_html(letters, colours, dat_links)

    text = get_html(params_list_json, legend_rows_html)

    with open(f, 'w') as fid:
        fid.write(text)

    return f


def _relhref(target, outdir):
    """ Relative link from outdir to target, using forward slashes so the
        generated href works regardless of platform. """
    if target is None:
        return None
    return os.path.relpath(target, start=outdir).replace(os.sep, '/')


# ---------------------------------------------------------------------------
# overview index page
# ---------------------------------------------------------------------------

def write_index(outdir, entries, labels):
    """ Writes an overview index.html with an iframe grid, in the same
        visual style as mcplotdiff.py's write_index().

        'entries' is a list of dicts, one per monitor, with keys:
          'coplot'     - path to the co-plot (linear) html page
          'coplot_log' - path to the co-plot (log) html page, or None

        labels already carries full identification even when
        resolve_labels() had to fall back (its basename-collision
        fallback is each dataset's own full input path, not an
        uninformative placeholder), so there's nothing further to show
        beyond the identity_lines block below.
    """
    filename = os.path.join(outdir, "index.html")
    letters = _legend_letters(len(labels))

    gridgap = int(round(WIDTH * 0.05))
    initial_scale = 0.5
    init_w = WIDTH * initial_scale
    init_h = HEIGHT * initial_scale
    init_pct = int(round(initial_scale * 100))

    title_summary = ' vs '.join(labels)

    with open(filename, 'w') as outfile:
        outfile.write("<html><head>\n")
        outfile.write(f"<title>Co-plots: {title_summary}</title>\n")
        outfile.write("<style>\n")
        outfile.write("  body { background-color: #e0e0e0; margin: 12px; font-family: sans-serif; }\n")
        outfile.write(f"  .plotgrid {{ display: flex; flex-wrap: wrap; gap: {gridgap}px; }}\n")
        outfile.write("  .plotcell { background-color: #ffffff; display: flex; flex-direction: column; align-items: flex-start; }\n")
        outfile.write("  .plotcell iframe { border: none; }\n")
        outfile.write("  .plotcell .links { margin-top: 4px; width: 100%; box-sizing: border-box; display: flex; flex-wrap: wrap; align-items: center; gap: 4px 12px; }\n")
        outfile.write("  .iframe-wrap { overflow: hidden; border: 2px solid #b0b0b0; }\n")
        outfile.write("  .iframe-wrap iframe { transform-origin: top left; display: block; }\n")
        outfile.write("  #sizecontrol { margin-bottom: 16px; font-size: 14px; }\n")
        outfile.write("  #sizecontrol input[type=range] { vertical-align: middle; margin: 0 8px; }\n")
        outfile.write("</style>\n")
        outfile.write("</head><body>\n")
        letters_summary = ' vs '.join(letters)
        outfile.write(f"<h1>Co-plots {letters_summary}:</h1>\n")
        identity_lines = '<br>'.join('%s=%s' % (letter, label) for letter, label in zip(letters, labels))
        outfile.write(f"<h2>{identity_lines}</h2>\n")
        outfile.write(f"<p>Each panel overlays {len(labels)} datasets' monitor data on the same axes.</p>\n")
        outfile.write("<div id='sizecontrol'>\n")
        outfile.write("  <label for='sizeslider'>Figure size:</label>\n")
        outfile.write(f"  <input type='range' id='sizeslider' min='20' max='200' value='{init_pct}' step='5'>\n")
        outfile.write(f"  <span id='sizevalue'>{init_pct}%</span>\n")
        outfile.write("</div>\n")
        outfile.write("<div class='plotgrid'>\n")
        for entry in entries:
            fname = entry['coplot']
            fname_log = entry.get('coplot_log')
            basename = os.path.basename(fname)
            outfile.write(f"<div class='plotcell' data-base-w='{WIDTH}' style='width:{init_w}px;'>\n")
            outfile.write(f"<div class='iframe-wrap' data-base-w='{WIDTH}' data-base-h='{HEIGHT}' style='width:{init_w}px;height:{init_h}px;'>\n")
            outfile.write(f"<iframe src='{basename}' title='{basename}' width={WIDTH} height={HEIGHT} style='transform:scale({initial_scale});'></iframe>\n")
            outfile.write("</div>\n")
            outfile.write("<div class='links'>\n")
            outfile.write(f"<a href='{basename}' target=_blank>[ {basename} ]</a>\n")
            if fname_log:
                basename_log = os.path.basename(fname_log)
                outfile.write(f"<a href='{basename_log}' target=_blank>[ {basename_log} ]</a>\n")
            outfile.write("</div>\n")
            outfile.write("</div>\n")
        outfile.write("</div>\n")
        outfile.write("<script>\n")
        outfile.write("  var slider = document.getElementById('sizeslider');\n")
        outfile.write("  var sizevalue = document.getElementById('sizevalue');\n")
        outfile.write("  var wraps = document.querySelectorAll('.iframe-wrap');\n")
        outfile.write("  var cells = document.querySelectorAll('.plotcell');\n")
        outfile.write("  slider.addEventListener('input', function() {\n")
        outfile.write("    var scale = slider.value / 100;\n")
        outfile.write("    sizevalue.textContent = slider.value + '%';\n")
        outfile.write("    wraps.forEach(function(wrap) {\n")
        outfile.write("      var bw = parseFloat(wrap.dataset.baseW);\n")
        outfile.write("      var bh = parseFloat(wrap.dataset.baseH);\n")
        outfile.write("      wrap.style.width = (bw * scale) + 'px';\n")
        outfile.write("      wrap.style.height = (bh * scale) + 'px';\n")
        outfile.write("      var fr = wrap.querySelector('iframe');\n")
        outfile.write("      fr.style.transform = 'scale(' + scale + ')';\n")
        outfile.write("    });\n")
        outfile.write("    cells.forEach(function(cell) {\n")
        outfile.write("      var bw = parseFloat(cell.dataset.baseW);\n")
        outfile.write("      cell.style.width = (bw * scale) + 'px';\n")
        outfile.write("    });\n")
        outfile.write("  });\n")
        outfile.write("</script>\n")
        outfile.write("</body></html>\n")

    return filename


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------

logscale = False
libpath = ""
autosize = False


def main(args):
    logging.basicConfig(level=logging.INFO)

    global libpath
    if args.libpath:
        libpath = args.libpath[0] + "/"
    global autosize
    if args.autosize:
        autosize = True
    global WIDTH
    if args.width:
        WIDTH = int(args.width[0])
    global HEIGHT
    if args.height:
        HEIGHT = int(args.height[0])

    paths = args.datasets
    if len(paths) < 2:
        print("mccoplot: need at least 2 datasets to co-plot, got %d" % len(paths))
        sys.exit(-1)

    given_labels = args.labels[0].split(',') if args.labels else [None] * len(paths)
    if len(given_labels) != len(paths):
        print("mccoplot: --labels has %d entries but %d datasets were given" % (len(given_labels), len(paths)))
        sys.exit(-1)
    given_labels = [l if l else None for l in given_labels]

    given_colours = args.colours[0].split(',') if args.colours else None

    labels, used_fallback = resolve_labels(paths, given_labels)
    colours = resolve_colours(len(paths), given_colours)

    # identities is what _title_for() shows in each per-monitor figure's
    # "A=.../B=.../..." block: simply the resolved labels themselves -
    # resolve_labels() already falls back to each dataset's full input
    # path when the auto-derived labels would otherwise collide (e.g.
    # several runs all ending in a plain ".../<instrument>/1/" folder), so
    # there's nothing further to add here. The compact on-plot legend
    # keeps just the letters regardless (see _legend_letters()).
    identities = labels

    # determine output directory - deliberately built from the actual
    # input paths (dirsafe_name), not the display labels: labels may
    # legitimately collapse to bare letters when their basenames collide
    # (see resolve_labels()), which would otherwise put every such
    # comparison in the same "coplot_A_vs_B..." folder - a real problem
    # for batch/CI use running many comparisons out of one working
    # directory.
    if args.output:
        outdir = args.output[0]
    else:
        outdir = "coplot_" + "_vs_".join(dirsafe_name(p) for p in paths)
    try:
        os.makedirs(outdir, exist_ok=True)
    except:
        # Fallback for longer outdir names than allowed by OS:
        try:
            print("\nWARNING mccoplot: failed to create directory:\n %s\n - reattempt with 'coplot_mutiple' \n\n" % outdir)
            outdir = "coplot_multiple"
            os.makedirs(outdir, exist_ok=True)
        except Exception as e:
            print('mccoplot loader: ' + e.__str__())
            sys.exit(-1)

    # copy js lib files locally if no lib path was specified
    if libpath == "":
        copyfile(os.path.join(os.path.dirname(__file__), 'd3.v4.min.js'), os.path.join(outdir, 'd3.v4.min.js'))
        copyfile(os.path.join(os.path.dirname(__file__), 'plotfuncs.js'), os.path.join(outdir, 'plotfuncs.js'))

    # load all simulations
    monitors_list = []
    dirs = []
    try:
        for p in paths:
            monitors, d = load_monitors(p)
            monitors_list.append(monitors)
            dirs.append(d)
    except Exception as e:
        print('mccoplot loader: ' + e.__str__())
        sys.exit(-1)

    # Make sure each dataset's own ordinary mcplot-html pages actually
    # exist, so the per-dataset "(plot)" links below have something real
    # to point at, rather than depending on the user having separately
    # run mcplot-html on these directories beforehand.
    for p in paths:
        _ensure_html_plots(p)

    pairs = match_monitors_multi(monitors_list, labels)

    if len(pairs) == 0:
        print("mccoplot: no matching 1D monitors found across all %d datasets, nothing to plot." % len(paths))
        sys.exit(-1)

    # single monitor case: just write one page (or a pair, if --log), like
    # mcplotdiff.py does for a single monitor file input
    single_input = all(os.path.isfile(p) for p in paths)

    entries = []
    for key, datas in pairs:
        # link out to the pre-existing individual mcplot-html pages for
        # each dataset, if they've already been generated, same as
        # mcplotdiff.py does
        dat_links = []
        for d, data in zip(dirs, datas):
            lin, log = find_original_plot(d, data.filename)
            dat_links.append(_relhref(lin, outdir))

        f = coplot_single(key, datas, outdir, False, colours, dat_links, identities)
        f_log = None
        if single_input:
            if args.log:
                f_log = coplot_single(key, datas, outdir, True, colours, dat_links, identities)
        else:
            # folder mode: always produce both linear and log variants,
            # exactly like mcplotdiff.py does for multi-monitor overviews
            f_log = coplot_single(key, datas, outdir, True, colours, dat_links, identities)

        entries.append({'coplot': f, 'coplot_log': f_log})
        print("Generated: %s" % f)
        if f_log:
            print("Generated: %s" % f_log)

    if single_input and len(entries) == 1:
        target = entries[0]['coplot']
        print("Generated: %s" % target)
        if not args.nobrowse:
            browse(target)
        return

    index_file = write_index(outdir, entries, labels)
    print("Generated: %s" % index_file)

    if not args.nobrowse:
        browse(index_file)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('datasets', nargs='+',
                         help='2 or more simulation files or directories to co-plot together '
                              '(e.g. "mccoplot-html run_a run_b run_c")')
    parser.add_argument('-n', '--nobrowse', action='store_true', help='do not open a webbrowser viewer')
    parser.add_argument('-l', '--log', action='store_true',
                         help='also produce a log-scale plot when comparing single monitor files '
                              '(folder-mode always produces both linear and log-style plots)')
    parser.add_argument('--autosize', action='store_true', help='expand to window size on load')
    parser.add_argument('--libpath', nargs='*', help='js lib files path')
    parser.add_argument('-o', '--output', nargs=1, help='specify output directory for the generated plots')
    parser.add_argument('-L', '--labels', nargs=1,
                         help='comma-separated short labels, one per dataset, in the same order '
                              '(e.g. --labels "RunA,RunB,RunC"); default: derived from each path')
    parser.add_argument('-C', '--colours', nargs=1,
                         help='comma-separated overlay colours, one per dataset, in the same order; '
                              'default: %s' % ', '.join(DEFAULT_PALETTE))
    parser.add_argument('-W', '--width', nargs=1, help='width of iframes')
    parser.add_argument('-H', '--height', nargs=1, help='height of iframes')
    args = parser.parse_args()

    main(args)
