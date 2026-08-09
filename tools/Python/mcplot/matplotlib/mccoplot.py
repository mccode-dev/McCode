#!/usr/bin/env python3
'''
matplotlib mccoplot frontend.

This is a companion to mcplotdiff.py (mcplot-diff-matplotlib), sharing its
command-line syntax and monitor-matching logic. Instead of computing and
plotting the *difference* between two simulation results, mccoplot.py
overlays the two datasets on the same axes, for direct visual comparison of
curve shape and position rather than the size of the gap between them.

Only 1D monitors are supported: a and b are matched by output filename via
mccodelib.mcplotdiffloader.match_1d_monitors() (shared with the
mcplot-coplot-html frontend), and any matched pair that isn't a 1D/1D match
is skipped with a warning - overlaying two 2D images doesn't have an
equally natural single-plot representation, so that case is left to the
diff tools instead.

Rendering reuses plotfuncs.py's panel-layout helpers (figure sizing, panel
grid math, font scaling, title wrapping) - the same ones mcplot.py and
mcplotdiff.py use - but with its own drawing/driver code, since a co-plot
panel overlays two curves rather than showing one Data1D/Data2D from a plot
graph. There's deliberately no click-based drill-down here (unlike ordinary
mcplot-matplotlib): a co-plot panel is already the finest level of detail
on offer, so there's nothing further to navigate into. Keyboard shortcuts
(log toggle, save png/pdf/svg/jpg, quit, help) are otherwise identical,
reusing plotfuncs.py's keypress()/print_help()/dumpfile() directly.
'''
import argparse
import logging
import os
import sys
import matplotlib
import numpy as np

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

import plotfuncs

from mccodelib import mcplotdiffloader as diffloader
from mccodelib import mccode_config

# Default overlay colours (a, b): a colourblind-friendly blue/red pair.
COLOUR_A = '#1f77b4'
COLOUR_B = '#d62728'


def _plot_coplot_panel(data_a, data_b, i, n, log, label_a, label_b, colour_a, colour_b):
    ''' plot one overlaid (a, b) pair into subplot i of n '''
    dims = plotfuncs._calc_panel_size(n)
    subplt = plotfuncs.pylab.subplot(dims[1], dims[0], i + 1)

    fontsize = plotfuncs._panel_fontsize(n)
    title_fontsize = plotfuncs._title_fontsize(n)
    verbose = (n == 1)

    xmin = data_a.xlimits[0]
    xmax = data_a.xlimits[1]
    plotfuncs.pylab.xlim(xmin, xmax)

    x = np.array(data_a.xvals).astype(float)
    ya = np.array(data_a.yvals).astype(float)
    yaerr = np.array(data_a.y_err_vals).astype(float)
    yb = np.array(data_b.yvals).astype(float)
    yberr = np.array(data_b.y_err_vals).astype(float)

    ylabel = data_a.ylabel
    if log:
        def _tolog(y, yerr):
            y = y.copy()
            invalid = np.where(y <= 0)
            valid = np.where(y > 0)
            if len(valid[0]):
                min_valid = np.min(y[valid])
                y[invalid] = min_valid / 10
            yerr = yerr / y
            return np.log(y), yerr
        ya, yaerr = _tolog(ya, yaerr)
        yb, yberr = _tolog(yb, yberr)
        ylabel = "log(" + data_a.ylabel + ")"

    plotfuncs.pylab.errorbar(x, ya, yaerr, color=colour_a, label=label_a)
    plotfuncs.pylab.errorbar(x, yb, yberr, color=colour_b, label=label_b)

    plotfuncs.pylab.xlabel(data_a.xlabel, fontsize=fontsize, fontweight='bold')
    plotfuncs.pylab.ylabel(ylabel, fontsize=fontsize, fontweight='bold')

    # short title in an overview grid, fuller detail (matching data_a.title
    # and both sides' I/statistics) once drilled down to a single panel -
    # the same "verbose only at n==1" convention plotfuncs.plot_single_data
    # uses for ordinary (non-coplot) monitors.
    if verbose:
        try:
            title = '%s [%s]\n%s\n%s: I=%s Err=%s N=%s; %s\n%s: I=%s Err=%s N=%s; %s' % (
                data_a.component, data_a.filename, data_a.title,
                label_a, data_a.values[0], data_a.values[1], data_a.values[2], data_a.statistics,
                label_b, data_b.values[0], data_b.values[1], data_b.values[2], data_b.statistics)
        except Exception:
            title = '%s [%s]' % (data_a.component, data_a.filename)
    else:
        title = '%s [%s]' % (data_a.component, data_a.filename)
    title = plotfuncs._wrap_title(title, plotfuncs._title_wrap_width(n, title_fontsize))
    plotfuncs.pylab.title(title, fontsize=title_fontsize, fontweight='bold')

    leg = plotfuncs.pylab.legend(loc='upper left', fontsize=title_fontsize, framealpha=0.85,
                                  handlelength=1.2, handletextpad=0.5, borderpad=0.4)
    leg.set_draggable(True)

    return subplt


class McCoplotPlotter():
    ''' Matplotlib co-plot frontend: renders a grid of overlaid (a, b) 1D
        monitor pairs, with the same overview <-> single-panel navigation
        as ordinary mcplot-matplotlib/mcplotdiff-matplotlib (click a panel
        to view it full-size; right-click, 'b', or back-navigate to
        return), reusing plotfuncs.py's generic click()/keypress()/
        print_help()/dumpfile() directly - none of those depend on there
        being an actual plot graph, they just need a list of "click_cbs"
        (one per currently-visible panel) and a "back_cb". Since a co-plot
        pair has no further level of detail beyond the single-panel view
        (unlike an ordinary monitor's plot graph, which can have further
        primaries/secondaries to sweep through), click_cbs is simply empty
        once drilled down - only the back-navigation remains active. '''

    def __init__(self, pairs, label_a, label_b, colour_a, colour_b, log, path_note=None):
        self.pairs = pairs
        self.label_a = label_a
        self.label_b = label_b
        self.colour_a = colour_a
        self.colour_b = colour_b
        self.log = log
        self.path_note = path_note
        self.current = None  # None = overview grid; else index into self.pairs
        self.event_dc_cid = None

    def _flip_log(self):
        self.log = not self.log

    def _visible_pairs(self):
        if self.current is None:
            return self.pairs
        return [self.pairs[self.current]]

    def _show_overview(self):
        self.current = None
        self._replot()

    def _show_single(self, idx):
        self.current = idx
        self._replot()

    def _click_proxy(self, event):
        ''' state-updating proxy for plotfuncs.click(), mirroring
            McMatplotlibPlotter._click_proxy() '''
        dc_cb = lambda: plotfuncs.pylab.disconnect(self.event_dc_cid)
        plotfuncs.click(event, subplts=self.subplts, click_cbs=self.click_cbs,
                         ctrl_cbs=[], back_cb=self._show_overview, dc_cb=dc_cb)

    def _keypress_proxy(self, event):
        plotfuncs.keypress(event, back_cb=self._show_overview, replot_cb=self._replot,
                            togglelog_cb=self._flip_log)

    def _render(self):
        visible = self._visible_pairs()
        n = len(visible)
        fig_w, fig_h = plotfuncs._figure_size(n)
        fig = plotfuncs.pylab.figure(figsize=(fig_w, fig_h))

        self.subplts = [
            _plot_coplot_panel(data_a, data_b, i, n, self.log,
                                self.label_a, self.label_b, self.colour_a, self.colour_b)
            for i, (key, data_a, data_b) in enumerate(visible)
        ]

        fig.subplots_adjust(left=0.06, right=0.98, top=0.97, bottom=0.06,
                             wspace=0.3, hspace=0.35)

        if self.path_note:
            # label_a/label_b are bare "A"/"B" here (see default_labels()),
            # since the two source paths' basenames collided (e.g. both
            # ended in ".../<instrument>/1/") - shown once as a figure-
            # level title (matplotlib's actual "figure title" concept,
            # distinct from each panel's own title) rather than repeated
            # inside every panel, which would get cluttered across a
            # multi-panel overview.
            fig.suptitle(self.path_note, fontsize=9, y=0.998, va='top')
            fig.subplots_adjust(top=0.90 if n == 1 else 0.92)

        # Left-click on a panel drills into it (only meaningful in overview
        # mode - once already on a single panel there's nowhere further to
        # go, so click_cbs is left empty and only right-click/'b' back-
        # navigation remains live).
        if self.current is None:
            self.click_cbs = [lambda idx=i: self._show_single(idx) for i in range(n)]
        else:
            self.click_cbs = []

        return fig

    def _replot(self):
        ''' Re-renders from scratch and re-enters the GUI event loop.

            pylab.show() must be called again here, not just once from
            plot() below - the SAME pattern plotfuncs.McMatplotlibPlotter.
            plot_node() already uses for every click/keypress-triggered
            redraw. Without it: _render() calls pylab.close() (destroying
            the figure the original blocking show() call was watching) and
            then creates a brand new figure via pylab.figure() - but with
            no show() call for that new figure, many backends' blocking
            event loop exits as soon as the figure count momentarily hits
            zero, so the process falls straight through the original
            show() and the new figure is never actually displayed. The
            visible symptom is exactly what it looks like: any click
            (drilling into a panel, going back, or even just toggling log
            via 'l') appears to close the window outright. '''
        plotfuncs.pylab.close()
        self._render()
        self.event_dc_cid = plotfuncs.pylab.connect('button_press_event', self._click_proxy)
        plotfuncs.pylab.connect('key_press_event', self._keypress_proxy)
        plotfuncs.pylab.show()

    def plot(self):
        ''' render and show the interactive grid '''
        self._replot()

    def html(self, fileobj):
        ''' render and save to html using mpld3 '''
        import mpld3
        plotfuncs.pylab.close()
        self._render()
        mpld3.save_html(plotfuncs.pylab.gcf(), fileobj)


def main(args):
    ''' load and match two simulation results' 1D monitors, then hand the
        resulting pairs to the matplotlib co-plot frontend above. '''
    logging.basicConfig(level=logging.INFO)

    # ensure keyboardinterrupt ctr-c
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    try:
        label_a = args.label_a[0] if args.label_a else None
        label_b = args.label_b[0] if args.label_b else None
        colour_a = args.colour_a[0] if args.colour_a else COLOUR_A
        colour_b = args.colour_b[0] if args.colour_b else COLOUR_B

        if args.format or args.output:
            matplotlib.use('template')

        if args.backend:
            try:
                matplotlib.use(args.backend)
            except Exception as e:
                print('backend use error: ' + e.__str__())
                return

        from matplotlib import pylab
        plotfuncs.pylab = pylab

        label_a, label_b, used_fallback = diffloader.default_labels(args.a, args.b, label_a, label_b)

        # When the auto-derived labels collided (e.g. two runs both ending
        # in a plain ".../<instrument>/1/" folder) and default_labels()
        # fell back to bare "A"/"B", those letters carry no identifying
        # information on their own - show the full source paths as a
        # figure-level suptitle instead, while the per-panel legend keeps
        # just "A"/"B".
        path_note = None
        if used_fallback:
            path_note = "A: %s   B: %s" % (args.a, args.b)

        try:
            monitors_a, dir_a = diffloader.load_monitors(args.a)
            monitors_b, dir_b = diffloader.load_monitors(args.b)
        except Exception as e:
            print('mccoplot loader: ' + e.__str__())
            plotfuncs.print_help(nogui=True)
            quit()

        pairs = diffloader.match_1d_monitors(monitors_a, monitors_b, label_a, label_b)

        if len(pairs) == 0:
            print("mccoplot: no matching 1D monitors found between '%s' and '%s', nothing to plot."
                  % (args.a, args.b))
            quit()

        if args.test:
            print("mccoplot: %d matched 1D monitor pair(s):" % len(pairs))
            for key, data_a, data_b in pairs:
                print("  - %s (%s)" % (key, data_a.component))

        # default base name for --format/--output dumps and for --html,
        # since there's no single simulation file to derive it from -
        # deliberately built from the actual input paths (dirsafe_name),
        # not label_a/label_b: those may legitimately collapse to bare
        # "A"/"B" when their basenames collide (see default_labels()),
        # which would otherwise make every such comparison overwrite the
        # same "coplot_A_vs_B.*" files - a real problem for batch/CI use
        # running many comparisons out of one working directory.
        plotfuncs.filenamebase = "coplot_%s_vs_%s" % (
            diffloader.dirsafe_name(args.a), diffloader.dirsafe_name(args.b))

        plotter = McCoplotPlotter(pairs, label_a, label_b, colour_a, colour_b, log=args.log, path_note=path_note)

        if (sys.platform == "linux" or sys.platform == "linux2") and args.html:
            # save to html and exit
            plotter.html(open('%s.html' % plotfuncs.filenamebase, 'w'))
        else:
            # display gui / prepare graphics dump
            plotfuncs.print_help(nogui=True)
            plotter.plot()

        if args.output or args.format:
            try:
                plotfuncs.dumpfile(args.format, args.output)
            except Exception as e:
                print('dumpfile issue: ' + e.__str__())

    except KeyboardInterrupt:
        print('keyboard interrupt')
    except Exception as e:
        print('mccoplot error: %s' % e.__str__())
        raise e


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('a', help='first simulation file or directory')
    parser.add_argument('b', help='second simulation file or directory, co-plotted alongside "a"')
    parser.add_argument('-A', '--label-a', nargs=1, help='short label used for simulation a in the legend/titles')
    parser.add_argument('-B', '--label-b', nargs=1, help='short label used for simulation b in the legend/titles')
    parser.add_argument('--colour-a', nargs=1, help='override the overlay colour used for a (default %s)' % COLOUR_A)
    parser.add_argument('--colour-b', nargs=1, help='override the overlay colour used for b (default %s)' % COLOUR_B)
    parser.add_argument('-t', '--test', action='store_true', default=False, help='print the matched monitor pairs before plotting')
    parser.add_argument('--html', action='store_true', help='save plot to html using mpld3 (linux only)')
    parser.add_argument('--format', dest='format', help='save plot to pdf/png/eps/svg... without bringing up window')
    parser.add_argument('--output', nargs=1, dest='output', default=None,
                         help='save plot to given file without bringing up window. Extension '
                              '(e.g. pdf/png/eps/svg) can be specified in the file name or --format')
    parser.add_argument('--log', action='store_true', help='initiate plot(s) with log of signal')
    parser.add_argument('--backend', dest='backend', help='use non-default backend for matplotlib plot')

    args = parser.parse_args()

    mccode_config.load_config("user")

    main(args)
