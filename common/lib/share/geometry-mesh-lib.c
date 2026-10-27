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
  #include <limits.h>
  #include <stdarg.h>
  #include <stdint.h>

  #ifdef OPENACC
    // fprintf and stderr are not available in GPU device code, so error
    // messages are sent through printf instead (as in interoff-lib)
    #define fprintf(stderr, ...) printf (__VA_ARGS__)
  #endif

// ==========================================================================
// Small helpers
// ==========================================================================

// realloc that exits on failure. A request of 0 bytes is made as 1 byte, since
// realloc(pointer, 0) may free pointer and return NULL, which would look like
// a failure.
static void*
gmesh_xrealloc (void* pointer, size_t size) {
  void* result;
  if (size == 0) {
    size = 1;
  }
  result = realloc (pointer, size);
  if (!result) {
    fprintf (stderr, "Error: memory allocation of %lu bytes failed (geometry-mesh-lib)\n", (unsigned long)size);
    exit (-1);
  }
  return result;
}

// calloc of count elements of size bytes (all zero) that exits on failure
static void*
gmesh_xcalloc (size_t count, size_t size) {
  void* result;
  if (count == 0) {
    count = 1;
  }
  result = calloc (count, size);
  if (!result) {
    fprintf (stderr, "Error: memory allocation of %lu elements failed (geometry-mesh-lib)\n", (unsigned long)count);
    exit (-1);
  }
  return result;
}

// 1 when extension (may be NULL) equals lower or upper, e.g. ".stl" or ".STL"
static int
gmesh_is_extension (const char* extension, const char* lower, const char* upper) {
  if (!extension)
    return 0;
  if (!strcmp (extension, lower))
    return 1;
  return !strcmp (extension, upper);
}

// the unsigned 32 bit integer stored little endian in bytes[0..3]
static uint32_t
gmesh_uint32_le (const unsigned char* bytes) {
  uint32_t value = (uint32_t)bytes[0];
  value |= (uint32_t)bytes[1] << 8;
  value |= (uint32_t)bytes[2] << 16;
  value |= (uint32_t)bytes[3] << 24;
  return value;
}

// One polygon of a gmesh_raw: nverts pointers into gmesh_raw.verts, and the
// nprops optional numbers given for it in the file (OFF: numbers after the
// vertex indices, PLY: scalar face properties), e.g. m, alpha, W of a guide
// wall as in interoff-lib.
typedef struct gmesh_polygon {
  int nverts;
  Coords** verts;
  int nprops;
  double* props;
} gmesh_polygon;

// Raw geometry as read from a file, before welding and triangulation.
// A file is read twice: the counting pass (counting = 1) only counts the
// vertices, polygons, polygon corners and properties. The arrays are then
// allocated with exactly these sizes and the second pass fills them.
typedef struct gmesh_raw {
  int counting;
  int keep_props; // 0: per-face properties in the file are skipped
  Coords* verts;
  int nverts;
  gmesh_polygon* faces;
  int nfaces;
  Coords** corners; // storage for the vertex pointers of all polygons
  int ncorners;
  double* props; // storage for the properties of all polygons
  int nprops;
  char* line; // line buffer, holds the longest line of the file
  int line_cap;
} gmesh_raw;

// allocate raw->line to hold the longest line of file, from the current
// position to the end of the file. Lines of binary data longer than
// INT_MAX-1 are allowed, but only their first INT_MAX-1 characters are kept.
static void
gmesh_alloc_line (FILE* file, gmesh_raw* raw) {
  long start = ftell (file);
  long length = 0;
  long longest = 0;
  int character = fgetc (file);
  while (character != EOF) {
    if (character == '\n') {
      if (length > longest)
        longest = length;
      length = 0;
    } else {
      length++;
    }
    character = fgetc (file);
  }
  if (length > longest)
    longest = length;
  if (longest > INT_MAX - 1)
    longest = INT_MAX - 1;
  fseek (file, start, SEEK_SET);
  raw->line_cap = (int)longest + 1;
  raw->line = gmesh_xrealloc (NULL, raw->line_cap);
}

// read the rest of the current line of file into raw->line (without '\n').
// Returns the line length, -1 at EOF with nothing read.
static int
gmesh_readline (FILE* file, gmesh_raw* raw) {
  int length = 0;
  int character;
  if (!raw->line) {
    gmesh_alloc_line (file, raw);
  }
  character = fgetc (file);
  while (character != EOF && character != '\n') {
    if (length < raw->line_cap - 1) {
      raw->line[length] = (char)character;
      length++;
    }
    character = fgetc (file);
  }
  raw->line[length] = '\0';
  if (character == EOF && length == 0)
    return -1;
  return length;
}

// append the vertex (x, y, z) to raw (only counted in the counting pass)
static void
gmesh_raw_vertex (gmesh_raw* raw, double x, double y, double z) {
  if (!raw->counting) {
    raw->verts[raw->nverts] = coords_set (x, y, z);
  }
  raw->nverts++;
}

// start a new polygon with vertex_count corners. Returns the array where the
// reader stores its vertex_count vertex pointers, NULL in the counting pass.
static Coords**
gmesh_raw_face (gmesh_raw* raw, int vertex_count) {
  Coords** corners = NULL;
  if (!raw->counting) {
    gmesh_polygon* face = &raw->faces[raw->nfaces];
    face->nverts = vertex_count;
    face->verts = &raw->corners[raw->ncorners];
    face->nprops = 0;
    face->props = &raw->props[raw->nprops];
    corners = face->verts;
  }
  raw->nfaces++;
  raw->ncorners += vertex_count;
  return corners;
}

// append one property value to the most recently started polygon
// (ignored when raw->keep_props is 0)
static void
gmesh_raw_prop (gmesh_raw* raw, double value) {
  if (!raw->keep_props)
    return;
  if (!raw->counting) {
    raw->props[raw->nprops] = value;
    raw->faces[raw->nfaces - 1].nprops++;
  }
  raw->nprops++;
}

// after the counting pass: allocate the arrays with the counted sizes and
// reset the counts for the filling pass
static void
gmesh_raw_alloc (gmesh_raw* raw) {
  raw->verts = gmesh_xrealloc (NULL, raw->nverts * sizeof (Coords));
  raw->faces = gmesh_xrealloc (NULL, raw->nfaces * sizeof (gmesh_polygon));
  raw->corners = gmesh_xrealloc (NULL, raw->ncorners * sizeof (Coords*));
  raw->props = gmesh_xrealloc (NULL, raw->nprops * sizeof (double));
  raw->nverts = 0;
  raw->nfaces = 0;
  raw->ncorners = 0;
  raw->nprops = 0;
  raw->counting = 0;
}

// free the arrays held by raw and reset it to an empty gmesh_raw
// (raw itself is not freed, it lives on the caller's stack)
static void
gmesh_raw_free (gmesh_raw* raw) {
  free (raw->verts);
  free (raw->faces);
  free (raw->corners);
  free (raw->props);
  free (raw->line);
  memset (raw, 0, sizeof (gmesh_raw));
}

// ==========================================================================
// OFF reader
// ==========================================================================

// 1 when index is a whole number in [0, count), else 0
static int
gmesh_valid_index (double index, long count) {
  if (index < 0)
    return 0;
  if (index >= count)
    return 0;
  return index == floor (index);
}

// read an OFF file (also COFF, NOFF, STOFF, ...) into raw.
// Returns 1 on success, 0 on error (with a message).
static int
gmesh_read_off (FILE* file, char* filename, gmesh_raw* raw) {
  int length;
  long nvertices = 0;
  long nfaces = 0;
  long i;
  int extra = 0;
  int is_off;
  char* header; // the header line without leading blanks, e.g. "COFF 8 6 0"
  char* keyword; // the "OFF" in header, after any S/T/C/N prefix letters
  char* text = NULL; // a line without leading blanks

  // skip blank lines before the header
  length = gmesh_readline (file, raw);
  while (length >= 0 && strspn (raw->line, " \t\r") == (size_t)length) {
    length = gmesh_readline (file, raw);
  }
  if (length < 0) {
    fprintf (stderr, "Error: empty file %s (geometry-mesh-lib)\n", filename);
    return 0;
  }
  header = raw->line + strspn (raw->line, " \t");
  keyword = header;
  while (*keyword && strchr ("STCN", *keyword)) {
    keyword++;
  }
  // "OFF" must be followed by the end of the word
  is_off = !strncmp (keyword, "OFF", 3);
  if (is_off && keyword[3]) {
    is_off = isspace ((unsigned char)keyword[3]);
  }
  if (!is_off) {
    fprintf (stderr, "Error: %s: header '%s' is not a supported OFF header (geometry-mesh-lib)\n", filename, header);
    return 0;
  }
  // prefixed headers (COFF, NOFF, ...) have extra numbers after each vertex
  extra = (keyword != header);
  // the counts are either on the header line or on the next non-comment line
  if (sscanf (keyword + 3, "%ld %ld", &nvertices, &nfaces) != 2) {
    for (length = gmesh_readline (file, raw); length >= 0; length = gmesh_readline (file, raw)) {
      text = raw->line + strspn (raw->line, " \t\r");
      if (text[0] != '#' && text[0] != '\0')
        break;
    }
    if (length < 0 || sscanf (text, "%ld %ld", &nvertices, &nfaces) != 2) {
      fprintf (stderr, "Error: %s: can not read the numbers of vertices and faces (geometry-mesh-lib)\n", filename);
      return 0;
    }
  }
  if (nvertices <= 0 || nfaces <= 0) {
    fprintf (stderr, "Error: %s defines %ld vertices and %ld faces (geometry-mesh-lib)\n", filename, nvertices, nfaces);
    return 0;
  }
  for (i = 0; i < nvertices; i++) {
    double x, y, z;
    int result = fscanf (file, "%lg %lg %lg", &x, &y, &z);
    // fscanf returns 0 on a non-number, i.e. a comment line: skip it
    while (result == 0) {
      gmesh_readline (file, raw);
      result = fscanf (file, "%lg %lg %lg", &x, &y, &z);
    }
    if (result != 3) {
      fprintf (stderr, "Error: %s: can not read vertex %ld (geometry-mesh-lib)\n", filename, i);
      return 0;
    }
    if (extra) {
      gmesh_readline (file, raw);
    }
    gmesh_raw_vertex (raw, x, y, z);
  }
  for (i = 0; i < nfaces; i++) {
    int count, j;
    double index;
    Coords** corners;
    int result = fscanf (file, "%d", &count);
    while (result == 0) {
      gmesh_readline (file, raw);
      result = fscanf (file, "%d", &count);
    }
    if (result != 1 || count < 1) {
      fprintf (stderr, "Error: %s: can not read face %ld (geometry-mesh-lib)\n", filename, i);
      return 0;
    }
    corners = gmesh_raw_face (raw, count);
    for (j = 0; j < count; j++) {
      int valid = (fscanf (file, "%lg", &index) == 1);
      if (valid) {
        valid = gmesh_valid_index (index, nvertices);
      }
      if (!valid) {
        fprintf (stderr, "Error: %s: invalid vertex index #%d of face %ld (geometry-mesh-lib)\n", filename, j, i);
        return 0;
      }
      if (corners) {
        corners[j] = &raw->verts[(int)index];
      }
    }
    // optional per-face numbers until the end of the line or a '#' comment
    if (gmesh_readline (file, raw) > 0) {
      char* position = raw->line;
      char* end;
      char* hash = strchr (raw->line, '#');
      double value;
      if (hash) {
        *hash = '\0';
      }
      value = strtod (position, &end);
      while (end != position) {
        gmesh_raw_prop (raw, value);
        position = end;
        value = strtod (position, &end);
      }
    }
  }
  return 1;
}

// ==========================================================================
// STL reader
// ==========================================================================

// read an ASCII or binary STL file into raw.
// Returns 1 on success, 0 on error (with a message).
static int
gmesh_read_stl (char* filename, gmesh_raw* raw) {
  FILE* file = Open_File (filename, "rb", NULL);
  unsigned char head[84];
  long size;
  long ntriangles = 0;
  long i;
  int binary = 0;
  if (!file)
    return 0;
  fseek (file, 0, SEEK_END);
  size = ftell (file);
  fseek (file, 0, SEEK_SET);
  if (size >= 84 && fread (head, 1, 84, file) == 84) {
    ntriangles = (long)gmesh_uint32_le (head + 80);
    // binary when the size matches exactly, whatever the header says
    if (size == 84 + 50 * ntriangles) {
      binary = 1;
    } else {
      int k = 0;
      while (k < 80 && isspace (head[k])) {
        k++;
      }
      if (strncmp ((char*)head + k, "solid", 5)) {
        fprintf (stderr,
                 "Error: %s is neither an ASCII STL nor a binary STL of consistent size "
                 "(%ld bytes for %ld triangles) (geometry-mesh-lib)\n",
                 filename, size, ntriangles);
        fclose (file);
        return 0;
      }
    }
  }
  if (binary) {
    for (i = 0; i < ntriangles; i++) {
      unsigned char record[50];
      int k;
      float xyz[9];
      Coords** corners;
      if (fread (record, 1, 50, file) != 50) {
        fprintf (stderr, "Error: %s: truncated binary STL at triangle %ld (geometry-mesh-lib)\n", filename, i);
        fclose (file);
        return 0;
      }
      // little endian float32 coordinates, after the normal
      for (k = 0; k < 9; k++) {
        uint32_t bits = gmesh_uint32_le (record + 12 + 4 * k);
        memcpy (&xyz[k], &bits, 4);
      }
      corners = gmesh_raw_face (raw, 3);
      for (k = 0; k < 3; k++) {
        gmesh_raw_vertex (raw, xyz[3 * k], xyz[3 * k + 1], xyz[3 * k + 2]);
        if (corners) {
          corners[k] = &raw->verts[raw->nverts - 1];
        }
      }
    }
  } else {
    int nloop = 0;
    int first = 0;
    fseek (file, 0, SEEK_SET);
    while (gmesh_readline (file, raw) >= 0) {
      char* text = raw->line + strspn (raw->line, " \t");
      if (!strncmp (text, "outer", 5)) {
        first = raw->nverts;
        nloop = 0;
      } else if (!strncmp (text, "vertex", 6)) {
        double x, y, z;
        if (sscanf (text + 6, "%lg %lg %lg", &x, &y, &z) != 3) {
          fprintf (stderr, "Error: %s: bad vertex line '%s' (geometry-mesh-lib)\n", filename, text);
          fclose (file);
          return 0;
        }
        gmesh_raw_vertex (raw, x, y, z);
        nloop++;
      } else if (!strncmp (text, "endloop", 7)) {
        if (nloop >= 1) {
          int k;
          Coords** corners = gmesh_raw_face (raw, nloop);
          if (corners) {
            for (k = 0; k < nloop; k++) {
              corners[k] = &raw->verts[first + k];
            }
          }
        }
        nloop = 0;
      }
    }
    if (!raw->nfaces) {
      fprintf (stderr, "Error: %s: no facets found in ASCII STL (geometry-mesh-lib)\n", filename);
      fclose (file);
      return 0;
    }
  }
  fclose (file);
  return 1;
}

// ==========================================================================
// PLY reader (ascii and binary)
// ==========================================================================

typedef struct gmesh_ply_prop {
  char name[64];
  int type; // scalar type, or item type for lists
  int list; // 1 for list properties
  int count_type; // count type for lists
} gmesh_ply_prop;

typedef struct gmesh_ply_elem {
  char name[64];
  long count;
  int nprop;
  gmesh_ply_prop prop[32];
} gmesh_ply_elem;

// type codes: size in bytes, sign/float encoded
enum { GMP_I8 = 1, GMP_U8, GMP_I16, GMP_U16, GMP_I32, GMP_U32, GMP_F32, GMP_F64 };

// type code of a PLY type name, 0 when unknown
static int
gmesh_ply_type (const char* name) {
  if (!strcmp (name, "char") || !strcmp (name, "int8"))
    return GMP_I8;
  if (!strcmp (name, "uchar") || !strcmp (name, "uint8"))
    return GMP_U8;
  if (!strcmp (name, "short") || !strcmp (name, "int16"))
    return GMP_I16;
  if (!strcmp (name, "ushort") || !strcmp (name, "uint16"))
    return GMP_U16;
  if (!strcmp (name, "int") || !strcmp (name, "int32"))
    return GMP_I32;
  if (!strcmp (name, "uint") || !strcmp (name, "uint32"))
    return GMP_U32;
  if (!strcmp (name, "float") || !strcmp (name, "float32"))
    return GMP_F32;
  if (!strcmp (name, "double") || !strcmp (name, "float64"))
    return GMP_F64;
  return 0;
}

// size in bytes of a value of the given type code
static int
gmesh_ply_size (int type) {
  if (type <= GMP_U8)
    return 1;
  if (type <= GMP_U16)
    return 2;
  if (type <= GMP_F32)
    return 4;
  return 8;
}

// format code of a PLY format name: 0 ascii, 1 binary little endian,
// 2 binary big endian, -1 when unknown
static int
gmesh_ply_format (const char* name) {
  if (!strcmp (name, "ascii"))
    return 0;
  if (!strcmp (name, "binary_little_endian"))
    return 1;
  if (!strcmp (name, "binary_big_endian"))
    return 2;
  return -1;
}

// read one value of the given type code from file into *value, in the given
// format code (see gmesh_ply_format). Returns 1 on success, 0 on error.
static int
gmesh_ply_value (FILE* file, int format, int type, double* value) {
  unsigned char bytes[8];
  int size = gmesh_ply_size (type);
  int k;
  uint16_t one = 1;
  unsigned char first_byte;
  int host_little_endian;
  int file_big_endian = (format == 2);
  if (format == 0)
    return fscanf (file, "%lg", value) == 1;
  if ((int)fread (bytes, 1, size, file) != size)
    return 0;
  memcpy (&first_byte, &one, 1);
  host_little_endian = (first_byte == 1);
  // reverse the bytes when the file and host byte orders differ
  if (file_big_endian == host_little_endian) {
    for (k = 0; k < size / 2; k++) {
      unsigned char swap = bytes[k];
      bytes[k] = bytes[size - 1 - k];
      bytes[size - 1 - k] = swap;
    }
  }
  switch (type) {
  case GMP_I8:
    *value = (double)(int8_t)bytes[0];
    break;
  case GMP_U8:
    *value = (double)bytes[0];
    break;
  case GMP_I16: {
    int16_t number;
    memcpy (&number, bytes, 2);
    *value = number;
    break;
  }
  case GMP_U16: {
    uint16_t number;
    memcpy (&number, bytes, 2);
    *value = number;
    break;
  }
  case GMP_I32: {
    int32_t number;
    memcpy (&number, bytes, 4);
    *value = number;
    break;
  }
  case GMP_U32: {
    uint32_t number;
    memcpy (&number, bytes, 4);
    *value = number;
    break;
  }
  case GMP_F32: {
    float number;
    memcpy (&number, bytes, 4);
    *value = number;
    break;
  }
  case GMP_F64: {
    double number;
    memcpy (&number, bytes, 8);
    *value = number;
    break;
  }
  default:
    return 0;
  }
  return 1;
}

// read an ASCII or binary PLY file into raw.
// Returns 1 on success, 0 on error (with a message).
static int
gmesh_read_ply (FILE* file, char* filename, gmesh_raw* raw) {
  gmesh_ply_elem elements[16];
  int nelements = 0;
  int format = -1;
  int e;
  long nvertices = -1;
  while (gmesh_readline (file, raw) >= 0) {
    char a[64] = "", b[64] = "", c[64] = "", d[64] = "";
    int nwords = sscanf (raw->line, "%63s %63s %63s %63s", a, b, c, d);
    if (nwords < 1)
      continue;
    if (!strcmp (a, "end_header"))
      break;
    if (!strcmp (a, "format")) {
      format = gmesh_ply_format (b);
      if (format < 0) {
        fprintf (stderr, "Error: %s: unknown PLY format '%s' (geometry-mesh-lib)\n", filename, b);
        return 0;
      }
    } else if (!strcmp (a, "element")) {
      gmesh_ply_elem* element;
      if (nelements >= 16) {
        fprintf (stderr, "Error: %s: too many PLY elements (geometry-mesh-lib)\n", filename);
        return 0;
      }
      element = &elements[nelements];
      snprintf (element->name, sizeof (element->name), "%s", b);
      element->count = atol (c);
      element->nprop = 0;
      nelements++;
    } else if (!strcmp (a, "property") && nelements) {
      gmesh_ply_elem* element = &elements[nelements - 1];
      gmesh_ply_prop* property;
      int unknown_type;
      if (element->nprop >= 32) {
        fprintf (stderr, "Error: %s: too many PLY properties (geometry-mesh-lib)\n", filename);
        return 0;
      }
      property = &element->prop[element->nprop];
      element->nprop++;
      if (!strcmp (b, "list")) {
        char name[64] = "";
        property->list = 1;
        property->count_type = gmesh_ply_type (c);
        property->type = gmesh_ply_type (d);
        sscanf (raw->line, "%*s %*s %*s %*s %63s", name);
        strcpy (property->name, name);
      } else {
        property->list = 0;
        property->count_type = 0;
        property->type = gmesh_ply_type (b);
        snprintf (property->name, sizeof (property->name), "%s", c);
      }
      unknown_type = !property->type;
      if (property->list && !property->count_type) {
        unknown_type = 1;
      }
      if (unknown_type) {
        fprintf (stderr, "Error: %s: unknown PLY property type in '%s' (geometry-mesh-lib)\n", filename, raw->line);
        return 0;
      }
    }
  }
  if (format < 0) {
    fprintf (stderr, "Error: %s: PLY format line missing (geometry-mesh-lib)\n", filename);
    return 0;
  }

  for (e = 0; e < nelements; e++) {
    gmesh_ply_elem* element = &elements[e];
    int is_vertex = !strcmp (element->name, "vertex");
    int is_face = !strcmp (element->name, "face");
    int x_index = -1, y_index = -1, z_index = -1, list_index = -1, k;
    long i;
    if (is_vertex) {
      int has_xyz;
      for (k = 0; k < element->nprop; k++) {
        if (!strcmp (element->prop[k].name, "x"))
          x_index = k;
        if (!strcmp (element->prop[k].name, "y"))
          y_index = k;
        if (!strcmp (element->prop[k].name, "z"))
          z_index = k;
      }
      has_xyz = (x_index >= 0 && y_index >= 0);
      if (z_index < 0) {
        has_xyz = 0;
      }
      if (!has_xyz) {
        fprintf (stderr, "Error: %s: PLY vertex without x y z (geometry-mesh-lib)\n", filename);
        return 0;
      }
      nvertices = element->count;
    }
    if (is_face) {
      // the vertex list is "vertex_indices" or "vertex_index", else the first list
      for (k = 0; k < element->nprop; k++) {
        const char* name = element->prop[k].name;
        int is_index_list = (!strcmp (name, "vertex_indices") || !strcmp (name, "vertex_index"));
        if (!element->prop[k].list)
          continue;
        if (list_index < 0 || is_index_list)
          list_index = k;
      }
      if (list_index < 0) {
        fprintf (stderr, "Error: %s: PLY face without vertex list (geometry-mesh-lib)\n", filename);
        return 0;
      }
      if (nvertices < 0) {
        fprintf (stderr, "Error: %s: PLY faces before vertices (geometry-mesh-lib)\n", filename);
        return 0;
      }
    }
    for (i = 0; i < element->count; i++) {
      double xyz[3] = { 0, 0, 0 };
      double scalars[32];
      int nscalars = 0;
      for (k = 0; k < element->nprop; k++) {
        gmesh_ply_prop* property = &element->prop[k];
        double value;
        if (property->list) {
          long nitems, j;
          if (!gmesh_ply_value (file, format, property->count_type, &value))
            goto truncated;
          nitems = (long)value;
          if (is_face && k == list_index) {
            Coords** corners = gmesh_raw_face (raw, (int)nitems);
            for (j = 0; j < nitems; j++) {
              if (!gmesh_ply_value (file, format, property->type, &value))
                goto truncated;
              if (value < 0 || value >= nvertices) {
                fprintf (stderr, "Error: %s: invalid vertex index %g in face %ld (geometry-mesh-lib)\n", filename, value, i);
                return 0;
              }
              if (corners) {
                corners[j] = &raw->verts[(int)value];
              }
            }
          } else {
            for (j = 0; j < nitems; j++) {
              if (!gmesh_ply_value (file, format, property->type, &value))
                goto truncated;
            }
          }
        } else {
          if (!gmesh_ply_value (file, format, property->type, &value))
            goto truncated;
          if (is_vertex) {
            if (k == x_index) {
              xyz[0] = value;
            } else if (k == y_index) {
              xyz[1] = value;
            } else if (k == z_index) {
              xyz[2] = value;
            }
          }
          if (is_face) {
            // scalar face property (e.g. colour, m value)
            scalars[nscalars] = value;
            nscalars++;
          }
        }
      }
      if (is_vertex) {
        gmesh_raw_vertex (raw, xyz[0], xyz[1], xyz[2]);
      }
      if (is_face) {
        for (k = 0; k < nscalars; k++) {
          gmesh_raw_prop (raw, scalars[k]);
        }
      }
    }
  }
  return 1;
truncated:
  fprintf (stderr, "Error: %s: PLY data truncated or unreadable (geometry-mesh-lib)\n", filename);
  return 0;
}

// ==========================================================================
// Hash for vertex welding and edges
// ==========================================================================

// 64 bit hash finaliser (MurmurHash3 fmix64)
static uint64_t
gmesh_mix (uint64_t hash) {
  hash ^= hash >> 33;
  hash *= 0xff51afd7ed558ccdULL;
  hash ^= hash >> 33;
  hash *= 0xc4ceb9fe1a85ec53ULL;
  hash ^= hash >> 33;
  return hash;
}

// edge table entry: undirected edge (a < b), number of uses n, direction sum
// dir (+1 per use as a->b, -1 per use as b->a) and the first two triangles
typedef struct gmesh_edge {
  int a, b;
  int n;
  int dir;
  int t0, t1;
} gmesh_edge;

// open addressing hash table of edges: cap entries (a power of 2), n used
typedef struct gmesh_etab {
  gmesh_edge* e;
  int cap, n;
} gmesh_etab;

// allocate an empty edge table for up to nedges edges, at most about half
// full so that the probe sequences stay short
static void
gmesh_etab_alloc (gmesh_etab* table, int nedges) {
  table->cap = 16;
  while (table->cap < 2 * nedges + 16) {
    table->cap *= 2;
  }
  table->e = gmesh_xcalloc (table->cap, sizeof (gmesh_edge));
  table->n = 0;
}

// the entry of the undirected edge (vertex_a, vertex_b) in table, added
// with n = 0 when the edge is not in the table yet
static gmesh_edge*
gmesh_edge_get (gmesh_etab* table, int vertex_a, int vertex_b) {
  int low = vertex_a;
  int high = vertex_b;
  uint64_t hash;
  int i;
  gmesh_edge* edge;
  if (vertex_b < vertex_a) {
    low = vertex_b;
    high = vertex_a;
  }
  hash = gmesh_mix ((uint64_t)low * 0x9E3779B97F4A7C15ULL ^ (uint64_t)high);
  i = (int)(hash & (uint64_t)(table->cap - 1));
  while (table->e[i].n) {
    if (table->e[i].a == low && table->e[i].b == high)
      return &table->e[i];
    i = (i + 1) & (table->cap - 1);
  }
  edge = &table->e[i];
  edge->a = low;
  edge->b = high;
  edge->dir = 0;
  edge->t0 = -1;
  edge->t1 = -1;
  table->n++;
  return edge;
}

// build the edge table of the ntriangles triangles in triangles[3*ntriangles]
static void
gmesh_etab_build (gmesh_etab* table, int* triangles, int ntriangles) {
  int i, k;
  gmesh_etab_alloc (table, 3 * ntriangles);
  for (i = 0; i < ntriangles; i++) {
    for (k = 0; k < 3; k++) {
      int a = triangles[3 * i + k];
      int b = triangles[3 * i + (k + 1) % 3];
      gmesh_edge* edge = gmesh_edge_get (table, a, b);
      if (edge->n == 0) {
        edge->t0 = i;
      } else if (edge->n == 1) {
        edge->t1 = i;
      }
      edge->n++;
      if (a < b) {
        edge->dir += 1;
      } else {
        edge->dir -= 1;
      }
    }
  }
}

// ==========================================================================
// Geometry helpers
// ==========================================================================

// normal of the polygon vertices[indices[0..count-1]] by Newell's method,
// right-handed with respect to the vertex order; its length is twice the area
static Coords
gmesh_newell (Coords* vertices, int* indices, int count) {
  Coords normal = coords_set (0, 0, 0);
  int i;
  int previous = count - 1;
  for (i = 0; i < count; i++) {
    Coords a = vertices[indices[i]];
    Coords b = vertices[indices[previous]];
    normal.x += (b.y - a.y) * (a.z + b.z);
    normal.y += (b.z - a.z) * (a.x + b.x);
    normal.z += (b.x - a.x) * (a.y + b.y);
    previous = i;
  }
  return normal;
}

// append the triangle (a, b, c) to triangles, which has room for it
static void
gmesh_add_triangle (int* triangles, int* triangle_count, int a, int b, int c) {
  int n = *triangle_count;
  triangles[3 * n] = a;
  triangles[3 * n + 1] = b;
  triangles[3 * n + 2] = c;
  (*triangle_count)++;
}

// twice the signed area of the 2D triangle (a, b, point) with coordinates
// x[], y[]: positive when point lies to the left of the edge a->b
static double
gmesh_turn (double* x, double* y, int a, int b, int point) {
  return (x[b] - x[a]) * (y[point] - y[a]) - (y[b] - y[a]) * (x[point] - x[a]);
}

// ear-clipping triangulation of the polygon vertices[indices[0..count-1]]
// (welded, no repeats). Appends the triangles to *triangles, of *capacity
// triangles, grown as needed. Returns the number of triangles added.
static int
gmesh_triangulate (Coords* vertices, int* indices, int count, int** triangles, int* triangle_count, int* capacity) {
  int added = 0;
  int i;
  if (count < 3)
    return 0;
  if (*triangle_count + count > *capacity) {
    if (*capacity == 0) {
      *capacity = 4096;
    }
    while (*triangle_count + count > *capacity) {
      *capacity *= 2;
    }
    *triangles = gmesh_xrealloc (*triangles, 3 * (*capacity) * sizeof (int));
  }
  if (count == 3) {
    gmesh_add_triangle (*triangles, triangle_count, indices[0], indices[1], indices[2]);
    return 1;
  }
  {
    Coords normal = gmesh_newell (vertices, indices, count);
    double ax = fabs (normal.x);
    double ay = fabs (normal.y);
    double az = fabs (normal.z);
    double sign;
    int px, py; // the two coordinates of the projection plane
    int remaining = count;
    int guard = 0;
    int* corner = gmesh_xrealloc (NULL, count * sizeof (int)); // corners not clipped yet
    double* X = gmesh_xrealloc (NULL, count * sizeof (double));
    double* Y = gmesh_xrealloc (NULL, count * sizeof (double));
    // project on the plane most perpendicular to the normal
    if (az >= ax && az >= ay) {
      px = 0;
      py = 1;
      sign = normal.z;
    } else if (ay >= ax) {
      px = 2;
      py = 0;
      sign = normal.y;
    } else {
      px = 1;
      py = 2;
      sign = normal.x;
    }
    if (sign >= 0) {
      sign = 1;
    } else {
      sign = -1;
    }
    for (i = 0; i < count; i++) {
      Coords p = vertices[indices[i]];
      double xyz[3] = { p.x, p.y, p.z };
      X[i] = xyz[px];
      Y[i] = xyz[py];
      corner[i] = i;
    }
    while (remaining > 3 && guard < 2 * count * count) {
      int found = 0;
      for (i = 0; i < remaining && !found; i++) {
        int ia = corner[(i + remaining - 1) % remaining];
        int ib = corner[i];
        int ic = corner[(i + 1) % remaining];
        int j;
        // reflex or flat corner
        if (gmesh_turn (X, Y, ia, ib, ic) * sign <= 0)
          continue;
        // no other vertex inside the ear
        for (j = 0; j < remaining; j++) {
          int ip = corner[j];
          int inside;
          if (ip == ia || ip == ib)
            continue;
          if (ip == ic)
            continue;
          inside = (gmesh_turn (X, Y, ia, ib, ip) * sign >= 0 && gmesh_turn (X, Y, ib, ic, ip) * sign >= 0);
          if (inside && gmesh_turn (X, Y, ic, ia, ip) * sign >= 0)
            break;
        }
        if (j < remaining)
          continue;
        gmesh_add_triangle (*triangles, triangle_count, indices[ia], indices[ib], indices[ic]);
        added++;
        for (j = i; j < remaining - 1; j++) {
          corner[j] = corner[j + 1];
        }
        remaining--;
        found = 1;
      }
      // self-intersecting / degenerate: fan the rest
      if (!found)
        break;
      guard++;
    }
    for (i = 1; i + 1 < remaining; i++) {
      gmesh_add_triangle (*triangles, triangle_count, indices[corner[0]], indices[corner[i]], indices[corner[i + 1]]);
      added++;
    }
    free (corner);
    free (X);
    free (Y);
  }
  return added;
}

// integer key of point for welding: the coordinates rounded to multiples of
// tolerance, or the bits of the coordinates when tolerance is 0
static void
gmesh_quantize (Coords point, double tolerance, int64_t key[3]) {
  if (tolerance > 0) {
    key[0] = (int64_t)llround (point.x / tolerance);
    key[1] = (int64_t)llround (point.y / tolerance);
    key[2] = (int64_t)llround (point.z / tolerance);
  } else {
    memcpy (&key[0], &point.x, 8);
    memcpy (&key[1], &point.y, 8);
    memcpy (&key[2], &point.z, 8);
  }
}

// reverse the vertex order (and the normal) of a triangle of mesh
static void
gmesh_flip_triangle (gmesh_struct* mesh, int triangle) {
  int swap = mesh->tri[3 * triangle + 1];
  mesh->tri[3 * triangle + 1] = mesh->tri[3 * triangle + 2];
  mesh->tri[3 * triangle + 2] = swap;
  mesh->tri_normal[triangle] = coords_scale (mesh->tri_normal[triangle], -1);
}

// 1 when a triangle of mesh runs along the edge from -> to in this direction
static int
gmesh_tri_has_edge (gmesh_struct* mesh, int triangle, int from, int to) {
  int k;
  for (k = 0; k < 3; k++) {
    if (mesh->tri[3 * triangle + k] == from && mesh->tri[3 * triangle + (k + 1) % 3] == to)
      return 1;
  }
  return 0;
}

// the vertex of mesh farthest from point, and its distance in *distance
static Coords
gmesh_farthest_vertex (gmesh_struct* mesh, Coords point, double* distance) {
  Coords farthest = point;
  int i;
  *distance = -1;
  for (i = 0; i < mesh->nverts; i++) {
    double d = coords_len (coords_sub (mesh->vtx[i], point));
    if (d > *distance) {
      *distance = d;
      farthest = mesh->vtx[i];
    }
  }
  return farthest;
}

// polygons as sorted vertex lists ([n i1 .. in] at gmesh_sort_key_start[face]),
// used by gmesh_cmp_faces while sorting with qsort
static int* gmesh_sort_keys;
static int* gmesh_sort_key_start;

// qsort comparison of two polygon numbers by their sorted vertex lists:
// the number of vertices first, then the vertices in order
static int
gmesh_cmp_faces (const void* first, const void* second) {
  const int* first_key = &gmesh_sort_keys[gmesh_sort_key_start[*(const int*)first]];
  const int* second_key = &gmesh_sort_keys[gmesh_sort_key_start[*(const int*)second]];
  int k;
  if (first_key[0] < second_key[0])
    return -1;
  if (first_key[0] > second_key[0])
    return 1;
  for (k = 1; k <= first_key[0]; k++) {
    if (first_key[k] < second_key[k])
      return -1;
    if (first_key[k] > second_key[k])
      return 1;
  }
  return 0;
}

// qsort comparison of two vertex indices
static int
gmesh_cmp_indices (const void* first, const void* second) {
  int first_index = *(const int*)first;
  int second_index = *(const int*)second;
  if (first_index < second_index)
    return -1;
  if (first_index > second_index)
    return 1;
  return 0;
}

// mark in duplicate[] every polygon of faces[0..size-1] (polygon i starts at
// face_start[i]) whose set of vertices is given more than once, e.g. the inner
// faces of a volumetric mesh, each shared by two cells.
// Returns the number of marked polygons.
static int
gmesh_find_duplicates (int* faces, int size, int* face_start, int nfaces, char* duplicate) {
  int* keys = gmesh_xrealloc (NULL, size * sizeof (int));
  int* order = gmesh_xrealloc (NULL, nfaces * sizeof (int));
  int nduplicate = 0;
  int i, j, k;
  memcpy (keys, faces, size * sizeof (int));
  for (i = 0; i < nfaces; i++) {
    int* key = &keys[face_start[i]];
    qsort (key + 1, key[0], sizeof (int), gmesh_cmp_indices);
    order[i] = i;
  }
  gmesh_sort_keys = keys;
  gmesh_sort_key_start = face_start;
  qsort (order, nfaces, sizeof (int), gmesh_cmp_faces);
  i = 0;
  while (i < nfaces) {
    j = i + 1;
    while (j < nfaces && gmesh_cmp_faces (&order[i], &order[j]) == 0) {
      j++;
    }
    // polygons of less than 3 vertices give no triangles anyway
    if (j - i > 1 && keys[face_start[order[i]]] >= 3) {
      for (k = i; k < j; k++) {
        duplicate[order[k]] = 1;
        nduplicate++;
      }
    }
    i = j;
  }
  free (keys);
  free (order);
  return nduplicate;
}

// ==========================================================================
// BVH build
// ==========================================================================

// centroids and axis used by gmesh_cmp_cent while sorting with qsort
static Coords* gmesh_sort_cent;
static int gmesh_sort_axis;

// qsort comparison of two triangle numbers by the coordinate of their
// centroids along gmesh_sort_axis
static int
gmesh_cmp_cent (const void* first, const void* second) {
  Coords first_centroid = gmesh_sort_cent[*(const int*)first];
  Coords second_centroid = gmesh_sort_cent[*(const int*)second];
  double first_xyz[3] = { first_centroid.x, first_centroid.y, first_centroid.z };
  double second_xyz[3] = { second_centroid.x, second_centroid.y, second_centroid.z };
  double first_value = first_xyz[gmesh_sort_axis];
  double second_value = second_xyz[gmesh_sort_axis];
  if (first_value < second_value)
    return -1;
  if (first_value > second_value)
    return 1;
  return 0;
}

// grow the box [lower, upper] to contain a triangle of triangles[]
static void
gmesh_box_tri (gmesh_struct* mesh, int* triangles, int triangle, Coords* lower, Coords* upper) {
  int k;
  for (k = 0; k < 3; k++) {
    Coords p = mesh->vtx[triangles[3 * triangle + k]];
    if (p.x < lower->x)
      lower->x = p.x;
    if (p.y < lower->y)
      lower->y = p.y;
    if (p.z < lower->z)
      lower->z = p.z;
    if (p.x > upper->x)
      upper->x = p.x;
    if (p.y > upper->y)
      upper->y = p.y;
    if (p.z > upper->z)
      upper->z = p.z;
  }
}

// a BVH node still to be built from the triangles order[start..end-1]
typedef struct gmesh_bvh_task {
  int node;
  int start;
  int end;
  int depth;
} gmesh_bvh_task;

// push a node to build on the stack of *top tasks
static void
gmesh_bvh_push (gmesh_bvh_task* stack, int* top, int node, int start, int end, int depth) {
  stack[*top].node = node;
  stack[*top].start = start;
  stack[*top].end = end;
  stack[*top].depth = depth;
  (*top)++;
}

// build the bounding volume hierarchy of mesh (median split along the
// longest axis of the centroids) and put the triangles in BVH order
static void
gmesh_build_bvh (gmesh_struct* mesh) {
  int n = mesh->ntris;
  int i;
  int top = 0;
  int* order = gmesh_xrealloc (NULL, n * sizeof (int));
  Coords* cent = gmesh_xrealloc (NULL, n * sizeof (Coords));
  int* sorted_tri;
  int* sorted_face;
  Coords* sorted_normal;
  gmesh_bvh_task stack[2 * GMESH_BVH_STACK + 2];
  mesh->node = gmesh_xrealloc (NULL, (2 * n + 1) * sizeof (gmesh_node));
  for (i = 0; i < n; i++) {
    Coords a = mesh->vtx[mesh->tri[3 * i]];
    Coords b = mesh->vtx[mesh->tri[3 * i + 1]];
    Coords c = mesh->vtx[mesh->tri[3 * i + 2]];
    cent[i] = coords_set ((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3);
    order[i] = i;
  }
  mesh->nnodes = 1;
  gmesh_bvh_push (stack, &top, 0, 0, n, 0);
  while (top > 0) {
    gmesh_bvh_task task;
    gmesh_node* node;
    Coords lo = coords_set (FLT_MAX, FLT_MAX, FLT_MAX);
    Coords hi = coords_set (-FLT_MAX, -FLT_MAX, -FLT_MAX);
    Coords clo = lo; // box of the centroids
    Coords chi = hi;
    top--;
    task = stack[top];
    node = &mesh->node[task.node];
    for (i = task.start; i < task.end; i++) {
      Coords c = cent[order[i]];
      gmesh_box_tri (mesh, mesh->tri, order[i], &lo, &hi);
      if (c.x < clo.x)
        clo.x = c.x;
      if (c.y < clo.y)
        clo.y = c.y;
      if (c.z < clo.z)
        clo.z = c.z;
      if (c.x > chi.x)
        chi.x = c.x;
      if (c.y > chi.y)
        chi.y = c.y;
      if (c.z > chi.z)
        chi.z = c.z;
    }
    node->lo = lo;
    node->hi = hi;
    if (task.end - task.start <= GMESH_BVH_LEAF || task.depth >= GMESH_BVH_STACK - 2) {
      node->first = task.start;
      node->count = (int)(task.end - task.start);
    } else {
      double dx = chi.x - clo.x;
      double dy = chi.y - clo.y;
      double dz = chi.z - clo.z;
      int mid = (task.start + task.end) / 2;
      int left = mesh->nnodes;
      if (dx >= dy && dx >= dz) {
        gmesh_sort_axis = 0;
      } else if (dy >= dz) {
        gmesh_sort_axis = 1;
      } else {
        gmesh_sort_axis = 2;
      }
      gmesh_sort_cent = cent;
      qsort (order + task.start, task.end - task.start, sizeof (int), gmesh_cmp_cent);
      node->first = left;
      node->count = 0;
      mesh->nnodes += 2;
      gmesh_bvh_push (stack, &top, left, task.start, mid, task.depth + 1);
      gmesh_bvh_push (stack, &top, left + 1, mid, task.end, task.depth + 1);
    }
  }
  // permute the triangles into BVH order
  sorted_tri = gmesh_xrealloc (NULL, 3 * n * sizeof (int));
  sorted_face = gmesh_xrealloc (NULL, n * sizeof (int));
  sorted_normal = gmesh_xrealloc (NULL, n * sizeof (Coords));
  for (i = 0; i < n; i++) {
    int o = order[i];
    sorted_tri[3 * i] = mesh->tri[3 * o];
    sorted_tri[3 * i + 1] = mesh->tri[3 * o + 1];
    sorted_tri[3 * i + 2] = mesh->tri[3 * o + 2];
    sorted_face[i] = mesh->tri_face[o];
    sorted_normal[i] = mesh->tri_normal[o];
  }
  free (mesh->tri);
  free (mesh->tri_face);
  free (mesh->tri_normal);
  mesh->tri = sorted_tri;
  mesh->tri_face = sorted_face;
  mesh->tri_normal = sorted_normal;
  mesh->node = gmesh_xrealloc (mesh->node, mesh->nnodes * sizeof (gmesh_node));
  free (order);
  free (cent);
}

// ==========================================================================
// Init
// ==========================================================================

// read filename (STL, PLY or OFF) into raw, choosing the reader from the
// extension or the first bytes. Returns 1 on success, 0 on error.
static int
gmesh_read_file (char* filename, const char* extension, gmesh_raw* raw) {
  FILE* file;
  char head[8] = "";
  int ok;
  if (gmesh_is_extension (extension, ".stl", ".STL")) {
    return gmesh_read_stl (filename, raw);
  }
  file = Open_File (filename, "rb", NULL);
  if (!file)
    return 0;
  if (fread (head, 1, 3, file) != 3) {
    fclose (file);
    fprintf (stderr, "Error: %s is empty (geometry-mesh-lib)\n", filename);
    return 0;
  }
  fseek (file, 0, SEEK_SET);
  if (!strncmp (head, "sol", 3)) {
    // STL without .stl extension
    fclose (file);
    return gmesh_read_stl (filename, raw);
  }
  if (!strncmp (head, "ply", 3)) {
    ok = gmesh_read_ply (file, filename, raw);
  } else {
    ok = gmesh_read_off (file, filename, raw);
  }
  fclose (file);
  return ok;
}

// read an STL/OFF/PLY file into mesh, see geometry-mesh-lib.h
int
gmesh_init (char* filename, double scale, int face_props, gmesh_struct* mesh) {
  gmesh_raw raw;
  const char* extension;
  int ok = 0;
  int i, k;
  int* map;

  if (!mesh || !filename)
    return 0;
  if (!strlen (filename))
    return 0;
  memset (mesh, 0, sizeof (gmesh_struct));
  memset (&raw, 0, sizeof (gmesh_raw));
  mesh->filename = filename;

  MPI_MASTER (printf ("Loading geometry file (STL/OFF/PLY): %s\n", filename););

  extension = strrchr (filename, '.');
  if (gmesh_is_extension (extension, ".step", ".STEP") || gmesh_is_extension (extension, ".stp", ".STP")) {
    fprintf (stderr, "Error: %s: STEP files are not supported, please export the geometry as STL, OFF or PLY (geometry-mesh-lib)\n", filename);
    return 0;
  }
  // counting pass, then allocate and read again to fill the arrays
  raw.counting = 1;
  raw.keep_props = (face_props != GMESH_PROPS_IGNORE);
  ok = gmesh_read_file (filename, extension, &raw);
  if (!raw.nverts || !raw.nfaces) {
    ok = 0;
  }
  if (ok) {
    gmesh_raw_alloc (&raw);
    ok = gmesh_read_file (filename, extension, &raw);
  }
  // a component that needs the properties gets them for every face
  if (ok && face_props == GMESH_PROPS_REQUIRED) {
    for (i = 0; i < raw.nfaces; i++) {
      if (raw.faces[i].nprops == 0) {
        fprintf (stderr, "Error: %s: face %d has no per-face properties, which this component requires (geometry-mesh-lib)\n", filename, i);
        ok = 0;
        break;
      }
    }
  }
  if (!ok) {
    gmesh_raw_free (&raw);
    return 0;
  }

  // scale, and bounding box
  if (!scale)
    scale = 1;
  mesh->bbmin = coords_set (FLT_MAX, FLT_MAX, FLT_MAX);
  mesh->bbmax = coords_set (-FLT_MAX, -FLT_MAX, -FLT_MAX);
  for (i = 0; i < raw.nverts; i++) {
    Coords* p = &raw.verts[i];
    p->x *= scale;
    p->y *= scale;
    p->z *= scale;
    if (p->x < mesh->bbmin.x)
      mesh->bbmin.x = p->x;
    if (p->x > mesh->bbmax.x)
      mesh->bbmax.x = p->x;
    if (p->y < mesh->bbmin.y)
      mesh->bbmin.y = p->y;
    if (p->y > mesh->bbmax.y)
      mesh->bbmax.y = p->y;
    if (p->z < mesh->bbmin.z)
      mesh->bbmin.z = p->z;
    if (p->z > mesh->bbmax.z)
      mesh->bbmax.z = p->z;
  }
  mesh->length_scale = coords_len (coords_sub (mesh->bbmax, mesh->bbmin));
  if (mesh->length_scale <= 0)
    mesh->length_scale = 1;

  // weld vertices: identical within GMESH_WELD_TOL*diagonal (grid rounding)
  map = gmesh_xrealloc (NULL, raw.nverts * sizeof (int));
  mesh->vtx = gmesh_xrealloc (NULL, raw.nverts * sizeof (Coords));
  {
    double tolerance = GMESH_WELD_TOL * mesh->length_scale;
    int hash_cap = 16;
    int* hash_table;
    while (hash_cap < 2 * raw.nverts) {
      hash_cap *= 2;
    }
    hash_table = gmesh_xrealloc (NULL, hash_cap * sizeof (int));
    for (i = 0; i < hash_cap; i++) {
      hash_table[i] = -1;
    }
    mesh->nverts = 0;
    for (i = 0; i < raw.nverts; i++) {
      Coords p = raw.verts[i];
      int64_t key[3];
      int64_t other_key[3];
      uint64_t hash;
      int slot;
      gmesh_quantize (p, tolerance, key);
      hash = gmesh_mix ((uint64_t)key[0] ^ gmesh_mix ((uint64_t)key[1] ^ gmesh_mix ((uint64_t)key[2])));
      slot = (int)(hash & (uint64_t)(hash_cap - 1));
      // probe until an empty slot or a welded vertex with the same key
      while (hash_table[slot] >= 0) {
        gmesh_quantize (mesh->vtx[hash_table[slot]], tolerance, other_key);
        if (!memcmp (other_key, key, sizeof (key)))
          break;
        slot = (slot + 1) & (hash_cap - 1);
      }
      if (hash_table[slot] >= 0) {
        map[i] = hash_table[slot];
      } else {
        hash_table[slot] = mesh->nverts;
        mesh->vtx[mesh->nverts] = p;
        map[i] = mesh->nverts;
        mesh->nverts++;
      }
    }
    free (hash_table);
    mesh->nwelded = raw.nverts - mesh->nverts;
    mesh->vtx = gmesh_xrealloc (mesh->vtx, mesh->nverts * sizeof (Coords));
  }

  // faces with welded indices (consecutive repeats removed)
  mesh->nfaces = raw.nfaces;
  mesh->faceArray = gmesh_xrealloc (NULL, (raw.nfaces + raw.ncorners) * sizeof (int));
  {
    int* face_start = gmesh_xrealloc (NULL, raw.nfaces * sizeof (int));
    char* duplicate = gmesh_xcalloc (raw.nfaces, 1);
    int face_index;
    int write;
    int ntri = 0;
    int* tri = NULL;
    int* tri_face = NULL;
    int tri_cap = 0;
    mesh->faceSize = 0;
    for (face_index = 0; face_index < raw.nfaces; face_index++) {
      gmesh_polygon* face = &raw.faces[face_index];
      int nn = 0;
      int start = mesh->faceSize;
      int j;
      int* out;
      face_start[face_index] = start;
      mesh->faceArray[start] = 0;
      mesh->faceSize++;
      out = &mesh->faceArray[mesh->faceSize];
      for (j = 0; j < face->nverts; j++) {
        int id = map[face->verts[j] - raw.verts];
        if (nn && out[nn - 1] == id)
          continue;
        out[nn] = id;
        nn++;
      }
      while (nn > 1 && out[nn - 1] == out[0]) {
        nn--;
      }
      mesh->faceArray[start] = nn;
      mesh->faceSize += nn;
    }
    // polygons given more than once (inner faces of a volumetric mesh) are removed
    mesh->nduplicate = gmesh_find_duplicates (mesh->faceArray, mesh->faceSize, face_start, raw.nfaces, duplicate);
    // triangulation; removed polygons stay as empty polygons, so that the face
    // numbers still match the file
    write = 0;
    for (face_index = 0; face_index < raw.nfaces; face_index++) {
      int nn = (int)mesh->faceArray[face_start[face_index]];
      int before = ntri;
      int j;
      int* out;
      if (duplicate[face_index]) {
        nn = 0;
      }
      memmove (&mesh->faceArray[write + 1], &mesh->faceArray[face_start[face_index] + 1], nn * sizeof (int));
      mesh->faceArray[write] = nn;
      out = &mesh->faceArray[write + 1];
      write += nn + 1;
      if (nn >= 3) {
        gmesh_triangulate (mesh->vtx, out, nn, &tri, &ntri, &tri_cap);
      }
      if (ntri > before) {
        tri_face = gmesh_xrealloc (tri_face, tri_cap * sizeof (int));
        for (j = before; j < ntri; j++) {
          tri_face[j] = face_index;
        }
      }
    }
    mesh->faceSize = write;
    free (face_start);
    free (duplicate);
    mesh->tri = tri;
    mesh->tri_face = tri_face;
    mesh->ntris = ntri;
  }
  free (map);

  // per-face properties (column-major, missing values 0)
  for (i = 0; i < raw.nfaces; i++) {
    if (raw.faces[i].nprops > mesh->nfaceprops)
      mesh->nfaceprops = raw.faces[i].nprops;
  }
  if (mesh->nfaceprops) {
    mesh->facePropSize = (int)mesh->nfaceprops * raw.nfaces;
    mesh->facePropArray = gmesh_xcalloc (mesh->facePropSize, sizeof (double));
    for (i = 0; i < raw.nfaces; i++) {
      for (k = 0; k < raw.faces[i].nprops; k++) {
        mesh->facePropArray[k * raw.nfaces + i] = raw.faces[i].props[k];
      }
    }
  }
  gmesh_raw_free (&raw);

  // normals; drop zero-area triangles
  mesh->tri_normal = gmesh_xrealloc (NULL, mesh->ntris * sizeof (Coords));
  {
    int kept = 0;
    for (i = 0; i < mesh->ntris; i++) {
      Coords a = mesh->vtx[mesh->tri[3 * i]];
      Coords b = mesh->vtx[mesh->tri[3 * i + 1]];
      Coords c = mesh->vtx[mesh->tri[3 * i + 2]];
      Coords normal = coords_xp (coords_sub (b, a), coords_sub (c, a));
      double length = coords_len (normal);
      if (length <= 1e-14 * mesh->length_scale * mesh->length_scale) {
        mesh->ndegenerate++;
        continue;
      }
      mesh->tri[3 * kept] = mesh->tri[3 * i];
      mesh->tri[3 * kept + 1] = mesh->tri[3 * i + 1];
      mesh->tri[3 * kept + 2] = mesh->tri[3 * i + 2];
      mesh->tri_face[kept] = mesh->tri_face[i];
      mesh->tri_normal[kept] = coords_scale (normal, 1 / length);
      mesh->area += length / 2;
      kept++;
    }
    mesh->ntris = kept;
  }
  if (!mesh->ntris) {
    fprintf (stderr, "Error: %s contains no valid triangles (geometry-mesh-lib)\n", filename);
    gmesh_free (mesh);
    return 0;
  }

  // topology, and consistent orientation by propagation over shared edges
  {
    gmesh_etab edges;
    int nused = 0;
    int* queue = gmesh_xrealloc (NULL, mesh->ntris * sizeof (int));
    int* visited = gmesh_xcalloc (mesh->ntris, sizeof (int));
    char* used = gmesh_xcalloc (mesh->nverts, 1);
    gmesh_etab_build (&edges, mesh->tri, mesh->ntris);
    // BFS: flip triangles so that shared (manifold) edges run in opposite directions
    for (i = 0; i < mesh->ntris; i++) {
      int head = 0;
      int tail = 0;
      if (visited[i])
        continue;
      visited[i] = 1;
      queue[tail] = i;
      tail++;
      while (head < tail) {
        int t = queue[head];
        head++;
        for (k = 0; k < 3; k++) {
          int a = mesh->tri[3 * t + k];
          int b = mesh->tri[3 * t + (k + 1) % 3];
          int other;
          gmesh_edge* edge = gmesh_edge_get (&edges, a, b);
          if (edge->n != 2)
            continue;
          if (edge->t0 == t) {
            other = edge->t1;
          } else {
            other = edge->t0;
          }
          if (visited[other])
            continue;
          // other uses the edge a->b in the same direction: flip it
          if (gmesh_tri_has_edge (mesh, other, a, b)) {
            gmesh_flip_triangle (mesh, other);
          }
          visited[other] = 1;
          queue[tail] = other;
          tail++;
        }
      }
    }
    free (edges.e);
    gmesh_etab_build (&edges, mesh->tri, mesh->ntris);
    mesh->nedges = edges.n;
    mesh->oriented = 1;
    for (i = 0; i < edges.cap; i++) {
      gmesh_edge* edge = &edges.e[i];
      if (!edge->n)
        continue;
      if (edge->n == 1) {
        mesh->nboundary++;
      } else if (edge->n > 2) {
        mesh->nnonmanifold++;
      } else if (edge->dir != 0) {
        mesh->oriented = 0;
      }
    }
    if (mesh->nnonmanifold)
      mesh->oriented = 0;
    free (edges.e);
    for (i = 0; i < 3 * mesh->ntris; i++) {
      if (!used[mesh->tri[i]]) {
        used[mesh->tri[i]] = 1;
        nused++;
      }
    }
    mesh->euler = (int)(nused - mesh->nedges + mesh->ntris);
    mesh->closed = (mesh->nboundary == 0 && mesh->nnonmanifold == 0);
    free (queue);
    free (visited);
    free (used);
  }

  // volume (divergence theorem); outward normals for closed oriented meshes
  {
    double volume = 0;
    int has_inside = (mesh->closed && mesh->oriented);
    for (i = 0; i < mesh->ntris; i++) {
      Coords a = mesh->vtx[mesh->tri[3 * i]];
      Coords b = mesh->vtx[mesh->tri[3 * i + 1]];
      Coords c = mesh->vtx[mesh->tri[3 * i + 2]];
      volume += coords_sp (a, coords_xp (b, c)) / 6;
    }
    if (has_inside && volume < 0) {
      for (i = 0; i < mesh->ntris; i++) {
        gmesh_flip_triangle (mesh, i);
      }
      volume = -volume;
      mesh->flipped = 1;
    }
    if (has_inside) {
      mesh->volume = volume;
    }
  }

  // bounding sphere (Ritter)
  {
    double distance;
    double radius;
    Coords b = gmesh_farthest_vertex (mesh, mesh->vtx[0], &distance);
    Coords c = gmesh_farthest_vertex (mesh, b, &distance);
    mesh->bscenter = coords_scale (coords_add (b, c), 0.5);
    radius = distance / 2;
    for (i = 0; i < mesh->nverts; i++) {
      double d = coords_len (coords_sub (mesh->vtx[i], mesh->bscenter));
      if (d > radius) {
        double new_radius = (radius + d) / 2;
        Coords shift = coords_scale (coords_sub (mesh->vtx[i], mesh->bscenter), (new_radius - radius) / d);
        mesh->bscenter = coords_add (mesh->bscenter, shift);
        radius = new_radius;
      }
    }
    mesh->bsradius = radius;
  }

  gmesh_build_bvh (mesh);
  mesh->ntris3 = 3 * mesh->ntris;

  MPI_MASTER (gmesh_print_info (mesh););
  return mesh->ntris;
} // gmesh_init

  // ==========================================================================
  // Ray tracing
  // ==========================================================================

  // slab test of the line origin + t*direction against the box [lower, upper],
  // for t in [t_start, t_end]. Returns 1 when that part of the line meets the box.
  // The BVH traversal uses it to skip every node whose box the line misses.
  #pragma acc routine seq
static int
gmesh_slab (Coords lower, Coords upper, Coords origin, Coords direction, double t_start, double t_end) {
  double origin_xyz[3] = { origin.x, origin.y, origin.z };
  double direction_xyz[3] = { direction.x, direction.y, direction.z };
  double lower_xyz[3] = { lower.x, lower.y, lower.z };
  double upper_xyz[3] = { upper.x, upper.y, upper.z };
  int k;
  for (k = 0; k < 3; k++) {
    double o = origin_xyz[k];
    double d = direction_xyz[k];
    double low = lower_xyz[k];
    double high = upper_xyz[k];
    double pad = 1e-9 * (high - low) + 1e-12 * (fabs (low) + fabs (high)) + 1e-300;
    low -= pad;
    high += pad;
    if (d == 0) {
      if (o < low || o > high)
        return 0;
    } else {
      double t1 = (low - o) / d;
      double t2 = (high - o) / d;
      if (t1 > t2) {
        double swap = t1;
        t1 = t2;
        t2 = swap;
      }
      if (t1 > t_start)
        t_start = t1;
      if (t2 < t_end)
        t_end = t2;
      if (t_start > t_end)
        return 0;
    }
  }
  return 1;
}

  // Moeller-Trumbore intersection of the line origin + t*direction with a
  // triangle of mesh, with tolerant barycentric coordinates. Returns 1 on a
  // hit, with its t in *hit_t and *on_edge = 1 when it is close to an edge.
  #pragma acc routine seq
static int
gmesh_tri_hit (gmesh_struct* mesh, int triangle, Coords origin, Coords direction, double* hit_t, int* on_edge) {
  Coords v0 = mesh->vtx[mesh->tri[3 * triangle]];
  Coords v1 = mesh->vtx[mesh->tri[3 * triangle + 1]];
  Coords v2 = mesh->vtx[mesh->tri[3 * triangle + 2]];
  Coords e1 = coords_sub (v1, v0);
  Coords e2 = coords_sub (v2, v0);
  Coords p = coords_xp (direction, e2);
  Coords s;
  Coords q;
  double det = coords_sp (e1, p);
  double inv;
  double u;
  double v;
  double eps = GMESH_EPSILON;
  // parallel
  if (fabs (det) <= 1e-12 * coords_len (e1) * coords_len (e2) * coords_len (direction))
    return 0;
  inv = 1 / det;
  s = coords_sub (origin, v0);
  u = coords_sp (s, p) * inv;
  if (u < -eps || u > 1 + eps)
    return 0;
  q = coords_xp (s, e1);
  v = coords_sp (direction, q) * inv;
  if (v < -eps || u + v > 1 + eps)
    return 0;
  *hit_t = coords_sp (e2, q) * inv;
  *on_edge = (u < eps || v < eps);
  if (u + v > 1 - eps) {
    *on_edge = 1;
  }
  return 1;
}

  // BVH traversal for the line origin + t*direction, t > t_min. Stores the
  // max_hits nearest hits in hits, sorted by t, with hits on shared edges and
  // vertices kept once; every hit is visited, so a return value above
  // *stored_count means hits were dropped. When hits is NULL the hits are only counted, each
  // triangle hit once, with *ambiguous = 1 when a hit is on an edge or at the
  // origin. Returns the number of hits found.
  #pragma acc routine seq
static int
gmesh_trace (gmesh_struct* mesh, Coords origin, Coords direction, double t_min, gmesh_hit* hits, int max_hits, int* stored_count, int* ambiguous) {
  int count_only = (hits == NULL);
  int stack[GMESH_BVH_STACK + 2];
  int top = 0;
  int total = 0;
  double direction_length = coords_len (direction);
  double t_tolerance;
  if (direction_length <= 0)
    return 0;
  if (!mesh->nnodes)
    return 0;
  t_tolerance = 1e-9 * mesh->length_scale / direction_length;
  stack[top] = 0;
  top++;
  while (top > 0) {
    gmesh_node* node;
    int t;
    top--;
    node = &mesh->node[stack[top]];
    if (!gmesh_slab (node->lo, node->hi, origin, direction, t_min, FLT_MAX))
      continue;
    if (node->count == 0) {
      // cannot happen: depth limited at build
      if (top + 2 > GMESH_BVH_STACK)
        continue;
      stack[top] = node->first;
      top++;
      stack[top] = node->first + 1;
      top++;
      continue;
    }
    for (t = node->first; t < node->first + node->count; t++) {
      double hit_t;
      int on_edge;
      int in_out;
      int k;
      int position;
      int duplicate = 0;
      gmesh_hit hit;
      if (!gmesh_tri_hit (mesh, t, origin, direction, &hit_t, &on_edge))
        continue;
      if (hit_t <= t_min)
        continue;
      if (coords_sp (mesh->tri_normal[t], direction) < 0) {
        in_out = 1;
      } else {
        in_out = -1;
      }
      if (count_only) {
        total++;
        if (on_edge || fabs (hit_t) <= t_tolerance)
          *ambiguous = 1;
        continue;
      }
      hit.t = hit_t;
      hit.normal = mesh->tri_normal[t];
      hit.tri = t;
      hit.face = mesh->tri_face[t];
      hit.in_out = in_out;
      hit.edge = on_edge;
      // hits on shared edges/vertices: keep once
      for (k = 0; k < *stored_count; k++) {
        if (hits[k].in_out == in_out && fabs (hits[k].t - hit_t) <= t_tolerance) {
          duplicate = 1;
          break;
        }
      }
      if (duplicate)
        continue;
      total++;
      position = *stored_count;
      if (position == max_hits) {
        if (max_hits == 0)
          continue;
        if (hit_t >= hits[max_hits - 1].t)
          continue;
        position = max_hits - 1;
      } else {
        (*stored_count)++;
      }
      while (position > 0 && hits[position - 1].t > hit_t) {
        hits[position] = hits[position - 1];
        position--;
      }
      hits[position] = hit;
    }
  }
  return total;
}

// set once a hit buffer overflow has been reported
int gmesh_overflow_warned = 0;
  #pragma acc declare create(gmesh_overflow_warned)

  // intersections of a line with mesh, see geometry-mesh-lib.h
  #pragma acc routine seq
int
gmesh_intersect (gmesh_struct* mesh, Coords position, Coords direction, double t_min, gmesh_hit hits[GMESH_MAX_HITS]) {
  int stored = 0;
  int ambiguous = 0;
  int total = gmesh_trace (mesh, position, direction, t_min, hits, GMESH_MAX_HITS, &stored, &ambiguous);
  if (total > stored && !gmesh_overflow_warned) {
    gmesh_overflow_warned = 1;
    fprintf (stderr,
             "Warning: %s: a ray crossed the mesh %d times, but only the %d nearest hits are kept "
             "(geometry-mesh-lib).\n"
             "  To keep more, add this line after the DEFINE INSTRUMENT line of your .instr file:\n"
             "    DEPENDENCY \" -DGMESH_MAX_HITS=%d \"\n"
             "  This warning is shown once.\n",
             mesh->filename, total, GMESH_MAX_HITS, total);
  }
  return stored;
}

  // 1 when point is inside the box [lower, upper], borders included
  #pragma acc routine seq
static int
gmesh_in_box (Coords point, Coords lower, Coords upper) {
  if (point.x < lower.x || point.x > upper.x)
    return 0;
  if (point.y < lower.y || point.y > upper.y)
    return 0;
  if (point.z < lower.z || point.z > upper.z)
    return 0;
  return 1;
}

  // point in (closed) mesh test, see geometry-mesh-lib.h
  #pragma acc routine seq
int
gmesh_inside (gmesh_struct* mesh, Coords position) {
  // fixed, "irrational" directions: no random numbers are consumed
  const double D[3][3] = { { 0.4719, 0.6213, 0.6254 }, { 0.8017, -0.3259, 0.5012 }, { -0.2263, 0.9147, -0.3349 } };
  int j;
  int votes = 0;
  if (!gmesh_in_box (position, mesh->bbmin, mesh->bbmax))
    return 0;
  for (j = 0; j < 3; j++) {
    int stored = 0;
    int ambiguous = 0;
    Coords direction = coords_set (D[j][0], D[j][1], D[j][2]);
    int n = gmesh_trace (mesh, position, direction, 0, NULL, 0, &stored, &ambiguous);
    if (!ambiguous)
      return (int)(n % 2);
    votes += (int)(n % 2);
  }
  return votes >= 2;
}

  // per-face property, see geometry-mesh-lib.h
  #pragma acc routine seq
double
gmesh_face_prop (gmesh_struct* mesh, int face, int property, double fallback) {
  if (!mesh || !mesh->facePropArray)
    return fallback;
  if (property < 0 || property >= mesh->nfaceprops)
    return fallback;
  if (face < 0 || face >= mesh->nfaces)
    return fallback;
  return mesh->facePropArray[(int)property * mesh->nfaces + face];
}

// ==========================================================================
// Info, display, free
// ==========================================================================

// print a summary of mesh (sizes, topology, volume)
void
gmesh_print_info (gmesh_struct* mesh) {
  printf ("  %s: %d vertices (%d duplicates welded), %d faces, %d triangles", mesh->filename, mesh->nverts, mesh->nwelded, mesh->nfaces, mesh->ntris);
  if (mesh->nduplicate)
    printf (" (%d duplicated faces removed)", mesh->nduplicate);
  if (mesh->ndegenerate)
    printf (" (%d degenerate dropped)", mesh->ndegenerate);
  printf ("\n  Bounding box [%g:%g]x[%g:%g]x[%g:%g], area %g", mesh->bbmin.x, mesh->bbmax.x, mesh->bbmin.y, mesh->bbmax.y, mesh->bbmin.z, mesh->bbmax.z,
          mesh->area);
  if (mesh->volume)
    printf (", volume %g", mesh->volume);
  printf ("\n  Topology: %d edges, Euler characteristic %d, %s, %s%s\n", mesh->nedges, mesh->euler, mesh->closed ? "closed" : "OPEN",
          mesh->oriented ? "consistently oriented" : "NOT consistently oriented", mesh->flipped ? " (orientation reversed to outward normals)" : "");
  if (!mesh->closed) {
    printf ("  Warning: %d boundary and %d non-manifold edges: the mesh does not enclose a volume.\n", mesh->nboundary, mesh->nnonmanifold);
  }
  if (mesh->nfaceprops)
    printf ("  Per-face properties: %d\n", mesh->nfaceprops);
}

// append printf-style text to the string *buffer of *capacity bytes that
// holds *length characters, growing the buffer as needed
static void
gmesh_append (char** buffer, size_t* capacity, size_t* length, const char* format, ...) {
  va_list arguments;
  int written;
  va_start (arguments, format);
  written = vsnprintf (*buffer + *length, *capacity - *length, format, arguments);
  va_end (arguments);
  if (written < 0)
    return;
  if (*length + written + 1 > *capacity) {
    while (*length + written + 1 > *capacity) {
      *capacity *= 2;
    }
    *buffer = gmesh_xrealloc (*buffer, *capacity);
    va_start (arguments, format);
    vsnprintf (*buffer + *length, *capacity - *length, format, arguments);
    va_end (arguments);
  }
  *length += written;
}

// draw the polygon edges of mesh (each once), or a polyhedron with mcdotrace==2
void
gmesh_display (gmesh_struct* mesh) {
  int i, j;
  if (!mesh || !mesh->faceSize)
    return;
  if (mcdotrace == 2) {
    // polyhedron as JSON, polygons as in the file
    size_t capacity = 4096;
    size_t length = 0;
    char* json = gmesh_xrealloc (NULL, capacity);
    json[0] = 0;
    gmesh_append (&json, &capacity, &length, "{ \"vertices\": [");
    for (i = 0; i < mesh->nverts; i++) {
      const char* separator = ", ";
      if (i == mesh->nverts - 1)
        separator = "";
      gmesh_append (&json, &capacity, &length, "[%g, %g, %g]%s", mesh->vtx[i].x, mesh->vtx[i].y, mesh->vtx[i].z, separator);
    }
    gmesh_append (&json, &capacity, &length, "], \"faces\": [");
    i = 0;
    while (i < mesh->faceSize) {
      int n = mesh->faceArray[i];
      gmesh_append (&json, &capacity, &length, "{ \"face\": [");
      for (j = 1; j <= n; j++) {
        const char* separator = ", ";
        if (j == n)
          separator = "";
        gmesh_append (&json, &capacity, &length, "%d%s", mesh->faceArray[i + j], separator);
      }
      gmesh_append (&json, &capacity, &length, "]}");
      i += n + 1;
      if (i < mesh->faceSize)
        gmesh_append (&json, &capacity, &length, ", ");
    }
    gmesh_append (&json, &capacity, &length, "]}");
    mcdis_polyhedron (json);
    free (json);
  } else {
    // each polygon edge once, a random subset when there are too many
    gmesh_etab edges;
    int ncorners = 0;
    double ratio = GMESH_DISPLAY_MAX;
    for (i = 0; i < mesh->faceSize; i += mesh->faceArray[i] + 1) {
      ncorners += mesh->faceArray[i];
    }
    gmesh_etab_alloc (&edges, ncorners);
    if (ncorners > 0)
      ratio = (double)GMESH_DISPLAY_MAX / (double)ncorners;
    for (i = 0; i < mesh->faceSize; i += mesh->faceArray[i] + 1) {
      int n = mesh->faceArray[i];
      for (j = 0; j < n; j++) {
        int a = mesh->faceArray[i + 1 + j];
        int b = mesh->faceArray[i + 1 + (j + 1) % n];
        gmesh_edge* edge;
        if (a == b)
          continue;
        edge = gmesh_edge_get (&edges, a, b);
        edge->n++;
        if (edge->n > 1)
          continue;
        if (ratio < 1 && rand01 () > ratio)
          continue;
        mcdis_line (mesh->vtx[a].x, mesh->vtx[a].y, mesh->vtx[a].z, mesh->vtx[b].x, mesh->vtx[b].y, mesh->vtx[b].z);
      }
    }
    free (edges.e);
  }
}

// release the memory held by mesh
void
gmesh_free (gmesh_struct* mesh) {
  if (!mesh)
    return;
  free (mesh->vtx);
  free (mesh->tri);
  free (mesh->tri_face);
  free (mesh->tri_normal);
  free (mesh->node);
  free (mesh->faceArray);
  free (mesh->facePropArray);
  memset (mesh, 0, sizeof (gmesh_struct));
}

#endif // GEOMETRY_MESH_LIB_C

// end of geometry-mesh-lib.c
