#include "mcstas-basics.h"
#include "mccode-r.h"

char instrument_source[] = "";
char *instrument_exe = NULL;

static _class_particle _particle_storage;
_class_particle *_particle = &_particle_storage;

void mcdis_polyhedron(char *vertices_faces)
{
  (void)vertices_faces;
}
