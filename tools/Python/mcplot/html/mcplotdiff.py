#!/usr/bin/env python3

"""
Display the difference between two McCode simulation results as HTML pages.

This is an alternative variant of mcplot.py (the mcplot-html / mxplot-html tool)
that, instead of plotting the monitors of a single simulation, takes two
simulation folders (or two single monitor files) 'a' and 'b' and produces one
plot per monitor of the *difference* diff.monN = a.monN - b.monN, for every
monitor that is present (with matching binning) in both simulations.

The monitor-loading, matching and subtraction logic lives in
mccodelib.mcplotdiffloader, shared with the pyqtgraph-based mcplot-diff
frontend. This script builds on that, and reuses the same client-side d3-based
plotting code (plotfuncs.js / template_1d.html / template_2d.html) as
mcplot.py / mcplot-html, so that difference plots look and behave like
ordinary mcplot-html plots.
"""
import argparse
import logging
import os
import sys
import io
import base64
import json
import subprocess

import numpy as np
from PIL import Image
from shutil import copyfile

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

from mccodelib.mcplotloader import Data1D, Data2D
from mccodelib import mccode_config
from mccodelib import mcplotdiffloader as diffloader
from mccodelib.mcplotdiffloader import (
    path_base_name, default_labels, dirsafe_name, load_monitors, compute_diffs, find_original_plot,
    write_mccode_dat, write_mccode_sim,
)

global WIDTH, HEIGHT
WIDTH = 1024
HEIGHT = 768


# ---------------------------------------------------------------------------
# colour maps
# ---------------------------------------------------------------------------

def get_cm_diverging(n=65):
    """ blue - white - red diverging colour map (colorbrewer 'RdBu', reversed
        so that blue = negative, red = positive), suitable for signed
        difference data. """
    c_neg = np.array([33, 102, 172])   # blue  -> negative values
    c_mid = np.array([255, 255, 255])  # white -> zero
    c_pos = np.array([178, 24, 43])    # red   -> positive values

    cm = np.zeros((n, 4), dtype=np.ubyte)
    for i in range(n):
        t = i / (n - 1)  # 0..1
        if t <= 0.5:
            frac = t / 0.5
            rgb = c_neg + frac * (c_mid - c_neg)
        else:
            frac = (t - 0.5) / 0.5
            rgb = c_mid + frac * (c_pos - c_mid)
        cm[i, 0:3] = np.round(rgb)
        cm[i, 3] = 255
    return cm


def lookup(cm, x):
    """ Real number from 0 to 1 mapped onto the colour map, clamped to range. """
    if np.isnan(x):
        x = 0.5
    x = min(max(x, 0.0), 1.0)
    xp = (len(cm) - 1) * x
    idx = int(np.round(xp))
    idx = min(max(idx, 0), len(cm) - 1)
    return cm[idx]


def lookup_vec(cm, x):
    """ Vectorized equivalent of lookup(): x is an ndarray of values
        (nominally in [0, 1]), returns an array of shape x.shape + (4,)
        by indexing into the colour map cm for every element at once.

        This replaces what used to be a per-pixel Python-level call to
        lookup() inside a nested "for i / for j" loop in
        get_params_str_2D_diff() - for a modestly sized 2D monitor (e.g.
        100x100 = 10000 pixels), that's 10000 individual Python function
        calls per image, x2 images (linear + log) x2 (data + colourbar
        rendering is cheap by comparison, but the main image dominates),
        which adds up badly across hundreds of monitors. Doing the same
        clamp/round/index operations as whole-array numpy operations here
        is the same math, just executed in C rather than the Python
        interpreter loop. """
    x = np.nan_to_num(x, nan=0.5)
    x = np.clip(x, 0.0, 1.0)
    idx = np.round((len(cm) - 1) * x).astype(int)
    idx = np.clip(idx, 0, len(cm) - 1)
    return cm[idx]


# ---------------------------------------------------------------------------
# html / json generation (mirrors mcplot.py's get_html/get_json_*)
# ---------------------------------------------------------------------------

def get_html(template_name, params, dat_basename=None):
    text = open(os.path.join(os.path.dirname(__file__), template_name)).read()
    text = text.replace("@PARAMS@", params)

    if dat_basename:
        # dat_basename is a relative path (same directory as the html page
        # itself), pointing at the McCode-format .dat file written by
        # write_mccode_dat() (absent only if --no-dat was given).
        text = text.replace("@DATAFILE@", dat_basename)
        text = text.replace("@DATALINK@", "Download difference data (McCode format): %s" % dat_basename)
    else:
        # --no-dat was given, so there's genuinely no file to link to -
        # keep the anchor inert rather than pointing at a basename that
        # was never written to this directory.
        text = text.replace("@DATAFILE@", "#")
        text = text.replace("@DATALINK@", "Diff mode (no physical datafile - rerun without --no-dat to generate one)")

    logscalestr = "true" if logscale == True else "false"
    text = text.replace("@LOGSCALE@", logscalestr)
    text = text.replace("@LIBPATH@", libpath)
    return text


def get_json_1d(x, y, yerr, xlabel, ylabel, title):
    p = {}
    p['w'] = WIDTH
    p['h'] = HEIGHT
    p['x'] = x
    p['y'] = y
    p['yerr'] = yerr
    p['xlabel'] = xlabel
    p['ylabel'] = ylabel
    p['title'] = title
    if autosize:
        p['autosize'] = True
    return json.dumps(p, indent=4)


def get_json_2d(xmin, xmax, ymin, ymax, image_str, colorbar_img_str, cb_min, cb_max,
                image_str_log, colorbar_img_str_log, cb_min_log, cb_max_log,
                xlabel, ylabel, title):
    p = {}
    p['w'] = WIDTH
    p['h'] = HEIGHT
    p['xmin'] = xmin
    p['xmax'] = xmax
    p['ymin'] = ymin
    p['ymax'] = ymax

    p['img2dData'] = image_str
    p['imgColorbar'] = colorbar_img_str
    p['cbMin'] = cb_min
    p['cbMax'] = cb_max

    p['img2dDataLog'] = image_str_log
    p['imgColorbarLog'] = colorbar_img_str_log
    p['cbMinLog'] = cb_min_log
    p['cbMaxLog'] = cb_max_log

    p['xlabel'] = xlabel
    p['ylabel'] = ylabel
    p['title'] = title
    if autosize:
        p['autosize'] = True
    return json.dumps(p, indent=4)


def get_params_str_1D(data):
    x = data.xvals
    y = data.yvals
    yerr = data.y_err_vals
    try:
        title = '%s [%s] %s\nI = %s Err = %s N = %s\n %s' % (
            data.component, data.filename, data.title,
            data.values[0], data.values[1], data.values[2], data.statistics)
    except Exception:
        title = '%s\n[%s]' % (data.component, data.filename)
    return get_json_1d(x, y, yerr, data.xlabel, data.ylabel, title)


def _encode_png(img_array):
    image = Image.fromarray(np.flipud(img_array).astype(np.uint8))
    output = io.BytesIO()
    image.save(output, format="png")
    contents = output.getvalue()
    output.close()
    return str(base64.b64encode(contents)).lstrip('b').strip("'")


def get_params_str_2D_diff(data):
    """ Like mcplot.py's get_params_str_2D, but colours the 2d image with a
        diverging (blue/white/red) colour map centred on zero, since diff
        data can be both positive and negative. The 'log' variant uses a
        signed (symlog-style) transform: sign(v) * log10(1 + |v|/eps),
        so that small differences remain visible while preserving sign. """
    vals = np.array(data.zvals, dtype=float)
    cm = get_cm_diverging()

    # --- linear diverging image ---
    maxabs = float(np.max(np.abs(vals))) if vals.size else 0.0
    if maxabs > 0:
        x = (vals / maxabs + 1.0) / 2.0
    else:
        x = np.full(vals.shape, 0.5)
    img = lookup_vec(cm, x)
    encoded_2d_data = _encode_png(img)

    # --- signed-log ("symlog") diverging image ---
    if maxabs > 0:
        eps = maxabs / 1e4
        slog = np.sign(vals) * np.log10(1.0 + np.abs(vals) / eps)
        maxslog = float(np.max(np.abs(slog))) if slog.size else 0.0
    else:
        slog = vals
        maxslog = 0.0

    if maxslog > 0:
        xlog = (slog / maxslog + 1.0) / 2.0
    else:
        xlog = np.full(vals.shape, 0.5)
    img_log = lookup_vec(cm, xlog)
    encoded_2d_data_log = _encode_png(img_log)

    # --- colour bars ---
    def make_colorbar(cm):
        # equivalent to the old "for i in range(256): img[255-i,0] = lookup(cm, i/255)":
        # colors[i] = lookup(cm, i/255); reversing gives img[r] = colors[255-r]
        colors = lookup_vec(cm, np.arange(256) / 255.0)
        return _encode_png(colors[::-1].reshape(256, 1, 4))

    encoded_cb = make_colorbar(cm)
    encoded_cb_log = make_colorbar(cm)

    # axis limits
    xmin = data.xlimits[0]
    xmax = data.xlimits[1]
    ymin = data.xlimits[2]
    ymax = data.xlimits[3]

    # colour bar limits: symmetric around zero
    cb_min, cb_max = -maxabs, maxabs
    cb_min_log, cb_max_log = -maxslog, maxslog

    try:
        title = '%s [%s] %s\nI = %s Err = %s N = %s\n %s' % (
            data.component, data.filename, data.title,
            data.values[0], data.values[1], data.values[2], data.statistics)
    except Exception:
        title = '%s\n[%s]' % (data.component, data.filename)

    return get_json_2d(xmin, xmax, ymin, ymax,
                        encoded_2d_data, encoded_cb, cb_min, cb_max,
                        encoded_2d_data_log, encoded_cb_log, cb_min_log, cb_max_log,
                        data.xlabel, data.ylabel, title)


def browse(html_filepath):
    try:
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], html_filepath), shell=True)
    except Exception as e:
        raise Exception('Os-specific open browser: %s' % e.__str__())


# ---------------------------------------------------------------------------
# per-monitor plot writers
# ---------------------------------------------------------------------------

def plot_diff_single(data, outdir, use_logscale, dat_basename=None):
    """ Writes one diff monitor to an html file in outdir, mirroring
        mcplot.py's plotfunc_single. Returns the file path written. """
    global logscale
    logscale = use_logscale

    basename = 'diff_' + path_base_name(data.filename)
    fname = basename + ('_log.html' if use_logscale else '.html')
    f = os.path.join(outdir, fname)

    if os.path.exists(f):
        os.remove(f)

    if isinstance(data, Data1D):
        text = get_html('template_1d.html', get_params_str_1D(data), dat_basename)
    elif isinstance(data, Data2D):
        text = get_html('template_2d.html', get_params_str_2D_diff(data), dat_basename)
    else:
        return None

    with open(f, 'w') as fid:
        fid.write(text)

    return f


def _relhref(target, outdir):
    """ Relative link from outdir to target, using forward slashes so the
        generated href works regardless of platform. """
    if target is None:
        return None
    return os.path.relpath(target, start=outdir).replace(os.sep, '/')


def write_index(outdir, entries, label_a, label_b, path_a=None, path_b=None, used_fallback=False):
    """ Writes an overview index.html with an iframe grid, in the same visual
        style as mcplot.py's PNMultiple index page.

        'entries' is a list of dicts, one per monitor, with keys:
          'diff'      - path to the diff (linear) html page (inside outdir)
          'diff_log'  - path to the diff (log) html page, or None
          'a_lin'     - path to the pre-existing mcplot-html page for
                        simulation a's monitor (linear), or None
          'a_log'     - ... (log), or None
          'b_lin'     - path to the pre-existing mcplot-html page for
                        simulation b's monitor (linear), or None
          'b_log'     - ... (log), or None

        path_a/path_b/used_fallback come from default_labels(): when the
        auto-derived labels collided (e.g. two runs both ending in a plain
        ".../<instrument>/1/" folder), default_labels() falls back to using
        each side's full input path as its label directly - so label_a/
        label_b already carry full identification in that case, and
        path_a/path_b/used_fallback are only kept here for backward
        compatibility with existing callers; nothing further is shown for
        them.
    """
    filename = os.path.join(outdir, "index.html")

    gridgap = int(round(WIDTH * 0.05))
    initial_scale = 0.5
    init_w = WIDTH * initial_scale
    init_h = HEIGHT * initial_scale
    init_pct = int(round(initial_scale * 100))

    with open(filename, 'w') as outfile:
        outfile.write("<html><head>\n")
        outfile.write(f"<title>Difference plots: {label_a} - {label_b}</title>\n")
        outfile.write("<style>\n")
        outfile.write("  body { background-color: #e0e0e0; margin: 12px; font-family: sans-serif; }\n")
        outfile.write(f"  .plotgrid {{ display: flex; flex-wrap: wrap; gap: {gridgap}px; }}\n")
        outfile.write("  .plotcell { background-color: #ffffff; display: flex; flex-direction: column; align-items: flex-start; }\n")
        outfile.write("  .plotcell iframe { border: none; }\n")
        outfile.write("  .plotcell .links { margin-top: 4px; width: 100%; box-sizing: border-box; display: flex; flex-wrap: wrap; align-items: center; gap: 4px 12px; }\n")
        outfile.write("  .plotcell .links .origlinks { color: #999999; }\n")
        outfile.write("  .iframe-wrap { overflow: hidden; border: 2px solid #b0b0b0; }\n")
        outfile.write("  .iframe-wrap iframe { transform-origin: top left; display: block; }\n")
        outfile.write("  #sizecontrol { margin-bottom: 16px; font-size: 14px; }\n")
        outfile.write("  #sizecontrol input[type=range] { vertical-align: middle; margin: 0 8px; }\n")
        outfile.write("</style>\n")
        outfile.write("</head><body>\n")
        outfile.write("<h1>Difference plots: A vs B</h1>\n")
        outfile.write(f"<h2>A={label_a}<br>B={label_b}</h2>\n")
        outfile.write(f"<p>diff.monitor = ({label_a}).monitor &minus; ({label_b}).monitor</p>\n")
        outfile.write("<div id='sizecontrol'>\n")
        outfile.write("  <label for='sizeslider'>Figure size:</label>\n")
        outfile.write(f"  <input type='range' id='sizeslider' min='20' max='200' value='{init_pct}' step='5'>\n")
        outfile.write(f"  <span id='sizevalue'>{init_pct}%</span>\n")
        outfile.write("</div>\n")
        outfile.write("<div class='plotgrid'>\n")
        for entry in entries:
            fname = entry['diff']
            fname_log = entry.get('diff_log')
            basename = os.path.basename(fname)
            outfile.write(f"<div class='plotcell' data-base-w='{WIDTH}' style='width:{init_w}px;'>\n")
            outfile.write(f"<div class='iframe-wrap' data-base-w='{WIDTH}' data-base-h='{HEIGHT}' style='width:{init_w}px;height:{init_h}px;'>\n")
            outfile.write(f"<iframe src='{basename}' title='{basename}' width={WIDTH} height={HEIGHT} style='transform:scale({initial_scale});'></iframe>\n")
            outfile.write("</div>\n")
            outfile.write("<div class='links'>\n")
            outfile.write("<span class='links'>\n")
            outfile.write(f"<a href='{basename}' target=_blank>[ {basename} ]</a>\n")
            if fname_log:
                basename_log = os.path.basename(fname_log)
                outfile.write(f"<a href='{basename_log}' target=_blank>[ {basename_log} ]</a>\n")
            dat_path = entry.get('dat')
            if dat_path:
                basename_dat = os.path.basename(dat_path)
                outfile.write(f"<a href='{basename_dat}' download>[ {basename_dat} ]</a>\n")
            outfile.write("</span><br>\n")
            # links to the pre-existing mcplot-html plots of the two
            # original monitors, if they were found on disk
            a_lin = _relhref(entry.get('a_lin'), outdir)
            a_log = _relhref(entry.get('a_log'), outdir)
            b_lin = _relhref(entry.get('b_lin'), outdir)
            b_log = _relhref(entry.get('b_log'), outdir)
            if a_lin or a_log or b_lin or b_log:
                outfile.write("<br><span class='origlinks'>\n")
            if a_lin:
                outfile.write(f"<a href='{a_lin}' target=_blank>[ A ]</a>\n")
            if a_log:
                outfile.write(f"<a href='{a_log}' target=_blank>[ A (log) ]</a>\n")
            if b_lin:
                outfile.write(f"<a href='{b_lin}' target=_blank>[ B ]</a>\n")
            if b_log:
                outfile.write(f"<a href='{b_log}' target=_blank>[ B (log) ]</a>\n")
            if a_lin or a_log or b_lin or b_log:
                outfile.write("</span>\n")
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

    # writing .dat/mccode.sim alongside the HTML plots is the default now
    # that the HTML pages link to them properly (see get_html()) - -D/--no-dat
    # opts back out, e.g. for a quick one-off comparison where the extra
    # files aren't wanted.
    write_dat = not args.no_dat

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

    label_a, label_b, used_fallback = default_labels(
        args.a, args.b,
        args.label_a[0] if args.label_a else None,
        args.label_b[0] if args.label_b else None)

    # determine output directory - deliberately built from the actual
    # input paths (dirsafe_name), not the display labels: label_a/label_b
    # may legitimately collapse to bare "A"/"B" when their basenames
    # collide (see default_labels()), which would otherwise put every such
    # comparison in the same "diff_A_vs_B" folder - a real problem for
    # batch/CI use running many comparisons out of one working directory.
    if args.output:
        outdir = args.output[0]
    else:
        outdir = "diff_%s_vs_%s" % (dirsafe_name(args.a), dirsafe_name(args.b))
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
        print('mcplotdiff loader: ' + e.__str__())
        sys.exit(-1)

    diffs = compute_diffs(monitors_a, monitors_b, label_a, label_b)

    if len(diffs) == 0:
        print("mcplotdiff: no matching monitors found between '%s' and '%s', nothing to plot." % (args.a, args.b))
        sys.exit(-1)

    # single monitor case: just write one page (or a pair, if --log), like
    # mcplot.py does for a single monitor file input
    single_input = os.path.isfile(args.a) and os.path.isfile(args.b)

    entries = []
    for data in diffs:
        dat_path = None
        if write_dat:
            dat_path = write_mccode_dat(data, outdir)
            print("Generated: %s" % dat_path)
        dat_basename = os.path.basename(dat_path) if dat_path else None

        f = plot_diff_single(data, outdir, False, dat_basename)
        f_log = None
        if single_input:
            if args.log:
                f_log = plot_diff_single(data, outdir, True, dat_basename)
        else:
            # folder mode: always produce both linear and log variants,
            # exactly like mcplot.py does for multi-monitor overviews
            f_log = plot_diff_single(data, outdir, True, dat_basename)

        # locate any pre-existing mcplot-html plots for this monitor, so
        # we can link to the original a/b data alongside the diff plot
        a_lin, a_log = find_original_plot(dir_a, data.filename)
        b_lin, b_log = find_original_plot(dir_b, data.filename)
        if not (a_lin or a_log):
            print("Note: no existing mcplot-html output found for '%s' in '%s'" % (data.filename, args.a))
        if not (b_lin or b_log):
            print("Note: no existing mcplot-html output found for '%s' in '%s'" % (data.filename, args.b))

        entries.append({
            'diff': f, 'diff_log': f_log, 'dat': dat_path,
            'a_lin': a_lin, 'a_log': a_log,
            'b_lin': b_lin, 'b_log': b_log,
        })
        print("Generated: %s" % f)
        if f_log:
            print("Generated: %s" % f_log)

    if single_input and len(entries) == 1:
        target = entries[0]['diff']
        print("Generated: %s" % target)
        if not args.nobrowse:
            browse(target)
        return

    if write_dat:
        # A mccode.sim index only makes sense for a multi-monitor output
        # directory (a lone diff .dat file has no "simulation directory"
        # to index) - lets `outdir` be opened directly as a proper McCode
        # simulation directory (e.g. mcplot-html <outdir>/), via the
        # standard mccode.sim-indexed loading path, rather than only via
        # the folder-of-loose-files fallback.
        sim_path = write_mccode_sim(diffs, outdir, label_a=label_a, label_b=label_b,
                                     instrument='diff_%s_vs_%s' % (label_a, label_b))
        print("Generated: %s" % sim_path)

    index_file = write_index(outdir, entries, label_a, label_b,
                              path_a=args.a, path_b=args.b, used_fallback=used_fallback)
    print("Generated: %s" % index_file)

    if not args.nobrowse:
        browse(index_file)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('a', help='first simulation file or directory (the minuend, "a")')
    parser.add_argument('b', help='second simulation file or directory (the subtrahend, "b"); diff = a - b')
    parser.add_argument('-n', '--nobrowse', action='store_true', help='do not open a webbrowser viewer')
    parser.add_argument('-D', '--no-dat', dest='no_dat', action='store_true',
                         help='do not write each difference dataset out in standard McCode ASCII '
                              '("# comment header + data body") format (default: written alongside '
                              'the HTML plots, along with a mccode.sim index in folder mode, so the '
                              'output directory can be reopened as a normal McCode simulation directory '
                              'by mcplot-html/-pyqtgraph/-matplotlib or any other McCode-format reader)')
    parser.add_argument('-l', '--log', action='store_true',
                         help='also produce a log-scale plot when comparing two single monitor files '
                              '(folder-mode always produces both linear and log-style plots)')
    parser.add_argument('--autosize', action='store_true', help='expand to window size on load')
    parser.add_argument('--libpath', nargs='*', help='js lib files path')
    parser.add_argument('-o', '--output', nargs=1, help='specify output directory for the generated plots')
    parser.add_argument('-A', '--label-a', nargs=1, help='short label used for simulation a in titles/filenames')
    parser.add_argument('-B', '--label-b', nargs=1, help='short label used for simulation b in titles/filenames')
    parser.add_argument('-W', '--width', nargs=1, help='width of iframes')
    parser.add_argument('-H', '--height', nargs=1, help='height of iframes')
    args = parser.parse_args()

    main(args)
