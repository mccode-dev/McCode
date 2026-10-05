"""Plot two dimensional scans of Test_Dispersion_relation_TAS.instr side by side.

Loads every point of an mcrun -M scan with McStasScript and draws the detector
intensity (or with --quantity ncount the number of rays reaching the detector,
which does not depend on the ray weights) as a map over any two of the
instrument parameters, such as (h, l) at constant energy transfer or (h, dE)
along a line in reciprocal space. One panel is drawn per scan folder. By
default each map is normalised to its own maximum; with --scale shared all
maps share one absolute colour scale.

The theoretical prediction is drawn on top as the curve where the energy of a
mode equals the energy transfer, omega(h, k, l) = |dE|, for every mode of the
reference model. Coordinates that are not axes of the plot are taken from the
fixed parameters of the scan (k = 0 in the instrument). For an (h, l) map at
fixed dE this is the constant energy contour, for an (h, dE) map at fixed l the
dispersion curve. The reference follows sample_choice of the scan: the
Phonon_simple phonon for the phonon samples and the SpinWave_BCO magnon for the
magnon samples, with the defaults of the generator scripts.

The column labels in the mccode.dat summary of an mcrun -M scan are not always
in the order of the data columns, so the summary is not used.
"""
import argparse
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import mcstasscript as ms
import numpy as np
from matplotlib.colors import LinearSegmentedColormap
from matplotlib.lines import Line2D

from generate_magnon_dispersion import default_parameters, magnon
from generate_phonon_dispersion import phonon_simple_omega

SURFACE = "#fcfcfb"
TEXT_PRIMARY = "#0b0b0b"
TEXT_SECONDARY = "#52514e"
# Single hue sequential ramp, light to dark
SEQUENTIAL = ["#fcfcfb", "#cde2fb", "#9ec5f4", "#6da7ec", "#3987e5", "#256abf", "#184f95", "#0d366b"]

UNITS = {"h": "r.l.u.", "k": "r.l.u.", "l": "r.l.u.", "dE": "meV", "Ef": "meV", "T": "K"}
PHONON_SAMPLES = (0, 1, 4)
MAGNON_SAMPLES = (2, 3)


def read_scan_points(folder, monitor="detector"):
    """Parameters and monitor results of every point of a scan, as arrays keyed by name.

    Each point of an mcrun scan is a numbered folder with its own mccode.sim,
    which McStasScript loads together with the parameter values of that point.
    The monitor results are stored as "intensity", "error" and "ncount".
    """
    points = []
    for point in sorted(os.listdir(folder)):
        point_folder = os.path.join(folder, point)
        if not os.path.isfile(os.path.join(point_folder, "mccode.sim")):
            continue
        metadata = ms.name_search(monitor, ms.load_data(point_folder)).metadata
        values = {name: value for name, value in metadata.parameters.items()
                  if isinstance(value, (int, float))}
        values.update(intensity=metadata.total_I, error=metadata.total_E, ncount=metadata.total_N)
        points.append(values)
    if not points:
        raise ValueError(f"No scan point folders with mccode.sim in {folder}")
    return {name: np.array([point[name] for point in points]) for name in points[0]}


def varying_parameters(scan):
    ignored = {"intensity", "error", "ncount"}
    return [name for name, values in scan.items() if name not in ignored and np.unique(values).size > 1]


def fixed_value(scan, name, default=None):
    """The value of a parameter that is the same for all points, or default if absent."""
    if name not in scan:
        return default
    values = np.unique(scan[name])
    if values.size != 1:
        raise ValueError(f"The parameter {name} varies in the scan but is not an axis of the plot")
    return values[0]


def to_grid(x, y, values):
    x_values = np.unique(x)
    y_values = np.unique(y)
    grid = np.full((len(y_values), len(x_values)), np.nan)
    grid[np.searchsorted(y_values, y), np.searchsorted(x_values, x)] = values
    return x_values, y_values, grid


def edges(centres):
    if len(centres) == 1:
        return np.array([centres[0] - 0.5, centres[0] + 0.5])
    step = np.diff(centres)
    return np.concatenate([[centres[0] - step[0]/2], centres[:-1] + step/2, [centres[-1] + step[-1]/2]])


def reference_model(name, phonon_a, phonon_c):
    """Name of the model and a function giving the energies of all its modes at (h, k, l)."""
    if name == "phonon":
        period = 4*np.pi/phonon_a  # h, k, l in units of 4 pi/a, the cube of side a/2

        def modes(h, k, l):
            return [phonon_simple_omega(h*period, k*period, l*period, phonon_a, phonon_c)]
        return "Phonon_simple", modes
    if name == "magnon":
        parameters = default_parameters()

        def modes(h, k, l):
            q = (h*2*np.pi/parameters.a1, k*2*np.pi/parameters.a2, l*2*np.pi/parameters.a3)
            return [magnon(*q, branch, parameters)[0] for branch in (0, 1)]
        return "SpinWave_BCO", modes
    raise ValueError(f"Unknown reference model {name}")


def reference_for(scan, requested):
    if requested != "auto":
        return None if requested == "none" else requested
    sample_choice = fixed_value(scan, "sample_choice")
    if sample_choice in PHONON_SAMPLES:
        return "phonon"
    if sample_choice in MAGNON_SAMPLES:
        return "magnon"
    return None


def axis_label(name):
    return f"{name} [{UNITS[name]}]" if name in UNITS else name


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("scan_folders", nargs="+", help="Output folders of the mcrun scans")
    parser.add_argument("--labels", nargs="+", help="Panel title for each scan folder")
    parser.add_argument("--x", help="Parameter along the horizontal axis (default: the first varying one)")
    parser.add_argument("--y", help="Parameter along the vertical axis (default: the second varying one)")
    parser.add_argument("--quantity", choices=["intensity", "ncount"], default="intensity",
                        help="Plot the detector intensity or the number of rays reaching the detector")
    parser.add_argument("--scale", choices=["each", "shared"], default="each",
                        help="Normalise each map to its own maximum, or use one absolute scale for all")
    parser.add_argument("--reference", choices=["auto", "phonon", "magnon", "none"], default="auto",
                        help="Theoretical prediction to draw; auto follows sample_choice of the first scan")
    parser.add_argument("--a", type=float, default=4.95, help="fcc lattice constant of the phonon [AA]")
    parser.add_argument("--c", type=float, default=10.0, help="Velocity of sound of the phonon [meV AA]")
    parser.add_argument("--output", default="scan.png")
    args = parser.parse_args()
    labels = args.labels or args.scan_folders
    if len(labels) != len(args.scan_folders):
        parser.error("Give one label per scan folder")

    scans = [read_scan_points(folder) for folder in args.scan_folders]
    varying = varying_parameters(scans[0])
    x_name = args.x or (varying[0] if varying else None)
    y_name = args.y or next((name for name in varying if name != x_name), None)
    if x_name is None or y_name is None:
        parser.error(f"Need two scanned parameters; the first scan varies {varying}")

    plt.rcParams.update({"font.size": 11, "text.color": TEXT_PRIMARY,
                         "axes.labelcolor": TEXT_SECONDARY, "xtick.color": TEXT_SECONDARY,
                         "ytick.color": TEXT_SECONDARY, "axes.edgecolor": TEXT_SECONDARY})
    n_scans = len(scans)
    fig, axes = plt.subplots(1, n_scans, figsize=(4.6*n_scans + 1.2, 5.2), facecolor=SURFACE,
                             squeeze=False, sharey=True, layout="constrained")
    axes = axes[0]
    colormap = LinearSegmentedColormap.from_list("sequential_blue", SEQUENTIAL)

    grids = [to_grid(scan[x_name], scan[y_name], scan[args.quantity]) for scan in scans]
    shared_maximum = max(np.nanmax(grid) for _, _, grid in grids)
    # Show a shared scale in units of its power of ten
    exponent = int(np.floor(np.log10(shared_maximum))) if shared_maximum > 0 else 0
    unit = "rays" if args.quantity == "ncount" else "n/s"

    reference_name = None
    for ax, scan, (x_values, y_values, grid), label in zip(axes, scans, grids, labels):
        maximum = np.nanmax(grid)
        if args.scale == "shared":
            normalised = grid/10.0**exponent
            vmax = shared_maximum/10.0**exponent
        else:
            normalised = grid/maximum if maximum > 0 else grid
            vmax = 1

        ax.set_facecolor(SURFACE)
        mesh = ax.pcolormesh(edges(x_values), edges(y_values), normalised, cmap=colormap,
                             vmin=0, vmax=vmax, shading="flat")

        model = reference_for(scan, args.reference)
        if model is not None:
            reference_name, modes = reference_model(model, args.a, args.c)
            X, Y = np.meshgrid(np.linspace(x_values[0], x_values[-1], 400),
                               np.linspace(y_values[0], y_values[-1], 400))
            coordinates = {}
            for name in ("h", "k", "l", "dE"):
                if name == x_name:
                    coordinates[name] = X
                elif name == y_name:
                    coordinates[name] = Y
                else:
                    coordinates[name] = np.full_like(X, fixed_value(scan, name, 0.0))
            for omega in modes(coordinates["h"], coordinates["k"], coordinates["l"]):
                ax.contour(X, Y, omega - np.abs(coordinates["dE"]), levels=[0], colors=TEXT_PRIMARY,
                           linewidths=2, linestyles="dashed")

        ax.set_title(f"{label}\nmax {maximum:.3g} {unit}", color=TEXT_PRIMARY, loc="left", fontsize=12)
        ax.set_xlabel(axis_label(x_name))
        if UNITS.get(x_name) == UNITS.get(y_name):
            ax.set_aspect("equal")
        for spine in ("top", "right"):
            ax.spines[spine].set_visible(False)
    axes[0].set_ylabel(axis_label(y_name))

    colorbar = fig.colorbar(mesh, ax=axes, pad=0.02, shrink=0.85)
    quantity_name = "Detector ray count" if args.quantity == "ncount" else "Detector intensity"
    if args.scale == "shared":
        colorbar.set_label(f"{quantity_name} [10$^{{{exponent}}}$ {unit}]")
    else:
        colorbar.set_label(f"{quantity_name} / maximum of scan")
    colorbar.outline.set_visible(False)

    fixed = []
    for name in ("h", "k", "l", "dE"):
        if name not in (x_name, y_name):
            value = fixed_value(scans[0], name, 0.0 if name == "k" else None)
            if value is not None:
                fixed.append(f"{name} = {value:g}" + (" meV" if name == "dE" else ""))
    fig.suptitle("Scans" + (" at " + ", ".join(fixed) if fixed else "")
                 + (", rays reaching the detector" if args.quantity == "ncount" else ""),
                 x=0.02, ha="left", color=TEXT_PRIMARY, fontsize=13)
    if reference_name:
        reference_line = Line2D([], [], color=TEXT_PRIMARY, linewidth=2, linestyle="dashed",
                                label=f"{reference_name}: ω(h, k, l) = |dE|")
        fig.legend(handles=[reference_line], loc="outside lower left", frameon=False, labelcolor=TEXT_SECONDARY)

    fig.savefig(args.output, dpi=150, facecolor=SURFACE)
    print(f"Saved {args.output}")


if __name__ == "__main__":
    main()
