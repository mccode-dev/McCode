'''
Intermediary layer for setting up a --trace pipe and interpreting the results.
'''
import os
import re
from pathlib import Path

from .pipetools import McrunPipeMan
from .instrparser import InstrTraceParser, InstrObjectConstructor
from .particleparser import ParticleBundleRayFactory, ParticleTraceParser
from .fcparticleparser import FlowChartParticleTraceParser
from . import mccode_config

if not os.name == 'nt':
    import shlex as lexer
else:
    import mslex as lexer

class McDisplayReader(object):
    ''' High-level trace manager '''
    def __init__(self, instr=None, inspect=None, default=False, n=300, dir=None, debug=False, options=None, trace=1, no_mpi=False, **kwds):
        ext = mccode_config.platform["EXESUFFIX"]

        if instr is None or ('.instr' not in instr and ext not in instr):
            print(f"A .instr or compiled instrument file ({ext}) is required")
            exit()
        
        # assemble command
        mcruncmd = str(Path(mccode_config.directories['bindir'],mccode_config.configuration['MCRUN']))

        if no_mpi:
            cmd = f"{mcruncmd} --no-mpi {instr} --no-output-files --trace={trace} --ncount={n}"
        else:
            cmd = f"{mcruncmd} {instr} --no-output-files --trace={trace} --ncount={n}"

        if dir:
            cmd = cmd + ' --dir=' + lexer.quote(dir)
        if options:
            cmd += ' ' + lexer.join(options)
        
        self.debug = debug
        self.count = n
        self.cmd = cmd
        self.pipeman = McrunPipeMan(cmd, inspect=inspect, send_enter=default)
    
    def read_instrument(self):
        ''' starts a pipe to mcrun given cmd, waits for instdef and reads, returning the parsed instrument '''
        self.pipeman.start_pipe()
        self.pipeman.join()
        
        instrdef = self.pipeman.read_instrdef()
        
        if self.debug:
            file_save(instrdef, 'instrdata')
        
        instrparser = InstrTraceParser(instrdef)
        instrbuilder = InstrObjectConstructor(instrparser.parsetree)
        instrument = instrbuilder.build_instr()
        
        instrument.set_cmd(self.cmd)
        
        return instrument
    
    def read_particles(self):
        ''' waits for pipeman object to finish, then read and parse neutron data '''
        rays = None
        if self.count > 0:
            print("reading particle data...")
            particles = self.pipeman.read_particles()
            print(self.pipeman.read_comments())
            if self.debug:
                file_save(particles, 'particledata')
            parser = FlowChartParticleTraceParser()
            rays = parser.execute(particles)
        return rays

def file_save(data, filename):
    ''' saves data for debug purposes '''
    f = open(filename, 'w')
    f.write(data)
    f.close()

def read_campath(filename):
    ''' camera path for the webgl tours (--campath): rows of x y z (m), optionally followed by the
    viewing direction dx dy dz, separated by spaces, tabs or commas; # starts a comment '''
    import sys, math
    def fail(msg):
        print(f"campath file {filename}: {msg}")
        sys.exit(-1)
    try:
        lines = Path(filename).read_text().splitlines()
    except OSError as e:
        fail(e.strerror)
    rows = []
    for i, line in enumerate(lines, 1):
        line = line.split('#')[0].strip()
        if not line:
            continue
        try:
            row = [float(v) for v in re.split(r'[\s,]+', line)]
        except ValueError:
            fail(f"line {i}: not a number")
        if len(row) not in (3, 6) or (rows and len(row) != len(rows[0])):
            fail(f"line {i}: {len(row)} columns, expected 3 (x y z) or 6 (x y z dx dy dz) on every line")
        if not all(math.isfinite(v) for v in row) or (len(row) == 6 and not any(row[3:])):
            fail(f"line {i}: not finite, or zero viewing direction")
        rows.append(row)
    if len(rows) < 2:
        fail("needs at least 2 points")
    return {'file': Path(filename).name, 'points': rows}

def read_simdir(dirname):
    ''' 1D and 2D monitor data of a simulation output directory (--simdir), by component name '''
    import sys
    from .mcplotloader import _get_filenames_from_mccodesim, _load_data_from_mcfiles, Data1D, Data2D
    sim = Path(dirname, 'mccode.sim')
    if not sim.is_file():
        print(f"simulation directory {dirname}: no mccode.sim")
        sys.exit(-1)
    monitors = {}
    for handle in _load_data_from_mcfiles(_get_filenames_from_mccodesim(str(sim))):
        d = handle.getdata()
        if isinstance(d, Data2D):
            monitors[d.component] = {'z': d.zvals, 'limits': d.xlimits, 'title': d.title, 'xlabel': d.xlabel, 'ylabel': d.ylabel}
        elif isinstance(d, Data1D):
            monitors[d.component] = {'y': d.yvals, 'limits': d.xlimits, 'title': d.title, 'xlabel': d.xlabel}
    return monitors

def make_common_parser(filename, documentation):
    """ Create an argparse.ArgumentParser with arguments (instr, --default, options)"""
    from pathlib import Path
    scriptname = Path(filename).stem
    #possibly replace the mc prefix in the description string
    from argparse import ArgumentParser

    ext = mccode_config.platform["EXESUFFIX"]
    prefix=scriptname[:2]

    parser = ArgumentParser(description=documentation.replace('mcdisplay',scriptname))
    parser.add_argument('instr', help=f'display this instrument file (.instr or .{ext})')
    parser.add_argument('--default', action='store_true', help='automatically use instrument defaults for simulation run')
    parser.add_argument('--no-mpi', action='store_true', help='Forward --no-mpi to run-script')
    parser.add_argument('options', nargs='*', help='simulation options and instrument params')

    return parser, prefix
