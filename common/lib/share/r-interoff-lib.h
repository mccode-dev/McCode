/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2008, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Runtime: share/r-interoff-lib.h
*
* %Identification
* Written by: Peter Link / Gaetano Mangiapia
* Date: Mar 10, 2017 / May 30, 2022
*
* DEPRECATED compatibility wrapper.
*
* The per-face reflectivity (m, alpha, W) support of r-interoff-lib has been
* merged into interoff-lib, which now reads any numbers following the vertex
* indices of a face as per-face properties, and provides the face indices of
* the intersected polygons through off_intersect_idx / off_x_intersect_idx.
*
* This file only maps the former r_ names onto interoff-lib, so that existing
* components keep compiling. New code should %include "interoff-lib" and use
*   off_struct, off_init, off_intersect_idx, off_intersect_all_idx,
*   off_x_intersect_idx, off_face_prop, off_display
* The data fields face_m_Array, face_alpha_Array and face_W_Array are available
* in off_struct (NULL unless the file gives at least 3 values per face).
*
*******************************************************************************/

%include "interoff-lib"

#ifndef R_INTEROFF_LIB_H
#define R_INTEROFF_LIB_H "$Revision$"

typedef intersection r_intersection;
typedef polygon      r_polygon;
typedef off_struct   r_off_struct;

#define r_off_init          off_init
#define r_off_intersect_all off_intersect_all_idx
#define r_off_intersect     off_intersect_idx
#define r_off_x_intersect   off_x_intersect_idx
#define r_off_display       off_display
#define r_p_to_quadratic    p_to_quadratic
#define r_quadraticSolve    quadraticSolve

#endif

/* end of r-interoff-lib.h */
