#!/usr/bin/env python3
# -*- coding: utf-8 -*-
'''
Converts MCDISPLAY geometry information, read from an instrument binary,
to common CAD / 3D formats.

Supported output formats: glTF (.gltf), GLB, OBJ (+MTL), STL, PLY, OFF, VRML97 (.wrl),
X3D (also as legacy 'xml'), VTK legacy (.vtk), STEP (AP214 faceted BREP, pure
Python), and VTKJS (.vtkjs zip of the vtk.js JSON scene format).

Each instrument component is assigned a color from a cycling palette, so
adjacent components are visually distinguishable in the output.
Color-capable formats: glTF, GLB, OBJ (via companion .mtl), PLY (vertex colors),
VRML97, X3D. STL, OFF and legacy VTK carry no color information.
STL, PLY and OFF hold surfaces only (no lines); PLY and OFF can be read back by
the McStas OFF components (interoff-lib).

Only dependency: numpy (already required by mccodelib.instrgeom).

All primitives are tessellated in pure Python/numpy:
  - lines / dashed lines / multilines  -> polylines (wireframe)
  - circles / new circles              -> polylines
  - rectangles                         -> quad (mesh + outline)
  - boxes                              -> 12-triangle cube + edges
  - spheres, cones, cylinders, discs, annuli -> triangulated meshes
  - polyhedron                         -> triangulated faces (fan)
Every draw call is placed in global coordinates via the component m4 matrix.
'''
__author__ = "Gregory Tucker (CadQuery version); Emmanuel Farhi (drop CadQuery depends)"
__date__ = "2026-10-17"

import sys
import json
import math
import struct
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parent.parent.parent))
from mccodelib.instrgeom import (Vector3d, DrawMagnify, DrawLine, DrawDashedLine,
                                  DrawMultiline, DrawRectangle, DrawBox, DrawCircle,
                                  DrawNewCircle, DrawSphere, DrawCone, DrawCylinder,
                                  DrawDisc, DrawAnnulus, DrawPolygon, DrawPolyhedron)
from mccodelib import mccode_config

import numpy as np


# ---------------------------------------------------------------------------
# Component color palette (cycled per component)
# ---------------------------------------------------------------------------

# RGB tuples in [0, 1]; chosen to be reasonably distinguishable in sequence.
COLOR_PALETTE = [
    (0.90, 0.10, 0.10),  # red
    (0.10, 0.55, 0.10),  # green
    (0.15, 0.35, 0.95),  # blue
    (1.00, 0.70, 0.00),  # orange
    (0.60, 0.20, 0.80),  # purple
    (0.00, 0.80, 0.80),  # cyan
    (1.00, 0.45, 0.70),  # pink
    (0.45, 0.35, 0.20),  # brown
    (0.50, 0.50, 0.50),  # gray
    (0.20, 0.90, 0.40),  # lime
]


def color_for(index):
    ''' cycling palette color for component number `index` '''
    return COLOR_PALETTE[index % len(COLOR_PALETTE)]


def _rgb01(c):
    return ' '.join(f'{max(0.0, min(1.0, x)):.4g}' for x in c)


def _rgb255(c):
    return ' '.join(str(int(round(max(0.0, min(1.0, x)) * 255))) for x in c)


# ---------------------------------------------------------------------------
# Small geometry containers
# ---------------------------------------------------------------------------

class Mesh:
    ''' triangle soup: verts (N,3) float64, tris (M,3) int '''
    def __init__(self, verts=None, tris=None):
        self.verts = np.asarray(verts, dtype=np.float64).reshape((-1, 3))
        self.tris = np.asarray(tris, dtype=np.int64).reshape((-1, 3))
        if self.tris.size and self.tris.max() >= len(self.verts):
            raise ValueError("Mesh: triangle index out of range")

    def empty(self):
        return len(self.tris) == 0

    def transformed(self, m4):
        return Mesh(transform_points(m4, self.verts), self.tris)

    def merge(self, other):
        if other is None or other.empty():
            return self
        if self.empty():
            return other
        tris = np.vstack((self.tris, other.tris + len(self.verts)))
        return Mesh(np.vstack((self.verts, other.verts)), tris)


class Part:
    ''' one named component: an optional surface mesh plus wireframe polylines '''
    def __init__(self, name, color=(0.8, 0.8, 0.8)):
        self.name = name
        self.color = color  # (r, g, b) in [0, 1]
        self.mesh = None
        self.polylines = []  # list of (N,3) arrays

    def add_mesh(self, mesh):
        self.mesh = mesh if self.mesh is None else self.mesh.merge(mesh)

    def add_polyline(self, pts):
        pts = np.asarray(pts, dtype=np.float64).reshape((-1, 3))
        if len(pts) >= 2:
            self.polylines.append(pts)

    def transformed(self, m4):
        p = Part(self.name, self.color)
        if self.mesh is not None:
            p.mesh = self.mesh.transformed(m4)
        p.polylines = [transform_points(m4, pl) for pl in self.polylines]
        return p

    def is_empty(self):
        return (self.mesh is None or self.mesh.empty()) and not self.polylines


def transform_points(m4, pts):
    ''' apply a row-major 4x4 (3x4 usable) matrix to an (N,3) point array '''
    pts = np.asarray(pts, dtype=np.float64).reshape((-1, 3))
    r = np.array(m4, dtype=np.float64).reshape(4, 4)[:3, :3]
    t = np.array(m4, dtype=np.float64).reshape(4, 4)[:3, 3]
    return pts @ r.T + t


# ---------------------------------------------------------------------------
# Tessellation of the mccodelib draw calls (local coordinates)
# ---------------------------------------------------------------------------

def _basis_from_axis(axis):
    ''' orthonormal (w, u, v) with w parallel to axis '''
    w = np.asarray(axis, dtype=np.float64).reshape(3)
    n = np.linalg.norm(w)
    if n < 1e-12:
        w = np.array([0.0, 0.0, 1.0])
    else:
        w = w / n
    a = np.array([1.0, 0.0, 0.0]) if abs(w[0]) < 0.9 else np.array([0.0, 1.0, 0.0])
    u = np.cross(w, a)
    u /= np.linalg.norm(u)
    v = np.cross(w, u)
    return w, u, v


def _circle_pts(center, radius, axis, segments=64, closed=True):
    ''' points on a circle of given center, radius and axis (default z) '''
    c = np.asarray(center, dtype=np.float64).reshape(3)
    if axis is not None:
        _, u, v = _basis_from_axis(axis)
    else:
        u, v = np.array([1.0, 0.0, 0.0]), np.array([0.0, 1.0, 0.0])
    theta = np.linspace(0.0, 2.0 * np.pi, segments, endpoint=not closed)
    pts = c + radius * (np.outer(np.cos(theta), u) + np.outer(np.sin(theta), v))
    if closed:  # close the loop
        pts = np.vstack((pts, pts[0]))
    return pts


def _fan(center, ring_pts):
    ''' triangulate a (closed, non-repeated) ring into a fan from center '''
    c = np.asarray(center, dtype=np.float64).reshape((1, 3))
    ring = np.asarray(ring_pts, dtype=np.float64).reshape((-1, 3))
    n = len(ring)
    verts = np.vstack((c, ring))
    tris = np.array([[0, 1 + i, 1 + (i + 1) % n] for i in range(n)], dtype=np.int64)
    return Mesh(verts, tris)


def _strip(ring_a, ring_b, flip=False):
    ''' triangulate the side wall between two same-length rings '''
    a = np.asarray(ring_a, dtype=np.float64).reshape((-1, 3))
    b = np.asarray(ring_b, dtype=np.float64).reshape((-1, 3))
    n = len(a)
    verts = np.vstack((a, b))
    off = n
    tris = []
    for i in range(n):
        j = (i + 1) % n
        if flip:
            tris.append([i, j, off + j])
            tris.append([i, off + j, off + i])
        else:
            tris.append([i, off + j, j])
            tris.append([i, off + i, off + j])
    return Mesh(verts, np.array(tris, dtype=np.int64))


def _rot_y_to(axis):
    ''' smallest rotation taking local y onto axis (half turn around z for -y), as in the webgl viewers and --trace=1 '''
    n = np.asarray(axis, dtype=np.float64)
    n = n / np.linalg.norm(n)
    if n[1] < -1 + 1e-12:
        return np.diag([-1.0, -1.0, 1.0])
    k = np.array([[0, n[0], 0], [-n[0], 0, -n[2]], [0, n[2], 0]])  # cross((0,1,0), n) as a matrix
    return np.eye(3) + k + k @ k / (1 + n[1])


def _box(c, rot, xw, yh, zl):
    ''' closed box mesh + face outlines, centred at c, local axes rotated by rot '''
    V = np.array([(sx*xw/2, sy*yh/2, sz*zl/2)
                  for sz in (-1, 1) for sy in (-1, 1) for sx in (-1, 1)]) @ rot.T + c
    # faces: bottom(z-), top(z+), sides
    F = [[0, 1, 3, 2], [4, 6, 7, 5], [0, 4, 5, 1], [2, 3, 7, 6], [0, 2, 6, 4], [1, 5, 7, 3]]
    tris, lines = [], []
    for f in F:
        tris += [[f[0], f[1], f[2]], [f[0], f[2], f[3]]]
        lines.append(np.vstack((V[f], V[f[0]])))
    return Mesh(V, tris), lines


def _v3(p):
    return (p.x, p.y, p.z)


def tessellate(d, segments=64):
    ''' convert one draw call into (mesh_or_None, [polylines]) in local coords '''
    if isinstance(d, DrawMagnify):
        return None, []
    if isinstance(d, DrawPolygon):
        return None, []  # awaiting C code upstream; nothing to draw
    if isinstance(d, (DrawLine,)):
        p1, p2 = _v3(d.point_1), _v3(d.point_2)
        return None, [np.array([p1, p2])]
    if isinstance(d, DrawDashedLine):
        p1, p2 = np.asarray(_v3(d.point_1)), np.asarray(_v3(d.point_2))
        n = max(2, int(np.linalg.norm(p2 - p1) / 0.05) + 1)  # 5 cm dashes
        t = np.linspace(0, 1, n)
        seg = [p1 + (p2 - p1) * ti for ti in t]
        lines = [np.array([seg[i], seg[i + 1]]) for i in range(0, len(seg) - 1, 2)]
        return None, lines
    if isinstance(d, DrawMultiline):
        return None, [np.array([_v3(p) for p in d.points])]
    if isinstance(d, DrawRectangle):
        plane = d.plane.lower()
        c = _v3(d.center)
        w, h = d.width / 2.0, d.height / 2.0
        if plane == 'xy':
            quad = [(c[0]-w, c[1]-h, c[2]), (c[0]+w, c[1]-h, c[2]),
                    (c[0]+w, c[1]+h, c[2]), (c[0]-w, c[1]+h, c[2])]
        elif plane == 'yz':
            quad = [(c[0], c[1]-w, c[2]-h), (c[0], c[1]-w, c[2]+h),
                    (c[0], c[1]+w, c[2]+h), (c[0], c[1]+w, c[2]-h)]
        else:  # xz
            quad = [(c[0]-w, c[1], c[2]-h), (c[0]+w, c[1], c[2]-h),
                    (c[0]+w, c[1], c[2]+h), (c[0]-w, c[1], c[2]+h)]
        quad = np.array(quad)
        closed = np.vstack((quad, quad[0]))
        mesh = Mesh(quad, [[0, 1, 2], [0, 2, 3]])
        return mesh, [closed]
    if isinstance(d, DrawBox):
        c = np.asarray(_v3(d.center))
        rot = _rot_y_to(_v3(d.axis_vector))
        mesh, lines = _box(c, rot, d.xwidth, d.yheight, d.zlength)
        t2 = 2 * d.thickness
        if d.thickness > 0 and d.xwidth > t2 and d.yheight > t2 and d.zlength > t2:
            # hollow: closed inner cavity, as in e.g. Incoherent
            inner, inlines = _box(c, rot, d.xwidth - t2, d.yheight - t2, d.zlength - t2)
            mesh, lines = mesh.merge(inner), lines + inlines
        return mesh, lines
    if isinstance(d, DrawCircle):
        axis = {'xy': (0, 0, 1), 'xz': (0, 1, 0), 'yz': (1, 0, 0),
                'yx': (0, 0, 1), 'zx': (0, 1, 0), 'zy': (1, 0, 0)}.get(d.plane.lower(), (0, 0, 1))
        return None, [_circle_pts(_v3(d.center), d.radius, axis, segments)]
    if isinstance(d, DrawNewCircle):
        return None, [_circle_pts(_v3(d.center), d.radius, _v3(d.axis_vector), segments)]
    if isinstance(d, DrawSphere):
        c = np.asarray(_v3(d.center))
        r = d.radius
        nu, nv = segments, max(8, segments // 2)
        u = np.linspace(0, 2 * np.pi, nu, endpoint=False)
        v = np.linspace(0, np.pi, nv)
        uu, vv = (g.ravel() for g in np.meshgrid(u, v))  # row-major: index i*nu + j
        pts = np.column_stack((c[0] + r * np.cos(uu) * np.sin(vv),
                               c[1] + r * np.sin(uu) * np.sin(vv),
                               c[2] + r * np.cos(vv)))
        tris = []
        for i in range(nv - 1):
            for j in range(nu):
                a = i * nu + j
                b = i * nu + (j + 1) % nu
                cc = (i + 1) * nu + j
                dd = (i + 1) * nu + (j + 1) % nu
                tris += [[a, b, cc], [b, dd, cc]]
        return Mesh(pts, tris), []
    if isinstance(d, DrawCone):
        c = np.asarray(_v3(d.center))
        w, u, v = _basis_from_axis(_v3(d.axis_vector))
        th = np.linspace(0, 2*np.pi, segments, endpoint=False)
        ring = c + d.radius * (np.outer(np.cos(th), u) + np.outer(np.sin(th), v))
        apex = (c + w * d.height).reshape((1, 3))
        mesh = _strip(ring, np.repeat(apex, len(ring), axis=0), flip=True)
        closed = np.vstack((ring, ring[0]))
        return mesh, [np.vstack((closed, apex[0], closed[0]))]
    if isinstance(d, DrawCylinder):
        c = np.asarray(_v3(d.center))
        w, u, v = _basis_from_axis(_v3(d.axis_vector))
        th = np.linspace(0, 2 * np.pi, segments, endpoint=False)

        def closed_cylinder(r, h):
            ring = c + r * (np.outer(np.cos(th), u) + np.outer(np.sin(th), v))
            bot = ring - w * (h / 2.0)
            top = ring + w * (h / 2.0)
            mesh = _strip(bot, top)
            mesh = mesh.merge(_fan(c - w * (h / 2.0), bot))
            mesh = mesh.merge(_fan(c + w * (h / 2.0), top))
            return mesh, [np.vstack((bot, bot[0])), np.vstack((top, top[0]))]

        mesh, lines = closed_cylinder(d.radius, d.height)
        t = d.thickness
        if t > 0 and d.radius > t and d.height > 2 * t:
            # hollow: closed inner cavity, as in e.g. Incoherent
            inner, inlines = closed_cylinder(d.radius - t, d.height - 2 * t)
            mesh, lines = mesh.merge(inner), lines + inlines
        return mesh, lines
    if isinstance(d, DrawDisc):
        th = np.linspace(0, 2 * np.pi, segments, endpoint=False)
        c = np.asarray(_v3(d.center))
        # build in the plane defined by axis_vector
        _, u, v = _basis_from_axis(_v3(d.axis_vector))
        ring = c + d.radius * (np.outer(np.cos(th), u) + np.outer(np.sin(th), v))
        mesh = _fan(c, ring)
        return mesh, [np.vstack((ring, ring[0]))]
    if isinstance(d, DrawAnnulus):
        c = np.asarray(_v3(d.center))
        _, u, v = _basis_from_axis(_v3(d.axis_vector))
        th = np.linspace(0, 2 * np.pi, segments, endpoint=False)
        outer = c + d.outer_radius * (np.outer(np.cos(th), u) + np.outer(np.sin(th), v))
        r_in = max(0.0, d.outer_radius - d.inner_radius)  # instrgeom's inner_radius holds the ring width (mcdis_annulus thickness)
        inner = c + r_in * (np.outer(np.cos(th), u) + np.outer(np.sin(th), v))
        mesh = _strip(outer, inner, flip=True)
        return mesh, [np.vstack((outer, outer[0])), np.vstack((inner, inner[0]))]
    if isinstance(d, DrawPolyhedron):
        return _polyhedron(d.faces_vertices)
    print(f"Drawing of {type(d)} not supported; please extend {__file__}!")
    return None, []


def _polyhedron(json_str):
    ''' polyhedron from the --trace=2 JSON: {"vertices": [[x,y,z],...], "faces": [{"face": [i,j,k,...]},...]} '''
    try:
        data = json.loads(json_str)
        V = np.asarray(data['vertices'], dtype=np.float64).reshape((-1, 3))
        mesh, lines = None, []
        for f in data['faces']:
            f = V[np.asarray(f['face'], dtype=np.int64)]
            if len(f) < 3:
                continue
            m = Mesh(f, [[0, i, i + 1] for i in range(1, len(f) - 1)])
            mesh = m if mesh is None else mesh.merge(m)
            lines.append(np.vstack((f, f[0])))
        return mesh, lines
    except Exception as e:
        print(f"Polyhedron interpretation failed ({e}); skipped")
        return None, []


# ---------------------------------------------------------------------------
# instrument -> parts
# ---------------------------------------------------------------------------

def component_to_part(comp, color, segments=64):
    part = Part(comp.name, color)
    for d in comp.drawcalls:
        mesh, lines = tessellate(d, segments)
        if mesh is not None:
            part.add_mesh(mesh)
        for l in lines:
            part.add_polyline(l)
    return part.transformed(comp.m4)


def rays_to_part(inst, bundle):
    ''' one grey part holding every particle ray as a polyline, in instrument coordinates '''
    m4 = {c.name: c.m4 for c in inst.components}
    part = Part('rays', (0.75, 0.75, 0.75))
    for story in bundle.rays:
        pts = [transform_points(m4[g.compname], [e.args[:3] for e in g.events])
               for g in story.groups if g.compname in m4 and g.events]
        if pts:
            part.add_polyline(np.vstack(pts))
    return part


def instrument_to_parts(inst, segments=64):
    ''' convert components to parts, assigning a cycling color per component '''
    parts = []
    ci = 0  # color index over all components (so colors differ even if some are empty)
    for comp in inst.components:
        part = component_to_part(comp, color_for(ci), segments)
        ci += 1
        if not part.is_empty():
            parts.append(part)
    return parts


# ---------------------------------------------------------------------------
# Exporters (all pure Python)
# ---------------------------------------------------------------------------

def _fmt(x):
    return f'{x:.6g}'


def export_obj(parts, path):
    ''' OBJ with a companion .mtl carrying one material (color) per component '''
    mtl_path = str(path)[:-len('.obj')] + '.mtl' if str(path).endswith('.obj') \
        else str(path) + '.mtl'

    def mtl_name(i, p):
        # material names must not contain whitespace or '#'
        return 'comp_' + ''.join(ch if ch.isalnum() else '_' for ch in p.name) + f'_{i}'

    with open(mtl_path, 'w') as mf:
        mf.write('# McCode mcdisplay export (MTL)\n')
        for i, p in enumerate(parts):
            r, g, b = p.color
            mf.write(f'newmtl {mtl_name(i, p)}\n')
            mf.write(f'Kd {r:.4g} {g:.4g} {b:.4g}\nKa 0 0 0\nKs 0.2 0.2 0.2\nNs 10\n\n')
    with open(path, 'w') as f:
        f.write('# McCode mcdisplay export (OBJ)\n')
        f.write(f'mtllib {Path(mtl_path).name}\n')
        # OBJ vertex indices are global across the whole file: one running offset
        voff = 1
        for i, p in enumerate(parts):
            f.write(f'o {p.name}\ng {p.name}\n')
            for pl in p.polylines:
                f.write(''.join(f'v {_fmt(x)} {_fmt(y)} {_fmt(z)}\n' for x, y, z in pl))
                f.write('l ' + ' '.join(str(voff + i2) for i2 in range(len(pl))) + '\n')
                voff += len(pl)
            if p.mesh is not None and not p.mesh.empty():
                f.write(f'usemtl {mtl_name(i, p)}\n')
                f.write(''.join(f'v {_fmt(x)} {_fmt(y)} {_fmt(z)}\n' for x, y, z in p.mesh.verts))
                for a, b, c in p.mesh.tris:
                    f.write(f'f {voff+a} {voff+b} {voff+c}\n')
                voff += len(p.mesh.verts)


def export_stl(parts, path):
    tris = []
    for p in parts:
        if p.mesh is not None and not p.mesh.empty():
            tris.append(p.mesh.verts[p.mesh.tris])
    if tris:
        tris = np.vstack(tris)
    else:
        tris = np.zeros((0, 3, 3))
    with open(path, 'wb') as f:
        f.write(b'mcdisplay STL export'.ljust(80, b'\0'))
        f.write(struct.pack('<I', len(tris)))
        for t in tris:
            n = np.cross(t[1] - t[0], t[2] - t[0])
            ln = np.linalg.norm(n)
            n = n / ln if ln > 0 else n
            f.write(struct.pack('<3f', *n))
            for v in t:
                f.write(struct.pack('<3f', *v))
            f.write(struct.pack('<H', 0))
    _warn_surfaces_only(parts, 'STL')


def _warn_surfaces_only(parts, fmt):
    ''' surface-only formats (STL, PLY, OFF) drop lines: say what was lost '''
    nmeshed = sum(1 for p in parts if p.mesh is not None and not p.mesh.empty())
    if not nmeshed:
        print(f"WARNING: {fmt} has no triangles (only lines were drawn); use -t 2 and/or --tubes")
    elif nmeshed < len(parts):
        print(f"NOTE: {len(parts) - nmeshed} of {len(parts)} components are wireframe-only "
              f"and cannot be represented in {fmt} (surfaces only: {nmeshed})")


def export_ply(parts, path):
    ''' ASCII PLY, surfaces only, per-vertex colors. Readable by McStas interoff-lib (OFF components):
    no edge element (interoff reads the faces right after the vertices) and no face colors
    (trailing numbers on a face line are per-face properties there, e.g. m/alpha/W in Guide_anyshape) '''
    verts, colors, faces = [], [], []
    for p in parts:
        if p.mesh is not None and not p.mesh.empty():
            faces.append(p.mesh.tris + len(verts))
            verts += list(p.mesh.verts)
            colors += [tuple(int(round(c * 255)) for c in p.color)] * len(p.mesh.verts)
    with open(path, 'w') as f:
        f.write('ply\nformat ascii 1.0\ncomment McCode mcdisplay export\n')
        f.write(f'element vertex {len(verts)}\nproperty float x\nproperty float y\nproperty float z\n'
                'property uchar red\nproperty uchar green\nproperty uchar blue\n')
        f.write(f'element face {sum(len(t) for t in faces)}\nproperty list uchar int vertex_indices\nend_header\n')
        for v, c in zip(verts, colors):
            f.write(f'{v[0]:.12g} {v[1]:.12g} {v[2]:.12g} {c[0]} {c[1]} {c[2]}\n')
        for t in faces:
            for a, b, c in t:
                f.write(f'3 {a} {b} {c}\n')
    _warn_surfaces_only(parts, 'PLY')


def export_off(parts, path):
    ''' plain OFF, surfaces only, all components merged; readable by McStas interoff-lib (OFF components).
    No colors: trailing numbers on a face line are per-face properties there (e.g. m/alpha/W in Guide_anyshape) '''
    m = Mesh(np.zeros((0, 3)), np.zeros((0, 3)))
    for p in parts:
        m = m.merge(p.mesh)
    with open(path, 'w') as f:
        f.write(f'OFF\n# McCode mcdisplay export, metres\n{len(m.verts)} {len(m.tris)} 0\n')
        for v in m.verts:
            f.write(f'{v[0]:.12g} {v[1]:.12g} {v[2]:.12g}\n')
        for a, b, c in m.tris:
            f.write(f'3 {a} {b} {c}\n')
    _warn_surfaces_only(parts, 'OFF')


def export_vrml(parts, path):
    with open(path, 'w') as f:
        f.write('#VRML V2.0 utf8\n# McCode mcdisplay export\n')
        for p in parts:
            f.write(f'DEF {p.name.replace(" ", "_")} Transform {{ children [\n')
            if p.polylines:
                f.write('Shape { appearance Appearance { material Material { diffuseColor '
                        + _rgb01(p.color) + ' emissiveColor ' + _rgb01(p.color) + ' } } '
                        'geometry IndexedLineSet { '
                        'coord Coordinate { point [')
                pts, idx, off = [], [], 0
                for pl in p.polylines:
                    for v in pl:
                        pts.append(f'{_fmt(v[0])} {_fmt(v[1])} {_fmt(v[2])}')
                    idx.append(','.join(str(off + i) for i in range(len(pl))) + ',-1')
                    off += len(pl)
                f.write(','.join(pts) + '] } coordIndex [' + ','.join(idx) + '] } },\n')
            if p.mesh is not None and not p.mesh.empty():
                m = p.mesh
                f.write('Shape { appearance Appearance { material Material { diffuseColor '
                        + _rgb01(p.color) + ' } } geometry IndexedFaceSet { '
                        'coord Coordinate { point [')
                f.write(','.join(f'{_fmt(x)} {_fmt(y)} {_fmt(z)}' for x, y, z in m.verts))
                f.write('] } coordIndex [')
                f.write(','.join(f'{a},{b},{c},-1' for a, b, c in m.tris))
                f.write('] solid FALSE } }')
            f.write('] }\n')


def export_x3d(parts, path):
    with open(path, 'w') as f:
        f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
        f.write('<X3D profile="Interchange" version="3.3" '
                'xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance">\n')
        f.write('<Scene>\n')
        for p in parts:
            nm = p.name.replace('"', '')
            mat = (f'<Material diffuseColor="{_rgb01(p.color)}" '
                   f'emissiveColor="{_rgb01(p.color)}"/>')
            if p.polylines:
                pts, idx, off = [], [], 0
                for pl in p.polylines:
                    for v in pl:
                        pts.append('%.6g %.6g %.6g' % tuple(v))
                    idx.extend([off + i for i in range(len(pl))] + [-1])
                    off += len(pl)
                f.write(f'<Transform><Shape><Appearance>{mat}</Appearance>')
                f.write(f'<IndexedLineSet coordIndex="{" ".join(map(str, idx))}">'
                        f'<Coordinate point="{" ".join(pts)}"/></IndexedLineSet>')
                f.write(f'</Shape></Transform><!-- {nm} lines -->\n')
            if p.mesh is not None and not p.mesh.empty():
                m = p.mesh
                pts = ' '.join('%.6g %.6g %.6g' % tuple(v) for v in m.verts)
                idx = ' '.join(map(str, sum(([int(a), int(b), int(c), -1] for a, b, c in m.tris), [])))
                f.write(f'<Transform><Shape><Appearance>{mat}</Appearance>')
                f.write(f'<IndexedFaceSet solid="false" coordIndex="{idx}">'
                        f'<Coordinate point="{pts}"/></IndexedFaceSet>')
                f.write(f'</Shape></Transform><!-- {nm} -->\n')
        f.write('</Scene>\n</X3D>\n')


def export_vtk(parts, path):
    pts, lines, faces = [], [], []
    for p in parts:
        for pl in p.polylines:
            base = len(pts)
            pts.extend(map(tuple, pl))
            lines.append(len(pl))
            lines.extend(range(base, base + len(pl)))
        if p.mesh is not None and not p.mesh.empty():
            base = len(pts)
            pts.extend(map(tuple, p.mesh.verts))
            for t in p.mesh.tris:
                faces.append(3)
                faces.extend(base + int(i) for i in t)
    with open(path, 'w') as f:
        f.write('# vtk DataFile Version 3.0\nMcCode mcdisplay export\nASCII\nDATASET POLYDATA\n')
        f.write(f'POINTS {len(pts)} double\n')
        for v in pts:
            f.write(f'{_fmt(v[0])} {_fmt(v[1])} {_fmt(v[2])}\n')
        nlpoly = sum(len(pp.polylines) for pp in parts)
        f.write(f'LINES {nlpoly} {len(lines)}\n')
        f.write(' '.join(map(str, lines)) + '\n')
        nface = sum(1 for x in faces if x == 3)
        f.write(f'POLYS {nface} {len(faces)}\n')
        f.write(' '.join(map(str, faces)) + '\n')
        # per-point component color scalars (COLORSCALAR names are auto-loaded by many VTK viewers)
        f.write(f'POINT_DATA {len(pts)}\n')
        f.write('COLORSCALARS component_color 3\n')
        for p in parts:
            npts = sum(len(pl) for pl in p.polylines)
            if p.mesh is not None and not p.mesh.empty():
                npts += len(p.mesh.verts)
            for _ in range(npts):
                f.write(' '.join(f'{c:.4g}' for c in p.color) + '\n')


def _tube_mesh(p0, p1, radius, segments=4):
    ''' open 4-sided cylinder (no caps) between two points, as a triangle mesh;
    viewers see the inside via doubleSided materials '''
    p0 = np.asarray(p0, dtype=np.float64).reshape(3)
    p1 = np.asarray(p1, dtype=np.float64).reshape(3)
    w, u, v = _basis_from_axis(p1 - p0)
    th = np.linspace(0, 2 * np.pi, segments, endpoint=False)
    ring = radius * (np.outer(np.cos(th), u) + np.outer(np.sin(th), v))
    return _strip(p0 + ring, p1 + ring)


_G = (1 + 5 ** 0.5) / 2  # icosahedron: the sphere at each polyline corner
ICO_V = np.array([(-1, _G, 0), (1, _G, 0), (-1, -_G, 0), (1, -_G, 0), (0, -1, _G), (0, 1, _G),
                  (0, -1, -_G), (0, 1, -_G), (_G, 0, -1), (_G, 0, 1), (-_G, 0, -1), (-_G, 0, 1)]) / np.hypot(1, _G)
ICO_F = np.array([(0, 11, 5), (0, 5, 1), (0, 1, 7), (0, 7, 10), (0, 10, 11), (1, 5, 9), (5, 11, 4),
                  (11, 10, 2), (10, 7, 6), (7, 1, 8), (3, 9, 4), (3, 4, 2), (3, 2, 6), (3, 6, 8),
                  (3, 8, 9), (4, 9, 5), (2, 4, 11), (6, 2, 10), (8, 6, 7), (9, 8, 1)])


def _tubes(xyz, r, k=6):
    ''' NaN-separated polylines -> Mesh: a k-sided open cylinder per segment, a sphere per corner
    (same construction as the mcdisplay-plopp 3D view) '''
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
    return Mesh(np.concatenate([cyl, sph]), np.concatenate([cyl_f, sph_f]))


def lines_to_tubes(parts, thickness=1e-4):
    ''' replace every part's polylines by solid tubes; radius = thickness x scene bounding-box diagonal '''
    # ponytail: fixed thickness as in mcdisplay-plopp; expose it on the CLI if 1e-4 doesn't suit
    pts = [p.mesh.verts for p in parts if p.mesh is not None] + [pl for p in parts for pl in p.polylines]
    if not pts:
        return
    pts = np.vstack(pts)
    r = thickness * np.linalg.norm(pts.max(0) - pts.min(0))
    nan = np.full((1, 3), np.nan)
    for p in parts:
        if p.polylines:
            p.add_mesh(_tubes(np.vstack([np.vstack((pl, nan)) for pl in p.polylines]), r))
            p.polylines = []


def _gltf_buffers(parts):
    ''' build accessor+buffers for all parts; returns glTF dict + binary blob '''
    bin_chunks = []
    accessors = []
    buffviews = []
    materials = []

    def add(arr, dtype, target):
        a = np.ascontiguousarray(arr)
        off = sum(len(b) for b in bin_chunks)
        # pad to 4 bytes
        pad = (4 - (off % 4)) % 4
        if pad:
            bin_chunks.append(b'\0' * pad)
            off += pad
        data = a.astype(dtype).tobytes()
        bin_chunks.append(data)
        buffviews.append({'buffer': 0, 'byteOffset': off, 'byteLength': len(data),
                          'target': target})
        comp = {np.dtype('float32'): 5126, np.dtype('uint32'): 5125}[np.dtype(dtype)]
        if a.ndim == 2 and a.shape[1] == 3:
            atype, amin, amax = 'VEC3', a.min(axis=0), a.max(axis=0)
        else:
            atype, amin, amax = 'SCALAR', np.array([a.min()]), np.array([a.max()])
        acc = {'bufferView': len(buffviews) - 1, 'componentType': comp,
               'count': int(len(a)), 'type': atype,
               'min': [float(x) for x in amin], 'max': [float(x) for x in amax]}
        accessors.append(acc)
        return len(accessors) - 1

    def add_material(color):
        materials.append({'pbrMetallicRoughness': {
            'baseColorFactor': [float(c) for c in color] + [1.0],
            'metallicFactor': 0.1, 'roughnessFactor': 0.7},
            'doubleSided': True})  # wireframe tubes have no caps
        return len(materials) - 1

    nodes, meshes = [], []
    # Wireframe lines are exported as thin tubes (triangle meshes) instead of
    # glTF line primitives (mode 1): Blender's glTF-IO importer crashes on line
    # primitives with a numpy concatenation error, and web viewers (three.js,
    # f3d, castle-model-viewer) support them inconsistently. Tubes load as
    # plain triangles everywhere. Radius scales with the scene size.
    all_pts = []
    for p in parts:
        if p.mesh is not None and not p.mesh.empty():
            all_pts.append(p.mesh.verts)
        all_pts.extend(p.polylines)
    diag = float(np.linalg.norm(np.vstack(all_pts).max(axis=0) - np.vstack(all_pts).min(axis=0))) \
        if all_pts else 1.0
    tube_r = max(diag * 2e-5, 1e-6)
    # Merge all components sharing the same color into ONE mesh: web viewers
    # (castle-model-viewer, three.js) spend memory and time per node/mesh/draw
    # call, so a few large meshes load much faster than hundreds of small ones.
    by_color = {}  # color tuple -> [merged_mesh, [component names]]
    for p in parts:
        mesh = p.mesh
        if p.polylines:
            tubes = None
            for pl in p.polylines:
                # merge consecutive collinear segments into one tube
                runs = [[pl[0], pl[1]]]
                for j in range(1, len(pl) - 1):
                    a, b = pl[j], pl[j + 1]
                    prev = runs[-1]
                    d_prev = prev[1] - prev[0]
                    d_cur = b - a
                    n_p, n_c = np.linalg.norm(d_prev), np.linalg.norm(d_cur)
                    if n_p > 0 and n_c > 0 and abs(np.dot(d_prev / n_p, d_cur / n_c) - 1.0) < 1e-9:
                        prev[1] = b  # collinear continuation: extend the previous run
                    else:
                        runs.append([a, b])
                for a, b in runs:
                    t = _tube_mesh(a, b, tube_r)
                    tubes = t if tubes is None else tubes.merge(t)
            if tubes is not None:
                mesh = tubes if mesh is None else mesh.merge(tubes)
        if mesh is not None and not mesh.empty():
            entry = by_color.setdefault(tuple(p.color), [None, []])
            entry[0] = mesh if entry[0] is None else entry[0].merge(mesh)
            entry[1].append(p.name)
    nodes, meshes = [], []
    for gi, (color, (mesh, names)) in enumerate(by_color.items()):
        mat_idx = add_material(color)
        pv = add(mesh.verts, 'float32', 34962)
        pi = add(mesh.tris.reshape(-1), 'uint32', 34963)
        meshes.append({'primitives': [{'attributes': {'POSITION': pv}, 'indices': pi,
                                       'mode': 4, 'material': mat_idx}]})
        nm = names[0] if len(names) == 1 else f'color_group_{gi} ({len(names)} components)'
        nodes.append({'mesh': len(meshes) - 1, 'name': nm})
    blob = b''.join(bin_chunks)
    gltf = {'asset': {'version': '2.0', 'generator': 'mcdisplay (dependency-free)'},
            'scene': 0,
            'scenes': [{'nodes': list(range(len(nodes)))}],
            'nodes': nodes, 'meshes': meshes, 'materials': materials,
            'buffers': [{'byteLength': len(blob)}],
            'bufferViews': buffviews, 'accessors': accessors}
    return gltf, blob


def export_gltf(parts, path):
    gltf, blob = _gltf_buffers(parts)
    gltf['buffers'][0]['uri'] = Path(path).name + '.bin'
    with open(path, 'w') as f:
        json.dump(gltf, f, separators=(',', ':'))
    with open(str(path) + '.bin', 'wb') as f:
        f.write(blob)


def export_glb(parts, path):
    gltf, blob = _gltf_buffers(parts)
    gltf['buffers'][0]['bufferView'] = len(gltf['bufferViews'])
    gltf['bufferViews'].append({'buffer': 0, 'byteOffset': 0, 'byteLength': len(blob)})
    js = json.dumps(gltf, separators=(',', ':')).encode()
    while len(js) % 4:
        js += b' '
    while len(blob) % 4:
        blob += b'\0'
    with open(path, 'wb') as f:
        f.write(b'glTF' + struct.pack('<II', 2, 12 + 8 + len(js) + 8 + len(blob)))
        f.write(struct.pack('<I', len(js)) + b'JSON' + js)
        f.write(struct.pack('<I', len(blob)) + b'BIN\0' + blob)


def export_step(parts, path):
    ''' Pure-Python STEP (AP214-ish) writer -- no CadQuery/OpenCASCADE needed.

    Each component becomes one PRODUCT:
      - components with a mesh -> faceted BREP: OPEN_SHELL of FACE_SURFACEs,
        each triangle a PLANE bounded by a POLY_LOOP of 3 CARTESIAN_POINTs,
        wrapped in a SHELL_BASED_SURFACE_MODEL
        (MANIFOLD_SURFACE_SHAPE_REPRESENTATION)
      - wireframe-only components -> GEOMETRICALLY_BOUNDED_WIREFRAME_SHAPE_
        REPRESENTATION of a GEOMETRIC_CURVE_SET of POLYLINEs
    Component colors are attached as STYLED_ITEMs (COLOUR_RGB) where the
    reader supports it (e.g. FreeCAD). Coordinates are in metres. '''
    entities = []
    nid = 0

    def E(text):
        nonlocal nid
        nid += 1
        entities.append(f'#{nid}={text};')
        return f'#{nid}'

    def r(x):
        # STEP REAL needs a decimal point: 0 -> 0., 1E-05 -> 1.E-05
        m, e, x = f'{x:.12G}'.partition('E')
        return (m if '.' in m else m + '.') + e + x

    def r3(p):
        return f'({r(p[0])},{r(p[1])},{r(p[2])})'

    def pt(p):
        return E(f"CARTESIAN_POINT('',{r3(p)})")

    # --- global contexts and units ---
    appctx = E("APPLICATION_CONTEXT('core data for automotive mechanical design processes')")
    E(f"APPLICATION_PROTOCOL_DEFINITION('international standard','automotive_design',2010,{appctx})")
    lu = E('(LENGTH_UNIT()NAMED_UNIT(*)SI_UNIT($,.METRE.))')
    pau = E('(NAMED_UNIT(*)PLANE_ANGLE_UNIT()SI_UNIT($,.RADIAN.))')
    sau = E('(NAMED_UNIT(*)SI_UNIT($,.STERADIAN.)SOLID_ANGLE_UNIT())')
    unc = E(f"UNCERTAINTY_MEASURE_WITH_UNIT(LENGTH_MEASURE(1.E-07),{lu},"
            f"'distance_accuracy_value','')")
    # one complex instance for the representation context
    nid += 1
    ctx = f'#{nid}'
    entities.append(f"{ctx}=(GEOMETRIC_REPRESENTATION_CONTEXT(3)"
                    f"GLOBAL_UNCERTAINTY_ASSIGNED_CONTEXT(({unc}))"
                    f"GLOBAL_UNIT_ASSIGNED_CONTEXT(({lu},{pau},{sau}))"
                    f"REPRESENTATION_CONTEXT('',''));" )

    # --- one product per component ---
    for p in parts:
        nm = p.name.replace("'", "''").replace(';', '').replace('#', '')
        pctx = E(f"PRODUCT_CONTEXT('',{appctx},'mechanical')")
        prod = E(f"PRODUCT('{nm}','{nm}','',({pctx}))")
        form = E(f"PRODUCT_DEFINITION_FORMATION_WITH_SPECIFIED_SOURCE('','',{prod},.NOT_KNOWN.)")
        pdctx = E(f"PRODUCT_DEFINITION_CONTEXT('part definition',{appctx},'design')")
        pdef = E(f"PRODUCT_DEFINITION('design','',{form},{pdctx})")
        pds = E(f"PRODUCT_DEFINITION_SHAPE('','',{pdef})")

        styled = []  # styled item ids for this component
        rep = None
        if p.mesh is not None and not p.mesh.empty():
            faces = []
            for tri in p.mesh.verts[p.mesh.tris]:
                a, b, c = tri
                n = np.cross(b - a, c - a)
                ln = np.linalg.norm(n)
                if ln < 1e-12:
                    continue  # skip degenerate triangles
                n = n / ln
                pa, pb, pc = pt(a), pt(b), pt(c)
                loop = E(f"POLY_LOOP('',({pa},{pb},{pc}))")
                fb = E(f'FACE_OUTER_BOUND(\'\',{loop},.T.)')
                dn = E(f"DIRECTION('',{r3(n)})")
                ref = np.array([1.0, 0.0, 0.0]) if abs(n[0]) < 0.9 else np.array([0.0, 1.0, 0.0])
                dr = np.cross(n, ref)
                dr /= np.linalg.norm(dr)
                dd = E(f"DIRECTION('',{r3(dr)})")
                ax = E(f'AXIS2_PLACEMENT_3D(\'\',{pa},{dn},{dd})')
                pl = E(f'PLANE(\'\',{ax})')
                faces.append(E(f"FACE_SURFACE('',({fb}),{pl},.T.)"))
            if faces:
                shell = E(f"OPEN_SHELL('',({','.join(map(str, faces))}))")
                sbsm = E(f'SHELL_BASED_SURFACE_MODEL(\'\',({shell}))')
                rep = E(f'MANIFOLD_SURFACE_SHAPE_REPRESENTATION(\'\',({sbsm}),{ctx})')
                # attach the component color to every face
                col = E(f"COLOUR_RGB('',{','.join(r(x) for x in p.color)})")
                fasc = E(f'FILL_AREA_STYLE_COLOUR(\'\',{col})')
                fas = E(f'FILL_AREA_STYLE(\'\',({fasc}))')
                ssf = E(f'SURFACE_STYLE_FILL_AREA({fas})')
                sss = E(f"SURFACE_SIDE_STYLE('',({ssf}))")
                ssu = E(f'SURFACE_STYLE_USAGE(.BOTH.,{sss})')
                psa = E(f'PRESENTATION_STYLE_ASSIGNMENT(({ssu}))')
                for f_id in faces:
                    styled.append(E(f"STYLED_ITEM('',({psa}),{f_id})"))
        if rep is None and p.polylines:
            # wireframe-only component
            pols = []
            for pl in p.polylines:
                ids = ','.join(str(pt(v)) for v in pl)
                pols.append(E(f"POLYLINE('',({ids}))"))
            gcs = E(f"GEOMETRIC_CURVE_SET('',({','.join(map(str, pols))}))")
            rep = E(f'GEOMETRICALLY_BOUNDED_WIREFRAME_SHAPE_REPRESENTATION(\'\',({gcs}),{ctx})')
        if rep is None:
            continue  # nothing representable in STEP
        E(f'SHAPE_DEFINITION_REPRESENTATION({pds},{rep})')
        if styled:
            E(f"MECHANICAL_DESIGN_GEOMETRIC_PRESENTATION_REPRESENTATION('',"
              f"({','.join(map(str, styled))}),{ctx})")

    with open(path, 'w') as f:
        f.write('ISO-10303-21;\n')
        f.write('HEADER;\n')
        f.write("FILE_DESCRIPTION(('MCDISPLAY geometry export'),'2;1');\n")
        f.write(f"FILE_NAME('{Path(path).name}','{__date__}',('mcdisplay'),"
                f"('McCode'),'mcdisplay dependency-free','McCode','');\n")
        f.write("FILE_SCHEMA(('AUTOMOTIVE_DESIGN { 1 0 10303 214 1 1 1 1 }'));\n")
        f.write('ENDSEC;\nDATA;\n')
        f.write('\n'.join(entities) + '\n')
        f.write('ENDSEC;\nEND-ISO-10303-21;\n')


def export_vtkjs(parts, path):
    ''' Pure-Python VTKJS export -- no CadQuery/VTK needed.

    Writes a .vtkjs (zip) in the VTK JSON scene format used by the vtk.js
    HttpSceneLoader / ParaView Glance viewers:
      index.json             scene description (one entry per component)
      <n>/index.json         vtkPolyData dataset for component n, with binary
                             arrays base64-encoded (float32 points, uint32
                             connectivity), plus a per-component RGB point
                             data array for the cycling colors. '''
    import base64
    import zipfile

    def b64(arr, dtype):
        return base64.b64encode(np.ascontiguousarray(arr, dtype=dtype).tobytes()).decode()

    def ranges(pts):
        out = []
        for c, nm in ((0, 'X'), (1, 'Y'), (2, 'Z')):
            col = pts[:, c]
            out.append({'min': float(col.min()), 'max': float(col.max()),
                        'component': c, 'name': nm})
        return out

    scene = []
    datasets = []
    for i, p in enumerate(parts):
        pts, lines, polys = [], [], []
        for pl in p.polylines:
            base = len(pts)
            pts.extend(map(tuple, pl))
            lines.append(len(pl))
            lines.extend(range(base, base + len(pl)))
        if p.mesh is not None and not p.mesh.empty():
            m = p.mesh
            base = len(pts)
            pts.extend(map(tuple, m.verts))
            for t in m.tris:
                polys += [3] + [base + int(x) for x in t]
        if not pts:
            continue
        pts = np.asarray(pts, dtype=np.float64).reshape((-1, 3))
        ds = {
            'metadata': {'name': p.name},
            'vtkClass': 'vtkPolyData',
            'points': {'vtkClass': 'vtkPoints', 'name': '_points',
                       'numberOfComponents': 3, 'size': int(len(pts) * 3),
                       'dataType': 'Float32Array', 'buffer': b64(pts, '<f4'),
                       'ranges': ranges(pts)},
            'pointData': {
                'vtkClass': 'vtkDataSetAttributes',
                'attributes': [{
                    'vtkClass': 'vtkDataArray', 'name': 'color',
                    'numberOfComponents': 3, 'size': int(len(pts) * 3),
                    'dataType': 'Float32Array',
                    'buffer': b64(np.tile(np.asarray(p.color, dtype=np.float64), (len(pts), 1)), '<f4'),
                }]},
        }
        if polys:
            ds['polys'] = {'vtkClass': 'vtkCellArray', 'name': '_polys',
                           'numberOfComponents': 1, 'size': len(polys),
                           'dataType': 'Uint32Array', 'buffer': b64(polys, '<u4')}
        if lines:
            ds['lines'] = {'vtkClass': 'vtkCellArray', 'name': '_lines',
                           'numberOfComponents': 1, 'size': len(lines),
                           'dataType': 'Uint32Array', 'buffer': b64(lines, '<u4')}
        datasets.append((f'{i}', p.name, ds))
        scene.append({
            'name': p.name, 'type': 'httpdataset',
            'httpdataset': {'url': f'{i}/'},
            'rendering': {
                'actor': {'origin': [0, 0, 0], 'scale': [1, 1, 1], 'position': [0, 0, 0]},
                'mapper': {'colorByArrayName': 'color', 'scalarMode': 'usePointData'},
            },
        })
    index = {'metadata': {'fetchGzip': False}, 'scene': scene}
    with zipfile.ZipFile(path, 'w', zipfile.ZIP_DEFLATED) as z:
        z.writestr('index.json', json.dumps(index))
        for folder, _, ds in datasets:
            z.writestr(f'{folder}/index.json', json.dumps(ds))


EXPORTERS = {
    'obj': (export_obj, '.obj'),
    'stl': (export_stl, '.stl'),
    'ply': (export_ply, '.ply'),
    'off': (export_off, '.off'),
    'vrml': (export_vrml, '.wrl'),
    'x3d': (export_x3d, '.x3d'),
    'xml': (export_x3d, '.x3d'),
    'vtk': (export_vtk, '.vtk'),
    'gltf': (export_gltf, '.gltf'),
    'glb': (export_glb, '.glb'),
    'step': (export_step, '.step'),
    'vtkjs': (export_vtkjs, '.vtkjs'),
}


# ---------------------------------------------------------------------------
# main (same flow as the original script)
# ---------------------------------------------------------------------------

def main(instr=None, dirname=None, **kwds):
    from logging import basicConfig, INFO
    import time
    basicConfig(level=INFO)

    fmt = kwds.get('format', 'stl')
    segments = int(kwds.pop('segments', 64) or 64)  # tessellation resolution
    tubes = kwds.pop('tubes', False)

    # output directory
    if dirname is None:
        from datetime import datetime as dt
        p = Path(instr).resolve()
        dirname = str(p.parent.joinpath(f"{p.stem}_{dt.strftime(dt.now(), '%Y%m%d_%H%M%S')}"))

    from mccodelib.mcdisplayutils import McDisplayReader
    reader = McDisplayReader(dir=dirname, instr=instr, **kwds)
    instrument = reader.read_instrument()
    bundle = reader.read_particles()  # None unless -n > 0

    # The reader fails if this is done before reader.read_instrument() above ?!
    root = Path(dirname)
    root.mkdir(parents=True, exist_ok=True)

    # Convert the instrument into tessellated parts, then save
    parts = instrument_to_parts(instrument, segments)
    if bundle and bundle.rays:
        parts.append(rays_to_part(instrument, bundle))
    if tubes:
        lines_to_tubes(parts)
    output = str(root.joinpath(f"{Path(instr).stem}{EXPORTERS[fmt][1]}"))
    EXPORTERS[fmt][0](parts, output)
    print(f"Wrote {output} ({len(parts)} components)")

    # Use mccode_config / the OS to open the resulting file...
    if Path(output).is_file():
        mccode_config.open_in_browser(output)


def output_format(astring):
    f = astring.lower()
    if f in EXPORTERS:
        return f
    print(f"Unknown file format {astring}. Choose between STEP, STL, VRML, GLTF, GLB, "
          f"OBJ, PLY, OFF, XML/X3D, VTK, VTKJS")
    return 'stl'


if __name__ == '__main__':
    from mccodelib.mcdisplayutils import make_common_parser
    # Only pre-sets instr, --default, options
    parser, prefix = make_common_parser(__file__, __doc__)

    parser.add_argument('--dirname', help='output directory name override')
    parser.add_argument('-n', '--ncount', dest='n', type=float, default=0, help='Number of particle rays to trace and include in the output (default 0: geometry only)')
    parser.add_argument('-s', '--segments', dest='segments', type=int, default=16, help='Tessellation resolution (circle segments); lower = smaller/lighter files, e.g. 32 or 16. Default: 16')
    parser.add_argument('-t', '--trace', dest='trace', type=int, default=1, help='mcrun --trace level: 1 (default, fastest) gives the legacy wireframe drawing, 2 solid primitives. NB: with 1, surface-only formats (STL) may come out empty and many viewers ignore lines in OBJ/PLY; add --tubes to get solids')
    parser.add_argument('--tubes', action='store_true', help='Draw lines (multiline etc.) as solid cylinders with spheres at the corners, as in mcdisplay-plopp. NB: with -n > 0 the particle rays are tubed too, which can make files very large (e.g. ~150 MB STEP for 100 rays in PSI_DMC)')
    parser.add_argument('-f', '--format', dest='format', type=output_format, default='glb', help='Output CAD file format (STEP, STL, VRML, GLTF, GLB, OBJ, PLY, OFF, XML/X3D, VTK and VTKJS). Default: GLB')

    args, unknown = parser.parse_known_args()
    # if --inspect --first or --last are given after instr, the remaining args become "unknown",
    # but we assume that they are instr_options
    args = {k: args.__getattribute__(k) for k in dir(args) if k[0] != '_'}
    if len(unknown):
        args['options'] += unknown

    main(**args)
