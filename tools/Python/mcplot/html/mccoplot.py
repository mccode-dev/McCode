#!/usr/bin/env python3

"""
Co-plot (overlay) two McCode simulation results as HTML pages.

This is a companion to mcplotdiff.py (mcplot-diff-html), sharing its
command-line syntax and monitor-matching logic, but instead of subtracting
diff.monN = a.monN - b.monN, mccoplot.py overlays a.monN and b.monN on the
same axes for direct visual comparison ("does the curve shape/position
still line up"), rather than the difference tool's "how big is the gap".

Only 1D monitors are supported: a and b are matched by output filename
(exactly like mcplotdiff.py), and any matched pair that isn't a 1D/1D match
(a 2D monitor, or a type mismatch) is skipped with a warning - overlaying
two 2D images doesn't have an equally natural single-plot representation,
so that case is intentionally left to mcplotdiff.py's diverging-colourmap
image instead.

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
    path_base_name, default_labels, dirsafe_name, load_monitors, find_original_plot, match_1d_monitors,
)

# The bare html-plotter (mcplot-html itself), imported directly rather than
# shelled out to, so we can guarantee each source dataset's own per-monitor
# pages (<monitor>.dat.html / <monitor>.dat_log.html) actually exist before
# find_original_plot() goes looking for them below - see _ensure_html_plots().
try:
    import mcplot
except ImportError:
    import mxplot

global WIDTH, HEIGHT
WIDTH = 1024
HEIGHT = 768

# Default overlay colours (A, B): a colourblind-friendly blue/red pair,
# matching Plot1D's expected `colour` (British spelling) params field.
COLOUR_A = '#1f77b4'
COLOUR_B = '#d62728'


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
    return json.dumps(p, indent=4)


def _title_for(data_a, data_b, identity_a, identity_b):
    """ identity_a/identity_b are what gets shown in the "A=.../B=..."
        block below: normally the same as label_a/label_b, but the actual
        source paths (args.a/args.b) when default_labels() collapsed the
        labels to bare "A"/"B" (basenames collided) - bare letters alone
        would carry no identifying information there, unlike in the
        compact on-plot legend where "A"/"B" is exactly what's wanted. """
    try:
        title = '%s [%s]\n \nA=%s\nB=%s\n \n%s\nA: I = %s Err = %s N = %s\nB: I = %s Err = %s N = %s' % (
            data_a.component, data_a.filename, identity_a, identity_b, data_a.title,
            data_a.values[0], data_a.values[1], data_a.values[2],
            data_b.values[0], data_b.values[1], data_b.values[2])
    except Exception:
        title = '%s\n[%s]' % (data_a.component, data_a.filename)
    return title


def get_html(params_a_json, params_b_json, colour_a, colour_b,
             dat_link_a=None, dat_link_b=None):
    text = open(os.path.join(os.path.dirname(__file__), 'template_1d_coplot.html')).read()
    text = text.replace("@PARAMS_A@", params_a_json)
    text = text.replace("@PARAMS_B@", params_b_json)
    text = text.replace("@COLOUR_A@", colour_a)
    text = text.replace("@COLOUR_B@", colour_b)
    text = text.replace("@DATALINK_A@", ('(<a href="%s" target=_blank>plot</a>)' % dat_link_a) if dat_link_a else '')
    text = text.replace("@DATALINK_B@", ('(<a href="%s" target=_blank>plot</a>)' % dat_link_b) if dat_link_b else '')
    logscalestr = "true" if logscale else "false"
    text = text.replace("@LOGSCALE@", logscalestr)
    text = text.replace("@LIBPATH@", libpath)
    return text


def _ensure_html_plots(path):
    """ Runs the bare mcplot-html plotter (mcplot.py's own main()) on
        `path` with --nobrowse, so its per-monitor pages
        (<monitor>.dat.html / <monitor>.dat_log.html, next to the monitor's
        own .dat file) are guaranteed to exist before find_original_plot()
        looks for them - rather than depending on the user having already
        run mcplot-html on that source directory separately (which is what
        made the "A (plot)" / "B (plot)" links unreliable before this).

        Works for both a directory and a single monitor file, matching
        whatever args.a/args.b themselves are - mcplot.py's own main()
        already handles both cases the same way mccoplot.py's own inputs
        do.

        Deliberately non-fatal: the co-plot itself doesn't depend on these
        extra pages existing, only the "view original A/B plot" links do,
        so any failure here (e.g. the path doesn't parse as a McCode
        result for some reason) is reported but doesn't abort the co-plot
        run - mcplot.py's own main() calls quit() (SystemExit) on a loader
        failure rather than raising, so that's caught explicitly too. """
    mcplot_args = argparse.Namespace(
        simulation=[path], nobrowse=True, log=False, autosize=False,
        libpath=None, output=None, width=None, height=None,
    )
    try:
        mcplot.main(mcplot_args)
    except SystemExit:
        print("Warning: could not generate mcplot-html pages for '%s' "
              "(used for the A/B 'plot' links)" % path)
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

def coplot_single(key, data_a, data_b, outdir, use_logscale, label_a, label_b,
                   colour_a, colour_b, dat_link_a=None, dat_link_b=None,
                   identity_a=None, identity_b=None):
    """ Writes one co-plot (overlaid a/b) monitor page to outdir. Returns
        the file path written. """
    global logscale
    logscale = use_logscale

    basename = 'coplot_' + path_base_name(data_a.filename)
    fname = basename + ('_log.html' if use_logscale else '.html')
    f = os.path.join(outdir, fname)

    if os.path.exists(f):
        os.remove(f)

    title_a = _title_for(data_a, data_b, identity_a if identity_a else label_a,
                          identity_b if identity_b else label_b)
    params_a = get_params_json(data_a, colour_a, title_a)
    params_b = get_params_json(data_b, colour_b, "")  # B's title isn't used (see template)

    text = get_html(params_a, params_b, colour_a, colour_b,
                     dat_link_a, dat_link_b)

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

def write_index(outdir, entries, label_a, label_b, path_note=None):
    """ Writes an overview index.html with an iframe grid, in the same
        visual style as mcplotdiff.py's write_index().

        'entries' is a list of dicts, one per monitor, with keys:
          'coplot'     - path to the co-plot (linear) html page
          'coplot_log' - path to the co-plot (log) html page, or None

        path_note, when given (see default_labels()'s used_fallback), is
        shown under the header - label_a/label_b are bare "A"/"B" in that
        case, carrying no identifying information of their own.
    """
    filename = os.path.join(outdir, "index.html")

    gridgap = int(round(WIDTH * 0.05))
    initial_scale = 0.5
    init_w = WIDTH * initial_scale
    init_h = HEIGHT * initial_scale
    init_pct = int(round(initial_scale * 100))

    with open(filename, 'w') as outfile:
        outfile.write("<html><head>\n")
        outfile.write(f"<title>Co-plots: {label_a} vs {label_b}</title>\n")
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
        outfile.write("  .pathnote { color: #666666; font-size: 13px; }\n")
        outfile.write("</style>\n")
        outfile.write("</head><body>\n")
        outfile.write(f"<h1>Co-plots A vs B:</h1>\n")
        outfile.write(f"<h2>A={label_a}<br>B={label_b}</h2>\n")
        outfile.write(f"<p>Each panel overlays ({label_a}).monitor and ({label_b}).monitor on the same axes.</p>\n")
        if path_note:
            note_html = path_note.replace('\n', '<br>')
            outfile.write(f"<p class='pathnote'>{note_html}</p>\n")
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

    colour_a = args.colour_a[0] if args.colour_a else COLOUR_A
    colour_b = args.colour_b[0] if args.colour_b else COLOUR_B

    label_a, label_b, used_fallback = default_labels(
        args.a, args.b,
        args.label_a[0] if args.label_a else None,
        args.label_b[0] if args.label_b else None)

    # When the auto-derived labels collided (e.g. two runs both ending in a
    # plain ".../<instrument>/1/" folder) and default_labels() fell back
    # to bare "A"/"B", those letters carry no identifying information on
    # their own. path_note (shown on the overview index page) and
    # identity_a/identity_b (shown in each per-monitor figure's "A=.../
    # B=..." block) both then use the full source paths instead - while
    # the compact on-plot legend keeps just "A"/"B" either way.
    path_note = None
    identity_a, identity_b = label_a, label_b
    if used_fallback:
        path_note = "A: %s\nB: %s" % (args.a, args.b)
        identity_a, identity_b = args.a, args.b

    # determine output directory - deliberately built from the actual
    # input paths (dirsafe_name), not the display labels: label_a/label_b
    # may legitimately collapse to bare "A"/"B" when their basenames
    # collide (see default_labels()), which would otherwise put every such
    # comparison in the same "coplot_A_vs_B" folder - a real problem for
    # batch/CI use running many comparisons out of one working directory.
    if args.output:
        outdir = args.output[0]
    else:
        outdir = "coplot_%s_vs_%s" % (dirsafe_name(args.a), dirsafe_name(args.b))
    os.makedirs(outdir, exist_ok=True)

    # copy js lib files locally if no lib path was specified
    if libpath == "":
        copyfile(os.path.join(os.path.dirname(__file__), 'd3.v4.min.js'), os.path.join(outdir, 'd3.v4.min.js'))
        copyfile(os.path.join(os.path.dirname(__file__), 'plotfuncs.js'), os.path.join(outdir, 'plotfuncs.js'))

    # load both simulations
    try:
        monitors_a, dir_a = load_monitors(args.a)
        monitors_b, dir_b = load_monitors(args.b)
    except Exception as e:
        print('mccoplot loader: ' + e.__str__())
        sys.exit(-1)

    # Make sure each side's own ordinary mcplot-html pages actually exist,
    # so the "A (plot)" / "B (plot)" links below have something real to
    # point at, rather than depending on the user having separately run
    # mcplot-html on these directories beforehand.
    _ensure_html_plots(args.a)
    _ensure_html_plots(args.b)

    pairs = match_1d_monitors(monitors_a, monitors_b, label_a, label_b)

    if len(pairs) == 0:
        print("mccoplot: no matching 1D monitors found between '%s' and '%s', nothing to plot." % (args.a, args.b))
        sys.exit(-1)

    # single monitor case: just write one page (or a pair, if --log), like
    # mcplotdiff.py does for a single monitor file input
    single_input = os.path.isfile(args.a) and os.path.isfile(args.b)

    entries = []
    for key, data_a, data_b in pairs:
        # link out to the pre-existing individual mcplot-html pages for
        # each side, if they've already been generated, same as
        # mcplotdiff.py does
        a_lin, a_log = find_original_plot(dir_a, data_a.filename)
        b_lin, b_log = find_original_plot(dir_b, data_b.filename)
        dat_link_a = _relhref(a_lin, outdir)
        dat_link_b = _relhref(b_lin, outdir)

        f = coplot_single(key, data_a, data_b, outdir, False, label_a, label_b,
                           colour_a, colour_b, dat_link_a, dat_link_b,
                           identity_a=identity_a, identity_b=identity_b)
        f_log = None
        if single_input:
            if args.log:
                f_log = coplot_single(key, data_a, data_b, outdir, True, label_a, label_b,
                                       colour_a, colour_b, dat_link_a, dat_link_b,
                                       identity_a=identity_a, identity_b=identity_b)
        else:
            # folder mode: always produce both linear and log variants,
            # exactly like mcplotdiff.py does for multi-monitor overviews
            f_log = coplot_single(key, data_a, data_b, outdir, True, label_a, label_b,
                                   colour_a, colour_b, dat_link_a, dat_link_b,
                                   identity_a=identity_a, identity_b=identity_b)

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

    index_file = write_index(outdir, entries, label_a, label_b, path_note=path_note)
    print("Generated: %s" % index_file)

    if not args.nobrowse:
        browse(index_file)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('a', help='first simulation file or directory')
    parser.add_argument('b', help='second simulation file or directory, co-plotted alongside "a"')
    parser.add_argument('-n', '--nobrowse', action='store_true', help='do not open a webbrowser viewer')
    parser.add_argument('-l', '--log', action='store_true',
                         help='also produce a log-scale plot when comparing two single monitor files '
                              '(folder-mode always produces both linear and log-style plots)')
    parser.add_argument('--autosize', action='store_true', help='expand to window size on load')
    parser.add_argument('--libpath', nargs='*', help='js lib files path')
    parser.add_argument('-o', '--output', nargs=1, help='specify output directory for the generated plots')
    parser.add_argument('-A', '--label-a', nargs=1, help='short label used for simulation a in the legend/titles')
    parser.add_argument('-B', '--label-b', nargs=1, help='short label used for simulation b in the legend/titles')
    parser.add_argument('--colour-a', nargs=1, help='override the overlay colour used for a (default %s)' % COLOUR_A)
    parser.add_argument('--colour-b', nargs=1, help='override the overlay colour used for b (default %s)' % COLOUR_B)
    parser.add_argument('-W', '--width', nargs=1, help='width of iframes')
    parser.add_argument('-H', '--height', nargs=1, help='height of iframes')
    args = parser.parse_args()

    main(args)
