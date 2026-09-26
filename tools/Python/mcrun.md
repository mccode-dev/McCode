[&larr; back to overview](README.md)

# mcrun / mxrun

Options are identical between McStas and McXtrace (only the particle name
in help text, and the McStas-only `-g`/`--gravitation` flag, differ).

## mcrun / mxrun — general & compile options

| Option | Description |
|---|---|
| `-c`, `--force-compile` | force rebuilding of instrument |
| `--cogen cogen` | Choice of code-generator (implies -c) |
| `-C`, `--c-lint` | Use c-linter (e.g. cppcheck) to lint the generated code. Configure linter via mccode_config.json. Implies -c and -v, but also NO simulation will be run. |
| `-I I` | Append to McCode search path (implies -c) |
| `--D1 D1` | Set extra -D args (implies -c) |
| `--D2 D2` | Set extra -D args (implies -c) |
| `--D3 D3` | Set extra -D args (implies -c) |
| `--no-cflags` | Disable optimising compiler flags for faster compilation |
| `--no-main` | Do not generate a main(), e.g. for use with mcstas2vitess.pl. Implies -c |
| `--verbose` | Enable verbose output during code-generation and simulation |
| `--write-user-config` | Generate a user config file |
| `--edit-user-config` | Generate and edit user config file in EDITOR |
| `--override-config PATH` | Load config file from specific dir |
| `--showcfg ITEM` | `'Print selected cfg item and exit (paths are resolved and absolute). Allowed values are %s.' % cfg_items_prettyprint` *(dynamic expression - verify by hand)* |
| `--embed` | Store copy of instrument file in output directory |

## mcrun / mxrun — parameters, scanning & optimisation

| Option | Description |
|---|---|
| `param=val`, `param=min,max`, `param=min,delta,max`, `param=a,b,c,d` | fixed parameter, scan interval (comma-separated), list-oriented inputs |
| `-p FILE`, `--param FILE` | Forward parameters from file FILE to Instrument |
| `-N NP`, `--numpoints NP` | Set number of scan points. Two input modes available:  1) A single integer applies the same point count to every    scanned parameter (default, and only valid form without -M) 2) Together with -M/--multi, a comma-separated list    (e.g. -N=5,10,20) gives each scanned parameter its    own point count, in the order in which parameters are listed    on the command line. If a parameter is given as par="min:delta:max"   the point count is instead computed from the requested bin width. |
| `-L`, `--list` | Use list-mode scanning. Multiple input modes available:  1) If multiple lists (of identical length) are given (and -M is     not requested) the lists are scanned together in lockstep. 2) Combined with -M/--multi, the cartesian product of each    parameter's own list is used to set up a multidimensional    'grid' scan (lists may have different lengths) 3) Any parameter given as "min:delta:max" is expanded into its    own explicit list of equidistant points and may be freely mixed    with other, explicitly-listed parameters    (e.g. a list of filenames) under -L. |
| `-M`, `--multi` | Run a multi-dimensional scan (cartesian product of every scanned parameter's points, rather than a co-linear scan). Combine with -L/--list or give -N as a comma-separated list (see -N/--numpoints). |
| `--scan_split scan_split` | Scan by parallelising steps as individual cpu threads. Initialise by number of wanted threads (e.g. your number of cores). |
| `--seeds SEEDS` | Set range of seeds to scan (each must be: SEED != 0) |
| `--optimize` | Optimize instrument variable parameters to maximize monitors |
| `--optimize-maxiter optimize_maxiter` | Maximum number of optimization iterations to perform. Default=1000 |
| `--optimize-tol optimize_tol` | Tolerance for optimization termination. When optimize-tol is specified, the selected optimization algorithm sets some relevant solver-specific tolerance(s) equal to optimize-tol |
| `--optimize-method optimize_method` | Optimization solver in ['powell', 'nelder-mead', 'cg', 'bfgs', 'newton-cg', 'l-bfgs-b', 'tnc', 'cobyla', 'slsqp', 'trust-constr', 'dogleg', 'trust-ncg', 'trust-exact', 'trust-krylov'] (default: powell) You can use your custom method method(fun, x0, args, **kwargs, **options). Please refer to scipy documentation for proper use of it: https://docs.scipy.org/doc/scipy/reference/generated/scipy.optimize.minimize.html?highlight=minimize |
| `--optimize-eval optimize_eval` | Optimization expression to evaluate for each detector "d" structure. You may combine: "d.intensity" The detector intensity; "d.error"     The detector intensity uncertainty; "d.values"    An array with [intensity, error, counts]; "d.X0 d.Y0"   Center of signal (1st moment); "d.dX d.dY"   Width  of signal (2nd moment). Default is "d.intensity". Examples are:  "d.intensity/d.dX" and "d.intensity/d.dX/d.dY" |
| `--optimize-minimize` | Choose to minimize the monitors instead of maximize |
| `--optimize-monitor optimize_monitor` | Name of a single monitor to optimize (default is to use all) |
| `--optimise-file FILE` | Store scan results in FILE (defaults to: "mccode.dat") |

## mcrun / mxrun — simulation & instrument options

| Option | Description |
|---|---|
| `--autoplot` | Open plotter on generated dataset |
| `--invcanvas` | Forward request for inverted canvas to plotter |
| `--autoplotter AUTOPLOTTER` | Specify the plotter used with --autoplot |
| `--mpi NB_CPU` | Spread simulation over NB_CPU machines using MPI |
| `--machines machines` | Defines path of MPI machinefile to use in parallel mode |
| `--openacc` | parallelize using openacc |
| `--funnel` | funneling simulation flow, e.g. for mixed CPU/GPU |
| `--vecsize VECSIZE` | vector length in OpenACC parallel scenarios |
| `--numgangs NUMGANGS` | number of 'gangs' in OpenACC parallel scenarios |
| `--gpu_innerloop INNERLOOP` | Maximum particles in an OpenACC kernel run. (If INNERLOOP is smaller than ncount we repeat) |
| `-s SEED`, `--seed SEED` | Set random seed (must be: SEED != 0) |
| `-n COUNT`, `--ncount COUNT` | Set number of neutrons to simulate |
| `-t trace`, `--trace trace` | Enable trace of neutrons through instrument |
| `--no-trace` | Disable trace of neutrons in instrument (combine with -c) |
| `-y`, `--yes` | Assume any default parameter value in instrument |
| `-g`, `--gravitation`, `--gravity` | *(McStas only)* Enable gravitation for all trajectories |
| `-i`, `--info` | Detailed instrument information |
| `--list-parameters` | Print the instrument parameters to standard out |
| `--meta-list` | Print all metadata defining component names |
| `--meta-defined META_DEFINED` | Print metadata names for component, or indicate if component:name exists |
| `--meta-type META_TYPE` | Print metadata type for component:name |
| `--meta-data META_DATA` | Print metadata for component:name |
| `-d DIR`, `--dir DIR` | Put all data files in directory DIR. If unspecified INSTRUMENT_TIMESTAMP is used |
| `--dirprefix dirprefix` | Put all data files in directory PREFIX_TIMESTAMP |
| `--dirsuffix dirsuffix` | Put all data files in directory INSTRUMENT_DIRSUFFIX |
| `-a`, `--append` | Append data files to those already in directory DIR |
| `--format FORMAT` | Output data files using format FORMAT, usually McCode or NeXus (format list obtained from <instr>.out -h) |
| `--IDF` | *(McStas only)* Flag to attempt inclusion of XML-based IDF when --format=NeXus (format list obtained from <instr>.out -h) |
| `--no-output-files` | Do not write any data files |
| `--bufsiz BUFSIZ` | Monitor_nD list/buffer-size (defaults to 1e7) |

---
[&larr; back to overview](README.md)
