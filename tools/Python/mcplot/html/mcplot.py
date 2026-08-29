#!/usr/bin/env python3

"""Display simulation results as HTML pages for single monitor files, and directories (generates an overview)."""
import argparse
import logging
import os
import sys
import numpy as np
import io
import base64
import json
import subprocess
import concurrent.futures

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

from mccodelib.mcplotloader import McCodeDataLoader, Data1D, Data2D
from mccodelib.plotgraph import PNSingle, PNMultiple
from mccodelib import mccode_config
from shutil import copyfile
from PIL import Image

global WIDTH,HEIGHT
WIDTH = 1024
HEIGHT = 768

def get_default_workers():
    ''' Number of per-monitor plot jobs to run in parallel by default: the
        number of processors available to this process. Prefers
        os.sched_getaffinity(0) (Linux only) over os.cpu_count(), since the
        former respects cgroup/taskset CPU restrictions (e.g. inside a
        container or batch job allocation) that the latter ignores. '''
    try:
        return len(os.sched_getaffinity(0))
    except AttributeError:
        # os.sched_getaffinity doesn't exist on macOS/Windows
        return os.cpu_count() or 4

def file_base_name(file_name):
    if '.' in file_name:
        separator_index = file_name.index('.')
        base_name = file_name[:separator_index]
        return base_name
    else:
        return file_name

def path_base_name(path):
    file_name = os.path.basename(path)
    return file_base_name(file_name)

def get_html(template_name, params, simfile, use_logscale):
    '''  '''
    text = open(os.path.join(os.path.dirname(__file__),template_name)).read()
    text = text.replace("@PARAMS@", params)
    text = text.replace("@DATAFILE@", simfile)
    text = text.replace("@DATALINK@", "Click for ascii data")

    logscalestr = "true" if use_logscale else "false"
    text = text.replace("@LOGSCALE@", logscalestr)
    text = text.replace("@LIBPATH@", libpath)
    return text

def get_json_1d(x, y, yerr, xlabel, ylabel, title):
    params = {}
    p = params
    p['w'] = WIDTH
    p['h'] = HEIGHT
    p['x'] = x
    p['y'] = y
    p['yerr'] = yerr
    p['xlabel'] = xlabel
    p['ylabel'] = ylabel
    p['title'] = title
    if autosize:
        p['autosize'] = True
    return json.dumps(params, indent=4)

def get_json_2d(xmin, xmax, ymin, ymax, image_str, colorbar_img_str, cb_min, cb_max, image_str_log,
                colorbar_img_str_log, cb_min_log, cb_max_log, xlabel, ylabel, title):
    params = {}
    p = params
    p['w'] = WIDTH
    p['h'] = HEIGHT
    p['xmin'] = xmin
    p['xmax'] = xmax
    p['ymin'] = ymin
    p['ymax'] = ymax

    p['img2dData'] = image_str
    p['imgColorbar'] = colorbar_img_str
    p['cbMin'] = cb_min
    p['cbMax'] = cb_max
    
    p['img2dDataLog'] = image_str_log
    p['imgColorbarLog'] = colorbar_img_str_log
    p['cbMinLog'] = cb_min_log 
    p['cbMaxLog'] = cb_max_log
    
    p['xlabel'] = xlabel
    p['ylabel'] = ylabel
    p['title'] = title
    if autosize:
        p['autosize'] = True
    return json.dumps(params, indent=4)

def get_cm():
    ''' colour map for 2d plotting '''
    return np.array([[  0,   0, 143, 255], [  0,   0, 159, 255], [  0,   0, 175, 255], [  0,   0, 191, 255], [  0,   0, 207, 255], [  0,   0, 223, 255], [  0,   0, 239, 255], [  0,   0, 255, 255], [  0,  16, 255, 255], [  0,  32, 255, 255], [  0,  48, 255, 255], [  0,  64, 255, 255], [  0,  80, 255, 255], [  0,  96, 255, 255], [  0, 112, 255, 255], [  0, 128, 255, 255], [  0, 143, 255, 255], [  0, 159, 255, 255], [  0, 175, 255, 255], [  0, 191, 255, 255], [  0, 207, 255, 255], [  0, 223, 255, 255], [  0, 239, 255, 255], [  0, 255, 255, 255], [ 16, 255, 239, 255], [ 32, 255, 223, 255], [ 48, 255, 207, 255], [ 64, 255, 191, 255], [ 80, 255, 175, 255], [ 96, 255, 159, 255], [112, 255, 143, 255], [128, 255, 128, 255], [143, 255, 112, 255], [159, 255,  96, 255], [175, 255,  80, 255], [191, 255,  64, 255], [207, 255,  48, 255], [223, 255,  32, 255], [239, 255,  16, 255], [255, 255,   0, 255], [255, 239,   0, 255], [255, 223,   0, 255], [255, 207,   0, 255], [255, 191,   0, 255], [255, 175,   0, 255], [255, 159,   0, 255], [255, 143,   0, 255], [255, 128,   0, 255], [255, 112,   0, 255], [255,  96,   0, 255], [255,  80,   0, 255], [255,  64,   0, 255], [255,  48,   0, 255], [255,  32,   0, 255], [255,  16,   0, 255], [255,   0,   0, 255], [239,   0,   0, 255], [223,   0,   0, 255], [207,   0,   0, 255], [191,   0,   0, 255], [175,   0,   0, 255], [159,   0,   0, 255], [143,   0,   0, 255], [128,   0,   0, 255]], dtype=np.ubyte)

def lookup(cm, x):
    # Real number from 0 to len(cm)-1 [0:63]
    xp = (len(cm)-1) * x
    # Simply round off and return
    if np.isnan(xp):
        #idx=np.int(len(cm)-1)
        idx=0
    else:
        idx = int(np.round(xp))
    return cm[idx]

def get_params_str_1D(data):
    x = data.xvals
    y = data.yvals
    yerr = data.y_err_vals
    try:
        title = '%s [%s] %s\nI = %s Err = %s N = %s\n %s' % (data.component, data.filename, data.title, data.values[0], data.values[1], data.values[2], data.statistics)
    except:
        title = '%s\n[%s]' % (data.component, data.filename)
    return get_json_1d(x, y, yerr, data.xlabel, data.ylabel, title)

def get_params_str_2D(data):
    vals = np.array(data.zvals)
    dims = np.shape(vals)

    # create the 2d data as a png given our colormap
    img = np.zeros((dims[0], dims[1], 4))
    img_log = np.zeros((dims[0], dims[1], 4))
    maxval = np.max(vals)
    cm = get_cm()
    for i in range(dims[0]):
        for j in range(dims[1]):
            if maxval:
                color = lookup(cm, vals[i,j]/maxval)
                img[i,j,:] = color

    # encode png as base64 string
    image = Image.fromarray(np.flipud(img).astype(np.uint8))
    output = io.BytesIO()
    image.save(output, format="png")
    contents = output.getvalue()
    output.close()
    encoded_2d_data = str(base64.b64encode(contents)).lstrip('b').strip("\'")

    # create log data as another png
    img_log = np.zeros((dims[0], dims[1], 4))
    minval = 1e19
    for row in vals:
        for val in row:
            if val > 0 and val < minval:
                minval = val
    minval_log = np.log10(minval/10)
    signal_log = np.ma.log10(vals).filled(minval_log) # masked array filling in zeros (zero must be mask false...)
    maxval_log = np.max(signal_log)
    for i in range(dims[0]):
        for j in range(dims[1]):
            try:
                img_log[i,j,:] = lookup(cm, (signal_log[i][j] - minval_log)/(maxval_log - minval_log))
            except Exception as e:
                print(e)

    # encode png as base64 string
    image_log = Image.fromarray(np.flipud(img_log).astype(np.uint8))
    output = io.BytesIO()
    image_log.save(output, format="png")
    encoded_2d_data_log = str(base64.b64encode(output.getvalue())).lstrip('b').strip("\'")
    output.close()

    # create the colorbar as a 1 x 256 image
    img = np.zeros((256, 1, 4))
    for i in range(256):
        color = lookup(cm, i/255)
        img[255-i, 0] = color
    cb_img = Image.fromarray(img.astype(np.uint8))
    output = io.BytesIO()
    cb_img.save(output, format='png')
    contents = output.getvalue()
    output.close()
    encoded_cb = str(base64.b64encode(contents)).lstrip('b').strip("\'")

    # log color bar
    tmpimg = np.zeros((256, 1, 4))
    for i in range(256):
        color = lookup(cm, i/255)
        tmpimg[255-i, 0] = color
    cb_img_log = Image.fromarray(tmpimg.astype(np.uint8))
    output = io.BytesIO()
    cb_img_log.save(output, format='png')
    encoded_cb_log = str(base64.b64encode(output.getvalue())).lstrip('b').strip("\'")
    output.close()

    # axis limits
    xmin = data.xlimits[0]
    xmax = data.xlimits[1]
    ymin = data.xlimits[2]
    ymax = data.xlimits[3]

    # color bar limits
    cb_min = np.min(vals)
    cb_max = np.max(vals)
    cb_min_log = np.min(signal_log)
    cb_max_log = np.max(signal_log)

    # generate the title
    verbose = True
    try:
        title = '%s\nI = %s' % (data.component, data.values[0])
        if verbose:
            title = '%s [%s] %s\nI = %s Err = %s N = %s\n %s' % (data.component, data.filename, data.title, data.values[0], data.values[1], data.values[2], data.statistics)
    except:
        title = '%s\n[%s]' % (data.component, data.filename)

    return get_json_2d(xmin, xmax, ymin, ymax, 
                       encoded_2d_data, encoded_cb, cb_min, cb_max,
                       encoded_2d_data_log, encoded_cb_log, cb_min_log, cb_max_log,
                       data.xlabel, data.ylabel, title)

def browse(html_filepath):
    # open a web-browser in a cross-platform way
    try:
        subprocess.Popen('%s %s' % (mccode_config.configuration['BROWSER'], html_filepath), shell=True)
    except Exception as e:
        raise Exception('Os-specific open browser: %s' % e.__str__())

def plotfunc(node, filename=None):
    ''' plot a plotnode to a html file as an svg-plot using plotfuncs.js and d3.v4.min.js '''

    if isinstance(filename, list):
        filename = filename[0]
    # get data and set file path
    if type(node) is PNSingle:
        data = node.getdata_idx(0)
        return plotfunc_single(data, filename, use_logscale=logscale)

    elif type(node) is PNMultiple:
        data_lst = node.getdata_lst()

        # Each monitor's linear and log-scale pages are fully independent
        # (own numpy arrays, own PIL image, own output file - no shared
        # matplotlib-style global figure/axes state the way the
        # mcplot-matplotlib tool has), so unlike that tool this is safe to
        # parallelize directly: build the full list of (monitor, use_log)
        # jobs up front and run them through a bounded thread pool, instead
        # of the previous sequential loop that flipped a shared global
        # `logscale` flag between calls (unsafe to parallelize as-is, since
        # concurrent threads would race on that flag).
        jobs = []
        for dat in data_lst:
            jobs.append((dat, False))
            jobs.append((dat, True))

        def _run_job(job):
            dat, use_log = job
            return plotfunc_single(dat, None, use_logscale=use_log)

        max_workers = get_default_workers()
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            # executor.map preserves input order regardless of completion
            # order, so results[0::2]/[1::2] line up with data_lst as before
            results = list(executor.map(_run_job, jobs))

        f     = results[0::2]
        f_log = results[1::2]
        # now create an overview HTML (index.html with <iframe>)
        directory  = os.path.dirname(f[0])
        baseinstr = os.path.basename(os.getcwd())
        print(baseinstr)
        print(os.getcwd())
        if filename is None:
            filename = os.path.join(directory, "index.html")
            
        with open(filename, 'w') as outfile:
            outfile.write("<html><head>\n")
            outfile.write(f"<title>Simulation results in folder {baseinstr}/{directory}</title>\n")
            gridgap = int(round(WIDTH * 0.05))
            initial_scale = 0.5
            init_w = WIDTH * initial_scale
            init_h = HEIGHT * initial_scale
            init_pct = int(round(initial_scale * 100))
            outfile.write("<style>\n")
            outfile.write("  body { background-color: #e0e0e0; margin: 12px; font-family: sans-serif; }\n")
            outfile.write(f"  .plotgrid {{ display: flex; flex-wrap: wrap; gap: {gridgap}px; }}\n")
            outfile.write("  .plotcell { background-color: #ffffff; display: flex; flex-direction: column; align-items: flex-start; }\n")
            outfile.write("  .plotcell iframe { border: none; }\n")
            outfile.write("  .plotcell .links { margin-top: 4px; width: 100%; box-sizing: border-box; display: flex; flex-wrap: wrap; gap: 4px 12px; }\n")
            outfile.write("  .iframe-wrap { overflow: hidden; border: 2px solid #b0b0b0; }\n")
            outfile.write("  .iframe-wrap iframe { transform-origin: top left; display: block; }\n")
            outfile.write("  #sizecontrol { margin-bottom: 16px; font-size: 14px; }\n")
            outfile.write("  #sizecontrol input[type=range] { vertical-align: middle; margin: 0 8px; }\n")
            outfile.write("</style>\n")
            outfile.write("</head><body>\n")
            outfile.write(f"<h1>Simulation results {baseinstr}/{directory}</h1>\n")
            outfile.write("<div id='sizecontrol'>\n")
            outfile.write("  <label for='sizeslider'>Figure size:</label>\n")
            outfile.write(f"  <input type='range' id='sizeslider' min='20' max='200' value='{init_pct}' step='5'>\n")
            outfile.write(f"  <span id='sizevalue'>{init_pct}%</span>\n")
            outfile.write("</div>\n")
            outfile.write("<div class='plotgrid'>\n")
            for fname in f:
                basename = os.path.basename(fname)
                if os.path.dirname(filename) == directory:  
                    # we are saving all in the simulation dir
                    # we can use relative path
                    linkname = basename
                else:
                    # we are saving outside simulation dir
                    # we use full path
                    linkname = os.path.join(directory, basename)
                outfile.write(f"<div class='plotcell' data-base-w='{WIDTH}' style='width:{init_w}px;'>\n")
                outfile.write(f"<div class='iframe-wrap' data-base-w='{WIDTH}' data-base-h='{HEIGHT}' style='width:{init_w}px;height:{init_h}px;'>\n")
                outfile.write(f"<iframe src='{linkname}' title='{basename}' width={WIDTH} height={HEIGHT} style='transform:scale({initial_scale});'></iframe>\n")
                outfile.write("</div>\n")
                outfile.write("<div class='links'>\n")
                outfile.write(f"<a href='{linkname}' target=_blank>[ {basename} ]</a>\n")
                # add the LOG-scale plot, if any
                filepart = os.path.splitext(basename)
                basename = filepart[0]+"_log"+filepart[1]
                if os.path.isfile(os.path.join(directory, basename)):
                    # append log scale plot link
                    if os.path.dirname(filename) == directory:  
                        # we are saving all in the simulation dir
                        # we can use relative path
                        linkname = basename
                    else:
                        # we are saving outside simulation dir
                        # we use full path
                        linkname = os.path.join(directory, basename)
                    outfile.write(f"<a href='{basename}' target=_blank>[ {basename} ]</a>\n")
                outfile.write("</div>\n")
                outfile.write("</div>\n")
            outfile.write("</div>\n")
            outfile.write("<script>\n")
            outfile.write("  var slider = document.getElementById('sizeslider');\n")
            outfile.write("  var sizevalue = document.getElementById('sizevalue');\n")
            outfile.write("  var wraps = document.querySelectorAll('.iframe-wrap');\n")
            outfile.write("  var cells = document.querySelectorAll('.plotcell');\n")
            outfile.write("  slider.addEventListener('input', function() {\n")
            outfile.write("    var scale = slider.value / 100;\n")
            outfile.write("    sizevalue.textContent = slider.value + '%';\n")
            outfile.write("    wraps.forEach(function(wrap) {\n")
            outfile.write("      var bw = parseFloat(wrap.dataset.baseW);\n")
            outfile.write("      var bh = parseFloat(wrap.dataset.baseH);\n")
            outfile.write("      wrap.style.width = (bw * scale) + 'px';\n")
            outfile.write("      wrap.style.height = (bh * scale) + 'px';\n")
            outfile.write("      var fr = wrap.querySelector('iframe');\n")
            outfile.write("      fr.style.transform = 'scale(' + scale + ')';\n")
            outfile.write("    });\n")
            outfile.write("    cells.forEach(function(cell) {\n")
            outfile.write("      var bw = parseFloat(cell.dataset.baseW);\n")
            outfile.write("      cell.style.width = (bw * scale) + 'px';\n")
            outfile.write("    });\n")
            outfile.write("  });\n")
            outfile.write("</script>\n")
            outfile.write("</body></html>\n")
        return filename
    
def plotfunc_single(data, f = None, use_logscale=False):
    """save the data node into given file"""
    # three cases of plot data (1D, 2D and multiple), each block should end with a fully formed 'text' variable
    text = ""
    
    if f is None:
        if use_logscale:
            f = data.filepath + "_log.html"
        else:
            f = data.filepath + ".html"
    
    if os.path.exists(f):
        os.remove(f)
    
    # create 1D html
    if type(data) is Data1D:
        text = get_html('template_1d.html', get_params_str_1D(data), os.path.basename(data.filename), use_logscale)

    # create 2D html
    elif type(data) is Data2D:
        text = get_html('template_2d.html', get_params_str_2D(data), os.path.basename(data.filename), use_logscale)
    
    # write to file
    with open(f, 'w') as fid:
        fid.write(text)
        fid.write("")
    return f

def plotgraph_recurse(node, action_on_node):
    ''' depth-first tree iteration (can not handle circular graphs) '''
    action_on_node(node)
    for primary_child in node.get_primaries():
        plotgraph_recurse(primary_child, action_on_node)
    for secondary_child in node.get_secondaries():
        plotgraph_recurse(secondary_child, action_on_node)

logscale = False
libpath = ""
autosize = False
def main(args):
    """Main routine for mcplot-html"""
    logging.basicConfig(level=logging.INFO)

    if len(args.simulation) == 0:
        simfile = ''
    else:
        simfile = args.simulation[0]
        if os.path.isfile(simfile):
            simdir = os.path.dirname(simfile)
        elif os.path.isdir(simfile):
            simdir = simfile
            simfile = os.path.join(simdir,'mccode.sim')
        else:
            print(simfile + " is neither a .sim file or directory, exiting")
            exit(-1)

    # logscale house keeping
    global logscale
    if args.log: 
        logscale = True
    global libpath
    if args.libpath:
        libpath = args.libpath[0] + "/"
    global autosize
    if args.autosize:
        autosize = True
    global WIDTH
    if args.width:
        WIDTH=int(args.width[0])
    global HEIGHT
    if args.height:
        HEIGHT=int(args.height[0])

    # TODO: safeguard, exit: if simfile is not a file or a directory

    # copy plotfuncs.js and d3.v4.min.js locally if no lib path was specified
    if libpath == "":
        # libdir undefined, copy js lib files to plotting dir
        if os.path.isdir(simdir):
            copyfile(os.path.join(os.path.dirname(__file__),'d3.v4.min.js'), os.path.join(simdir, 'd3.v4.min.js'))
            copyfile(os.path.join(os.path.dirname(__file__),'plotfuncs.js'), os.path.join(simdir, 'plotfuncs.js'))
        elif os.path.isdir(simfile):
            copyfile(os.path.join(os.path.dirname(__file__),'d3.v4.min.js'), os.path.join(simfile, 'd3.v4.min.js'))
            copyfile(os.path.join(os.path.dirname(__file__),'plotfuncs.js'), os.path.join(simfile, 'plotfuncs.js'))

    # load data
    loader = McCodeDataLoader(simfile=simfile)
    try:
        loader.load()
    except Exception as e:
        print('mcplot loader: ' + e.__str__())
        quit()
    rootnode = loader.plot_graph

    # generate html file(s)
    f = plotfunc(rootnode, args.output) # can be a single or multiple 'plotnode'
    print(f"Generated: {f}")

    # browse if input was a specific, and therefore browsable, file
    if not args.nobrowse and os.path.isfile(f):
        browse(f)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('simulation', nargs='*', help='file or directory to plot')
    parser.add_argument('-n','--nobrowse', action='store_true', help='do not open a webbrowser viewer')
    parser.add_argument('-l','--log', action='store_true', help='enable logscale on plot')
    parser.add_argument('--autosize', action='store_true', help='expand to window size on load')
    parser.add_argument('--libpath', nargs='*', help='js lib files path')
    parser.add_argument('-o','--output', nargs=1, help='specify output file (.html extension)')
    parser.add_argument('-W','--width', nargs=1, help='width of iframes')
    parser.add_argument('-H','--height', nargs=1, help='heigth of iframes')
    args = parser.parse_args()

    main(args)
