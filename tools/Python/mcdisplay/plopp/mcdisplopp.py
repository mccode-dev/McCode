#!/usr/bin/env python3
'''
mcdisplopp: draw a McStas/McXtrace instrument and traced rays with plopp (scipp).

UI:      mcdisplopp.py <instr> [-n 100] [param=value ...]   (opens JupyterLab, 3D)
Static:  mcdisplopp.py <instr> [-n 100] [-y] [param=value ...] -o out.png   (2D projections)
'''
import argparse
import json
import os
import subprocess
import sys
import warnings

import numpy as np
try:
    import scipp as sc
    import plopp as pp
except ImportError as e:
    sys.exit(f'mcdisplay-plopp: {e}. scipp and plopp are required for this tool to function '
             '(plus ipympl and jupyterlab for the interactive UI), e.g.:\n'
             '  conda install -c conda-forge plopp ipympl jupyterlab\n'
             '  python3 -m pip install plopp ipympl jupyterlab')

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))
from mccodelib.mcdisplayutils import McDisplayReader
from mccodelib.instrgeom import (Vector3d, DrawLine, DrawMultiline, DrawCircle, DrawNewCircle, DrawSphere,
                                  DrawCone, DrawCylinder, DrawDisc, DrawAnnulus, DrawBox, DrawPolyhedron)

warnings.filterwarnings('ignore', 'The input contains a coordinate with unsorted', RuntimeWarning)  # polylines, not histograms
BREAK = [(np.nan,) * 3]  # NaN between polylines lifts the pen, so one line per component
K = 24  # segments around circles and solids of revolution
HOLLOW_OPACITY = 0.4  # 3D view: components with a hollow box/cylinder, so the closed inner cavity shows


def _xyz(v):
    return v.x, v.y, v.z


def _polylines(pls):
    return np.array([p for pl in pls for p in list(pl) + BREAK], dtype=float).reshape(-1, 3)


def _frame(n):
    ''' unit axis n plus two unit vectors u, v spanning the plane normal to it '''
    n = np.asarray(n, float) / np.linalg.norm(n)
    u = np.cross(n, [1., 0, 0] if abs(n[0]) < 0.9 else [0., 1, 0])
    u /= np.linalg.norm(u)
    return n, u, np.cross(n, u)


def _loft(rings, closed=False):
    ''' (m, k, 3) stack of k-point rings -> (vertices, triangles) joining ring i to i+1 (and last to first) '''
    m, k = rings.shape[:2]
    i, j = np.arange(m if closed else m - 1)[:, None], np.arange(k)[None]
    a, b = i * k + j, i * k + (j + 1) % k
    c, d = (i + 1) % m * k + j, (i + 1) % m * k + (j + 1) % k
    return rings.reshape(-1, 3), np.stack([np.stack([a, b, d], -1), np.stack([a, d, c], -1)], 2).reshape(-1, 3)


def _revolve(c, n, rr, hh, closed=False):
    ''' solid of revolution about axis n through c: profile points (radius rr, height hh along n) '''
    n, u, v = _frame(n)
    th = 2 * np.pi * np.arange(K) / K
    ring = np.cos(th)[:, None] * u + np.sin(th)[:, None] * v
    rings = np.asarray(c, float) + np.asarray(hh, float)[:, None, None] * n + np.asarray(rr, float)[:, None, None] * ring
    return _loft(rings, closed)


def _rot_y_to(n):
    ''' rotation taking +y onto n (as three.js setFromUnitVectors, used by mcdisplay-webgl for boxes) '''
    n = np.asarray(n, float) / np.linalg.norm(n)
    w, c = np.cross([0., 1, 0], n), n[1]
    if c < -1 + 1e-9:
        return np.diag([1., -1, -1])
    W = np.array([[0, -w[2], w[1]], [w[2], 0, -w[0]], [-w[1], w[0], 0]])
    return np.eye(3) + W + W @ W / (1 + c)


def _box(x, y, z, w, h, l, t, nx, ny, nz):
    ''' box of size (w, h, l) about (x, y, z), local y turned onto n; t > 0: closed inner cavity (w-2t, h-2t, l-2t),
        as in e.g. Incoherent '''
    rect = lambda hx, hz, yy: [(-hx, yy, -hz), (hx, yy, -hz), (hx, yy, hz), (-hx, yy, hz)]
    solid = lambda hx, hy, hz: _loft(np.array([[(0, -hy, 0)] * 4, rect(hx, hz, -hy), rect(hx, hz, hy), [(0, hy, 0)] * 4]))
    parts = [solid(w / 2, h / 2, l / 2)]
    if t > 0 and min(w, h, l) > 2 * t:
        parts.append(solid(w / 2 - t, h / 2 - t, l / 2 - t))
    v, f = _merge(parts)
    return v @ _rot_y_to((nx, ny, nz)).T + (x, y, z), f


def _polyhedron(js):
    ''' OFF/polygon JSON from the runtime; polygon faces fanned into triangles '''
    d = json.loads(js)
    tri = [(f[0], f[i], f[i + 1]) for face in d['faces'] for f in [face['face']] for i in range(1, len(f) - 1)]
    return np.array(d['vertices'], float).reshape(-1, 3), np.array(tri, int).reshape(-1, 3)


def _wire(d):
    ''' polyline (local coords) for the line-like draw calls, else None '''
    if isinstance(d, DrawLine):
        return [_xyz(d.point_1), _xyz(d.point_2)]
    if isinstance(d, DrawMultiline):
        return [_xyz(p) for p in d.points]
    if isinstance(d, DrawCircle):
        return [_xyz(p) for p in d.get_points_on_circle(K + 1)]
    if isinstance(d, DrawNewCircle):
        x, y, z, r, *n = d.args
        n, u, v = _frame(n)
        th = 2 * np.pi * np.arange(K + 1) / K
        return (np.array([x, y, z]) + r * (np.cos(th)[:, None] * u + np.sin(th)[:, None] * v)).tolist()


def _solid(d):
    ''' (vertices, triangles) in local coords for --trace=2 solids, else None. Centred shapes along n, as
        mcdisplay-webgl (the annulus "inner_radius" is a ring width there and in mcdisplay-matplotlib) '''
    if isinstance(d, DrawPolyhedron):
        return _polyhedron(d.faces_vertices)
    a = d.args
    if isinstance(d, DrawSphere):
        x, y, z, r = a
        phi = np.linspace(0, np.pi, K // 2 + 1)
        return _revolve((x, y, z), (0, 1, 0), r * np.sin(phi), -r * np.cos(phi))
    if isinstance(d, DrawCone):
        x, y, z, r, h, *n = a
        return _revolve((x, y, z), n, [0, r, 0], [-h / 2, -h / 2, h / 2])
    if isinstance(d, DrawCylinder):
        x, y, z, r, h, t, *n = a
        can = lambda r, h: _revolve((x, y, z), n, [0, r, r, 0], [-h / 2, -h / 2, h / 2, h / 2])
        if t > 0 and r > t and h > 2 * t:  # closed inner cavity, as in e.g. Incoherent
            return _merge([can(r, h), can(r - t, h - 2 * t)])
        return can(r, h)
    if isinstance(d, DrawDisc):
        x, y, z, r, *n = a
        return _revolve((x, y, z), n, [0, r], [0, 0])
    if isinstance(d, DrawAnnulus):
        x, y, z, r, w, *n = a
        return _revolve((x, y, z), n, [r - w, r], [0, 0])
    if isinstance(d, DrawBox):
        return _box(*a[:10])


def _merge(parts):
    ''' [(vertices, triangles), ...] -> one (vertices, triangles) '''
    parts = [p for p in parts if len(p[0])] or [(np.zeros((0, 3)), np.zeros((0, 3), int))]
    off = np.cumsum([0] + [len(v) for v, f in parts[:-1]])
    return np.concatenate([v for v, f in parts]), np.concatenate([f + o for (v, f), o in zip(parts, off)])


def trace(instr, n=100, params=(), yes=False):
    ''' run mcrun --trace=2; yes forwards -y (unset params take defaults), otherwise mcrun prompts for them.
        Returns ({component: (lines, solid vertices, solid triangles, hollow)}, ray xyz); lines/rays are NaN-separated
        (N,3) polylines, all in instrument coordinates; hollow: the component draws a box/cylinder with thickness > 0 '''
    reader = McDisplayReader(instr=instr, n=n, options=list(params) + ['-y'] * yes, no_mpi=True, trace=2)
    instrument = reader.read_instrument()
    bundle = reader.read_particles()

    geom = {}
    for c in instrument.components:
        t = c.transform
        M = np.array([[t.a11, t.a12, t.a13], [t.a21, t.a22, t.a23], [t.a31, t.a32, t.a33]])
        t0 = np.array([t.a14, t.a24, t.a34])
        wires = [w for w in map(_wire, c.drawcalls) if w]
        solids = [s for s in map(_solid, c.drawcalls) if s is not None]
        if wires or solids:
            v, f = _merge(solids)
            hollow = any(isinstance(d, (DrawBox, DrawCylinder)) and d.thickness > 0 for d in c.drawcalls)
            geom[c.name] = (_polylines(wires) @ M.T + t0, v @ M.T + t0, f, hollow)  # ponytail: DrawPolygon (unused at trace=2) skipped

    tf = {c.name: c.transform for c in instrument.components}
    ray_pls = [[_xyz(tf[g.compname].apply(Vector3d(*e.args[:3]))) for g in story.groups if g.compname in tf
                for e in g.events] for story in (bundle.rays if bundle else [])]
    return geom, _polylines(ray_pls)


def _edges(v, f):
    ''' triangle outlines as NaN-separated polylines, for the 2D view '''
    tri = v[f]
    return np.concatenate([tri, tri[:, :1], np.full((len(f), 1, 3), np.nan)], 1).reshape(-1, 3)


def _projection(xyz, h, v):
    ''' 1D DataArray: values = vertical axis, coord = horizontal axis (McStas: z beam, y up) '''
    ax = 'xyz'
    return sc.DataArray(sc.array(dims=[h], values=xyz[:, ax.index(v)], unit='m'),
                        coords={h: sc.array(dims=[h], values=xyz[:, ax.index(h)], unit='m')})


def view(geom, rays):
    ''' top (z-x) above side (z-y) projection; one colour per component (solids as triangle outlines), rays in grey '''
    flat = {name: np.concatenate([lines, _edges(v, f)]) for name, (lines, v, f, _) in geom.items()}
    z = np.concatenate([rays[:, 2]] + [xyz[:, 2] for xyz in flat.values()])
    pad = 0.02 * (np.nanmax(z) - np.nanmin(z))  # plopp's autoscale overshoots on the NaN separators
    figs = []
    for v in 'xy':
        lines = {'rays': _projection(rays, 'z', v)}  # first, so components draw on top
        lines.update({name: _projection(xyz, 'z', v) for name, xyz in flat.items()})
        figs.append(pp.plot(lines, linestyle='-', marker='', legend=False,
                            color={'rays': 'lightgrey'}, ylabel=f'{v} [m]', title=f'z-{v}',
                            xmin=np.nanmin(z) - pad, xmax=np.nanmax(z) + pad))
    return figs[0] / figs[1]


_G = (1 + 5 ** 0.5) / 2  # icosahedron: the sphere at each polyline vertex
ICO_V = np.array([(-1, _G, 0), (1, _G, 0), (-1, -_G, 0), (1, -_G, 0), (0, -1, _G), (0, 1, _G),
                  (0, -1, -_G), (0, 1, -_G), (_G, 0, -1), (_G, 0, 1), (-_G, 0, -1), (-_G, 0, 1)]) / np.hypot(1, _G)
ICO_F = np.array([(0, 11, 5), (0, 5, 1), (0, 1, 7), (0, 7, 10), (0, 10, 11), (1, 5, 9), (5, 11, 4),
                  (11, 10, 2), (10, 7, 6), (7, 1, 8), (3, 9, 4), (3, 4, 2), (3, 2, 6), (3, 6, 8),
                  (3, 8, 9), (4, 9, 5), (2, 4, 11), (6, 2, 10), (8, 6, 7), (9, 8, 1)])


def _tubes(xyz, r, k=6):
    ''' NaN-separated polylines -> (vertices, faces): a k-sided open cylinder per segment, a sphere per corner '''
    a, b = xyz[:-1], xyz[1:]
    keep = np.isfinite(a).all(1) & np.isfinite(b).all(1) & (np.linalg.norm(b - a, axis=1) > 0)
    a, b = a[keep], b[keep]
    d = (b - a) / np.linalg.norm(b - a, axis=1, keepdims=True)
    u = np.cross(d, np.where(abs(d[:, :1]) < 0.9, [1., 0, 0], [0., 1, 0]))  # helper axis not parallel to d
    u /= np.linalg.norm(u, axis=1, keepdims=True)
    v = np.cross(d, u)
    th = 2 * np.pi * np.arange(k) / k
    ring = r * (np.cos(th)[None, :, None] * u[:, None] + np.sin(th)[None, :, None] * v[:, None])  # (n, k, 3)
    cyl = np.concatenate([a[:, None] + ring, b[:, None] + ring], axis=1).reshape(-1, 3)
    j, j1 = np.arange(k), (np.arange(k) + 1) % k
    side = np.concatenate([np.stack([j, j1, k + j], 1), np.stack([j1, k + j1, k + j], 1)])
    cyl_f = (side[None] + 2 * k * np.arange(len(a))[:, None, None]).reshape(-1, 3)

    d1, d2 = xyz[1:-1] - xyz[:-2], xyz[2:] - xyz[1:-1]  # joints only where the polyline turns > ~20 deg;
    with np.errstate(invalid='ignore', divide='ignore'):  # NaN breaks, ends and smooth arcs get none
        cos = (d1 * d2).sum(1) / (np.linalg.norm(d1, axis=1) * np.linalg.norm(d2, axis=1))
    pts = xyz[1:-1][cos < 0.94]
    sph = (pts[:, None] + r * ICO_V[None]).reshape(-1, 3)
    sph_f = (ICO_F[None] + len(cyl) + 12 * np.arange(len(pts))[:, None, None]).reshape(-1, 3)
    return np.concatenate([cyl, sph]), np.concatenate([cyl_f, sph_f])


def _mesh(parts, cols):
    ''' plopp graph node holding one mesh (vertices, faces, per-vertex colour index), as plopp.mesh3d builds it '''
    verts, faces = _merge(parts)
    v = sc.vectors(dims=['vertices'], values=verts, unit='m')
    return pp.Node(sc.DataArray(sc.array(dims=['vertices'], values=np.concatenate(cols) if cols else np.zeros(0)),
                                coords={'x': v.fields.x, 'y': v.fields.y, 'z': v.fields.z, 'vertices': v,
                                        'faces': sc.scalar(sc.array(dims=['faces', 'vertex'], values=faces))}))


def view3d(geom, rays, thickness=1e-4):
    ''' 3D (Jupyter/pythreejs): solids as meshes, lines as tubes, coloured per component; rays as thinner grey tubes.
        Tube radius = thickness x the instrument's bounding-box diagonal; rays get half of it.
        Components with hollow shapes go in a second, translucent mesh so their inner cavity shows '''
    from plopp.graphics import mesh3dfigure
    allpts = np.concatenate([rays] + [np.concatenate([lines, v]) for lines, v, f, _ in geom.values()])
    r = thickness * np.linalg.norm(np.nanmax(allpts, 0) - np.nanmin(allpts, 0))
    parts, cols = {False: [], True: []}, {False: [], True: []}
    for i, (lines, v, f, hollow) in enumerate(geom.values()):
        for part in (_tubes(lines, r), (v, f)):
            parts[hollow].append(part), cols[hollow].append(np.full(len(part[0]), i % 20, dtype=float))
    parts[False].append(_tubes(rays, r / 2)), cols[False].append(np.full(len(parts[False][-1][0]), np.nan))
    nodes = {h: _mesh(parts[h], cols[h]) for h in (False, True) if parts[h]}
    fig = mesh3dfigure(*nodes.values(), vertexcolors=True, cmap='tab20', cmin=0, cmax=19, nan_color='lightgrey',
                       figsize=(900, 600))
    if True in nodes:
        fig.view.artists[nodes[True].id].opacity = HOLLOW_OPACITY
    return fig


def ui(instr, n=100, params=''):
    ''' Jupyter: 3D view; edit params / ray count, re-trace on click '''
    import ipywidgets as w
    from IPython.display import display
    pars = w.Text(value=params, description='params', placeholder='lambda=2.36 ...', layout={'width': '40em'})
    rays = w.IntText(value=n, description='rays')
    go = w.Button(description='Trace', button_style='primary')
    thick = w.FloatLogSlider(value=1e-4, base=10, min=-6, max=-3, description='thickness', readout_format='.1e')
    out = w.Output()
    last = {}

    def draw(_=None):  # redraws the last trace, no mcrun
        out.clear_output(wait=True)
        with out:
            display(view3d(*last['trace'], thick.value))

    def run(_=None):
        go.disabled = True
        try:
            with out:
                last['trace'] = trace(instr, rays.value, pars.value.split(), yes=True)  # a widget can't answer prompts
            draw()
        finally:
            go.disabled = False
    go.on_click(run)
    thick.observe(draw, 'value')
    run()
    return w.VBox([w.HBox([pars, rays, go, thick]), out])


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


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('instr', help='.instr or compiled instrument')
    p.add_argument('params', nargs='*', help='instrument parameters, name=value')
    p.add_argument('-n', type=int, default=100, help='number of rays to trace')
    p.add_argument('-o', '--output', help='write a static png/pdf/svg instead of opening the UI')
    p.add_argument('-y', '--yes', action='store_true', help='forwarded to mcrun: use defaults for unset parameters '
                   '(always on in the UI)')
    args = p.parse_intermixed_args()
    if not args.output:
        return launch(f'import mcdisplopp; mcdisplopp.ui({os.path.abspath(args.instr)!r}, {args.n}, {" ".join(args.params)!r})')

    view(*trace(args.instr, args.n, args.params, args.yes)).save(args.output)
    print(f'saved {args.output}')


if __name__ == '__main__':
    main()
