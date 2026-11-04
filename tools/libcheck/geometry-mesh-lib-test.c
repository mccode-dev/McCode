#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "mcstas-stub.h"
#include "geometry-mesh-lib.h"

int main(void)
{
  gmesh_struct mesh;
  char path[512];

  snprintf(path, sizeof(path), "%s/cube.off", MCCODE_DATA_DIR);

  assert(gmesh_init(path, 1, GMESH_PROPS_IGNORE, &mesh));
  assert(mesh.nverts == 8);
  assert(mesh.nfaces == 6);
  assert(mesh.ntris == 12);
  assert(mesh.closed == 1);
  assert(fabs(mesh.volume - 1.0) < 1e-9);
  assert(fabs(mesh.area - 6.0) < 1e-9);
  assert(mesh.bbmin.x == 0 && mesh.bbmin.y == 0 && mesh.bbmin.z == 0);
  assert(mesh.bbmax.x == 1 && mesh.bbmax.y == 1 && mesh.bbmax.z == 1);

  gmesh_free(&mesh);
  return 0;
}
