'''
functionality for loading mccode data into suitable data types,
and assembling it in a plot-friendly way.
'''
import glob
import re
import os
from os.path import isfile, isdir, join, dirname, basename, splitext, exists
from os import walk
from decimal import Decimal

from .flowchart import FCNDecisionBool, FCNDecisionMulti, FCNProcess, FCNTerminal, FlowChartControl
from .plotgraph import PlotGraphPrint, DataHandle, PNMultiple, PNSingle


'''
McCode simulation output data types.
'''
class DataMcCode(object):
    ''' base type holding only the data object's title '''
    def __init__(self, *args, **kwargs):
        self.title = ''
        self.filepath = ''

    def __str__(self, *args, **kwargs):
        return self.title


class Data0D(DataMcCode):
    # Bring back empty 'component' field, courtesy
    # of the scan plotter (otherwise we fail abrubtly)
    DataMcCode.component=''
    pass


class Data1D(DataMcCode):
    ''' 1d plots use this data type '''
    def __init__(self):
        super(Data1D, self).__init__()

        self.component = ''
        self.filename = ''
        self.title = ''
        self.xlabel = ''
        self.ylabel = ''

        self.xvar = ''
        self.xlimits = () # pair

        self.variables = []

        self.yvar = () # pair
        self.values = () # triplet
        self.statistics = ''

        # data references
        self.xvals = []
        self.yvals = []
        self.y_err_vals = []
        self.Nvals = []


    def clone(self):
        data = Data1D()

        data.filepath = self.filepath

        data.component = self.component
        data.filename = self.filename
        data.title = self.title
        data.xlabel = self.xlabel
        data.ylabel = self.ylabel

        data.xvar = self.xvar
        data.xlimits = self.xlimits

        data.variables = self.variables

        data.yvar = self.yvar
        data.values = self.values
        data.statistics = self.statistics

        # data references
        data.xvals = self.xvals
        data.yvals = self.yvals
        data.y_err_vals = self.y_err_vals
        data.Nvals = self.Nvals

        return data


    def get_stats_title(self):
        '''I=.... Err=... N=...; X0=...; dX=...;'''
        try:
            stitle = '%s=%e Err=%e N=%d; %s' % (
                self.yvar[0], self.values[0], self.values[1],
                self.values[2], self.statistics)
        except:
            stitle = '%s of %s' % (self.yvar[0], self.xvar)
        return stitle


    def __str__(self):
        return 'Data1D, ' + self.get_stats_title()


class Data2D(DataMcCode):
    ''' PSD data type '''
    def __init__(self):
        super(Data2D, self).__init__()

        self.component = ''
        self.filename = ''
        self.title = ''

        self.xlabel = ''
        self.ylabel = ''

        self.xvar = ''
        self.yvar = ''
        self.zvar = ''
        self.xylimits = () # quadruple

        self.values = () # triplet
        self.statistics = '' # quadruple
        self.signal = ''

        # data references
        self.zvals = []
        self.counts = []


    def get_stats_title(self):
        '''I=.... Err=... N=...; X0=...; dX=...;'''
        stitle = '%s=%e Err=%e N=%d' % (
            self.zvar, self.values[0], self.values[1], self.values[2])
        return stitle


    def __str__(self):
        return 'Data2D, ' + self.get_stats_title()


''' 
Utility funcitons for loading and parsing mccode output files
'''
freetext_pat = '.+'

def _parse_1D_monitor(text):
    ''' populates data fields of new Data1D object using the text from a mccode data file '''
    data = Data1D()

    try:
        # load essential header data
        '''# component: Ldetector'''
        m = re.search(r'\# component: ([\w\.]+)', text)
        data.component = m.group(1)
        '''# filename: Edet.dat'''
        m = re.search(r'\# filename: ([\-\+\w\.\,]+)', text)
        data.filename = m.group(1)
        '''# title: Wavelength monitor'''
        m = re.search(r'\# title: (%s)' % freetext_pat, text)
        data.title = m.group(1)
        '''# xlabel: Wavelength [AA]'''
        m = re.search(r'\# xlabel: (%s)' % freetext_pat, text)
        data.xlabel = m.group(1)
        '''# ylabel: Intensity'''
        m = re.search(r'\# ylabel: (%s)' % freetext_pat, text)
        data.ylabel = m.group(1)

        '''# xvar: L'''
        m = re.search(r'\# xvar: ([\w]+)', text)
        data.xvar = m.group(1)
        '''# xlimits: 5.5 6.5'''
        m = re.search(r'\# xlimits: ([\d\.\-\+e]+) ([\d\.\-\+e]+)', text)
        data.xlimits = (float(m.group(1)), float(m.group(2)))

        '''# yvar: (I,I_err)'''
        m = re.search(r'\# yvar: \(([\w]+),([\w]+)\)', text)
        data.yvar = (m.group(1), m.group(2))

        '''# values: 6.72365e-17 4.07766e-18 4750'''
        m = re.search(r'\# values: ([\d\-\+\.e]+) ([\d\-\+\.e]+) ([\d\-\+\.e]+)', text)
        data.values = (Decimal(m.group(1)), Decimal(m.group(2)), float(m.group(3)))
        '''# statistics: X0=5.99569; dX=0.0266368;'''
        m = re.search(r'\# statistics: X0=([\d\.\-\+e]+); dX=([\d\.\-\+e]+);', text)
        data.statistics = 'X0=%.2E; dX=%.2E;' % (Decimal(m.group(1)), Decimal(m.group(2)))

        # load the actual data
        lines = text.splitlines()
        xvals = []
        yvals = []
        y_err_vals = []
        Nvals = []
        for l in lines:
            if '#' in l:
                continue

            vals = l.split()
            xvals.append(float(vals[0]))
            yvals.append(float(vals[1]))
            y_err_vals.append(float(vals[2]))
            Nvals.append(float(vals[3]))

        data.xvals = xvals
        data.yvals = yvals
        data.y_err_vals = y_err_vals
        data.Nvals = Nvals

    except Exception as e:
        print('Data1D load error.')
        raise e

    return data


def _parse_2D_monitor(text):
    data = Data2D()

    ''' populates data fields using the text from a mccode data file '''
    try:
        # load essential header data
        '''# component: detector'''
        m = re.search(r'\# component: ([\w]+)', text)
        if m:
            data.component = m.group(1)
        else:
            data.component = "(no comp name)"
        '''# filename: PSD.dat'''
        m = re.search(r'\# filename: ([\-\+\w\.\,]+)', text)
        data.filename = m.group(1)
        '''# title: PSD monitor'''
        m = re.search(r'\# title: (%s)' % freetext_pat, text)
        data.title = m.group(1)

        '''# xlabel: X position [cm]'''
        m = re.search(r'\# xlabel: (%s)' % freetext_pat, text)
        data.xlabel = m.group(1)
        '''# ylabel: Y position [cm]'''
        m = re.search(r'\# ylabel: (%s)' % freetext_pat, text)
        data.ylabel = m.group(1)

        '''# xvar: X'''
        m = re.search(r'\# xvar: (%s)' % freetext_pat, text)
        data.xvar = m.group(1)
        '''# yvar: Y '''
        m = re.search(r'\# yvar: (%s)' % freetext_pat, text)
        data.yvar = m.group(1)

        '''# zvar: I '''
        m = re.search(r'\# zvar: (%s)' % freetext_pat, text)
        data.zvar = m.group(1)
        '''
        # xylimits: -30 30 -30 30
        # xylimits: 0 5e+06 0.5 100
        '''
        m = re.search(r'\# xylimits: ([\d\.\-\+e]+) ([\d\.\-\+e]+) ([\d\.\-\+e]+) ([\d\.\-\+e]+)([\ \d\.\-\+e]*)', text)
        data.xlimits = (float(m.group(1)), float(m.group(2)), float(m.group(3)), float(m.group(4)))

        '''# values: 6.72365e-17 4.07766e-18 4750'''
        m = re.search(r'\# values: ([\d\+\-\.e]+) ([\d\+\-\.e]+) ([\d\+\-\.e]+)', text)
        data.values = (Decimal(m.group(1)), Decimal(m.group(2)), float(m.group(3)))
        '''# statistics: X0=5.99569; dX=0.0266368;'''
        m = re.search(r'\# statistics: X0=([\d\.\+\-e]+); dX=([\d\.\+\-e]+); Y0=([\d\.\+\-e]+); dY=([\d\.\+\-e]+);', text)

        data.statistics = 'X0=%.2E; dX=%.2E; Y0=%.2E; dY=%.2E;' % (Decimal(m.group(1)), Decimal(m.group(2)), Decimal(m.group(3)), Decimal(m.group(4)))
        '''# signal: Min=0; Max=1.20439e-18; Mean=4.10394e-21;'''
        m = re.search(r'\# signal: Min=([\ \d\.\+\-e]+); Max=([\ \d\.\+\-e]+); Mean=([\ \d\.\+\-e]+);', text)
        data.signal = 'Min=%f; Max=%f; Mean=%f;' % (float(m.group(1)), float(m.group(2)), float(m.group(3)))

        '''# Data [detector/PSD.dat] I:'''
        '''# Events [detector/PSD.dat] N:'''
        lines = text.splitlines()
        dat = True
        datacount=0;
        events = False
        for l in lines:
            if '# Data ' in l:
                datacount=datacount+1;
                # In case we meet mutiple # Data entries, file is probably saved via -USR2 or from 
                # Progress_bar(). In that case, better flush earlier data entries:
                if (datacount>1):
                    data.zvals=[]
                dat = True
                continue

            if '# Events ' in l:
                dat = False
                events = True
                continue

            if '# Errors ' in l:
                # NOTE: error values are not loaded
                dat = False
                events = False
                continue

            if dat:
                try:
                    vals = [float(item) for item in l.strip().split()]
                    data.zvals.append(vals)
                except:
                    pass

            if events:
                try:
                    vals = [float(item) for item in l.strip().split()]
                    data.counts.append(vals)
                except:
                    pass

    except Exception as e:
        print('Data2D load error.')
        raise e

    return data


def _load_monitor(monitorfile):
    ''' deferred loading: returns a data handle, which the user must call getdata() on to load the actual data '''
    def load(monfile):
        f = monfile
        if not f == 'No file':
            text = open(f).read()
            # determine 1D / 2D data

            m = re.search(r'\# type: (\w+)', text)
            typ = m.group(1)
            if typ == 'array_0d':
                print("load_monitor: Not loading 0d dataset %s" % monitorfile)
                data = Data0D()
            elif typ == 'array_1d':
                data = _parse_1D_monitor(text)
            elif typ == 'array_2d':
                data = _parse_2D_monitor(text)
            else:
                print('load_monitor: unknown data format %s' % typ)
                data = None
            data.filepath = f
            return data
        else:
            return Data0D()
    data = DataHandle(load_fct=lambda m=monitorfile: load(monfile=m))

    return data


def _get_filenames_from_mccodesim(mccodesim):
    dir = dirname(mccodesim)

    text = open(mccodesim).read()
    data_idx = text.find('begin data')
    filenames = []
    secs = text.split('begin data')
    for sec in secs[1:]:
        m = None
        for line in sec.splitlines():
            if m == None:
                m = re.search(r'filename: ([\w\.\,_\-+]+)\s*', line)
        if m: 
            filenames.append(join(dir, m.group(1)))
        else:
            filenames.append('No file')
    return filenames


def _load_data_from_mcfiles(filenames):
    data_lst = []
    for f in filenames:
        data = _load_monitor(f)
        data_lst.append(data)
    return data_lst


def _load_multiplot_1D_lst(f_dat):
    '''
    loads the one-dimensional 'multiplot' data sets from a mccode.dat scan sweep file,
    corresponding to each monitor as a function of the sweep parameter.
    '''
    text = open(f_dat).read()
    data_handle_lst = []

    try:
        header = Data1D()
        header.component = ''
        header.filename = 'mccode.dat'

        # NOTE: title this is overwritten below to be equal to yvar
        '''# title: Scan of lambda'''
        m = re.search(r'\# title: ([\w, ]+)', text)
        header.title = m.group(1)

        '''# xlabel: 'lambda\''''
        m = re.search(r'\# xlabel: ([\w \[\]\/\^\',]+)', text)
        header.xlabel = m.group(1).strip("\'")
        '''# ylabel: 'Intensity\''''
        m = re.search(r'\# ylabel: ([\w \[\]\/\^\',]+)', text)
        header.ylabel = m.group(1).strip("\'")

        # NOTE: this only supports a single xvar
        '''# xvars: lambda'''
        m = re.search(r'\# xvars: ([\w, ]+)', text)
        header.xvar = m.group(1).replace(',', '')
        num_xvars = len(header.xvar.split())

        '''# xlimits: 6 7'''
        m = re.search(r'\# xlimits: ([\d\.\-e]+) ([\d\.\-e]+)', text)
        header.xlimits = (float(m.group(1)), float(m.group(2)))

        '''# variables: lambda Ldetector_I Ldetector_ERR PSDrad_I PSDrad_ERR PSDrad_I PSDrad_ERR detector_I detector_ERR'''
        m = re.search(r'\# variables: ([\w ]+)', text)
        variables = m.group(1).split()

        '''# yvars: (AutoTOFL0_I,AutoTOFL0_ERR) (AutoTOF0_I,AutoTOF0_ERR) (AutoL0_I,AutoL0_ERR) ...'''
        m = re.search(r'\# yvars: ([\w \(\)\,]+)', text)
        unsplit = m.group(1)
        unsplit = unsplit.replace('(', ' ')
        unsplit = unsplit.replace(')', ' ')
        unsplit = unsplit.replace(',', ' ')
        yvars = unsplit.split()

        # get x and y values (the latter is a list of a list, the infamous yvals_lst which contains yvals values, which are lists)
        lines = text.splitlines()
        xvals = []
        yvals_lst = []
        yvals_err_lst = []

        for l in lines:
            if '#' in l:
                continue
            xvals.append(float(l.split()[0]))

            for i in range(len(yvars)//2):
                yvals_lst.append([])
                yvals_err_lst.append([])
                yvals_lst[i].append(float(l.split()[2*i+num_xvars]))
                yvals_err_lst[i].append(float(l.split()[2*i+num_xvars+1]))
        header.xvals = xvals

        # create a new instance for each y variable
        for i in range(len(yvars)//2):
            data = header.clone()
            data.yvals = yvals_lst[i]
            data.yvar = yvars[2*i]
            data.title = '%s' % (data.yvar)
            data.component = data.title
            data.y_err_vals = yvals_err_lst[i]
            data_handle_lst.append(DataHandle(load_fct=None, data=data))

    except Exception as e:
        print(e.__str__())
        raise e

    return data_handle_lst


def _load_sweep_monitors(rootdir):
    '''
    loads the files of a scan sweep into plotable datastructures
    '''
    def walkfunc(arg, dirname, fnames):
        mnames = []
        dirsignature = (dirname, mnames)
        for f in fnames:
            # NOTE: this will attempt to load all files except for mccode.sim
            if f not in mnames and f != 'mccode.sim' and f != 'mcstas.sim':
                mnames.append(f)
        arg.append(dirsignature)

    # get the subdirs somehow
    subdirtuple = []
    for root, dirs, files in walk(top=rootdir):
        walkfunc(subdirtuple, root, files)
    del subdirtuple[0] # remove root dir
    subdirs = [t[0] for t in subdirtuple]
    # get the right order of subdirs by recreating them a little bit
    subdirs = [join(dirname(subdirs[i]), str(i)) for i in range(len(subdirs))] # sortalpha(subdirs)

    # get the monitor ordering right by snooping the '  filename:' labels out of the scan point file 0/mccode.sim
    def get_subdir_monitors(subdir):
        mons = []
        if exists(join(subdir, 'mccode.sim')):
            indexfile='mccode.sim'
        elif exists(join(subdir, 'mccode.sim')):
            indexfile='mcstas.sim'
        else:
            return

        # Search for filenames - but also add dummy 'No file' in case of 0D monitor - fixed here
        # instead of in mccode itself for backward compatibility - otherwise Perl code barfs
        text = open(join(subdir, indexfile)).read()
        secs = text.split('begin data')
        for sec in secs[1:]:
            m = None
            for line in sec.splitlines():
                if m == None:
                    m = re.search(r'filename: ([\w\.\,_\-+]+)\s*', line)
                    if m: 
                        mons.append(join(subdir, m.group(1)))
                        break
            if not m:
                mons.append('No file')

        return mons

    monitors_by_subdir = []
    for s in subdirs:
        monitors_by_subdir.append(get_subdir_monitors(s))

    # notice that columns and rows are swapped, so we get to use a
    # list-of-lists data structure, with rows the same monitor
    sweep_monitors = [None]*len(monitors_by_subdir[0])
    for i in range(len(monitors_by_subdir[0])):          # N
        mon_lst = [None]*len(monitors_by_subdir)
        for j in range(len(monitors_by_subdir)):
            mon_lst[j] = _load_monitor(monitors_by_subdir[j][i])
        sweep_monitors[i] = mon_lst

    return sweep_monitors


'''
Flowchart functions for decision and terminal nodes (there are no process nodes in the implemented chart, 
although a cleaner implementation might include some).

Decision (bool) functions only probe disk contents. Terminal functions will load and interprit data files.
Therefore, any load errors are encountered at the terminal nodes' load data step.

NOTE: Functions can be context-dependent, according to the corresponding node position.
The function has_filename must be called first (see test_decfuncs).
'''

def is_dat_file(f):
    if not isfile(f):
        return False
    f = open(f, 'rb')
    line = f.readline().decode()
    m = re.match(r'\# Format\: McCode with text headers', line)
    if m:
        return True
    return False


def has_filename(args):
    f = args['simfile']
    if not isfile(f):
        if not isdir(f):
            if not f == '':
                raise Exception('Invalid input file.')
            else:
                f = '.'
        args['directory'] = f
        args['simfile'] = ''
        return False
    else:
        args['directory'] = dirname(f)
        return True


def is_mccodesim_or_mccodedat(args):
    f = args['simfile']
    f_name = basename(f)
    return (f_name == 'mccode.sim' or f_name == 'mcstas.sim' or f_name == 'mccode.dat') and isfile(f)


def is_monitorfile(args):
    f = args['simfile']
    ext = splitext(f)[1]
    if isfile(f):
        args['monitorfile'] = f
        return True
    else:
        return False


def is_sweepfolder(args):
    d = args['directory']

    if not isdir(d):
        return False
    dotsim = join(d, 'mccode.sim')
    dotdat = join(d, 'mccode.dat')
    return isfile(dotsim) and isfile(dotdat)


def is_broken_sweepfolder(args):
    ''' not implemented (returns trivial answer) '''
    return False


def is_sweep_data_present(args):
    ''' not implemented '''
    raise Exception('is_sweep_data_present has not been implemented.')


def is_mccodesim_w_monitors(args):
    f = args['simfile']
    d = args['directory']
    # checks mccode.sim existence
    if isfile(join(d, 'mccode.sim')):
        indexfile='mccode.sim'
    elif isfile(join(d, 'mcstas.sim')):
        indexfile='mcstas.sim'
    else:
        return False

    f = join(d, indexfile)
    args['simfile'] = f
    
    # look for any "unkonwn" files, could be data files
    datfiles = glob.glob(join(d, '*'))
    if 'mccode.sim' in datfiles: 
        datfiles.remove('mccode.sim')
    if 'mcstas.sim' in datfiles: 
        datfiles.remove('mcstas.sim')
    if 'mccode.dat' in datfiles: 
        datfiles.remove('mccode.dat')
    return len(datfiles) > 0


def has_datfile(args):
    d = args['directory']
    
    # look for any "unkonwn" files, could be data files
    datfiles = glob.glob(join(d, '*'))
    if 'mccode.sim' in datfiles: 
        datfiles.remove('mccode.sim')
    if 'mcstas.sim' in datfiles: 
        datfiles.remove('mcstas.sim')
    if 'mccode.dat' in datfiles: 
        datfiles.remove('mccode.dat')
    if len(datfiles) > 0:
        for f in datfiles:
            if not is_dat_file(f):
                return False
        args['monitorfile'] = datfiles[0]
        return True
    else:
        return False


def has_multiple_datfiles(args):
    d = args['directory']

    # look for any "unkonwn" files, could be data files
    datfiles = glob.glob(join(d, '*'))
    if 'mccode.sim' in datfiles:
        datfiles.remove('mccode.sim')
    if 'mcstas.sim' in datfiles:
        datfiles.remove('mcstas.sim')
    if 'mccode.dat' in datfiles:
        datfiles.remove('mccode.dat')
    for f in datfiles:
        if not is_dat_file(f):
            return False
    return len(datfiles) > 1


def test_decfuncs(simfile):
    ''' calls all decision functions in the node tree '''
    args = {}
    args['simfile'] = simfile

    print('has_filename:              %s' % str(has_filename(args)))
    print('is_mccodesim_or_mccodedat: %s' % str(is_mccodesim_or_mccodedat(args)))
    print('is_monitorfile:            %s' % str(is_monitorfile(args)))
    print('is_sweepfolder:            %s' % str(is_sweepfolder(args)))
    print('is_broken_sweepfolder:     %s' % str(is_broken_sweepfolder(args)))
    #print('is_sweep_data_present:    %s' % str(is_sweep_data_present(args))) # should not be called until implemented
    print('is_mccodesim_w_monitors:   %s' % str(is_mccodesim_w_monitors(args)))
    print('has_datfile:               %s' % str(has_datfile(args)))
    print('has_multiple_datfiles:     %s' % str(has_multiple_datfiles(args)))



'''
Terminal load functions - calls data load utilities, assembles and returns data graph.
'''
def load_monitor(args):
    # assume monitorfile is present and exists
    data = _load_monitor(args['monitorfile'])

    # plot graph only has one node in this case
    root = PNSingle(data)

    return root


def load_simulation(args):
    # assume simfile is mccode.sim
    f = args['simfile']
    d = args['directory']

    # load monitor data handles
    if isfile(join(d, 'mccode.sim')):
        indexfile='mccode.sim'
    elif isfile(join(d, 'mcstas.sim')):
        indexfile='mcstas.sim'
    else:
        indexfile=''
    data_lst = _load_data_from_mcfiles(_get_filenames_from_mccodesim(join(d, indexfile)))

    # construct two-level plot graph
    root = PNMultiple(data_lst)
    primnodes = []
    for data in data_lst:
        node = PNSingle(data)
        primnodes.append(node)
    root.set_primaries(primnodes)
    root.set_secondaries(primnodes) # there is only one way to click here...could also be None

    return root


def load_sweep(args):
    d = args['directory']
    f_dat = join(d, 'mccode.dat')
    if isfile(join(d, 'mcstas.sim')):
        f_dat = join(d, 'mcstas.sim')

    # load primary data_handle, 1D sweep values
    data_handle_lst_sweep1D = _load_multiplot_1D_lst(f_dat)
    root = PNMultiple(data_handle_lst_sweep1D)

    # primary nodes (zoom on 1D sweep values)
    primnodes_lst = []
    for data_handle in data_handle_lst_sweep1D:
        primnode = PNSingle(data_handle)
        primnodes_lst.append(primnode)
    root.set_primaries(primnodes_lst)

    # load secondary data_handle, sweep data_handle points organized by monitor
    monitors = _load_sweep_monitors(d)

    # secondary modes (individual sweep monitors)
    secnodes_lst = []
    for i in range(len(monitors)):
        m_lst = monitors[i]
        secnode = PNMultiple(m_lst)
        children = []
        for m in m_lst:
            child = PNSingle(m)
            children.append(child)
        secnode.set_primaries(children)
        secnode.set_secondaries(children)
        secnodes_lst.append(secnode)
    root.set_secondaries(secnodes_lst)

    # lengths of the root, primary and secondary plot item sets
    root_len = len(data_handle_lst_sweep1D)
    prim_len = len(primnodes_lst)
    sec_len = len(secnodes_lst)

    # ensure that root, primary and secondary plot items match
    # (e.g., remove superfluous event files from secondary plot items)
    plt_idx = 0
    while plt_idx < root_len:
        if plt_idx >= len(primnodes_lst) or plt_idx >= len(secnodes_lst):
            print("Warning: Too few primary/secondary plot items.")
            break

        root_yvar = root.getdata_idx(plt_idx).yvar
        prim_yvar = primnodes_lst[plt_idx].getdata_idx(0).yvar
        sec_prims = secnodes_lst[plt_idx].get_primaries()

        # check primary plot items
        if root_yvar != prim_yvar:
            print("Warning: Root (%s) and primary (%s) plot items do not match." % (
		root_yvar, prim_yvar))

        # check secondary plot items
        remove_secnode = False
        if len(sec_prims) == 0:
            remove_secnode = True
        else:
            # check if the component name for a secondary plot item corresponds
            # to the yvar names of the root and primary plot items
            sec_yvar = sec_prims[0].getdata_idx(0).component
            if not root_yvar.startswith(sec_yvar):
                remove_secnode = True

        # remove non-matching secondary plot item
        if remove_secnode:
            secnodes_lst.pop(plt_idx)
            sec_len -= 1
            continue

        #print("Plot %d: root: %s, 1st: %s, 2nd: %s." % (plt_idx,
        #    root_yvar, prim_yvar, sec_prims[0].getdata_idx(0).filename))

        plt_idx += 1

    # remove remaining superfluous secondary plot items
    while sec_len > root_len:
        secnodes_lst.pop()
        sec_len -= 1

    # length checks
    if root_len != prim_len:
        print("Warning: Unequal number of root (%d) and primary (%d) plot items."
            % (root_len, prim_len))
    if root_len != sec_len:
        print("Warning: Unequal number of root (%d) and secondary (%d) plot items."
            % (root_len, sec_len))

    return root


def load_sweep_b(args):
    raise Exception('load_sweep_b is not implemented.')


def load_sweep_c(args):
    raise Exception('load_sweep_c is not implemented.')


def load_monitor_folder(args):
    # assume simfile is folder with multiple dat files
    d = args['directory']

    # load monitor files into a list
    datfiles = glob.glob(join(d, '*'))
    data_lst = _load_data_from_mcfiles(datfiles)

    # construct two-level plot graph
    root = PNMultiple(data_lst)
    primnodes = []
    for data in data_lst:
        node = PNSingle(data)
        primnodes.append(node)
    root.set_primaries(primnodes)
    root.set_secondaries(primnodes)

    return root


def throw_error(args):
    raise Exception('Could not load "%s".' % args['simfile'])


class McCodeDataLoader():
    ''' assembly and execution of mccode data loader flowchart '''
    def __init__(self, simfile):
        '''  '''
        self.simfile = simfile
        self.plot_graph = None

    def load(self):
        ''' loads mccode data and assembles the plotable data graph '''

        # possible exit terminals
        exit_term_error  = FCNTerminal(key = "error",  fct = throw_error)
        exit_term_case1  = FCNTerminal(key = "case1",  fct = load_monitor)
        exit_term_case2  = FCNTerminal(key = "case2",  fct = load_simulation)
        exit_term_case3  = FCNTerminal(key = "case3",  fct = load_sweep)
        exit_term_case3b = FCNTerminal(key = "case3b", fct = throw_error)
        exit_term_case3c = FCNTerminal(key = "case3c", fct = throw_error)
        exit_term_case4  = FCNTerminal(key = "case4",  fct = load_monitor_folder)

        # decision nodes (assembled in backwards order)
        dec_multiplefiles        = FCNDecisionBool(fct = has_multiple_datfiles,
                                                   node_T = exit_term_case4,
                                                   node_F = exit_term_case1)
        dec_hasdatfile           = FCNDecisionBool(fct = has_datfile,
                                                   node_T = dec_multiplefiles,
                                                   node_F = exit_term_error)
        dec_ismccodesimwmonitors = FCNDecisionBool(fct = is_mccodesim_w_monitors,
                                                   node_T = exit_term_case2,
                                                   node_F = dec_hasdatfile)
        dec_datafolderspresent   = FCNDecisionBool(fct = is_sweep_data_present,
                                                   node_T = exit_term_case3b,
                                                   node_F = exit_term_case3c)
        dec_isbrokensweep        = FCNDecisionBool(fct = is_broken_sweepfolder,
                                                   node_T = dec_datafolderspresent,
                                                   node_F = dec_ismccodesimwmonitors)
        dec_issweepfolder        = FCNDecisionBool(fct = is_sweepfolder,
                                                   node_T = exit_term_case3,
                                                   node_F = dec_isbrokensweep)
        dec_ismonitor            = FCNDecisionBool(fct = is_monitorfile,
                                                   node_T = exit_term_case1,
                                                   node_F = exit_term_error)
        dec_ismccodesimordat     = FCNDecisionBool(fct = is_mccodesim_or_mccodedat,
                                                   node_T = dec_issweepfolder,
                                                   node_F = dec_ismonitor)
        dec_hasfilename          = FCNDecisionBool(fct = has_filename,
                                                   node_T = dec_ismccodesimordat,
                                                   node_F = dec_issweepfolder)

        # enter terminal node
        enter_term_simfile = FCNTerminal(key = 'enter', node_next = dec_hasfilename)

        # get the "args" which is just the simfile, a string,
        # which may correspond to a file or a folder
        args = {}
        args['simfile'] = self.simfile

        # traverse the flow chart
        control = FlowChartControl(terminal_enter = enter_term_simfile)
        output_node = control.process(args=args)

        self.plot_graph = output_node.result

        self.directory = args['directory']
