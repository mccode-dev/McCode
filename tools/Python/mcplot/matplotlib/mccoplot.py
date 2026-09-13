#!/usr/bin/env python3
'''
matplotlib mccoplot frontend.

This is a companion to mcplotdiff.py (mcplot-diff-matplotlib), sharing
part of its command-line syntax and monitor-matching logic. Instead of
computing and plotting the *difference* between two simulation results,
mccoplot.py overlays any number (2 or more) of datasets on the same axes,
for direct visual comparison of curve shape and position across all of
them at once, rather than the difference tool's pairwise "how big is the
gap". Unlike the diff tools (which are deliberately staying two-way only),
this is an interactive, end-user comparison tool - typically used with a
handful (2-8 or so) of related runs, e.g. sweeping a parameter or comparing
several code versions/platforms against each other directly, not against
one designated reference.

Only 1D monitors are supported: datasets are matched by output filename via
mccodelib.mcplotdiffloader.match_monitors_multi() (shared with the
mcplot-coplot-html/-pyqtgraph frontends), and any monitor that isn't a
matching 1D monitor across *every* dataset is skipped with a warning -
overlaying more than two 2D images doesn't have an equally natural
single-plot representation, so that case is left to the (two-way) diff
tools instead.

Rendering reuses plotfuncs.py's panel-layout helpers (figure sizing, panel
grid math, font scaling, title wrapping) - the same ones mcplot.py and
mcplotdiff.py use - but with its own drawing/driver code, since a co-plot
panel overlays N curves rather than showing one Data1D/Data2D from a plot
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


def _legend_letters(n):
    """ 'A', 'B', 'C', ... - the same compact, purely positional legend
        markers the html/pyqtgraph co-plot tools use, deliberately
        independent of `labels` itself: resolve_labels() now defaults to
        each dataset's full input path when the auto-derived short names
        would otherwise collide, which is exactly the information a
        legend entry has no room for - keeping the legend on fixed,
        compact letters regardless of how long the real labels are is
        what avoids a repeat of the earlier issue where legend content
        grew unpredictably. """
    import string
    letters = string.ascii_uppercase
    if n <= len(letters):
        return list(letters[:n])
    return ['S%d' % i for i in range(n)]


def _build_verbose_title_lines(datas, labels):
    ''' The per-line breakdown of a single-monitor co-plot's verbose title:
        component/filename, the monitor's own title, then one
        "letter=label: I=... Err=... N=...; statistics" line per
        co-plotted dataset. Shared between _plot_coplot_panel()'s in-plot
        title and McCoplotPlotter._render()'s separate title side-window
        (see show_text_window() in plotfuncs.py), so a long dataset list's
        title text can be moved out of the main plot entirely (via
        --no-titles) without the information simply being lost. '''
    d0 = datas[0]
    try:
        letters = _legend_letters(len(datas))
        lines = ['%s [%s]' % (d0.component, d0.filename), d0.title]
        for data, letter, label in zip(datas, letters, labels):
            lines.append('%s=%s: I=%s Err=%s N=%s; %s' % (
                letter, label, data.values[0], data.values[1], data.values[2], data.statistics))
        return lines
    except Exception:
        return ['%s [%s]' % (d0.component, d0.filename)]


def _plot_coplot_panel(datas, labels, colours, i, n, log, no_legends=False, no_titles=False):
    ''' plot one overlaid N-dataset group into subplot i of n '''
    dims = plotfuncs._calc_panel_size(n)
    subplt = plotfuncs.pylab.subplot(dims[1], dims[0], i + 1)

    fontsize = plotfuncs._panel_fontsize(n)
    title_fontsize = plotfuncs._title_fontsize(n)
    verbose = (n == 1)

    d0 = datas[0]
    xmin = d0.xlimits[0]
    xmax = d0.xlimits[1]
    plotfuncs.pylab.xlim(xmin, xmax)

    ylabel = d0.ylabel
    series = []  # (x, y, yerr) per dataset, post log-transform if needed
    for data in datas:
        x = np.array(data.xvals).astype(float)
        y = np.array(data.yvals).astype(float)
        yerr = np.array(data.y_err_vals).astype(float)
        if log:
            y = y.copy()
            invalid = np.where(y <= 0)
            valid = np.where(y > 0)
            if len(valid[0]):
                min_valid = np.min(y[valid])
                y[invalid] = min_valid / 10
            yerr = yerr / y
            y = np.log(y)
        series.append((x, y, yerr))
    if log:
        ylabel = "log(" + d0.ylabel + ")"

    for (x, y, yerr), letter, colour in zip(series, _legend_letters(len(datas)), colours):
        plotfuncs.pylab.errorbar(x, y, yerr, color=colour, label=letter)

    plotfuncs.pylab.xlabel(d0.xlabel, fontsize=fontsize, fontweight='bold')
    plotfuncs.pylab.ylabel(ylabel, fontsize=fontsize, fontweight='bold')

    # short title in an overview grid, fuller detail (matching d0.title and
    # every dataset's identity/I/statistics) once drilled down to a single
    # panel - the same "verbose only at n==1" convention
    # plotfuncs.plot_single_data uses for ordinary (non-coplot) monitors.
    # Skipped entirely (no pylab.title() call at all, not just an empty
    # one) when no_titles is set - for a long list of co-plotted datasets
    # the verbose form in particular grows by one line per dataset, and at
    # 20+ datasets can end up taking more vertical space than the actual
    # plot itself.
    if not no_titles:
        if verbose:
            title = '\n'.join(_build_verbose_title_lines(datas, labels))
        else:
            title = '%s [%s]' % (d0.component, d0.filename)
        title = plotfuncs._wrap_title(title, plotfuncs._title_wrap_width(n, title_fontsize))
        plotfuncs.pylab.title(title, fontsize=title_fontsize, fontweight='bold')

    # Legend similarly skipped entirely (not just made invisible) when
    # no_legends is set - with many co-plotted datasets the legend box
    # itself (one row per dataset) can grow tall enough to cover a large
    # fraction of the panel, which is exactly the "can't see the actual
    # plots" problem this option exists to avoid.
    if not no_legends:
        leg = plotfuncs.pylab.legend(loc='upper left', fontsize=title_fontsize, framealpha=0.85,
                                      handlelength=1.2, handletextpad=0.5, borderpad=0.4)
        leg.set_draggable(True)

    return subplt


class McCoplotPlotter():
    ''' Matplotlib co-plot frontend: renders a grid of overlaid N-dataset
        monitor groups, with the same overview <-> single-panel navigation
        as ordinary mcplot-matplotlib/mcplotdiff-matplotlib (click a panel
        to view it full-size; right-click, 'b', or back-navigate to
        return), reusing plotfuncs.py's generic click()/keypress()/
        print_help()/dumpfile() directly - none of those depend on there
        being an actual plot graph, they just need a list of "click_cbs"
        (one per currently-visible panel) and a "back_cb". Since a co-plot
        group has no further level of detail beyond the single-panel view
        (unlike an ordinary monitor's plot graph, which can have further
        primaries/secondaries to sweep through), click_cbs is simply empty
        once drilled down - only the back-navigation remains active. '''

    def __init__(self, pairs, labels, colours, log, identity_note=None, no_legends=False, no_titles=False):
        self.pairs = pairs  # [(key, [data_0, ..., data_N-1]), ...]
        self.labels = labels
        self.colours = colours
        self.log = log
        self.identity_note = identity_note
        self.no_legends = no_legends
        self.no_titles = no_titles
        self.current = None  # None = overview grid; else index into self.pairs
        self.event_dc_cid = None
        # Separate side windows (see _render()) shown only for the
        # single-monitor drill-down (n==1) view, when --no-titles/
        # --no-legends have freed up the main plot but the information
        # itself shouldn't just disappear. Tracked explicitly (rather than
        # relying on matplotlib's own "current figure") so they can be
        # closed by reference on every re-render, including when
        # navigating away from the single-monitor view entirely.
        self.title_window = None
        self.legend_window = None

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

    def _close_side_windows(self):
        ''' Closes any previously-open title/legend side windows by direct
            reference (see __init__'s note on why not via matplotlib's own
            "current figure") - called at the start of every _render(), so
            a stale side window from a previous single-monitor view never
            lingers after navigating to a different monitor or back to the
            overview grid. '''
        for attr in ('title_window', 'legend_window'):
            fig = getattr(self, attr)
            if fig is not None:
                try:
                    plotfuncs.pylab.close(fig)
                except Exception:
                    pass
                setattr(self, attr, None)

    def _render(self, side_windows=True):
        self._close_side_windows()
        visible = self._visible_pairs()
        n = len(visible)
        fig_w, fig_h = plotfuncs._figure_size(n)
        fig = plotfuncs.pylab.figure(figsize=(fig_w, fig_h))

        self.subplts = [
            _plot_coplot_panel(datas, self.labels, self.colours, i, n, self.log,
                                no_legends=self.no_legends, no_titles=self.no_titles)
            for i, (key, datas) in enumerate(visible)
        ]

        if n == 1:
            if self.no_titles:
                # No title text is drawn at all in this case (see
                # _plot_coplot_panel()), so there's nothing to reserve
                # extra vertical space for - use the same generous margin
                # as the overview grid case below, rather than the
                # dataset-count-scaled one a few lines down (which exists
                # specifically to make room for the verbose title that
                # isn't being drawn here).
                top = 0.97
            else:
                # Single-panel drill-down: the verbose title now includes
                # one letter=label identity line per dataset (in addition
                # to the existing component/filename/monitor-title/stats
                # lines), so its height grows with how many datasets are
                # co-plotted - a fixed top margin tuned for the old,
                # shorter title clipped the top lines off the figure
                # entirely once there were more than a couple of datasets.
                # Scale the margin down (more headroom) as N grows; this is
                # a rough heuristic, matching the established style of
                # _title_wrap_width()'s own "good enough" character-width
                # estimate rather than exact text measurement.
                n_datasets = len(visible[0][1])
                top = max(0.45, 0.95 - 0.045 * n_datasets)
            fig.subplots_adjust(left=0.06, right=0.98, top=top, bottom=0.06,
                                 wspace=0.3, hspace=0.35)
        else:
            fig.subplots_adjust(left=0.06, right=0.98, top=0.97, bottom=0.06,
                                 wspace=0.3, hspace=0.35)

        if self.identity_note and n > 1 and not self.no_titles:
            # The legend now always uses compact positional letters
            # (_legend_letters()), never the real labels directly (which
            # may now be a full input path - see resolve_labels()) - shown
            # once as a figure-level title (matplotlib's actual "figure
            # title" concept, distinct from each panel's own title) rather
            # than repeated inside every panel's legend, which would get
            # cluttered across a multi-panel overview.
            #
            # Only in the overview grid (n > 1), not the single-panel
            # drill-down view: at n==1, _plot_coplot_panel()'s own verbose
            # title already spells out the full letter=label mapping (plus
            # per-dataset stats) inside the panel itself, so a suptitle
            # here would be a redundant second copy of the same text - and,
            # with both competing for the same cramped space above a
            # single (2x-scaled) panel, they visibly overlapped.
            suptitle_fontsize = 9
            usable_w_in = fig_w * 0.95
            avg_char_width_in = (suptitle_fontsize * 0.6) / 72.0
            wrap_width = max(20, int(usable_w_in / avg_char_width_in))
            wrapped_note = plotfuncs.textwrap.fill(self.identity_note, width=wrap_width)
            fig.suptitle(wrapped_note, fontsize=suptitle_fontsize, y=0.998, va='top')
            fig.subplots_adjust(top=0.92 if '\n' not in wrapped_note else 0.88)

        # Left-click on a panel drills into it (only meaningful in overview
        # mode - once already on a single panel there's nowhere further to
        # go, so click_cbs is left empty and only right-click/'b' back-
        # navigation remains live).
        if self.current is None:
            self.click_cbs = [lambda idx=i: self._show_single(idx) for i in range(n)]
        else:
            self.click_cbs = []

        # Single-monitor view only ("not an overview-plot", per the actual
        # request this responds to): when --no-titles/--no-legends have
        # freed up the main plot, open a separate, full-screen-height,
        # narrow side window for each piece of information that was
        # removed, rather than losing it outright. side_windows=False (see
        # html()) skips this entirely for the non-interactive mpld3 export
        # path, where a GUI side window would serve no purpose and could
        # even fail outright in a headless export context.
        if side_windows and n == 1:
            datas = visible[0][1]
            if self.no_titles:
                lines = [(line, None) for line in _build_verbose_title_lines(datas, self.labels)]
                self.title_window = plotfuncs.show_text_window(lines, 'mccoplot: title')
            if self.no_legends:
                letters = _legend_letters(len(datas))
                legend_lines = [('%s = %s' % (letter, label), colour)
                                 for letter, label, colour in zip(letters, self.labels, self.colours)]
                self.legend_window = plotfuncs.show_text_window(legend_lines, 'mccoplot: legend')
            # Side windows created above (via plotfuncs.pylab.figure())
            # become matplotlib's new "current figure" - restore the main
            # plot figure as current before returning, since _replot()'s
            # own pylab.close() (no arguments - see its docstring) closes
            # whatever figure is current at the time, and needs that to
            # still be THIS figure on the next call, not a leftover side
            # window.
            if self.title_window is not None or self.legend_window is not None:
                plotfuncs.pylab.figure(fig.number)

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
        self._render(side_windows=False)
        mpld3.save_html(plotfuncs.pylab.gcf(), fileobj)


def main(args):
    ''' load and match N simulation results' 1D monitors, then hand the
        resulting groups to the matplotlib co-plot frontend above. '''
    logging.basicConfig(level=logging.INFO)

    # ensure keyboardinterrupt ctr-c
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    try:
        paths = args.datasets
        if len(paths) < 2:
            print("mccoplot: need at least 2 datasets to co-plot, got %d" % len(paths))
            quit()

        given_labels = args.labels[0].split(',') if args.labels else [None] * len(paths)
        if len(given_labels) != len(paths):
            print("mccoplot: --labels has %d entries but %d datasets were given"
                  % (len(given_labels), len(paths)))
            quit()
        given_labels = [l if l else None for l in given_labels]

        given_colours = args.colours[0].split(',') if args.colours else None

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

        labels, used_fallback = diffloader.resolve_labels(paths, given_labels)
        colours = diffloader.resolve_colours(len(paths), given_colours)

        # Shown as a figure-level suptitle: the legend itself now always
        # uses compact positional letters (_legend_letters()), never
        # `labels` directly, so this "A=<label>" mapping is the only place
        # (in the overview grid at least) that ties a legend entry back to
        # which dataset it actually is - needed unconditionally now, not
        # just when resolve_labels() had to fall back to full paths.
        letters = _legend_letters(len(paths))
        identity_note = "   ".join("%s=%s" % (letter, lbl) for letter, lbl in zip(letters, labels))

        monitors_list = []
        try:
            for p in paths:
                monitors, _ = diffloader.load_monitors(p)
                monitors_list.append(monitors)
        except Exception as e:
            print('mccoplot loader: ' + e.__str__())
            plotfuncs.print_help(nogui=True)
            quit()

        pairs = diffloader.match_monitors_multi(monitors_list, labels)

        if len(pairs) == 0:
            print("mccoplot: no matching 1D monitors found across all %d datasets, nothing to plot."
                  % len(paths))
            quit()

        if args.test:
            print("mccoplot: %d matched 1D monitor group(s) across %d datasets:" % (len(pairs), len(paths)))
            for key, datas in pairs:
                print("  - %s (%s)" % (key, datas[0].component))

        # default base name for --format/--output dumps and for --html,
        # since there's no single simulation file to derive it from -
        # deliberately built from the actual input paths (dirsafe_name),
        # not the display labels: those may legitimately collapse to bare
        # letters when their basenames collide (see resolve_labels()),
        # which would otherwise make every such comparison overwrite the
        # same "coplot_A_vs_B....*" files - a real problem for batch/CI
        # use running many comparisons out of one working directory.
        plotfuncs.filenamebase = "coplot_" + "_vs_".join(diffloader.dirsafe_name(p) for p in paths)

        plotter = McCoplotPlotter(pairs, labels, colours, log=args.log, identity_note=identity_note,
                                   no_legends=args.no_legends, no_titles=args.no_titles)

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
    parser.add_argument('datasets', nargs='+',
                         help='2 or more simulation files or directories to co-plot together '
                              '(e.g. "mccoplot run_a run_b run_c")')
    parser.add_argument('-L', '--labels', nargs=1,
                         help='comma-separated short labels, one per dataset, in the same order '
                              '(e.g. --labels "RunA,RunB,RunC"); default: derived from each path')
    parser.add_argument('-C', '--colours', nargs=1,
                         help='comma-separated overlay colours, one per dataset, in the same order; '
                              'default: %s' % ', '.join(diffloader.DEFAULT_PALETTE))
    parser.add_argument('-t', '--test', action='store_true', default=False, help='print the matched monitor groups before plotting')
    parser.add_argument('--html', action='store_true', help='save plot to html using mpld3 (linux only)')
    parser.add_argument('--format', dest='format', help='save plot to pdf/png/eps/svg... without bringing up window')
    parser.add_argument('--output', nargs=1, dest='output', default=None,
                         help='save plot to given file without bringing up window. Extension '
                              '(e.g. pdf/png/eps/svg) can be specified in the file name or --format')
    parser.add_argument('--log', action='store_true', help='initiate plot(s) with log of signal')
    parser.add_argument('--backend', dest='backend', help='use non-default backend for matplotlib plot')
    parser.add_argument('--no-legends', action='store_true', default=False,
                         help='do not draw the per-panel legend (the compact A/B/C/... letters) - '
                              'useful with many co-plotted datasets, where the legend box itself can '
                              'grow tall enough to cover a large part of the panel')
    parser.add_argument('--no-titles', action='store_true', default=False,
                         help='do not draw panel titles or the figure-level dataset identity note - '
                              'useful with many co-plotted datasets, where the verbose single-panel '
                              'title (one line per dataset) can end up taller than the plot itself')

    args = parser.parse_args()

    mccode_config.load_config("user")

    main(args)
