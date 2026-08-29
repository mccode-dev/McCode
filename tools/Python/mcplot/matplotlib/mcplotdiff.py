#!/usr/bin/env python3
'''
matplotlib mcplotdiff frontend.

This is an alternative variant of mcplot.py (the mcplot-matplotlib /
mxplot-matplotlib tool) that, instead of interactively browsing the monitors
of a single simulation, takes two simulation folders (or two single monitor
files) 'a' and 'b' and interactively browses the *difference*
diff.monN = a.monN - b.monN, for every monitor that is present (with
matching binning) in both simulations.

The monitor-loading, matching and subtraction logic lives in
mccodelib.mcplotdiffloader (shared with the mcplot-diff-html and
mcplot-diff-pyqtgraph frontends); this script just assembles a plot graph
from the diff data and hands it to the same plotting code (plotfuncs.py)
used by ordinary mcplot-matplotlib, so that difference data can be browsed,
zoomed, and saved to disk exactly like regular simulation data.
'''
import argparse
import logging
import os
import sys
import matplotlib

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

import plotfuncs

from mccodelib.plotgraph import PlotGraphPrint
from mccodelib import mcplotdiffloader as diffloader
from mccodelib import mccode_config


def main(args):
    ''' load and diff two simulation results, then hand the resulting diff
        plot graph to the matplotlib frontend, same way mcplot.py does for a
        single simulation. '''
    logging.basicConfig(level=logging.INFO)

    # ensure keyboardinterrupt ctr-c
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    try:
        label_a = args.label_a[0] if args.label_a else None
        label_b = args.label_b[0] if args.label_b else None

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

        try:
            diffs, dir_a, dir_b, label_a, label_b, used_fallback = diffloader.load_and_diff(
                args.a, args.b, label_a, label_b)
        except Exception as e:
            print('mcplotdiff loader: ' + e.__str__())
            plotfuncs.print_help(nogui=True)
            quit()

        if len(diffs) == 0:
            print("mcplotdiff: no matching monitors found between '%s' and '%s', nothing to plot."
                  % (args.a, args.b))
            quit()

        graph = diffloader.build_diff_plotgraph(diffs)

        if args.test:
            PlotGraphPrint(graph)

        # default base name for --format/--output dumps and for --html,
        # since there's no single simulation file to derive it from -
        # deliberately built from the actual input paths (dirsafe_name),
        # not label_a/label_b: those may legitimately collapse to bare
        # "A"/"B" when their basenames collide (see default_labels()),
        # which would otherwise make every such comparison overwrite the
        # same "diff_A_vs_B.*" files - a real problem for batch/CI use
        # running many comparisons out of one working directory.
        plotfuncs.filenamebase = "diff_%s_vs_%s" % (
            diffloader.dirsafe_name(args.a), diffloader.dirsafe_name(args.b))

        # label_a/label_b already carry full identification even when the
        # auto-derived labels collided (e.g. two runs both ending in a
        # plain ".../<instrument>/1/" folder) - default_labels() falls
        # back to using each side's full input path as its label directly
        # in that case, so there's nothing further to add here.
        sourcedir = 'diff: %s - %s' % (label_a, label_b)
        plotter = plotfuncs.McMatplotlibPlotter(sourcedir=sourcedir, log=args.log)

        if (sys.platform == "linux" or sys.platform == "linux2") and args.html:
            # save to html and exit
            plotter.html_node(graph, open('%s.html' % plotfuncs.filenamebase, 'w'))
        else:
            # display gui / prepare graphics dump
            plotfuncs.print_help(nogui=True)
            plotter.plot_node(graph)

        if args.output or args.format:
            try:
                plotfuncs.dumpfile(args.format, args.output)
            except Exception as e:
                print('dumpfile issue: ' + e.__str__())

    except KeyboardInterrupt:
        print('keyboard interrupt')
    except Exception as e:
        print('mcplotdiff error: %s' % e.__str__())
        raise e


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('a', help='first simulation file or directory (the minuend, "a")')
    parser.add_argument('b', help='second simulation file or directory (the subtrahend, "b"); diff = a - b')
    parser.add_argument('-A', '--label-a', nargs=1, help='short label used for simulation a in titles/filenames')
    parser.add_argument('-B', '--label-b', nargs=1, help='short label used for simulation b in titles/filenames')
    parser.add_argument('-t', '--test', action='store_true', default=False, help='plot graph structure test/print')
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
