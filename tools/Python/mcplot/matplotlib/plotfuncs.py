'''
Reusable matplotlib plotting/interaction code for mcplot-matplotlib.

This mirrors the pyqtgraph tool's plotfuncs.py / pqtgfrontend.py split: the
plot-graph-driven rendering and interaction logic lives here (unrenamed, so
it can be imported by name regardless of McStas/McXtrace flavor), while
mcplot.py stays a thin CLI wrapper around it. mcplot-diff-matplotlib reuses
this exact module, applied to a diff plot graph instead of an ordinary
simulation's plot graph.
'''
import os
import sys
import math
import textwrap
import numpy as np

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

from mccodelib.plotgraph import PNSingle
from mccodelib.mcplotloader import Data1D, Data2D
from mccodelib.mccode_config import get_mccode_prefix

# Legacy fixed font size, kept for backward compatibility with any external
# code that may reference plotfuncs.FONTSIZE directly. Plotting itself now
# uses _panel_fontsize(n), which scales with the number of panels in view.
FONTSIZE = 10

# Global placeholder for later import of pylab by the caller
# - necessary due to use of matplotlib.use() which has to happen
# before import of pylab
pylab = 0

# Default base name used by dumpfile(); callers can override this
# (e.g. mcplot.py sets it from the loaded simulation, mcplotdiff.py sets it
# from the two simulation labels) before calling dumpfile().
filenamebase = get_mccode_prefix() + 'plot'


def _calc_panel_size(nfigs):
    ''' golden-ratio-ish grid dimensions (ncols, nrows) for nfigs panels '''
    nx = int(math.sqrt(nfigs*1.61803398875)) # golden ratio
    ny = nfigs // nx + int(bool(nfigs % nx))
    return nx, ny


# matplotlib's own stock default figure size (inches)
_DEFAULT_FIGSIZE = (6.4, 4.8)

# Scale-up applied to the single-plot (drill-down) view, relative to
# matplotlib's stock default. The grid-based overview sizing below does NOT
# reduce to this by itself at n=1 (it would actually come out smaller than
# the stock default), so n<=1 is special-cased.
_SINGLE_VIEW_SCALE = 2.0


def _figure_size(n):
    ''' (width, height) in inches for a view of n panels.

        n <= 1: the single-plot drill-down view, sized as a multiple of
        matplotlib's own default figure size (rather than the grid formula
        below, which would otherwise come out smaller than that default).

        n > 1: overview grid, sized to the panel grid so multi-panel views
        actually use the available window/screen real estate, capped so
        very large grids don't produce an unreasonably huge window. '''
    if n <= 1:
        return (_DEFAULT_FIGSIZE[0] * _SINGLE_VIEW_SCALE,
                _DEFAULT_FIGSIZE[1] * _SINGLE_VIEW_SCALE)
    cols, rows = _calc_panel_size(n)
    fig_w = min(3.4 * cols + 1.0, 22)
    fig_h = min(2.8 * rows + 1.0, 14)
    return (fig_w, fig_h)


def _panel_fontsize(n):
    ''' Point size for axis labels, scaled by how many panels are in view:
        a couple of panels can afford large, readable text; once a view
        gets past a dozen or so panels, labels need to shrink substantially
        or they crowd the plot. '''
    if n <= 2:
        return 14
    elif n <= 16:
        return 10
    else:
        return 8


def _title_fontsize(n):
    ''' Point size for panel titles specifically. Deliberately smaller than
        _panel_fontsize(): titles here can carry a long, multi-field string
        (component/filename/title/I/Err/N/statistics), and that combination
        of digits, decimals and "="/"+"/"-" symbols renders noticeably wider
        per character than ordinary prose at the same point size - so
        titles need a smaller size than axis labels to reliably fit. '''
    if n <= 2:
        return 11
    elif n <= 16:
        return 9
    else:
        return 7


def _title_wrap_width(n, fontsize):
    ''' Approximate number of characters that fit across one panel's width
        at the given fontsize, used to wrap long titles instead of letting
        them overflow the figure. Deliberately a rough heuristic (average
        bold-character width as a fraction of the font's point size) rather
        than an exact text-measurement - good enough to catch the common
        case of a long component/filename/statistics string, without
        depending on a renderer being available at layout time.

        The titles this wraps tend to be dense with digits and symbols
        ("I = 1.2E+7 Err = ...; X0=...; dX=...;"), which render wider per
        character than ordinary prose, so the width-per-character factor
        here is deliberately generous (i.e. wraps a bit earlier than a
        prose-tuned estimate would) to leave headroom against that. '''
    fig_w, _ = _figure_size(n)
    cols, _ = _calc_panel_size(n) if n > 1 else (1, 1)
    panel_w_in = fig_w / max(cols, 1)

    avg_char_width_in = (fontsize * 0.85) / 72.0  # bold, digit/symbol-heavy text
    usable_w_in = panel_w_in * 0.88               # leave a bit more margin
    return max(15, int(usable_w_in / avg_char_width_in))


def _wrap_title(title, width):
    ''' Wrap each already-newline-separated line of `title` to at most
        `width` characters, so a long component/filename/statistics line
        doesn't run wider than the figure. '''
    return '\n'.join(
        textwrap.fill(line, width=width) if line else line
        for line in title.split('\n')
    )


def plot_single_data(node, i, n, log):
    ''' plot the data of node, at index i, to a subplot '''
    if type(node) == PNSingle and i != 0:
        raise Exception("inconsistent plot request, idx=%s" % str(i))

    dims = _calc_panel_size(n)
    subplt = pylab.subplot(dims[1],dims[0],i+1)
    data = node.getdata_idx(i)

    verbose = n == 1
    fontsize = _panel_fontsize(n)

    if type(data) is Data1D:
        ''' plot 1D data '''
        xmin = data.xlimits[0]
        xmax = data.xlimits[1]
        pylab.xlim(xmin,xmax)

        x = np.array(data.xvals).astype(float)
        y = np.array(data.yvals).astype(float)
        yerr = np.array(data.y_err_vals).astype(float)

        ylabel = data.ylabel
        if log == True:
            # handle Log intensity
            invalid = np.where(y <= 0)
            valid   = np.where(y > 0)
            if len(valid[0]):
                min_valid  = np.min(y[valid])
                y[invalid] = min_valid/10
            yerr=yerr / y
            y=np.log(y)
            ylabel ="log(" + data.ylabel +")"

        errbar = pylab.errorbar(x, y, yerr)

        pylab.xlabel(data.xlabel, fontsize=fontsize, fontweight='bold')
        pylab.ylabel(ylabel, fontsize=fontsize, fontweight='bold')
        try:
            legend_text = '%s\nI = %s' % (data.component, data.values[0])
            if hasattr(data, 'diff_pct_str'):
                legend_text += '\nDiff: %s' % data.diff_pct_str
            if verbose:
                legend_text = '%s [%s]\n%s\nI = %s Err = %s N = %s; %s' % (data.component, data.filename, data.title, data.values[0], data.values[1], data.values[2], data.statistics)
        except:
            legend_text = '%s\n[%s]' % (data.component, data.filename)
        title_fontsize = _title_fontsize(n)
        legend_text = _wrap_title(legend_text, _title_wrap_width(n, title_fontsize))
        errbar[0].set_label(legend_text)
        leg = pylab.legend(loc='upper left', fontsize=title_fontsize, framealpha=0.85,
                            handlelength=1.2, handletextpad=0.5, borderpad=0.4)
        leg.set_draggable(True)

    elif type(data) is Data2D:
        ''' plot 2D data '''
        zvals = np.array(data.zvals)

        xmin = data.xlimits[0]
        xmax = data.xlimits[1]
        ymin = data.xlimits[2]
        ymax = data.xlimits[3]

        mysize=zvals.shape
        x = pylab.linspace(xmin, xmax, mysize[1])
        y = pylab.linspace(ymin, ymax, mysize[0])
        pylab.xlim(xmin, xmax)
        pylab.ylim(ymin, ymax)

        if log == True:
            invalid = np.where(zvals <= 0)
            valid = np.where(zvals > 0)
            min_valid = np.min(zvals[valid])
            zvals[invalid] = min_valid/10
            zvals = np.log(zvals)

        ''' out-commented code: alternative plot types '''
        #from mpl_toolkits.mplot3d import Axes3D
        #ax = Axes3D(pylab.gcf())
        #pylab.contour(x,y,zvals)
        #ax.plot_surface(x,y,zvals)
        # pcolormesh (QuadMesh) instead of pcolor (PolyCollection): pcolor
        # builds one vector polygon per grid cell, which is dramatically
        # slower to construct and render for vector output formats
        # (mctest.py generates a PDF overview per test via --format=pdf),
        # and produces much larger files. pcolormesh is a same-signature
        # drop-in given our regularly-gridded x/y (from linspace), and
        # rasterized=True has vector backends embed it as a single bitmap
        # rather than thousands of individual polygons; PNG/interactive
        # output is raster either way and unaffected.
        pylab.pcolormesh(x, y, zvals, shading='auto', rasterized=True)
        pylab.colorbar()

        pylab.xlabel(data.xlabel, fontsize=fontsize, fontweight='bold')
        pylab.ylabel(data.ylabel, fontsize=fontsize, fontweight='bold')

        try:
            legend_text = '%s\nI = %s' % (data.component, data.values[0])
            if hasattr(data, 'diff_pct_str'):
                legend_text += '\nDiff: %s' % data.diff_pct_str
            if verbose:
                legend_text = '%s [%s]\n%s\nI = %s Err = %s N = %s; %s' % (data.component, data.filename, data.title, data.values[0], data.values[1], data.values[2], data.statistics)
        except:
            legend_text = '%s\n[%s]' % (data.component, data.filename)
        title_fontsize = _title_fontsize(n)
        legend_text = _wrap_title(legend_text, _title_wrap_width(n, title_fontsize))
        # pcolor() returns a QuadMesh, which doesn't make a useful legend
        # swatch; use an invisible dummy artist to carry the label instead
        # (same technique the pyqtgraph tool's plot_Data2D uses).
        pylab.plot([], [], ' ', label=legend_text)
        leg = pylab.legend(loc='upper left', fontsize=title_fontsize, framealpha=0.85,
                            handlelength=0, handletextpad=0, borderpad=0.4)
        leg.set_draggable(True)

    else:
        print("Unsuported plot data type %s" % type(data))

    return subplt


class McMatplotlibPlotter():
    ''' matplotlib plotting frontend '''
    def __init__(self, sourcedir, log):
        self.sourcedir = sourcedir
        self.event_dc_cid = None
        self.log = log


    def _flip_log(self):
        self.log = not self.log


    def _click_proxy(self, event):
        ''' state-updating proxy for click handler '''
        dc_cb = lambda: pylab.disconnect(self.event_dc_cid)
        click(event, subplts=self.subplts, click_cbs=self.click_cbs,
              ctrl_cbs=self.ctrl_cbs, back_cb=self.back_cb, dc_cb=dc_cb)


    def _keypress_proxy(self, event):
        ''' state-updating proxy for keypress handler '''
        keypress(event, back_cb=self.back_cb, replot_cb=self.replot_cb,
                 togglelog_cb=self._flip_log)


    def plot_node(self, node):
        ''' plot recursion '''
        # safety
        if not node:
            return

        # clear
        pylab.close()

        # plot data and keep subplots for the click area filter
        n = node.getnumdata()

        # Size the figure to the view: 2x matplotlib's stock default for a
        # single-plot drill-down, or to the panel grid for an overview, so
        # either way the window actually uses available screen real estate.
        fig_w, fig_h = _figure_size(n)
        fig = pylab.figure(figsize=(fig_w, fig_h))

        self.subplts = [plot_single_data(node, i, n, self.log) for i in range(n)]

        # Tighten the outer margins (matplotlib's defaults leave a
        # surprisingly large blank border). Titles now live as an in-plot
        # legend rather than text spanning above each panel, so there's no
        # longer a need for the extra hspace headroom that used to guard
        # against titles overlapping the row above.
        fig.subplots_adjust(left=0.06, right=0.98, top=0.97, bottom=0.06,
                             wspace=0.3, hspace=0.35)

        # create callbacks
        self.click_cbs = [lambda nde=n: self.plot_node(nde) for n in node.primaries]
        self.ctrl_cbs = [lambda nde=n: self.plot_node(nde) for n in node.secondaries]
        # we need root node back click to prompt a replot, rather than to be ignored,
        # because disconnect is always called by the click handler
        bck_node = node.parent if node.parent else node 
        self.back_cb = lambda n=bck_node: self.plot_node(n)
        self.replot_cb = lambda n=node: self.plot_node(n) if node else None

        # regiseter click events
        self.event_dc_cid = pylab.connect('button_press_event', self._click_proxy)

        # register keypress events
        pylab.connect('key_press_event', self._keypress_proxy)

        # show the plot
        pylab.show()


    def html_node(self, node, fileobj):
        '''  plots node and saves to html using mpld3 '''
        import mpld3
        
        n = node.getnumdata()

        fig_w, fig_h = _figure_size(n)
        fig = pylab.figure(figsize=(fig_w, fig_h))

        self.subplts = [plot_single_data(node, i, n, self.log) for i in range(n)]

        fig.subplots_adjust(left=0.06, right=0.98, top=0.97, bottom=0.06,
                             wspace=0.3, hspace=0.35)

        mpld3.save_html(pylab.gcf(), fileobj)


def keypress(event, back_cb, replot_cb, togglelog_cb):
    key = event.key.lower()

    if key == 'q':
        quit()
    elif key == 'l':
        togglelog_cb()
        replot_cb()
    elif event.key == 'p':
        dumpfile('ps')
    elif event.key == 'd':
        dumpfile('pdf')
    elif event.key == 'n':
        dumpfile('png')
    elif event.key == 'j':
        dumpfile('jpg')
    elif key == 'b':
        back_cb()
    elif key == 'f5':
        replot_cb()
    elif key == 'f1' or key == 'h':
        print_help()
    elif key == 'b':
        back_cb()


def print_help(nogui=False):
    helplines = []
    helplines.append('')
    helplines.append('q              - quit')
    helplines.append('F1/h           - help')
    helplines.append('l              - toggle log')
    helplines.append('p              - save ps')
    helplines.append('d              - save pdf')
    helplines.append('n              - save png')
    helplines.append('j              - save jpg')
    helplines.append('s              - native save dialog')
    helplines.append('F5             - replot')
    helplines.append('click          - display subplot')
    helplines.append('right-click/b  - back')
    helplines.append('ctrl + click   - sweep monitors')
    print('\n'.join(helplines))


def dumpfile(frmat, filename = None):
    """ save current fig to softcopy """
    from pylab import savefig

    global filenamebase
    
    if filename is None:
        filename = filenamebase
        
    # get directory, basename and extension
    if isinstance(filename, list):
        filename = filename[0]
    basename  = os.path.splitext(filename)[0] # contains full path and base filename
    if frmat is None:
        ext       = os.path.splitext(filename)[1] # extension with the dot
    else:
        ext = frmat
    if ext[0] != '.':
        ext = '.'+ext
    # assemble target name
    saveto = basename + ext
    
    # Check for existance of earlier exports
    if os.path.isfile(saveto):
        index=1
        saveto = f"{basename}_{index}{frmat}"
        while os.path.isfile(saveto):
            index += 1
            saveto = f"{basename}_{index}{frmat}"

    savefig(saveto)
    print("Saved " + saveto)


def click(event, subplts, click_cbs, ctrl_cbs, back_cb, dc_cb):
    subplt = event.inaxes
    if not subplt:
        return False
    else:
        lclick = event.button==1
        rclick = event.button==3
        ctrlmod = (event.key == 'control' or event.key == 'cmd' or event.key == 'x')

        if lclick and len(click_cbs) > 0 and not ctrlmod:
            # left button
            idx = subplts.index(subplt)
            dc_cb()
            click_cbs[idx]()
        elif rclick and back_cb and not ctrlmod:
            # right button
            dc_cb()
            back_cb()
        elif ctrlmod and len(ctrl_cbs) > 0:
            # control-click
            idx = subplts.index(subplt)
            dc_cb()
            ctrl_cbs[idx]()
