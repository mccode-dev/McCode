#include <assert.h>
#include "mcstas-stub.h"
#include "geometry-mesh-lib.c"

int main(void)
{
  assert(gmesh_is_extension(".stl", ".stl", ".STL"));
  assert(gmesh_is_extension(".STL", ".stl", ".STL"));
  assert(gmesh_is_extension(".step", ".step", ".STEP"));
  assert(gmesh_is_extension(".STEP", ".step", ".STEP"));
  assert(gmesh_is_extension(".stp", ".stp", ".STP"));
  assert(gmesh_is_extension(".STP", ".stp", ".STP"));
  assert(!gmesh_is_extension(".obj", ".stl", ".STL"));
  return 0;
}
