# Parked components and instruments
In this folder, we store instruments and components that rely on
features not yet available in the mcstas-3.x series

## *Scatter_log* and **shielding** instruments / components
The scatter logger and shielding mechanisms redefine the SCATTER keyword, at
present we do not know how to implement this feature in McStas 3.
The shielding instruments also need components that are not in McStas.

## Test_Magnetic_Userdefined.instr
For now, user-written magnetic field functions are not available in
McStas 3. Most usecases should however be covered with what is
available in the ```Pol_Bfield ``` and ```Pol_tabled_field```
components.

## Test_shellguides.instr
This instrument and the related components have not been ported to
McStas 3. Ideally they should be rewritten, taking advantage of the
functionalities of McStas 3.
