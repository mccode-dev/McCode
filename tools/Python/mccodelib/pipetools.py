'''
Tools for piping a process to the terminal for std I/O.
Buffering and low-level filtering. Thread management.
'''
import re
import subprocess
from subprocess import Popen, PIPE
from threading import Thread, Event
import os
import signal

class DataBox():
    ''' container for pre-processed trace data '''
    def __init__(self):
        self.comments = []
        self.instrdef = []
        self.particle_blocks = []
        
        # event objects
        self.instrdone = Event()
        self.instrdone.clear()
        self.particlesdone = Event()
        self.particlesdone.clear()
    
    def add_comment(self, line):
        self.comments.append(line)
    
    def add_instrdef(self, line):
        self.instrdef.append(line)
    
    def add_particleblock(self, block):
        self.particle_blocks.append(block)

    def set_instrdone(self):
        self.instrdone.set()
    
    def set_particlesdone(self):
        self.particlesdone.set()
    
    def get_instrdef(self):
        self.instrdone.wait()
        return ''.join(self.instrdef)
    
    def get_particles(self):
        self.particlesdone.wait()
        return ''.join(self.particle_blocks)

    def get_comments(self):
        self.instrdone.wait()
        self.particlesdone.wait()
        return ''.join(self.comments)
        
class LineHandlerState(object):
    def __init__(self, setcurrent, next, databox, args=None):
        '''
        Abstract state for all line handler states - instrdef, particles, etc.
            setcurrent: a fuction of state, line
            allstates: a dictionary of all states given typename
            databox: global data destination / buffer
        '''
        self.setcurrent = setcurrent
        self.next = next
        self.databox = databox
        self.args = args
        
    def add_line(self, line):
        ''' override to implement a state '''
        pass

class PromptState(LineHandlerState):
    def add_line(self, line):
        # switch case
        if re.match(r'INSTRUMENT:\n', line):
            self.setcurrent(self.next, line)
            return
        
        # prompt case
        print(line.rstrip('\n'))
        self.databox.add_comment(line)
        if re.search(r'\]:', line):
            if not self.args['use_defaultpars']:
                inpt = input()
                self.process.stdin.write(inpt + '\n')
                self.process.stdin.flush()
            else:
                self.process.stdin.write('\n')
                self.process.stdin.flush()
    
    def setprocess(self, process):
        self.process = process

class InstrState(LineHandlerState):
    def __init__(self, setcurrent, next, databox, args=None):
        self.idx = 0
        self.first = True
        self.second = False
        super(InstrState, self).__init__(setcurrent, next, databox, args)
        
    def add_line(self, line):
        if self.first and re.match('INSTRUMENT:', line):
            self.databox.add_instrdef(line)
            self.first, self.second = False, True
        elif self.second and re.match(r'Instrument \'[\w\-\.]+\' \([/\w\\\:\-\.]+(\.instr)?\)', line):
            self.databox.add_instrdef(line)
            self.second = False
        elif re.match('COMPONENT:', line):
            self.databox.add_instrdef(line)
        elif re.match('POS:', line):
            self.databox.add_instrdef(line)
        elif re.match('MCDISPLAY: start', line):
            self.databox.set_instrdone()
            self.setcurrent(self.next, line)
        else:
            self.databox.add_comment(line)

class McdisplayState(LineHandlerState):
    def add_line(self, line):
        if re.match('MCDISPLAY: ', line):
            self.databox.add_instrdef(line)
        elif re.match('MANTID_PIXEL: ', line):
            self.databox.add_instrdef(line)
        elif re.match('MANTID_RECTANGULAR_DET: ', line):
            self.databox.add_instrdef(line)
        elif re.match('MANTID_BANANA_DET: ', line):
            self.databox.add_instrdef(line)
        elif re.match('INSTRUMENT END:', line):
            self.databox.add_instrdef(line)
        elif re.match('ENTER:', line):
            self.databox.set_instrdone()
            self.setcurrent(self.next, line)
        else:
            self.databox.add_comment(line)

class ParticlesTraceState(LineHandlerState):
    def __init__(self, setcurrent, next, databox, args=None):
        self.process = None
        self.block = []
        self.active = False
        self.leaveflag = False
        
        self.inspect = None
        if args:
            self.inspect = args.get('inspect', None)
        super(ParticlesTraceState, self).__init__(setcurrent, next, databox, args)
        
        self.max_particles = 100000
        if args:
            self.max_particles = args.get('max_particles', 100000)
        
        self.block_count = 0
    
    def add_line(self, line):
        def check(line):
            if len(line) > 5:
                return line[:5] in ('SCATT', 'STATE', 'COMP:', 'ABSOR')
        
        if re.match('LEAVE:', line):
            self.block.append(line)
            self.leaveflag = True
            self.active = False
        elif self.active and check(line):
            self.block.append(line)
        elif re.match('ENTER:', line):
            self.block.append(line)
            self.active = True
        elif self.leaveflag and check(line):
            self.block.append(line)
            
            # inspect
            accept_block = True
            if self.inspect:
                accept_block = False
                for b in self.block:
                    if re.match('COMP: "%s"' % self.inspect, b):
                        accept_block = True
                        break
            
            if accept_block:
                self.databox.add_particleblock(''.join(self.block))
                if self.block_count > self.max_particles:
                    print('max particle count exceeded, blocking all further trace particle trace lines...')
                    self.setcurrent(self.next, None)

                self.block_count += 1
            
            self.block = []
            self.leaveflag = False
        else:
            self.databox.add_comment(line)
    
class PostParticletraceState(LineHandlerState):
    ''' this state does nothing, so nothing happens from now on '''
    waskilled = False
    def add_line(self, line):
        # Only kill if PID still can be killed, buffered stream data
        # means that we could enter here multiple times still.
        try:
            os.killpg(os.getpgid(self.process.pid), signal.SIGTERM)
        except:
            # The above os.killpg works fine on Unix, but on Win32 we can instead
            try:
                subprocess.call(['taskkill', '/F', '/T', '/PID',  str(self.process.pid)])
            except:
                pass
        self.waskilled = True

    def setprocess(self, process):
        self.process = process
        
class ThreadException(Exception):
    pass

class TraceReader(Thread):
    def _setcurrent(self, current, line):
        self.current = current
        current.add_line(line)
    
    def __init__(self, cmd, inspect=None, use_defaultpars=False, max_particles=100000):
        self.exc_obj = None
        
        # set up state machine
        allstates = {}
        databox = DataBox()
        
        allstates['post_particles'] = PostParticletraceState(self._setcurrent, None, databox=databox)
        allstates['particles'] = ParticlesTraceState(self._setcurrent, next=allstates['post_particles'], databox=databox, 
                                                     args={'inspect': inspect, 'max_particles': max_particles})
        allstates['mcdisplay'] = McdisplayState(self._setcurrent, next=allstates['particles'], databox=databox)
        allstates['instr'] = InstrState(self._setcurrent, next=allstates['mcdisplay'], databox=databox)
        allstates['prompt'] = PromptState(self._setcurrent, next=allstates['instr'], databox=databox, 
                                          args={'use_defaultpars': use_defaultpars})
        
        # remember
        self.current = allstates['prompt']
        self.allstates = allstates
        self.databox = databox
        self.cmd = cmd
        
        Thread.__init__(self)
        self.daemon = True
        
        self.debug = False
        
    def run(self):
        try:
            ''' create a process given command and read, print and write to it depending on state '''
            # os.setsid defineds a fresh process group so that the calling Python survives when the
            # simulation is potentially terminated.
            if not os.name == 'nt':
                process = Popen(self.cmd, shell=True, preexec_fn=os.setsid, universal_newlines=True,
                                stdout=PIPE,
                                stderr=PIPE,
                                stdin=PIPE
                                )
            else:
                process = Popen(self.cmd, shell=True, universal_newlines=True,
                                stdout=PIPE,
                                stderr=PIPE,
                                stdin=PIPE
                                )
            
            # special case: give the prompt state access to process to allow automation flag, 
            # and particles to end the process in max particle count is exceeded
            self.allstates['prompt'].setprocess(process)
            self.allstates['post_particles'].setprocess(process)

            poll = process.poll()
            while poll == None:
                for data in process.stdout:
                    self.current.add_line(data)
                    if self.debug:
                        print("debug - TraceReader read line: %s" % data.strip())

                for data in process.stderr:
                    print(data.strip())

                poll = process.poll()

            # fail state exit status from mcrun process
            if poll != 0:
                if not self.allstates['post_particles'].waskilled:
                    self.databox.set_instrdone()
                    self.databox.set_particlesdone()
                    raise Exception("TraceReader - mcrun process exited with code: %s" % str(poll))

            # If we made it all the way here, sim has ended or been killed
            self.databox.set_particlesdone()

        except Exception as e:
            self.exc_obj = e

class TestTraceReader:
    ''' can be used with data from a file '''
    def _setcurrent(self, current, line):
        self.current = current
        current.add_line(line)
    
    def __init__(self, text):
        self.exc_obj = None
        
        allstates = {}
        databox = DataBox()
        
        allstates['particles'] = ParticlesTraceState(self._setcurrent, next=None, databox=databox)
        allstates['mcdisplay'] = McdisplayState(self._setcurrent, next=allstates['particles'], databox=databox)
        allstates['instr'] = InstrState(self._setcurrent, next=allstates['mcdisplay'], databox=databox)
        
        # remember
        self.current = allstates['instr']
        self.allstates = allstates
        self.databox = databox

        for line in text.splitlines():
            self.current.add_line(line + '\n')
        self.databox.set_particlesdone()
    
    def start(self):
        pass

    def join(self, timeout=-1):
        pass
    
    
class McrunPipeMan(object):
    '''
    Proxy class for setting up the McrunPipeThread thread and LineBuffer.
    These are intended to provide parallel processing and piping, due to 
    potentially long simulation execution times.
    '''
    cmd = ''
    instrdef_start = ''
    instrdef_end = ''
    
    def __init__(self, cmd, inspect=None, send_enter=False):
        self.cmd = cmd
        self.reader = TraceReader(cmd=cmd, inspect=inspect, use_defaultpars=send_enter)
    
    def start_pipe(self):
        self.reader.start()

    def join(self):
        self.reader.join(1000)
        if self.reader.exc_obj:
            print(self.reader.databox.get_comments())
            raise self.reader.exc_obj
    
    def read_particles(self):
        return self.reader.databox.get_particles()
    
    def read_instrdef(self):
        return self.reader.databox.get_instrdef()
    
    def read_comments(self):
        return self.reader.databox.get_comments()
