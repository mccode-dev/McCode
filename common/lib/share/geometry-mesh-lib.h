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
 *   - optional scaling (e.g. 1e-3 for files in mm), no re-sizing or centring
 *   - welds duplicated vertices (STL stores 3 vertices per triangle)
 *   - removes faces given more than once (all copies), e.g. the inner faces
 *     of a volumetric mesh shared by two cells
 *   - triangulates polygons (ear clipping, works for non-convex polygons)
 *   - computes unit normals, area, volume and mesh topology: number of edges,
 *     boundary / non-manifold edges, Euler characteristic, consistent
 *     orientation. Closed, consistently oriented meshes are oriented with
 *     outward normals.
 *   - builds a bounding volume hierarchy (BVH) so that a ray costs
 *     O(log N) instead of O(N) triangle tests, without any allocation per ray.
 *
 * Ray tracing
 *   gmesh_intersect()      the GMESH_MAX_HITS nearest intersections of a
 *                          straight line, sorted in t
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

  // barycentric tolerance for hits on edges (relative)
  #ifndef GMESH_EPSILON
    #define GMESH_EPSILON 1e-9
  #endif
  // vertex welding tolerance, relative to the bounding box diagonal (0: exact)
  #ifndef GMESH_WELD_TOL
    #define GMESH_WELD_TOL 1e-9
  #endif
  // max triangles per BVH leaf
  #ifndef GMESH_BVH_LEAF
    #define GMESH_BVH_LEAF 4
  #endif
  // traversal stack depth (the tree depth is limited accordingly at build)
  #ifndef GMESH_BVH_STACK
    #define GMESH_BVH_STACK 64
  #endif
  // size of the hit buffer of gmesh_intersect. To change it from an instrument,
  // add DEPENDENCY " -DGMESH_MAX_HITS=20 " after its DEFINE INSTRUMENT line
  #ifndef GMESH_MAX_HITS
    #define GMESH_MAX_HITS 50
  #endif
  #ifndef GMESH_DISPLAY_MAX
    #define GMESH_DISPLAY_MAX 200000
  #endif

typedef struct gmesh_hit {
  double t; // position along the line: pos + t*dir
  Coords normal; // unit normal of the triangle hit (outward for closed oriented meshes)
  int tri; // triangle index (BVH order)
  int face; // index of the face (polygon) in the file, 0-based
  int in_out; // +1 entering (normal.dir < 0), -1 leaving
  int edge; // 1 when the hit is within tolerance of a triangle edge
} gmesh_hit;

typedef struct gmesh_node {
  Coords lo, hi; // node bounding box
  int first; // leaf: first triangle; inner node: index of left child (right = first+1)
  int count; // leaf: number of triangles; inner node: 0
} gmesh_node;

typedef struct gmesh_struct {
  int nverts; // number of (welded) vertices
  int ntris; // number of triangles
  int nfaces; // number of faces (polygons) in the file
  int nnodes; // number of BVH nodes
  Coords* vtx; // [nverts]
  #pragma acc shape(vtx[0 : nverts]) init_needed(nverts)
  int* tri; // [3*ntris] vertex indices, in BVH order
  #pragma acc shape(tri[0 : ntris3]) init_needed(ntris3)
  int ntris3; // = 3*ntris
  int* tri_face; // [ntris] face each triangle comes from
  #pragma acc shape(tri_face[0 : ntris]) init_needed(ntris)
  Coords* tri_normal; // [ntris] unit normals
  #pragma acc shape(tri_normal[0 : ntris]) init_needed(ntris)
  gmesh_node* node; // [nnodes] BVH, node 0 is the root
  #pragma acc shape(node[0 : nnodes]) init_needed(nnodes)
  int* faceArray; // polygons as in the file: [nv i1 .. inv | nv ...]
  int faceSize;
  int nfaceprops; // per-face properties, column-major [k*nfaces+face]
  double* facePropArray;
  #pragma acc shape(facePropArray[0 : facePropSize]) init_needed(facePropSize)
  int facePropSize;
  Coords bbmin, bbmax; // bounding box
  Coords bscenter; // bounding sphere
  double bsradius;
  double length_scale; // bounding box diagonal
  double area, volume; // surface area, enclosed volume (closed meshes)
  int nedges, nboundary, nnonmanifold, ndegenerate, nwelded;
  int nduplicate; // faces given more than once (all copies removed)
  int euler; // V - E + F (2 for a closed genus-0 surface)
  int closed; // every edge shared by exactly two triangles
  int oriented; // shared edges run in opposite directions
  int flipped; // orientation was reversed to make normals point outward
  char* filename;
} gmesh_struct;

  // face_props argument of gmesh_init: what the component does with the
  // per-face properties of the file (numbers after the vertex indices of an
  // OFF face, scalar face properties of a PLY file)
  #define GMESH_PROPS_IGNORE 0 // not read; nfaceprops is 0
  #define GMESH_PROPS_OPTIONAL 1 // read when given; missing values are 0
  #define GMESH_PROPS_REQUIRED 2 // error unless every face has properties

// int gmesh_init(char *filename, double scale, int face_props, gmesh_struct *mesh)
// ACTION: read an STL/OFF/PLY file and build the mesh structure.
// INPUT:  scale: multiply file coordinates (1 for m, 1e-3 for mm, 0 same as 1)
// face_props: GMESH_PROPS_IGNORE, GMESH_PROPS_OPTIONAL or GMESH_PROPS_REQUIRED
// RETURN: number of triangles, 0 on error
int gmesh_init (char* filename, double scale, int face_props, gmesh_struct* mesh);

  // int gmesh_intersect(gmesh_struct *mesh, Coords position, Coords direction,
  // double t_min, gmesh_hit hits[GMESH_MAX_HITS])
  // ACTION: intersections of the line position + t*direction with the mesh, for
  // t > t_min (use -FLT_MAX for the whole line). The GMESH_MAX_HITS hits
  // with the smallest t are stored in 'hits', sorted by increasing t.
  // When the line has more hits, the farther ones are dropped and a
  // warning (shown once) explains how to raise GMESH_MAX_HITS.
  // RETURN: number of hits stored
  #pragma acc routine seq
int gmesh_intersect (gmesh_struct* mesh, Coords position, Coords direction, double t_min, gmesh_hit hits[GMESH_MAX_HITS]);

  // int gmesh_inside(gmesh_struct *mesh, Coords position)
  // ACTION: point in (closed) mesh test, parity of ray crossings along fixed
  // directions. Does not use random numbers.
  // RETURN: 1 inside, 0 outside
  #pragma acc routine seq
int gmesh_inside (gmesh_struct* mesh, Coords position);

  // per-face property number 'property' of face 'face', or fallback when it is
  // not given in the file
  #pragma acc routine seq
double gmesh_face_prop (gmesh_struct* mesh, int face, int property, double fallback);

// print a summary of the mesh (sizes, topology, volume)
void gmesh_print_info (gmesh_struct* mesh);

// draw the polygon edges (each once), or a polyhedron with mcdotrace==2
void gmesh_display (gmesh_struct* mesh);

// release the memory held by the mesh
void gmesh_free (gmesh_struct* mesh);

#endif

// end of geometry-mesh-lib.h
