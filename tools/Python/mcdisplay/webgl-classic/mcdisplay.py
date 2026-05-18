#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
mcdisplay webgl-classic script.
'''
import sys
import logging
import json
import subprocess
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parent.parent.parent))

from mccodelib import mccode_config
from mccodelib.mcdisplayutils import McDisplayReader
from mccodelib.instrgeom import Vector3d
from mccodelib.utils import get_file_text_direct

class SimpleWriter(object):
    ''' a minimal, django-omiting "glue file" writer tightly coupled to some comments in the file template.html '''
    def __init__(self, templatefile, campos, box, html_filename, invcanvas):
        self.template = templatefile
        self.campos = campos
        self.box = box
        self.html_filename = html_filename
        self.invcanvas = invcanvas
    
    def write(self):
        # load and modify
        template = get_file_text_direct(self.template)
        lines = template.splitlines()
        for i in range(len(lines)):
            if 'INSERT_CAMPOS_HERE' in lines[i]:
                lines[i] = '        campos_x = %s, campos_y = %s, campos_z = %s; // line written by SimpleWriter' % (str(self.campos.x), str(self.campos.y), str(self.campos.z))
                box = self.box
                lines[i+1] = '        box_x1 = %s, box_x2 = %s, box_y1 = %s, box_y2 = %s, box_z1 = %s, box_z2 = %s; // line written by SimpleWriter' % (str(box.x1), str(box.x2), str(box.y1), str(box.y2), str(box.z1), str(box.z2))
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
    box = instrument.get_boundingbox(first, last)
    box_total = instrument.get_boundingbox()
    
    # create camera view coordinates given the bounding box
    dx = box.x2 - box.x1
    dy = box.y2 - box.y1
    dz = box.z2 - box.z1
    x = -(box.x1 + max(dx, dz)/2)
    y = max(dx, dz)/2
    z = box.z1 + dz/2
    campos = Vector3d(x, y, z)
    
    # render html
    templatefile = Path(__file__).absolute().parent.joinpath("template.html")
    writer = SimpleWriter(templatefile, campos, box_total, html_filepath, invcanvas)
    writer.write()

def write_browse(instrument, raybundle, dirname, instrname, nobrowse=None, first=None, last=None, invcanvas=None, **kwds):
    ''' writes instrument definitions to html/ js '''
    from shutil import copy as shutil_copy
    def copy(a, b):
        shutil_copy(str(a), str(b))

    source = Path(__file__).absolute().parent
    dest = Path(dirname)
    if dest.exists() and not dest.is_dir():
        raise RuntimeError(f"The specified destination {dirname} exists but is not a directory")
    if not dest.exists():
        dest.mkdir(parents=True)

    # copy mcdisplay.js to _mcdisplay.js
    copy(source.joinpath('mcdisplay.js'), dest.joinpath('_mcdisplay.js'))
    # copy JavaScript files without name changes
    for file in ('three.min', 'dat.gui.min', 'OrbitControls', 'Lut', 'jquery.min'):
        copy(source.joinpath(f'{file}.js'), dest.joinpath(f'{file}.js'))
    
    # write html
    html_filepath = str(dest.joinpath('index.html'))
    _write_html(instrument, html_filepath, first=first, last=last, invcanvas=invcanvas)
    
    # write instrument
    json_instr = 'MCDATA_instrdata = %s;' % json.dumps(instrument.jsonize(), indent=0)
    file_save(json_instr, dest.joinpath('_instr.js'))

    if raybundle is not None:
        # write particles
        json_particles = 'MCDATA_particledata = %s;' % json.dumps(raybundle.jsonize(), indent=0)
    else:
        # write empty list
        json_particles = 'MCDATA_particledata = [];'
    file_save(json_particles, dest.joinpath('_particles.js'))

    # Workaround for allowing non-relative paths to instrname
    # see https://github.com/mccode-dev/McCode/issues/1426
    temp_instrname = Path(instrname)
    instrname = temp_instrname.name

    # copy McCode instrument (this *may* be a binary!)
    shutil_copy(instrname, str(dest.joinpath(instrname)))
    
    # exit if nobrowse flag has been set
    if nobrowse is not None and nobrowse:
        return
    
    # open a web-browser in a cross-platform way
    try:
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], html_filepath), shell=True)
    except Exception as e:
        raise Exception('Os-specific open browser: %s' % e.__str__())
    
def file_save(data, filename):
    ''' saves data for debug purposes '''
    with open(filename, 'w') as f:
        f.write(data)

def main(instr=None, dirname=None, debug=None, n=None, **kwds):
    logging.basicConfig(level=logging.INFO)
    
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
    write_browse(instrument, raybundle, dirname, instr, **kwds)

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
    parser.add_argument('-t', '--trace', dest='trace', type=int, default=1, help='Select visualization mode')

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

