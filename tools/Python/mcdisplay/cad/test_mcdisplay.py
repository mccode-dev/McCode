''' geometry self-check of the mcdisplay-cad tessellation; run: python3 test_mcdisplay.py (needs mccodelib on PYTHONPATH) '''
import json
from collections import Counter
import numpy as np
from mccodelib.instrgeom import DrawSphere, DrawCylinder, DrawCone, DrawBox, DrawDisc, DrawAnnulus, DrawPolyhedron
from mcdisplay import tessellate, _tubes


def closed(m):  # welded by position, every edge shared by exactly two triangles (degenerate pole/cap triangles dropped)
    f = np.unique(np.round(m.verts, 9), axis=0, return_inverse=True)[1].reshape(-1)[m.tris]
    f = f[(f[:, 0] != f[:, 1]) & (f[:, 1] != f[:, 2]) & (f[:, 0] != f[:, 2])]
    return set(Counter(tuple(sorted(e)) for t in f for e in ((t[0], t[1]), (t[1], t[2]), (t[2], t[0]))).values()) == {2}


m, _ = tessellate(DrawSphere([1, 2, 3, 0.5]), 16)
assert np.allclose(np.linalg.norm(m.verts - (1, 2, 3), axis=1), 0.5) and closed(m)

a = np.array([1, 2, 3]) / np.sqrt(14)
m, _ = tessellate(DrawCylinder([1, 2, 3, 0.2, 1.0, 0, *a]), 16)  # tilted axis, as in Text.comp strokes
off = m.verts - (1, 2, 3)
h = off @ a
rad = np.linalg.norm(off - np.outer(h, a), axis=1)
assert np.allclose(np.abs(h), 0.5) and (np.isclose(rad, 0.2) | np.isclose(rad, 0)).all() and closed(m)

m, _ = tessellate(DrawCone([0, 0, 0, 0.3, 1.0, 0, 0, 1]), 16)
assert np.isclose(m.verts[:, 2].max(), 1.0) and np.allclose(np.linalg.norm(m.verts[:, :2], axis=1).max(), 0.3)

m, _ = tessellate(DrawBox([1, 2, 3, 0.2, 0.4, 0.6, 0, 0, 1, 0]), 16)
assert np.allclose(m.verts.min(0), (0.9, 1.8, 2.7)) and np.allclose(m.verts.max(0), (1.1, 2.2, 3.3)) and closed(m)

m, _ = tessellate(DrawDisc([0, 0, 1, 0.5, 0, 0, 1]), 16)
assert np.allclose(m.verts[:, 2], 1) and np.isclose(np.linalg.norm(m.verts[:, :2], axis=1).max(), 0.5)

m, _ = tessellate(DrawAnnulus([0, 0, 0, 0.06, 0.02, 0, 1, 0]), 16)  # 5th arg is the ring width, as in Annulus.comp
assert np.allclose(m.verts[:, 1], 0) and set(np.round(np.linalg.norm(m.verts[:, [0, 2]], axis=1), 9)) == {0.04, 0.06}

js = json.dumps({'vertices': [[0, 0, 0], [1, 0, 0], [0, 1, 0], [0, 0, 1]],
                 'faces': [{'face': [0, 2, 1]}, {'face': [0, 1, 3]}, {'face': [0, 3, 2]}, {'face': [1, 2, 3]}]})
m, _ = tessellate(DrawPolyhedron([js]), 16)  # tetrahedron, as emitted at --trace=2
assert len(m.tris) == 4 and closed(m)

m = _tubes(np.array([(0, 0, 0), (1, 0, 0), (1, 1, 0.)]), r=0.1)  # 2 segments + 1 corner sphere
assert len(m.tris) == 2 * 12 + 20 and m.tris.max() == len(m.verts) - 1
import os, tempfile
from mcdisplay import Part, export_off, export_ply
p = Part('t', (1, 0, 0))
p.add_mesh(m)
p.add_polyline(np.array([(0, 0, 0), (1, 0, 0.)]))  # lines must be dropped
for exp, ext in ((export_off, '.off'), (export_ply, '.ply')):
    fn = os.path.join(tempfile.mkdtemp(), 'x' + ext)
    exp([p], fn)
    lines = open(fn).read().splitlines()
    faces = lines[-len(m.tris):]
    # interoff-lib reads faces right after the vertices; extra face numbers would be supermirror parameters
    assert all(l.split() == ['3'] + [str(i) for i in t] for l, t in zip(faces, m.tris))
    assert 'edge' not in ' '.join(lines[:20])

# VTK: polygon count right also when vertex 3 is used, colors per point, RGBA with transparency
from mcdisplay import export_vtk, Mesh
q = Part('q', (1, 0, 0), 0.5)
q.add_mesh(Mesh([(0, 0, 0), (1, 0, 0), (1, 1, 0), (0, 1, 0)], [[0, 1, 2], [0, 2, 3]]))
fn = os.path.join(tempfile.mkdtemp(), 'x.vtk')
export_vtk([q], fn)
lines = open(fn).read().splitlines()
assert 'POLYGONS 2 8' in lines and 'COLOR_SCALARS component_color 4' in lines and lines[-1] == '1 0 0 0.5'

# --simdir data: flat 2D map (hot bin at +x, -y), flat 1D curve mirrored, and turned upright on an upside-down monitor
from types import SimpleNamespace
from mcdisplay import monitor_to_parts
from mccodelib.instrgeom import DrawMultiline
frame = DrawMultiline([-1, -1, 0, 1, -1, 0, 1, 1, 0, -1, 1, 0, -1, -1, 0])
up, turned = np.eye(4).ravel(), np.diag([-1., -1, 1, 1]).ravel()  # turned: ROTATED (0, 0, 180)
comp = SimpleNamespace(name='m', drawcalls=[frame], m4=up)
z = np.zeros((4, 4))
z[0, 3] = 1
hot = monitor_to_parts(comp, {'z': z, 'limits': [-1, 1, -1, 1], 'xlabel': 'X', 'ylabel': 'Y'}, transparency=0.5)[-1]
assert np.allclose(hot.mesh.verts.min(0), (0.5, -1, 0)) and np.allclose(hot.mesh.verts.max(0), (1, -0.5, 0)) and hot.transparency == 0.5
mon = {'y': [0, 0, 0, 1], 'limits': [0, 1], 'xlabel': 'L'}
for m4 in (up, turned):
    comp.m4 = m4
    peak = monitor_to_parts(comp, mon)[0].polylines[0][3]
    assert np.allclose(peak, (-0.75, 2 * 228 / 256 - 1, 0))
assert monitor_to_parts(comp, mon, show=['2D']) == []
# banana: the hot bin at 75 deg
comp = SimpleNamespace(name='b', drawcalls=[DrawCylinder([0, 0, 0, 2, 1, 0, 0, 1, 0])], m4=up)
mon = {'y': [0, 1], 'limits': [-60, 120], 'xlabel': 'Longitude [deg]'}
x, _, zz = monitor_to_parts(comp, mon)[0].polylines[0][1]
assert np.isclose(np.degrees(np.arctan2(x, zz)), 75) and np.isclose(np.hypot(x, zz), 2)
assert monitor_to_parts(comp, mon, show=['1D']) == []
print('ok')
