#!/usr/bin/env python3

#Suppress 'loading xxx configuration' print statement, since it might interfere
#with printouts of e.g. --version, --showcfg, ...:
import os
os.environ['MCCODE_SUPPRESS_LOAD_CONFIG_PRINT_STATEMENT']='1'

from os import mkdir
from os.path import isfile, isdir, abspath, dirname, basename, join
from shutil import copyfile
from optparse import OptionParser, OptionGroup, OptionValueError
from decimal import Decimal, InvalidOperation
from datetime import datetime
import multiprocessing
from mccode import McStas, Process
from optimisation import Scanner, Scanner_split, LinearInterval, MultiInterval, Optimizer

# import config
import sys

sys.path.insert(0,join(dirname(__file__), '..'))

from mccodelib import mccode_config

from log import getLogger, setupLogger, setLogLevel, McRunException
from log import DEBUG

LOG = getLogger('main')

# File path friendly date format (avoid ':' and white space)
DATE_FORMAT_PATH = "%Y%m%d_%H%M%S"

# list of scipy default optimizers
# see: https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.minimize.html
MINIMIZE_METHODS = ['powell', 'nelder-mead', 'cg', 'bfgs', 'newton-cg',
                    'l-bfgs-b', 'tnc', 'cobyla', 'slsqp', 'trust-constr',
                    'dogleg', 'trust-ncg', 'trust-exact', 'trust-krylov']


# Helper functions
def build_checker(accept, msg='Invalid value'):
    ''' Build checker from accept() function '''

    def checker(option, _opt_str, value, parser):
        ''' value must be acceptable '''
        if not accept(value):
            raise OptionValueError('option %s: %s (was: "%s")' % \
                                   (option, msg, value))
        # Update parser with accepted value
        setattr(parser.values, option.dest, value)

    return checker


def add_mcrun_options(parser):
    ''' Add option group for McRun options to parser '''

    # McRun options
    opt = OptionGroup(parser, '%s options' % (mccode_config.configuration["MCRUN"]))
    add = opt.add_option

    add('-c', '--force-compile',
        action='store_true',
        help='force rebuilding of instrument')

    add('--cogen',
        metavar='cogen',
        help='Choice of code-generator (implies -c)')

    add('-C','--c-lint',
        action='store_true',
        help='Use c-linter (e.g. cppcheck) to lint the generated code. Configure linter via mccode_config.json. Implies -c and -v, but also NO simulation will be run. ')

    add('-I',
        metavar='I',
        help='Append to McCode search path (implies -c)')

    add('--D1',
        metavar='D1',
        help='Set extra -D args (implies -c)')

    add('--D2',
        metavar='D2',
        help='Set extra -D args (implies -c)')

    add('--D3',
        metavar='D3',
        help='Set extra -D args (implies -c)')

    add('-p', '--param',
        metavar='FILE',
        help='Read parameters from file FILE')

    add('-N', '--numpoints',
        metavar='NP',
        help='Set number of scan points. A single integer applies the same '
             'point count to every scanned parameter (the default, and the '
             'only valid form without -M). With -M/--multi, a comma-separated '
             'list (e.g. -N=5,10,20) instead gives each scanned parameter its '
             'own point count, in the same order the parameters are listed '
             'on the command line. Not needed at all for a parameter given as '
             '"min:delta:max" (see the usage line above) - the point count is '
             'computed from the requested bin width instead.')

    add('--seeds',
        metavar='SEEDS',
        help='Set range of seeds to scan (each must be: SEED != 0)')

    add('-L', '--list',
        action='store_true',
        help='Use a fixed list of points for scanning, walking every scanned '
             'parameter\'s list together in lockstep (all lists must then be '
             'the same length). Combine with -M/--multi instead to take the '
             'cartesian product of each parameter\'s own list (lists may then '
             'have different lengths). A parameter given as "min:delta:max" '
             '(see the usage line above) is expanded into its own explicit '
             'list of equidistant points and can be freely mixed with other, '
             'explicitly-listed parameters (e.g. a list of filenames) under -L.')

    add('-M', '--multi',
        action='store_true',
        help='Run a multi-dimensional scan (the cartesian product of every '
             'scanned parameter\'s own points, rather than walking them all '
             'in lockstep). Combine with -L/--list (each parameter\'s '
             'explicit list can then have a different length) or give -N '
             'a comma-separated list (see -N/--numpoints) for per-parameter '
             'point counts.')

    add("--scan_split",
        type=int,
        metavar="scan_split",
        help='Scan by parallelising steps as individual cpu threads. Initialise by number of wanted threads (e.g. your number of cores).')

    add('--autoplot',
        action='store_true',
        help='Open plotter on generated dataset')

    add('--invcanvas',
        action='store_true',
        help='Forward request for inverted canvas to plotter')

    add('--autoplotter',
        action='store',
        type=str,
        help='Specify the plotter used with --autoplot')

    add('--embed',
        action='store_true', default=True,
        help='Store copy of instrument file in output directory')

    # Multiprocessing
    add('--mpi',
        metavar='NB_CPU',
        help='Spread simulation over NB_CPU machines using MPI')

    # Accellerator-support
    add('--openacc',
        action='store_true', default=False,
        help='parallelize using openacc')

    add('--funnel',
        action='store_true', default=False,
        help='funneling simulation flow, e.g. for mixed CPU/GPU')

    add('--machines',
        metavar='machines',
        help='Defines path of MPI machinefile to use in parallel mode')

    # Optimisation
    add('--optimise-file',
        metavar='FILE',
        help='Store scan results in FILE '
             '(defaults to: "mccode.dat")')

    add('--no-cflags',
        action='store_true', default=False,
        help='Disable optimising compiler flags for faster compilation')

    add('--no-main',
        action='store_true', default=False,
        help='Do not generate a main(), e.g. for use with mcstas2vitess.pl. Implies -c')

    add('--verbose',
        action='store_true', default=False,
        help='Enable verbose output')

    add('--write-user-config',
        action='store_true', default=False,
        help='Generate a user config file')

    add('--edit-user-config',
        action='store_true', default=False,
        help='Generate and edit user config file in EDITOR')

    
    add('--override-config',
        metavar='PATH', default=False,
        help='Load config file from specific dir')

    add('--optimize',
        action='store_true', default=False,
        help='Optimize instrument variable parameters to maximize monitors')

    add(
        "--optimize-maxiter",
        metavar="optimize_maxiter",
        type=int,
        help="Maximum number of optimization iterations to perform. Default=1000",
        nargs=1,
        default=1000,
    )
    add(
        "--optimize-tol",
        metavar="optimize_tol",
        type=float,
        help="Tolerance for optimization termination. When optimize-tol is specified, the selected optimization algorithm sets some relevant solver-specific tolerance(s) equal to optimize-tol",
        nargs=1,
    )
    add(
        "--optimize-method",
        metavar='optimize_method',
        type=str,
        help='Optimization solver in ' + str(MINIMIZE_METHODS) + '\n' +
             '(default: ' + MINIMIZE_METHODS[0] + ')' + '\n' +
             'You can use your custom method method(fun, x0, args, **kwargs, **options). Please refer to scipy documentation for proper use of it:' + '\n' +
             'https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.minimize.html?highlight=minimize',
        nargs=1,
        default=MINIMIZE_METHODS[0],
    )
    add(
        "--optimize-eval",
        metavar='optimize_eval',
        type=str,
        help='Optimization expression to evaluate for each detector "d" structure. You may combine:\n' +
             '"d.intensity" The detector intensity;\n' +
             '"d.error"     The detector intensity uncertainty;\n' +
             '"d.values"    An array with [intensity, error, counts];\n' +
             '"d.X0 d.Y0"   Center of signal (1st moment);\n' +
             '"d.dX d.dY"   Width  of signal (2nd moment).\n' +
             'Default is "d.intensity". Examples are: \n' +
             '"d.intensity/d.dX" and "d.intensity/d.dX/d.dY"',
        nargs=1,
        default=None,
    )
    add(
        "--optimize-minimize",
        action='store_true',
        help='Choose to minimize the monitors instead of maximize',
    )
    add(
        "--optimize-monitor",
        metavar="optimize_monitor",
        type=str,
        help="Name of a single monitor to optimize (default is to use all)",
        nargs=1,
        default="",
    )

    #    --optimize-maxiter maxiter  max iter of optimization
    #    --tol tol          tolerance criteria to end the optimization
    #    --method method    Method to maximize the intensity in ['nelder-mead', 'powell', 'cg', 'bfgs', 'newton-cg', 'l-bfgs-b', 'tnc', 'cobyla', 'slsqp', 'trust-constr', 'dogleg', 'trust-ncg', 'trust-exact', 'trust-krylov']
    #                       (default: nelder-mead)
    #                       You can use your own method by entering something else, it will add it as a librairy. Please refer to scipy documentation for proper use of it:
    #                       https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.minimize.html?highlight=minimize
    #    --minimize         choose to minimize the function if needed
    #    --monitor monitor  monitor name

    cfg_items = ['bindir','libdir','resourcedir','tooldir']
    cfg_items_prettyprint =   '"%s", and "%s"'%('", "'.join(cfg_items[:-1]),cfg_items[-1])
    add(
        "--showcfg", choices=cfg_items, metavar="ITEM",
        help="Print selected cfg item and exit (paths are resolved and absolute). Allowed values are %s."%cfg_items_prettyprint
    )

    parser.add_option_group(opt)


def add_mcstas_options(parser):
    ''' Add option group for McStas options to parser '''

    opt = OptionGroup(parser, 'Instrument options')
    add = opt.add_option

    # Misc options
    check_seed = build_checker(lambda seed: seed != 0,
                               'SEED cannot be 0')

    add('-s', '--seed',
        metavar='SEED', type=int, action='callback', callback=check_seed,
        help='Set random seed (must be: SEED != 0)')

    add('-n', '--ncount',
        metavar='COUNT', type=float, default=1000000,
        help='Set number of %ss to simulate' % (mccode_config.configuration["PARTICLE"]))

    add('-t', '--trace',
        metavar='trace', type=int, default=0,
        help='Enable trace of %ss through instrument' % (mccode_config.configuration["PARTICLE"]))
  
    add('--no-trace',
        action='store_true', metavar='notrace', default=None,
        help='Disable trace of %ss in instrument (combine with -c)' % (mccode_config.configuration["PARTICLE"]))
  
    add('-y', '--yes',
        action='store_true', default=False,
        help='Assume any default parameter value in instrument')

    if (mccode_config.configuration["MCCODE"] == 'mcstas'):
        add('-g', '--gravitation', '--gravity',
            action='store_true', default=False,
            help='Enable gravitation for all trajectories')

    # Data options
    dir_exists = lambda path: isdir(abspath(path))

    def check_file(exist=True):
        ''' Validate the path to a file '''
        if exist:
            is_valid = isfile
        else:
            def is_valid(path):
                ''' Ensure path to file exists and filename is provided '''
                if path == "." or path == "./" or path == ".\\":
                    return True
                if not dir_exists(dirname(path)):
                    return False
                return not isdir(abspath(path))
        return build_checker(is_valid, 'invalid path')

    add('-d', '--dir',
        metavar='DIR', type=str,
        help='Put all data files in directory DIR. If unspecified INSTRUMENT_TIMESTAMP is used')

    add('--dirprefix',
        metavar='dirprefix', type=str,
        help='Put all data files in directory PREFIX_TIMESTAMP')

    add('--dirsuffix',
        metavar='dirsuffix', type=str,
        help='Put all data files in directory INSTRUMENT_DIRSUFFIX')

    add('-a', '--append',
        action='store_true', default=False,
        help='Append data files to those already in directory DIR')

    add('--format',
        metavar='FORMAT', default='McCode',
        help='Output data files using format FORMAT, usually McCode or NeXus '
             '(format list obtained from <instr>.%s -h)' % mccode_config.platform["EXESUFFIX"])

    # --IDF-option only makes sense in McStas case
    if (mccode_config.configuration["MCCODE"] == 'mcstas'):
        add('--IDF',
            action='store_true', default=False,
            help='Flag to attempt inclusion of XML-based IDF when --format=NeXus '
                 '(format list obtained from <instr>.%s -h)' % mccode_config.platform["EXESUFFIX"])

    add('--bufsiz',
        metavar='BUFSIZ', default=mccode_config.configuration["NDBUFFERSIZE"],
        help='Monitor_nD list/buffer-size (defaults to '+mccode_config.configuration["NDBUFFERSIZE"]+')')

    add('--vecsize',
        metavar='VECSIZE', default='',
        help='vector length in OpenACC parallel scenarios')

    add('--numgangs',
        metavar='NUMGANGS', default='',
        help='number of \'gangs\' in OpenACC parallel scenarios')

    add('--gpu_innerloop',
        metavar='INNERLOOP', default='',
        help='Maximum particles in an OpenACC kernel run. (If INNERLOOP is smaller than ncount we repeat)')

    add('--no-output-files',
        action='store_true', default=False,
        help='Do not write any data files')

    # Information
    add('-i', '--info',
        action='store_true', default=False,
        help='Detailed instrument information')

    add('--list-parameters', action='store_true', default=False,
        help='Print the instrument parameters to standard out')

    add('--meta-list', action='store_true', default=False, help='Print all metadata defining component names')
    add('--meta-defined', default=None, help="Print metadata names for component, or indicate if component:name exists")
    add('--meta-type', default=None, help="Print metadata type for component:name")
    add('--meta-data', default=None, help="Print metadata for component:name")

    parser.add_option_group(opt)


def expand_options(options):
    ''' Add extra options based on previous choices '''
    # McCode version and library
    if options.cogen is None:
        options.mccode_bin = mccode_config.configuration['MCCOGEN']
    else:
        options.mccode_bin = options.cogen
    options.mccode_lib = mccode_config.configuration['MCCODE_LIB_DIR']

    # MPI
    if options.mpi is not None:
        options.use_mpi = True
        if options.openacc is True:
            options.cc = mccode_config.compilation['OACC']
        else:
            options.cc = mccode_config.compilation['MPICC']
        options.mpirun = mccode_config.compilation['MPIRUN']
    elif options.openacc is True:
        options.use_openacc = True
        options.cc = mccode_config.compilation['OACC']
        options.use_mpi = False
    else:
        options.use_mpi = False
        options.cc = mccode_config.compilation['CC']

    # Check if options.cc is a bareword 'command' or a full path
    if not dirname(options.cc) == '':
        if not os.path.exists(options.cc):
            LOG.warning('Full-path compiler "%s" not found!!', options.cc)
            options.cc=basename(options.cc)
            LOG.warning('Attempting replacement by "%s"', options.cc)

    if options.funnel is not None:
        options.use_funnel = True

    # Output dir
    # Case 1, we got a directory. Just use that
    if options.dir is not None:
        pass;

    # Case 2, no directory but we got a prefix
    if options.dir is None and options.dirprefix is not None:
        options.dir = "%s_%s" % \
                      (options.dirprefix,
                       datetime.strftime(datetime.now(), DATE_FORMAT_PATH))

    # Case 3, no directory, generate one from instr name
    if options.dir is None:
        instr = options.instr
        instr = instr.endswith('.instr') and instr[:-6] or instr
        # Case 3.1, dirsuffix is given, use this instead of timestamp
        if options.dirsuffix is not None:
            options.dir = "%s_%s" % \
                      (basename(instr),options.dirsuffix)
        # Case 4.2 'fallback', generate from instr name and timestamp
        else:# use unique directory neither dirname or suffix is specified
            options.dir = "%s_%s" % \
                      (basename(instr),
                       datetime.strftime(datetime.now(), DATE_FORMAT_PATH))
        # alert user
        LOG.info('No output directory specified (--dir)')

    # Output file
    if options.optimise_file is None:
        # use mccode.dat when unspecified
        options.optimise_file = '%s/mccode.dat' % options.dir
    if options.optimize:
        options.optimize_methods = MINIMIZE_METHODS


def is_decimal(string):
    ''' Check if string is parsable as decimal/float '''
    try:
        Decimal(string)
        return True
    except InvalidOperation:
        return False


def get_parameters(options):
    ''' Get fixed and scan/optimise parameters '''
    fixed_params = {}
    intervals = {}
    # Per-key point counts implied by the "a:delta:b" syntax below - kept
    # separate from intervals (which only ever holds the [a, b] endpoints,
    # matching every other scan mode's shape) so main() can tell which
    # parameters had an explicit point count baked into their own syntax,
    # as opposed to needing one supplied via -N.
    equidistant_numpoints = {}

    for param in options.params:
        if '=' in param:
            key, value = param.split('=', 1)

            # "par=a:delta:b" - an equidistant scan specified by its bin
            # width (delta) rather than an explicit point count: mcrun
            # computes how many points are needed to cover [a, b] in steps
            # of (approximately - see rounding below) delta, rather than
            # the user needing to work out -N by hand. Checked before the
            # comma-based interval parsing below, since a colon can never
            # appear in a numeric value/list, so a colon anywhere in the
            # value unambiguously means this syntax was intended.
            if ':' in value:
                parts = value.split(':')
                if len(parts) != 3:
                    raise OptionValueError(
                        'Parameter "%s" uses "a:delta:b" syntax but has %d colon-separated part(s) '
                        '(expected exactly 3: start:delta:stop): "%s"' % (key, len(parts), value))
                try:
                    a, delta, b = (float(p) for p in parts)
                except ValueError:
                    raise OptionValueError(
                        'Parameter "%s" uses "a:delta:b" syntax but not all three parts are numbers: "%s"'
                        % (key, value))
                if delta == 0:
                    raise OptionValueError(
                        'Parameter "%s" uses "a:delta:b" syntax with delta=0, which would need '
                        'infinitely many points: "%s"' % (key, value))
                if a == b:
                    raise OptionValueError(
                        'Parameter "%s" uses "a:delta:b" syntax with a == b (%s), so there is nothing '
                        'to scan - use a fixed value "%s=%s" instead.' % (key, a, key, a))
                # Rounds to the nearest point count that covers [a, b] as
                # closely as possible to the requested delta - the actual
                # step size will usually differ very slightly from delta
                # itself, since [a, b] isn't guaranteed to be an exact
                # multiple of delta and both endpoints are always included.
                n_points = max(2, round(abs(b - a) / abs(delta)) + 1)
                step = (b - a) / (n_points - 1)
                if options.list:
                    # -L is active: a:delta:b conceptually already IS a
                    # list of equidistant points, so expand it into the
                    # full explicit list here and let it flow through the
                    # exact same -L/-M machinery as any other explicit
                    # list (e.g. an accompanying filename list) - no
                    # special-casing needed anywhere else for this case.
                    intervals[key] = [str(a + i * step) for i in range(n_points)]
                    LOG.debug('interval[%s]: %s (a:delta:b syntax, expanded to %d explicit points for -L)',
                              key, intervals[key], n_points)
                else:
                    # -L not given: keep the [a, b] endpoint pair, with the
                    # point count tracked separately - main()'s normal
                    # -N/-M machinery (LinearInterval/MultiInterval
                    # .from_range()) already knows how to turn an
                    # endpoint pair plus a point count into the same
                    # equidistant points, without needing them written out
                    # explicitly here.
                    intervals[key] = [str(a), str(b)]
                    equidistant_numpoints[key] = n_points
                    LOG.debug('interval[%s]: %s (a:delta:b syntax, delta=%s -> %d points)',
                              key, intervals[key], delta, n_points)
                continue

            interval = value.split(',')
            # When just one point is present, fix as constant
            if len(interval) == 1:
                fixed_params[key] = value
            else:
                LOG.debug('interval[%s]: %s', key, interval)
                intervals[key] = interval
        else:
            LOG.warning('Ignoring invalid parameter: "%s"', param)
    return (fixed_params, intervals, equidistant_numpoints)


def find_instr_file(instr):
    instr = clean_quotes(instr)

    # Append ".instr" if needed
    if not isfile(instr) and isfile(instr + ".instr"):
        instr += ".instr"

    return instr


def clean_quotes(string):
    ''' Remove all leading and ending quotes (" and \') '''
    return string.strip('"' + "'")


def main():
    ''' Main routine '''
    setupLogger()

    # Add options
    usage = ('usage: %prog [-cpnN] Instr [-sndftgahi] '
             'params={val|min,max|min:delta:max|min,guess,max}...')
    parser = OptionParser(usage, version=mccode_config.configuration['MCCODE_VERSION'])

    add_mcrun_options(parser)
    add_mcstas_options(parser)

    # Parse options
    (options, args) = parser.parse_args()

    if options.showcfg:
        #For now, all options are actually directly available as keys in the
        #mccode_config.directories dictionary:
        assert options.showcfg in mccode_config.directories.keys()
        print(mccode_config.directories[options.showcfg])
        raise SystemExit

    # Write user config file and exit
    if options.write_user_config:
        mccode_config.save_user_config()
        raise SystemExit

    # Write user config file, edit and exit
    if options.edit_user_config:
        mccode_config.save_user_config(Edit=True)
        raise SystemExit
    
    # Override system and user level config files if prompted
    if options.override_config:
        mccode_config.load_config(options.override_config)
        mccode_config.check_env_vars()

    # Extract instrument and parameters
    if len(args) == 0:
        print(parser.get_usage())
        parser.exit()

    # Set path of instrument-file after locating it
    options.instr = find_instr_file(args[0])

    if options.param:
        # load params from file
        text = open(options.param).read()
        import re
        params = re.findall(r'[\w0-9]+=[^=\s]+', text)
        options.params = map(clean_quotes, params)
    else:
        # Clean out quotes (perl mcgui requires this step)
        options.params = map(clean_quotes, args[1:])

    # On windows, ensure that backslashes in the filename are escaped
    if sys.platform == "win32":
        options.instr = options.instr.replace("\\", "\\\\")

    # Fill out extra information
    expand_options(options)

    if options.c_lint is not None:
        options.force_compile = True
        options.verbose = True

    if options.verbose:
        setLogLevel(DEBUG)

    # Inform user of what is happening
    # TODO: More info?
    LOG.info('Using directory: "%s"' % options.dir)
    if options.dir == "." or options.dir == "./" or options == ".\\":
        LOG.warning('Existing files in "%s" will be overwritten!' % options.dir)
        LOG.warning(' - and datafiles catenated...')
        options.dir = '';

    # Run McStas
    mcstas = McStas(options.instr)
    mcstas.prepare(options)

    (fixed_params, intervals, equidistant_numpoints) = get_parameters(options)
    # Add --seeds as an 'interval', to allow scanning simulation seed
    if options.seeds:
        intervals['--seed']=options.seeds.split(',')

    # Indicate end of setup / start of computations
    LOG.info('===')

    if options.info or options.list_parameters or \
            options.meta_list or options.meta_defined or options.meta_type or options.meta_data:
        mcstas.run(override_mpi=False)
        exit()

    # Set fixed parameters
    for key, value in fixed_params.items():
        mcstas.set_parameter(key, value)

    # Check for linear scanning
    interval_points = None

    # Can't both do list and interval scanning
    if options.list and options.numpoints:
        raise OptionValueError('--numpoints cannot be used with --list')

    # Can't both do list and --seeds scanning
    if options.list and options.seeds:
        raise OptionValueError('--seeds cannot be used with --list')

    # An explicit -N is only actually redundant/conflicting when EVERY
    # scanned parameter already gets its point count from "a:delta:b"
    # syntax (see get_parameters()) without -L active (with -L, a:delta:b
    # expands directly into an explicit list in intervals[key], so
    # equidistant_numpoints stays empty and this check can't fire at all).
    # A scan mixing a:delta:b with a plain "min,max" parameter still
    # legitimately needs -N to say how many points that one should have
    # (see the "mixed" branch below).
    if equidistant_numpoints and options.numpoints and len(equidistant_numpoints) == len(intervals):
        raise OptionValueError(
            'The "a:delta:b" syntax (used for %s) already determines its own point count for every '
            'scanned parameter, so an explicit -N/--numpoints is redundant here.' % ', '.join(equidistant_numpoints))

    # Parse -N/--numpoints (a plain string now, not auto-int'd by optparse -
    # see add_mcrun_options()): with -M/--multi it may be a comma-separated
    # list of integers, one per scanned parameter in the same order the
    # parameters were given on the command line, rather than a single
    # integer applied uniformly to every dimension. A list form without -M
    # is rejected outright: a plain (co-linear) scan walks every parameter
    # in lockstep over the same number of steps, so per-dimension point
    # counts don't apply there. Unreachable when --list was also given,
    # thanks to the check just above.
    numpoints_list = None
    if options.numpoints is not None:
        numpoints_parts = str(options.numpoints).split(',')
        if len(numpoints_parts) > 1:
            if not options.multi:
                raise OptionValueError(
                    'A comma-separated list for -N/--numpoints (e.g. -N=5,10,20) is only valid '
                    'together with -M/--multi.')
            try:
                numpoints_list = [int(p) for p in numpoints_parts]
            except ValueError:
                raise OptionValueError('-N/--numpoints list must contain only integers: "%s"' % options.numpoints)
            if any(n < 2 for n in numpoints_list):
                raise OptionValueError(
                    'Cannot scan using only one data point - every entry in -N/--numpoints must be at least 2.')
            options.numpoints = None  # resolved into numpoints_list/numpoints_dict instead, below
        else:
            try:
                options.numpoints = int(numpoints_parts[0])
            except ValueError:
                raise OptionValueError(
                    '-N/--numpoints must be an integer (or, with -M, a comma-separated list of integers): "%s"'
                    % options.numpoints)

    if options.list:
        if len(intervals) == 0:
            raise OptionValueError(
                '--list was chosen but no lists was presented.')
        if options.multi:
            # -L + -M: cartesian product across each parameter's own
            # explicit list of points - unlike plain -L (which walks every
            # list together in lockstep, requiring them all to be the same
            # length), each dimension is independent here, so the lists
            # may have different lengths.
            interval_points = MultiInterval.from_list(intervals)
            options.numpoints = 1
            for values in intervals.values():
                options.numpoints *= len(values)
        else:
            pointlist = list(intervals.values())
            points = len(pointlist[0])
            if not (all(map(lambda i: len(i) == points, intervals.values()))):
                raise OptionValueError(
                    'All variables must have an equal amount of points.')
            interval_points = LinearInterval.from_list(
                points, intervals)
            options.numpoints = points

    elif numpoints_list is not None:
        # -M + -N=a,b,c,...: per-dimension point counts, no explicit lists
        if len(numpoints_list) != len(intervals):
            raise OptionValueError(
                '-N/--numpoints list has %d entr%s but %d parameter%s being scanned (%s); '
                'provide exactly one point-count per scanned parameter, in the same order.' % (
                    len(numpoints_list), 'y' if len(numpoints_list) == 1 else 'ies',
                    len(intervals), '' if len(intervals) == 1 else 's are',
                    ', '.join(intervals)))
        numpoints_dict = dict(zip(intervals.keys(), numpoints_list))
        interval_points = MultiInterval.from_range(numpoints_dict, intervals)
        total = 1
        for n in numpoints_list:
            total *= n
        options.numpoints = total

    elif equidistant_numpoints:
        # "a:delta:b" syntax: each such parameter already has its own
        # point count computed in get_parameters(), independent of -N/-M.
        if len(equidistant_numpoints) == len(intervals):
            # Every scanned parameter uses a:delta:b.
            distinct_n = set(equidistant_numpoints.values())
            if options.multi:
                # -M: cartesian product, each dimension keeping its own
                # delta-derived point count - identical in spirit to
                # -N=a,b,c,... + -M above, just sourced from delta instead.
                interval_points = MultiInterval.from_range(equidistant_numpoints, intervals)
                total = 1
                for n in equidistant_numpoints.values():
                    total *= n
                options.numpoints = total
            elif len(distinct_n) == 1:
                # No -M, but every parameter's delta happens to imply the
                # same point count anyway - a perfectly ordinary co-linear
                # scan, so there's no need to force the user to add -M.
                options.numpoints = distinct_n.pop()
                interval_points = LinearInterval.from_range(options.numpoints, intervals)
            else:
                raise OptionValueError(
                    'Parameter(s) %s use "a:delta:b" syntax with different resulting point counts (%s) - '
                    'add -M/--multi to scan them independently (a cartesian product), or use matching '
                    'delta values for a co-linear scan.' % (
                        ', '.join(equidistant_numpoints),
                        ', '.join('%s=%d' % (k, v) for k, v in equidistant_numpoints.items())))
        else:
            # Mixed: some parameters use a:delta:b, others a plain min,max
            # (no delta) that still needs a point count from somewhere.
            missing = [k for k in intervals if k not in equidistant_numpoints]
            if not options.multi:
                raise OptionValueError(
                    'Parameter(s) %s use "a:delta:b" syntax alongside plain interval(s) %s - add '
                    '-M/--multi to scan them independently, or use "a:delta:b" for every scanned '
                    'parameter.' % (', '.join(equidistant_numpoints), ', '.join(missing)))
            if options.numpoints is None:
                raise OptionValueError(
                    'Parameter(s) %s need a point count - use "a:delta:b" syntax for them too, or '
                    'supply a plain -N value.' % ', '.join(missing))
            full_numpoints_dict = dict(equidistant_numpoints)
            for k in missing:
                full_numpoints_dict[k] = options.numpoints
            interval_points = MultiInterval.from_range(full_numpoints_dict, intervals)
            total = 1
            for n in full_numpoints_dict.values():
                total *= n
            options.numpoints = total

    else:
        scan = options.multi or options.numpoints
        if (options.numpoints is not None and options.numpoints < 2) or (scan and options.numpoints is None):
            raise OptionValueError((f'Cannot scan variable(s) {", ".join(intervals)} using only one data point. '
                                    'Please use -N to specify the number of points.'))

        if options.multi is not None:
            interval_points = MultiInterval.from_range(options.numpoints, intervals)
        elif options.numpoints is not None:
            interval_points = LinearInterval.from_range(options.numpoints, intervals)


    # Check that mpi and scan split are not both used. Default to mpi if they are
    if options.scan_split is not None and options.mpi is not None:
        options.scan_split = None
        
    # Parameters for linear scanning present
    if interval_points and (options.scan_split is None):
        scanner = Scanner(mcstas, intervals)
        scanner.set_points(interval_points)
        if (not options.dir == ''):
            mkdir(options.dir)
        scanner.run()  # in optimisation.py

    elif options.scan_split is not None:
        if options.scan_split == 0:
            options.scan_split = multiprocessing.cpu_count()-1
        split_scanner = Scanner_split(mcstas, intervals, options.scan_split)
        split_scanner.set_points(interval_points)
        if (not options.dir == ''):
            mkdir(options.dir)
        split_scanner.run()  # in optimisation.py

    elif options.optimize:
        optimizer = Optimizer(mcstas, intervals)
        if (not options.dir == ''):
            mkdir(options.dir)
        optimizer.run()  # in optimisation.py
    else:
        # Only run a simulation if we have a nonzero ncount
        if options.ncount != 0.0 or options.trace:
            mcstas.run()  # in mccode.py

    if isdir(options.dir):
        LOG.info('Placing instr file copy %s in dataset %s', options.instr, options.dir)
        copyfile(options.instr, join(options.dir, basename(options.instr)))
        cfile = os.path.splitext(options.instr)[0] + ".c"
        if os.path.exists(cfile):
            LOG.info('Placing generated c-code copy %s in dataset %s', cfile, options.dir)
            copyfile(cfile, join(options.dir, basename(cfile)))

    if options.autoplot is not None:
        autoplotter = mccode_config.configuration['MCPLOT']
        # apply selected autoplotter, if used
        if options.autoplotter is not None:
            autoplotter = options.autoplotter
        if isdir(options.dir):
            LOG.info('Running plotter %s on dataset %s', autoplotter, options.dir)
            if not options.invcanvas:
                Process(autoplotter).run([options.dir])
            else:
                Process(autoplotter).run([options.dir, '--invcanvas'])

if __name__ == '__main__':
    try:

        mccode_config.load_config("user")
        mccode_config.check_env_vars()

        main()
    except KeyboardInterrupt:
        LOG.fatal('User interrupt.')
    except OptionValueError as e:
        LOG.fatal(str(e))
    except McRunException as e:
        LOG.fatal(str(e))
