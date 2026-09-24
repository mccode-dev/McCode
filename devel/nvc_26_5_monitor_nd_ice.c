#include <stdlib.h>

typedef struct {
  double *buffer;
} buffer_state;

#pragma acc routine
void copy_rows(buffer_state *state)
{
  int i;

  for (i = 0; i < 4; i++) {
    #pragma acc atomic write
    state->buffer[i] = 0.0;
  }
}

int main(void)
{
  double *buffer = calloc(4, sizeof(*buffer));
  buffer_state state;

  if (!buffer)
    return 1;

  state.buffer = buffer;

  #pragma acc enter data copyin(buffer[0:4])
  #pragma acc enter data copyin(state)
  #pragma acc enter data attach(state.buffer)
  #pragma acc parallel present(state)
  {
    copy_rows(&state);
  }
  #pragma acc exit data detach(state.buffer)
  #pragma acc exit data copyout(buffer[0:4])
  #pragma acc exit data delete(state)

  for (int i = 0; i < 4; i++) {
    if (buffer[i] != 0.0)
      return 1;
  }

  free(buffer);
  return 0;
}
