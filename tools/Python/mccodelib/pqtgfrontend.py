'''
Plugable pyqtgraph mcplot frontend.
'''
import os
import sys
import math
import subprocess
import numpy as np

import pyqtgraph as pg

# qtpy normalises Qt5/Qt6/PySide2/PySide6 into a single API.
# QtCore.Qt.LeftButton, Qt.RightButton, Qt.ControlModifier … are all
# available at the same path regardless of which binding is active.
from qtpy import QtCore, QtGui, QtWidgets

from . import utils
from . import mccode_config
from .mcplotloader import McCodeDataLoader, test_decfuncs, PlotGraphPrint
from .plotgraph import PNSingle, PNMultiple
from .mcplotloader import Data2D


# ---------------------------------------------------------------------------
# Helpers for modifier / button comparison that work across Qt5 and Qt6.
# Under Qt5  Qt.ControlModifier is an int; under Qt6 it is an enum member.
# qtpy exposes them at the same attribute path, but comparing enum members
# to ints with == raises a TypeError in Qt6.  We normalise everything to int.
# ---------------------------------------------------------------------------

def _int_mod(mod):
    """Return the integer value of a keyboard modifier flag."""
    try:
        return int(mod)
    except (TypeError, ValueError):
        return mod.value


def _event_mods_int(event):
    """Return event.modifiers() as a plain int."""
    try:
        return int(event.modifiers())
    except (TypeError, ValueError):
        return event.modifiers().value


def _event_button_int(event):
    """Return event.button() as a plain int (1=Left, 2=Right, ...)."""
    try:
        return int(event.button())
    except (TypeError, ValueError):
        return event.button().value


# ---------------------------------------------------------------------------
# Keyboard modifier constants (resolved once at import time via qtpy)
# ---------------------------------------------------------------------------

_Qt = QtCore.Qt


def _get_modifier(modname):
    """Resolve a named modifier to a normalised int."""
    if modname == "none":
        return 0
    if modname == "ctrl":
        return _int_mod(_Qt.ControlModifier)
    if modname == "shft":
        return _int_mod(_Qt.ShiftModifier)
    if modname == "ctrl-shft":
        return _int_mod(_Qt.ControlModifier) | _int_mod(_Qt.ShiftModifier)
    if modname == "alt":
        return _int_mod(_Qt.AltModifier)
    raise ValueError("Unknown modifier name: %s" % modname)


# Left / right mouse button as plain ints
_LEFT_BUTTON  = _int_mod(_Qt.LeftButton)
_RIGHT_BUTTON = _int_mod(_Qt.RightButton)


# ---------------------------------------------------------------------------

def get_help_string():
    if sys.platform == 'darwin':
        modifier = 'Meta'
    elif sys.platform == 'win32':
        modifier = 'alt'
    else:
        modifier = 'ctrl'

    helplines = []
    helplines.append('')
    helplines.append('q              - quit')
    helplines.append('p              - save png')
    helplines.append('s              - save svg')
    helplines.append('l              - log toggle (disables plot from zero)')
    helplines.append('0              - plot 1D-curves from zero (disables log)')
    helplines.append('t              - textinfo toggle')
    helplines.append('c              - cycle colormap')
    helplines.append('F1/h           - help')
    helplines.append('F5             - replot')
    helplines.append('click          - display subplot')
    helplines.append('right-click/b  - back')
    helplines.append('%s + click   - sweep monitors' % modifier)
    helplines.append('x              - expand subplots')
    return '\n'.join(helplines)


class ViewModel():
    '''
    Logstate / view-options housekeeping object.
    '''
    def __init__(self, log=False, fromzero=False, legend=True, sourcedir=None, icolormap=0):
        self.log = log
        self.fromzero = fromzero
        self.icolormap = icolormap
        self.legend = legend
        self.sourcedir = sourcedir

    def flip_log(self):
        self.log = not self.log
        if self.log:
            self.fromzero = False
        return self.log

    def flip_fromzero(self):
        self.fromzero = not self.fromzero
        if self.fromzero:
            self.log = False
        return self.fromzero

    def inc_colormap(self):
        self.icolormap += 1

    def flip_legend(self):
        self.legend = not self.legend
        return self.legend

    def logstate(self):
        return self.log

    def fromzerostate(self):
        return self.fromzero

    def legendstate(self):
        return self.legend

    def cmapindex(self):
        return self.icolormap

    def get_sourcedir(self):
        return self.sourcedir


class McPyqtgraphPlotter():
    '''
    PyQtGraph-based plotter class.  Uses qtpy so the same code runs under
    PyQt5, PyQt6, PySide2, and PySide6 without branching.
    '''

    def __init__(self, plotgraph, sourcedir, plot_func, invcanvas, title=None, icolormap=0):
        '''
        plotgraph  : plotgraph root node
        sourcedir  : data files source directory (also used for the 'x'
                     expand-subplots shortcut, which lists and re-plots its
                     immediate subdirectories)
        plot_func  : user-supplied plotting function
                     signature: (node, i, plt, opts) -> (view_box, plt_item)
        invcanvas  : invert background (white bg / black fg)
        title      : window title; defaults to sourcedir if not given
        icolormap  : starting colormap index (see plotfuncs.COLORMAP_KEYS)
        '''
        self.graph     = plotgraph
        self.sourcedir = sourcedir
        self.plot_func = plot_func
        self.title     = title if title is not None else sourcedir
        self.icolormap = icolormap

        self.app = QtWidgets.QApplication.instance() or QtWidgets.QApplication(sys.argv)

        if invcanvas:
            pg.setConfigOption('background', 'w')
            pg.setConfigOption('foreground', 'k')

    # ------------------------------------------------------------------
    # Public entry point
    # ------------------------------------------------------------------

    def runplot(self):
        node = self.graph

        self.create_plotwindow(title=self.title)

        self.viewmodel = ViewModel(sourcedir=self.sourcedir, icolormap=self.icolormap)

        self.plot_node(node)
        self.update_statusbar(None, node, None)

        # qtpy exposes exec_() on all bindings for backwards-compat, but also
        # maps exec() on Qt6.  Use the attribute that exists.
        exec_fn = getattr(self.app, "exec", None) or self.app.exec_
        sys.exit(exec_fn())

    # ------------------------------------------------------------------
    # Window setup
    # ------------------------------------------------------------------

    def create_plotwindow(self, title):
        '''Set up the main window with a pyqtgraph GraphicsView and status bar.'''

        helpmessage = QtWidgets.QLabel()
        helpmessage.setText("Press 'h' for app shortcuts.")

        self.statusmessage = QtWidgets.QLabel()

        statusbar = QtWidgets.QStatusBar()
        statusbar.addWidget(helpmessage)
        statusbar.addPermanentWidget(self.statusmessage)

        self.main_window = QtWidgets.QMainWindow()
        self.graphics_view = pg.GraphicsView(self.main_window)
        self.main_window.setCentralWidget(self.graphics_view)
        self.main_window.setStatusBar(statusbar)
        self.main_window.setWindowTitle(title)
        self.main_window.setMouseTracking(True)

        # Screen size – primaryScreen() is available in Qt 5.6+ and Qt 6
        screen = QtWidgets.QApplication.primaryScreen()
        if screen is not None:
            rect = screen.size()
        else:
            # Very old Qt5 fallback
            rect = QtWidgets.QApplication.desktop().screenGeometry()

        w = int(0.85 * rect.width())
        h = int(0.85 * rect.height())
        self.main_window.resize(w, h)

        self.plot_layout = pg.GraphicsLayout(border=None)
        self.graphics_view.setCentralItem(self.plot_layout)
        self.plot_layout.setContentsMargins(2, 2, 2, 2)

        self.main_window.show()
        self.main_window.raise_()
        self.main_window.activateWindow()

    # ------------------------------------------------------------------
    # Plot node (recursive, event-driven)
    # ------------------------------------------------------------------

    def plot_node(self, node, sync_zoom_obj_ids=[]):
        '''
        Event-driven recursive plot function.
        Click events are re-registered on each recursion.
        '''
        if node is None:
            return

        self.clear_window_and_handlers()

        parent   = node.parent
        prim_lst = node.primaries
        sec_lst  = node.secondaries

        # Build plots
        n = node.getnumdata()
        self.viewbox_list = []
        self.plot_list    = []
        for i in range(n):
            viewbox, plt = self.add_plot(node, i, n)
            self.viewbox_list.append(viewbox)
            self.plot_list.append(plt)

        if id(node) in sync_zoom_obj_ids:
            self.sync_views_zooming()

        # Tag sync-zoom node ids (secondaries)
        sync_zoom_obj_ids = sync_zoom_obj_ids + [id(nd) for nd in sec_lst]

        # Node lists per interaction type
        node_list_click     = prim_lst
        node_list_rclick    = [parent] * n if parent else []
        node_list_ctrlclick = sec_lst

        plot_node_cb = lambda nd: self.plot_node(nd, sync_zoom_obj_ids=sync_zoom_obj_ids)

        # plain left-click  → primary node
        self.set_handler(node_list_click,     plot_node_cb, "click",  _get_modifier("none"))
        # right-click       → parent node
        self.set_handler(node_list_rclick,    plot_node_cb, "rclick", _get_modifier("none"))
        # alt+click / ctrl+click → secondary node (sweep monitors)
        self.set_handler(node_list_ctrlclick, plot_node_cb, "click",  _get_modifier("alt"))
        self.set_handler(node_list_ctrlclick, plot_node_cb, "click",  _get_modifier("ctrl"))

        # Keyboard handler
        replot_cb = lambda: self.plot_node(node)
        back_cb   = lambda: self.plot_node(parent)
        self.set_keyhandler(replot_cb, back_cb)

    # ------------------------------------------------------------------
    # Help dialog
    # ------------------------------------------------------------------

    def show_help(self):
        prefix = "mc" if mccode_config.configuration["MCCODE"] == "mcstas" else "mx"
        QtWidgets.QMessageBox.about(self.main_window, prefix + 'plot-pyqtgraph', get_help_string())

    # ------------------------------------------------------------------
    # Keyboard handler
    # ------------------------------------------------------------------

    def set_keyhandler(self, replot_cb, back_cb):
        '''Install a scene-level key-press handler.'''

        # Resolve Key_* constants once – same path in Qt5 and Qt6 via qtpy
        K = _Qt

        savefile_cb = lambda fmt: utils.dumpfile_pqtg(
            scene=self.plot_layout.scene(), format=fmt)
        expand_sp = lambda: self.expand_subplots(sourcedir=self.viewmodel.get_sourcedir())

        def key_handler(ev):
            key = ev.key()
            if   key == K.Key_Q:                    QtWidgets.QApplication.quit()
            elif key == K.Key_L:                    self.viewmodel.flip_log();      replot_cb()
            elif key == K.Key_0:                    self.viewmodel.flip_fromzero(); replot_cb()
            elif key == K.Key_P:                    savefile_cb('png')
            elif key == K.Key_S or key == 83:       savefile_cb('svg')   # 83 == ord('s')
            elif key == K.Key_T:
                print("Toggle legend visibility")
                self.viewmodel.flip_legend()
                replot_cb()
            elif key == K.Key_C:                    self.viewmodel.inc_colormap(); replot_cb()
            elif key == K.Key_F5:                   replot_cb()
            elif key == K.Key_X:                    expand_sp()
            elif key in (K.Key_F1, K.Key_H):        self.show_help()
            elif key == K.Key_B:                    back_cb()

        self.plot_layout.scene().keyPressEvent = key_handler

    # ------------------------------------------------------------------
    # Mouse / click handler
    # ------------------------------------------------------------------

    def set_handler(self, node_list, node_cb, click, modifier):
        '''
        Register a sigMouseClicked handler for a specific button + modifier
        combination.

        click    : "click" (left button) or "rclick" (right button)
        modifier : normalised int from _get_modifier()
        '''
        if not list(node_list):
            return

        expected_mod = modifier   # already an int

        def click_handler(event):
            # --- modifier check ---
            if _event_mods_int(event) != expected_mod:
                return

            # --- button check ---
            btn = _event_button_int(event)
            if click == "rclick" and btn != _RIGHT_BUTTON:
                return
            if click == "click"  and btn != _LEFT_BUTTON:
                return

            # --- spatial hit-test ---
            try:
                posScene = event.scenePos()
                plotIdx  = self.get_plot_index(posScene)
                posItem  = event.pos()
            except AttributeError:
                return

            if 0 <= plotIdx < len(node_list):
                target_node = node_list[plotIdx]
                node_cb(target_node)
                self.update_statusbar(None, target_node, posItem)

        self.plot_layout.scene().sigMouseClicked.connect(click_handler)

    # ------------------------------------------------------------------
    # Plot helpers
    # ------------------------------------------------------------------

    def get_plot_func_opts(self, fromzero, log, legend, icolormap,
                           verbose, fontsize, cbmin=None, cbmax=None, cell_height=None):
        d = {}
        d['log']       = log
        d['fromzero']  = fromzero
        d['legend']    = legend
        d['icolormap'] = icolormap
        d['verbose']   = verbose
        d['fontsize']  = fontsize
        d['cell_height'] = cell_height
        if cbmin is not None and cbmax is not None:
            d['cbmin'] = cbmin
            d['cbmax'] = cbmax
        return d

    def get_sweep_multiplot_colorbar_limits(self, node):
        if type(node) == PNMultiple:
            cbmin = float("inf")
            cbmax = float("-inf")
            monname = None
            for data in node.getdata_lst():
                if type(data) != Data2D:
                    continue
                if monname is None:
                    monname = data.component
                elif data.component != monname:
                    return None, None
                localmin = np.min(np.array(data.zvals))
                localmax = np.max(np.array(data.zvals))
                cbmin = min(cbmin, localmin)
                cbmax = max(cbmax, localmax)
            return cbmin, cbmax
        return None, None

    def add_plot(self, node, i, n):
        '''Construct a single plot item and add it to the layout.'''

        def get_golden_rowlen(n):
            return int(math.sqrt(n * 1.61803398875))

        plt    = pg.PlotItem()
        rowlen = get_golden_rowlen(n)
        rows   = math.ceil(n / rowlen) if rowlen > 0 else 1

        # Approximate on-screen height of a single grid cell, from the
        # *current* window size (so it reacts to live window resizes, not
        # just the size at startup). Used to scale the colour bar's tick
        # font: the same 12-pane overview should get a bigger colour bar
        # font on a large/maximised window than squeezed into a small one.
        win_size = self.main_window.size()
        cell_height = win_size.height() / rows if rows > 0 else win_size.height()

        verbose  = n <= 4
        # NOTE: plotfuncs.plot_Data2D() only draws its colour-bar panel when
        # fontsize >= 8 (it (mis)uses this text-sizing value as a crowding
        # threshold), so the smallest tier below must never go under 8.
        # The breakpoint for that smallest tier is deliberately set above 12
        # (rather than at 12) since a 12-pane overview is common enough that
        # it should keep the more readable 10pt size; only genuinely dense
        # views (>16 monitors) drop to 8pt.
        if n <= 2:
            fontsize = 14
        elif n <= 16:
            fontsize = 10
        else:
            fontsize = 8

        if self.viewmodel.logstate():
            cbmin = cbmax = None
        else:
            cbmin, cbmax = self.get_sweep_multiplot_colorbar_limits(node)

        options = self.get_plot_func_opts(
            self.viewmodel.fromzerostate(),
            self.viewmodel.logstate(),
            self.viewmodel.legendstate(),
            self.viewmodel.cmapindex(),
            verbose, fontsize, cbmin, cbmax, cell_height=cell_height)

        view_box, plt_itm = self.plot_func(node, i, plt, options)
        if view_box:
            self.plot_layout.addItem(plt_itm, i // rowlen, i % rowlen)

        # Reconnect mouse-move handler (disconnect stale one first)
        try:
            self.plot_layout.scene().sigMouseMoved.disconnect()
        except TypeError:
            pass

        self.plot_layout.scene().sigMouseMoved.connect(
            lambda pos: self.update_statusbar(plt, node, pos))

        return view_box, plt

    # ------------------------------------------------------------------
    # Status bar updates
    # ------------------------------------------------------------------

    def update_statusbar(self, plot, node, pos):
        if isinstance(node, PNSingle):
            if plot is None and len(self.plot_list) == 1:
                plot = self.plot_list[0]
            self.single_plot_mouse_moved(plot, node, pos)
        elif isinstance(node, PNMultiple):
            self.multi_plot_mouse_moved(node, pos)

    def single_plot_mouse_moved(self, plot, node, pos):
        if plot is not None and pos is not None:
            pos = plot.getViewBox().mapSceneToView(pos)

        xlabel = ylabel = ""
        if node is not None:
            xlabel = node.getdata_idx(0).xlabel
            ylabel = node.getdata_idx(0).ylabel
        if xlabel == "":
            xlabel = "x"
        if ylabel == "":
            ylabel = "y"

        if pos is not None:
            logstr = " (log)" if self.viewmodel.log else ""
            posstr = "%.24s=%g, %.24s=%g%s." % (xlabel, pos.x(), ylabel, pos.y(), logstr)
        else:
            posstr = ""
        self.statusmessage.setText(posstr)

    def multi_plot_mouse_moved(self, node, pos):
        prim      = node.primaries
        num_plots = node.getnumdata()
        statustext = ""

        idx = self.get_plot_index(pos)
        if idx >= 0:
            statustext = "Plot %d/%d" % (idx + 1, num_plots)
            if prim is not None:
                title = prim[idx].getdata_idx(0).title
                if len(title) > 64:
                    title = title[:64] + "..."
                statustext += ": " + title
            statustext += "."
        else:
            statustext = "Showing %d plot%s." % (num_plots, "" if num_plots == 1 else "s")

        prefix = "Mode: log. " if self.viewmodel.log else "Mode: lin. "
        self.statusmessage.setText(prefix + statustext)

    # ------------------------------------------------------------------
    # Geometry helpers
    # ------------------------------------------------------------------

    def get_plot_index(self, pos):
        '''Return the index of the viewbox that contains scene-position pos.'''
        if self.viewbox_list is None or pos is None:
            return -1

        for idx, viewbox in enumerate(self.viewbox_list):
            topRight   = viewbox.mapViewToScene(viewbox.viewRect().topRight())
            bottomLeft = viewbox.mapViewToScene(viewbox.viewRect().bottomLeft())

            rect = QtCore.QRectF()
            rect.setTopRight(topRight)
            rect.setBottomLeft(bottomLeft)

            if rect.contains(pos):
                return idx

        return -1

    def sync_views_zooming(self):
        '''Replace per-viewbox wheel events with a single global one.'''
        org_wheel_events = [vb.wheelEvent for vb in self.viewbox_list]

        def modded_wheel_event(ev, axis=None):
            for vb in self.viewbox_list:
                org_wheel_events[self.viewbox_list.index(vb)](ev, axis=axis)

        for vb in self.viewbox_list:
            vb.wheelEvent = modded_wheel_event

    # ------------------------------------------------------------------
    # Cleanup
    # ------------------------------------------------------------------

    def clear_window_and_handlers(self):
        self.plot_layout.clear()
        try:
            self.plot_layout.scene().sigMouseClicked.disconnect()
            self.plot_layout.scene().sigMouseMoved.disconnect()
        except TypeError:
            pass

    # ------------------------------------------------------------------
    # Subprocess expansion
    # ------------------------------------------------------------------

    def expand_subplots(self, sourcedir):
        '''Open a new mcplot-pyqtgraph process for each immediate subdirectory.'''
        def get_immediate_subdirectories(a_dir):
            return [name for name in os.listdir(a_dir)
                    if os.path.isdir(os.path.join(a_dir, name))]

        def sortalpha(data):
            return sorted(data, key=lambda item: (
                int(item.partition(' ')[0]) if item[0].isdigit() else float('inf'),
                item))

        subdirs = sortalpha(get_immediate_subdirectories(sourcedir))
        if not subdirs:
            print("no subdirs to plot")
            return

        prefix = "mc" if mccode_config.configuration["MCCODE"] == "mcstas" else "mx"
        for s in subdirs:
            subprocess.Popen(
                '%splot-pyqtgraph %s' % (prefix, os.path.join(sourcedir, s)),
                shell=True, cwd=os.getcwd())
