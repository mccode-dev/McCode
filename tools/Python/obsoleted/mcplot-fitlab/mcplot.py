#!/usr/bin/env python
'''
mcplot script for testing the ifit interface.
'''
import argparse
import logging
import os
import sys

import numpy as np
import pyqtgraph as pg

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))
from mccodelib import pqtgfrontend
from mccodelib import plotgraph


'''
Local plotting functions
'''
def plot_iData_1D(data, plt, log=False, legend=True, icolormap=0, verbose=True, legend_fontsize=10):
    ''' create a plotItem and populate it with data, Data1D '''
    # data
    #x = np.array(data.axesvals).astype(np.float)
    x = data.axesvals[0]
    y = data.signal
    e = data.error

    if log:
        nonzeros=[]
        zeros=[]
        for i in range(len(y)):
            if y[i]>0:
                nonzeros.append(i)
            else:
                zeros.append(i)
        if len(nonzeros)>0:
            y[zeros] = np.min(y[nonzeros])/10
            plt.setLogMode(y=True)
        else:
            plt.setLogMode(y=False)
    else:
        plt.setLogMode(y=False)

    plt.setXRange(np.min(x), np.max(x), padding=0)

    # labels
    #plt.setLabels(title=" ",bottom=data.xlabel, left=data.ylabel)

    # how to add labels with html styling:
    #plt.titleLabel.item.setHtml('<span style="font-size:5pt; text-align:center;color:#FFFFFF">data.component <br>hest</span>')
    #axis_style = {'color': '#FFF', 'font-size': '5pt'}
    #plt.setLabel(axis='left', text=data.ylabel, **axis_style)
    #plt.setLabel(axis='bottom', text=data.xlabel, **axis_style)

    # error bars
    beam = 0
    if len(x) > 1:
        beam = (x[1]-x[0])*0.5

    # TODO: Find solution for adding errorbars in the log case
    if not log:
        err = pg.ErrorBarItem(x=x, y=y, height=e, beam=beam)
        plt.addItem(err)

    # plots data
    plt.plot(x, y)

    plt.setMenuEnabled(False)
    vb = plt.getViewBox()

    return vb

def plot_iData_2D(data, plt, log=False, legend=True, icolormap=0, verbose=True, legend_fontsize=10):
    ''' create a layout and populate a plotItem with data Data2D, adding a color bar '''
    # data
    img = pg.ImageItem()
    dataset = np.array(data.signal)
    x = data.axesvals[0]
    y = data.axesvals[1]
    dataset = np.transpose(dataset)
    
    img.setImage(dataset)
    
    # scale(x,y) is in %, translate(x,y) is in the original units
    dx = (np.max(x) - np.min(x))/len(x)
    dy = (np.max(y) - np.min(y))/len(y)
    img.scale(dx,dy)
    # Calculate translation in original pixel units
    img.translate(np.min(x)/dx, np.min(y)/dy)
    
    # color map (by lookup table)
    pos_min = np.min(dataset)
    pos_max = np.max(dataset)
    
    # color map
    cm = np.array([[  0,   0, 143, 255], [  0,   0, 159, 255], [  0,   0, 175, 255], [  0,   0, 191, 255], [  0,   0, 207, 255], [  0,   0, 223, 255], [  0,   0, 239, 255], [  0,   0, 255, 255], [  0,  16, 255, 255], [  0,  32, 255, 255], [  0,  48, 255, 255], [  0,  64, 255, 255], [  0,  80, 255, 255], [  0,  96, 255, 255], [  0, 112, 255, 255], [  0, 128, 255, 255], [  0, 143, 255, 255], [  0, 159, 255, 255], [  0, 175, 255, 255], [  0, 191, 255, 255], [  0, 207, 255, 255], [  0, 223, 255, 255], [  0, 239, 255, 255], [  0, 255, 255, 255], [ 16, 255, 239, 255], [ 32, 255, 223, 255], [ 48, 255, 207, 255], [ 64, 255, 191, 255], [ 80, 255, 175, 255], [ 96, 255, 159, 255], [112, 255, 143, 255], [128, 255, 128, 255], [143, 255, 112, 255], [159, 255,  96, 255], [175, 255,  80, 255], [191, 255,  64, 255], [207, 255,  48, 255], [223, 255,  32, 255], [239, 255,  16, 255], [255, 255,   0, 255], [255, 239,   0, 255], [255, 223,   0, 255], [255, 207,   0, 255], [255, 191,   0, 255], [255, 175,   0, 255], [255, 159,   0, 255], [255, 143,   0, 255], [255, 128,   0, 255], [255, 112,   0, 255], [255,  96,   0, 255], [255,  80,   0, 255], [255,  64,   0, 255], [255,  48,   0, 255], [255,  32,   0, 255], [255,  16,   0, 255], [255,   0,   0, 255], [239,   0,   0, 255], [223,   0,   0, 255], [207,   0,   0, 255], [191,   0,   0, 255], [175,   0,   0, 255], [159,   0,   0, 255], [143,   0,   0, 255], [128,   0,   0, 255]], dtype=np.ubyte)
    pos = pos_min + (pos_max - pos_min) * np.arange(len(cm))/(len(cm)-1)
    colormap = pg.ColorMap(pos, cm)

    lut = colormap.getLookupTable(pos_min, pos_max, 256)
    img.setLookupTable(lut)

    plt.setMenuEnabled(False)
    
    plt.addItem(img)
    # Set the x and y ranges correctly
    plt.getViewBox().setXRange(np.min(x), np.max(x), padding=0)
    plt.getViewBox().setYRange(np.min(y), np.max(y), padding=0)
    
    return plt.getViewBox()


def plot(node, i, plt, opts):
    '''
    plug-in function used by plotter ui called PyQtGraphFrontend
    
    node : plot node containing data
    i    : index of said data in node
    opts : dict containing options such as --> log, legend, icolormap, verbose, legend_fontsize
    '''
    if type(node) == plotgraph.PNSingle and i != 0:
        raise Exception("inconsistent plot request, idx=%s" % str(i))

    data = node.getdata_idx(i)

    plotfunc = None
    if len(data.axesvals) == 1:
        plotfunc = plot_iData_1D
    elif len(data.axesvals) == 2:
        plotfunc = plot_iData_2D
    else:
        raise Exception("three- or higher dimensional plotting not supported on this device")
    
    view_box = plotfunc(data, plt, log=opts['log'], legend=opts['legend'], icolormap=opts['icolormap'], verbose=opts['verbose'], legend_fontsize=opts['fontsize'])
    return view_box, plt


'''
iFit interface classes
'''
class IDataShadow:
    def __init__(self, signal, error, monitor, axesvals):
        self.signal = signal
        self.error = error
        self.monitor = monitor
        self.axesvals = axesvals


class IFuncShadow:
    def __init__(self, definition):
        self.definition = definition


class IFitInterfaceOfficial:
    '''
    The lowest level above matlab, with special functions for getting
    data out of iFit objects of type iData and iFunc.
    '''
    def __init__(self):
        import matlab.engine # official mathworks impl
        self.eng = matlab.engine.start_matlab('-nodesktop -nosplash', async=False)
        self.eng.eval("addpath(genpath('/home/jaga/source/REPO_ifit'))")

    def get_idata(self, varname):
        ''' load axes, signal and error from an ifit idata object '''
        ndims = self.eng.eval('ndims(%s)' % varname)
        ndims = int(ndims)
        
        signal = None
        error = None
        axes_names = self.eng.eval('%s.Axes' % varname, nargout=1) # NOTE: len(axes_names) == ndims
        axesvals = []
        
        if not ndims == len(axes_names):
            # TODO: handle this case seperately, in which ifit has not found any axes in the data
            raise Exception("ifit could not find axes")

        # get signal
        if ndims == 1:
            xvals = np.array(self.eng.eval('a.%s' % axes_names[0])[0]).astype(np.float)
            axesvals.append(xvals)
            
            signal = np.array(self.eng.eval('%s.Signal' % varname, nargout=1)).astype(np.float)
            signal = np.reshape(signal, (1, len(signal)))[0].tolist()
            error = np.array(self.eng.eval('%s.Error' % varname, nargout=1)).astype(np.float)
            error = np.reshape(error, (1, len(error)))[0]
            
            # TODO: what about monitor?
            #monitor = np.array(self.eng.eval('%s.Monitor' % varname, nargout=1))
            #monitor = np.reshape(monitor, (1, len(monitor)))[0]
            #monitor = None
        elif ndims == 2:
            xvals = np.array(self.eng.eval('a.%s' % axes_names[0])[0]).astype(np.float)
            yvals = np.array(self.eng.eval('a.%s' % axes_names[1])[0]).astype(np.float)
            axesvals.append(xvals)
            axesvals.append(yvals)

            signal = np.array(self.eng.eval('%s.Signal' % varname, nargout=1)).astype(np.float)
            error = np.array(self.eng.eval('%s.Error' % varname, nargout=1)).astype(np.float)
        else:
            for i in range(ndims):
                ivals = np.array(self.eng.eval('a.%s' % axes_names[i])[0]).astype(np.float)
                axesvals.append(ivals)
            signal = np.array(self.eng.eval('%s.Signal' % varname, nargout=1)).astype(np.float)
            error = np.array(self.eng.eval('%s.Error' % varname, nargout=1)).astype(np.float)
        
        return IDataShadow(signal, error, None, axesvals)

    def get_ifunc(self, varname):
        definition = self.eng.eval('%s.Signal' % varname, nargout=1)
        return IFuncShadow(definition)

    def get(self, varname):
        something = self.eng.eval('%s' % varname, nargout=1)
        return something

    def assign(self, varname, expression):
        self.eng.eval("%s = %s" % (varname, expression), nargout=0)

    def eval(self, expression):
        self.eng.eval("" % expression, nargout=1)


class IFitInterface:
    '''
    The lowest level above matlab, with special functions for getting
    data out of iFit objects of type iData and iFunc.
    '''
    def __init__(self):
        import matlab_ef # Emmanuel Farhi's matlab interface
        self.eng = matlab_ef.Matlab()

    def get_idata(self, varname):
        signal = np.array(self.eng.get('%s.Signal' % varname))
        error = np.array(self.eng.get('%s.Error' % varname))
        monitor = None # np.array(self.eng.eval('%s.Monitor' % varname))
        
        axes_names = self.eng.get('%s.Axes' % varname)
        firstaxes_vals = np.array(self.eng.get('a.%s' % axes_names))

        return IDataShadow(signal, error, monitor, firstaxes_vals)

    def get_ifunc(self, varname):
        definition = self.eng.get('%s.Signal' % varname)
        return IFuncShadow(definition)

    def get(self, varname):
        something = self.eng.get('%s' % varname)
        return something

    def assign(self, varname, expression):
        self.eng.eval("%s = %s" % (varname, expression))

    def eval(self, expression):
        self.eng.eval("" % expression)


'''
Local script classes
'''
class IFitLoaderSimple:
    '''
    loads 1D data from ifit and transforms this into a plotgraph.
    '''
    def __init__(self, datafile):
        '''  '''
        #import matlab_ef # Emmanuel Farhi's matlab interface
        self.datafile = datafile[0]
        self.plot_graph = None
        #self.interface = IFitInterface()
        self.interface = IFitInterfaceOfficial()
        if not os.path.exists(self.datafile):
            raise Exception("requested file to load does not exist")

    def load(self):

        i = self.interface

        i.assign('a', 'load(iData, \'%s\')' % self.datafile)

        #i.assign('a', 'load(iData, [ ifitpath \'Data/sv1850.scn\' ])')
        #i.assign('a', 'load(iData, \'\/home\/jaga\/source\/McCode\/tools\/Python\/mcplot\/fitlab\/100706.dat\')')

        #ap = eng.eval('a.Signal', nargout=1)
        iData = i.get_idata('a')

        graph = plotgraph.PNSingle(plotgraph.DataHandle(load_fct=None, data=iData))
        return graph


def main(args):
    ''' load data from iFit interface and send it to the pyqtgraph frontend '''
    logging.basicConfig(level=logging.INFO)

    # ensure keyboardinterrupt ctr-c
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    try:
        # load data
        loader = IFitLoaderSimple(datafile=args.datafile)
        graph = loader.load()

        # run pqtg frontend
        plotter = pqtgfrontend.McPyqtgraphPlotter(graph, sourcedir='/nosourcedir/', plot_func=plot, invcanvas=False)
        plotter.runplot()

    except KeyboardInterrupt:
        print('keyboard interrupt')
    except Exception as e:
        print('mcplot error: %s' % e.__str__())
        raise e


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('datafile', nargs='*', help='file to plot')
    args = parser.parse_args()

    main(args)
