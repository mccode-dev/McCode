"""Write the SpinWave_BCO antiferromagnetic magnon as Dispersion_relation input files.

SpinWave_BCO models spin waves on a body centred orthorhombic lattice with the
lattice constants a1, a2, a3 along the sample x, y, z axes. For the
antiferromagnet (FM=0) it has two modes, b = 0 and 1,
    omega_b(q) = sqrt(A(q)^2 - S^2 J(q)^2) - (2b - 1) (B g mu_B + 2 S (j_110 - j_110') sin(q_x a1) sin(q_y a2)),
    A(q) = S (J(0) - J1(0) + J1(q)) - 2 D (S - 1/2),
    J(q) = 8 j cos(q_x a1/2) cos(q_y a2/2) cos(q_z a3/2),
    J1(q) = 2 (ja cos(q_x a1) + jb cos(q_y a2) + jc cos(q_z a3)) + 2 (j_110 + j_110') cos(q_x a1) cos(q_y a2).
Only J(q)^2 enters omega, so the dispersion repeats with the conventional cell
a1 x a2 x a3, and each mode is written to its own file in the output folder.

Dispersion_relation has (h, k, l) along the sample (x, y, z) axes, like a1,
a2, a3, so it is given a = a1, b = a2 and c = a3. Its unit cell volume a*b*c
is then the volume V_0 = a1 a2 a3 that SpinWave_BCO uses.

The last column is the SpinWave_BCO cross section of the mode in the form
Dispersion_relation reads, in barn/sr per unit cell:
    I_b = (gamma r_0)^2 * (g F / 2)^2 * (1 + kappa_z^2/kappa^2)/4 * C_b(kappa),
with the coherence factor
    C_b = 2 S (S (J(0) - J(kappa)) - S (J1(0) - J1(kappa)) - 2 D (S - 1/2))
          / (omega_b + (2b - 1) (B g mu_B + 2 S (j_110 - j_110') sin(kappa_x a1) sin(kappa_y a2))).
The polarisation factor and J(kappa) in C_b change sign or value between
zones, so I is written for the zone around one reciprocal lattice point given
by --zone in Dispersion_relation's (h, k, l). The files are only valid for
scans in that zone. Dispersion_relation adds k_f/k_i, DW and the Bose factor.

The defaults are the MnF2 parameters of the SpinWave_BCO example instrument.
"""
import argparse
import os

import numpy as np

GAMMA_R0_SQUARED = (1.913*2.8179)**2*0.01  # (gamma r_0)^2 [barn]
G_FACTOR = 2.0023
B2E = 0.11590188615547234  # g mu_B [meV/T], as in SpinWave_BCO


def magnon(qx, qy, qz, branch, args):
    """omega [meV] and coherence factor of mode `branch` at q in AA^-1 (sample axes)."""
    a1, a2, a3, S, D = args.a1, args.a2, args.a3, args.S, args.D
    Jq = 8*args.j*np.cos(qx*a1/2)*np.cos(qy*a2/2)*np.cos(qz*a3/2)
    J0 = 8*args.j
    J1q = (2*(args.ja*np.cos(qx*a1) + args.jb*np.cos(qy*a2) + args.jc*np.cos(qz*a3))
           + 2*(args.j_110 + args.j_110_prime)*np.cos(qx*a1)*np.cos(qy*a2))
    J10 = 2*(args.ja + args.jb + args.jc) + 2*(args.j_110 + args.j_110_prime)
    A = S*(J0 - J10 + J1q) - 2*D*(S - 0.5)
    splitting = (2*branch - 1)*(args.B*B2E + 2*S*(args.j_110 - args.j_110_prime)*np.sin(qx*a1)*np.sin(qy*a2))
    omega = np.sqrt(np.clip(A**2 - S**2*Jq**2, 0, None)) - splitting
    with np.errstate(divide="ignore", invalid="ignore"):
        coherence = 2*S*(S*(J0 - Jq) - S*(J10 - J1q) - 2*D*(S - 0.5))/(omega + splitting)
    return omega, coherence


def unfold(coordinate, zone_centre):
    """Shift a folded coordinate in [0, 1] by whole periods to within 0.5 of zone_centre."""
    return coordinate + np.round(zone_centre - coordinate)


def build_parser():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--a1", type=float, default=4.873, help="Lattice constant along x [AA]")
    parser.add_argument("--a2", type=float, default=4.873, help="Lattice constant along y [AA]")
    parser.add_argument("--a3", type=float, default=3.130, help="Lattice constant along z [AA]")
    parser.add_argument("--S", type=float, default=2.5, help="Spin")
    parser.add_argument("--j", type=float, default=2*0.152, help="Coupling to body centred spins [meV]")
    parser.add_argument("--ja", type=float, default=2*0.004, help="Coupling along a [meV]")
    parser.add_argument("--jb", type=float, default=2*0.004, help="Coupling along b [meV]")
    parser.add_argument("--jc", type=float, default=-2*0.028, help="Coupling along c [meV]")
    parser.add_argument("--j_110", type=float, default=0, help="Coupling along +x+y [meV]")
    parser.add_argument("--j_110_prime", type=float, default=0, help="Coupling along +x-y [meV]")
    parser.add_argument("--D", type=float, default=-0.023, help="Uniaxial anisotropy [meV]")
    parser.add_argument("--B", type=float, default=0, help="Magnetic field along z [T]")
    parser.add_argument("--Fq", type=float, default=1, help="Magnetic form factor")
    parser.add_argument("--zone", type=float, nargs=3, default=[1, 0, 0], metavar=("H", "K", "L"),
                        help="Reciprocal lattice point in Dispersion_relation's (h, k, l) whose zone "
                             "the cross section is written for")
    parser.add_argument("--points", type=int, default=41, help="Grid points along each axis")
    parser.add_argument("--output", default="magnon_dispersion", help="Folder for the mode files")
    return parser


def default_parameters():
    """The magnon parameters used when no options are given (MnF2)."""
    return build_parser().parse_args([])


def main():
    args = build_parser().parse_args()

    grid = np.linspace(0, 1, args.points)
    H, K, L = np.meshgrid(grid, grid, grid, indexing="ij")
    # Dispersion_relation axes: h along x (a1), k along y (a2), l along z (a3)
    Hu, Ku, Lu = unfold(H, args.zone[0]), unfold(K, args.zone[1]), unfold(L, args.zone[2])
    qx, qy, qz = Hu*2*np.pi/args.a1, Ku*2*np.pi/args.a2, Lu*2*np.pi/args.a3
    kappa2 = qx**2 + qy**2 + qz**2
    with np.errstate(divide="ignore", invalid="ignore"):
        polarisation = np.where(kappa2 > 0, 1 + qz**2/kappa2, 0)

    os.makedirs(args.output, exist_ok=True)
    for branch in (0, 1):
        omega, coherence = magnon(qx, qy, qz, branch, args)
        cross_section = GAMMA_R0_SQUARED*(G_FACTOR*args.Fq/2)**2*polarisation/4*coherence
        # The coherence factor diverges where omega = 0 (magnetic Bragg points)
        cross_section = np.where(np.isfinite(cross_section) & (omega > 0), cross_section, 0)
        output = os.path.join(args.output, f"mode_{branch}.dat")
        np.savetxt(output, np.column_stack([H.ravel(), K.ravel(), L.ravel(), omega.ravel(), cross_section.ravel()]),
                   fmt="%.7e")
        print(f"Wrote {omega.size} points to {output}, omega from {omega.min():.3g} to {omega.max():.3g} meV, "
              f"cross section up to {cross_section.max():.3g} barn/sr, zone around {tuple(args.zone)}")


if __name__ == "__main__":
    main()
