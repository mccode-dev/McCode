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
 * Functions and structure definitons for Union components.
 *
 ******************************************************************************/

// Declarations for the Union component library; the function definitions are
// in union-lib.c. Every Union component requests the library with
//   %include "union-lib"
// which each McCode code generator emits once per instrument: this header
// ahead of all component SHARE sections, and union-lib.c either straight
// after it or after the SHARE sections.
#ifndef UNION_LIB_H
#define UNION_LIB_H

// McXtrace Union structures hold t_Table
%include "read_table-lib"

// Lets code test whether the Union library is loaded.
#ifndef Union
#define Union 1
#endif

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
  Compton_xrl,
  KN_xrl,
  Rayleigh_xrl,
  Powder,
  Single_crystal,
  NCrystal,
  Template
};

// -------------    Dispatch registry   -----------------------------------------------------------
// OpenACC device code cannot call through function pointers, so a process is
// selected by switching on its enum value. Each case is supplied by
// the component that defines the process: its SHARE section replaces one of
// the empty defaults below with a real case label, e.g.
//   #undef  UNION_CASE_PHYSICS_MY_POWDER
//   #define UNION_CASE_PHYSICS_MY_POWDER(out, ...) case Powder: out = Powder_physics_my(__VA_ARGS__); break;
// Macros expand where they are used, not where they are defined. The
// UNION_PHYSICS_* macros are only used in Union_master TRACE code, which every
// code generator emits after all component SHARE sections, so each switch sees
// every process in the instrument without help from the code generator.
// Adding a process means adding one empty default per dispatch here.
#define UNION_CASE_PHYSICS_MY_INCOHERENT(out, ...)
#define UNION_CASE_PHYSICS_MY_COMPTON_XRL(out, ...)
#define UNION_CASE_PHYSICS_MY_KN_XRL(out, ...)
#define UNION_CASE_PHYSICS_MY_RAYLEIGH_XRL(out, ...)
#define UNION_CASE_PHYSICS_MY_POWDER(out, ...)
#define UNION_CASE_PHYSICS_MY_SINGLE_CRYSTAL(out, ...)
#define UNION_CASE_PHYSICS_MY_NCRYSTAL(out, ...)
#define UNION_CASE_PHYSICS_MY_TEMPLATE(out, ...)

#define UNION_CASE_PHYSICS_SCATTERING_INCOHERENT(out, ...)
#define UNION_CASE_PHYSICS_SCATTERING_COMPTON_XRL(out, ...)
#define UNION_CASE_PHYSICS_SCATTERING_KN_XRL(out, ...)
#define UNION_CASE_PHYSICS_SCATTERING_RAYLEIGH_XRL(out, ...)
#define UNION_CASE_PHYSICS_SCATTERING_POWDER(out, ...)
#define UNION_CASE_PHYSICS_SCATTERING_SINGLE_CRYSTAL(out, ...)
#define UNION_CASE_PHYSICS_SCATTERING_NCRYSTAL(out, ...)
#define UNION_CASE_PHYSICS_SCATTERING_TEMPLATE(out, ...)

#define UNION_CASES_PHYSICS_MY(out, ...) \
  UNION_CASE_PHYSICS_MY_INCOHERENT(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_MY_COMPTON_XRL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_MY_KN_XRL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_MY_RAYLEIGH_XRL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_MY_POWDER(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_MY_SINGLE_CRYSTAL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_MY_NCRYSTAL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_MY_TEMPLATE(out, __VA_ARGS__)

#define UNION_CASES_PHYSICS_SCATTERING(out, ...) \
  UNION_CASE_PHYSICS_SCATTERING_INCOHERENT(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_SCATTERING_COMPTON_XRL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_SCATTERING_KN_XRL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_SCATTERING_RAYLEIGH_XRL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_SCATTERING_POWDER(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_SCATTERING_SINGLE_CRYSTAL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_SCATTERING_NCRYSTAL(out, __VA_ARGS__) \
  UNION_CASE_PHYSICS_SCATTERING_TEMPLATE(out, __VA_ARGS__)

// A process that defines no case (e.g. a user's own process component written
// before these macros existed) falls back to its function pointer on the CPU.
// Device code cannot call through a function pointer, so an OpenACC build
// reports the missing case instead.
#ifdef OPENACC
#define UNION_PROCESS_FALLBACK(out, name, call) \
  printf("%s: no Union dispatch case for this scattering process\n", name); \
  out = 0;
#else
#define UNION_PROCESS_FALLBACK(out, name, call) out = call;
#endif

// Statement macros used by Union_master TRACE; `out` receives the return value.
#define UNION_PHYSICS_MY(out, process, my, k_initial, focus_data, particle) \
  switch ((process)->eProcess) { \
    UNION_CASES_PHYSICS_MY(out, my, k_initial, (process)->data_transfer, focus_data, particle) \
    default: \
      UNION_PROCESS_FALLBACK(out, "physics_my", \
        (process)->probability_for_scattering_function(my, k_initial, (process)->data_transfer, focus_data, particle)) \
      break; \
  }

#define UNION_PHYSICS_SCATTERING(out, process, k_final, k_initial, weight, focus_data, particle) \
  switch ((process)->eProcess) { \
    UNION_CASES_PHYSICS_SCATTERING(out, k_final, k_initial, weight, (process)->data_transfer, focus_data, particle) \
    default: \
      UNION_PROCESS_FALLBACK(out, "physics_scattering", \
        (process)->scattering_function(k_final, k_initial, weight, (process)->data_transfer, focus_data, particle)) \
      break; \
  }

struct intersection_time_table_struct {
int num_volumes;
int *calculated;
int *n_elements;
double **intersection_times;
};

struct line_segment{
//struct position point1;
//struct position point2;
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
Coords Aim;
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
  double bins_1; // McXtrace uses doubles for bin numbers for some reason
  double bins_2;
  double bins_3;
  double ***Array_N; // McXtrace uses doubles for number of rays in each bin for some reason
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
  double bins_1; // McXtrace uses doubles for bin numbers for some reason
  double bins_2;
  double **Array_N; // McXtrace uses doubles for number of rays in each bin for some reason
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
  double bins; // McXtrace uses doubles for bin numbers for some reason
  double *Array_N; // McXtrace uses doubles for number of rays in each bin for some reason
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
  // Contains pointers to all the functions assosiated with this logger
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
double geometry_p_interact; // fraction of rays that interact with this volume for each scattering (between 0 and 1, 0 for disable)
union geometry_parameter_union geometry_parameters; // relevant parameters for this shape
union geometry_parameter_union (*copy_geometry_parameters)(union geometry_parameter_union*);
struct focus_data_struct focus_data; // Used for focusing from this geometry

// New focus data implementation to remove the focusing bug for non-isotripic processes
struct focus_data_array_struct focus_data_array;
struct pointer_to_1d_int_list focus_array_indices; // Add 1D integer array with indecies for correct focus_data for each process


// intersect_function takes position/velocity of ray and parameters, returns time list
int (*intersect_function)(double*,int*,double*,double*,struct geometry_struct*);
//                        t_array,n_ar,r      ,v

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
};

struct physics_struct
{
  char name[256]; // User defined material name
  int interact_control;
  int is_vacuum;
  double my_a;
  int number_of_elements;
  // pointer to element data structures
  struct element_data_struct *p_element_array;
  int number_of_processes;
  // pointer to array of pointers to physics_sub structures that each describe a scattering process
  struct scattering_process_struct *p_scattering_array;
};

union data_transfer_union{
    // List of pointers to storage structs for all supported physical processes
    struct Incoherent_physics_storage_struct  *pointer_to_a_Incoherent_physics_storage_struct;
    struct Powder_physics_storage_struct *pointer_to_a_Powder_physics_storage_struct;
    struct Single_crystal_physics_storage_struct *pointer_to_a_Single_crystal_physics_storage_struct;
    struct Template_physics_storage_struct *pointer_to_a_Template_physics_storage_struct;
    struct Compton_xrl_physics_storage_struct *pointer_to_a_Compton_xrl_physics_storage_struct;
    struct KN_xrl_physics_storage_struct *pointer_to_a_KN_xrl_physics_storage_struct;
    struct Rayleigh_xrl_physics_storage_struct *pointer_to_a_Rayleigh_xrl_physics_storage_struct;
    // possible to add as many structs as wanted, without increasing memory footprint.
};


struct scattering_process_struct
{
char name[256];                // User defined process name
enum process eProcess;         // enum value corresponding to this process GPU
double process_p_interact;     // double between 0 and 1 that describes the fraction of events forced to undergo this process. -1 for disable
int non_isotropic_rot_index;   // -1 if process is isotrpic, otherwise is the index of the process rotation matrix in the volume
Rotation rotation_matrix;      // rotation matrix of process, reported by component in local frame, transformed and moved to volume struct in main

union data_transfer_union data_transfer; // The way to reach the storage space allocated for this process (see examples in process.comp files)

// probability_for_scattering_functions calculates this probability given k_i and parameters
int (*probability_for_scattering_function)(double*,double*,union data_transfer_union,struct focus_data_struct*, _class_particle *_particle);
//                                         prop,   k_i,   ,parameters               , focus data / function

// A scattering_function takes k_i and parameters, returns k_f
int (*scattering_function)(double*,double*,double*,union data_transfer_union,struct focus_data_struct*, _class_particle *_particle);
//                         k_f,    k_i,    weight, parameters               , focus data / function
};

//this object stores data relevant for absorption
struct element_data_struct
{
  char name[12];  //element name (leave room for ion things as well
  int multiplicity; // how many atoms are present per unit of the element
  double rho,Ar,Z; //mass density, Atomic weight, and atomic number
  t_Table element_table; // material constants table taken from database
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

struct global_positions_to_transform_list_struct {
int num_elements;
Coords **positions;
};

struct global_rotations_to_transform_list_struct {
int num_elements;
Rotation **rotations;
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


// -------------    Physics functions   ---------------------------------------------------------

//#include "Test_physics.c"
//#include "Incoherent_test.c"

// -------------    General functions   ---------------------------------------------------------
double distance_between(Coords position1,Coords position2);



double length_of_3vector(double *r);

double length_of_position_vector(Coords point);

Coords make_position(double *r);

Coords coords_scalar_mult(Coords input,double scalar);

double union_coords_dot(Coords vector1,Coords vector2);


int sum_int_list(struct pointer_to_1d_int_list list);

int on_int_list(struct pointer_to_1d_int_list list,int target);

int find_on_int_list(struct pointer_to_1d_int_list list,int target);

void on_both_int_lists(struct pointer_to_1d_int_list *list1, struct pointer_to_1d_int_list *list2, struct pointer_to_1d_int_list *common);

void remove_element_in_list_by_index(struct pointer_to_1d_int_list *list,int index);

void remove_element_in_list_by_value(struct pointer_to_1d_int_list *list,int value);

void merge_lists(struct pointer_to_1d_int_list *result,struct pointer_to_1d_int_list *list1,struct pointer_to_1d_int_list *list2);

void add_element_to_double_list(struct pointer_to_1d_double_list *list,double value);

void add_element_to_int_list(struct pointer_to_1d_int_list *list,int value);

// Need to check if absolute_rotation is preserved correctly.
void add_element_to_focus_data_array(struct focus_data_array_struct *focus_data_array,struct focus_data_struct focus_data);


void add_to_logger_with_data(struct logger_with_data_struct *logger_with_data, struct logger_struct *logger);

void add_to_abs_logger_with_data(struct abs_logger_with_data_struct *abs_logger_with_data, struct abs_logger_struct *abs_logger);


// Used typedef to avoid having to change this function later. May update others to use same phillosphy.
void add_function_to_conditional_list(struct conditional_list_struct *list,conditional_function_pointer new, union conditional_data_union *data_union);


// could make function that removes a element from a 1d_int_list, and have each list generation as a function that takes a copy of an overlap list

void print_1d_int_list(struct pointer_to_1d_int_list list,char *name);

void print_1d_double_list(struct pointer_to_1d_double_list list,char *name);

void print_position(Coords pos,char *name);

void print_rotation(Rotation rot, char *name);

void allocate_list_from_temp(int num_elements,struct pointer_to_1d_int_list original,struct pointer_to_1d_int_list *new);

void allocate_logic_list_from_temp(int num_elements,struct pointer_to_1d_int_list original, struct pointer_to_1d_int_list *new);

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

void add_position_pointer_to_list(struct global_positions_to_transform_list_struct *list, Coords *new_position_pointer);

void add_rotation_pointer_to_list(struct global_rotations_to_transform_list_struct *list, Rotation *new_rotation_pointer);

void add_element_to_process_list(struct pointer_to_global_process_list *list,struct global_process_element_struct new_element);

void add_element_to_material_list(struct pointer_to_global_material_list *list,struct global_material_element_struct new_element);

void add_element_to_geometry_list(struct pointer_to_global_geometry_list *list,struct global_geometry_element_struct new_element);

void add_element_to_logger_list(struct pointer_to_global_logger_list *list,struct global_logger_element_struct new_element);

void add_element_to_abs_logger_list(struct pointer_to_global_abs_logger_list *list, struct global_abs_logger_element_struct new_element);

void add_element_to_tagging_conditional_list(struct global_tagging_conditional_list_struct *list,struct global_tagging_conditional_element_struct new_element);

void add_element_to_master_list(struct pointer_to_global_master_list *list,struct global_master_element_struct new_element);

void add_initialized_logger_in_volume(struct loggers_struct *loggers,int number_of_processes);


void add_initialized_abs_logger_in_volume(struct abs_loggers_struct *abs_loggers);


// -------------    Functions used to shorten master trace    ---------------------------------------------


void update_current_mask_intersect_status(struct pointer_to_1d_int_list *current_mask_intersect_list_status, struct pointer_to_1d_int_list *mask_status_list, struct Volume_struct **Volumes, int *current_volume);

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

struct tagging_tree_node_struct *make_tagging_tree_node(void);


struct tagging_tree_node_struct  *simple_initialize_tagging_tree_node(struct tagging_tree_node_struct *new_node);


struct tagging_tree_node_struct *initialize_tagging_tree_node(struct tagging_tree_node_struct *new_node, struct tagging_tree_node_struct *above_node, struct Volume_struct *this_volume);

struct tagging_tree_node_struct *goto_process_node(struct tagging_tree_node_struct *current_node, int process_index, struct Volume_struct *this_volume, int *stop_tagging_ray, int stop_creating_nodes);

struct tagging_tree_node_struct *goto_volume_node(struct tagging_tree_node_struct *current_node,int current_volume, int next_volume, struct Volume_struct **Volumes, int *stop_tagging_ray, int stop_creating_nodes);

void add_statistics_to_node(struct tagging_tree_node_struct *current_node, Coords *r, Coords *v, double *weight, int *counter);

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


void add_to_history(struct dynamic_history_list *history, int volume_index, int process_index);

void printf_history(struct dynamic_history_list *history);

void fprintf_total_history(struct saved_history_struct *history, FILE *fp);

int Sample_compare_history_intensities (const void* a, const void* b);

void write_tagging_tree(struct list_of_tagging_tree_node_pointers *master_list, struct Volume_struct **Volumes, int total_history_counter, int number_of_volumes);


// -------------    Intersection table functions   --------------------------------------------------------
int clear_intersection_table(struct intersection_time_table_struct *intersection_time_table);

void print_intersection_table(struct intersection_time_table_struct *intersection_time_table);

// -------------    Drawing functions   --------------------------------------------------------
void merge_lines_to_draw(struct lines_to_draw *lines_master,struct lines_to_draw *lines_new);

int r_has_highest_priority(Coords point,int N,struct geometry_struct **Geometries,int number_of_volumes);

void draw_line_positions(Coords point1,Coords point2);

int Sample_compare_doubles (const void *a, const void *b);

struct lines_to_draw draw_line_with_highest_priority(Coords position1,Coords position2,int N,struct geometry_struct **Geometries,int number_of_volumes,int max_number_of_solutions);

struct lines_to_draw draw_circle_with_highest_priority(Coords center,Coords vector,double radius,int N,struct geometry_struct **Geometries,int number_of_volumes,int max_number_of_solutions);

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
Coords transform_position(Coords ray_position, Coords component_position, Rotation component_t_rotation);


union geometry_parameter_union allocate_box_storage_copy(union geometry_parameter_union *union_input);


union geometry_parameter_union allocate_cylinder_storage_copy(union geometry_parameter_union *union_input);

union geometry_parameter_union allocate_sphere_storage_copy(union geometry_parameter_union *union_input);

union geometry_parameter_union allocate_cone_storage_copy(union geometry_parameter_union *union_input);

union geometry_parameter_union allocate_mesh_storage_copy(union geometry_parameter_union *union_input);

// -------------    Surroundings  ---------------------------------------------------------------
int r_within_surroundings(Coords pos,struct geometry_struct *geometry);

// -------------    General geometry ------------------------------------------------------------

Coords point_on_circle(Coords center, Coords direction, double radius, int point_nr, int number_of_points);

void points_on_circle(Coords *output, Coords center, Coords direction, double radius, int number_of_points);

// -------- Brute force last resorts for within / overlap -------------------------------------

int A_within_B(struct geometry_struct *child, struct geometry_struct *parent, int resolution);

int mesh_A_within_B(struct geometry_struct *child, struct geometry_struct *parent);

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
int sample_box_intersect_advanced(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

void box_corners_global_frame(Coords *corner_points, struct geometry_struct *geometry);

void box_corners_local_frame(Coords *corner_points, struct geometry_struct *geometry);

int sample_box_intersect_simple(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

int r_within_box_simple(Coords pos,struct geometry_struct *geometry);

int r_within_cone(Coords pos,struct geometry_struct *geometry);

int sample_cone_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

int cone_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

int r_within_mesh(Coords pos,struct geometry_struct *geometry);


int sample_mesh_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

int r_within_box_advanced(Coords pos,struct geometry_struct *geometry);

// -------------    Functions for box ray tracing used in initialize ----------------------------
// These functions does not need to be fast, as they are only used once
int box_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int existence_of_intersection(Coords point1, Coords point2, struct geometry_struct *geometry);

int box_overlaps_box(struct geometry_struct *geometry1,struct geometry_struct *geometry2);


// -------------    Functions for sphere ray tracing used in trace ------------------------------
// These functions needs to be fast, as they may be used many times for each ray
int sample_sphere_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

int r_within_sphere(Coords pos,struct geometry_struct *geometry);

// -------------    Functions for sphere ray tracing used in initialize -------------------------
// These functions does not need to be fast, as they are only used once
int sphere_overlaps_sphere(struct geometry_struct *geometry1,struct geometry_struct *geometry2);

int sphere_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

// -------------    Functions for cylinder ray tracing used in trace ------------------------------
// These functions needs to be fast, as they may be used many times for each ray
int sample_cylinder_intersect(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

int r_within_cylinder(Coords pos,struct geometry_struct *geometry);

// -------------    Functions for cylinder ray tracing used in initialize -------------------------
// These functions does not need to be fast, as they are only used once
int cylinder_overlaps_cylinder(struct geometry_struct *geometry1,struct geometry_struct *geometry2);

int cylinder_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cylinder_within_cylinder_backup(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cone_overlaps_cone(struct geometry_struct *geometry1,struct geometry_struct *geometry2);

int cone_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int mesh_overlaps_mesh(struct geometry_struct *geometry1,struct geometry_struct *geometry2);

int mesh_within_mesh(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

// -------------    Overlap functions for two different geometries --------------------------------

int box_overlaps_cylinder(struct geometry_struct *geometry_box,struct geometry_struct *geometry_cyl);

int cylinder_overlaps_box(struct geometry_struct *geometry_cyl,struct geometry_struct *geometry_box);

int cylinder_overlaps_sphere(struct geometry_struct *geometry_cyl,struct geometry_struct *geometry_sph);

int box_overlaps_sphere(struct geometry_struct *geometry_box,struct geometry_struct *geometry_sph);


// sym sphere
int sphere_overlaps_cylinder(struct geometry_struct *geometry_sph,struct geometry_struct *geometry_cyl);

int sphere_overlaps_box(struct geometry_struct *geometry_sph,struct geometry_struct *geometry_box);

int cone_overlaps_sphere(struct geometry_struct *geometry_cone,struct geometry_struct *geometry_sph);

int sphere_overlaps_cone(struct geometry_struct *geometry_sph,struct geometry_struct *geometry_cone);

int cone_overlaps_cylinder(struct geometry_struct *geometry_cone,struct geometry_struct *geometry_cylinder);

int cylinder_overlaps_cone(struct geometry_struct *geometry_cylinder,struct geometry_struct *geometry_cone);

int cone_overlaps_box(struct geometry_struct *geometry_cone,struct geometry_struct *geometry_box);

int box_overlaps_cone(struct geometry_struct *geometry_box,struct geometry_struct *geometry_cone);

// -------------    Within functions for two different geometries ---------------------------------

double dist_from_point_to_plane(Coords point,Coords plane_p1, Coords plane_p2, Coords plane_p3);

int box_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cylinder_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cylinder_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int sphere_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int box_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int sphere_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cone_within_sphere(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cone_within_cylinder(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cone_within_box(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int sphere_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int cylinder_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);

int box_within_cone(struct geometry_struct *geometry_child,struct geometry_struct *geometry_parent);


// Flexible intersection function
int intersect_function(double *t,int *num_solutions,double *r,double *v,struct geometry_struct *geometry);

// Flexible within function
int r_within_function(Coords pos,struct geometry_struct *geometry);


// -------------    List generator functions   --------------------------------------------------


int within_which_volume(Coords pos, struct pointer_to_1d_int_list input_list, struct pointer_to_1d_int_list destinations_list, struct Volume_struct **Volumes, struct pointer_to_1d_int_list *mask_status_list, int number_of_volumes, int *volume_logic_copy, int *ListA, int *ListB);

int within_which_volume_GPU(Coords pos, struct pointer_to_1d_int_list input_list, struct pointer_to_1d_int_list destinations_list, struct Volume_struct **Volumes, struct pointer_to_1d_int_list *mask_status_list, int number_of_volumes, int *volume_logic_copy, int *ListA, int *ListB);




int within_which_volume_debug(Coords pos, struct pointer_to_1d_int_list input_list, struct pointer_to_1d_int_list volume_logic, struct Volume_struct **Volumes, int *volume_logic_copy, int *ListA, int *ListB);

int inside_function(struct Volume_struct *parent_volume, struct Volume_struct *child_volume);

void generate_children_lists(struct Volume_struct **Volumes, struct pointer_to_1d_int_list **true_children_lists, int number_of_volumes, int verbal);

void generate_overlap_lists(struct pointer_to_1d_int_list **true_overlap_lists, struct pointer_to_1d_int_list **raw_overlap_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal);

void add_to_mask_intersect_list(struct pointer_to_1d_int_list *mask_intersect_list, int given_volume_index);


void generate_intersect_check_lists(struct pointer_to_1d_int_list **overlap_lists,struct Volume_struct **Volumes, int number_of_volumes, int verbal);

void generate_parents_lists(struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal, int mask_mode);

void generate_true_parents_lists(struct pointer_to_1d_int_list **parents_lists, struct pointer_to_1d_int_list **true_children_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal, int mask_mode);

void generate_intersect_check_lists_experimental(struct pointer_to_1d_int_list **true_overlap_lists, struct pointer_to_1d_int_list **raw_overlap_lists, struct pointer_to_1d_int_list **parents_lists, struct pointer_to_1d_int_list **true_parents_lists , struct Volume_struct **Volumes, int number_of_volumes, int verbal);

void generate_grandparents_lists(struct pointer_to_1d_int_list **grandparents_lists, struct pointer_to_1d_int_list **parents_lists, int number_of_volumes, int verbal);


void generate_destinations_lists_experimental(struct pointer_to_1d_int_list **true_overlap_lists, struct pointer_to_1d_int_list **true_children_lists, struct pointer_to_1d_int_list **true_parents_lists, struct pointer_to_1d_int_list **true_grandparents_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal);

void generate_destinations_list(int N_volume,struct Volume_struct **Volumes,struct pointer_to_1d_int_list original_overlap_list,struct pointer_to_1d_int_list *parent_list, struct pointer_to_1d_int_list *grandparent_list);

void generate_destinations_lists(struct pointer_to_1d_int_list **grandparents_lists, struct pointer_to_1d_int_list **parents_lists, struct pointer_to_1d_int_list **overlap_lists,struct Volume_struct **Volumes, int number_of_volumes, int verbal);


void generate_reduced_destinations_lists(struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes,int number_of_volumes,int verbal);

void generate_direct_children_lists(struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes,int number_of_volumes,int verbal);

void generate_starting_logic_list(struct starting_lists_struct *starting_lists, struct Volume_struct **Volumes, int number_of_volumes, int verbal);

void generate_reduced_starting_destinations_list(struct starting_lists_struct *starting_lists, struct pointer_to_1d_int_list **parents_lists, struct Volume_struct **Volumes,int number_of_volumes,int verbal);


void generate_next_volume_list(struct Volume_struct **Volumes, int number_of_volumes, int verbal);

void generate_lists(struct Volume_struct **Volumes, struct starting_lists_struct *starting_lists, int number_of_volumes, int verbal);

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

void randvec_target_rect_angular_union(Coords *v_out,double *solid_angle_out, struct focus_data_struct *focus_data);

void randvec_target_rect_union(Coords *v_out,double *solid_angle_out, struct focus_data_struct *focus_data);

void randvec_target_circle_union(Coords *v_out,double *solid_angle_out, struct focus_data_struct *focus_data);



void focus_initialize(struct geometry_struct *geometry, Coords POS_A_TARGET, Coords POS_A_CURRENT, Rotation ROT_A_CURRENT, int target_index, double target_x, double target_y, double target_z, double angular_focus_width, double angular_focus_height, double spatial_focus_width, double spatial_focus_height, double spatial_focus_radius, char *component_name);


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
void initialize_absorption_file();

void write_events_to_file(int last_index, struct abs_event *events);

void record_abs_to_file(double *r, double t1, double *r_old, double t2, double weight_change, int volume, int neutron_id, int *data_index, struct abs_event *events);

// -------------    Shared Union state   ----------------------------------------------------------
// Union components communicate through lists that every component appends to
// in INITIALIZE and each Union_master consumes. The lists live in one instance
// of this struct. Every Union component calls union_acquire() in INITIALIZE
// and union_release() in FINALLY; the last release checks for components that
// no Union_master consumed and frees the lists, whatever order the FINALLY
// sections run in.
struct union_state_struct {
  int refcount; // components currently holding the state

  struct global_positions_to_transform_list_struct u_positions_to_transform_list;
  struct global_rotations_to_transform_list_struct u_rotations_to_transform_list;
  struct pointer_to_global_process_list u_process_list;
  struct pointer_to_global_material_list u_material_list;
  struct pointer_to_global_geometry_list u_geometry_list;
  struct pointer_to_global_logger_list u_all_volume_logger_list;
  struct pointer_to_global_logger_list u_specific_volumes_logger_list;
  struct pointer_to_global_abs_logger_list u_all_volume_abs_logger_list;
  struct pointer_to_global_abs_logger_list u_specific_volumes_abs_logger_list;
  struct global_tagging_conditional_list_struct u_tagging_conditional_list;
  struct pointer_to_global_master_list u_master_list;
};

// Zero-initialised: every list starts empty.
struct union_state_struct union_state;

struct union_state_struct *union_acquire_state(int master_present);

// Union components do nothing without a Union_master (or Union_master_GPU),
// whose SHARE section defines the identifier below. Components call
// union_acquire() in INITIALIZE, which every code generator emits after all
// SHARE sections, so an instrument without a master fails to compile with an
// error naming this identifier.
#define union_acquire() union_acquire_state(Union_components_need_a_Union_master_in_the_instrument)

// A Union_master consumes the components placed before it, so anything placed
// after the last master never takes part.
void union_report_unconsumed(const char *name, int component_index, int last_master_index);

void union_check_unconsumed(void);

void union_free_conditional_list(struct conditional_list_struct *list);

// The transform lists are not freed here: each Union_master frees them once it
// has applied them.
void union_free_state(void);

void union_release(const char *comp_name);

#endif /* UNION_LIB_H */
