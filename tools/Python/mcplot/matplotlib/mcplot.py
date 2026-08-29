#!/usr/bin/env python3
'''
matplotlib mcplot implementation
'''

import argparse
import logging
import os
import sys
import subprocess
import matplotlib

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

import plotfuncs

from mccodelib.mcplotloader import McCodeDataLoader, test_decfuncs
from mccodelib.plotgraph import PlotGraphPrint
from mccodelib.mccode_config import get_mccode_prefix
from mccodelib import mccode_config


def main(args):
    ''' load data from mcplot backend and send it to the matplotlib frontend above '''
    logging.basicConfig(level=logging.INFO)
    
    # ensure keyboardinterrupt ctr-c
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    
    try:
        if len(args.simulation) == 0:
            simfile = ''
        else:
            simfile = args.simulation[0]
            h5file = None
            # Check if we are prompted with a h5 file or a directory with mccode.h5 index
            # -> try spawning nexpy
            if ('mccode.h5' in args.simulation[0]):
                h5file = args.simulation[0]
            if (os.path.isdir(args.simulation[0]) and
                    os.path.isfile(os.path.join(args.simulation[0],'mccode.h5')) and
                not os.path.isfile(os.path.join(args.simulation[0],'mccode.sim'))):
                h5file = os.path.join(args.simulation[0],'mccode.h5')
            if (h5file):
                if not os.path.isabs(h5file):
                    h5file = os.path.join(os.getcwd(),h5file)
                try:
                    cmd = mccode_config.configuration['HDFVIEW'] + ' ' + h5file
                    print('Spawning ' + mccode_config.configuration['HDFVIEW'])
                    sub=subprocess.Popen(cmd, shell=True)
                    sub.wait()
                except:
                    print("Could not launch " + mccode_config.configuration['HDFVIEW'] + " on " + h5file)
                quit()

        if args.test:
            test_decfuncs(simfile)

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

        # load data
        loader = McCodeDataLoader(simfile=simfile)
        try:
            loader.load()
        except Exception as e:
            # invallid input case:
            print('mcplot loader: ' + e.__str__())
            plotfuncs.print_help(nogui=True)
            quit()
        rootnode = loader.plot_graph
        if args.test:
            PlotGraphPrint(rootnode)

        # start the plotter
        plotter = plotfuncs.McMatplotlibPlotter(sourcedir=loader.directory, log=args.log)

        if (sys.platform == "linux" or sys.platform == "linux2") & args.html:
            # save to html and exit
            plotter.html_node(rootnode, open('%s.html' % os.path.splitext(simfile)[0], 'w'))
        else:
            # display gui / prepare graphics dump
            plotfuncs.print_help(nogui=True)
            plotter.plot_node(rootnode)
        
        if args.output or args.format:
            try:
                plotfuncs.dumpfile(args.format, args.output)
            except Exception as e:
                print('dumpfile issue: ' + e.__str__())

    except KeyboardInterrupt:
        print('keyboard interrupt')
    except Exception as e:
        print('mcplot error: %s' % e.__str__())
        raise e


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('simulation', nargs='*', 
        help='file or directory to plot')
    parser.add_argument('-t', '--test',  action='store_true', default=False, 
        help='mccode data loader test run')
    parser.add_argument('--html', action='store_true', 
        help='save plot to html using mpld3 (linux only)')
    parser.add_argument('--format', dest='format', 
        help='save plot to pdf/png/eps/svg... without bringing up window')
    parser.add_argument('--output',nargs=1, dest='output', default=None,
        help='save plot to given file without bringing up window. Extension (e.g. pdf/png/eps/svg) can be specified in the file name or --format')
    parser.add_argument('--log', action='store_true', 
        help='initiate plot(s) with log of signal')
    parser.add_argument('--backend', dest='backend', 
        help='use non-default backend for matplotlib plot')

    args = parser.parse_args()

    mccode_config.load_config("user")

    main(args)

