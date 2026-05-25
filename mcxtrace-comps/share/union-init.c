/*******************************************************************************
*
*  McXtrace, photon ray-tracing package
*  Copyright(C) 2007 Risoe National Laboratory.
*
* %I
* Written by: Mads Bertelsen
* Date: 20.08.15
* Version: $Revision: 0.1 $
* Origin: University of Copenhagen
*
* A sample component to separate geometry and phsysics
*
* %D
* Alpha version, no input system yet
* Hardcode input to geometry engine
* Allows complicated geometry by combination of simple shapes
*
* Algorithm:
* Described elsewhere
*
* %P
* INPUT PARAMETERS:
* radius:  [m] Outer radius of sample in (x,z) plane
*
* OUTPUT PARAMETERS:
* V_rho:  [AA^-3] Atomic density
*
* %L
* The test/example instrument <a href="../examples/Test_Phonon.instr">Test_Phonon.instr</a>.
*
* %E
******************************************************************************/


// This file sets up global lists needed for Union components to communicate with each other
// These all have dynamically allocated memory somewhere in the structure, which is deallocated
//  by the last Union_master.


// Initialize global positions / rotations to transform lists
  // These are lists of pointers to positons / rotations, that will be updated from global frame
  //  to the frame of the master component that uses them in that masters initialize section.
  struct global_positions_to_transform_list_struct global_positions_to_transform_list = {0,NULL};
  struct global_rotations_to_transform_list_struct global_rotations_to_transform_list = {0,NULL};

// Initialize global_process_list
  // Used to facilitate communication between processes and the other types of Union components
  struct pointer_to_global_process_list global_process_list = {0,NULL};

// Initialize global_material_list
  // Used to facilitate communication between materials and the other types of Union components
  struct pointer_to_global_material_list global_material_list = {0,NULL};

// Initialize global_geometry_list
  // Used to facilitate communication between geometries and the other types of Union components
  struct pointer_to_global_geometry_list global_geometry_list = {0,NULL};

// Initialize global_logger_lists
  // Used to facilitate communication between loggers and the other types of Union components
  struct pointer_to_global_logger_list global_all_volume_logger_list = {0,NULL};
  struct pointer_to_global_logger_list global_specific_volumes_logger_list = {0,NULL};

// Initialize global_tagging_conditional_list
  // Used to facilitate communication between conditionals and the other types of Union components
  struct global_tagging_conditional_list_struct global_tagging_conditional_list = {0,0,NULL};

// Initialize global_master_list
  // Used to facilitate communication between Master components (mainly for deallocation)
  struct pointer_to_global_master_list global_master_list = {0,NULL};
