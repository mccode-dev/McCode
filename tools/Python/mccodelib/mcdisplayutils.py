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
    def __init__(self, instr=None, inspect=None, default=False, n=300, dir=None, debug=False, options=None, trace=1, no_mpi=True, **kwds):
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
