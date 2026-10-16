#!/usr/bin/env python3
'''
mcplopp: plot McStas/McXtrace output with plopp (scipp).

UI:      mcplopp.py <dir|mccode.sim|monitor.dat|mccode.h5>       (opens JupyterLab)
Static:  mcplopp.py <dir|mccode.sim|monitor.dat|mccode.h5> -o out.png [--log] [--source S --sample S]
Events:  Monitor_nD pixel/list data in NeXus files via McStasToX, given the source and sample components
'''
import argparse
import json
import math
import os
import re
import subprocess
import sys

import numpy as np
try:
    import scipp as sc
    import plopp as pp
except ImportError as e:
    sys.exit(f'mcplot-plopp: {e}. scipp and plopp are required for this tool to function '
             '(plus ipympl and jupyterlab for the interactive UI), e.g.:\n'
             '  conda install -c conda-forge plopp ipympl jupyterlab\n'
             '  python3 -m pip install plopp ipympl jupyterlab')

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))
from mccodelib.mcplotloader import McCodeDataLoader, Data1D, Data2D
from mccodelib.plotgraph import PNMultiple

UNITS = {'AA': 'angstrom', 'Angs': 'angstrom', 'meV': 'meV', 'deg': 'deg', 'us': 'us', 'cm': 'cm', 'm': 'm', 's': 's'}


def _edges(label, lo, hi, n):
    ''' bin-edge coord from an axis label like "Wavelength [AA]"; unknown units stay in the label '''
    m = re.search(r'\s*\[(.*?)\]', label)
    unit = UNITS.get(m.group(1)) if m else None
    dim = label.replace(m.group(0), '').strip() if unit else label.strip()
    return dim, sc.linspace(dim, lo, hi, n + 1, unit=unit or '')


def _curve(I, err, xlabel, lim):
    x, ex = _edges(xlabel, *lim, len(I))
    return sc.DataArray(sc.array(dims=[x], values=I, variances=np.square(err)), coords={x: ex})


def _image(z, err, xlabel, ylabel, lim):
    ''' z is (ny, nx), rows are y, as in the ASCII files and the matplotlib frontend '''
    x, ex = _edges(xlabel, *lim[:2], z.shape[1])
    y, ey = _edges(ylabel, *lim[2:4], z.shape[0])
    if y == x:
        y += ' (y)'
        ey = ey.rename_dims({x: y})
    return sc.DataArray(sc.array(dims=[y, x], values=z, variances=None if err is None else np.square(err)),
                        coords={x: ex, y: ey})


def to_dataarray(d):
    ''' Data1D/Data2D -> DataArray with bin edges; 0D/event lists -> None '''
    if isinstance(d, Data1D):
        da = _curve(d.yvals, d.y_err_vals, d.xlabel, d.xlimits)
    elif isinstance(d, Data2D):
        # ponytail: no variances for 2D, mcplotloader skips the "# Errors" block
        da = _image(np.array(d.zvals), None, d.xlabel, d.ylabel, d.xlimits)
    else:
        return None
    da.name = 'I'  # colorbar/y label; the title goes in the load() key
    return da


def _nx_dataarray(g):
    ''' McCode NeXus monitor (NXdata group) -> (title, DataArray), None for 0D / event lists '''
    a = {k: v.decode() if isinstance(v, bytes) else v for k, v in g.attrs.items()}
    typ = a.get('type', '')
    if typ.startswith('array_1d'):
        da = _curve(g['data'][()], g['errors'][()], a['xlabel'], [float(v) for v in a['xlimits'].split()])
    elif typ.startswith('array_2d'):
        # NeXus stores (nx, ny) with axis=1 on x, i.e. the transpose of the ASCII block
        da = _image(g['data'][()].T, g['errors'][()].T, a['xlabel'], a['ylabel'], [float(v) for v in a['xylimits'].split()])
    else:
        return None
    da.name = 'I'
    return a.get('title', '').strip(), da


def load_nexus(path):
    ''' {name: DataArray} from a McCode mccode.h5; one NXentry per scan step, stacked over the scanned
        parameter (taken from entry*/simulation/Param) as in load() '''
    try:
        import h5py
    except ImportError as e:
        sys.exit(f'mcplot-plopp: {e}. h5py is required to read NeXus files')
    with h5py.File(path, 'r') as f:
        entries = sorted(f, key=lambda e: int(re.sub(r'\D', '', e) or 0))
        steps = []
        for e in entries:
            comps = f[e]['instrument/components']
            mons = [_nx_dataarray(comps[c]['output'][m]) for c in sorted(comps) if 'output' in comps[c]
                    for m in comps[c]['output']]
            par = f[e]['simulation/Param'].attrs if 'simulation/Param' in f[e] else {}
            steps.append(([m for m in mons if m], {k: v.decode() if isinstance(v, bytes) else v for k, v in par.items()}))

    if len(steps) == 1:
        return {f'{t} [{i}]': da for i, (t, da) in enumerate(steps[0][0])}
    pars = [p for _, p in steps]
    scanned = [k for k in pars[0] if k != 'NX_class' and len({p.get(k) for p in pars}) > 1]
    dim = scanned[0] if scanned else 'step'  # ponytail: first varying parameter only, as mccode.dat's single xvar
    vals = [float(p[dim]) for p in pars] if scanned else np.arange(len(steps))
    out = {}
    for i, (t, _) in enumerate(steps[0][0]):
        da = sc.concat([mons[i][1] for mons, _ in steps], dim=dim)
        da.coords[dim] = sc.array(dims=[dim], values=vals)
        out[f'{t} [{i}]'] = da
    return out


def _nexus_file(path):
    ''' the NeXus file to read for path, or None for ASCII output '''
    if path.endswith(('.h5', '.nxs')):
        return path
    h5 = os.path.join(path, 'mccode.h5')
    if os.path.isfile(h5) and not os.path.isfile(os.path.join(path, 'mccode.sim')):  # as the matplotlib frontend
        return h5


AA2MS = 629.622368  # k[1/AA] -> v[m/s], as in mcstas-r.h; lambda[AA] = 2 pi AA2MS / v[m/s]


def _mcstastox():
    try:
        import mcstastox
        return mcstastox
    except ImportError as e:
        raise ImportError(f'{e}. McStasToX is required for event data: '
                          'conda install -c conda-forge mcstastox / python3 -m pip install mcstastox') from None


def event_components(path):
    ''' (all components, components with pixel-id event data) of a NeXus file; [] , [] for ASCII output '''
    h5 = _nexus_file(path)
    if not h5:
        return [], []  # ponytail: McStasToX reads NeXus only; ASCII list files lack pixel geometry and rotations
    with _mcstastox().Read(*os.path.split(os.path.abspath(h5))) as d:
        return d.file_object.get_components(), d.file_object.get_components_with_ids()


def load_events(path, source, sample):
    ''' Monitor_nD pixel events ("neutron pixel min=... t, list all neutrons") via McStasToX, grouped by pixel with
        pixel positions. Adds per-event wavelength from the source -> sample -> pixel flight path.
        McStasToX reads entry1 only, i.e. the first step of a NeXus scan '''
    with _mcstastox().Read(*os.path.split(os.path.abspath(_nexus_file(path)))) as d:
        ev = d.export_scipp(source_name=source, sample_name=sample)['events']
    c = ev.coords
    L = sc.norm(c['sample_position'] - c['source_position']) + sc.norm(c['position'] - c['sample_position'])
    ev.bins.coords['wavelength'] = sc.scalar(2 * np.pi * AA2MS, unit='angstrom*m/s') * ev.bins.coords['t'] / L
    return ev


def event_figures(ev, threed=False):
    ''' pixel intensity map (3D scatter in Jupyter, z-x top view otherwise) + time-of-flight and wavelength spectra '''
    pix = ev.bins.sum()
    if threed:
        det = pp.scatter3d(pix, pos='position', cbar=True, title='pixel intensity')
    else:
        for ax in 'xz':
            pix.coords[ax] = pix.coords['position'].fields.__getattribute__(ax)
        det = pp.scatter(pix, x='z', y='x', cbar=True, title='pixel intensity (top view)')
    allev = ev.bins.concat()
    return [det, pp.plot(allev.hist(t=200), title='time of flight'), pp.plot(allev.hist(wavelength=200), title='wavelength')]


def load(path):
    ''' {name: DataArray}. Scans stack each monitor over the scan parameter (1D->2D, 2D->3D). '''
    if _nexus_file(path):
        return load_nexus(_nexus_file(path))
    loader = McCodeDataLoader(simfile=path)
    loader.load()
    root = loader.plot_graph
    secs = root.get_secondaries()
    if not (secs and isinstance(secs[0], PNMultiple)):  # single run or single monitor
        ds = root.getdata_lst()
        return {f'{d.title.strip()} [{i}]': to_dataarray(d) for i, d in enumerate(ds) if isinstance(d, (Data1D, Data2D))}

    scan = root.getdata_idx(0)  # mccode.dat curve: xvals are the scan points
    out = {}
    for i, sec in enumerate(secs):
        steps = [to_dataarray(d) for d in sec.getdata_lst()]
        if steps[0] is None:
            continue
        da = sc.concat(steps, dim=scan.xvar)
        # ponytail: failed scan steps leave no subfolder; fall back to step index if counts differ
        vals = scan.xvals if len(scan.xvals) == len(steps) else np.arange(len(steps))
        da.coords[scan.xvar] = sc.array(dims=[scan.xvar], values=vals)
        out[f'{sec.getdata_idx(0).title.strip()} [{i}]'] = da
    return out


def scan_slicer(da, log=False):
    ''' Jupyter (ipympl) only: step through a stacked scan monitor '''
    return pp.slicer(da, keep=list(da.dims[1:]),  # load() puts the scan dim first
                     enable_player=True, mode='single', autoscale=log, norm='log' if log else 'linear')


def ui(path):
    ''' Jupyter (%matplotlib widget): pick a monitor; scans get the step player, log via the plot toolbar '''
    import ipywidgets as w
    from IPython.display import display
    das = load(path)
    pick = w.Dropdown(options=list(das), description='monitor', layout={'width': '40em'})
    out = w.Output()

    def show(_=None):  # ponytail: one figure at a time, so 100+ monitor instruments stay responsive
        da = das[pick.value]
        out.clear_output(wait=True)
        with out:
            display(scan_slicer(da) if not da.coords.is_edges(da.dims[0]) else pp.plot(da, title=pick.value))
    pick.observe(show, 'value')
    rows = []
    if das:  # a file can hold only event monitors
        show()
        rows = [pick, out]

    comps, banks = event_components(path)
    if banks:  # pixel events present: which components are source and sample needs the user
        guess = lambda key, i: next((c for c in comps if key in c.lower()), comps[i])
        src = w.Dropdown(options=comps, value=guess('source', 0), description='source')
        smp = w.Dropdown(options=comps, value=guess('sample', len(comps) // 2), description='sample')
        go = w.Button(description=f'Load events ({", ".join(banks)})', layout={'width': 'auto'})
        evout = w.Output()

        def show_events(_=None):
            evout.clear_output(wait=True)
            with evout:
                det, tof, lam = event_figures(load_events(path, src.value, smp.value), threed=True)
                display(w.VBox([det, w.HBox([tof, lam])]))
        go.on_click(show_events)
        rows += [w.HTML('<b>Event data</b> (McStasToX)'), w.HBox([src, smp, go]), evout]
    return w.VBox(rows)


def launch(call):
    ''' write a one-cell notebook running `call` next to this tool's import, open it in JupyterLab '''
    src = f"%matplotlib widget\nimport sys; sys.path.insert(0, {os.path.dirname(os.path.abspath(__file__))!r})\n{call}"
    nb = {'nbformat': 4, 'nbformat_minor': 4, 'metadata': {'kernelspec': {'name': 'python3', 'display_name': 'Python 3', 'language': 'python'}},
          'cells': [{'cell_type': 'code', 'metadata': {}, 'execution_count': None, 'outputs': [], 'source': src}]}
    name = os.path.splitext(os.path.basename(__file__))[0] + '.ipynb'
    with open(name, 'w') as f:
        json.dump(nb, f)
    print(f'wrote {name}; run its cell in JupyterLab')
    subprocess.run([sys.executable, '-m', 'jupyter', 'lab', name])


def figures(das, log=False):
    ''' static figures; 3D scan monitors become one tile per step '''
    norm = 'log' if log else 'linear'
    figs = []
    for name, da in das.items():
        if da.ndim == 3:
            scan_dim = da.dims[0]
            figs += [pp.plot(da[scan_dim, j], norm=norm, title=f'{name} {scan_dim}={da.coords[scan_dim][j].value:g}')
                     for j in range(da.sizes[scan_dim])]
        else:
            figs.append(pp.plot(da, norm=norm, title=name))
    return figs


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('simfile', nargs='?', default='.', help='output dir, mccode.sim, a monitor file, or mccode.h5 (NeXus)')
    p.add_argument('-o', '--output', help='write a static png/pdf/svg instead of opening the UI')
    p.add_argument('--log', action='store_true', help='log intensity scale')
    p.add_argument('--source', help='event data (NeXus, McStasToX): name of the source component')
    p.add_argument('--sample', help='event data (NeXus, McStasToX): name of the sample component')
    args = p.parse_args()
    if not args.output:
        return launch(f'import mcplopp; mcplopp.ui({os.path.abspath(args.simfile)!r})')

    figs = figures(load(args.simfile), args.log)
    if args.source and args.sample:
        figs += event_figures(load_events(args.simfile, args.source, args.sample))
    ncols = math.ceil(math.sqrt(len(figs)))
    nrows = math.ceil(len(figs) / ncols)
    tiled = pp.tiled(nrows, ncols, figsize=(4 * ncols, 3 * nrows))
    for k, f in enumerate(figs):
        tiled[k // ncols, k % ncols] = f
    tiled.save(args.output)
    print(f'saved {args.output}')


if __name__ == '__main__':
    main()
