/****************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2003, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* In library: share/supermirror.h
* Depends on "polyhedron" (which depends on "plane")
*
* Usage: in SHARE
* %include "supermirror"
*
* %I
* Written by: Wai Tung Lee
* Date: September 2024
* Origin: ESS
*
* %D
* Calculate the neutron reflection and transmission of a flat supermirror with parallel mirror surfaces.
* The following can be specified: 
* 1. reflection from either mirror coating or substrate surface, mirror coating can be single-side, double-side, 
* 2. absorber coating:  beneath mirror coating, single-side coated, double-side coated, or uncoated,
* 3. refraction and total reflection at substrate surface,
* 4. attenuation and internal reflection inside substrate. 
*
* More details in "supermirror.h"
*
*******************************************************************************/

#ifndef SUPERMIRROR_LIB_H
#include "supermirror-lib.h"
#endif

#ifndef SUPERMIRROR_LIB_C
#define SUPERMIRROR_LIB_C

/*************************************/
/* Definition of internal paramaters */
/*************************************/
typedef struct SimState {
	//neutron state parameters
	int ray_count;
	char location[CHAR_BUF_LENGTH];
	char event[CHAR_BUF_LENGTH];
	double w; 
	double t; 
	Coords p; 
	Coords v; 
	Coords s;
	double ws[2]; //index=spin+,spin-
	
	double v_len;
	double vn_len;
	
	//intersect parameters
	int side; int plane; int layer; 
	int n_mirror_intersect; //number of intersects to a mirror 
	int n_surface_intersect; //number of intersects to a surface
	Coords fn; //surface normal of intersected plane
	double last_time; Coords last_point; int last_plane;
	
	//parameters indexing on plane number
	int ir_order_at_plane[2]; //index=plane number, ir-order at plane
	Coords v_exit_at_plane[2]; //index=plane number, v_exit
	Coords v_ir_at_plane[2]; //index = plane number {0, 1}, internal reflection v 
	
	double Rm_at_plane[2][2];  //1st index=plane number {0, 1}, 2nd index=spin+,spin-, mirror reflectivity
	double L_attn_abs_at_plane[2]; //index=plane number {0, 1}, absorber layer attenuation length
	double t_prop_abs_at_plane[2]; //index=plane number {0, 1}, time through absorber layer
	double T_prop_abs_at_plane[2]; //index = plane number {0, 1}, transmission through absorber layer
	
	double Rs_at_plane[2]; //index = plane number, substrate surface reflectivity
	double L_attn_sub; //substrate attenuation length
	double t_prop_sub; //time through substrate
	double T_prop_sub; //transmission through substrate
	
	//parameters indexing on the order of intersecting reflecting surfaces
	int side_at_ir_order[2];  //index=order: first (0),second (1), internal reflection side: sm_ReflectedSide, sm_TransmittedSide
	int plane_at_ir_order[2]; //index=order: first (0),second (1), internal reflection plane: sm_SurfacePlane1, sm_SurfacePlane2
	double T_ir_at_ir_order[2][2];  //1st index=order: first (0),second (1), 2nd index=spin+,spin-, internal transmittion after transmitting substrate and refleced back into substrate
	
	//parameters indexing on spin state
	double T_ir_all[2]; //index=spin+,spin-, T_ir_at_ir_order[0][spin] * T_ir_at_ir_order[1][spin]
	double T_ir_at_ir_order_0[2]; //index=spin+,spin-, internal transmittion at internal reflection order 0 
	
	//polyhedron intersect parameters
	double idtime[2];
	Coords idpoint[2];
	double itime[2];
	Coords ipoint[2];
	int iplane[2];
	int itype[2]; ////type: 1=x plane, 2=x edge, 3=x vertex, 4=on plane, 5=on edge
	
}SimState;

typedef enum SupermirrorLocationEventCodeInternal { 
	//sm->proc->side
	sm_ReflectedSide = 0, 
	sm_TransmittedSide = 1, 
	sm_EdgeSide = 2,
	
	//sm->proc->plane, numerical definition in struct Supermirror
	//must not reorder
	sm_SurfacePlane1 = 0,
	sm_SurfacePlane2 = 1,
	sm_EdgeFrontPlane = 2,
	sm_EdgeBackPlane = 3, 
	sm_EdgeSidePlane1 = 4, 
	sm_EdgeSidePlane2 = 5,
			
	//sm->proc->layer
	sm_MirrorLayer = 0, 
	sm_AbsorberLayer = 1, 
	sm_SubstrateSurfaceLayer = 2,
	sm_SubstrateEdgeLayer = 3,
	sm_SubstrateLayer = 4,
	
	//sm->proc->event
	sm_Reflected = 5,
	sm_Transmitted = 6, 
	sm_Refracted = 7,
	sm_NotRefracted = 8,
	sm_InternalReflection = 9,
	
	//types
	sm_refl_type_refl = 0x1, 
	sm_abs_type_attn = 0x1,
	sm_sub_type_attn = 0x1,
	sm_subsurface_type_refl = 0x1,
	sm_subsurface_type_total_refl = 0x2,
	sm_subsurface_type_refract = 0x4

} SupermirrorLocationEventCodeInternal; 

/***************************/
/* General Tools functions */
/***************************/

int isBlank (char*line);
int to_lower_case(char *in, char **out);
FILE *sm_open_file(char *File, const char *Mode, char *Path, int*file_location_option);

/*****************************/
/* Debugging print functions */
/*****************************/
char*supermirror_flat_inc_prefix(int*supermirror_flat_i_prefix, char*supermirror_flat_s_prefix);
char*supermirror_flat_dec_prefix(int*supermirror_flat_i_prefix, char*supermirror_flat_s_prefix);
void sm_print_state(char*header, SimState*state, Supermirror*sm, double ws_target);
void sm_print_intersect(char*subheader, int num_intersect, double*dtime, Coords*dpoint, double*time, Coords*point, int*plane, int*type);
void sm_print_refl(char *event, SimState*state, Supermirror*sm);
void sm_print_error(SimState*state, Supermirror *sm, char*topic, char*error_message, double ws_target, int outcome);

/*********************************/
/* Internal initialise functions */
/*********************************/

int set_supermirror_flat_geometry (

		double length, //m supermirror length projection along z-axis
		double thickness_in_mm, //mm
		Coords side_edge_1_normal, Coords side_edge_1_point, 
		Coords side_edge_2_normal, Coords side_edge_2_point, 
		char*initial_placement_at_origin,	//"TopFrontEdge","FrontSubstrateCentre","BottomFrontEdge"
		char*tilt_y_axis_location, 	//"TopFrontEdge","TopMirrorCentre","TopBackEdge"
									//"FrontSubstrateCentre","SubstrateCentre","BackSubstrateCentre", 
									//"BottomFrontEdge","BottomMirrorCentre","BottomBackEdge"
		double tilt_about_y_first_in_degree,  //first, tile about y_axis at selected location 
		Coords translation_second,  //second, translate reference point 
		double rot_about_z_third_in_degree, //third, rotate about global z-axis 
		Polyhedron*geo, CoordOp*co);
int load_substrate_material_parameters(char*substrate_material_name, double*data);
int set_substrate_material_parameters(char*substrate_material_name, double substrate_L_abs, double substrate_L_inc, double substrate_SLD, SubstrateParameters*sub, SubstrateSurfaceParameters*subsurface);
int load_mirror_material_parameters(char*mirror_material_name, double*data);
int set_mirror_material_parameters(char*mirror_material_name, double*mirror_material_spec, double mirror_m, ReflectionParameters*mir, SubstrateSurfaceParameters*subsurface);
int load_absorber_material_parameters(char*substrate_material_name, double*data);
int set_absorber_material_parameters(char*absorber_material_name, double absorber_L_abs, double absorber_L_inc, double absorber_thickness_in_micron, AbsorberParameters*abs);
void set_process_parameters(int is_tracking, SupermirrorProcess *proc);
void set_location_text(Supermirror*sm, char*name, int side, int plane, int layer, char*location);
void set_location(SimState *state, Supermirror*sm);
void set_event(SimState *state, Supermirror*sm, int event_code); 

/****************************/
/* Neutron record functions */
/****************************/
int add_neutron_record(double w, double t, Coords p, Coords v, Coords s, SupermirrorProcess*proc);
int empty_neutron_record(SupermirrorProcess*proc);
void free_neutron_record(SupermirrorProcess*proc);

/*******************************/
/* Ray-Tracing Tools functions */
/*******************************/

void sm_initialise_state(SimState*state);

//reflection & transmission at mirrors and at substrate surface
double sm_calc_Rm(double q, double Qc, double R0, double alpha, double m, double W, double beta); 
void sm_get_Rm_at_plane (ReflectionParameters*mir, double vn_len, double *R_plus, double *R_minus);
int sm_reflect_or_transmit_at_mirror (SimState*state, Supermirror*sm, double ws_target, int out_is_Reflected_or_Transmitted); 

void sm_get_Rs (SubstrateSurfaceParameters*subsurface, double vn_len, double *Rs);
int sm_reflect_or_transmit_at_substrate_surface (SimState*state, Supermirror*sm, double ws_target, int out_is_Reflected_or_Transmitted);

//attenuation and transmission at absorbers and at substrate
double sm_get_L_attn_abs_at_plane (AbsorberParameters*abs, double v_len);
void sm_get_prop_abs_at_plane (AbsorberParameters*abs, double v_len, double vn_len, double*L_attn_abs_at_plane, double*t_prop_abs_at_plane, double*T_prop_abs_at_plane); 
int sm_transmit_through_absorber_at_plane (SimState*state, Supermirror*sm, double ws_target);

double sm_get_L_attn_sub (SubstrateParameters*sub, double v_len);
int sm_transmit_through_substrate (SimState*state, Supermirror*sm, double ws_target); 

//refraction going into substrate and going out of supermirror
void sm_get_inward_refracted_v (SubstrateSurfaceParameters*subsurface, Coords v_in, Coords fn, Coords *v_out);
int sm_refract_inward_at_substrate_surface (SimState*state, Supermirror*sm);
void sm_get_outward_refracted_v (SubstrateSurfaceParameters*subsurface, Coords v_in, Coords fn, Coords *v_out);
int sm_refract_outward_at_mirror (SimState*state, Supermirror*sm);

// Into substrate layer
void sm_enter_substrate_layer(SimState*state, Supermirror*sm, int event);

//internal reflection
void sm_set_internal_reflection_parameters (SimState*state, Supermirror*sm, double ws_target);
int sm_internal_reflection_w (SimState*state, int n, double*w_out);

/************************/
/* Trajectory functions */
/************************/

int sm_external_intersect(SimState *state, Supermirror *sm, double ws_target); 
int sm_internal_intersect(SimState *state, Supermirror *sm, double ws_target); 
int sm_internal_reflections(SimState *state, Supermirror *sm, double ws_target); 

/***************************/
/* General Tools functions */
/***************************/
/************************************************************************************/
/* Convert up to CHAR_BUF_LENGTH of characters in char array "in" to lower case     */
/* To save the converted string to "out", provide a pointer to char array "out"     */
/* make sure "out" has a minimum size of CHAR_BUF_LENGTH                            */
/* If pointer "out" = 0, the converted string will be saved back in char array "in" */
/* returns the total number of characters in the char array                         */
/************************************************************************************/
int isBlank (char*line)
{
  char * ch;
  int is_blank = -1;

  // Iterate through each character.
  for (ch = line; *ch != '\0'; ++ch)
  {
    if (!isspace(*ch))
    {
      // Found a non-whitespace character.
      is_blank = 0;
      break;
    }
  }

  return is_blank;
}

int to_lower_case(char *in, char **out) 
{
	int n = 0;
	if (in) {
		n=strlen(in);
		if (n > 0) {
			int i; 
			if (out) {
				*out=(char*)calloc(n+1,sizeof(char));
				for(i=0; i<n; i++) (*out)[i]= in[i]>='A'&&in[i]<='Z' ? in[i]|0x60 : in[i]; //convert to lower case
				if (i < CHAR_BUF_LENGTH) (*out)[i] = 0; //zero terminate the out string. 
			}
			else {
				for(i=0; i<n; i++) in[i]= in[i]>='A'&&in[i]<='Z' ? in[i]|0x60 : in[i]; //convert to lower case
			}
		}
	}
	return n;
}

/********************************************************************************/
/* Function modified from Open_file in read_table-lib.c							*/
/* FILE *sm_open_file(char *name, char *Mode, char *path)						*/
/*   ACTION: search for a file and open it. Optionally return the opened path.	*/
/*   input   name:  file name from which table should be extracted				*/
/*           mode: "r", "w", "a" or any valid fopen mode						*/
/*   output  path:  NULL or a pointer to allocated chars PATH_MAX long			*/
/*   file_location_option: stores the option number + 1 when a file is found.   */
/*           user begin with assigning 0 to an integer and pass its pointer in. */
/*           The function will try the next available options until a file is   */
/*           found, or return NULL. This is useful if the data needed is not in */
/*           the file found and need to find the next file to check.   			*/
/*   return  initialized file handle or NULL in case of error					*/
/********************************************************************************/
FILE *sm_open_file(char *File, const char *Mode, char *Path, int*file_location_option) {

/* portability - for VC on Windows */
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

	char path[PATH_MAX];
	FILE *hfile = NULL;
	
	if (!File || !file_location_option) return(NULL);
	if (File[0]=='\0' || *file_location_option<0) return(NULL);
	if (!strcmp(File,"NULL") || !strcmp(File,"0")) return(NULL);
	
	// search in current or full path
	switch(*file_location_option) {
		case 0: 
			++(*file_location_option);
			strncpy(path, File, PATH_MAX);
			hfile = fopen(path, Mode);
			if (hfile) break;
		case 1:
			++(*file_location_option);
			char dir[PATH_MAX];
			if (instrument_source[0] != '\0' && strlen(instrument_source)) {
				// search in instrument source location
				char *path_pos   = NULL;
				// extract path: searches for last file separator 
				path_pos    = strrchr(instrument_source, MC_PATHSEP_C);  // last PATHSEP 
				if (path_pos) {
					long path_length = path_pos +1 - instrument_source;  // from start to path+sep 
					if (path_length) {
						strncpy(dir, instrument_source, path_length);
						dir[path_length] = '\0';
						snprintf(path, PATH_MAX, "%s%c%s", dir, MC_PATHSEP_C, File);
						hfile = fopen(path, Mode);
					}
				}
			}
			if (hfile) break;
		case 2: 
			++(*file_location_option);
			if (instrument_exe[0] != '\0' && strlen(instrument_exe)) {
				// search in PWD instrument executable location 
				char *path_pos   = NULL;
				// extract path: searches for last file separator 
				path_pos = strrchr(instrument_exe, MC_PATHSEP_C);  // last PATHSEP 
				if (path_pos) {
					long path_length = path_pos +1 - instrument_exe;  // from start to path+sep 
					if (path_length) {
						strncpy(dir, instrument_exe, path_length);
						dir[path_length] = '\0';
						snprintf(path, PATH_MAX, "%s%c%s", dir, MC_PATHSEP_C, File);
						hfile = fopen(path, Mode);
					}
				}
			}
			if (hfile) break;
		case 3: 
			++(*file_location_option);
			// search in HOME or . 
			strcpy(dir, getenv("HOME") ? getenv("HOME") : ".");
			snprintf(path, PATH_MAX, "%s%c%s", dir, MC_PATHSEP_C, File);
			hfile = fopen(path, Mode);
			if (hfile) break;
		case 4: 
			++(*file_location_option);
			// search in MCSTAS/data 
			strcpy(dir, getenv(FLAVOR_UPPER) ? getenv(FLAVOR_UPPER) : MCSTAS);
			snprintf(path, PATH_MAX, "%s%c%s%c%s", dir, MC_PATHSEP_C, "data", MC_PATHSEP_C, File);
			hfile = fopen(path, Mode);
			if (hfile) break;
		case 5: 
			++(*file_location_option);
			// search in MVCSTAS/contrib 
			strcpy(dir, getenv(FLAVOR_UPPER) ? getenv(FLAVOR_UPPER) : MCSTAS);
			snprintf(path, PATH_MAX, "%s%c%s%c%s", dir, MC_PATHSEP_C, "contrib", MC_PATHSEP_C, File);
			hfile = fopen(path, Mode);
			if (hfile) break;
		case 6: 
			++(*file_location_option);
			// search in MVCSTAS/share 
			strcpy(dir, getenv(FLAVOR_UPPER) ? getenv(FLAVOR_UPPER) : MCSTAS);
			snprintf(path, PATH_MAX, "%s%c%s%c%s", dir, MC_PATHSEP_C, "share", MC_PATHSEP_C, File);
			hfile = fopen(path, Mode);
			if (hfile) break;
		default: 
			fprintf(stderr, "sm_Error: Could not open input file '%s' (sm_open_file)\n", File);
			return (NULL);
	}
	if (Path) strncpy(Path, path, PATH_MAX);
	return(hfile);

} // end sm_open_file 

/*****************************/
/* Debugging print functions */
/*****************************/
char* supermirror_flat_inc_prefix(int *supermirror_flat_i_prefix, char*supermirror_flat_s_prefix) {
	supermirror_flat_s_prefix[*supermirror_flat_i_prefix]='\t';
	++(*supermirror_flat_i_prefix);
	supermirror_flat_s_prefix[*supermirror_flat_i_prefix]=0;
	return supermirror_flat_s_prefix;
}
char* supermirror_flat_dec_prefix(int *supermirror_flat_i_prefix, char*supermirror_flat_s_prefix) {
	if (*supermirror_flat_i_prefix == 0) return supermirror_flat_s_prefix;
	--(*supermirror_flat_i_prefix);
	supermirror_flat_s_prefix[*supermirror_flat_i_prefix]=0;
	return supermirror_flat_s_prefix;
}
void sm_print_error(SimState*state, Supermirror *sm, char*topic, char*error_message, double ws_target, int outcome) {

	#ifdef USE_MPI
	//Only print mpi root node's state
	if (mpi_node_rank != mpi_node_root) 
		return;
	#endif
	
	char header[CHAR_BUF_LENGTH];
	switch(outcome) {
		case sm_Error: //something's wrong
		{
			set_event(state, sm, sm_Error);
			if ((sm->proc).is_tracking) {
				add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
			}
			
			sprintf(header, "%s: ERROR. Return sm_Error.", topic);
			printf("%s\n", header);
			if (strlen(error_message) > 0) {
				printf("%s\n", error_message);
			}
			sm_print_state (header, state, sm, ws_target);

			break; 
		}
		default: 
		{
			sprintf(header, "%s: ERROR outcome=%d is out of range. Return sm_Error.", topic, outcome);
			sm_print_state (header, state, sm, ws_target);
			printf("%s \n", header);

			break; 
		}
	}
	return;

}
void sm_print_state(char*subheader, SimState*state, Supermirror*sm, double ws_target) { 

	#ifdef USE_MPI
	//Only print mpi root node's state
	if (mpi_node_rank != mpi_node_root) 
		return;
	#endif

	int supermirror_flat_i_prefix = 0; 
	char supermirror_flat_s_prefix[CHAR_BUF_LENGTH]; 

	int i;

	printf("%s sm_print_state: %s\n", supermirror_flat_s_prefix, subheader);
	supermirror_flat_inc_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
	
	printf("%s neutron state parameters\n", supermirror_flat_s_prefix);
	supermirror_flat_inc_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
		if (state->plane==I_INDETERMINED) {
			printf("%s location/event=indetermined\n", supermirror_flat_s_prefix);
		}
		else {
			printf("%s location/event=%s/%s\n", supermirror_flat_s_prefix, 
					state->location, state->event);
		}
		printf("%s w, ws+,ws-,ws,ws_target = %5e, (% 5f,% 5f),% 5f,% 5f\n", supermirror_flat_s_prefix,
				state->w, state->ws[0], state->ws[1], state->ws[0]+state->ws[1], ws_target); 
		printf("%s t,p,v:|v|,s3-dot-fa:s:s2 = %6f,(%6f,%6f,%6f),(%6f,%6f,%6f):%6f,(%6f,%6f,%6f)dot(%6f,%6f,%6f):%6f:(%6f,%6f)\n", supermirror_flat_s_prefix,
				state->t, state->p.x, state->p.y, state->p.z, state->v.x, state->v.y, state->v.z, coords_len(state->v), 
				state->s.x, state->s.y, state->s.z, ((sm->co).fa).x, ((sm->co).fa).y, ((sm->co).fa).z, 
				coords_sp((sm->co).fa, state->s), (1+coords_sp((sm->co).fa, state->s))/2, (1-coords_sp((sm->co).fa, state->s))/2);
	supermirror_flat_dec_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);

	printf("%s intersect parameters\n", supermirror_flat_s_prefix);
	supermirror_flat_inc_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
		printf("%s n_mirror_intersected=%d\n", supermirror_flat_s_prefix, state->n_mirror_intersect);
		if (state->plane==I_INDETERMINED) {
			printf(	"%s side =indetermined\n"
					"%s plane=indetermined\n"
					"%s layer=indetermined\n", 
					supermirror_flat_s_prefix, supermirror_flat_s_prefix, supermirror_flat_s_prefix);
		}
		else {
			printf(	"%s side =%s(%d)\n"
					"%s plane=%s(%d)\n"
					"%s layer=%s(%d)\n",  
					supermirror_flat_s_prefix, ((sm->proc).side)[state->side], state->side, 
					supermirror_flat_s_prefix, ((sm->proc).plane)[state->plane], state->plane, 
					supermirror_flat_s_prefix, ((sm->proc).layer)[state->layer], state->layer);
		}
		if (state->fn.x == F_INDETERMINED || state->fn.x == D_INDETERMINED)
			printf("%s fn = indetermined\n", supermirror_flat_s_prefix);
		else
			printf("%s fn = (%6f,%6f,%6f)\n", supermirror_flat_s_prefix,
					state->fn.x, state->fn.y, state->fn.z); 
		if (state->last_time == F_INDETERMINED || state->last_time == D_INDETERMINED)
			printf("%s last t,p=indetermined\n", supermirror_flat_s_prefix);
		else
			printf("%s last t,p=%6f,(%6f,%6f,%6f)\n", supermirror_flat_s_prefix, state->last_time, state->last_point.x, state->last_point.y, state->last_point.z);
	supermirror_flat_dec_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);

	printf("%s parameters indexing on plane number\n", supermirror_flat_s_prefix);
	supermirror_flat_inc_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
		printf("%s ir_order_at_plane = (", supermirror_flat_s_prefix);
		for (i = 0; i < SM_Num_Mirror_Planes; i++) {
			if (state->ir_order_at_plane[i] == I_INDETERMINED)
				printf("indetermined");
			else
				printf("%d",state->ir_order_at_plane[i]);
			if (i == 0) printf(",");
			else printf(")\n");
		}
		if ((state->v_exit_at_plane[0]).x == F_INDETERMINED || (state->v_exit_at_plane[0]).x == D_INDETERMINED)
			printf("%s v_exit_at_plane = indetermined\n", supermirror_flat_s_prefix);
		else
			printf("%s v_exit_at_plane[0] = (%6f,%6f,%6f), v_exit_at_plane[1] = (%6f,%6f,%6f)\n", supermirror_flat_s_prefix,
					(state->v_exit_at_plane[0]).x, (state->v_exit_at_plane[0]).y, (state->v_exit_at_plane[0]).z, 
					(state->v_exit_at_plane[1]).x, (state->v_exit_at_plane[1]).y, (state->v_exit_at_plane[1]).z); 
		if ((state->v_ir_at_plane[0]).x == F_INDETERMINED || (state->v_ir_at_plane[0]).x == D_INDETERMINED)
			printf("%s v_ir_at_plane = indetermined\n", supermirror_flat_s_prefix);
		else
			printf("%s v_ir_at_plane[0] = (%6f,%6f,%6f), v_ir_at_plane[1] = (%6f,%6f,%6f)\n", supermirror_flat_s_prefix,
					(state->v_ir_at_plane[0]).x, (state->v_ir_at_plane[0]).y, (state->v_ir_at_plane[0]).z, 
					(state->v_ir_at_plane[1]).x, (state->v_ir_at_plane[1]).y, (state->v_ir_at_plane[1]).z); 
		if (state->Rm_at_plane[0][0] == F_INDETERMINED || state->Rm_at_plane[0][0] == D_INDETERMINED) 
			printf("%s Rm0+,Rm0- = indetermined, ", supermirror_flat_s_prefix);
		else
			printf("%s Rm0+,Rm0- = % 5f,% 5f, ", supermirror_flat_s_prefix, state->Rm_at_plane[0][0], state->Rm_at_plane[0][1]);
		if (state->Rm_at_plane[1][0] == F_INDETERMINED || state->Rm_at_plane[1][0] == D_INDETERMINED) 
			printf("Rm1+,Rm1- = indetermined\n");
		else
			printf("Rm1+,Rm1- = % 5f,% 5f\n", state->Rm_at_plane[1][0], state->Rm_at_plane[1][1]);
		if (state->L_attn_abs_at_plane[0] == F_INDETERMINED || state->L_attn_abs_at_plane[0] == D_INDETERMINED) 
			printf("%s L_attn_abs_at_plane[0] = indetermined, ", supermirror_flat_s_prefix);
		else
			printf("%s L_attn_abs_at_plane[0] = % 5f, ", supermirror_flat_s_prefix, state->L_attn_abs_at_plane[0]);
		if (state->L_attn_abs_at_plane[1] == F_INDETERMINED || state->L_attn_abs_at_plane[1] == D_INDETERMINED) 
			printf("L_attn_abs_at_plane[1] = indetermined\n");
		else
			printf("L_attn_abs_at_plane[1] = % 5f\n", state->L_attn_abs_at_plane[1]);
		if (state->t_prop_abs_at_plane[0] == F_INDETERMINED || state->t_prop_abs_at_plane[0] == D_INDETERMINED) 
			printf("%s t_prop_abs_at_plane[0] = indetermined, ", supermirror_flat_s_prefix);
		else
			printf("%s t_prop_abs_at_plane[0] = % 5f, ", supermirror_flat_s_prefix, state->t_prop_abs_at_plane[0]);
		if (state->t_prop_abs_at_plane[1] == F_INDETERMINED || state->t_prop_abs_at_plane[1] == D_INDETERMINED) 
			printf("t_prop_abs_at_plane[1] = indetermined\n");
		else
			printf("t_prop_abs_at_plane[1] = % 5f\n", state->t_prop_abs_at_plane[1]);
		if (state->T_prop_abs_at_plane[0] == F_INDETERMINED || state->T_prop_abs_at_plane[0] == D_INDETERMINED) 
			printf("%s T_prop_abs_at_plane[0] = indetermined, ", supermirror_flat_s_prefix);
		else
			printf("%s T_prop_abs_at_plane[0] = % 5f, ", supermirror_flat_s_prefix, state->T_prop_abs_at_plane[0]);
		if (state->T_prop_abs_at_plane[1] == F_INDETERMINED || state->T_prop_abs_at_plane[1] == D_INDETERMINED) 
			printf("T_prop_abs_at_plane[1] = indetermined\n");
		else
			printf("T_prop_abs_at_plane[1] = % 5f\n", state->T_prop_abs_at_plane[1]);
		if (state->Rs_at_plane[0] == F_INDETERMINED || state->Rs_at_plane[0] == D_INDETERMINED) 
			printf("%s Rs_at_plane[0] = indetermined, ", supermirror_flat_s_prefix);
		else
			printf("%s Rs_at_plane[0] = % 5f, ", supermirror_flat_s_prefix, state->Rs_at_plane[0]);
		if (state->Rs_at_plane[1] == F_INDETERMINED || state->Rs_at_plane[1] == D_INDETERMINED) 
			printf("Rs_at_plane[1] = indetermined\n");
		else
			printf("Rs_at_plane[1] = % 5f\n", state->Rs_at_plane[1]);
		if (state->L_attn_sub == F_INDETERMINED || state->L_attn_sub == D_INDETERMINED) 
			printf("%s L_attn_sub = indetermined\n", supermirror_flat_s_prefix);
		else
			printf("%s L_attn_sub = % 5f\n", supermirror_flat_s_prefix, state->L_attn_sub);
		if (state->t_prop_sub == F_INDETERMINED || state->t_prop_sub == D_INDETERMINED) 
			printf("%s t_prop_sub = indetermined\n", supermirror_flat_s_prefix);
		else
			printf("%s t_prop_sub = % 5f\n", supermirror_flat_s_prefix, state->t_prop_sub);
		if (state->T_prop_sub == F_INDETERMINED || state->T_prop_sub == D_INDETERMINED) 
			printf("%s T_prop_sub = indetermined\n", supermirror_flat_s_prefix);
		else
			printf("%s T_prop_sub = % 5f\n", supermirror_flat_s_prefix, state->T_prop_sub);
	supermirror_flat_dec_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
	
	printf("%s parameters indexing on the order of intersecting reflecting surfaces\n", supermirror_flat_s_prefix);
	supermirror_flat_inc_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
		printf("%s side,plane at ir order = (", supermirror_flat_s_prefix);
		for (i = 0; i < SM_Num_Mirror_Planes; i++) {
			if (state->side_at_ir_order[i] == I_INDETERMINED) 
				printf("Indetermined"); 
			else 
				printf("%s ",(sm->proc).side[state->side_at_ir_order[i]]);
			if (i == 0) printf(",");
			else printf("), (");
		}
		for (i = 0; i < SM_Num_Mirror_Planes; i++) {
			if (state->plane_at_ir_order[i] == I_INDETERMINED)
				printf("indetermined");
			else
				printf("%s ",(sm->proc).plane[state->plane_at_ir_order[i]]);
			if (i == 0) printf(",");
			else printf(")\n");
		}
		if (state->T_ir_at_ir_order[0][0] == F_INDETERMINED || state->T_ir_at_ir_order[0][0] == D_INDETERMINED) {
			printf("%s T_ir_at_ir_order[0][+/-] = indetermined\n", supermirror_flat_s_prefix);
			printf("%s T_ir_at_ir_order[1][+/-] = indetermined\n", supermirror_flat_s_prefix);
		}
		else {
			printf("%s T_ir_at_ir_order[0][+/-] = (% 5f,% 5f)\n", supermirror_flat_s_prefix,
					state->T_ir_at_ir_order[0][0], state->T_ir_at_ir_order[0][1]);
			printf("%s T_ir_at_ir_order[1][+/-] = (% 5f,% 5f)\n", supermirror_flat_s_prefix,
					state->T_ir_at_ir_order[1][0], state->T_ir_at_ir_order[1][1]);
		}
	supermirror_flat_dec_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);

	printf("%s parameters indexing on spin state\n", supermirror_flat_s_prefix);
	supermirror_flat_inc_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
		if (state->T_ir_all[0] == F_INDETERMINED || state->T_ir_all[0] == D_INDETERMINED) {
			printf("%s T_ir_all[+/-] = indetermined\n", supermirror_flat_s_prefix);
			printf("%s T_ir_at_ir_order_0[+/-] = indetermined\n", supermirror_flat_s_prefix);
		}
		else {
			printf("%s T_ir_all[+/-] = (% 5f,% 5f %)\n", supermirror_flat_s_prefix,
				state->T_ir_all[0], state->T_ir_all[1]);
			printf("%s T_ir_at_ir_order_0[+/-] = (% 5f,% 5f %)\n", supermirror_flat_s_prefix,
				state->T_ir_at_ir_order_0[1], state->T_ir_at_ir_order_0[1]);
		}
	supermirror_flat_dec_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
	
	supermirror_flat_dec_prefix(&supermirror_flat_i_prefix, supermirror_flat_s_prefix);
	
}
void sm_print_intersect(char*event, int num_intersect, double*dtime, Coords*dpoint, double*time, Coords*point, int*plane, int*type) {

	if (num_intersect > 0) {
		printf("sm_print_intersect: %s\n", event);
		for (int i = 0; i < num_intersect; i++) {
			printf("n.intersect=%d/%d", i, num_intersect);
			if (plane) printf(", plane=%d ", plane[i]);
			if (dtime) printf(", dt=% 6f ", dtime[i]);
			if (dpoint) printf(", dp=(% 6f,% 6f,% 6f) ", (dpoint[i]).x, (dpoint[i]).y, (dpoint[i]).z);
			if (time) printf(", t=% 6f ", time[i]);
			if (point) printf(", p=(% 6f,% 6f,% 6f) ", (point[i]).x, (point[i]).y, (point[i]).z);
			if (type) printf(", type=%d ", type[i]); 
			printf("\n"); 
		}
	}
}
void sm_print_refl(char *event, SimState*state, Supermirror*sm) {

	#ifdef USE_MPI
	//Only save mpi root node's state
	if (mpi_node_rank != mpi_node_root) 
		return;
	#endif

	printf("%s mirror=%d, Rm_at_plane+:%s=% 5f Rm_at_plane-:%s=% 5f, subSLD:%s=%5e\n", 
			event,
			state->plane, 
			((sm->mat).mir[state->plane][0]).name, state->Rm_at_plane[state->plane][0],
			((sm->mat).mir[state->plane][1]).name, state->Rm_at_plane[state->plane][1],
			(sm->mat).sub.name, (sm->mat).sub.SLD);
}

/*********************************/
/* Internal initialise functions */
/*********************************/

int set_supermirror_flat_geometry (

		double length, //m supermirror length projection along z-axis
		double thickness_in_mm, //mm
		Coords side_edge_1_normal, Coords side_edge_1_point, 
		Coords side_edge_2_normal, Coords side_edge_2_point, 
		char*initial_placement_at_origin,	//"TopFrontEdge","FrontSubstrateCentre","BottomFrontEdge"
		char*tilt_y_axis_location, 	//"TopFrontEdge","TopMirrorCentre","TopBackEdge"
									//"FrontSubstrateCentre","SubstrateCentre","BackSubstrateCentre", 
									//"BottomFrontEdge","BottomMirrorCentre","BottomBackEdge"
		double tilt_about_y_first_in_degree,  //first, tile about y_axis at selected location 
		Coords translation_second,  //second, translate reference point 
		double rot_about_z_third_in_degree, //third, rotate about global z-axis 
		Polyhedron*geo, CoordOp*co) 
{	
	if (geo == 0) {
		MPI_MASTER(printf("set_supermirror_flat_geometry: error, pointer to Polyhedron variable \"geo\" must not be NULL.\n");) 
		return 0;
	}
	if (co == 0) {
		MPI_MASTER(printf("set_supermirror_flat_geometry: error, pointer to CoordOp variable \"co\" must not be NULL.\n");)
		return 0;
	}
	
////////////////////////////////////////////////////////////////////////
//Follow-on coding: supermirror with no thickness, set as error for now
////////////////////////////////////////////////////////////////////////
	if (thickness_in_mm <= DBL_EPSILON) {
		MPI_MASTER(printf("set_supermirror_flat_geometry: error, thickness_in_mm must have thickness.\n")); 
		return 0;
	}
	if (initial_placement_at_origin == 0) {
		MPI_MASTER(printf("set_supermirror_flat_geometry: error, pointer to initial_placement_at_origin name must not be NULL.\n")); 
		return 0;
	}
	if (tilt_y_axis_location == 0) {
		MPI_MASTER(printf("set_supermirror_flat_geometry: error, pointer to tilt_y_axis_location name must not be NULL.\n")); 
		return 0;
	}
	
	int i, j;
	char *lc = 0;
	
	Coords plane_normal[6], plane_point[6];
	plane_normal[0] = coords_set(1,0,0);
	plane_normal[1] = coords_set(-1,0,0);
	plane_normal[2] = coords_set(0,0,-1);
	plane_normal[3] = coords_set(0,0,1);
	plane_normal[4] = side_edge_1_normal; //+y side
	plane_normal[5] = side_edge_2_normal; //-y side
	for (i = 0; i < 6; i++) 
		coords_norm(&(plane_normal[i]));
	
	Coords fsc_wrt_origin; // front substrate-centre location w.r.t. device initial_placement_at_origin  

	if (to_lower_case(initial_placement_at_origin, &lc) == 0) {
		return 0;
	}
	if (strcmp(lc,"frontsubstratecentre") == 0 || strcmp(lc,"frontsubstratecenter") == 0 ) {
		plane_point[0] = coords_set( thickness_in_mm/1000/2, 0, 0);
		plane_point[1] = coords_set(-thickness_in_mm/1000/2, 0, 0);
		fsc_wrt_origin = coords_set(0, 0, 0); 
	}
	else if (strcmp(lc,"topfrontedgecentre") == 0 || strcmp(lc,"topfrontedgecenter") == 0) {
		plane_point[0] = coords_set(0,0,0);
		plane_point[1] = coords_set(-thickness_in_mm/1000, 0, 0);
		fsc_wrt_origin = coords_set(-thickness_in_mm/1000/2, 0, 0);
	}
	else if (strcmp(lc,"bottomfrontedgecentre") == 0 || strcmp(lc,"bottomfrontedgecenter") == 0) {
		plane_point[0] = coords_set(thickness_in_mm/1000, 0, 0);
		plane_point[1] = coords_set(0,0,0);
		fsc_wrt_origin = coords_set(thickness_in_mm/1000/2, 0, 0);
	}
	else {
		MPI_MASTER(printf("set_supermirror_flat_geometry: error initial_placement_at_origin=%s - "
							"must be \"FrontSubstrateCentre\", \"TopFrontEdgeCentre\" or \"BottomFrontEdgeCentre\" (not case-sensitive)\n", 
							initial_placement_at_origin);
		)
		if (lc != 0) {
			free(lc);
			lc = 0;
		}
		return 0;
	}
	if (lc != 0) {
		free(lc);
		lc = 0;
	}
	plane_point[2] = coords_set(0,0,0);
	plane_point[3] = coords_set(0,0,length);
	plane_point[4] = side_edge_1_point; //+y side
	plane_point[5] = side_edge_2_point; //-y side
		
	//normal vector should have length
	for (i = 0; i < 6; i++) {
		if (coords_len(plane_normal[i]) < DBL_EPSILON) {
			MPI_MASTER(printf("set_supermirror_flat_geometry: ERROR plane %d normal vector magnitude too small, normal=(% 6f,% 6f,% 6f), point=(% 6f,% 6f,% 6f)\n", i, 
				plane_point[i].x, plane_point[i].y, plane_point[i].z, 
				plane_normal[i].x, plane_normal[i].y, plane_normal[i].z);) 
			return 0;
		}
	}
	
	j = form_polyhedron(6, plane_normal, plane_point, geo);
	if (j < 6) { 
		MPI_MASTER(printf("set_supermirror_flat_geometry: ERROR number of planes expected = 6, number of plane in polyhedron = %d\n", j);
		) 
		empty_polyhedron(geo);
		return 0;
	}
	
	Coords field_axis = coords_set(0,1,0); 

	Coords tilt_y_axis_wrt_fsc; //tilt y_axis location w.r.t. device initial_placement_at_origin = tilt axis wrt fsc + fsc wrt initial_placement_at_origin
	to_lower_case(tilt_y_axis_location, &lc);
	if      (strcmp(lc,"topfrontedge") == 0) 			tilt_y_axis_wrt_fsc = coords_set( thickness_in_mm/1000/2, 0, 0       );
	else if (strcmp(lc,"fronttopedge") == 0) 			tilt_y_axis_wrt_fsc = coords_set( thickness_in_mm/1000/2, 0, 0       );
	else if (strcmp(lc,"frontsubstratecentre") == 0)	tilt_y_axis_wrt_fsc = coords_set( 0,                                0, 0       );
	else if (strcmp(lc,"frontsubstratecenter") == 0)	tilt_y_axis_wrt_fsc = coords_set( 0,                                0, 0       );
	else if (strcmp(lc,"substratefrontcentre") == 0)	tilt_y_axis_wrt_fsc = coords_set( 0,                                0, 0       );
	else if (strcmp(lc,"substratefrontcenter") == 0)	tilt_y_axis_wrt_fsc = coords_set( 0,                                0, 0       );
	else if (strcmp(lc,"substratecentrefront") == 0)	tilt_y_axis_wrt_fsc = coords_set( 0,                                0, 0       );
	else if (strcmp(lc,"substratecenterfront") == 0)	tilt_y_axis_wrt_fsc = coords_set( 0,                                0, 0       );
	else if (strcmp(lc,"bottomfrontedge") == 0) 		tilt_y_axis_wrt_fsc = coords_set(-thickness_in_mm/1000/2, 0, 0       );
	else if (strcmp(lc,"frontbottomedge") == 0) 		tilt_y_axis_wrt_fsc = coords_set(-thickness_in_mm/1000/2, 0, 0       );
	else if (strcmp(lc,"topmirrorcentre") == 0) 		tilt_y_axis_wrt_fsc = coords_set( thickness_in_mm/1000/2, 0, length/2);
	else if (strcmp(lc,"topmirrorcenter") == 0) 		tilt_y_axis_wrt_fsc = coords_set( thickness_in_mm/1000/2, 0, length/2);
	else if (strcmp(lc,"substratecentre") == 0)			tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length/2);
	else if (strcmp(lc,"substratecenter") == 0)			tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length/2);
	else if (strcmp(lc,"bottommirrorcentre") == 0) 		tilt_y_axis_wrt_fsc = coords_set(-thickness_in_mm/1000/2, 0, length/2);
	else if (strcmp(lc,"bottommirrorcenter") == 0) 		tilt_y_axis_wrt_fsc = coords_set(-thickness_in_mm/1000/2, 0, length/2);
	else if (strcmp(lc,"topbackedge") == 0) 			tilt_y_axis_wrt_fsc = coords_set( thickness_in_mm/1000/2, 0, length  );
	else if (strcmp(lc,"backtopedge") == 0) 			tilt_y_axis_wrt_fsc = coords_set( thickness_in_mm/1000/2, 0, length  );
	else if (strcmp(lc,"backsubstratecentre") == 0)		tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length  );
	else if (strcmp(lc,"backsubstratecenter") == 0)		tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length  );
	else if (strcmp(lc,"substratebackcentre") == 0)		tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length  );
	else if (strcmp(lc,"substratebackcenter") == 0)		tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length  );
	else if (strcmp(lc,"substratecentreback") == 0)		tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length  );
	else if (strcmp(lc,"substratecenterback") == 0)		tilt_y_axis_wrt_fsc = coords_set( 0,                                0, length  );
	else if (strcmp(lc,"bottombackedge") == 0) 			tilt_y_axis_wrt_fsc = coords_set(-thickness_in_mm/1000/2, 0, length  );
	else if (strcmp(lc,"backbottomedge") == 0) 			tilt_y_axis_wrt_fsc = coords_set(-thickness_in_mm/1000/2, 0, length  );
	else {
		MPI_MASTER(printf(	"set_supermirror_flat_geometry: ERROR tilt_y_axis_location=\"%s\" must be one of the following\n"
					"                               \"TopFrontEdge\",\"TopMirrorCentre\",\"TopBackEdge\"\n"
					"                               \"FrontSubstrateCentre\",\"SubstrateCentre\",\"BackSubstrateCentre\"\n" 
					"                               \"BottomFrontEdge\",\"BottomMirrorCentre\",\"BottomBackEdge\" (case-insensitive)\n"
					);)
		if (lc != 0) {
			free(lc);
			lc = 0;
		}
		return 0;
	}
	if (lc != 0) {
		free(lc);
		lc = 0;
	}

//	//Keep for reference, code replaced by the polyhedron function calls below
//	
//	rot_set_rotation(co->ry,-tilt_about_y_first_in_degree*DEG2RAD,0,0);
//	co->tr = translation_second;
//	rot_set_rotation(co->rz,0,0,-rot_about_z_third_in_degree*DEG2RAD);
//	
//	for (i = 0; i < 6; i++) {
//		geo->fn[i] = rot_apply(co->ry, geo->fn[i]);
//		geo->fn[i] = rot_apply(co->rz, geo->fn[i]);
//		geo->fp[i] = rot_apply(co->ry, geo->fp[i]);
//		geo->fp[i] = coords_add(geo->fp[i], co->tr);
//		geo->fp[i] = rot_apply(co->rz, geo->fp[i]);
//	}

	co->ty = coords_add(tilt_y_axis_wrt_fsc, fsc_wrt_origin); //tilt y-axis location w.r.t. device initial_placement_at_origin = tilt axis wrt fsc + fsc wrt initial_placement_at_origin
	co->rty = coords_neg(co->ty);
	translate_polyhedron (geo, co->rty); 
	rotate_polyhedron_about_axis (geo, coords_set(0,1,0), tilt_about_y_first_in_degree, &(co->ry));
	translate_polyhedron (geo, co->ty); 
	translate_polyhedron (geo, translation_second); co->tr = translation_second;
	rotate_polyhedron_about_axis (geo, coords_set(0,0,1), rot_about_z_third_in_degree, &(co->rz));
	
	co->fa = rot_apply(co->ry, field_axis);
	co->fa = rot_apply(co->rz, co->fa);

	rot_transpose(co->ry, co->rry); 
	co->rtr = coords_neg(co->tr);
	rot_transpose(co->rz, co->rrz); 
	
	return 1; //initialisation successful
}

int load_substrate_material_parameters(char*material_name, double*data) 
{
	int found = 0;	
	FILE *file; 
	int file_location_option = 0; 
	char *name_from_function = 0;
	to_lower_case(material_name, &name_from_function); //convert to lower case	
	char name_from_file[CHAR_BUF_LENGTH];
	int n;
	char line[CHAR_BUF_LENGTH]; 
	
	while (file = sm_open_file("Supermirror_substrate_materials.txt", "r", 0, &file_location_option)) {
		while(fgets(line, sizeof(line), file) != NULL) {

			//skip lines that have been commented out
			if (strlen(line) == 0) continue;
			if (line[0] == '#' || line[0] == '\\' || line[0] == '/') continue;
			if (isBlank(line)) continue;			

			n = sscanf(line, " %99[^,],%lf,%lf,%lf", name_from_file, data, data+1, data+2);
			
			to_lower_case(name_from_file, 0); //convert to lower case

			if (n == 4) {
				if (strcmp(name_from_file, name_from_function) == 0) {
					found = 1;
					break;
				}
			}
		}
		fclose(file);	
		if (found) break;
	}
	if (name_from_function != 0) {
		free(name_from_function);
		name_from_function = 0;
	}
	return found;
}

int set_substrate_material_parameters(char*substrate_material_name, double substrate_L_abs, double substrate_L_inc, double substrate_SLD, SubstrateParameters*sub, SubstrateSurfaceParameters*subsurface) 
{

	if (sub == 0 || subsurface == 0) {
		MPI_MASTER(printf("set_substrate_material_parameters: error, pointers to SubstrateParameters & SubstrateSurfaceParameters variables must both not be 0.\n");) 
		return 0;
	}
	
	//start with nothing
	strcpy(sub->name,"empty");
	strcpy(subsurface[0].name,"empty");
	strcpy(subsurface[1].name,"empty");
	sub->L_abs = -1;
	sub->L_inc = -1;
	sub->SLD = 0;
	subsurface[0].Qc_sub = subsurface[1].Qc_sub = 0;
	subsurface[0].delta_n_2 = subsurface[1].delta_n_2 = 0;
	sub->sub_type = 0;
	subsurface[0].subsurface_type = subsurface[1].subsurface_type = 0;
	
	//substrate_material_spec specified?
	if (substrate_L_abs != -1 || substrate_L_inc != -1 || substrate_SLD != 0) {
		strcpy(sub->name,"UserDefine");
		strcpy(subsurface[0].name,"UserDefine");
		strcpy(subsurface[1].name,"UserDefine");
		if (substrate_material_name != 0) {
			if (strlen(substrate_material_name) > 0) {
				strcpy(sub->name, substrate_material_name);
				strcpy(subsurface[0].name,substrate_material_name);
				strcpy(subsurface[1].name,substrate_material_name);
			}
		}
		sub->L_abs = substrate_L_abs;
		sub->L_inc = substrate_L_inc;
		sub->SLD = substrate_SLD;
		subsurface[0].Qc_sub = subsurface[1].Qc_sub = sqrt(16 * M_PI * fabs(sub->SLD));
		subsurface[0].delta_n_2 = subsurface[1].delta_n_2 = AMS * AMS * sub->SLD / M_PI;
	}

	//Look up name from Supermirror_substrate_materials.txt.
	//If found, replace substrate parameters
	if (substrate_material_name != 0)
	if (strlen(substrate_material_name) > 0) {

		int found = 0;
		double d_temp[3];
		MPI_MASTER (
			found = load_substrate_material_parameters(substrate_material_name, d_temp);
		)
		#if USE_MPI
		//broadcast the value of found to all processes
		MPI_Bcast(&found, 1, MPI_INT, mpi_node_root, MPI_COMM_WORLD);
		#endif
		if (found) {
			#if USE_MPI
			//broadcast to all processes
			MPI_Bcast(d_temp, 3, MPI_DOUBLE, mpi_node_root, MPI_COMM_WORLD);
			#endif
			
			sprintf(sub->name,"%s",substrate_material_name);
			sprintf(subsurface[0].name,"%s",substrate_material_name);
			sprintf(subsurface[1].name,"%s",substrate_material_name);
			
			sub->L_abs = d_temp[0];
			sub->L_inc = d_temp[1];
			sub->SLD = d_temp[2];
			subsurface[0].Qc_sub = subsurface[1].Qc_sub = sqrt(16 * M_PI * sub->SLD);
			subsurface[0].delta_n_2 = subsurface[1].delta_n_2 = AMS * AMS * sub->SLD / M_PI;
		}
		else {
			MPI_MASTER (printf("set_substrate_material_parameters: No file \"Supermirror_substrate_materials.txt\" or \"%s\" not on file\n",substrate_material_name);)
			return 0;
		}
	}
	
	//check if parameters entered or loaded means substrate is empty
	char *name_from_function = 0;
	to_lower_case(sub->name, &name_from_function); //convert to lower case
	if (strcmp(name_from_function, "empty") == 0 || (sub->L_abs == -1 && sub->L_inc == -1 && sub->SLD == 0)) {
		strcpy(sub->name,"empty");
		sub->L_abs = -1;
		sub->L_inc = -1;
		sub->SLD = 0;
		subsurface[0].Qc_sub = subsurface[1].Qc_sub = 0;
		subsurface[0].delta_n_2 = subsurface[1].delta_n_2 = 0;
	}
	if (name_from_function != 0) {
		free(name_from_function);
		name_from_function = 0;
	}
	
	//determine sub->sub_type and subsurface[SM_Num_Mirror_Planes].subsurface_type
	if (sub->L_abs != -1 || sub->L_inc != -1) {
		sub->sub_type = sm_sub_type_attn; //has attenuation
	}
	if (sub->SLD != 0) {
		subsurface[0].subsurface_type = subsurface[1].subsurface_type = sm_subsurface_type_refl | sm_subsurface_type_total_refl | sm_subsurface_type_refract; //has reflection, total reflection, and refraction  
	}

	return 1;
}

int load_mirror_material_parameters(char*mirror_material_name, double*data) 
{
	int found = 0;	
	FILE *file; 
	int file_location_option = 0; 
	char *name_from_function = 0;
	to_lower_case(mirror_material_name, &name_from_function); //convert to lower case	
	char name_from_file[CHAR_BUF_LENGTH];
	int n;
	char line[CHAR_BUF_LENGTH]; 
	
	while (file = sm_open_file("Supermirror_reflective_coating_materials.txt", "r", 0, &file_location_option)) {
		while(fgets(line, sizeof(line), file) != NULL) {

			//skip lines that have been commented out
			if (line[0] == '#' || line[0] == '\\' || line[0] == '/') continue;

			n = sscanf(line, " %99[^,],%lf,%lf,%lf,%lf,%lf,%lf", name_from_file, data, data+1, data+2, data+3, data+4, data+5);
			
			to_lower_case(name_from_file, 0); //convert to lower case

			if (n == 7) {
				if (strcmp(name_from_file, name_from_function) == 0) {
					found = 1;
					break;
				}
			}
		}
		fclose(file);	
		if (found) break;
	}
	if (name_from_function != 0) {
		free(name_from_function);
		name_from_function = 0;
	}
	return found;
}

int set_mirror_material_parameters(char*mirror_material_name, double*mirror_material_spec, double mirror_m, ReflectionParameters*mir, SubstrateSurfaceParameters*subsurface) 
{

	if (mir == 0) {
		MPI_MASTER(printf("set_mirror_material_parameters: error, pointer to ReflectionParameters variable must not be NULL.\n");) 
		return 0;
	}
	
	//start with nothing
	strcpy(mir->name, "NoSurfaceBoundary");
	mir->R0 = 0;
	mir->Qc = 0;
	mir->alpha = 0;
	mir->m = 0;
	mir->W = 0;
	mir->beta = 0;
	
	if (mirror_material_name == 0 && mirror_material_spec == 0) {
		MPI_MASTER(printf("set_mirror_material_parameters: pointers of mirror_material_name and mirror_material_spec are both not given.\n");)
	}
	
	//mirror_material_spec specified?
	if (mirror_material_spec != 0) {
		strcpy(mir->name,"UserDefine");
		if (mirror_material_name != 0) {
			if (strlen(mirror_material_name) > 0) {
				strcpy(mir->name, mirror_material_name);
			}
		}
		mir->R0 = mirror_material_spec[0];
		mir->Qc = mirror_material_spec[1];
		mir->alpha = mirror_material_spec[2];
		mir->m = mirror_material_spec[3];
		mir->W = mirror_material_spec[4];
		mir->beta = mirror_material_spec[5];
	}
	
	//look for mirror_material_name in file 	
	if (mirror_material_name != 0)
	if (strlen(mirror_material_name) > 0) {
		
		int found = 0; //assume name is not found in Supermirror_reflective_coating_materials.txt
		double d_temp[6];
		MPI_MASTER (	
			found = load_mirror_material_parameters(mirror_material_name, d_temp);
		)
		#if USE_MPI
		//broadcast the value of found to all processes
		MPI_Bcast(&found, 1, MPI_INT, mpi_node_root, MPI_COMM_WORLD);
		#endif
		if (found) {
			#if USE_MPI
			//broadcast to all processes
			MPI_Bcast(d_temp, 6, MPI_DOUBLE, mpi_node_root, MPI_COMM_WORLD);
			#endif
			
			strcpy(mir->name,mirror_material_name);
			mir->R0 = d_temp[0];
			mir->Qc = d_temp[1];
			mir->alpha = d_temp[2];
			mir->m = d_temp[3];
			mir->W = d_temp[4];
			mir->beta = d_temp[5];
		}
		else {
			MPI_MASTER (printf("set_mirror_material_parameters: No file \"Supermirror_reflective_coating_materials.txt\" or \"%s\" not on file\n",mirror_material_name);)
			return 0;
		}
	}
	
	//if mirror_m == 0, use value in file for mir->m. 
	//else mir->m = mirror_m. 
	if (mirror_m != -1) {
		mir->m = mirror_m;
	}
	
	//check for name & spec consistency
	//determine type
	char *name_from_function = 0;
	to_lower_case(mir->name, &name_from_function); //convert to lower case
	if (strcmp(name_from_function,"nosurfaceboundary") == 0 || 
		strcmp(name_from_function,"noreflection") == 0 ) {
		mir->R0 = 0;
		mir->Qc = 0;
		mir->alpha = 0;
		mir->m = 0;
		mir->W = 0;
		mir->beta = 0;
		
		mir->refl_type = 0; //No reflection from mirror or substrate
		subsurface->subsurface_type = 0; //No substrate reflection, total reflection or refraction
	}
	else if (strcmp(name_from_function,"substrate") == 0 || 
			 strcmp(name_from_function,"substratesurface") == 0 || 
			 strcmp(name_from_function,"empty") == 0 ||
			 mir->R0 <= 0 || mir->Qc <= 0 || mir->m <= 0) {
		mir->R0 = 0;
		mir->Qc = 0;
		mir->alpha = 0;
		mir->m = 0;
		mir->W = 0;
		mir->beta = 0;
		mir->refl_type = mir->refl_type & ~sm_refl_type_refl; //set to no mirror surface reflection
		//substrate interface reflection, total reflection & refraction types determined in set_substrate_material_parameters
	}
	else {
		//reflection from mirror, not from bare substrate interface
		//total reflection and refraction from substrate will be based on substrate's own parameters
		mir->refl_type = mir->refl_type | sm_refl_type_refl; //set to mirror surface reflection
		subsurface->subsurface_type = subsurface->subsurface_type & ~sm_subsurface_type_refl; //override substrate interface type to no substrate reflection
		//substrate total reflection & refraction types determined in set_substrate_material_parameters
	}
	if (name_from_function != 0) {
		free(name_from_function);
		name_from_function = 0;
	}

	if ((mir->refl_type & sm_refl_type_refl) == sm_refl_type_refl) {
		//initialise_mirror_material_parameters
		// If W and alpha are set to 0, use parametrization from Henrik Jacobsen's approach. The values depend on m only:
		// 	double m_value=m*0.9853+0.1978;
		// 	double W=-0.0002*m_value+0.0022;
		// 	For m <= 3,
		// 		double alpha=m_value;
		// 		double beta=0;
		// 	For m > 3,
		// 		double alpha=0.2304*m_value+5.0944;
		// 		double beta=-7.6251*m_value+68.1137;
		if (mir->W==0 && mir->alpha==0) {
			mir->m=mir->m*0.9853+0.1978;
			mir->W=-0.0002*mir->m+0.0022;
			if (mir->m<=3) {
				mir->alpha=mir->m;
				mir->beta=0;
			}
			else {
				mir->alpha=0.2304*mir->m+5.0944;
				mir->beta=-7.6251*mir->m+68.1137;
			}
		}
	}
	
	return 1;
}

int load_absorber_material_parameters(char*material_name, double*data) 
{
	int found = 0;	
	FILE *file; 
	int file_location_option = 0; 
	char *name_from_function = 0;
	to_lower_case(material_name, &name_from_function); //convert to lower case	
	char name_from_file[CHAR_BUF_LENGTH];
	int n;
	char line[CHAR_BUF_LENGTH]; 
	
	while (file = sm_open_file("Supermirror_absorber_coating_materials.txt", "r", 0, &file_location_option)) {
		while(fgets(line, sizeof(line), file) != NULL) {

			//skip lines that have been commented out
			if (strlen(line) == 0) continue;
			if (line[0] == '#' || line[0] == '\\' || line[0] == '/') continue;
			if (isBlank(line)) continue;			

			n = sscanf(line, " %99[^,],%lf,%lf,%lf", name_from_file, data, data+1, data+2);
			
			to_lower_case(name_from_file, 0); //convert to lower case

			if (n == 4) {
				if (strcmp(name_from_file, name_from_function) == 0) {
					found = 1;
					break;
				}
			}
		}
		fclose(file);	
		if (found) break;
	}
	if (name_from_function != 0) {
		free(name_from_function);
		name_from_function = 0;
	}
	return found;
}

int set_absorber_material_parameters(char*absorber_material_name, double absorber_L_abs, double absorber_L_inc, double absorber_thickness_in_micron, AbsorberParameters*abs) 
{
	if (abs == 0) {
		MPI_MASTER(printf("set_absorber_material_parameters: error, pointer to SubstrateParameters variable must not be NULL.\n");) 
		return 0;
	}
	
	//start with nothing
	strcpy(abs->name,"NoAbsorber");
	abs->L_abs = -1;
	abs->L_inc = -1;
	abs->thickness_in_micron = 0;
	
	//absorber parameters given?
	if ((absorber_L_abs != -1 || absorber_L_inc != -1) && absorber_thickness_in_micron != 0) {
		strcpy(abs->name,"UserDefine");
		abs->L_abs = absorber_L_abs;
		abs->L_inc = absorber_L_inc;
		abs->thickness_in_micron = absorber_thickness_in_micron;
	}

	//look for absorber name in substrate parameter file
	if (absorber_material_name != 0)
	if (strlen(absorber_material_name) > 0) {
		
		int found = 0;
		double d_temp[3];
		MPI_MASTER (
			found = load_absorber_material_parameters(absorber_material_name, d_temp);
		)
		#if USE_MPI
		//broadcast the value of found to all processes
		MPI_Bcast(&found, 1, MPI_INT, mpi_node_root, MPI_COMM_WORLD);
		#endif
		if (found) {
			#if USE_MPI
			//broadcast to all processes
			MPI_Bcast(d_temp, 3, MPI_DOUBLE, mpi_node_root, MPI_COMM_WORLD);
			#endif
			
			sprintf(abs->name,"%s",absorber_material_name);
			
			abs->L_abs = d_temp[0];
			abs->L_inc = d_temp[1];
		}
		else {
			MPI_MASTER (printf("set_absorber_material_parameters: No file \"Supermirror_absorber_coating_materials.txt\" or \"%s\" not on file\n",absorber_material_name);)
			return 0;
		}
	}
	abs->thickness_in_micron = absorber_thickness_in_micron;
	
	//check for name & spec consistency
	//determine type
	char *name_from_function = 0;

	to_lower_case(abs->name, &name_from_function); //convert to lower case
	if (strcmp(name_from_function,"nocoating") == 0 ||
		strcmp(name_from_function,"noabsorber") == 0 || 
		strcmp(name_from_function,"empty") == 0 || 
		(abs->L_abs == -1 && abs->L_inc == -1) || 
		abs->thickness_in_micron == 0) {
		strcpy(abs->name,"NoAbsorber");
		abs->L_abs = -1;
		abs->L_inc = -1;
		abs->thickness_in_micron = 0;
		abs->abs_type = 0;
	}
	else {
		abs->abs_type = sm_abs_type_attn;
	}
	if (name_from_function != 0) {
		free(name_from_function);
		name_from_function = 0;
	}
	
	return 1;
}

void set_process_parameters(int is_tracking, SupermirrorProcess *proc) 
{ 
	proc->is_tracking = is_tracking;
	if (is_tracking) {
		proc->n_nr = 0;
		proc->nr_allocation_size = 10;
		proc->nr = (NeutronRecord*)calloc(proc->nr_allocation_size, sizeof(NeutronRecord));
		proc->n_nr_allocated = proc->nr_allocation_size; 
		NeutronRecord*nr;
		
		for (int i=0; i<proc->n_nr_allocated; i++) { 
			nr=&((proc->nr)[i]);
			nr->nr_n = -1;
		}
	}
	
	strcpy(proc->side[sm_ReflectedSide],"ReflectedSide");
	strcpy(proc->side[sm_TransmittedSide],"TransmittedSide");
	strcpy(proc->side[sm_EdgeSide],"EdgeSide");
	
	strcpy(proc->plane[sm_SurfacePlane1],"Surface1");
	strcpy(proc->plane[sm_SurfacePlane2],"Surface2");
	strcpy(proc->plane[sm_EdgeFrontPlane],"EdgeFront");
	strcpy(proc->plane[sm_EdgeBackPlane],"EdgeBack");
	strcpy(proc->plane[sm_EdgeSidePlane1],"EdgeSide1");
	strcpy(proc->plane[sm_EdgeSidePlane2],"EdgeSide2");
	
	strcpy(proc->layer[sm_MirrorLayer],"MirrorLayer");
	strcpy(proc->layer[sm_AbsorberLayer],"AbsorberLayer");
	strcpy(proc->layer[sm_SubstrateSurfaceLayer],"SubstrateSurfaceLayer");
	strcpy(proc->layer[sm_SubstrateEdgeLayer],"SubstrateEdgeLayer");
	strcpy(proc->layer[sm_SubstrateLayer],"SubstrateLayer");
	
	strcpy(proc->event[sm_Error],"Error");
	strcpy(proc->event[sm_Exited],"Exited");
	strcpy(proc->event[sm_Absorbed],"Absorbed");
	strcpy(proc->event[sm_Intersected],"Intersected");
	strcpy(proc->event[sm_Missed],"Missed");
	strcpy(proc->event[sm_Reflected],"Reflected");
	strcpy(proc->event[sm_Transmitted],"Transmitted");
	strcpy(proc->event[sm_Refracted],"Refracted");
	strcpy(proc->event[sm_NotRefracted],"NotRefracted");
	strcpy(proc->event[sm_InternalReflection],"InternalReflection");
	
}

void set_location_text(Supermirror*sm, char*name, int side, int plane, int layer, char*location)
{
	sprintf(location,"%s/%s/%s/%s",
		name, ((sm->proc).side)[side], ((sm->proc).plane)[plane], ((sm->proc).layer)[layer]);
}
void set_location(SimState *state, Supermirror*sm) 
{

	sprintf(state->location,"%s/%s/%s/%s",
		sm->name, ((sm->proc).side)[state->side], ((sm->proc).plane)[state->plane], ((sm->proc).layer)[state->layer]);

}

void set_event_text(Supermirror*sm, int event_code, char*event) 
{
	strcpy(event, ((sm->proc).event)[event_code]);
}
void set_event(SimState *state, Supermirror*sm, int event_code) 
{
	strcpy(state->event, ((sm->proc).event)[event_code]);
}
	

/****************************/
/* Neutron record functions */
/****************************/
int add_neutron_record(double w, double t, Coords p, Coords v, Coords s, SupermirrorProcess*proc) {

	NeutronRecord*nr;
	int i,j,i_nr;
	
	if (proc->n_nr_allocated == proc->n_nr) { 
		//increase memory if full
		int j = proc->n_nr_allocated;
		proc->n_nr_allocated += proc->nr_allocation_size;
		proc->nr = (NeutronRecord*)realloc(proc->nr, proc->n_nr_allocated * sizeof(NeutronRecord));
		for (i=proc->n_nr_allocated-1; i>=j; i--) {
			nr=&((proc->nr)[i]);
			nr->nr_n = -1;
			nr->nr_w = 0;
			nr->nr_t = 0;
			nr->nr_p = coords_set(0,0,0);
			nr->nr_v = coords_set(0,0,0);
			nr->nr_s = coords_set(0,0,0);
			nr->nr_nhn = 0;
		}
		i_nr = j;
	}
	else {
		//find an empty record if not full
		for (i=0; i<proc->n_nr_allocated; i++) {
			if (((proc->nr)[i]).nr_n == -1) {
				i_nr = i;
				break;
			}
		}
	}

	//save data
	nr = &((proc->nr)[i_nr]);
	nr->nr_n = mcrun_num;
	nr->nr_w = w;
	nr->nr_t = t;
	nr->nr_p = p;
	nr->nr_v = v;
	nr->nr_s = s;

	++(proc->n_nr);

}

int empty_neutron_record(SupermirrorProcess*proc) {
	if (proc->n_nr == 0) 
		return 0;
	NeutronRecord*nr;
	int i;
	for(i=0; i<proc->n_nr_allocated; i++) {
		nr=&((proc->nr)[i]);
		nr->nr_n = -1;
	}
	proc->n_nr = 0;
	return 1;	
}

void free_neutron_record(SupermirrorProcess*proc) {
	free(proc->nr);
	proc->n_nr = 0;
	proc->n_nr_allocated = 0; 
	proc->nr_allocation_size = 0;
}

/*******************************/
/* Ray-Tracing Tools functions */
/*******************************/

void sm_initialise_state(SimState*state) {

	Coords null_vector = coords_set(F_INDETERMINED,F_INDETERMINED,F_INDETERMINED);
	
	state->location[0]='\0';
	state->event[0] ='\0'; 
	
	state->w = F_INDETERMINED; 
	state->t = F_INDETERMINED; 
	state->p = null_vector; 
	state->v = null_vector; 
	state->s = null_vector;
	state->ws[0] = F_INDETERMINED; state->ws[1] = F_INDETERMINED;
	
	state->last_time = F_INDETERMINED; 
	state->last_point = null_vector; 
	state->last_plane = I_INDETERMINED;

	state->v_len = F_INDETERMINED;
	state->vn_len = F_INDETERMINED;
	
	state->side = I_INDETERMINED; 
	state->plane = I_INDETERMINED; 
	state->layer = I_INDETERMINED; 
	state->fn = null_vector; 
	state->n_mirror_intersect = 0;
	state->n_surface_intersect = 0;
	
	state->ir_order_at_plane[0] = I_INDETERMINED; state->ir_order_at_plane[1] = I_INDETERMINED;
	state->v_exit_at_plane[0] = null_vector; state->v_exit_at_plane[1] = null_vector;
	state->v_ir_at_plane[0] = null_vector; state->v_ir_at_plane[1] = null_vector;

	state->Rm_at_plane[0][0] = F_INDETERMINED; state->Rm_at_plane[0][1] = F_INDETERMINED; state->Rm_at_plane[1][0] = F_INDETERMINED; state->Rm_at_plane[1][1] = F_INDETERMINED; 
	state->L_attn_abs_at_plane[0] = F_INDETERMINED; state->L_attn_abs_at_plane[1] = F_INDETERMINED; 
	state->t_prop_abs_at_plane[0] = F_INDETERMINED; state->t_prop_abs_at_plane[1] = F_INDETERMINED; 
	state->T_prop_abs_at_plane[0] = F_INDETERMINED; state->T_prop_abs_at_plane[1] = F_INDETERMINED; 

	state->Rs_at_plane[0] = F_INDETERMINED; state->Rs_at_plane[1] = F_INDETERMINED; 
	state->L_attn_sub = F_INDETERMINED; 
	state->t_prop_sub = F_INDETERMINED; 
	state->T_prop_sub = F_INDETERMINED; 
	
	state->side_at_ir_order[0] = I_INDETERMINED; state->side_at_ir_order[1] = I_INDETERMINED; 
	state->plane_at_ir_order[0] = I_INDETERMINED; state->plane_at_ir_order[1] = I_INDETERMINED; 
	state->T_ir_at_ir_order[0][0] = F_INDETERMINED; state->T_ir_at_ir_order[0][1] = F_INDETERMINED; state->T_ir_at_ir_order[1][0] = F_INDETERMINED; state->T_ir_at_ir_order[1][1] = F_INDETERMINED; 
	
	state->T_ir_all[0] = F_INDETERMINED; state->T_ir_all[1] = F_INDETERMINED; 
	state->T_ir_at_ir_order_0[0] = F_INDETERMINED; state->T_ir_at_ir_order_0[1] = F_INDETERMINED; 
	
}

/********************************/
/* Reflection by mirror surface */
/********************************/
double sm_calc_Rm(double q, double Qc, double R0, double alpha, double m, double W, double beta) 
/******************************************************************************************************************************
sm_calc_Rm calculates reflectivity from supermirror parameters
Modified from algorithm used by McStas and VITESS. 
Now: Qc = the actual supermirror critical Qc for total reflection. Previous: Must use Qc = 0.0217 [1/Å], i.e. m=1 definition.
Now: arg = (q - m * Qc natural Ni)/W with Qc natural Ni = 0.0217 [1/Å]. Previous: arg = (q - m * Qc)/W
uses: 		
update:   
return: 	Rm
output: 
called by: 	sm_get_Rm_at_plane
			InitialiseStdSupermirrorFlat_detail (to output reflectivities to file)
calls: 
********************************************************************************************************************************/
{
	//Reflection from mirror-coating
	double Qc_m  = m * 0.0217; //m * Qc for natural Ni (0.0217 [1/Å]
	
	// apply the formulation:
	// arg = (q - m * Qc_Ni)/w, 
	// reflectivity Rm_at_plane = R0*0.5*(1-tanh(arg))*(1-alpha*(q-Qc_m)+beta*(q-Qc_m)*(q-Qc_m));
	
	double arg = W > 0 ? (q - Qc_m)/W : 11;
	
	if (arg > 10 || m <= 0 || Qc_m <= 0 || R0 <= 0) {
		return 0;
	}
	
	//Note: total reflection. 
	if (q <= Qc) {
		return R0;
	}
	else {
		q -= Qc;
		return R0*0.5*(1 - tanh(arg))*(1 - alpha*q + beta*q*q);
	}
}
void sm_get_Rm_at_plane (ReflectionParameters*mir, double vn_len, double *R_plus, double *R_minus) 
/******************************************************************************************************************************
calculates the reflectivity of mirror surface [plane], Check if user select Rm[plane][+/-] to be 0 
uses: 
update:   
return: 
output: 	R_plus, R_minus
called by: 	sm_external_intersect - 1.2 sm_MirrorLayer(external to mirror)
calls:  	sm_calc_Rm
********************************************************************************************************************************/
{

	*R_plus = *R_minus = 0;

	double q = 2 * vn_len * V2Q;
	if (fabs(q) > DBL_EPSILON) {
		ReflectionParameters *mir_plus  = &(mir[0]);
		ReflectionParameters *mir_minus = &(mir[1]);
		if ((mir_plus->refl_type & sm_refl_type_refl) == sm_refl_type_refl && 
			(mir_minus->refl_type & sm_refl_type_refl) == sm_refl_type_refl) {
			*R_plus  = sm_calc_Rm(q, mir_plus->Qc, mir_plus->R0, mir_plus->alpha, mir_plus->m, mir_plus->W, mir_plus->beta);
			*R_minus = sm_calc_Rm(q, mir_minus->Qc, mir_minus->R0, mir_minus->alpha, mir_minus->m, mir_minus->W, mir_minus->beta);
		}
	}
}
int sm_reflect_or_transmit_at_mirror (SimState*state, Supermirror*sm, double ws_target, int out_is_Reflected_or_Transmitted) 
/******************************************************************************************************************************
uses: 		(state->Rm_at_plane)[state->plane][0]/[1]
update:   	state-> v, ws, s, last_time, last_point, last_plane
return: 	sm_Reflected, sm_Transmitted, sm_Error.
output:		
called by: 	sm_external_intersect - 1.2 sm_MirrorLayer(external to mirror)
calls: 
********************************************************************************************************************************/
{

	int outcome; 
	int plane = state->plane;
	double Rm_plus  = (state->Rm_at_plane)[plane][0];
	double Rm_minus = (state->Rm_at_plane)[plane][1];
	double *ws = state->ws;

//////1. determine reflect or transmit
	
	//At the boundary of exiting event (reflect out, absorbed, transmitted out) vs non-exiting event (transmitted in, not absorbed, reflect back in)
	//probability weight boundary must use 
	//exit range=[ws_start, ws-start-of-non-exiting-event], continue range=(ws-start-of-non-exiting-event, 0)
	//and use one of the following
	//if (ws_target >= ws-start-of-non-exiting-event) { handle exit event } else { handle continue neutron propagation }
	//if (ws-start-of-non-exiting-event > ws_target) { handle continue neutron propagation } else { handle exit event }
	
	switch (out_is_Reflected_or_Transmitted) {
		case sm_Reflected: 
		{ 
			//2. Here: exiting event=reflect out => probability weight boundary ws_boundary = probably weight when transmitting through interface  
			//	 weight range: reflected-out = [ws_start, ws_boundary], transmitted-in = (ws_boundary, 0)
			double ws_boundary = ws[0] * (1 - Rm_plus) + ws[1] * (1 - Rm_minus);
			
			//2. check if the outcome is reflect out or transmit through at interface
			if (ws_target >= ws_boundary) {
				outcome = sm_Reflected;
			}
			else {
				outcome = sm_Transmitted;
			}
			break;
		}
		case sm_Transmitted: 
		{
			//2. Here: exiting event=transmitted out => probability weight boundary ws_boundary = probably weight when reflected by interface  
			//	 weight range: transmitted-out = [ws_start, ws_boundary], reflected-in = (ws_boundary, 0)
			double ws_boundary = ws[0] * Rm_plus + ws[1] * Rm_minus;
		
			//2. check if the outcome is reflect out or transmit through at interface
			if (ws_boundary > ws_target) {
				outcome = sm_Reflected;
			}
			else {
				outcome = sm_Transmitted;
			}
			break;
		}
	}

//////3. process reflect or transmit
		
	switch (outcome) {
		case sm_Reflected:	
		{
			//3. neutron reflected at interface
			
			//3. update state variables ws+/-, s+/-
			
			if (out_is_Reflected_or_Transmitted == sm_Reflected) {
				//Only for beam from outside reflected away from supermirror
				state->v = coords_mirror(state->v, state->fn);
			}
			else {
				//for beam from inside reflected back into supermirror
				state->v = state->v_ir_at_plane[plane];
			}
			ws[0] *= Rm_plus; 
			ws[1] *= Rm_minus;
			
			if ((state->v).z <= 0)  {

				printf("ERROR: vz = %f going backwards after reflection\n",(state->v).z);

				outcome = sm_Error;
				break;
			}

			if (Rm_plus != Rm_minus) {
				state->s = fabs(ws[0]+ws[1]) > DBL_EPSILON ? coords_scale( (sm->co).fa, (ws[0]-ws[1])/(ws[0]+ws[1]) ) : coords_set(0,0,0);
			}
			
			state->last_time  = state->t;
			state->last_point = state->p;
			state->last_plane = plane;

			break;
		}
		case sm_Transmitted:
		{
			//2. neutron transmitted through interface
			
			//2. update ws+/-, s+/-
			
			ws[0] *= 1 - Rm_plus; 
			ws[1] *= 1 - Rm_minus;
			
			if (Rm_plus != Rm_minus) {
				state->s = fabs(ws[0]+ws[1]) > DBL_EPSILON ? coords_scale( (sm->co).fa, (ws[0]-ws[1])/(ws[0]+ws[1]) ) : coords_set(0,0,0);
			}
			
			state->last_time  = state->t;
			state->last_point = state->p;
			state->last_plane = plane;
			
			break;
		}
	}
	
	state->layer = sm_MirrorLayer; 
	set_location(state, sm);
	set_event(state, sm, outcome);
	
	if ((sm->proc).is_tracking) { 
		add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
	}

	return outcome;
}

/***********************************/
/* Reflection by substrate surface */
/***********************************/
void sm_get_Rs (SubstrateSurfaceParameters*subsurface, double vn_len, double *Rs) 
/******************************************************************************************************************************
calculates the reflectivity of substrate surface, Check if user select Rs to be 0 
uses: 
update:   
return: 
output: 	Rs
called by: 	sm_external_intersect - 1.4 sm_SubstrateSurfaceLayer(mirror to substrate) 
calls: 
********************************************************************************************************************************/
{
	*Rs = 0;
	
	double q = 2 * vn_len * V2Q;
	double Qc_sub = subsurface->Qc_sub;
	if (fabs(q) > DBL_EPSILON) {
		if ((subsurface->subsurface_type & sm_subsurface_type_total_refl ) == sm_subsurface_type_total_refl || 
			(subsurface->subsurface_type & sm_subsurface_type_refl ) == sm_subsurface_type_refl) {
			if ((subsurface->subsurface_type & sm_subsurface_type_refl ) == sm_subsurface_type_refl) {
				//both total reflection and beyond
				if (q <= Qc_sub) { 
					*Rs = 1; 
				}
				
				double dd = sqrt(q*q - Qc_sub*Qc_sub);	
				dd = (q - dd) / (q + dd);	
				dd *= dd;
				*Rs = dd;
			}
			else {
				//only total reflection
				*Rs = q <= subsurface->Qc_sub? 1 : 0; 
			}
		}
	}
}
int sm_reflect_or_transmit_at_substrate_surface (SimState*state, Supermirror*sm, double ws_target, int out_is_Reflected_or_Transmitted) 
/******************************************************************************************************************************
uses:  		(state->Rs_at_plane)[state->plane]
update: 	state-> plane, v, ws, last_time, last_point, last_plane
return: 	sm_Reflected, sm_Transmitted, sm_Error.
Output:		
called by: 	sm_external_intersect - 1.4 sm_SubstrateSurfaceLayer(mirror to substrate)
calles: 
********************************************************************************************************************************/
{
	int outcome; 
	
	int plane = state->plane;
	double Rs = (state->Rs_at_plane)[plane];
	double *ws = state->ws;
	
//////1. determine reflect or transmit

	//At the boundary of exiting event (reflect out, absorbed, transmitted out) vs non-exiting event (transmitted in, not absorbed, reflect back in)
	//probability weight boundary must use 
	//exit range=[ws_start, ws-start-of-non-exiting-event], continue range=(ws-start-of-non-exiting-event, 0)
	//and use one of the following
	//if (ws_target >= ws-start-of-non-exiting-event) { handle exit event } else { handle continue neutron propagation }
	//if (ws-start-of-non-exiting-event > ws_target) { handle continue neutron propagation } else { handle exit event }
	
	switch (out_is_Reflected_or_Transmitted) {
		case sm_Reflected: 
		{ 
			//2. Here: exiting event=reflect out => probability weight boundary ws_boundary = probably weight when transmitting through interface  
			//	 weight range: reflected-out = [ws_start, ws_boundary], transmitted-in = (ws_boundary, 0)
			double ws_boundary = (ws[0] + ws[1]) * (1 - Rs);
		
			//2. check if the outcome is reflect out or transmit through at interface
			if (ws_target >= ws_boundary) {
				outcome = sm_Reflected;
			}
			else {
				outcome = sm_Transmitted;
			}
			break;
		}
		case sm_Transmitted: 
		{
			//2. Here: exiting event=transmitted out => probability weight boundary ws_boundary = probably weight when reflected by interface  
			//	 weight range: transmitted-out = [ws_start, ws_boundary], reflected-in = (ws_boundary, 0)
			double ws_boundary = (ws[0]  + ws[1]) * Rs;
		
			//2. check if the outcome is reflect out or transmit through at interface
			if (ws_boundary > ws_target) {
				outcome = sm_Reflected;
			}
			else {
				outcome = sm_Transmitted;
			}
			break;
		}
	}

//////3. process reflect or transmit
		
	switch (outcome) {
	case sm_Reflected:	
		//3. neutron reflected at interface
		
		//3. update state variables ws+/-, s+/-

		if (out_is_Reflected_or_Transmitted == sm_Reflected) {
			//Only for beam from outside reflected away from supermirror
			state->v = coords_mirror(state->v, state->fn);
		}
		else {
			//for beam from inside reflected back into supermirror
			state->v = state->v_ir_at_plane[plane];
		}
		ws[0] *= Rs; 
		ws[1] *= Rs;
		
		state->last_time  = state->t;
		state->last_point = state->p;
		state->last_plane = plane;

		break;
	
	case sm_Transmitted:
		//2. neutron transmitted through interface
		
		//2. update ws+/-, s+/-
		
		ws[0] *= 1 - Rs; 
		ws[1] *= 1 - Rs;
		
		state->last_time  = state->t;
		state->last_point = state->p;
		state->last_plane = plane;
		
		break;
	}
	
	state->layer = sm_SubstrateSurfaceLayer; 
	set_location(state, sm);
	set_event(state, sm, outcome);
	
	if ((sm->proc).is_tracking) { 
		add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
	}

	return outcome;
}

/***********************************/
/* Transmit through absorber layer */
/***********************************/
double sm_get_L_attn_abs_at_plane (AbsorberParameters*abs, double v_len) 
/******************************************************************************************************************************
Purpose:	calculate L_attn_abs through absorber, check if user select no absorption
uses:  
update: 
return: 	L_attn_abs
output:		
called by: 	sm_get_prop_abs_at_plane
calls: 
********************************************************************************************************************************/
{
//////////obtain L_abs, L_inc
	
	if ((abs->abs_type & sm_abs_type_attn) == 0) {
		return -1;
	}
	
	//obtain L_abs
	double L_abs = abs->L_abs; 
	double L_inc = abs->L_inc; 
	
	if (L_abs == -1 && L_inc == -1) {
		return -1;
	}

//////get L_attn, here either L_abs != -1 or L_inc != -1
	//default L_attn=-1, no attenuation
	double L_attn = -1; 
	
	if (L_abs >= 0) {
		L_attn = (v_len * L_abs) / AMS;
	}
	
	if (L_attn != 0 && L_inc >= 0) {
	if (L_attn == -1) {
			L_attn = L_inc;
		}
		else {
			L_attn *= L_inc / (L_attn + L_inc);
		}
	}
	
	return L_attn; 
}
void sm_get_prop_abs_at_plane (AbsorberParameters*abs, double v_len, double vn_len, double*L_attn_abs_at_plane, double*t_prop_abs_at_plane, double*T_prop_abs_at_plane) 
/******************************************************************************************************************************
Purpose:	calculate time & transmission through absorber layer, top & bottom surfaces only, i.e. side edges not taken into account.
uses: 		
update: 	
return:		
output: 	L_attn_abs_at_plane, t_prop_abs_at_plane, T_prop_abs_at_plane.
called by: 	sm_external_intersect - 1.3 sm_AbsorberLayer(mirror to substrate)
calls: 		sm_get_L_attn_abs_at_plane
********************************************************************************************************************************/
{
	double L_attn, t_prop, T_prop;

	if (vn_len <= FLT_EPSILON) {
		//neutron flying parallel to surface, no intersect
		//neutron flying parallel to surface, no intersect
		L_attn = -1;
		t_prop = F_INDETERMINED;
		T_prop = 1;
	}
	else {

		//get t_prop
		t_prop = fabs(abs->thickness_in_micron * 1E-6 / vn_len);
		
		//calculate L_attn
		L_attn = sm_get_L_attn_abs_at_plane (abs, v_len);
		
		//calculate T_prop
		if (L_attn != -1) {
			T_prop = L_attn > DBL_EPSILON? exp(-t_prop * v_len / L_attn) : 0;
		}
		else {
			//no absorption
			T_prop = 1;
		}
	}

	*L_attn_abs_at_plane = L_attn;
	*t_prop_abs_at_plane = t_prop;
	*T_prop_abs_at_plane = T_prop;
}
int sm_transmit_through_absorber_at_plane (SimState*state, Supermirror*sm, double ws_target) 
/******************************************************************************************************************************
Purpose:	calculate transmittion through absorber at plane
Uses: 		state->L_attn_abs_at_plane[state->plane]
			state->t_prop_abs_at_plane[state->plane]
			state->T_prop_abs_at_plane[state->plane]
			state-> plane, p, v, fn
Update: 	state-> t, p, ws, last_time, last_point, last_plane, side, plane, layer 
Return: 	sm_Transmitted, sm_Exited, sm_Absorbed. 
called by: 	sm_external_intersect - 1.3 sm_AbsorberLayer(mirror to substrate) 
			sm_external_intersect - 1.5 sm_AbsorberLayer(substrate to mirror) 
calls: 		line_polyhedron_intersect
			sm_get_L_attn_abs_at_plane. 
********************************************************************************************************************************/
{
	int outcome=sm_Exited; 
	double dt, dL;
	int plane = state->plane;
	AbsorberParameters*abs = &(((sm->mat).abs)[plane]);
	double L_attn_abs = state->L_attn_abs_at_plane[plane];
	double t_prop_abs = state->t_prop_abs_at_plane[plane]; //time to propagate through absorber layer
	double T_prop_abs = state->T_prop_abs_at_plane[plane];
	
//////calculate time & transmission through absorber layer
	
//////calculate time to edge
//////Find state->iplane[0] and state->idtime[0] to next intersect through substrate edges.
	double t_exit_abs;
	
	//si->type: 1=x plane, 2=x edge, 3=x vertex, 4=on plane, 5=on edge
	//SupermirrorIntersect *si = &((sm->proc).si);
	int skip_plane[] = {0, 1};
	int n_skip_plane = 2;
	int num_intersect = 1; //only use values of the first intersect
	line_polyhedron_intersect(state->t, state->p, state->v, &(sm->geo), Maximum_On_Plane_Distance, n_skip_plane, skip_plane,  
								state->last_time, state->last_point, state->last_plane, 
								&num_intersect, state->idtime, state->idpoint, state->itime, state->ipoint, state->iplane, state->itype); 
	
	//For transmitted beam from inside, 
	//must have 1 valid intersects and not riding on surface or edge, 
	if (num_intersect > 0 && state->itype[0] < 4) {
		t_exit_abs = state->idtime[0];
		
//////Determine outcome: absoprtion or transmit through mirror-side surfaces or edge  
		if (t_prop_abs <= t_exit_abs) {
			outcome = sm_Transmitted;
			dt = t_prop_abs; 
		}
		else {
			outcome = sm_Exited;
			dt = t_exit_abs; 
		}
		
//////////Determine outcome: absoprtion or transmit through mirror-side surfaces or edge  
		if (L_attn_abs != -1) 
		{
			double t_abs_abs = -log(ws_target/(state->ws[0] + state->ws[1])) * L_attn_abs / state->v_len;

			if (t_abs_abs <= dt) { 
				//neutron absorbed
				outcome = sm_Absorbed;
				dt = t_abs_abs; 
			}
		}
	}
	else { //something's not right
		printf(	"sm_transmit_through_absorber_at_plane: Inside module but found no intersect, \n"
				"maybe riding along edge surface or line outside(state->itype[0]=%d), \n"
				"something's wrong. Return 0 (exit module)\n", state->itype[0]);
		outcome = sm_Error;
	}

//////update state variables
	switch (outcome) {
		case sm_Transmitted: 
		{
			state->layer = sm_AbsorberLayer; 

			state->t += dt; 
			state->p = coords_add(state->p, coords_scale(state->v, dt));
	
			state->ws[0] *= T_prop_abs;
			state->ws[1] *= T_prop_abs;
			
			state->last_time  = state->t;
			state->last_point = state->p;
			state->last_plane = state->plane;
			
			set_location(state, sm);	
			set_event(state, sm, sm_Transmitted);
			if ((sm->proc).is_tracking) { 
				add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
			}
			
			break;
		}
		case sm_Exited:
		{
			state->side = sm_EdgeSide;
			state->plane = state->iplane[0];
			state->layer = sm_AbsorberLayer; 
			
			state->t += dt; 
			state->p = coords_add(state->p, coords_scale(state->v, dt));

			state->fn = (sm->geo).fn[state->iplane[0]];
			
			state->last_time  = state->t;
			state->last_point = state->p;
			state->last_plane = state->plane;
			
			set_location(state, sm);	
			set_event(state, sm, sm_Exited);
			if ((sm->proc).is_tracking) {
				add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
			}
	
			break;
		}
		case sm_Absorbed:
		{
			state->layer = sm_AbsorberLayer; 
	
			state->t += dt; 
			state->p = coords_add(state->p, coords_scale(state->v, dt));

			state->last_time = F_INDETERMINED;
			state->last_point = coords_set(F_INDETERMINED, F_INDETERMINED, F_INDETERMINED);
			state->last_plane = I_INDETERMINED;
			
			set_location(state, sm);	
			set_event(state, sm, sm_Absorbed);
			if ((sm->proc).is_tracking) {
				add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
			}

			break;
		}
		case sm_Error:
		{
			state->layer = sm_AbsorberLayer; 
	
			state->last_time = F_INDETERMINED;
			state->last_point = coords_set(F_INDETERMINED, F_INDETERMINED, F_INDETERMINED);
			state->last_plane = I_INDETERMINED;
			
			set_location(state, sm);	
			set_event(state, sm, sm_Error);
			if ((sm->proc).is_tracking) {
				add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
			}

			break;
		}
	}

	return outcome; //Return outcome.
}

/******************************/
/* Transmit through substrate */
/******************************/
double sm_get_L_attn_sub (SubstrateParameters*sub, double v_len) 
/******************************************************************************************************************************
Purpose:	calculate L_attn_sub through substrate, check if user select no absorption
uses:  
update: 
return: 	L_attn_sub
output:		
called by: 	sm_get_prop_sub
calls: 
********************************************************************************************************************************/
{
//////////determine t_prop_at_layer, L_prop_at_layer, obtain L_abs, L_inc
	
	//default: no attentuation
	if ((sub->sub_type & sm_sub_type_attn) == 0) {
		return -1;
	}
	
	//obtain L_abs
	double L_abs = sub->L_abs; 
	double L_inc = sub->L_inc; 
	
	if (L_abs == -1 && L_inc == -1) {
		return -1;
	}

//////get L_attn, here either L_abs != -1 or L_inc != -1
	//default L_attn=-1, no attenuation
	double L_attn = -1; 
	
	if (L_abs >= 0) {
		L_attn = (v_len * L_abs) / AMS;
	}
	
	if (L_attn != 0 && L_inc >= 0) {
	if (L_attn == -1) {
			L_attn = L_inc;
		}
		else {
			L_attn *= L_inc / (L_attn + L_inc);
		}
	}
	
	return L_attn; 
}

int sm_transmit_through_substrate (SimState*state, Supermirror*sm, double ws_target) 
/******************************************************************************************************************************
Purpose:	calculate transmittion through substrate
Uses: 		state->L_attn_sub
			state-> t, p, v, ws
update: 	state-> t, p, plane, fn, ws, last_time, last_point, last_plane, side, plane, layer 
			state-> t_prop_sub, T_prop_sub if state->n_mirror_intersect == 2
return: 	sm_Transmitted, sm_Exited, sm_Absorbed. 
output:		
called by: 	sm_internal_intersect - n.1 sm_SubstrateLayer 
calls: 		line_polyhedron_intersect
			sm_get_L_attn_sub. 
********************************************************************************************************************************/
{
	int outcome; 
	double L_attn_sub = state->L_attn_sub;
	

//////calculate time & transmission through substrate
	
	//state->itype[0]: 1=x plane, 2=x edge, 3=x vertex, 4=on plane, 5=on edge
	int iplane, itype; double idt;
	int num_intersect = 1; //only use values of the first intersect
	//Skip the current plane in finding intersect, neutron should propagate to other planes.
	//Also, refraction may result in trajectory almost parallel to current plane, can unphysically transmit out due to numerical precision limit. 
	int skip_plane[] = {state->plane};
	int n_skip_plane = 1;
	line_polyhedron_intersect(state->t, state->p, state->v, &(sm->geo), Maximum_On_Plane_Distance, n_skip_plane, skip_plane,  
								state->last_time, state->last_point, state->last_plane, 
								&num_intersect, &idt, 0, 0, 0, &iplane, &itype); 
	
	//For transmitted beam from inside, 
	//must have 1 valid intersects and not riding on surface or edge, 
	if (num_intersect > 0 && itype < 4) {
		
//////Determine outcome: transmit through mirror-side surfaces or edge 
	
		if (iplane < SM_Num_Mirror_Planes) {
			//intersect mirror surface
			outcome = sm_Transmitted; 
		}
		else {
			//intersect edge, exit supermirror
			outcome = sm_Exited; 
		}
		
//////////Determine outcome: transmission or absorption
		if (L_attn_sub != -1) 
		{
			double t_abs_sub = -log(ws_target/(state->ws[0] + state->ws[1])) * L_attn_sub / state->v_len;

			if (t_abs_sub <= idt) { 
				//neutron absorbed
				outcome = sm_Absorbed;
				idt = t_abs_sub; 
			}
		}
	}
	else { //something's not right
		printf(	"sm_transmit_through_substrate: Inside module but found no intersect, \n"
				"maybe riding along edge surface or line outside(itype=%d), \n"
				"something's wrong. Return 0 (exit module)\n", itype);
		outcome = sm_Error; 
	}

//////update state variables
	state->t += idt; 
	state->p = coords_add(state->p, coords_scale(state->v, idt));
			
	switch (outcome) {
		case sm_Transmitted: 
		{
			//neutron hits mirror surface from inside
			++(state->n_surface_intersect);
			++(state->n_mirror_intersect);
			
			state->plane = iplane;
			state->layer = sm_SubstrateSurfaceLayer;
			state->fn = (sm->geo).fn[iplane];
			
			//update state variables at 2nd surface intersect
			if (state->n_surface_intersect == 2) {
				//2nd surface intersect that is a mirror is always at the transmitted side
				state->side = sm_TransmittedSide;
				//also a good place to update v_ir_at_plane for subsequent use in internal_intersect and internal_reflection
				state->v_ir_at_plane[iplane] = coords_mirror(state->v, state->fn); 
				state->v_ir_at_plane[1-iplane] = state->v; 
			}
			else {
				//otherwise switch between transmitted and reflected side
				state->side = state->side == sm_ReflectedSide? sm_TransmittedSide : sm_ReflectedSide;
			}
			
			//update substrate transmission parameters until 2nd mirror intersect = mirror to mirror neutron transmission
			if (state->n_mirror_intersect <= SM_Num_Mirror_Planes) {
				state->t_prop_sub = idt;
				if (L_attn_sub == -1) {
					state->T_prop_sub = 1;
				}
				else if (L_attn_sub > DBL_EPSILON) {
					state->T_prop_sub = exp(-(idt * state->v_len /L_attn_sub));
				}
				else {
					state->T_prop_sub = 0;
				}
			}
	
			state->ws[0] *= state->T_prop_sub;
			state->ws[1] *= state->T_prop_sub;
			
			state->last_time  = state->t;
			state->last_point = state->p;
			state->last_plane = state->plane;
				
			set_event(state, sm, sm_Transmitted);
			
			break;
		}
		case sm_Exited:
		{
			++(state->n_surface_intersect);
			
			state->side = sm_EdgeSide;
			state->plane = iplane;
			state->layer = sm_SubstrateEdgeLayer;
			state->fn = (sm->geo).fn[iplane];

			state->last_time  = state->t;
			state->last_point = state->p;
			state->last_plane = state->plane;
			
			set_event(state, sm, sm_Exited);
			
			break;
		}
		case sm_Absorbed:
		{
			set_event(state, sm, sm_Absorbed);
			break;
		}
		case sm_Error:
		{
			set_event(state, sm, sm_Error);
			break;
		}
	}

	set_location(state, sm);	
	if ((sm->proc).is_tracking) { 
		add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
	}

	return outcome; //Return outcome.
}


/***************/
/* Refraction  */
/***************/
void sm_get_inward_refracted_v (SubstrateSurfaceParameters*subsurface, Coords v_in, Coords fn, Coords *v_out) 
/******************************************************************************************************************************
Purpose:	calculate v refracted into sm_SubstrateSurfaceLayer, check user selected no refraction. 
uses:  		
update: 	
return: 	
output:		v_out
called by: 	sm_external_intersect - 1.5 sm_SubstrateSurfaceLayer(mirror to substrate), refraction
calls: 		
********************************************************************************************************************************/
{
	if ((subsurface->subsurface_type & sm_subsurface_type_refract) == 0) {
		return;
	}

	*v_out= v_in; //default: no refraction
	
	double vn_len = coords_sp(v_in, fn); 
	
	if (fabs(vn_len) < FLT_EPSILON) {
		return;
	}

	//calculate material index of refraction
	double v_len_2 = coords_sp(v_in, v_in); 
	
	//start with substrate to air index of refraction,  then invert for air to substrate index of refraction
	//n = n(substrate)/n(air): 1-n^2 = delta_n_2 / v^2
	//delta_n_2 = AMS * AMS * SLD / M_PI;
	double one_minus_n_2 = subsurface->delta_n_2 / v_len_2; //start with substrate to air
	one_minus_n_2 = -one_minus_n_2/(1-one_minus_n_2); //1-n^2, n(air)/n(substrate), invert for air to substrate
	
	if (fabs(one_minus_n_2) < DBL_EPSILON) {
		return;
	}

	double n_2 = 1 - one_minus_n_2; //n^2
	double v_len_2_minus_v2s_len_2 = v_len_2*one_minus_n_2 + vn_len*vn_len - one_minus_n_2*vn_len*vn_len;

	if (v_len_2_minus_v2s_len_2 < 0) {
		//total reflection somehow sneak through!? Ignore. 
		return;
	}

	//Now calculate refraction, refracted vn_len has same sign as incoming vn_len
	vn_len = (vn_len < 0? -1 : 1) * sqrt( n_2 * v_len_2_minus_v2s_len_2 );
	*v_out = coords_add( coords_scale(coords_xp(coords_xp(fn, v_in), fn), n_2), coords_scale(fn, vn_len) ); //v = vs + vn

	return; //calculation done
}
int sm_refract_inward_at_substrate_surface (SimState*state, Supermirror*sm) 
/******************************************************************************************************************************
Purpose:	calculate refraction at substrate surface (going into substrate)
Uses: 		state-> v
update: 	state-> v, v_len, vn_len
return: 	sm_Refracted, sm_NotRefracted 
output:		
called by: 	sm_external_intersect - 1.5 sm_SubstrateSurfaceLayer(mirror to substrate), refraction 
calls: 		sm_get_outward_refracted_v
********************************************************************************************************************************/
{
	int outcome;
	int plane = state->plane;
	SubstrateSurfaceParameters*subsurface = &(((sm->mat).subsurface)[plane]);
	
	if ((subsurface->subsurface_type & sm_subsurface_type_refract) == 0) {
		//No refraction by user selection
		outcome = sm_NotRefracted;
	}
	else if (state->vn_len < FLT_EPSILON) {
		outcome =  sm_NotRefracted;
	}
	else {
		sm_get_inward_refracted_v (subsurface, state->v, state->fn, &(state->v));
		state->v_len = coords_len(state->v);
		state->vn_len = fabs(coords_sp(state->v, state->fn));
		outcome = sm_Refracted;
	}
	
	state->layer = sm_SubstrateSurfaceLayer; 
	set_location(state, sm);
	set_event(state, sm, outcome);
	if ((sm->proc).is_tracking) { 
		add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
	}
	return outcome; //Return outcome.
}

void sm_get_outward_refracted_v (SubstrateSurfaceParameters*subsurface, Coords v_in, Coords fn, Coords *v_out) 
/******************************************************************************************************************************
Purpose:	calculate v refracted out of supermirror, check user selected no refraction. 
uses:  		
update: 	
return: 	
output:		v_out
called by: 	sm_internal_intersect - ????????????????????????????
calls: 		
********************************************************************************************************************************/
{
	if ((subsurface->subsurface_type & sm_subsurface_type_refract) == 0) {
		return;
	}

	*v_out= v_in; //default: no refraction
	
	double vn_len = coords_sp(v_in, fn); 
	
	if (fabs(vn_len) < FLT_EPSILON) {
		return;
	}
	
	//calculate material index of refraction
	double v_len_2 = coords_sp(v_in, v_in); 
	
	//substrate to air index of refraction, 
	//n = n(substrate)/n(air): 1-n^2 = delta_n_2 / v^2
	//delta_n_2 = AMS * AMS * SLD / M_PI;
	double one_minus_n_2 = subsurface->delta_n_2 / v_len_2; 
	if (fabs(one_minus_n_2) < DBL_EPSILON) {
		return;
	}

	double n_2 = 1 - one_minus_n_2; //n^2
	double v_len_2_minus_v2s_len_2 = v_len_2*one_minus_n_2 + vn_len*vn_len - one_minus_n_2*vn_len*vn_len;

	if (v_len_2_minus_v2s_len_2 < 0) {
		//total reflection somehow sneak through!? Ignore. 
		return;
	}

	//Now calculate refraction, refracted vn_len has same sign as incoming vn_len
	vn_len = (vn_len < 0? -1 : 1) * sqrt( n_2 * v_len_2_minus_v2s_len_2 );
	*v_out = coords_add( coords_scale(coords_xp(coords_xp(fn, v_in), fn), n_2), coords_scale(fn, vn_len) ); //v = vs + vn
	
	return; //calculation done
}
int sm_refract_outward_at_mirror (SimState*state, Supermirror*sm) 
/******************************************************************************************************************************
Purpose:	calculate refraction at mirror (going out of supermirror)
Uses: 		state-> v_exit_at_plane[state->plane]
update: 	state-> v
return: 	sm_Refracted, sm_NotRefracted 
output:		
called by: 	sm_internal_intersect - ???????????????????????????? 
calls: 		 
********************************************************************************************************************************/
{
	int outcome;
	int plane = state->plane;
	SubstrateSurfaceParameters*subsurface = &(((sm->mat).subsurface)[plane]);
	
	if ((subsurface->subsurface_type & sm_subsurface_type_refract) == 0) {
		//No refraction by user selection
		outcome = sm_NotRefracted;
	}
	else if (state->vn_len < FLT_EPSILON) {
		outcome =  sm_NotRefracted;
	}
	else {
		state->v = state->v_exit_at_plane[plane];
		state->v_len = coords_len(state->v);
		state->vn_len = fabs(coords_sp(state->v, state->fn));
		outcome = sm_Refracted;
	}
	
	state->layer = sm_MirrorLayer; 
	set_location(state, sm);
	set_event(state, sm, outcome);
	if ((sm->proc).is_tracking) {
		add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
	}
	return outcome; //Return outcome.
}


/*************************/
/* Into substrate layer  */
/*************************/
void sm_enter_substrate_layer(SimState*state, Supermirror*sm, int event) {
	state->layer = sm_SubstrateLayer;
	set_location(state, sm);
	set_event(state, sm, event);
	if ((sm->proc).is_tracking) { 
		add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
	}
}


/***********************/
/* Internal reflection */
/***********************/
void sm_set_internal_reflection_parameters (SimState*state, Supermirror*sm, double ws_target) { 
	
	int ir_order = 1;
	int plane = state->plane;
	Coords fn=state->fn;
	Coords v=state->v;
	double v_len = state->v_len;
	double vn_len = state->vn_len;
	
	state->ir_order_at_plane[plane] = ir_order; state->ir_order_at_plane[1-plane] = 1-ir_order;
	state->plane_at_ir_order[ir_order] = plane; state->plane_at_ir_order[1-ir_order] = 1-plane;
	if (state->n_surface_intersect == 2) {
		state->side_at_ir_order[ir_order] = sm_TransmittedSide;
		state->side_at_ir_order[1-ir_order] = sm_ReflectedSide;
	}
	else {
		state->side_at_ir_order[ir_order] = sm_ReflectedSide;
		state->side_at_ir_order[1-ir_order] = sm_TransmittedSide;
	}

	for (plane = 0; plane < SM_Num_Mirror_Planes; ++plane) {
		
		if ((state->Rm_at_plane)[plane][0] == F_INDETERMINED || (state->Rm_at_plane)[plane][0] == D_INDETERMINED) {
			sm_get_Rm_at_plane(((sm->mat).mir)[plane], vn_len, 
								&((state->Rm_at_plane)[plane][0]), 
								&((state->Rm_at_plane)[plane][1]));
		}
		
		if ((state->L_attn_abs_at_plane)[plane] == F_INDETERMINED || (state->L_attn_abs_at_plane)[plane] == D_INDETERMINED) {
			sm_get_prop_abs_at_plane (&(((sm->mat).abs)[plane]), v_len, vn_len, 
										&((state->L_attn_abs_at_plane)[plane]), 
										&((state->t_prop_abs_at_plane)[plane]), 
										&((state->T_prop_abs_at_plane)[plane]));
		}
		
		if ((state->Rs_at_plane)[plane] == F_INDETERMINED || (state->Rs_at_plane)[plane] == D_INDETERMINED) {
			sm_get_Rs (&(((sm->mat).subsurface)[plane]), vn_len, &((state->Rs_at_plane)[plane]));
		}
		
		if (((state->v_exit_at_plane)[plane]).x == F_INDETERMINED || ((state->v_exit_at_plane)[plane]).x == D_INDETERMINED) {
			sm_get_outward_refracted_v (&(((sm->mat).subsurface)[plane]), (state->v_ir_at_plane)[1-plane], state->fn, 
										&((state->v_exit_at_plane)[plane]));
		}
	}

	for (int spin=0; spin<SM_Num_Spin_States; spin++) {
		
		state->T_ir_all[spin] = 1; 
		
		for (ir_order=0; ir_order<SM_Num_Mirror_Planes; ir_order++) {
			
			plane = state->plane_at_ir_order[ir_order];
			
			state->T_ir_at_ir_order[ir_order][spin] = 
				state->T_prop_sub * state->Rs_at_plane[plane] 
				+ 
				state->T_prop_sub * (1 - state->Rs_at_plane[plane]) * 
				state->T_prop_abs_at_plane[plane] * 
				state->Rm_at_plane[plane][spin] * 
				state->T_prop_abs_at_plane[plane]; 
			
			state->T_ir_all[spin] *= state->T_ir_at_ir_order[ir_order][spin]; 
			
		}
		
		state->T_ir_at_ir_order_0[spin] = state->T_ir_at_ir_order[0][spin];

	}
}

int sm_internal_reflection_w (SimState*state, int n, double*w_out) 
/******************************************************************************************************************************
starting at given state, calculates the weight ws after internally reflecting n times
********************************************************************************************************************************/
{
	if (n < 0) return 0; //n should not be < 0
	if (n == 0) {
		if (w_out) { 
			w_out[0] = state->ws[0];
			w_out[1] = state->ws[1];
		}
		return 1; //no further internal reflection, out=in. 
	}
	//n = 2m + 1-r
	int m = (int)floor(n/2);
	int r = 2*m+1-n;

	if (w_out) {
		for (int spin = 0; spin < SM_Num_Spin_States; spin++) {
			w_out[spin] = state->ws[spin]
							* pow(state->T_ir_all[spin], m) 
							* (r == 0 ? state->T_ir_at_ir_order_0[spin] : 1);
		}
	}
	return 1; //calculation done
}



/************************/
/* Trajectory functions */
/************************/

int sm_external_intersect(SimState *state, Supermirror *sm, double ws_target) 
//1st intersect comes from neutrons outside of the supermirror
{
	
//////1.1 intersect from external to supermirror
	
//////1.1 find line-polyhedron intersects
	
	//type: 1=x plane, 2=x edge, 3=x vertex, 4=on plane, 5=on edge
	int num_intersect = 2; //only use values of the first two intersects
	line_polyhedron_intersect(state->t, state->p, state->v, &(sm->geo), Maximum_On_Plane_Distance, 0, 0, 
								state->last_time, state->last_point, state->last_plane, 
								&num_intersect, state->idtime, state->idpoint, state->itime, state->ipoint, state->iplane, state->itype); 
	
	//For incident beam on supermirror from outside, 
	//must have 2 valid intersects and not riding on surface or edge to go in, 
	//otherwise miss supermirror
	if (num_intersect < 2 || state->itype[0] > 3 || state->itype[1] > 3) {
		//incident neutron passes corner or edge without going in
		
		set_event(state, sm, sm_Missed);
		if ((sm->proc).is_tracking) {
			add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
		}
		
		return sm_Missed; //neutron misses module, , exit calculation
	}
	
//////1.1 Intersect found, propagate to intersect, update state variables
	state->n_surface_intersect = 1;
	
	state->plane = state->iplane[0];
	if (state->plane < SM_Num_Mirror_Planes) {
		state->side = sm_ReflectedSide;
		state->layer = sm_MirrorLayer;
	}
	else {
		state->side = sm_EdgeSide;
		state->layer = sm_SubstrateEdgeLayer;
	}
	set_location(state, sm);
	set_event(state, sm, sm_Intersected);
	
	state->fn = (sm->geo).fn[state->iplane[0]];
	
	state->t = state->itime[0];
	state->p = state->ipoint[0];
	
	state->last_time  = state->itime[0];
	state->last_point = state->ipoint[0];
	state->last_plane = state->iplane[0];

	double s_pol = coords_sp((sm->co).fa, state->s);
	state->s = coords_scale((sm->co).fa, s_pol);
	state->ws[0] = (1 + s_pol) / 2; //plus intensity 
	state->ws[1] = (1 - s_pol) / 2; //minus intensity
	
	if ((sm->proc).is_tracking) {
		add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
	}

//////1.1 
//////    if surface plane: reflect out? transmit in?
//////    else: not surface plane, transmit in. 

	int plane;
	double v_len = state->v_len = coords_len(state->v); 
	double vn_len = state->vn_len = fabs(coords_sp(state->v, state->fn));
	
	int outcome;

	switch (state->layer) {
		
//////////1.1 sm_MirrorLayer(external to mirror)
		case sm_MirrorLayer: 
		{
//////////////1.2 sm_MirrorLayer(external to mirror), sm_Reflected-sm_Exited or sm_Transmitted-continue
			//first get Rm_at_plane
			plane = state->plane;
			sm_get_Rm_at_plane(((sm->mat).mir)[plane], vn_len, 
								&((state->Rm_at_plane)[plane][0]), 
								&((state->Rm_at_plane)[plane][1]));
			
			outcome = sm_reflect_or_transmit_at_mirror(state, sm, ws_target, sm_Reflected);
			switch (outcome) {  
			
//////////////////1.2 sm_MirrorLayer(external to mirror), sm_Reflected, sm_Exited, return sm_Exited (exit module).
				case sm_Reflected:	
				{
					//location: ReflectSide, sm_MirrorLayer(external to mirror)
					set_event(state, sm, sm_Exited);
					if ((sm->proc).is_tracking) {
						add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
					}
					return sm_Exited; //sm_Exited, return sm_Exited (exit module)
					break;
				}

//////////////////1.2 sm_MirrorLayer(external to mirror), sm_Transmitted, continue.
				case sm_Transmitted: //sm_MirrorLayer(external to mirror), sm_Transmitted, continue.
				{
					//ReflectSide, sm_MirrorLayer(mirror to substrate)

//////////////////////1.3 sm_AbsorberLayer(mirror to substrate), sm_Absorbed or sm_Transmitted-continue. 
					
					//location: ReflectSide, sm_MirrorLayer(external to mirror)
					sm_get_prop_abs_at_plane (&(((sm->mat).abs)[plane]), v_len, vn_len, 
												&((state->L_attn_abs_at_plane)[plane]), 
												&((state->t_prop_abs_at_plane)[plane]), 
												&((state->T_prop_abs_at_plane)[plane]));
					outcome = sm_transmit_through_absorber_at_plane (state, sm, ws_target);
					switch (outcome) {  
					
//////////////////////////1.3 sm_AbsorberLayer(mirror to substrate), sm_Absorbed. 
						case sm_Absorbed: //sm_AbsorberLayer(mirror to substrate), sm_Absorbed, return sm_Absorbed (exit module).
						{
							//location: ReflectSide, sm_AbsorberLayer(mirror to substrate)
							return sm_Absorbed;
							break; 
						}

//////////////////////////1.3 sm_AbsorberLayer(mirror to substrate), sm_Exited. 
						case sm_Exited: //sm_AbsorberLayer(mirror to substrate), sm_Exited, return sm_Exited (exit module).
						{
							//location: ReflectSide, sm_AbsorberLayer(mirror to substrate)
							return sm_Exited; //sm_Exited, return sm_Exited (exit module)
							break;
						}
						
//////////////////////////1.3 sm_AbsorberLayer(mirror to substrate), sm_Transmitted, continue. 
						case sm_Transmitted: //sm_AbsorberLayer(mirror to substrate), sm_Transmitted, continue.
						{

//////////////////////////////1.4 sm_SubstrateSurfaceLayer(mirror to substrate), sm_Reflected-continue or sm_Transmitted-continue

							//location: ReflectSide, sm_SubstrateSurfaceLayer(mirror to substrate)
							sm_get_Rs (&(((sm->mat).subsurface)[plane]), vn_len, &((state->Rs_at_plane)[plane]));
							outcome = sm_reflect_or_transmit_at_substrate_surface (state, sm, ws_target, sm_Reflected);
							switch (outcome) {
								
//////////////////////////////////1.4 sm_SubstrateSurfaceLayer(mirror to substrate), sm_Reflected, continue.
								case sm_Reflected: 
								{
									//location: ReflectSide, sm_SubstrateSurfaceLayer(mirror to substrate)
									
//////////////////////////////////////1.5 Absorber Layer(substrate to mirror), sm_Absorbed or sm_Transmitted-continue
									
									//sm_get_prop_abs_at_plane called in 1.3 to obtain L_attn_abs, t_prop_abs, T_prop_abs
									outcome = sm_transmit_through_absorber_at_plane (state, sm, ws_target);
									switch (outcome) { 
										case sm_Absorbed: //sm_AbsorberLayer(substrate to mirror), sm_Absorbed, return sm_Absorbed (exit module).
										{
											//location: ReflectSide, sm_AbsorberLayer(substrate to mirror)
											return sm_Absorbed;
											break; 
										}

										case sm_Transmitted: //sm_AbsorberLayer(substrate to mirror), sm_Transmitted, continue.
										{
//////////////////////////////////////////////1.5 sm_AbsorberLayer(substrate to mirror), sm_Transmitted, continue.

//////////////////////////////////////////////1.6 sm_MirrorLayer(substrate to mirror), transmitted, sm_Exited, return sm_Exited (exit module).
											
											//location: ReflectSide, sm_MirrorLayer(substrate to mirror)
											state->layer = sm_MirrorLayer; 
											set_location(state, sm);
											set_event(state, sm, sm_Transmitted);
											if ((sm->proc).is_tracking) { 
												add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
											}
											
//////////////////////////////////////////////1.7 sm_MirrorLayer(substrate to mirror), transmitted, sm_Exited, return sm_Exited (exit module).
											set_event(state, sm, sm_Exited);
											if ((sm->proc).is_tracking) {
												add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
											}

											//ReflectSide, sm_MirrorLayer
											return sm_Exited; //Exit

											break; 
										}
										
										default: 
										{
											return sm_Error;
											break; 
										}

									}
									break;
								}
								
//////////////////////////////////1.4 sm_SubstrateSurfaceLayer(mirror to substrate), sm_Transmitted, prepare substrate propagation parameters, continue.
								case sm_Transmitted: 
								{
									//ReflectSide, sm_SubstrateSurfaceLayer
									
//////////////////////////////////////update internal reflection state variables
									state->n_mirror_intersect = 1;
									state->v_exit_at_plane[state->plane] = coords_mirror(state->v, state->fn);
									state->v_exit_at_plane[1-state->plane] = state->v;
									
//////////////////////////////////////1.5 sm_SubstrateSurfaceLayer(mirror to substrate), refraction.
									
									//Update refraction and internal reflection parameters just before refracting at sm_SubstrateSurfaceLayer and entering substrate
									//note at this stage, state->n_mirror_intersect = 0
									outcome = sm_refract_inward_at_substrate_surface (state, sm);
									switch (outcome) {
										case sm_Refracted: 
										{
											//ReflectSide, sm_SubstrateSurfaceLayer
											break;
										}
										case sm_NotRefracted: 
										{
											//ReflectSide, sm_SubstrateSurfaceLayer
											break; 
										}
										default: 
										{
											return sm_Error;
											break; 
										}
									}
									
//////////////////////////////////////1.6 sm_SubstrateLayer, inc mirror intersect count, sm_Intersected, return sm_Intersected (stay in module).
									
									//delete Rm, T_prop_abs, Rs that were calculated using v before refraction. 
									state->Rm_at_plane[0][0] = F_INDETERMINED; state->Rm_at_plane[0][1] = F_INDETERMINED; state->Rm_at_plane[1][0] = F_INDETERMINED; state->Rm_at_plane[1][1] = F_INDETERMINED; 
									state->L_attn_abs_at_plane[0] = F_INDETERMINED; state->L_attn_abs_at_plane[1] = F_INDETERMINED; 
									state->t_prop_abs_at_plane[0] = F_INDETERMINED; state->t_prop_abs_at_plane[1] = F_INDETERMINED; 
									state->T_prop_abs_at_plane[0] = F_INDETERMINED; state->T_prop_abs_at_plane[1] = F_INDETERMINED; 

									state->Rs_at_plane[0] = F_INDETERMINED; state->Rs_at_plane[1] = F_INDETERMINED; 
									
									//prepare substrate propagation parameters
									state->L_attn_sub = sm_get_L_attn_sub (&((sm->mat).sub), state->v_len);
									sm_enter_substrate_layer(state, sm, sm_Intersected);
									
									return sm_Intersected;
									break;
									
								}
								
								default: 
								{
									sm_print_error(state, sm, "1.4 sm_SubstrateSurfaceLayer(mirror to substrate), sm_reflect_or_transmit_at_interface", "outcome doesn't exist.", ws_target, outcome);
									
									return sm_Error;
									break; 
								}
							}
							break; 
						}
						
						default: 
						{
							sm_print_error(state, sm, "1.3 sm_AbsorberLayer(mirror to substrate), sm_transmit_through_layer", "outcome doesn't exist.", ws_target, outcome);
							return sm_Error;
							break; 
						}
					}
					break;
				}
				
				default: 
				{
					sm_print_error(state, sm, "1.2 sm_MirrorLayer(external to mirror), sm_reflect_or_transmit_at_interface", "outcome doesn't exist.", ws_target, outcome);
					return sm_Error;
					break; 
				}
			}
			break;
		}
		
//////////1.1 sm_SubstrateEdgeLayer(external to substrate), transmitted into substrate, return sm_Transmitted (stay in module).
		case sm_SubstrateEdgeLayer: 
		{
			
//////////////1.3 sm_SubstrateLayer, sm_Intersected, return sm_Intersected (stay in module).
			
			//prepare substrate propagation parameters
			state->L_attn_sub = sm_get_L_attn_sub (&((sm->mat).sub), state->v_len);
			sm_enter_substrate_layer(state, sm, sm_Intersected);
			
			return sm_Intersected;
			break;
		}
		
		default: 
		{
			//sm_Error
			sm_print_error(state, sm, "1.1 switch (state->layer)", "state->layer is neither sm_MirrorLayer or sm_SubstrateEdgeLayer", ws_target, sm_Error);
			
			return sm_Error;
			break; 
		}
		
	}

}

int sm_internal_intersect(SimState *state, Supermirror *sm, double ws_target) 
//neutron is inside the supermirror substrate, 
//calculate intersects 2nd (if 1st is mirror), possible 3rd (if 1st is edge, 2nd is mirror), and last intersect before absorbed or transmitted out 
{
	int plane;
	double v_len = state->v_len = coords_len(state->v); 
	double vn_len = state->vn_len = fabs(coords_sp(state->v, state->fn));
	
	int outcome;
	
//////n.1 from n-1 intersect, through substrate, to attenuate along the way
	//Start at n-1 intersect, sm_SubstrateLayer
	//n.1 sm_SubstrateLayer, sm_transmit_through_layer, sm_Absorbed or sm_Transmitted-continue.
	
	//sm_SubstrateLayer
	
	outcome = sm_transmit_through_substrate(state, sm, ws_target);
	
	plane = state->plane;

	switch (outcome) {  
//////////n.1 sm_SubstrateLayer, sm_Absorbed, return sm_Absorbed (exit module)
		case sm_Absorbed: //sm_SubstrateLayer, sm_Absorbed, return sm_Absorbed (exit module).
		{
			//sm_SubstrateLayer
			return sm_Absorbed;
			break; 
		}
		
//////////n.1 sm_SubstrateEdgeLayer, sm_Exited, return sm_Exited (exit module)
		case sm_Exited: //sm_SubstrateEdgeLayer, sm_Exited, return sm_Exited (exit module).
		{
			return sm_Exited;
			break; 
		}
		
//////////n.1 sm_SubstrateLayer, sm_Transmitted, continue.
		case sm_Transmitted: //transmitted through substrate layer to the opposing mirror side, continue to next step
		{
			//sm_SubstrateLayer
			
//////////////n.2 sm_SubstrateSurfaceLayer(substract to mirror), sm_reflect_or_transmit_at_substrate_surface

			//sm_TransmittedSide or sm_ReflectedSide, sm_SubstrateSurfaceLayer
			if (state->Rs_at_plane[plane] == F_INDETERMINED || state->Rs_at_plane[plane] == D_INDETERMINED) {
				//calc R-sub parameters except the last call after internal reflections
				sm_get_Rs (&(((sm->mat).subsurface)[plane]), vn_len, &(state->Rs_at_plane[plane]));
			}
			
			outcome = sm_reflect_or_transmit_at_substrate_surface (state, sm, ws_target, sm_Transmitted); 
			switch (outcome) {
//////////////////n.2 sm_SubstrateSurfaceLayer(substrate to mirror), sm_reflect_or_transmit_at_substrate_surface: sm_Reflected, continue.
				case sm_Reflected: 
				{	
//////////////////////n.3 sm_SubstrateLayer, sm_InternalReflection, update internal reflection count, return sm_InternalReflection (stay in module).
					//sm_SubstrateLayer
					sm_enter_substrate_layer(state, sm, sm_InternalReflection);
					return sm_InternalReflection; //stay in module
					break;
				}
						
//////////////////n.2 sm_SubstrateSurfaceLayer(substrate to mirror), sm_Transmitted, continue.
				case sm_Transmitted: 
				{
//////////////////////n.3 sm_AbsorberLayer(substrate to mirror), sm_transmit_through_layer, sm_Absorbed, sm_Exited or sm_Transmitted-continue.
					if (state->n_mirror_intersect <= 2) {
						//calc R-abs parameters except the last call after internal reflections
						sm_get_prop_abs_at_plane (&(((sm->mat).abs)[plane]), v_len, vn_len, 
													&((state->L_attn_abs_at_plane)[plane]), 
													&((state->t_prop_abs_at_plane)[plane]), 
													&((state->T_prop_abs_at_plane)[plane])); 
					}
					//sm_TransmittedSide or sm_ReflectedSide
					outcome = sm_transmit_through_absorber_at_plane (state, sm, ws_target);
					switch (outcome) { 

//////////////////////////n.3 sm_AbsorberLayer(substrate to mirror), sm_Absorbed, return sm_Absorbed (exit module).
						case sm_Absorbed: 
						{
							//sm_TransmittedSide or sm_ReflectedSide, sm_AbsorberLayer
							return sm_Absorbed;
							break; 
						}
						
//////////////////////////n.3 sm_AbsorberLayer(substrate to mirror), sm_Exited, return sm_Exited (exit module)
						case sm_Exited: 
						{
							//sm_TransmittedSide or sm_ReflectedSide, sm_AbsorberLayer
							return sm_Exited;
							break; 
						}
						
//////////////////////////n.3 sm_AbsorberLayer(substrate to mirror), sm_Transmitted, continue
						case sm_Transmitted: 
						{
							//sm_TransmittedSide or sm_ReflectedSide, sm_AbsorberLayer

//////////////////////////////n.4 sm_MirrorLayer(substrate to mirror), sm_Transmitted-continue or sm_Reflected-continue.

							//sm_TransmittedSide or sm_ReflectedSide
							state->layer = sm_MirrorLayer; 
							set_location(state, sm);
							if (state->n_mirror_intersect <= 2) {
								sm_get_Rm_at_plane(((sm->mat).mir)[plane], vn_len, 
													&((state->Rm_at_plane)[plane][0]), 
													&((state->Rm_at_plane)[plane][1]));
							}
							outcome = sm_reflect_or_transmit_at_mirror(state, sm, ws_target, sm_Transmitted);
							switch (outcome) { 
						
//////////////////////////////////n.4 sm_MirrorLayer(substrate to mirror), sm_Transmitted.
								case sm_Transmitted: 
								{
									//sm_TransmittedSide or sm_ReflectedSide, sm_MirrorLayer
									
//////////////////////////////////////n.5 sm_MirrorLayer(subtrate to mirror), sm_Refracted or sm_NotRefracted
									if (state->n_mirror_intersect != state->n_surface_intersect) {
										//first intersect is not mirror --> v_exit values not set, need to calculate refraction
										sm_get_outward_refracted_v (&(((sm->mat).subsurface)[plane]), state->v, state->fn, &(state->v_exit_at_plane[plane]));
									}
									
									outcome = sm_refract_outward_at_mirror(state, sm);
									switch (outcome) {
										case sm_Refracted: 
										{
											//sm_TransmittedSide or sm_ReflectedSide, sm_SubstrateSurfaceLayer
											break;
										}
										case sm_NotRefracted: 
										{
											//sm_TransmittedSide or sm_ReflectedSide, sm_MirrorLayer
											break; 
										}
										default: 
										{
											sm_print_error(state, sm, "n.5: sm_MirrorLayer(subtrate to mirror), sm_refract_outward_at_mirror", "outcome doesn't exist.", ws_target, outcome);
											return sm_Error;
											break; 
										}
									}
								
//////////////////////////////////////n.6 sm_MirrorLayer(subtrate to mirror), sm_Exited
									set_event(state, sm, sm_Exited);
									if ((sm->proc).is_tracking) {
										add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
									}

									return sm_Exited; //Exit
									break;
								}
										
//////////////////////////////////n.4 sm_MirrorLayer(substrate to mirror), sm_Reflected, continue
								case sm_Reflected: 
								{
									//sm_TransmittedSide or sm_ReflectedSide, sm_MirrorLayer
									
//////////////////////////////////////n.5 sm_AbsorberLayer(mirror to substrate), sm_Absorbed or sm_Transmitted-continue.
										
									//sm_get_prop_abs_at_plane called in n.3 to obtain L_attn_abs, t_prop_abs, T_prop_abs
									outcome = sm_transmit_through_absorber_at_plane(state, sm, ws_target);
									switch (outcome) { 
									
//////////////////////////////////////////n.5 sm_AbsorberLayer(mirror to substrate), sm_Absorbed, return sm_Absorbed (exit module).
										case sm_Absorbed: 
										{
											//sm_TransmittedSide or sm_ReflectedSide, sm_AbsorberLayer
											return sm_Absorbed;
											break; 
										}

//////////////////////////////////////////n.5 sm_AbsorberLayer(mirror to substrate), sm_Exited, return sm_Exited (exit module).
										case sm_Exited: 
										{
											//sm_TransmittedSide or sm_ReflectedSide, sm_AbsorberLayer
											return sm_Exited;
											break; 
										}

//////////////////////////////////////////n.5 sm_AbsorberLayer(mirror to substrate), sm_Transmitted, continue.
										case sm_Transmitted: 
										{
											//sm_TransmittedSide or sm_ReflectedSide, sm_AbsorberLayer

//////////////////////////////////////////////n.6 sm_SubstrateSurfaceLayer, sm_Transmitted-continue

											//sm_TransmittedSide or sm_ReflectedSide, sm_SubstrateSurfaceLayer
											state->layer = sm_SubstrateSurfaceLayer; 
											set_location(state, sm);
											set_event(state, sm, sm_Transmitted);
											if ((sm->proc).is_tracking) {
												add_neutron_record(state->w, state->t, state->p, state->v, state->s, &(sm->proc));
											}
											
//////////////////////////////////////////////n.7 sm_SubstrateLayer, sm_Intersected, update mirror intersect count, return sm_InternalReflection (stay in module).

											//Substrate
											sm_enter_substrate_layer(state, sm, sm_InternalReflection);
											return sm_InternalReflection; //stay in module
											break; 
										}

										default: 
										{
											sm_print_error(state, sm, "n.5: sm_AbsorberLayer(mirror to substrate), sm_transmit_through_absorber_at_plane", "outcome doesn't exist.", ws_target, outcome);
											return sm_Error;
											break; 
										}
									}
								}
								default: 
								{
									sm_print_error(state, sm, "n.4: sm_MirrorLayer(substrate to mirror), sm_reflect_or_transmit_at_mirror", "outcome doesn't exist.", ws_target, outcome);
									return sm_Error;
									break; 
								}
							}
							break; 
						}
						default: 
						{
							sm_print_error(state, sm, "n.3: sm_AbsorberLayer(substrate to mirror), sm_transmit_through_absorber_at_plane", "outcome doesn't exist.", ws_target, outcome);
							return sm_Error;
							break; 
						}

					}
					
					break;
				}

				default: 
				{
					sm_print_error(state, sm, "n.2: sm_SubstrateSurfaceLayer(substract to mirror), sm_reflect_or_transmit_at_substrate_surface", "outcome doesn't exist.", ws_target, outcome);
					return sm_Error;
					break; 
				}
				
			}
			break; 
		}
		
		default: 
		{
			sm_print_error(state, sm, "n.1: sm_SubstrateLayer, sm_transmit_through_layer", "outcome doesn't exist.", ws_target, outcome);
			return sm_Error;
			break; 
		}

	}
}

int sm_internal_reflections(SimState *state, Supermirror *sm, double ws_target) 
//Find the reflection number n just before absorption or transmitting out of the supermirror, prepare trajectory for last calculation
{
//////ir.1 starting point is time to transit to supermirror edge exit and time to transit between mirrors 

	
	//ir.1 Calculate how long it takes to reach one of the edges of the supermirror using surface velocity
	//Find line-polyhedron intersects
	//state->itype: 1=x plane, 2=x edge, 3=x vertex, 4=on plane, 5=on edge
	int skip_plane[] = {0, 1};
	int n_skip_plane = 2;
	int num_intersect = 1; //only use values of the first intersect
	line_polyhedron_intersect(state->t, state->p, state->v, &(sm->geo), Maximum_On_Plane_Distance, n_skip_plane, skip_plane, 
								state->last_time, state->last_point, state->last_plane, 
								&num_intersect, state->idtime, state->idpoint, state->itime, state->ipoint, state->iplane, state->itype); 

	if (num_intersect == 0 || state->itype[0] > 3) { //something's not right
		printf("ir.1: Something's wrong. VS to reach end of mirror. Inside mirror but no intersect. Return 0 (exit module)\n");
		return sm_Error; //somethings wrong, exit calculation
	}

	double idts = state->idtime[0];
	
	//ir.1 Time it takes between each internal reflection is state->t_prop_sub

	double idtn = state->t_prop_sub;
	
	if (idts < idtn) {
		//no more internal reflection
		return sm_InternalReflection;
	}

//////ir.2. Calculate the maximum number of internal reflections n_max before the last trajectory inside the supermirror 
	int n_max = (int)floor(idts / idtn); 
	
//////Set T_ir parameters
	sm_set_internal_reflection_parameters (state, sm, ws_target);
	
//////Check if ws_target reaches end of sm?
	int sm_ir_n, m, sm_ir_r;
	double ws_n_equals_2,ir_w[2];

	//calculate the weight after maximum possible internal reflection n_max
	sm_internal_reflection_w(state, n_max, ir_w);

//////If the target weight is already smaller than 
//////the weight just after the last reflection before exiting the supermirror,  
//////neutron is either absorbed or transmitted out before or at the last internal reflection point
//////otherwise neutron goes beyond the last possible reflection point 
	if (ir_w[0]+ir_w[1] > ws_target) {//neutron goes beyond the last possible internal reflection point 
		sm_ir_n = n_max; 
	} 
	else {//neutron is either reflected out or absorbed before or at the last possible internal reflection.
		//Find largest sm_ir_n where (ir_w+ + ir_w-)>ws_target and  sm_ir_n+1 -> (ir_w+ + ir_w-)<=ws_target
		//No analytic solution so first approximate sm_ir_n, then follow the trajectory reflection by reflection
		ws_n_equals_2 = state->ws[0] * state->T_ir_at_ir_order[0][0] * state->T_ir_at_ir_order[1][0] + 
						state->ws[1] * state->T_ir_at_ir_order[0][1] * state->T_ir_at_ir_order[1][1];
		if (ws_n_equals_2 < FLT_EPSILON) {
			sm_ir_n = 0;
		}
		else {
			if (1-ws_n_equals_2 > FLT_EPSILON) { 
				sm_ir_n = (int) 2 * floor( log(ws_target) / log(ws_n_equals_2) );
			}
			else {
				sm_ir_n = n_max; 
			}
		}

		if (sm_ir_n < 0) sm_ir_n = 0; //safeguard in case the estimation goes wrong

		sm_internal_reflection_w(state, sm_ir_n, ir_w);

		if (ir_w[0]+ir_w[1] > ws_target) { //iteratively increase sm_ir_n by 1 until (ir_w+ + ir_w-) just <= ws_target, i.e. just reach or pass ws_target
			while (ir_w[0]+ir_w[1] > ws_target && sm_ir_n < n_max) {
				sm_internal_reflection_w(state, ++sm_ir_n, ir_w);
			}
		}
		if (ir_w[0]+ir_w[1] <= ws_target) { //then iteratively check sm_ir_n-1 until (ir_w+ + ir_w-)>ws_target
			while (ir_w[0]+ir_w[1] <= ws_target && sm_ir_n >= 0) {
				sm_internal_reflection_w(state, --sm_ir_n, ir_w);
			}
		}
		//Found sm_ir_n,  before (ir_w+ + ir_w-)<=ws_target
		sm_internal_reflection_w(state, sm_ir_n, ir_w);
	}
	
	sm_ir_r = 1 - (sm_ir_n) % 2; //sm_ir_n = 2*m + 1 - sm_ir_r

//////3. Update state variables to just after the internal reflection number sm_ir_n

	//state is at i_ir=0, m=0, sm_ir_r=1
	//next internal reflections jumps to i_ir=1, m=0, sm_ir_r=0
	//v=vs+vn
	//p += 	time_from_one_layer_to_another * vs * i_ir + 
	//		time_from_one_layer_to_another * vn if jumping from sm_ir_r=1(starting side) to sm_ir_r=0(the other side)
	
	//prepare 
	//dps_ir = time_from_one_layer_to_another * vs
	//dpn_ir_at_ir_order_1 = time_from_one_layer_to_another * vn
	Coords dps_ir = coords_scale(coords_xp(coords_xp(state->fn, state->v), state->fn), idtn);
	int plane = state->plane_at_ir_order[1];
	Coords fn = (sm->geo).fn[plane]; 
	Coords dpn_ir_at_ir_order_1 = coords_scale(fn, idtn * coords_sp(fn, state->v_ir_at_plane[plane])); 

	if ((sm->proc).is_tracking) {
		if (sm_ir_n > 0) {

			SimState save_state; 
			int i_ir, ir_order, plane; 
			char location[2][CHAR_BUF_LENGTH], event[CHAR_BUF_LENGTH]; 
			double save_ir_w[2];
			
			for (ir_order = 0; ir_order < SM_Num_Mirror_Planes; ir_order++) {
				set_location_text(sm, sm->name, state->side_at_ir_order[ir_order], state->plane_at_ir_order[ir_order], sm_MirrorLayer, location[ir_order]);
			}
			set_event_text(sm, sm_InternalReflection, save_state.event); 
			
			//calculate vn at sm_ir_r=1(starting side), vn_ir_at_ir_order_1
			
			for (i_ir = 1, ir_order = 0, plane = state->plane_at_ir_order[0]; i_ir <= sm_ir_n; i_ir++) {
				
				save_state = *state;
				
				save_state.plane = plane;
				
				save_state.t = state->t + i_ir * idtn;
				sm_internal_reflection_w(state, i_ir, save_ir_w);
				
				save_state.p = coords_add(state->p, coords_scale(dps_ir, i_ir));
				if (ir_order == 0) save_state.p = coords_add(save_state.p, dpn_ir_at_ir_order_1); 
				save_state.v = state->v_ir_at_plane[plane];
				save_state.s = coords_scale(sm->co.fa, (save_ir_w[0]-save_ir_w[1])/(save_ir_w[0]+save_ir_w[1]));
				
				add_neutron_record(save_state.w, save_state.t, save_state.p, save_state.v, save_state.s, &(sm->proc));
				
				//prepare for next ir_order
				ir_order = 1 - ir_order; //flip ir_order between 0 & 1 
				plane = 1 - plane; //flip plane between 0 & 1 
				
			}
		}
	}

	state->side = state->side_at_ir_order[sm_ir_r];
	state->plane = state->plane_at_ir_order[sm_ir_r];
	state->layer = sm_MirrorLayer; 
	set_location(state, sm);
	set_event(state, sm, sm_InternalReflection);
	state->fn = (sm->geo).fn[state->iplane[0]];

	//p starts at sm_ir_r=1 plane, propagates sm_ir_n reflections. 
	state->t += sm_ir_n * idtn;
	state->p = coords_add(state->p, coords_scale(dps_ir, sm_ir_n));

	//p starts at sm_ir_r=1 plane, p after sm_ir_n reflections with sm_ir_r=0 is at the other side
	if (sm_ir_r==0) state->p = coords_add(state->p, dpn_ir_at_ir_order_1); 
	
	state->v = state->v_ir_at_plane[state->plane];
	
	state->last_time = state->t;
	state->last_point = state->p;
	state->last_plane = state->plane;
	
	state->ws[0] = ir_w[0]; 
	state->ws[1] = ir_w[1];
	state->s = fabs(state->ws[0]+state->ws[1]) > DBL_EPSILON ? coords_scale( (sm->co).fa, (state->ws[0]-state->ws[1])/(state->ws[0]+state->ws[1]) ) : coords_set(0,0,0);
		
	return sm_InternalReflection; //stay in module, continue to calculate the exit

}


/*******************************/
/* Functions called by modules */
/*******************************/

int InitialiseStdSupermirrorFlat( 
		
		char *name, //Supermirror name
		
		////////////////////////////////////////////////////////////////////////////////
		//STEP 1: Specify the supermirror shape, mirror coatings and substrate material
		//In this step, the supermirror is lying horizontally on the xz plane with the long side along +z.
		double length, //m, use SI unit unless specified
		double thickness_in_mm, //mm - note the unit
		Coords side_edge_normal, Coords side_edge_point, //edges in +x and -x sides, symmetric about yz plane. 
		
		char*mirror_coated_side, //Sequential combination of keywords of
								 //position: "Both", "Top", "Bottom";
								 //surface property: "Coated", "Substrate", "NoReflection"; 
								 //e.g. "BothCoated", "BottomCoatedTopSubstrate",
								 //case-insensitive.
		char*mirror_spin_plus_material_name, //defined in "Supermirror_reflective_coating_materials.txt", non-polarising: use same for spin minus, case-insensitive
		double mirror_spin_plus_m, 
		char*mirror_spin_minus_material_name, //defined in "Supermirror_reflective_coating_materials.txt", non-polarising: use same for spin plus, case-insensitive
		double mirror_spin_minus_m, 
		
		char*absorber_coated_side, //"BothNotCoated", "BothCoated", "TopCoated", "BottomCoated" 
		char*absorber_material_name, //defined in "Supermirror_absorber_coating_materials.txt" or "Empty", case-insensitive
		double absorber_thickness_in_micron, //micrometer
		
		char*substrate_material_name, //defined in "Supermirror_substrate_materials.txt" or "Empty", case-insensitive.
		
		////////////////////////////////////////////////////////////////////////////////
		//STEP 2: Orient and position the supermirror 
		char*initial_placement_at_origin,	 //"TopFrontEdgeCentre","FrontSubstrateCentre","BottomFrontEdgeCentre"
											 //(insensitive to case and reginal English spelling)
		char*tilt_y_axis_location, 			 //"TopFrontEdge","TopMirrorCentre","TopBackEdge"
											 //"FrontSubstrateCentre","SubstrateCentre","BackSubstrateCentre", 
											 //"BottomFrontEdge","BottomMirrorCentre","BottomBackEdge"
											 //(insensitive to case and reginal English spelling)
		double tilt_about_y_first_in_degree, //degree, first, tile about y-axis at selected location 
		Coords translation_second,			 //second, translate reference point 
		double rot_about_z_third_in_degree,	 //third, rotate about global z-axis 

		////////////////////////////////////////////////////////////////////////////////
		//simulation process control parameters
		int is_tracking, //1=tracking, 0=not tracking
		
		////////////////////////////////////////////////////////////////////////////////
		//OUTPUT: Supermirror struct with all parameter values entered and initialised
		//User declares "Supermirror supermirror;" or its equivalence, 
		//then passes pointer "&supermirror" to function.
		Supermirror*sm
		) 
{ //Wrapper function to call InitialiseStdSupermirrorFlat_detail
	char *lc = 0;
	
	char top_mirror_spin_plus_material[CHAR_BUF_LENGTH], top_mirror_spin_minus_material[CHAR_BUF_LENGTH], 
		 bottom_mirror_spin_plus_material[CHAR_BUF_LENGTH], bottom_mirror_spin_minus_material[CHAR_BUF_LENGTH]; 
	double top_mirror_spin_plus_m, top_mirror_spin_minus_m, bottom_mirror_spin_plus_m, bottom_mirror_spin_minus_m; 
	if (mirror_coated_side != 0) {
		if (strlen(mirror_coated_side) != 0) {
			to_lower_case(mirror_coated_side, &lc);
		}
	}
	if (lc == 0) {
		lc = (char*)calloc(14, sizeof(char));
		strcpy(lc,"bothnotcoated");
	}
	if (strcmp(lc,"bothcoated") == 0) {
		strcpy(top_mirror_spin_plus_material, mirror_spin_plus_material_name); top_mirror_spin_plus_m = mirror_spin_plus_m;
		strcpy(top_mirror_spin_minus_material, mirror_spin_minus_material_name); top_mirror_spin_minus_m = mirror_spin_minus_m;
		strcpy(bottom_mirror_spin_plus_material, mirror_spin_plus_material_name); bottom_mirror_spin_plus_m = mirror_spin_plus_m;
		strcpy(bottom_mirror_spin_minus_material, mirror_spin_minus_material_name); bottom_mirror_spin_minus_m = mirror_spin_minus_m;
	}
	else if (strcmp(lc,"bothsubstrate") == 0 
		  || strcmp(lc,"bothsubstratesurface") == 0 
		  || strcmp(lc,"bothnocoating") == 0 
		  || strcmp(lc,"bothnotcoated") == 0 
		  || strcmp(lc,"empty") == 0) { 
		strcpy(top_mirror_spin_plus_material, "SubstrateSurface"); top_mirror_spin_plus_m = 0;
		strcpy(top_mirror_spin_minus_material, "SubstrateSurface"); top_mirror_spin_minus_m = 0;
		strcpy(bottom_mirror_spin_plus_material, "SubstrateSurface"); bottom_mirror_spin_plus_m = 0;
		strcpy(bottom_mirror_spin_minus_material, "SubstrateSurface"); bottom_mirror_spin_minus_m = 0;
	}
	else if (strcmp(lc,"bothnosurfaceboundary") == 0 
		  || strcmp(lc,"bothnoreflection") == 0) { 
		strcpy(top_mirror_spin_plus_material, "NoSurfaceBoundary"); top_mirror_spin_plus_m = 0;
		strcpy(top_mirror_spin_minus_material, "NoSurfaceBoundary"); top_mirror_spin_minus_m = 0;
		strcpy(bottom_mirror_spin_plus_material, "NoSurfaceBoundary"); bottom_mirror_spin_plus_m = 0;
		strcpy(bottom_mirror_spin_minus_material, "NoSurfaceBoundary"); bottom_mirror_spin_minus_m = 0;
	}
	else if (strcmp(lc,"topcoatedbottomsubstratesurface") == 0 || strcmp(lc,"bottomsubstratesurfacetopcoated") == 0 || 
			 strcmp(lc,"topcoatedbottomsubstrate") == 0 || strcmp(lc,"bottomsubstratetopcoated") == 0 || 
			 strcmp(lc,"topcoatedbottomnocoating") == 0 || strcmp(lc,"bottomnocoatingtopcoated") == 0) { 
		strcpy(top_mirror_spin_plus_material, mirror_spin_plus_material_name); top_mirror_spin_plus_m = mirror_spin_plus_m;
		strcpy(top_mirror_spin_minus_material, mirror_spin_minus_material_name); top_mirror_spin_minus_m = mirror_spin_minus_m;
		strcpy(bottom_mirror_spin_plus_material, "SubstrateSurface"); bottom_mirror_spin_plus_m =0;
		strcpy(bottom_mirror_spin_minus_material, "SubstrateSurface"); bottom_mirror_spin_minus_m = 0;
	}
	else if (strcmp(lc,"topsubstratesurfacebottomcoated") == 0 || strcmp(lc,"bottomcoatedtopsubstratesurface") == 0 ||
			 strcmp(lc,"topsubstratebottomcoated") == 0 || strcmp(lc,"bottomcoatedtopsubstrate") == 0 ||
			 strcmp(lc,"topnocoatingbottomcoated") == 0 || strcmp(lc,"bottomcoatedtopnocoating") == 0) {
		strcpy(top_mirror_spin_plus_material, "SubstrateSurface"); top_mirror_spin_plus_m = 0;
		strcpy(top_mirror_spin_minus_material, "SubstrateSurface"); top_mirror_spin_minus_m = 0;
		strcpy(bottom_mirror_spin_plus_material, mirror_spin_plus_material_name); bottom_mirror_spin_plus_m = mirror_spin_plus_m;
		strcpy(bottom_mirror_spin_minus_material, mirror_spin_minus_material_name); bottom_mirror_spin_minus_m = mirror_spin_minus_m;
	}
	else if (strcmp(lc,"topcoatedbottomnosurfaceboundary") == 0 || strcmp(lc,"bottomnosurfaceboundarytopcoated") == 0 || 
			 strcmp(lc,"topcoatedbottomnoreflection") == 0 || strcmp(lc,"bottomnoreflectiontopcoated") == 0) { 
		strcpy(top_mirror_spin_plus_material, mirror_spin_plus_material_name); top_mirror_spin_plus_m = mirror_spin_plus_m;
		strcpy(top_mirror_spin_minus_material, mirror_spin_minus_material_name); top_mirror_spin_minus_m = mirror_spin_minus_m;
		strcpy(bottom_mirror_spin_plus_material, "NoSurfaceBoundary"); bottom_mirror_spin_plus_m = 0;
		strcpy(bottom_mirror_spin_minus_material, "NoSurfaceBoundary"); bottom_mirror_spin_minus_m = 0;
	}
	else if (strcmp(lc,"topnosurfaceboundarybottomcoated") == 0 || strcmp(lc,"bottomcoatedtopnosurfaceboundary") == 0 || 
			 strcmp(lc,"topnoreflectionbottomcoated") == 0 || strcmp(lc,"bottomcoatedtopnoreflection") == 0) {
		strcpy(top_mirror_spin_plus_material, "NoSurfaceBoundary"); top_mirror_spin_plus_m = 0;
		strcpy(top_mirror_spin_minus_material, "NoSurfaceBoundary"); top_mirror_spin_minus_m = 0;
		strcpy(bottom_mirror_spin_plus_material, mirror_spin_plus_material_name); bottom_mirror_spin_plus_m = mirror_spin_plus_m;
		strcpy(bottom_mirror_spin_minus_material, mirror_spin_minus_material_name); bottom_mirror_spin_minus_m = mirror_spin_minus_m;
	}
	else {
		MPI_MASTER(printf("InitialiseStdSupermirrorFlat: sm_Error: mirror_coated_side must be sequential combination of keywords of\n"
							"position: \"Both\", \"Top\", \"Bottom\"; \n"
							"surface property: \"Coated\", \"Substrate\", \"NoReflection\"; \n"
							"e.g. \"BothCoated\", \"TopCoatedBottomSubstrate\", \n"
							"case-insensitive. \n"
							"Return 0.\n");)
		if (lc != 0) {
			free(lc);
			lc = 0;
		}
		return 0; 
	}
	if (lc != 0) {
		free(lc);
		lc = 0;
	}

	char abs_side[CHAR_BUF_LENGTH], abs_name[CHAR_BUF_LENGTH], top_absorber_material[CHAR_BUF_LENGTH], bottom_absorber_material[CHAR_BUF_LENGTH]; 
	double absorber_top_thickness_in_micron, absorber_bottom_thickness_in_micron; 
	strcpy(abs_side, "bothnotcoated");
	if (absorber_coated_side != 0) {
		if (strlen(absorber_coated_side) !=0) {
			strcpy(abs_side, absorber_coated_side);
		}
	}
	strcpy(abs_name, "empty");
	if (absorber_material_name != 0) {
		if (strlen(absorber_material_name) !=0) {
			strcpy(abs_name, absorber_material_name);
		}
	}
	to_lower_case(abs_side, &lc);
	if (strcmp(lc,"bothcoated") == 0) {
		strcpy(top_absorber_material, abs_name); absorber_top_thickness_in_micron = absorber_thickness_in_micron;
		strcpy(bottom_absorber_material, abs_name); absorber_bottom_thickness_in_micron = absorber_thickness_in_micron;
	}
	else if (strcmp(lc,"bothnotcoated") == 0) { 
		strcpy(top_absorber_material, "Empty"); absorber_top_thickness_in_micron = 0;
		strcpy(bottom_absorber_material, "Empty"); absorber_bottom_thickness_in_micron = 0;
	}
	else if (strcmp(lc,"topcoated") == 0 || strcmp(lc,"topcoatedbottomnotcoated") == 0 || strcmp(lc,"bottomnotcoatedtopcoated") == 0) { 
		strcpy(top_absorber_material, abs_name); absorber_top_thickness_in_micron = absorber_thickness_in_micron;
		strcpy(bottom_absorber_material, "Empty"); absorber_bottom_thickness_in_micron = 0;
	}
	else if (strcmp(lc,"bottomcoated") == 0 || strcmp(lc,"bottomcoatedtopnotcoated") == 0 || strcmp(lc,"topnotcoatedbottomcoated") == 0) {
		strcpy(top_absorber_material, "Empty"); absorber_top_thickness_in_micron = 0;
		strcpy(bottom_absorber_material, abs_name); absorber_bottom_thickness_in_micron = absorber_thickness_in_micron;
	}
	else {
		MPI_MASTER(printf("InitialiseStdSupermirrorFlat: sm_Error: absorber_coated_side must be \"BothCoated\", \"TopCoated\", \"BottomCoated\", or \"BothNotCoated\". Return 0\n");)
		if (lc != 0) {
			free(lc);
			lc = 0;
		}
		return 0; 
	}
	if (lc != 0) {
		free(lc);
		lc = 0;
	}
	
	char substrate_material[CHAR_BUF_LENGTH];
	strcpy(substrate_material, "empty");
	if (substrate_material_name != 0) {
		if (strlen(substrate_material_name) !=0) {
			strcpy(substrate_material, substrate_material_name);
		}
	}
		
	Coords n = coords_set(0, 1, 0); //reflecting +y edge to -y edge, vice versa
		
	return InitialiseStdSupermirrorFlat_detail (
		name,
		length,
		thickness_in_mm,
		side_edge_normal, side_edge_point, 
		coords_mirror(side_edge_normal, n), coords_mirror(side_edge_point, n),
		
		top_mirror_spin_plus_material, 0, top_mirror_spin_plus_m, 
		top_mirror_spin_minus_material, 0, top_mirror_spin_minus_m, 
		bottom_mirror_spin_plus_material, 0, bottom_mirror_spin_plus_m, 
		bottom_mirror_spin_minus_material, 0, bottom_mirror_spin_minus_m, 
		top_absorber_material, -1, -1, absorber_top_thickness_in_micron, 
		bottom_absorber_material, -1, -1, absorber_bottom_thickness_in_micron, 
		substrate_material, -1, -1, 0, 
		
		initial_placement_at_origin,
		tilt_y_axis_location,
		tilt_about_y_first_in_degree,
		translation_second,
		rot_about_z_third_in_degree,
		
		is_tracking,
		sm);

}

int InitialiseStdSupermirrorFlat_detail ( 
		char *name, //Supermirror name
		
		////////////////////////////////////////////////////////////////////////////////
		//STEP 1: Specify the supermirror shape, mirror coatings and substrate material
		//In this step, the supermirror is lying horizontally on the xz plane with the long side along +z.
		double length, //m, use SI unit unless specified
		double thickness_in_mm, //in mm - note the unit
		Coords side_edge_1_normal, Coords side_edge_1_point, //edge in +x or -x side 
		Coords side_edge_2_normal, Coords side_edge_2_point, //edge in +x or -x side 
		
		//If materials name is specified and matching those defined in "Supermirror_reflective_coating_materials.txt" 
		//material name overrides material spec, otherwise 6-parameter material spec is used.  
		//Use the same for spin plus and spin minus if the mirror is non-polarising
		char*mirror_top_spin_plus_material_name, double*mirror_top_spin_plus_material_spec, double mirror_top_spin_plus_m, 
		char*mirror_top_spin_minus_material_name, double*mirror_top_spin_minus_material_spec, double mirror_top_spin_minus_m, 
		char*mirror_bottom_spin_plus_material_name, double*mirror_bottom_spin_plus_material_spec, double mirror_bottom_spin_plus_m, 
		char*mirror_bottom_spin_minus_material_name, double*mirror_bottom_spin_minus_material_spec, double mirror_bottom_spin_minus_m, 

		//INPUT: Absorber parameters are absorption paramter and thickness in microns: 
		//If materials name is specified and matching those defined in "Supermirror_absorber_coating_materials.txt"  
		//material name overrides material spec, otherwise 1-parameter material spec is used.  
		char*absorber_top_material_name, double absorber_top_L_abs, double absorber_top_L_inc, double absorber_top_thickness_in_micron,
		char*absorber_bottom_material_name, double absorber_bottom_L_abs, double absorber_bottom_L_inc, double absorber_bottom_thickness_in_micron,

		//INPUT: Substrate parameters are substrate attenuation paramters and SLD: 
		//If materials name is specified and matching those defined in "Supermirror_substrate_materials.txt" or is "Empty",  
		//material name overrides material spec, otherwise the 3 material parameters are used. 
		//Name is case-insensitive. 0 if not specified. 
		char*substrate_material_name, double substrate_L_abs, double substrate_L_inc, double substrate_SLD, 

		////////////////////////////////////////////////////////////////////////////////
		//STEP 2: Orient and position the supermirror 
		char*initial_placement_at_origin,	 //"TopFrontEdgeCentre","FrontSubstrateCentre","BottomFrontEdgeCentre"
											 //(insensitive to case and reginal English spelling)
		char*tilt_y_axis_location, 			 //"TopFrontEdge","TopMirrorCentre","TopBackEdge"
											 //"FrontSubstrateCentre","SubstrateCentre","BackSubstrateCentre", 
											 //"BottomFrontEdge","BottomMirrorCentre","BottomBackEdge"
											 //(insensitive to case and reginal English spelling)
		double tilt_about_y_first_in_degree, //degree, first, tile about y-axis at selected location 
		Coords translation_second,			 //second, translate reference point 
		double rot_about_z_third_in_degree,	 //third, rotate about global z-axis 

		////////////////////////////////////////////////////////////////////////////////
		//simulation process control parameters
		int is_tracking, //1=tracking, 0=not tracking
		
		////////////////////////////////////////////////////////////////////////////////
		//OUTPUT: Supermirror struct with all parameter values entered and initialised
		//User declares "Supermirror supermirror;" or its equivalence, 
		//then passes pointer "&supermirror" to function.
		Supermirror*sm
		) 
{ //Initialisation function

	int i,j,k;
	if (sm->proc.initialised != 0) { 
		EmptySupermirrorFlatData(sm); 
	}
	

	if (name != 0) strcpy(sm->name,name);
	
//	sm->geo = (void*)calloc(1, sizeof(Polyhedron));
	if (set_supermirror_flat_geometry(
			length, thickness_in_mm, //mm
			side_edge_1_normal, side_edge_1_point, 
			side_edge_2_normal, side_edge_2_point, 
			initial_placement_at_origin, 
			tilt_y_axis_location, tilt_about_y_first_in_degree, 
			translation_second, 
			rot_about_z_third_in_degree, 
			&(sm->geo), &(sm->co)) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading geometry\n");)
			return 0; //initialisation failed
	}

	//substrate parameters to be set first. sub->sub_type may be overrided when setting mirror and absorber parameters
	if (set_substrate_material_parameters(
			substrate_material_name, substrate_L_abs, substrate_L_inc, substrate_SLD,  
			&((sm->mat).sub), (sm->mat).subsurface) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading substrate parameters\n");)
			return 0; //initialisation failed
	}
	
	
	//mirror parameters to be set first. 
	if (set_mirror_material_parameters(
			mirror_top_spin_plus_material_name, mirror_top_spin_plus_material_spec, mirror_top_spin_plus_m, 
			&((sm->mat).mir[0][0]), 
			&((sm->mat).subsurface[0])) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading top mirror spin+ parameters\n");)
			return 0; //initialisation failed
	}
	if (set_mirror_material_parameters(
			mirror_top_spin_minus_material_name, mirror_top_spin_minus_material_spec, mirror_top_spin_minus_m, 
			&((sm->mat).mir[0][1]), 
			&((sm->mat).subsurface[0])) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading top mirror spin- parameters\n");)
			return 0; //initialisation failed
	}
	if (set_mirror_material_parameters(
			mirror_bottom_spin_plus_material_name, mirror_bottom_spin_plus_material_spec, mirror_bottom_spin_plus_m, 
			&((sm->mat).mir[1][0]), 
			&((sm->mat).subsurface[1])) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading bottom mirror spin+ parameters\n");)
			return 0; //initialisation failed
	}
	if (set_mirror_material_parameters(
			mirror_bottom_spin_minus_material_name, mirror_bottom_spin_minus_material_spec, mirror_bottom_spin_minus_m, 
			&((sm->mat).mir[1][1]), 
			&((sm->mat).subsurface[1])) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading bottom mirror spin- parameters\n");)
			return 0; //initialisation failed
	}
			
	//absorber parameters to be set last. may override mir->refl_type (if bare substrate) and sub->sub_type (no reflection or refraction)
	if (set_absorber_material_parameters(
			absorber_top_material_name, absorber_top_L_abs, absorber_top_L_inc, absorber_top_thickness_in_micron, 
			&((sm->mat).abs[0])) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading top abs parameters\n");)
			return 0; //initialisation failed
	}
	if (set_absorber_material_parameters(
			absorber_bottom_material_name, absorber_bottom_L_abs, absorber_bottom_L_inc, absorber_bottom_thickness_in_micron, 
			&((sm->mat).abs[1])) == 0) {
			MPI_MASTER(printf("InitializeStdSupermirrorFlatFunc - error loading bottom abs parameters\n");)
			return 0; //initialisation failed
	}
		
	set_process_parameters(is_tracking, &(sm->proc));
			
	sm->proc.initialised = 1;
	
	#if USE_MPI
		MPI_Barrier(MPI_COMM_WORLD);
	#endif

	FILE*fpp;

	MPI_MASTER (
		char refl_check_file_path[CHAR_BUF_LENGTH]; 
		sprintf(refl_check_file_path, "%s%srefl_check.csv", dirname, MC_PATHSEP_S);
		fpp=fopen(refl_check_file_path,"r");
		if (fpp) { 
			fclose(fpp);
		}
		else {
			ReflectionParameters*mir[2][2]; 
			for (i = 0; i < SM_Num_Mirror_Planes; i++)
			for (j = 0; j < SM_Num_Spin_States; j++) {
				mir[i][j] = &((sm->mat).mir[i][j]);
			}
			int save_refl = 0; 
			for (i = 0; i < SM_Num_Mirror_Planes; i++) {
				if (mir[i][0]->m != mir[i][1]->m) {
					save_refl = 1;
				}
			}
			
			if (save_refl == 1) {
 				fpp = fopen(refl_check_file_path,"w+");
				if (fpp != 0) {
					fprintf(fpp,"name,R0,Qc,alpha,m,W,beta,SLD,Qc_sub m_plot_scale\n");
					
					double m_plot_scale = 0;
					double qc = 0.0217;
					
					for (i = 0; i < SM_Num_Mirror_Planes; i++)
					for (j = 0; j < SM_Num_Spin_States; j++) {
						m_plot_scale = MAX(m_plot_scale, mir[i][j]->m * 1.2);
						fprintf(fpp,"%s, % 5f, % 5f, % 5f, % 5f, % 5f, % 5f, % 5f, % 5f\n",
								mir[i][j]->name, mir[i][j]->R0, mir[i][j]->Qc, mir[i][j]->alpha, mir[i][j]->m, mir[i][j]->W, mir[i][j]->beta, 
								((sm->mat).sub).SLD, (((sm->mat).subsurface)[i]).Qc_sub, m_plot_scale);
						if (qc < mir[i][j]->Qc) qc = mir[i][j]->Qc;
					}
					
					m_plot_scale /= 100;
					fprintf(fpp,"Q,Rm_at_plane[0][+],Rm_at_plane[0][-],Rm_at_plane[1][+],Rm_at_plane[1][-]\n");
					double q; 
					for (k = 0; k < 100; k++) {
						q = qc * k * m_plot_scale; 
						fprintf(fpp, "%le",q);
						for (i = 0; i < SM_Num_Mirror_Planes; i++)
						for (j = 0; j < SM_Num_Spin_States; j++) {
							fprintf(fpp, ",%12.9e",sm_calc_Rm(q, mir[i][j]->Qc, mir[i][j]->R0, mir[i][j]->alpha, mir[i][j]->m, mir[i][j]->W, mir[i][j]->beta));
						}
						fprintf(fpp, "\n");
					}
					fclose(fpp);
				}
			}
		}
	)

	return 1; //initialisation succeed
}

int IntersectStdSupermirrorFlat(
		//Calculate the supermirror intersect time and position of a neutron from outside the supermirror 
		//return values: sm_Intersected, sm_Missed, sm_Error in "typedef enum SupermirrorEventCode" in header file
		//note: sm_Missed = neutron either skips supermirror, or intersect only once (i.e. one edge or one vertex), or flies on plane or vertex
		//INPUT
		double t_in, Coords p_in, Coords v_in, 
		double last_time, Coords last_point, int last_plane, 
		Supermirror *sm, 
		//OUTPUT 
		//User declare one or more parameters, e.g. "int num_intersect; double first_intersect_time; Coords first_intersect_point; int first_intersect_plane;", 
		//then passes pointers "&num_intersect, &first_intersect_time, &first_intersect_point, &first_intersect_plane" to function.
		//Pass 0 if not needed.
		int *num_intersect, 
		double*first_intersect_dtime, Coords*first_intersect_dpoint, double*first_intersect_time, Coords*first_intersect_point, int*first_intersect_plane) 
{
	if (num_intersect) *num_intersect = 0;
	if (first_intersect_dtime) *first_intersect_dtime = F_INDETERMINED;
	if (first_intersect_dpoint) *first_intersect_dpoint = coords_set(F_INDETERMINED,F_INDETERMINED,F_INDETERMINED);
	if (first_intersect_time) *first_intersect_time = F_INDETERMINED;
	if (first_intersect_point) *first_intersect_point = coords_set(F_INDETERMINED,F_INDETERMINED,F_INDETERMINED);
	if (first_intersect_plane) *first_intersect_plane = I_INDETERMINED;
	
	//Find line-polyhedron intersects
	//state.itype: 1=x plane, 2=x edge, 3=x vertex, 4=on plane, 5=on edge

	SimState state;
	sm_initialise_state(&state);
				
	int n = 1; //only use values of the first intersect
	line_polyhedron_intersect(t_in, p_in, v_in, &(sm->geo), Maximum_On_Plane_Distance, 0, 0, 
								last_time, last_point, last_plane,  
								&n, state.idtime, state.idpoint, state.itime, state.ipoint, state.iplane, state.itype); 

	if (n < 2 || state.itype[0] > 3) {
		//Neutron either does not intersect, intersect edge or vertex, or flies on plane or vertex, skip
		return sm_Missed; //missed, exit calculation
	}
	
	if (num_intersect) *num_intersect = n;
	if (first_intersect_dtime) *first_intersect_dtime = state.idtime[0];
	if (first_intersect_dpoint) *first_intersect_dpoint = state.idpoint[0];
	if (first_intersect_time) *first_intersect_time = state.itime[0];
	if (first_intersect_point) *first_intersect_point = state.ipoint[0];
	if (first_intersect_plane) *first_intersect_plane = state.iplane[0];
	
	return sm_Intersected; //intersected, exit calculation
}

int StdSupermirrorFlat(
		//Calculates how incoming neutron propagates through the supermirror
		//return values: sm_Exited, sm_Absorbed, sm_Error in "typedef enum SupermirrorEventCode" in header file 
		//INPUT
		double *w_sm, double *t_sm, Coords *p_sm, Coords *v_sm, Coords *s_sm, 
		double*last_time, Coords*last_point, int*last_plane, 
		Supermirror *sm) 
{
	SimState state;
	sm_initialise_state(&state);
	
	state.ray_count = mcrun_num;
	
	state.w = *w_sm; 
	state.t = *t_sm; 
	state.p = *p_sm; 
	state.v = *v_sm; 
	
	state.s = *s_sm;
	double s_len = coords_sp((sm->co).fa, *s_sm); //incident polarisation is projected w.r.t. field-axis to determine polarisation
	state.ws[0] = (1+s_len)/2; state.ws[1] = (1-s_len)/2;
	
	state.last_time = *last_time; 
	state.last_point = *last_point; 
	state.last_plane = *last_plane;
	
	empty_neutron_record(&(sm->proc));
		
	int outcome; 


//////Target weight associate with exit event or attenuation
	double ws_target = rand01();

//////1st intersect

	outcome = sm_external_intersect(&state, sm, ws_target);
	
	switch (outcome) {
		case sm_Intersected: 
			break; //internal reflection, continue to next step
		case sm_Missed: 
			sm_print_state("sm_external_intersect 1: sm_Missed", &state, sm, ws_target);
			printf("StdSupermirrorFlat: sm_external_intersect outcome = %s(%d), should've been caught in IntersectStdSupermirrorFlat(...), return sm_Error.\n\n",
				((sm->proc).event)[outcome], outcome); 
			return sm_Error; break; //neutron skips module, should have been caught in function IntersectStdSupermirrorFlat(...)
		case sm_Exited: 
			*w_sm = state.w; 
			*t_sm = state.t; 
			*p_sm = state.p; 
			*v_sm = state.v; 
			*s_sm = state.s;

			*last_time = state.t;
			*last_point = state.p;
			*last_plane = state.plane;

			return sm_Exited; break; //scattered
		case sm_Absorbed:
			*w_sm = state.w; 
			*t_sm = state.t; 
			*p_sm = state.p; 
			*v_sm = state.v; 
			*s_sm = state.s;
			return sm_Absorbed; break; //absorbed
		case sm_Error: 
			sm_print_state("sm_external_intersect 1: sm_Error", &state, sm, ws_target);
			printf("StdSupermirrorFlat: sm_external_intersect outcome = %s(%d), something's wrong, return sm_Error.\n\n", 
				((sm->proc).event)[outcome], outcome);
			return sm_Error; break; //something's wrong
		default: 
			sm_print_state("sm_external_intersect 1: sm_Error", &state, sm, ws_target);
			printf("StdSupermirrorFlat: sm_external_intersect outcome = %d, something's wrong, return sm_Error.\n\n", outcome); 
			return sm_Error; break; //something's wrong
	}

//////2nd intersect
	outcome = sm_internal_intersect(&state, sm, ws_target);
	
	*last_time = state.last_time;
	last_point->x = (state.last_point).x;
	last_point->y = (state.last_point).y;
	last_point->z = (state.last_point).z;
	*last_plane = state.last_plane;
	
	switch (outcome) {
		case  sm_InternalReflection: 
			break; //internal reflection, continue to next step
		case  sm_Exited: 
			*w_sm = state.w; 
			*t_sm = state.t; 
			*p_sm = state.p; 
			*v_sm = state.v; 
			*s_sm = state.s;

			*last_time = state.t;
			*last_point = state.p;
			*last_plane = state.plane;

			return sm_Exited; break; //scattered
		case  sm_Absorbed:
			*w_sm = state.w; 
			*t_sm = state.t; 
			*p_sm = state.p; 
			*v_sm = state.v; 
			*s_sm = state.s;
			return sm_Absorbed; break; //absorbed
		case  sm_Error: 
			sm_print_state("sm_internal_intersect 2: sm_Error", &state, sm, ws_target);
			printf("StdSupermirrorFlat: %d sm_internal_intersect outcome = %s(%d), something's wrong, return sm_Error.\n\n", 
				state.n_surface_intersect, ((sm->proc).event)[outcome], outcome);
			return sm_Error; break; //something's wrong
		default: 
			sm_print_state("sm_internal_intersect 2: sm_Error", &state, sm, ws_target);
			printf("StdSupermirrorFlat: %d sm_internal_intersect outcome = %d, something's wrong, return sm_Error.\n\n", 
				state.n_surface_intersect, outcome); 
			return sm_Error; break; //something's wrong
	}

//////3rd intersect
	if (state.n_mirror_intersect < SM_Num_Mirror_Planes) {

		outcome = sm_internal_intersect(&state, sm, ws_target);
		
		switch (outcome) {
			case  sm_InternalReflection: 
				break; //internal reflection, continue to next step
			case  sm_Exited: 
				*w_sm = state.w; 
				*t_sm = state.t; 
				*p_sm = state.p; 
				*v_sm = state.v; 
				*s_sm = state.s;
			
				*last_time = state.t;
				*last_point = state.p;
				*last_plane = state.plane;
					
				return sm_Exited; break; //scattered
			case  sm_Absorbed:
				*w_sm = state.w; 
				*t_sm = state.t; 
				*p_sm = state.p; 
				*v_sm = state.v; 
				*s_sm = state.s;
				return sm_Absorbed; break; //absorbed
			case  sm_Error: 
				sm_print_state("sm_internal_intersect 3: sm_Error", &state, sm, ws_target);
				printf("StdSupermirrorFlat: %d sm_internal_intersect outcome = %s(%d), something's wrong, return sm_Error.\n\n", 
					state.n_surface_intersect, ((sm->proc).event)[outcome], outcome);
				return sm_Error; break; //something's wrong
			default: 
				sm_print_state("sm_internal_intersect 3: sm_Error", &state, sm, ws_target);
				printf("StdSupermirrorFlat: %d sm_internal_intersect outcome = %d, something's wrong, return sm_Error.\n\n", 
					state.n_surface_intersect, outcome); 
				return sm_Error; break; //something's wrong
		}
	}

//////Internal reflections
	if (state.n_mirror_intersect == 2) {
		outcome = sm_internal_reflections(&state, sm, ws_target);

		switch (outcome) {
			case  sm_InternalReflection: 
				break; //internal reflection, continue to next step
			case  sm_Error:
				sm_print_state("sm_internal_reflections: sm_Error", &state, sm, ws_target);
				printf("StdSupermirrorFlat: sm_internal_reflections = %s(%d), something's wrong, return sm_Error.\n\n", 
					((sm->proc).event)[outcome], outcome);
				return sm_Error; 
				break; //something's wrong
			default: 
				sm_print_state("sm_internal_reflections: sm_Error", &state, sm, ws_target);
				printf("StdSupermirrorFlat: sm_internal_reflections outcome = %d, something's wrong, return sm_Error.\n\n", 
						outcome); 
				return sm_Error; 
				break; //something's wrong
		}
	}

//////Last intersect
	outcome = sm_internal_intersect(&state, sm, ws_target);
		
	switch (outcome) {
		case sm_Exited:
			*w_sm = state.w; 
			*t_sm = state.t; 
			*p_sm = state.p; 
			*v_sm = state.v; 
			*s_sm = state.s;
			
			*last_time = state.t;
			*last_point = state.p;
			*last_plane = state.plane;
			
			return sm_Exited; break; //scattered
		case sm_Absorbed:
			*w_sm = state.w; 
			*t_sm = state.t; 
			*p_sm = state.p; 
			*v_sm = state.v; 
			*s_sm = state.s;
			return sm_Absorbed; break; //absorbed
		case sm_InternalReflection: 
				sm_print_state("sm_internal_intersect 4: sm_Error", &state, sm, ws_target);
				printf("StdSupermirrorFlat: %d sm_internal_intersect outcome = %s(%d), neutron internally reflected at last internal intersect, something's wrong, return sm_Error.\n", 
					state.n_surface_intersect, ((sm->proc).event)[outcome], outcome);
				return sm_Error; break; //internal reflection, something's wrong
		case sm_Error: 
				sm_print_state("sm_internal_intersect 4: sm_Error", &state, sm, ws_target);
				printf("StdSupermirrorFlat: %d sm_internal_intersect outcome = %s(%d), something's wrong, return sm_Error.\n\n", 
					state.n_surface_intersect, ((sm->proc).event)[outcome], outcome);
				return sm_Error; break; //something's wrong
		default: 
				sm_print_state("sm_internal_intersect 4: sm_Error", &state, sm, ws_target);
				printf("StdSupermirrorFlat: %d sm_internal_intersect outcome = %d, something's wrong, return sm_Error.\n\n", 
					state.n_surface_intersect, outcome); 
				return sm_Error; break; //something's wrong
	}

//////Something's wrong if it gets here	
	printf("StdSupermirrorFlat: SOMETHING'S WRONG, PASSED LAST INTERSECT. Return sm_Error.\n\n");
	return sm_Error;
}

void EmptySupermirrorFlatData(Supermirror*sm) 
{//Clean up when finished all MC calculations
	if (sm->proc.initialised != 0) {
		empty_polyhedron(&(sm->geo));
	}	
	if (sm->proc.is_tracking) {
		free_neutron_record(&(sm->proc));
	}
	sm->proc.initialised = 0;
}


#endif

/* end of ref-lib.c */

