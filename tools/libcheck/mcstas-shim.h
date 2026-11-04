#ifndef MCSTAS_SHIM_H
#define MCSTAS_SHIM_H

/* Pieces of the generated-instrument C file that a SHARE lib normally gets
   for free, needed to compile one standalone: the per-instrument RNG state
   and particle types (opaque here, unused by these tests), and a display
   primitive geometry-mesh-lib.c calls that mccode-r.h itself doesn't declare. */
typedef unsigned long randstate_t;
typedef struct _class_particle _class_particle;
void mcdis_polyhedron(char *vertices_faces);

#endif
