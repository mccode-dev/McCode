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

from mccode import McStas, Process
from optimisation import Scanner, LinearInterval, MultiInterval, Optimizer

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
        type=int, metavar='NP',
        help='Set number of scan points')

    add('-L', '--list',
        action='store_true',
        help='Use a fixed list of points for linear scanning')

    add('-M', '--multi',
        action='store_true',
        help='Run a multi-dimensional scan')

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
        help='Set number of %s to simulate' % (mccode_config.configuration["PARTICLE"]))

    add('-t', '--trace',
        metavar='trace', type=int, default=0,
        help='Enable trace of %s through instrument' % (mccode_config.configuration["PARTICLE"]))

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
        action='callback', callback=check_file(exist=False),
        help='Put all data files in directory DIR')

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
    if options.dir is None:
        instr = options.instr
        instr = instr.endswith('.instr') and instr[:-6] or instr
        # use unique directory when unspecified
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

    for param in options.params:
        if '=' in param:
            key, value = param.split('=', 1)
            interval = value.split(',')
            # When just one point is present, fix as constant
            if len(interval) == 1:
                fixed_params[key] = value
            else:
                LOG.debug('interval[%s]: %s', key, interval)
                intervals[key] = interval
        else:
            LOG.warning('Ignoring invalid parameter: "%s"', param)
    return (fixed_params, intervals)


def find_instr_file(instr):
    # Remove [-mpi].out to avoid parsing a binary file
    instr = clean_quotes(instr)
    if instr.endswith("-mpi." + mccode_config.platform['EXESUFFIX']):
        instr = instr[:-(5 + len(mccode_config.platform['EXESUFFIX']))]
    if instr.endswith("." + mccode_config.platform['EXESUFFIX']):
        instr = instr[:-(1 + len(mccode_config.platform['EXESUFFIX']))]

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
             'params={val|min,max|min,guess,max}...')
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

    (fixed_params, intervals) = get_parameters(options)

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

    if options.list:
        if len(intervals) == 0:
            raise OptionValueError(
                '--list was chosen but no lists was presented.')
        pointlist = list(intervals.values())
        points = len(pointlist[0])
        if not (all(map(lambda i: len(i) == points, intervals.values()))):
            raise OptionValueError(
                'All variables much have an equal amount of points.')
        interval_points = LinearInterval.from_list(
            points, intervals)

    scan = options.multi or options.numpoints
    if (options.numpoints is not None and options.numpoints < 2) or (scan and options.numpoints is None):
        raise OptionValueError((f'Cannot scan variable(s) {", ".join(intervals)} using only one data point. '
                                'Please use -N to specify the number of points.'))
    ## ## This *was* unreachable due to its indentation. Should it be removed entirely?
    # # Check that input is valid decimals
    # if not all(map(lambda i: len(i) == 2 and all(map(is_decimal, i)), intervals.values())):
    #     raise OptionValueError(f'Could not parse intervals -- result: {intervals}')

    if options.multi is not None:
        interval_points = MultiInterval.from_range(options.numpoints, intervals)
    elif options.numpoints is not None:
        interval_points = LinearInterval.from_range(options.numpoints, intervals)

    # Parameters for linear scanning present
    if interval_points:
        scanner = Scanner(mcstas, intervals)
        scanner.set_points(interval_points)
        if (not options.dir == ''):
            mkdir(options.dir)
        scanner.run()  # in optimisation.py
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
