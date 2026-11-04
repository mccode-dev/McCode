/*******************************************************************************
*
* McCode, neutron/xray ray-tracing package
*         Copyright (C) 1997-2015, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Runtime: share/interpolation.h
*
* %Identification
* Written by: EF
* Date:    May 5th 2015
* Release: McStas X.Y/McXtrace X.Y
* Version: $Revision: 5455 $
*
* Table interpolation routines (header)
*
* Usage: Automatically embbeded in the c code whenever required, with e.g.:
*   %include "interpolation-lib"
*
* public function:
* interpolator = interpolator_load(filename, 0, 0, NULL);
*   or
* interpolator = interpolator_load(filename, space_dim, field_dim, "regular" or "kdtree");
*
* interpolator_info(interpolator);
* 
* interpolator_interpolate(interpolator, {x,y,z...}, {bx,by,bz...});
*   or 
* interpolator_interpolate3_3(interpolator, x,y,z, &bx,&by,&bz);
* 
* interpolator_save(interpolator);
*
* Example:
*   struct interpolator_struct interpolator = 
*             interpolator_load("filename", space_dim, field_dim, NULL);
*   interpolator_info(interpolator);
*   double space[space_dim]={x,y,z};
*   double field[field_dim]; // will contain interpolated values
*   interpolator_interpolate(interpolator, space, field); 
*
* Data file format:
* file is a list of rows [x,y,z...    field_x, field_y, ... ]
*                        | space ... | field  ... |
*
* ---------------------------------------------------------------------------------
*             !! Important notes on table dimensionality etc: !!
* ---------------------------------------------------------------------------------
* 1. On GPU's (NVIDIA/OpenACC) only the 'regular' interpolation method is available
*    and us hence the 'default'. A GPU-compiled instrument will exit with an error
*    if you decide to force 'kdtree' mode.
*    ('kdtree' needs the macro R_SWAP which works node connectivity/placement in
*      the loaded dataset structure - is thus not thread-safe. And difficult to
*      make 'atomic' / would require 'one file content pr. neutron'... :-( )
*
* 2. On CPU's the default is 'NULL'/0, meaning that the library will itself try to
*    evaluate if a dataset is suitable for 'regular' or 'kdtree'. You may still
*    request one of the method explicitly if this makes sense in your case.
*
* 3. 'regular' means 'quite regular indeed'... Voxels in the volume MUST be of
*    uniform size AND dimensions of the volume MUST be equal on all spatial axes.
* ---------------------------------------------------------------------------------
*/

/*******************************************************************************
 * begin declaration (.h) section
 ******************************************************************************/
%include "read_table-lib"

#ifndef INTERPOLATION_LIB_H
#define INTERPOLATION_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#ifndef INTERPOLATOR_DIMENSIONS
#define INTERPOLATOR_DIMENSIONS 3
#endif

typedef struct
{
  // This is the location of this point (space).
  short  space_dimensionality;
  double *v;    // e.g. double []
  
  // These are the values for our field at this location.
  double *data; // e.g. double []

  // This is the point index in the point list.
  int    index;
  #pragma acc shape(v[0:space_dimensionality], data[0:space_dimensionality]) init_needed(space_dimensionality)
} vertex;
 
/* This struct will store each node of our kdtree. */
typedef struct _treeNode {
  vertex   *point;
  int       depth;
  struct _treeNode *rChild;
  struct _treeNode *lChild;
  #pragma acc shape(point[0:1],rChild[0:1],lChild[0:1])
} treeNode;

struct interpolator_struct {
  char  method[256];
  long  space_dimensionality; // [x,y,z...]
  long  field_dimensionality; // [bx,by,bz...]
  long  points;
  char  filename[1024];
  treeNode *kdtree;    /* for k-d tree */
  #pragma acc shape(kdtree[0:1])
  double  *gridx;  /* each grid contains a component of the field */
  double  *gridy;
  double  *gridz;
  long prod;
  #pragma acc shape(gridx[0:prod],gridy[0:prod],gridz[0:prod]) init_needed(prod)
  double   min[INTERPOLATOR_DIMENSIONS];
  double   max[INTERPOLATOR_DIMENSIONS];
  long     bin[INTERPOLATOR_DIMENSIONS];
  double   step[INTERPOLATOR_DIMENSIONS];
  long     constant_step[INTERPOLATOR_DIMENSIONS];
};

/******************************************************************************/
// interpolator_info: print information about the interpolator
void interpolator_info(struct interpolator_struct *interpolator);
 
/*******************************************************************************
 * interpolator_load: interpolation initialiser, from point cloud
 *   returns the interpolator structure
 * The input is mainly the file name, which is a column based text format.
 * The interpolator->method is set as 'kdtree' or 'regular' as set at points load
 ******************************************************************************/ 
struct interpolator_struct *interpolator_load(char *filename, 
   long space_dimensionality, long field_dimensionality,
   char *method);
     
/*******************************************************************************
 * interpolator_interpolate: main interpolation routine.
 *   returns the 'field' value (of length interpolator->field_dimensionality)
 *   at the given 'space' location (of length interpolator->space_dimensionality)
 *   The returned array 'field' MUST be pre-allocated.
 ******************************************************************************/
#pragma acc routine
double *interpolator_interpolate(struct interpolator_struct *interpolator,
  double *space, double *field);


/*******************************************************************************
 * interpolator_interpolate3_3: main interpolation routine for 3D space
 *   returns the 'field' value (e.g. 3d)
 *   at the given 'coord' location (e.g. 3d)
 * The interpolator->method can be 'kdtree' or 'regular' as set at points load
 ******************************************************************************/
#pragma acc routine
double *interpolator_interpolate3_3(struct interpolator_struct *interpolator,
                    double  x,  double  y,  double  z,
                    double *bx, double *by, double *bz);

#endif

