#include "mcstas-shim.h"
#include "mccode-r.h"

char instrument_name[] = "";
char instrument_source[] = "";
char *instrument_exe = NULL;
int mcdotrace = 0;

/* Coords math copied verbatim from common/lib/share/mccode-r.c: a generated
   instrument file links these in automatically, a standalone SHARE lib does
   not, and hand-rewriting them risks drifting from the real behaviour
   (e.g. the z-snapping in coords_add/coords_sub below). */

Coords coords_set(MCNUM x, MCNUM y, MCNUM z)
{
  Coords a;
  a.x = x;
  a.y = y;
  a.z = z;
  return a;
}

Coords coords_add(Coords a, Coords b)
{
  Coords c;
  c.x = a.x + b.x;
  c.y = a.y + b.y;
  c.z = a.z + b.z;
  if (fabs(c.z) < 1e-14) c.z = 0.0;
  return c;
}

Coords coords_sub(Coords a, Coords b)
{
  Coords c;
  c.x = a.x - b.x;
  c.y = a.y - b.y;
  c.z = a.z - b.z;
  if (fabs(c.z) < 1e-14) c.z = 0.0;
  return c;
}

Coords coords_scale(Coords b, double scale)
{
  Coords a;
  a.x = b.x * scale;
  a.y = b.y * scale;
  a.z = b.z * scale;
  return a;
}

double coords_sp(Coords a, Coords b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Coords coords_xp(Coords b, Coords c)
{
  Coords a;
  a.x = b.y * c.z - c.y * b.z;
  a.y = b.z * c.x - c.z * b.x;
  a.z = b.x * c.y - c.x * b.y;
  return a;
}

double coords_len(Coords a)
{
  return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

void mcdis_line(double x1, double y1, double z1, double x2, double y2, double z2)
{
  (void)x1; (void)y1; (void)z1; (void)x2; (void)y2; (void)z2;
}

void mcdis_polyhedron(char *vertices_faces)
{
  (void)vertices_faces;
}
