#!/usr/bin/env python3
"""Write an OFF mesh of pore channels for PowderN_porous(pores=...) or a Union
mask (Union_mesh) in a cylinder (radius R, height H, axis y). Each channel runs
bottom to top as one swept tube: a chain of cylinder segments with mitered
joints, each segment tilted from y by a gaussian angle. Channel ends stick out
of top/bottom (open). Channels do not overlap, so every pore is one closed,
outward oriented shell (needed by the parity inside-test of Union). Units: m.
Run with --check to verify mesh volume/orientation."""
import argparse
import numpy as np
from scipy.spatial import cKDTree


def tube(P, d, nseg):
    """Closed, outward oriented tube of diameter d along polyline P (k x 3)."""
    r, n = d / 2, len(P)
    t = np.diff(P, axis=0)
    t /= np.linalg.norm(t, axis=1)[:, None]
    axes = [t[0]] + [(t[i - 1] + t[i]) / np.linalg.norm(t[i - 1] + t[i]) for i in range(1, n - 1)] + [t[-1]]
    e1 = np.cross(t[0], [1.0, 0, 0] if abs(t[0][0]) < 0.9 else [0, 0, 1.0])
    phi = 2 * np.pi * np.arange(nseg) / nseg
    v = [P[0]]
    for p, a in zip(P, axes):
        e1 = e1 - np.dot(e1, a) * a  # parallel transport: no twist
        e1 /= np.linalg.norm(e1)
        e2 = np.cross(a, e1)  # (e1, e2, a) right handed: outward faces below
        # ponytail: plain circle at mitered joints, pinches bends by 1-cos(bend/2)
        v += [p + r * (np.cos(f) * e1 + np.sin(f) * e2) for f in phi]
    v.append(P[-1])
    top = len(v) - 1
    f = [(0, 1 + (i + 1) % nseg, 1 + i) for i in range(nseg)]
    for j in range(n - 1):
        a, b = 1 + j * nseg, 1 + (j + 1) * nseg
        f += [(a + i, a + (i + 1) % nseg, b + (i + 1) % nseg, b + i) for i in range(nseg)]
    a = 1 + (n - 1) * nseg
    f += [(a + i, a + (i + 1) % nseg, top) for i in range(nseg)]
    return np.array(v), f


def volume(v, faces):
    """Signed volume (divergence theorem); > 0 for outward faces."""
    return sum(np.dot(v[f[0]], np.cross(v[f[i]], v[f[i + 1]])) for f in faces for i in range(1, len(f) - 1)) / 6


def channel(rng, R, H, d, length, l_sig, tilt):
    """Polyline from y=-H/2-d/2 to y>=H/2+d/2, each segment tilted from +y by a
    gaussian angle, staying inside radius R-d/2."""
    rr, ph = (R - d / 2) * np.sqrt(rng.uniform()), rng.uniform(0, 2 * np.pi)
    P = [np.array([rr * np.cos(ph), -H / 2 - d / 2, rr * np.sin(ph)])]
    while P[-1][1] < H / 2 + d / 2:
        for _ in range(100):
            ll = max(rng.normal(length, l_sig), d / 2)
            th, ph = abs(rng.normal(0, np.radians(tilt))), rng.uniform(0, 2 * np.pi)
            q = P[-1] + ll * np.array([np.sin(th) * np.cos(ph), np.cos(th), np.sin(th) * np.sin(ph)])
            if np.hypot(q[0], q[2]) + d / 2 <= R:
                break
        else:
            return None
        P.append(q)
    return np.array(P)


def sample(P, step):
    """Points along polyline P, spacing <= step."""
    return np.vstack([np.linspace(a, b, int(np.ceil(np.linalg.norm(b - a) / step)) + 1) for a, b in zip(P[:-1], P[1:])])


def pores(R, H, porosity, d, d_sig, length, l_sig, tilt, gap, nseg, seed):
    """Non-overlapping channels until porosity (or no more fit)."""
    rng = np.random.default_rng(seed)
    target, vol, out, fails = porosity * np.pi * R**2 * H, 0.0, [], 0
    pts, rmax, tree = [], 0.0, None
    step = max(d - 3 * d_sig, d / 4) / 4
    while vol < target and fails < 500:
        dd = rng.normal(d, d_sig)
        P = channel(rng, R, H, dd, length, l_sig, tilt) if 0 < dd < 2 * R else None
        if P is None:
            fails += 1
            continue
        s = sample(P, step)
        # ponytail: largest radius so far for every neighbour, a bit conservative
        if tree is not None and tree.query(s)[0].min() < dd / 2 + rmax + gap + step:
            fails += 1
            continue
        fails = 0
        out.append(tube(P, dd, nseg))
        pts.append(s)
        rmax, tree = max(rmax, dd / 2), cKDTree(np.vstack(pts))
        vol += np.pi * dd**2 / 4 * np.sum(np.linalg.norm(np.diff(P, axis=0), axis=1))
    if vol < target:
        print(f"Warning: only porosity ~{vol / (np.pi * R**2 * H):.3f} fits (target {porosity})")
    return out, vol / (np.pi * R**2 * H)


def write_off(fname, shells):
    nv = sum(len(v) for v, _ in shells)
    nf = sum(len(f) for _, f in shells)
    with open(fname, "w") as fh:
        fh.write(f"OFF\n{nv} {nf} 0\n")
        for v, _ in shells:
            fh.writelines(f"{x:.9g} {y:.9g} {z:.9g}\n" for x, y, z in v)
        o = 0
        for v, f in shells:
            fh.writelines(f"{len(t)} " + " ".join(str(i + o) for i in t) + "\n" for t in f)
            o += len(v)


if __name__ == "__main__":
    a = argparse.ArgumentParser(description=__doc__)
    a.add_argument("-o", "--output", default="pores.off")
    a.add_argument("--radius", type=float, default=0.005, help="outer cylinder radius [m]")
    a.add_argument("--yheight", type=float, default=0.02, help="outer cylinder height [m]")
    a.add_argument("--porosity", type=float, default=0.2, help="target pore volume fraction (incl. ends sticking out)")
    a.add_argument("--diameter", type=float, default=5e-4, help="mean channel diameter [m]")
    a.add_argument("--diameter_sigma", type=float, default=1e-4, help="gaussian sigma of diameter [m]")
    a.add_argument("--length", type=float, default=3e-3, help="mean segment length [m]")
    a.add_argument("--length_sigma", type=float, default=1e-3, help="gaussian sigma of segment length [m]")
    a.add_argument("--tilt", type=float, default=10, help="gaussian sigma of segment tilt from y [deg]")
    a.add_argument("--gap", type=float, default=1e-5, help="minimum matrix wall between channels [m]")
    a.add_argument("--nseg", type=int, default=12, help="segments around each channel")
    a.add_argument("--seed", type=int, default=None)
    a.add_argument("--check", action="store_true", help="self-check tube mesh and exit")
    p = a.parse_args()
    if p.check:
        n = 256
        v, f = tube(np.array([[0, 0, 0], [0, 1.0, 0], [0.1, 2.0, 0]]), 1.0, n)
        poly = n / 2 * 0.25 * np.sin(2 * np.pi / n)  # polygon area, r=0.5
        exact = poly * (1 + np.hypot(0.1, 1))
        assert abs(volume(v, f) / exact - 1) < 0.01, (volume(v, f), exact)
        print("ok")
        raise SystemExit
    shells, phi = pores(p.radius, p.yheight, p.porosity, p.diameter, p.diameter_sigma,
                        p.length, p.length_sigma, p.tilt, p.gap, p.nseg, p.seed)
    write_off(p.output, shells)
    print(f"{p.output}: {len(shells)} channels, porosity ~{phi:.3f}")
