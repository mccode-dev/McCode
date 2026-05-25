# Test instrument for the Inhomogenous_incoherent component

### Author: Daniel Lomholt Christensen @NBI

This instrument is a test for the Inhomogenous incoherent process component, that works with the McStas Union system.


Below I have tested the implementation in three important setups. For each setup, I use three different approaches.
First I build the union materials with the Incoherent_process component (and here 10 Union_boxes are used).
Secondly I build the union materials with the Inhomogenous_incoherent_process (here using only 1 component, but setting the number of sampling points to 10).
Thirdly I build a material that uses both the Inhomogenous_incoherent and incoherent processes, to perform the same action.


The first two setups, I have a detector placed on the side of a sample, with 10 different boxes that follow each other.
For the first experiment, the boxes have the same scattering cross section.
For the second experiment, the boxes vary according to a linear rise in scattering cross section.
For a visualization of these setups, see the below figure,
![Sketch of instrument](experiment.png)

And the results of the first experiment is,
![Results experiment 1](sigma_const.png)

While the results of the second experiment is,
![Results experiment 2](linear_sigma.png)

For the last setup, the detector is placed in a transmission geometry, with the 10 boxes rotated 90 degrees.
This means that they vary their scattering cross section linearly across x in the mcstas coordinate system.
The result of this is,
![Transmission geometry](trans_linear.png)

Another experiment is performed, just to check that adding a directional process, such as the PhononSimple_process does not break the system.
This material is created with the linearly rising scattering cross section, implemented with the Inhomogenous process, as well as a PhononSimple_process.



