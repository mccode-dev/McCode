#ifndef MCSTAS_STUB_H
#define MCSTAS_STUB_H

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {double x, y, z;} Coords;

static Coords coords_set(double x, double y, double z)
{
  Coords c;
  c.x = x;
  c.y = y;
  c.z = z;
  return c;
}

static Coords coords_add(Coords a, Coords b) { return coords_set(a.x + b.x, a.y + b.y, a.z + b.z); }
static Coords coords_sub(Coords a, Coords b) { return coords_set(a.x - b.x, a.y - b.y, a.z - b.z); }
static Coords coords_scale(Coords a, double s) { return coords_set(a.x * s, a.y * s, a.z * s); }
static double coords_sp(Coords a, Coords b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static double coords_len(Coords a) { return sqrt(coords_sp(a, a)); }
static Coords coords_xp(Coords a, Coords b)
{
  return coords_set(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

#define MPI_MASTER(statement) statement
#define rand01() 0.5

static int mcdotrace = 0;
static void mcdis_polyhedron(char *json) { (void)json; }
static void mcdis_line(double x1, double y1, double z1, double x2, double y2, double z2)
{
  (void)x1; (void)y1; (void)z1; (void)x2; (void)y2; (void)z2;
}

FILE *Open_File(char *name, const char *Mode, char *path)
{
  (void)path;
  return fopen(name, Mode);
}

#endif
