from os.path import basename
from log import McRunException, getLogger
from datetime import datetime
from decimal import Decimal
from os.path import join 
from multiprocessing import Pool
import copy

try:
  from scipy.optimize import minimize
  from numpy import sqrt, zeros, exp
except:
  # Optimizer class not available
  pass

LOG = getLogger('optimisation')


def build_header(options, params, intervals, detectors):
    template = """
# Instrument-source: '%(instr)s'
# Date: %(date)s
# Ncount: %(ncount)i
# Numpoints: %(numpoints)i
# Param: %(params)s
# type: %(type)s
# title: %(title)s
# xlabel: '%(xvars)s'
# ylabel: 'Intensity'
# xvars: %(xvars)s
# yvars: %(yvars)s
# list: %(xvals)s
# xlimits: %(xmin)s %(xmax)s
# filename: %(filename)s
# variables: %(variables)s
    """.strip()

    # Date format: Fri Aug 26 12:21:39 2011
    date = datetime.strftime(datetime.now(), '%a %b %d %H %M %Y')

    # Strip header keys of -- (to make --seed -> seed
    hdrparams = {key.lstrip('-') for key in params}
    xvars = ', '.join(hdrparams)
    lst = intervals[list(params)[0]]
    if options.list:
        xmin=1
        xmax=len(lst)
    else:
        xmin = min(lst)
        xmax = max(lst)
    # Get Numpoints from length of -L list
    N = len(lst)
    # ... or using options.numponts if in fact a normal scan
    if options.numpoints:
        N = options.numpoints

    # TODO: figure out correct scan type
    if options.optimize:
        N =  1
        title = 'Optimization of %s' % xvars
    else:
        title = 'Scan of %s' % xvars

    scantype = 'multiarray_1d(%d)' % N

    variables = list(hdrparams)
    for detector in detectors:
        variables += [detector + '_I', detector + '_ERR']

    values = {
        'instr': options.instr,
        'date': date,

        'ncount': options.ncount,
        'numpoints': N,

        'params': ', '.join('%s = %s' % (xvar, intervals[xvar][0])
                            for xvar in params),
        'type': scantype,
        'title': title,

        'xvars': xvars,
        'yvars': ' '.join('(%s_I,%s_ERR)' % (d, d) for d in detectors),

        'xvals': str(lst),

        'xmin': xmin,
        'xmax': xmax,

        'filename': basename(options.optimise_file),
        'variables': ' '.join(variables),
    }
    
    result = (template % values) + '\n'
    return result


def build_mccodesim_header(options, intervals: dict, detectors: list, version: str):
    template = """
begin instrument:
  Creator: %(version)s
  Source: %(instr)s
  Parameters:  %(xvars)s
  Trace_enabled: %(istrace)s
  Default_main: yes
  Embedded_runtime: yes
end instrument

begin simulation
Date: %(date)s
Ncount: %(ncount)i
Numpoints: %(scanpoints)i
Param: %(params)s
end simulation

begin data
type: multiarray_1d(%(scanpoints)i)
title: %(title)s
xvars: %(xvars)s
yvars: %(yvars)s
xlabel: '%(xvars)s'
ylabel: 'Intensity'
xlimits: %(xmin)s %(xmax)s
filename: %(filename)s
variables: %(variables)s
end data
    """.strip()
    interval_names = ', '.join(intervals.keys())
    first_key_interval = intervals[list(intervals.keys())[0]]

    # TODO: figure out correct scan type
    numpoints = 1 if options.optimize else options.numpoints

    # -L list scan: use the position (1..N) within the list, matching
    # build_header()'s existing convention for -L scans above - meaningful
    # for a non-numeric list (e.g. filenames), where a literal min()/max()
    # of the raw strings would be lexicographic and essentially
    # meaningless, and harmless for a numeric one (the actual per-point
    # values are written into mccode.dat itself; this is just the
    # header's overall axis-range hint). Equidistant (-N/-M, non-list)
    # scans are untouched, keeping their existing min()/max() behaviour.
    if options.list:
        xmin, xmax = 1, len(first_key_interval)
    else:
        xmin, xmax = min(first_key_interval), max(first_key_interval)

    values = {
        'instr': options.instr,
        'date': datetime.strftime(datetime.now(), '%a %b %d %H %M %Y'),

        'ncount': options.ncount,
        'scanpoints': numpoints,

        'params': ', '.join(f'{key} = {val}' for (key, vals) in intervals.items() for val in vals),
        'type': f'multiarray_1d({numpoints})',
        'title': f'{"Optimization" if options.optimize else "Scan"} of {interval_names}',

        'xvars': interval_names,
        'yvars': ' '.join(f'({d}_I,{d}_ERR' for d in detectors),

        'xmin': xmin,
        'xmax': xmax,

        'filename': basename(options.optimise_file) or 'mccode.dat',
        'variables': ' '.join(intervals.keys()) + ' '.join(f'{d}_I {d}_ERR' for d in detectors),
        
        'version': version,
        'istrace': 'yes' if options.trace else 'no'
    }
    
    result = (template % values) + '\n'
    return result


def mcsimdetectors(directory_name: str):
    """Read back detector (name, intensity, error, ray count, data file name) sets from a mccode.sim file"""
    # TODO this function should be kept synchronized with build_mccode_header above
    from pathlib import Path
    from mccode import Detector
    directory = Path(directory_name)
    if not directory.exists() and directory.is_dir():
        raise RuntimeError(f"{directory_name} is not a directory")
    filepath = directory.joinpath('mccode.sim')
    hdfpath  = directory.joinpath('mccode.h5')
    if not filepath.exists() and hdfpath.exists():
        return
    if not filepath.exists():
        raise RuntimeError(f'The simulation file {filepath} does not exist')
    with filepath.open('r') as file:
        contents = file.read()
    # Each detector has a block between "begin data" and "end data"
    blocks = [x.split('end data')[0].strip() for x in contents.split('begin data') if 'end data' in x]
    # with lines of the form "{key}: {value}"
    blocks = [{k.strip(): v.strip() for k, v in [z.split(':', 1) for z in b.split('\n')]} for b in blocks]
    # This object only cares about extracting the (name, I, Err, N, data file) sets for each detector
    return [Detector(d['component'], *d['values'].split(), d['filename'], d['statistics']) for d in blocks]


def point_at(N, key, minmax, step):
    """ Helper to compute the point for key at step """
    low, high = map(Decimal, minmax)
    return step * (high - low) / Decimal(N - 1) + low


def resolve_scan_value(key, value, intervals):
    """ Returns a numeric representation of one scanned parameter's value
        for one scan point, for writing into mccode.dat's per-point data
        row (mccode.dat's format is a matrix of numbers - see module
        docstring/build_header() - so every column needs one, regardless
        of what kind of value the parameter itself actually is).

        A genuinely numeric value (the overwhelming majority of scans, and
        the only kind LinearInterval/MultiInterval.from_range() ever
        produce) passes straight through unchanged - this function has no
        effect at all outside of an -L/--list scan with a non-numeric
        list.

        A non-numeric value (e.g. a -L scan like
        filename=Na2Ca3Al2F14.laz,YBaCuO.lau,Fe.laz,Cu.laz) is replaced by
        its own *index* within intervals[key] - the position it appears
        at in the original -L list, e.g. that list gives indices 0,1,2,3
        respectively - so mccode.dat keeps a properly numeric column for
        this parameter too, and remains plottable against it (as a
        categorical/index axis) rather than needing the actual string
        embedded in a number matrix.

        Each scanned parameter is resolved independently, unlike the
        previous behaviour of collapsing the ENTIRE row down to a single
        step-index the moment ANY ONE scanned parameter was non-numeric -
        which silently discarded every OTHER parameter's real value too
        (numeric ones included), and produced only one parameter column
        regardless of how many were actually being scanned - a mismatch
        against the header's declared xvars/variables count that broke
        every downstream plotting tool, since they parse a fixed number
        of parameter columns based on that count. """
    try:
        return float(value)
    except (TypeError, ValueError):
        pass
    try:
        return float(list(intervals[key]).index(value))
    except (KeyError, ValueError):
        # value isn't literally in intervals[key] (shouldn't normally
        # happen, since scan points are always built FROM intervals[key] -
        # but fall back to a stable value rather than crashing outright)
        return float(abs(hash(value)) % 1000000)


class LinearInterval:
    """ Intervals for linear scanning """

    @staticmethod
    def from_range(N, intervals):
        print(f"LinearInterval from {N=} and {intervals=}")
        for step in range(N):
            yield dict((key, point_at(N, key, intervals[key], step))
                       for key in intervals)

    @staticmethod
    def from_list(N, intervals):
        print(f"LinearInterval from_list {N=} and {intervals=}")
        for step in range(N):
            yield dict((key, intervals[key][step]) for key in intervals)


class MultiInterval:
    """ Points for multi-dimensional scanning """

    @staticmethod
    def from_range(N, intervals):
        """ N is either a single int (the same point count applied to
            every scanned dimension - the original behaviour) or a dict
            mapping each interval key to its own point count (mcrun's
            -N=a,b,c,... list-form, only valid together with -M, letting
            different parameters be sampled at different resolutions -
            e.g. a coarse 3-point sweep on one axis against a fine
            20-point sweep on another). """
        print(f"MultiInterval from {N=} and {intervals=}")
        # base case: no intervals yields empty dict
        if len(intervals) == 0:
            yield {}
            return
        # recursively generate the multi dict
        intervals = intervals.copy()
        key, minmax = intervals.popitem()
        n_here = N[key] if isinstance(N, dict) else N
        for step in range(n_here):
            point = point_at(n_here, key, minmax, step)
            for dic in MultiInterval.from_range(N, intervals):
                dic[key] = point
                yield dic

    @staticmethod
    def from_list(intervals):
        """ Cartesian product across each key's own explicit list of
            points (mcrun's -L/--list combined with -M/--multi). Unlike
            LinearInterval.from_list() (co-linear: every key's list is
            walked together in lockstep, so all lists must be the same
            length), each key here is varied independently, so the lists
            may have different lengths - which is also how different
            parameters naturally end up with different numbers of scan
            points in this mode, without needing a separate -N. """
        print(f"MultiInterval from_list {intervals=}")
        if len(intervals) == 0:
            yield {}
            return
        intervals = intervals.copy()
        key, values = intervals.popitem()
        for value in values:
            for dic in MultiInterval.from_list(intervals):
                dic[key] = value
                yield dic


class InvalidInterval(McRunException):
    pass

def _simulate_point(args):
    i, point, intervals, mcstas_config, mcstas_dir = args

    from shutil import copyfile
    from os.path import join

    # Make a new instance of McStas and configure it
    mcstas = copy.deepcopy(mcstas_config)  # You need to define a way to deepcopy or clone your mcstas object
    par_values = []

    # Ensure we get a mccode.sim pr. thread subdir (e.g. for monitoring seed value
    mcstas.simfile      = join(mcstas_dir, 'mccode.sim')

    # Shift thread seed to avoid duplicate simulations / biasing
    mcstas.options.seed = (i*1024)+mcstas.options.seed

    for key in intervals:
        mcstas.set_parameter(key, point[key])
        # set_parameter() above needs the real value (a genuine instrument
        # filename parameter needs the actual string, not an index) - only
        # what goes into the OUTPUT ROW (par_values, eventually written to
        # mccode.dat) needs the numeric-or-index resolution. Unlike
        # Scanner.run() (only reachable for -L scans), this path is shared
        # with plain equidistant multi-dim scans too, but
        # resolve_scan_value() is a no-op for those - every value is
        # already numeric there.
        par_values.append(resolve_scan_value(key, point[key], intervals))

    current_dir = f'{mcstas_dir}/{i}'
    mcstas.run(pipe=False, extra_opts={'dir': current_dir})
    detectors = mcsimdetectors(current_dir)

    result = {
        'index': i,
        'params': par_values,
        'detectors': detectors
    }
    return result

class Scanner:
    """ Perform a series of simulation steps along a given set of points"""
    def __init__(self, mcstas, intervals):
        self.mcstas = mcstas
        self.intervals = intervals
        self.points = None
        self.outfile = mcstas.options.optimise_file
        self.simfile = join(mcstas.options.dir, 'mccode.sim')

    def set_points(self, points):
        self.points = points

    def set_outfile(self, path):
        self.outfile = path

    def run(self):
        LOG.info('Running Scanner, result file is "%s"' % self.outfile)

        if len(self.intervals) == 0:
            raise InvalidInterval('No interval range specified')

        # each run will be in "dir/1", "dir/2", ...
        mcstas_dir = self.mcstas.options.dir
        if mcstas_dir == '':
            mcstas_dir = '.'

        with open(self.outfile, 'w') as outfile:
            for i, point in enumerate(self.points):
                par_values = []
                for key in self.intervals:
                    self.mcstas.set_parameter(key, point[key])
                    LOG.debug("%s: %s", key, point[key])
                    par_values.append(point[key])

                if not self.mcstas.options.format.lower() == 'nexus':
                    LOG.info(', '.join(f'{name}: {value}' for name, value in point.items()))
                    # Change subdirectory as an extra option (dir/1 -> dir/2)
                    current_dir = f'{mcstas_dir}/{i}'
                    LOG.info(f"Output step into scan directory {current_dir}")
                    self.mcstas.run(pipe=False, extra_opts={'dir': current_dir})
                else:
                    current_dir = mcstas_dir
                    LOG.info(f"NeXus output step into scan directory {current_dir}")
                    self.mcstas.options.append=True
                    self.mcstas.run(pipe=False, extra_opts={'dir': current_dir})

                LOG.info("Finish running step, get detectors")
                detectors = mcsimdetectors(current_dir)
                if detectors is not None:
                    LOG.info("Got detectors")
                    if i == 0:
                        LOG.info("Write headers")
                        names = [det.name for det in detectors]
                        outfile.write(build_header(self.mcstas.options, self.intervals.keys(), self.intervals, names))
                        # Opening a file inside of this loop seems like a bad idea ... oh well
                        with open(self.simfile, 'w') as simfile:
                            simfile.write(build_mccodesim_header(self.mcstas.options, self.intervals, names,
                                                                version=self.mcstas.version))
                        LOG.info("Wrote headers")
                    LOG.info(f"Write step detectors line into {self.outfile}")
                    values = ['%s %s' % (d.intensity, d.error) for d in detectors]

                    if not self.mcstas.options.list:
                        # Normal equidistant scan: LinearInterval/MultiInterval
                        # .from_range() only ever produce numeric values, so
                        # this is unchanged.
                        line = '%s %s\n' % (' '.join(map(str, par_values)), ' '.join(values))
                    else:
                        # -L list scan: resolve each scanned parameter's
                        # value independently (see resolve_scan_value()) -
                        # a genuinely numeric value passes straight
                        # through, and only a non-numeric one (e.g. a
                        # filename) becomes its own index within that
                        # parameter's own list, keeping one proper numeric
                        # column per scanned parameter either way.
                        resolved = [resolve_scan_value(key, val, self.intervals)
                                    for key, val in zip(self.intervals.keys(), par_values)]
                        line = '%s %s\n' % (' '.join(map(str, resolved)), ' '.join(values))
                    outfile.write(line)
                    outfile.flush()


class Scanner_split:
    """ Perform a series of simulation steps along a given set of points,
        Where each simulation is controlled by its own thread. """
    def __init__(self, mcstas, intervals, nb_cpu):
        self.mcstas = mcstas
        self.intervals = intervals
        self.points = None
        self.nb_cpu = nb_cpu
        self.outfile = mcstas.options.optimise_file
        self.simfile = join(mcstas.options.dir, 'mccode.sim')

    def set_points(self, points):
        self.points = points

    def set_outfile(self, path):
        self.outfile = path

    def run(self):
        LOG.info('Running Scanner split, result file is "%s"' % self.outfile)

        if len(self.intervals) == 0:
            raise InvalidInterval('No interval range specified')

        mcstas_dir = self.mcstas.options.dir or '.'

        if self.mcstas.options.seed is None:
          dt=datetime.now()
          LOG.info('No incoming seed from cmdline, setting to current Unix epoch (%d)!' % dt.timestamp())
          self.mcstas.options.seed=dt.timestamp()

        # Prepare data to pass into processes
        args_list = [
            (i, point, self.intervals, self.mcstas, mcstas_dir)
            for i, point in enumerate(self.points)
        ]

        with Pool(processes=self.nb_cpu) as pool:
            results = pool.map(_simulate_point, args_list)

        # Sort results to preserve order
        results.sort(key=lambda r: r['index'])

        with open(self.outfile, 'w') as outfile:
            wrote_headers = False
            for result in results:
                if result['detectors'] is None:
                    continue

                if not wrote_headers:
                    names = [d.name for d in result['detectors']]
                    outfile.write(build_header(self.mcstas.options, self.intervals.keys(), self.intervals, names))
                    with open(self.simfile, 'w') as simfile:
                        simfile.write(build_mccodesim_header(
                            self.mcstas.options,
                            self.intervals,
                            names,
                            version=self.mcstas.version
                        ))
                    wrote_headers = True

                values = ['%s %s' % (d.intensity, d.error) for d in result['detectors']]
                line = '%s %s\n' % (' '.join(map(str, result['params'])), ' '.join(values))
                outfile.write(line)
                outfile.flush()


class Optimizer:
    """ Optimize monitors by varying the parameters within interval """

    def __init__(self, mcstas, intervals):
        self.mcstas       = mcstas
        self.intervals    = intervals
        self.points       = None
        self.outfile      = mcstas.options.optimise_file # e.g. mccode.dat
        self.simfile      = join(mcstas.options.dir, 'mccode.sim')
        self.iterations   = 0
        self.wrote_header = False
        self.parsHistory  = []
        self.criteriaHistory = []

    def run(self):
        """ Optimization procedure """

        LOG.info('Running Optimizer, result file is "%s"' % self.outfile)

        if len(self.intervals) == 0:
            raise InvalidInterval('No interval range specified')

        # determine starting parameter set
        pars_start, bounds = self.get_start()

        # handle options
        options={'disp':True}
        if self.mcstas.options.optimize_maxiter:
            options["maxiter"] = self.mcstas.options.optimize_maxiter
        if self.mcstas.options.optimize_tol:
            options["tol"] = self.mcstas.options.optimize_tol

        # call scipy.optimize.minimize
        try:
            result = minimize(
                McCode_runner, pars_start,
                args   = self,
                method = self.mcstas.options.optimize_method,
                bounds = bounds,
                options= options)
        except (NameError,ImportError) as err:
            print("ERROR: mcrun --optimize is not available as scipy is not installed.")
            raise err

        # estimate uncertainties
        uncertainties = self.estimate_error_history(self.criteriaHistory, result.x, self.parsHistory)

        LOG.info("Parameter uncertainties:\n")
        for i,key in enumerate(self.intervals):
            LOG.info('%s = %f ± %f'% (key, result.x[i], uncertainties[i]))

    def get_start(self):
        """ Get starting parameters from the instrument parameters intervals """

        pars_start = []
        bounds     = []

        # we iterate on intervals.keys() and .values()
        for key in self.intervals:
            values=self.intervals[key]
            values = [float(x) for x in values]
            if len(values) == 2:
                pars_start.append((values[0]+values[1])/2)
                par_min = values[0]
                par_max = values[1]
            elif len(values) == 3:
                pars_start.append(values[1])
                par_min = values[0]
                par_max = values[2]
            else:
                raise InvalidInterval('Optimization interval for %s must be min,max or min,start,max' % key)
            bounds.append( (par_min,par_max) )

        return pars_start, bounds

    def estimate_error_history(self, criteriaHistory, parsBest, parsHistory):
        """ Estimate errors from the history """

        criteriaHistory        = [float(x) for x in criteriaHistory]
        parsHistoryUncertainty = parsBest*0
        parsWeightSum          = 0
        minCriteria            = min(criteriaHistory)

        for index in range(len(parsHistory)):
            # difference of parameters around optimum
            delta_pars    = parsHistory[index] - parsBest

            # Gaussian weighting for the parameter set
            weight_pars   = exp(-((criteriaHistory[index]-minCriteria))**2 / 8)
            parsWeightSum = parsWeightSum+weight_pars

            parsHistoryUncertainty = parsHistoryUncertainty + (delta_pars*delta_pars*weight_pars)

        # sqrt(sum(delta_pars.*delta_pars.*weight_pars)./sum(weight_pars))
        parsHistoryUncertainty = sqrt(parsHistoryUncertainty/parsWeightSum)

        return parsHistoryUncertainty

# ------------------------------------------------------------------------------
def McCode_runner(x, args):
    """ Launch a single optimization step, calling McStas.run() """

    # Change subdirectory as an extra option (dir/1 -> dir/2)
    # each run will be in "dir/1", "dir/2", ...
    mcstas_dir = args.mcstas.options.dir
    if mcstas_dir == '':
        mcstas_dir ='.'
    current_dir = '%s/%i' % (mcstas_dir, args.iterations)

    # must now set instrument parameters to 'x'
    for index,key in enumerate(args.intervals):
        args.mcstas.set_parameter(key, x[index])

    args.parsHistory.append(x)

    args.mcstas.run(pipe=False, extra_opts={'dir': current_dir})

    # track iteration number
    args.iterations = args.iterations+1

    # get monitors out, compute criteria
    detectors = mcsimdetectors(current_dir)
    values = []

    # add monitors that match a given name
    for d in detectors:
        if d.name in args.mcstas.options.optimize_monitor:
            if args.mcstas.options.optimize_eval:
              values.append(eval(args.mcstas.options.optimize_eval))
            else:
              values.append(d.intensity)
    # in case monitor name is not found, we use all monitor values
    if len(values) == 0:
        for d in detectors:
            if args.mcstas.options.optimize_eval:
              values.append(eval(args.mcstas.options.optimize_eval))
            else:
              values.append(d.intensity)

    values = [float(d) for d in values]

    # open output files
    mode = 'a' if args.wrote_header else 'w'
    with open(args.outfile, mode) as outfile:
        # output files (close)
        if not args.wrote_header:
            names = [det.name for det in detectors]
            outfile.write(build_header(args.mcstas.options, args.intervals.keys(), args.intervals, names))
            with open(args.simfile, mode) as simfile:
                simfile.write(build_mccodesim_header(args.mcstas.options, args.intervals, names,
                                                     version=args.mcstas.version))
            args.wrote_header = True

        outfile.write(f"{' '.join(map(str, x))} {' '.join(f'{d.intensity} {d.error}' for d in detectors)}\n")
        outfile.flush()

    if args.mcstas.options.optimize_minimize:
        criteria = sum(values)  # minimize
    else:
        criteria = -sum(values)  # maximize

    args.criteriaHistory.append(criteria)
    return criteria
