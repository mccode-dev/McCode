"""Write the dispersion files of Test_Dispersion_relation_TAS.instr that are missing.

Run by the SHELL command of the instrument when it is compiled. Each folder is
only written if it does not exist yet, in the working directory, where the
instrument reads it; delete a folder to have it written again:

    phonon_dispersion            generate_phonon_dispersion.py
    phonon_dispersion_primitive  generate_phonon_dispersion.py --cell primitive --points 81
    magnon_dispersion            generate_magnon_dispersion.py

The generators are run with the same Python interpreter as this script.
"""
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

FILES = [
    ("phonon_dispersion/phonon.dat",
     ["generate_phonon_dispersion.py"]),
    ("phonon_dispersion_primitive/phonon.dat",
     ["generate_phonon_dispersion.py", "--cell", "primitive", "--points", "81",
      "--output", "phonon_dispersion_primitive/phonon.dat"]),
    ("magnon_dispersion/mode_0.dat",
     ["generate_magnon_dispersion.py"]),
]


def main():
    for output, command in FILES:
        if os.path.isfile(output):
            continue
        script, *arguments = command
        subprocess.run([sys.executable, os.path.join(HERE, script), *arguments], check=True)


if __name__ == "__main__":
    main()
