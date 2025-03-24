#!/usr/bin/env python3
"""Compute X-ray reflectivity tables for materials
The 'reflec_xraydb' command computes the reflectivity of materials vs energy and 
take-off angle. All arguments are optional except the material formula. Use a known material (see them with --list) or a new one when given its density (--density).
  
EXAMPLES:
  reflec_xraydb.py --emin 5600 --emax 46300 Pd  --density 12.02; 
  reflec_xraydb.py --emin 3400 --emax 19000 Pt  --density 21.45; 
  reflec_xraydb.py --emin 3400 --emax 46300 B4C --density  2.52;
"""

import numpy  as     np
from   xraydb import mirror_reflectivity, get_materials, find_material, add_material
import sys
import argparse      # to parse input arguments

def compute_reflectivity(formula, E_min=3400, E_max=19000, E_nb=200, theta_min=0.0001, theta_max=2, theta_nb=2000, density=None, verbose=0):

    # set energy (eV) and angle (rad) lists
    E_min   = float(E_min)
    E_max   = float(E_max)
    E_step  = (E_max-E_min)/(E_nb-1)
    liste_e = []
    liste_e.append(E_min)

    for i in range(1,E_nb):
        liste_e.append(E_min+i*E_step)
        
    # angle: deg -> radians
    theta_min = float(theta_min)*np.pi/180
    theta_max = float(theta_max)*np.pi/180
    theta_step= (theta_max-theta_min)/(theta_nb-1)
    theta     = np.linspace(theta_min, theta_max, theta_nb)
    
    if verbose:
      print( "Material: ", formula)
      print(f"Energy= {E_min}:{E_max} in {E_nb} steps [eV]")
      print(f"Angle=  {theta_min*180/np.pi}:{theta_max*180/np.pi} in {theta_nb} steps [deg]")

    # now compute the reflectivity with xraydb for each energu,angle and write file.
    filename = formula+".dat"
    
    # mirror_reflectivity, other parameters: density, roughness, polarization ... 
    # see https://xraypy.github.io/XrayDB/python.html#xraydb.mirror_reflectivity
    with open(filename,'w') as f:
        #for the file: E in kev, theta in deg
        f.write(f"# This is a reflecivity file for material {formula}\n")
        f.write( "# To be used for e.g. McXtrace mirror components\n")
        f.write( "# One row per energy (eV). Columns are along angle (deg).\n")
        f.write("#-------------------------------------------------------------------------------")
        f.write("# param=eth\n")
        f.write(f"# material={formula}\n")
        if density:
          f.write("# density="+str(density)+"  # material density in [g/cm^3]\n")
        f.write("# E_min="+str(E_min*10**(-3))+"  # minimum energy in [eV]\n")
        f.write("# E_max="+str(E_max*10**(-3))+"  # maximum energy in [eV]\n")
        f.write("# E_step="+str(E_step*10**(-3))+"  # number of energy bins\n")
        f.write("# theta_min="+str(theta_min*180/np.pi)+"  # minimum take-off angle [deg]\n")
        f.write("# theta_max="+str(theta_max*180/np.pi)+"  # maximum take-off angle [deg]\n")
        f.write("# theta_step="+str(theta_step*180/np.pi)+"  # number of angle bins\n")
        
        for single_energy in liste_e:
            #print(single_energy)
            r_si = mirror_reflectivity(formula, theta, single_energy, density=float(density) if density else None)
            for single_ref in r_si:            
                f.write(str(single_ref)+"      ")
            f.write('\n')
    
    if verbose:
      print("Generated ", filename)
    return filename

# ------------------------------------------------------------------------------
#                                  THE MAIN
# ------------------------------------------------------------------------------
if __name__ == "__main__":

    version = '25.03.18'

    # argparse doc format:
    # https://stackoverflow.com/questions/3853722/how-to-insert-newlines-on-argparse-help-text
    class Formatter(
      argparse.ArgumentDefaultsHelpFormatter): pass

    # read input arguments (after loading the YAML file)
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=Formatter)
    # required argument
    parser.add_argument('formula', 
        help="A chemical formula, e.g. 'Pt','B4C'. Use --list to show all known materials, or specify --density to add a new chemical formula.", nargs='?', default=None, type=str)
        
    # optional arguments
    parser.add_argument(
        '--emin', '--energy-min',
        nargs=1,
        metavar="EMIN",
        help="Minimum energy in [eV]",
        type=float, required=False, default=[3400],
    )
    parser.add_argument(
        '--emax', '--energy-max',
        nargs=1,
        metavar="EMAX",
        help="Maximum energy in [eV]",
        type=float, required=False, default=[19000],
    )
    parser.add_argument(
        '--ebin', '--energy-bin',
        nargs=1,
        metavar="EBIN",
        help="Number of energy bins",
        type=float, required=False, default=[200],
    )
    
    parser.add_argument(
        '--amin', '--theta-min',
        nargs=1,
        metavar="THETA_MIN",
        help="Minimum take-off angle in [deg]",
        type=float, required=False, default=[0.0001],
    )
    parser.add_argument(
        '--amax', '--theta-max',
        nargs=1,
        metavar="THETA_MAX",
        help="Maximum take-off angle in [deg]",
        type=float, required=False, default=[2],
    )
    parser.add_argument(
        '--abin', '--theta-bin', 
        nargs=1,
        metavar="EBIN",
        help="Number of angle bins",
        type=float, required=False, default=[2000],
    )
    
    parser.add_argument(
        '--density', 
        nargs=1,
        metavar="DENSITY",
        help="Material density in [g/cm^3]. Obtained from formula when ommitted.",
        type=float, required=False, default=None,
    )
    
    # version
    parser.add_argument(
        '-v','--version',
        action="store_true",
        help="Show version.",
        required=False, default=None,
    )
    
    # verbose
    parser.add_argument(
        '--verbose',
        action="store_true",
        help="Show more information.",
        required=False, default=0,
    )
    
    # list materials
    parser.add_argument(
        '--list',
        action="store_true",
        help="Show pre-defined materials. Set a chemical formula and its --density to define a new one.",
        required=False, default=0,
    )
    
    # parse arguments
    args = parser.parse_args()
    
    if args.version is not None:
        print(__file__, version)
        print("\nCopyright (C) 2022- Synchrotron Soleil, GPL3.\n")
        print("This is free software; see the source for copying conditions.  There is NO")
        print("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n")
        print("Written by Antoine Padovani and Emmanuel Farhi <emmanuel.farhi@synchrotron-soleil.fr>")
        exit(0)
        
    if args.list:
        # show all available materials
        print("Pre-defined materials (xraydb):")
        for name, m in get_materials().items():
          print(m)
        exit(0)
    
    if args.formula is None:
        raise ValueError(f"\nERROR: Please give material. Use --help for help or --list to show known materials, or specify a new chemical formula with its --density.")
    
    # check if material exists
    if find_material(args.formula) is None:
        if args.density:
          # we can add material, hoping the formula is OK
          if args.verbose:
            print("Using new chemical formula {args.formula} with density {args.density[0]} [g/cm3] (xraydb).")
          add_material(args.formula, args.formula, args.density[0], categories=['user'])
        else:
          raise ValueError(f"\nERROR: Unknown material {args.formula}. Use --help for help or --list to show known materials, or specify a new chemical formula with its --density.")
    
    # compute
    filename = compute_reflectivity(args.formula, args.emin[0], args.emax[0], args.ebin[0], \
      args.amin[0], args.amax[0], args.abin[0], \
      args.density[0] if args.density else None, args.verbose)

    # end of script reflec_xraydb
