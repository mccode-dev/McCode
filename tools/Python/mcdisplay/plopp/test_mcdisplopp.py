import numpy as np
from mcdisplopp import _polylines, _projection

xyz = _polylines([[(0, 0, 0), (1, 2, 3)], [(4, 5, 6)]])
assert xyz.shape == (5, 3) and np.isnan(xyz[2]).all() and np.isnan(xyz[4]).all()
assert _polylines([]).shape == (0, 3)  # -n 0: no rays
da = _projection(xyz, 'z', 'x')  # z horizontal, x vertical
assert da.dims == ('z',) and da.coords['z'].values[1] == 3 and da.values[1] == 1
print('ok')

from collections import Counter
from mcdisplopp import _tubes, ICO_F, ICO_V
edges = Counter(tuple(sorted(e)) for f in ICO_F for e in ((f[0], f[1]), (f[1], f[2]), (f[2], f[0])))
assert len(edges) == 30 and set(edges.values()) == {2}  # closed icosahedron
assert np.allclose(np.linalg.norm(ICO_V, axis=1), 1)

v, f = _tubes(xyz, r=0.1, k=6)  # 1 valid segment (others hit NaN), no corners
assert v.shape == (2 * 6, 3) and f.shape == (2 * 6, 3) and f.max() == len(v) - 1
vL, fL = _tubes(np.array([(0, 0, 0), (1, 0, 0), (1, 1, 0), (1, 2, 0.)]), r=0.1, k=6)  # one 90 deg corner, one straight joint
assert vL.shape == (3 * 12 + 1 * 12, 3) and fL.shape == (3 * 12 + 1 * 20, 3) and np.allclose(vL[36:].mean(0), (1, 0, 0))
a, b = np.array([0, 0, 0]), np.array([1, 2, 3])
d = (b - a) / np.linalg.norm(b - a)
off = v[:12] - a
assert np.allclose(np.linalg.norm(off - np.outer(off @ d, d), axis=1), 0.1)  # ring radius about the axis
print('ok tubes')

from mcdisplopp import _box, _revolve, _polyhedron, _rot_y_to, _edges, K
def closed(v, f):  # welded by position, every edge shared by exactly two triangles (degenerate cap triangles dropped)
    f = np.unique(np.round(v, 9), axis=0, return_inverse=True)[1].reshape(-1)[f]
    f = f[(f[:, 0] != f[:, 1]) & (f[:, 1] != f[:, 2]) & (f[:, 0] != f[:, 2])]
    return set(Counter(tuple(sorted(e)) for t in f for e in ((t[0], t[1]), (t[1], t[2]), (t[2], t[0]))).values()) == {2}

v, f = _box(1, 2, 3, 0.2, 0.4, 0.6, 0, 0, 1, 0)  # solid, unrotated: extents w, h, l about the centre
assert np.allclose(v.min(0), (0.9, 1.8, 2.7)) and np.allclose(v.max(0), (1.1, 2.2, 3.3))
v, f = _box(0, 0, 0, 0.2, 0.4, 0.6, 0.05, 0, 0, 1)  # hollow, local y (height 0.4) turned onto z
assert np.allclose(v.max(0), (0.1, 0.3, 0.2)) and len(v) == 16
assert np.allclose(_rot_y_to((0, 0, 1)) @ (0, 1, 0), (0, 0, 1)) and np.allclose(_rot_y_to((0, -1, 0)) @ (0, 1, 0), (0, -1, 0))

v, f = _revolve((0, 0, 0), (1, 0, 0), [0, 0.5, 0.5, 0], [-1, -1, 1, 1])  # closed cylinder along x
assert np.allclose(np.abs(v).max(0), (1, 0.5, 0.5), atol=0.01) and closed(v, f)
v, f = _revolve((0, 0, 0), (0, 0, 1), [0.4, 0.5, 0.5, 0.4], [-1, -1, 1, 1], closed=True)  # hollow cylinder
assert np.allclose(sorted(set(np.round(np.hypot(v[:, 0], v[:, 1]), 6))), (0.4, 0.5)) and closed(v, f)
v, f = _revolve((0, 0, 0), (0, 1, 0), [0, 0.5, 0], [-1, -1, 1])  # cone: base at -h/2, apex at +h/2 (three.js)
assert np.isclose(v[:, 1].max(), 1) and np.isclose(v[:, 1][np.hypot(v[:, 0], v[:, 2]) > 0.49].max(), -1) and closed(v, f)

v, f = _polyhedron('{"vertices": [[0,0,0],[1,0,0],[1,1,0],[0,1,0],[0,0,1]], "faces": [{"face": [0,1,2,3]}, {"face": [0,1,4]}]}')
assert f.tolist() == [[0, 1, 2], [0, 2, 3], [0, 1, 4]]
assert _edges(v, f).shape == (3 * 5, 3)
print('ok solids')
