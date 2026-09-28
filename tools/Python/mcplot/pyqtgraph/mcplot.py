#!/usr/bin/env python3
'''
pyqtgraph mcplot frontend, which uses the mcplot graph-based data loader.
'''
import argparse
import logging
import os
import sys
import subprocess

import qtpy
import plotfuncs

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

from mccodelib.mcplotloader import McCodeDataLoader, test_decfuncs
from mccodelib.plotgraph import PlotGraphPrint
from mccodelib import pqtgfrontend
from mccodelib import mccode_config


def main(args):
    ''' load data from mcplot backend and send it to the pyqtgraph frontend above '''
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
                # A NeXus-format scan (as opposed to a single simulation)
                # also writes a scan-summary mccode.dat alongside
                # mccode.h5. If it's there, also spawn a separate,
                # independent instance of this same mcplot variant pointed
                # directly at mccode.dat, running in the background
                # alongside the HDFVIEW.
                datfile = os.path.join(os.path.dirname(h5file), 'mccode.dat')
                if os.path.isfile(datfile):
                    try:
                        print('Also spawning %s on %s' % (os.path.basename(__file__), datfile))
                        subprocess.Popen([sys.executable, os.path.abspath(__file__), datfile])
                    except Exception as e:
                        print('Could not launch a second mcplot instance on ' + datfile + ': ' + e.__str__())
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
        
        # load data
        loader = McCodeDataLoader(simfile=simfile)
        try:
            loader.load()
        except Exception as e:
            # invallid input case:
            print('mcplot loader: ' + e.__str__())
            print(pqtgfrontend.get_help_string())
            quit()
        graph = loader.plot_graph
        if args.test:
            printer = PlotGraphPrint(graph)
        
        # run pqtg frontend
        plotter = pqtgfrontend.McPyqtgraphPlotter(
            graph, sourcedir=loader.directory,
            plot_func=plotfuncs.plot, invcanvas=args.invcanvas)
        print(pqtgfrontend.get_help_string())
        plotter.runplot()
        
    except KeyboardInterrupt:
        print('keyboard interrupt')
    except Exception as e:
        print('mcplot error: %s' % e.__str__())
        raise e


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('simulation', nargs='*', help='file or directory to plot')
    parser.add_argument('-t', '--test',  action='store_true', default=False, help='mccode data loader test run')
    parser.add_argument('--invcanvas', action='store_true', help='invert canvas background from black to white')
    args = parser.parse_args()

    mccode_config.load_config("user")
    
    main(args)

