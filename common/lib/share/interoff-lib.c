/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2008, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Runtime: share/interoff-lib.c
*
* %Identification
* Written by: Reynald Arnerin
* Date:    Jun 12, 2008
* Origin: ILL
* Release: $Revision$
* Version: McStas X.Y
*
* Object File Format intersection library for McStas/McXtrace.
*
* Such files may be obtained with e.g.
*   qhull < points.xyz Qx Qv Tv o > points.off
* where points.xyz has format (it supports comments):
*   3
*   <nb_points>
*   <x> <y> <z>
*   ...
* The resulting file should have its first line being changed from '3' into 'OFF'.
* It can then be displayed with geomview.
* A similar, but somewhat older solution is to use 'powercrust' with e.g.
*   powercrust -i points.xyz
* which will generate a 'pc.off' file to be renamed as suited.
*
* Per-face properties (merged from r-interoff-lib by Peter Link / Gaetano
* Mangiapia, 2017/2022): numbers following the vertex indices of a face are
* stored per face, e.g. m, alpha and W supermirror values:
*   4  0 5 6 4   1.0 6.07 0.003
*
*******************************************************************************/

#ifndef INTEROFF_LIB_H
#include "interoff-lib.h"
#endif

#ifndef INTEROFF_LIB_C
#define INTEROFF_LIB_C "$Revision$"

#include <ctype.h>

#ifdef OPENACC // If on GPU map fprintf to printf
#define fprintf(stderr,...) printf(__VA_ARGS__)
#endif

#pragma acc routine
double off_F(double x, double y,double z,double A,double B,double C,double D) {
  return ( A*x + B*y + C*z + D );
}

#pragma acc routine
char off_sign(double a) {
  if (a<0)       return(-1);
  else if (a==0) return(0);
  else           return(1);
}

/* component k (0,1,2) of a Coords */
#pragma acc routine
double off_coord(Coords c, int k) {
  return (k==0 ? c.x : (k==1 ? c.y : c.z));
}

// off_normal ******************************************************************
// gives the unit normal vector of polygon p (Newell method, any number of vertices)
// returns the length of the non-normalised Newell vector (= 2*area)
#pragma acc routine
double off_normal(Coords* n, polygon p)
{
  int i=0,j=0;
  double len;
  n->x=0;n->y=0;n->z=0;
  for (i = 0, j = p.npol-1; i < p.npol; j = i++)
  {
    Coords v1=p.vtx[p.idx[i]];
    Coords v2=p.vtx[p.idx[j]];
    n->x += (v1.y - v2.y) * (v1.z + v2.z);
    n->y += (v1.z - v2.z) * (v1.x + v2.x);
    n->z += (v1.x - v2.x) * (v1.y + v2.y);
  }
  len = sqrt(n->x*n->x + n->y*n->y + n->z*n->z);
  if (len > 0) {
    n->x /= len; n->y /= len; n->z /= len;
  }
  return len;
} /* off_normal */

// off_pnpoly ******************************************************************
//based on http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
//The polygon is projected on the coordinate plane where its area is largest,
//selected from the (pre-computed) polygon normal.
//return 0 if the vertex is out
//    1 if it is in
//   -1 if on the boundary
#pragma acc routine
int off_pnpoly(polygon p, Coords v)
{
  int i=0, j=0, c = 0;
  int pol2dx=0,pol2dy=1;          //2d restriction of the poly
  double ax=fabs(p.normal.x), ay=fabs(p.normal.y), az=fabs(p.normal.z);
  double x, y;

  /* projected areas are proportional to the normal components: drop the largest */
  if(az<ax){
    if(ax<ay){
      pol2dy=2;   /* scratch y */
    }else{
      pol2dx=2;   /* scratch x */
    }
  }else if (az<ay){
    pol2dy=2;     /* scratch y */
  }
  x = off_coord(v, pol2dx);
  y = off_coord(v, pol2dy);

  //trace rays and test number of intersection
  for (i = 0, j = p.npol-1; i < p.npol; j = i++) {
    Coords vi = p.vtx[p.idx[i]], vj = p.vtx[p.idx[j]];
    double xi = off_coord(vi, pol2dx), yi = off_coord(vi, pol2dy);
    double xj = off_coord(vj, pol2dx), yj = off_coord(vj, pol2dy);
    double ex = xj - xi, ey = yj - yi;
    double px = x - xi,  py = y - yi;
    double cross = px*ey - py*ex;       /* = |e| * distance to the edge line */
    double len2  = ex*ex + ey*ey;

    /* is the point on the edge [vi,vj] (within OFF_EPSILON) ? */
    if (cross*cross <= OFF_EPSILON*OFF_EPSILON*len2) {
      double dot = px*ex + py*ey;
      if ((dot >= 0 && dot <= len2) || px*px+py*py <= OFF_EPSILON*OFF_EPSILON
          || (x-xj)*(x-xj)+(y-yj)*(y-yj) <= OFF_EPSILON*OFF_EPSILON) {
        c = -1;
        break;
      }
    }

    if ((((yi<=y) && (y<yj)) || ((yj<=y) && (y<yi))) &&
        (x < ex * (y - yi) / ey + xi))
      c = !c;
  }

  return c;
} /* off_pnpoly */

// off_intersectPoly ***********************************************************
//gives the intersection vertex between ray [a,b) and polygon p and its parametric value on (a b)
//based on http://geometryalgorithms.com/Archive/algorithm_0105/algorithm_0105.htm
#pragma acc routine
int off_intersectPoly(intersection *inter, Coords a, Coords b, polygon p)
{
  //direction vector of [a,b]
  Coords dir = {b.x-a.x, b.y-a.y, b.z-a.z};

  //the (unit) normal vector to the polygon, pre-computed at init
  Coords normale=p.normal;

  //scalar products
  MCNUM nw0  = p.D - scalar_prod(normale.x,normale.y,normale.z,a.x,a.y,a.z);
  MCNUM ndir = scalar_prod(normale.x,normale.y,normale.z,dir.x,dir.y,dir.z);
  inter->time = inter->edge = inter->in_out=0;
  inter->v = inter->normal = coords_set(0,0,1);
  inter->index = 0;

  if (fabs(ndir) < OFF_EPSILON)    // ray is parallel to polygon plane (or degenerate polygon)
    return 0;

  // get intersect point of ray with polygon plane
  inter->time = nw0 / ndir;            //parametric value the point on line (a,b)

  inter->v = coords_set(a.x + inter->time * dir.x,// intersect point of ray and plane
    a.y + inter->time * dir.y,
    a.z + inter->time * dir.z);

  int res=off_pnpoly(p,inter->v);

  inter->edge=(res==-1);
  if (ndir<0)
    inter->in_out=1;  //the negative dot product means we enter the surface
  else
    inter->in_out=-1;

  inter->normal=p.normal;

  return res;         //true if the intersection point lies inside the poly
} /* off_intersectPoly */

// off_insert4 *****************************************************************
// keep the 4 intersections of interest: t[0] is the largest negative time,
// t[1..3] are the three smallest positive times (sorted). Initially
// t = [-FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX].
// Duplicates (same time, same direction: hit on an edge/vertex shared by
// several faces) are only counted once.
#pragma acc routine
void off_insert4(intersection *t, int *t_size, intersection x)
{
  int k;
  intersection xtmp;
  for (k=0; k<4; k++)
    if (t[k].in_out == x.in_out && fabs(t[k].time - x.time) < OFF_EPSILON)
      return;
  if (*t_size < 4) (*t_size)++;
  if (x.time < 0) {
    if (x.time > t[0].time) t[0]=x;
  } else if (x.time < t[3].time) {
    t[3]=x;
    if (t[3].time < t[2].time) { xtmp = t[2]; t[2] = t[3]; t[3] = xtmp; }
    if (t[2].time < t[1].time) { xtmp = t[1]; t[1] = t[2]; t[2] = xtmp; }
  }
} /* off_insert4 */

// off_store_intersection ******************************************************
// store intersection x in t, either as full list (OFF_LEGACY) or 4 slots
#pragma acc routine
void off_store_intersection(intersection *t, int *t_size, intersection x)
{
#ifdef OFF_LEGACY
  if (*t_size >= OFF_INTERSECT_MAX) {
    #ifndef OPENACC
    static int warned=0;
    if (!warned++)
      fprintf(stderr, "Warning: number of intersection exceeded (%d) (interoff-lib/off_store_intersection)\n", OFF_INTERSECT_MAX);
    #endif
    return;
  }
  t[(*t_size)++]=x;
#else
  off_insert4(t, t_size, x);
#endif
}

// off_readline ****************************************************************
// read the rest of the current line (without '\n') into a growing buffer.
// returns the number of chars read, or -1 at EOF with nothing read.
long off_readline(FILE *f, char **buf, size_t *cap)
{
  long n=0;
  int  c;
  if (!*buf || !*cap) {
    *cap = CHAR_BUF_LENGTH;
    *buf = malloc(*cap);
    if (!*buf) { fprintf(stderr, "Error: memory allocation (interoff/off_readline)\n"); exit(-1); }
  }
  while ((c = fgetc(f)) != EOF && c != '\n') {
    if ((size_t)(n+1) >= *cap) {
      char *tmp = realloc(*buf, 2*(*cap));
      if (!tmp) { fprintf(stderr, "Error: memory allocation (interoff/off_readline)\n"); exit(-1); }
      *buf = tmp; *cap *= 2;
    }
    (*buf)[n++] = (char)c;
  }
  (*buf)[n] = '\0';
  if (c == EOF && n == 0) return -1;
  return n;
} /* off_readline */

// off_getBlocksIndex **********************************************************
/*reads the header of the OFF/PLY file:
OFF:
  line 1  [ST][C][N]OFF [nbVertex nbFaces nbEdges]
  line 2  nbVertex nbFaces nbEdges   (unless given on line 1)
PLY:
  ply / format ascii 1.0 / element vertex N / property ... / element face M / ... / end_header
'lineVertex' is set when vertex lines carry more than 3 numbers (to be skipped)
*/
FILE *off_getBlocksIndex(char* filename, long* vtxSize, long* polySize, int *lineVertex)
{
  FILE* f = Open_File(filename,"r", NULL); /* from read_table-lib: FILE *Open_File(char *name, char *Mode, char *path) */
  if (!f) return (f);

  char *line=NULL;
  size_t cap=0;
  long   len=0;
  *vtxSize = *polySize = 0;
  *lineVertex = 0;

  /* **************** start to read the file header */
  do { /* skip empty lines before the header */
    len = off_readline(f, &line, &cap);
    if (len < 0) {
      fprintf(stderr, "Error: Can not read 1st line in file %s (interoff/off_getBlocksIndex)\n", filename);
      exit(1);
    }
  } while (strspn(line, " \t\r") == (size_t)len);

  { /* a '\r' followed by other characters indicates classic-Mac line endings */
    char *cr = strchr(line, '\r');
    if (cr && cr[strspn(cr, "\r \t")] != '\0') {
      fprintf(stderr,"Error: First line in %s is not terminated by '\\n' (CR line endings?).\n"
              "       Please convert the file to Unix or DOS line endings.\n", filename);
      fclose(f); free(line);
      return(NULL);
    }
  }

  char *hdr = line + strspn(line, " \t");
  if (!strncmp(hdr,"ply",3)) {
    /* PLY file: read all lines until find 'end_header'
       and locate 'element face' and 'element vertex' */
    int in_vertex=0, nvprop=0, got_vertex=0;
    do
    {
      len = off_readline(f, &line, &cap);
      if (len < 0)
      {
        fprintf(stderr, "Error: Can not read line in file %s (interoff/off_getBlocksIndex)\n", filename);
        exit(1);
      }
      if (!strncmp(line,"element",7)) {
        in_vertex = 0;
        if (!strncmp(line,"element face",12)) {
          if (!got_vertex) {
            fprintf(stderr, "Error: PLY file %s defines faces before vertices, which is not supported (interoff/off_getBlocksIndex)\n", filename);
            exit(1);
          }
          sscanf(line,"element face %ld",polySize);
        } else if (!strncmp(line,"element vertex",14)) {
          sscanf(line,"element vertex %ld",vtxSize);
          in_vertex = got_vertex = 1;
        } else if (!got_vertex) {
          fprintf(stderr, "Error: PLY file %s has element '%s' before the vertices, which is not supported (interoff/off_getBlocksIndex)\n", filename, line);
          exit(1);
        }
      } else if (in_vertex && !strncmp(line,"property",8)) {
        char type[64]="", name[64]="";
        sscanf(line, "property %63s %63s", type, name);
        if ((nvprop==0 && strcmp(name,"x")) || (nvprop==1 && strcmp(name,"y")) || (nvprop==2 && strcmp(name,"z"))) {
          fprintf(stderr, "Error: PLY file %s: the first vertex properties must be x y z (got '%s') (interoff/off_getBlocksIndex)\n", filename, name);
          exit(1);
        }
        nvprop++;
      }
      else if (!strncmp(line,"format",6) && strncmp(line,"format ascii",12))
        exit(fprintf(stderr,
          "Error: Can not read binary PLY file %s, only 'format ascii' (interoff/off_getBlocksIndex)\n%s\n",
          filename, line));
    } while (strncmp(line,"end_header",10));
    *lineVertex = (nvprop > 3);
  } else {
    /* OFF file: [ST][C][N]OFF, or legacy '3' */
    char *p = hdr, *rest;
    if (*p == '3' && (p[1]=='\0' || isspace((unsigned char)p[1]))) {
      rest = p+1;
    } else {
      while (*p && strchr("STCN", *p)) p++;
      if (strncmp(p, "OFF", 3)) {
        fprintf(stderr, "Error: %s is probably not an OFF, NOFF or PLY file (interoff/off_getBlocksIndex).\n"
                        "       Requires first line to be 'OFF', '3' or 'ply'.\n",filename);
        fclose(f); free(line);
        return(NULL);
      }
      if (p[3] && !isspace((unsigned char)p[3])) {
        fprintf(stderr, "Error: %s: header '%s' is not supported (4OFF/nOFF). (interoff/off_getBlocksIndex).\n",
          filename, hdr);
        fclose(f); free(line);
        return(NULL);
      }
      *lineVertex = (p != hdr); /* extra vertex data (normals, colours, texture) */
      rest = p+3;
    }
    /* the counts may follow the keyword on the same line */
    if (sscanf(rest, "%ld %ld", vtxSize, polySize) != 2) {
      do  /* OFF file: skip # comments and empty lines which may be there */
      {
        len = off_readline(f, &line, &cap);
        if (len < 0)
        {
          fprintf(stderr, "Error: Can not read line in file %s (interoff/off_getBlocksIndex)\n", filename);
          exit(1);
        }
        hdr = line + strspn(line, " \t\r");
      } while (hdr[0]=='#' || hdr[0]=='\0');
      //line = nblines of vertex,faces and edges arrays
      if (sscanf(hdr,"%ld %ld",vtxSize,polySize) != 2) {
        fprintf(stderr, "Error: can not read number of vertices and faces in file %s: '%s' (interoff/off_getBlocksIndex)\n",
          filename, hdr);
        fclose(f); free(line);
        return(NULL);
      }
    }
  }
  free(line);
  if (*vtxSize <= 0 || *polySize <= 0) {
    fprintf(stderr, "Error: file %s defines %ld vertices and %ld faces (interoff/off_getBlocksIndex)\n",
      filename, *vtxSize, *polySize);
    fclose(f);
    return(NULL);
  }

  /* The FILE is left opened ready to read 'vtxSize' vertices (vtxSize *3 numbers)
     and then polySize polygons (rows) */

  return(f);
} /* off_getBlocksIndex */

// off_init_planes *************************************************************
//gives the equations of 2 independent planes which both contain the line (a,b):
//n.r + D = 0 with n = dir x e_i, e_i being the two coordinate axes which are not
//the dominant direction of dir (each normal thus has one zero component).
//Returns 0 when a==b.
#pragma acc routine
int off_init_planes(Coords a, Coords b, Coords *n1, MCNUM* D1, Coords *n2, MCNUM* D2)
{
  //direction vector of [a b]
  Coords dir={b.x-a.x, b.y-a.y, b.z-a.z};
  double ax=fabs(dir.x), ay=fabs(dir.y), az=fabs(dir.z);
  Coords dxe = coords_set(0, dir.z, -dir.y);   /* dir x e_x */
  Coords dye = coords_set(-dir.z, 0, dir.x);   /* dir x e_y */
  Coords dze = coords_set(dir.y, -dir.x, 0);   /* dir x e_z */

  if (ax==0 && ay==0 && az==0) return 0;
  if (az >= ax && az >= ay)      { *n1 = dxe; *n2 = dye; }
  else if (ay >= ax && ay >= az) { *n1 = dxe; *n2 = dze; }
  else                           { *n1 = dye; *n2 = dze; }
  *D1 = -scalar_prod(n1->x,n1->y,n1->z, a.x,a.y,a.z);
  *D2 = -scalar_prod(n2->x,n2->y,n2->z, a.x,a.y,a.z);
  return 1;
} /* off_init_planes */

// off_straddle ****************************************************************
// returns 1 when the vertices of face (npol indices in idx) are not all
// strictly on the same side of the plane n.r + D = 0
#pragma acc routine seq
int off_straddle(const Coords* vtxArray, const unsigned long *idx, int npol, const Coords *n, double D)
{
  int j;
  const Coords *v=&vtxArray[idx[0]];
  double f0 = n->x*v->x + n->y*v->y + n->z*v->z + D;
  if (f0 == 0) return 1;
  if (f0 > 0) {
    for (j=1; j<npol; j++) {
      v=&vtxArray[idx[j]];
      if (n->x*v->x + n->y*v->y + n->z*v->z + D <= 0) return 1;
    }
  } else {
    for (j=1; j<npol; j++) {
      v=&vtxArray[idx[j]];
      if (n->x*v->x + n->y*v->y + n->z*v->z + D >= 0) return 1;
    }
  }
  return 0;
} /* off_straddle */

// off_line_hits_box ***********************************************************
// returns 0 when the infinite line a + s*dir certainly misses the box [lo,hi]
// (slightly enlarged to be safe against rounding), 1 otherwise
#pragma acc routine
int off_line_hits_box(Coords a, Coords dir, Coords lo, Coords hi)
{
  double smin=-FLT_MAX, smax=FLT_MAX;
  int k;
  for (k=0; k<3; k++) {
    double ak=off_coord(a,k), dk=off_coord(dir,k);
    double l=off_coord(lo,k), h=off_coord(hi,k);
    double tol = 1e-9*(fabs(l)+fabs(h)+(h-l)) + OFF_EPSILON;
    l -= tol; h += tol;
    if (dk == 0) {
      if (ak < l || ak > h) return 0;
    } else {
      double s1=(l-ak)/dk, s2=(h-ak)/dk;
      if (s1 > s2) { double tmp=s1; s1=s2; s2=tmp; }
      if (s1 > smin) smin=s1;
      if (s2 < smax) smax=s2;
      if (smin > smax) return 0;
    }
  }
  return 1;
} /* off_line_hits_box */

// off_clip_3D_mod *************************************************************
// straight line (a,b) intersections with all faces
#pragma acc routine
int off_clip_3D_mod(intersection* t, Coords a, Coords b,
  Coords* vtxArray, unsigned long vtxSize, unsigned long* faceArray,
  unsigned long faceSize, Coords* normalArray, double* DArray)
{
  Coords n1, n2;
  MCNUM  D1=0, D2=0;      //planes containing [a,b]
  int t_size=0;
  unsigned long i=0,indPoly=0;

  if (!off_init_planes(a, b, &n1, &D1, &n2, &D2)) return 0;

  //exploring the polygons :
  while (i<faceSize)
  {
    polygon pol;
    pol.npol  = faceArray[i];                //nb vertex of polygon
    pol.vtx   = vtxArray;
    pol.idx   = &faceArray[i+1];             //polygon's vertex indices in vtxArray

    // a polygon hit by the line must straddle both planes containing it
    if (off_straddle(vtxArray, pol.idx, pol.npol, &n1, D1)
     && off_straddle(vtxArray, pol.idx, pol.npol, &n2, D2))
    {
      intersection x;
      pol.normal=normalArray[indPoly];
      pol.D     =DArray[indPoly];
      if (off_intersectPoly(&x, a, b, pol))
      {
        x.index = indPoly;
        off_store_intersection(t, &t_size, x);
      }
    }
    i += pol.npol+1;
    indPoly++;
  } /* while i<faceSize */
  return t_size;
} /* off_clip_3D_mod */

// off_clip_3D_mod_grav *************************************************************
/*******************************************************************************
parabolic trajectory pos + vel*t + acc*t^2/2 intersections with all faces.
All roots (negative and positive times) are kept, as for the straight line.
*******************************************************************************/
#pragma acc routine seq
int off_clip_3D_mod_grav(intersection* t, Coords pos, Coords vel, Coords acc,
  Coords* vtxArray, unsigned long vtxSize, unsigned long* faceArray,
  unsigned long faceSize, Coords* normalArray, double* DArray)
{
  int t_size=0;
  double quadratic [3];
  unsigned long i=0,indPoly=0;
  //exploring the polygons :
  while (i<faceSize)
  {
    polygon pol;
    double roots[2];
    int nsol, r;
    pol.npol  = faceArray[i];                //nb vertex of polygon
    pol.vtx   = vtxArray;
    pol.idx   = &faceArray[i+1];
    pol.normal= normalArray[indPoly];
    pol.D     = DArray[indPoly];

    p_to_quadratic(pol.normal, pol.D, acc, pos, vel, quadratic);
    nsol = quadraticSolve(quadratic, &roots[0], &roots[1]);

    for (r=0; r<nsol; r++) {
      double time = roots[r];
      intersection inters;
      double t2 = time * time * 0.5;
      inters.v = coords_set(pos.x + time * vel.x + t2 * acc.x,
                            pos.y + time * vel.y + t2 * acc.y,
                            pos.z + time * vel.z + t2 * acc.z);
      Coords tvel = coords_set(vel.x + time * acc.x,
                               vel.y + time * acc.y,
                               vel.z + time * acc.z);
      inters.time = time;
      inters.normal = pol.normal;
      inters.index = indPoly;
      int res=off_pnpoly(pol,inters.v);
      if (res != 0) {
        inters.edge=(res==-1);
        MCNUM ndir = scalar_prod(pol.normal.x,pol.normal.y,pol.normal.z,tvel.x,tvel.y,tvel.z);
        if (ndir<0) {
          inters.in_out=1;  //the negative dot product means we enter the surface
        } else {
          inters.in_out=-1;
        }
        off_store_intersection(t, &t_size, inters);
      }
    }
    i += pol.npol+1;
    indPoly++;
  } /* while i<faceSize */
  return t_size;
} /* off_clip_3D_mod_grav */

#ifdef OFF_LEGACY
// off_compare *****************************************************************
int off_compare (void const *a, void const *b)
{
   intersection const *pa = a;
   intersection const *pb = b;

   return off_sign(pa->time - pb->time);
} /* off_compare */

// off_cleanDouble *************************************************************
//given a sorted array of intersections throw those which appear several times
#pragma acc routine
int off_cleanDouble(intersection* t, int* t_size)
{
  int i=1;
  while (i<*t_size)
  {
    if (fabs(t[i-1].time-t[i].time)<OFF_EPSILON && t[i-1].in_out==t[i].in_out)
    {
      int k;
      for (k=i+1; k<*t_size; ++k) t[k-1]=t[k];
      *t_size-=1;
    }
    else ++i;
  }
  return 1;
} /* off_cleanDouble */

// off_cleanInOut **************************************************************
//given an array of intesections throw those which enter and exit in the same time
//Meaning the ray passes very close to the volume
//(such intersections must be adjacent in the array : run off_cleanDouble before)
#pragma acc routine
int off_cleanInOut(intersection* t, int* t_size)
{
  int i=1;
  while (i<*t_size)
  {
    if (fabs(t[i-1].time-t[i].time)<OFF_EPSILON && t[i-1].in_out!=t[i].in_out)
    {
      int j;
      for (j=i+1; j<*t_size; ++j) t[j-2]=t[j];
      *t_size-=2;
      if (i>1) i--;
    }
    else ++i;
  }
  return (*t_size);
} /* off_cleanInOut */
#endif

/* PUBLIC functions ******************************************************** */

/*******************************************************************************
* long off_init(  char *offfile, double xwidth, double yheight, double zdepth, off_struct* data)
* ACTION: read an OFF file, optionally center object and rescale, initialize OFF data structure
* INPUT: 'offfile' OFF file to read
*        'xwidth,yheight,zdepth' if given as non-zero, apply bounding box.
*           Specifying only one of these will also use the same ratio on all axes
*        'notcenter' center the object to the (0,0,0) position in local frame when set to zero
* RETURN: number of polyhedra and 'data' OFF structure
*******************************************************************************/
long off_init(  char *offfile, double xwidth, double yheight, double zdepth,
                int notcenter, off_struct* data)
{
  // data to be initialized
  long    vtxSize =0, polySize=0, i=0, ret=0, faceSize=0, faceCap=0;
  Coords* vtxArray        =NULL;
  Coords* normalArray     =NULL;
  double* DArray          =NULL;
  unsigned long* faceArray=NULL;
  double* propTmp         =NULL; /* per-face properties, row-major, growing */
  long    propTmpSize=0, propTmpCap=0;
  long*   propOffset      =NULL; /* start of face i properties in propTmp */
  int*    propCount       =NULL; /* number of properties of face i */
  int     nprops=0, nprops_mixed=0, lineVertex=0;
  long    ndegenerate=0;
  char*   line            =NULL;
  size_t  lineCap=0;
  FILE*   f               =NULL; /* the FILE with vertices and polygons */
  double minx=FLT_MAX,maxx=-FLT_MAX,miny=FLT_MAX,maxy=-FLT_MAX,minz=FLT_MAX,maxz=-FLT_MAX;

  // get the indexes
  if (!data) return(0);
  memset(data, 0, sizeof(off_struct));

  MPI_MASTER(
  printf("Loading geometry file (OFF/PLY): %s\n", offfile);
  );

  f=off_getBlocksIndex(offfile,&vtxSize,&polySize,&lineVertex);
  if (!f) return(0);

  // read vertex table = [x y z | x y z | ...] =================================
  // now we read the vertices as 'vtxSize*3' numbers and store it in vtxArray
  MPI_MASTER(
  printf("  Number of vertices: %ld\n", vtxSize);
  );
  vtxArray   = malloc(vtxSize*sizeof(Coords));
  if (!vtxArray) { fclose(f); return(0); }
  i=0;
  while (i<vtxSize)
  {
    double x,y,z;
    ret=fscanf(f, "%lg%lg%lg", &x,&y,&z);
    if (ret == 0) {
      // invalid line: we skip it (probably a comment)
      off_readline(f, &line, &lineCap);
      continue;
    }
    if (ret != 3) {
      fprintf(stderr, "Error: can not read [xyz] coordinates for vertex %li in file %s (interoff/off_init). Read %li values.\n",
        i, offfile, ret);
      exit(2);
    }
    if (lineVertex) off_readline(f, &line, &lineCap); /* skip normals/colours */
    vtxArray[i].x=x;
    vtxArray[i].y=y;
    vtxArray[i].z=z;

    //bounding box
    if (vtxArray[i].x<minx) minx=vtxArray[i].x;
    if (vtxArray[i].x>maxx) maxx=vtxArray[i].x;
    if (vtxArray[i].y<miny) miny=vtxArray[i].y;
    if (vtxArray[i].y>maxy) maxy=vtxArray[i].y;
    if (vtxArray[i].z<minz) minz=vtxArray[i].z;
    if (vtxArray[i].z>maxz) maxz=vtxArray[i].z;
    i++; // inquire next vertex
  }

  // resizing and repositioning params
  double centerx=0, centery=0, centerz=0;
  if (!notcenter) {
    centerx=(minx+maxx)*0.5;
    centery=(miny+maxy)*0.5;
    centerz=(minz+maxz)*0.5;
  }

  double rangex=-minx+maxx,
         rangey=-miny+maxy,
         rangez=-minz+maxz;

  double ratiox=1,ratioy=1,ratioz=1;

  if (xwidth && rangex)
  {
    ratiox=xwidth/rangex;
    ratioy=ratiox;
    ratioz=ratiox;
  }

  if (yheight && rangey)
  {
    ratioy=yheight/rangey;
    if(!xwidth)  ratiox=ratioy;
    ratioz=ratioy;
  }

  if (zdepth && rangez)
  {
    ratioz=zdepth/rangez;
    if(!xwidth)  ratiox=ratioz;
    if(!yheight) ratioy=ratioz;
  }

  rangex *= ratiox;
  rangey *= ratioy;
  rangez *= ratioz;

  //center and resize the object
  for (i=0; i<vtxSize; ++i)
  {
    vtxArray[i].x=(vtxArray[i].x-centerx)*ratiox+(!notcenter ? 0 : centerx);
    vtxArray[i].y=(vtxArray[i].y-centery)*ratioy+(!notcenter ? 0 : centery);
    vtxArray[i].z=(vtxArray[i].z-centerz)*ratioz+(!notcenter ? 0 : centerz);
  }

  // read face table = [nbvertex v1 v2 vn | nbvertex v1 v2 vn ...] =============
  MPI_MASTER(
  printf("  Number of polygons: %ld\n", polySize);
  );
  normalArray= malloc(polySize*sizeof(Coords));
  DArray     = malloc(polySize*sizeof(double));
  propOffset = malloc(polySize*sizeof(long));
  propCount  = malloc(polySize*sizeof(int));
  faceCap    = polySize*5;  // grows when needed: any number of vertices per polygon
  faceArray  = malloc(faceCap*sizeof(unsigned long));
  if (!normalArray || !faceArray || !DArray || !propOffset || !propCount) {
    fprintf(stderr, "Error: memory allocation for %ld polygons in file %s (interoff/off_init)\n", polySize, offfile);
    exit(-1);
  }

  // fill faces
  faceSize=0;
  i=0;
  while (i<polySize) {
    int  nbVertex=0, j=0;
    // read the length of this polygon
    ret=fscanf(f, "%d", &nbVertex);
    if (ret == 0) {
      // invalid line: we skip it (probably a comment)
      off_readline(f, &line, &lineCap);
      continue;
    }
    if (ret != 1) {
      fprintf(stderr, "Error: can not read polygon %li length in file %s (interoff/off_init)\n",
        i, offfile);
      exit(3);
    }
    if (nbVertex < 1) {
      fprintf(stderr, "Error: polygon %li has %d vertices in file %s (interoff/off_init)\n",
        i, nbVertex, offfile);
      exit(3);
    }
    if (faceSize + nbVertex + 1 > faceCap) {
      unsigned long *tmp;
      while (faceSize + nbVertex + 1 > faceCap) faceCap *= 2;
      tmp = realloc(faceArray, faceCap*sizeof(unsigned long));
      if (!tmp) {
        fprintf(stderr, "Error: memory allocation for polygon %li in file %s (interoff/off_init)\n", i, offfile);
        exit(-1);
      }
      faceArray = tmp;
    }
    faceArray[faceSize++] = nbVertex; // length of the polygon/face
    // then read the vertex ID's
    for (j=0; j<nbVertex; j++) {
      double vtx=-1;
      ret=fscanf(f, "%lg", &vtx);
      if (ret != 1 || vtx < 0 || vtx >= vtxSize || vtx != floor(vtx)) {
        fprintf(stderr, "Error: invalid vertex index #%i for polygon %li in file %s (interoff/off_init). Must be an integer in [0:%li]\n",
          j, i, offfile, vtxSize-1);
        exit(3);
      }
      faceArray[faceSize++] = (unsigned long)vtx;   // add vertices index after length of polygon
    }
    if (nbVertex < 3) ndegenerate++;
    // then the optional per-face properties, until the end of the line (or a '#')
    propOffset[i] = propTmpSize;
    propCount[i]  = 0;
    if (off_readline(f, &line, &lineCap) > 0) {
      char *s = line, *end;
      char *hash = strchr(line, '#');
      if (hash) *hash = '\0';
      for (;;) {
        double val = strtod(s, &end);
        if (end == s) break;
        if (propTmpSize >= propTmpCap) {
          double *tmp;
          propTmpCap = (propTmpCap ? 2*propTmpCap : 3*polySize+16);
          tmp = realloc(propTmp, propTmpCap*sizeof(double));
          if (!tmp) {
            fprintf(stderr, "Error: memory allocation for face properties in file %s (interoff/off_init)\n", offfile);
            exit(-1);
          }
          propTmp = tmp;
        }
        propTmp[propTmpSize++] = val;
        propCount[i]++;
        s = end;
      }
      s += strspn(s, " \t\r");
      if (*s) {
        fprintf(stderr, "Warning: unexpected text '%s' after polygon %li in file %s (interoff/off_init). Ignored.\n",
          s, i, offfile);
      }
    }
    if (i && propCount[i] != propCount[i-1]) nprops_mixed=1;
    if (propCount[i] > nprops) nprops = propCount[i];
    i++;
  }
  fclose(f); f=NULL;
  free(line); line=NULL;

  // store per-face properties column-wise: [k*polySize + face]
  data->nfaceprops    = nprops;
  data->facePropSize  = (long)nprops*polySize;
  data->facePropArray = NULL;
  if (nprops) {
    double *props = calloc((size_t)nprops*polySize, sizeof(double));
    int k;
    if (!props) {
      fprintf(stderr, "Error: memory allocation for face properties in file %s (interoff/off_init)\n", offfile);
      exit(-1);
    }
    for (i=0; i<polySize; i++)
      for (k=0; k<propCount[i]; k++)
        props[k*polySize+i] = propTmp[propOffset[i]+k];
    data->facePropArray = props;
    MPI_MASTER(
    printf("  Number of per-face properties: %d\n", nprops);
    if (nprops_mixed)
      printf("Warning: faces in %s do not all define the same number of properties.\n"
             "         Missing values are set to 0.\n", offfile);
    );
  }
  free(propTmp); free(propOffset); free(propCount);

  // precomputes normals
  long indNormal=0;//index in polyArray
  i=0;    //index in faceArray
  while (i<faceSize)
  {
    polygon p;
    p.npol=faceArray[i];
    p.vtx =vtxArray;
    p.idx =&faceArray[i+1];
    if (off_normal(&(p.normal),p) == 0) ndegenerate++;
    normalArray[indNormal]=p.normal;
    Coords v0 = vtxArray[p.idx[0]];
    DArray[indNormal] = scalar_prod(p.normal.x,p.normal.y,p.normal.z, v0.x,v0.y,v0.z);
    i += p.npol+1;
    indNormal++;
  }

  MPI_MASTER(
  if (ndegenerate)
    printf("Warning: %ld degenerate polygon(s) (less than 3 vertices or zero area) in %s.\n"
           "         They will never be intersected.\n", ndegenerate, offfile);
  if (ratiox!=ratioy || ratiox!=ratioz || ratioy!=ratioz)
    printf("Warning: Aspect ratio of the geometry %s was modified.\n"
           "         If you want to keep the original proportions, specifiy only one of the dimensions.\n",
           offfile);
  if ( xwidth==0 && yheight==0 && zdepth==0 ) {
    printf("Warning: Neither xwidth, yheight or zdepth are defined.\n"
	   "           The file-defined (non-scaled) geometry the OFF geometry %s will be applied!\n",
           offfile);
  }
  printf("  Bounding box dimensions for geometry %s:\n", offfile);
  printf("    Length=%f (%.3f%%)\n", rangex, ratiox*100);
  printf("    Width= %f (%.3f%%)\n", rangey, ratioy*100);
  printf("    Depth= %f (%.3f%%)\n", rangez, ratioz*100);
  );

  data->bbmin = coords_set( FLT_MAX, FLT_MAX, FLT_MAX);
  data->bbmax = coords_set(-FLT_MAX,-FLT_MAX,-FLT_MAX);
  for (i=0; i<vtxSize; i++) {
    if (vtxArray[i].x < data->bbmin.x) data->bbmin.x = vtxArray[i].x;
    if (vtxArray[i].y < data->bbmin.y) data->bbmin.y = vtxArray[i].y;
    if (vtxArray[i].z < data->bbmin.z) data->bbmin.z = vtxArray[i].z;
    if (vtxArray[i].x > data->bbmax.x) data->bbmax.x = vtxArray[i].x;
    if (vtxArray[i].y > data->bbmax.y) data->bbmax.y = vtxArray[i].y;
    if (vtxArray[i].z > data->bbmax.z) data->bbmax.z = vtxArray[i].z;
  }
  data->vtxArray   = vtxArray;
  data->normalArray= normalArray;
  data->DArray     = DArray;
  data->faceArray  = faceArray;
  data->vtxSize    = vtxSize;
  data->polySize   = polySize;
  data->faceSize   = faceSize;
  data->filename   = offfile;
  if (nprops >= 3) {
    data->face_m_Array     = data->facePropArray + OFF_FACEPROP_M    *polySize;
    data->face_alpha_Array = data->facePropArray + OFF_FACEPROP_ALPHA*polySize;
    data->face_W_Array     = data->facePropArray + OFF_FACEPROP_W    *polySize;
  }
#ifdef OFF_LEGACY
  data->intersects = malloc(OFF_INTERSECT_MAX*sizeof(intersection));
  if (!data->intersects) {
    fprintf(stderr, "Error: memory allocation for intersections (interoff/off_init)\n");
    exit(-1);
  }
#endif
  #ifdef OPENACC
  acc_attach((void *)&data->vtxArray);
  acc_attach((void *)&data->normalArray);
  acc_attach((void *)&data->faceArray);
  acc_attach((void *)&data->DArray);
  #endif

  return(polySize);
} /* off_init */

#ifdef OFF_LEGACY
#pragma acc routine
int Min_int(int x, int y) {
  return (x<y)? x :y;
}

#pragma acc routine
void merge(intersection *arr, int l, int m, int r)
{
int i, j, k;
int n1 = m - l + 1;
int n2 =  r - m;

/* create temp arrays */
intersection *L, *R;
 L = (intersection *)malloc(sizeof(intersection) * n1);
 R = (intersection *)malloc(sizeof(intersection) * n2);
 if (!L||!R) {
   fprintf(stderr,"Error allocating intersection arrays\n");
   exit(-1);
 }
/* Copy data to temp arrays L[] and R[] */
for (i = 0; i < n1; i++)
    L[i] = arr[l + i];
for (j = 0; j < n2; j++)
    R[j] = arr[m + 1+ j];

/* Merge the temp arrays back into arr[l..r]*/
i = 0;
j = 0;
k = l;
while (i < n1 && j < n2)
{
    if (L[i].time <= R[j].time) arr[k++] = L[i++];
    else                        arr[k++] = R[j++];
}
/* Copy the remaining elements of L[] and R[], if there are any */
while (i < n1) arr[k++] = L[i++];
while (j < n2) arr[k++] = R[j++];
free(L);
free(R);
}

#pragma acc routine
void gpusort(intersection *arr, int size)
{
  int curr_size;  // For current size of subarrays to be merged
  int left_start; // For picking starting index of left subarray
  for (curr_size=1; curr_size<=size-1; curr_size = 2*curr_size)
  {
    for (left_start=0; left_start<size-1; left_start += 2*curr_size)
    {
      int mid = left_start + curr_size - 1;
      int right_end = Min_int(left_start + 2*curr_size - 1, size-1);
      if (mid < right_end) merge(arr, left_start, mid, right_end);
    }
  }
}
#endif

/*******************************************************************************
void p_to_quadratic(Coords norm, MCNUM d, Coords acc, Coords pos, Coords vel,
                    double* teq)
* ACTION: define the quadratic for the intersection of a parabola with a plane
* INPUT: plane equation norm . r = d
*        'acc' acceleration vector
*        'vel' velocity of the particle
*        'pos' position of the particle
* RETURN: equation of parabola: teq(0) * t^2 + teq(1) * t + teq(2)
*******************************************************************************/
#pragma acc routine
void p_to_quadratic(Coords norm, MCNUM d, Coords acc, Coords pos, Coords vel,
		    double* teq)
{
  teq[0] = scalar_prod(norm.x, norm.y, norm.z, acc.x, acc.y, acc.z) * 0.5;
  teq[1] = scalar_prod(norm.x, norm.y, norm.z, vel.x, vel.y, vel.z);
  teq[2] = scalar_prod(norm.x, norm.y, norm.z, pos.x, pos.y, pos.z) - d;
  return;
}

/*******************************************************************************
int quadraticSolve(double eq[], double* x1, double* x2);
* ACTION: solves the quadratic for the roots x1 and x2
*         eq[0] * t^2 + eq[1] * t + eq[2] = 0
* INPUT: 'eq' the coefficients of the parabola
* RETURN: roots x1 and x2 and the number of solutions
*******************************************************************************/
#pragma acc routine
int quadraticSolve(double* eq, double* x1, double* x2)
{
  *x1 = *x2 = 1.0e36;
  if (eq[0] == 0.0) { // This is a linear equation
    if (eq[1] != 0.0) { // one solution
      *x1 = -eq[2]/eq[1];
      return 1;
    }
    return 0; // no solutions, 1.0e36 will be ignored.
  }
  double delta = eq[1]*eq[1]-4.0*eq[0]*eq[2];
  if (delta < 0.0) { // no solutions, both are imaginary
    return 0;
  }
  // numerically stable form
  double s = (eq[1] < 0 ? -1.0 : 1.0);
  double q = -0.5*(eq[1] + s * sqrt(delta));
  *x1 = q/eq[0];
  *x2 = (q != 0.0 ? eq[2]/q : 0.0); // q==0 only when eq[1]==eq[2]==0: double root at 0
  return 2;
}

// off_intersect_all_idx *******************************************************
#pragma acc routine
int off_intersect_all_idx(double* t0, double* t3,
     Coords *n0, Coords *n3,
     unsigned long *faceindex0, unsigned long *faceindex3,
     double x,  double y,  double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct *data )
{
    int t_size = 0;
    /* a curved trajectory is only needed with a non-zero acceleration */
    int use_grav = (mcgravitation && (ax != 0 || ay != 0 || az != 0));
#ifdef OFF_LEGACY
    intersection *t = data->intersects;
#else
    intersection t[4];
    int k;
    for (k=0; k<4; k++) {
      t[k].time   = (k==0 ? -FLT_MAX : FLT_MAX);
      t[k].v      = t[k].normal = coords_set(0,0,0);
      t[k].in_out = t[k].edge = 0;
      t[k].index  = 0;
    }
#endif

    if (use_grav) {
      Coords pos={ x,  y,  z};
      Coords vel={vx, vy, vz};
      Coords acc={ax, ay, az};
      t_size=off_clip_3D_mod_grav(t, pos, vel, acc,
				  data->vtxArray, data->vtxSize, data->faceArray,
				  data->faceSize, data->normalArray, data->DArray);
    } else {
      Coords A={x, y, z};
      Coords B={x+vx, y+vy, z+vz};
      Coords V={vx, vy, vz};
      /* quick rejection: the straight line misses the object bounding box */
      if (!off_line_hits_box(A, V, data->bbmin, data->bbmax)) return 0;
      t_size=off_clip_3D_mod(t, A, B,
			     data->vtxArray, data->vtxSize, data->faceArray,
			     data->faceSize, data->normalArray, data->DArray);
    }

#ifdef OFF_LEGACY
    #ifndef OPENACC
    qsort(t, t_size, sizeof(intersection),  off_compare);
    #else
    gpusort(t, t_size);
    #endif
    off_cleanDouble(t, &t_size);
    off_cleanInOut(t,  &t_size);
    data->numintersect=t_size;

    /*find intersections "closest" to 0 (favouring positive ones)*/
    if(t_size>0){
      int i=0;
      if(t_size>1) {
        for (i=1; i < t_size-1; i++){
          if (t[i-1].time > 0 && t[i].time > 0)
            break;
        }
        if (t0) *t0 = t[i-1].time;
        if (n0) *n0 = t[i-1].normal;
        if (faceindex0) *faceindex0 = t[i-1].index;
        if (t3) *t3 = t[i].time;
        if (n3) *n3 = t[i].normal;
        if (faceindex3) *faceindex3 = t[i].index;
        data->nextintersect=(int)t[i-1].index;
      } else {
        if (t0) *t0 = t[0].time;
        if (n0) *n0 = t[0].normal;
        if (faceindex0) *faceindex0 = t[0].index;
        data->nextintersect=(int)t[0].index;
      }
      return t_size;
    }
#else
    if(t_size>0){
      int i=0;
      if (t[0].time == -FLT_MAX) i=1;
      data->numintersect=t_size;
      if (t0) *t0 = t[i].time;
      if (n0) *n0 = t[i].normal;
      if (t3) *t3 = t[i+1].time;
      if (n3) *n3 = t[i+1].normal;
      if (faceindex0) *faceindex0 = t[i].index;
      if (faceindex3) *faceindex3 = t[i+1].index;

      if (t[1].time == FLT_MAX)
      {
        if (t3) *t3 = 0.0;
      }

      data->nextintersect=(int)t[i].index;
      return t_size;
    }
#endif
    return 0;
} /* off_intersect_all_idx */

/*******************************************************************************
* int off_intersect_all(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct *data )
* ACTION: computes intersection of neutron trajectory with an object.
* INPUT:  x,y,z and vx,vy,vz are the position and velocity of the neutron
*         ax, ay, az are the local acceleration vector
*         data points to the OFF data structure
* RETURN: the number of polyhedral which trajectory intersects
*         t0 and t3 are the smallest incoming and outgoing intersection times
*         n0 and n3 are the corresponding normal vectors to the surface
*         data is the full OFF structure, including a list intersection type
*******************************************************************************/
#pragma acc routine
int off_intersect_all(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x,  double y,  double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct *data )
{
  return off_intersect_all_idx(t0, t3, n0, n3, NULL, NULL,
    x, y, z, vx, vy, vz, ax, ay, az, data);
} /* off_intersect_all */

/*******************************************************************************
* int off_intersect(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double vx, double vy, double vz,
     off_struct data )
* ACTION: computes intersection of neutron trajectory with an object.
* INPUT:  x,y,z and vx,vy,vz are the position and velocity of the neutron
*         data points to the OFF data structure
* RETURN: the number of polyhedral which trajectory intersects
*         t0 and t3 are the smallest incoming and outgoing intersection times
*         n0 and n3 are the corresponding normal vectors to the surface
*******************************************************************************/
#pragma acc routine
int off_intersect(double* t0, double* t3,
     Coords *n0, Coords *n3,
     double x,  double y,  double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct data )
{
  return off_intersect_all_idx(t0, t3, n0, n3, NULL, NULL,
    x, y, z, vx, vy, vz, ax, ay, az, &data );
} /* off_intersect */

#pragma acc routine
int off_intersect_idx(double* t0, double* t3,
     Coords *n0, Coords *n3,
     unsigned long *faceindex0, unsigned long *faceindex3,
     double x,  double y,  double z,
     double vx, double vy, double vz,
     double ax, double ay, double az,
     off_struct data )
{
  return off_intersect_all_idx(t0, t3, n0, n3, faceindex0, faceindex3,
    x, y, z, vx, vy, vz, ax, ay, az, &data );
} /* off_intersect_idx */

/*****************************************************************************
* int off_x_intersect(double* l0, double* l3,
     Coords *n0, Coords *n3,
     double x, double y, double z,
     double kx, double ky, double kz,
     off_struct data )
* ACTION: computes intersection of an xray trajectory with an object.
* INPUT:  x,y,z and kx,ky,kz, are spatial coordinates and wavevector of the x-ray
*         respectively. data points to the OFF data structure.
* RETURN: the number of polyhedral the trajectory intersects
*         l0 and l3 are the smallest incoming and outgoing intersection lengths
*         n0 and n3 are the corresponding normal vectors to the surface
*******************************************************************************/
#pragma acc routine
int off_x_intersect_idx(double *l0,double *l3,
     Coords *n0, Coords *n3,
     unsigned long *faceindex0, unsigned long *faceindex3,
     double x,  double y,  double z,
     double kx, double ky, double kz,
     off_struct data )
{
  /*This function simply reformats and calls off_intersect (as for neutrons)
   *by normalizing the wavevector - this will yield the intersection lengths
   *in m*/
  double jx,jy,jz,invk;
  invk=1/sqrt(scalar_prod(kx,ky,kz,kx,ky,kz));
  jx=kx*invk;jy=ky*invk;jz=kz*invk;
  return off_intersect_all_idx(l0,l3,n0,n3,faceindex0,faceindex3,
    x,y,z,jx,jy,jz,0.0,0.0,0.0,&data);
} /* off_x_intersect_idx */

#pragma acc routine
int off_x_intersect(double *l0,double *l3,
     Coords *n0, Coords *n3,
     double x,  double y,  double z,
     double kx, double ky, double kz,
     off_struct data )
{
  return off_x_intersect_idx(l0,l3,n0,n3,NULL,NULL,x,y,z,kx,ky,kz,data);
} /* off_x_intersect */

/*******************************************************************************
* double off_face_prop(off_struct *data, unsigned long face, int k, double def)
* ACTION: return per-face property 'k' of face 'face', or 'def' when not defined
*******************************************************************************/
#pragma acc routine
double off_face_prop(off_struct *data, unsigned long face, int k, double def)
{
  if (!data || !data->facePropArray || k < 0 || k >= data->nfaceprops
   || face >= (unsigned long)data->polySize)
    return def;
  return data->facePropArray[(long)k*data->polySize + face];
} /* off_face_prop */

/* growing string buffer used by off_display */
typedef struct off_strbuf { char *s; size_t len, cap; } off_strbuf;

void off_strbuf_printf(off_strbuf *b, const char *fmt, ...)
{
  va_list ap;
  int n;
  if (!b->s) {
    b->cap = 4096; b->len = 0;
    b->s = malloc(b->cap);
    if (!b->s) { fprintf(stderr, "Error: memory allocation (interoff/off_display)\n"); exit(-1); }
    b->s[0] = '\0';
  }
  va_start(ap, fmt);
  n = vsnprintf(b->s + b->len, b->cap - b->len, fmt, ap);
  va_end(ap);
  if (n < 0) return;
  if (b->len + n + 1 > b->cap) {
    char *tmp;
    while (b->len + n + 1 > b->cap) b->cap *= 2;
    tmp = realloc(b->s, b->cap);
    if (!tmp) { fprintf(stderr, "Error: memory allocation (interoff/off_display)\n"); exit(-1); }
    b->s = tmp;
    va_start(ap, fmt);
    vsnprintf(b->s + b->len, b->cap - b->len, fmt, ap);
    va_end(ap);
  }
  b->len += n;
}

/*******************************************************************************
* void off_display(off_struct data)
* ACTION: display up to N_VERTEX_DISPLAYED polygons from the object
*******************************************************************************/
void off_display(off_struct data)
{
  long i;
  if (!data.faceSize || !data.vtxArray || !data.faceArray) return;
  if(mcdotrace==2){
    off_strbuf json = {NULL, 0, 0};
    off_strbuf_printf(&json, "{ \"vertices\": [");
    for (i = 0; i < data.vtxSize; i++) {
      off_strbuf_printf(&json, "[%g, %g, %g]", data.vtxArray[i].x, data.vtxArray[i].y, data.vtxArray[i].z);
      if (i < data.vtxSize - 1) off_strbuf_printf(&json, ", ");
    }
    off_strbuf_printf(&json, "], \"faces\": [");
    for (i = 0; i < data.faceSize;) {
      long j, num = data.faceArray[i];
      off_strbuf_printf(&json, "{ \"face\": [");
      for (j = 1; j <= num; j++) {
        off_strbuf_printf(&json, "%lu", data.faceArray[i + j]);
        if (j < num) off_strbuf_printf(&json, ", ");
      }
      off_strbuf_printf(&json, "]}");
      i += num + 1;
      if (i < data.faceSize) off_strbuf_printf(&json, ", ");
    }
    off_strbuf_printf(&json, "]}");
    mcdis_polyhedron(json.s);
    free(json.s);
  }
  else {
    double ratio=(double)(N_VERTEX_DISPLAYED)/(double)data.faceSize;
    long pixel=0;
    off_strbuf pixelinfo = {NULL, 0, 0};
    for (i=0; i<data.faceSize; ) {
      int j;
      int nbVertex = data.faceArray[i];
      double x0,y0,z0;
      x0 = data.vtxArray[data.faceArray[i+1]].x;
      y0 = data.vtxArray[data.faceArray[i+1]].y;
      z0 = data.vtxArray[data.faceArray[i+1]].z;
      double x1=x0,y1=y0,z1=z0;
      double cmx=0,cmy=0,cmz=0;

      int drawthis = rand01() < ratio;
      // First pass, calculate center of mass location...
      for (j=1; j<=nbVertex; j++) {
        cmx = cmx+data.vtxArray[data.faceArray[i+j]].x;
        cmy = cmy+data.vtxArray[data.faceArray[i+j]].y;
        cmz = cmz+data.vtxArray[data.faceArray[i+j]].z;
      }
      cmx /= nbVertex;
      cmy /= nbVertex;
      cmz /= nbVertex;

      if (data.mantidflag) {
        pixelinfo.len = 0;
        off_strbuf_printf(&pixelinfo, "%li,%li,%li,%i,%g,%g,%g,%g,%g,%g", data.mantidoffset+pixel, data.mantidoffset, data.mantidoffset+data.polySize-1, nbVertex, cmx, cmy, cmz, x1-cmx, y1-cmy, z1-cmz);
      }
      for (j=2; j<=nbVertex; j++) {
        double x2,y2,z2;
        x2 = data.vtxArray[data.faceArray[i+j]].x;
        y2 = data.vtxArray[data.faceArray[i+j]].y;
        z2 = data.vtxArray[data.faceArray[i+j]].z;
        if (data.mantidflag)
          off_strbuf_printf(&pixelinfo, ",%g,%g,%g", x2-cmx, y2-cmy, z2-cmz);
        if (ratio > 1 || drawthis) {
          mcdis_line(x1,y1,z1,x2,y2,z2);
        }
        x1 = x2; y1 = y2; z1 = z2;
      }
      if (ratio > 1 || drawthis) {
        mcdis_line(x1,y1,z1,x0,y0,z0);
      }
      if (data.mantidflag) {
        printf("MANTID_PIXEL: %s\n", pixelinfo.s);
        pixel++;
      }
      i += nbVertex+1;
    }
    free(pixelinfo.s);
  }
} /* off_display */

/* end of interoff-lib.c */
#endif // INTEROFF_LIB_C
