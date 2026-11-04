#ifndef MCSTAS_BASICS_H
#define MCSTAS_BASICS_H

#include <stdint.h>

/* The RNG state and particle types a generated instrument file provides
   per-instrument (see mccode/src/cogen.c.in:2505-2639); mirrored here just
   enough to let the real mccode-r.c link standalone. */
#define randstate_t uint64_t
#define RANDSTATE_LEN 7

struct _struct_particle {
  double x, y, z, vx, vy, vz, sx, sy, sz;
  randstate_t randstate[RANDSTATE_LEN];
  long _absorbed;
};
typedef struct _struct_particle _class_particle;

extern _class_particle *_particle;

#endif
