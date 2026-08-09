#!/usr/bin/env python3
'''
pyqtgraph mccoplot frontend.

This is a companion to mcplotdiff.py (mcplot-diff-pyqtgraph), sharing its
command-line syntax and monitor-matching logic. Instead of computing and
plotting the *difference* between two simulation results, mccoplot.py
overlays the two datasets on the same axes, for direct visual comparison of
curve shape and position rather than the size of the gap between them.

Only 1D monitors are supported: a and b are matched by output filename via
mccodelib.mcplotdiffloader.match_1d_monitors() (shared with the
mcplot-coplot-html and mcplot-coplot-matplotlib frontends), and any matched
pair that isn't a 1D/1D match is skipped with a warning.

Unlike ordinary mcplot-pyqtgraph (mccodelib.pqtgfrontend.McPyqtgraphPlotter),
there is no click-based drill-down here: a co-plot panel already overlays
everything comparable for that monitor, so there's nothing further to
navigate into. This script therefore builds its own minimal static-grid
window rather than reusing McPyqtgraphPlotter, though it follows the same
qtpy-based conventions (so the same code runs under PyQt5/PyQt6/PySide2/
PySide6) and reuses plotfuncs.ModLegend for the on-plot legend.
'''
import argparse
import logging
import os
import sys
import math

import numpy as np
import pyqtgraph as pg

# qtpy normalises Qt5/Qt6/PySide2/PySide6 into a single API, same as
# mccodelib.pqtgfrontend.
from qtpy import QtCore, QtWidgets

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

import plotfuncs

from mccodelib import mcplotdiffloader as diffloader
from mccodelib import mccode_config
from mccodelib import utils

_Qt = QtCore.Qt


# ---------------------------------------------------------------------------
# Helpers for modifier / button comparison that work across Qt5 and Qt6,
# copied from mccodelib.pqtgfrontend's convention (see there for details:
# under Qt5 Qt.ControlModifier is an int, under Qt6 it's an enum member;
# qtpy exposes them at the same attribute path, but comparing enum members
# to ints with == raises a TypeError in Qt6, hence normalising to int).
# ---------------------------------------------------------------------------

def _int_mod(mod):
    try:
        return int(mod)
    except (TypeError, ValueError):
        return mod.value


def _event_mods_int(event):
    try:
        return int(event.modifiers())
    except (TypeError, ValueError):
        return event.modifiers().value


def _event_button_int(event):
    try:
        return int(event.button())
    except (TypeError, ValueError):
        return event.button().value


_LEFT_BUTTON = _int_mod(_Qt.LeftButton)
_RIGHT_BUTTON = _int_mod(_Qt.RightButton)

# Default overlay colours (a, b): a colourblind-friendly blue/red pair.
COLOUR_A = '#1f77b4'
COLOUR_B = '#d62728'


def get_help_string():
    helplines = []
    helplines.append('')
    helplines.append('q              - quit')
    helplines.append('p              - save png')
    helplines.append('s              - save svg')
    helplines.append('l              - log toggle')
    helplines.append('F1/h           - help')
    helplines.append('click          - display single panel')
    helplines.append('right-click/b  - back to overview')
    return '\n'.join(helplines)


def plot_coplot_1D(data_a, data_b, plt, label_a, label_b, colour_a, colour_b,
                    log=False, legend=True, fontsize=10, verbose=False):
    ''' overlay data_a and data_b (both Data1D) into the pyqtgraph PlotItem plt '''
    x_a = np.array(data_a.xvals).astype(float)
    y_a = np.array(data_a.yvals).astype(float)
    e_a = np.array(data_a.y_err_vals).astype(float)
    x_b = np.array(data_b.xvals).astype(float)
    y_b = np.array(data_b.yvals).astype(float)
    e_b = np.array(data_b.y_err_vals).astype(float)

    if log:
        def _tolog(y):
            nonzeros = np.where(y > 0)[0]
            if len(nonzeros) > 0:
                y = y.copy()
                y[y <= 0] = np.min(y[nonzeros]) / 10
                return y, True
            return y, False
        y_a, ok_a = _tolog(y_a)
        y_b, ok_b = _tolog(y_b)
        plt.setLogMode(y=(ok_a or ok_b))
    else:
        plt.setLogMode(y=False)

    xmin = min(np.min(x_a), np.min(x_b))
    xmax = max(np.max(x_a), np.max(x_b))
    plt.setXRange(xmin, xmax, padding=0)

    try:
        header = '%s [%s]' % (data_a.component, data_a.filename)
        if verbose:
            header = '%s [%s]<br>%s' % (data_a.component, data_a.filename, data_a.title)
    except Exception:
        header = '%s' % data_a.component
    plt.setTitle(header)
    plt.getAxis('bottom').setLabel(data_a.xlabel)
    plt.getAxis('left').setLabel(data_a.ylabel)

    beam_a = (x_a[1] - x_a[0]) * 0.5 if len(x_a) > 1 else 0
    beam_b = (x_b[1] - x_b[0]) * 0.5 if len(x_b) > 1 else 0

    # TODO (same as plotfuncs.plot_Data1D): no error bars in log mode
    if not log:
        plt.addItem(pg.ErrorBarItem(x=x_a, y=y_a, height=e_a, beam=beam_a, pen=colour_a))
        plt.addItem(pg.ErrorBarItem(x=x_b, y=y_b, height=e_b, beam=beam_b, pen=colour_b))

    if legend:
        plt.legend = plotfuncs.ModLegend(offset=(-1, 1), text_size='%spt' % str(fontsize))
        plt.legend.setParentItem(plt.vb)

    # actual curves, plotted with an explicit pen per series - since
    # plt.legend is already set, plot(..., name=...) registers each curve
    # with the legend automatically, using the curve's own pen as the
    # swatch colour (no need for the "invisible dummy artist" trick
    # ordinary single-dataset plots use, since here both series are real
    # and worth a legend entry each). Both names are bare labels
    # ("A"/"B" or whatever was given) - deliberately symmetric, rather
    # than one side carrying the full component/filename/title block:
    # ModLegend lays each entry out in its own row sized to that row's
    # content, so a long multi-line entry next to a single short word
    # rendered visibly misaligned (the descriptive text now lives in the
    # panel's own title instead, via plt.setTitle() above).
    plt.plot(x_a, y_a, pen=colour_a, name=label_a)
    plt.plot(x_b, y_b, pen=colour_b, name=label_b)

    plt.setMenuEnabled(False)
    return plt.getViewBox()


class McCoplotPlotter():
    ''' pyqtgraph co-plot frontend: renders a grid of overlaid (a, b) 1D
        monitor pairs, with the same overview <-> single-panel navigation
        as ordinary mcplot-pyqtgraph (mccodelib.pqtgfrontend.McPyqtgraphPlotter):
        click a panel to view it full-size; right-click or 'b' to return.
        Since a co-plot pair has no further level of detail beyond the
        single-panel view (unlike an ordinary monitor's plot graph, which
        can have further primaries/secondaries to sweep through), a click
        on the single panel itself is a no-op - only the back-navigation
        is live there. '''

    def __init__(self, pairs, label_a, label_b, colour_a, colour_b, invcanvas=False, title=None,
                 path_note=None, filenamebase=None):
        self.pairs = pairs
        self.label_a = label_a
        self.label_b = label_b
        self.colour_a = colour_a
        self.colour_b = colour_b
        self.log = False
        self.path_note = path_note
        self.current = None  # None = overview grid; else index into self.pairs
        self.viewbox_list = []
        self.title = title if title is not None else ('coplot: %s vs %s' % (label_a, label_b))
        # deliberately NOT derived from label_a/label_b here: those may
        # legitimately collapse to bare "A"/"B" when their basenames
        # collide (see default_labels()), which would make every such
        # comparison overwrite the same "coplot_A_vs_B.*" export files -
        # a real problem for batch/CI use running many comparisons out of
        # one working directory. Callers should pass the dirsafe_name()-
        # based name explicitly; this fallback only exists for callers
        # that don't care (e.g. quick interactive use, direct construction).
        self.filenamebase = filenamebase if filenamebase is not None else ('coplot_%s_vs_%s' % (label_a, label_b))

        self.app = QtWidgets.QApplication.instance() or QtWidgets.QApplication(sys.argv)

        if invcanvas:
            pg.setConfigOption('background', 'w')
            pg.setConfigOption('foreground', 'k')

    def run(self):
        self._create_window()
        self._replot()

        exec_fn = getattr(self.app, "exec", None) or self.app.exec_
        sys.exit(exec_fn())

    def _create_window(self):
        helpmessage = QtWidgets.QLabel()
        helpmessage.setText("Press 'h' for app shortcuts.")

        statusbar = QtWidgets.QStatusBar()
        statusbar.addWidget(helpmessage)

        self.main_window = QtWidgets.QMainWindow()
        self.graphics_view = pg.GraphicsView(self.main_window)
        self.main_window.setCentralWidget(self.graphics_view)
        self.main_window.setStatusBar(statusbar)
        self.main_window.setWindowTitle(self.title)

        # Screen size - primaryScreen() is available in Qt 5.6+ and Qt 6
        screen = QtWidgets.QApplication.primaryScreen()
        if screen is not None:
            rect = screen.size()
        else:
            # Very old Qt5 fallback
            rect = QtWidgets.QApplication.desktop().screenGeometry()

        w = int(0.7 * rect.width())
        h = int(0.7 * rect.height())
        self.main_window.resize(w, h)

        self.plot_layout = pg.GraphicsLayout(border=None)
        self.graphics_view.setCentralItem(self.plot_layout)
        self.plot_layout.setContentsMargins(2, 2, 2, 2)

        self.main_window.show()
        self.main_window.raise_()
        self.main_window.activateWindow()

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

    def _render(self):
        self.plot_layout.clear()

        visible = self._visible_pairs()
        n = len(visible)
        rowlen = max(1, int(math.sqrt(n * 1.61803398875)))

        row_offset = 0
        if self.path_note:
            # label_a/label_b are bare "A"/"B" here (see default_labels()),
            # since the two source paths' basenames collided (e.g. both
            # ended in ".../<instrument>/1/") - shown here as an on-canvas
            # header row (rather than only in the window title, which
            # dumpfile_pqtg()'s scene export wouldn't capture) so the
            # disambiguation survives into saved PNG/SVG exports too.
            self.plot_layout.addLabel(self.path_note, row=0, col=0, colspan=max(rowlen, 1))
            row_offset = 1

        if n <= 2:
            fontsize = 14
        elif n <= 16:
            fontsize = 10
        else:
            fontsize = 8
        # verbose (fuller title/statistics) once drilled down to the single
        # panel, matching McPyqtgraphPlotter's own n<=4 overview threshold
        # for ordinary monitors, but here n==1 always means "single view"
        # rather than "a small overview", so use that as the trigger.
        verbose = (n == 1)

        self.viewbox_list = []
        for i, (key, data_a, data_b) in enumerate(visible):
            plt = pg.PlotItem()
            vb = plot_coplot_1D(data_a, data_b, plt, self.label_a, self.label_b,
                                 self.colour_a, self.colour_b, log=self.log,
                                 fontsize=fontsize, verbose=verbose)
            self.viewbox_list.append(vb)
            self.plot_layout.addItem(plt, row_offset + i // rowlen, i % rowlen)

    def _get_plot_index(self, pos):
        ''' Index of the viewbox containing scene-position pos, or -1.
            Copied from pqtgfrontend.McPyqtgraphPlotter.get_plot_index(). '''
        if not self.viewbox_list or pos is None:
            return -1
        for idx, viewbox in enumerate(self.viewbox_list):
            topRight = viewbox.mapViewToScene(viewbox.viewRect().topRight())
            bottomLeft = viewbox.mapViewToScene(viewbox.viewRect().bottomLeft())
            rect = QtCore.QRectF()
            rect.setTopRight(topRight)
            rect.setBottomLeft(bottomLeft)
            if rect.contains(pos):
                return idx
        return -1

    def _click_handler(self, event):
        if _event_mods_int(event) != 0:
            return  # no ctrl/alt-click concept for co-plot (no "sweep")

        btn = _event_button_int(event)
        try:
            idx = self._get_plot_index(event.scenePos())
        except AttributeError:
            return
        if idx < 0:
            return

        if btn == _LEFT_BUTTON and self.current is None:
            # only meaningful in overview mode - the single view has
            # nowhere further to drill into
            self._show_single(idx)
        elif btn == _RIGHT_BUTTON and self.current is not None:
            self._show_overview()

    def _show_help(self):
        prefix = "mc" if mccode_config.configuration["MCCODE"] == "mcstas" else "mx"
        QtWidgets.QMessageBox.about(self.main_window, prefix + 'coplot-pyqtgraph', get_help_string())

    def _set_handlers(self):
        try:
            self.plot_layout.scene().sigMouseClicked.disconnect()
        except TypeError:
            pass
        self.plot_layout.scene().sigMouseClicked.connect(self._click_handler)

        K = _Qt
        savefile_cb = lambda fmt: utils.dumpfile_pqtg(
            scene=self.plot_layout.scene(), filenamebase=self.filenamebase, format=fmt)

        def key_handler(ev):
            key = ev.key()
            if key == K.Key_Q:
                QtWidgets.QApplication.quit()
            elif key == K.Key_L:
                self.log = not self.log
                self._replot()
            elif key == K.Key_P:
                savefile_cb('png')
            elif key == K.Key_S or key == 83:  # 83 == ord('s')
                savefile_cb('svg')
            elif key == K.Key_B:
                if self.current is not None:
                    self._show_overview()
            elif key in (K.Key_F1, K.Key_H):
                self._show_help()

        self.plot_layout.scene().keyPressEvent = key_handler

    def _replot(self):
        self._render()
        self._set_handlers()


def main(args):
    ''' load and match two simulation results' 1D monitors, then hand the
        resulting pairs to the pyqtgraph co-plot frontend above. '''
    logging.basicConfig(level=logging.INFO)

    # ensure keyboardinterrupt ctr-c
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    try:
        label_a = args.label_a[0] if args.label_a else None
        label_b = args.label_b[0] if args.label_b else None
        colour_a = args.colour_a[0] if args.colour_a else COLOUR_A
        colour_b = args.colour_b[0] if args.colour_b else COLOUR_B

        label_a, label_b, used_fallback = diffloader.default_labels(args.a, args.b, label_a, label_b)

        # When the auto-derived labels collided (e.g. two runs both ending
        # in a plain ".../<instrument>/1/" folder) and default_labels()
        # fell back to bare "A"/"B", those letters carry no identifying
        # information on their own - show the full source paths as an
        # on-canvas header row (see McCoplotPlotter._render()) and in the
        # window title, while the per-panel legend keeps just "A"/"B".
        path_note = None
        title = 'coplot: %s vs %s' % (label_a, label_b)
        if used_fallback:
            path_note = "A: %s   B: %s" % (args.a, args.b)
            title = 'coplot: %s' % path_note

        try:
            monitors_a, dir_a = diffloader.load_monitors(args.a)
            monitors_b, dir_b = diffloader.load_monitors(args.b)
        except Exception as e:
            print('mccoplot loader: ' + e.__str__())
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

        plotter = McCoplotPlotter(pairs, label_a, label_b, colour_a, colour_b,
                                   invcanvas=args.invcanvas, title=title, path_note=path_note,
                                   filenamebase="coplot_%s_vs_%s" % (
                                       diffloader.dirsafe_name(args.a), diffloader.dirsafe_name(args.b)))
        print(get_help_string())
        plotter.run()

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
    parser.add_argument('--invcanvas', action='store_true', help='invert canvas background from black to white')
    args = parser.parse_args()

    mccode_config.load_config("user")

    main(args)
