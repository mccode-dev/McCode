/*******************************************************************************
 * mcvine-union-lib.c : implementation of mcvine-union-lib.h
 *******************************************************************************/
#ifndef MCVINE_UNION_LIB_C
#define MCVINE_UNION_LIB_C
#ifndef MCVINE_UNION_LIB_H
  #include "mcvine-union-lib.h"
#endif

int
MCViNE_physics_my (double* my, double* k_initial, union data_transfer_union data_transfer, struct focus_data_struct* focus_data, _class_particle* _particle) {
  *my = data_transfer.pointer_to_a_MCViNE_physics_storage_struct->m_my_scattering;
  return 1;
}

int
MCViNE_physics_scattering (double* k_final, double* k_initial, double* weight, union data_transfer_union data_transfer, struct focus_data_struct* focus_data,
                           _class_particle* _particle) {
  struct MCViNE_physics_storage_struct* st = data_transfer.pointer_to_a_MCViNE_physics_storage_struct;
  double v[3], r[3] = { 0, 0, 0 };
  int ok, i;
  for (i = 0; i < 3; i++)
    v[i] = k_initial[i] * K2V;
  if (st->m_kind == MCVINE_UNION_DGSSXRES) {
    /* aim with the Union focusing set on the geometry (target_index, focus_r, ...) */
    Coords k_out;
    double solid_angle = 0, d[3], n, L;
    L = sqrt (focus_data->RayAim.x * focus_data->RayAim.x + focus_data->RayAim.y * focus_data->RayAim.y + focus_data->RayAim.z * focus_data->RayAim.z);
    if (!(L > 0))
      return 0;
    focus_data->focusing_function (&k_out, &solid_angle, focus_data);
    n = sqrt (k_out.x * k_out.x + k_out.y * k_out.y + k_out.z * k_out.z);
    d[0] = k_out.x / n;
    d[1] = k_out.y / n;
    d[2] = k_out.z / n;
    ok = mcvine_DGSSXRes_final ((mcvine_kernel_DGSSXRes*)st->m_kernel, d, solid_angle, L, v, _particle->t, weight, _particle);
  } else {
    ok = st->m_S (st->m_kernel, r, v, _particle->t, weight, _particle);
  }
  if (!ok || !(*weight > 0) || v[0] != v[0] || v[1] != v[1] || v[2] != v[2])
    return 0;
  for (i = 0; i < 3; i++)
    k_final[i] = v[i] * V2K;
  return 1;
}

void
mcvine_union_register (struct pointer_to_global_process_list* process_list, struct scattering_process_struct* proc,
                       struct global_process_element_struct* elem, struct MCViNE_physics_storage_struct* storage,
                       const char* name, int comp_index, double interact_fraction, int anisotropic, Rotation rot) {
  scattering_process_struct_init (proc);
  proc->non_isotropic_rot_index = anisotropic ? 1 : -1;
  proc->needs_cross_section_focus = -1;
  proc->eProcess = MCViNE;
  sprintf (proc->name, "%s", name);
  proc->process_p_interact = interact_fraction;
  proc->data_transfer.pointer_to_a_MCViNE_physics_storage_struct = storage;
  proc->probability_for_scattering_function = &MCViNE_physics_my;
  proc->scattering_function = &MCViNE_physics_scattering;
  rot_copy (proc->rotation_matrix, rot);
  sprintf (elem->name, "%s", name);
  elem->component_index = comp_index;
  elem->p_scattering_process = proc;
  add_element_to_process_list (process_list, *elem);
}
#endif
