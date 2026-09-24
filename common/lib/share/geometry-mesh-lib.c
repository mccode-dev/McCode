/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2026, All rights reserved
*         DTU Physics, Kongens Lyngby, Denmark
*
* Runtime: share/geometry-mesh-lib.c
*
* %Identification
* Written by: Peter Willendrup, based on the Union_mesh component (Martin Olsen,
*             Daniel Lomholt Christensen) and interoff-lib (Reynald Arnerin,
*             Emmanuel Farhi)
* Date: Sep 2026
* Origin: DTU
*
* Triangle-mesh geometry library (STL, OFF, PLY). See geometry-mesh-lib.h
*
*******************************************************************************/

#ifndef GEOMETRY_MESH_LIB_H
#include "geometry-mesh-lib.h"
#endif

#ifndef GEOMETRY_MESH_LIB_C
#define GEOMETRY_MESH_LIB_C "$Revision$"

#include <ctype.h>
#include <stdint.h>

#ifdef OPENACC
#define fprintf(stderr,...) printf(__VA_ARGS__)
#endif

/* ========================================================================== */
/*  Small helpers                                                             */
/* ========================================================================== */

#pragma acc routine seq
double gmesh_coord(Coords c, int k) { return k == 0 ? c.x : (k == 1 ? c.y : c.z); }

static void *gmesh_xrealloc(void *p, size_t n)
{
  void *q = realloc(p, n ? n : 1);
  if (!q) { fprintf(stderr, "Error: memory allocation of %lu bytes failed (geometry-mesh-lib)\n", (unsigned long)n); exit(-1); }
  return q;
}

/* read the rest of the current line (without '\n'); -1 at EOF with nothing read */
static long gmesh_readline(FILE *f, char **buf, size_t *cap)
{
  long n = 0; int c;
  if (!*buf || !*cap) { *cap = 1024; *buf = gmesh_xrealloc(NULL, *cap); }
  while ((c = fgetc(f)) != EOF && c != '\n') {
    if ((size_t)(n+1) >= *cap) { *cap *= 2; *buf = gmesh_xrealloc(*buf, *cap); }
    (*buf)[n++] = (char)c;
  }
  (*buf)[n] = '\0';
  if (c == EOF && n == 0) return -1;
  return n;
}

/* raw geometry as read from a file: vertices, polygons, per-face properties */
typedef struct gmesh_raw {
  Coords *v;  long nv, capv;
  unsigned long *f; long fs, capf; long nf;
  double *p; long np, capp;          /* all properties, row-major */
  long *poff; int *pcnt; long capfaces;
  int nprops, mixed;
} gmesh_raw;

static void gmesh_raw_vertex(gmesh_raw *r, double x, double y, double z)
{
  if (r->nv >= r->capv) { r->capv = r->capv ? 2*r->capv : 1024; r->v = gmesh_xrealloc(r->v, r->capv*sizeof(Coords)); }
  r->v[r->nv++] = coords_set(x, y, z);
}

/* start a new polygon with nv vertices; indices are then added with gmesh_raw_index */
static void gmesh_raw_face(gmesh_raw *r, long nv)
{
  if (r->fs + nv + 1 > r->capf) {
    while (r->fs + nv + 1 > r->capf) r->capf = r->capf ? 2*r->capf : 4096;
    r->f = gmesh_xrealloc(r->f, r->capf*sizeof(unsigned long));
  }
  if (r->nf >= r->capfaces) {
    r->capfaces = r->capfaces ? 2*r->capfaces : 1024;
    r->poff = gmesh_xrealloc(r->poff, r->capfaces*sizeof(long));
    r->pcnt = gmesh_xrealloc(r->pcnt, r->capfaces*sizeof(int));
  }
  r->f[r->fs++] = nv;
  r->poff[r->nf] = r->np; r->pcnt[r->nf] = 0;
  r->nf++;
}
static void gmesh_raw_index(gmesh_raw *r, unsigned long i) { r->f[r->fs++] = i; }
static void gmesh_raw_prop(gmesh_raw *r, double val)
{
  if (r->np >= r->capp) { r->capp = r->capp ? 2*r->capp : 1024; r->p = gmesh_xrealloc(r->p, r->capp*sizeof(double)); }
  r->p[r->np++] = val; r->pcnt[r->nf-1]++;
}
static void gmesh_raw_free(gmesh_raw *r)
{
  free(r->v); free(r->f); free(r->p); free(r->poff); free(r->pcnt);
  memset(r, 0, sizeof(gmesh_raw));
}

/* ========================================================================== */
/*  OFF reader                                                                */
/* ========================================================================== */

static int gmesh_read_off(FILE *f, char *filename, gmesh_raw *r)
{
  char *line = NULL; size_t cap = 0; long len, nv = 0, nf = 0, i;
  int extra = 0;
  char *hdr, *p;
  do { len = gmesh_readline(f, &line, &cap); } while (len >= 0 && strspn(line, " \t\r") == (size_t)len);
  if (len < 0) { fprintf(stderr, "Error: empty file %s (geometry-mesh-lib)\n", filename); free(line); return 0; }
  hdr = line + strspn(line, " \t");
  p = hdr;
  while (*p && strchr("STCN", *p)) p++;
  if (strncmp(p, "OFF", 3) || (p[3] && !isspace((unsigned char)p[3]))) {
    fprintf(stderr, "Error: %s: header '%s' is not a supported OFF header (geometry-mesh-lib)\n", filename, hdr);
    free(line); return 0;
  }
  extra = (p != hdr);
  if (sscanf(p+3, "%ld %ld", &nv, &nf) != 2) {
    do {
      len = gmesh_readline(f, &line, &cap);
      if (len < 0) break;
      hdr = line + strspn(line, " \t\r");
    } while (hdr[0] == '#' || hdr[0] == '\0');
    if (len < 0 || sscanf(hdr, "%ld %ld", &nv, &nf) != 2) {
      fprintf(stderr, "Error: %s: can not read the numbers of vertices and faces (geometry-mesh-lib)\n", filename);
      free(line); return 0;
    }
  }
  if (nv <= 0 || nf <= 0) {
    fprintf(stderr, "Error: %s defines %ld vertices and %ld faces (geometry-mesh-lib)\n", filename, nv, nf);
    free(line); return 0;
  }
  for (i = 0; i < nv; ) {
    double x, y, z;
    int ret = fscanf(f, "%lg %lg %lg", &x, &y, &z);
    if (ret == 0) { gmesh_readline(f, &line, &cap); continue; } /* comment */
    if (ret != 3) {
      fprintf(stderr, "Error: %s: can not read vertex %ld (geometry-mesh-lib)\n", filename, i);
      free(line); return 0;
    }
    if (extra) gmesh_readline(f, &line, &cap);
    gmesh_raw_vertex(r, x, y, z);
    i++;
  }
  for (i = 0; i < nf; ) {
    long n, j; double idx;
    int ret = fscanf(f, "%ld", &n);
    if (ret == 0) { gmesh_readline(f, &line, &cap); continue; }
    if (ret != 1 || n < 1) {
      fprintf(stderr, "Error: %s: can not read face %ld (geometry-mesh-lib)\n", filename, i);
      free(line); return 0;
    }
    gmesh_raw_face(r, n);
    for (j = 0; j < n; j++) {
      if (fscanf(f, "%lg", &idx) != 1 || idx < 0 || idx >= nv || idx != floor(idx)) {
        fprintf(stderr, "Error: %s: invalid vertex index #%ld of face %ld (geometry-mesh-lib)\n", filename, j, i);
        free(line); return 0;
      }
      gmesh_raw_index(r, (unsigned long)idx);
    }
    /* optional per-face numbers until the end of the line */
    if (gmesh_readline(f, &line, &cap) > 0) {
      char *s = line, *end, *hash = strchr(line, '#');
      if (hash) *hash = '\0';
      for (;;) { double val = strtod(s, &end); if (end == s) break; gmesh_raw_prop(r, val); s = end; }
    }
    i++;
  }
  free(line);
  return 1;
}

/* ========================================================================== */
/*  STL reader                                                                */
/* ========================================================================== */

static int gmesh_read_stl(char *filename, gmesh_raw *r)
{
  FILE *f = Open_File(filename, "rb", NULL);
  unsigned char head[84];
  long size, ntri = 0, i;
  int binary = 0;
  if (!f) return 0;
  fseek(f, 0, SEEK_END); size = ftell(f); fseek(f, 0, SEEK_SET);
  if (size >= 84 && fread(head, 1, 84, f) == 84) {
    uint32_t n = (uint32_t)head[80] | ((uint32_t)head[81] << 8) | ((uint32_t)head[82] << 16) | ((uint32_t)head[83] << 24);
    ntri = (long)n;
    /* binary when the size matches exactly, whatever the header says */
    if (size == 84 + 50*(long)n) binary = 1;
    else {
      long k = 0; while (k < 80 && isspace(head[k])) k++;
      if (strncmp((char*)head+k, "solid", 5)) {
        fprintf(stderr, "Error: %s is neither an ASCII STL nor a binary STL of consistent size "
                "(%ld bytes for %ld triangles) (geometry-mesh-lib)\n", filename, size, ntri);
        fclose(f); return 0;
      }
    }
  }
  if (binary) {
    unsigned char rec[50];
    for (i = 0; i < ntri; i++) {
      int k;
      float xyz[9];
      if (fread(rec, 1, 50, f) != 50) {
        fprintf(stderr, "Error: %s: truncated binary STL at triangle %ld (geometry-mesh-lib)\n", filename, i);
        fclose(f); return 0;
      }
      for (k = 0; k < 9; k++) { /* little endian float32, skip the normal */
        uint32_t u = (uint32_t)rec[12+4*k] | ((uint32_t)rec[13+4*k] << 8) | ((uint32_t)rec[14+4*k] << 16) | ((uint32_t)rec[15+4*k] << 24);
        memcpy(&xyz[k], &u, 4);
      }
      gmesh_raw_face(r, 3);
      for (k = 0; k < 3; k++) {
        gmesh_raw_vertex(r, xyz[3*k], xyz[3*k+1], xyz[3*k+2]);
        gmesh_raw_index(r, r->nv-1);
      }
    }
  } else {
    char *line = NULL; size_t cap = 0; long nloop = 0, first = 0;
    fseek(f, 0, SEEK_SET);
    while (gmesh_readline(f, &line, &cap) >= 0) {
      char *s = line + strspn(line, " \t");
      if (!strncmp(s, "outer", 5))       { first = r->nv; nloop = 0; }
      else if (!strncmp(s, "vertex", 6)) {
        double x, y, z;
        if (sscanf(s+6, "%lg %lg %lg", &x, &y, &z) != 3) {
          fprintf(stderr, "Error: %s: bad vertex line '%s' (geometry-mesh-lib)\n", filename, s);
          free(line); fclose(f); return 0;
        }
        gmesh_raw_vertex(r, x, y, z); nloop++;
      } else if (!strncmp(s, "endloop", 7)) {
        long k;
        if (nloop >= 1) {
          gmesh_raw_face(r, nloop);
          for (k = 0; k < nloop; k++) gmesh_raw_index(r, first+k);
        }
        nloop = 0;
      }
    }
    free(line);
    if (!r->nf) { fprintf(stderr, "Error: %s: no facets found in ASCII STL (geometry-mesh-lib)\n", filename); fclose(f); return 0; }
  }
  fclose(f);
  return 1;
}

/* ========================================================================== */
/*  PLY reader (ascii and binary)                                             */
/* ========================================================================== */

typedef struct gmesh_ply_prop {
  char name[64];
  int  type;        /* scalar type, or item type for lists */
  int  list;        /* 1 for list properties */
  int  ctype;       /* count type for lists */
} gmesh_ply_prop;
typedef struct gmesh_ply_elem {
  char name[64]; long count; int nprop; gmesh_ply_prop prop[32];
} gmesh_ply_elem;

/* type codes: size in bytes, sign/float encoded */
enum { GMP_I8=1, GMP_U8, GMP_I16, GMP_U16, GMP_I32, GMP_U32, GMP_F32, GMP_F64 };
static int gmesh_ply_type(const char *s)
{
  if (!strcmp(s,"char")   || !strcmp(s,"int8"))    return GMP_I8;
  if (!strcmp(s,"uchar")  || !strcmp(s,"uint8"))   return GMP_U8;
  if (!strcmp(s,"short")  || !strcmp(s,"int16"))   return GMP_I16;
  if (!strcmp(s,"ushort") || !strcmp(s,"uint16"))  return GMP_U16;
  if (!strcmp(s,"int")    || !strcmp(s,"int32"))   return GMP_I32;
  if (!strcmp(s,"uint")   || !strcmp(s,"uint32"))  return GMP_U32;
  if (!strcmp(s,"float")  || !strcmp(s,"float32")) return GMP_F32;
  if (!strcmp(s,"double") || !strcmp(s,"float64")) return GMP_F64;
  return 0;
}
static int gmesh_ply_size(int t) { return t<=GMP_U8 ? 1 : t<=GMP_U16 ? 2 : t<=GMP_F32 ? 4 : 8; }

/* read one value; fmt 0 ascii, 1 binary little endian, 2 binary big endian */
static int gmesh_ply_value(FILE *f, int fmt, int type, double *val)
{
  unsigned char b[8]; int n = gmesh_ply_size(type), k, host_le;
  uint16_t one = 1; unsigned char c1;
  if (fmt == 0) return fscanf(f, "%lg", val) == 1;
  if ((int)fread(b, 1, n, f) != n) return 0;
  memcpy(&c1, &one, 1); host_le = (c1 == 1);
  if ((fmt == 2) == host_le) for (k = 0; k < n/2; k++) { unsigned char c = b[k]; b[k] = b[n-1-k]; b[n-1-k] = c; }
  switch (type) {
    case GMP_I8:  *val = (double)(int8_t)b[0]; break;
    case GMP_U8:  *val = (double)b[0]; break;
    case GMP_I16: { int16_t  v; memcpy(&v, b, 2); *val = v; break; }
    case GMP_U16: { uint16_t v; memcpy(&v, b, 2); *val = v; break; }
    case GMP_I32: { int32_t  v; memcpy(&v, b, 4); *val = v; break; }
    case GMP_U32: { uint32_t v; memcpy(&v, b, 4); *val = v; break; }
    case GMP_F32: { float    v; memcpy(&v, b, 4); *val = v; break; }
    case GMP_F64: { double   v; memcpy(&v, b, 8); *val = v; break; }
    default: return 0;
  }
  return 1;
}

static int gmesh_read_ply(FILE *f, char *filename, gmesh_raw *r)
{
  char *line = NULL; size_t cap = 0;
  gmesh_ply_elem el[16]; int nel = 0, fmt = -1, e;
  long nvtx = -1;
  while (gmesh_readline(f, &line, &cap) >= 0) {
    char a[64] = "", b[64] = "", c[64] = "", d[64] = "";
    int n = sscanf(line, "%63s %63s %63s %63s", a, b, c, d);
    if (n < 1) continue;
    if (!strcmp(a, "end_header")) break;
    if (!strcmp(a, "format")) {
      fmt = !strcmp(b, "ascii") ? 0 : !strcmp(b, "binary_little_endian") ? 1 : !strcmp(b, "binary_big_endian") ? 2 : -1;
      if (fmt < 0) { fprintf(stderr, "Error: %s: unknown PLY format '%s' (geometry-mesh-lib)\n", filename, b); free(line); return 0; }
    } else if (!strcmp(a, "element")) {
      if (nel >= 16) { fprintf(stderr, "Error: %s: too many PLY elements (geometry-mesh-lib)\n", filename); free(line); return 0; }
      snprintf(el[nel].name, sizeof(el[nel].name), "%s", b); el[nel].count = atol(c); el[nel].nprop = 0; nel++;
    } else if (!strcmp(a, "property") && nel) {
      gmesh_ply_elem *E = &el[nel-1];
      gmesh_ply_prop *P;
      if (E->nprop >= 32) { fprintf(stderr, "Error: %s: too many PLY properties (geometry-mesh-lib)\n", filename); free(line); return 0; }
      P = &E->prop[E->nprop++];
      if (!strcmp(b, "list")) { P->list = 1; P->ctype = gmesh_ply_type(c); P->type = gmesh_ply_type(d);
        { char nm[64] = ""; sscanf(line, "%*s %*s %*s %*s %63s", nm); strcpy(P->name, nm); } }
      else { P->list = 0; P->ctype = 0; P->type = gmesh_ply_type(b); snprintf(P->name, sizeof(P->name), "%s", c); }
      if (!P->type || (P->list && !P->ctype)) {
        fprintf(stderr, "Error: %s: unknown PLY property type in '%s' (geometry-mesh-lib)\n", filename, line); free(line); return 0;
      }
    }
  }
  free(line); line = NULL; cap = 0;
  if (fmt < 0) { fprintf(stderr, "Error: %s: PLY format line missing (geometry-mesh-lib)\n", filename); return 0; }

  for (e = 0; e < nel; e++) {
    gmesh_ply_elem *E = &el[e];
    int isv = !strcmp(E->name, "vertex"), isf = !strcmp(E->name, "face");
    int ix = -1, iy = -1, iz = -1, il = -1, k;
    long i;
    if (isv) { for (k = 0; k < E->nprop; k++) {
        if (!strcmp(E->prop[k].name, "x")) ix = k;
        if (!strcmp(E->prop[k].name, "y")) iy = k;
        if (!strcmp(E->prop[k].name, "z")) iz = k; }
      if (ix < 0 || iy < 0 || iz < 0) { fprintf(stderr, "Error: %s: PLY vertex without x y z (geometry-mesh-lib)\n", filename); return 0; }
      nvtx = E->count; }
    if (isf) { for (k = 0; k < E->nprop; k++)
        if (E->prop[k].list && (!strcmp(E->prop[k].name, "vertex_indices") || !strcmp(E->prop[k].name, "vertex_index") || il < 0)) il = k;
      if (il < 0) { fprintf(stderr, "Error: %s: PLY face without vertex list (geometry-mesh-lib)\n", filename); return 0; }
      if (nvtx < 0) { fprintf(stderr, "Error: %s: PLY faces before vertices (geometry-mesh-lib)\n", filename); return 0; } }
    for (i = 0; i < E->count; i++) {
      double xyz[3] = {0,0,0}, sc[32];
      int nsc = 0;
      for (k = 0; k < E->nprop; k++) {
        gmesh_ply_prop *P = &E->prop[k];
        double val;
        if (P->list) {
          long nitem, j;
          if (!gmesh_ply_value(f, fmt, P->ctype, &val)) goto trunc;
          nitem = (long)val;
          if (isf && k == il) {
            gmesh_raw_face(r, nitem);
            for (j = 0; j < nitem; j++) {
              if (!gmesh_ply_value(f, fmt, P->type, &val)) goto trunc;
              if (val < 0 || val >= nvtx) {
                fprintf(stderr, "Error: %s: invalid vertex index %g in face %ld (geometry-mesh-lib)\n", filename, val, i);
                return 0;
              }
              gmesh_raw_index(r, (unsigned long)val);
            }
          } else for (j = 0; j < nitem; j++) if (!gmesh_ply_value(f, fmt, P->type, &val)) goto trunc;
        } else {
          if (!gmesh_ply_value(f, fmt, P->type, &val)) goto trunc;
          if (isv) { if (k == ix) xyz[0] = val; else if (k == iy) xyz[1] = val; else if (k == iz) xyz[2] = val; }
          if (isf) sc[nsc++] = val;   /* scalar face property (e.g. colour, m value) */
        }
      }
      if (isv) gmesh_raw_vertex(r, xyz[0], xyz[1], xyz[2]);
      if (isf) for (k = 0; k < nsc; k++) gmesh_raw_prop(r, sc[k]);
    }
  }
  return 1;
trunc:
  fprintf(stderr, "Error: %s: PLY data truncated or unreadable (geometry-mesh-lib)\n", filename);
  return 0;
}

/* ========================================================================== */
/*  Hash for vertex welding and edges                                         */
/* ========================================================================== */

static uint64_t gmesh_mix(uint64_t h)
{
  h ^= h >> 33; h *= 0xff51afd7ed558ccdULL; h ^= h >> 33; h *= 0xc4ceb9fe1a85ec53ULL; h ^= h >> 33;
  return h;
}

/* edge table: undirected edge (a<b) -> number of uses, direction sum, two triangles */
typedef struct gmesh_edge { long a, b; int n; int dir; long t0, t1; } gmesh_edge;
typedef struct gmesh_etab { gmesh_edge *e; long cap, n; } gmesh_etab;

static gmesh_edge *gmesh_edge_get(gmesh_etab *T, long a, long b)
{
  long lo = a < b ? a : b, hi = a < b ? b : a;
  uint64_t h = gmesh_mix((uint64_t)lo * 0x9E3779B97F4A7C15ULL ^ (uint64_t)hi);
  long i = (long)(h & (uint64_t)(T->cap-1));
  while (T->e[i].n) {
    if (T->e[i].a == lo && T->e[i].b == hi) return &T->e[i];
    i = (i+1) & (T->cap-1);
  }
  T->e[i].a = lo; T->e[i].b = hi; T->e[i].dir = 0; T->e[i].t0 = T->e[i].t1 = -1; T->n++;
  return &T->e[i];
}

static void gmesh_etab_build(gmesh_etab *T, long *tri, long ntris)
{
  long i, k;
  T->cap = 16; while (T->cap < 6*ntris+16) T->cap *= 2;
  T->e = calloc(T->cap, sizeof(gmesh_edge)); T->n = 0;
  if (!T->e) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
  for (i = 0; i < ntris; i++) for (k = 0; k < 3; k++) {
    long a = tri[3*i+k], b = tri[3*i+(k+1)%3];
    gmesh_edge *E = gmesh_edge_get(T, a, b);
    if (E->n == 0) E->t0 = i; else if (E->n == 1) E->t1 = i;
    E->n++; E->dir += (a < b ? 1 : -1);
  }
}

/* ========================================================================== */
/*  Geometry helpers                                                          */
/* ========================================================================== */

static Coords gmesh_newell(Coords *v, unsigned long *idx, long n)
{
  Coords N = coords_set(0,0,0); long i, j;
  for (i = 0, j = n-1; i < n; j = i++) {
    Coords a = v[idx[i]], b = v[idx[j]];
    /* a = vertex i, b = previous vertex: N = sum (b - a) x-terms, i.e. right-handed w.r.t. the vertex order */
    N.x += (b.y - a.y)*(a.z + b.z); N.y += (b.z - a.z)*(a.x + b.x); N.z += (b.x - a.x)*(a.y + b.y);
  }
  return N;
}

/* ear-clipping triangulation of polygon idx[0..n-1] (welded, no repeats).
   Appends triangles to tri (grown as needed). Returns number of triangles. */
static long gmesh_triangulate(Coords *v, unsigned long *idx, long n, long **tri, long *ntri, long *cap)
{
  long added = 0, i;
  if (n < 3) return 0;
  if (*ntri + n > *cap) { while (*ntri + n > *cap) *cap = *cap ? 2**cap : 4096; *tri = gmesh_xrealloc(*tri, 3*(*cap)*sizeof(long)); }
  if (n == 3) {
    (*tri)[3**ntri] = idx[0]; (*tri)[3**ntri+1] = idx[1]; (*tri)[3**ntri+2] = idx[2]; (*ntri)++;
    return 1;
  }
  {
    Coords N = gmesh_newell(v, idx, n);
    double ax = fabs(N.x), ay = fabs(N.y), az = fabs(N.z), sgn;
    int px, py;
    long *rem = malloc(n*sizeof(long)), m = n, guard = 0;
    double *X = malloc(n*sizeof(double)), *Y = malloc(n*sizeof(double));
    if (!rem || !X || !Y) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
    if (az >= ax && az >= ay) { px = 0; py = 1; sgn = N.z; }
    else if (ay >= ax)        { px = 2; py = 0; sgn = N.y; }
    else                      { px = 1; py = 2; sgn = N.x; }
    sgn = (sgn >= 0 ? 1 : -1);
    for (i = 0; i < n; i++) {
      Coords p = v[idx[i]];
      double c[3] = {p.x, p.y, p.z};
      X[i] = c[px]; Y[i] = c[py]; rem[i] = i;
    }
    while (m > 3 && guard < 2*n*n) {
      int found = 0;
      for (i = 0; i < m && !found; i++) {
        long ia = rem[(i+m-1)%m], ib = rem[i], ic = rem[(i+1)%m], j;
        double cr = (X[ib]-X[ia])*(Y[ic]-Y[ia]) - (Y[ib]-Y[ia])*(X[ic]-X[ia]);
        if (cr*sgn <= 0) continue;                       /* reflex or flat corner */
        for (j = 0; j < m; j++) {                        /* no other vertex inside the ear */
          long ip = rem[j];
          double d1, d2, d3;
          if (ip == ia || ip == ib || ip == ic) continue;
          d1 = (X[ib]-X[ia])*(Y[ip]-Y[ia]) - (Y[ib]-Y[ia])*(X[ip]-X[ia]);
          d2 = (X[ic]-X[ib])*(Y[ip]-Y[ib]) - (Y[ic]-Y[ib])*(X[ip]-X[ib]);
          d3 = (X[ia]-X[ic])*(Y[ip]-Y[ic]) - (Y[ia]-Y[ic])*(X[ip]-X[ic]);
          if (d1*sgn >= 0 && d2*sgn >= 0 && d3*sgn >= 0) break;
        }
        if (j < m) continue;
        (*tri)[3**ntri] = idx[ia]; (*tri)[3**ntri+1] = idx[ib]; (*tri)[3**ntri+2] = idx[ic]; (*ntri)++; added++;
        for (j = i; j < m-1; j++) rem[j] = rem[j+1];
        m--; found = 1;
      }
      if (!found) break;   /* self-intersecting / degenerate: fan the rest */
      guard++;
    }
    for (i = 1; i+1 < m; i++) {
      (*tri)[3**ntri] = idx[rem[0]]; (*tri)[3**ntri+1] = idx[rem[i]]; (*tri)[3**ntri+2] = idx[rem[i+1]]; (*ntri)++; added++;
    }
    free(rem); free(X); free(Y);
  }
  return added;
}

/* ========================================================================== */
/*  BVH build                                                                 */
/* ========================================================================== */

static Coords *gmesh_sort_cent; static int gmesh_sort_axis;
static int gmesh_cmp_cent(const void *a, const void *b)
{
  double ca = gmesh_coord(gmesh_sort_cent[*(const long*)a], gmesh_sort_axis);
  double cb = gmesh_coord(gmesh_sort_cent[*(const long*)b], gmesh_sort_axis);
  return (ca < cb) ? -1 : (ca > cb) ? 1 : 0;
}

static void gmesh_box_tri(gmesh_struct *m, long *tri, long t, Coords *lo, Coords *hi)
{
  int k;
  for (k = 0; k < 3; k++) {
    Coords p = m->vtx[tri[3*t+k]];
    if (p.x < lo->x) lo->x = p.x;
    if (p.y < lo->y) lo->y = p.y;
    if (p.z < lo->z) lo->z = p.z;
    if (p.x > hi->x) hi->x = p.x;
    if (p.y > hi->y) hi->y = p.y;
    if (p.z > hi->z) hi->z = p.z;
  }
}

static void gmesh_build_bvh(gmesh_struct *m)
{
  long n = m->ntris, i, *order, *tri2, *face2, top = 0;
  Coords *cent, *nrm2;
  struct { long node, start, end; int depth; } stack[2*GMESH_BVH_STACK+2];
  order = malloc(n*sizeof(long)); cent = malloc(n*sizeof(Coords));
  m->node = malloc((2*n+1)*sizeof(gmesh_node));
  if (!order || !cent || !m->node) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
  for (i = 0; i < n; i++) {
    Coords a = m->vtx[m->tri[3*i]], b = m->vtx[m->tri[3*i+1]], c = m->vtx[m->tri[3*i+2]];
    cent[i] = coords_set((a.x+b.x+c.x)/3, (a.y+b.y+c.y)/3, (a.z+b.z+c.z)/3);
    order[i] = i;
  }
  m->nnodes = 1;
  stack[top].node = 0; stack[top].start = 0; stack[top].end = n; stack[top].depth = 0; top++;
  while (top > 0) {
    long nd, s, e; int depth;
    Coords lo = coords_set(FLT_MAX,FLT_MAX,FLT_MAX), hi = coords_set(-FLT_MAX,-FLT_MAX,-FLT_MAX);
    Coords clo = lo, chi = hi;
    top--; nd = stack[top].node; s = stack[top].start; e = stack[top].end; depth = stack[top].depth;
    for (i = s; i < e; i++) {
      Coords c = cent[order[i]];
      gmesh_box_tri(m, m->tri, order[i], &lo, &hi);
      if (c.x < clo.x) clo.x = c.x;
      if (c.y < clo.y) clo.y = c.y;
      if (c.z < clo.z) clo.z = c.z;
      if (c.x > chi.x) chi.x = c.x;
      if (c.y > chi.y) chi.y = c.y;
      if (c.z > chi.z) chi.z = c.z;
    }
    m->node[nd].lo = lo; m->node[nd].hi = hi;
    if (e - s <= GMESH_BVH_LEAF || depth >= GMESH_BVH_STACK-2) {
      m->node[nd].first = s; m->node[nd].count = (int)(e - s);
    } else {
      double dx = chi.x-clo.x, dy = chi.y-clo.y, dz = chi.z-clo.z;
      long mid = (s + e)/2, l = m->nnodes;
      gmesh_sort_axis = (dx >= dy && dx >= dz) ? 0 : (dy >= dz ? 1 : 2);
      gmesh_sort_cent = cent;
      qsort(order + s, e - s, sizeof(long), gmesh_cmp_cent);
      m->node[nd].first = l; m->node[nd].count = 0;
      m->nnodes += 2;
      stack[top].node = l;   stack[top].start = s;   stack[top].end = mid; stack[top].depth = depth+1; top++;
      stack[top].node = l+1; stack[top].start = mid; stack[top].end = e;   stack[top].depth = depth+1; top++;
    }
  }
  /* permute the triangles into BVH order */
  tri2 = malloc(3*n*sizeof(long)); face2 = malloc(n*sizeof(long)); nrm2 = malloc(n*sizeof(Coords));
  if (!tri2 || !face2 || !nrm2) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
  for (i = 0; i < n; i++) {
    long o = order[i];
    tri2[3*i] = m->tri[3*o]; tri2[3*i+1] = m->tri[3*o+1]; tri2[3*i+2] = m->tri[3*o+2];
    face2[i] = m->tri_face[o]; nrm2[i] = m->tri_normal[o];
  }
  free(m->tri); free(m->tri_face); free(m->tri_normal);
  m->tri = tri2; m->tri_face = face2; m->tri_normal = nrm2;
  m->node = gmesh_xrealloc(m->node, m->nnodes*sizeof(gmesh_node));
  free(order); free(cent);
}

/* ========================================================================== */
/*  Init                                                                      */
/* ========================================================================== */

long gmesh_init(char *filename, double scale, double xwidth, double yheight,
                double zdepth, int notcenter, gmesh_struct *m)
{
  gmesh_raw r;
  const char *dot;
  int ok = 0;
  long i, k;
  double minx=FLT_MAX,maxx=-FLT_MAX,miny=FLT_MAX,maxy=-FLT_MAX,minz=FLT_MAX,maxz=-FLT_MAX;
  long *map;

  if (!m || !filename || !strlen(filename)) return 0;
  memset(m, 0, sizeof(gmesh_struct));
  memset(&r, 0, sizeof(gmesh_raw));
  m->filename = filename;

  MPI_MASTER( printf("Loading geometry file (STL/OFF/PLY): %s\n", filename); );

  dot = strrchr(filename, '.');
  if (dot && (!strcmp(dot, ".step") || !strcmp(dot, ".stp") || !strcmp(dot, ".STEP") || !strcmp(dot, ".STP"))) {
    fprintf(stderr, "Error: %s: STEP files are not supported, please export the geometry as STL, OFF or PLY (geometry-mesh-lib)\n", filename);
    return 0;
  }
  if (dot && (!strcmp(dot, ".stl") || !strcmp(dot, ".STL"))) ok = gmesh_read_stl(filename, &r);
  else {
    FILE *f = Open_File(filename, "rb", NULL);
    char head[8] = "";
    if (!f) return 0;
    if (fread(head, 1, 3, f) != 3) { fclose(f); fprintf(stderr, "Error: %s is empty (geometry-mesh-lib)\n", filename); return 0; }
    fseek(f, 0, SEEK_SET);
    if (!strncmp(head, "ply", 3)) ok = gmesh_read_ply(f, filename, &r);
    else if (!strncmp(head, "sol", 3)) { fclose(f); f = NULL; ok = gmesh_read_stl(filename, &r); } /* STL without .stl extension */
    else ok = gmesh_read_off(f, filename, &r);
    if (f) fclose(f);
  }
  if (!ok || !r.nv || !r.nf) { gmesh_raw_free(&r); return 0; }

  /* scale, resize and center (same conventions as interoff-lib) */
  if (!scale) scale = 1;
  for (i = 0; i < r.nv; i++) {
    Coords *p = &r.v[i];
    p->x *= scale; p->y *= scale; p->z *= scale;
    if (p->x<minx) minx=p->x;
    if (p->x>maxx) maxx=p->x;
    if (p->y<miny) miny=p->y;
    if (p->y>maxy) maxy=p->y;
    if (p->z<minz) minz=p->z;
    if (p->z>maxz) maxz=p->z;
  }
  {
    double rx = maxx-minx, ry = maxy-miny, rz = maxz-minz, fx = 1, fy = 1, fz = 1;
    double cx = (minx+maxx)/2, cy = (miny+maxy)/2, cz = (minz+maxz)/2;
    /* scale around the bounding box center, then optionally put it back */
    double bx = notcenter ? cx : 0, by = notcenter ? cy : 0, bz = notcenter ? cz : 0;
    if (xwidth && rx) {
      fx = xwidth/rx; fy = fx; fz = fx;
    }
    if (yheight && ry) {
      fy = yheight/ry; fz = fy;
      if (!xwidth) fx = fy;
    }
    if (zdepth && rz) {
      fz = zdepth/rz;
      if (!xwidth) fx = fz;
      if (!yheight) fy = fz;
    }
    if (!(notcenter && fx == 1 && fy == 1 && fz == 1))  /* else: coordinates kept exactly */
    for (i = 0; i < r.nv; i++) {
      r.v[i].x = (r.v[i].x - cx)*fx + bx;
      r.v[i].y = (r.v[i].y - cy)*fy + by;
      r.v[i].z = (r.v[i].z - cz)*fz + bz;
    }
    if (!(notcenter && fx == 1 && fy == 1 && fz == 1)) {
    minx = (minx-cx)*fx + bx; maxx = (maxx-cx)*fx + bx;
    miny = (miny-cy)*fy + by; maxy = (maxy-cy)*fy + by;
    minz = (minz-cz)*fz + bz; maxz = (maxz-cz)*fz + bz;
    }
  }
  m->bbmin = coords_set(minx, miny, minz); m->bbmax = coords_set(maxx, maxy, maxz);
  m->length_scale = sqrt((maxx-minx)*(maxx-minx) + (maxy-miny)*(maxy-miny) + (maxz-minz)*(maxz-minz));
  if (m->length_scale <= 0) m->length_scale = 1;

  /* weld vertices: identical within GMESH_WELD_TOL*diagonal (grid rounding) */
  map = malloc(r.nv*sizeof(long));
  m->vtx = malloc(r.nv*sizeof(Coords));
  if (!map || !m->vtx) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
  {
    double tol = GMESH_WELD_TOL*m->length_scale;
    long hcap = 16, *htab;
    while (hcap < 2*r.nv) hcap *= 2;
    htab = malloc(hcap*sizeof(long));
    if (!htab) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
    for (i = 0; i < hcap; i++) htab[i] = -1;
    m->nverts = 0;
    for (i = 0; i < r.nv; i++) {
      Coords p = r.v[i];
      int64_t q[3];
      uint64_t h;
      long s;
      if (tol > 0) { q[0] = (int64_t)llround(p.x/tol); q[1] = (int64_t)llround(p.y/tol); q[2] = (int64_t)llround(p.z/tol); }
      else { memcpy(&q[0], &p.x, 8); memcpy(&q[1], &p.y, 8); memcpy(&q[2], &p.z, 8); }
      h = gmesh_mix((uint64_t)q[0] ^ gmesh_mix((uint64_t)q[1] ^ gmesh_mix((uint64_t)q[2])));
      s = (long)(h & (uint64_t)(hcap-1));
      for (;;) {
        long j = htab[s];
        if (j < 0) { htab[s] = m->nverts; m->vtx[m->nverts] = p; map[i] = m->nverts++; break; }
        {
          Coords w = m->vtx[j]; int64_t qw[3];
          if (tol > 0) { qw[0] = (int64_t)llround(w.x/tol); qw[1] = (int64_t)llround(w.y/tol); qw[2] = (int64_t)llround(w.z/tol); }
          else { memcpy(&qw[0], &w.x, 8); memcpy(&qw[1], &w.y, 8); memcpy(&qw[2], &w.z, 8); }
          if (qw[0] == q[0] && qw[1] == q[1] && qw[2] == q[2]) { map[i] = j; break; }
        }
        s = (s+1) & (hcap-1);
      }
    }
    free(htab);
    m->nwelded = r.nv - m->nverts;
    m->vtx = gmesh_xrealloc(m->vtx, m->nverts*sizeof(Coords));
  }

  /* faces with welded indices (consecutive repeats removed), triangulation */
  m->nfaces = r.nf;
  m->faceArray = malloc((r.fs)*sizeof(unsigned long));
  if (!m->faceArray) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
  {
    long pos = 0, fi = 0, ntri = 0;
    long *tri = NULL, *tface = NULL, tcap = 0;
    m->faceSize = 0;
    while (pos < r.fs) {
      long n = r.f[pos], nn = 0, start = m->faceSize, j, before;
      unsigned long *out;
      m->faceArray[m->faceSize++] = 0;
      out = &m->faceArray[m->faceSize];
      for (j = 0; j < n; j++) {
        unsigned long id = map[r.f[pos+1+j]];
        if (nn && out[nn-1] == id) continue;
        out[nn++] = id;
      }
      while (nn > 1 && out[nn-1] == out[0]) nn--;
      m->faceArray[start] = nn;
      m->faceSize += nn;
      before = ntri;
      if (nn >= 3) gmesh_triangulate(m->vtx, out, nn, &tri, &ntri, &tcap);
      if (ntri > before) {
        tface = gmesh_xrealloc(tface, tcap*sizeof(long));
        for (j = before; j < ntri; j++) tface[j] = fi;
      }
      pos += n + 1; fi++;
    }
    m->tri = tri; m->tri_face = tface; m->ntris = ntri;
  }
  free(map);

  /* per-face properties (column-major, missing values 0) */
  for (i = 0; i < r.nf; i++) {
    if (r.pcnt[i] > m->nfaceprops) m->nfaceprops = r.pcnt[i];
    if (i && r.pcnt[i] != r.pcnt[i-1]) r.mixed = 1;
  }
  if (m->nfaceprops) {
    m->facePropSize = (long)m->nfaceprops*r.nf;
    m->facePropArray = calloc(m->facePropSize, sizeof(double));
    if (!m->facePropArray) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
    for (i = 0; i < r.nf; i++) for (k = 0; k < r.pcnt[i]; k++)
      m->facePropArray[k*r.nf + i] = r.p[r.poff[i]+k];
  }
  gmesh_raw_free(&r);

  /* normals; drop zero-area triangles */
  m->tri_normal = malloc((m->ntris ? m->ntris : 1)*sizeof(Coords));
  if (!m->tri_normal) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
  {
    long j = 0;
    for (i = 0; i < m->ntris; i++) {
      Coords a = m->vtx[m->tri[3*i]], b = m->vtx[m->tri[3*i+1]], c = m->vtx[m->tri[3*i+2]];
      Coords n = coords_xp(coords_sub(b, a), coords_sub(c, a));
      double l = coords_len(n);
      if (l <= 1e-14*m->length_scale*m->length_scale) { m->ndegenerate++; continue; }
      m->tri[3*j] = m->tri[3*i]; m->tri[3*j+1] = m->tri[3*i+1]; m->tri[3*j+2] = m->tri[3*i+2];
      m->tri_face[j] = m->tri_face[i];
      m->tri_normal[j] = coords_scale(n, 1/l);
      m->area += l/2;
      j++;
    }
    m->ntris = j;
  }
  if (!m->ntris) {
    fprintf(stderr, "Error: %s contains no valid triangles (geometry-mesh-lib)\n", filename);
    gmesh_free(m); return 0;
  }

  /* topology, and consistent orientation by propagation over shared edges */
  {
    gmesh_etab T;
    long nused = 0, *queue, *visited;
    char *used;
    gmesh_etab_build(&T, m->tri, m->ntris);
    queue = malloc(m->ntris*sizeof(long)); visited = calloc(m->ntris, sizeof(long));
    used = calloc(m->nverts, 1);
    if (!queue || !visited || !used) { fprintf(stderr, "Error: memory allocation (geometry-mesh-lib)\n"); exit(-1); }
    /* BFS: flip triangles so that shared (manifold) edges run in opposite directions */
    for (i = 0; i < m->ntris; i++) {
      long qh = 0, qt = 0;
      if (visited[i]) continue;
      visited[i] = 1; queue[qt++] = i;
      while (qh < qt) {
        long t = queue[qh++];
        for (k = 0; k < 3; k++) {
          long a = m->tri[3*t+k], b = m->tri[3*t+(k+1)%3], o;
          gmesh_edge *E = gmesh_edge_get(&T, a, b);
          if (E->n != 2) continue;
          o = (E->t0 == t) ? E->t1 : E->t0;
          if (visited[o]) continue;
          { /* does o use the edge a->b in the same direction? then flip it */
            int kk, same = 0;
            for (kk = 0; kk < 3; kk++) if (m->tri[3*o+kk] == a && m->tri[3*o+(kk+1)%3] == b) same = 1;
            if (same) {
              long tmp = m->tri[3*o+1]; m->tri[3*o+1] = m->tri[3*o+2]; m->tri[3*o+2] = tmp;
              m->tri_normal[o] = coords_scale(m->tri_normal[o], -1);
            }
          }
          visited[o] = 1; queue[qt++] = o;
        }
      }
    }
    free(T.e);
    gmesh_etab_build(&T, m->tri, m->ntris);
    m->nedges = T.n; m->oriented = 1;
    for (i = 0; i < T.cap; i++) {
      if (!T.e[i].n) continue;
      if (T.e[i].n == 1) m->nboundary++;
      else if (T.e[i].n > 2) m->nnonmanifold++;
      else if (T.e[i].dir != 0) m->oriented = 0;
    }
    if (m->nnonmanifold) m->oriented = 0;
    free(T.e);
    for (i = 0; i < 3*m->ntris; i++) if (!used[m->tri[i]]) { used[m->tri[i]] = 1; nused++; }
    m->euler = (int)(nused - m->nedges + m->ntris);
    m->closed = (m->nboundary == 0 && m->nnonmanifold == 0);
    free(queue); free(visited); free(used);
  }

  /* volume (divergence theorem); outward normals for closed oriented meshes */
  {
    double vol = 0;
    for (i = 0; i < m->ntris; i++) {
      Coords a = m->vtx[m->tri[3*i]], b = m->vtx[m->tri[3*i+1]], c = m->vtx[m->tri[3*i+2]];
      vol += coords_sp(a, coords_xp(b, c))/6;
    }
    if (m->closed && m->oriented && vol < 0) {
      for (i = 0; i < m->ntris; i++) {
        long tmp = m->tri[3*i+1]; m->tri[3*i+1] = m->tri[3*i+2]; m->tri[3*i+2] = tmp;
        m->tri_normal[i] = coords_scale(m->tri_normal[i], -1);
      }
      vol = -vol; m->flipped = 1;
    }
    m->volume = (m->closed && m->oriented) ? vol : 0;
  }

  /* bounding sphere (Ritter) */
  {
    Coords a = m->vtx[0], b = a, c;
    double d, best = -1, rad;
    for (i = 0; i < m->nverts; i++) { d = coords_len(coords_sub(m->vtx[i], a)); if (d > best) { best = d; b = m->vtx[i]; } }
    c = b; best = -1;
    for (i = 0; i < m->nverts; i++) { d = coords_len(coords_sub(m->vtx[i], b)); if (d > best) { best = d; c = m->vtx[i]; } }
    m->bscenter = coords_scale(coords_add(b, c), 0.5); rad = best/2;
    for (i = 0; i < m->nverts; i++) {
      d = coords_len(coords_sub(m->vtx[i], m->bscenter));
      if (d > rad) { double nr = (rad + d)/2;
        m->bscenter = coords_add(m->bscenter, coords_scale(coords_sub(m->vtx[i], m->bscenter), (nr - rad)/d)); rad = nr; }
    }
    m->bsradius = rad;
  }

  gmesh_build_bvh(m);
  m->ntris3 = 3*m->ntris;

  MPI_MASTER( gmesh_print_info(m); );
  return m->ntris;
} /* gmesh_init */

/* ========================================================================== */
/*  Ray tracing                                                               */
/* ========================================================================== */

/* slab test of the line o + t*d against a box, for t in [tlo, thi] */
#pragma acc routine seq
static int gmesh_slab(Coords lo, Coords hi, Coords o, Coords d, double tlo, double thi)
{
  int k;
  for (k = 0; k < 3; k++) {
    double ok = gmesh_coord(o,k), dk = gmesh_coord(d,k), l = gmesh_coord(lo,k), h = gmesh_coord(hi,k);
    double pad = 1e-9*(h - l) + 1e-12*(fabs(l) + fabs(h)) + 1e-300;
    l -= pad; h += pad;
    if (dk == 0) { if (ok < l || ok > h) return 0; }
    else {
      double t1 = (l - ok)/dk, t2 = (h - ok)/dk;
      if (t1 > t2) { double tmp = t1; t1 = t2; t2 = tmp; }
      if (t1 > tlo) tlo = t1;
      if (t2 < thi) thi = t2;
      if (tlo > thi) return 0;
    }
  }
  return 1;
}

/* Moeller-Trumbore with tolerant barycentric coordinates.
   returns 1 on hit, with t, and edge flag */
#pragma acc routine seq
static int gmesh_tri_hit(gmesh_struct *m, long t, Coords o, Coords d, double *tt, int *edge)
{
  Coords v0 = m->vtx[m->tri[3*t]], v1 = m->vtx[m->tri[3*t+1]], v2 = m->vtx[m->tri[3*t+2]];
  Coords e1 = coords_sub(v1, v0), e2 = coords_sub(v2, v0);
  Coords p = coords_xp(d, e2), s, q;
  double det = coords_sp(e1, p), inv, u, v, eps = GMESH_EPSILON;
  if (fabs(det) <= 1e-12*coords_len(e1)*coords_len(e2)*coords_len(d)) return 0;   /* parallel */
  inv = 1/det;
  s = coords_sub(o, v0);
  u = coords_sp(s, p)*inv;
  if (u < -eps || u > 1+eps) return 0;
  q = coords_xp(s, e1);
  v = coords_sp(d, q)*inv;
  if (v < -eps || u + v > 1+eps) return 0;
  *tt = coords_sp(e2, q)*inv;
  *edge = (u < eps || v < eps || u + v > 1-eps);
  return 1;
}

/* generic BVH traversal. mode 0: sorted buffer, 1: four slots (interoff), 2: count */
#pragma acc routine seq
static long gmesh_trace(gmesh_struct *m, Coords o, Coords d, double tmin, int mode,
                        gmesh_hit *hits, int maxhits, int *nstored, int *ambiguous)
{
  long stack[GMESH_BVH_STACK+2];
  int top = 0;
  long total = 0;
  double dlen = coords_len(d);
  double ttol = (dlen > 0 ? 1e-9*m->length_scale/dlen : 0);
  if (dlen <= 0 || !m->nnodes) return 0;
  stack[top++] = 0;
  while (top > 0) {
    gmesh_node *N = &m->node[stack[--top]];
    double thi = FLT_MAX;
    if (mode == 0 && *nstored == maxhits && maxhits > 0) thi = hits[maxhits-1].t + ttol;
    if (!gmesh_slab(N->lo, N->hi, o, d, tmin, thi)) continue;
    if (N->count == 0) {
      if (top + 2 > GMESH_BVH_STACK) continue;   /* cannot happen: depth limited at build */
      stack[top++] = N->first; stack[top++] = N->first + 1;
      continue;
    }
    {
      long t;
      for (t = N->first; t < N->first + N->count; t++) {
        double tt; int edge, io, k, dup = 0;
        gmesh_hit h;
        if (!gmesh_tri_hit(m, t, o, d, &tt, &edge)) continue;
        if (tt <= tmin) continue;
        io = (coords_sp(m->tri_normal[t], d) < 0) ? 1 : -1;
        if (mode == 2) {
          total++;
          if (edge || fabs(tt) <= ttol) *ambiguous = 1;
          continue;
        }
        h.t = tt; h.normal = m->tri_normal[t]; h.tri = t; h.face = m->tri_face[t]; h.in_out = io; h.edge = edge;
        /* hits on shared edges/vertices: keep once */
        for (k = 0; k < (mode == 1 ? 4 : *nstored); k++)
          if (hits[k].in_out == io && fabs(hits[k].t - tt) <= ttol) { dup = 1; break; }
        if (dup) continue;
        total++;
        if (mode == 1) {
          gmesh_hit tmp;
          if (tt < 0) { if (tt > hits[0].t) hits[0] = h; }
          else if (tt < hits[3].t) {
            hits[3] = h;
            if (hits[3].t < hits[2].t) { tmp = hits[2]; hits[2] = hits[3]; hits[3] = tmp; }
            if (hits[2].t < hits[1].t) { tmp = hits[1]; hits[1] = hits[2]; hits[2] = tmp; }
          }
        } else {
          int pos = *nstored;
          if (pos == maxhits) { if (maxhits == 0 || tt >= hits[maxhits-1].t) continue; pos = maxhits-1; }
          else (*nstored)++;
          while (pos > 0 && hits[pos-1].t > tt) { hits[pos] = hits[pos-1]; pos--; }
          hits[pos] = h;
        }
      }
    }
  }
  return total;
}

#pragma acc routine seq
int gmesh_intersect(gmesh_struct *m, Coords pos, Coords dir, double tmin,
                    gmesh_hit *hits, int maxhits, long *ntotal)
{
  int nstored = 0, amb = 0;
  long tot = gmesh_trace(m, pos, dir, tmin, 0, hits, maxhits, &nstored, &amb);
  if (ntotal) *ntotal = tot;
  return nstored;
}

#pragma acc routine seq
int gmesh_intersect_t0t3(double *t0, double *t3, Coords *n0, Coords *n3,
                         long *f0, long *f3, double x, double y, double z,
                         double vx, double vy, double vz, gmesh_struct *m)
{
  gmesh_hit h[4];
  int k, nst = 0, amb = 0, i;
  long tot;
  for (k = 0; k < 4; k++) {
    h[k].t = (k == 0 ? -FLT_MAX : FLT_MAX); h[k].normal = coords_set(0,0,0);
    h[k].tri = h[k].face = 0; h[k].in_out = 0; h[k].edge = 0;
  }
  tot = gmesh_trace(m, coords_set(x,y,z), coords_set(vx,vy,vz), -FLT_MAX, 1, h, 4, &nst, &amb);
  if (!tot) return 0;
  i = (h[0].t == -FLT_MAX) ? 1 : 0;
  if (t0) *t0 = h[i].t;
  if (n0) *n0 = h[i].normal;
  if (f0) *f0 = h[i].face;
  if (t3) *t3 = h[i+1].t;
  if (n3) *n3 = h[i+1].normal;
  if (f3) *f3 = h[i+1].face;
  if (h[1].t == FLT_MAX && t3) *t3 = 0.0;
  return (int)(tot < 4 ? tot : 4);
}

#pragma acc routine seq
int gmesh_inside(gmesh_struct *m, Coords pos)
{
  /* fixed, "irrational" directions: no random numbers are consumed */
  const double D[3][3] = { { 0.4719, 0.6213, 0.6254 }, { 0.8017, -0.3259, 0.5012 }, { -0.2263, 0.9147, -0.3349 } };
  int j, votes = 0;
  if (pos.x < m->bbmin.x || pos.x > m->bbmax.x || pos.y < m->bbmin.y || pos.y > m->bbmax.y
   || pos.z < m->bbmin.z || pos.z > m->bbmax.z) return 0;
  for (j = 0; j < 3; j++) {
    int nst = 0, amb = 0;
    long n = gmesh_trace(m, pos, coords_set(D[j][0], D[j][1], D[j][2]), 0, 2, NULL, 0, &nst, &amb);
    if (!amb) return (int)(n % 2);
    votes += (int)(n % 2);
  }
  return votes >= 2;
}

#pragma acc routine seq
double gmesh_face_prop(gmesh_struct *m, long face, int k, double def)
{
  if (!m || !m->facePropArray || k < 0 || k >= m->nfaceprops || face < 0 || face >= m->nfaces) return def;
  return m->facePropArray[(long)k*m->nfaces + face];
}

/* ========================================================================== */
/*  Info, display, free                                                       */
/* ========================================================================== */

void gmesh_print_info(gmesh_struct *m)
{
  printf("  %s: %ld vertices (%ld duplicates welded), %ld faces, %ld triangles",
         m->filename, m->nverts, m->nwelded, m->nfaces, m->ntris);
  if (m->ndegenerate) printf(" (%ld degenerate dropped)", m->ndegenerate);
  printf("\n  Bounding box [%g:%g]x[%g:%g]x[%g:%g], area %g",
         m->bbmin.x, m->bbmax.x, m->bbmin.y, m->bbmax.y, m->bbmin.z, m->bbmax.z, m->area);
  if (m->volume) printf(", volume %g", m->volume);
  printf("\n  Topology: %ld edges, Euler characteristic %d, %s, %s%s\n", m->nedges, m->euler,
         m->closed ? "closed" : "OPEN", m->oriented ? "consistently oriented" : "NOT consistently oriented",
         m->flipped ? " (orientation reversed to outward normals)" : "");
  if (!m->closed)
    printf("  Warning: %ld boundary and %ld non-manifold edges: the mesh does not enclose a volume.\n",
           m->nboundary, m->nnonmanifold);
  if (m->nfaceprops) printf("  Per-face properties: %d\n", m->nfaceprops);
}

void gmesh_display(gmesh_struct *m)
{
  long i, j;
  if (!m || !m->faceSize) return;
  if (mcdotrace == 2) {
    /* polyhedron as JSON, polygons as in the file */
    size_t cap = 4096, len = 0;
    char *s = malloc(cap);
    #define GMESH_APPEND(...) do { int n_ = snprintf(s+len, cap-len, __VA_ARGS__); \
      if (n_ >= 0 && len + n_ + 1 > cap) { while (len + n_ + 1 > cap) cap *= 2; s = gmesh_xrealloc(s, cap); snprintf(s+len, cap-len, __VA_ARGS__); } \
      if (n_ > 0) len += n_; } while (0)
    if (!s) return;
    s[0] = 0;
    GMESH_APPEND("{ \"vertices\": [");
    for (i = 0; i < m->nverts; i++) GMESH_APPEND("[%g, %g, %g]%s", m->vtx[i].x, m->vtx[i].y, m->vtx[i].z, i < m->nverts-1 ? ", " : "");
    GMESH_APPEND("], \"faces\": [");
    for (i = 0; i < m->faceSize; ) {
      long n = m->faceArray[i];
      GMESH_APPEND("{ \"face\": [");
      for (j = 1; j <= n; j++) GMESH_APPEND("%lu%s", m->faceArray[i+j], j < n ? ", " : "");
      GMESH_APPEND("]}");
      i += n + 1;
      if (i < m->faceSize) GMESH_APPEND(", ");
    }
    GMESH_APPEND("]}");
    #undef GMESH_APPEND
    mcdis_polyhedron(s);
    free(s);
  } else {
    /* each polygon edge once */
    gmesh_etab T;
    long nfe = 0, cnt = 0;
    double ratio;
    for (i = 0; i < m->faceSize; i += m->faceArray[i] + 1) nfe += m->faceArray[i];
    T.cap = 16; while (T.cap < 2*nfe+16) T.cap *= 2;
    T.e = calloc(T.cap, sizeof(gmesh_edge)); T.n = 0;
    if (!T.e) return;
    ratio = (double)GMESH_DISPLAY_MAX/(double)(nfe ? nfe : 1);
    for (i = 0; i < m->faceSize; i += m->faceArray[i] + 1) {
      long n = m->faceArray[i];
      for (j = 0; j < n; j++) {
        long a = m->faceArray[i+1+j], b = m->faceArray[i+1+(j+1)%n];
        gmesh_edge *E;
        if (a == b) continue;
        E = gmesh_edge_get(&T, a, b);
        if (E->n++) continue;
        if (ratio < 1 && rand01() > ratio) continue;
        mcdis_line(m->vtx[a].x, m->vtx[a].y, m->vtx[a].z, m->vtx[b].x, m->vtx[b].y, m->vtx[b].z);
        cnt++;
      }
    }
    free(T.e);
  }
}

void gmesh_free(gmesh_struct *m)
{
  if (!m) return;
  free(m->vtx); free(m->tri); free(m->tri_face); free(m->tri_normal); free(m->node);
  free(m->faceArray); free(m->facePropArray);
  memset(m, 0, sizeof(gmesh_struct));
}

#endif /* GEOMETRY_MESH_LIB_C */

/* end of geometry-mesh-lib.c */
