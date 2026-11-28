"""Write the dispersion file read by the Dispersion_relation samples of Test_UB_matrix.instr.

Run by the SHELL command of the instrument when it is compiled; the file is only
written if it does not exist yet. The mode is deliberately anisotropic in h, k
and l, so a wrongly oriented lattice changes the scattered intensity.
"""
import math
import os

OUTPUT = os.path.join("UB_dispersion", "mode_0.dat")
POINTS = 9

if not os.path.exists(OUTPUT):
    os.makedirs(os.path.dirname(OUTPUT), exist_ok=True)
    grid = [i / (POINTS - 1) for i in range(POINTS)]
    with open(OUTPUT, "w") as f:
        for h in grid:
            for k in grid:
                for l in grid:
                    energy = 3 + 4 * math.sin(math.pi * h) ** 2 + 2 * math.sin(math.pi * k) ** 2 + math.sin(math.pi * l) ** 2
                    intensity = 1 + h
                    f.write(f"{h:.6g} {k:.6g} {l:.6g} {energy:.6g} {intensity:.6g}\n")
