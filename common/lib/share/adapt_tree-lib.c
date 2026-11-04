/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2002, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Library: share/adapt_tree-lib.c
*
* %Identification
* Written by: KN, EF
* Date:   Sep 02, 2002
* Origin: Risoe/ILL
* Release: McStas 1.6
* Version: $Revision$
*
* This file is to be imported by components handling adaptive trees, like
* Source_adapt and Adapt_check (in lib/sources)
* Embedded within instrument in runtime mode.
*
* Usage: within SHARE
* %include "adapt_tree-lib"
*
*******************************************************************************/

#ifndef ADAPT_TREE_LIB_H
#include "adapt_tree-lib.h"
#endif

/*******************************************************************************
* Find i in adaptive search tree t s.t. v(i) <= v < v(i+1).
*******************************************************************************/
int adapt_tree_search(struct adapt_tree *t, adapt_t v)
{
  adapt_t F = 0;    /* Current value. */
  int i = 0;      /* Current candidate. */
  int step = t->initstep;
  adapt_t *s = t->s;
  int j;
  for(j = t->root; step > 0; step >>= 1)
  {
    F += s[j];      /* Cumulative value in current node */
    if(v < F)
      j -= step;    /* Value is to the left or above. */
    else
      i = j, j += step;   /* Value is current or to the right. */
  }
  /* Now j is at the bottom of a tree (a leaf node). */
  if(v < F + s[j])
    return i;
  else
    return j;
}

/*******************************************************************************
* Add v to v[i], updating the cumulative sums appropriately.
*******************************************************************************/
void adapt_tree_add(struct adapt_tree *t, int i, adapt_t v)
{
  int j = t->root;
  int step = t->initstep;
  adapt_t *s = t->s;
  t->total += v;
  t->v[i++] += v;
  for(;;)
  {
    while(j < i)
      j += step, step >>= 1;
    s[j] += v;
    while(j > i)
      j -= step, step >>= 1;
    if(j == i)
      break;
    s[j] -= v;
  }
  if(step)
    s[j - step] -= v;
}

/*******************************************************************************
* Initialise an adaptive search tree. The tree has N nodes, and all nodes are
* initialized to zero. Any N > 0 is allowed, but is rounded up to the nearest
* value of the form N = 2**k - 2.
*******************************************************************************/
struct adapt_tree *
adapt_tree_init(int N)
{
  struct adapt_tree *t;
  int i;
  int depth;

  /* Round up to nearest 2**k - 2 */
  for(depth = 0; ((1 << (depth + 1)) - 2) < N; depth++);
  N = (1 << (depth + 1)) - 2;

  t = malloc(sizeof(*t));
  if(t)
  {
    t->s = malloc((N + 1) * sizeof(*(t->s)));
    t->v = malloc(N * sizeof(*(t->v)));
  }
  if(!(t && t->s && t->v))
  {
    fprintf(stderr, "Error: Out of memory (adapt_tree_init).\n");
    exit(1);
  }
  t->N = N;
  t->depth = depth;
  t->root = (1 << t->depth) - 1;
  t->initstep = (1 << (t->depth - 1));
  for(i = 0; i < t->N; i++)
  {
    t->s[i] = 0.0;
    t->v[i] = 0.0;
  }
  t->s[i] = 0.0;
  t->total = 0.0;
  return t;
}

/*******************************************************************************
* Free memory allocated to an adaptive search tree.
*******************************************************************************/
void
adapt_tree_free(struct adapt_tree *t)
{
  free(t->v);
  free(t->s);
  free(t);
}

/* end of adapt_tree-lib.c */
