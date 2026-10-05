"""Write the Phonon_simple dispersion relation as a Dispersion_relation input file.

Phonon_simple models an fcc crystal with nearest neighbour interactions,
    omega(q) = c/a * sqrt(12 - J(q)),
    J(q) = 2 * sum cos(a/2 * (q_i +- q_j)) over the pairs xy, xz, yz.
Along the cubic axes this dispersion repeats every 4*pi/a, so the grid covers
one such period. The Dispersion_relation component must then be given the
lattice constant a/2, so that its a* = 2*pi/(a/2) matches this period, and
h = 1 corresponds to the fcc (2 0 0) reciprocal lattice vector.

The last column is the Phonon_simple cross section written in the form that
Dispersion_relation reads, in barn/sr per unit cell of side a/2:
    I = (rho_atom/rho_cell) * b^2/(4 pi) * (hbar^2 kappa^2/2m_n)/omega / M.
It depends on the full scattering vector kappa, which the folded grid cannot
hold, so it is written for the Brillouin zone around one reciprocal lattice
point given by --zone: each folded coordinate is unfolded to lie within half
a period of that point. The file is only valid for scans in that zone.

With --cell primitive the dispersion is written on the primitive fcc cell,
a = (0, 1/2, 1/2) a, b = (1/2, 0, 1/2) a, c = (1/2, 1/2, 0) a, whose angles are
60 degrees, for Dispersion_relation given these vectors as ax ... cz. Then
(h, k, l) are the coordinates along the primitive reciprocal vectors, the cross
section is per primitive cell (one atom), and the zone dependent kappa is the
image of q nearest the --zone point in Cartesian space.

The components have (h, k, l) along the sample (x, y, z) axes, as the cubic
axes of Phonon_simple.
"""
import argparse
import os

import numpy as np

HBAR2_OVER_2MN = 2.0721  # hbar^2/(2 m_n) [meV AA^2], K2V^2*VS2E in McStas


def phonon_simple_omega(qx, qy, qz, a, c):
    """Phonon_simple dispersion in meV for q in AA^-1."""
    ah = a/2
    Jq = 2*(np.cos(ah*(qx + qy)) + np.cos(ah*(qx - qy))
            + np.cos(ah*(qx + qz)) + np.cos(ah*(qx - qz))
            + np.cos(ah*(qy + qz)) + np.cos(ah*(qy - qz)))
    return c/a*np.sqrt(np.clip(12 - Jq, 0, None))


def phonon_simple_cross_section(kappa2, omega, a, b, M, cell_volume=None):
    """Phonon_simple cross section in barn/sr per unit cell of the given volume.

    Phonon_simple weights a ray by rho_atom/(4 pi) * (hbar^2 kappa^2/2m_n)/omega * b^2/M,
    with b in fm, while Dispersion_relation uses rho_cell * I with I in barn/sr.
    The default cell is the cube of side a/2.
    """
    rho_atom = 4/a**3
    rho_cell = 1/(cell_volume if cell_volume is not None else (a/2)**3)
    b2_barn = b**2*0.01
    with np.errstate(divide="ignore", invalid="ignore"):
        cross_section = (rho_atom/rho_cell)*b2_barn/(4*np.pi)*HBAR2_OVER_2MN*kappa2/omega/M
    # omega = 0 only at the reciprocal lattice points, where no phonon is excited
    return np.where(omega > 0, cross_section, 0)


def unfold(coordinate, zone_centre):
    """Shift a folded coordinate in [0, 1] by whole periods to within 0.5 of zone_centre."""
    return coordinate + np.round(zone_centre - coordinate)


def primitive_fcc_vectors(a):
    """Rows are the primitive fcc lattice vectors a, b, c [AA] with the cubic axes along x, y, z."""
    return a/2*np.array([[0.0, 1, 1], [1, 0, 1], [1, 1, 0]])


def nearest_image(q, reciprocal, centre):
    """Shift q (..., 3) by reciprocal lattice vectors (columns of `reciprocal`) to lie nearest `centre`."""
    shifts = np.array([[i, j, k] for i in range(-2, 3) for j in range(-2, 3) for k in range(-2, 3)], dtype=float)
    images = q[..., np.newaxis, :] + shifts @ reciprocal.T
    distances = np.linalg.norm(images - centre, axis=-1)
    return np.take_along_axis(images, distances.argmin(axis=-1)[..., np.newaxis, np.newaxis], axis=-2)[..., 0, :]


def write_grid(output, columns):
    os.makedirs(os.path.dirname(output) or ".", exist_ok=True)
    np.savetxt(output, np.column_stack([column.ravel() for column in columns]), fmt="%.7e")


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--a", type=float, default=4.95, help="fcc lattice constant [AA] (Pb)")
    parser.add_argument("--c", type=float, default=10.0, help="Velocity of sound [meV AA] (Pb)")
    parser.add_argument("--b", type=float, default=9.405, help="Scattering length [fm] (Pb)")
    parser.add_argument("--M", type=float, default=207.2, help="Atomic mass [a.u.] (Pb)")
    parser.add_argument("--zone", type=float, nargs=3, default=[1, 0, 1], metavar=("H", "K", "L"),
                        help="Reciprocal lattice point, in units of 4 pi/a, whose zone the cross section is written for")
    parser.add_argument("--cell", choices=["cubic", "primitive"], default="cubic",
                        help="cubic: the cube of side a/2 with a, b, c along x, y, z. "
                             "primitive: the primitive fcc cell a = (0, 1/2, 1/2) a, b = (1/2, 0, 1/2) a, "
                             "c = (1/2, 1/2, 0) a, with 60 degree angles, for Dispersion_relation's ax ... cz")
    parser.add_argument("--points", type=int, default=41, help="Grid points along each axis")
    parser.add_argument("--output", default="phonon_dispersion/phonon.dat")
    args = parser.parse_args()

    period = 4*np.pi/args.a

    def mode(points):
        grid = np.linspace(0, 1, points)
        H, K, L = np.meshgrid(grid, grid, grid, indexing="ij")
        # Sample axes: h along x, k along y, l along z
        omega = phonon_simple_omega(H*period, K*period, L*period, args.a, args.c)
        kappa2 = period**2*(unfold(H, args.zone[0])**2 + unfold(K, args.zone[1])**2
                            + unfold(L, args.zone[2])**2)
        return H, K, L, omega, phonon_simple_cross_section(kappa2, omega, args.a, args.b, args.M)

    def primitive_mode(points):
        # (h, k, l) along the primitive reciprocal vectors; q = h a* + k b* + l c*
        lattice = primitive_fcc_vectors(args.a)
        cell_volume = abs(np.linalg.det(lattice))
        reciprocal = 2*np.pi*np.linalg.inv(lattice)  # columns a*, b*, c*, since lattice @ reciprocal = 2 pi
        grid = np.linspace(0, 1, points)
        H, K, L = np.meshgrid(grid, grid, grid, indexing="ij")
        q = np.stack([H, K, L], axis=-1) @ reciprocal.T
        omega = phonon_simple_omega(q[..., 0], q[..., 1], q[..., 2], args.a, args.c)
        # --zone is in cubic units of 4 pi/a; the cross section uses the image of q nearest it
        kappa = nearest_image(q, reciprocal, np.array(args.zone)*period)
        kappa2 = (kappa**2).sum(axis=-1)
        return H, K, L, omega, phonon_simple_cross_section(kappa2, omega, args.a, args.b, args.M, cell_volume)

    H, K, L, omega, cross_section = (primitive_mode if args.cell == "primitive" else mode)(args.points)
    write_grid(args.output, [H, K, L, omega, cross_section])
    print(f"Wrote {omega.size} points to {args.output}, omega from {omega.min():.3g} to {omega.max():.3g} meV, "
          f"cross section up to {cross_section.max():.3g} barn/sr, zone around {tuple(args.zone)}")


if __name__ == "__main__":
    main()
