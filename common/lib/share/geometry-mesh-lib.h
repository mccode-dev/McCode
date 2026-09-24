/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2026, All rights reserved
*         DTU Physics, Kongens Lyngby, Denmark
*
* Runtime: share/geometry-mesh-lib.h
*
* %Identification
* Written by: Peter Willendrup, based on the Union_mesh component (Martin Olsen,
*             Daniel Lomholt Christensen) and interoff-lib (Reynald Arnerin,
*             Emmanuel Farhi)
* Date: Sep 2026
* Origin: DTU
*
* Triangle-mesh geometry library: read, check, and ray-trace closed or open
* surface meshes.
*
* File formats
*   STL  ASCII and binary (binary detected from the file size, so binary files
*        whose 80-byte header starts with "solid" are handled correctly)
*   OFF  OFF/NOFF/COFF/CNOFF/STOFF, polygons of any size, optional per-face
*        numbers after the vertex indices are kept as face properties
*   PLY  ascii, binary_little_endian and binary_big_endian, any extra
*        elements/properties are skipped; scalar face properties are kept
*   (STEP is not supported: export the geometry as STL, OFF or PLY)
*
* What the library does at init
*   - optional scaling (e.g. 1e-3 for files in mm), re-sizing and centring
*   - welds duplicated vertices (STL stores 3 vertices per triangle)
*   - triangulates polygons (ear clipping, works for non-convex polygons)
*   - computes unit normals, area, volume and mesh topology: number of edges,
*     boundary / non-manifold edges, Euler characteristic, consistent
*     orientation. Closed, consistently oriented meshes are oriented with
*     outward normals.
*   - builds a bounding volume hierarchy (BVH) so that a ray costs
*     O(log N) instead of O(N) triangle tests, without any allocation per ray.
*
* Ray tracing
*   gmesh_intersect()      all intersections of a straight line, sorted in t
*   gmesh_intersect_t0t3() the off_intersect() convention of interoff-lib
*   gmesh_inside()         point-in-mesh test (deterministic, no random numbers)
*   Hits exactly on an edge or vertex shared by several triangles are
*   reported once.
*
* This library is not (yet) a drop-in replacement for the Union mesh code or
* for interoff-lib; all symbols are prefixed gmesh_ / GMESH_ so it can be used
* together with both.
*
*******************************************************************************/

%include "read_table-lib"

#ifndef GEOMETRY_MESH_LIB_H
#define GEOMETRY_MESH_LIB_H "$Revision$"

/* barycentric tolerance for hits on edges (relative) */
#ifndef GMESH_EPSILON
#define GMESH_EPSILON 1e-9
#endif
/* vertex welding tolerance, relative to the bounding box diagonal (0: exact) */
#ifndef GMESH_WELD_TOL
#define GMESH_WELD_TOL 1e-9
#endif
/* max triangles per BVH leaf */
#ifndef GMESH_BVH_LEAF
#define GMESH_BVH_LEAF 4
#endif
/* traversal stack depth (the tree depth is limited accordingly at build) */
#ifndef GMESH_BVH_STACK
#define GMESH_BVH_STACK 64
#endif
#ifndef GMESH_DISPLAY_MAX
#define GMESH_DISPLAY_MAX 200000
#endif

typedef struct gmesh_hit {
  double t;        /* position along the line: pos + t*dir */
  Coords normal;   /* unit normal of the triangle hit (outward for closed oriented meshes) */
  long   tri;      /* triangle index (BVH order) */
  long   face;     /* index of the face (polygon) in the file, 0-based */
  int    in_out;   /* +1 entering (normal.dir < 0), -1 leaving */
  int    edge;     /* 1 when the hit is within tolerance of a triangle edge */
} gmesh_hit;

typedef struct gmesh_node {
  Coords lo, hi;   /* node bounding box */
  long   first;    /* leaf: first triangle; inner node: index of left child (right = first+1) */
  int    count;    /* leaf: number of triangles; inner node: 0 */
} gmesh_node;

typedef struct gmesh_struct {
  long    nverts;           /* number of (welded) vertices */
  long    ntris;            /* number of triangles */
  long    nfaces;           /* number of faces (polygons) in the file */
  long    nnodes;           /* number of BVH nodes */
  Coords* vtx;              /* [nverts] */
  #pragma acc shape(vtx[0:nverts]) init_needed(nverts)
  long*   tri;              /* [3*ntris] vertex indices, in BVH order */
  #pragma acc shape(tri[0:ntris3]) init_needed(ntris3)
  long    ntris3;           /* = 3*ntris */
  long*   tri_face;         /* [ntris] face each triangle comes from */
  #pragma acc shape(tri_face[0:ntris]) init_needed(ntris)
  Coords* tri_normal;       /* [ntris] unit normals */
  #pragma acc shape(tri_normal[0:ntris]) init_needed(ntris)
  gmesh_node* node;         /* [nnodes] BVH, node 0 is the root */
  #pragma acc shape(node[0:nnodes]) init_needed(nnodes)
  unsigned long* faceArray; /* polygons as in the file: [nv i1 .. inv | nv ...] */
  long    faceSize;
  int     nfaceprops;       /* per-face properties, column-major [k*nfaces+face] */
  double* facePropArray;
  #pragma acc shape(facePropArray[0:facePropSize]) init_needed(facePropSize)
  long    facePropSize;
  Coords  bbmin, bbmax;     /* bounding box */
  Coords  bscenter;         /* bounding sphere */
  double  bsradius;
  double  length_scale;     /* bounding box diagonal */
  double  area, volume;     /* surface area, enclosed volume (closed meshes) */
  long    nedges, nboundary, nnonmanifold, ndegenerate, nwelded;
  int     euler;            /* V - E + F (2 for a closed genus-0 surface) */
  int     closed;           /* every edge shared by exactly two triangles */
  int     oriented;         /* shared edges run in opposite directions */
  int     flipped;          /* orientation was reversed to make normals point outward */
  char*   filename;
} gmesh_struct;

/*******************************************************************************
* long gmesh_init(char *filename, double scale, double xwidth, double yheight,
*                 double zdepth, int notcenter, gmesh_struct *m)
* ACTION: read an STL/OFF/PLY file and build the mesh structure.
* INPUT:  scale: multiply file coordinates (1 for m, 1e-3 for mm, 0 same as 1)
*         xwidth,yheight,zdepth: when non-zero, re-size the bounding box (as interoff)
*         notcenter: when 0, center the bounding box on (0,0,0)
* RETURN: number of triangles, 0 on error
*******************************************************************************/
long gmesh_init(char *filename, double scale, double xwidth, double yheight,
                double zdepth, int notcenter, gmesh_struct *m);

/*******************************************************************************
* int gmesh_intersect(gmesh_struct *m, Coords pos, Coords dir, double tmin,
*                     gmesh_hit *hits, int maxhits, long *ntotal)
* ACTION: intersections of the line pos + t*dir with the mesh, for t > tmin
*         (use -FLT_MAX for the whole line). The maxhits hits with the smallest
*         t are stored in 'hits', sorted by increasing t.
* RETURN: number of hits stored; *ntotal (if not NULL) = number of hits found
*******************************************************************************/
#pragma acc routine seq
int gmesh_intersect(gmesh_struct *m, Coords pos, Coords dir, double tmin,
                    gmesh_hit *hits, int maxhits, long *ntotal);

/*******************************************************************************
* int gmesh_intersect_t0t3(double *t0, double *t3, Coords *n0, Coords *n3,
*        long *f0, long *f3, double x, double y, double z,
*        double vx, double vy, double vz, gmesh_struct *m)
* ACTION: same result convention as off_intersect() of interoff-lib (straight
*         line only): t0/t3 are the largest negative and smallest positive
*         times, or the two smallest positive times when the particle is outside.
*         f0/f3 are the face indices (may be NULL).
* RETURN: number of intersections (capped at 4)
*******************************************************************************/
#pragma acc routine seq
int gmesh_intersect_t0t3(double *t0, double *t3, Coords *n0, Coords *n3,
                         long *f0, long *f3, double x, double y, double z,
                         double vx, double vy, double vz, gmesh_struct *m);

/*******************************************************************************
* int gmesh_inside(gmesh_struct *m, Coords pos)
* ACTION: point in (closed) mesh test, parity of ray crossings along fixed
*         directions. Does not use random numbers.
* RETURN: 1 inside, 0 outside
*******************************************************************************/
#pragma acc routine seq
int gmesh_inside(gmesh_struct *m, Coords pos);

/* per-face property k of face 'face', or def when not given in the file */
#pragma acc routine seq
double gmesh_face_prop(gmesh_struct *m, long face, int k, double def);

/* print a summary of the mesh (sizes, topology, volume) */
void gmesh_print_info(gmesh_struct *m);

/* draw the polygon edges (each once), or a polyhedron with mcdotrace==2 */
void gmesh_display(gmesh_struct *m);

/* release the memory held by the mesh */
void gmesh_free(gmesh_struct *m);

#endif

/* end of geometry-mesh-lib.h */
