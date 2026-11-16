#!/usr/bin/env python3
"""Reduce the number of faces in an OFF/STL/OBJ/PLY mesh by quadric edge collapse."""
import argparse
import sys

import fast_simplification
import trimesh


def reduce(mesh, reduction=None, faces=None, lossless=False):
    """Return a reduced copy of mesh. reduction is in %, faces is the wanted face count,
    lossless only merges exactly flat regions."""
    if faces is not None and faces >= len(mesh.faces):
        return mesh
    # Open-mesh boundaries are kept fixed; closed meshes have none
    v, f = fast_simplification.simplify(
        mesh.vertices, mesh.faces, preserve_border=True,
        target_reduction=None if reduction is None else reduction / 100,
        target_count=faces, lossless=lossless)
    return trimesh.Trimesh(v, f)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("input", help="input mesh file")
    p.add_argument("output", help="output mesh file, format from extension (e.g. .off, .stl)")
    g = p.add_mutually_exclusive_group(required=True)
    g.add_argument("-r", "--reduction", type=float, help="percentage of faces to remove, 0-100")
    g.add_argument("-n", "--faces", type=int, help="wanted number of faces")
    g.add_argument("--lossless", action="store_true", help="only merge exactly flat regions")
    a = p.parse_args()

    if a.reduction is not None and not 0 <= a.reduction < 100:
        p.error("--reduction must be in [0, 100)")
    if a.faces is not None and a.faces < 4:
        p.error("--faces must be at least 4")

    mesh = trimesh.load(a.input, force="mesh")
    out = reduce(mesh, a.reduction, a.faces, a.lossless)
    out.export(a.output)
    print(f"{a.input}: {len(mesh.faces)} -> {len(out.faces)} triangles, written to {a.output}")
    if mesh.is_watertight and not out.is_watertight:
        print("WARNING: input mesh was closed, reduced mesh is not", file=sys.stderr)


if __name__ == "__main__":
    main()
