#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
mcdisplay webgl script.
'''
import os
import sys
import re
import signal
import time
import logging
import json
import subprocess
import webbrowser
import time
from pathlib import Path
from http.server import HTTPServer, SimpleHTTPRequestHandler
import threading
import socket
if os.name=='nt':
    import _winapi

sys.path.append(str(Path(__file__).resolve().parent.parent.parent))

from mccodelib import mccode_config
from mccodelib.mcdisplayutils import McDisplayReader
from mccodelib.instrgeom import Vector3d
from mccodelib.utils import get_file_text_direct
from threading import Thread
from shutil import copy as shutil_copy, copytree, ignore_patterns

class SimpleWriter(object):
    ''' a minimal, django-omiting "glue file" writer tightly coupled to some comments in the file template.html '''
    def __init__(self, templatefile, html_filename, invcanvas):
        self.template = templatefile
        self.html_filename = html_filename
        self.invcanvas = invcanvas
    
    def write(self):
        # load and modify
        template = get_file_text_direct(self.template)
        lines = template.splitlines()
        for i in range(len(lines)):
            if 'INSERT_CAMPOS_HERE' in lines[i]:
                lines[i+2] = '        invert_canvas = %s; // line written by SimpleWriter' % 'true' if self.invcanvas else 'false'
        self.text = '\n'.join(lines)
        
        # write to disk
        try:
            f = open(self.html_filename, 'w')
            f.write(self.text)
        finally:
            f.close()

class DjangoWriter(object):
    ''' writes a django template from the instrument representation '''
    instrument = None
    text = ''
    templatefile = ''
    campos = None
    
    def __init__(self, instrument, templatefile, campos):
        self.instrument = instrument
        self.templatefile = templatefile
        self.campos = campos
        
        # django stuff
        from django.template import Context
        from django.template import Template
        self.Context = Context
        self.Template = Template
        from django.conf import settings
        settings.configure()
    
    def build(self):
        templ = get_file_text_direct(self.templatefile)
        t = self.Template(templ)
        c = self.Context({'instrument': self.instrument, 
            'campos_x': self.campos.x, 'campos_y': self.campos.y, 'campos_z': self.campos.z,})
        self.text = t.render(c)
    
    def save(self, filename):
        ''' save template to disk '''
        try:
            f = open(filename, 'w')
            f.write(self.text)
        finally:
            f.close()

def _write_html(instrument, html_filepath, first=None, last=None, invcanvas=False):
    ''' writes instrument definition to html/js '''
    
    # create camera view coordinates given the bounding box

    # render html
    templatefile = Path(__file__).absolute().parent.joinpath("template.html")
    writer = SimpleWriter(templatefile, html_filepath, invcanvas)
    writer.write()

def write_browse(instrument, raybundle, dirname, instrname, timeout, nobrowse=None, first=None, last=None, invcanvas=None, **kwds):
    ''' writes instrument definitions to html/ js '''
    print("Launching WebGL... Once launched, server will run for " + str(timeout) + " s")
    def copy(a, b):
        shutil_copy(str(a), str(b))

    suffix=""
    if os.getenv('CONDA_DEFAULT_ENV'):
            suffix="_" + os.path.basename(os.getenv('CONDA_DEFAULT_ENV'))
    if os.name == 'nt':
            source =  Path(os.path.join(os.path.expandvars("$USERPROFILE"),"AppData",mccode_config.configuration['MCCODE'],mccode_config.configuration['MCCODE_VERSION']+suffix,'webgl'))
    else:
            source =  Path(os.path.join(os.path.expandvars("$HOME"),"." + mccode_config.configuration['MCCODE'],mccode_config.configuration['MCCODE_VERSION']+suffix,'webgl'))

    dest = Path(dirname)
    if dest.exists():
        raise RuntimeError(f"The specified destination {dirname} already exists!")

    # Copy the app files - i.e. creating dest
    copytree(source.joinpath('dist'), dest)

    # Copy package.json from source to dest
    package_json_source = source.joinpath('package.json')
    package_json_dest = dest.joinpath('package.json')
    shutil_copy(package_json_source, package_json_dest)

    # Copy node_modules from source to destination
    node_modules_source = source.joinpath('node_modules')
    node_modules_dest = dest.joinpath('node_modules')
    if not os.name=='nt':
        os.symlink(node_modules_source, node_modules_dest)
    else:
        _winapi.CreateJunction(str(node_modules_source), str(node_modules_dest))

    # Ensure execute permissions for vite binary
    vite_bin = node_modules_dest.joinpath('.bin/vite')
    os.chmod(vite_bin, 0o755)

    # Copy start-vite.js script to destination
    start_vite_source = source.joinpath('start-vite.js')
    start_vite_dest = dest.joinpath('start-vite.js')
    shutil_copy(start_vite_source, start_vite_dest)

    # Write instrument
    json_instr = '%s' % json.dumps(instrument.jsonize(), indent=2)
    file_save(json_instr, dest.joinpath('instrument.json'))

    # Write particles
    json_particles = '%s' % json.dumps(raybundle.jsonize(), indent=2)
    file_save(json_particles, dest.joinpath('particles.json'))

    # Exit if nobrowse flag has been set
    if nobrowse is not None and nobrowse:
        return

    destdist = dest

    # Function to run npm commands and capture port
    def run_npm_and_capture_port(port_container):
        if not os.name == 'nt':
            npmexe = "npm"
        else:
            npmexe = "npm.cmd"
        ansi_escape = re.compile(r'\x1B\[[0-?]*[ -/]*[@-~]')
        try:
            proc = subprocess.Popen([npmexe,"run","dev"], cwd=str(destdist), stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            for line in proc.stdout:
                line = ansi_escape.sub('', line)
                print(line)
                if 'Local:' in line:
                    parts = line.strip().split(' ')
                    for part in parts:
                        if part.startswith('http://localhost:'):
                            port = part.split(':')[-1].rstrip('/')
                            port_container['port'] = port
                            port_container['process'] = proc
                            return
        except subprocess.CalledProcessError as e:
            print(f"npm run dev failed: {e}")
            return None

    def signal_handler(sig, frame):
        global port_container
        print('Received signal ' + str(sig))
        port_container['process'].send_signal(signal.SIGTERM)
        sys.exit(0)

    # Container to hold the port information
    global port_container
    port_container = {'port': None, 'process': None}

    # Start npm and capture the port in a separate thread
    npm_thread = Thread(target=lambda: run_npm_and_capture_port(port_container))
    npm_thread.start()
    npm_thread.join()

    # If a port was found, open the browser
    if port_container['port']:
        print("Opening browser...")
        webbrowser.open(f"http://localhost:{port_container['port']}/")
    else:
        print("Failed to determine the localhost port")
    if port_container['process']:
        signal.signal(signal.SIGTERM, signal_handler)
        signal.signal(signal.SIGINT, signal_handler)

        if not os.name == 'nt':
            signal.signal(signal.SIGUSR1, signal_handler)
            signal.signal(signal.SIGUSR2, signal_handler)
            print('Press Ctrl+C to exit\n(visualisation server will terminate server after ' + str(timeout) + ' s)')
            time.sleep(timeout)
            print("Sending SIGTERM to npm/vite server")
            port_container['process'].send_signal(signal.SIGTERM)
            sys.exit(0)
        else:
            print('Press Ctrl+C to exit visualisation server')
            port_container['process'].wait()
            sys.exit(0)

def file_save(data, filename):
    ''' saves data for debug purposes '''
    with open(filename, 'w') as f:
        f.write(data)

def main(instr=None, dirname=None, debug=None, n=None, timeout=None, **kwds):
    logging.basicConfig(level=logging.INFO)

    # Function to run npm commands and capture port
    def run_npminstall(envsuffix):
        toolpath=Path(__file__).absolute().parent
        npminst = str(toolpath / "npminstall")
        errtool = mccode_config.configuration['MCCODE']+"_errmsg"
        if os.name == 'nt':
            npminst += ".bat"
            errtool += ".bat"


        warning="Warning: First launch of WEBGL display tool. Executing 'npminstall' for installation of required modules. Please do not abort execution...  Requires internet access and may take minutes!"
        proc = subprocess.Popen([errtool, warning], stdout=subprocess.PIPE)
        try:
            proc = subprocess.Popen([npminst, envsuffix], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            print("Installing npm / vite modules")
            for line in proc.stdout:
                 print(line.rstrip())
            print("Installing npm / vite modules - stderr:")  
            for line in proc.stderr:
                 print(line.rstrip())            
            print("Done installing npm / vite modules")
        except subprocess.CalledProcessError as e:
            print(f"npminstall failed: {e}")
            return None
    
    # 1st run setup: Check if the user has a "webgl" folder or not
    suffix=""
    if os.getenv('CONDA_DEFAULT_ENV'):
            suffix="_" + os.path.basename(os.getenv('CONDA_DEFAULT_ENV'))
    if os.name == 'nt':
            userdir =  os.path.join(os.path.expandvars("$USERPROFILE"),"AppData",mccode_config.configuration['MCCODE'],mccode_config.configuration['MCCODE_VERSION']+suffix,'webgl')
    else:
            userdir =  os.path.join(os.path.expandvars("$HOME"),"." + mccode_config.configuration['MCCODE'],mccode_config.configuration['MCCODE_VERSION']+suffix,'webgl')

    if not os.path.isdir(userdir):
        try:
            run_npminstall(suffix)
        except Exception as e:
            print("Local WebGL Directory %s could not be created: %s " % (userdir, e.__str__()))


    # output directory
    if dirname is None:
        from datetime import datetime as dt
        p = Path(instr).absolute()
        dirname = str(p.parent.joinpath(f"{p.stem}_{dt.strftime(dt.now(), '%Y%m%d_%H%M%S')}"))
    
    # set up a pipe, read and parse the particle trace
    reader = McDisplayReader(instr=instr, n=n, dir=dirname, debug=debug, **kwds)
    instrument = reader.read_instrument()
    raybundle = reader.read_particles()
    
    # write output files
    write_browse(instrument, raybundle, dirname, instr, timeout, **kwds)

    if debug:
        # this should enable template.html to load directly
        jsonized = json.dumps(instrument.jsonize(), indent=0)
        file_save(jsonized, 'jsonized.json')

if __name__ == '__main__':
    from mccodelib.mcdisplayutils import make_common_parser
    # Only pre-sets instr, --default, options
    parser, prefix = make_common_parser(__file__, __doc__)
    parser.add_argument('--dirname', '-d', help='output directory name override')
    parser.add_argument('--inspect', help='display only particle rays reaching this component')
    parser.add_argument('--nobrowse', action='store_true', help='do not open a webbrowser viewer')
    parser.add_argument('--invcanvas', action='store_true', help='invert canvas background from black to white')
    parser.add_argument('--first', help='zoom range first component')
    parser.add_argument('--last', help='zoom range last component')
    parser.add_argument('-n', '--ncount', dest='n', type=float, default=300, help='Number of particles to simulate')
    parser.add_argument('-t', '--trace', dest='trace', type=int, default=2, help='Select visualization mode')
    parser.add_argument('--timeout', dest='timeout', type=int, default=300, help='Shutdown time of npm/vite server')
    args, unknown = parser.parse_known_args()
    # Convert the defined arguments in the args Namespace structure to a dict
    args = {k: args.__getattribute__(k) for k in dir(args) if k[0] != '_'}
    # if --inspect --first or --last are given after instr, the remaining args become "unknown",
    # but we assume that they are instr_options
    if len(unknown):
        args['options'] = unknown

    try:
        main(**args)
    except KeyboardInterrupt:
        print('')

