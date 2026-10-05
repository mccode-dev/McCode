/*******************************************************************************
 *
 * mcvine-union-lib.h : glue between the MCViNE kernels (mcvine-lib) and the
 * McStas Union framework. Used by the MCViNE_*_process.comp components.
 *
 * The process type "MCViNE" is declared in share/union-lib.h (the enum entry
 * MCViNE in enum process and the member pointer_to_a_MCViNE_physics_storage_struct
 * in union data_transfer_union), which must be included first. All MCViNE
 * kernels share this one process type; its Union_master dispatch cases are
 * registered at the end of this file.
 *
 *******************************************************************************/
#ifndef MCVINE_UNION_LIB_H
#define MCVINE_UNION_LIB_H

#define MCVINE_UNION_GENERIC 0  /* kernel samples its own final state      */
#define MCVINE_UNION_DGSSXRES 1 /* final direction from Union focusing      */

struct MCViNE_physics_storage_struct {
  void* m_kernel;         /* pointer to an mcvine_kernel_* struct             */
  mcvine_S_fn m_S;        /* its S() function                                 */
  double m_my_scattering; /* scattering inverse penetration depth [1/m]      */
  int m_kind;             /* MCVINE_UNION_*                                   */
};

int MCViNE_physics_my (double* my, double* k_initial, union data_transfer_union data_transfer, struct focus_data_struct* focus_data,
                       _class_particle* _particle);
int MCViNE_physics_scattering (double* k_final, double* k_initial, double* weight, union data_transfer_union data_transfer,
                               struct focus_data_struct* focus_data, _class_particle* _particle);
/* fill a Union scattering_process_struct for an MCViNE kernel */
void mcvine_union_register (struct pointer_to_global_process_list* process_list, struct scattering_process_struct* proc,
                            struct global_process_element_struct* elem, struct MCViNE_physics_storage_struct* storage,
                            const char* name, int comp_index, double interact_fraction, int anisotropic, Rotation rot);

/* Register the MCViNE process with the Union_master dispatch (see union-lib.h) */
#undef UNION_CASE_PHYSICS_MY_MCVINE
#define UNION_CASE_PHYSICS_MY_MCVINE(out, ...) case MCViNE: out = MCViNE_physics_my(__VA_ARGS__); break;
#undef UNION_CASE_PHYSICS_SCATTERING_MCVINE
#define UNION_CASE_PHYSICS_SCATTERING_MCVINE(out, ...) case MCViNE: out = MCViNE_physics_scattering(__VA_ARGS__); break;
#endif
