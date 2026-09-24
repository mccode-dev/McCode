/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2008, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Runtime: share/interoff-lib.h
*
* %Identification
* Written by: Reynald Arnerin
* Date:    Jun 12, 2008
* Release:
* Version:
*
* Object File Format intersection header for McStas/McXtrace.
*
* Such files may be obtained with e.g.
*   qhull < points.xyz Qx Qv Tv o > points.off
* where points.xyz has format:
*   3
*   <nb_points>
*   <x> <y> <z>
*   ...
* The resulting file should have its first line being changed from '3' into 'OFF'.
* It can then be displayed with geomview.
* A similar, but somewhat older solution is to use 'powercrust' with e.g.
*   powercrust -i points.xyz
* which will generate a 'pc.off' file to be renamed as suited.
*
* Supported file formats
*   OFF (and the NOFF/COFF/CNOFF/STOFF variants, extra vertex data is ignored)
*   PLY in 'format ascii' (x y z must be the first three vertex properties)
*
* Polygons may have any number (>=3) of vertices.
*
* Per-face properties (e.g. reflectivity)
*   Any numbers following the vertex indices of a face, on the same line, are
*   stored as per-face properties:
*     nv  i1 i2 ... inv   p0 p1 p2 ...
*   They are available as data.facePropArray (column-major, see off_struct)
*   or through off_face_prop(&data, face, k, default).
*   Reflecting components (Guide_anyshape_r) use the convention
*     p0 = m,  p1 = alpha [AA],  p2 = W [AA^-1]
*   for which the convenience pointers face_m_Array, face_alpha_Array and
*   face_W_Array are set when at least 3 properties are present.
*   The face index returned by off_intersect_idx/off_x_intersect_idx is the
*   0-based order of the face in the file.
*
* This library replaces the former r-interoff-lib (which is now a thin
* compatibility wrapper around this one).
*
*******************************************************************************/

%include "read_table-lib"

#ifndef INTEROFF_LIB_H
#define INTEROFF_LIB_H "$Revision$"

#ifndef OFF_EPSILON
#define OFF_EPSILON 1e-13
#endif

/* Only used with -DOFF_LEGACY (full sorted list of intersections) */
#ifndef OFF_INTERSECT_MAX
#ifdef OPENACC
#define OFF_INTERSECT_MAX 100
#else
#define OFF_INTERSECT_MAX 1024
#endif
#endif

#define N_VERTEX_DISPLAYED    200000

/* Conventional column indices for reflectivity face properties */
#define OFF_FACEPROP_M      0
#define OFF_FACEPROP_ALPHA  1
#define OFF_FACEPROP_W      2

typedef struct intersection {
	MCNUM time;  	  //time of the intersection
	Coords v;	      //intersection point
	Coords normal;  //normal vector of the surface intersected (unit length)
	short in_out;	  //1 if the ray enters the volume, -1 otherwise
	short edge;	    //1 if the intersection is on the boundary of the polygon, and error is possible
	unsigned long index; // index of the face
} intersection;

/* A polygon is a view into the global vertex table: no copy, no size limit */
typedef struct polygon {
  Coords*        vtx;   // global vertex table (off_struct.vtxArray)
  unsigned long* idx;   // npol vertex indices into vtx (points into off_struct.faceArray)
  int            npol;  // number of vertices
  Coords         normal;// unit normal
  double         D;     // plane equation: normal . r = D
} polygon;

typedef struct off_struct {
    long vtxSize;          // number of vertices
    long polySize;         // number of faces (polygons)
    long faceSize;         // length of faceArray
    Coords* vtxArray;
    #pragma acc shape(vtxArray[0:vtxSize]) init_needed(vtxSize)
    Coords* normalArray;   // unit normal per face
    #pragma acc shape(normalArray[0:polySize]) init_needed(polySize)
    unsigned long* faceArray; // [nv i1 .. inv | nv i1 .. inv | ...]
    #pragma acc shape(faceArray[0:faceSize]) init_needed(faceSize)
    double* DArray;        // plane constant per face: normal . r = D
    #pragma acc shape(DArray[0:polySize]) init_needed(polySize)
    /* optional per-face properties, read from the numbers following the vertex
       indices of each face. Property k of face i is facePropArray[k*polySize+i] */
    int     nfaceprops;    // number of property columns (0 when none are given)
    long    facePropSize;  // = nfaceprops*polySize
    double* facePropArray;
    #pragma acc shape(facePropArray[0:facePropSize]) init_needed(facePropSize)
    /* convenience views (m, alpha, W) into facePropArray, NULL unless nfaceprops>=3 */
    double* face_m_Array;
    #pragma acc shape(face_m_Array[0:polySize]) init_needed(polySize)
    double* face_alpha_Array;
    #pragma acc shape(face_alpha_Array[0:polySize]) init_needed(polySize)
    double* face_W_Array;
    #pragma acc shape(face_W_Array[0:polySize]) init_needed(polySize)
    Coords bbmin, bbmax;   // bounding box of the (scaled) vertices
    char *filename;
    int mantidflag;
    long mantidoffset;
#ifdef OFF_LEGACY
    intersection* intersects; // After a call to off_intersect_all contains the list of intersections.
    #pragma acc shape(intersects[0:OFF_INTERSECT_MAX])
#endif
    int nextintersect;     // face index of the 'next' intersection after call to off_intersect_all
    int numintersect;      // Number of intersections after call to off_intersect_all
} off_struct;

/*******************************************************************************
* long off_init(  char *offfile, double xwidth, double yheight, double zdepth, off_struct* data)
* ACTION: read an OFF file, optionally center object and rescale, initialize OFF data structure
* INPUT: 'offfile' OFF file to read
*        'xwidth,yheight,zdepth' if given as non-zero, apply bounding box.
*           Specifying only one of these will also use the same ratio on all axes
*        'notcenter' center the object to the (0,0,0) position in local frame when set to zero
* RETURN: number of polyhedra and 'data' OFF structure
*         per-face properties (if any) are stored in data->facePropArray
*******************************************************************************/
long off_init(  char *offfile, double xwidth, double yheight, double zdepth,
                int notcenter, off_struct* data);

/*******************************************************************************
* int off_intersect_all(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct *data )
* ACTION: computes intersection of neutron trajectory with an object.
* INPUT:  x,y,z and vx,vy,vz are the position and velocity of the neutron
*         ax, ay, az are the local acceleration vector
*         data points to the OFF data structure
* RETURN: the number of polyhedral which trajectory intersects
*         t0 and t3 are the smallest incoming and outgoing intersection times
*         n0 and n3 are the corresponding (unit) normal vectors to the surface
*         data is the full OFF structure, including a list intersection type
*******************************************************************************/
#pragma acc routine
int off_intersect_all(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct *data );

/* Same as off_intersect_all, but also returns the (0-based) indices of the
   faces hit at t0 and t3 in fi0 and fi3 (NULL pointers are allowed). */
#pragma acc routine
int off_intersect_all_idx(double* t0, double* t3,
     Coords *n0, Coords *n3,
     unsigned long *fi0, unsigned long *fi3,
     double x, double y, double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct *data );

/*******************************************************************************
* int off_intersect(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct data )
* ACTION: computes intersection of neutron trajectory with an object.
* INPUT:  x,y,z and vx,vy,vz are the position and velocity of the neutron
*         ax, ay, az are the local acceleration vector
*         data points to the OFF data structure
* RETURN: the number of polyhedral which trajectory intersects
*         t0 and t3 are the smallest incoming and outgoing intersection times
*         n0 and n3 are the corresponding (unit) normal vectors to the surface
*******************************************************************************/
#pragma acc routine
int off_intersect(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct data );

/* Same as off_intersect, also returning the indices of the faces hit */
#pragma acc routine
int off_intersect_idx(double* t0, double* t3,
     Coords *n0, Coords *n3,
     unsigned long *fi0, unsigned long *fi3,
     double x, double y, double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct data );

/*****************************************************************************
* int off_x_intersect(double* l0, double* l3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double kx, double ky, double kz,
     off_struct data )
* ACTION: computes intersection of an xray trajectory with an object.
* INPUT:  x,y,z and kx,ky,kz, are spatial coordinates and wavevector of the x-ray
*         respectively. data points to the OFF data structure.
* RETURN: the number of polyhedral the trajectory intersects
*         l0 and l3 are the smallest incoming and outgoing intersection lengths
*         n0 and n3 are the corresponding (unit) normal vectors to the surface
*******************************************************************************/
#pragma acc routine
int off_x_intersect(double *l0,double *l3,
     Coords *n0, Coords *n3,
     double x,  double y,  double z,
     double kx, double ky, double kz,
     off_struct data );

/* Same as off_x_intersect, also returning the indices of the faces hit */
#pragma acc routine
int off_x_intersect_idx(double *l0,double *l3,
     Coords *n0, Coords *n3,
     unsigned long *fi0, unsigned long *fi3,
     double x,  double y,  double z,
     double kx, double ky, double kz,
     off_struct data );

/*******************************************************************************
* double off_face_prop(off_struct *data, unsigned long face, int k, double def)
* ACTION: return per-face property 'k' of face 'face', or 'def' when not defined
*******************************************************************************/
#pragma acc routine
double off_face_prop(off_struct *data, unsigned long face, int k, double def);

/*******************************************************************************
* void off_display(off_struct data)
* ACTION: display up to N_VERTEX_DISPLAYED points from the object
*******************************************************************************/
void off_display(off_struct);

/*******************************************************************************
void p_to_quadratic(Coords norm, MCNUM d, Coords acc, Coords pos, Coords vel,
                    double* teq)
* ACTION: define the quadratic for the intersection of a parabola with a plane
* INPUT: plane equation norm . r = d
*        'acc' acceleration vector
*        'vel' velocity of the particle
*        'pos' position of the particle
* RETURN: equation of parabola: teq(0) * t^2 + teq(1) * t + teq(2)
*******************************************************************************/
#pragma acc routine
void p_to_quadratic(Coords norm, MCNUM d, Coords acc, Coords pos, Coords vel,
		    double* teq);

/*******************************************************************************
int quadraticSolve(double eq[], double* x1, double* x2);
* ACTION: solves the quadratic for the roots x1 and x2
*         eq[0] * t^2 + eq[1] * t + eq[2] = 0
* INPUT: 'eq' the coefficients of the parabola
* RETURN: roots x1 and x2 and the number of solutions
*******************************************************************************/
#pragma acc routine
int quadraticSolve(double* eq, double* x1, double* x2);

#endif

/* end of interoff-lib.h */
