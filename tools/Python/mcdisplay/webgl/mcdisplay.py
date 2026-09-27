#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
mcdisplay webgl script.
'''
import os
import sys
import signal
import time
import logging
import json
import subprocess
import webbrowser
from pathlib import Path
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import threading

sys.path.append(str(Path(__file__).resolve().parent.parent.parent))

from mccodelib import mccode_config
from mccodelib.mcdisplayutils import McDisplayReader
from mccodelib.instrgeom import Vector3d
from mccodelib.utils import get_file_text_direct
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
    ''' writes instrument definitions to html/ js, then serves them as plain static files '''
    print("Launching WebGL... Once launched, server will run for " + str(timeout) + " s")
    def copy(a, b):
        shutil_copy(str(a), str(b))

    # The pre-built, static webgl app always lives next to this script -
    # whether that's a conda-installed tool dir or a dev checkout.
    sysdir = Path(__file__).resolve().parent

    dest = Path(dirname)
    if dest.exists():
        raise RuntimeError(f"The specified destination {dirname} already exists!")

    # Copy the pre-built app files - i.e. creating dest
    copytree(sysdir.joinpath('dist'), dest)

    # Write instrument
    json_instr = '%s' % json.dumps(instrument.jsonize(), indent=2)
    file_save(json_instr, dest.joinpath('instrument.json'))

    if raybundle is not None:
        # Write particles
        json_particles = '%s' % json.dumps(raybundle.jsonize(), indent=2)
    else:
        # write empty list
        json_particles = '[]'
    file_save(json_particles, dest.joinpath('particles.json'))

    # Exit if nobrowse flag has been set
    if nobrowse is not None and nobrowse:
        return

    # Serve dest as a plain static site. `dist` is a self-contained, hashed
    # Vite production build (relative fetch()es for instrument.json /
    # particles.json), so no Node/npm/Vite runtime is required to view it -
    # the stdlib HTTP server is sufficient and starts instantly.
    class Handler(SimpleHTTPRequestHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, directory=str(dest), **kwargs)
        def log_message(self, fmt, *args):
            pass  # keep stdout quiet; comment out to debug requests

    httpd = ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    port = httpd.server_address[1]
    server_thread = threading.Thread(target=httpd.serve_forever, daemon=True)
    server_thread.start()

    url = f"http://127.0.0.1:{port}/"
    print(f"Serving WebGL viewer at {url}")
    webbrowser.open(url)

    def shutdown(sig=None, frame=None):
        print('Received signal ' + str(sig)) if sig is not None else None
        httpd.shutdown()

    signal.signal(signal.SIGTERM, shutdown)
    signal.signal(signal.SIGINT, shutdown)
    if not os.name == 'nt':
        signal.signal(signal.SIGUSR1, shutdown)
        signal.signal(signal.SIGUSR2, shutdown)

    print('Press Ctrl+C to exit\n(visualisation server will terminate after ' + str(timeout) + ' s)')
    try:
        server_thread.join(timeout)
    except KeyboardInterrupt:
        pass
    if server_thread.is_alive():
        shutdown()

def file_save(data, filename):
    ''' saves data for debug purposes '''
    with open(filename, 'w') as f:
        f.write(data)

def main(instr=None, dirname=None, debug=None, n=None, timeout=None, **kwds):
    logging.basicConfig(level=logging.INFO)

    sysdir = Path(__file__).resolve().parent

    # Build the static webgl app (npm install && npm run build), directly in
    # sysdir, if it hasn't been built already (e.g. packaged with a prebuilt
    # dist/, or already built by a previous run/dev checkout).
    def run_npminstall():
        npminst = str(sysdir / "npminstall")
        errtool = mccode_config.configuration['MCCODE']+"_errmsg"
        if os.name == 'nt':
            npminst += ".bat"
            errtool += ".bat"

        warning="Warning: First launch of WEBGL display tool. Building the WebGL viewer (one-off, requires internet access, may take a minute or two). Please do not abort execution..."
        proc = subprocess.Popen([errtool, warning], stdout=subprocess.PIPE)
        try:
            proc = subprocess.Popen([npminst], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            print("Building WebGL viewer (npm install && npm run build)")
            for line in proc.stdout:
                 print(line.rstrip())
            print("Building WebGL viewer - stderr:")
            for line in proc.stderr:
                 print(line.rstrip())
            print("Done building WebGL viewer")
        except subprocess.CalledProcessError as e:
            print(f"npminstall failed: {e}")
            return None

    # 1st run setup (or a fresh dev checkout / repackage): build dist/ once if missing
    if not (sysdir / 'dist').is_dir():
        try:
            run_npminstall()
        except Exception as e:
            print("WebGL viewer could not be built in %s: %s " % (sysdir, e.__str__()))


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
    parser.add_argument('--timeout', dest='timeout', type=int, default=300, help='Shutdown time of the WebGL viewer server')
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
