#!/usr/bin/env python3
"""Digitise the ESS emblem for the ESS_butterfly MCDISPLAY (target wheel).

Flattens the emblem outline of the ess.eu logo (https://ess.eu/themes/custom/ess/graphics/logo-small.svg)
into polygon contours, triangulates the filled area (ear clipping, holes bridged in) and writes the
result as C arrays into ESS_butterfly-geometry.c, between the "BEGIN/END generated ESS logo data"
markers. Coordinates are centred on the emblem and scaled to unit diameter (largest extent 1),
u to the right and v upwards (SVG y flipped). The ESS logo is (c) ESS. All rights reserved.

Usage: python3 ESS_butterfly-logo.py logo-small.svg ESS_butterfly-geometry.c [--dry-run]
                                     [--tolerance 0.25] [--preview preview.png]
--dry-run reports what would change without writing; --preview (needs matplotlib) draws the result.
"""
import argparse
import math
import re
import sys

BEGIN = "/* BEGIN generated ESS logo data"
END = "/* END generated ESS logo data */"


def parse_path(d):
    toks = re.findall(r'[MmCcHhVvLlSsZz]|-?(?:\d+\.?\d*|\.\d+)(?:e-?\d+)?', d)
    i, cmd = 0, None
    cur = (0.0, 0.0)
    start = cur
    paths, pts = [], []

    def num():
        nonlocal i
        v = float(toks[i]); i += 1
        return v

    while i < len(toks):
        if re.match(r'[A-Za-z]', toks[i]):
            cmd = toks[i]; i += 1
        if cmd in 'Mm':
            x, y = num(), num()
            if cmd == 'm':
                x += cur[0]; y += cur[1]
            if pts:
                paths.append(pts)
            cur = start = (x, y); pts = [cur]
            cmd = 'L' if cmd == 'M' else 'l'
        elif cmd in 'Ll':
            x, y = num(), num()
            if cmd == 'l':
                x += cur[0]; y += cur[1]
            cur = (x, y); pts.append(cur)
        elif cmd in 'Hh':
            x = num()
            cur = (x + (cur[0] if cmd == 'h' else 0), cur[1]); pts.append(cur)
        elif cmd in 'Vv':
            y = num()
            cur = (cur[0], y + (cur[1] if cmd == 'v' else 0)); pts.append(cur)
        elif cmd in 'Cc':
            c = [num() for _ in range(6)]
            if cmd == 'c':
                c = [c[k] + cur[k % 2] for k in range(6)]
            p0, p1, p2, p3 = cur, (c[0], c[1]), (c[2], c[3]), (c[4], c[5])
            n = max(2, int(math.ceil((math.dist(p0, p1) + math.dist(p1, p2) + math.dist(p2, p3)) / 1.0)))
            for k in range(1, n + 1):
                t = k / n; a = 1 - t
                pts.append((a**3*p0[0] + 3*a*a*t*p1[0] + 3*a*t*t*p2[0] + t**3*p3[0],
                            a**3*p0[1] + 3*a*a*t*p1[1] + 3*a*t*t*p2[1] + t**3*p3[1]))
            cur = p3
        elif cmd in 'Zz':
            cur = start
        else:
            raise ValueError(cmd)
    if pts:
        paths.append(pts)
    return paths


def dp(pts, tol):
    """Douglas-Peucker on an open polyline"""
    if len(pts) < 3:
        return pts
    a, b = pts[0], pts[-1]
    L = math.dist(a, b) or 1e-12
    dmax, imax = 0, 0
    for k in range(1, len(pts) - 1):
        p = pts[k]
        dd = abs((b[0]-a[0])*(a[1]-p[1]) - (a[0]-p[0])*(b[1]-a[1])) / L
        if dd > dmax:
            dmax, imax = dd, k
    if dmax <= tol:
        return [a, b]
    return dp(pts[:imax+1], tol)[:-1] + dp(pts[imax:], tol)


def simplify_ring(r, tol):
    r = [p for k, p in enumerate(r) if math.dist(p, r[k-1]) > 1e-9]
    # split at the point farthest from r[0] so DP keeps both ends
    far = max(range(len(r)), key=lambda k: math.dist(r[0], r[k]))
    out = dp(r[:far+1], tol)[:-1] + dp(r[far:] + [r[0]], tol)[:-1]
    return out


def area(r):
    return 0.5 * sum(r[k-1][0]*r[k][1] - r[k][0]*r[k-1][1] for k in range(len(r)))


def inside(p, r):
    c = False
    for k in range(len(r)):
        a, b = r[k-1], r[k]
        if (a[1] > p[1]) != (b[1] > p[1]) and p[0] < a[0] + (p[1]-a[1])*(b[0]-a[0])/(b[1]-a[1]):
            c = not c
    return c


def cross(o, a, b):
    return (a[0]-o[0])*(b[1]-o[1]) - (a[1]-o[1])*(b[0]-o[0])


def seg_intersect(p1, p2, q1, q2):
    d1, d2 = cross(q1, q2, p1), cross(q1, q2, p2)
    d3, d4 = cross(p1, p2, q1), cross(p1, p2, q2)
    return (d1*d2 < 0) and (d3*d4 < 0)


def bridge(outer, hole, V):
    """merge hole (CW) into outer (CCW), both index lists into V, via a mutually visible pair"""
    hi = max(range(len(hole)), key=lambda k: V[hole[k]][0])
    hp = V[hole[hi]]
    best = None
    for oi in sorted(range(len(outer)), key=lambda k: math.dist(V[outer[k]], hp)):
        op = V[outer[oi]]
        ok = True
        for ring in (outer, hole):
            for k in range(len(ring)):
                a, b = V[ring[k-1]], V[ring[k]]
                if seg_intersect(hp, op, a, b):
                    ok = False; break
            if not ok:
                break
        if ok:
            best = oi; break
    assert best is not None
    h = hole[hi:] + hole[:hi]
    return outer[:best+1] + h + [h[0], outer[best]] + outer[best+1:]


def earclip(poly, V):
    idx = list(poly)
    tris = []
    guard = 0
    while len(idx) > 3 and guard < 100000:
        guard += 1
        n = len(idx)
        cut = False
        for k in range(n):
            a, b, c = idx[k-1], idx[k], idx[(k+1) % n]
            A, B, C = V[a], V[b], V[c]
            if cross(A, B, C) <= 1e-12:
                continue
            ok = True
            for j in idx:
                if j in (a, b, c):
                    continue
                P = V[j]
                if P in (A, B, C):
                    continue
                if cross(A, B, P) >= 0 and cross(B, C, P) >= 0 and cross(C, A, P) >= 0:
                    ok = False; break
            if ok:
                tris.append((a, b, c)); del idx[k]; cut = True; break
        if not cut:
            # degenerate remainder: drop a collinear vertex
            for k in range(len(idx)):
                if abs(cross(V[idx[k-1]], V[idx[k]], V[idx[(k+1) % len(idx)]])) < 1e-12:
                    del idx[k]; cut = True; break
            if not cut:
                raise RuntimeError("ear clipping stuck with %d vertices" % len(idx))
    if len(idx) == 3 and cross(V[idx[0]], V[idx[1]], V[idx[2]]) > 1e-12:
        tris.append(tuple(idx))
    return tris


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("svg")
    ap.add_argument("geometry_c")
    ap.add_argument("--tolerance", type=float, default=0.25, help="curve tolerance in SVG units")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--preview")
    args = ap.parse_args()
    s = open(args.svg, encoding="utf-8").read()
    d = re.search(r'\sd="([^"]*)"', s, re.S).group(1)
    rings = [simplify_ring(r, args.tolerance) for r in parse_path(d)]
    # unit diameter, centred, v up
    xs = [p[0] for r in rings for p in r]; ys = [p[1] for r in rings for p in r]
    cx, cy = (min(xs)+max(xs))/2, (min(ys)+max(ys))/2
    D = max(max(xs)-min(xs), max(ys)-min(ys))
    rings = [[((x-cx)/D, -(y-cy)/D) for x, y in r] for r in rings]
    # holes: odd nesting depth; orient outers CCW, holes CW
    depth = [sum(inside(r[0], q) for j, q in enumerate(rings) if j != i) for i, r in enumerate(rings)]
    for i, r in enumerate(rings):
        if (area(r) > 0) == (depth[i] % 2 == 1):
            r.reverse()
    V, contours = [], []
    for r in rings:
        contours.append((len(V), len(r))); V.extend(r)
    rid = [list(range(st, st+n)) for st, n in contours]
    tris = []
    for i in range(len(rings)):
        if depth[i] % 2:
            continue
        poly = rid[i]
        holes = [j for j in range(len(rings)) if depth[j] == depth[i]+1 and inside(rings[j][0], rings[i])]
        for j in sorted(holes, key=lambda j: -max(p[0] for p in rings[j])):
            poly = bridge(poly, rid[j], V)
        tris += earclip(poly, V)
    A_rings = sum(area(r) for r in rings)
    A_tris = sum(0.5*cross(V[a], V[b], V[c]) for a, b, c in tris)
    print("contours %d (nesting %s), points %d, triangles %d, area contours %.6f triangles %.6f"
          % (len(rings), depth, len(V), len(tris), A_rings, A_tris))
    if abs(A_rings - A_tris) > 1e-6 * max(1, abs(A_rings)) or not all(0.5*cross(V[a], V[b], V[c]) > 0 for a, b, c in tris):
        sys.exit("Error: triangulation does not cover the emblem exactly, nothing written")

    out = [BEGIN + " (by ESS_butterfly-logo.py, do not edit by hand):",
           "   ESS emblem from the ess.eu logo (themes/custom/ess/graphics/logo-small.svg), %d contours"
           % len(rings),
           "   (holes running opposite to outlines) of %d points in total, centred and scaled to unit" % len(V),
           "   diameter with u to the right and v up, plus %d cap triangles (indices into the points)."
           % len(tris),
           "   The ESS logo is (c) ESS. All rights reserved. */",
           "int ess_logo_ncontours = %d;" % len(rings),
           "int ess_logo_contours[] = {%s};" % ", ".join("%d, %d" % c for c in contours),
           "int ess_logo_npoints = %d;" % len(V)]
    for name, k in (("u", 0), ("v", 1)):
        vals = ["%.4f" % p[k] for p in V]
        out.append("double ess_logo_%s[] = {" % name)
        out += ["  " + ", ".join(vals[j:j+12]) + ("," if j+12 < len(vals) else "") for j in range(0, len(vals), 12)]
        out.append("};")
    out.append("int ess_logo_ntriangles = %d;" % len(tris))
    out.append("int ess_logo_triangles[] = {")
    flat = ["%d" % i for t in tris for i in t]
    out += ["  " + ", ".join(flat[j:j+18]) + ("," if j+18 < len(flat) else "") for j in range(0, len(flat), 18)]
    out += ["};", END]
    block = "\n".join(out)

    geo = open(args.geometry_c, encoding="utf-8").read()
    i0, i1 = geo.find(BEGIN), geo.find(END)
    if i0 < 0 or i1 < i0:
        sys.exit("Error: markers not found in %s" % args.geometry_c)
    new = geo[:i0] + block + geo[i1+len(END):]
    if new == geo:
        print("%s: logo data unchanged" % args.geometry_c)
    elif args.dry_run:
        print("%s: logo data would change (dry run, nothing written)" % args.geometry_c)
    else:
        open(args.geometry_c, "w", encoding="utf-8").write(new)
        print("%s: logo data updated" % args.geometry_c)

    if args.preview:
        import matplotlib; matplotlib.use("Agg")
        import matplotlib.pyplot as plt
        from matplotlib.collections import PolyCollection
        fig, ax = plt.subplots(1, 2, figsize=(12, 6))
        ax[0].add_collection(PolyCollection([[V[a], V[b], V[c]] for a, b, c in tris], facecolor="#0099dc", edgecolor="none"))
        ax[1].add_collection(PolyCollection([[V[a], V[b], V[c]] for a, b, c in tris], facecolor="#cfe9f7", edgecolor="#0099dc", lw=0.3))
        for a_ in ax:
            a_.set_xlim(-0.55, 0.55); a_.set_ylim(-0.55, 0.55); a_.set_aspect("equal")
        ax[0].set_title("filled triangles"); ax[1].set_title("triangulation")
        plt.savefig(args.preview, dpi=100)


if __name__ == "__main__":
    main()
