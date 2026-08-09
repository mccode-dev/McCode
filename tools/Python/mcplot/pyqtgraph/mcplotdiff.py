#!/usr/bin/env python3
'''
pyqtgraph mcplotdiff frontend.

This is an alternative variant of mcplot.py (the mcplot-pyqtgraph /
mxplot-pyqtgraph tool) that, instead of interactively browsing the monitors
of a single simulation, takes two simulation folders (or two single monitor
files) 'a' and 'b' and interactively browses the *difference*
diff.monN = a.monN - b.monN, for every monitor that is present (with
matching binning) in both simulations.

The monitor-loading, matching and subtraction logic lives in
mccodelib.mcplotdiffloader (shared with the html-based mcplot-diff-html
frontend); this script just assembles a plot graph from the diff data and
hands it to the same interactive pyqtgraph frontend (mccodelib.pqtgfrontend)
and plotting functions (plotfuncs.py) used by ordinary mcplot-pyqtgraph, so
that difference data can be browsed, zoomed, and colour-mapped exactly like
regular simulation data.

Since difference data is signed (positive where 'a' exceeds 'b', negative
where 'b' exceeds 'a'), 2D monitors default to a diverging blue/white/red
colour map (blue = negative, white = zero, red = positive) instead of the
usual 'jet' colour map. Press 'c' to cycle through the other colour maps as
usual.
'''
import argparse
import logging
import os
import sys

import qtpy
import plotfuncs

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

from mccodelib.plotgraph import PlotGraphPrint
from mccodelib import mcplotdiffloader as diffloader
from mccodelib import pqtgfrontend
from mccodelib import mccode_config


def main(args):
    ''' load and diff two simulation results, then hand the resulting diff
        plot graph to the pyqtgraph frontend, same way mcplot.py does for a
        single simulation. '''
    logging.basicConfig(level=logging.INFO)

    # ensure keyboardinterrupt ctr-c
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    try:
        label_a = args.label_a[0] if args.label_a else None
        label_b = args.label_b[0] if args.label_b else None

        try:
            diffs, dir_a, dir_b, label_a, label_b, used_fallback = diffloader.load_and_diff(
                args.a, args.b, label_a, label_b)
        except Exception as e:
            print('mcplotdiff loader: ' + e.__str__())
            quit()

        if len(diffs) == 0:
            print("mcplotdiff: no matching monitors found between '%s' and '%s', nothing to plot."
                  % (args.a, args.b))
            quit()

        graph = diffloader.build_diff_plotgraph(diffs)

        if args.test:
            printer = PlotGraphPrint(graph)

        # run pqtg frontend: reuse the ordinary plotfuncs.plot rendering
        # code, but start out on the diverging colour map (better suited to
        # signed difference data) and a window title that makes the a/b
        # comparison explicit. If the auto-derived labels collided (e.g.
        # two runs both ending in a plain ".../<instrument>/1/" folder) and
        # default_labels() fell back to bare "A"/"B", those letters alone
        # carry no identifying information - show the full source paths in
        # the title instead.
        title = 'diff: %s - %s' % (label_a, label_b)
        if used_fallback:
            title = 'diff: A: %s   B: %s' % (args.a, args.b)
        plotter = pqtgfrontend.McPyqtgraphPlotter(
            graph, sourcedir=dir_a, plot_func=plotfuncs.plot, invcanvas=args.invcanvas,
            title=title,
            icolormap=plotfuncs.get_colormap_index('diverging'))
        print(pqtgfrontend.get_help_string())
        plotter.runplot()

    except KeyboardInterrupt:
        print('keyboard interrupt')
    except Exception as e:
        print('mcplotdiff error: %s' % e.__str__())
        raise e


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('a', help='first simulation file or directory (the minuend, "a")')
    parser.add_argument('b', help='second simulation file or directory (the subtrahend, "b"); diff = a - b')
    parser.add_argument('-A', '--label-a', nargs=1, help='short label used for simulation a in titles')
    parser.add_argument('-B', '--label-b', nargs=1, help='short label used for simulation b in titles')
    parser.add_argument('-t', '--test', action='store_true', default=False, help='mccode data loader test run')
    parser.add_argument('--invcanvas', action='store_true', help='invert canvas background from black to white')
    args = parser.parse_args()

    mccode_config.load_config("user")

    main(args)
