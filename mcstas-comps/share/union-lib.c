/*******************************************************************************
*
*  McStas, neutron ray-tracing package
*  Copyright(C) 2007 Risoe National Laboratory.
*
* %I
* Written by: Mads Bertelsen
* Date: 20.08.15
* Version: $Revision: 0.1 $
* Origin: University of Copenhagen
*
* Functions and structure definitons for Union components.
*
******************************************************************************/

// -------------    Definition of data structures   ---------------------------------------------
// GPU
enum shape {
  surroundings,
  box,
  sphere,
  cylinder,
  cone,
  mesh
};

enum process {
  Incoherent,
  Powder,
  Single_crystal,
  AF_HB_1D,
  PhononSimple,
  Texture,
  IncoherentPhonon,
  NCrystal,
  Non,
  Template
};

enum surface {
  Mirror,
  SurfaceTemplate  	
};

enum in_or_out {
	inward_bound,
	outward_bound
};

struct intersection_time_table_struct {
int num_volumes;
int *calculated;
int *n_elements;
double **intersection_times;
double **normal_vector_x;
double **normal_vector_y;
double **normal_vector_z;
int **surface_index;
};

struct line_segment{
Coords point1;
Coords point2;
int number_of_dashes;
};

struct pointer_to_1d_int_list {
int num_elements;
int *elements;
#pragma acc shape(elements[0:num_elements]) init_needed(num_elements)
};

struct pointer_to_1d_double_list {
int num_elements;
double *elements;
#pragma acc shape(elements[0:num_elements]) init_needed(num_elements)
};

struct pointer_to_1d_coords_list {
int num_elements;
Coords *elements;
#pragma acc shape(elements[0:num_elements]) init_needed(num_elements)
};

struct lines_to_draw{
int number_of_lines;
struct line_segment *lines;
#pragma acc shape(lines[0:number_of_lines]) init_needed(number_of_lines)
};

// Todo: see if the union geometry_parameter_union and other geometry structs can be here
union geometry_parameter_union{
    struct sphere_storage   *p_sphere_storage;
    struct cylinder_storage *p_cylinder_storage;
    struct box_storage      *p_box_storage;
    struct cone_storage     *p_cone_storage;
    struct mesh_storage     *p_mesh_storage;
    // add as many pointers to structs as wanted, without increasing memory footprint.
};


struct rotation_struct{
double x;
double y;
double z;
};

struct focus_data_struct {
Coords RayAim; // Vector from ray position (within geometry) to target
Coords Aim; // Vector from geometry to target
double angular_focus_width;
double angular_focus_height;
double spatial_focus_width;
double spatial_focus_height;
double spatial_focus_radius;
Rotation absolute_rotation;
// focusing_function creates a vector per selected criteria of focus_data_struct / selected focus function and returns solid angle
void (*focusing_function)(Coords*, double*, struct focus_data_struct*);
//                        v_out  , solid_a,
};

struct focus_data_array_struct
{
struct focus_data_struct *elements;
int num_elements;
#pragma acc shape(elements[0:num_elements]) init_needed(num_elements)
};

struct Detector_3D_struct {
  char title_string[256];
  char string_axis_1[256];
  char string_axis_2[256];
  char string_axis_3[256];
  char Filename[256];
  double D1min;
  double D1max;
  double D2min;
  double D2max;
  double D3min;
  double D3max;
  double bins_1; // McStas uses doubles for bin numbers for some reason
  double bins_2;
  double bins_3;
  double ***Array_N; // McStas uses doubles for number of rays in each bin for some reason
  double ***Array_p;
  double ***Array_p2;
};

struct Detector_2D_struct {
  char title_string[256];
  char string_axis_1[256];
  char string_axis_2[256];
  char Filename[256];
  double D1min;
  double D1max;
  double D2min;
  double D2max;
  double bins_1; // McStas uses doubles for bin numbers for some reason
  double bins_2;
  double **Array_N; // McStas uses doubles for number of rays in each bin for some reason
  double **Array_p;
  double **Array_p2;
};

struct Detector_1D_struct {
  char title_string[256];
  char string_axis[256];
  char string_axis_short[64];
  char string_axis_value[256];
  char Filename[256];
  double min;
  double max;
  double bins; // McStas uses doubles for bin numbers for some reason
  double *Array_N; // McStas uses doubles for number of rays in each bin for some reason
  double *Array_p;
  double *Array_p2;
};


union logger_data_union{
  struct a_2DQ_storage_struct *p_2DQ_storage;
  struct a_2DS_storage_struct *p_2DS_storage;
  struct a_3DS_storage_struct *p_3DS_storage;
  struct a_1D_storage_struct *p_1D_storage;
  struct a_2DS_t_storage_struct *p_2DS_t_storage;
  struct a_2D_kf_storage_struct *p_2D_kf_storage;
  struct a_2D_kf_t_storage_struct *p_2D_kf_t_storage;
  // Additional logger storage structs to be addedd
};

struct logger_with_data_struct {
  int used_elements;
  int allocated_elements;
  struct logger_struct **logger_pointers;
};

// logger_pointer_struct
// contains pointers to the different logger functions and it's data union
struct logger_pointer_set_struct {
  // The logger has two record functions, an active and an inactive. Normally the active one will be to permanent storage,
  //  but if a conditional has been defined, it can switch the two, making the active one recording to temporary, which
  //  can then be filtered based on the future path of the ray
  
  // function input Coords position, k[3], k_old[3], p, p_old, NV, NPV, N, logger_data_union, logger_with_data_struct
  void (*active_record_function)(Coords*, double*, double*, double, double, double, int, int, int, struct logger_struct*, struct logger_with_data_struct*);
  void (*inactive_record_function)(Coords*, double*, double*, double, double, double, int, int, int, struct logger_struct*, struct logger_with_data_struct*);
  
  // A clear temporary data function (for new ray)
  void (*clear_temp)(union logger_data_union*);
  
  // Write temporary to permanent is used when the record to temp function is active, and the condition is met.
  void (*temp_to_perm)(union logger_data_union*);
  
  // Write temporary final_p to permanent is used when the record to temp function is active, and the condition is met
  //  and the final weight is given to be used for all stored events in the logger.
  void (*temp_to_perm_final_p)(union logger_data_union*, double);
  
  // Select which temp_to_perm function to use
  int select_t_to_p; // 1: temp_to_perm, 2: temp_to_perm_final_p
  
};

union abs_logger_data_union{
  struct a_2D_abs_storage_struct *p_2D_abs_storage;
  struct a_1D_abs_storage_struct *p_1D_abs_storage;
  struct a_1D_time_abs_storage_struct *p_1D_time_abs_storage;
  struct a_1D_time_to_lambda_abs_storage_struct *p_1D_time_to_lambda_abs_storage;
  struct a_event_abs_storage_struct *p_event_abs_storage;
  struct a_1D_event_abs_storage_struct *p_1D_event_abs_storage;
  struct a_nD_abs_storage_struct *p_nD_abs_storage;
  struct a_time_abs_storage_struct  *p_time_abs_storage;
  // Additional logger storage structs to be addedd
};

struct abs_logger_with_data_struct {
  int used_elements;
  int allocated_elements;
  struct abs_logger_struct **abs_logger_pointers;
};

struct abs_logger_pointer_set_struct {
  // The logger has two record functions, an active and an inactive. Normally the active one will be to permanent storage,
  //  but if a conditional has been defined, it can switch the two, making the active one recording to temporary, which
  //  can then be filtered based on the future path of the ray
  
  // function input Coords position, k[3], p, NV, N, logger_data_union, logger_with_data_struct
  void (*active_record_function)(Coords*, double*, double,  double, int, int, struct abs_logger_struct*, struct abs_logger_with_data_struct*);
  void (*inactive_record_function)(Coords*, double*, double,  double, int, int, struct abs_logger_struct*, struct abs_logger_with_data_struct*);
  
  // A clear temporary data function (for new ray)
  void (*clear_temp)(union abs_logger_data_union*);
  
  // Write temporary to permanent is used when the record to temp function is active, and the condition is met.
  void (*temp_to_perm)(union abs_logger_data_union*);
  
  // Write temporary final_p to permanent is used when the record to temp function is active, and the condition is met
  //  and the final weight is given to be used for all stored events in the logger.
  void (*temp_to_perm_final_p)(union abs_logger_data_union*, double);
  
  // Select which temp_to_perm function to use
  //int select_t_to_p; // 1: temp_to_perm, 2: temp_to_perm_final_p
  
};


struct conditional_standard_struct{
  // Data to be transfered to the conditional function
  double Emax;
  double Emin;
  int E_limit;
  
  double Tmin;
  double Tmax;
  int T_limit;
  
  int volume_index;
  
  double Total_scat_max;
  double Total_scat_min;
  int Total_scat_limit;
  
  double exit_volume_index;
  
  // Test
  Coords test_position;
  Rotation test_rotation;
  Rotation test_t_rotation;
};

struct conditional_PSD_struct{
  double PSD_half_xwidth;
  double PSD_half_yheight;
  
  double Tmin;
  double Tmax;
  int T_limit;
  
  // Position of the PSD
  Coords PSD_position;
  Rotation PSD_rotation;
  Rotation PSD_t_rotation;
};

union conditional_data_union {
  struct conditional_standard_struct *p_standard;
  struct conditional_PSD_struct *p_PSD;
  // Add more as conditional components are made
};

// General input for conditional functions: Position, Velocity/Wavevector, weight, time, total_scat, scattered_flag, scattered_flag_VP,
// Optional extras: data_union? tree base(s)? tree base for current ray?
typedef int (*conditional_function_pointer)(union conditional_data_union*,Coords*, Coords*, double*, double*, int*, int*, int*, int**);
//typedef int (**conditional_function_pointer_array)(union *conditional_data_union,Coords*, Coords*, double*, double*, int*, int*, int**);

struct conditional_list_struct{
  int num_elements;
  
  union conditional_data_union **p_data_unions;
  conditional_function_pointer *conditional_functions;
  //int (**conditional_functions)(Coords*, Coords*, double*, double*, int*, int*, int**);
};

struct logger_struct {
  char name[256];
  // Contains ponters to all the functions assosiated with this logger
  struct logger_pointer_set_struct function_pointers;
  // Contains hard copy of logger_data_union since the size is the same as a pointer.
  union logger_data_union data_union;
  
  int logger_extend_index; // Contain index conditional_extend_array defined in master that can be acsessed from extend section.
  
  struct conditional_list_struct conditional_list;
};


// To be stored in volume, a list of pointers to the relevant loggers corresponding to each process
struct logger_for_each_process_list {
  int num_elements;
  struct logger_struct **p_logger_process;
};

// List of logger_for_each_process_list
struct loggers_struct {
  int num_elements;
  struct logger_for_each_process_list *p_logger_volume;
  #pragma acc shape(p_logger_volume[0:num_elements]) init_needed(num_elements)
};


struct abs_logger_struct {
  char name[256];
  // Contains ponters to all the functions assosiated with this logger
  struct abs_logger_pointer_set_struct function_pointers;
  // Contains hard copy of logger_data_union since the size is the same as a pointer.
  union abs_logger_data_union data_union;
  
  // Position and rotation of the abs_logger
  Coords position;
  Rotation rotation;
  Rotation t_rotation;
  
  int abs_logger_extend_index; // Contain index conditional_extend_array defined in master that can be acsessed from extend section.
  
  struct conditional_list_struct conditional_list;
};

// To be stored in volume, a list of pointers to the relevant abs loggers corresponding to each process
/*
struct abs_logger_for_each_process_list {
  int num_elements;
  struct abs_logger_struct **p_abs_logger_process;
};
*/

// List of abs logger_for_each_process_list
struct abs_loggers_struct {
  int num_elements;
  //struct abs_logger_for_each_process_list *p_abs_logger_volume;
  struct abs_logger_struct **p_abs_logger;
};



struct geometry_struct
{
char shape[64];     // name of shape used (sphere, cylinder, box, off, ...)
enum shape eShape;  // enum with shape for flexible functions GPU
double priority_value;    // priority of the geometry
Coords center;      // Center position of volume, reported by components in global frame, updated to main frame in initialize
// Rotation of this volume
Rotation rotation_matrix; // rotation matrix of volume, reported by component in global frame, updated to main frame in initialize
Rotation transpose_rotation_matrix; // As above
// Array of prrotation matrixes for processes assigned to this volume (indexed by non_isotropic_rot_index in the processes)
Rotation *process_rot_matrix_array;             // matrix that transforms from main coordinate system to local process in this specific volume
Rotation *transpose_process_rot_matrix_array;   // matrix that transforms from local process in this specific volume to main coordinate system
int process_rot_allocated;  // Keeps track of allocation status of rot_matrix_array

struct rotation_struct rotation; // Not used, is the x y and z rotation angles.
int visualization_on; // If visualization_on is true, the volume will be drawn in mcdisplay, otherwise not
int is_exit_volume; // If is exit volume = 1, the ray will exit the component when it enters this volume.
int is_mask_volume; // 1 if volume itself is a mask (masking the ones in it's mask list), otherwise 0
int mask_index;
int is_masked_volume; // 1 if this volume is being masked by another volume, the volumes that mask it is in masked_by_list
int mask_mode; // ALL/ANY 1/2. In ALL mode, only parts covered by all masks is simulated, in ANY mode, area covered by just one mask is simulated
int skip_hierarchy_optimization;
double geometry_p_interact; // fraction of rays that interact with this volume for each scattering (between 0 and 1, 0 for disable)
union geometry_parameter_union geometry_parameters; // relevant parameters for this shape
union geometry_parameter_union (*copy_geometry_parameters)(union geometry_parameter_union*);

struct focus_data_array_struct focus_data_array; // Focusing specified by user is element 0 and used for isotropic processes, rotated versions are added by master
struct pointer_to_1d_int_list focus_array_indices; // Add 1D integer array with indecies for correct focus_data for each process


// intersect_function takes position/velocity of ray and parameters, returns time list
int (*intersect_function)(double*, double*, double*, double*, int*, int*, double*, double*, struct geometry_struct*);
//                        t_array, nx array, ny array, nz array, surface_index array, n arary, r ,v

// within_function that checks if the ray origin is within this volume
int (*within_function)(Coords,struct geometry_struct*);
//                     r,      parameters

// mcdisplay function, draws the geometry
//void (*mcdisplay_function)(struct lines_to_draw*,int,struct Volume_struct**,int);
void (*mcdisplay_function)(struct lines_to_draw*,int,struct geometry_struct**,int);
//                                lines          index             Geometries   N

void (*initialize_from_main_function)(struct geometry_struct*);

struct pointer_to_1d_coords_list (*shell_points)(struct geometry_struct*, int maximum_number_of_points);

// List of other volumes to be check when ray starts within this volume.
struct pointer_to_1d_int_list intersect_check_list;
// List of other volumes the ray may enter, if the ray intersects the volume itself.
struct pointer_to_1d_int_list destinations_list;
// The destinations list stored as a logic list which makes some tasks quicker. OBSOLETE
//struct pointer_to_1d_int_list destinations_logic_list;
// Reduced list of other volumes the ray may enter, if the ray intersects the volume itself.
struct pointer_to_1d_int_list reduced_destinations_list;
// List of other volumes that are within this volume
struct pointer_to_1d_int_list children;
// List of other volumes that are within this volume, but does not have any parents that are children of this volume
struct pointer_to_1d_int_list direct_children;
// List of next possible volumes (only used in tagging)
struct pointer_to_1d_int_list next_volume_list;
// List of volumes masked by this volume (usually empty)
struct pointer_to_1d_int_list mask_list;
// List of volumes masking this volume
struct pointer_to_1d_int_list masked_by_list;
// List of masks masking this volume (global mask indices)
struct pointer_to_1d_int_list masked_by_mask_index_list;
// Additional intersect lists dependent on mask status
//struct indexed_mask_lists_struct mask_intersect_lists;
// Simpler way of storing the mask_intersect_lists
struct pointer_to_1d_int_list mask_intersect_list;

// Surfaces
// Could make structure for this and support functions?
int number_of_faces;
struct surface_stack_struct **surface_stack_for_each_face;
struct surface_stack_struct *internal_cut_surface_stack;
};

struct physics_struct
{
char name[256]; // User defined material name
int interact_control;
int is_vacuum;
int any_process_needs_cross_section_focus;
double my_a;
int number_of_processes;
// pointer to array of pointers to physics_sub structures that each describe a scattering process
struct scattering_process_struct *p_scattering_array;

// refraction related
int has_refraction_info;
double refraction_scattering_length_density; // [AA^-2]
double refraction_Qc;
};

union data_transfer_union{
    // List of pointers to storage structs for all supported physical processes
    struct Incoherent_physics_storage_struct  *pointer_to_a_Incoherent_physics_storage_struct;
    struct Powder_physics_storage_struct *pointer_to_a_Powder_physics_storage_struct;
    struct Single_crystal_physics_storage_struct *pointer_to_a_Single_crystal_physics_storage_struct;
    struct AF_HB_1D_physics_storage_struct *pointer_to_a_AF_HB_1D_physics_storage_struct;
    struct IncoherentPhonon_physics_storage_struct *pointer_to_a_IncoherentPhonon_physics_storage_struct;
    struct PhononSimpleNumeric_physics_storage_struct *pointer_to_a_PhononSimpleNumeric_storage_struct;
    struct PhononSimple_physics_storage_struct *pointer_to_a_PhononSimple_storage_struct;
    struct MagnonSimple_physics_storage_struct *pointer_to_a_MagnonSimple_storage_struct;
    struct Sans_spheres_physics_storage_struct *pointer_to_a_Sans_spheres_physics_storage_struct;
    struct Texture_physics_storage_struct *pointer_to_a_Texture_physics_storage_struct;
    struct NCrystal_physics_storage_struct *pointer_to_a_NCrystal_physics_storage_struct;
    struct Non_physics_storage_struct *pointer_to_a_Non_physics_storage_struct;
    struct Template_physics_storage_struct *pointer_to_a_Template_physics_storage_struct;
    // possible to add as many structs as wanted, without increasing memory footprint.
};



struct scattering_process_struct
{
char name[256];                // User defined process name
enum process eProcess;         // enum value corresponding to this process GPU
double process_p_interact;     // double between 0 and 1 that describes the fraction of events forced to undergo this process. -1 for disable
int non_isotropic_rot_index;   // -1 if process is isotrpic, otherwise is the index of the process rotation matrix in the volume
int needs_cross_section_focus; // 1 if physics_my needs to call focus functions, otherwise -1
Rotation rotation_matrix;      // rotation matrix of process, reported by component in local frame, transformed and moved to volume struct in main

union data_transfer_union data_transfer; // The way to reach the storage space allocated for this process (see examples in process.comp files)

// probability_for_scattering_functions calculates this probability given k_i and parameters
int (*probability_for_scattering_function)(double*,double*,union data_transfer_union,struct focus_data_struct*, _class_particle *_particle);
//                                         prop,   k_i,   ,parameters               , focus data / function

// A scattering_function takes k_i and parameters, returns k_f
int (*scattering_function)(double*,double*,double*,union data_transfer_union,struct focus_data_struct*, _class_particle *_particle);
//                         k_f,    k_i,    weight, parameters               , focus data / function
};

//Utility function for initialising a scattering_process_struct with default
//values:
void scattering_process_struct_init( struct scattering_process_struct * sps )
{
  memset(sps,0,sizeof(struct scattering_process_struct));//catch all
  sps->name[0] = '\0';
  sps->probability_for_scattering_function = NULL;
  sps->scattering_function = NULL;
  sps->non_isotropic_rot_index = -1;
  sps->needs_cross_section_focus = -1;
}

union surface_data_transfer_union 
{
	struct Mirror_surface_storage_struct *pointer_to_a_Mirror_surface_storage_struct;
	struct Template_surface_storage_struct *pointer_to_a_Template_surface_storage_struct;	
};

struct surface_process_struct
{
char name[256];
enum surface eSurface;
union surface_data_transfer_union data_transfer;
};

struct surface_stack_struct
{
int number_of_surfaces;
struct surface_process_struct **p_surface_array;
};

struct Volume_struct
{
char name[256]; // User defined volume name
struct geometry_struct geometry;        // Geometry properties (including intersect functions, generated lists)
struct physics_struct *p_physics;       // Physical properties (list of scattering processes, absorption)
struct loggers_struct loggers;          // Loggers assosiated with this volume
struct abs_loggers_struct abs_loggers;  // Loggers assosiated with this volume
};

// example of calling a scattering process
// volume_pointer_list[3]->physics.scattering_process[5].probability_for_scattering_function(input,volume_pointer_list[3]->physics.scattering_process[5])

struct starting_lists_struct
{
struct pointer_to_1d_int_list allowed_starting_volume_logic_list;
struct pointer_to_1d_int_list reduced_start_list;
struct pointer_to_1d_int_list start_logic_list;
struct pointer_to_1d_int_list starting_destinations_list;
};

struct global_positions_to_transform_list_struct
{
int num_elements;
Coords **positions;
};

struct global_rotations_to_transform_list_struct
{
int num_elements;
Rotation **rotations;
};

struct global_surface_element_struct
{
char name[256]; // Name of the process
int component_index;
struct surface_process_struct *p_surface_process;
};

struct pointer_to_global_surface_list 
{
int num_elements;
struct global_surface_element_struct *elements;
};

struct global_process_element_struct
{
char name[256]; // Name of the process
int component_index;
struct scattering_process_struct *p_scattering_process;
};

struct pointer_to_global_process_list {
int num_elements;
struct global_process_element_struct *elements;
};

struct global_material_element_struct
{
char name[128];
int component_index;
struct physics_struct *physics;
};

struct pointer_to_global_material_list {
int num_elements;
struct global_material_element_struct *elements;
};

struct global_geometry_element_struct
{
char name[128];
int component_index;
int activation_counter;
int stored_copies;
int active;
struct Volume_struct *Volume;
};

struct pointer_to_global_geometry_list {
int num_elements;
struct global_geometry_element_struct *elements;
};

struct global_logger_element_struct {
char name[128];
int component_index;
struct logger_struct *logger;
};

struct pointer_to_global_logger_list {
int num_elements;
struct global_logger_element_struct *elements;
};

struct global_abs_logger_element_struct {
char name[128];
int component_index;
struct abs_logger_struct *abs_logger;
};

struct pointer_to_global_abs_logger_list {
int num_elements;
struct global_abs_logger_element_struct *elements;
};

struct global_tagging_conditional_element_struct {
struct conditional_list_struct conditional_list;
int extend_index;
char name[1024];
int use_status;
};

struct global_tagging_conditional_list_struct {
int num_elements;
int current_index;
struct global_tagging_conditional_element_struct *elements;
};


struct global_master_element_struct {
char name[128];
int component_index;
int stored_number_of_scattering_events; // TEST
struct conditional_list_struct *tagging_conditional_list_pointer;
};

struct pointer_to_global_master_list {
int num_elements;
struct global_master_element_struct *elements;
};


void geometry_struct_init(struct geometry_struct *geometry){
  memset(geometry, 0, sizeof(struct geometry_struct));
  geometry->skip_hierarchy_optimization = 0;
}
// -------------    Physics functions   ---------------------------------------------------------

//#include "Test_physics.c"
//#include "Incoherent_test.c"

// -------------    General functions   ---------------------------------------------------------
double distance_between(Coords position1,Coords position2) {
    return sqrt((position1.x-position2.x)*(position1.x-position2.x) +
                (position1.y-position2.y)*(position1.y-position2.y) +
                (position1.z-position2.z)*(position1.z-position2.z));
};



double length_of_3vector(double *r) {
        return sqrt(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
    };

double length_of_position_vector(Coords point) {
        return sqrt(point.x*point.x+point.y*point.y+point.z*point.z);
    };

Coords make_position(double *r) {
    Coords temp;
    
    temp.x = r[0];temp.y = r[1];temp.z = r[2];
    return temp;
};

Coords coords_scalar_mult(Coords input,double scalar) {
    return coords_set(scalar*input.x,scalar*input.y,scalar*input.z);
};

double union_coords_dot(Coords vector1,Coords vector2) {
    return vector1.x*vector2.x + vector1.y*vector2.y + vector1.z*vector2.z;
}


int sum_int_list(struct pointer_to_1d_int_list list) {
    int iterate,sum = 0;
    for (iterate = 0;iterate < list.num_elements;iterate++) sum += list.elements[iterate];
    return sum;
    };
    
int on_int_list(struct pointer_to_1d_int_list list,int target) {
    int iterate,output=0;
    for (iterate = 0; iterate<list.num_elements ;iterate++) {
        if (list.elements[iterate] == target) output = 1;
    }
    return output;
    };

int find_on_int_list(struct pointer_to_1d_int_list list,int target) {
    int iterate;
    for (iterate = 0;iterate < list.num_elements;iterate++) {
        if (list.elements[iterate] == target) return iterate;
    }
    return -1;
    };
    
void on_both_int_lists(struct pointer_to_1d_int_list *list1, struct pointer_to_1d_int_list *list2, struct pointer_to_1d_int_list *common) {
    // Assume common.elements is allocated to a number of int's large enough to hold the resulting list
    int iterate,used_elements=0;
    for (iterate=0;iterate<list1->num_elements;iterate++) {
        if (on_int_list(*list2,list1->elements[iterate])) common->elements[used_elements++] = list1->elements[iterate];
    }
    common->num_elements = used_elements;
    };

void remove_element_in_list_by_index(struct pointer_to_1d_int_list *list,int index) {
    if (index >= list->num_elements) {
      printf("ERROR(remove_element_in_list_by_index): trying to remove an index that wasn't allocated to begin with");
      exit(EXIT_FAILURE);
    }
    else {
        int iterate;
        int *temp;
        for (iterate = index;iterate < list->num_elements -1;iterate++) {
            list->elements[iterate] = list->elements[iterate+1];
        }
        list->num_elements--;
        //if (list->num_elements==0) printf("Making empty list!\n");
        temp = malloc(list->num_elements * sizeof(int));
	if (!temp) {
	  fprintf(stderr,"Failure allocating list in Union function remove_element_in_list_by_index 1/2 - Exit!\n");
	  exit(EXIT_FAILURE);
	}
        for (iterate = 0;iterate < list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
        free(list->elements);
        list->elements = malloc(list->num_elements * sizeof(int));
	if (!list->elements) {
	  fprintf(stderr,"Failure allocating list in Union function remove_element_in_list_by_index 2/2 - Exit!\n");
	  exit(EXIT_FAILURE);
	}
        for (iterate = 0;iterate < list->num_elements;iterate++) list->elements[iterate] = temp[iterate];
        free(temp);

    }
    };
    
void remove_element_in_list_by_value(struct pointer_to_1d_int_list *list,int value) {
    int iterate;
    for (iterate = 0;iterate < list->num_elements;iterate++) {
        if (list->elements[iterate] == value) remove_element_in_list_by_index(list,iterate);
    }
    };
    
void merge_lists(struct pointer_to_1d_int_list *result,struct pointer_to_1d_int_list *list1,struct pointer_to_1d_int_list *list2) {
    if (result->num_elements > 0) free(result->elements);
    result->num_elements = list1->num_elements + list2->num_elements;
    if (result->num_elements != 0) {
      result->elements = malloc(result->num_elements*sizeof(int));
      if (!result->elements) {
	fprintf(stderr,"Failure allocating list in Union function merge_lists- Exit!\n");
	  exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate = 0;iterate < list1->num_elements;iterate++)
          result->elements[iterate] = list1->elements[iterate];
      for (iterate = 0;iterate < list2->num_elements;iterate++)
          result->elements[list1->num_elements+iterate] = list2->elements[iterate];
    }
    };

void add_element_to_double_list(struct pointer_to_1d_double_list *list,double value) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(double));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_double_list 1/3 - Exit!\n");
	  exit(EXIT_FAILURE);
      }
      list-> elements[0] = value;
    } else {
      double *temp=malloc(list->num_elements*sizeof(double));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_double_list 2/3 - Exit!\n");
	  exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(double));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_double_list 3/3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = value;
    }
    };

void add_element_to_int_list(struct pointer_to_1d_int_list *list,int value) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(int));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_int_list 1/3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list-> elements[0] = value;
    } else {
      double *temp=malloc(list->num_elements*sizeof(double));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_int_list 2/3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(int));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_int_list 3/3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = value;
    }
    };

// Need to check if absolute_rotation is preserved correctly.
void add_element_to_focus_data_array(struct focus_data_array_struct *focus_data_array,struct focus_data_struct focus_data) {
    if (focus_data_array->num_elements == 0) {
      focus_data_array->num_elements++;
      focus_data_array->elements = malloc(focus_data_array->num_elements*sizeof(struct focus_data_struct));
      if (!focus_data_array->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_focus_data_array 1/3- Exit!\n");
	exit(EXIT_FAILURE);
      }
      focus_data_array->elements[0] = focus_data;
    } else {
      struct focus_data_struct *temp=malloc(focus_data_array->num_elements*sizeof(struct focus_data_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_focus_data_array 2/3- Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<focus_data_array->num_elements;iterate++) temp[iterate] = focus_data_array->elements[iterate];
      free(focus_data_array->elements);
      focus_data_array->num_elements++;
      focus_data_array-> elements = malloc(focus_data_array->num_elements*sizeof(struct focus_data_struct));
      if (!focus_data_array->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_focus_data_array 3/3- Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<focus_data_array->num_elements-1;iterate++) focus_data_array->elements[iterate] = temp[iterate];
      free(temp);
      focus_data_array->elements[focus_data_array->num_elements-1] = focus_data;
    }
    };
	
void copy_focus_data_array(struct focus_data_array_struct *original_array, struct focus_data_array_struct *new_array) {
	
	new_array->num_elements = original_array->num_elements;
	new_array->elements = malloc(new_array->num_elements*sizeof(struct focus_data_struct));

	if (new_array->elements == NULL) {
		fprintf(stderr, "Memory allocation failed in copy_focus_data_struct \n");
		exit(EXIT_FAILURE);
	}
		
	int iterate;
	for (iterate=0;iterate<original_array->num_elements;iterate++) {
		new_array->elements[iterate] = original_array->elements[iterate];
	}

	};



void add_to_logger_with_data(struct logger_with_data_struct *logger_with_data, struct logger_struct *logger) {
    // May reorder the order of the if conditions to avoid checking the == 0 for every single ray
    if (logger_with_data->allocated_elements == 0) {
        logger_with_data->allocated_elements = 5;
        logger_with_data->logger_pointers = malloc(logger_with_data->allocated_elements*sizeof(struct logger_struct*));
	if (!logger_with_data->logger_pointers) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_logger_with_data 1/3- Exit!\n");
	  exit(EXIT_FAILURE);
	}
        logger_with_data->used_elements = 1;
        logger_with_data->logger_pointers[0] = logger;
    } else if (logger_with_data->used_elements > logger_with_data->allocated_elements-1) {
        struct logger_with_data_struct temp_logger_with_data;
        temp_logger_with_data.logger_pointers = malloc((logger_with_data->used_elements)*sizeof(struct logger_struct*));
	if (!temp_logger_with_data.logger_pointers) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_logger_with_data 2/3- Exit!\n");
	  exit(EXIT_FAILURE);
	}
        int iterate;
        for (iterate=0;iterate<logger_with_data->used_elements;iterate++) {
            temp_logger_with_data.logger_pointers[iterate]  = logger_with_data->logger_pointers[iterate];
        }
        free(logger_with_data->logger_pointers);
        logger_with_data->allocated_elements = logger_with_data->allocated_elements+5;
        logger_with_data->logger_pointers = malloc(logger_with_data->allocated_elements*sizeof(struct logger_struct*));
	if (!logger_with_data->logger_pointers) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_logger_with_data 3/3- Exit!\n");
	  exit(EXIT_FAILURE);
	}
        for (iterate=0;iterate<logger_with_data->used_elements;iterate++) {
            logger_with_data->logger_pointers[iterate]  = temp_logger_with_data.logger_pointers[iterate];
        }

        logger_with_data->logger_pointers[logger_with_data->used_elements++] = logger;

	// Clear up temporary memory
	free(temp_logger_with_data.logger_pointers);

    } else {
        logger_with_data->logger_pointers[logger_with_data->used_elements++] = logger;
    }
    
};

void add_to_abs_logger_with_data(struct abs_logger_with_data_struct *abs_logger_with_data, struct abs_logger_struct *abs_logger) {
    // May reorder the order of the if conditions to avoid checking the == 0 for every single ray
    if (abs_logger_with_data->allocated_elements == 0) {
        abs_logger_with_data->allocated_elements = 5;
        abs_logger_with_data->abs_logger_pointers = malloc(abs_logger_with_data->allocated_elements*sizeof(struct abs_logger_struct*));
	if (!abs_logger_with_data->abs_logger_pointers) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_abs_logger_with_data 1/3- Exit!\n");
	  exit(EXIT_FAILURE);
	}
        abs_logger_with_data->used_elements = 1;
        abs_logger_with_data->abs_logger_pointers[0] = abs_logger;
    } else if (abs_logger_with_data->used_elements > abs_logger_with_data->allocated_elements-1) {
        struct abs_logger_with_data_struct temp_abs_logger_with_data;
        temp_abs_logger_with_data.abs_logger_pointers = malloc((abs_logger_with_data->used_elements)*sizeof(struct abs_logger_struct*));
	if (!temp_abs_logger_with_data.abs_logger_pointers) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_abs_logger_with_data 2/3- Exit!\n");
	  exit(EXIT_FAILURE);
	}
        int iterate;
        for (iterate=0;iterate<abs_logger_with_data->used_elements;iterate++) {
            temp_abs_logger_with_data.abs_logger_pointers[iterate]  = abs_logger_with_data->abs_logger_pointers[iterate];
        }
        free(abs_logger_with_data->abs_logger_pointers);
        abs_logger_with_data->allocated_elements = abs_logger_with_data->allocated_elements+5;
        abs_logger_with_data->abs_logger_pointers = malloc(abs_logger_with_data->allocated_elements*sizeof(struct abs_logger_struct*));
	if (!abs_logger_with_data->abs_logger_pointers) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_abs_logger_with_data 3/3- Exit!\n");
	  exit(EXIT_FAILURE);
	}
        for (iterate=0;iterate<abs_logger_with_data->used_elements;iterate++) {
            abs_logger_with_data->abs_logger_pointers[iterate] = temp_abs_logger_with_data.abs_logger_pointers[iterate];
        }

        abs_logger_with_data->abs_logger_pointers[abs_logger_with_data->used_elements++] = abs_logger;

	// Clear up temporary memory
	free(temp_abs_logger_with_data.abs_logger_pointers);

    } else {
        abs_logger_with_data->abs_logger_pointers[abs_logger_with_data->used_elements++] = abs_logger;
    }
    
};


// Used typedef to avoid having to change this function later. May update others to use same phillosphy.
void add_function_to_conditional_list(struct conditional_list_struct *list,conditional_function_pointer new, union conditional_data_union *data_union) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->conditional_functions = malloc(list->num_elements*sizeof(conditional_function_pointer));
      if (!list->conditional_functions) {
	fprintf(stderr,"Failure allocating list in Union function add_function_to_conditional_list 1/6 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->p_data_unions = malloc(list->num_elements*sizeof(union conditional_data_union*));
      if (!list->p_data_unions) {
	fprintf(stderr,"Failure allocating list in Union function add_function_to_conditional_list 2/6 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->conditional_functions[0] = new;
      list->p_data_unions[0] = data_union;
    }
    else {
    conditional_function_pointer *temp_fp=malloc(list->num_elements*sizeof(conditional_function_pointer));
    if (!temp_fp) {
      fprintf(stderr,"Failure allocating list in Union function add_function_to_conditional_list 3/6 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    union conditional_data_union **temp_du=malloc(list->num_elements*sizeof(union conditional_data_union));
    if (!temp_du) {
      fprintf(stderr,"Failure allocating list in Union function add_function_to_conditional_list 4/6 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    int iterate;
    // Could even get away with a shallow copy here instead of the loop, but it is not relevant for performance.
    for (iterate=0;iterate<list->num_elements;iterate++) {
      temp_fp[iterate] = list->conditional_functions[iterate];
      temp_du[iterate] = list->p_data_unions[iterate];
    }
    free(list->conditional_functions);
    free(list->p_data_unions);
    list->num_elements++;
    list->conditional_functions = malloc(list->num_elements*sizeof(conditional_function_pointer));
    if (!list->conditional_functions) {
      fprintf(stderr,"Failure allocating list in Union function add_function_to_conditional_list 5/6 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    list->p_data_unions = malloc(list->num_elements*sizeof(union conditional_data_union*));
    if (!list->p_data_unions) {
      fprintf(stderr,"Failure allocating list in Union function add_function_to_conditional_list 6/6 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    
    for (iterate=0;iterate<list->num_elements-1;iterate++) {
      list->conditional_functions[iterate] = temp_fp[iterate];
      list->p_data_unions[iterate] = temp_du[iterate];
    }
    list->conditional_functions[list->num_elements-1] = new;
    list->p_data_unions[list->num_elements-1] = data_union;
    }
};


// could make function that removes a element from a 1d_int_list, and have each list generation as a function that takes a copy of an overlap list

void print_1d_int_list(struct pointer_to_1d_int_list list,char *name) {
        int iterate;
        printf("LIST: ");printf("%s",name);printf(" = [");
        for (iterate = 0; iterate < list.num_elements; iterate++) {
            printf("%d",list.elements[iterate]);
            if (iterate < list.num_elements - 1) printf(",");
        }
        printf("]\n");
    };

void print_1d_double_list(struct pointer_to_1d_double_list list,char *name) {
        int iterate;
        printf("LIST: ");printf("%s",name);printf(" = [");
        for (iterate = 0; iterate < list.num_elements; iterate++) {
            printf("%f",list.elements[iterate]);
            if (iterate < list.num_elements - 1) printf(",");
        }
        printf("]\n");
    };

void print_position(Coords pos,char *name) {
    printf("POSITION: ");printf("%s",name);printf(" = (%f,%f,%f)\n",pos.x,pos.y,pos.z);
    };

void print_rotation(Rotation rot, char *name) {
    printf("ROT MATRIX: %s \n",name);
    printf("[%f %f %f]\n",rot[0][0],rot[0][1],rot[0][2]);
    printf("[%f %f %f]\n",rot[1][0],rot[1][1],rot[1][2]);
    printf("[%f %f %f]\n\n",rot[2][0],rot[2][1],rot[2][2]);
};
    
void allocate_list_from_temp(int num_elements,struct pointer_to_1d_int_list original,struct pointer_to_1d_int_list *new) {
        int iterate;
    
        new->num_elements = num_elements;
        if (num_elements > 0) {
            new->elements = malloc(num_elements*sizeof(int));
	    if (!new->elements) {
	      fprintf(stderr,"Failure allocating list in Union function allocate_list_from_temp - Exit!\n");
	      exit(EXIT_FAILURE);
	    }
            for (iterate = 0;iterate < num_elements; iterate++) new->elements[iterate] = original.elements[iterate];
        } else new->elements = NULL;
    
    };

void allocate_logic_list_from_temp(int num_elements,struct pointer_to_1d_int_list original, struct pointer_to_1d_int_list *new) {
        // A logic list shares the same structure of a normal list, but instead of listing numbers, it is a list of yes / no (1/0)
        // Giving this function a list of [1 3 5] (and num_elements = 9) would return [0 1 0 1 0 1 0 0 0];
        int iterate;
    
        new->num_elements = num_elements;
        if (num_elements > 0) {
            new->elements = malloc(num_elements*sizeof(int));
	    if (!new->elements) {
	      fprintf(stderr,"Failure allocating list in Union function allocate_logic_list_from_temp - Exit!\n");
	      exit(EXIT_FAILURE);
	    }
            for (iterate = 0;iterate < num_elements;iterate++) new->elements[iterate] = 0;
            for (iterate = 0;iterate < original.num_elements;iterate++) {
                if (original.elements[iterate] < num_elements)
                    new->elements[original.elements[iterate]] = 1;
                else printf("Trying to allocate logical list without enough memory\n");
            }
        } else new->elements = NULL;
    };

/*
struct global_positions_to_transform_list_struct {
int num_elements;
Coords **positions;
}

struct global_rotations_to_transform_list_struct {
int num_elements;
Rotation **rotations;
}
*/

void add_position_pointer_to_list(struct global_positions_to_transform_list_struct *list, Coords *new_position_pointer) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->positions = malloc(list->num_elements*sizeof(Coords*));
      if (!list->positions) {
	fprintf(stderr,"Failure allocating list in Union function add_position_pointer_to_list - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->positions[0] = new_position_pointer;
    } else {
      Coords **temp;
      temp = malloc(list->num_elements*sizeof(Coords*));
      if (temp == NULL) {
	fprintf(stderr,"malloc failed in add_position_pointer_to_list for temp\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++)
        temp[iterate] = list->positions[iterate];
      free(list->positions);
      list->num_elements++;
      list->positions = malloc(list->num_elements*sizeof(Coords*));
      if (list->positions == NULL) {
	fprintf(stderr,"malloc failed in add_position_pointer_to_list for list->positions\n");
	exit(EXIT_FAILURE);
      }
      
      for (iterate=0;iterate<list->num_elements-1;iterate++)
        list->positions[iterate] = temp[iterate];
      free(temp);
      list->positions[list->num_elements-1] = new_position_pointer;
    }
};

void add_rotation_pointer_to_list(struct global_rotations_to_transform_list_struct *list, Rotation *new_rotation_pointer) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->rotations = malloc(list->num_elements*sizeof(Rotation*));
      if (!list->rotations) {
	fprintf(stderr,"Failure allocating list in Union function add_rotation_pointer_to_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->rotations[0] = new_rotation_pointer;
    } else {
      Rotation **temp;
      temp = malloc(list->num_elements*sizeof(Rotation*));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_rotation_pointer_to_list 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++)
        temp[iterate] = list->rotations[iterate];
      free(list->rotations);
      list->num_elements++;
      list->rotations = malloc(list->num_elements*sizeof(Rotation*));
      if (!list->rotations) {
	fprintf(stderr,"Failure allocating list in Union function add_rotation_pointer_to_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++)
        list->rotations[iterate] = temp[iterate];
      free(temp);
      list->rotations[list->num_elements-1] = new_rotation_pointer;
    }
};

void add_element_to_process_list(struct pointer_to_global_process_list *list,struct global_process_element_struct new_element) {
    if (list->num_elements == 0) {
    list->num_elements++;
    list-> elements = malloc(list->num_elements*sizeof(struct global_process_element_struct));
    if (!list->elements) {
      fprintf(stderr,"Failure allocating list in Union function add_element_to_process_list 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    list-> elements[0] = new_element;
    }
    else {
      struct global_process_element_struct *temp=malloc(list->num_elements*sizeof(struct global_process_element_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_process_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(struct global_process_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_process_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = new_element;
    }
};

void add_element_to_material_list(struct pointer_to_global_material_list *list,struct global_material_element_struct new_element) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->elements = malloc(list->num_elements*sizeof(struct global_material_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_material_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->elements[0] = new_element;
    }
    else {
      struct global_material_element_struct *temp=malloc(list->num_elements*sizeof(struct global_material_element_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_material_list 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(struct global_material_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_material_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = new_element;
    }
};

void add_element_to_surface_list(struct pointer_to_global_surface_list *list, struct global_surface_element_struct new_element) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->elements = malloc(list->num_elements*sizeof(struct global_surface_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_surface_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->elements[0] = new_element;
    }
    else {
      struct global_surface_element_struct *temp=malloc(list->num_elements*sizeof(struct global_surface_element_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_surface_list 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(struct global_surface_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_surface_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = new_element;
    }
};

void add_element_to_surface_stack(struct surface_stack_struct *list, struct surface_process_struct *new_element) {
    if (list->number_of_surfaces == 0) {
        if (!list->p_surface_array) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        list->p_surface_array[0] = new_element;
        list->number_of_surfaces = 1;
    } else {
        // Reallocate with space for one more element
        struct surface_process_struct **temp = realloc(list->p_surface_array, 
            (list->number_of_surfaces + 1) * sizeof(struct surface_process_struct*));
        if (!temp) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(EXIT_FAILURE);
        }
        list->p_surface_array = temp;
        list->p_surface_array[list->number_of_surfaces] = new_element;
        list->number_of_surfaces++;
    }
};

void add_element_to_geometry_list(struct pointer_to_global_geometry_list *list,struct global_geometry_element_struct new_element) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->elements = malloc(list->num_elements*sizeof(struct global_geometry_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_geometry_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->elements[0] = new_element;
    }
    else {
      struct global_geometry_element_struct *temp=malloc(list->num_elements*sizeof(struct global_geometry_element_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_geometry_list 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(struct global_geometry_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_geometry_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = new_element;
    }
};

void add_element_to_logger_list(struct pointer_to_global_logger_list *list,struct global_logger_element_struct new_element) {
    if (list->num_elements == 0) {
    list->num_elements++;
    list->elements = malloc(list->num_elements*sizeof(struct global_logger_element_struct));
    if (!list->elements) {
      fprintf(stderr,"Failure allocating list in Union function add_element_to_logger_list 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    list->elements[0] = new_element;
    }
    else {
    struct global_logger_element_struct *temp=malloc(list->num_elements*sizeof(struct global_logger_element_struct));
    if (!temp) {
      fprintf(stderr,"Failure allocating list in Union function add_element_to_logger_list 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    int iterate;
    for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
    free(list->elements);
    list->num_elements++;
    list-> elements = malloc(list->num_elements*sizeof(struct global_logger_element_struct));
    if (!list->elements) {
      fprintf(stderr,"Failure allocating list in Union function add_element_to_logger_list 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
    free(temp);
    list->elements[list->num_elements-1] = new_element;
    }
};

void add_element_to_abs_logger_list(struct pointer_to_global_abs_logger_list *list, struct global_abs_logger_element_struct new_element) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->elements = malloc(list->num_elements*sizeof(struct global_abs_logger_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_abs_logger_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->elements[0] = new_element;
    }
    else {
      struct global_abs_logger_element_struct *temp=malloc(list->num_elements*sizeof(struct global_abs_logger_element_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_abs_logger_list 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(struct global_abs_logger_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_abs_logger_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = new_element;
    }
};

void add_element_to_tagging_conditional_list(struct global_tagging_conditional_list_struct *list,struct global_tagging_conditional_element_struct new_element) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->elements = malloc(list->num_elements*sizeof(struct global_tagging_conditional_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_tagging_conditional_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->elements[0] = new_element;
    }
    else {
      struct global_tagging_conditional_element_struct *temp=malloc(list->num_elements*sizeof(struct global_tagging_conditional_element_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_tagging_conditional_list 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list->elements = malloc(list->num_elements*sizeof(struct global_tagging_conditional_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_tagging_conditional_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = new_element;
    }
};

void add_element_to_master_list(struct pointer_to_global_master_list *list,struct global_master_element_struct new_element) {
    if (list->num_elements == 0) {
      list->num_elements++;
      list->elements = malloc(list->num_elements*sizeof(struct global_master_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_master_list 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      list->elements[0] = new_element;
    }
    else {
      struct global_master_element_struct *temp=malloc(list->num_elements*sizeof(struct global_master_element_struct));
      if (!temp) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_master_list 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      int iterate;
      for (iterate=0;iterate<list->num_elements;iterate++) temp[iterate] = list->elements[iterate];
      free(list->elements);
      list->num_elements++;
      list-> elements = malloc(list->num_elements*sizeof(struct global_master_element_struct));
      if (!list->elements) {
	fprintf(stderr,"Failure allocating list in Union function add_element_to_master_list 3 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<list->num_elements-1;iterate++) list->elements[iterate] = temp[iterate];
      free(temp);
      list->elements[list->num_elements-1] = new_element;
    }
};

void add_initialized_logger_in_volume(struct loggers_struct *loggers,int number_of_processes) {
  int iterate;
  if (loggers->num_elements == 0) {
    loggers->num_elements++;
    loggers->p_logger_volume = malloc(loggers->num_elements * sizeof(struct logger_for_each_process_list));
    if (!loggers->p_logger_volume) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_logger_in_volume 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    loggers->p_logger_volume[0].num_elements = number_of_processes;
    loggers->p_logger_volume[0].p_logger_process = malloc(number_of_processes * sizeof(struct logger_struct**));
    if (!loggers->p_logger_volume[0].p_logger_process) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_logger_in_volume 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<number_of_processes;iterate++)
      loggers->p_logger_volume[0].p_logger_process[iterate] = NULL;
  } else {
    // Already some elements, store them in temp, free main, transfer back and add newest.
    struct logger_for_each_process_list *temp=malloc(loggers->num_elements*sizeof(struct logger_for_each_process_list));
    if (!temp) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_logger_in_volume 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<loggers->num_elements;iterate++) temp[iterate] = loggers->p_logger_volume[iterate];
    free(loggers->p_logger_volume);
    loggers->num_elements++;
    loggers->p_logger_volume = malloc(loggers->num_elements*sizeof(struct logger_for_each_process_list));
    if (!loggers->p_logger_volume) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_logger_in_volume 4 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<loggers->num_elements-1;iterate++) loggers->p_logger_volume[iterate] = temp[iterate];
    free(temp);
    loggers->p_logger_volume[loggers->num_elements-1].num_elements = number_of_processes;
    loggers->p_logger_volume[loggers->num_elements-1].p_logger_process = malloc(number_of_processes * sizeof(struct logger_struct**));
    if (!loggers->p_logger_volume[loggers->num_elements-1].p_logger_process) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_logger_in_volume 5 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<number_of_processes;iterate++) {
      loggers->p_logger_volume[loggers->num_elements-1].p_logger_process[iterate] = NULL;
    }
  }
};


void add_initialized_abs_logger_in_volume(struct abs_loggers_struct *abs_loggers) {
  int iterate;
  if (abs_loggers->num_elements == 0) {
    abs_loggers->num_elements++;
    abs_loggers->p_abs_logger = malloc(abs_loggers->num_elements * sizeof(struct abs_logger_struct*));
    if (!abs_loggers->p_abs_logger) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_abs_logger_in_volume 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
  } else {
    // Already some elements, store them in temp, free main, transfer back and add newest.
    struct abs_logger_struct **temp=malloc(abs_loggers->num_elements*sizeof(struct abs_logger_struct *));
    if (!temp) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_abs_logger_in_volume 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<abs_loggers->num_elements;iterate++) temp[iterate] = abs_loggers->p_abs_logger[iterate];
    free(abs_loggers->p_abs_logger);
    
    abs_loggers->num_elements++;
    abs_loggers->p_abs_logger = malloc(abs_loggers->num_elements*sizeof(struct abs_logger_struct*));
    if (!abs_loggers->p_abs_logger) {
      fprintf(stderr,"Failure allocating list in Union function add_initialized_abs_logger_in_volume 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<abs_loggers->num_elements-1;iterate++) abs_loggers->p_abs_logger[iterate] = temp[iterate];
    free(temp);
    abs_loggers->p_abs_logger[abs_loggers->num_elements-1] = NULL;
  }
};


// -------------    Functions used to shorten master trace    ---------------------------------------------


void update_current_mask_intersect_status(struct pointer_to_1d_int_list *current_mask_intersect_list_status, struct pointer_to_1d_int_list *mask_status_list, struct Volume_struct **Volumes, int *current_volume) {
  // This function is to be executed whenever the current volume changes, or the mask status changes
  // It updates the effective mask status for each element of a volumes mask_intersect_list
  // The effective mask takes the ALL/ANY mode assosiated with each volume into account,
  //  meaning a volume needs to be within ALL/ANY of it's masks to have a status of 1
  // In most cases this mask_intersect_list will be empty, and memory operations are avoided
  //printf("Number of elements to be check: %d \n",Volumes[*current_volume]->geometry.mask_intersect_list.num_elements);
  
  if (Volumes[*current_volume]->geometry.mask_intersect_list.num_elements > 0) {
    int iterate,this_element,*mask_start,*mask_check;
    for (iterate=0;iterate<Volumes[*current_volume]->geometry.mask_intersect_list.num_elements;iterate++) {
      this_element = Volumes[*current_volume]->geometry.mask_intersect_list.elements[iterate];
      //printf("We are investigating volume number %d from the mask_intersect_list of volume %d",this_element,*current_volume);
      if (Volumes[this_element]->geometry.mask_mode == 2) { // ANY mask mode
        //printf("We are in ANY mask mode! \n");
        current_mask_intersect_list_status->elements[iterate] = 0; // Assume the mask status is 0, but if any are 1, take that instead
        for (mask_start=mask_check=Volumes[this_element]->geometry.masked_by_mask_index_list.elements;mask_check-mask_start<Volumes[this_element]->geometry.masked_by_mask_index_list.num_elements;mask_check++) {
          //printf("Checking all the mask statuses of volumes masking %d, now mask with global mask index %d ! \n",this_element,*mask_check);
          if (mask_status_list->elements[*mask_check] == 1) {
            //printf("The status was 1, so the effective status is set to 1 and the loop is stopped");
            current_mask_intersect_list_status->elements[iterate] = 1;
            break;
          }
        }
      } else { // ALL mask mode
        //printf("We are in ALL mask mode! \n");
        current_mask_intersect_list_status->elements[iterate] = 1; // Assume the mask status is 1, but if any one is 0, take that instead
        for (mask_start=mask_check=Volumes[this_element]->geometry.masked_by_mask_index_list.elements;mask_check-mask_start<Volumes[this_element]->geometry.masked_by_mask_index_list.num_elements;mask_check++) {
          //printf("Checking all the mask statuses of volumes masking %d, now mask with global mask index %d ! \n",this_element,*mask_check);
          if (mask_status_list->elements[*mask_check] == 0) {
            //printf("The status was 0, so the effective status is set to 0 and the loop is stopped \n");
            current_mask_intersect_list_status->elements[iterate] = 0;
            break;
          }
        }
      }
    }
  }
    
  /* Original version compatible with trace scope
  for (iterate=0;iterate<Volumes[current_volume]->geometry.mask_intersect_list.num_elements;iterate++) {
    this_element = Volumes[current_volume]->geometry.mask_intersect_list.elements[iterate];
    if (Volumes[this_element]->geometry.mask_mode == 2) { // ANY mask mode
      current_mask_intersect_list_status.elements[iterate] = 0; // Assume the mask status is 0, but if any are 1, take that instead
      for (mask_start=mask_check=Volumes[this_element]->geometry.masked_by_mask_index_list.elements;mask_check-mask_start<Volumes[this_element]->geometry.masked_by_mask_index_list.num_elements;mask_check++) {
        if (mask_status_list[*mask_check] == 1) {
          current_mask_intersect_list_status.elements[iterate] = 1;
          break;
        }
      }
    } else { // ALL mask mode
      current_mask_intersect_list_status.elements[iterate] = 1; // Assume the mask status is 1, but if any one is 0, take that instead
      for (mask_start=mask_check=Volumes[this_element]->geometry.masked_by_mask_index_list.elements;mask_check-mask_start<Volumes[this_element]->geometry.masked_by_mask_index_list.num_elements;mask_check++) {
        if (mask_status_list[*mask_check] == 0) {
          current_mask_intersect_list_status.elements[iterate] = 0;
          break;
        }
      }
    }
  }
  */
}

// -------------    Tagging functions    ------------------------------------------------------------------

struct tagging_tree_node_struct {
  // Statistics:
  double intensity;
  int number_of_rays;
  // tree pointers
  struct tagging_tree_node_struct *above;   // Pointer to node above
  struct tagging_tree_node_struct **volume_branches;
  struct tagging_tree_node_struct **process_branches;
};

struct list_of_tagging_tree_node_pointers {
  struct tagging_tree_node_struct **elements;
  int num_elements;
};

struct tagging_tree_node_struct *make_tagging_tree_node(void) {
    return (struct tagging_tree_node_struct *) malloc(sizeof(struct tagging_tree_node_struct));
}


struct tagging_tree_node_struct  *simple_initialize_tagging_tree_node(struct tagging_tree_node_struct *new_node) {
    new_node = make_tagging_tree_node();
    
    if (new_node == NULL) printf("ERROR, Union tagging system could not allocate memory\n");
    new_node->intensity = 4.2; // (double) 4.2;
    new_node->number_of_rays = 42; //(int) 42;
    
    printf("new_node->intensity = %f, new_node->number_of_rays = %d \n",new_node->intensity,new_node->number_of_rays);
    return new_node;
};


struct tagging_tree_node_struct *initialize_tagging_tree_node(struct tagging_tree_node_struct *new_node, struct tagging_tree_node_struct *above_node, struct Volume_struct *this_volume) {
    new_node = make_tagging_tree_node();
    
    new_node->intensity = (double) 0;
    new_node->number_of_rays = (int) 0;
    new_node->above = above_node;
    
    int next_volume_list_length = this_volume->geometry.next_volume_list.num_elements;
    new_node->volume_branches = malloc(next_volume_list_length*sizeof(struct tagging_tree_node_struct*));
    if (!new_node->volume_branches) {
      fprintf(stderr,"Failure allocating list in Union function tagging_tree_node_struct 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    int iterate;
    // Initializing pointers so that they can be checked for NULL later. Is this redundant? Does malloc return null pointers?
    for (iterate=0;iterate<next_volume_list_length;iterate++) new_node->volume_branches[iterate] = NULL;
    
    int number_of_processes;
    if (this_volume->p_physics == NULL) number_of_processes = 0;
    else number_of_processes = this_volume->p_physics->number_of_processes;
    
    new_node->process_branches = malloc(number_of_processes*sizeof(struct tagging_tree_node_struct*));
    if (!new_node->process_branches) {
      fprintf(stderr,"Failure allocating list in Union function tagging_tree_node_struct 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    // Initializing pointers so that they can be checked for NULL later. Is this redundant? Does malloc return null pointers?
    for (iterate=0;iterate<number_of_processes;iterate++) new_node->process_branches[iterate] = NULL;

    return new_node;
};

struct tagging_tree_node_struct *goto_process_node(struct tagging_tree_node_struct *current_node, int process_index, struct Volume_struct *this_volume, int *stop_tagging_ray, int stop_creating_nodes) {
    // Either create a new node if it has not been created yet, or travel down the tree
    if (current_node->process_branches[process_index] == NULL) {
      if (stop_creating_nodes == 0) {
        current_node->process_branches[process_index] = initialize_tagging_tree_node(current_node->process_branches[process_index],current_node,this_volume);
        return current_node->process_branches[process_index];
      } else {
        // This stops the ray from using more goto node functions and being counted in the statistics.
        // Happens because the history limit is reached, and no new histories should be started.
        *stop_tagging_ray = 1;
        return current_node;
      }
    } else {
        current_node = current_node->process_branches[process_index];
        return current_node;
    }
    
    //return current_node;
};

struct tagging_tree_node_struct *goto_volume_node(struct tagging_tree_node_struct *current_node,int current_volume, int next_volume, struct Volume_struct **Volumes, int *stop_tagging_ray, int stop_creating_nodes) {
    // I have only allocated the number of node branches that corresponds to the current_volumes next_volume_list
    // The problem is to find where on the destination list the current volume is without doing a manual search
    
    // With the new mask system there is a risk of going from and to the same volume, which should be ignored by the tagging system
    if (current_volume == next_volume) return current_node;
    
    struct tagging_tree_node_struct *output;
    int next_volume_list_index = -1;
    // Temporary slow method for finding the correct index on the destination list
    int iterate;
    
    for (iterate=0;iterate<Volumes[current_volume]->geometry.next_volume_list.num_elements;iterate++)
        if (Volumes[current_volume]->geometry.next_volume_list.elements[iterate] == next_volume) {
            next_volume_list_index = iterate;
            break;
        }
    
    // Debug phase
    //printf("Tagging: going from volume %d to volume %d, which is index number %d on it's next volume list \n",current_volume,next_volume,next_volume_list_index);
    #ifndef OPENACC
    if (next_volume_list_index == -1) {
        printf("ERROR in Union component, tagging or destination system failed, next volume was not on next volume list\n");
        printf("current_volume = %d, next_volume = %d \n",current_volume,next_volume);
        print_1d_int_list(Volumes[current_volume]->geometry.destinations_list,"destinations_list for current volume");
        exit(EXIT_FAILURE);
    }
    #endif

    // Either create a new node if it has not been created yet, or travel down the tree
    if (current_node->volume_branches[next_volume_list_index] == NULL) {
      if (stop_creating_nodes == 0) {
        current_node->volume_branches[next_volume_list_index] =  initialize_tagging_tree_node(current_node->volume_branches[next_volume_list_index],current_node,Volumes[next_volume]);
        return current_node->volume_branches[next_volume_list_index];
      } else {
        // This stops the ray from using more goto node functions and being counted in the statistics.
        // Happens because the history limit is reached, and no new histories should be started.
        *stop_tagging_ray = 1;
        return current_node;
      }
    } else {
        //current_node = current_node->volume_branches[next_volume_list_index];
        //return current_node;
        current_node = current_node->volume_branches[next_volume_list_index];
        //printf("used allocated node \n");
        return current_node;
    }
};

void add_statistics_to_node(struct tagging_tree_node_struct *current_node, Coords *r, Coords *v, double *weight, int *counter) {
    if (current_node->number_of_rays == 0) (*counter)++;
    current_node->number_of_rays = current_node->number_of_rays + 1;
    current_node->intensity = current_node->intensity + *weight;
};

struct history_node_struct {
    int volume_index;
    int process_index;
};

struct dynamic_history_list {
  struct history_node_struct *elements;
  int used_elements;
  int allocated_elements;
};

struct saved_history_struct {
    struct history_node_struct *elements;
    int used_elements;
    double intensity;
    int number_of_rays;
};

struct total_history_struct {
    struct saved_history_struct *saved_histories;
    int used_elements;
    int allocated_elements;
};


void add_to_history(struct dynamic_history_list *history, int volume_index, int process_index) {
    //printf("Adding to history[%d]: volume_index = %d, process_index = %d \n",history->used_elements,volume_index,process_index);
    if (history->allocated_elements == 0) {
        history->elements = malloc(5*sizeof(struct history_node_struct));
	if (!history->elements) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_history 1 - Exit!\n");
	  exit(EXIT_FAILURE);
	}
        history->used_elements = 1;
        history->allocated_elements = 5;
        history->elements[0].volume_index = volume_index;
        history->elements[0].process_index = process_index;
    } else if (history->used_elements > history->allocated_elements-1) {
        struct dynamic_history_list temp_history;
        temp_history.elements = malloc((history->used_elements)*sizeof(struct history_node_struct));
	if (!temp_history.elements) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_history 2 - Exit!\n");
	  exit(EXIT_FAILURE);
	}
        int iterate;
        for (iterate=0;iterate<history->used_elements;iterate++) {
            temp_history.elements[iterate].volume_index  = history->elements[iterate].volume_index;
            temp_history.elements[iterate].process_index = history->elements[iterate].process_index;
        }
        free(history->elements);
        history->elements = malloc((history->allocated_elements+5)*sizeof(struct history_node_struct));
	if (!history->elements) {
	  fprintf(stderr,"Failure allocating list in Union function add_to_history 3 - Exit!\n");
	  exit(EXIT_FAILURE);
	}
        for (iterate=0;iterate<history->used_elements;iterate++) {
            history->elements[iterate].volume_index  = temp_history.elements[iterate].volume_index;
            history->elements[iterate].process_index = temp_history.elements[iterate].process_index;
        }
        
        history->allocated_elements = history->allocated_elements+5;
        
        history->elements[history->used_elements].volume_index = volume_index;
        history->elements[history->used_elements].process_index = process_index;
        history->used_elements = history->used_elements+1;

	// Clear up temporary memory
	free(temp_history.elements);

    } else {
        history->elements[history->used_elements].volume_index = volume_index;
        history->elements[history->used_elements].process_index = process_index;
        history->used_elements++;
    }
    
};

void printf_history(struct dynamic_history_list *history) {
    int history_iterate;
          //printf("History number %d, intensity = %f, number of rays = %d:",hist_num, search_node->intensity, search_node->number_of_rays);
          for (history_iterate=0;history_iterate<history->used_elements-1;history_iterate++) {
            if (history->elements[history_iterate].process_index == -1) {
                printf(" V%d ->",history->elements[history_iterate].volume_index);
            } else {
                printf(" P%d ->",history->elements[history_iterate].process_index);
            }
          }
          if (history->elements[history_iterate].process_index == -1) {
                printf(" V%d \n",history->elements[history_iterate].volume_index);
          } else {
                printf(" P%d \n",history->elements[history_iterate].process_index);
          }
}

void fprintf_total_history(struct saved_history_struct *history, FILE *fp) {
    fprintf(fp,"%d\t N I=%E \t", history->number_of_rays, history->intensity);
        int history_iterate;
          for (history_iterate=0;history_iterate<history->used_elements-1;history_iterate++) {
            if (history->elements[history_iterate].process_index == -1) {
                fprintf(fp," V%d ->",history->elements[history_iterate].volume_index);
            } else {
                fprintf(fp," P%d ->",history->elements[history_iterate].process_index);
            }
          }
          if (history->elements[history_iterate].process_index == -1) {
                fprintf(fp," V%d \n",history->elements[history_iterate].volume_index);
          } else {
                fprintf(fp," P%d \n",history->elements[history_iterate].process_index);
          }
}

int Sample_compare_history_intensities (const void* a, const void* b) {
  const double da = ((const struct saved_history_struct *)a)->intensity;
  const double db = ((const struct saved_history_struct *)b)->intensity;
  return (da < db) - (da > db);
}

void write_tagging_tree(struct list_of_tagging_tree_node_pointers *master_list, struct Volume_struct **Volumes, int total_history_counter, int number_of_volumes) {
  // Start from top of tree, go to extremeties, take results and add to disk / database, free that node
  int volume_index,done,volume_iterate,process_iterate,current_volume,next_node_found,current_number_of_processes,history_iterate,hist_num;
  
  struct tagging_tree_node_struct *search_node;
  struct tagging_tree_node_struct **kill_candidate;
  struct dynamic_history_list history_data; // Allocate the history list struct
  struct dynamic_history_list *history;     // Use this pointer in the algorithm
  
  struct total_history_struct total_history;
  total_history.saved_histories = malloc(total_history_counter * sizeof(struct saved_history_struct));
  if (!total_history.saved_histories) {
    fprintf(stderr,"Failure allocating list in Union function write_tagging_tree 1 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  total_history.allocated_elements = total_history_counter;
  total_history.used_elements = 0;
  
  history = &history_data;
  
  history->used_elements = 0;
  history->allocated_elements = 0;
  
  hist_num = 0;
  for (volume_index=0;volume_index<master_list->num_elements;volume_index++) {
    
    search_node = master_list->elements[volume_index];
    
    if (volume_index != 0)
        current_number_of_processes = Volumes[volume_index]->p_physics->number_of_processes;
    else
        current_number_of_processes = 0;
    
    current_volume = volume_index;
    done = 0;
    
    history->used_elements = 0;
    
    add_to_history(history,current_volume,-1);
    while(done == 0) {
        next_node_found=0;
        for (volume_iterate=0;volume_iterate<Volumes[current_volume]->geometry.next_volume_list.num_elements;volume_iterate++) {
            //printf("searc_node->volume_branches[0]->intensity = %f\n",search_node->volume_branches[0]->intensity);
            if (search_node->volume_branches[volume_iterate] != NULL) {
                current_volume = Volumes[current_volume]->geometry.next_volume_list.elements[volume_iterate];
                if (current_volume != 0)
                    current_number_of_processes = Volumes[current_volume]->p_physics->number_of_processes;
                else
                    current_number_of_processes = 0;
                
                //search_node = &(search_node->volume_branches[volume_iterate]);
                kill_candidate = &(search_node->volume_branches[volume_iterate]);
                search_node = search_node->volume_branches[volume_iterate];
                next_node_found = 1;
                add_to_history(history,current_volume,-1);
                //printf_history(history);
                break;
            }
        }
        if (next_node_found == 0) {
          //printf("doing process loop with %d steps \n",current_number_of_processes);
          for (process_iterate=0;process_iterate<current_number_of_processes;process_iterate++) {
            //if (&(search_node->process_branches[volume_iterate]) != NULL) {
            if (search_node->process_branches[process_iterate] != NULL) {
                //printf("was not NULL (process)\n");
                //search_node = &(search_node->process_branches[process_iterate]);
                kill_candidate = &(search_node->process_branches[process_iterate]);
                search_node = search_node->process_branches[process_iterate];
                next_node_found = 1;
                add_to_history(history,current_volume,process_iterate);
                //printf_history(history);
                break;
            }
          }
        }
        
        if (next_node_found == 0) {
          // write this history to disk / memory
          hist_num++;
          //printf("Reached next_node_found == 0 \n");
          if (history->used_elements > 0 && search_node->number_of_rays > 0) {
            //printf("%d rays (I=%E) with history: \t", search_node->number_of_rays, search_node->intensity);
            //printf_history(history);
            
            total_history.saved_histories[total_history.used_elements].used_elements = history->used_elements;
            total_history.saved_histories[total_history.used_elements].elements = malloc(total_history.saved_histories[total_history.used_elements].used_elements*sizeof(struct history_node_struct));
	    if (!total_history.saved_histories[total_history.used_elements].elements) {
	      fprintf(stderr,"Failure allocating list in Union function write_tagging_tree 2 - Exit!\n");
	      exit(EXIT_FAILURE);
	    }
            for (history_iterate = 0;history_iterate<history->used_elements;history_iterate++) {
                total_history.saved_histories[total_history.used_elements].elements[history_iterate] = history->elements[history_iterate];
                //printf("total_history.saved_histories[total_history.used_elements].elements[%d].volume_index \n",history_iterate,total_history.saved_histories[total_history.used_elements].elements[history_iterate].volume_index);
            }
            //total_history.saved_histories[total_history.used_elements].elements = history->elements;
            total_history.saved_histories[total_history.used_elements].intensity = search_node->intensity;
            total_history.saved_histories[total_history.used_elements].number_of_rays = search_node->number_of_rays;
            total_history.used_elements++;
          }
          
          history->used_elements = 0;
          
          // end of tree, no new nodes
          if (search_node->above == NULL) {
            done = 1;
          } else {
            // reset to the root of the tree
            *kill_candidate = NULL;
            free(search_node);
            search_node = master_list->elements[volume_index];
            
            if (volume_index != 0)
              current_number_of_processes = Volumes[volume_index]->p_physics->number_of_processes;
            else
              current_number_of_processes = 0;
    
            current_volume = volume_index;
            add_to_history(history,current_volume,-1);
          }
          
        }
    }
  }
  
  if (history->allocated_elements > 0) free(history->elements);

  qsort(total_history.saved_histories,total_history.used_elements,sizeof (struct saved_history_struct), Sample_compare_history_intensities);
  
  
  
  MPI_MASTER(
  printf("\n\n");
  printf("Top 20 most common histories. Shows the index of volumes entered (VX), and the scattering processes (PX)\n");
  for (history_iterate=0;history_iterate<total_history.used_elements && history_iterate<20;history_iterate++) {
    printf("%d\t N I=%E \t", total_history.saved_histories[history_iterate].number_of_rays, total_history.saved_histories[history_iterate].intensity);
    /*TK Changed from
      printf_history(&total_history.saved_histories[history_iterate]); to the
      following (of course this cast is only safe because the initial layout of
      the two struct types is the same... I am not 100% sure it is actually
      entirely guaranteed by the standard): */
    printf_history((struct dynamic_history_list *)(&total_history.saved_histories[history_iterate]));
  }
   
  FILE *fp;
  
  fp = fopen("union_history.dat","w");
  if(!fp) {
    fprintf(stderr,"WARNING: Could not write to logging output file union_history.dat\n");
  } else {
    fprintf(fp,"History file written by the McStas component Union_master \n");
    fprintf(fp,"When running with MPI, the results may be from just a single thread, meaning intensities are divided by number of threads\n");
    fprintf(fp,"----- Description of the used volumes -----------------------------------------------------------------------------------\n");
    
    fprintf(fp,"V0: Surrounding vacuum \n");
    for (volume_iterate=1;volume_iterate<number_of_volumes;volume_iterate++) {
      fprintf(fp,"V%d: %s  ",volume_iterate,Volumes[volume_iterate]->name);
      fprintf(fp,"Material: %s  ",Volumes[volume_iterate]->p_physics->name);
      for (process_iterate=0;process_iterate<Volumes[volume_iterate]->p_physics->number_of_processes;process_iterate++) {
	fprintf(fp," P%d: %s",process_iterate,Volumes[volume_iterate]->p_physics->p_scattering_array[process_iterate].name);
      }
      fprintf(fp,"\n");
    }
    fprintf(fp,"----- Histories sorted after intensity ----------------------------------------------------------------------------------\n");
    
    for (history_iterate=0;history_iterate<total_history.used_elements;history_iterate++) {
      fprintf_total_history(&total_history.saved_histories[history_iterate],fp);
      // Garbage collection
      if (total_history.saved_histories[history_iterate].used_elements > 0) free(total_history.saved_histories[history_iterate].elements);
    }
    fclose(fp);
  }
  )
  
  // Garbage collection
  if (total_history.allocated_elements > 0) free(total_history.saved_histories);
  
  
};


// -------------    Intersection table functions   --------------------------------------------------------
int clear_intersection_table(struct intersection_time_table_struct *intersection_time_table) {
    // Resets the intersection table when a scattering have occured.
    int iterate_volumes,iterate_solutions;
    
    // Start at one because vacuum (0) does not have a listing in the intersection table
    for (iterate_volumes = 1;iterate_volumes < intersection_time_table->num_volumes;iterate_volumes++) {
        intersection_time_table->calculated[iterate_volumes] = 0;
        // This second loop is added for safty in debugging phase, but can be removed as the information should never be accesed when calculated = 0
        for (iterate_solutions = 0;iterate_solutions < intersection_time_table->n_elements[iterate_volumes];iterate_solutions++) {
            intersection_time_table->intersection_times[iterate_volumes][iterate_solutions] = -1;
        }
    }
    
    return 1;
};

void print_intersection_table(struct intersection_time_table_struct *intersection_time_table) {

    int num_volumes,iterate,solutions;
    int max_number_of_solutions = 0;
    
    num_volumes = intersection_time_table->num_volumes;
    
    for (iterate = 0;iterate < num_volumes;iterate++) {
        if (max_number_of_solutions < intersection_time_table->n_elements[iterate])
            max_number_of_solutions = intersection_time_table->n_elements[iterate];
    }
    
    printf("------------------ INTERSECTION_TIME_TABLE -----------------");
    for (solutions = 2;solutions < max_number_of_solutions;solutions++) printf("------------");
    printf("\n");
    
    // printf("iterate      |");

    printf("           ");
    printf("| CALCULATED  |");
    for (solutions = 0;solutions < max_number_of_solutions;solutions++) {
        printf(" - SOLUTION %d - |", solutions);
    }
    for (solutions = 0;solutions < max_number_of_solutions;solutions++) {
        printf(" - SURFACE %d - |", solutions);
    }

    printf("\n");
    for (iterate = 0;iterate < num_volumes;iterate++){
    // print iterate number
    printf("Volume %d   |",iterate);
    printf(" ---- %d ---- |",intersection_time_table->calculated[iterate]);
    
        for (solutions = 0;solutions < max_number_of_solutions;solutions++) {
          if (intersection_time_table->n_elements[iterate] > solutions && intersection_time_table->calculated[iterate] == 1)
            if (intersection_time_table->intersection_times[iterate][solutions] > 0)
             printf("   %1.8f   |",intersection_time_table->intersection_times[iterate][solutions]);
            else
             printf("   %1.7f   |",intersection_time_table->intersection_times[iterate][solutions]);
          else
            printf("                |");
        }
        for (solutions = 0;solutions < max_number_of_solutions;solutions++) {
          if (intersection_time_table->n_elements[iterate] > solutions && intersection_time_table->calculated[iterate] == 1)
             printf("   %1.9d   |",intersection_time_table->surface_index[iterate][solutions]);
          else
            printf("               |");
        }
    printf("\n");
    }
    printf("------------------------------------------------------------");
    for (solutions = 2;solutions < max_number_of_solutions;solutions++) printf("------------");
    printf("\n");
    
    
    };
    
// -------------    Drawing functions   --------------------------------------------------------
void merge_lines_to_draw(struct lines_to_draw *lines_master,struct lines_to_draw *lines_new) {
    if (lines_master->number_of_lines == 0) {
    lines_master->number_of_lines = lines_new->number_of_lines;
    if (!lines_master->number_of_lines) {
      fprintf(stderr,"Failure allocating list in Union function merge_lines_to_draw 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    lines_master->lines = malloc(lines_master->number_of_lines*sizeof(struct line_segment));
    if (!lines_master->lines) {
      fprintf(stderr,"Failure allocating list in Union function merge_lines_to_draw 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    lines_master->lines = lines_new->lines;
    // One could free lines_new->lines;
    } else {
    int iterate;
    struct line_segment *temp_lines;
    temp_lines = malloc(lines_master->number_of_lines*sizeof(struct line_segment));
    if (!temp_lines) {
      fprintf(stderr,"Failure allocating list in Union function merge_lines_to_draw 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate = 0;iterate < lines_master->number_of_lines;iterate++) temp_lines[iterate] = lines_master->lines[iterate];
    free(lines_master->lines);
    lines_master->lines = malloc((lines_master->number_of_lines+lines_new->number_of_lines)*sizeof(struct line_segment));
    if (!lines_master->lines) {
      fprintf(stderr,"Failure allocating list in Union function merge_lines_to_draw 4 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate = 0;iterate < lines_master->number_of_lines;iterate++) lines_master->lines[iterate] = temp_lines[iterate];
    for (iterate = 0;iterate < lines_new->number_of_lines;iterate++) lines_master->lines[iterate+lines_master->number_of_lines] = lines_new->lines[iterate];
    lines_master->number_of_lines = lines_master->number_of_lines + lines_new->number_of_lines;
    free(temp_lines);
    }
    };

int r_has_highest_priority(Coords point,int N,struct geometry_struct **Geometries,int number_of_volumes) {
    // Function that test if a point in Volume N has the highest priority.
    // Returns 0 if another volume has higher priority at the point, and it's mask status is 1
    // Returns 1 if no other volume has higher priority at the point
    // (not active) Returns a larger integer if the volume N is a mask, and the point is not in the volume it is masking,
    //  which results in the drawing function making that number of dashes
    
    
    int mask_status,*mask_start,*mask_check;
    // If the volume is a mask, it does not have a priority, and is always on top
    if (Geometries[N]->is_mask_volume == 1) {
      return 1;
      // Can draw parts of the mask that is outside the volume it masks as dashed lines, but it just looks messy
      /*
      mask_status = 1;
      for (mask_check=mask_start=Geometries[N]->mask_list.elements;mask_check-mask_start<Geometries[N]->mask_list.num_elements;mask_check++) {
        if (Geometries[*mask_check]->within_function(point,Geometries[*mask_check]) == 1) {
          return 1; // If the point is within a volume the mask is masking, draw it as a solid line
        }
      }
      // As it was not inside any of the volumes the mask is masking, draw it as a dashed line
      return 5;
      */
    }
    
    // If the volume is a masked volume, check if the point is within it's masks
    if (Geometries[N]->is_masked_volume == 1) {
      if (Geometries[N]->mask_mode == 1) { // ALL mode, need to be within ALL masks
        for (mask_check=mask_start=Geometries[N]->masked_by_list.elements;mask_check-mask_start<Geometries[N]->masked_by_list.num_elements;mask_check++) {
          if (Geometries[*mask_check]->within_function(point,Geometries[*mask_check]) == 0) {
            return 0; // If the point is just outside one mask, the mask status is 0 and the point does not have highest priority
          }
        }
      } else { // ANY mode, need to be within at least one mask
        mask_status = 0;
        for (mask_check=mask_start=Geometries[N]->masked_by_list.elements;mask_check-mask_start<Geometries[N]->masked_by_list.num_elements;mask_check++) {
          if (Geometries[*mask_check]->within_function(point,Geometries[*mask_check]) == 1) {
            mask_status = 1;
            break;
          }
        }
        if (mask_status == 0) {
          return 0; // If it was not in a single of it's masks, the point did not have highest priority
        }
      }
    }
    
    int volume_index;
    double self_priority;
    self_priority = Geometries[N]->priority_value;
    
    
    for (volume_index = 1;volume_index<number_of_volumes;volume_index++) {
      if (volume_index != N && Geometries[volume_index]->is_mask_volume == 0) {
        if (Geometries[volume_index]->within_function(point,Geometries[volume_index])) {
          if (Geometries[volume_index]->is_masked_volume == 1) {
            // Since this volume is masked, the mask status need to be checked
            if (Geometries[volume_index]->mask_mode == 1) { //ALL mode, need to be within ALL masks
              mask_status = 1;
              for (mask_check=mask_start=Geometries[volume_index]->masked_by_list.elements;mask_check-mask_start<Geometries[volume_index]->masked_by_list.num_elements;mask_check++) {
                if (Geometries[*mask_check]->within_function(point,Geometries[*mask_check]) == 0) {
                  mask_status = 0;
                  break;
                }
              }
            } else { // ANY mode, need to be within at least one mask
              mask_status = 0;
              for (mask_check=mask_start=Geometries[volume_index]->masked_by_list.elements;mask_check-mask_start<Geometries[volume_index]->masked_by_list.num_elements;mask_check++) {
                if (Geometries[*mask_check]->within_function(point,Geometries[*mask_check]) == 1) {
                  mask_status = 1;
                  break;
                }
              }
            }
          } else mask_status = 1;
          if (Geometries[volume_index]->priority_value > self_priority && mask_status == 1) {
            // printf("Volume %d did not have highest priority at (%f,%f,%f) (Volume number %d was above)\n",N,point.x,point.y,point.z,volume_index);
            return 0;
          }
        }
      }
    }
    // printf("Volume %d did have highest priority at (%f,%f,%f) \n",N,point.x,point.y,point.z);
    return 1;
    }
    
void draw_line_positions(Coords point1,Coords point2) {
        //line(point1.x,point1.y,point1.z,point2.x,point2.y,point2.z);
        // sigh, can not use line in share. Need to save the information and pass to the mcdisplay part.
    }

int Sample_compare_doubles (const void *a, const void *b) {
  const double *da = (const double *) a;
  const double *db = (const double *) b;

  return (*da > *db) - (*da < *db);
}

struct lines_to_draw draw_line_with_highest_priority(Coords position1,Coords position2,int N,struct geometry_struct **Geometries,int number_of_volumes,int max_number_of_solutions) {
    
    int volume_index,iterate,permanent_list_length = 0;
    int number_of_solutions;
    double *temp_intersection=malloc(max_number_of_solutions*sizeof(double));
    if (!temp_intersection) {
      fprintf(stderr,"Failure allocating list in Union function lines_to_draw 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    double r1[3],r2[3],direction[3];
    struct pointer_to_1d_double_list intersection_list;
    
    intersection_list.num_elements = 0;
    
    // double *permanent_intersection_list,*storage; // old ways
    
    r1[0] = position1.x;
    r1[1] = position1.y;
    r1[2] = position1.z;
    r2[0] = position2.x;
    r2[1] = position2.y;
    r2[2] = position2.z;
    
    // printf("r1 = (%f,%f,%f) \n",r1[0],r1[1],r1[2]);
    // printf("r2 = (%f,%f,%f) \n",r2[0],r2[1],r2[2]);
    
    direction[0] = r2[0] - r1[0];
    direction[1] = r2[1] - r1[1];
    direction[2] = r2[2] - r1[2];
    int geometry_output;
    
    // Todo: switch to nicer intersect function call
    double *double_dummy = malloc(2*sizeof(double));
    int int_dummy[2];

	
    // Find intersections
    for (volume_index = 1;volume_index < number_of_volumes; volume_index++) {
        if (volume_index != N) {
         if (Geometries[volume_index]->eShape==mesh){
            double_dummy = realloc(double_dummy, sizeof(double)*1000);
            temp_intersection = realloc(temp_intersection, sizeof(double)*1000);
            
         }
            geometry_output = Geometries[volume_index]->intersect_function(temp_intersection, double_dummy, double_dummy, double_dummy, int_dummy, 
			                                                               &number_of_solutions, r1, direction, Geometries[volume_index]);
                for (iterate=0;iterate<number_of_solutions;iterate++) {
                    if (temp_intersection[iterate] > 0 && temp_intersection[iterate] < 1) {
		      add_element_to_double_list(&intersection_list,temp_intersection[iterate]);
		    }
                }
        }
    }
    free(temp_intersection);
    // Now we have a list of intersection distances between r1 and r2 and all volumes.
    // This list needs to be sorted before we continue!
    
    qsort(intersection_list.elements,intersection_list.num_elements,sizeof (double), Sample_compare_doubles);
    
    Coords *points=malloc((intersection_list.num_elements+2)*sizeof(Coords));
    if (!points) {
      fprintf(stderr,"Failure allocating list in Union function lines_to_draw 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    points[0] = coords_set(r1[0],r1[1],r1[2]);
    points[intersection_list.num_elements+1] = coords_set(r2[0],r2[1],r2[2]);
    for (iterate = 1;iterate < intersection_list.num_elements+1;iterate++) {
        points[iterate].x = r1[0] + direction[0]*intersection_list.elements[iterate-1];
        points[iterate].y = r1[1] + direction[1]*intersection_list.elements[iterate-1];
        points[iterate].z = r1[2] + direction[2]*intersection_list.elements[iterate-1];
    }
  
    struct line_segment *lines=malloc((intersection_list.num_elements+1)*sizeof(struct line_segment));
    if (!lines) {
      fprintf(stderr,"Failure allocating list in Union function lines_to_draw 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    int *draw_logic=malloc((intersection_list.num_elements+1)*sizeof(int));
    if (!draw_logic) {
      fprintf(stderr,"Failure allocating list in Union function lines_to_draw 4 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    Coords midpoint;
    struct lines_to_draw draw_order;
    draw_order.number_of_lines = 0;
    draw_order.lines=NULL;

    int number_of_dashes;
    
    for (iterate = 0;iterate < intersection_list.num_elements + 1;iterate++) {
        lines[iterate].point1 = points[iterate];
        lines[iterate].point2 = points[iterate+1];
        midpoint.x = 0.5*(lines[iterate].point1.x + lines[iterate].point2.x);
        midpoint.y = 0.5*(lines[iterate].point1.y + lines[iterate].point2.y);
        midpoint.z = 0.5*(lines[iterate].point1.z + lines[iterate].point2.z);
        
        if ((number_of_dashes = r_has_highest_priority(midpoint,N,Geometries,number_of_volumes)) != 0) {
            draw_order.number_of_lines++;
            draw_logic[iterate] = number_of_dashes;
        } else draw_logic[iterate] = 0;
    }
       
    if (draw_order.number_of_lines > 0) {
        draw_order.lines = malloc(draw_order.number_of_lines*sizeof(struct line_segment));
	if (!draw_order.lines) {
	  fprintf(stderr,"Failure allocating list in Union function lines_to_draw 5 - Exit!\n");
	  exit(EXIT_FAILURE);
	}
        draw_order.number_of_lines = 0;
        for (iterate = 0;iterate < intersection_list.num_elements + 1;iterate++) {
            if (draw_logic[iterate] != 0) {
              lines[iterate].number_of_dashes = draw_logic[iterate];
              draw_order.lines[draw_order.number_of_lines++] = lines[iterate];
            }
        }
        if (intersection_list.num_elements > 0) free(intersection_list.elements);
    }
    
    free(points);
    free(lines);
    free(draw_logic);
    return draw_order;
    }

struct lines_to_draw draw_circle_with_highest_priority(Coords center,Coords vector,double radius,int N,struct geometry_struct **Geometries,int number_of_volumes,int max_number_of_solutions) {
    int number_of_positions = 100;

    // normalize vector
    double vector_length = length_of_position_vector(vector);
    // print_position(vector,"start vector");
    vector.x /= vector_length;
    vector.y /= vector_length;
    vector.z /= vector_length;
    // print_position(vector,"start vector normalized");
    
    // Create a vector from the center of the circle to a point on the circumference by cross product
    double cross_input[3] = {0,1,0};
    // In case the cross input is parallel with the vector, a new is chosen. Both can't be parallel.
    if (scalar_prod(cross_input[0],cross_input[1],cross_input[2],vector.x,vector.y,vector.z) > 0.99) {
        cross_input[0] = 1; cross_input[1] = 0; cross_input[2] = 0;
    }
    // print_position(make_position(cross_input),"cross input");
    
    double cross_product1[3] = {0,0,0};
    vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],vector.x,vector.y,vector.z,cross_input[0],cross_input[1],cross_input[2]);
    
    // print_position(make_position(cross_product1),"cross_product1");
    
    double cross_length = length_of_3vector(cross_product1);
    
    // printf("cross_length = %f \n",cross_length);
    
    cross_product1[0] /= cross_length;
    cross_product1[1] /= cross_length;
    cross_product1[2] /= cross_length;
    
    cross_product1[0] *= radius;
    cross_product1[1] *= radius;
    cross_product1[2] *= radius;
    
    // printf("cross_product1 = (%f,%f,%f) \n",cross_product1[0],cross_product1[1],cross_product1[2]);
    
    int iterate;
    double rotate_angle;
    Coords radial_position,old_radial_position;
    struct lines_to_draw temp_draw_order,return_draw_order;
    return_draw_order.number_of_lines = 0;
    // Generate an array of positions by rotating this vector around the given vector, this corresponds to points on the circle
    old_radial_position.x = center.x + cross_product1[0];
    old_radial_position.y = center.y + cross_product1[1];
    old_radial_position.z = center.z + cross_product1[2];

    // printf("old_radial_position = (%f,%f,%f) \n",old_radial_position.x,old_radial_position.y,old_radial_position.z);
    
    for (iterate = 0;iterate < number_of_positions-1;iterate++) {
        rotate_angle = 2*3.14159*((double) iterate + 1.0)/((double) number_of_positions);
        rotate(radial_position.x,radial_position.y,radial_position.z,cross_product1[0],cross_product1[1],cross_product1[2],rotate_angle,vector.x,vector.y,vector.z);
        radial_position.x += center.x;
        radial_position.y += center.y;
        radial_position.z += center.z;
        
        // Use the draw_lines_with_highest_priority to draw get draw_orders for each line piece
        temp_draw_order = draw_line_with_highest_priority(radial_position,old_radial_position,N,Geometries,number_of_volumes,max_number_of_solutions);
       // Assemble these draw_orders to one large draw order
        merge_lines_to_draw(&return_draw_order,&temp_draw_order);

        old_radial_position.x = radial_position.x;
        old_radial_position.y = radial_position.y;
        old_radial_position.z = radial_position.z;
    }
    
    radial_position.x = center.x + cross_product1[0];
    radial_position.y = center.y + cross_product1[1];
    radial_position.z = center.z + cross_product1[2];
    // Use the draw_lines_with_highest_priority to draw get draw_orders for each line piece
    temp_draw_order = draw_line_with_highest_priority(radial_position,old_radial_position,N,Geometries,number_of_volumes,max_number_of_solutions);
    // Assemble these draw_orders to one large draw order
    merge_lines_to_draw(&return_draw_order,&temp_draw_order);
    
    // clean up
    if (temp_draw_order.number_of_lines > 0) free(temp_draw_order.lines);
    
    // return the large draw order.
    return return_draw_order;
    }

// -------------    Geometry functions   -----------------------------------------------------------------------
/*
 * This file section contains functions used for geometry.
 *
 * For each geometry A type there are:
 *  intersection function: determines intersection between straight line and the geometry type
 *  within function: determines wether a point is within the geometry, or outside
 *  geometryA_overlaps_geometryB: determines if geometry A overlaps with geometry B
 *  geometryA_inside_geometryB: determines if geometry A is completely inside geometry B
 *
 * At the end of the file, there is functions describing the logic for determining if one geometry
 *  is inside/overlaps another. It is placed here, so that all code to be expanded by adding a new
 *  geometry is within the same file.
 *
 * To add a new geometry one needs to:
 *  Write a geometry_storage_struct that contains the paramters needed to describe the geometry
 *  Add a pointer to this storage type in the geometry_parameter_union
 *  Write a function for intersection with line, using the same input scheme as for the others
 *  Write a function checking if a point is within the geometry
 *  Write a function checking if one instance of the geometry overlaps with another
 *  Write a function checking if one instance of the geometry is inside another
 *  For each exsisting geometry: 
 *      Write a function checking if an instance of this geometry overlaps with an instance of the exsisting
 *      Write a function checking if an instance of this geometry is inside an instance of the exsisting
 *      Write a function checking if an instance of an existing geometry is inside an instance of this geometry
 *
 *  Add these functions to geometry to the logic at the end of this file
 *  Write a component file similar to the exsisting ones, taking the input from the instrument file, and sending
 *   it on to the master component.
*/

struct sphere_storage{
double sph_radius;
};

struct cylinder_storage{
double cyl_radius;
double height;
Coords direction_vector;
};

struct box_storage{
double x_width1;
double y_height1;
double z_depth;
double x_width2;
double y_height2;
int is_rectangle; // Is rectangle = 1 if x_width1 = x_width2 / h1 = h2
Coords x_vector; // In main component frame
Coords y_vector;
Coords z_vector;
Coords normal_vectors[6]; // In local frame
};

struct cone_storage{
double cone_radius_top;
double cone_radius_bottom;
double height;
Coords direction_vector;
};

struct mesh_storage{
int n_facets;
int counter;
double *v1_x;
double *v1_y;
double *v1_z;
double *v2_x;
double *v2_y;
double *v2_z;
double *v3_x;
double *v3_y;
double *v3_z;
double *normal_x;
double *normal_y;
double *normal_z;
Coords direction_vector;
Coords Bounding_Box_Center;
double Bounding_Box_Radius;
};

// A number of functions below use Dot() as scalar product, replace by coords_sp define
#define Dot(a, b) coords_sp(a, b)

// Function for transforming a ray position / velocity to a local frame
Coords transform_position(Coords ray_position, Coords component_position, Rotation component_t_rotation) {

    Coords non_rotated_position = coords_sub(ray_position,component_position);
    
    // Rotate the position of the neutron around the center of the cylinder
    Coords rotated_coordinates = rot_apply(component_t_rotation,non_rotated_position);
    
    return rotated_coordinates;
}


union geometry_parameter_union allocate_box_storage_copy(union geometry_parameter_union *union_input) {
  union geometry_parameter_union union_output;
  // Allocate the space for a cylinder_storage structe in the new union_output (union as the c structre)
  union_output.p_box_storage = malloc(sizeof(struct box_storage));
  if (!union_output.p_box_storage) {
    fprintf(stderr,"Failure allocating list in Union function allocate_box_storage_copy - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // Copy the input storage to the output
  *union_output.p_box_storage = *union_input->p_box_storage;
  
  return union_output;
}


union geometry_parameter_union allocate_cylinder_storage_copy(union geometry_parameter_union *union_input) {
  union geometry_parameter_union union_output;
  // Allocate the space for a cylinder_storage structe in the new union_output (union as the c structre)
  union_output.p_cylinder_storage = malloc(sizeof(struct cylinder_storage));
  if (!union_output.p_cylinder_storage) {
    fprintf(stderr,"Failure allocating list in Union function allocate_cylinder_storage_copy - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // Copy the input storage to the output
  *union_output.p_cylinder_storage = *union_input->p_cylinder_storage;
  
  return union_output;
}

union geometry_parameter_union allocate_sphere_storage_copy(union geometry_parameter_union *union_input) {
  union geometry_parameter_union union_output;
  // Allocate the space for a cylinder_storage structe in the new union_output (union as the c structre)
  union_output.p_sphere_storage = malloc(sizeof(struct sphere_storage));
  if (!union_output.p_sphere_storage) {
    fprintf(stderr,"Failure allocating list in Union function allocate_sphere_storage_copy - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // Copy the input storage to the output
  *union_output.p_sphere_storage = *union_input->p_sphere_storage;
  
  return union_output;
}

union geometry_parameter_union allocate_cone_storage_copy(union geometry_parameter_union *union_input) {
  union geometry_parameter_union union_output;
  // Allocate the space for a cone_storage structe in the new union_output (union as the c structre)
  union_output.p_cone_storage = malloc(sizeof(struct cone_storage));
  if (!union_output.p_cone_storage) {
    fprintf(stderr,"Failure allocating list in Union function allocate_cone_storage_copy - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // Copy the input storage to the output
  *union_output.p_cone_storage = *union_input->p_cone_storage;
  
  return union_output;
}

union geometry_parameter_union allocate_mesh_storage_copy(union geometry_parameter_union *union_input) {
  union geometry_parameter_union union_output;
  // Allocate the space for a mesh_storage structe in the new union_output (union as the c structre)
  union_output.p_mesh_storage = malloc(sizeof(struct mesh_storage));
  if (!union_output.p_mesh_storage) {
    fprintf(stderr,"Failure allocating list in Union function allocate_mesh_storage_copy - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // Copy the input storage to the output
  *union_output.p_mesh_storage = *union_input->p_mesh_storage;
  
  return union_output;
}

// -------------    Surroundings  ---------------------------------------------------------------
int r_within_surroundings(Coords pos,struct geometry_struct *geometry) {
    // The surroundings are EVERYWHERE
        return 1;
    }

// -------------    General geometry ------------------------------------------------------------

Coords point_on_circle(Coords center, Coords direction, double radius, int point_nr, int number_of_points) {

    Coords output;
    Coords cross_input = coords_set(0,1,0);
    
    if (scalar_prod(cross_input.x,cross_input.y,cross_input.z,direction.x,direction.y,direction.z) > 0.9) {
            cross_input.x = 1; cross_input.y = 0; cross_input.z = 0;
    }
    
    Coords cross_product;
    vec_prod(cross_product.x,cross_product.y,cross_product.z,direction.x,direction.y,direction.z,cross_input.x,cross_input.y,cross_input.z);

    double cross_length = length_of_position_vector(cross_product);
    double radius_over_cross_length = radius/cross_length;
    cross_product = coords_scalar_mult(cross_product,radius_over_cross_length);
    
    
    double rotate_angle = 2*PI*((double) point_nr)/((double) number_of_points);
    
    rotate(output.x,output.y,output.z,cross_product.x,cross_product.y,cross_product.z,rotate_angle,direction.x,direction.y,direction.z);
    
    output = coords_add(output,center);
    
    return output;
};

void points_on_circle(Coords *output, Coords center, Coords direction, double radius, int number_of_points) {

    Coords cross_input = coords_set(0,1,0);
    
    if (scalar_prod(cross_input.x,cross_input.y,cross_input.z,direction.x,direction.y,direction.z) > 0.9) {
            cross_input.x = 1; cross_input.y = 0; cross_input.z = 0;
    }
    
    Coords cross_product;
    vec_prod(cross_product.x,cross_product.y,cross_product.z,direction.x,direction.y,direction.z,cross_input.x,cross_input.y,cross_input.z);

    double cross_length = length_of_position_vector(cross_product);
    double radius_over_cross_length = radius/cross_length;
    cross_product = coords_scalar_mult(cross_product,radius_over_cross_length);
    
    int point_nr;
    double rotate_angle;
    for (point_nr = 0;point_nr<number_of_points;point_nr++) {
        rotate_angle = 2*PI*((double) point_nr)/((double) number_of_points);
        rotate(output[point_nr].x,output[point_nr].y,output[point_nr].z,cross_product.x,cross_product.y,cross_product.z,rotate_angle,direction.x,direction.y,direction.z);
        output[point_nr] = coords_add(output[point_nr],center);
    }
};

// -------- Brute force last resorts for within / overlap -------------------------------------

int A_within_B(struct geometry_struct *child, struct geometry_struct *parent, int resolution) {
  // This function assumes the parent (B) is a convex geoemtry
  // If all points on the shell of geometry A is within B, so are all lines between them.
  
  // Should be done first, but takes so little time and may save a longer computation
  if (parent->within_function(child->center,parent) == 0) return 0;
  
  // resolution selects the number of points to be generated on the shell.
  struct pointer_to_1d_coords_list shell_points;
  shell_points = child->shell_points(child,resolution);
  // Shell_points.elements need to be freed before leaving this function
  
  if (shell_points.num_elements > resolution || shell_points.num_elements < 0) {
    printf("\nERROR: Shell point function used in A_within_B return garbage num_elements. \n");
    exit(EXIT_FAILURE);
  }
  
  int iterate;
  
  for (iterate=0;iterate<shell_points.num_elements;iterate++) {
    if (parent->within_function(shell_points.elements[iterate],parent) == 0) {
      free(shell_points.elements);
      return 0;
    }
  }
  
  free(shell_points.elements);
  
  // If all points are inside, the entire geometry is assumed inside as parent should be convex
  return 1;
}

int mesh_A_within_B(struct geometry_struct *child, struct geometry_struct *parent) {
  // This function assumes the parent (B) is a convex geoemtry
  // If all points on the shell of geometry A is within B, so are all lines between them.
  // This is modified so resolution is not set manually, but all mesh shell points are taken
  
  // resolution selects the number of points to be generated on the shell.
  struct pointer_to_1d_coords_list shell_points;
  int resolution = 300;
  shell_points = child->shell_points(child, resolution); // mesh shell points do not use max points
  // Shell_points.elements need to be freed before leaving this function
  //printf("\n GOT OUT TEST");
  int iterate;
  
  for (iterate=0;iterate<shell_points.num_elements;iterate++) {
    if (parent->within_function(shell_points.elements[iterate],parent) == 0) {
      free(shell_points.elements);
      return 0;
    }
  }
  
  
  // If all points are inside, the entire geometry is assumed inside as parent should be convex
  free(shell_points.elements);
  return 1;
}

/*
// Turned out to be harder to generalize the overlap functions, but at least within was doable.
int A_overlaps_B(struct geometry_struct *child, struct geometry_struct *parent) {
  // This function assumes the parent (B) is a convex geoemtry
  // Does not work, need to check lines between points
  
  // Starting this system with a simple constant 64 point generation.
  struct pointer_to_1d_coords_list shell_points;
  shell_points = child.shell_points(child,64);
  
  int iterate;
  
  for (iterate=0;iterate<shell_points.num_elements;iterate++) {
    if (parent.within_function(shell_points.elements[iterate],parent) == 1) return 1;
  }
  
  
  // This requires that the points on the shell are saved pair wise in such a way that
  //  the lines between them would cover the entire surface when the resolution goes to
  //  infinity. NOT GOING TO WORK FOR BOX
  
  
  for (iterate=0;iterate<floor(shell_points.num_elements/2);iterate = iterate + 2) {
    // check intersections with parent between the two child points
    if (existence_of_intersection(shell_points.elements[iterate],shell_points.elements[iterate+1],parent) == 1) return 1;
  }
  
  
  // If no points were inside, the geometries are assumed not to overlap as parent should be convex
  return 0;
}
*/



// -------------    Functions for box ray tracing used in trace ---------------------------------
// These functions needs to be fast, as they may be used many times for each ray
int sample_box_intersect_advanced(double *t, double *nx, double *ny, double *nz, int *surface_index, int *num_solutions,double *r,double *v,struct geometry_struct *geometry) {
    // possible approaches
    // rotate to a simple coordinate system by rotating the ray (easier to switch to McStas standard)
    
    // There are still many variables here that can be pre calculated and saved in the box_storage.
    
    double depth = geometry->geometry_parameters.p_box_storage->z_depth;
    double width1 = geometry->geometry_parameters.p_box_storage->x_width1;
    double width2 = geometry->geometry_parameters.p_box_storage->x_width2;
    double height1 = geometry->geometry_parameters.p_box_storage->y_height1;
    double height2 = geometry->geometry_parameters.p_box_storage->y_height2;
    
    Coords x_vector = geometry->geometry_parameters.p_box_storage->x_vector;
    Coords y_vector = geometry->geometry_parameters.p_box_storage->y_vector;
    Coords z_vector = geometry->geometry_parameters.p_box_storage->z_vector;
    
    Coords normal_vectors;
    
    // Declare variables for the function
    Coords coordinates;
    
    // Coordinate transformation
    coordinates.x = r[0] - geometry->center.x;
    coordinates.y = r[1] - geometry->center.y;
    coordinates.z = r[2] - geometry->center.z;
    
    Coords rotated_coordinates;
    // Rotate the position of the neutron around the center of the cylinder
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
    
    Coords velocity = coords_set(v[0],v[1],v[2]);
    Coords rotated_velocity;
    
    // Rotate the position of the neutron around the center of the cylinder
    rotated_velocity = rot_apply(geometry->transpose_rotation_matrix,velocity);
    
    double x_result,y_result,z_result;
    
    *num_solutions = 0;
    // normal_vectors 0 and 1 have x and y = 0;
    // normal vectors 0: point in plane [0 0 -0.5*depth]
    normal_vectors = geometry->geometry_parameters.p_box_storage->normal_vectors[0];
    t[*num_solutions] = (-0.5*depth - rotated_coordinates.z)*normal_vectors.z/(normal_vectors.z*rotated_velocity.z);
    //printf("Intersection_time for face 0 = %f\n",t[*num_solutions]);
    x_result = rotated_coordinates.x + t[*num_solutions]*rotated_velocity.x;
    y_result = rotated_coordinates.y + t[*num_solutions]*rotated_velocity.y;
    z_result = rotated_coordinates.z + t[*num_solutions]*rotated_velocity.z; // only for debug
    //printf("Test solution for face number 0: (x,y) = (%f,%f,%f)\n",x_result,y_result,z_result);
    if (x_result >= -0.5*width1 && x_result <= 0.5*width1 && y_result >= -0.5*height1 && y_result <= 0.5*height1) {
		nx[*num_solutions] = normal_vectors.x;
		ny[*num_solutions] = normal_vectors.y;
		nz[*num_solutions] = -normal_vectors.z;
		surface_index[*num_solutions] = 0;
        (*num_solutions)++;		
        //printf("Solution found for face number 0\n");
    }
    
    // normal vectors 1: point in plane [0 0 0.5*depth]
    normal_vectors = geometry->geometry_parameters.p_box_storage->normal_vectors[1];
    t[*num_solutions] = (0.5*depth - rotated_coordinates.z)*normal_vectors.z/(normal_vectors.z*rotated_velocity.z);
    //printf("Intersection_time for face 1 = %f\n",t[*num_solutions]);
    x_result = rotated_coordinates.x + t[*num_solutions]*rotated_velocity.x;
    y_result = rotated_coordinates.y + t[*num_solutions]*rotated_velocity.y;
    //z_result = rotated_coordinates.z + t[*num_solutions]*rotated_velocity.z; // only for debug
    //printf("Test solution for face number 1: (x,y) = (%f,%f,%f)\n",x_result,y_result,z_result);
    if (x_result >= -0.5*width2 && x_result <= 0.5*width2 && y_result >= -0.5*height2 && y_result <= 0.5*height2) {
		nx[*num_solutions] = normal_vectors.x;
		ny[*num_solutions] = normal_vectors.y;
		nz[*num_solutions] = normal_vectors.z;
		surface_index[*num_solutions] = 1;				
        (*num_solutions)++;		
        //printf("Solution found for face number 1\n");
    }
    // These were done first as they are fastest, and most likely to be the solutions (normal to do small depth and large width/height), and standard orientation is to have one of these faces towards the source. When the fastest and most likely are done first, there is larger chance to skip more and slower calculations
    
    if (*num_solutions != 2) {
        // normal vectors 2 and 3 have y = 0
        normal_vectors = geometry->geometry_parameters.p_box_storage->normal_vectors[2];
        t[*num_solutions] = ((0.5*width1 - rotated_coordinates.x)*normal_vectors.x + (-0.5*depth - rotated_coordinates.z)*normal_vectors.z)/(normal_vectors.x*rotated_velocity.x+normal_vectors.z*rotated_velocity.z);
        // x_result = rotated_coordinates.x + t[*num_solutions]*rotated_velocity.x;
        y_result = rotated_coordinates.y + t[*num_solutions]*rotated_velocity.y;
        z_result = rotated_coordinates.z + t[*num_solutions]*rotated_velocity.z;
        if (z_result > -0.5*depth && z_result < 0.5*depth && y_result >= -0.5*(height1+(height2-height1)*(0.5*depth+z_result)/depth) && y_result < 0.5*(height1+(height2-height1)*(0.5*depth+z_result)/depth)) {
			nx[*num_solutions] = normal_vectors.x;
			ny[*num_solutions] = normal_vectors.y;
			nz[*num_solutions] = normal_vectors.z;	
			surface_index[*num_solutions] = 2;					
            (*num_solutions)++;			
            //printf("Solution found for face number 2\n");
        }
    }
    
    if (*num_solutions != 2) {
        // normal vectors 2 and 3 have y = 0
        normal_vectors = geometry->geometry_parameters.p_box_storage->normal_vectors[3];
        t[*num_solutions] = ((-0.5*width1 - rotated_coordinates.x)*normal_vectors.x + (-0.5*depth - rotated_coordinates.z)*normal_vectors.z)/(normal_vectors.x*rotated_velocity.x+normal_vectors.z*rotated_velocity.z);
        // x_result = rotated_coordinates.x + t[*num_solutions]*rotated_velocity.x;
        y_result = rotated_coordinates.y + t[*num_solutions]*rotated_velocity.y;
        z_result = rotated_coordinates.z + t[*num_solutions]*rotated_velocity.z;
        if (z_result > -0.5*depth && z_result < 0.5*depth && y_result > -0.5*(height1+(height2-height1)*(0.5*depth+z_result)/depth) && y_result <= 0.5*(height1+(height2-height1)*(0.5*depth+z_result)/depth)) {
			nx[*num_solutions] = normal_vectors.x;
			ny[*num_solutions] = normal_vectors.y;
			nz[*num_solutions] = normal_vectors.z;
			surface_index[*num_solutions] = 3;						
            (*num_solutions)++;			
            //printf("Solution found for face number 3\n");
        }
    }
    
    if (*num_solutions != 2) {
        // normal vectors 4 and 5 have x = 0
        normal_vectors = geometry->geometry_parameters.p_box_storage->normal_vectors[4];
        t[*num_solutions] = ((0.5*height1 - rotated_coordinates.y)*normal_vectors.y + (-0.5*depth - rotated_coordinates.z)*normal_vectors.z)/(normal_vectors.y*rotated_velocity.y+normal_vectors.z*rotated_velocity.z);
        x_result = rotated_coordinates.x + t[*num_solutions]*rotated_velocity.x;
        //y_result = rotated_coordinates.y + t[*num_solutions]*rotated_velocity.y;
        z_result = rotated_coordinates.z + t[*num_solutions]*rotated_velocity.z;
        if (z_result > -0.5*depth && z_result < 0.5*depth && x_result >= -0.5*(width1+(width2-width1)*(0.5*depth+z_result)/depth) && x_result < 0.5*(width1+(width2-width1)*(0.5*depth+z_result)/depth)) {
			nx[*num_solutions] = normal_vectors.x;
			ny[*num_solutions] = normal_vectors.y;
			nz[*num_solutions] = normal_vectors.z;
			surface_index[*num_solutions] = 4;			
            (*num_solutions)++;			
            //printf("Solution found for face number 4\n");
        }
    }
    
    if (*num_solutions != 2) {
        // normal vectors 4 and 5 have x = 0
        normal_vectors = geometry->geometry_parameters.p_box_storage->normal_vectors[5];
        t[*num_solutions] = ((-0.5*height1 - rotated_coordinates.y)*normal_vectors.y + (-0.5*depth - rotated_coordinates.z)*normal_vectors.z)/(normal_vectors.y*rotated_velocity.y+normal_vectors.z*rotated_velocity.z);
        x_result = rotated_coordinates.x + t[*num_solutions]*rotated_velocity.x;
        //y_result = rotated_coordinates.y + t[*num_solutions]*rotated_velocity.y;
        z_result = rotated_coordinates.z + t[*num_solutions]*rotated_velocity.z;
        if (z_result > -0.5*depth && z_result < 0.5*depth && x_result > -0.5*(width1+(width2-width1)*(0.5*depth+z_result)/depth) && x_result <= 0.5*(width1+(width2-width1)*(0.5*depth+z_result)/depth)) {
			nx[*num_solutions] = normal_vectors.x;
			ny[*num_solutions] = normal_vectors.y;
			nz[*num_solutions] = normal_vectors.z;
			surface_index[*num_solutions] = 5;						
            (*num_solutions)++;			
            //printf("Solution found for face number 5\n");
        }
    }
	
	Coords normal_vector_rotated;
	Coords normal_vector;
	
	// Sort solution according to intersection time and rotate normal vectors to master coordinate system 
    switch(*num_solutions) {
    case 2:
        if (t[0] > t[1]) {
            double temp = t[1];
            t[1] = t[0];
            t[0] = temp;
			
			// Also switch the normal vectors
			temp = nx[1];
			nx[1] = nx[0];
			nx[0] = temp;
			
			temp = ny[1];
			ny[1] = ny[0];
			ny[0] = temp;
			
			temp = nz[1];
			nz[1] = nz[0];
			nz[0] = temp;
			
			// Switch surface_index
			int temp_int = surface_index[1];
			surface_index[1] = surface_index[0];
			surface_index[0] = temp_int;
        }
		
		// Rotate back to master coordinate system	
		normal_vector_rotated = coords_set(nx[0], ny[0], nz[0]);
	    normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		nx[0] = normal_vector.x; ny[0] = normal_vector.y; nz[0] = normal_vector.z;
		
		normal_vector_rotated = coords_set(nx[1], ny[1], nz[1]);
	    normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		nx[1] = normal_vector.x; ny[1] = normal_vector.y; nz[1] = normal_vector.z;		
		
        return 1;
    case 1:
        t[1] = -1;
		
		// Rotate back to master coordinate system	
		normal_vector_rotated = coords_set(nx[0], ny[0], nz[0]);
	    normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		nx[0] = normal_vector.x; ny[0] = normal_vector.y; nz[0] = normal_vector.z;
		
        return 1;
    case 0:
        t[0] = -1;
        t[1] = -1;
        return 0;
    }
    
    // Above switch will catch all solutions, but the return 0 here silences a compiler warning.
    return 0;
};

void box_corners_global_frame(Coords *corner_points, struct geometry_struct *geometry) {
    // Returns a pointer to an array containing the 8 positions of the corners of the box.
    double depth = geometry->geometry_parameters.p_box_storage->z_depth;
    double width1 = geometry->geometry_parameters.p_box_storage->x_width1;
    double width2 = geometry->geometry_parameters.p_box_storage->x_width2;
    double height1 = geometry->geometry_parameters.p_box_storage->y_height1;
    double height2 = geometry->geometry_parameters.p_box_storage->y_height2;
    
    Coords x_vector = geometry->geometry_parameters.p_box_storage->x_vector;
    Coords y_vector = geometry->geometry_parameters.p_box_storage->y_vector;
    Coords z_vector = geometry->geometry_parameters.p_box_storage->z_vector;
    
    Coords center = geometry->center;
    
    corner_points[0] = coords_add(coords_add(coords_add(center,coords_scalar_mult(z_vector,-0.5*depth)),coords_scalar_mult(x_vector,-0.5*width1)),coords_scalar_mult(y_vector,-0.5*height1));
    
    corner_points[1] = coords_add(corner_points[0],coords_scalar_mult(x_vector,width1));
    corner_points[2] = coords_add(corner_points[1],coords_scalar_mult(y_vector,height1));
    corner_points[3] = coords_add(corner_points[0],coords_scalar_mult(y_vector,height1));
    
    corner_points[4] = coords_add(coords_add(coords_add(center,coords_scalar_mult(z_vector,0.5*depth)),coords_scalar_mult(x_vector,-0.5*width2)),coords_scalar_mult(y_vector,-0.5*height2));
    
    corner_points[5] = coords_add(corner_points[4],coords_scalar_mult(x_vector,width2));
    corner_points[6] = coords_add(corner_points[5],coords_scalar_mult(y_vector,height2));
    corner_points[7] = coords_add(corner_points[4],coords_scalar_mult(y_vector,height2));
};

void box_corners_local_frame(Coords *corner_points, struct geometry_struct *geometry) {
    double depth = geometry->geometry_parameters.p_box_storage->z_depth;
    double width1 = geometry->geometry_parameters.p_box_storage->x_width1;
    double width2 = geometry->geometry_parameters.p_box_storage->x_width2;
    double height1 = geometry->geometry_parameters.p_box_storage->y_height1;
    double height2 = geometry->geometry_parameters.p_box_storage->y_height2;
    
    Coords center = geometry->center;
    Coords x_vector = coords_set(1,0,0);
    Coords y_vector = coords_set(0,1,0);
    Coords z_vector = coords_set(0,0,1);
    Coords origo = coords_set(0,0,0);

    // Bug fixed on 25/11, center was used instead of origo
    corner_points[0] = coords_add(coords_add(coords_add(origo,coords_scalar_mult(z_vector,-0.5*depth)),coords_scalar_mult(x_vector,-0.5*width1)),coords_scalar_mult(y_vector,-0.5*height1));
    
    corner_points[1] = coords_add(corner_points[0],coords_scalar_mult(x_vector,width1));
    corner_points[2] = coords_add(corner_points[1],coords_scalar_mult(y_vector,height1));
    corner_points[3] = coords_add(corner_points[0],coords_scalar_mult(y_vector,height1));
    
    corner_points[4] = coords_add(coords_add(coords_add(origo,coords_scalar_mult(z_vector,0.5*depth)),coords_scalar_mult(x_vector,-0.5*width2)),coords_scalar_mult(y_vector,-0.5*height2));
    
    corner_points[5] = coords_add(corner_points[4],coords_scalar_mult(x_vector,width2));
    corner_points[6] = coords_add(corner_points[5],coords_scalar_mult(y_vector,height2));
    corner_points[7] = coords_add(corner_points[4],coords_scalar_mult(y_vector,height2));
};

int sample_box_intersect_simple(double *t, double *nx, double *ny, double *nz, int *surface_index, int *num_solutions, double *r, double *v, struct geometry_struct *geometry) {
    double width = geometry->geometry_parameters.p_box_storage->x_width1;
    double height = geometry->geometry_parameters.p_box_storage->y_height1;
    double depth = geometry->geometry_parameters.p_box_storage->z_depth;
	
    // Declare variables for the function
    double x_new,y_new,z_new;
    
    // Coordinate transformation
    x_new = r[0] - geometry->center.x;
    y_new = r[1] - geometry->center.y;
    z_new = r[2] - geometry->center.z;
    
    Coords coordinates = coords_set(x_new,y_new,z_new);
    Coords rotated_coordinates;
    //printf("Cords coordinates = (%f,%f,%f)\n",coordinates.x,coordinates.y,coordinates.z);
    
    // Rotate the position of the neutron around the center of the cylinder
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
    // rotated_coordinates = rot_apply(rotation_matrix_debug,coordinates);
    //printf("Cords rotated_coordinates = (%f,%f,%f)\n",rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z);
    
    Coords velocity = coords_set(v[0],v[1],v[2]);
    Coords rotated_velocity;
    //printf("Cords velocity = (%f,%f,%f)\n",velocity.x,velocity.y,velocity.z);
    
    // Rotate the position of the neutron around the center of the cylinder
    rotated_velocity = rot_apply(geometry->transpose_rotation_matrix,velocity);
    // rotated_velocity = rot_apply(rotation_matrix_debug,velocity);
    //printf("Cords rotated_velocity = (%f,%f,%f)\n",rotated_velocity.x,rotated_velocity.y,rotated_velocity.z);
    
    int output = 0;
    // Run McStas built in box intersect funtion (box centered around origin)
    if ((output = box_intersect(&t[0],&t[1],rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z,rotated_velocity.x,rotated_velocity.y,rotated_velocity.z,width,height,depth)) == 0) {
        *num_solutions = 0;t[0]=-1;t[1]=-1;
	}
    else if (t[1] != 0) *num_solutions = 2;
    else {*num_solutions = 1;t[1]=-1;} // t[2] is a memory error!
	
    // Rewritten code from refractor.comp
    int index;
    double x, y, z, dt;
    double rotated_nx, rotated_ny, rotated_nz;
    Coords normal_vector_rotated;
    Coords normal_vector;
    for (index=0; index<*num_solutions; index++) {

        dt = t[index];
        
        // Intersection point in box coordinate system
        x = rotated_coordinates.x + dt*rotated_velocity.x;
        y = rotated_coordinates.y + dt*rotated_velocity.y;
        z = rotated_coordinates.z + dt*rotated_velocity.z;
        
        // determine hit face: difference to plane is closest to 0 (in box coordinate system)
		// A deviation of 0 means its on that surface, due to finite accuracy it will never be exact, so the closest is chosen
		double x_deviation = fabs(fabs(x/width) - 0.5); 
	    double y_deviation =  fabs(fabs(y/height) - 0.5);
		double z_deviation = fabs(fabs(z/depth) - 0.5);
		
		if (x_deviation <= y_deviation && x_deviation <= z_deviation) {
		    normal_vector_rotated = coords_set(x > 0 ? 1.0 : -1.0, 0.0, 0.0);
		} else if (y_deviation <= x_deviation && y_deviation <= z_deviation) {
		    normal_vector_rotated = coords_set(0.0, y > 0 ? 1.0 : -1.0, 0.0);
		} else {
		    normal_vector_rotated = coords_set(0.0, 0.0, z > 0 ? 1.0 : -1.0);
		}
		
		// Set surface index
		if (normal_vector_rotated.z < -0.5)
			// back
			surface_index[index] = 0;
		else if(normal_vector_rotated.z > 0.5)
			// front
			surface_index[index] = 1;
		else if(normal_vector_rotated.x > 0.5)
			// left
			surface_index[index] = 2;			
		else if(normal_vector_rotated.x < -0.5)
			// right
			surface_index[index] = 3;			
		else if(normal_vector_rotated.y > 0.5)
			// top
			surface_index[index] = 4;			
		else if(normal_vector_rotated.y < -0.5)
			// bottom
			surface_index[index] = 5;
		
        // Rotate back to master coordinate system
        normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		
        // Set the normal vector components
        nx[index] = normal_vector.x;
        ny[index] = normal_vector.y;
        nz[index] = normal_vector.z;
		
		NORM(nx[index], ny[index], nz[index]);
    }
    
    return output;
};

int r_within_box_simple(Coords pos,struct geometry_struct *geometry) {
    // Unpack parameters
    double width = geometry->geometry_parameters.p_box_storage->x_width1;
    double height = geometry->geometry_parameters.p_box_storage->y_height1;
    double depth = geometry->geometry_parameters.p_box_storage->z_depth;
    
    //Coords coordinates = coords_set(x_new,y_new,z_new);
    Coords coordinates = coords_sub(pos,geometry->center);
    
    Coords rotated_coordinates;
    // printf("Cords coordinates = (%f,%f,%f)\n",coordinates.x,coordinates.y,coordinates.z);
    
    // Rotate the position of the neutron around the center of the cylinder
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
    
    // May be faster to check for one at a time to get an early return 0
    return (rotated_coordinates.x > -0.5*width && rotated_coordinates.x < 0.5*width && rotated_coordinates.y > -0.5*height && rotated_coordinates.y < 0.5*height && rotated_coordinates.z > -0.5*depth && rotated_coordinates.z < 0.5*depth);
};

int r_within_cone(Coords pos,struct geometry_struct *geometry) {
    // Is point inside cone?
// Unpack parameters
    double radius_top = geometry->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom = geometry->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height = geometry->geometry_parameters.p_cone_storage->height;
    Coords center = geometry->center;
    
    double x_new,y_new,z_new;
    
    // Coordinate transformation
    x_new = pos.x - geometry->center.x;
    y_new = pos.y - geometry->center.y;
    z_new = pos.z - geometry->center.z;
    
    Coords coordinates = coords_set(x_new,y_new,z_new);
    Coords rotated_coordinates;
   
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);

    
    int verbal = 0;
    // Generate unit direction vector along center axis of cones
    
    // Start with vector that points along the cone in the simple frame, and rotate to global
    Coords simple_vector = coords_set(0,1,0);
    
    int inside = 1;
    
    if (height*0.5 < fabs(rotated_coordinates.y)) {
            if (verbal == 1) printf("point sticks out height wise \n");
            inside = 0;
    } else {

        // Test for separation radially
        // if (rSum − |Delta − Dot(W0,Delta)∗W0| < 0) seperated = 1;
        // double vector_between_cone_axis[3];
        // vector_between_cone_axis[0] = delta.x - scalar_prod1*vector1.x;
        // vector_between_cone_axis[1] = delta.y - scalar_prod1*vector1.y;
        // vector_between_cone_axis[2] = delta.z - scalar_prod1*vector1.z;

        Coords vector1 = coords_sub(rotated_coordinates,center);
        //printf("\nrotated coordinates = [%f,%f,%f]",rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z);

        // Calculate radius at the y height of the tested point
        double cone_slope = (radius_top-radius_bottom)/height;
        double radius_pos = radius_bottom + cone_slope * (rotated_coordinates.y + (height/2.0)); // Here delta.y is used. Make sure that y is in the height direction of cone...
        
        //printf("\nradius_pos = %f",radius_pos);
        //printf("\nradius_pos distance = %f",sqrt(rotated_coordinates.x*rotated_coordinates.x+rotated_coordinates.z*rotated_coordinates.z));

        

        if (radius_pos *radius_pos < (rotated_coordinates.x*rotated_coordinates.x+rotated_coordinates.z*rotated_coordinates.z)) {
            if (verbal == 1) printf("Point sticks out radially \n");
            inside = 0;
        }
        //printf("\n IS INSIDE? %i",inside);
    }

    if (inside == 0) return 0;
    else return 1;
    };

int sample_cone_intersect(double *t, double *nx, double *ny, double *nz, int *surface_index, int *num_solutions, double *r, double *v, struct geometry_struct *geometry) {

    /*
    double radius_top = geometry->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom = geometry->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height = geometry->geometry_parameters.p_cone_storage->height;
    */

    double radius_top = geometry->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom = geometry->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height = geometry->geometry_parameters.p_cone_storage->height;
    
    
    
    //Coords direction = geometry->geometry_parameters.p_cone_storage->direction_vector;
    Coords center = geometry->center;

    Coords direction = coords_set(0,1,0);



    //Coords bottom_point = coords_add(center,coords_scalar_mult(direction,-0.5*height));
    //Coords top_point = coords_add(center,coords_scalar_mult(direction,0.5*height));

    // Declare variables for the function
    double x_new,y_new,z_new;
    
    // Coordinate transformation
    x_new = r[0] - geometry->center.x;
    y_new = r[1] - geometry->center.y;
    z_new = r[2] - geometry->center.z;
    
    Coords coordinates = coords_set(x_new,y_new,z_new);
    Coords rotated_coordinates;
    // printf("Cords coordinates = (%f,%f,%f)\n",coordinates.x,coordinates.y,coordinates.z);
    
    // debug
    // Rotation rotation_matrix_debug[3][3];
    // rot_set_rotation(rotation_matrix_debug,-1.0*geometry->rotation.x,-1.0*geometry->rotation.y,-1.0*geometry->rotation.z);
    // rot_transpose(geometry->rotation_matrix,rotation_matrix_debug);

    // Rotate the position of the neutron around the center of the cone
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
    // rotated_coordinates = rot_apply(rotation_matrix_debug,coordinates);
    //     printf("Cords rotated_coordinates = (%f,%f,%f)\n",rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z);
    
    Coords velocity = coords_set(v[0],v[1],v[2]);
    Coords rotated_velocity;
    //     printf("Cords velocity = (%f,%f,%f)\n",velocity.x,velocity.y,velocity.z);
    
    // Rotate the position of the neutron around the center of the cone
    rotated_velocity = rot_apply(geometry->transpose_rotation_matrix,velocity);
    // rotated_velocity = rot_apply(rotation_matrix_debug,velocity);
    //     printf("Cords rotated_velocity = (%f,%f,%f)\n",rotated_velocity.x,rotated_velocity.y,rotated_velocity.z);
    

    Coords normal_vector_rotated;
    Coords normal_vector;
	    
    // Test if the ray gets close to the cone by making a sphere around cone and check intersection
    double Y;
    double max_r;

    Y = -(0.5*height)-(radius_top*radius_top-radius_bottom*radius_bottom)/(2*height);
    if (radius_top > radius_bottom){
        max_r = radius_top;
    }else{
        max_r = radius_bottom;
    }
    double sphere_radius =  sqrt((Y+(1/2)*height)*(Y+(1/2)*height)+max_r*max_r);
    Coords sphere_pos = coords_set(center.x+direction.x*Y,center.y+direction.y*Y,center.z+direction.z*Y);
    
    double x_sphere = sphere_pos.x - geometry->center.x;
    double y_sphere = sphere_pos.y - geometry->center.y;
    double z_sphere = sphere_pos.z - geometry->center.z;
    double sphere_t[2];
    
    
    int output = 0;
    // Run McStas built in sphere intersect funtion (sphere centered around origin)
    if ((output = sphere_intersect(&sphere_t[0],&sphere_t[1],x_sphere,y_sphere,z_sphere,v[0],v[1],v[2],sphere_radius)) == 0)
    

    if (sphere_t[0] > -1){
        if (sphere_t[1] > -1){
            t[0] = -1;
            t[1] = -1;
            return 0;
        }
    }
    
    //Coords normal_vector_rotated;
	//Coords normal_vector;
    
    double tmp;
    

    // Check if the ray intersects with the top and bottom circles
    double t_plane[2];
    t_plane[0] = (height/2 - rotated_coordinates.y) / rotated_velocity.y;
    t_plane[1] = (-height/2 - rotated_coordinates.y) / rotated_velocity.y;
    *num_solutions = 2;
    // Reduce from infinite plane to circles
            //  sqrt(xpos^2 + zpos^2) > r  => t = -1
    double xpos;
    double zpos;
    xpos=rotated_coordinates.x+t_plane[0]*rotated_velocity.x;
    zpos=rotated_coordinates.z+t_plane[0]*rotated_velocity.z;
 

    if ((xpos*xpos + zpos*zpos) > radius_top*radius_top){
        t_plane[0] = -1;
        *num_solutions = *num_solutions-1;
    } else {
		normal_vector_rotated = coords_set(0,1,0);
			
	    normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		
	    // Set the normal vector components
	    nx[0] = normal_vector.x;
	    ny[0] = normal_vector.y;
	    nz[0] = normal_vector.z;
		
		surface_index[0] = 1; // top index
    }
    xpos=rotated_coordinates.x+t_plane[1]*rotated_velocity.x;
    zpos=rotated_coordinates.z+t_plane[1]*rotated_velocity.z;

    if ((xpos*xpos + zpos*zpos) > radius_bottom*radius_bottom){
        t_plane[1] = -1;
        *num_solutions = *num_solutions-1;
    } else {
		normal_vector_rotated = coords_set(0,-1,0);
			
	    normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		
	    // Set the normal vector components
	    nx[1] = normal_vector.x;
	    ny[1] = normal_vector.y;
	    nz[1] = normal_vector.z;
		
		surface_index[1] = 2; // bottom index
    }
    
    // sort solutions:
    if (t_plane[0]>t_plane[1]){
       tmp = t_plane[1];
       t_plane[1] = t_plane[0];
       t_plane[0] = tmp;
	   
	   // Also switch the normal vectors
	   tmp = nx[1];
 	   nx[1] = nx[0];
	   nx[0] = tmp;
	
	   tmp = ny[1];
	   ny[1] = ny[0];
	   ny[0] = tmp;
	
	   tmp = nz[1];
	   nz[1] = nz[0];
	   nz[0] = tmp;
	
	   // Switch surface_index
	   int temp_int = surface_index[1];
	   surface_index[1] = surface_index[0];
	   surface_index[0] = temp_int;
    }

    
	double nx_cone[2], ny_cone[2], nz_cone[2];
	int surface_index_cone[2];
	double r_current;
	double x, y, z, dt;
	
    if (*num_solutions == 2){
        // Intersect only on planes
        t[0] = t_plane[0];
        t[1] = t_plane[1];
        
    } else {
        // Intersects with cone
            // Intersection with cone:
            // solve the equation: t*A+sqrt(t*B)+C = 0
        tmp = (rotated_velocity.y*radius_top/height-rotated_velocity.y*radius_bottom/height);
        double A = rotated_velocity.x*rotated_velocity.x+rotated_velocity.z*rotated_velocity.z-tmp*tmp;
        double B = 2*rotated_velocity.x*rotated_coordinates.x+2*rotated_velocity.z*rotated_coordinates.z-(2*(rotated_velocity.y*radius_top/height-rotated_velocity.y*radius_bottom/height))*((0.5)*radius_bottom+rotated_coordinates.y*radius_top/height-rotated_coordinates.y*radius_bottom/height+radius_top/2);
        tmp = (radius_bottom/2+rotated_coordinates.y*radius_top/height-rotated_coordinates.y*radius_bottom/height+radius_top/2);
        double C = rotated_coordinates.x*rotated_coordinates.x+rotated_coordinates.z*rotated_coordinates.z-tmp*tmp;
        double t_cone[2];
               t_cone[1]= -(B+sqrt(-4*A*C+B*B))/(2*A);
            t_cone[0]= -(B-sqrt(-4*A*C+B*B))/(2*A);
        //solve_2nd_order(&t_cone[0], &t_cone[1], A, B, C);

        // remove solutions on cone over top and under bottom
        if (fabs(t_cone[0]*rotated_velocity.y+rotated_coordinates.y) > height/2) {
            t_cone[0] = -1;
        } else {
	        dt = t_cone[0];
        
	        // Intersection point in cylinder coordinate system
	        x = rotated_coordinates.x + dt*rotated_velocity.x;
	        y = rotated_coordinates.y + dt*rotated_velocity.y;
	        z = rotated_coordinates.z + dt*rotated_velocity.z;
			
			r_current = radius_top + ((y-0.5*height)/height)*(radius_top - radius_bottom);
				
			
			if (radius_bottom==radius_top) {
 			    normal_vector_rotated = coords_set(x/r_current, 0.0, z/r_current);
			} else {
			    normal_vector_rotated = coords_set(x/r_current, (radius_bottom - radius_top)/height, z/r_current);
			}

			NORM(normal_vector_rotated.x, normal_vector_rotated.y, normal_vector_rotated.z);
			
	        // Rotate back to master coordinate system
	        normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		
	        // Set the normal vector components
	        nx_cone[0] = normal_vector.x;
	        ny_cone[0] = normal_vector.y;
	        nz_cone[0] = normal_vector.z;
			
			surface_index_cone[0] = 0;
			
        }
        if (fabs(t_cone[1]*rotated_velocity.y+rotated_coordinates.y) > height/2) {
            t_cone[1] = -1;
		} else { 
			
		    dt = t_cone[1];
			
	        // Intersection point in cylinder coordinate system
	        x = rotated_coordinates.x + dt*rotated_velocity.x;
	        y = rotated_coordinates.y + dt*rotated_velocity.y;
	        z = rotated_coordinates.z + dt*rotated_velocity.z;
			
			r_current = radius_top + ((y-0.5*height)/height)*(radius_top - radius_bottom);
				
			
			if (radius_bottom==radius_top) {
 			    normal_vector_rotated = coords_set(x/r_current, 0.0, z/r_current);
			} else {
			    normal_vector_rotated = coords_set(x/r_current, (radius_bottom - radius_top)/height, z/r_current);
			}

			NORM(normal_vector_rotated.x, normal_vector_rotated.y, normal_vector_rotated.z);
			
	        // Rotate back to master coordinate system
	        normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		
	        // Set the normal vector components
	        nx_cone[1] = normal_vector.x;
	        ny_cone[1] = normal_vector.y;
	        nz_cone[1] = normal_vector.z;
			
			surface_index_cone[1] = 0;			
        }

        
        // sort solutions:
        if (t_cone[0]>t_cone[1]){
            tmp = t_cone[1];
            t_cone[1] = t_cone[0];
            t_cone[0] = tmp;
			
	 	   // Also switch the normal vectors
	 	   tmp = nx_cone[1];
	  	   nx_cone[1] = nx_cone[0];
	 	   nx_cone[0] = tmp;
	
	 	   tmp = ny_cone[1];
	 	   ny_cone[1] = ny_cone[0];
	 	   ny_cone[0] = tmp;
	
	 	   tmp = nz_cone[1];
	 	   nz_cone[1] = nz_cone[0];
	 	   nz_cone[0] = tmp;
	
	 	   // Switch surface_index
	 	   int temp_int = surface_index_cone[1];
	 	   surface_index_cone[1] = surface_index_cone[0];
	 	   surface_index_cone[0] = temp_int;
        }
        
        if (*num_solutions == 1){
            t[0] = t_cone[1];
			nx[0] = nx_cone[1];
			ny[0] = ny_cone[1];
			nz[0] = nz_cone[1];
			surface_index[0] = surface_index_cone[1];
			
            t[1] = t_plane[1];
        }
        if (*num_solutions == 0){
            t[0] = t_cone[0];
			nx[0] = nx_cone[0];
			ny[0] = ny_cone[0];
			nz[0] = nz_cone[0];
			surface_index[0] = surface_index_cone[0];
			
            t[1] = t_cone[1];
			nx[1] = nx_cone[1];
			ny[1] = ny_cone[1];
			nz[1] = nz_cone[1];
			surface_index[1] = surface_index_cone[1];
        }
    }


    /*
    // Count solutions
    *num_solutions = 0;
    if (t[0] > 0){
        *num_solutions += 1;
    }else {
        t[0]=-1;
    }
    if (t[1] > 0){
        *num_solutions += 1;
    }else {
        t[1]=-1;
    }
	*/
	
	*num_solutions = 2;
    
    
    if (t[0] > t[1]) {
        tmp = t[1];
        t[1] = t[0];
        t[0] = tmp;
		
 	   tmp = nx[1];
  	   nx[1] = nx[0];
 	   nx[0] = tmp;
	
 	   tmp = ny[1];
 	   ny[1] = ny[0];
 	   ny[0] = tmp;
	
 	   tmp = nz[1];
 	   nz[1] = nz[0];
 	   nz[0] = tmp;
	
 	   // Switch surface_index
 	   int temp_int = surface_index[1];
 	   surface_index[1] = surface_index[0];
 	   surface_index[0] = temp_int;
    }
switch(*num_solutions) {
    case 2:
	    //printf("case 2 t[0]=%lf (%lf, %lf, %lf) t[1]=%lf (%lf, %lf, %lf) \n", t[0], nx[0], ny[0], nz[0], t[1], nx[1], ny[1], nz[1]);
        return 1;
    case 1:
        t[0] = -1;
	    //printf("case 1 t[0]=%lf t[1]=%lf \n", t[0], t[1]);		
        return 1;
    case 0:
        t[0] = -1;
        t[1] = -1;
	    //printf("case 0 t[0]=%lf t[1]=%lf \n", t[0], t[1]);		
        return 0;
}

 // FIXME should we ever reach / return here?
 return -2;
};

int cone_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry){
/*
This function takes the inputs from a neutron and calculates all intersections with the cone geometry.
Output is true or false depending on intersections will occour, and a list of time-stapms of all possible intersections.

Math used here is based on the math found on:
http://lousodrome.net/blog/light/2017/01/03/intersection-of-a-ray-and-a-cone/
But has been modified to sollve the problem within the syntax needed in Union

This function was created by Martin Olsen at NBI on september 20, 2018.
*/


    double radius_top = geometry->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom = geometry->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height = geometry->geometry_parameters.p_cone_storage->height;
    Coords direction = geometry->geometry_parameters.p_cone_storage->direction_vector;
    Coords center = geometry->center;
    
    Coords bottom_point = coords_add(center,coords_scalar_mult(direction,-0.5*height));
    Coords top_point = coords_add(center,coords_scalar_mult(direction,0.5*height));

/*
    // check if this is a cylinder
    int isCylinder = 0;
    if (radius_top==radius_bottom){
        isCylinder = 1;
    }


    // Intersection with a cone
    if (isCylinder == 0){
 

    }
    // Intersection with a cylinder
    if (isCylinder == 1){


    }
*/
    // FIXME Is it meaningful that this function is of int type? Anyone requesting output?
    return 0;
};

int r_within_mesh(Coords pos,struct geometry_struct *geometry) {
  //TODO: Make a single intersection algorithm that all the three mesh intersections
  // Can use
// Unpack parameters

    Coords center = geometry->center;
    int n_facets = geometry->geometry_parameters.p_mesh_storage->n_facets;
    double *normal_x = geometry->geometry_parameters.p_mesh_storage->normal_x;
    double *normal_y = geometry->geometry_parameters.p_mesh_storage->normal_y;
    double *normal_z = geometry->geometry_parameters.p_mesh_storage->normal_z;
    double *v1_x = geometry->geometry_parameters.p_mesh_storage->v1_x;
    double *v1_y = geometry->geometry_parameters.p_mesh_storage->v1_y;
    double *v1_z = geometry->geometry_parameters.p_mesh_storage->v1_z;
    double *v2_x = geometry->geometry_parameters.p_mesh_storage->v2_x;
    double *v2_y = geometry->geometry_parameters.p_mesh_storage->v2_y;
    double *v2_z = geometry->geometry_parameters.p_mesh_storage->v2_z;
    double *v3_x = geometry->geometry_parameters.p_mesh_storage->v3_x;
    double *v3_y = geometry->geometry_parameters.p_mesh_storage->v3_y;
    double *v3_z = geometry->geometry_parameters.p_mesh_storage->v3_z;
    
    double x_new,y_new,z_new;
    
    // Coordinate transformation
    x_new = pos.x - geometry->center.x;
    y_new = pos.y - geometry->center.y;
    z_new = pos.z - geometry->center.z;
    
    Coords coordinates = coords_set(x_new,y_new,z_new);
    Coords rotated_coordinates;
   
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);

    
    int verbal = 0;
    // Generate unit direction vector along center axis of meshs
    
    // Start with vector that points along the mesh in the simple frame, and rotate to global
    Coords simple_vector = coords_set(0,1,0);
    Coords test_vector = coords_set(0,1,0);
    // Check intersections with every single facet:
    int iter =0;
    int counter=0; int neg_counter=0;
    Coords edge1,edge2,h,s,q,tmp,intersect_pos;
    double UNION_EPSILON = 1e-27;
    double this_facet_t;
    double a,f,u,V;
    //////printf("\n RWITHIN TEST 1ste");
    for (iter = 0 ; iter < n_facets ; iter++){
        // Intersection with face plane (Möller–Trumbore)
        edge1 = coords_set(*(v2_x+iter)-*(v1_x+iter),*(v2_y+iter)-*(v1_y+iter),*(v2_z+iter)-*(v1_z+iter));
        edge2 = coords_set(*(v3_x+iter)-*(v1_x+iter),*(v3_y+iter)-*(v1_y+iter),*(v3_z+iter)-*(v1_z+iter));
        
        vec_prod(h.x,h.y,h.z,test_vector.x,test_vector.y,test_vector.z,edge2.x,edge2.y,edge2.z);
        
        a = Dot(edge1,h);
        if (a > -UNION_EPSILON && a < UNION_EPSILON){
            //////printf("\n UNION_EPSILON fail");
        } else{
            f = 1.0/a;
            s = coords_sub(rotated_coordinates, coords_set(*(v1_x+iter),*(v1_y+iter),*(v1_z+iter)));
            u = f * (Dot(s,h));
            if (u < 0.0 || u > 1.0){
            }else{
                //q = vec_prod(s,edge1);
                vec_prod(q.x,q.y,q.z,s.x,s.y,s.z,edge1.x,edge1.y,edge1.z);
                V = f * Dot(test_vector,q);
                if (V < 0.0 || u + V > 1.0){
                } else {
                    // At this stage we can compute t to find out where the intersection point is on the line.
                    if (f* Dot(q,edge2) > 0){
                        counter++;

                    } else {
                        neg_counter++;

                    }
                    if (fabs(f* Dot(q,edge2)) > UNION_EPSILON){
                    } else { //printf("\n [%f %f %f] Failed due to being close to surface, E = %f",rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z,f* Dot(q,edge2));
                     }
                    
                    
                    
                }
            }
        }
    }
    int C1 = counter;
    
    int maxC; int sameNr =0;
    if (counter % 2 == neg_counter % 2){
        maxC = counter;
        sameNr = 1;
    } else {
        //printf("\n not the same intersection numbers (%i , %i)",counter,neg_counter);
        maxC = counter;
        sameNr = 0;
    }

 if (sameNr == 0){
     test_vector = coords_set(0,0,1);
    iter =0;
    counter=0;
    for (iter = 0 ; iter < n_facets ; iter++){
        // Intersection with face plane (Möller–Trumbore)
        edge1 = coords_set(*(v2_x+iter)-*(v1_x+iter),*(v2_y+iter)-*(v1_y+iter),*(v2_z+iter)-*(v1_z+iter));
        edge2 = coords_set(*(v3_x+iter)-*(v1_x+iter),*(v3_y+iter)-*(v1_y+iter),*(v3_z+iter)-*(v1_z+iter));
        
        vec_prod(h.x,h.y,h.z,test_vector.x,test_vector.y,test_vector.z,edge2.x,edge2.y,edge2.z);
        
        a = Dot(edge1,h);
        if (a > -UNION_EPSILON && a < UNION_EPSILON){
            //////printf("\n UNION_EPSILON fail");
        } else{
            f = 1.0/a;
            s = coords_sub(rotated_coordinates , coords_set(*(v1_x+iter),*(v1_y+iter),*(v1_z+iter)));
            u = f * (Dot(s,h));
            if (u < 0.0 || u > 1.0){
            }else{
                //q = vec_prod(s,edge1);
                vec_prod(q.x,q.y,q.z,s.x,s.y,s.z,edge1.x,edge1.y,edge1.z);
                V = f * Dot(test_vector,q);
                if (V < 0.0 || u + V > 1.0){
                } else {
                    // At this stage we can compute t to find out where the intersection point is on the line.

                    if (f* Dot(q,edge2) > 0){
                        counter++;

                    } else {
                        neg_counter++;

                    }
                    if (fabs(f* Dot(q,edge2)) > UNION_EPSILON){
                    } else { printf("\n [%f %f %f] Failed due to being close to surface (2. iteration), E = %f",rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z,f* Dot(q,edge2));
                     }
                    
                    
                    
                }
            }
        }
    }
    }
    
    if (counter % 2 == neg_counter % 2){
        maxC = counter;
        sameNr = 1;
    } else {
        printf("\n not the same intersection numbers (%i , %i) second iteration",counter,neg_counter);
        maxC = counter;
        sameNr = 0;
    }

    if (sameNr == 0){
     test_vector = coords_set(1,0,0);
    iter =0;
    counter=0;
    for (iter = 0 ; iter < n_facets ; iter++){
        // Intersection with face plane (Möller–Trumbore)
        edge1 = coords_set(*(v2_x+iter)-*(v1_x+iter),*(v2_y+iter)-*(v1_y+iter),*(v2_z+iter)-*(v1_z+iter));
        edge2 = coords_set(*(v3_x+iter)-*(v1_x+iter),*(v3_y+iter)-*(v1_y+iter),*(v3_z+iter)-*(v1_z+iter));
        
        vec_prod(h.x,h.y,h.z,test_vector.x,test_vector.y,test_vector.z,edge2.x,edge2.y,edge2.z);
        
        a = Dot(edge1,h);
        if (a > -UNION_EPSILON && a < UNION_EPSILON){
            //////printf("\n UNION_EPSILON fail");
        } else{
            f = 1.0/a;
            s = coords_sub(rotated_coordinates , coords_set(*(v1_x+iter),*(v1_y+iter),*(v1_z+iter)));
            u = f * (Dot(s,h));
            if (u < 0.0 || u > 1.0){
                //////printf("\n Nope 1");
            }else{
                //q = vec_prod(s,edge1);
                vec_prod(q.x,q.y,q.z,s.x,s.y,s.z,edge1.x,edge1.y,edge1.z);
                V = f * Dot(test_vector,q);
                if (V < 0.0 || u + V > 1.0){
                } else {
                    // At this stage we can compute t to find out where the intersection point is on the line.

                    if (f* Dot(q,edge2) > 0){
                        counter++;

                    } else {
                        neg_counter++;

                    }
                    if (fabs(f* Dot(q,edge2)) > UNION_EPSILON){
                    } else { printf("\n [%f %f %f] Failed due to being close to surface (3. iteration), E = %f",rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z,f* Dot(q,edge2));
                    }
                    
                    
                    
                }
            }
        }
    }

    }
   
    
    if (counter % 2 == neg_counter % 2){
        maxC = counter;
    } else {
        return 0;
    }
    if ( maxC % 2 == 0) {
        return 0;
    }else{
        return 1;
        
    }
    
    return 0;
    };



int sample_mesh_intersect(double *t,
                          double *nx, double *ny, double*nz,
                          int *surface_index,
                          int *num_solutions,double *r,double *v,
                          struct geometry_struct *geometry) {

    int n_facets = geometry->geometry_parameters.p_mesh_storage->n_facets;
    double *normal_x = geometry->geometry_parameters.p_mesh_storage->normal_x;
    double *normal_y = geometry->geometry_parameters.p_mesh_storage->normal_y;
    double *normal_z = geometry->geometry_parameters.p_mesh_storage->normal_z;
    double *v1_x = geometry->geometry_parameters.p_mesh_storage->v1_x;
    double *v1_y = geometry->geometry_parameters.p_mesh_storage->v1_y;
    double *v1_z = geometry->geometry_parameters.p_mesh_storage->v1_z;
    double *v2_x = geometry->geometry_parameters.p_mesh_storage->v2_x;
    double *v2_y = geometry->geometry_parameters.p_mesh_storage->v2_y;
    double *v2_z= geometry->geometry_parameters.p_mesh_storage->v2_z;
    double *v3_x = geometry->geometry_parameters.p_mesh_storage->v3_x;
    double *v3_y = geometry->geometry_parameters.p_mesh_storage->v3_y;
    double *v3_z = geometry->geometry_parameters.p_mesh_storage->v3_z;
    Coords Bounding_Box_Center = geometry->geometry_parameters.p_mesh_storage->Bounding_Box_Center;
    double Bounding_Box_Radius = geometry->geometry_parameters.p_mesh_storage->Bounding_Box_Radius;
    int i;

    double x_new,y_new,z_new;
    // Coordinate transformation
    x_new = r[0] - geometry->center.x;
    y_new = r[1] - geometry->center.y;
    z_new = r[2] - geometry->center.z;
    
    double x_bb,y_bb,z_bb;
    x_bb = r[0] - Bounding_Box_Center.x - geometry->center.x;
    y_bb = r[1] - Bounding_Box_Center.y - geometry->center.y;
    z_bb = r[2] - Bounding_Box_Center.z - geometry->center.z;
    
    Coords coordinates = coords_set(x_new,y_new,z_new);
    Coords rotated_coordinates;

    // Rotate the position of the neutron around the center of the mesh
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
    
    Coords bounding_box_coordinates = coords_set(x_bb, y_bb, z_bb);
    Coords bounding_box_rotated_coordinates;
    
    bounding_box_rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,bounding_box_coordinates);
    
    Coords velocity = coords_set(v[0],v[1],v[2]);
    Coords rotated_velocity;
    
    // Rotate the position of the neutron around the center of the mesh
    rotated_velocity = rot_apply(geometry->transpose_rotation_matrix,velocity);
    
    int output = 0;
    double tmpres[2];
    // Test intersection with bounding sphere 
    if ((output = sphere_intersect(&tmpres[0],&tmpres[1],
                                   bounding_box_rotated_coordinates.x,
                                   bounding_box_rotated_coordinates.y,
                                   bounding_box_rotated_coordinates.z,
                                   rotated_velocity.x,
                                   rotated_velocity.y,
                                   rotated_velocity.z,
                                   Bounding_Box_Radius)) == 0) {
        t[0] = -1;
        t[1] = -1;
        *num_solutions = 0;
        return 0;
    }
    // Check intersections with every single facet:
    int iter =0;
    int counter=0;
    Coords edge1,edge2,h,s,q,tmp,intersect_pos;
    double UNION_EPSILON = 0.0000001;
    double this_facet_t;
    double a,f,u,V;
    double *t_intersect=malloc(n_facets*sizeof(double));
    int *facet_index = malloc(n_facets*sizeof(int));
    if (!t_intersect || !facet_index) {
      fprintf(stderr,"Failure allocating list in Union function sample_mesh_intersect - Exit!\n");
      exit(EXIT_FAILURE);
    }
    *num_solutions = 0;
    for (iter = 0 ; iter < n_facets ; iter++){
        // Intersection with face plane (Möller–Trumbore)
        edge1 = coords_set(*(v2_x+iter)-*(v1_x+iter),*(v2_y+iter)-*(v1_y+iter),*(v2_z+iter)-*(v1_z+iter));
        edge2 = coords_set(*(v3_x+iter)-*(v1_x+iter),*(v3_y+iter)-*(v1_y+iter),*(v3_z+iter)-*(v1_z+iter));
        vec_prod(h.x,h.y,h.z,rotated_velocity.x,rotated_velocity.y,rotated_velocity.z,edge2.x,edge2.y,edge2.z);
        a = Dot(edge1,h);
        //if (a > -UNION_EPSILON && a < UNION_EPSILON){
            ////printf("\n UNION_EPSILON fail");
        //}
            f = 1.0/a;
            s = coords_sub(rotated_coordinates, coords_set(*(v1_x+iter),*(v1_y+iter),*(v1_z+iter)));
            u = f * (Dot(s,h));
            if (u < 0.0 || u > 1.0){
            }else{
                //q = vec_prod(s,edge1);
                vec_prod(q.x,q.y,q.z,s.x,s.y,s.z,edge1.x,edge1.y,edge1.z);
                V = f * Dot(rotated_velocity,q);
                if (V < 0.0 || u + V > 1.0){
                } else {
                    // At this stage we can compute t to find out where the intersection point is on the line.
                    //tmp = Dot(q,edge2)
                    if (f* Dot(q,edge2) > 0){
                        
                        t_intersect[counter] = f* Dot(q,edge2);
                        facet_index[counter] = iter;
                        //printf("\nIntersects at time: t= %f\n",t_intersect[counter] );
                        counter++;
                    }
                }
            }
    }
    
    // Return all t
    int counter2=0;
    double x_intersect, y_intersect, z_intersect;
    *num_solutions =0;
    for (iter=0; iter < counter ; iter++){
        if (t_intersect[iter] > 0.0){
          t[counter2] = t_intersect[iter];
          x_intersect = t[counter2]*v[0] + x_new;
		      y_intersect = t[counter2]*v[1] + y_new;
		      z_intersect = t[counter2]*v[2] + z_new;
          coordinates = coords_set(x_intersect,y_intersect,z_intersect);
		      NORM(coordinates.x, coordinates.y, coordinates.z);
		      nx[counter2] = normal_x[facet_index[iter]];
		      ny[counter2] = normal_x[facet_index[iter]];
		      nz[counter2] = normal_x[facet_index[iter]];				
		      surface_index[counter2] = 0;
		      counter2++;
          *num_solutions = counter2;
        }
    }
    // Sort t:
    if (*num_solutions == 0){
        free(t_intersect);
        free(facet_index);
        return 0;
    }
    qsort(t,*num_solutions,sizeof (double), Sample_compare_doubles);
    free(facet_index);
    free(t_intersect);
    return 1;
};


int r_within_box_advanced(Coords pos,struct geometry_struct *geometry) {
    // Unpack parameters
    double width1 = geometry->geometry_parameters.p_box_storage->x_width1;
    double height1 = geometry->geometry_parameters.p_box_storage->y_height1;
    double width2 = geometry->geometry_parameters.p_box_storage->x_width2;
    double height2 = geometry->geometry_parameters.p_box_storage->y_height2;
    double depth = geometry->geometry_parameters.p_box_storage->z_depth;
    
    // Transform to the center
    Coords coordinates = coords_sub(pos,geometry->center);
    
    Coords rotated_coordinates;
    
    // Rotate the position of the neutron around the center of the cylinder
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
    
    if (rotated_coordinates.z < -0.5*depth || rotated_coordinates.z > 0.5*depth) return 0;
    
    double depth_ratio = ((rotated_coordinates.z+0.5*depth)/depth);
    double width_at_depth = width1 + (width2-width1)*depth_ratio;
    if (rotated_coordinates.x < -0.5*width_at_depth || rotated_coordinates.x > 0.5*width_at_depth) return 0;
    
    double height_at_depth = height1 + (height2-height1)*depth_ratio;
    if (rotated_coordinates.y < -0.5*height_at_depth || rotated_coordinates.y > 0.5*height_at_depth) return 0;
    
    return 1;
};

// -------------    Functions for box ray tracing used in initialize ----------------------------
// These functions does not need to be fast, as they are only used once
int box_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Is geometry child inside geometry parent?
    // For box child to be inside of box parent, all corners of box child must be inside of box parent.
    
    // Generate coordinates of corners of box2
    Coords corner_points[8];
    box_corners_global_frame(corner_points,geometry_child);
    
    // Check earch corner seperatly
    int iterate;
    for (iterate=0;iterate<8;iterate++) {
        if (geometry_parent->within_function(corner_points[iterate],geometry_parent) == 0) {
            return 0; // If a corner is outside, box 2 is not within box 1
        }
    }
    return 1; // If no corner was outside, box 2 is inside box 1
};

int existence_of_intersection(Coords point1, Coords point2, struct geometry_struct *geometry) {
    Coords vector_between = coords_sub(point2,point1);
    double start_point[3],vector_between_v[3];
    double temp_solution[2];
    int number_of_solutions;

    start_point[0] = point1.x;start_point[1] = point1.y;start_point[2] = point1.z;
    vector_between_v[0] = vector_between.x;vector_between_v[1] = vector_between.y;vector_between_v[2] = vector_between.z;
	
	// todo: Switch to nicer intersect call
	double dummy_double[2];
	int dummy_int[2];
   printf("\nChecking the existence of intersections");	
    geometry->intersect_function(temp_solution, dummy_double, dummy_double, dummy_double, dummy_int, &number_of_solutions, start_point, vector_between_v, geometry);
    if (number_of_solutions > 0) {
      printf("\nMore than 1 solution has been found");
        if (temp_solution[0] > 0 && temp_solution[0] < 1) return 1;
        if (number_of_solutions == 2) {
            if (temp_solution[1] > 0 && temp_solution[1] < 1) return 1;
        }
    }
    return 0;
};

int box_overlaps_box(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
    // Algorithm for checking if two boxes overlap
    
    // First check if one is inside the other by checking corners and within the other
    
    // Generate coordinates of corners of box1
    Coords corner_points1[8];
    box_corners_global_frame(corner_points1,geometry1);
    
    // Check earch corner seperatly
    int iterate;
    for (iterate=0;iterate<8;iterate++) {
        if (geometry2->within_function(corner_points1[iterate],geometry2) == 1) {
            return 1; // If a corner of box 1 is inside box 2, the two boxes overlaps
        }
    }
    
    Coords corner_points2[8];
    box_corners_global_frame(corner_points2,geometry2);
    for (iterate=0;iterate<8;iterate++) {
        if (geometry1->within_function(corner_points2[iterate],geometry1) == 1) {
            return 1; // If a corner of box 2 is inside box 1, the two boxes overlaps
        }
    }
    
    // Check intersections for the lines between the corners of box1 and the box2 geometry
    // 12 sides to a box, if any one of them intersects, the volumes overlaps
    for (iterate=0;iterate<3;iterate++) { //
        if (existence_of_intersection(corner_points1[iterate],corner_points1[iterate+1],geometry2) == 1) return 1;
    }
    if (existence_of_intersection(corner_points1[3],corner_points1[0],geometry2) == 1) return 1;
    for (iterate=4;iterate<7;iterate++) {
        if (existence_of_intersection(corner_points1[iterate],corner_points1[iterate+1],geometry2) == 1) return 1;
    }
    if (existence_of_intersection(corner_points1[7],corner_points1[4],geometry2) == 1) return 1;
    for (iterate=0;iterate<4;iterate++) {
        if (existence_of_intersection(corner_points1[iterate],corner_points1[iterate+4],geometry2) == 1) return 1;
    }
    
    // Check intersections for the lines between the corners of box2 and the box1 geometry
    // 12 sides to a box, if any one of them intersects, the volumes overlaps
    for (iterate=0;iterate<3;iterate++) { //
        if (existence_of_intersection(corner_points2[iterate],corner_points2[iterate+1],geometry1) == 1) return 1;
    }
    if (existence_of_intersection(corner_points2[3],corner_points2[0],geometry1) == 1) return 1;
    for (iterate=4;iterate<7;iterate++) {
        if (existence_of_intersection(corner_points2[iterate],corner_points2[iterate+1],geometry1) == 1) return 1;
    }
    if (existence_of_intersection(corner_points2[7],corner_points2[4],geometry1) == 1) return 1;
    for (iterate=0;iterate<4;iterate++) {
        if (existence_of_intersection(corner_points2[iterate],corner_points2[iterate+4],geometry1) == 1) return 1;
    }
    
    // If none of the boxes corners are inside the other box, and none of the sides of the boxes intersect the other box, they do not overlap.
    return 0;
};


// -------------    Functions for sphere ray tracing used in trace ------------------------------
// These functions needs to be fast, as they may be used many times for each ray
int sample_sphere_intersect(double *t, double *nx, double *ny, double *nz, int *surface_index, int *num_solutions,double *r,double *v,struct geometry_struct *geometry) {
    double radius = geometry->geometry_parameters.p_sphere_storage->sph_radius;
    
    // Declare variables for the function
    double x_new,y_new,z_new;
    
    // Coordinate transformation
    x_new = r[0] - geometry->center.x;
    y_new = r[1] - geometry->center.y;
    z_new = r[2] - geometry->center.z;
    
    int output = 0;
    // Run McStas built in sphere intersect funtion (sphere centered around origin)
    if ((output = sphere_intersect(&t[0],&t[1],x_new,y_new,z_new,v[0],v[1],v[2],radius)) == 0) {
        *num_solutions = 0;t[0]=-1;t[1]=-1;}
    else if (t[1] != 0) *num_solutions = 2;
    else {*num_solutions = 1;t[1]=-1;}
	
	// Calculate normals
	int iterator;
	double x_intersect, y_intersect, z_intersect; // relative to sphere center
	Coords coordinates;
	Coords rotated_coordinates;	
	
	for (iterator=0;iterator<*num_solutions;iterator++) {
		x_intersect = t[iterator]*v[0] + x_new;
		y_intersect = t[iterator]*v[1] + y_new;
		z_intersect = t[iterator]*v[2] + z_new;
		
	    coordinates = coords_set(x_intersect,y_intersect,z_intersect);
		NORM(coordinates.x, coordinates.y, coordinates.z);
		nx[iterator] = coordinates.x;
		ny[iterator] = coordinates.y;
		nz[iterator] = coordinates.z;				
		surface_index[iterator] = 0;
		
		/*
		// Since the ray was never rotated into the sphere coordinate system (due to symmetry)
		//  rotating back is not necessary
			
	    // printf("Cords coordinates = (%f,%f,%f)\n",coordinates.x,coordinates.y,coordinates.z);
    
	    // debug
	    // Rotation rotation_matrix_debug[3][3];
	    // rot_set_rotation(rotation_matrix_debug,-1.0*geometry->rotation.x,-1.0*geometry->rotation.y,-1.0*geometry->rotation.z);
	    // rot_transpose(geometry->rotation_matrix,rotation_matrix_debug);

	    // Rotate the position of the neutron around the center of the cylinder
	    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
		*/
			
	}
    
    return output;
};

int r_within_sphere(Coords pos,struct geometry_struct *geometry)
    {
    // Unpack parameters
    double radius = geometry->geometry_parameters.p_sphere_storage->sph_radius;
    
    // Calculate the distance between the center and the sphere, and the current position
    double distance = distance_between(pos,geometry->center);

    //printf("distance = %f\n",distance);
    //printf("radius   = %f\n",radius);
    //printf("current_position.x = %f,current_position.y = %f,current_position.z = %f\n",current_position.x,current_position.y,current_position.z);
    //printf("geometry.x = %f,geometry.y = %f,geometry.z = %f\n",geometry->center.x,geometry->center.y,geometry->center.z);
    //printf("return   = %d\n",(distance <= radius));
    
    return (distance < radius);
    };

// -------------    Functions for sphere ray tracing used in initialize -------------------------
// These functions does not need to be fast, as they are only used once
int sphere_overlaps_sphere(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
    // Unpack parameters
    double radius1 = geometry1->geometry_parameters.p_sphere_storage->sph_radius;
    double radius2 = geometry2->geometry_parameters.p_sphere_storage->sph_radius;

    // Calculate distance
    double distance = distance_between(geometry1->center,geometry2->center);
    
    // Return 0 if the spheres does not overlap, 1 if they do.
    // printf("Output from sphere_overlaps_sphere = %d \n",(distance <= (radius1 + radius2)));
    return (distance <= (radius1 + radius2));
};

int sphere_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Unpack parameters
    double radius_child = geometry_child->geometry_parameters.p_sphere_storage->sph_radius;
    double radius_parent = geometry_parent->geometry_parameters.p_sphere_storage->sph_radius;

    // Calculate distance
    double distance = distance_between(geometry_child->center,geometry_parent->center);
    
    // Return 1 if sphere child is within sphere parent, 0 if they do not.
    return (distance + radius_child <= radius_parent);
};

// -------------    Functions for cylinder ray tracing used in trace ------------------------------
// These functions needs to be fast, as they may be used many times for each ray
int sample_cylinder_intersect(double *t, double *nx, double *ny, double *nz, int *surface_index, int *num_solutions,double *r,double *v,struct geometry_struct *geometry) {
    double radius = geometry->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height = geometry->geometry_parameters.p_cylinder_storage->height;
    
    // Declare position variables for the local coordinate system
    double x_new,y_new,z_new;
    
    // Coordinate transformation
    x_new = r[0] - geometry->center.x;
    y_new = r[1] - geometry->center.y;
    z_new = r[2] - geometry->center.z;
    
    Coords coordinates = coords_set(x_new,y_new,z_new);
    Coords rotated_coordinates;

    // Rotate the position of the neutron around the center of the cylinder
    rotated_coordinates = rot_apply(geometry->transpose_rotation_matrix,coordinates);
    
    Coords velocity = coords_set(v[0],v[1],v[2]);
    Coords rotated_velocity;
    
    // Rotate the position of the neutron around the center of the cylinder
    rotated_velocity = rot_apply(geometry->transpose_rotation_matrix,velocity);
    
	int output = 0;    
	// Cases where the velocity is parallel with the cylinder axis have given problems, and is checked for explicitly
	if (sqrt(rotated_velocity.x*rotated_velocity.x+rotated_velocity.z*rotated_velocity.z)/fabs(rotated_velocity.y) < 0.00001) {
	  // The velocity is parallel with the cylinder axis. Either there are no solutions or two solutions
	  if (sqrt(rotated_coordinates.x*rotated_coordinates.x+rotated_coordinates.z*rotated_coordinates.z) > radius) {
	    *num_solutions = 0;
	    return 0;
	  } else {
	    *num_solutions = 2;
	    t[0] = (0.5*height - rotated_coordinates.y)/rotated_velocity.y;
		surface_index[0] = 1; // index indicating top
		
	    t[1] = (-0.5*height - rotated_coordinates.y)/rotated_velocity.y;
		surface_index[1] = 2; // index indicating bottom
		
		// sort solutions
		if (t[0] > t[1]) {
		  double d_temp;
		  
		  d_temp = t[0];
		  t[0] = t[1];
		  t[1] = d_temp;
		  
		  int i_temp;
		  
		  i_temp = surface_index[0];
		  surface_index[0] = surface_index[1];
		  surface_index[1] = i_temp;
		}
	  }
	} else {
	  // velocity not parallel to cylinder axis, call standard mcstas cylinder intersect 

	  // Run McStas built in sphere intersect funtion (sphere centered around origin)
	  if ((output = cylinder_intersect(&t[0],&t[1],
	                                   rotated_coordinates.x,rotated_coordinates.y,rotated_coordinates.z,
	                                   rotated_velocity.x,rotated_velocity.y,rotated_velocity.z,radius,height)) == 0) {
	      *num_solutions = 0;t[0]=-1;t[1]=-1;
	  }
	  else if (t[1] != 0) *num_solutions = 2;
	  else {*num_solutions = 1; t[1]=-1;}
	
	  // decode output value
	  // Check the bitmask for entry and exit
	  if (*num_solutions > 0) {
	    int entry_index = 0;
	    if (output & 2) entry_index = 1; // Entry intersects top cap
	    if (output & 4) entry_index = 2; // Entry intersects bottom cap
	    surface_index[0] = entry_index;
	  }
	
	  if (*num_solutions > 1) {
	    int exit_index = 0;		
	    if (output & 8) exit_index = 1;  // Exit intersects top cap
	    if (output & 16) exit_index = 2; // Exit intersects bottom cap
	    surface_index[1] = exit_index;
	  }
	}
    
	
	// Calculate normal vectors from surface index and cylinder geometry
    int index;
    double x, y, z, dt;
    Coords normal_vector_rotated;
    Coords normal_vector;
	
    for (index=0; index<*num_solutions; index++) {
		
		// top and bottom easy
		if (surface_index[index] == 1) {
			normal_vector_rotated = coords_set(0,1,0);
			
		} else if (surface_index[index] == 2) {
			normal_vector_rotated = coords_set(0,-1,0);			
		} else {
	        dt = t[index];
        
	        // Intersection point in cylinder coordinate system
	        x = rotated_coordinates.x + dt*rotated_velocity.x;
	        y = rotated_coordinates.y + dt*rotated_velocity.y;
	        z = rotated_coordinates.z + dt*rotated_velocity.z;
			
			normal_vector_rotated = coords_set(x,0,z);			
			NORM(normal_vector_rotated.x, normal_vector_rotated.y, normal_vector_rotated.z);
		}
		
        // Rotate back to master coordinate system
        normal_vector = rot_apply(geometry->rotation_matrix, normal_vector_rotated);
		
        // Set the normal vector components
        nx[index] = normal_vector.x;
        ny[index] = normal_vector.y;
        nz[index] = normal_vector.z;
    }
    
    return output;
};

int r_within_cylinder(Coords pos,struct geometry_struct *geometry) {
	// Unpack parameters
    double radius = geometry->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height = geometry->geometry_parameters.p_cylinder_storage->height;

    
    int verbal = 0;
    // Generate unit direction vector along center axis of cylinders
    
    // Start with vector that points along the cylinder in the simple frame, and rotate to global
    Coords simple_vector = coords_set(0,1,0);
    Coords vector1;
    if (verbal == 1) printf("Cords start_vector = (%f,%f,%f)\n",simple_vector.x,simple_vector.y,simple_vector.z);

    // Rotate the position of the neutron around the center of the cylinder
    vector1 = rot_apply(geometry->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector1.z);
    
    // The cylinders are parallel.
    int seperated = 0;
    //double delta[3];
    //delta[0] = geometry->center.x - r[0];
    //delta[1] = geometry->center.y - r[1];
    //delta[2] = geometry->center.z - r[2];
    
    Coords delta = coords_sub(geometry->center,pos);

    // Test for separation by height
    // if (h0Div2 + h1Div2 − |Dot(W0, Delta )| < 0) seperated = 1;
    
    if (verbal == 1) printf("vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector1.z);
    if (verbal == 1) printf("delta1 = (%f,%f,%f)\n",delta.x,delta.y,delta.z);
    double scalar_prod1 = scalar_prod(vector1.x,vector1.y,vector1.z,delta.x,delta.y,delta.z);
    if (verbal == 1) printf("scalar product = %f \n",scalar_prod1);
    if (verbal == 1) printf("height 1 = %f \n",height);
    
    int inside = 1;
    
    if (height*0.5 < fabs(scalar_prod1)) {
            if (verbal == 1) printf("point sticks out height wise \n");
            inside = 0;
    }

    // Test for separation radially
    // if (rSum − |Delta − Dot(W0,Delta)∗W0| < 0) seperated = 1;
    double vector_between_cyl_axis[3];
    vector_between_cyl_axis[0] = delta.x - scalar_prod1*vector1.x;
    vector_between_cyl_axis[1] = delta.y - scalar_prod1*vector1.y;
    vector_between_cyl_axis[2] = delta.z - scalar_prod1*vector1.z;
    if (verbal == 1) printf("vector_between = (%f,%f,%f)\n",vector_between_cyl_axis[0],vector_between_cyl_axis[1],vector_between_cyl_axis[2]);
    if (verbal == 1) printf("length of vector between = %f\n",length_of_3vector(vector_between_cyl_axis));
    
    if (radius < length_of_3vector(vector_between_cyl_axis)) {
            if (verbal == 1) printf("Point sticks out radially \n");
            inside = 0;
    }
    
    if (inside == 0) return 0;
    else return 1;
    };

// -------------    Functions for cylinder ray tracing used in initialize -------------------------
// These functions does not need to be fast, as they are only used once
int cylinder_overlaps_cylinder(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
    // Unpack parameters
    double radius1 = geometry1->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height1 = geometry1->geometry_parameters.p_cylinder_storage->height;
    double radius2 = geometry2->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height2 = geometry2->geometry_parameters.p_cylinder_storage->height;
    
    int verbal = 0;
    // Generate unit direction vector along center axis of cylinders
    
    // Start with vector that points along the cylinder in the simple frame, and rotate to global
    Coords simple_vector = coords_set(0,1,0);
    Coords vector1,vector2;
    if (verbal == 1) printf("Cords start_vector = (%f,%f,%f)\n",simple_vector.x,simple_vector.y,simple_vector.z);

    // Rotate the position of the neutron around the center of the cylinder
    vector1 = rot_apply(geometry1->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector1.z);
    // Rotate the position of the neutron around the center of the cylinder
    vector2 = rot_apply(geometry2->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector2 = (%f,%f,%f)\n",vector2.x,vector2.y,vector2.z);
    
    // if vector1 and vector2 are parallel, the problem is simple, but if not complicated
    double cross_product1[3] = {0,0,0};
    // printf("%f\n",cross_product1[0]);
    // vec prod(&ax,&ay,&az,bx,by,bz, cx,cy,cz)
    vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],vector1.x,vector1.y,vector1.z,vector2.x,vector2.y,vector2.z);
    // I get an error taking the adress of cross_product1[0], &cross_product1[0]. Took the pointer adresses instead. Works fine.
    if (verbal == 1) printf("cross_product = (%f,%f,%f)\n",cross_product1[0],cross_product1[1],cross_product1[2]);
    double cross_product_length = length_of_3vector(cross_product1);
    
    if (cross_product_length == 0) {
        // The cylinders are parallel.
        int seperated = 0;
        double delta[3];
        delta[0] = geometry1->center.x - geometry2->center.x;
        delta[1] = geometry1->center.y - geometry2->center.y;
        delta[2] = geometry1->center.z - geometry2->center.z;

        // Test for separation by height
        // if (h0Div2 + h1Div2 − |Dot(W0, Delta )| < 0) seperated = 1;
        
        if (verbal == 1) printf("vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector2.z);
        if (verbal == 1) printf("delta1 = (%f,%f,%f)\n",delta[0],delta[1],delta[2]);
        double scalar_prod1 = scalar_prod(vector1.x,vector1.y,vector1.z,delta[0],delta[1],delta[2]);
        if (verbal == 1) printf("scalar product = %f \n",scalar_prod1);
        if (verbal == 1) printf("height 1 = %f, height 2 = %f \n",height1,height2);
        if (height1*0.5 + height2*0.5 - fabs(scalar_prod1) < 0) {
                if (verbal == 1) printf("seperated by height \n");
                return 0;
                }
    
        // Test for separation radially
        // if (rSum − |Delta − Dot(W0,Delta)∗W0| < 0) seperated = 1;
        double vector_between_cyl_axis[3];
        vector_between_cyl_axis[0] = delta[0] - scalar_prod1*vector1.x;
        vector_between_cyl_axis[1] = delta[1] - scalar_prod1*vector1.y;
        vector_between_cyl_axis[2] = delta[2] - scalar_prod1*vector1.z;
        if (verbal == 1) printf("vector_between = (%f,%f,%f)\n",vector_between_cyl_axis[0],vector_between_cyl_axis[1],vector_between_cyl_axis[2]);
        if (verbal == 1) printf("length of vector between = %f\n",length_of_3vector(vector_between_cyl_axis));
        
        if (radius1+radius2 - length_of_3vector(vector_between_cyl_axis) < 0) {
                if (verbal == 1) printf("seperated radially \n");
                return 0;
                }
    
        if (verbal == 1) printf("cylinders not seperated\n");
        return 1;
        
    } else {
    
        // Todo: Speed up analysis by starting with a bounding sphere approach to avoid brute force in many cases
        
        
        // printf("The component uses a raytracing method for non parallel cylinders.\n");
        // printf("  Make sure not to give this algorithm edge cases, where cylinders just touch.\n");
        Coords cyl_direction1 = geometry1->geometry_parameters.p_cylinder_storage->direction_vector;
        
        // Doing a simple but not perfect overlap test

        // Checking cylinder sides.
        
        // Taking cylinder 1, making a vector at the base center.
        Coords base_point;
        
        base_point.x = geometry1->center.x - 0.5*height1*cyl_direction1.x;
        base_point.y = geometry1->center.y - 0.5*height1*cyl_direction1.y;
        base_point.z = geometry1->center.z - 0.5*height1*cyl_direction1.z;
        
        // Making a point at the circumference of the bottom circle of the cylinder
        double cross_input[3] = {0,1,0};
        // In case the cross input is parallel with the vector, a new is chosen. Both can't be parallel.
        if (scalar_prod(cross_input[0],cross_input[1],cross_input[2],cyl_direction1.x,cyl_direction1.y,cyl_direction1.z) > 0.99) {
            cross_input[0] = 1; cross_input[1] = 0; cross_input[2] = 0;
        }
        // print_position(make_position(cross_input),"cross input");
        
        double cross_product1[3] = {0,0,0};
        vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],cyl_direction1.x,cyl_direction1.y,cyl_direction1.z,cross_input[0],cross_input[1],cross_input[2]);
        
        // print_position(make_position(cross_product1),"cross_product1");
        double cross_length = length_of_3vector(cross_product1);
        
        // printf("cross_length = %f \n",cross_length);
        cross_product1[0] /= cross_length;
        cross_product1[1] /= cross_length;
        cross_product1[2] /= cross_length;
        
        cross_product1[0] *= radius1;
        cross_product1[1] *= radius1;
        cross_product1[2] *= radius1;
        
        Coords circ_point;
        double radial_position[3],cyl_direction_pointer[3],base_point_vector[3],cyl_radial_direction[3];
        
        cyl_direction_pointer[0] = cyl_direction1.x;
        cyl_direction_pointer[1] = cyl_direction1.y;
        cyl_direction_pointer[2] = cyl_direction1.z;
        
        int iterate,number_of_solutions,solutions,number_of_positions = 300;
        double rotate_angle,temp_solution[2];
        
        // printf("length of cyl_direction_pointer = %f \n",length_of_3vector(cyl_direction_pointer));
        
        // Check intersection with cylinder 2 with that point, and cyl_direction, if there is an intersection before height1, they overlap.
        // Rotate the circumference point around the cyl_direction for a full circle to detect intersections all the way around.
        
        // Here cross_product1 is a vector from the base point to a point n the circumference
        // circ_point is a vector from the base point to the circumference rotated an angle.
        // radial_position is the actual position on the circumference on the cylinder as a vector from origo.
        for (iterate = 0;iterate < number_of_positions;iterate++) {
                rotate_angle = 2*3.14159*((double) iterate)/((double) number_of_positions);
                rotate(circ_point.x,circ_point.y,circ_point.z,cross_product1[0],cross_product1[1],cross_product1[2],rotate_angle,cyl_direction1.x,cyl_direction1.y,cyl_direction1.z);
            
                radial_position[0] = base_point.x + circ_point.x;
                radial_position[1] = base_point.y + circ_point.y;
                radial_position[2] = base_point.z + circ_point.z;
                // sample_cylinder_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry) {
				double nx_dummy[2], ny_dummy[2], nz_dummy[2];
				int surface_index_dummy[2];
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy,
				                          &number_of_solutions,radial_position,cyl_direction_pointer,geometry2);
                for (solutions = 0;solutions < number_of_solutions;solutions++) {
                    if (temp_solution[solutions] > 0 && temp_solution[solutions] < height1) {
                        // cylinders must overlap.
                        return 1;
                    }
                }
                if (number_of_solutions == 2) {
                    if (temp_solution[0] < 0 && temp_solution[1] > 0) return 1; // cylinder 1 inside cylinder 2
                    if (temp_solution[0] > 0 && temp_solution[1] < 0) return 1; // cylinder 1 inside cylinder 2
                }
            
                cyl_radial_direction[0] = circ_point.x;
                cyl_radial_direction[1] = circ_point.y;
                cyl_radial_direction[2] = circ_point.z;
                // Note it has length radius1
            
                base_point_vector[0] = base_point.x;
                base_point_vector[1] = base_point.y;
                base_point_vector[2] = base_point.z;
            
                // The vector circ_point is from the base to the circumference. This is used to check the bottom cap.
                sample_cylinder_intersect(temp_solution,nx_dummy, ny_dummy, nz_dummy, surface_index_dummy, 
				                          &number_of_solutions,base_point_vector,cyl_radial_direction,geometry2);
                for (solutions = 0;solutions < number_of_solutions;solutions++) {
                    if (temp_solution[solutions] > 0 && temp_solution[solutions] < 1) {
                        // cylinders must overlap.
                        return 1;
                    }
                }
                if (number_of_solutions == 2) {
                    if (temp_solution[0] < 0 && temp_solution[1] > 0) return 1; // cylinder 1 inside cylinder 2
                    if (temp_solution[0] > 0 && temp_solution[1] < 0) return 1; // cylinder 1 inside cylinder 2
                }
            
                // Now check the top
                base_point_vector[0] = base_point.x + height1*cyl_direction1.x;
                base_point_vector[1] = base_point.y + height1*cyl_direction1.y;
                base_point_vector[2] = base_point.z + height1*cyl_direction1.z;
            
                // The vector circ_point is from the base to the circumference. This is used to check the bottom cap.
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy,
				                          &number_of_solutions,base_point_vector,cyl_radial_direction,geometry2);
                for (solutions = 0;solutions < number_of_solutions;solutions++) {
                    if (temp_solution[solutions] > 0 && temp_solution[solutions] < 1) {
                        // cylinders must overlap.
                        return 1;
                    }
                }
                if (number_of_solutions == 2) {
                    if (temp_solution[0] < 0 && temp_solution[1] > 0) return 1; // cylinder 1 inside cylinder 2
                    if (temp_solution[0] > 0 && temp_solution[1] < 0) return 1; // cylinder 1 inside cylinder 2
                }
        }
        // The above method is not perfect as it basicly tests a mesh grid of the cylinder aginst another perfect cylinder.
        // Can be improved.
        
        // If the entire perfect cylinder (cylinder 2) is within the meshgrid one, there will not be a solution to anything.
        // Check with a simple call to r_within_cylinder
        
        // r_within_cylinder(double *r,struct geometry_struct *geometry) {
        
        // if the center of cylinder 2 is within cylinder 1;
        
        if (r_within_cylinder(geometry2->center,geometry1)) return 1;  // if cylinder 2 is within cylinder 1, they clearly overlap.
        
        return 0;
    }

};

int cylinder_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Unpack parameters
    double radius1 = geometry_parent->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height1 = geometry_parent->geometry_parameters.p_cylinder_storage->height;
    double radius2 = geometry_child->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height2 = geometry_child->geometry_parameters.p_cylinder_storage->height;
    
    int verbal = 0;
    // Generate unit direction vector along center axis of cylinders
    
    // Start with vector that points along the cylinder in the simple frame, and rotate to global
    Coords simple_vector = coords_set(0,1,0);
    Coords vector1,vector2;
    if (verbal == 1) printf("Cords start_vector = (%f,%f,%f)\n",simple_vector.x,simple_vector.y,simple_vector.z);

    // Rotate the position of the ray around the center of the cylinder
    vector1 = rot_apply(geometry_parent->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector1.z);
    // Rotate the position of the ray around the center of the cylinder
    vector2 = rot_apply(geometry_child->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector2 = (%f,%f,%f)\n",vector2.x,vector2.y,vector2.z);
    
    // if vector1 and vector2 are parallel, the problem is simple, but if not complicated
    double cross_product1[3] = {0,0,0};
    // printf("%f\n",cross_product1[0]);
    // vec prod(&ax,&ay,&az,bx,by,bz, cx,cy,cz)
    vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],vector1.x,vector1.y,vector1.z,vector2.x,vector2.y,vector2.z);
    // I get an error taking the adress of cross_product1[0], &cross_product1[0]. Took the pointer adresses instead. Works fine.
    if (verbal == 1) printf("cross_product = (%f,%f,%f)\n",cross_product1[0],cross_product1[1],cross_product1[2]);
    double cross_product_length = length_of_3vector(cross_product1);
    
    if (cross_product_length == 0) {
        // The cylinders are parallel.
        int seperated = 0;
        double delta[3];
        delta[0] = geometry_parent->center.x - geometry_child->center.x;
        delta[1] = geometry_parent->center.y - geometry_child->center.y;
        delta[2] = geometry_parent->center.z - geometry_child->center.z;

        // Test for separation by height
        // if (h0Div2 + h1Div2 − |Dot(W0, Delta )| < 0) seperated = 1;
        
        if (verbal == 1) printf("vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector2.z);
        if (verbal == 1) printf("delta1 = (%f,%f,%f)\n",delta[0],delta[1],delta[2]);
        double scalar_prod1 = scalar_prod(vector1.x,vector1.y,vector1.z,delta[0],delta[1],delta[2]);
        if (verbal == 1) printf("scalar product = %f \n",scalar_prod1);
        if (verbal == 1) printf("height 1 = %f, height 2 = %f \n",height1,height2);
        
        int inside = 1;
        
        if (height1*0.5 < height2*0.5 + fabs(scalar_prod1)) {
                if (verbal == 1) printf("Cylinder sticks out height wise \n");
                inside = 0;
                }
    
        // Test for separation radially
        // if (rSum − |Delta − Dot(W0,Delta)∗W0| < 0) seperated = 1;
        double vector_between_cyl_axis[3];
        vector_between_cyl_axis[0] = delta[0] - scalar_prod1*vector1.x;
        vector_between_cyl_axis[1] = delta[1] - scalar_prod1*vector1.y;
        vector_between_cyl_axis[2] = delta[2] - scalar_prod1*vector1.z;
        if (verbal == 1) printf("vector_between = (%f,%f,%f)\n",vector_between_cyl_axis[0],vector_between_cyl_axis[1],vector_between_cyl_axis[2]);
        if (verbal == 1) printf("length of vector between = %f\n",length_of_3vector(vector_between_cyl_axis));
        if (verbal == 1) printf("radius1 = %f , radius2=%f\n",radius1,radius2);
        if (radius1 < radius2 + length_of_3vector(vector_between_cyl_axis)) { // Answers: Does cylinder 2 stick out of cylinder 1?
        //if (radius1 + length_of_3vector(vector_between_cyl_axis) > radius2 ) { // Answers: Does cylinder 1 stick out of cylinder 2 radially?
                if (verbal == 1) printf("Cylinder sticks out radially \n");
                inside = 0;
                }
        
        if (inside == 0) return 0;
        else return 1;
        
    } else {
        // printf("The component uses a raytracing method for non parallel cylinders.\n");
        // printf("  Make sure not to give this algorithm edge cases, where cylinders just touch.\n");
        Coords cyl_direction2 = geometry_child->geometry_parameters.p_cylinder_storage->direction_vector;
        
        // The center point of the perfect cylinder (cylinder 2) needs to be within the meshgrid one (cylinder 1), otherwise it can not be within
        // Check with a simple call to r_within_cylinder
        
        // if the center of cylinder 2 is within cylinder 1;
        if (r_within_cylinder(geometry_child->center,geometry_parent) == 0) return 0;  // if cylinder 2 center is not within cylinder 1, it is clearly not within
        
        
        // Doing a simple but not perfect overlap test

        // Checking cylinder sides.
        
        // Taking cylinder 1, making a vector at the base center.
        Coords base_point;
        
        base_point.x = geometry_child->center.x - 0.5*height2*cyl_direction2.x;
        base_point.y = geometry_child->center.y - 0.5*height2*cyl_direction2.y;
        base_point.z = geometry_child->center.z - 0.5*height2*cyl_direction2.z;
        
        if (verbal==1) print_position(base_point,"Base point position (for inside cylinder)");
        
        // Making a point at the circumference of the bottom circle of the cylinder
        double cross_input[3] = {0,1,0};
        // In case the cross input is parallel with the vector, a new is chosen. Both can't be parallel.
        if (scalar_prod(cross_input[0],cross_input[1],cross_input[2],cyl_direction2.x,cyl_direction2.y,cyl_direction2.z) > 0.99) {
            cross_input[0] = 1; cross_input[1] = 0; cross_input[2] = 0;
        }
        // print_position(make_position(cross_input),"cross input");
        
        double cross_product1[3] = {0,0,0};
        vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],cyl_direction2.x,cyl_direction2.y,cyl_direction2.z,cross_input[0],cross_input[1],cross_input[2]);
        
        // print_position(make_position(cross_product1),"cross_product1");
        double cross_length = length_of_3vector(cross_product1);
        
        // printf("cross_length = %f \n",cross_length);
        cross_product1[0] /= cross_length;
        cross_product1[1] /= cross_length;
        cross_product1[2] /= cross_length;
        
        cross_product1[0] *= radius2;
        cross_product1[1] *= radius2;
        cross_product1[2] *= radius2;
        
        Coords circ_point;
        double radial_position[3],cyl_direction_pointer[3],base_point_vector[3],cyl_radial_direction[3];
        
        cyl_direction_pointer[0] = cyl_direction2.x;
        cyl_direction_pointer[1] = cyl_direction2.y;
        cyl_direction_pointer[2] = cyl_direction2.z;
        
        //print_position(coords_set(cyl_direction_pointer[0],cyl_direction_pointer[1],cyl_direction_pointer[2]),"cylinder direction vector");
        //print_position(coords_set(cross_product1[0],cross_product1[1],cross_product1[2]),"cross product (before rotation)");
        
        int iterate,number_of_solutions,solutions,number_of_positions = 30;
        double rotate_angle,temp_solution[2],positive_solution,negative_solution;
        
        // printf("length of cyl_direction_pointer = %f \n",length_of_3vector(cyl_direction_pointer));
        
        // Check intersection with cylinder 2 with that point, and cyl_direction, if there is an intersection before height1, they overlap.
        // Rotate the circumference point around the cyl_direction for a full circle to detect intersections all the way around.
        
        // Here cross_product1 is a vector from the base point to a point n the circumference
        // circ_point is a vector from the base point to the circumference rotated an angle.
        // radial_position is the actual position on the circumference on the cylinder as a vector from origo.
        Coords radial_coords,top_coords;
        
        for (iterate = 0;iterate < number_of_positions;iterate++) {
                rotate_angle = 2*3.14159*((double) iterate)/((double) number_of_positions);
                rotate(circ_point.x,circ_point.y,circ_point.z,cross_product1[0],cross_product1[1],cross_product1[2],rotate_angle,cyl_direction2.x,cyl_direction2.y,cyl_direction2.z);
            
                radial_position[0] = base_point.x + circ_point.x;
                radial_position[1] = base_point.y + circ_point.y;
                radial_position[2] = base_point.z + circ_point.z;
                // Debug check
                radial_coords = coords_add(base_point,circ_point);
                if (r_within_cylinder(radial_coords,geometry_parent) == 0) {
                    //printf("Radial pointer number %d was not inside cylinder 1 (%f %f %f)\n",iterate,radial_position[0],radial_position[1],radial_position[2]);
                    return 0;
                }
            
                // sample_cylinder_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry) {
				double nx_dummy[2], ny_dummy[2], nz_dummy[2];
				int surface_index_dummy[2];
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy, 
				                          &number_of_solutions,radial_position,cyl_direction_pointer,geometry_parent);
            
                if (number_of_solutions == 2) {
                    if (temp_solution[0]*temp_solution[1] > 0) {
                        // If both solutions are in the future or past, the point is outside the cylinder
                        if (verbal == 1) printf("Along axis: Not inside, as the solutions have the same sign: %f %f\n",temp_solution[0],temp_solution[1]);
                        return 0;
                    } else {
                        // The solutions have different signs
                        if (temp_solution[0] < 0) {
                            negative_solution = temp_solution[0];
                            positive_solution = temp_solution[1];
                        } else {
                            negative_solution = temp_solution[1];
                            positive_solution = temp_solution[0];
                        }
                        // If there is a solution before the cylinder ends, cylinder 2 can not be within cylinder 1
                        if (positive_solution < height2) {
                            if (verbal == 1) printf("Along axis: Not inside, as the positive solutions is less than the cylinder height: %f %f\n",temp_solution[0],temp_solution[1]);
                            if (verbal == 1) printf("Radial position = (%f,%f,%f) \n",radial_position[0],radial_position[1],radial_position[2]);
                            return 0;
                        }
                    }
                } else {
                    if (verbal == 1) printf("Along axis: 0 or 1 solution!\n");
                    return 0; // If there are 1 or 0 solutions, the radial position (on cylinder 2) would not be inside cylinder 1
                }
            
                cyl_radial_direction[0] = circ_point.x;
                cyl_radial_direction[1] = circ_point.y;
                cyl_radial_direction[2] = circ_point.z;
                // Note it has length radius1

                // Debug check
                if (r_within_cylinder(base_point,geometry_parent) == 0) {
                    //printf("Base point number %d was not inside cylinder 1 (%f %f %f)\n",iterate,base_point_vector[0],base_point_vector[1],base_point_vector[2]);
                    return 0;
                }
            
                // Base point in vector notation needed for intersect function.
                base_point_vector[0] = base_point.x;
                base_point_vector[1] = base_point.y;
                base_point_vector[2] = base_point.z;
            
                // The vector circ_point is from the base to the circumference. This is used to check the bottom cap.
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy,
				                          &number_of_solutions,base_point_vector,cyl_radial_direction,geometry_parent);
                
                if (number_of_solutions == 2) {
                    if (temp_solution[0]*temp_solution[1] > 0) {
                        // If both solutions are in the future or past, the point is outside the cylinder
                        if (verbal == 1) printf("Radial bottom: Not inside, as the solutions have the same sign: %f %f\n",temp_solution[0],temp_solution[1]);
                        return 0;
                    } else {
                        // The solutions have different signs
                        if (temp_solution[0] < 0) {
                            negative_solution = temp_solution[0];
                            positive_solution = temp_solution[1];
                        } else {
                            negative_solution = temp_solution[1];
                            positive_solution = temp_solution[0];
                        }
                        // If there is a solution before the line reaches the circumference from the center, cylinder 2 can not be within cylinder 1
                        if (positive_solution < 1 || negative_solution > -1) {
                            if (verbal == 1) printf("Radial bottom: Not inside, as the positive solutions is less than the cylinder radius: %f %f\n",temp_solution[0],temp_solution[1]);
                            return 0;
                        }
                    }
                } else {
                    if (verbal == 1) printf("Radially bottom: 0 or 1 solution!\n");
                    if (verbal == 1) print_position(circ_point,"current circ point");
                    if (verbal == 1) print_position(coords_set(cyl_radial_direction[0],cyl_radial_direction[1],cyl_radial_direction[2]),"current cyl_radial_direction (should be same as above)");
                    return 0; // If there are 1 or 0 solutions, the radial position (on cylinder 2) would not be inside cylinder 1
                }
            
                // Now check the top
                base_point_vector[0] = base_point.x + height2*cyl_direction2.x;
                base_point_vector[1] = base_point.y + height2*cyl_direction2.y;
                base_point_vector[2] = base_point.z + height2*cyl_direction2.z;
            
                top_coords = coords_set(base_point_vector[0],base_point_vector[1],base_point_vector[2]);
                // Debug check
                if (r_within_cylinder(top_coords,geometry_parent) == 0) {
                    //printf("Top point number %d was not inside cylinder 1 (%f %f %f)\n",iterate,base_point_vector[0],base_point_vector[1],base_point_vector[2]);
                    return 0;
                }
            
                // The vector circ_point is from the base to the circumference. This is used to check the bottom cap.
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy,
				                          &number_of_solutions,base_point_vector,cyl_radial_direction,geometry_parent);
                if (number_of_solutions == 2) {
                    if (temp_solution[0]*temp_solution[1] > 0) {
                        // If both solutions are in the future or past, the point is outside the cylinder
                        if (verbal == 1) printf("Radial top: Not inside, as the solutions have the same sign: %f %f\n",temp_solution[0],temp_solution[1]);
                        return 0;
                    } else {
                        // The solutions have different signs
                        if (temp_solution[0] < 0) {
                            negative_solution = temp_solution[0];
                            positive_solution = temp_solution[1];
                        } else {
                            negative_solution = temp_solution[1];
                            positive_solution = temp_solution[0];
                        }
                        // If there is a solution before the line reaches the circumference from the center, cylinder 2 can not be within cylinder 1
                        if (positive_solution < 1 || negative_solution > -1) {
                            if (verbal == 1) printf("Radial top: Not inside, as the positive solutions is less than the cylinder radius: %f %f\n",temp_solution[0],temp_solution[1]);
                            return 0;
                        }
                    }
                } else {
                    if (verbal == 1) printf("Radially top: 0 or 1 solution!\n");
                    return 0; // If there are 1 or 0 solutions, the radial position (on cylinder 2) would not be inside cylinder 1
                }

        }
        // The above method is not perfect as it basicly tests a mesh grid of the cylinder aginst another perfect cylinder.
        // Can be improved.
        
        // If no intersections is found and the center of cylinder 2 is within cylinder 1, cylinder 2 must be within cylinder 1.
        return 1;
    
    }

};

int cylinder_within_cylinder_backup(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
//int cylinder_within_cylinder(struct geometry_struct *geometry_parent,struct geometry_struct *geometry_child) {
    // Unpack parameters
    double radius1 = geometry_parent->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height1 = geometry_parent->geometry_parameters.p_cylinder_storage->height;
    double radius2 = geometry_child->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height2 = geometry_child->geometry_parameters.p_cylinder_storage->height;
    
    int verbal = 1;
    // Generate unit direction vector along center axis of cylinders
    
    // Start with vector that points along the cylinder in the simple frame, and rotate to global
    Coords simple_vector = coords_set(0,1,0);
    Coords vector1,vector2;
    if (verbal == 1) printf("Cords start_vector = (%f,%f,%f)\n",simple_vector.x,simple_vector.y,simple_vector.z);

    // Rotate the position of the ray around the center of the cylinder
    vector1 = rot_apply(geometry_parent->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector1.z);
    // Rotate the position of the ray around the center of the cylinder
    vector2 = rot_apply(geometry_child->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector2 = (%f,%f,%f)\n",vector2.x,vector2.y,vector2.z);
    
    // if vector1 and vector2 are parallel, the problem is simple, but if not complicated
    double cross_product1[3] = {0,0,0};
    // printf("%f\n",cross_product1[0]);
    // vec prod(&ax,&ay,&az,bx,by,bz, cx,cy,cz)
    vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],vector1.x,vector1.y,vector1.z,vector2.x,vector2.y,vector2.z);
    // I get an error taking the adress of cross_product1[0], &cross_product1[0]. Took the pointer adresses instead. Works fine.
    if (verbal == 1) printf("cross_product = (%f,%f,%f)\n",cross_product1[0],cross_product1[1],cross_product1[2]);
    double cross_product_length = length_of_3vector(cross_product1);
    
    if (cross_product_length == 0) {
        // The cylinders are parallel.
        int seperated = 0;
        double delta[3];
        delta[0] = geometry_parent->center.x - geometry_child->center.x;
        delta[1] = geometry_parent->center.y - geometry_child->center.y;
        delta[2] = geometry_parent->center.z - geometry_child->center.z;

        // Test for separation by height
        // if (h0Div2 + h1Div2 − |Dot(W0, Delta )| < 0) seperated = 1;
        
        if (verbal == 1) printf("vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector2.z);
        if (verbal == 1) printf("delta1 = (%f,%f,%f)\n",delta[0],delta[1],delta[2]);
        double scalar_prod1 = scalar_prod(vector1.x,vector1.y,vector1.z,delta[0],delta[1],delta[2]);
        if (verbal == 1) printf("scalar product = %f \n",scalar_prod1);
        if (verbal == 1) printf("height 1 = %f, height 2 = %f \n",height1,height2);
        
        int inside = 1;
        
        if (height1*0.5 < height2*0.5 + fabs(scalar_prod1)) {
                if (verbal == 1) printf("Cylinder sticks out height wise \n");
                inside = 0;
                }
    
        // Test for separation radially
        // if (rSum − |Delta − Dot(W0,Delta)∗W0| < 0) seperated = 1;
        double vector_between_cyl_axis[3];
        vector_between_cyl_axis[0] = delta[0] - scalar_prod1*vector1.x;
        vector_between_cyl_axis[1] = delta[1] - scalar_prod1*vector1.y;
        vector_between_cyl_axis[2] = delta[2] - scalar_prod1*vector1.z;
        if (verbal == 1) printf("vector_between = (%f,%f,%f)\n",vector_between_cyl_axis[0],vector_between_cyl_axis[1],vector_between_cyl_axis[2]);
        if (verbal == 1) printf("length of vector between = %f\n",length_of_3vector(vector_between_cyl_axis));
        if (verbal == 1) printf("radius1 = %f , radius2=%f\n",radius1,radius2);
        if (radius1 < radius2 + length_of_3vector(vector_between_cyl_axis)) { // Answers: Does cylinder 2 stick out of cylinder 1?
        //if (radius1 + length_of_3vector(vector_between_cyl_axis) > radius2 ) { // Answers: Does cylinder 1 stick out of cylinder 2 radially?
                if (verbal == 1) printf("Cylinder sticks out radially \n");
                inside = 0;
                }
        
        if (inside == 0) return 0;
        else return 1;
        
    } else {
        // printf("The component uses a raytracing method for non parallel cylinders.\n");
        // printf("  Make sure not to give this algorithm edge cases, where cylinders just touch.\n");
        Coords cyl_direction1 = geometry_parent->geometry_parameters.p_cylinder_storage->direction_vector;
        
        // The center point of the perfect cylinder (cylinder 2) needs to be within the meshgrid one (cylinder 1), otherwise it can not be within
        // Check with a simple call to r_within_cylinder
        
        // if the center of cylinder 2 is within cylinder 1;
        if (r_within_cylinder(geometry_child->center,geometry_parent) == 0) return 0;  // if cylinder 2 center is not within cylinder 1, it is clearly not within
        
        
        // Doing a simple but not perfect overlap test

        // Checking cylinder sides.
        
        // Taking cylinder 1, making a vector at the base center.
        Coords base_point;
        
        base_point.x = geometry_parent->center.x - 0.5*height1*cyl_direction1.x;
        base_point.y = geometry_parent->center.y - 0.5*height1*cyl_direction1.y;
        base_point.z = geometry_parent->center.z - 0.5*height1*cyl_direction1.z;
        
        // Making a point at the circumference of the bottom circle of the cylinder
        double cross_input[3] = {0,1,0};
        // In case the cross input is parallel with the vector, a new is chosen. Both can't be parallel.
        if (scalar_prod(cross_input[0],cross_input[1],cross_input[2],cyl_direction1.x,cyl_direction1.y,cyl_direction1.z) > 0.99) {
            cross_input[0] = 1; cross_input[1] = 0; cross_input[2] = 0;
        }
        // print_position(make_position(cross_input),"cross input");
        
        double cross_product1[3] = {0,0,0};
        vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],cyl_direction1.x,cyl_direction1.y,cyl_direction1.z,cross_input[0],cross_input[1],cross_input[2]);
        
        // print_position(make_position(cross_product1),"cross_product1");
        double cross_length = length_of_3vector(cross_product1);
        
        // printf("cross_length = %f \n",cross_length);
        cross_product1[0] /= cross_length;
        cross_product1[1] /= cross_length;
        cross_product1[2] /= cross_length;
        
        cross_product1[0] *= radius1;
        cross_product1[1] *= radius1;
        cross_product1[2] *= radius1;
        
        Coords circ_point;
        double radial_position[3],cyl_direction_pointer[3],base_point_vector[3],cyl_radial_direction[3];
        
        cyl_direction_pointer[0] = cyl_direction1.x;
        cyl_direction_pointer[1] = cyl_direction1.y;
        cyl_direction_pointer[2] = cyl_direction1.z;
        
        print_position(make_position(cyl_direction_pointer),"cylinder direction vector");
        
        int iterate,number_of_solutions,solutions,number_of_positions = 30;
        double rotate_angle,temp_solution[2];
        
        // printf("length of cyl_direction_pointer = %f \n",length_of_3vector(cyl_direction_pointer));
        
        // Check intersection with cylinder 2 with that point, and cyl_direction, if there is an intersection before height1, they overlap.
        // Rotate the circumference point around the cyl_direction for a full circle to detect intersections all the way around.
        
        // Here cross_product1 is a vector from the base point to a point n the circumference
        // circ_point is a vector from the base point to the circumference rotated an angle.
        // radial_position is the actual position on the circumference on the cylinder as a vector from origo.
        for (iterate = 0;iterate < number_of_positions;iterate++) {
                rotate_angle = 2*3.14159*((double) iterate)/((double) number_of_positions);
                rotate(circ_point.x,circ_point.y,circ_point.z,cross_product1[0],cross_product1[1],cross_product1[2],rotate_angle,cyl_direction1.x,cyl_direction1.y,cyl_direction1.z);
            
                radial_position[0] = base_point.x + circ_point.x;
                radial_position[1] = base_point.y + circ_point.y;
                radial_position[2] = base_point.z + circ_point.z;
                // sample_cylinder_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry) {
				double nx_dummy[2], ny_dummy[2], nz_dummy[2];
				int surface_index_dummy[2];
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy,
				                          &number_of_solutions,radial_position,cyl_direction_pointer,geometry_child);
                for (solutions = 0;solutions < number_of_solutions;solutions++) {
                    if (temp_solution[solutions] > 0 && temp_solution[solutions] < height1) {
                        // cylinders must overlap.
                        return 0;
                    }
                }
                // if (number_of_solutions == 2) {
                //    if (temp_solution[0] < 0 && temp_solution[1] < 0) return 0; // cylinder 2 outside cylinder 1
                //    if (temp_solution[0] > 0 && temp_solution[1] > 0) return 0; // cylinder 2 outside cylinder 1
                //}
            
                cyl_radial_direction[0] = circ_point.x;
                cyl_radial_direction[1] = circ_point.y;
                cyl_radial_direction[2] = circ_point.z;
                // Note it has length radius1
            
                base_point_vector[0] = base_point.x;
                base_point_vector[1] = base_point.y;
                base_point_vector[2] = base_point.z;
            
                // The vector circ_point is from the base to the circumference. This is used to check the bottom cap.
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy,
				                          &number_of_solutions,base_point_vector,cyl_radial_direction,geometry_child);
                for (solutions = 0;solutions < number_of_solutions;solutions++) {
                    if (temp_solution[solutions] > 0 && temp_solution[solutions] < 1) {
                        // cylinders must overlap.
                        return 0;
                    }
                }
                // if (number_of_solutions == 2) {
                //    if (temp_solution[0] < 0 && temp_solution[1] < 0) return 0; // cylinder 2 outside cylinder 1
                //    if (temp_solution[0] > 0 && temp_solution[1] > 0) return 0; // cylinder 2 outside cylinder 1
                //}
            
                // Now check the top
                base_point_vector[0] = base_point.x + height1*cyl_direction1.x;
                base_point_vector[1] = base_point.y + height1*cyl_direction1.y;
                base_point_vector[2] = base_point.z + height1*cyl_direction1.z;
            
                // The vector circ_point is from the base to the circumference. This is used to check the bottom cap.
                sample_cylinder_intersect(temp_solution, nx_dummy, ny_dummy, nz_dummy, surface_index_dummy,
				                          &number_of_solutions,base_point_vector,cyl_radial_direction,geometry_child);
                for (solutions = 0;solutions < number_of_solutions;solutions++) {
                    if (temp_solution[solutions] > 0 && temp_solution[solutions] < 1) {
                        // cylinders must overlap.
                        return 0;
                    }
                }
                // if (number_of_solutions == 2) {
                //    if (temp_solution[0] < 0 && temp_solution[1] < 0) return 0; // cylinder 2 outside cylinder 1
                //    if (temp_solution[0] > 0 && temp_solution[1] > 0) return 0; // cylinder 2 outside cylinder 1
                //}
        }
        // The above method is not perfect as it basicly tests a mesh grid of the cylinder aginst another perfect cylinder.
        // Can be improved.
        
        // If no intersections is found and the center of cylinder 2 is within cylinder 1, cylinder 2 must be within cylinder 1.
        return 1;
    
    }

};

int cone_overlaps_cone(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
  // Overlap function should return 1 if the to geometries both cover some volume
  // Temporary function

    // Load Variables:
    Coords direction_1 = geometry1->geometry_parameters.p_cone_storage->direction_vector;
    Coords center_1 = geometry1->center;
    double radius_top_1 = geometry1->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom_1 = geometry1->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height_1 = geometry1->geometry_parameters.p_cone_storage->height;

    Coords direction_2 = geometry2->geometry_parameters.p_cone_storage->direction_vector;
    Coords center_2 = geometry2->center;
    double radius_top_2 = geometry2->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom_2 = geometry2->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height_2 = geometry2->geometry_parameters.p_cone_storage->height;

    double Y;
    double max_r;


    // // Simple test to see if they are far away (with smallest spheres outside)
    // Create Spheres

    /*
    Y = -(0.5*height_1)-(radius_top_1*radius_top_1-radius_bottom_1*radius_bottom_1)/(2.0*height_1);
    if (radius_top_1 > radius_bottom_1){
        max_r = radius_top_1;
    }else{
        max_r = radius_bottom_1;
    }
    double sphere_1_radius =  sqrt((Y+0.5*height_1)*(Y+0.5*height_1)+max_r*max_r);
    Coords sphere_1_pos = coords_set(center_1.x+direction_1.x*Y,center_1.y+direction_1.y*Y,center_1.z+direction_1.z*Y);
    */

    // Not sure above works, writing own version.
    
    double dist_above_bottom = 0.5*(radius_top_1*radius_top_1+height_1*height_1-radius_bottom_1*radius_bottom_1)/height_1;
    double dist_from_center = dist_above_bottom - 0.5*height_1;
    Coords sphere_1_pos = coords_set(center_1.x+direction_1.x*dist_from_center,
                                     center_1.y+direction_1.y*dist_from_center,
                                     center_1.z+direction_1.z*dist_from_center);
    double sphere_1_radius = sqrt(radius_bottom_1*radius_bottom_1+dist_above_bottom*dist_above_bottom);
    
    /*
    Y = -(0.5*height_2)-(radius_top_2*radius_top_2-radius_bottom_2*radius_bottom_2)/(2.0*height_2);
    if (radius_top_2 > radius_bottom_2){
        max_r = radius_top_2;
    }else{
        max_r = radius_bottom_2;
    }
    double sphere_2_radius =  sqrt((Y+0.5*height_2)*(Y+0.5*height_2)+max_r*max_r);
    Coords sphere_2_pos = coords_set(center_2.x+direction_2.x*Y,center_2.y+direction_2.y*Y,center_2.z+direction_2.z*Y);
    */
    
    dist_above_bottom = 0.5*(radius_top_2*radius_top_2+height_2*height_2-radius_bottom_2*radius_bottom_2)/height_2;
    dist_from_center = dist_above_bottom - 0.5*height_2;
    Coords sphere_2_pos = coords_set(center_2.x+direction_2.x*dist_from_center,
                                     center_2.y+direction_2.y*dist_from_center,
                                     center_2.z+direction_2.z*dist_from_center);
    double sphere_2_radius = sqrt(radius_bottom_2*radius_bottom_2+dist_above_bottom*dist_above_bottom);

    // Test if spheres are too long apart to have any chance of intersecting

    double dist_spheres = sqrt((sphere_1_pos.x-sphere_2_pos.x)*(sphere_1_pos.x-sphere_2_pos.x)+(sphere_1_pos.y-sphere_2_pos.y)*(sphere_1_pos.y-sphere_2_pos.y)+(sphere_1_pos.z-sphere_2_pos.z)*(sphere_1_pos.z-sphere_2_pos.z));


    if (dist_spheres > sphere_1_radius + sphere_2_radius){
        //printf("\nSpherical method determined that cones are too far away for intersection to be relevant\n");
        return 0;
    }

    // // Simple test to see if they are inside (with largest spheres inside)
    // Brute force in two steps.
    // 1. Check if any points on 1 lies within 2
    // 2. Check if any transversal lines on the mesh of 1 intersects with 2

    // Calculate needed information
    Coords cone_1_bottom_point = coords_add(center_1,coords_scalar_mult(direction_1,-0.5*height_1));
    Coords cone_1_top_point = coords_add(center_1,coords_scalar_mult(direction_1,0.5*height_1));
    Coords cone_2_bottom_point = coords_add(center_2,coords_scalar_mult(direction_2,-0.5*height_2));
    Coords cone_2_top_point = coords_add(center_2,coords_scalar_mult(direction_2,0.5*height_2));


    // Create two circles for both geometries
    int resoultuion = 500;

    struct pointer_to_1d_coords_list cone_1_points = geometry1->shell_points(geometry1,resoultuion);

    //points_on_circle(cone_1_top,cone_1_top_point,direction_1,radius_top_1,resoultuion);
    //points_on_circle(cone_1_bottom,cone_1_bottom_point,direction_1,radius_bottom_1,resoultuion);

    //printf("\nTEST\n");
    int i;
    // Test geometry 1 points inside geometry 2

    for (i = 0 ; i < cone_1_points.num_elements ; i++){
        
        if (r_within_cone(cone_1_points.elements[i],geometry2) == 1){
            //printf("\nOne point on cone 1 is inside cone 2\n");
            return 1;
        }
    }
    
    struct pointer_to_1d_coords_list cone_2_points = geometry2->shell_points(geometry2,resoultuion);

    // Test geometry 2 points inside geometry 1
    for (i = 0 ; i < cone_2_points.num_elements ; i++){
        
        if (r_within_cone(cone_2_points.elements[i],geometry1) == 1){
            //printf("\nOne point on cone 2 is inside cone 1\n");
            return 1;
        }
    }

    // Test 1 within 2


    // // Test if there is any intersection (intersection function or eqation?)

    // This is an implementation of brute force. Maybe do this with a calculated function?
    int circ_resolution = 50;  // how many lines will the be checked for
    int height_resolution = 150;

    double length_of_cone_side_1 = sqrt(pow(radius_top_1-radius_bottom_1,2)+pow(height_1,2));
    double length_of_cone_side_2 = sqrt(pow(radius_top_2-radius_bottom_2,2)+pow(height_2,2));

    double slope_1 = (radius_top_1-radius_bottom_1)/height_1;
    double slope_2 = (radius_top_2-radius_bottom_2)/height_2;

    double local_radius;

    Coords cone_1_direction = geometry1->geometry_parameters.p_cone_storage->direction_vector;
    Coords cone_2_direction = geometry2->geometry_parameters.p_cone_storage->direction_vector;

    //printf("\nlength_of_cone_side_1 = %f\n",length_of_cone_side_1);

    Coords circ_points[50];
    double circ_offset;
    Coords circ_center;

    int j;

    for (i = 0 ; i < height_resolution ; i++){
        // Calculate circ offset
        //circ_offset = i * length_of_cone_side_1 / height_resolution; // Possible bug
        circ_offset = i * height_1 / height_resolution;

        // Calculate middle point
        circ_center = coords_add(cone_1_bottom_point,coords_set(cone_1_direction.x * circ_offset,cone_1_direction.y * circ_offset,cone_1_direction.z * circ_offset));

        // Calculate radius
        local_radius = circ_offset * slope_1 + radius_bottom_1;

        // Make points on circle
        //printf("points on circle: circ_center = [%f,%f,%f] , cone_1_direction = [%f,%f,%f] , local_radius = %f , circ_resolution = %i",circ_center.x,circ_center.y,circ_center.z,cone_1_direction.x,cone_1_direction.y,cone_1_direction.z,local_radius,circ_resolution);
        points_on_circle(circ_points,circ_center,cone_1_direction,local_radius,circ_resolution);

        // Test if any points lies within geomtry 2
        for (j = 0 ; j < circ_resolution; j++){
            //printf("\ntested if point [%i] [%f,%f,%f] is inside",j,circ_points[j].x,circ_points[j].y,circ_points[j].z);
            if (r_within_cone(circ_points[j],geometry2) == 1){
                //printf("\nOne point on cone 1 is inside cone 2\n");
                return 1;
            }
        }
    }


    for (i = 0 ; i < height_resolution ; i++){
        // Calculate circ offset
        // circ_offset = i * length_of_cone_side_1 / height_resolution; // Possible bug
        circ_offset = i * height_2 / height_resolution; // Possible bug

        // Calculate middle point
        circ_center = coords_add(cone_2_bottom_point,coords_set(cone_2_direction.x * circ_offset,cone_2_direction.y * circ_offset,cone_2_direction.z * circ_offset));

        // Calculate radius
        local_radius = circ_offset * slope_2 + radius_bottom_2;

        // Make points on circle
        //printf("points on circle: circ_center = [%f,%f,%f] , cone_1_direction = [%f,%f,%f] , local_radius = %f , circ_resolution = %i",circ_center.x,circ_center.y,circ_center.z,cone_2_direction.x,cone_2_direction.y,cone_2_direction.z,local_radius,circ_resolution);
        points_on_circle(circ_points,circ_center,cone_2_direction,local_radius,circ_resolution);

        // Test if any points lies within geomtry 2
        for (j = 0 ; j < circ_resolution; j++){
            //printf("\ntested if point [%i] [%f,%f,%f] is inside",j,circ_points[j].x,circ_points[j].y,circ_points[j].z);
            if (r_within_cone(circ_points[j],geometry1) == 1){
                //printf("\nOne point on cone 2 is inside cone 1\n");
                return 1;
            }
        }
    }

    return 0;

};

int cone_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return A_within_B(geometry_child,geometry_parent,(int) 300); // 150 points on each end cap
};

int mesh_overlaps_mesh(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
  // Overlap function should return 1 if the to geometries both cover some volume
  // Temporary function

    // Brute force check if there is one point of geometry 1 in 2 and 2 in 1.

    // Should also have a secondary check with edges intersecting on faces.
    // Could be made faster with a bounding box (or sphere)


    // Load Variables:
    struct pointer_to_1d_coords_list shell_points1 = geometry1->shell_points(geometry1,144);
    struct pointer_to_1d_coords_list shell_points2 = geometry2->shell_points(geometry2,144);

    int i;
    for (i = 0 ; i < shell_points1.num_elements ; i++){
        if (geometry2->within_function(shell_points1.elements[i],geometry2)){
            free(shell_points1.elements);
            free(shell_points2.elements);
            return 1;
        }
    }
    for (i = 0 ; i < shell_points2.num_elements ; i++){
        if (geometry1->within_function(shell_points2.elements[i],geometry1)){
            free(shell_points1.elements);
            free(shell_points2.elements);
            return 1;
        }
    }
    
    free(shell_points1.elements);
    free(shell_points2.elements);

    return 0;

};
int mesh_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};

int box_within_mesh(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};

int mesh_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};
int sphere_within_mesh(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};
int cone_within_mesh(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};


int mesh_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};

int mesh_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};


int cylinder_within_mesh(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};

int mesh_within_mesh(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // WARNING: This may fail as one or both of the meshes may not be convex
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    return mesh_A_within_B(geometry_child,geometry_parent); // 30 points on each end cap
};

// -------------    Overlap functions for two different geometries --------------------------------

int box_overlaps_cylinder(struct geometry_struct *geometry_box,struct geometry_struct *geometry_cyl) {
    // Checking if the box and cylinder described by geometry_box and geometry_cyl overlaps.
    // Done in steps:
    // If any corner points of the box is within the cylinder, they do overlap
    // If any points on the cylinders end caps are within the box, they do overlap
    // If any of the lines describing the sides of the box intersect the cylinder, they do overlap
    // If the symmetry line of the cylinder intersect the box, they do overlap
    // If none of the above are true, they do not overlap
    
    // A problem with this algorithm is a lack of a quick exit if the volumes obviously does not overlap

    // Generate coordinates of corners of box
    Coords corner_points[8];
    box_corners_global_frame(corner_points,geometry_box);
    
    // Check earch corner seperatly
    int iterate;
    for (iterate=0;iterate<8;iterate++) {
        if (geometry_cyl->within_function(corner_points[iterate],geometry_cyl) == 1) {
            return 1; // If a corner of the box is inside the cylinder, the two volumes overlap
        }
    }
    
    Coords cyl_direction = geometry_cyl->geometry_parameters.p_cylinder_storage->direction_vector;
    Coords center = geometry_cyl->center;
    double radius = geometry_cyl->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height = geometry_cyl->geometry_parameters.p_cylinder_storage->height;
    
    Coords cyl_top_point = coords_add(center,coords_scalar_mult(cyl_direction,0.5*height));
    Coords cyl_bottom_point = coords_add(center,coords_scalar_mult(cyl_direction,-0.5*height));
    
    // Generate 100 points on the circle describing the top of the cylinder
    Coords *circle_point_array;
    int number_of_points = 150;
    circle_point_array = malloc(number_of_points * sizeof(Coords));
    if (!circle_point_array) {
      fprintf(stderr,"Failure allocating list in Union function box_overlaps_cylinder - Exit!\n");
      exit(EXIT_FAILURE);
    }
    points_on_circle(circle_point_array,cyl_top_point,cyl_direction,radius,number_of_points);
    
    // Check parts of cylinder top seperatly
    for (iterate=0;iterate<number_of_points;iterate++) {
        if (geometry_box->within_function(circle_point_array[iterate],geometry_box) == 1) {
            return 1; // If part of the cylinder is inside the box, the volumes overlap
        }
    }
    
    // Check parts of cylinder bottom seperatly
    points_on_circle(circle_point_array,cyl_bottom_point,cyl_direction,radius,number_of_points);
    for (iterate=0;iterate<number_of_points;iterate++) {
        if (geometry_box->within_function(circle_point_array[iterate],geometry_box) == 1) {
            return 1; // If part of the cylinder is inside the box, the volumes overlap
        }
    }
    free(circle_point_array);

    // Check intersections for the lines between the corners of the box and the cylinder
    // 12 sides to a box, if any one of them intersects, the volumes overlaps
    for (iterate=0;iterate<3;iterate++) { //
        if (existence_of_intersection(corner_points[iterate],corner_points[iterate+1],geometry_cyl) == 1) return 1;
    }
    if (existence_of_intersection(corner_points[3],corner_points[0],geometry_cyl) == 1) return 1;
    for (iterate=4;iterate<7;iterate++) {
        if (existence_of_intersection(corner_points[iterate],corner_points[iterate+1],geometry_cyl) == 1) return 1;
    }
    if (existence_of_intersection(corner_points[7],corner_points[4],geometry_cyl) == 1) return 1;
    for (iterate=0;iterate<4;iterate++) {
        if (existence_of_intersection(corner_points[iterate],corner_points[iterate+4],geometry_cyl) == 1) return 1;
    }
    
    // Only need to test the intersection between the symetry line of the cylinder and the box
    if (existence_of_intersection(cyl_top_point,cyl_bottom_point,geometry_box) == 1) return 1;
    
    // If all the tests change, the volumes do not overlap
    return 0;
};

int cylinder_overlaps_box(struct geometry_struct *geometry_cyl,struct geometry_struct *geometry_box) {
    // overlap functions are symetrical, but it is convinient to have both defined
    return box_overlaps_cylinder(geometry_box,geometry_cyl);
};

int cylinder_overlaps_sphere(struct geometry_struct *geometry_cyl,struct geometry_struct *geometry_sph) {
 
    // If the sphere center is inside, one can exit fast
    Coords sph_center = geometry_sph->center;
    if (geometry_cyl->within_function(sph_center,geometry_cyl) == 1) return 1;
    
    // If cylinder center is inside, one can exit fast
    Coords cyl_center = geometry_cyl->center;
    if (geometry_sph->within_function(cyl_center,geometry_sph) == 1) return 1;
    
    double cyl_radius = geometry_cyl->geometry_parameters.p_cylinder_storage->cyl_radius;
    double cyl_height = geometry_cyl->geometry_parameters.p_cylinder_storage->height;
    Coords cyl_direction = geometry_cyl->geometry_parameters.p_cylinder_storage->direction_vector;
    
    // Or cylinder top / bottom point
    Coords cyl_top_point = coords_add(cyl_center,coords_scalar_mult(cyl_direction,0.5*cyl_height));
    if (geometry_sph->within_function(cyl_top_point,geometry_sph) == 1) return 1;
    
    Coords cyl_bottom_point = coords_add(cyl_center,coords_scalar_mult(cyl_direction,-0.5*cyl_height));
    if (geometry_sph->within_function(cyl_bottom_point,geometry_sph) == 1) return 1;
    
    // Calculate distance
    double distance = distance_between(geometry_cyl->center,geometry_sph->center);
    
    double sph_radius = geometry_sph->geometry_parameters.p_sphere_storage->sph_radius;
    
    // Return 0 if the bounding sphere and the sphere do not overlap, otherwise do brute force
    if (distance > sph_radius + sqrt(cyl_radius*cyl_radius+0.25*cyl_height*cyl_height)) return 0;
    
    // Could check "inner sphere" of cylinder against sphere, if they overlap, the geometries overlap
    if (cyl_height >= 2.0*cyl_radius) {
        if (distance < sph_radius + cyl_radius) return 1;
    } else {
        if (distance < sph_radius + 0.5*cyl_height) return 1;
    }
    
    // Projection method
    // Find the distance between cylinder and sphere perpendicular to the cylinder direction.
    Coords difference = coords_sub(sph_center,cyl_center);
    
    // projection is simple as the cylinder direction vector is a normal vector
    Coords projection = coords_scalar_mult(cyl_direction,union_coords_dot(difference,cyl_direction));
    Coords perpendicular = coords_sub(difference,projection);
    
    if (length_of_position_vector(perpendicular) > sph_radius + cyl_radius) return 0;
    
    // Brute force
    // Consider enlarging the sphere slightly to decrease the probability for false negatives
    //  at the cost of some false positives. This is acceptable as false positives will not
    //  have any severe effect, but false negatives causes errors.
    
    // Random tests shows no issues with this approach, false negatives disapeared.
    struct sphere_storage temp_sph_storage;
    temp_sph_storage.sph_radius = 1.02*sph_radius;

    struct geometry_struct temp_sph;
    temp_sph.geometry_parameters.p_sphere_storage = &temp_sph_storage;
    temp_sph.center = geometry_sph->center;
    
    // temp_sph is not fully initialized, it just has geometrical information
    
    struct pointer_to_1d_coords_list shell_points;
    shell_points = geometry_sph->shell_points(&temp_sph,300*300); // using 300 rings with 300 points, works but is slow
    //shell_points = geometry_sph->shell_points(&temp_sph,70*70); // using 50 rings with 50 points
    //shell_points = geometry_sph->shell_points(&temp_sph,50*50); // using 50 rings with 50 points
    
    int iterate;
    for (iterate=0;iterate<shell_points.num_elements;iterate++) {
      if (geometry_cyl->within_function(shell_points.elements[iterate],geometry_cyl) == 1) {
        free(shell_points.elements);
        return 1;
      }
    }
    
    free(shell_points.elements);
    
    shell_points = geometry_cyl->shell_points(geometry_cyl,400); // 200 on each ring
    for (iterate=0;iterate<shell_points.num_elements;iterate++) {
      if (geometry_sph->within_function(shell_points.elements[iterate],&temp_sph) == 1) {
        free(shell_points.elements);
        return 1;
      }
    }
    
    free(shell_points.elements);
    return 0;
    
    
    /*
    // Using the actual sphere size and position
    struct pointer_to_1d_coords_list shell_points;
    shell_points = geometry_sph->shell_points(geometry_sph,250000); // using 500 rings with 500 points
    
    
    int iterate;
    for (iterate=0;iterate<shell_points.num_elements;iterate++) {
      if (geometry_cyl->within_function(shell_points.elements[iterate],geometry_cyl) == 1) {
        free(shell_points.elements);
        return 1;
      }
    }
    
    free(shell_points.elements);
    
    shell_points = geometry_cyl->shell_points(geometry_cyl,400); // 200 on each ring
    for (iterate=0;iterate<shell_points.num_elements;iterate++) {
      if (geometry_sph->within_function(shell_points.elements[iterate],geometry_sph) == 1) {
        free(shell_points.elements);
        return 1;
      }
    }
    
    free(shell_points.elements);
    return 0;
    */
};

int box_overlaps_sphere(struct geometry_struct *geometry_box,struct geometry_struct *geometry_sph) {
    
    //printf("\n checking sphere center in box\n");
    // If the sphere center is inside box, one can exit fast
    Coords sph_center = geometry_sph->center;
    if (geometry_box->within_function(sph_center,geometry_box) == 1) return 1;
    
    //printf("\n checking box center in sphere\n");
    // If the box center is inside sphere, one can exit fast
    Coords box_center = geometry_box->center;
    if (geometry_sph->within_function(box_center,geometry_sph) == 1) return 1;
    
    // Check if box corners are inside the sphere
    int iterate;
    struct pointer_to_1d_coords_list shell_points;
    shell_points = geometry_box->shell_points(geometry_box,8);
    
    for (iterate=0;iterate<shell_points.num_elements;iterate++) {
      if (geometry_sph->within_function(shell_points.elements[iterate],geometry_sph) == 1) {
        free(shell_points.elements);
        return 1;
      }
    }
    free(shell_points.elements);
    
    // Can not find elegant solution to this problem. Will use brute force.
    
    // Before brute forcing, find negative solutions for obvious cases.
    // Use circle - circle overlap algorithm, find bounding circle for box.
    
    //printf("\n checking bounding sphere approach\n");
    Coords corner_ps[8];
    
    double this_length,max_length = 0;
    
    box_corners_local_frame(corner_ps,geometry_box); // Local frame: center in (0,0,0)
    for (iterate=0;iterate<8;iterate++) {
      this_length = length_of_position_vector(corner_ps[iterate]);
      if (this_length > max_length) max_length = this_length;
    }
    // Box has a bounding circle with radius max_length and it's normal center.
    //printf("bounding sphere for box has radius = %f \n",max_length);
    
    double radius = geometry_sph->geometry_parameters.p_sphere_storage->sph_radius;

    // Calculate distance
    double distance = distance_between(geometry_box->center,geometry_sph->center);
    
    
    // Return 0 if the bounding sphere and the sphere do not overlap, otherwise do brute force
    if (distance > radius + max_length) {
      //printf("\n Bounding sphere avoided brute force method in sphere / box overlap\n");
      return 0;
    }
    
    //printf("\n doing brute force method in box overlaps sphere\n");
    // Brute force
    
    // Slightly increase size of the sphere to avoid edgecases, original value already saved
    geometry_sph->geometry_parameters.p_sphere_storage->sph_radius = 1.02*radius;
    
    // Shell points must be free'ed before leaving this function
    shell_points = geometry_sph->shell_points(geometry_sph,100*100); // using 100 rings with 100 points
  
    for (iterate=0;iterate<shell_points.num_elements;iterate++) {
      if (geometry_box->within_function(shell_points.elements[iterate],geometry_box) == 1) {
        free(shell_points.elements);
        geometry_sph->geometry_parameters.p_sphere_storage->sph_radius = radius;
        return 1;
      }
    }
    
    // Reset sphere radius to correct value
    geometry_sph->geometry_parameters.p_sphere_storage->sph_radius = radius;
    
    free(shell_points.elements);
    return 0;
    
};


// sym sphere
int sphere_overlaps_cylinder(struct geometry_struct *geometry_sph,struct geometry_struct *geometry_cyl) {
  return cylinder_overlaps_sphere(geometry_cyl,geometry_sph);
};

int sphere_overlaps_box(struct geometry_struct *geometry_sph,struct geometry_struct *geometry_box) {
  return box_overlaps_sphere(geometry_box,geometry_sph);
};

int cone_overlaps_sphere(struct geometry_struct *geometry_cone,struct geometry_struct *geometry_sph) {
  // Overlap function should return 1 if the to geometries both cover some volume
  // Temporary function
  // Load Variables:
    /*
    Coords direction_1 = geometry_cone->geometry_parameters.p_cone_storage->direction_vector;
    Coords center_1 = geometry_cone->center;
    double radius_top_1 = geometry_cone->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom_1 = geometry_cone->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height_1 = geometry_cone->geometry_parameters.p_cone_storage->height;

    Coords direction_2 = geometry_sph->geometry_parameters.p_sphere_storage->direction_vector;
    Coords center_2 = geometry_sph->center;
    double radius_2 = geometry_sph->geometry_parameters.p_sphere_storage->sph_radius;
    */

    double Y;
    double max_r;
    int resolution = 300;


    // This function is a rewritten verstion of the A_within_B.

    // This function assumes the parent (B) is a convex geoemtry
      // If all points on the shell of geometry A is within B, so are all lines between them.

    
    // FIRST CHECK IF POINTS N CONE IS INSIDE SPHERE:

      // resolution selects the number of points to be generated on the shell.
      struct pointer_to_1d_coords_list shell_points;
      shell_points = geometry_cone->shell_points(geometry_cone,resolution);
      // Shell_points.elements need to be freed before leaving this function
    
      if (shell_points.num_elements > resolution || shell_points.num_elements < 0) {
        printf("\nERROR: Shell point function used in A_within_B return garbage num_elements. \n");
        exit(EXIT_FAILURE);
      }
    
      int iterate;

      for (iterate=0;iterate<shell_points.num_elements;iterate++) {
        if (geometry_sph->within_function(shell_points.elements[iterate],geometry_sph) == 1) {
          free(shell_points.elements);
          //printf("\n ONE POINT OF SPH IS INSIDE CONE\n");
          return 1;
        }
      }
    
      free(shell_points.elements);

    // CHECK IF SPHERE POINTS ARE INSIDE CONE

      // resolution selects the number of points to be generated on the shell.
      shell_points = geometry_sph->shell_points(geometry_sph,resolution);
      // Shell_points.elements need to be freed before leaving this function
    
      if (shell_points.num_elements > resolution || shell_points.num_elements < 0) {
        printf("\nERROR: Shell point function used in A_within_B return garbage num_elements. \n");
        exit(EXIT_FAILURE);
      }
    


      for (iterate=0;iterate<shell_points.num_elements;iterate++) {
        if (geometry_cone->within_function(shell_points.elements[iterate],geometry_cone) == 1) {
          free(shell_points.elements);
          //printf("\n ONE POINT OF CONE IS INSIDE SPH\n");
          return 1;
        }
      }
    
      free(shell_points.elements);


    
      // If just one points is inside, the entire geometry is assumed inside as parent should be convex
      return 0;

};

int sphere_overlaps_cone(struct geometry_struct *geometry_sph,struct geometry_struct *geometry_cone) {
  // This problem is symetrical.
  return cone_overlaps_sphere(geometry_cone,geometry_sph);
};

int cone_overlaps_cylinder(struct geometry_struct *geometry_cone,struct geometry_struct *geometry_cylinder) {
  // Overlap function should return 1 if the to geometries both cover some volume
  // This now works for the simple case where the two directions are parallel. Otherwise it uses A within B.

    // Load Variables:
    Coords direction_1 = geometry_cone->geometry_parameters.p_cone_storage->direction_vector;
    Coords center_1 = geometry_cone->center;
    double radius_top_1 = geometry_cone->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom_1 = geometry_cone->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height_1 = geometry_cone->geometry_parameters.p_cone_storage->height;

    Coords direction_2 = geometry_cylinder->geometry_parameters.p_cylinder_storage->direction_vector;
    Coords center_2 = geometry_cylinder->center;
    double radius_2 = geometry_cylinder->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height_2 = geometry_cylinder->geometry_parameters.p_cylinder_storage->height;

    double radius_bottom_2 = radius_2;
    double radius_top_2 = radius_2;

    // // Simple test to see if they are far away (with smallest spheres outside)
    // Create Spheres
    
    double dist_above_bottom = 0.5*(radius_top_1*radius_top_1+height_1*height_1-radius_bottom_1*radius_bottom_1)/height_1;
    double dist_from_center = dist_above_bottom - 0.5*height_1;
    Coords sphere_1_pos = coords_set(center_1.x+direction_1.x*dist_from_center,
                                     center_1.y+direction_1.y*dist_from_center,
                                     center_1.z+direction_1.z*dist_from_center);
    double sphere_1_radius = sqrt(radius_bottom_1*radius_bottom_1+dist_above_bottom*dist_above_bottom);

    double sphere_2_radius = sqrt(radius_2*radius_2+height_2*height_2);
    Coords sphere_2_pos = center_2;

    //print_position(sphere_1_pos,"sphere_1 pos");
    //printf("sphere_1 radius = %lf \n", sphere_1_radius);
    //print_position(sphere_2_pos,"sphere_2 pos");
    //printf("sphere_2 radius = %lf \n", sphere_2_radius);
    // Test if spheres are too long apart to have any chance of intersecting

    double dist_spheres = sqrt((sphere_1_pos.x-sphere_2_pos.x)*(sphere_1_pos.x-sphere_2_pos.x)+(sphere_1_pos.y-sphere_2_pos.y)*(sphere_1_pos.y-sphere_2_pos.y)+(sphere_1_pos.z-sphere_2_pos.z)*(sphere_1_pos.z-sphere_2_pos.z));


    if (dist_spheres > sphere_1_radius + sphere_2_radius){
        printf("\nSpherical method determined that cones are too far away for intersection to be relevant\n");
        return 0;
    }

    // // Simple test to see if they are inside (with largest spheres inside)
    // Brute force in two steps.
    // 1. Check if any points on 1 lies within 2
    // 2. Check if any transversal lines on the mesh of 1 intersects with 2

    // Calculate needed information
    Coords cone_1_bottom_point = coords_add(center_1,coords_scalar_mult(direction_1,-0.5*height_1));
    Coords cone_1_top_point = coords_add(center_1,coords_scalar_mult(direction_1,0.5*height_1));
    Coords cone_2_bottom_point = coords_add(center_2,coords_scalar_mult(direction_2,-0.5*height_2));
    Coords cone_2_top_point = coords_add(center_2,coords_scalar_mult(direction_2,0.5*height_2));


    // Create two circles for both geometries
    int resoultuion = 300;

    struct pointer_to_1d_coords_list cone_1_points = geometry_cone->shell_points(geometry_cone,resoultuion);
    

    //points_on_circle(cone_1_top,cone_1_top_point,direction_1,radius_top_1,resoultuion);
    //points_on_circle(cone_1_bottom,cone_1_bottom_point,direction_1,radius_bottom_1,resoultuion);


    //printf("\nTEST\n");
    int i;
    // Test geometry 1 points inside geometry 2

    for (i = 0 ; i < cone_1_points.num_elements ; i++){
        
        if (r_within_cylinder(cone_1_points.elements[i],geometry_cylinder) == 1){
            //printf("\nOne point on cone 1 is inside cone 2\n");
            return 1;
        }
    }

    struct pointer_to_1d_coords_list cone_2_points = geometry_cylinder->shell_points(geometry_cylinder,resoultuion);

    // Test geometry 2 points inside geometry 1
    for (i = 0 ; i < cone_2_points.num_elements ; i++){
        
        if (r_within_cone(cone_2_points.elements[i],geometry_cone) == 1){
            //printf("\nOne point on cone 2 is inside cone 1\n");
            return 1;
        }
    }


    // Test 1 within 2


    // // Test if there is any intersection (intersection function or eqation?)

    // This is an implementation of brute force. Maybe do this with a calculated function?
    int circ_resolution = 150;  // how many lines will the be checked for
    int height_resolution = 300;

    double length_of_cone_side_1 = sqrt(pow(radius_top_1-radius_bottom_1,2)+pow(height_1,2));
    double length_of_cone_side_2 = sqrt(pow(radius_top_2-radius_bottom_2,2)+pow(height_2,2));

    double slope_1 = (radius_top_1-radius_bottom_1)/height_1;
    double slope_2 = (radius_top_2-radius_bottom_2)/height_2;

    double local_radius;

    Coords cone_1_direction = geometry_cone->geometry_parameters.p_cone_storage->direction_vector;
    Coords cone_2_direction = geometry_cylinder->geometry_parameters.p_cylinder_storage->direction_vector;

    //printf("\nlength_of_cone_side_1 = %f\n",length_of_cone_side_1);

    Coords circ_points[150];
    double circ_offset;
    Coords circ_center;

    int j;

    for (i = 0 ; i < height_resolution ; i++){
        // Calculate circ offset
        circ_offset = i * height_1 / height_resolution;

        // Calculate middle point
        circ_center = coords_add(cone_1_bottom_point,coords_set(cone_1_direction.x * circ_offset,cone_1_direction.y * circ_offset,cone_1_direction.z * circ_offset));

        // Calculate radius
        local_radius = circ_offset * slope_1 + radius_bottom_1;

        // Make points on circle
        //printf("points on circle: circ_center = [%f,%f,%f] , cone_1_direction = [%f,%f,%f] , local_radius = %f , circ_resolution = %i",circ_center.x,circ_center.y,circ_center.z,cone_1_direction.x,cone_1_direction.y,cone_1_direction.z,local_radius,circ_resolution);
        points_on_circle(circ_points,circ_center,cone_1_direction,local_radius,circ_resolution);

        // Test if any points lies within geomtry 2
        for (j = 0 ; j < circ_resolution; j++){
            //printf("\ntested if point [%i] [%f,%f,%f] is inside",j,circ_points[j].x,circ_points[j].y,circ_points[j].z);
            if (r_within_cylinder(circ_points[j],geometry_cylinder) == 1){
                //printf("\nOne point on cone 1 is inside cone 2\n");
                return 1;
            }
        }
    }


    for (i = 0 ; i < height_resolution ; i++){
        // Calculate circ offset
        circ_offset = i * height_2 / height_resolution;

        // Calculate middle point
        circ_center = coords_add(cone_2_bottom_point,coords_set(cone_2_direction.x * circ_offset,cone_2_direction.y * circ_offset,cone_2_direction.z * circ_offset));

        // Calculate radius
        local_radius = circ_offset * slope_2 + radius_bottom_2;

        // Make points on circle
        //printf("points on circle: circ_center = [%f,%f,%f] , cone_1_direction = [%f,%f,%f] , local_radius = %f , circ_resolution = %i",circ_center.x,circ_center.y,circ_center.z,cone_2_direction.x,cone_2_direction.y,cone_2_direction.z,local_radius,circ_resolution);
        points_on_circle(circ_points,circ_center,cone_2_direction,local_radius,circ_resolution);

        // Test if any points lies within geomtry 2
        for (j = 0 ; j < circ_resolution; j++){
            //printf("\ntested if point [%i] [%f,%f,%f] is inside",j,circ_points[j].x,circ_points[j].y,circ_points[j].z);
            if (r_within_cone(circ_points[j],geometry_cone) == 1){
                //printf("\nOne point on cone 2 is inside cone 1\n");
                return 1;
            }
        }
    }

    return 0;

};

int cylinder_overlaps_cone(struct geometry_struct *geometry_cylinder,struct geometry_struct *geometry_cone) {
  // This problem is symetrical.
  return cone_overlaps_cylinder(geometry_cone,geometry_cylinder);
};

int cone_overlaps_box(struct geometry_struct *geometry_cone,struct geometry_struct *geometry_box) {
  // Overlap function should return 1 if the to geometries both cover some volume
    //  cone_overlaps_box(struct geometry_struct *geometry_cone,struct geometry_struct *geometry_box)

    // Load Variables:
    Coords direction_cone = geometry_cone->geometry_parameters.p_cone_storage->direction_vector;
    Coords center_cone = geometry_cone->center;
    double radius_top_cone = geometry_cone->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius_bottom_cone = geometry_cone->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height_cone = geometry_cone->geometry_parameters.p_cone_storage->height;

    //Coords normal_vectors_box[6] = geometry_box->geometry_parameters.p_box_storage->normal_vectors;
    int is_rectangle =  geometry_box->geometry_parameters.p_box_storage->is_rectangle;
    double x_width1 = geometry_box->geometry_parameters.p_box_storage->x_width1;
    double y_height1 = geometry_box->geometry_parameters.p_box_storage->y_height1;
    double z_depth= geometry_box->geometry_parameters.p_box_storage->z_depth;
    double x_width2 = geometry_box->geometry_parameters.p_box_storage->x_width2;
    double y_height2 = geometry_box->geometry_parameters.p_box_storage->y_height2;
    Coords x_vector = geometry_box->geometry_parameters.p_box_storage->x_vector;
    Coords y_vector = geometry_box->geometry_parameters.p_box_storage->y_vector;
    Coords z_vector = geometry_box->geometry_parameters.p_box_storage->z_vector;
    Coords center_box = geometry_box->center;
    //Coords direction_box = geometry_cone->geometry_parameters.p_box_storage->direction;



    double Y;
    double max_r;


    // // Simple test to see if they are far away (with smallest spheres outside)
    // Create Spheres

    /*
    Y = -(0.5*height_cone)-(radius_top_cone*radius_top_cone-radius_bottom_cone*radius_bottom_cone)/(2*height_cone);
    if (radius_top_cone > radius_bottom_cone){
        max_r = radius_top_cone;
    }else{
        max_r = radius_bottom_cone;
    }
    double sphere_1_radius =  sqrt((Y+(1/2)*height_cone)*(Y+(1/2)*height_cone)+max_r*max_r);
    Coords sphere_1_pos = coords_set(center_cone.x+direction_cone.x*Y,center_cone.y+direction_cone.y*Y,center_cone.z+direction_cone.z*Y);
    */

    double dist_above_bottom = 0.5*(radius_top_cone*radius_top_cone+height_cone*height_cone-radius_bottom_cone*radius_bottom_cone)/height_cone;
    double dist_from_center = dist_above_bottom - 0.5*height_cone;
    Coords sphere_1_pos = coords_set(center_cone.x+direction_cone.x*dist_from_center,
                                     center_cone.y+direction_cone.y*dist_from_center,
                                     center_cone.z+direction_cone.z*dist_from_center);
    double sphere_1_radius = sqrt(radius_bottom_cone*radius_bottom_cone+dist_above_bottom*dist_above_bottom);


    double dist_to_corner;
    double sphere_2_radius = 0;

    dist_to_corner = sqrt(pow(x_width1,2)+pow(x_width1,2));
    if (dist_to_corner > sphere_2_radius) { sphere_2_radius = dist_to_corner ; }
    dist_to_corner = sqrt(pow(x_width1,2)+pow(x_width2,2));
    if (dist_to_corner > sphere_2_radius) { sphere_2_radius = dist_to_corner ; }
    dist_to_corner = sqrt(pow(x_width2,2)+pow(x_width1,2));
    if (dist_to_corner > sphere_2_radius) { sphere_2_radius = dist_to_corner ; }
    dist_to_corner = sqrt(pow(x_width2,2)+pow(x_width2,2));
    if (dist_to_corner > sphere_2_radius) { sphere_2_radius = dist_to_corner ; }

    Coords sphere_2_pos = center_box;


    // Test if spheres are too long apart to have any chance of intersecting

    double dist_spheres = sqrt((sphere_1_pos.x-sphere_2_pos.x)*(sphere_1_pos.x-sphere_2_pos.x)+(sphere_1_pos.y-sphere_2_pos.y)*(sphere_1_pos.y-sphere_2_pos.y)+(sphere_1_pos.z-sphere_2_pos.z)*(sphere_1_pos.z-sphere_2_pos.z));


    if (dist_spheres > sphere_1_radius + sphere_2_radius){
        //printf("\nSpherical method determined that cones are too far away for intersection to be relevant\n");
        return 0;
    }

    // // Simple test to see if they are inside (with largest spheres inside)
    // Brute force in two steps.
    // 1. Check if any points on 1 lies within 2
    // 2. Check if any transversal lines on the mesh of 1 intersects with 2

    // Calculate needed information
    Coords cone_bottom_point = coords_add(center_cone,coords_scalar_mult(direction_cone,-0.5*height_cone));
    Coords cone_top_point = coords_add(center_cone,coords_scalar_mult(direction_cone,0.5*height_cone));



    // Create two circles for both geometries
    int resoultuion = 300;



    struct pointer_to_1d_coords_list cone_points = geometry_cone->shell_points(geometry_cone,resoultuion);
    struct pointer_to_1d_coords_list box_points = geometry_box->shell_points(geometry_box,resoultuion);

    //points_on_circle(cone_1_top,cone_top_point,direction_cone,radius_top_cone,resoultuion);
    //points_on_circle(cone_1_bottom,cone_bottom_point,direction_cone,radius_bottom_cone,resoultuion);


    //printf("\nTEST\n");
    int i;
    // Test cone points inside box

    for (i = 0 ; i < cone_points.num_elements ; i++){
        
        if (r_within_box_advanced(cone_points.elements[i],geometry_box) == 1){
            //printf("\nOne point on cone is inside box\n");
            return 1;
        }
    }

    // Test box points inside cone
    for (i = 0 ; i < box_points.num_elements ; i++){
        
        if (r_within_cone(box_points.elements[i],geometry_cone) == 1){
            //printf("\nOne point on box is inside cone\n");
            return 1;
        }
    }


    // Test 1 within 2


    // // Test if there is any intersection (intersection function or eqation?)


    // // Add more points
    // This is an implementation of brute force. Maybe do this with a calculated function?
    int circ_resolution = 50;  // how many lines will the be checked for
    int height_resolution = 150;

    double length_of_cone_side = sqrt(pow(radius_top_cone-radius_bottom_cone,2)+pow(height_cone,2));
    double length_of_box_side = z_depth;

    double slope_1 = (radius_top_cone-radius_bottom_cone)/height_cone;

    double local_radius;

    Coords cone_direction = geometry_cone->geometry_parameters.p_cone_storage->direction_vector;
    //Coords box_direction = geometry_box->geometry_parameters.p_box_storage->direction_vector;

    //printf("\nlength_of_cone_side = %f\n",length_of_cone_side);

    Coords circ_points[50];
    double circ_offset;
    Coords circ_center;

    Coords square_points[8];
    double square_offset;
    // PW FIXME: square_center needs init - probably no to 0, but certainly not to "random stuff on memory"
    Coords square_center=coords_set(0,0,0);
    Coords box_end_point = coords_sub(coords_set(0,0,-z_depth/2),square_center);

    int j;

    for (i = 0 ; i < height_resolution ; i++){
        // Calculate circ offset
        //circ_offset = i * length_of_cone_side / height_resolution; // Possible bug
        circ_offset = i * height_cone / height_resolution; // Possible bug

        // Calculate middle point
        circ_center = coords_add(cone_bottom_point,coords_set(cone_direction.x * circ_offset,cone_direction.y * circ_offset,cone_direction.z * circ_offset));

        // Calculate radius
        local_radius = circ_offset * slope_1 + radius_bottom_cone;

        // Make points on circle
        //printf("points on circle: circ_center = [%f,%f,%f] , cone_direction = [%f,%f,%f] , local_radius = %f , circ_resolution = %i",circ_center.x,circ_center.y,circ_center.z,cone_direction.x,cone_direction.y,cone_direction.z,local_radius,circ_resolution);
        points_on_circle(circ_points,circ_center,cone_direction,local_radius,circ_resolution);

        // Test if any points lies within geomtry 2
        for (j = 0 ; j < circ_resolution; j++){
            //printf("\ntested if point [%i] [%f,%f,%f] is inside",j,circ_points[j].x,circ_points[j].y,circ_points[j].z);
            if (r_within_box_advanced(circ_points[j],geometry_box) == 1){
                //printf("\nOne point on cone 1 is inside cone 2\n");
                return 1;
            }
        }
    }

    double box_offset;

    for (i = 0 ; i < height_resolution ; i++){
        // Calculate circ offset
        box_offset = i * length_of_box_side / height_resolution;

        // Calculate middle point
        square_center = coords_add(box_end_point,coords_set(z_vector.x * box_offset,z_vector.y * box_offset,z_vector.z * box_offset));

        // Calculate radius

        // Make points on square
        square_points[0]=coords_add(square_center,coords_set(x_width1/2,0,0)); // A point on the side of the box
        square_points[1]=coords_add(square_points[0],coords_set(0,y_height1/2,0)); // Corner
        square_points[2]=coords_add(square_points[0],coords_set(0,-y_height1/2,0)); // Corner
        square_points[3]=coords_add(square_center,coords_set(-x_width1/2,0,0)); // A point on the side of the box
        square_points[4]=coords_add(square_points[3],coords_set(0,y_height1/2,0)); // Corner
        square_points[5]=coords_add(square_points[3],coords_set(0,-y_height1/2,0)); // Corner
        
        square_points[6]=coords_add(square_center,coords_set(0,y_height1/2,0)); // A point on the side of
        square_points[7]=coords_add(square_center,coords_set(0,-y_height1/2,0)); // A point on the side of

        // Test if any points lies within geomtry 2
        for (j = 0 ; j < 3; j++){
            
            if (r_within_cone(square_points[j],geometry_cone) == 1){
                //printf("\nOne point on cone 2 is inside cone 1\n");
                return 1;
            }
        }
    }
    
    return 0;

};

int box_overlaps_cone(struct geometry_struct *geometry_box,struct geometry_struct *geometry_cone) {
  // This problem is symetrical.
  return cone_overlaps_box(geometry_cone,geometry_box);
}

int mesh_overlaps_box(struct geometry_struct *geometry1, struct geometry_struct *geometry2){
   return mesh_overlaps_mesh(geometry1, geometry2);
}
int mesh_overlaps_cone(struct geometry_struct *geometry1, struct geometry_struct *geometry2){
   return mesh_overlaps_mesh(geometry1, geometry2);
}
int mesh_overlaps_sphere(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
   return mesh_overlaps_mesh(geometry1, geometry2);
};
int mesh_overlaps_cylinder(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
   return mesh_overlaps_mesh(geometry1, geometry2);
};
int box_overlaps_mesh(struct geometry_struct *geometry1, struct geometry_struct *geometry2){
   return mesh_overlaps_mesh(geometry1, geometry2);
}
int cone_overlaps_mesh(struct geometry_struct *geometry1, struct geometry_struct *geometry2){
   return mesh_overlaps_mesh(geometry1, geometry2);
}
int sphere_overlaps_mesh(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
   return mesh_overlaps_mesh(geometry1, geometry2);
};
int cylinder_overlaps_mesh(struct geometry_struct *geometry1,struct geometry_struct *geometry2) {
   return mesh_overlaps_mesh(geometry1, geometry2);
};
// -------------    Within functions for two different geometries ---------------------------------

double dist_from_point_to_plane(Coords point,Coords plane_p1, Coords plane_p2, Coords plane_p3) {

  /*
  printf("Dist from point to plane stuff ---- \n");
  print_position(point,"point");
  print_position(plane_p1,"plane_p1");
  print_position(plane_p2,"plane_p2");
  print_position(plane_p3,"plane_p3");
  */
  // transform three points into normal vector
  Coords vector_1 = coords_sub(plane_p2,plane_p1);
  Coords vector_2 = coords_sub(plane_p3,plane_p1);
  
  Coords normal_vector;
  
  vec_prod(normal_vector.x,normal_vector.y,normal_vector.z,vector_1.x,vector_1.y,vector_1.z,vector_2.x,vector_2.y,vector_2.z);
  
  double denominator = length_of_position_vector(normal_vector);
  
  normal_vector = coords_scalar_mult(normal_vector,1.0/denominator);

  //print_position(normal_vector,"normal vector in dist from point to plane");
  
  Coords diff = coords_sub(point,plane_p1);
  
  return fabs(scalar_prod(normal_vector.x,normal_vector.y,normal_vector.z,diff.x,diff.y,diff.z));
};

int box_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Is geometry child inside geometry parent?
    // For box child to be inside of cylinder parent, all corners of box child must be inside of box parent.
    
    // Generate coordinates of corners of the box
    Coords corner_points[8];
    box_corners_global_frame(corner_points,geometry_child);
    
    // Check earch corner seperatly
    int iterate;
    for (iterate=0;iterate<8;iterate++) {
        if (geometry_parent->within_function(corner_points[iterate],geometry_parent) == 0) {
            return 0; // If a corner is outside, box child is not within cylinder parent
        }
    }
    return 1; // If no corner was outside, the box is inside the cylinder
};

int cylinder_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Is geometry child inside geometry parent?
    // For box child to be inside of cylinder parent, all corners of box child must be inside of box parent.
    
    Coords cyl_direction = geometry_child->geometry_parameters.p_cylinder_storage->direction_vector;
    Coords center = geometry_child->center;
    double radius = geometry_child->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height = geometry_child->geometry_parameters.p_cylinder_storage->height;
    
    Coords cyl_top_point = coords_add(center,coords_scalar_mult(cyl_direction,0.5*height));
    Coords cyl_bottom_point = coords_add(center,coords_scalar_mult(cyl_direction,-0.5*height));
    
    // quick escape: if end points of cylinder not in box, return 0
    if (geometry_parent->within_function(cyl_top_point,geometry_parent) == 0) return 0;
    if (geometry_parent->within_function(cyl_bottom_point,geometry_parent) == 0) return 0;
    
    // Generate 30 points on the circle describing the top of the cylinder
    Coords *circle_point_array;
    int number_of_points = 30;
    circle_point_array = malloc(number_of_points * sizeof(Coords));
    if (!circle_point_array) {
      fprintf(stderr,"Failure allocating list in Union function cylinder_within_box - Exit!\n");
      exit(EXIT_FAILURE);
    }
    points_on_circle(circle_point_array,cyl_top_point,cyl_direction,radius,number_of_points);
    
    // Check parts of cylinder top seperatly
    int iterate;
    for (iterate=0;iterate<number_of_points;iterate++) {
        if (geometry_parent->within_function(circle_point_array[iterate],geometry_parent) == 0) {
            return 0; // If part of the cylinder is outside the box, the cylinder is not inside the box
        }
    }
    
    // Check parts of cylinder bottom seperatly
    points_on_circle(circle_point_array,cyl_bottom_point,cyl_direction,radius,number_of_points);
    for (iterate=0;iterate<number_of_points;iterate++) {
        if (geometry_parent->within_function(circle_point_array[iterate],geometry_parent) == 0) {
            return 0; // If part of the cylinder is outside the box, the cylinder is not inside the box
        }
    }
    
    free(circle_point_array);
    
    return 1; // If no part of the cylinders end caps was outside, the cylinder is inside box 1
};

int cylinder_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Is a cylinder within a sphere?
    
    double cyl_radius = geometry_child->geometry_parameters.p_cylinder_storage->cyl_radius;
    double cyl_height = geometry_child->geometry_parameters.p_cylinder_storage->height;
    double sph_radius = geometry_parent->geometry_parameters.p_sphere_storage->sph_radius;
    
    // Quick checks to avoid overhead from A_within_B
    // Is the height of the cylinder larger than diameter of the sphere?
    if (cyl_height > 2.0*sph_radius) return 0;
    
    // Is the radius of the cylidner larger than the radius of the sphere?
    if (cyl_radius > sph_radius) return 0;
    
    // Is the center of the cylinder so far from the center of the sphere that it cant fit?
    double distance = distance_between(geometry_child->center,geometry_parent->center);
    if (0.5*cyl_height > cyl_radius) {
        if (sqrt(distance*distance + 0.25*cyl_height*cyl_height) > sph_radius)
            return 0;
    } else {
        if (sqrt(distance*distance + cyl_radius*cyl_radius) > sph_radius)
            return 0;
    }
    
    // Reasonable to brute force solution here
    return A_within_B(geometry_child,geometry_parent,(int) 400); // 200 points on each end cap
};

int sphere_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Is a sphere (child) within a cylinder (parent)?
    
    // If the center is not inside, one can exit fast
    Coords sph_center = geometry_child->center;
    if (geometry_parent->within_function(sph_center,geometry_parent) == 0) return 0;
    
    // Generate cylinder with height = height - r_s and r_c = r_c - 2*r_s and check if point is within.
    
    // Done by modifying parent cylinder.
    double original_radius = geometry_parent->geometry_parameters.p_cylinder_storage->cyl_radius;
    double original_height = geometry_parent->geometry_parameters.p_cylinder_storage->height;
    
    // Need sphere
    double sph_radius = geometry_child->geometry_parameters.p_sphere_storage->sph_radius;
    
    if (original_radius - sph_radius > 0 && original_height - 2.0*sph_radius > 0) {
      geometry_parent->geometry_parameters.p_cylinder_storage->cyl_radius = original_radius - sph_radius;
      geometry_parent->geometry_parameters.p_cylinder_storage->height = original_height - 2.0*sph_radius;
    } else return 0;
    
    int return_value = geometry_parent->within_function(sph_center,geometry_parent);
    
    // Reset the cylinder to it's original values (important not to return before)
    geometry_parent->geometry_parameters.p_cylinder_storage->cyl_radius = original_radius;
    geometry_parent->geometry_parameters.p_cylinder_storage->height = original_height;
    
    return return_value;
};

int box_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // If all 8 corners of the box are inside the sphere, the entire box is inside
    
    return A_within_B(geometry_child,geometry_parent,8);
};

int sphere_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Distance from any of the box sides must be greater than radius, and the center inside.
    
    // debug test, use A_within_B
    //return A_within_B(geometry_child,geometry_parent,100*100);
    
    // If the center is not inside, one can exit fast
    Coords sph_center = geometry_child->center;
    if (geometry_parent->within_function(sph_center,geometry_parent) == 0) {
      //printf("sphere not child of box because it's center is not in the box \n");
      return 0;
    }
    
    double radius = geometry_child->geometry_parameters.p_sphere_storage->sph_radius;
    
    // 6 planes
    // +z -z easy as are parallel and simple in the box's coordinate system
    Coords coordinates = coords_sub(sph_center,geometry_parent->center);
    
    // Rotate the position around the center of the box
    Coords rotated_coordinates;
    rotated_coordinates = rot_apply(geometry_parent->transpose_rotation_matrix,coordinates);
    
    double depth = geometry_parent->geometry_parameters.p_box_storage->z_depth;
    if (rotated_coordinates.z < -0.5*depth+radius || rotated_coordinates.z > 0.5*depth-radius) {
      //printf("sphere not child of box because it's center to close to z plane \n");
      return 0;
    }
    
    Coords corner_ps[8];
    box_corners_global_frame(corner_ps,geometry_parent);
    
    // The first 4 points are in the -z plane, the last 4 in the +z plane.
    
    // In the -z plane, 0 has neighbors 1 and 3, in the opposite 4
    // In the -z plane, 2 has neighbors 1 and 3, in the opposite 6
    
    // Then these are the four necessary calls for the two plans described by each group.
    double debug_dist;
    if ((debug_dist = dist_from_point_to_plane(sph_center,corner_ps[0],corner_ps[4],corner_ps[1])) < radius ) {
      //printf("sphere not child of box because it's center too close to plane 1, as distance was %f\n",debug_dist);
      return 0;
    }
    if ((debug_dist = dist_from_point_to_plane(sph_center,corner_ps[0],corner_ps[4],corner_ps[3])) < radius ) {
      //printf("sphere not child of box because it's center too close to plane 2, as distance was %f\n",debug_dist);
      return 0;
    }
    if ((debug_dist = dist_from_point_to_plane(sph_center,corner_ps[2],corner_ps[6],corner_ps[1])) < radius ) {
      //printf("sphere not child of box because it's center too close to plane 3, as distance was %f\n",debug_dist);
      return 0;
    }
    if ((debug_dist = dist_from_point_to_plane(sph_center,corner_ps[2],corner_ps[6],corner_ps[3])) < radius ) {
      //printf("sphere not child of box because it's center too close to plane 4, as distance was %f\n",debug_dist);
      return 0;
    }
    
    return 1; // If the cylinder center is inside, and more than radius away from all walls, it is inside
};

int cone_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the sphere, 0 otherwise
    // Brute force place holder
    return A_within_B(geometry_child,geometry_parent,(int) 300); // 150 points on each end cap
};

int cone_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the cylinder, 0 otherwise
    // Brute force place holder
    //return A_within_B(geometry_child,geometry_parent,(int) 60); // 30 points on each end cap
    // This now works for the simple case where the two directions are parallel. Otherwise it uses A within B.

    // Unpack parameters
    double radius1 = geometry_parent->geometry_parameters.p_cylinder_storage->cyl_radius;
    double height1 = geometry_parent->geometry_parameters.p_cylinder_storage->height;
    double radius2_top = geometry_child->geometry_parameters.p_cone_storage->cone_radius_top;
    double radius2_bottom = geometry_child->geometry_parameters.p_cone_storage->cone_radius_bottom;
    double height2 = geometry_child->geometry_parameters.p_cone_storage->height;
    
    int verbal = 0;
    // Generate unit direction vector along center axis of cylinders
    
    // Start with vector that points along the cylinder in the simple frame, and rotate to global
    Coords simple_vector = coords_set(0,1,0);
    Coords vector1,vector2;
    if (verbal == 1) printf("Cords start_vector = (%f,%f,%f)\n",simple_vector.x,simple_vector.y,simple_vector.z);

    // Rotate the position of the ray around the center of the cylinder
    vector1 = rot_apply(geometry_parent->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector1.z);
    // Rotate the position of the ray around the center of the cylinder
    vector2 = rot_apply(geometry_child->rotation_matrix,simple_vector);
    if (verbal == 1) printf("Cords vector2 = (%f,%f,%f)\n",vector2.x,vector2.y,vector2.z);
    
    // if vector1 and vector2 are parallel, the problem is simple, but if not complicated
    double cross_product1[3] = {0,0,0};
    // printf("%f\n",cross_product1[0]);
    // vec prod(&ax,&ay,&az,bx,by,bz, cx,cy,cz)
    vec_prod(cross_product1[0],cross_product1[1],cross_product1[2],vector1.x,vector1.y,vector1.z,vector2.x,vector2.y,vector2.z);
    // I get an error taking the adress of cross_product1[0], &cross_product1[0]. Took the pointer adresses instead. Works fine.
    if (verbal == 1) printf("cross_product = (%f,%f,%f)\n",cross_product1[0],cross_product1[1],cross_product1[2]);
    double cross_product_length = length_of_3vector(cross_product1);
    
    if (cross_product_length == 0) {
        // The cylinders are parallel.
        int seperated = 0;
        double delta[3];
        delta[0] = geometry_parent->center.x - geometry_child->center.x;
        delta[1] = geometry_parent->center.y - geometry_child->center.y;
        delta[2] = geometry_parent->center.z - geometry_child->center.z;

        // Test for separation by height
        // if (h0Div2 + h1Div2 − |Dot(W0, Delta )| < 0) seperated = 1;
        
        if (verbal == 1) printf("vector1 = (%f,%f,%f)\n",vector1.x,vector1.y,vector2.z);
        if (verbal == 1) printf("delta1 = (%f,%f,%f)\n",delta[0],delta[1],delta[2]);
        double scalar_prod1 = scalar_prod(vector1.x,vector1.y,vector1.z,delta[0],delta[1],delta[2]);
        if (verbal == 1) printf("scalar product = %f \n",scalar_prod1);
        if (verbal == 1) printf("height 1 = %f, height 2 = %f \n",height1,height2);
        
        int inside = 1;
        
        if (height1*0.5 < height2*0.5 + fabs(scalar_prod1)) {
                if (verbal == 1) printf("Cylinder sticks out height wise \n");
                inside = 0;
                }
    
        // Test for separation radially
        // if (rSum − |Delta − Dot(W0,Delta)∗W0| < 0) seperated = 1;
        double vector_between_cyl_axis[3];
        vector_between_cyl_axis[0] = delta[0] - scalar_prod1*vector1.x;
        vector_between_cyl_axis[1] = delta[1] - scalar_prod1*vector1.y;
        vector_between_cyl_axis[2] = delta[2] - scalar_prod1*vector1.z;
        if (verbal == 1) printf("vector_between = (%f,%f,%f)\n",vector_between_cyl_axis[0],vector_between_cyl_axis[1],vector_between_cyl_axis[2]);
        if (verbal == 1) printf("length of vector between = %f\n",length_of_3vector(vector_between_cyl_axis));
        if (verbal == 1) printf("radius1 = %f , radius2_top=%f , radius2_bottom=%f\n",radius1,radius2_top,radius2_bottom);


        if (radius1 < fmax(radius2_top,radius2_bottom) + length_of_3vector(vector_between_cyl_axis)) {
                if (verbal == 1) printf("Cylinder sticks out radially \n");
                inside = 0;
                }
        
        if (inside == 0) return 0;
        else return 1;
        
    } else {
        
    // Make shell points and check if they are inside
    return A_within_B(geometry_child,geometry_parent,(int) 200); // 100 points on each end cap

    }

};

int cone_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cone is completely within the box, 0 otherwise
    // Brute force place holder
    return A_within_B(geometry_child,geometry_parent,(int) 300); // 150 points on each end cap
};

int sphere_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the sphere is completely within the cone, 0 otherwise
    // Brute force place holder
    return A_within_B(geometry_child,geometry_parent,(int) 300); // 150 points on each end cap
};

int cylinder_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the cylinder is completely within the cone, 0 otherwise
    // Brute force place holder
    return A_within_B(geometry_child,geometry_parent,(int) 300); // 150 points on each end cap
};

int box_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent) {
    // Function returns 1 if the box is completely within the cone, 0 otherwise
    // Brute force place holder
    return A_within_B(geometry_child,geometry_parent,(int) 300); // 150 points on each end cap
};


// Flexible intersection function
int intersect_function(double *t, double *nx, double *ny, double *nz, int *surface_index, int *num_solutions, double *r, double *v, struct geometry_struct *geometry) {
    int output = 0;
    switch(geometry->eShape) {
        case box:
            if (geometry->geometry_parameters.p_box_storage->is_rectangle == 1) {
                output = sample_box_intersect_simple(t, nx, ny, nz, surface_index, num_solutions, r, v, geometry);
			} else {
                output = sample_box_intersect_advanced(t, nx, ny, nz, surface_index, num_solutions, r, v, geometry);
			}
            break;
        case sphere:
            output = sample_sphere_intersect(t, nx, ny, nz, surface_index, num_solutions, r, v, geometry);
            break;
        case cylinder:
            output = sample_cylinder_intersect(t, nx, ny, nz, surface_index, num_solutions, r, v, geometry);
            break;
        case cone:
            output = sample_cone_intersect(t, nx, ny, nz, surface_index, num_solutions, r, v, geometry);
            break;
        #ifndef OPENACC
        case mesh:
            output = sample_mesh_intersect(t, nx, ny, nz, surface_index, num_solutions, r, v, geometry);
            break;
        #endif
        default:
            printf("Intersection function: No matching geometry found!");
            break;
    }
	
    return output;
};

// Flexible within function
int r_within_function(Coords pos,struct geometry_struct *geometry) {
    int output = 0;
    switch(geometry->eShape) {
        case box:
            if (geometry->geometry_parameters.p_box_storage->is_rectangle == 1)
                output = r_within_box_simple(pos, geometry);
            else
                output = r_within_box_advanced(pos, geometry);
            break;
        case sphere:
            output = r_within_sphere(pos, geometry);
            break;
        case cylinder:
            output = r_within_cylinder(pos, geometry);
            break;
        case cone:
            output = r_within_cone(pos, geometry);
            break;
        #ifndef OPENACC
        case mesh:
            output = r_within_mesh(pos, geometry);
            break;
        #endif
        case surroundings:
            output = 1;
            break;
        default:
            printf("Within function: No matching geometry found!");
            break;
    }
    
    return output;
};


// -------------    List generator functions   --------------------------------------------------


int within_which_volume(Coords pos, struct pointer_to_1d_int_list input_list, struct pointer_to_1d_int_list destinations_list, struct Volume_struct **Volumes, struct pointer_to_1d_int_list *mask_status_list, int number_of_volumes, int *volume_logic_copy, int *ListA, int *ListB) {
    // This function identifies in which of the volumes of the input list the position pos lies in.
    // pos: position for which the current volume should be found for
    // input list: list of potential volumes, reduced to the ones without parents (their children will be checked)
    // OLD VERSION: volume logic: A logic list of allowed volumes for lookup, volume 1 3 and 5 in a case of 10 volumes would be [0 1 0 1 0 1 0 0 0 0]
    // destinations_list: list of allowed destinations (the original destinations list)
    // Volumes: Main volumes array
    // volume_logic_copy: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    // ListA: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    // ListB: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    
    // Algorithm description
    // Check all of input list for pos being within them, those that have it within them are:
    //      checked against the current highest priority
    //          if higher, is the new highest priority and the new pick for volume
    //      have all their direct children added to the next list to be checked (but any volume can only be added to that list once)
    // Once a run have been made where the next list to check is empty, the answer for new pick for volume is taken.
    
    // Mask update:
    // Algorithm description
    // Check all of input list for pos being within them, those that have it within them are:
    //      checked against the current highest priority and mask status
    //          if higher, this is the new highest priority and the new pick for volume
    //      have all their direct children added to the next list to be checked (but any volume can only be added to that list once)
    // Once a run have been made where the next list to check is empty, the answer for new pick for volume is taken.
    
    // The advantage of the method is that potentially large numbers of children are skipped when their parents do not contain the position.
    // The overhead cost is low, as all the lists are prealocated.
    // Should be checked which of the two implementations is faster, as this is much more complicated than simply checking all possibilities.
    // No within_function call should be made twice, as the same volume number will not be checked twice because of the properties of the direct_children list and the volume_logic that removes duplicates on each level.
    
    
    // This function uses too much memory, the memory required for the volume logic list is n_volumes^2 ints, or for a MACS monochromator 127000 ints.
    // Instead the original destinations list must be used, and a function for quick lookup in a (sorted) destinations list made.
    // Still need a list of n_volumes length for control to avoid adding the same volume to the list twice.
    
    
    int ListA_length=0,ListB_length=0;
    int done = 0;
    int i,direct_children_index;
    int *temp_pointer;
    double max_priority=-1000000;
    int residing_volume=0; // 0 can be removed from the input list if default is 0
    int this_mask_status,mask_index,mask_global_index;
    
    // volume_logic_copy
    //for (i=0;i<volume_logic.num_elements;i++) volume_logic_copy[i] = volume_logic.elements[i];
    
    // low memory version of volume_logic_copy
    for (i=0;i<number_of_volumes;i++) volume_logic_copy[i] = 0;
    for (i=0;i<destinations_list.num_elements;i++) volume_logic_copy[destinations_list.elements[i]] = 1;
    printf("within_which_volume debug %i\n",input_list.num_elements);

    // Does one loop through the algorithm first to set up ListA instead of copying it from input_list, which takes time
    for (i=0;i<input_list.num_elements;i++) {
            if (Volumes[input_list.elements[i]]->geometry.within_function(pos,&Volumes[input_list.elements[i]]->geometry) == 1) {
                printf("The position is inside of volume %d\n",input_list.elements[i]);
                if (Volumes[input_list.elements[i]]->geometry.is_masked_volume == 1) {
                    // if the volume is masked, I need to know if it can be a destination volume from the mask_status_list.
                    // if the masked volume is in ANY mode,
                    this_mask_status=1;
                    //print_1d_int_list(*mask_status_list,"mask status list from within_which_volume");
                    for (mask_index=0;mask_index<Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.num_elements;mask_index++) {
                        //printf("Looking at the mask with global index %d \n",Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]);
                        if (mask_status_list->elements[Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]] == 1) {
                            if (Volumes[input_list.elements[i]]->geometry.mask_mode == 2) { // ANY (break if any one in)
                                this_mask_status=1;
                                break;
                            }
                            //printf("global index %d had mask status = 1 \n",Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]);
                        } else {
                          //printf("global index %d had mask status = 0 \n",Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]);
                          this_mask_status = 0;
                          if(Volumes[input_list.elements[i]]->geometry.mask_mode == 1) break; // ALL (break if any one out)
                        }
                    }
                    //printf("This volume is masked, and the mask status is %d\n",this_mask_status);
                } else this_mask_status = 1; // if the volume is not masked
            
                if (Volumes[input_list.elements[i]]->geometry.priority_value > max_priority && this_mask_status == 1) {
                    max_priority = Volumes[input_list.elements[i]]->geometry.priority_value;
                    residing_volume = input_list.elements[i];
                    //printf("residing volume set to %d\n",residing_volume);
                }
                    for (direct_children_index = 0;direct_children_index < Volumes[input_list.elements[i]]->geometry.direct_children.num_elements;direct_children_index++) {
                        if (volume_logic_copy[Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index]] == 1) {
                            ListA[ListA_length++] = Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index];
                            volume_logic_copy[Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index]] = 0;
                        }
                    }
            }
    }
    //printf("Completed first loop, continued in while loop\n");
    if (ListA_length > 0) {
        while (done == 0) {
            for (i=0;i<ListA_length;i++) {
              //printf("checking element number %d of list A which is volume number %d \n",i,ListA[i]);
                if (Volumes[ListA[i]]->geometry.within_function(pos,&Volumes[ListA[i]]->geometry) == 1) {
                  //printf("ray was inside this volume \n");
                    if (Volumes[ListA[i]]->geometry.is_masked_volume == 1) {
                      //printf("it is a mask and thus need check of mask status \n");
                        // if the volume is masked, I need to know if it can be a destination volume from the mask_status_list.
                        // if the masked volume is in ANY mode,
                        this_mask_status=1;
                        for (mask_index=0;mask_index<Volumes[ListA[i]]->geometry.masked_by_mask_index_list.num_elements;mask_index++) {
                            if (mask_status_list->elements[Volumes[ListA[i]]->geometry.masked_by_mask_index_list.elements[mask_index]] == 1) {
                                if (Volumes[ListA[i]]->geometry.mask_mode == 2) { // ANY (break if any one in)
                                    this_mask_status=1;
                                    break;
                                }
                            } else {
                              this_mask_status = 0;
                              if(Volumes[ListA[i]]->geometry.mask_mode == 1) break; // ALL (break if any one out)
                            }
                        }
                    } else this_mask_status = 1;
                    //printf("the mask status is %d \n",this_mask_status);
                    if (Volumes[ListA[i]]->geometry.priority_value > max_priority && this_mask_status == 1) {
                        max_priority = Volumes[ListA[i]]->geometry.priority_value;
                        residing_volume = ListA[i];
                    }
                    //printf("Adding direct children to list B \n");
                        for (direct_children_index = 0;direct_children_index < Volumes[ListA[i]]->geometry.direct_children.num_elements;direct_children_index++) {
                            //printf("Checking direct_child number %d which is %d \n",direct_children_index,Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]);
                            if (volume_logic_copy[Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]] == 1) {
                                //printf("It's volume_logic was 1, and it is thus added to listB with index %d \n",ListB_length);
                                ListB[ListB_length++] = Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index];
                                volume_logic_copy[Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]] = 0;
                            }
                        }
                    //printf("List B is now: ");
                    //for (direct_children_index=0;direct_children_index<ListB_length;direct_children_index++) printf("%d ",ListB[direct_children_index]);
                    //printf("\n");
                    
                    
                }
            }
            if (ListB_length==0) done = 1;
            else {
                
                for (i=0;i<ListB_length;i++) ListA[i] = ListB[i];
                ListA_length = ListB_length;
                ListB_length = 0;
                
                /*
                // Could do this with pointers instead to avoid this for loop (and needless copy)
                // This code block fails on the cluster in rare circumstances
                ListA = temp_pointer;
                ListA = ListB;
                ListB = temp_pointer;
                ListA_length=ListB_length;
                ListB_length=0;
                */
            }
        }
    }
    //printf("residing volume returned %d\n",residing_volume);
    return residing_volume;
};

int within_which_volume_GPU(Coords pos, struct pointer_to_1d_int_list input_list, struct pointer_to_1d_int_list destinations_list, struct Volume_struct **Volumes, struct pointer_to_1d_int_list *mask_status_list, int number_of_volumes, int *volume_logic_copy, int *ListA, int *ListB) {
    // This function identifies in which of the volumes of the input list the position pos lies in.
    // pos: position for which the current volume should be found for
    // input list: list of potential volumes, reduced to the ones without parents (their children will be checked)
    // OLD VERSION: volume logic: A logic list of allowed volumes for lookup, volume 1 3 and 5 in a case of 10 volumes would be [0 1 0 1 0 1 0 0 0 0]
    // destinations_list: list of allowed destinations (the original destinations list)
    // Volumes: Main volumes array
    // volume_logic_copy: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    // ListA: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    // ListB: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    
    // Algorithm description
    // Check all of input list for pos being within them, those that have it within them are:
    //      checked against the current highest priority
    //          if higher, is the new highest priority and the new pick for volume
    //      have all their direct children added to the next list to be checked (but any volume can only be added to that list once)
    // Once a run have been made where the next list to check is empty, the answer for new pick for volume is taken.
    
    // Mask update:
    // Algorithm description
    // Check all of input list for pos being within them, those that have it within them are:
    //      checked against the current highest priority and mask status
    //          if higher, this is the new highest priority and the new pick for volume
    //      have all their direct children added to the next list to be checked (but any volume can only be added to that list once)
    // Once a run have been made where the next list to check is empty, the answer for new pick for volume is taken.
    
    // The advantage of the method is that potentially large numbers of children are skipped when their parents do not contain the position.
    // The overhead cost is low, as all the lists are prealocated.
    // Should be checked which of the two implementations is faster, as this is much more complicated than simply checking all possibilities.
    // No within_function call should be made twice, as the same volume number will not be checked twice because of the properties of the direct_children list and the volume_logic that removes duplicates on each level.
    
    
    // This function uses too much memory, the memory required for the volume logic list is n_volumes^2 ints, or for a MACS monochromator 127000 ints.
    // Instead the original destinations list must be used, and a function for quick lookup in a (sorted) destinations list made.
    // Still need a list of n_volumes length for control to avoid adding the same volume to the list twice.
    
    
    int ListA_length=0,ListB_length=0;
    int done = 0;
    int i,direct_children_index;
    int *temp_pointer;
    double max_priority=-1000000;
    int residing_volume=0; // 0 can be removed from the input list if default is 0
    int this_mask_status,mask_index,mask_global_index;
    
    // volume_logic_copy
    //for (i=0;i<volume_logic.num_elements;i++) volume_logic_copy[i] = volume_logic.elements[i];
    
    // low memory version of volume_logic_copy
    for (i=0;i<number_of_volumes;i++) volume_logic_copy[i] = 0;
    for (i=0;i<destinations_list.num_elements;i++) volume_logic_copy[destinations_list.elements[i]] = 1;
    //printf("within_which_volume debug\n");

    // Does one loop through the algorithm first to set up ListA instead of copying it from input_list, which takes time
    for (i=0;i<input_list.num_elements;i++) {
            if (r_within_function(pos, &Volumes[input_list.elements[i]]->geometry) == 1) {
                //printf("The position is inside of volume %d\n",input_list.elements[i]);
                if (Volumes[input_list.elements[i]]->geometry.is_masked_volume == 1) {
                    // if the volume is masked, I need to know if it can be a destination volume from the mask_status_list.
                    // if the masked volume is in ANY mode,
                    this_mask_status=1;
                    //print_1d_int_list(*mask_status_list,"mask status list from within_which_volume");
                    for (mask_index=0;mask_index<Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.num_elements;mask_index++) {
                        //printf("Looking at the mask with global index %d \n",Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]);
                        if (mask_status_list->elements[Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]] == 1) {
                            if (Volumes[input_list.elements[i]]->geometry.mask_mode == 2) { // ANY (break if any one in)
                                this_mask_status=1;
                                break;
                            }
                            //printf("global index %d had mask status = 1 \n",Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]);
                        } else {
                          //printf("global index %d had mask status = 0 \n",Volumes[input_list.elements[i]]->geometry.masked_by_mask_index_list.elements[mask_index]);
                          this_mask_status = 0;
                          if(Volumes[input_list.elements[i]]->geometry.mask_mode == 1) break; // ALL (break if any one out)
                        }
                    }
                    //printf("This volume is masked, and the mask status is %d\n",this_mask_status);
                } else this_mask_status = 1; // if the volume is not masked
            
                if (Volumes[input_list.elements[i]]->geometry.priority_value > max_priority && this_mask_status == 1) {
                    max_priority = Volumes[input_list.elements[i]]->geometry.priority_value;
                    residing_volume = input_list.elements[i];
                    //printf("residing volume set to %d\n",residing_volume);
                }
                    for (direct_children_index = 0;direct_children_index < Volumes[input_list.elements[i]]->geometry.direct_children.num_elements;direct_children_index++) {
                        if (volume_logic_copy[Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index]] == 1) {
                            ListA[ListA_length++] = Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index];
                            volume_logic_copy[Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index]] = 0;
                        }
                    }
            }
    }
    //printf("Completed first loop, continued in while loop\n");
    if (ListA_length > 0) {
        while (done == 0) {
            for (i=0;i<ListA_length;i++) {
              //printf("checking element number %d of list A which is volume number %d \n",i,ListA[i]);
                if (r_within_function(pos,&Volumes[ListA[i]]->geometry) == 1) {
                  //printf("ray was inside this volume \n");
                    if (Volumes[ListA[i]]->geometry.is_masked_volume == 1) {
                      //printf("it is a mask and thus need check of mask status \n");
                        // if the volume is masked, I need to know if it can be a destination volume from the mask_status_list.
                        // if the masked volume is in ANY mode,
                        this_mask_status=1;
                        for (mask_index=0;mask_index<Volumes[ListA[i]]->geometry.masked_by_mask_index_list.num_elements;mask_index++) {
                            if (mask_status_list->elements[Volumes[ListA[i]]->geometry.masked_by_mask_index_list.elements[mask_index]] == 1) {
                                if (Volumes[ListA[i]]->geometry.mask_mode == 2) { // ANY (break if any one in)
                                    this_mask_status=1;
                                    break;
                                }
                            } else {
                              this_mask_status = 0;
                              if(Volumes[ListA[i]]->geometry.mask_mode == 1) break; // ALL (break if any one out)
                            }
                        }
                    } else this_mask_status = 1;
                    //printf("the mask status is %d \n",this_mask_status);
                    if (Volumes[ListA[i]]->geometry.priority_value > max_priority && this_mask_status == 1) {
                        max_priority = Volumes[ListA[i]]->geometry.priority_value;
                        residing_volume = ListA[i];
                    }
                    //printf("Adding direct children to list B \n");
                        for (direct_children_index = 0;direct_children_index < Volumes[ListA[i]]->geometry.direct_children.num_elements;direct_children_index++) {
                            //printf("Checking direct_child number %d which is %d \n",direct_children_index,Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]);
                            if (volume_logic_copy[Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]] == 1) {
                                //printf("It's volume_logic was 1, and it is thus added to listB with index %d \n",ListB_length);
                                ListB[ListB_length++] = Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index];
                                volume_logic_copy[Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]] = 0;
                            }
                        }
                    //printf("List B is now: ");
                    //for (direct_children_index=0;direct_children_index<ListB_length;direct_children_index++) printf("%d ",ListB[direct_children_index]);
                    //printf("\n");
                    
                    
                }
            }
            if (ListB_length==0) done = 1;
            else {
                
                for (i=0;i<ListB_length;i++) ListA[i] = ListB[i];
                ListA_length = ListB_length;
                ListB_length = 0;
                
                /*
                // Could do this with pointers instead to avoid this for loop (and needless copy)
                // This code block fails on the cluster in rare circumstances
                ListA = temp_pointer;
                ListA = ListB;
                ListB = temp_pointer;
                ListA_length=ListB_length;
                ListB_length=0;
                */
            }
        }
    }
    //printf("residing volume returned %d\n",residing_volume);
    return residing_volume;
};




int within_which_volume_debug(Coords pos, struct pointer_to_1d_int_list input_list, struct pointer_to_1d_int_list volume_logic, struct Volume_struct **Volumes, int *volume_logic_copy, int *ListA, int *ListB) {
    // This function identifies in which of the volumes of the input list the position pos lies in.
    // pos: position for which the current volume should be found for
    // input list: list of potential volumes, reduced to the ones without parents (their children will be checked)
    // volume logic: A logic list of allowed volumes for lookup, volume 1 3 and 5 in a case of 10 volumes would be [0 1 0 1 0 1 0 0 0 0]
    // Volumes: Main volumes array
    // volume_logic_copy: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    // ListA: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    // ListB: A pointer to a integer array with at least length "number_of_volumes" (pre alocated for speed)
    
    // Algorithm description
    // Check all of input list for pos being within them, those that have it within them are:
    //      checked against the current highest priority
    //          if higher, is the new highest priority and the new pick for volume
    //      have all their direct children added to the next list to be checked (but any volume can only be added to that list once)
    // Once a run have been made where the next list to check is empty, the answer for new pick for volume is taken.
    
    // The advantage of the method is that potentially large numbers of children are skipped when their parents do not contain the position.
    // The overhead cost is low, as all the lists are prealocated.
    // Should be checked which of the two implementations is faster, as this is much more complicated than simply checking all possibilities.
    // No within_function call should be made twice, as the same volume number will not be checked twice because of the properties of the direct_children list and the volume_logic that removes duplicates on each level.
    
    
    int ListA_length=0,ListB_length=0;
    int done = 0;
    int i,direct_children_index;
    int *temp_pointer;
    double max_priority=-1000000;
    int residing_volume=0; // 0 can be removed from the input list if default is 0
    
    // volume_logic_copy
    for (i=0;i<volume_logic.num_elements;i++) volume_logic_copy[i] = volume_logic.elements[i];
    
    // Does one loop through the algorithm first to set up ListA instead of copying it from input_list, which takes time
    for (i=0;i<input_list.num_elements;i++) {
            if (Volumes[input_list.elements[i]]->geometry.within_function(pos,&Volumes[input_list.elements[i]]->geometry) == 1) {
                if (Volumes[input_list.elements[i]]->geometry.priority_value > max_priority) {
                    max_priority = Volumes[input_list.elements[i]]->geometry.priority_value;
                    residing_volume = input_list.elements[i];
                }
                    for (direct_children_index = 0;direct_children_index < Volumes[input_list.elements[i]]->geometry.direct_children.num_elements;direct_children_index++) {
                        if (volume_logic_copy[Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index]] == 1) {
                            ListA[ListA_length++] = Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index];
                            volume_logic_copy[Volumes[input_list.elements[i]]->geometry.direct_children.elements[direct_children_index]] = 0;
                        }
                    }
            }
    }
    if (ListA_length > 0) {
        while (done == 0) {
        
            printf("ListA = [");
            for (i=0;i<ListA_length;i++) {
                printf("%d,",ListA[i]);
            }
            printf("]\n");
            for (i=0;i<ListA_length;i++) {
                if (Volumes[ListA[i]]->geometry.within_function(pos,&Volumes[ListA[i]]->geometry) == 1) {
                    if (Volumes[ListA[i]]->geometry.priority_value > max_priority) {
                        max_priority = Volumes[ListA[i]]->geometry.priority_value;
                        residing_volume = ListA[i];
                    }
                    //if (ListA[i]!=0) {
                        for (direct_children_index = 0;direct_children_index < Volumes[ListA[i]]->geometry.direct_children.num_elements;direct_children_index++) {
                            if (volume_logic_copy[Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]] == 1) {
                                ListB[ListB_length++] = Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index];
                                volume_logic_copy[Volumes[ListA[i]]->geometry.direct_children.elements[direct_children_index]] = 0;
                            }
                        }
                    //}
                }
            }
            if (ListB_length==0) done = 1;
            else {
                temp_pointer = ListA;
                ListA = ListB;
                ListB = temp_pointer;
                ListA_length=ListB_length;
                ListB_length=0;
            }
        }
    }
    printf("Volume number %d had the highest priority of checked volumes\n",residing_volume);
    return residing_volume;
};

int inside_function(struct Volume_struct *parent_volume, struct Volume_struct *child_volume) {
    // Function that calls the correct within function depending on the shapes of the two volumes
    if (strcmp("sphere",parent_volume->geometry.shape) == 0 && strcmp("sphere",child_volume->geometry.shape) == 0) {
        if (sphere_within_sphere(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cylinder",parent_volume->geometry.shape) == 0 && strcmp("cylinder",child_volume->geometry.shape) == 0) {
        if (cylinder_within_cylinder(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("box",parent_volume->geometry.shape) == 0 && strcmp("box",child_volume->geometry.shape) == 0) {
        if (box_within_box(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cone",parent_volume->geometry.shape) == 0 && strcmp("cone",child_volume->geometry.shape) == 0) {
        if (cone_within_cone(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("mesh",parent_volume->geometry.shape) == 0 && strcmp("mesh",child_volume->geometry.shape) == 0) {
        if (mesh_within_mesh(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("box",parent_volume->geometry.shape) == 0 && strcmp("cylinder",child_volume->geometry.shape) == 0) {
        if (cylinder_within_box(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cylinder",parent_volume->geometry.shape) == 0 && strcmp("box",child_volume->geometry.shape) == 0) {
        if (box_within_cylinder(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("box",parent_volume->geometry.shape) == 0 && strcmp("sphere",child_volume->geometry.shape) == 0) {
        if (sphere_within_box(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("sphere",parent_volume->geometry.shape) == 0 && strcmp("box",child_volume->geometry.shape) == 0) {
        if (box_within_sphere(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("sphere",parent_volume->geometry.shape) == 0 && strcmp("cylinder",child_volume->geometry.shape) == 0) {
        if (cylinder_within_sphere(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cylinder",parent_volume->geometry.shape) == 0 && strcmp("sphere",child_volume->geometry.shape) == 0) {
        if (sphere_within_cylinder(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cone",parent_volume->geometry.shape) == 0 && strcmp("sphere",child_volume->geometry.shape) == 0) {
        if (sphere_within_cone(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("sphere",parent_volume->geometry.shape) == 0 && strcmp("cone",child_volume->geometry.shape) == 0) {
        if (cone_within_sphere(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cone",parent_volume->geometry.shape) == 0 && strcmp("cylinder",child_volume->geometry.shape) == 0) {
        if (cylinder_within_cone(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cylinder",parent_volume->geometry.shape) == 0 && strcmp("cone",child_volume->geometry.shape) == 0) {
        if (cone_within_cylinder(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cone",parent_volume->geometry.shape) == 0 && strcmp("box",child_volume->geometry.shape) == 0) {
        if (box_within_cone(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("box",parent_volume->geometry.shape) == 0 && strcmp("cone",child_volume->geometry.shape) == 0) {
        if (cone_within_box(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("mesh",parent_volume->geometry.shape) == 0 && strcmp("cylinder",child_volume->geometry.shape) == 0) {
        if (cylinder_within_mesh(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("mesh",parent_volume->geometry.shape) == 0 && strcmp("sphere",child_volume->geometry.shape) == 0) {
        if (sphere_within_mesh(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cone",parent_volume->geometry.shape) == 0 && strcmp("mesh",child_volume->geometry.shape) == 0) {
        if (mesh_within_cone(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("mesh",parent_volume->geometry.shape) == 0 && strcmp("cone",child_volume->geometry.shape) == 0) {
        if (cone_within_mesh(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("cylinder",parent_volume->geometry.shape) == 0 && strcmp("mesh",child_volume->geometry.shape) == 0) {
        if (cylinder_within_mesh(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("sphere",parent_volume->geometry.shape) == 0 && strcmp("mesh",child_volume->geometry.shape) == 0) {
        if (mesh_within_sphere(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("mesh",parent_volume->geometry.shape) == 0 && strcmp("box",child_volume->geometry.shape) == 0) {
        if (box_within_mesh(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else if (strcmp("box",parent_volume->geometry.shape) == 0 && strcmp("mesh",child_volume->geometry.shape) == 0) {
        if (mesh_within_box(&child_volume->geometry,&parent_volume->geometry)) return 1;
    }
    else {
        #ifndef OPENACC
        printf("Need within function for type: ");
        printf("%s",parent_volume->geometry.shape);
        printf(" and type: ");
        printf("%s",child_volume->geometry.shape);
        printf(".\n");
        printf("It is not yet supported to mix mesh geometries with the basic shapes, but several mesh geometries are allowed.\n");
        exit(EXIT_FAILURE);
	#endif
    }
    
    return 0;
};

void generate_children_lists(struct Volume_struct **Volumes, struct pointer_to_1d_int_list **true_children_lists, int number_of_volumes, int verbal) {
  // This function generates a list of children for each volume.
  // A volume m is a child of volume n, if the entire space ocupied by volume m is inside of the space ocupied by volume n
  // A volume m is a true child of volume n, if the entire space coupied by volume m after it's masks are applied is inside the volume ocupied by volume n after it's masks are applied
  
  
  MPI_MASTER(
  if (verbal) printf("\nGenerating children lists --------------------------- \n");
  )

  // Mask update: Creating a temporary list for each volume
  struct pointer_to_1d_int_list *temporary_children_lists;
  temporary_children_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list));
  if (!temporary_children_lists) {
    fprintf(stderr,"Failure allocating list in Union function generate_children_lists 1 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // The surrounding vacuum, volume 0, done outside of for loop.
  temporary_children_lists[0].num_elements = number_of_volumes - 1;
  temporary_children_lists[0].elements = malloc(temporary_children_lists[0].num_elements*sizeof(int));
  
  int parent;
  for (parent=1;parent<number_of_volumes;parent++) {
      temporary_children_lists[0].elements[parent-1] = parent;
  }

  // Hardcoding that every volume is a child of the surrounding vacuum
  Volumes[0]->geometry.children.num_elements = number_of_volumes-1;
  Volumes[0]->geometry.children.elements = malloc((number_of_volumes-1)*sizeof(int));
  if (!Volumes[0]->geometry.children.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_children_lists 2 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  true_children_lists[0] = malloc(sizeof(struct pointer_to_1d_int_list));
  if (!true_children_lists[0]) {
    fprintf(stderr,"Failure allocating list in Union function generate_children_lists 3 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  true_children_lists[0]->num_elements = number_of_volumes - 1;
  true_children_lists[0]->elements = malloc((number_of_volumes-1)*sizeof(int));
  if (!true_children_lists[0]->elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_children_lists 4 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  
  for (parent=1;parent<number_of_volumes;parent++) {
      Volumes[0]->geometry.children.elements[parent-1] = parent;
      true_children_lists[0]->elements[parent-1] = parent;
  }
  
  char string_output[128];
  MPI_MASTER(
  if (verbal) sprintf(string_output,"Children for Volume %d",0);
  if (verbal) print_1d_int_list(Volumes[0]->geometry.children,string_output);
  )
  
  
  // Generating the children lists for all other volumes using the appropriate geometry functions
  struct pointer_to_1d_int_list temp_list_local;
  temp_list_local.num_elements = number_of_volumes;
  temp_list_local.elements = malloc(number_of_volumes*sizeof(int));
  if (!temp_list_local.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_children_lists 5 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  
  struct pointer_to_1d_int_list true_temp_list_local;
  true_temp_list_local.num_elements = number_of_volumes;
  true_temp_list_local.elements = malloc(number_of_volumes*sizeof(int));
  if (!true_temp_list_local.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_children_lists 6 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  
  
  int child,used_elements,used_elements_true;
  for (parent=1;parent<number_of_volumes;parent++) {
      used_elements = 0;used_elements_true = 0;
      for (child=1;child<number_of_volumes;child++) {
        if (child != parent) {
          // Call inside_function that selects the proper within function for the two volumes
          if (1 == inside_function(Volumes[parent],Volumes[child])) {
            temp_list_local.elements[used_elements++] = child;
            true_temp_list_local.elements[used_elements_true++] = child;
          }
        } else true_temp_list_local.elements[used_elements_true++] = child; // Needed when children list takes mask into account
      }
      // Temp test
      allocate_list_from_temp(used_elements,temp_list_local,&Volumes[parent]->geometry.children);
      // Assing the children list to a temporary list as the masks have yet to be taken into account
      temporary_children_lists[parent].num_elements=0;
      allocate_list_from_temp(used_elements_true,true_temp_list_local,&temporary_children_lists[parent]);
      
      
      MPI_MASTER(
      if (verbal) sprintf(string_output,"Children for Volume %d (temporary_list)",parent);
      if (verbal) print_1d_int_list(temporary_children_lists[parent],string_output);
      )
      
      MPI_MASTER(
      if (verbal) sprintf(string_output,"Children for Volume %d (permanent_list)",parent);
      if (verbal) print_1d_int_list(Volumes[parent]->geometry.children,string_output);
      )
      
  }
  
  // mask update:
  // The logical expression: (child c parent AND child c parent_mask) OR (child_mask c parent AND child_mask c parent_mask)
  //  needs to be evaluated for each child / parent combination in order to take the masks of each into account
  
  int logic_var1,logic_var2,logic_var_ANY,logic_var_ALL;
  int mask_index,mask_index_child,mask_index_parent;
  int volume_C,volume_P;
  // Loop that takes masks into account
  for (parent=1;parent<number_of_volumes;parent++) {
    used_elements = 0;
    for (child=1;child<number_of_volumes;child++) {
     if (child != parent && 0 == on_int_list(Volumes[parent]->geometry.masked_by_list,child)) {
        // The children list for each volume does not need to contain the volume itself
        //  And a parent masked by it's child can not have that mask as a child
      
      // Here c means within in the sense of a set being part of another set
      // Logical expression to be evaluated: (child c parent AND child c parent_mask) OR (child_mask c parent AND child_mask c parent_mask)
      logic_var1 = on_int_list(temporary_children_lists[parent],child);
      if (logic_var1 == 1 && Volumes[parent]->geometry.is_masked_volume == 1) {
        // if the parent volume is masked, the child also need to be inclosed in these masks to fulfill this side of the logical expression
        logic_var_ANY = 0;
        for (mask_index=0;mask_index<Volumes[parent]->geometry.masked_by_list.num_elements;mask_index++) {
          if (0 == on_int_list(temporary_children_lists[Volumes[parent]->geometry.masked_by_list.elements[mask_index]],child)) {
            if (Volumes[parent]->geometry.mask_mode == 1) {
              logic_var1 = 0;
              break;
            }
          } else logic_var_ANY = 1;
        }
        if (Volumes[parent]->geometry.mask_mode == 2) logic_var1 = logic_var_ANY;
      }
      
      if (logic_var1 == 1) true_temp_list_local.elements[used_elements++] = child;
      else if (Volumes[child]->geometry.is_masked_volume == 1) {
        // If the first side of the logical expression is false, evalute the other side
        // The other side is only relevant if the child volume is masked, otherwise it is ignored
        //printf("Second side of logical expression \n");
        
        logic_var1 = 1; // Assume true
          
        // child_mask c parent
        logic_var_ALL = 0;
        for (mask_index=0;mask_index<Volumes[child]->geometry.masked_by_list.num_elements;mask_index++) {
          if (0 == on_int_list(temporary_children_lists[parent],Volumes[child]->geometry.masked_by_list.elements[mask_index])){
            if (Volumes[child]->geometry.mask_mode == 2) {
              logic_var1 = 0;
              break;
            }
          } else logic_var_ALL = 1;
        }
        if (Volumes[child]->geometry.mask_mode == 1) logic_var1 = logic_var_ALL;
        
        // This line allows the second part of the logical expression to be true in cases where the parent volume is not masked
        if (logic_var1 == 1 && Volumes[parent]->geometry.is_masked_volume == 0) true_temp_list_local.elements[used_elements++] = child;
        
        // There is no reason to check the other part (child_mask c parent_mask) if the first part was not true
        if (logic_var1 == 1 && Volumes[parent]->geometry.is_masked_volume == 1) {
          // This last part requires both the child and the parent to be masked
          // Need to evaluate (child_mask c parent_mask), where both can be a be a list of volume with ALL/ANY modes
          
          if (Volumes[parent]->geometry.mask_mode == 1) {
            logic_var2 = 1; // Assume the logical expression (child_mask c parent_mask) is true
            for (mask_index_parent=0;mask_index_parent<Volumes[parent]->geometry.masked_by_list.num_elements;mask_index_parent++) {
              // As the parent is in ALL mode, the child masks must be within ALL parent masks
              logic_var_ANY = 0; // Assume not a single child is within this parent
              for (mask_index_child=0;mask_index_child<Volumes[child]->geometry.masked_by_list.num_elements;mask_index_child++) {
                volume_P = Volumes[parent]->geometry.masked_by_list.elements[mask_index_parent];
                volume_C = Volumes[child]->geometry.masked_by_list.elements[mask_index_child];
                // Is volume C inside volume P? If yes, volume C must be on volume P's temporary children list
                if (0 == on_int_list(temporary_children_lists[volume_P],volume_C)) {
                  if (Volumes[child]->geometry.mask_mode == 2) {
                    // If child is in ANY mode, any one mask outside is enough to make the expression false
                    logic_var2 = 0;
                    break;
                  }
                } else logic_var_ANY = 1;
              }
              // If child is in ALL mode, then if any one child were within this mask, the logic expression holds true
              if (Volumes[child]->geometry.mask_mode == 1) logic_var2 = logic_var_ANY;
              if (logic_var2 == 0) break; // No need to continue
            }
          } else if (Volumes[parent]->geometry.mask_mode == 2) {
            // If the parent is in ANY mode, it is enough if the child masks are within just 1 of the parent masks
            for (mask_index_parent=0;mask_index_parent<Volumes[parent]->geometry.masked_by_list.num_elements;mask_index_parent++) {
              logic_var2 = 1; // Assume the logical expression (child_mask c parent_mask) is true
              logic_var_ANY = 0; // Assume not a single child is within this parent
              for (mask_index_child=0;mask_index_child<Volumes[child]->geometry.masked_by_list.num_elements;mask_index_child++) {
                volume_P = Volumes[parent]->geometry.masked_by_list.elements[mask_index_parent];
                volume_C = Volumes[child]->geometry.masked_by_list.elements[mask_index_child];
                // Is volume C inside volume P? If yes, volume C must be on volume P's temporary children list
                if (0 == on_int_list(temporary_children_lists[volume_P],volume_C)) {
                  if (Volumes[child]->geometry.mask_mode == 2) {
                    // If child is in ANY mode, any one mask outside is enough to make the expression false
                    logic_var2 = 0;
                    break;
                  }
                } else logic_var_ANY = 1;
              }
              // If child is in ALL mode, then if any one child were within this mask, the logic expression holds true
              if (Volumes[child]->geometry.mask_mode == 1) logic_var2 = logic_var_ANY;
              if (logic_var2 == 1) break; // No need to continue
            }
          }
          
          // if this point is reached, and logic_var2 is true, volume[child] is a child of volume[parent]
          if (logic_var2 == 1) true_temp_list_local.elements[used_elements++] = child;
          
        }
      }
     }
    }
    
    true_children_lists[parent] = malloc(sizeof(struct pointer_to_1d_int_list));
    if (!true_children_lists[parent]) {
      fprintf(stderr,"Failure allocating list in Union function generate_children_lists 7 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    true_children_lists[parent]->num_elements = 0;
    allocate_list_from_temp(used_elements,true_temp_list_local,true_children_lists[parent]);
    
    MPI_MASTER(
      if (verbal) sprintf(string_output,"True children for Volume (post mask) %d",parent);
      if (verbal) print_1d_int_list(*true_children_lists[parent],string_output);
    )
  }
  
  
  // Clean up of dynamically allocated memory
  
  for(child=0;child<number_of_volumes;child++) free(temporary_children_lists[child].elements);
  free(temporary_children_lists);
  free(true_temp_list_local.elements);
  free(temp_list_local.elements);
};


void generate_overlap_lists(struct pointer_to_1d_int_list **true_overlap_lists, struct pointer_to_1d_int_list **raw_overlap_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal) {
  // This function generates overlap lists for each volume
  // Volume m overlaps volume n if there is some subset of space they both ocupy (this is called raw overlap)
  // the true overlap list takes mask into account, meaning that the masks are applied before searching for overlap

  MPI_MASTER(
  if (verbal) printf("\nGenerating overlap lists ---------------------------- \n");
  )
  
  // Manually create the overlap list for the surrounding Vacuum, as it overlaps all other volumes
  
  // temporary_overlap_lists are used to save the calculated overlaps to avoid evaluating the heavy functions more than once (twice) for each possible volume pair
  struct pointer_to_1d_int_list *temporary_overlap_lists;
  temporary_overlap_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list));
  if (!temporary_overlap_lists) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 1 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  
  // Overlap_lists are the final result of the function, and for the surrounding volume it can be set immediatly
  true_overlap_lists[0] = malloc(sizeof(struct pointer_to_1d_int_list));
  if (!true_overlap_lists[0]) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 2 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  true_overlap_lists[0]->num_elements = number_of_volumes-1;
  true_overlap_lists[0]->elements = malloc((number_of_volumes-1)*sizeof(int));
  if (!true_overlap_lists[0]->elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 3 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  raw_overlap_lists[0] =  malloc(sizeof(struct pointer_to_1d_int_list));
  if (!raw_overlap_lists[0]) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 4 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  raw_overlap_lists[0]->num_elements = number_of_volumes;
  raw_overlap_lists[0]->elements = malloc(number_of_volumes*sizeof(int));
  if (!raw_overlap_lists[0]->elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 5 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  raw_overlap_lists[0]->elements[0] = 0; // Volume 0 overlaps itself
  
  int parent;
  for (parent=1;parent<number_of_volumes;parent++) {
      true_overlap_lists[0]->elements[parent-1] = parent;
      raw_overlap_lists[0]->elements[parent] = parent;
  }
  
  char string_output[128];
  MPI_MASTER(
  if (verbal) sprintf(string_output,"Overlaps for Volume %d",0);
  if (verbal) print_1d_int_list(*true_overlap_lists[0],string_output);
  )
  
  // Generate the overlap lists for the remaining volumes
  struct pointer_to_1d_int_list temp_list_local;
  temp_list_local.num_elements = number_of_volumes;
  temp_list_local.elements = malloc(number_of_volumes*sizeof(int));
  if (!temp_list_local.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 6 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  
  int child,used_elements;
  // Create overlap for the remaining volumes
  for (parent=1;parent<number_of_volumes;parent++) {
      true_overlap_lists[parent] = malloc(sizeof(struct pointer_to_1d_int_list));
      if (!true_overlap_lists[parent]) {
	fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 7 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      used_elements = 0;
      temp_list_local.elements[used_elements++] = 0; // Alwasy overlaps with the surrounding vacuum.
      
      for (child=1;child<number_of_volumes;child++) {
        if (child == parent) temp_list_local.elements[used_elements++] = child;
        else {
        if (strcmp("sphere",Volumes[parent]->geometry.shape) == 0 && strcmp("sphere",Volumes[child]->geometry.shape) == 0) {
            if (sphere_overlaps_sphere(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cylinder",Volumes[parent]->geometry.shape) == 0 && strcmp("cylinder",Volumes[child]->geometry.shape) == 0) {
            if (cylinder_overlaps_cylinder(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("box",Volumes[parent]->geometry.shape) == 0 && strcmp("box",Volumes[child]->geometry.shape) == 0) {
            if (box_overlaps_box(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cone",Volumes[parent]->geometry.shape) == 0 && strcmp("cone",Volumes[child]->geometry.shape) == 0) {
            if (cone_overlaps_cone(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("mesh",Volumes[parent]->geometry.shape) == 0 && strcmp("mesh",Volumes[child]->geometry.shape) == 0) {
            if (mesh_overlaps_mesh(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("box",Volumes[parent]->geometry.shape) == 0 && strcmp("cylinder",Volumes[child]->geometry.shape) == 0) {
            if (box_overlaps_cylinder(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cylinder",Volumes[parent]->geometry.shape) == 0 && strcmp("box",Volumes[child]->geometry.shape) == 0) {
            if (cylinder_overlaps_box(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("box",Volumes[parent]->geometry.shape) == 0 && strcmp("sphere",Volumes[child]->geometry.shape) == 0) {
            if (box_overlaps_sphere(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("sphere",Volumes[parent]->geometry.shape) == 0 && strcmp("box",Volumes[child]->geometry.shape) == 0) {
            if (sphere_overlaps_box(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("sphere",Volumes[parent]->geometry.shape) == 0 && strcmp("cylinder",Volumes[child]->geometry.shape) == 0) {
            if (sphere_overlaps_cylinder(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cylinder",Volumes[parent]->geometry.shape) == 0 && strcmp("sphere",Volumes[child]->geometry.shape) == 0) {
            if (cylinder_overlaps_sphere(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cone",Volumes[parent]->geometry.shape) == 0 && strcmp("sphere",Volumes[child]->geometry.shape) == 0) {
            if (cone_overlaps_sphere(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("sphere",Volumes[parent]->geometry.shape) == 0 && strcmp("cone",Volumes[child]->geometry.shape) == 0) {
            if (sphere_overlaps_cone(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cone",Volumes[parent]->geometry.shape) == 0 && strcmp("cylinder",Volumes[child]->geometry.shape) == 0) {
            if (cone_overlaps_cylinder(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cylinder",Volumes[parent]->geometry.shape) == 0 && strcmp("cone",Volumes[child]->geometry.shape) == 0) {
            if (cylinder_overlaps_cone(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cone",Volumes[parent]->geometry.shape) == 0 && strcmp("box",Volumes[child]->geometry.shape) == 0) {
            if (cone_overlaps_box(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("box",Volumes[parent]->geometry.shape) == 0 && strcmp("cone",Volumes[child]->geometry.shape) == 0) {
            if (box_overlaps_cone(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("mesh",Volumes[parent]->geometry.shape) == 0 && strcmp("sphere",Volumes[child]->geometry.shape) == 0) {
            if (mesh_overlaps_sphere(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("mesh",Volumes[parent]->geometry.shape) == 0 && strcmp("cylinder",Volumes[child]->geometry.shape) == 0) {
            if (mesh_overlaps_cylinder(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("mesh",Volumes[parent]->geometry.shape) == 0 && strcmp("box",Volumes[child]->geometry.shape) == 0) {
            if (mesh_overlaps_box(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("mesh",Volumes[parent]->geometry.shape) == 0 && strcmp("cone",Volumes[child]->geometry.shape) == 0) {
            if (mesh_overlaps_cone(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("sphere",Volumes[parent]->geometry.shape) == 0 && strcmp("mesh",Volumes[child]->geometry.shape) == 0) {
            if (sphere_overlaps_mesh(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cylinder",Volumes[parent]->geometry.shape) == 0 && strcmp("mesh",Volumes[child]->geometry.shape) == 0) {
            if (cylinder_overlaps_mesh(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("box",Volumes[parent]->geometry.shape) == 0 && strcmp("mesh",Volumes[child]->geometry.shape) == 0) {
            if (box_overlaps_mesh(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else if (strcmp("cone",Volumes[parent]->geometry.shape) == 0 && strcmp("mesh",Volumes[child]->geometry.shape) == 0) {
            if (cone_overlaps_mesh(&Volumes[parent]->geometry,&Volumes[child]->geometry)) temp_list_local.elements[used_elements++] = child;
        }
        else {
            printf("Need overlap function for type: ");
            printf("%s",Volumes[parent]->geometry.shape);
            printf(" and type: ");
            printf("%s",Volumes[child]->geometry.shape);
            printf(".\n");
            exit(1);
        }
        }
      }
      //allocate_list_from_temp(used_elements,temp_list_local,overlap_lists[parent]);
      allocate_list_from_temp(used_elements,temp_list_local,&temporary_overlap_lists[parent]);
      
      // Save the raw overlap data to the raw_overlap_lists[parent] list
      raw_overlap_lists[parent] = malloc(sizeof(struct pointer_to_1d_int_list));
      if (!raw_overlap_lists[parent]) {
	fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 8 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      raw_overlap_lists[parent]->num_elements = 0;
      allocate_list_from_temp(used_elements,temp_list_local,raw_overlap_lists[parent]);
      
      if (verbal) sprintf(string_output,"Overlaps for Volume (pre mask) %d",parent);
      MPI_MASTER(
      if (verbal) print_1d_int_list(temporary_overlap_lists[parent],string_output);
      )
  }
  
  
  // The temporary_overlap_lists gives the raw overlap data for all volume pairs
  // The next tasks is to take the masks into account, so that a volume is only said to overlap another if all of these statements are true:
  // The volumes overlap each other
  // The mask's of volume 1 overlap volume 2
  // The mask's of volume 2 overlap volume 1
  // The mask's of volume 1 overlap the masks of volume 2
  
  
  int logic_var;
  int overlap_ANY,overlap_ANY_p,overlap_ANY_c;
  int mask_index,mask_index_c,mask_index_p;
  int mask_volume_index,mask_volume_index_p,mask_volume_index_c;

  for (parent=1;parent<number_of_volumes;parent++) {
    used_elements = 0;
    temp_list_local.elements[used_elements++] = 0; // Alwasy overlaps with the surrounding vacuum.
    for (child=1;child<number_of_volumes;child++) {
      if (child != parent) {
        logic_var = 1; // Assume the volumes overlap, and search for evidence that they do not.
      
        // First check if the volumes overlap
        if (0 == on_int_list(temporary_overlap_lists[parent],child)) logic_var = 0;
        
      
        // Check if child overlap with parents masks
        if (logic_var == 1 && Volumes[parent]->geometry.is_masked_volume == 1) {
          overlap_ANY = 0;
          for (mask_index=0;mask_index<Volumes[parent]->geometry.masked_by_list.num_elements;mask_index++) {
            mask_volume_index = Volumes[parent]->geometry.masked_by_list.elements[mask_index];
            if (0 == on_int_list(temporary_overlap_lists[mask_volume_index],child)) {
              if (Volumes[parent]->geometry.mask_mode == 1) {
                logic_var = 0;
                break;
              }
            } else overlap_ANY = 1;
          }
          if (Volumes[parent]->geometry.mask_mode == 2) logic_var = overlap_ANY;
        }
      
        // Check if parent overlap with childs masks
        if (logic_var == 1 && Volumes[child]->geometry.is_masked_volume == 1) {
          overlap_ANY = 0;
          for (mask_index=0;mask_index<Volumes[child]->geometry.masked_by_list.num_elements;mask_index++) {
            mask_volume_index = Volumes[child]->geometry.masked_by_list.elements[mask_index];
            if (0 == on_int_list(temporary_overlap_lists[mask_volume_index],parent)) {
              if (Volumes[child]->geometry.mask_mode == 1) {
                logic_var = 0;
                break;
              }
            } else overlap_ANY = 1;
          }
          if (Volumes[child]->geometry.mask_mode == 2) logic_var = overlap_ANY;
        }
      
        // Check if parents masks overlap childrens masks
        if (logic_var == 1 && Volumes[parent]->geometry.is_masked_volume == 1 && Volumes[child]->geometry.is_masked_volume == 1) {
          overlap_ANY = 0;
          for (mask_index_p=0;mask_index_p<Volumes[parent]->geometry.masked_by_list.num_elements;mask_index_p++) {
            mask_volume_index_p = Volumes[parent]->geometry.masked_by_list.elements[mask_index_p];
            overlap_ANY_p = 1;
            overlap_ANY_c = 0;
            for (mask_index_c=0;mask_index_c<Volumes[child]->geometry.masked_by_list.num_elements;mask_index_c++) {
              mask_volume_index_c = Volumes[child]->geometry.masked_by_list.elements[mask_index_c];
              if (0 == on_int_list(temporary_overlap_lists[mask_volume_index_p],mask_volume_index_c)) {
                if (Volumes[parent]->geometry.mask_mode == 1 && Volumes[child]->geometry.mask_mode == 1) {
                  // If both are in ALL mode and just one combination of masks does not overlap, neither does the common set
                  logic_var = 0;
                  break;
                }
                if (Volumes[parent]->geometry.mask_mode == 2 && Volumes[child]->geometry.mask_mode == 1) {
                  // If the parent is in ANY mode, but the child is in ALL, any one child not overlapping this parent mask, stops the chance for this parent mask
                  overlap_ANY_p = 0;
                  break;
                }
                
              } else {
                // Here because mask_volume_index_p and mask_volume_index_c does overlap
                if (Volumes[parent]->geometry.mask_mode == 1 && Volumes[child]->geometry.mask_mode == 2) {
                  // If the parent is in ALL mode and the child is in ANY mode, stop if a single parent volume does not overlap any child
                  overlap_ANY_c = 1;
                }
                if (Volumes[parent]->geometry.mask_mode == 2 && Volumes[child]->geometry.mask_mode == 2) {
                  // If both parent and child are in any mode, any one overlap between the masks is sufficient
                  overlap_ANY = 1;
                  // Could actually just commit to the overlap list here, and stop all loops.
                }
              }
              
            }
            if (Volumes[parent]->geometry.mask_mode == 1 && Volumes[child]->geometry.mask_mode == 2) logic_var = overlap_ANY_c;
            if (Volumes[parent]->geometry.mask_mode == 2 && Volumes[child]->geometry.mask_mode == 1 && overlap_ANY_p == 1) {
              // When parent is in any mode, and child is in ALL mode, any parent mask that overlaps all children masks is enough to end the parent loop
              logic_var = 1;
              break; // Without this break, only the last parent will matter
            }
            // if (overlap_ANY == 1) break; would speed things up a bit, but only after testing has been started and then it will be repeated
          }
          if (Volumes[parent]->geometry.mask_mode == 2 && Volumes[child]->geometry.mask_mode == 2) logic_var = overlap_ANY;
          // If both volumes have the ANY mode, just one case of overlap is enough.
        }
        
        // If all of the 4 statements above evaluate to true, the two volumes parent and child do overlap and it is added to the list.
        if (logic_var == 1) temp_list_local.elements[used_elements++] = child;
        
      }
    }
    // Allocate the actual overlap list with the new temp_list_local
    allocate_list_from_temp(used_elements,temp_list_local,true_overlap_lists[parent]);
    if (verbal) sprintf(string_output,"Overlaps for Volume (post mask) %d",parent);
    MPI_MASTER(
    if (verbal) print_1d_int_list(*true_overlap_lists[parent],string_output);
    )
  }
  
  // Clean up of dynamically allocated memory
  for(child=1;child<number_of_volumes;child++) free(temporary_overlap_lists[child].elements);
  free(temporary_overlap_lists);
  free(temp_list_local.elements);
  
};

void add_to_mask_intersect_list(struct pointer_to_1d_int_list *mask_intersect_list, int given_volume_index) {
    // A bit simpler than before
    if (on_int_list(*mask_intersect_list,given_volume_index) == 0)
      add_element_to_int_list(mask_intersect_list,given_volume_index);
}


void generate_intersect_check_lists(struct pointer_to_1d_int_list **overlap_lists,struct Volume_struct **Volumes, int number_of_volumes, int verbal) {
  // Generate intersection list for each volume
  MPI_MASTER(
  if (verbal) printf("\nGenerating intersect check lists -------------------- \n");
  )
  // Take overlap list for volume n
  // Remove entries with p < p(n)
  // Remove entries with parents on the list
  
  // Mask update: Mask volumes does not have priorities, do not do step 2 for mask volumes
  // Mask update: Instead of step 3 (Remove entries with parents on the list), move these entries to a Mask intersect list if that parent is masking the entry, otherwise remove
  
  // 1) Mask update: Take overlap list for volume n
  // 2) Mask update: Remove entries with p < p(n), except mask volumes, that are moved to another list A
  // 3) Mask update: Entries with parents on the list are removed (if these parents are masked, only remove the entry if it is a child of their parents masks)
  // 4) Possible optimization: Entries with parents on the A list, are moved to the appropriate mask list for this volume
  // 5) Mask update: Entries on list A are added again, if they are not parents of volume n
  // 6) Mask update: Entries on the main list that are masked are moved to the appropriate mask list for this volume, if their mask is on list A
  
  // Justification of the top algorithm:
  // No need to check intersections with something that does not overlap this volume, so start with intersect list and remove from that.
  // Entries with p < p(n) are "beneath" this volume, and thus doesn't cut it, mask volumes are however always "above" and are treated with care (moved to list A)
  // Entries with parents still on the list is removed, as it is impossible to intersect with them without first going through the parent
  // Entries with parents on the A list can only be seen if the ray is in that particular mask (the parent on the A list), so it can be added to the appropriate mask list for this volume
  // Now all masks that cut the volume are transfered back to the main list, the only masks that overlap and does not cut are parents of this volume, and these are thus not added
  // The entries on the list that are masked are added to the appropriate mask lists of this volume, but only if their mask is on the A list, as only masks on list A can have mask status 1 which is required for this to be relevant (this was true automatically for the ones added to the appropriate lists in the optimization step).
  
  
  // The intersect check list for volume 0 is done manually
  // Declare list for holding logic data for each index in the overlap list of this volume
  struct pointer_to_1d_int_list logic_list;
  logic_list.num_elements = overlap_lists[0]->num_elements;
  logic_list.elements = malloc(logic_list.num_elements * sizeof(int));
  if (!logic_list.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 9 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // Declare similar list for List A
  struct pointer_to_1d_int_list mask_logic_list;
  mask_logic_list.num_elements = overlap_lists[0]->num_elements;
  mask_logic_list.elements = malloc(mask_logic_list.num_elements * sizeof(int));
  if (!mask_logic_list.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 10 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  
  int iterate;
  int overlap_index;
  
  // The intersect check lists for the remaining volumes are generated
  int volume_index,mask_volume_number,mask_index;
  for (volume_index = 0;volume_index < number_of_volumes;volume_index++) {
    
      // 1) Take overlap list for volume n
      logic_list.num_elements = overlap_lists[volume_index]->num_elements;
      logic_list.elements = malloc(logic_list.num_elements * sizeof(int));
      if (!logic_list.elements) {
	fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 11 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      mask_logic_list.num_elements = overlap_lists[volume_index]->num_elements;
      mask_logic_list.elements = malloc(mask_logic_list.num_elements * sizeof(int));
      if (!mask_logic_list.elements) {
	fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 11 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      
      
      if (Volumes[volume_index]->geometry.is_mask_volume == 1) {
        // Masks do not have any entries on their intersect check list, as they are never the current volume
        for (iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 0;
        for (iterate=0;iterate<mask_logic_list.num_elements;iterate++) mask_logic_list.elements[iterate] = 0;
      } else {
        for (iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 1;
        for (iterate=0;iterate<mask_logic_list.num_elements;iterate++) mask_logic_list.elements[iterate] = 0;
      }
      
      //2) Remove entries with p < p(n), except mask volumes, that are moved to another list A
      for (overlap_index = 0;overlap_index < overlap_lists[volume_index]->num_elements;overlap_index++) {
        
        if (overlap_lists[volume_index]->elements[overlap_index] == 0) logic_list.elements[overlap_index] = 0; // The surrounding vacuum has lower priority (and is not a mask)
        else if (Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.is_mask_volume == 1) {
           logic_list.elements[overlap_index] = 0;      // Remove this volume from the main list
           mask_logic_list.elements[overlap_index] = 1; // Add mask volumes to seperat list
        }
        else if (volume_index != 0) { // Only relevant to remove elements because of priority if the volume_index is different from 0, meaning the surrounding vacuum skips this step
           if (Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.priority_value < Volumes[volume_index]->geometry.priority_value) {
           // If the investigated volume on the overlap_list have lower priority than the volume with volume_index, remove it
           logic_list.elements[overlap_index] = 0; // Remove this volume from the list
           }
        }
      }
      
      // 3) Entries with parents on the list are removed
      for (overlap_index = 0;overlap_index < overlap_lists[volume_index]->num_elements;overlap_index++) {
        // Check if this overlap_lists[0]->elements[overlap_index] is a child of another member of the overlap list
        for (iterate = 0;iterate < overlap_lists[volume_index]->num_elements;iterate++) {
             if (iterate != overlap_index) { // Only necessary if a volume determines that it has itself as child, but a nice safety.
                // We are now checking if Volumes[overlap_lists[volume_index]->elements[iterate]]->geometry.children contains overlap_lists[overlap_index]
                // The && part is needed because we do not remove children of elements that have allready been removed because of their priority
                
                // if (on_int_list(Volumes[overlap_lists[volume_index]->elements[iterate]]->geometry.children,overlap_lists[volume_index]->elements[overlap_index]) && logic_list.elements[overlap_lists[volume_index]->elements[iterate]] == 1) {logic_list.elements[overlap_index] = 0; bug fixed on 3/4 2016
                if (on_int_list(Volumes[overlap_lists[volume_index]->elements[iterate]]->geometry.children,overlap_lists[volume_index]->elements[overlap_index]) && logic_list.elements[iterate] == 1) logic_list.elements[overlap_index] = 0;
                
                /*
                Explanation with simpler notation
                
                Overlap list of volume i = O_i
                Element j of overlap list i = O_i(j)
                
                Children list of volume i = C_i
                Element j on children list i = C_i(j)
                
                Priority of volume i p(i)
                
                for i = volumes
                    for j in O_i(j) 
                        logic(j) = 1;
                        if p(i) > p(O_i(j)) logic(j) = 0; // Remove if lower priority than the currently checked
                        
                        for k in O_i(k)
                            if (O_i(j) is contained on the list C_k && logic(k) == 1) logic(j) = 0
                */
                
                // 4) Entries with parents on the A list, are moved to the appropriate mask list for this volume
                if (on_int_list(Volumes[overlap_lists[volume_index]->elements[iterate]]->geometry.children,overlap_lists[volume_index]->elements[overlap_index]) && mask_logic_list.elements[iterate] == 1) {
                   logic_list.elements[overlap_index] = 0; // Remove from main list (Not strictly needed as it will be removed already if it is on list A)
                   // Add overlap_lists[volume_index]->elements[overlap_index] to volumes mask intersect list for mask with index overlap_lists[volume_index]->elements[iterate]
                   //add_to_mask_intersect_lists(&Volumes[volume_index]->geometry.mask_intersect_lists,overlap_lists[volume_index]->elements[iterate],overlap_lists[volume_index]->elements[overlap_index]);
                   // Simpler method that just collects all the masked intersect parts on a 1d_int list instead of seperating them for each mask
                   add_to_mask_intersect_list(&Volumes[volume_index]->geometry.mask_intersect_list,overlap_lists[volume_index]->elements[overlap_index]);
                }
                
             }
        }
      }
      
      // 5) Entries on list A are added again, if they are not parents of volume n
      // Time to add mask volumes removed back to the intersect list, if they are not parents of the current volume, meaning the current volume should not be a child of the mask
      for (overlap_index = 0;overlap_index < overlap_lists[volume_index]->num_elements;overlap_index++) {
        if (mask_logic_list.elements[overlap_index] == 1) {
          mask_volume_number = overlap_lists[volume_index]->elements[overlap_index];
          if (on_int_list(Volumes[mask_volume_number]->geometry.children,volume_index) == 0) logic_list.elements[overlap_index] = 1; // Add it back to the list
        }
      }
      
      // 6) Entries on the main list that are masked are moved to the appropriate mask list for this volume, if their mask is on list A
      int mask_index,mask_mode,found_index,logic_ALL;
      for (overlap_index = 0;overlap_index < overlap_lists[volume_index]->num_elements;overlap_index++) {
        if (logic_list.elements[overlap_index] == 1 && Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.is_masked_volume == 1) {
          logic_list.elements[overlap_index] = 0; // If the volume is masked, remove it from the intersect check list
          // Could actually keep it on the intersect list if the volume's mask is a parent of the current volume, now it will just be added in all cases
          
          // When ALL setting is used, all masks should overlap the current volume, otherwise the user probably made a mistake, this should be checked in error checking
          // When ANY setting is used, at least one mask should overlap the current volume, otherwise the user probably made a mistake, this should be checked in error checking
          // mask_mode == 1 => ALL / mask_mode == 2 => ANY
          mask_mode = Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.mask_mode;
          
          if (mask_mode == 1) {
            logic_ALL = 1;
            for (iterate=0;iterate<Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.masked_by_list.num_elements;iterate++) {
              mask_index = Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.masked_by_list.elements[iterate];
              
              if (on_int_list(*overlap_lists[volume_index],mask_index) == 0) {
                // If any one of the volumes masks do not overlap with this volume, there is no reason check intersections with the volume regardless of the mask status's
                logic_ALL = 0;
                break;
              }
            }
          if (logic_ALL == 1) {
            // If all of it's masks are overlapping the current volume, add it to all relevant mask intersect lists of this volume
            for (iterate=0;iterate<Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.masked_by_list.num_elements;iterate++) {
              mask_index = Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.masked_by_list.elements[iterate];
              //add_to_mask_intersect_lists(&Volumes[volume_index]->geometry.mask_intersect_lists,mask_index,overlap_lists[volume_index]->elements[overlap_index]);
              // Adding the masked intersect list elements to another list
              add_to_mask_intersect_list(&Volumes[volume_index]->geometry.mask_intersect_list,overlap_lists[volume_index]->elements[overlap_index]);
            }
          }
            
            
          } else if (mask_mode == 2) {
          // When in ANY mode, the problem is easier, as not all masks have to overlap the volume, and we can add each one to the list
            for (iterate=0;iterate<Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.masked_by_list.num_elements;iterate++) {
              mask_index = Volumes[overlap_lists[volume_index]->elements[overlap_index]]->geometry.masked_by_list.elements[iterate];
            
              if (on_int_list(*overlap_lists[volume_index],mask_index) == 1) {
                //add_to_mask_intersect_lists(&Volumes[volume_index]->geometry.mask_intersect_lists,mask_index,overlap_lists[volume_index]->elements[overlap_index]);
                // Adding the masked intersect list elements to another list
                add_to_mask_intersect_list(&Volumes[volume_index]->geometry.mask_intersect_list,overlap_lists[volume_index]->elements[overlap_index]);
              }
            }
            
          }
        }
      }
      
      
      Volumes[volume_index]->geometry.intersect_check_list.num_elements = sum_int_list(logic_list);
      Volumes[volume_index]->geometry.intersect_check_list.elements = malloc(Volumes[volume_index]->geometry.intersect_check_list.num_elements * sizeof(int));
      if (!Volumes[volume_index]->geometry.intersect_check_list.elements) {
	fprintf(stderr,"Failure allocating list in Union function generate_overlap_lists 12 - Exit!\n");
	exit(EXIT_FAILURE);
      }
	
      iterate = 0;
      for (overlap_index = 0;overlap_index < overlap_lists[volume_index]->num_elements;overlap_index++) {
            if (logic_list.elements[overlap_index])   Volumes[volume_index]->geometry.intersect_check_list.elements[iterate++] = overlap_lists[volume_index]->elements[overlap_index];
      }
      free(logic_list.elements); // Need to be careful with names for variables to be freed, as they can collide with names in main because of automatic declaration of external variables
      free(mask_logic_list.elements);
      
      
      char string_output[128];
      MPI_MASTER(
      if (verbal) sprintf(string_output,"Intersect check list for Volume %d",volume_index);
      if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.intersect_check_list,string_output);
      if (verbal) sprintf(string_output,"Mask intersect check list for Volume %d",volume_index);
      if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.mask_intersect_list,string_output)
      );
  }
};

void generate_parents_lists(struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal, int mask_mode) {
  // Function for generating parent lists for all volumes
  // A volume m has n as a parent, if volume n has volume m as a child
  
  // if mask_mode == 0, masks are ignored, if mask_mode == 1, masks are taken into account.
  
  MPI_MASTER(
  if (verbal) {
    if (mask_mode == 1)
        printf("\nGenerating parents lists ---------------------------- \n");
    else if (mask_mode == 0)
        printf("\nGenerating parents lists (ignoring masks) ----------- \n");
    else {
        printf("Error, the function parents_lists got a non defined mask_mode");
        exit(EXIT_FAILURE);
    }
  }
  
  )
  // Volume iterate has volume p as a parent, if volume p has volume iterate as child.
  
  struct pointer_to_1d_int_list temp_list_local;
  temp_list_local.num_elements = number_of_volumes;
  temp_list_local.elements = malloc(number_of_volumes*sizeof(int));
  if (!temp_list_local.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_parents_lists 1 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  // Loop over
  int iterate,parent,used_elements;
  for (iterate = 0;iterate < number_of_volumes;iterate++) {
    // clear temp list
    used_elements = 0;
    parents_lists[iterate] = malloc(sizeof(struct pointer_to_1d_int_list));
    if (!parents_lists[iterate]) {
      fprintf(stderr,"Failure allocating list in Union function generate_parents_lists 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (parent = 0;parent < number_of_volumes;parent++) {
        if (on_int_list(Volumes[parent]->geometry.children,iterate))
          if (mask_mode == 1 || (Volumes[parent]->geometry.is_mask_volume == 0 && Volumes[iterate]->geometry.is_mask_volume == 0))
            temp_list_local.elements[used_elements++] = parent;
    }
      allocate_list_from_temp(used_elements,temp_list_local,parents_lists[iterate]);
      
      char string_output[128];
      MPI_MASTER(
      if (verbal) sprintf(string_output,"Parents for Volume %d",iterate);
      if (verbal) print_1d_int_list(*parents_lists[iterate],string_output);
      )
  }
  free(temp_list_local.elements);

};

void generate_true_parents_lists(struct pointer_to_1d_int_list **parents_lists, struct pointer_to_1d_int_list **true_children_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal, int mask_mode) {
  // Function for generating parent lists for all volumes
  // A volume m has n as a parent, if volume n has volume m as a child
  
  // if mask_mode == 0, masks are ignored, if mask_mode == 1, masks are taken into account.
  
  MPI_MASTER(
  if (verbal) {
    if (mask_mode == 1)
        printf("\nGenerating parents lists ---------------------------- \n");
    else if (mask_mode == 0)
        printf("\nGenerating parents lists (ignoring masks) ----------- \n");
    else {
        printf("Error, the function parents_lists got a non defined mask_mode");
        exit(EXIT_FAILURE);
    }
  }
  
  )
  // Volume iterate has volume p as a parent, if volume p has volume iterate as child.
  
  struct pointer_to_1d_int_list temp_list_local;
  temp_list_local.num_elements = number_of_volumes;
  temp_list_local.elements = malloc(number_of_volumes*sizeof(int));
  if (!temp_list_local.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_true_parents_lists 1 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  
  // Loop over
  int iterate,parent,used_elements;
  for (iterate = 0;iterate < number_of_volumes;iterate++) {
    // clear temp list
    used_elements = 0;
    parents_lists[iterate] = malloc(sizeof(struct pointer_to_1d_int_list)); // allocate_list_from_temp allocates
    if (!parents_lists[iterate]) {
      fprintf(stderr,"Failure allocating list in Union function generate_true_parents_lists 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (parent = 0;parent < number_of_volumes;parent++) {
        //if (on_int_list(Volumes[parent]->geometry.children,iterate))
        if (on_int_list(*true_children_lists[parent],iterate))
          if (mask_mode == 1 || (Volumes[parent]->geometry.is_mask_volume == 0 && Volumes[iterate]->geometry.is_mask_volume == 0))
            temp_list_local.elements[used_elements++] = parent;
    }
      allocate_list_from_temp(used_elements,temp_list_local,parents_lists[iterate]);
      
      char string_output[128];
      MPI_MASTER(
      if (verbal) sprintf(string_output,"Parents for Volume %d",iterate);
      if (verbal) print_1d_int_list(*parents_lists[iterate],string_output);
      )
  }
  free(temp_list_local.elements);

};

void generate_intersect_check_lists_experimental(struct pointer_to_1d_int_list **true_overlap_lists, struct pointer_to_1d_int_list **raw_overlap_lists, struct pointer_to_1d_int_list **parents_lists, struct pointer_to_1d_int_list **true_parents_lists , struct Volume_struct **Volumes, int number_of_volumes, int verbal) {
  // Generates the intersect_check_list and mask_intersect_list for each Volume.
  /*
  Description of needed lists for volume n:
  Children list: List of volumes that is contained within volume n (is stored in the Volumes struct)
  True Children list: List of volumes that when masked by their masks is contained within volume n when masked by it's masks
  
  Parents list: List of volumes that contains volume n
  True parents list: List of volumes that when masked by their masks contains volume n when it is masked by it's masks
  
  raw overlap list: List of volumes whos geometry overlaps the geometry of volume n
  true overlap list: List of volumes whos geometry overlaps the geometry of volume n when the masks of both volumes are applied
  
  
  
  The algorithm:
  
  1) Take the true overlap list for volume n
  2) remove parents of n (normal parent list, with masks)
  3) remove volumes that do not mask n and are true parents of n
  4) remove volumes that are not masks and have lower priority than n
  5) remove volumes that have at least one true parent on the list (in step 4) that is not a mask volume
  6) split the list into two, the intersect_check_list which is all non masked volumes still on the list, and the mask_intersect_list which is the masked volumes
  7) remove volumes on the mask_intersect_list whos mask does not overlap (standard overlap list) n
  
  In step 5 the order in which the volumes are tested and removed may matter, so it is specifically stated that it is as the list looked in step 4.
  */
  
  MPI_MASTER(
  if (verbal) printf("\nGenerating intersect check lists -------------------- \n");
  )
  
  struct pointer_to_1d_int_list work_list;
  struct pointer_to_1d_int_list logic_list;
  
  int volume_index,iterate,parent,mask_index,masked_volume_index,ANY_logic,true_parent_volume_number;
  int *mask_check,*mask_start;
  for (volume_index = 0;volume_index < number_of_volumes;volume_index++) {
    
    // 1) Take the true overlap list for volume n
    // Create copy of true_overlap_lists to work with
    
    if (Volumes[volume_index]->geometry.is_mask_volume == 1) {
      // Bug fixed on 26/11/2016, do not create intersection lists for masks as they are not used, and affects destinations lists in a problematic way
      Volumes[volume_index]->geometry.intersect_check_list.num_elements = 0;
      Volumes[volume_index]->geometry.mask_intersect_list.num_elements = 0;
      
    } else {
      work_list.num_elements = true_overlap_lists[volume_index]->num_elements;
      work_list.elements = malloc(work_list.num_elements * sizeof(int));
      if (!work_list.elements) {
	fprintf(stderr,"Failure allocating list in Union function generate_intersect_check_lists_experimental 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      for (iterate=0;iterate<work_list.num_elements;iterate++) work_list.elements[iterate] = true_overlap_lists[volume_index]->elements[iterate];
    
    //if (verbal) print_1d_int_list(work_list,"After 1)");
    
    
    //2) remove parents of n (normal parent list, with masks)
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (on_int_list(*parents_lists[volume_index],work_list.elements[iterate]))
        remove_element_in_list_by_index(&work_list,iterate);
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 2)");
    
    //3) remove volumes that do not mask n and are true parents of n
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (on_int_list(Volumes[volume_index]->geometry.masked_by_list,work_list.elements[iterate]) == 0 && on_int_list(*true_parents_lists[volume_index],work_list.elements[iterate]))
        remove_element_in_list_by_index(&work_list,iterate);
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 3)");
    
    //4) remove volumes that are not masks and have lower priority than n
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (Volumes[work_list.elements[iterate]]->geometry.is_mask_volume == 0 && Volumes[work_list.elements[iterate]]->geometry.priority_value < Volumes[volume_index]->geometry.priority_value)
        remove_element_in_list_by_index(&work_list,iterate);
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 4)");
    
    //5) remove volumes that have at least one true parent on the list (in step 4) that is not a mask volume
    // Here a logic_list is used to not have the order of removal matter
    logic_list.num_elements = work_list.num_elements;
    logic_list.elements = malloc(logic_list.num_elements * sizeof(int));
    if (!logic_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_intersect_check_lists_experimental 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for(iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 1;
    
    for (iterate=0;iterate<work_list.num_elements;iterate++) {
      for (parent=0;parent<true_parents_lists[work_list.elements[iterate]]->num_elements;parent++) {
        true_parent_volume_number = true_parents_lists[work_list.elements[iterate]]->elements[parent];
        if (on_int_list(work_list,true_parent_volume_number) && Volumes[true_parent_volume_number]->geometry.is_mask_volume == 0) {
          // Since element number iterate on the work list have a true parent on the work list, it can be removed
          logic_list.elements[iterate] = 0;
          break;
        }
      }
    }
    
    // Now the elements marked for removal can be removed without interfering in the operation
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (logic_list.elements[iterate] == 0) remove_element_in_list_by_index(&work_list,iterate);
    }
    free(logic_list.elements);
    
    //if (verbal) print_1d_int_list(work_list,"After 5)");
      
    //6) split the list into two, the intersect_check_list which is all non masked volumes still on the list, and the mask_intersect_list which is the masked volumes
    
    Volumes[volume_index]->geometry.intersect_check_list.num_elements = 0;
    Volumes[volume_index]->geometry.mask_intersect_list.num_elements = 0;
    
    for (iterate=0;iterate<work_list.num_elements;iterate++) {
      if (Volumes[work_list.elements[iterate]]->geometry.is_masked_volume == 1) {
        add_element_to_int_list(&Volumes[volume_index]->geometry.mask_intersect_list,work_list.elements[iterate]);
      } else {
        add_element_to_int_list(&Volumes[volume_index]->geometry.intersect_check_list,work_list.elements[iterate]);
      }
    }
    
    //if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.intersect_check_list,"After 6) intersect_check_list");
    //if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.mask_intersect_list,"After 6) mask_intersect_list");
    
    //7) remove volumes on the mask_intersect_list whos masks does not overlap (standard overlap list) n
    for (iterate=Volumes[volume_index]->geometry.mask_intersect_list.num_elements-1;iterate>-1;iterate--) {
      // Need to check if the volumes masking Volumes[volume_index]->geometry.mask_intersect_list.elements[iterate] overlaps with n
      masked_volume_index = Volumes[volume_index]->geometry.mask_intersect_list.elements[iterate];
      
      if (Volumes[masked_volume_index]->geometry.mask_mode == 1) {// All mode, if just one does not overlap, remove the element
        for (mask_start=mask_check=Volumes[masked_volume_index]->geometry.masked_by_list.elements;mask_check-mask_start<Volumes[masked_volume_index]->geometry.masked_by_list.num_elements;mask_check++) {
          if (on_int_list(*raw_overlap_lists[volume_index],*mask_check) == 0) {
            remove_element_in_list_by_index(&Volumes[volume_index]->geometry.mask_intersect_list,iterate);
            break;
          }
        }
      } else { // ANY mode, just one need to overlap in order to keep the element
        ANY_logic = 0;
        for (mask_start=mask_check=Volumes[masked_volume_index]->geometry.masked_by_list.elements;mask_check-mask_start<Volumes[masked_volume_index]->geometry.masked_by_list.num_elements;mask_check++) {
          if (on_int_list(*raw_overlap_lists[volume_index],*mask_check) == 1) {
            ANY_logic = 1;
            break;
          }
        }
        if (ANY_logic == 0) remove_element_in_list_by_index(&Volumes[volume_index]->geometry.mask_intersect_list,iterate);
      }
    }
    
    if (work_list.num_elements > 0) free(work_list.elements);
    }
    
    
    //if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.mask_intersect_list,"After 7) mask_intersect_list");
    
    char string_output[128];
    MPI_MASTER(
    if (verbal) sprintf(string_output,"Intersect check list for Volume %d",volume_index);
    if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.intersect_check_list,string_output);
    if (verbal) sprintf(string_output,"Mask intersect check list for Volume %d",volume_index);
    if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.mask_intersect_list,string_output);
    )
    
    
  }
}

void generate_grandparents_lists(struct pointer_to_1d_int_list **grandparents_lists, struct pointer_to_1d_int_list **parents_lists, int number_of_volumes, int verbal) {
  // Function for generating grandparents lists
  // Volume iterate has volume p as a grandparent, if volume p has a parent T, who has volume iterate as parent.
  // Alternertively:
  // Volume iterate has volume p as a grandparent, if volume p have a child that is volume iterate's parent.
  
  MPI_MASTER(
  if (verbal) printf("\nGenerating grandparents lists ----------------------- \n");
  )

  struct pointer_to_1d_int_list common;
  common.num_elements = number_of_volumes;
  common.elements = malloc(common.num_elements*sizeof(int)); // Maximum needed space.
  if (!common.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_grandparents_lists 1 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  struct pointer_to_1d_int_list temp_list_local;
  temp_list_local.num_elements = number_of_volumes;
  temp_list_local.elements = malloc(number_of_volumes*sizeof(int));
  if (!temp_list_local.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_grandparents_lists 2 - Exit!\n");
    exit(EXIT_FAILURE);
  }

  int iterate,reset_int,parent,child,used_elements;
  for (iterate = 0;iterate < number_of_volumes;iterate++) {
    // clear temp list
    used_elements = 0;

    for (reset_int=0; reset_int<number_of_volumes; reset_int++) common.elements[reset_int] = -1; // Initialize to impossible volume ids
    for (reset_int=0; reset_int<number_of_volumes; reset_int++) temp_list_local.elements[reset_int] = -1; // Initialize to impossible volume ids

    grandparents_lists[iterate] = malloc(sizeof(struct pointer_to_1d_int_list));
    if (!grandparents_lists[iterate]) {
      fprintf(stderr,"Failure allocating list in Union function generate_grandparents_lists 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }

    
    for (parent = 0; parent<parents_lists[iterate]->num_elements; parent++) {
        // parent number p parents_lists[iterate].elements.[p] in the parent_list for iterate.
        on_both_int_lists(parents_lists[parents_lists[iterate]->elements[parent]], parents_lists[iterate], &common);
        // returns a pointer_to_1d_list, with all the elements that are in common.
        for (child = 0;child < common.num_elements;child++) {
            // Need to make sure the element is not already on the list
            if (0 == on_int_list(temp_list_local, common.elements[child])) {
              temp_list_local.elements[used_elements++] = common.elements[child];
            }
        }
    }
    allocate_list_from_temp(used_elements, temp_list_local, grandparents_lists[iterate]);
  
    char string_output[128];
    MPI_MASTER(
      if (verbal) sprintf(string_output,"Grandparents for Volume %d", iterate);
      if (verbal) print_1d_int_list(*grandparents_lists[iterate], string_output);
    )
  }
  free(temp_list_local.elements);
  free(common.elements);
};


void generate_destinations_lists_experimental(struct pointer_to_1d_int_list **true_overlap_lists, struct pointer_to_1d_int_list **true_children_lists, struct pointer_to_1d_int_list **true_parents_lists, struct pointer_to_1d_int_list **true_grandparents_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal) {
  // Generates destinations list for for all volumes
  // Current implementation uses true_parents_lists and true_grandparents_lists that are generated as if no masks were defined

  MPI_MASTER(
  if (verbal) printf("\nGenerating destinations lists ----------------------- \n");
  )
  // Volume 0 has an hardcoded empty destinations list
  Volumes[0]->geometry.destinations_list.num_elements = 0;

  struct pointer_to_1d_int_list work_list;
  int volume_index,iterate,iterate2,found_index,I_index,I_volume;
  
  for (volume_index=1;volume_index<number_of_volumes;volume_index++) {
    
    // 1) Take the true overlap list for volume n
    // Create copy of true_overlap_lists to work with
    work_list.num_elements = true_overlap_lists[volume_index]->num_elements;
    work_list.elements = malloc(work_list.num_elements * sizeof(int));
    if (!work_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_destinations_lists_experimental 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<work_list.num_elements;iterate++) work_list.elements[iterate] = true_overlap_lists[volume_index]->elements[iterate];
    
    //if (verbal) print_1d_int_list(work_list,"After 1)");
    
    // 2) Remove elements from n's intersection list
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (on_int_list(Volumes[volume_index]->geometry.intersect_check_list,work_list.elements[iterate]))
        remove_element_in_list_by_index(&work_list,iterate);
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 2)");
  
    // 3) Remove true children of non-mask elements on n's intersection list
    for (I_index=0;I_index<Volumes[volume_index]->geometry.intersect_check_list.num_elements;I_index++) {
      I_volume = Volumes[volume_index]->geometry.intersect_check_list.elements[I_index];
      if (Volumes[I_volume]->geometry.is_mask_volume == 0) {
        for (iterate=0;iterate<true_children_lists[I_volume]->num_elements;iterate++) {
          remove_element_in_list_by_value(&work_list,true_children_lists[I_volume]->elements[iterate]);
        }
      }
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 3)");
    
    // 4) Remove elements from mask intersection list
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (on_int_list(Volumes[volume_index]->geometry.mask_intersect_list,work_list.elements[iterate]))
        remove_element_in_list_by_index(&work_list,iterate);
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 4)");
    
    // 5) Remove true children of elements on n's mask intersection list
    for (I_index=0;I_index<Volumes[volume_index]->geometry.mask_intersect_list.num_elements;I_index++) {
      I_volume = Volumes[volume_index]->geometry.mask_intersect_list.elements[I_index];
      if (Volumes[I_volume]->geometry.is_mask_volume == 0) {
        for (iterate=0;iterate<true_children_lists[I_volume]->num_elements;iterate++) {
          remove_element_in_list_by_value(&work_list,true_children_lists[I_volume]->elements[iterate]);
        }
      }
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 5)");
    
    // 6) Remove true children of n
    for (iterate=0;iterate<true_children_lists[volume_index]->num_elements;iterate++)
      remove_element_in_list_by_value(&work_list,true_children_lists[volume_index]->elements[iterate]);
      
    //if (verbal) print_1d_int_list(work_list,"After 6)");
      
    // 7) Remove true grandparents of n
    for (iterate=0;iterate<true_grandparents_lists[volume_index]->num_elements;iterate++)
      remove_element_in_list_by_value(&work_list,true_grandparents_lists[volume_index]->elements[iterate]);
      
    //if (verbal) print_1d_int_list(work_list,"After 7)");
      
    // 8) Remove mask volumes
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (Volumes[work_list.elements[iterate]]->geometry.is_mask_volume == 1)
        remove_element_in_list_by_index(&work_list,iterate);
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 8)");
    
    // 9) Remove true parents of n on the list that has other true parents of n on the list with higher priority
    for (iterate=work_list.num_elements-1;iterate>-1;iterate--) {
      if (on_int_list(*true_parents_lists[volume_index],work_list.elements[iterate])){
        // work_list.elements[iterate] is the volume index of a volume that is a true parent of n
        for (iterate2=0;iterate2<work_list.num_elements;iterate2++) {
          if (on_int_list(*true_parents_lists[volume_index],work_list.elements[iterate2]) && iterate != iterate2) {
            if (Volumes[work_list.elements[iterate]]->geometry.priority_value < Volumes[work_list.elements[iterate2]]->geometry.priority_value) {
              //printf("Removing element number %d (V%d) because element number %d (V%d) had higher priority \n",iterate,work_list.elements[iterate],iterate2,work_list.elements[iterate2]);
              remove_element_in_list_by_index(&work_list,iterate);
              break; // Missing break inserted on 14/9/2016
            }
          }
        }
      }
    }
    
    //if (verbal) print_1d_int_list(work_list,"After 9)");
    
    // 10) The remaining list is the destinations_list
    Volumes[volume_index]->geometry.destinations_list.num_elements = work_list.num_elements;
    Volumes[volume_index]->geometry.destinations_list.elements = malloc(Volumes[volume_index]->geometry.destinations_list.num_elements*sizeof(int));
    if (!Volumes[volume_index]->geometry.destinations_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_destinations_lists_experimental 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    
    for (iterate=0;iterate<work_list.num_elements;iterate++)
      Volumes[volume_index]->geometry.destinations_list.elements[iterate] = work_list.elements[iterate];
    
    // Clean up after work_list so the next can be allocated
    free(work_list.elements);
  
  
    char string_output[128];
    MPI_MASTER(
    if (verbal) sprintf(string_output,"Destinations list for Volume %d",volume_index);
    if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.destinations_list,string_output);
    )

  }

};

void generate_destinations_list(int N_volume,struct Volume_struct **Volumes,struct pointer_to_1d_int_list original_overlap_list,struct pointer_to_1d_int_list *parent_list, struct pointer_to_1d_int_list *grandparent_list) {
    // This function generates the destinations_list for a single volume index, N_volume
    // The destination list describes which volumes a ray can enter when leaving N_volume
    // Each of the 6 steps for the algorithm is commented individually, and debug print statements are available
    
    // Mask update: Mask volumes are to be removed from all destinations_list as they can not be the current volume
    // It is not that simple, as some volume will then get empty destination lists. Need to revise this algorithm.
    
    // 1) Start with the overlap list of volume N
    // 2) remove all Volumes from the overlap list of N, which is also on the intersect_check_list
    // 3) remove the children of volumes removed in step 2)
    // 4) remove the children of N
    // 5) remove the grandparents of N
    // 6) remove volumes with lower priority than parents of N still on the list
    // 7) The remaing list is the destinations list
    
    
    // The destination list system should run without masks altogether, meaning parent and grandparent lists should not use them,
    //  and all masks should be removed in step 2
    
    // 1) Start with the overlap list of volume N
    // 2) remove all masks on the list
    // 3) remove all Volumes from the overlap list of N, which is also on the intersect_check_list
    // 4) remove the children of volumes removed in step 2) (if the removed volume was not a mask )
    // 5) remove the children of N
    // 6) remove the grandparents of N
    // 7) remove volumes with lower priority than parents of N still on the list
    // 8) The remaing list is the destinations list
    
    

    // 1) Start with the overlap list of volume N
    struct pointer_to_1d_int_list overlap_list;
    overlap_list.num_elements = original_overlap_list.num_elements;
    overlap_list.elements = malloc(overlap_list.num_elements*sizeof(int));
    if (!overlap_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_destinations_lists 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    int iterate;
    for (iterate=0;iterate<overlap_list.num_elements;iterate++) overlap_list.elements[iterate] = original_overlap_list.elements[iterate];
    
    char string_output[124];
    // sprintf(string_output,"Destinations list for Volume %d step 1",N_volume);
    // print_1d_int_list(overlap_list,string_output);
    
    // 2) remove all masks
    for (iterate=overlap_list.num_elements-1;iterate > -1;iterate--) {
        if (Volumes[overlap_list.elements[iterate]]->geometry.is_mask_volume == 1) {
            remove_element_in_list_by_index(&overlap_list,iterate);
        }
    }
    
    // 3) remove all Volumes from the overlap list of N, which is also on the intersect_check_list
    struct pointer_to_1d_int_list removed_under_2;
    removed_under_2.num_elements = 0;
    int to_check;
    removed_under_2.elements = malloc( Volumes[N_volume]->geometry.intersect_check_list.num_elements * sizeof(int));
    if (!removed_under_2.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_destinations_lists 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate < Volumes[N_volume]->geometry.intersect_check_list.num_elements;iterate++) {
        to_check = Volumes[N_volume]->geometry.intersect_check_list.elements[iterate];
        if (on_int_list(overlap_list,to_check)) {
            removed_under_2.elements[removed_under_2.num_elements++] = to_check;
            remove_element_in_list_by_value(&overlap_list,to_check);
        }
    }
    
    // sprintf(string_output,"Destinations list for Volume %d step 2",N_volume);
    // print_1d_int_list(overlap_list,string_output);
    
    // 4) remove the children of volumes removed in step 2)
    int children;
    for (iterate=0;iterate<removed_under_2.num_elements;iterate++){
        for (children = 0;children < Volumes[removed_under_2.elements[iterate]]->geometry.children.num_elements;children++) {
          remove_element_in_list_by_value(&overlap_list,Volumes[removed_under_2.elements[iterate]]->geometry.children.elements[children]);
        }
    }
    
    // sprintf(string_output,"Destinations list for Volume %d step 3",N_volume);
    // print_1d_int_list(overlap_list,string_output);
    
    // 5) remove the children of N
    for (children = 0;children < Volumes[N_volume]->geometry.children.num_elements;children++) {
        remove_element_in_list_by_value(&overlap_list,Volumes[N_volume]->geometry.children.elements[children]);
    }
    
    // sprintf(string_output,"Destinations list for Volume %d step 4",N_volume);
    // print_1d_int_list(overlap_list,string_output);
    
    // 6) remove the grandparents of N
    int grandparent;
    for (grandparent = 0;grandparent < grandparent_list->num_elements;grandparent++) {
        remove_element_in_list_by_value(&overlap_list,grandparent_list->elements[grandparent]);
    }
    
    // sprintf(string_output,"Destinations list for Volume %d step 5",N_volume);
    // print_1d_int_list(overlap_list,string_output);
    
    // 7) remove volumes with lower priority than parents of N still on the list
    struct pointer_to_1d_int_list logic_list;
    logic_list.num_elements = overlap_list.num_elements;
    logic_list.elements=NULL;
    if (logic_list.num_elements>0) {
      logic_list.elements = malloc(logic_list.num_elements*sizeof(int));
    }
    if (!logic_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_destinations_lists 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    for (iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 1;
    
    int parent;
    for (parent=0;parent<parent_list->num_elements;parent++) {
      if (on_int_list(overlap_list,parent_list->elements[parent])) {
        // Found a parent to N on the list, now check all other elements on the list
        for (iterate=0;iterate<overlap_list.num_elements;iterate++) {
          if (parent_list->elements[parent] != overlap_list.elements[iterate]) {
            // if the element iterate have lower priority than the parent, remove it from the list
            if (Volumes[overlap_list.elements[iterate]]->geometry.priority_value < Volumes[parent_list->elements[parent]]->geometry.priority_value) logic_list.elements[iterate] = 0;
          }
        }
      }
    }
    
    // 8) The remaing list is the destinations list
    Volumes[N_volume]->geometry.destinations_list.num_elements = sum_int_list(logic_list);
    Volumes[N_volume]->geometry.destinations_list.elements = malloc(Volumes[N_volume]->geometry.destinations_list.num_elements * sizeof(int));
    if (!Volumes[N_volume]->geometry.destinations_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_destinations_lists 4 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    
    int overlap_index,used_elements=0;
    for (overlap_index=0;overlap_index < overlap_list.num_elements;overlap_index++)
      if (logic_list.elements[overlap_index] == 1)
        Volumes[N_volume]->geometry.destinations_list.elements[used_elements++] = overlap_list.elements[overlap_index];
    
    if (overlap_list.num_elements>0) free(overlap_list.elements);
    if (logic_list.num_elements>0) free(logic_list.elements);

    // Clean up memory
    free(removed_under_2.elements);

    };

void generate_destinations_lists(struct pointer_to_1d_int_list **grandparents_lists, struct pointer_to_1d_int_list **parents_lists, struct pointer_to_1d_int_list **overlap_lists,struct Volume_struct **Volumes, int number_of_volumes, int verbal) {
    // Because of the complexity of the algortithm for generating the destinations list, the function is made for a single volume at the time to keep the notation simpler
    // This funtion runs the destinations list function for each volume
    MPI_MASTER(
    if (verbal) printf("\nGenerating destinations lists ----------------------- \n");
    )
    
    int volume_index;
    for (volume_index = 0;volume_index < number_of_volumes;volume_index++) {
      generate_destinations_list(volume_index,Volumes,*overlap_lists[volume_index],parents_lists[volume_index],grandparents_lists[volume_index]);
      
      char string_output[128];
      if (verbal) sprintf(string_output,"Destinations list for Volume %d",volume_index);
      MPI_MASTER(
      if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.destinations_list,string_output);
      )
    }
};


void generate_reduced_destinations_lists(struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes,int number_of_volumes,int verbal) {
    // The reduced destination list is the destination list of a volume, where each element that has a parent on the same destination list is removed
    // This list is to be fed to the which_volume function, as this funtion will automatically search through the direct children in a tree like manner
    // The optimization reduces the number of calculations of within functions in nested geometries.
    
    MPI_MASTER(
    if (verbal) printf("\nGenerating reduced destination lists ----------------------- \n");
    )
    
    struct pointer_to_1d_int_list logic_list;
  
    int volume_index,checked_dest_index,checked_dest_volume,rest_dest_index,rest_dest_volume,dest_index,iterate;
    for (volume_index = 0;volume_index < number_of_volumes;volume_index++) {
      
      //printf("Generating reduced destinations lists for volume %d\n",volume_index);
      logic_list.num_elements = Volumes[volume_index]->geometry.destinations_list.num_elements;
      logic_list.elements = malloc( (int) logic_list.num_elements * sizeof(int));
      if (!logic_list.elements) {
	fprintf(stderr,"Failure allocating list in Union function generate_reduced_destinations_lists 1 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      
      //printf("Sucsessfully allocated space for %d integers in logic list %d\n",logic_list.num_elements,volume_index);
      for (iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 1;
      
      for (checked_dest_index=0;checked_dest_index<Volumes[volume_index]->geometry.destinations_list.num_elements;checked_dest_index++) {
            checked_dest_volume = Volumes[volume_index]->geometry.destinations_list.elements[checked_dest_index];
            for (rest_dest_index=0;rest_dest_index<Volumes[volume_index]->geometry.destinations_list.num_elements;rest_dest_index++) {
                rest_dest_volume = Volumes[volume_index]->geometry.destinations_list.elements[rest_dest_index];
                // As every volume has 0 as a parent, these are ignored. It would work to include this, but would add an extra trivial step to within_which_volume
                if (rest_dest_volume != 0) {
                    if (on_int_list(*parents_lists[checked_dest_volume],rest_dest_volume) == 1) {
                        // In this case, do not include element checked_dest_index on the reduced destinations list
                        // ADD mask check
                        if (Volumes[rest_dest_volume]->geometry.is_masked_volume == 0) {
                          logic_list.elements[checked_dest_index] = 0;
                        }
                    }
                }
            }
      }
      
      Volumes[volume_index]->geometry.reduced_destinations_list.num_elements = sum_int_list(logic_list);
      Volumes[volume_index]->geometry.reduced_destinations_list.elements = malloc((int)Volumes[volume_index]->geometry.reduced_destinations_list.num_elements * sizeof(int));
      if (!Volumes[volume_index]->geometry.reduced_destinations_list.elements) {
	fprintf(stderr,"Failure allocating list in Union function generate_reduced_destinations_lists 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      
      iterate = 0;
      for (dest_index = 0;dest_index < Volumes[volume_index]->geometry.destinations_list.num_elements;dest_index++) {
            if (logic_list.elements[dest_index] == 1)   Volumes[volume_index]->geometry.reduced_destinations_list.elements[iterate++] = Volumes[volume_index]->geometry.destinations_list.elements[dest_index];
      }
      
      free(logic_list.elements);
      
      // Testing an optimization
      remove_element_in_list_by_value(&Volumes[volume_index]->geometry.reduced_destinations_list,0);
      
      char string_output[128];
      MPI_MASTER(
      if (verbal) sprintf(string_output,"Reduced destinations list for Volume %d",volume_index);
      if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.reduced_destinations_list,string_output);
      )
    }
};

void generate_direct_children_lists(struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes,int number_of_volumes,int verbal) {

  MPI_MASTER(
  if (verbal) printf("\nGenerating direct children lists ----------------------- \n");
  )
  // A direct children of volume n is a volume that is a child of n, but no other child of n is its parent
  
  // Mask update: Need to check that this step does not bug out when the mask system interferes with child/parent systems
  
  struct pointer_to_1d_int_list logic_list;
  int volume_index,child,parent,iterate;
  for (volume_index = 0;volume_index < number_of_volumes;volume_index++) {
        // Temp elements is used, and its actual number of elements is edited even though the memory allocated is not changed. This is so that the list functions handles it correctly.
        // The free function will free all the allocated memory regardless of the value of the .num_elements structure field, it is just there for convinience.
      
        logic_list.num_elements = Volumes[volume_index]->geometry.children.num_elements;
        logic_list.elements = malloc(logic_list.num_elements * sizeof(int));
	if (!logic_list.elements) {
	  fprintf(stderr,"Failure allocating list in Union function generate_direct_children_lists 1 - Exit!\n");
	  exit(EXIT_FAILURE);
	}
        for (iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 1;
      
        // Look through each child of volume n, and for each of those look through all the other children of n and search for one of them being a parent to the child
        for (child=0;child<Volumes[volume_index]->geometry.children.num_elements;child++) {
            for (parent=0;parent<parents_lists[Volumes[volume_index]->geometry.children.elements[child]]->num_elements;parent++) {
                if (on_int_list(Volumes[volume_index]->geometry.children,parents_lists[Volumes[volume_index]->geometry.children.elements[child]]->elements[parent]))
                    // If such a parent is found, remove that child from the list
                    logic_list.elements[child] = 0;
            }
        }
      
      Volumes[volume_index]->geometry.direct_children.num_elements = sum_int_list(logic_list);
      Volumes[volume_index]->geometry.direct_children.elements = malloc(Volumes[volume_index]->geometry.direct_children.num_elements*sizeof(int));
      if (!Volumes[volume_index]->geometry.direct_children.elements) {
	fprintf(stderr,"Failure allocating list in Union function generate_direct_children_lists 2 - Exit!\n");
	exit(EXIT_FAILURE);
      }
      
      iterate = 0;
      for (child = 0;child < Volumes[volume_index]->geometry.children.num_elements;child++) {
            if (logic_list.elements[child])   Volumes[volume_index]->geometry.direct_children.elements[iterate++] = Volumes[volume_index]->geometry.children.elements[child];
      }
      // Be careful with names in both main and a function, as they are automatically declared as external variables, which would then also free the main.
      free(logic_list.elements);
      
      char string_output[128];
      MPI_MASTER(
      if (verbal) sprintf(string_output,"Children list for Volume        %d",volume_index);
      if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.children,string_output);
      if (verbal) sprintf(string_output,"Direct_children list for Volume %d",volume_index);
      if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.direct_children,string_output);
      )
      
  }

};

void generate_starting_logic_list(struct starting_lists_struct *starting_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal) {
    // Function for generating logic list of volumes the ray can start in without an error.
    // Start with a list of all vacuum volumes
    // Remove all volumes that are children of non-vacuum volumes
    // It is still possible to have a volume on this list that is surrounded by non-vacuum volumes, but it is hard to detect these situations,
    //  meaning that it is ultimately partly the users responsibility to not send neutrons directly into materials.
    
    int volume_index,*start,*check;
    
    struct pointer_to_1d_int_list temp_list_local;
    temp_list_local.num_elements = number_of_volumes;
    temp_list_local.elements = malloc(number_of_volumes*sizeof(int));
    if (!temp_list_local.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_starting_logic_list - Exit!\n");
      exit(EXIT_FAILURE);
    }
    
    temp_list_local.elements[0] = 1; // Volume 0 is a vacuum volume.
    for (volume_index = 1;volume_index < number_of_volumes;volume_index++) {
        if (Volumes[volume_index]->p_physics->is_vacuum == 1) temp_list_local.elements[volume_index] = 1;
        else temp_list_local.elements[volume_index] = 0;
    }
    // temp_list_local is now a logic list of all vacuum volumes
    for (volume_index = 1;volume_index < number_of_volumes;volume_index++) { // All volumes ...
        if (temp_list_local.elements[volume_index] == 0) { // ... that are not vacuum ...
            for (start = check = Volumes[volume_index]->geometry.children.elements;check - start < Volumes[volume_index]->geometry.children.num_elements;check++) { // ... have all their children ...
                temp_list_local.elements[*check] = 0; // .. removed from the allowed_start_logic_list
            }
        }
    }
    allocate_list_from_temp(number_of_volumes,temp_list_local,&starting_lists->allowed_starting_volume_logic_list);
    
    free(temp_list_local.elements);
    //if (verbal==1) printf("sucessfully freed temp_list_local.elements, generate starting lists done\n");
    
    char string_output[128];
    MPI_MASTER(
    if (verbal) sprintf(string_output,"Allowed starting volume logic list");
    if (verbal) print_1d_int_list(starting_lists->allowed_starting_volume_logic_list,string_output);
    )
    
    
};

void generate_reduced_starting_destinations_list(struct starting_lists_struct *starting_lists, struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes,int number_of_volumes,int verbal) {
    // The starting_destinations_list is trivial, as it contains all volumes that are not masks.


    struct pointer_to_1d_int_list logic_list;
    //printf("Generating reduced destinations lists for volume %d\n",volume_index);
    logic_list.num_elements = number_of_volumes;
    logic_list.elements = malloc( (int) logic_list.num_elements * sizeof(int));
    if (!logic_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_reduced_starting_destinations_list 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    int iterate;
    for (iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 0;
    
    
    for (iterate=1;iterate<number_of_volumes;iterate++)
      if (Volumes[iterate]->geometry.is_mask_volume == 0) logic_list.elements[iterate] = 1;
    
    starting_lists->starting_destinations_list.num_elements = sum_int_list(logic_list);
    starting_lists->starting_destinations_list.elements = malloc(starting_lists->starting_destinations_list.num_elements*sizeof(int));
    if (!starting_lists->starting_destinations_list.elements) {
      fprintf(stderr,"Failure allocating list in Union function generate_reduced_starting_destinations_list 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    int used_elements=0;
    for (iterate=1;iterate<number_of_volumes;iterate++)
      if (logic_list.elements[iterate] == 1) starting_lists->starting_destinations_list.elements[used_elements++] = iterate;
    
    MPI_MASTER(
    if (verbal) printf("\nGenerating start destinations list ------------------------------ \n");
    if (verbal) print_1d_int_list(starting_lists->starting_destinations_list,"Starting destinations list");
    )

    // The reduced starting destination list is used when a ray enters the component in the search for which volume it starts in.
    // It facilitates the same optimization as the regular destination list.
    // The start logic list is also generated, as it is very simple and does not need a seperate function
    
    // Mask update: Need to remove mask volumes from the reduced starting destination list
    
    MPI_MASTER(
    if (verbal) printf("\nGenerating reduced start destination list ----------------------- \n");
    )
    
    int checked_dest_index,checked_dest_volume,rest_dest_index,rest_dest_volume,dest_index;
        
    logic_list.num_elements = starting_lists->starting_destinations_list.num_elements;
    free(logic_list.elements);
    logic_list.elements = malloc( (int) logic_list.num_elements * sizeof(int));
    
    for (iterate=0;iterate<logic_list.num_elements;iterate++) logic_list.elements[iterate] = 1;
  
    for (checked_dest_index=0;checked_dest_index<starting_lists->starting_destinations_list.num_elements;checked_dest_index++) {
        checked_dest_volume = starting_lists->starting_destinations_list.elements[checked_dest_index];
        for (rest_dest_index=0;rest_dest_index<starting_lists->starting_destinations_list.num_elements;rest_dest_index++) {
            rest_dest_volume = starting_lists->starting_destinations_list.elements[rest_dest_index];
            // As every volume has 0 as a parent, these are ignored. It would work to include this, but would add an extra trivial step to within_which_volume
            if (rest_dest_volume != 0) {
                if (on_int_list(*parents_lists[checked_dest_volume],rest_dest_volume) == 1) {
                    // In this case, do not include element checked_dest_index on the reduced destinations list
                    logic_list.elements[checked_dest_index] = 0;
                }
            }
        }
  }
  
  starting_lists->reduced_start_list.num_elements = sum_int_list(logic_list);
  starting_lists->reduced_start_list.elements = malloc((int)starting_lists->reduced_start_list.num_elements * sizeof(int));
  if (!starting_lists->reduced_start_list.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_reduced_starting_destinations_list 3 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  iterate = 0;
  for (dest_index = 0;dest_index < starting_lists->starting_destinations_list.num_elements;dest_index++) {
        if (logic_list.elements[dest_index] == 1) starting_lists->reduced_start_list.elements[iterate++] = starting_lists->starting_destinations_list.elements[dest_index];
  }
  
  free(logic_list.elements);
  
  MPI_MASTER(
  if (verbal) print_1d_int_list(starting_lists->reduced_start_list,"Reduced start destinations list");
  )
    
  // Making the start_logic_list.
  starting_lists->start_logic_list.num_elements = number_of_volumes;
  starting_lists->start_logic_list.elements = malloc ( starting_lists->start_logic_list.num_elements * sizeof(int));
  if (!starting_lists->start_logic_list.elements) {
    fprintf(stderr,"Failure allocating list in Union function generate_reduced_starting_destinations_list 4 - Exit!\n");
    exit(EXIT_FAILURE);
  }
  starting_lists->start_logic_list.elements[0] = 0;
  for (iterate=1;iterate<number_of_volumes;iterate++) starting_lists->start_logic_list.elements[iterate] = 1; // All volumes to be checked for starting volume
  MPI_MASTER(
  if (verbal) print_1d_int_list(starting_lists->start_logic_list,"Start logic list");
  )
};


void generate_next_volume_list(struct Volume_struct **Volumes, int number_of_volumes, int verbal) {
    // Generate list of volumes that can be the next volume which the ray enters. It is used for tagging, not the simulation / propagation

    // Mask update: These lists should be done as if all mask statuses are on, meaning they include all possible next volumes (will include more input to this function)
    
    // bug: next volume list is not complete and contains duplicates

    MPI_MASTER(
    if (verbal) printf("\nGenerating next volume list ------------------------------------- \n");
    )
    // Merge destinations list, intersection list and mask_intersection_list
    int volume_index,iterate,mask_index;
    struct pointer_to_1d_int_list full_intersection_list;
    full_intersection_list.num_elements=0;
    
    for (volume_index=0;volume_index<number_of_volumes;volume_index++) {
        Volumes[volume_index]->geometry.next_volume_list.num_elements = 0;
        // Before mask update
        //merge_lists(&Volumes[volume_index]->geometry.next_volume_list, &Volumes[volume_index]->geometry.destinations_list, &Volumes[volume_index]->geometry.intersect_check_list);
        
        merge_lists(&full_intersection_list, &Volumes[volume_index]->geometry.mask_intersect_list, &Volumes[volume_index]->geometry.intersect_check_list);
        merge_lists(&Volumes[volume_index]->geometry.next_volume_list,&Volumes[volume_index]->geometry.destinations_list,&full_intersection_list);
        
        /* // This complication is taken into account by adding the mask_intersect list instead
        // It is possible that the next volume is still not on this list, as when masks are encountered the next volume can be a volume they mask.
        // For each on the list, add the volumes masked by that mask (do not iterate over this, as masks can not be masked regardless)
        for (iterate=Volumes[volume_index]->geometry.next_volume_list.num_elements-1;iterate>-1;iterate--) {
          if (Volumes[Volumes[volume_index]->geometry.next_volume_list.elements[iterate]]->geometry.is_mask_volume == 1) {
            for (mask_index=0;mask_index<Volumes[Volumes[volume_index]->geometry.next_volume_list.elements[iterate]]->geometry.mask_list.num_elements;mask_index++) {
              add_element_to_int_list(&Volumes[volume_index]->geometry.next_volume_list,Volumes[Volumes[volume_index]->geometry.next_volume_list.elements[iterate]]->geometry.mask_list.elements[mask_index]);
            }
          }
        }
        */
        
        // Remove mask volumes from the next volume list, as they never occur as current_volume, and thus just create dead branches that takes up memory
        for (iterate=Volumes[volume_index]->geometry.next_volume_list.num_elements-1;iterate>-1;iterate--)
          if (Volumes[Volumes[volume_index]->geometry.next_volume_list.elements[iterate]]->geometry.is_mask_volume == 1)
            remove_element_in_list_by_index(&Volumes[volume_index]->geometry.next_volume_list,iterate);
        
        
        if (full_intersection_list.num_elements>0) free(full_intersection_list.elements);
        full_intersection_list.num_elements=0;
        
        char string_output[128];
        MPI_MASTER(
        if (verbal) sprintf(string_output,"Next volume list                %d",volume_index);
        if (verbal) print_1d_int_list(Volumes[volume_index]->geometry.next_volume_list,string_output);
        )
    }
};

void generate_lists(struct Volume_struct **Volumes, struct starting_lists_struct *starting_lists, int number_of_volumes, int verbal) {
    // Function to control the generation of lists
    // Some lists are only needed temporary, and are thus declared here to keep them out of the main scope
    // Others are stored in the volume structs as they are needed in the trace algorithm (or tagging)
    
    
    struct pointer_to_1d_int_list **true_children_lists;
    true_children_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!true_children_lists) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 1 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    // generate_children_lists both generate the normal children list for each volume, but also the true children list needed locally.
    generate_children_lists(Volumes, true_children_lists, number_of_volumes,verbal);
    
    
    struct pointer_to_1d_int_list **true_overlap_lists;
    true_overlap_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!true_overlap_lists) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 2 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    struct pointer_to_1d_int_list **raw_overlap_lists;
    raw_overlap_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!raw_overlap_lists) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 3 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_overlap_lists(true_overlap_lists, raw_overlap_lists, Volumes,number_of_volumes,verbal);
    
    
    //generate_intersect_check_lists(true_overlap_lists, Volumes, number_of_volumes, verbal);
    
    
    struct pointer_to_1d_int_list **parents_lists;
    parents_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!parents_lists) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 4 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_parents_lists(parents_lists,Volumes,number_of_volumes,verbal,1); // The last 1 means masks are taken into account
    
    // Generate version of parent list as it would be without masks
    struct pointer_to_1d_int_list **parents_lists_no_masks;
    parents_lists_no_masks = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!parents_lists_no_masks) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 5 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_parents_lists(parents_lists_no_masks,Volumes,number_of_volumes,verbal,0); // The last 0 means masks are NOT taken into account
    
    // Generate version of parent list using true_children instead
    struct pointer_to_1d_int_list **true_parents_lists;
    true_parents_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!true_parents_lists) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 6 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_true_parents_lists(true_parents_lists, true_children_lists, Volumes, number_of_volumes, verbal, 1);
    
    // Generate version of parent list no masks using true_children instead
    struct pointer_to_1d_int_list **true_parents_lists_no_masks;
    true_parents_lists_no_masks = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!true_parents_lists_no_masks) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 7 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_true_parents_lists(true_parents_lists_no_masks, true_children_lists, Volumes, number_of_volumes, verbal, 0);
    
    // New version of generate intersect lists
    generate_intersect_check_lists_experimental(true_overlap_lists, raw_overlap_lists, parents_lists, true_parents_lists, Volumes, number_of_volumes, verbal);
    
    struct pointer_to_1d_int_list **grandparents_lists;
    grandparents_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!grandparents_lists) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 8 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_grandparents_lists(grandparents_lists,parents_lists,number_of_volumes,verbal);
    
    // Generate version of grandparents list as it would have been if no masks were defined
    struct pointer_to_1d_int_list **grandparents_lists_no_masks;
    grandparents_lists_no_masks = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!grandparents_lists_no_masks) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 9 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_grandparents_lists(grandparents_lists_no_masks,parents_lists_no_masks,number_of_volumes,verbal);
    
    // Generate true_grandparents_lists
    struct pointer_to_1d_int_list **true_grandparents_lists;
    true_grandparents_lists = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!true_grandparents_lists) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 10 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_grandparents_lists(true_grandparents_lists,true_parents_lists,number_of_volumes,verbal);
    
    struct pointer_to_1d_int_list **true_grandparents_lists_no_masks;
    true_grandparents_lists_no_masks = malloc(number_of_volumes*sizeof(struct pointer_to_1d_int_list*));
    if (!true_grandparents_lists_no_masks) {
      fprintf(stderr,"Failure allocating list in Union function generate_lists 11 - Exit!\n");
      exit(EXIT_FAILURE);
    }
    generate_grandparents_lists(true_grandparents_lists_no_masks,true_parents_lists_no_masks,number_of_volumes,verbal);
    
    // The destinations lists are generated without taking masks into account (they are removed from the overlap list in an early step)
    //generate_destinations_lists(grandparents_lists_no_masks,parents_lists_no_masks,true_overlap_lists,Volumes,number_of_volumes,verbal);
    generate_destinations_lists_experimental(true_overlap_lists, true_children_lists, true_parents_lists_no_masks, true_grandparents_lists_no_masks, Volumes, number_of_volumes, verbal);
    
    // Obsolete, found a way around them in within_which_volume, but need to test the performance difference
    // generate_destinations_logic_lists(Volumes,number_of_volumes,verbal);
    
    generate_reduced_destinations_lists(parents_lists,Volumes,number_of_volumes,verbal);
    
    generate_direct_children_lists(parents_lists,Volumes,number_of_volumes,verbal);
    
    //generate_starting_list(starting_lists,Volumes,number_of_volumes,verbal);
    generate_starting_logic_list(starting_lists,Volumes,number_of_volumes,verbal);
    
    generate_reduced_starting_destinations_list(starting_lists,parents_lists,Volumes,number_of_volumes,verbal);
    
    // This list is stored with the volumes for convinience, but is only used for tagging
    generate_next_volume_list(Volumes,number_of_volumes,verbal);
    
    // Garbage collection for temporary dynamically allocated lists. (Permanent lists freed from FINALLY)
    int iterate;
    for (iterate=0;iterate<number_of_volumes;iterate++) {
        //printf("freeing for volume nr %d\n",iterate);
        //printf("true_overlap_lists[iterate]->num_elements = %d \n",true_overlap_lists[iterate]->num_elements);
        if (true_overlap_lists[iterate]->num_elements > 0) free(true_overlap_lists[iterate]->elements);
        free(true_overlap_lists[iterate]);
        
        //printf("raw_overlap_lists[iterate]->num_elements = %d \n",raw_overlap_lists[iterate]->num_elements);
        if (raw_overlap_lists[iterate]->num_elements > 0) free(raw_overlap_lists[iterate]->elements);
        free(raw_overlap_lists[iterate]);
        
        //printf("parents_lists[iterate]->num_elements = %d \n",parents_lists[iterate]->num_elements);
        if (parents_lists[iterate]->num_elements > 0) free(parents_lists[iterate]->elements);
        free(parents_lists[iterate]);
        
        //printf("parents_lists_no_masks[iterate]->num_elements = %d \n",parents_lists_no_masks[iterate]->num_elements);
        if (parents_lists_no_masks[iterate]->num_elements > 0) free(parents_lists_no_masks[iterate]->elements);
        free(parents_lists_no_masks[iterate]);
        
        //printf("true_parents_lists[iterate]->num_elements = %d \n",true_parents_lists[iterate]->num_elements);
        if (true_parents_lists[iterate]->num_elements > 0) free(true_parents_lists[iterate]->elements);
        free(true_parents_lists[iterate]);
        
        //printf("true_parents_lists_no_masks[iterate]->num_elements = %d \n",true_parents_lists_no_masks[iterate]->num_elements);
        if (true_parents_lists_no_masks[iterate]->num_elements > 0) free(true_parents_lists_no_masks[iterate]->elements);
        free(true_parents_lists_no_masks[iterate]);
        
        //printf("grandparents_lists[iterate]->num_elements = %d \n",grandparents_lists[iterate]->num_elements);
        if (grandparents_lists[iterate]->num_elements > 0) free(grandparents_lists[iterate]->elements);
        free(grandparents_lists[iterate]);
        
        //printf("true_grandparents_lists[iterate]->num_elements = %d \n",true_grandparents_lists[iterate]->num_elements);
        if (true_grandparents_lists[iterate]->num_elements > 0) free(true_grandparents_lists[iterate]->elements);
        free(true_grandparents_lists[iterate]);
        
        //printf("grandparents_lists_no_masks[iterate]->num_elements = %d \n",grandparents_lists_no_masks[iterate]->num_elements);
        if (grandparents_lists_no_masks[iterate]->num_elements > 0) free(grandparents_lists_no_masks[iterate]->elements);
        free(grandparents_lists_no_masks[iterate]);
        
        //printf("true_grandparents_lists_no_masks[iterate]->num_elements = %d \n",true_grandparents_lists_no_masks[iterate]->num_elements);
        if (true_grandparents_lists_no_masks[iterate]->num_elements > 0) free(true_grandparents_lists_no_masks[iterate]->elements);
        free(true_grandparents_lists_no_masks[iterate]);
        
        //printf("true_children_lists[iterate]->num_elements = %d \n",true_children_lists[iterate]->num_elements);
        if (true_children_lists[iterate]->num_elements > 0) free(true_children_lists[iterate]->elements);
        free(true_children_lists[iterate]);
    }
    //printf("generate lists volume specific free completed\n");
    free(true_overlap_lists);free(raw_overlap_lists);free(parents_lists);free(true_parents_lists);free(true_parents_lists_no_masks);
    free(parents_lists_no_masks);free(true_grandparents_lists);free(grandparents_lists);free(grandparents_lists_no_masks);free(true_grandparents_lists_no_masks);
    free(true_children_lists);
    //printf("generate lists free completed\n");
};

// -------------    Focusing functions   --------------------------------------------------------

// The focusing_data structure is set up by the geometry component, and a pointer to the appropriate
//  focusing function is added to the Volume structure (for this reason the input of all the functions
//  need to be identical, at least in terms of types). In this way there are no if statements to check
//  which of these to be used in the trace, but the focus_data_struct will carry some redundant
//  information, as only the appropriate parameters are set:
// Angular focus on a rectangle (angular_focus_height / angular_focus_width)
// Spatial focus on a rectangle (spatial_focus_height / spatial_focus_width)
// Spatial focus on a disk (sptial_focus_radius)
// No focus (randvec in 4pi) (all set to zero, will select randvec circle as it is slightly faster
//
// When adding a new physical process focusing becomes very easy, as one just calls the master focusing
//  function assosiated with the volume (placed in the geometry struct), using the focus_data_struct
//  also found in the geometry struct, and the process then supports all the focusing modes. It is even
//  possible to add new focusing modes in the future by updating just the geometry components, and this
//  section.

// focus_data_struct definitioon shown here, defined at the start of this file
//struct focus_data_struct {
//Coords Aim;
//double angular_focus_width;
//double angular_focus_height;
//double spatial_focus_width;
//double spatial_focus_height;
//double spatial_focus_radius;
//Rotation absolute_rotation;
//// focusing_function creates a vector per selected criteria of focus_data_struct / selected focus function and returns solid angle
//void (*focusing_function)(Coords*, double*, struct focus_data_struct*);
////                        v_out  , solid_a,
//};

void randvec_target_rect_angular_union(Coords *v_out,double *solid_angle_out, struct focus_data_struct *focus_data) {
    // Calls the standard McStas randvec_target_rect_angular focusing function, but is with the new data input format.
    randvec_target_rect_angular(&v_out->x, &v_out->y, &v_out->z, solid_angle_out, focus_data->RayAim.x,focus_data->RayAim.y, focus_data->RayAim.z, focus_data->angular_focus_width, focus_data->angular_focus_height,focus_data->absolute_rotation);
    //randvec_target_rect_angular(&vx, &vy, &vz, &solid_angle,aim_x, aim_y, aim_z, VarsInc.aw, VarsInc.ah, ROT_A_CURRENT_COMP);
};

void randvec_target_rect_union(Coords *v_out,double *solid_angle_out, struct focus_data_struct *focus_data) {
// Calls the standard McStas randvec_target_rect focusing function, but is with the new data input format.
    randvec_target_rect(&v_out->x, &v_out->y, &v_out->z, solid_angle_out, focus_data->RayAim.x,focus_data->RayAim.y, focus_data->RayAim.z, focus_data->spatial_focus_width, focus_data->spatial_focus_height,focus_data->absolute_rotation);
    // randvec_target_rect(&vx, &vy, &vz, &solid_angle,aim_x, aim_y, aim_z, VarsInc.xw, VarsInc.yh, ROT_A_CURRENT_COMP);
};

void randvec_target_circle_union(Coords *v_out,double *solid_angle_out, struct focus_data_struct *focus_data) {
// Calls the standard McStas randvec_target_circle focusing function, but is with the new data input format.

    // debug input into randvec_target_circle
    //print_position(focus_data->Aim,"Aim vector input for randvec_target_circle");
    //printf("Radius input %f\n",focus_data->spatial_focus_radius);

    randvec_target_circle(&v_out->x, &v_out->y, &v_out->z, solid_angle_out, focus_data->RayAim.x,focus_data->RayAim.y, focus_data->RayAim.z, focus_data->spatial_focus_radius);
    //randvec_target_circle(&vx, &vy, &vz, &solid_angle, aim_x, aim_y, aim_z, focus_r);
};



void focus_initialize(struct geometry_struct *geometry, Coords POS_A_TARGET, Coords POS_A_CURRENT, Rotation ROT_A_CURRENT, int target_index, double target_x, double target_y, double target_z, double angular_focus_width, double angular_focus_height, double spatial_focus_width, double spatial_focus_height, double spatial_focus_radius, char *component_name) {
    // Initialize focusing system
    // target_x/y/z needs to be double setting parameters with default value 0
    // target_index needs to be int setting parameter with default value 0
    // angular_focus_width, angular_focus_height, spatial_focus_width, spatial_focus_height, spatial_focus_radius nneds to be double setting parameters with default value 0
    // When those conditions are met, this code will identify which settings have been entered by the user and select the appropriate focusing parameters, which are loaded into the focus_data struct and the geometry struct.
    // The aim vector in the focus_data struct will be transformed from the local coordinate system of the geometry component to the coordinate system of the master component during the master component initialize

  // Input sanitation
  if (angular_focus_width < 0) {
    printf("\nERROR in Union geometry component named \"%s\", angular focus width focus_aw < 0! \n",component_name);
    exit(EXIT_FAILURE);
  }
  if (angular_focus_height < 0) {
    printf("\nERROR in Union geometry component named \"%s\", angular focus width focus_ah < 0! \n",component_name);
    exit(EXIT_FAILURE);
  }
  if (spatial_focus_width < 0) {
    printf("\nERROR in Union geometry component named \"%s\", spatial focus width focus_xw < 0! \n",component_name);
    exit(EXIT_FAILURE);
  }
  if (spatial_focus_height < 0) {
    printf("\nERROR in Union geometry component named \"%s\", spatial focus height focus_xh < 0! \n",component_name);
    exit(EXIT_FAILURE);
  }
  if (spatial_focus_radius < 0) {
    printf("\nERROR in Union geometry component named \"%s\", spatial focus radius focus_r < 0! \n",component_name);
    exit(EXIT_FAILURE);
  }
  
  struct focus_data_struct focus_data;

  // Initialize focus_data_struct
  focus_data.Aim = coords_set(0,0,0);
  focus_data.RayAim = coords_set(0,0,0);
  focus_data.angular_focus_width = 0;
  focus_data.angular_focus_height = 0;
  focus_data.spatial_focus_width = 0;
  focus_data.spatial_focus_height = 0;
  focus_data.spatial_focus_radius = 0;
  rot_copy(focus_data.absolute_rotation,ROT_A_CURRENT);

  // Built on code from Incoherent.comp by Kim Lefmann and Kristian Nielsen
  if (target_index != 0 && !target_x && !target_y && !target_z)
  {
    Coords ToTarget;
    //ToTarget = coords_sub(POS_A_COMP_INDEX(INDEX_CURRENT_COMP+target_index),POS_A_CURRENT_COMP);
    ToTarget = coords_sub(POS_A_TARGET, POS_A_CURRENT);
    ToTarget = rot_apply(ROT_A_CURRENT, ToTarget);
    coords_get(ToTarget, &focus_data.Aim.x, &focus_data.Aim.y, &focus_data.Aim.z);
  }
  else
  { focus_data.Aim.x = target_x; focus_data.Aim.y = target_y; focus_data.Aim.z = target_z; }
  
  if (!(focus_data.Aim.x || focus_data.Aim.y || focus_data.Aim.z)) {
    // Somehow set a variable to signify scattering into 4pi
    // printf("Union %s: The target is not defined. Using scattering into 4pi.\n",NAME_CURRENT_COMP);
    focus_data.Aim.z=1; // set aim to one so that the randvec output vector has length 1 instead of 0
  }

  int focusing_model_selected = 0;
  if (angular_focus_width != 0 && angular_focus_height != 0) {
    focus_data.focusing_function = &randvec_target_rect_angular_union;
    focus_data.angular_focus_width = DEG2RAD*angular_focus_width; // Convert to radians here
    focus_data.angular_focus_height = DEG2RAD*angular_focus_height;
    focusing_model_selected = 1;
  }
  if (spatial_focus_width != 0 && spatial_focus_height != 0) {
    focus_data.focusing_function = &randvec_target_rect_union;
    focus_data.spatial_focus_width = spatial_focus_width;
    focus_data.spatial_focus_height = spatial_focus_height;
    if (focusing_model_selected) {
        printf("ERROR %s: Select either angular or spatial focusing, not both! Exiting \n",component_name);
        exit(EXIT_FAILURE);
    }
    focusing_model_selected = 1;
  }
  if (spatial_focus_radius != 0) {
    focus_data.focusing_function = &randvec_target_circle_union;
    focus_data.spatial_focus_radius = spatial_focus_radius;
    if (focusing_model_selected) {
        printf("ERROR %s: Select a maximum of one focusing method (spatial rectangle or cicle, or angular rectangle! Exiting \n",component_name);
        exit(EXIT_FAILURE);
    }
    focusing_model_selected = 1;
  }
  if (focusing_model_selected == 0) {
    // Select 4pi focusing
    focus_data.spatial_focus_radius = 0;
    focus_data.focusing_function = &randvec_target_circle_union;
  }
  
  // Allocate the isotropic focus_data struct
  geometry->focus_data_array.num_elements = 0;
  add_element_to_focus_data_array(&geometry->focus_data_array,focus_data);
};


struct abs_event{
    double time1;
    double position1[3];
    double time2;
    double position2[3];
    double weight_change;
    int volume_index;
    int neutron_id;
};

// Functions for recording absorption
void initialize_absorption_file() {
  FILE *fp;
  fp = fopen("Union_absorption.dat","w");
  if(!fp) {
    fprintf(stderr,"WARNING: Could not write initial output to Union_absorption.dat\n");
  } else {
    fprintf(fp,"r_old x, r_old y, r_old z, old t, r x, r y, r z, new t, weight change, volume index, neutron id \n");
    fclose(fp);
  }
}

void write_events_to_file(int last_index, struct abs_event *events) {

  FILE *fp;
  fp = fopen("Union_absorption.dat","a");
  if(!fp) {
    fprintf(stderr,"WARNING: Could not write logging output to Union_absorption.dat\n");
  } else {
    struct abs_event *this_event;
    int iterate;
    for (iterate=0; iterate<last_index; iterate++) {
      
      this_event = &events[iterate];
      fprintf(fp,"%g, %g, %g, %g, %g, %g, %g, %g, %e, %i, %i \n",
	      this_event->position1[0], this_event->position1[1], this_event->position1[2], this_event->time1,
	      this_event->position2[0], this_event->position2[1], this_event->position2[2], this_event->time2,
	      this_event->weight_change,
	      this_event->volume_index,
	      this_event->neutron_id);
    }
    fclose(fp);
  }
}

void record_abs_to_file(double *r, double t1, double *r_old, double t2, double weight_change, int volume, int neutron_id, int *data_index, struct abs_event *events) {
  
  
  
  struct abs_event *this_event;
  
  this_event = &events[(*data_index)++];
  
  //printf("Recording something! %i\n", *data_index);
  
  this_event->position1[0] = r[0];
  this_event->position1[1] = r[1];
  this_event->position1[2] = r[2];
  this_event->time1 = t1;
  this_event->position2[0] = r_old[0];
  this_event->position2[1] = r_old[1];
  this_event->position2[2] = r_old[2];
  this_event->time2 = t2;
  this_event->weight_change = weight_change;
  this_event->volume_index = volume;
  this_event->neutron_id = neutron_id;
  
  
  if (*data_index == 999) {
    
      write_events_to_file(*data_index, events);
      *data_index = 0;

  }

};

void manual_linking_function_surface(char *input_string, struct pointer_to_global_surface_list *global_surface_list, struct pointer_to_1d_int_list *accepted_surfaces, char *component_name) {

   char *token;
   int loop_index;
   char local_string[256];
   
   strcpy(local_string, input_string);
   
   // get the first token
   token = strtok(local_string,",");
   
   // walk through tokens
   while(token != NULL) 
   {
      //printf( " %s\n", token );
      for (loop_index=0; loop_index<global_surface_list->num_elements; loop_index++) {
        if (strcmp(token, global_surface_list->elements[loop_index].name) == 0) {
          add_element_to_int_list(accepted_surfaces, loop_index);
          break;
        } 

        if (loop_index == global_surface_list->num_elements - 1) {
          // All possible surface names have been looked through, and the break was not executed.
          // Alert the user to this problem by showing the surface name that was not found and the currently available surface definitions
            printf("\n");
            printf("ERROR: The surface string \"%s\" in Union geometry \"%s\" had an entry that did not match a specified surface definition. \n", input_string, component_name);
            printf("       The unrecoignized surface name was: \"%s\" \n",token);
            printf("       The surfaces available at this point (need to be defined before the geometry): \n");
            for (loop_index=0; loop_index<global_surface_list->num_elements; loop_index++)
              printf("         %s\n",global_surface_list->elements[loop_index].name);
            exit(EXIT_FAILURE);
        }
      }
      
      // Updates the token
      token = strtok(NULL,",");
   }
}

void fill_surface_stack(char *input_string, struct pointer_to_global_surface_list *global_surface_list, char *component_name,
                        struct surface_stack_struct *surface_stack) {
	// Takes empty surface_stack struct, allocates the memory and fills it with appropriate pointers to the surfaces requested in input_string
				   
	if (input_string && strlen(input_string) && strcmp(input_string, "NULL") && strcmp(input_string, "0") && strcmp(input_string, "None")) {
		
	  struct pointer_to_1d_int_list accepted_surfaces;
	  accepted_surfaces.num_elements = 0;
	  
	  manual_linking_function_surface(input_string, global_surface_list, &accepted_surfaces, component_name);
	  
	  surface_stack->number_of_surfaces = accepted_surfaces.num_elements;
	  surface_stack->p_surface_array = malloc(surface_stack->number_of_surfaces*sizeof(struct surface_process_struct*));
	  if (!surface_stack->p_surface_array) {
	    fprintf(stderr,"Failure allocating list in Union function fill_surface_stack - Exit!\n");
	    exit(EXIT_FAILURE);
	  }
	
	  int loop_index;
  	  for (loop_index=0; loop_index<accepted_surfaces.num_elements; loop_index++) {
	      surface_stack->p_surface_array[loop_index]=global_surface_list->elements[accepted_surfaces.elements[loop_index]].p_surface_process;
	  }
		
	} else {
	  surface_stack->number_of_surfaces = 0;
	}

}

void overwrite_if_empty(char *input_string, char *overwrite) {
   if (!(input_string && strlen(input_string) && strcmp(input_string, "NULL") && strcmp(input_string, "0"))) {
	   strcpy(input_string, overwrite);
   }
}

