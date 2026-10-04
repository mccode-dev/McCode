/*******************************************************************************
 *
 * mcvine-lib.c : implementation for mcvine-lib.h (see header for overview)
 *
 * Ported from MCViNE (https://github.com/mcvine/mcvine), mccomponents/lib.
 * Each kernel section names the MCViNE source file it follows.
 *
 *******************************************************************************/
#ifndef MCVINE_LIB_C
#define MCVINE_LIB_C

#ifndef MCVINE_LIB_H
  #include "mcvine-lib.h"
#endif

/* allocation helpers: abort the simulation on out-of-memory */
static void* mcvine_xrealloc (void* ptr, size_t size) {
  void* q = realloc (ptr, size ? size : 1);
  if (!q) {
    fprintf (stderr, "mcvine-lib: out of memory (%lu bytes)\n", (unsigned long)size);
    free (ptr);
    exit (-1);
  }
  return q;
}
#define mcvine_xmalloc(size) mcvine_xrealloc (NULL, (size))

/* ======================================================================== */
/*  expression evaluator                                                    */
/* ======================================================================== */
enum {
  MCVX_CONST = 1,
  MCVX_VAR,
  MCVX_NEG,
  MCVX_NOT,
  MCVX_ADD,
  MCVX_SUB,
  MCVX_MUL,
  MCVX_DIV,
  MCVX_MOD,
  MCVX_POW,
  MCVX_LT,
  MCVX_GT,
  MCVX_LE,
  MCVX_GE,
  MCVX_EQ,
  MCVX_NE,
  MCVX_AND,
  MCVX_OR,
  MCVX_FN1,
  MCVX_FN2,
  MCVX_IF
};
static const char* mcvx_fn1_names[] = { "sin",  "cos", "tan",   "asin", "acos", "atan", "sinh", "cosh", "tanh", "exp",  "log",  "log10", "log2",
                                        "sqrt", "abs", "floor", "ceil", "int",  "sign", "cot",  "sec",  "csc",  "exp2", "cbrt", "trunc", NULL };
static const char* mcvx_fn2_names[] = { "pow", "atan2", "min", "max", "hypot", "fmod", NULL };

typedef struct {
  const char* s;
  int pos;
  int err;
  char msg[256];
  mcvine_expr* e;
  int nvars;
  const char** vars;
} mcvx_parser;

static void
mcvx_emit (mcvx_parser* P, int op, double val) {
  mcvine_expr* e = P->e;
  if (e->n >= e->cap) {
    e->cap = e->cap ? 2 * e->cap : 64;
    e->op = (int*)mcvine_xrealloc (e->op, e->cap * sizeof (int));
    e->val = (double*)mcvine_xrealloc (e->val, e->cap * sizeof (double));
  }
  e->op[e->n] = op;
  e->val[e->n] = val;
  e->n++;
}
static void
mcvx_skip (mcvx_parser* P) {
  while (P->s[P->pos] == ' ' || P->s[P->pos] == '\t' || P->s[P->pos] == '\n' || P->s[P->pos] == '\r')
    P->pos++;
}
static int
mcvx_peek (mcvx_parser* P) {
  mcvx_skip (P);
  return P->s[P->pos];
}
static void
mcvx_fail (mcvx_parser* P, const char* m) {
  if (!P->err) {
    P->err = 1;
    snprintf (P->msg, sizeof (P->msg), "%s at position %d", m, P->pos);
  }
}
static void mcvx_or (mcvx_parser* P);
static void mcvx_unary (mcvx_parser* P);

static void
mcvx_primary (mcvx_parser* P) {
  int c = mcvx_peek (P);
  if (P->err)
    return;
  if (c == '(') {
    P->pos++;
    mcvx_or (P);
    if (mcvx_peek (P) != ')') {
      mcvx_fail (P, "expected ')'");
      return;
    }
    P->pos++;
    return;
  }
  if ((c >= '0' && c <= '9') || c == '.') {
    char* end;
    double v = strtod (P->s + P->pos, &end);
    if (end == P->s + P->pos) {
      mcvx_fail (P, "bad number");
      return;
    }
    P->pos = end - P->s;
    mcvx_emit (P, MCVX_CONST, v);
    return;
  }
  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
    char name[64];
    int n = 0, i;
    while (n < 63
           && ((P->s[P->pos] >= 'a' && P->s[P->pos] <= 'z') || (P->s[P->pos] >= 'A' && P->s[P->pos] <= 'Z') || (P->s[P->pos] >= '0' && P->s[P->pos] <= '9')
               || P->s[P->pos] == '_'))
      name[n++] = P->s[P->pos++];
    name[n] = 0;
    for (i = 0; i < P->nvars; i++)
      if (!strcmp (name, P->vars[i])) {
        mcvx_emit (P, MCVX_VAR, i);
        return;
      }
    if (mcvx_peek (P) == '(') {
      int nargs = 0;
      P->pos++;
      if (mcvx_peek (P) != ')') {
        for (;;) {
          mcvx_or (P);
          nargs++;
          if (P->err)
            return;
          if (mcvx_peek (P) == ',') {
            P->pos++;
            continue;
          }
          break;
        }
      }
      if (mcvx_peek (P) != ')') {
        mcvx_fail (P, "expected ')' after function arguments");
        return;
      }
      P->pos++;
      if (!strcmp (name, "if")) {
        if (nargs != 3)
          mcvx_fail (P, "if() needs 3 arguments");
        else
          mcvx_emit (P, MCVX_IF, 0);
        return;
      }
      for (i = 0; mcvx_fn1_names[i]; i++)
        if (!strcmp (name, mcvx_fn1_names[i])) {
          if (nargs != 1)
            mcvx_fail (P, "function needs 1 argument");
          else
            mcvx_emit (P, MCVX_FN1, i);
          return;
        }
      for (i = 0; mcvx_fn2_names[i]; i++)
        if (!strcmp (name, mcvx_fn2_names[i])) {
          if (nargs != 2)
            mcvx_fail (P, "function needs 2 arguments");
          else
            mcvx_emit (P, MCVX_FN2, i);
          return;
        }
      mcvx_fail (P, "unknown function");
      return;
    }
    if (!strcmp (name, "pi")) {
      mcvx_emit (P, MCVX_CONST, MCVINE_PI);
      return;
    }
    if (!strcmp (name, "e")) {
      mcvx_emit (P, MCVX_CONST, exp (1.0));
      return;
    }
    snprintf (P->msg, sizeof (P->msg), "unknown variable '%s' at position %d", name, P->pos);
    P->err = 1;
    return;
  }
  mcvx_fail (P, "unexpected character");
}
static void
mcvx_power (mcvx_parser* P) {
  mcvx_primary (P);
  if (P->err)
    return;
  if (mcvx_peek (P) == '^' || (P->s[P->pos] == '*' && P->s[P->pos + 1] == '*')) {
    P->pos += (P->s[P->pos] == '^') ? 1 : 2;
    mcvx_unary (P);
    mcvx_emit (P, MCVX_POW, 0);
  }
}
static void
mcvx_unary (mcvx_parser* P) {
  int c = mcvx_peek (P);
  if (c == '-') {
    P->pos++;
    mcvx_unary (P);
    mcvx_emit (P, MCVX_NEG, 0);
    return;
  }
  if (c == '+') {
    P->pos++;
    mcvx_unary (P);
    return;
  }
  if (c == '!') {
    P->pos++;
    mcvx_unary (P);
    mcvx_emit (P, MCVX_NOT, 0);
    return;
  }
  mcvx_power (P);
}
static void
mcvx_prod (mcvx_parser* P) {
  mcvx_unary (P);
  for (;;) {
    int c = mcvx_peek (P);
    if (P->err)
      return;
    if (c == '*' && P->s[P->pos + 1] != '*') {
      P->pos++;
      mcvx_unary (P);
      mcvx_emit (P, MCVX_MUL, 0);
    } else if (c == '/') {
      P->pos++;
      mcvx_unary (P);
      mcvx_emit (P, MCVX_DIV, 0);
    } else if (c == '%') {
      P->pos++;
      mcvx_unary (P);
      mcvx_emit (P, MCVX_MOD, 0);
    } else
      return;
  }
}
static void
mcvx_sum (mcvx_parser* P) {
  mcvx_prod (P);
  for (;;) {
    int c = mcvx_peek (P);
    if (P->err)
      return;
    if (c == '+') {
      P->pos++;
      mcvx_prod (P);
      mcvx_emit (P, MCVX_ADD, 0);
    } else if (c == '-') {
      P->pos++;
      mcvx_prod (P);
      mcvx_emit (P, MCVX_SUB, 0);
    } else
      return;
  }
}
static void
mcvx_cmp (mcvx_parser* P) {
  mcvx_sum (P);
  for (;;) {
    int c = mcvx_peek (P), c2 = P->s[P->pos + 1];
    if (P->err)
      return;
    if (c == '<' && c2 == '=') {
      P->pos += 2;
      mcvx_sum (P);
      mcvx_emit (P, MCVX_LE, 0);
    } else if (c == '>' && c2 == '=') {
      P->pos += 2;
      mcvx_sum (P);
      mcvx_emit (P, MCVX_GE, 0);
    } else if (c == '!' && c2 == '=') {
      P->pos += 2;
      mcvx_sum (P);
      mcvx_emit (P, MCVX_NE, 0);
    } else if (c == '=') {
      P->pos += (c2 == '=') ? 2 : 1;
      mcvx_sum (P);
      mcvx_emit (P, MCVX_EQ, 0);
    } else if (c == '<') {
      P->pos++;
      mcvx_sum (P);
      mcvx_emit (P, MCVX_LT, 0);
    } else if (c == '>') {
      P->pos++;
      mcvx_sum (P);
      mcvx_emit (P, MCVX_GT, 0);
    } else
      return;
  }
}
static void
mcvx_and (mcvx_parser* P) {
  mcvx_cmp (P);
  while (!P->err && mcvx_peek (P) == '&') {
    P->pos++;
    if (P->s[P->pos] == '&')
      P->pos++;
    mcvx_cmp (P);
    mcvx_emit (P, MCVX_AND, 0);
  }
}
static void
mcvx_or (mcvx_parser* P) {
  mcvx_and (P);
  while (!P->err && mcvx_peek (P) == '|') {
    P->pos++;
    if (P->s[P->pos] == '|')
      P->pos++;
    mcvx_and (P);
    mcvx_emit (P, MCVX_OR, 0);
  }
}

int
mcvine_expr_compile (mcvine_expr* e, const char* src, int nvars, const char** varnames, const char* owner) {
  mcvx_parser P;
  int i, depth = 0, maxdepth = 0;
  memset (e, 0, sizeof (*e));
  if (!src || !src[0]) {
    fprintf (stderr, "%s: empty expression\n", owner);
    return 1;
  }
  strncpy (e->text, src, sizeof (e->text) - 1);
  memset (&P, 0, sizeof (P));
  P.s = src;
  P.e = e;
  P.nvars = nvars;
  P.vars = varnames;
  mcvx_or (&P);
  if (!P.err && mcvx_peek (&P) != 0)
    mcvx_fail (&P, "trailing characters");
  if (P.err) {
    fprintf (stderr, "%s: cannot parse expression \"%s\": %s\n  (allowed variables:", owner, src, P.msg);
    for (i = 0; i < nvars; i++)
      fprintf (stderr, " %s", varnames[i]);
    fprintf (stderr, ")\n");
    return 1;
  }
  for (i = 0; i < e->n; i++) {
    int op = e->op[i];
    if (op == MCVX_CONST || op == MCVX_VAR)
      depth++;
    else if (op == MCVX_NEG || op == MCVX_NOT || op == MCVX_FN1)
      ;
    else if (op == MCVX_IF)
      depth -= 2;
    else
      depth--;
    if (depth > maxdepth)
      maxdepth = depth;
  }
  if (maxdepth > MCVINE_EXPR_MAXSTACK) {
    fprintf (stderr, "%s: expression too deep\n", owner);
    return 1;
  }
  return 0;
}

void
mcvine_expr_free (mcvine_expr* e) {
  free (e->op);
  free (e->val);
  e->op = NULL;
  e->val = NULL;
  e->n = e->cap = 0;
}

double
mcvine_expr_eval (const mcvine_expr* e, const double* vars) {
  double st[MCVINE_EXPR_MAXSTACK];
  int sp = 0, i;
  for (i = 0; i < e->n; i++) {
    double a, b;
    switch (e->op[i]) {
    case MCVX_CONST:
      st[sp++] = e->val[i];
      break;
    case MCVX_VAR:
      st[sp++] = vars[(int)e->val[i]];
      break;
    case MCVX_NEG:
      st[sp - 1] = -st[sp - 1];
      break;
    case MCVX_NOT:
      st[sp - 1] = (st[sp - 1] == 0);
      break;
    case MCVX_FN1:
      a = st[sp - 1];
      switch ((int)e->val[i]) {
      case 0:
        a = sin (a);
        break;
      case 1:
        a = cos (a);
        break;
      case 2:
        a = tan (a);
        break;
      case 3:
        a = asin (a);
        break;
      case 4:
        a = acos (a);
        break;
      case 5:
        a = atan (a);
        break;
      case 6:
        a = sinh (a);
        break;
      case 7:
        a = cosh (a);
        break;
      case 8:
        a = tanh (a);
        break;
      case 9:
        a = exp (a);
        break;
      case 10:
        a = log (a);
        break;
      case 11:
        a = log10 (a);
        break;
      case 12:
        a = log (a) / log (2.0);
        break;
      case 13:
        a = sqrt (a);
        break;
      case 14:
        a = fabs (a);
        break;
      case 15:
        a = floor (a);
        break;
      case 16:
        a = ceil (a);
        break;
      case 17:
        a = floor (a + 0.5);
        break;
      case 18:
        a = (a > 0) - (a < 0);
        break;
      case 19:
        a = 1 / tan (a);
        break;
      case 20:
        a = 1 / cos (a);
        break;
      case 21:
        a = 1 / sin (a);
        break;
      case 22:
        a = pow (2.0, a);
        break;
      case 23:
        a = cbrt (a);
        break;
      case 24:
        a = trunc (a);
        break;
      }
      st[sp - 1] = a;
      break;
    case MCVX_FN2:
      b = st[--sp];
      a = st[sp - 1];
      switch ((int)e->val[i]) {
      case 0:
        a = pow (a, b);
        break;
      case 1:
        a = atan2 (a, b);
        break;
      case 2:
        a = a < b ? a : b;
        break;
      case 3:
        a = a > b ? a : b;
        break;
      case 4:
        a = hypot (a, b);
        break;
      case 5:
        a = fmod (a, b);
        break;
      }
      st[sp - 1] = a;
      break;
    case MCVX_IF: {
      double c3 = st[--sp], c2 = st[--sp];
      st[sp - 1] = (st[sp - 1] != 0) ? c2 : c3;
    } break;
    default:
      b = st[--sp];
      a = st[sp - 1];
      switch (e->op[i]) {
      case MCVX_ADD:
        a = a + b;
        break;
      case MCVX_SUB:
        a = a - b;
        break;
      case MCVX_MUL:
        a = a * b;
        break;
      case MCVX_DIV:
        a = a / b;
        break;
      case MCVX_MOD:
        a = fmod (a, b);
        break;
      case MCVX_POW:
        a = pow (a, b);
        break;
      case MCVX_LT:
        a = a < b;
        break;
      case MCVX_GT:
        a = a > b;
        break;
      case MCVX_LE:
        a = a <= b;
        break;
      case MCVX_GE:
        a = a >= b;
        break;
      case MCVX_EQ:
        a = a == b;
        break;
      case MCVX_NE:
        a = a != b;
        break;
      case MCVX_AND:
        a = (a != 0) && (b != 0);
        break;
      case MCVX_OR:
        a = (a != 0) || (b != 0);
        break;
      }
      st[sp - 1] = a;
    }
  }
  return sp > 0 ? st[sp - 1] : 0;
}

/* ======================================================================== */
/*  files and grids                                                         */
/* ======================================================================== */
FILE*
mcvine_fopen (const char* name, const char* mode) {
  /* Use the McStas file search (current dir, instrument dir, $MCSTAS/data,
     $MCSTAS/contrib) from read_table-lib when it is available. */
  char buf[4096];
  if (!name || !name[0])
    return NULL;
  strncpy (buf, name, sizeof (buf) - 1);
  buf[sizeof (buf) - 1] = 0;
#ifdef READ_TABLE_LIB_H
  return Open_File (buf, mode, NULL);
#else
  {
    FILE* f = fopen (buf, mode);
    const char* env;
    char path[8192];
    if (f)
      return f;
    env = getenv ("MCSTAS");
    if (env) {
      snprintf (path, sizeof (path), "%s/data/%s", env, buf);
      f = fopen (path, mode);
      if (f)
        return f;
    }
    return NULL;
  }
#endif
}

int
mcvine_read_rows (const char* file, mcvine_rows* r, const char* owner) {
  FILE* f = mcvine_fopen (file, "r");
  size_t cap = 1 << 16;
  char* line;
  int rowcap = 0;
  memset (r, 0, sizeof (*r));
  if (!f) {
    fprintf (stderr, "%s: cannot open file '%s'\n", owner, file);
    return 1;
  }
  line = (char*)mcvine_xmalloc (cap);
  for (;;) {
    size_t len = 0;
    int c;
    char *s, *end;
    int n = 0, ncap = 16;
    double* vals;
    while ((c = fgetc (f)) != EOF && c != '\n') {
      if (len + 2 >= cap) {
        cap *= 2;
        line = (char*)mcvine_xrealloc (line, cap);
      }
      line[len++] = (char)c;
    }
    line[len] = 0;
    if (c == EOF && len == 0)
      break;
    s = line;
    while (*s == ' ' || *s == '\t' || *s == '\r')
      s++;
    if (*s == '#' || *s == 0 || *s == '%') {
      if (c == EOF)
        break;
      continue;
    }
    for (end = s; *end; end++)
      if (*end == ',' || *end == ';' || *end == '[' || *end == ']')
        *end = ' ';
    vals = (double*)mcvine_xmalloc (ncap * sizeof (double));
    for (;;) {
      double v = strtod (s, &end);
      if (end == s)
        break;
      if (n >= ncap) {
        ncap *= 2;
        vals = (double*)mcvine_xrealloc (vals, ncap * sizeof (double));
      }
      vals[n++] = v;
      s = end;
    }
    if (n > 0) {
      if (r->nrows >= rowcap) {
        rowcap = rowcap ? 2 * rowcap : 64;
        r->rows = (double**)mcvine_xrealloc (r->rows, rowcap * sizeof (double*));
        r->ncols = (int*)mcvine_xrealloc (r->ncols, rowcap * sizeof (int));
      }
      r->rows[r->nrows] = vals;
      r->ncols[r->nrows] = n;
      r->nrows++;
    } else
      free (vals);
    if (c == EOF)
      break;
  }
  free (line);
  fclose (f);
  if (r->nrows == 0) {
    fprintf (stderr, "%s: no numeric data in '%s'\n", owner, file);
    return 1;
  }
  return 0;
}

void
mcvine_free_rows (mcvine_rows* r) {
  int i;
  for (i = 0; i < r->nrows; i++)
    free (r->rows[i]);
  free (r->rows);
  free (r->ncols);
  memset (r, 0, sizeof (*r));
}

/* flatten rows [first, nrows) into one array; returns count */
static int
mcvine_rows_flatten (const mcvine_rows* r, int first, double** out) {
  int i, j, n = 0, k = 0;
  for (i = first; i < r->nrows; i++)
    n += r->ncols[i];
  *out = (double*)mcvine_xmalloc ((n > 0 ? n : 1) * sizeof (double));
  for (i = first; i < r->nrows; i++)
    for (j = 0; j < r->ncols[i]; j++)
      (*out)[k++] = r->rows[i][j];
  return n;
}

/* index i with x[i] <= v <= x[i+1]; -1 if outside */
static int
mcvine_bracket (const double* x, int n, double v, double* frac) {
  int lo = 0, hi = n - 1;
  if (n < 2 || v < x[0] || v > x[n - 1] || v != v)
    return -1;
  while (hi - lo > 1) {
    int m = (lo + hi) / 2;
    if (x[m] <= v)
      lo = m;
    else
      hi = m;
  }
  *frac = (x[hi] > x[lo]) ? (v - x[lo]) / (x[hi] - x[lo]) : 0;
  return lo;
}

int
mcvine_table1d_load (mcvine_table1d* t, const char* file, const char* owner) {
  mcvine_rows r;
  int i;
  if (mcvine_read_rows (file, &r, owner))
    return 1;
  t->n = 0;
  t->x = (double*)mcvine_xmalloc (r.nrows * sizeof (double));
  t->y = (double*)mcvine_xmalloc (r.nrows * sizeof (double));
  for (i = 0; i < r.nrows; i++)
    if (r.ncols[i] >= 2) {
      t->x[t->n] = r.rows[i][0];
      t->y[t->n] = r.rows[i][1];
      t->n++;
    }
  mcvine_free_rows (&r);
  for (i = 1; i < t->n; i++)
    if (t->x[i] <= t->x[i - 1]) {
      fprintf (stderr, "%s: first column of '%s' must be strictly ascending\n", owner, file);
      return 1;
    }
  if (t->n < 2) {
    fprintf (stderr, "%s: '%s' needs at least 2 rows with 2 columns\n", owner, file);
    return 1;
  }
  return 0;
}
double
mcvine_table1d_eval (const mcvine_table1d* t, double x) {
  double f;
  int i = mcvine_bracket (t->x, t->n, x, &f);
  if (i < 0)
    return 0;
  return t->y[i] * (1 - f) + t->y[i + 1] * f;
}

double
mcvine_grid2d_eval (const mcvine_grid2d* g, double x, double y) {
  double fx, fy;
  int i = mcvine_bracket (g->x, g->nx, x, &fx), j = mcvine_bracket (g->y, g->ny, y, &fy);
  const double* f = g->f;
  int ny = g->ny;
  if (i < 0 || j < 0)
    return 0;
  return (1 - fx) * (1 - fy) * f[i * ny + j] + fx * (1 - fy) * f[(i + 1) * ny + j] + (1 - fx) * fy * f[i * ny + j + 1] + fx * fy * f[(i + 1) * ny + j + 1];
}
static int
mcvine_axis_ok (const double* x, int n, const char* what, const char* file, const char* owner) {
  int i;
  if (n < 2) {
    fprintf (stderr, "%s: %s axis in '%s' needs >= 2 values\n", owner, what, file);
    return 0;
  }
  for (i = 1; i < n; i++)
    if (x[i] <= x[i - 1]) {
      fprintf (stderr, "%s: %s axis in '%s' must be ascending\n", owner, what, file);
      return 0;
    }
  return 1;
}
int
mcvine_grid2d_load_sqw (mcvine_grid2d* g, const char* file, const char* owner) {
  mcvine_rows r;
  double* flat;
  int n, i;
  if (mcvine_read_rows (file, &r, owner))
    return 1;
  if (r.nrows < 3) {
    fprintf (stderr, "%s: '%s': expected q row, w row and S matrix\n", owner, file);
    return 1;
  }
  g->nx = r.ncols[0];
  g->ny = r.ncols[1];
  g->x = (double*)mcvine_xmalloc (g->nx * sizeof (double));
  g->y = (double*)mcvine_xmalloc (g->ny * sizeof (double));
  memcpy (g->x, r.rows[0], g->nx * sizeof (double));
  memcpy (g->y, r.rows[1], g->ny * sizeof (double));
  n = mcvine_rows_flatten (&r, 2, &flat);
  mcvine_free_rows (&r);
  if (!mcvine_axis_ok (g->x, g->nx, "q", file, owner) || !mcvine_axis_ok (g->y, g->ny, "w", file, owner))
    return 1;
  if (n != g->nx * g->ny) {
    fprintf (stderr, "%s: '%s': S matrix has %d values, expected nq*nw=%d\n", owner, file, n, g->nx * g->ny);
    return 1;
  }
  g->f = flat; /* already q-major: f[iq*nw + iw] */
  for (i = 0; i < n; i++)
    if (g->f[i] != g->f[i])
      g->f[i] = 0;
  return 0;
}
int
mcvine_grid2d_load_image (mcvine_grid2d* g, const char* file, const char* owner) {
  mcvine_rows r;
  double* flat;
  int n, ix, iy;
  if (mcvine_read_rows (file, &r, owner))
    return 1;
  if (r.nrows < 3) {
    fprintf (stderr, "%s: '%s': expected x row, y row and data\n", owner, file);
    return 1;
  }
  g->nx = r.ncols[0];
  g->ny = r.ncols[1];
  g->x = (double*)mcvine_xmalloc (g->nx * sizeof (double));
  g->y = (double*)mcvine_xmalloc (g->ny * sizeof (double));
  memcpy (g->x, r.rows[0], g->nx * sizeof (double));
  memcpy (g->y, r.rows[1], g->ny * sizeof (double));
  n = mcvine_rows_flatten (&r, 2, &flat);
  mcvine_free_rows (&r);
  if (!mcvine_axis_ok (g->x, g->nx, "x", file, owner) || !mcvine_axis_ok (g->y, g->ny, "y", file, owner))
    return 1;
  if (n != g->nx * g->ny) {
    fprintf (stderr, "%s: '%s': data has %d values, expected nx*ny=%d\n", owner, file, n, g->nx * g->ny);
    return 1;
  }
  g->f = (double*)mcvine_xmalloc (n * sizeof (double));
  for (iy = 0; iy < g->ny; iy++)
    for (ix = 0; ix < g->nx; ix++)
      g->f[ix * g->ny + iy] = flat[iy * g->nx + ix];
  free (flat);
  return 0;
}

int
mcvine_grid3d_load (mcvine_grid3d* g, const char* file, const char* owner) {
  mcvine_rows r;
  int i, n;
  if (mcvine_read_rows (file, &r, owner))
    return 1;
  if (r.nrows < 4) {
    fprintf (stderr, "%s: '%s': expected 3 axis rows (min max n) + data\n", owner, file);
    return 1;
  }
  for (i = 0; i < 3; i++) {
    if (r.ncols[i] < 3) {
      fprintf (stderr, "%s: '%s': axis row %d must be 'min max n'\n", owner, file, i + 1);
      return 1;
    }
    g->min[i] = r.rows[i][0];
    g->max[i] = r.rows[i][1];
    g->n[i] = (int)(r.rows[i][2] + 0.5);
    if (g->n[i] < 2 || g->max[i] <= g->min[i]) {
      fprintf (stderr, "%s: '%s': bad axis %d\n", owner, file, i + 1);
      return 1;
    }
    g->step[i] = (g->max[i] - g->min[i]) / (g->n[i] - 1);
  }
  n = mcvine_rows_flatten (&r, 3, &g->f);
  mcvine_free_rows (&r);
  if (n != g->n[0] * g->n[1] * g->n[2]) {
    fprintf (stderr, "%s: '%s': %d data values, expected %d\n", owner, file, n, g->n[0] * g->n[1] * g->n[2]);
    return 1;
  }
  return 0;
}
double
mcvine_grid3d_eval (const mcvine_grid3d* g, double x, double y, double z) {
  double v[3] = { x, y, z }, fr[3];
  int id[3], k, n1 = g->n[1], n2 = g->n[2];
  double c00, c01, c10, c11, c0, c1;
  const double* f = g->f;
  for (k = 0; k < 3; k++) {
    double rr = (v[k] - g->min[k]) / g->step[k];
    if (rr < 0 || rr > g->n[k] - 1 || rr != rr)
      return 0;
    id[k] = (int)floor (rr);
    if (id[k] >= g->n[k] - 1)
      id[k] = g->n[k] - 2;
    fr[k] = rr - id[k];
  }
#define MCV_G3(i, j, l) f[((i) * n1 + (j)) * n2 + (l)]
  c00 = MCV_G3 (id[0], id[1], id[2]) * (1 - fr[0]) + MCV_G3 (id[0] + 1, id[1], id[2]) * fr[0];
  c01 = MCV_G3 (id[0], id[1], id[2] + 1) * (1 - fr[0]) + MCV_G3 (id[0] + 1, id[1], id[2] + 1) * fr[0];
  c10 = MCV_G3 (id[0], id[1] + 1, id[2]) * (1 - fr[0]) + MCV_G3 (id[0] + 1, id[1] + 1, id[2]) * fr[0];
  c11 = MCV_G3 (id[0], id[1] + 1, id[2] + 1) * (1 - fr[0]) + MCV_G3 (id[0] + 1, id[1] + 1, id[2] + 1) * fr[0];
#undef MCV_G3
  c0 = c00 * (1 - fr[1]) + c10 * fr[1];
  c1 = c01 * (1 - fr[1]) + c11 * fr[1];
  return c0 * (1 - fr[2]) + c1 * fr[2];
}

double
mcvine_func_eval (const mcvine_func* f, const double* v) {
  switch (f->mode) {
  case 0:
    return f->c;
  case 1:
    return mcvine_expr_eval (&f->expr, v);
  case 2:
    return mcvine_table1d_eval (&f->t1, v[0]);
  case 3:
    return mcvine_grid2d_eval (&f->g2, v[0], v[1]);
  case 4:
    return mcvine_grid3d_eval (&f->g3, v[0], v[1], v[2]);
  }
  return 0;
}

int
mcvine_func_setup (mcvine_func* f, const char* expr, const char* file, int filetype, int nvars, const char** vars, const char* owner) {
  memset (f, 0, sizeof (*f));
  if (file && file[0] && strcmp (file, "NULL") && strcmp (file, "0")) {
    f->mode = filetype;
    if (filetype == 2)
      return mcvine_table1d_load (&f->t1, file, owner);
    if (filetype == 3)
      return mcvine_grid2d_load_sqw (&f->g2, file, owner);
    if (filetype == 4)
      return mcvine_grid3d_load (&f->g3, file, owner);
    fprintf (stderr, "%s: internal error, bad file type\n", owner);
    return 1;
  }
  if (!expr || !expr[0]) {
    fprintf (stderr, "%s: need an expression or a data file\n", owner);
    return 1;
  }
  {
    char* end;
    double c = strtod (expr, &end);
    while (end && (*end == ' ' || *end == '\t'))
      end++;
    if (end != expr && end && *end == 0) {
      f->mode = 0;
      f->c = c;
      return 0;
    }
  }
  f->mode = 1;
  return mcvine_expr_compile (&f->expr, expr, nvars, vars, owner);
}

/* ======================================================================== */
/*  vectors                                                                 */
/* ======================================================================== */
double
mcvine_len3 (const double* a) {
  return sqrt (a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
}
void
mcvine_cross3 (const double* a, const double* b, double* c) {
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
}
void
mcvine_frame (const double* v, double* e1, double* e2, double* e3) {
  double l = mcvine_len3 (v);
  e1[0] = v[0] / l;
  e1[1] = v[1] / l;
  e1[2] = v[2] / l;
  if (fabs (e1[0]) > 1e-4 || fabs (e1[1]) > 1e-4) {
    double z[3] = { 0, 0, 1 }, m;
    mcvine_cross3 (z, e1, e2);
    m = mcvine_len3 (e2);
    e2[0] /= m;
    e2[1] /= m;
    e2[2] /= m;
  } else {
    e2[0] = 1;
    e2[1] = 0;
    e2[2] = 0;
  }
  mcvine_cross3 (e1, e2, e3);
}
void
mcvine_dir_from_frame (const double* e1, const double* e2, const double* e3, double cost, double sint, double phi, double* dir) {
  double c = cos (phi), s = sin (phi);
  int i;
  for (i = 0; i < 3; i++)
    dir[i] = sint * c * e2[i] + sint * s * e3[i] + cost * e1[i];
}
static void
mcvine_random_direction (double* d, _class_particle* _particle) {
  double cost = 2 * rand01 () - 1, sint = sqrt (1 - cost * cost), phi = 2 * MCVINE_PI * rand01 ();
  d[0] = sint * cos (phi);
  d[1] = sint * sin (phi);
  d[2] = cost;
}
static double
mcvine_E2k (double E) {
  return E > 0 ? V2K * SE2V * sqrt (E) : 0;
}
static double
mcvine_k2E (double k) {
  return VS2E * (K2V * k) * (K2V * k);
}

/* Ridders' method (mccomponents/math/rootfinding.cc, zridd) */
typedef double (*mcvine_f1) (double, void*);
static int
mcvine_zridd (mcvine_f1 f, void* ctx, double x1, double x2, double xacc, double* root) {
  double fl = f (x1, ctx), fh = f (x2, ctx), ans, xl, xh, xm, fm, s, xnew, fnew;
  int j;
  if (fl * fh >= 0) {
    if (fl == 0) {
      *root = x1;
      return 1;
    }
    if (fh == 0) {
      *root = x2;
      return 1;
    }
    return 0;
  }
  xl = x1;
  xh = x2;
  ans = -1.11e30;
  for (j = 1; j < 60; j++) {
    xm = 0.5 * (xl + xh);
    fm = f (xm, ctx);
    s = sqrt (fm * fm - fl * fh);
    if (s == 0.0)
      break;
    xnew = xm + (xm - xl) * ((fl >= fh ? 1.0 : -1.0) * fm / s);
    if (fabs (xnew - ans) <= xacc) {
      ans = xnew;
      break;
    }
    ans = xnew;
    fnew = f (ans, ctx);
    if (fnew == 0.0)
      break;
    if ((fnew >= 0 ? fabs (fm) : -fabs (fm)) != fm) {
      xl = xm;
      fl = fm;
      xh = ans;
      fh = fnew;
    } else if ((fnew >= 0 ? fabs (fl) : -fabs (fl)) != fl) {
      xh = ans;
      fh = fnew;
    } else if ((fnew >= 0 ? fabs (fh) : -fabs (fh)) != fh) {
      xl = ans;
      fl = fnew;
    } else
      return 0;
    if (fabs (xh - xl) <= xacc)
      break;
  }
  if (ans == -1.11e30)
    return 0;
  *root = ans;
  return 1;
}
/* FindRootsEvenly: split [x1,x2] into nsteps and bracket each piece */
static int
mcvine_find_roots (mcvine_f1 f, void* ctx, double x1, double x2, int nsteps, double xacc, double* roots, int maxroots) {
  int i, n = 0;
  double step = (x2 - x1) / nsteps;
  for (i = 0; i < nsteps && n < maxroots; i++) {
    double r;
    if (mcvine_zridd (f, ctx, x1 + step * i, x1 + step * (i + 1), xacc, &r))
      roots[n++] = r;
  }
  return n;
}

/* ======================================================================== */
/*  shapes and transport (mccomponents/lib/homogeneous_scatterer)           */
/* ======================================================================== */
int
mcvine_shape_init (mcvine_shape* s, double radius, double xwidth, double yheight, double zdepth, double thickness, const char* owner) {
  memset (s, 0, sizeof (*s));
  s->radius = radius;
  s->xwidth = xwidth;
  s->yheight = yheight;
  s->zdepth = zdepth;
  s->thickness = thickness;
  if (xwidth > 0 && yheight > 0 && zdepth > 0)
    s->type = MCVINE_SHAPE_BOX;
  else if (radius > 0 && yheight > 0)
    s->type = MCVINE_SHAPE_CYLINDER;
  else if (radius > 0)
    s->type = MCVINE_SHAPE_SPHERE;
  else {
    fprintf (stderr, "%s: specify a box (xwidth,yheight,zdepth), a cylinder (radius,yheight[,thickness]) or a sphere (radius)\n", owner);
    return 1;
  }
  if (s->type == MCVINE_SHAPE_CYLINDER && thickness >= radius) {
    fprintf (stderr, "%s: thickness must be smaller than radius\n", owner);
    return 1;
  }
  return 0;
}

int
mcvine_shape_segments (const mcvine_shape* s, double x, double y, double z, double vx, double vy, double vz, double* seg) {
  double t0 = 0, t1 = 0, t2 = 0, t3 = 0, raw[4];
  int n = 0, i, k = 0, hit = 0;
  switch (s->type) {
  case MCVINE_SHAPE_BOX:
    hit = box_intersect (&t0, &t3, x, y, z, vx, vy, vz, s->xwidth, s->yheight, s->zdepth);
    break;
  case MCVINE_SHAPE_CYLINDER:
    hit = cylinder_intersect (&t0, &t3, x, y, z, vx, vy, vz, s->radius, s->yheight);
    break;
  case MCVINE_SHAPE_SPHERE:
    hit = sphere_intersect (&t0, &t3, x, y, z, vx, vy, vz, s->radius);
    break;
  }
  if (!hit)
    return 0;
  if (s->type == MCVINE_SHAPE_CYLINDER && s->thickness > 0 && cylinder_intersect (&t1, &t2, x, y, z, vx, vy, vz, s->radius - s->thickness, s->yheight)
      && t2 > t1) {
    raw[0] = t0;
    raw[1] = t1;
    raw[2] = t2;
    raw[3] = t3;
    n = 2;
  } else {
    raw[0] = t0;
    raw[1] = t3;
    n = 1;
  }
  for (i = 0; i < n; i++) {
    double a = raw[2 * i], b = raw[2 * i + 1];
    if (a < 0)
      a = 0;
    if (b > a + 1e-15) {
      seg[2 * k] = a;
      seg[2 * k + 1] = b;
      k++;
    }
  }
  return k;
}

double
mcvine_xs2coeff (double xs_barn, double V_AA3) {
  return V_AA3 > 0 ? xs_barn / V_AA3 * 100.0 : 0;
}

void
mcvine_scatterer_init (mcvine_scatterer* sc, const mcvine_shape* s, double mu2200, double sigma, double pack, double p_transmit, int order) {
  sc->shape = *s;
  sc->mu2200 = mu2200;
  sc->sigma = sigma;
  sc->pack = pack > 0 ? pack : 1;
  sc->p_transmit = (p_transmit > 0 && p_transmit < 1) ? p_transmit : 0;
  sc->order = order > 0 ? order : 1;
}

int
mcvine_scatterer_interact (const mcvine_scatterer* sc, void* kernel, mcvine_S_fn S, _class_particle* _particle) {
  double seg[4], r[3], vel[3];
  int nseg, k, nscat = 0;
  for (k = 0;; k++) {
    double v, T, L, mu, sig, tot, s, w, dt, acc;
    int i;
    nseg = mcvine_shape_segments (&sc->shape, _particle->x, _particle->y, _particle->z, _particle->vx, _particle->vy, _particle->vz, seg);
    if (!nseg)
      break;
    v = sqrt (_particle->vx * _particle->vx + _particle->vy * _particle->vy + _particle->vz * _particle->vz);
    if (v <= 0)
      return -1;
    T = 0;
    for (i = 0; i < nseg; i++)
      T += seg[2 * i + 1] - seg[2 * i];
    L = T * v;
    mu = sc->mu2200 * 2200.0 / v * sc->pack;
    sig = sc->sigma * sc->pack;
    tot = mu + sig;
    if (k == 0) {
      /* first passage: MCViNE interact_path1 (uniform depth, forced scattering) */
      if (sc->p_transmit > 0 && rand01 () < sc->p_transmit) {
        _particle->p *= exp (-tot * L) / sc->p_transmit;
        return 0;
      }
      s = rand01 () * L;
      w = L * exp (-tot * s) * sig;
      if (sc->p_transmit > 0)
        w /= (1 - sc->p_transmit);
    } else {
      double Pint;
      if (k >= sc->order) {
        _particle->p *= exp (-tot * L);
        break;
      } /* leave, attenuated */
      Pint = 1 - exp (-tot * L);
      if (!(tot > 0) || rand01 () >= Pint)
        break; /* escapes unscattered */
      s = -log (1 - rand01 () * Pint) / tot;
      w = sig / tot;
    }
    /* move to the interaction point at path length s inside the material */
    dt = s / v;
    acc = 0;
    for (i = 0; i < nseg; i++) {
      double d = seg[2 * i + 1] - seg[2 * i];
      if (dt <= acc + d || i == nseg - 1) {
        dt = seg[2 * i] + (dt - acc);
        break;
      }
      acc += d;
    }
    _particle->x += _particle->vx * dt;
    _particle->y += _particle->vy * dt;
    _particle->z += _particle->vz * dt;
    _particle->t += dt;
    _particle->p *= w;
    r[0] = _particle->x;
    r[1] = _particle->y;
    r[2] = _particle->z;
    vel[0] = _particle->vx;
    vel[1] = _particle->vy;
    vel[2] = _particle->vz;
    if (!S (kernel, r, vel, _particle->t, &_particle->p, _particle))
      return -1;
    if (!(_particle->p > 0) || vel[0] != vel[0] || vel[1] != vel[1] || vel[2] != vel[2])
      return -1;
    _particle->vx = vel[0];
    _particle->vy = vel[1];
    _particle->vz = vel[2];
    nscat++;
  }
  return nscat;
}

/* ======================================================================== */
/*  simple kernels                                                          */
/* ======================================================================== */
/* ConstantEnergyTransferKernel.cc */
int
mcvine_S_ConstantEnergyTransfer (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_ConstantEnergyTransfer* k = (mcvine_kernel_ConstantEnergyTransfer*)kk;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, Ef = Ei - k->m_E, vf, d[3];
  if (Ef < 0)
    return 0;
  vf = SE2V * sqrt (Ef);
  mcvine_random_direction (d, _particle);
  v[0] = d[0] * vf;
  v[1] = d[1] * vf;
  v[2] = d[2] * vf;
  return 1;
}

/* ConstantQEKernel.cc */
int
mcvine_S_ConstantQE (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_ConstantQE* k = (mcvine_kernel_ConstantQE*)kk;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, Ef = Ei - k->m_E, vf, ki, kf, cost, sint, e1[3], e2[3], e3[3], d[3];
  if (Ef <= 0)
    return 0;
  vf = SE2V * sqrt (Ef);
  ki = V2K * vi;
  kf = V2K * vf;
  cost = (ki * ki + kf * kf - k->m_Q * k->m_Q) / (2 * ki * kf);
  if (cost * cost > 1)
    return 0;
  sint = sqrt (1 - cost * cost);
  mcvine_frame (v, e1, e2, e3);
  mcvine_dir_from_frame (e1, e2, e3, cost, sint, 2 * MCVINE_PI * rand01 (), d);
  v[0] = d[0] * vf;
  v[1] = d[1] * vf;
  v[2] = d[2] * vf;
  return 1;
}

/* ConstantvQEKernel.cc */
int
mcvine_S_ConstantvQE (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_ConstantvQE* k = (mcvine_kernel_ConstantvQE*)kk;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, Ef, vf, E, x;
  int i;
  for (i = 0; i < 3; i++)
    v[i] = (v[i] * V2K - k->m_Q[i]) * K2V;
  vf = mcvine_len3 (v);
  Ef = VS2E * vf * vf;
  E = Ei - Ef;
  x = (E - k->m_E) / k->m_dE;
  *p *= exp (-x * x / 2);
  return 1;
}

/* E_Q_Kernel.icc : S(Q,E) = S(Q) delta(E - E(Q)) */
int
mcvine_S_E_Q (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_E_Q* k = (mcvine_kernel_E_Q*)kk;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, Q = 0, E, Ef, vf = 0, ki = V2K * vi, kf = 0, cost = 0, cost2 = 0, sint;
  double e1[3], e2[3], e3[3], d[3];
  int counter = 0, ok = 0, maxtries = k->m_unbiased ? 1 : 100;
  /* MCViNE retries up to 100 times and divides the weight by the number of
     attempts; that is slightly biased (E[1/n] != acceptance). unbiased=1:
     a single attempt, forbidden events get weight 0. */
  while (counter++ < maxtries) {
    Q = k->m_Qmin + (k->m_Qmax - k->m_Qmin) * rand01 ();
    E = mcvine_func_eval (&k->m_E_Q, &Q);
    Ef = Ei - E;
    if (Ef < 0)
      continue;
    vf = SE2V * sqrt (Ef);
    kf = V2K * vf;
    cost = (ki * ki + kf * kf - Q * Q) / (2 * ki * kf);
    cost2 = cost * cost;
    if (cost2 > 1)
      continue;
    ok = 1;
    break;
  }
  if (!ok)
    return 0;
  sint = sqrt (1 - cost2);
  mcvine_frame (v, e1, e2, e3);
  mcvine_dir_from_frame (e1, e2, e3, cost, sint, 2 * MCVINE_PI * rand01 (), d);
  *p *= mcvine_func_eval (&k->m_S_Q, &Q) * (vf / vi);
  *p *= Q * (k->m_Qmax - k->m_Qmin) / counter / (kf * ki) / 2;
  v[0] = d[0] * vf;
  v[1] = d[1] * vf;
  v[2] = d[2] * vf;
  return 1;
}

/* Broadened_E_Q_Kernel.icc / LorentzianBroadened_E_Q_Kernel.icc */
void
mcvine_Broadened_E_Q_init (mcvine_kernel_Broadened_E_Q* k) {
  int i, N = 100;
  double dQ = (k->m_Qmax - k->m_Qmin) / N;
  k->m_Emin = 1e300;
  k->m_Emax = -1e300;
  for (i = 0; i < N; i++) {
    double Q = k->m_Qmin + dQ * i, E = mcvine_func_eval (&k->m_E_Q, &Q), w = mcvine_func_eval (&k->m_W_Q, &Q);
    if (E - 3 * w < k->m_Emin)
      k->m_Emin = E - 3 * w;
    if (E + 3 * w > k->m_Emax)
      k->m_Emax = E + 3 * w;
  }
}
int
mcvine_S_Broadened_E_Q (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_Broadened_E_Q* k = (mcvine_kernel_Broadened_E_Q*)kk;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, Q = 0, E, Ef = 0, vf = 0, ki = V2K * vi, kf = 0, cost = 0, cost2 = 0, sint;
  double e1[3], e2[3], e3[3], d[3];
  int count = 0, ok = 0, maxtries = k->m_unbiased ? 1 : 99;
  if (Ei < k->m_Emin)
    return 0;
  /* MCViNE retries (at most 99 successful tries) without correcting the weight
     for the acceptance probability; unbiased=1 uses a single attempt. */
  while (count++ < maxtries) {
    double dE;
    Q = k->m_Qmin + (k->m_Qmax - k->m_Qmin) * rand01 ();
    if (k->m_lorentzian)
      dE = tan (MCVINE_PI * (rand01 () - 0.5)) * mcvine_func_eval (&k->m_W_Q, &Q);
    else {
      double x1, x2, w; /* polar Box-Muller, as mccomponents/math/random/gaussian.cc */
      do {
        x1 = 2 * rand01 () - 1;
        x2 = 2 * rand01 () - 1;
        w = x1 * x1 + x2 * x2;
      } while (w >= 1 || w == 0);
      dE = x1 * sqrt (-2 * log (w) / w) * mcvine_func_eval (&k->m_W_Q, &Q);
    }
    E = mcvine_func_eval (&k->m_E_Q, &Q) + dE;
    Ef = Ei - E;
    if (Ef <= 0)
      continue;
    vf = SE2V * sqrt (Ef);
    kf = V2K * vf;
    cost = (ki * ki + kf * kf - Q * Q) / (2 * ki * kf);
    cost2 = cost * cost;
    if (cost2 > 1)
      continue;
    ok = 1;
    break;
  }
  if (!ok)
    return 0;
  sint = sqrt (1 - cost2);
  mcvine_frame (v, e1, e2, e3);
  mcvine_dir_from_frame (e1, e2, e3, cost, sint, 2 * MCVINE_PI * rand01 (), d);
  *p *= mcvine_func_eval (&k->m_S_Q, &Q) * (vf / vi);
  *p /= 4 * MCVINE_PI;
  *p *= Q * (k->m_Qmax - k->m_Qmin) / (kf * ki) * 2 * MCVINE_PI;
  v[0] = d[0] * vf;
  v[1] = d[1] * vf;
  v[2] = d[2] * vf;
  return 1;
}

/* E_vQ_Kernel.icc : S(vQ,E) = S(vQ) delta(E - E(vQ)) */
typedef struct {
  const mcvine_func* E;
  double ukf[3], ki[3], Ei;
} mcvine_evq_ctx;
static double
mcvine_evq_f (double kf, void* c) {
  mcvine_evq_ctx* x = (mcvine_evq_ctx*)c;
  double Q[3];
  int i;
  for (i = 0; i < 3; i++)
    Q[i] = x->ki[i] - x->ukf[i] * kf;
  return x->Ei - mcvine_k2E (kf) - mcvine_func_eval (x->E, Q);
}
int
mcvine_S_E_vQ (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_E_vQ* k = (mcvine_kernel_E_vQ*)kk;
  mcvine_evq_ctx c;
  double roots[1024], vi = mcvine_len3 (v), kmag, kf, dkf, df, E, Q[3];
  int i, nkf, idx;
  c.E = &k->m_E_Q;
  c.Ei = VS2E * vi * vi;
  for (i = 0; i < 3; i++)
    c.ki[i] = V2K * v[i];
  kmag = V2K * vi;
  mcvine_random_direction (c.ukf, _particle);
  nkf = mcvine_find_roots (mcvine_evq_f, &c, mcvine_E2k (c.Ei - k->m_Emax > 0 ? c.Ei - k->m_Emax : 0), mcvine_E2k (c.Ei), k->m_nsteps, k->m_xacc, roots, 1024);
  if (nkf < 1)
    return 0;
  idx = nkf > 1 ? (int)floor (rand01 () * nkf) : 0;
  if (idx >= nkf)
    idx = nkf - 1;
  kf = roots[idx];
  *p *= nkf;
  dkf = kmag / 200;
  df = fabs (-mcvine_evq_f (kf + 2 * dkf, &c) + 8 * mcvine_evq_f (kf + dkf, &c) - 8 * mcvine_evq_f (kf - dkf, &c) + mcvine_evq_f (kf - 2 * dkf, &c)) / 12 / dkf;
  if (!(df > 0))
    return 0;
  *p *= 2 * MCVINE_KSQ2E * kf / df;
  for (i = 0; i < 3; i++)
    Q[i] = c.ki[i] - c.ukf[i] * kf;
  E = c.Ei - mcvine_k2E (kf);
  if (fabs (E - mcvine_func_eval (&k->m_E_Q, Q)) > k->m_Emax * 1e-5)
    return 0;
  for (i = 0; i < 3; i++)
    v[i] = c.ukf[i] * kf * K2V;
  *p *= mcvine_func_eval (&k->m_S_Q, Q) * kf / kmag;
  return 1;
}

/* SQkernel.cc : elastic, isotropic S(|Q|) */
int
mcvine_S_SQ (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_SQ* k = (mcvine_kernel_SQ*)kk;
  double vi = mcvine_len3 (v), ki = V2K * vi, kf = ki, Qmin, Qmax, Q, cost, sint, e1[3], e2[3], e3[3], d[3];
  Qmin = k->m_Qmin > 0 ? k->m_Qmin : 0;
  Qmax = k->m_Qmax < ki + kf ? k->m_Qmax : ki + kf;
  if (Qmax < Qmin)
    return 0;
  Q = Qmin + (Qmax - Qmin) * rand01 ();
  *p *= mcvine_func_eval (&k->m_S, &Q) * Q * (Qmax - Qmin) / (2 * ki * ki);
  cost = (kf * kf + ki * ki - Q * Q) / 2 / kf / ki;
  if (cost > 1)
    cost = 1;
  if (cost < -1)
    cost = -1;
  sint = sqrt (1 - cost * cost);
  mcvine_frame (v, e1, e2, e3);
  mcvine_dir_from_frame (e1, e2, e3, cost, sint, 2 * MCVINE_PI * rand01 (), d);
  v[0] = d[0] * vi;
  v[1] = d[1] * vi;
  v[2] = d[2] * vi;
  return 1;
}

/* SvQkernel.cc : elastic, S(vector Q) */
int
mcvine_S_SvQ (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_SvQ* k = (mcvine_kernel_SvQ*)kk;
  double vi = mcvine_len3 (v), d[3], Q[3];
  int i;
  mcvine_random_direction (d, _particle);
  for (i = 0; i < 3; i++)
    Q[i] = V2K * (v[i] - d[i] * vi);
  *p *= mcvine_func_eval (&k->m_S, Q);
  for (i = 0; i < 3; i++)
    v[i] = d[i] * vi;
  return 1;
}

/* SQEkernel.cc and SQE_EnergyFocusing_Kernel.cc */
int
mcvine_S_SQE (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_SQE* k = (mcvine_kernel_SQE*)kk;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, ki = V2K * vi, Emin, Emax, E, Ef, kf, Qmin, Qmax, Q, cost, sint, qe[2];
  double e1[3], e2[3], e3[3], d[3];
  if (k->m_Emin > Ei)
    return 0;
  if (k->m_focusing) {
    double Emin1 = Ei - (k->m_Ef + k->m_dEf), Emax1 = Ei - (k->m_Ef - k->m_dEf);
    Emin = k->m_Emin > Emin1 ? k->m_Emin : Emin1;
    Emax = Emax1 < Ei ? Emax1 : Ei;
    if (k->m_Emax < Emax)
      Emax = k->m_Emax;
  } else {
    Emin = k->m_Emin;
    Emax = k->m_Emax < Ei ? k->m_Emax : Ei;
  }
  if (!(Emax > Emin))
    return 0;
  E = Emin + (Emax - Emin) * rand01 ();
  Ef = Ei - E;
  kf = mcvine_E2k (Ef);
  Qmin = fabs (ki - kf);
  if (k->m_Qmin > Qmin)
    Qmin = k->m_Qmin;
  Qmax = ki + kf;
  if (k->m_Qmax < Qmax)
    Qmax = k->m_Qmax;
  if (Qmax < Qmin)
    return 0;
  Q = Qmin + (Qmax - Qmin) * rand01 ();
  qe[0] = Q;
  qe[1] = E;
  *p *= mcvine_func_eval (&k->m_S, qe) * Q * (Qmax - Qmin) * (Emax - Emin) / (2 * ki * ki);
  cost = (kf * kf + ki * ki - Q * Q) / 2 / kf / ki;
  if (cost > 1)
    cost = 1;
  if (cost < -1)
    cost = -1;
  sint = sqrt (1 - cost * cost);
  mcvine_frame (v, e1, e2, e3);
  mcvine_dir_from_frame (e1, e2, e3, cost, sint, 2 * MCVINE_PI * rand01 (), d);
  v[0] = d[0] * kf * K2V;
  v[1] = d[1] * kf * K2V;
  v[2] = d[2] * kf * K2V;
  return 1;
}

/* DGSSXResKernel.cc : resolution kernel, aim at a target disk and TOF window.
   mcvine_DGSSXRes_final: given a unit final direction, the solid angle it was
   sampled from and the distance L to the target, set the final velocity.  */
int
mcvine_DGSSXRes_final (const mcvine_kernel_DGSSXRes* k, const double* dir, double solid_angle, double L, double* v, double t, double* p,
                       _class_particle* _particle) {
  double tof, vf, Ef, dEdt, vi = mcvine_len3 (v);
  tof = k->m_tof_at_target + (rand01 () - 0.5) * k->m_dtof;
  if (tof - t <= 0)
    return 0;
  vf = L / (tof - t);
  Ef = VS2E * vf * vf;
  dEdt = 2 * Ef / tof;
  *p *= solid_angle / 4 / MCVINE_PI * k->m_dtof * dEdt * vf / vi;
  v[0] = dir[0] * vf;
  v[1] = dir[1] * vf;
  v[2] = dir[2] * vf;
  return 1;
}
int
mcvine_S_DGSSXRes (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_DGSSXRes* k = (mcvine_kernel_DGSSXRes*)kk;
  double disp[3], d[3], solid_angle = 0, n;
  int i;
  for (i = 0; i < 3; i++)
    disp[i] = k->m_target[i] - r[i];
  randvec_target_circle (&d[0], &d[1], &d[2], &solid_angle, disp[0], disp[1], disp[2], k->m_target_radius);
  n = mcvine_len3 (d);
  for (i = 0; i < 3; i++)
    d[i] /= n;
  return mcvine_DGSSXRes_final (k, d, solid_angle, mcvine_len3 (disp), v, t, p, _particle);
}

/* mcvine/acc kernels/SANS2D_ongrid.py : S(Qx,Qy) on a grid, beam along z */
int
mcvine_S_SANS2D_ongrid (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_SANS2D_ongrid* k = (mcvine_kernel_SANS2D_ongrid*)kk;
  double vi2 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2], Qx, Qy, vz2;
  Qx = k->m_Qx_min + (k->m_Qx_max - k->m_Qx_min) * rand01 ();
  Qy = k->m_Qy_min + (k->m_Qy_max - k->m_Qy_min) * rand01 ();
  v[0] -= K2V * Qx;
  v[1] -= K2V * Qy;
  vz2 = vi2 - v[0] * v[0] - v[1] * v[1];
  if (vz2 <= 0)
    return 0;
  v[2] = sqrt (vz2);
  *p *= mcvine_grid2d_eval (&k->m_S, Qx, Qy);
  return 1;
}

/* ======================================================================== */
/*  phonon support                                                          */
/* ======================================================================== */
double
mcvine_bose (double E, double T) {
  return 1.0 / (exp (fabs (E) / (T * MCVINE_KELVIN2MEV)) - 1);
}
double
mcvine_phonon_bose_factor (double E, double T) { /* phonon/utils.cc */
  if (E == 0)
    return 1;
  return (E > 0 ? 1.0 : 0.0) + mcvine_bose (E, T);
}

/* regression y = c x ; returns R^2 (phonon/utils.py linear_regression) */
static double
mcvine_linreg0 (const double* x, const double* y, int n, double* c) {
  double xy = 0, xx = 0, ys = 0, ave, tot = 0, err = 0;
  int i;
  for (i = 0; i < n; i++) {
    xy += x[i] * y[i];
    xx += x[i] * x[i];
    ys += y[i];
  }
  *c = xx > 0 ? xy / xx : 0;
  ave = ys / n;
  for (i = 0; i < n; i++) {
    tot += (y[i] - ave) * (y[i] - ave);
    err += (y[i] - *c * x[i]) * (y[i] - *c * x[i]);
  }
  return tot > 0 ? 1 - err / tot : 1;
}
/* fitparabolic() from phonon/utils.py; returns 0 when fit is bad */
static int
mcvine_fitparabolic (const double* E, double* g, int n, int force, const char* owner) {
  int N = 100, minN = 20, i, bad = 1;
  double c = 0, R2, *x;
  if (N > n)
    N = n;
  x = (double*)mcvine_xmalloc (n * sizeof (double));
  for (i = 0; i < n; i++)
    x[i] = E[i] * E[i];
  while (N > minN) {
    R2 = mcvine_linreg0 (x, g, N, &c);
    if (R2 < 0.9)
      N--;
    else {
      bad = 0;
      break;
    }
  }
  if (bad) {
    if (!force) {
      free (x);
      return 0;
    }
    fprintf (stderr, "%s: warning: unable to fit DOS to parabolic (forced)\n", owner);
  }
  for (i = 0; i < N && i < n; i++)
    g[i] = c * x[i];
  free (x);
  return 1;
}
/* smooth(x, 21, 'hanning') from phonon/utils.py */
static void
mcvine_smooth21 (double* x, int n) {
  const int w = 21;
  int i, m, ns = n + 2 * w - 2;
  double *s, *y, win[21], sum = 0;
  if (n < w)
    return;
  s = (double*)mcvine_xmalloc (ns * sizeof (double));
  y = (double*)mcvine_xmalloc (n * sizeof (double));
  for (m = 0; m < w; m++) {
    win[m] = 0.5 - 0.5 * cos (2 * MCVINE_PI * m / (w - 1));
    sum += win[m];
  }
  for (i = 0; i < w - 1; i++)
    s[i] = x[w - 1 - i];
  for (i = 0; i < n; i++)
    s[w - 1 + i] = x[i];
  for (i = 0; i < w - 1; i++)
    s[w - 1 + n + i] = x[n - 1 - i];
  for (i = 0; i < n; i++) {
    double acc = 0;
    int j = i + (w / 2 - 1);
    for (m = 0; m < w; m++)
      acc += win[m] / sum * s[j + m];
    y[i] = acc;
  }
  memcpy (x, y, n * sizeof (double));
  free (s);
  free (y);
}

static int
mcvine_dos_from_arrays (mcvine_dos* d, const double* E0, const double* g0, int n0, const char* owner) {
  int i, n, uniform = 1;
  double *E, *g, area = 0, c, R2, x[20], y[20];
  for (i = 2; i < n0; i++)
    if (fabs ((E0[i] - E0[i - 1]) - (E0[1] - E0[0])) > 1e-6 * fabs (E0[1] - E0[0]) + 1e-12)
      uniform = 0;
  if (n0 < 500 || !uniform) {
    double dE = E0[n0 - 1] / 500.0;
    n = 500;
    E = (double*)mcvine_xmalloc (n * sizeof (double));
    g = (double*)mcvine_xmalloc (n * sizeof (double));
    for (i = 0; i < n; i++) { /* np.interp semantics */
      double e = i * dE, f;
      int j;
      E[i] = e;
      if (e <= E0[0])
        g[i] = g0[0];
      else if (e >= E0[n0 - 1])
        g[i] = g0[n0 - 1];
      else {
        j = mcvine_bracket (E0, n0, e, &f);
        g[i] = g0[j] * (1 - f) + g0[j + 1] * f;
      }
    }
  } else {
    n = n0;
    E = (double*)mcvine_xmalloc (n * sizeof (double));
    g = (double*)mcvine_xmalloc (n * sizeof (double));
    memcpy (E, E0, n * sizeof (double));
    memcpy (g, g0, n * sizeof (double));
  }
  if (!mcvine_fitparabolic (E, g, n, 0, owner)) {
    mcvine_smooth21 (g, n);
    g[0] = 0;
    mcvine_fitparabolic (E, g, n, 1, owner);
  }
  d->n = n;
  d->e0 = E[0];
  d->de = E[1] - E[0];
  d->emax = d->e0 + d->de * (n - 1);
  for (i = 0; i < n; i++)
    area += g[i];
  area *= d->de;
  if (!(area > 0)) {
    fprintf (stderr, "%s: DOS has zero area\n", owner);
    free (E);
    free (g);
    return 1;
  }
  for (i = 0; i < n; i++)
    g[i] /= area;
  d->Z = g;
  free (E);
  /* LinearlyInterpolatedDOS::_compute_sod : fit first 20 points to c*E^2 */
  for (i = 0; i < 20 && i < n; i++) {
    x[i] = (d->e0 + d->de * i) * (d->e0 + d->de * i);
    y[i] = d->Z[i];
  }
  R2 = mcvine_linreg0 (x, y, i, &c);
  if (R2 < 0.9)
    fprintf (stderr, "%s: warning: DOS low-E part is not parabolic (R2=%g)\n", owner, R2);
  d->sod = c;
  return 0;
}

static int
mcvine_idf_header (FILE* f, char* filetype, int* D, int* Nb, int* Nq) {
  char ft[65], comment[1024];
  int version, v[3];
  if (fread (ft, 1, 64, f) != 64)
    return 1;
  ft[64] = 0;
  if (fread (&version, sizeof (int), 1, f) != 1)
    return 1;
  if (fread (comment, 1, 1024, f) != 1024)
    return 1;
  strcpy (filetype, ft);
  if (D) {
    if (fread (v, sizeof (int), 3, f) != 3)
      return 1;
    *D = v[0];
    *Nb = v[1];
    *Nq = v[2];
  }
  return 0;
}

int
mcvine_dos_load (mcvine_dos* d, const char* file, int ascii_THz, const char* owner) {
  FILE* f = mcvine_fopen (file, "rb");
  char magic[4] = { 0, 0, 0, 0 };
  double *E, *g;
  int n, i, ret;
  memset (d, 0, sizeof (*d));
  if (!f) {
    fprintf (stderr, "%s: cannot open DOS file '%s'\n", owner, file);
    return 1;
  }
  if (fread (magic, 1, 3, f) == 3 && !strncmp (magic, "DOS", 3)) {
    char ft[65];
    double dE;
    rewind (f);
    if (mcvine_idf_header (f, ft, NULL, NULL, NULL) || fread (&n, sizeof (int), 1, f) != 1 || fread (&dE, sizeof (double), 1, f) != 1) {
      fprintf (stderr, "%s: corrupt IDF DOS file '%s'\n", owner, file);
      fclose (f);
      return 1;
    }
    E = (double*)mcvine_xmalloc (n * sizeof (double));
    g = (double*)mcvine_xmalloc (n * sizeof (double));
    if ((int)fread (g, sizeof (double), n, f) != n) {
      fprintf (stderr, "%s: corrupt IDF DOS file '%s'\n", owner, file);
      fclose (f);
      free (E);
      free (g);
      return 1;
    }
    for (i = 0; i < n; i++)
      E[i] = i * dE * 2 * MCVINE_PI * 1e12 * MCVINE_HERTZ2MEV; /* THz -> meV */
    fclose (f);
  } else {
    mcvine_rows r;
    int thz = ascii_THz;
    char line[4096];
    rewind (f);
    while (fgets (line, sizeof (line), f))
      if (line[0] == '#' && (strstr (line, "TeraHz") || strstr (line, "THz")))
        thz = 1;
    fclose (f);
    if (mcvine_read_rows (file, &r, owner))
      return 1;
    E = (double*)mcvine_xmalloc (r.nrows * sizeof (double));
    g = (double*)mcvine_xmalloc (r.nrows * sizeof (double));
    n = 0;
    for (i = 0; i < r.nrows; i++)
      if (r.ncols[i] >= 2) {
        E[n] = r.rows[i][0] * (thz ? 2 * MCVINE_PI * 1e12 * MCVINE_HERTZ2MEV : 1);
        g[n] = r.rows[i][1];
        n++;
      }
    mcvine_free_rows (&r);
  }
  if (n < 3) {
    fprintf (stderr, "%s: DOS '%s' has too few points\n", owner, file);
    free (E);
    free (g);
    return 1;
  }
  for (i = 1; i < n; i++)
    if (E[i] <= E[i - 1]) {
      fprintf (stderr, "%s: DOS energies must be ascending\n", owner);
      free (E);
      free (g);
      return 1;
    }
  if (E[0] < 0) {
    fprintf (stderr, "%s: DOS energies must be >= 0\n", owner);
    free (E);
    free (g);
    return 1;
  }
  ret = mcvine_dos_from_arrays (d, E, g, n, owner);
  free (E);
  free (g);
  return ret;
}

double
mcvine_dos_value (const mcvine_dos* d, double E) {
  double r;
  int i;
  if (E < d->e0 || E >= d->emax)
    return 0;
  r = (E - d->e0) / d->de;
  i = (int)floor (r);
  if (i >= d->n - 1)
    i = d->n - 2;
  r -= i;
  return d->Z[i] * (1 - r) + d->Z[i + 1] * r;
}

/* phonon/DWFromDOS.icc : 2W = core * Q^2 */
double
mcvine_dw_core_from_dos (const mcvine_dos* d, double mass, double T, int ns) {
  double core = 0, wmin = d->e0, wmax = d->emax, dw = (wmax - wmin) / (ns - 1 + .00000001), *f, f0;
  int i, first = -1;
  f = (double*)calloc (ns, sizeof (double));
  for (i = 0; i < ns; i++) {
    double w = dw * i + wmin, Z = mcvine_dos_value (d, w), frac = 1;
    if (w < wmax / ns / 100.)
      continue;
    if (first == -1)
      first = i;
    f[i] = (2 * mcvine_bose (w, T) + 1) / w * Z;
    if (i == ns - 1)
      frac = 0.5;
    core += f[i] * frac;
  }
  if (first < 0 || first + 1 >= ns) {
    free (f);
    return 0;
  }
  f0 = f[first] - (dw * first + wmin) * (f[first + 1] - f[first]) / dw;
  core += f0 / 2;
  free (f);
  core /= MCVINE_ECHARGE * 1e-3;
  core *= dw;
  core *= MCVINE_HBAR * MCVINE_HBAR / 2 / MCVINE_AMU / mass;
  core *= 1e20;
  return core;
}

int
mcvine_atoms_load (mcvine_atom** atoms, const char* file, const char* owner) {
  mcvine_rows r;
  int i, n = 0;
  if (mcvine_read_rows (file, &r, owner))
    return -1;
  *atoms = (mcvine_atom*)calloc (r.nrows, sizeof (mcvine_atom));
  for (i = 0; i < r.nrows; i++) {
    mcvine_atom* a;
    if (r.ncols[i] < 7) {
      fprintf (stderr, "%s: atoms file '%s' row %d: need x y z mass b_coh sigma_inc sigma_abs\n", owner, file, i + 1);
      mcvine_free_rows (&r);
      return -1;
    }
    a = &(*atoms)[n++];
    a->pos[0] = r.rows[i][0];
    a->pos[1] = r.rows[i][1];
    a->pos[2] = r.rows[i][2];
    a->mass = r.rows[i][3];
    a->b_coh = r.rows[i][4];
    a->xs_inc = r.rows[i][5];
    a->xs_abs = r.rows[i][6];
    a->xs_coh = 4 * MCVINE_PI * a->b_coh * a->b_coh / 100.0; /* fm^2 -> barn */
  }
  mcvine_free_rows (&r);
  return n;
}

/* ---- IDF dispersion ---- */
static int
mcvine_parse_qgridinfo (mcvine_dispersion* d, const char* path, const char* owner) {
  FILE* f = mcvine_fopen (path, "r");
  char line[4096];
  int got = 0;
  if (!f) {
    fprintf (stderr, "%s: cannot open '%s'\n", owner, path);
    return 1;
  }
  while (fgets (line, sizeof (line), f)) {
    char *eq = strchr (line, '='), name[16] = { 0 }, *s, *q;
    int ni = 0;
    if (!eq || line[0] == '#')
      continue;
    for (s = line; s < eq && ni < 15; s++)
      if (*s != ' ' && *s != '\t')
        name[ni++] = *s;
    /* strip python decorations: array(, numpy., np., brackets */
    for (s = eq + 1; *s; s++)
      if (*s == '[' || *s == ']' || *s == '\n' || *s == '\r')
        *s = ' ';
    while ((q = strstr (eq + 1, "numpy.array")))
      memset (q, ' ', 11);
    while ((q = strstr (eq + 1, "np.array")))
      memset (q, ' ', 8);
    while ((q = strstr (eq + 1, "array")))
      memset (q, ' ', 5);
    while ((q = strstr (eq + 1, "numpy.float64")))
      memset (q, ' ', 13);
    while ((q = strstr (eq + 1, "np.float64")))
      memset (q, ' ', 10);
    while ((q = strstr (eq + 1, "float")))
      memset (q, ' ', 5);
    if (name[0] == 'b' && name[1] >= '1' && name[1] <= '3' && !name[2]) {
      int idx = name[1] - '1', c = 0;
      char buf[4096];
      strncpy (buf, eq + 1, sizeof (buf) - 1);
      buf[sizeof (buf) - 1] = 0;
      /* split on commas at parenthesis depth 0; if there is a single
         parenthesised token, e.g. "(x, y, z)", unwrap it and split again */
      {
        int pass;
        for (pass = 0; pass < 2; pass++) {
          char* parts[3];
          int np_ = 0, depth = 0;
          char* start = buf;
          for (s = buf;; s++) {
            if (*s == '(')
              depth++;
            else if (*s == ')')
              depth--;
            if ((*s == ',' && depth == 0) || *s == 0) {
              int end = (*s == 0);
              *s = 0;
              if (np_ < 3)
                parts[np_] = start;
              np_++;
              start = s + 1;
              if (end)
                break;
            }
          }
          if (np_ == 1 && pass == 0) {
            char *a0 = parts[0], *z;
            while (*a0 == ' ')
              a0++;
            z = a0 + strlen (a0) - 1;
            while (z > a0 && *z == ' ')
              z--;
            if (*a0 == '(' && *z == ')') {
              *a0 = ' ';
              *z = 0;
              memmove (buf, a0, strlen (a0) + 1);
              continue;
            }
          }
          for (c = 0; c < np_ && c < 3; c++) {
            mcvine_expr ex;
            if (mcvine_expr_compile (&ex, parts[c], 0, NULL, owner)) {
              fclose (f);
              return 1;
            }
            d->b[idx][c] = mcvine_expr_eval (&ex, NULL);
            mcvine_expr_free (&ex);
          }
          if (np_ != 3)
            c = np_;
          break;
        }
      }
      if (c != 3) {
        fprintf (stderr, "%s: '%s': %s needs 3 components\n", owner, path, name);
        fclose (f);
        return 1;
      }
      got |= 1 << idx;
    } else if (name[0] == 'n' && name[1] >= '1' && name[1] <= '3' && !name[2]) {
      mcvine_expr ex;
      if (mcvine_expr_compile (&ex, eq + 1, 0, NULL, owner)) {
        fclose (f);
        return 1;
      }
      d->n[name[1] - '1'] = (int)(mcvine_expr_eval (&ex, NULL) + 0.5);
      mcvine_expr_free (&ex);
      got |= 8 << (name[1] - '1');
    }
  }
  fclose (f);
  if (got != 63) {
    fprintf (stderr, "%s: '%s' must define b1,b2,b3,n1,n2,n3\n", owner, path);
    return 1;
  }
  return 0;
}

static void
mcvine_inv3 (double m[3][3], double inv[3][3]) {
  double det = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
               + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
  inv[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]) / det;
  inv[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) / det;
  inv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) / det;
  inv[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) / det;
  inv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) / det;
  inv[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) / det;
  inv[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]) / det;
  inv[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) / det;
  inv[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]) / det;
}

int
mcvine_dispersion_load_idf (mcvine_dispersion* d, const char* dir, const char* owner) {
  char path[4096], ft[65];
  FILE* f;
  int D, Nb, Nq, Nq2, D2, Nb2, i, br, nq, nbr;
  double inv[3][3], det;
  size_t neps;
  memset (d, 0, sizeof (*d));
  snprintf (path, sizeof (path), "%s/Qgridinfo", dir);
  if (mcvine_parse_qgridinfo (d, path, owner))
    return 1;
  mcvine_inv3 (d->b, inv);
  for (i = 0; i < 3; i++) {
    d->a[i][0] = inv[0][i];
    d->a[i][1] = inv[1][i];
    d->a[i][2] = inv[2][i];
  }
  det = d->b[0][0] * (d->b[1][1] * d->b[2][2] - d->b[1][2] * d->b[2][1]) - d->b[0][1] * (d->b[1][0] * d->b[2][2] - d->b[1][2] * d->b[2][0])
        + d->b[0][2] * (d->b[1][0] * d->b[2][1] - d->b[1][1] * d->b[2][0]);
  d->ucvol = pow (2 * MCVINE_PI, 3) / fabs (det);
  /* Omega2 */
  snprintf (path, sizeof (path), "%s/Omega2", dir);
  f = mcvine_fopen (path, "rb");
  if (!f || mcvine_idf_header (f, ft, &D, &Nb, &Nq) || strncmp (ft, "Omega2", 6)) {
    fprintf (stderr, "%s: cannot read IDF '%s'\n", owner, path);
    if (f)
      fclose (f);
    return 1;
  }
  nq = d->n[0] * d->n[1] * d->n[2];
  if (D != 3 || Nq != nq) {
    fprintf (stderr, "%s: '%s': D=%d, N_q=%d but Qgridinfo gives %d points\n", owner, path, D, Nq, nq);
    fclose (f);
    return 1;
  }
  d->natoms = Nb;
  d->nbranches = nbr = Nb * D;
  d->E = (double*)mcvine_xmalloc ((size_t)nq * nbr * sizeof (double));
  if (fread (d->E, sizeof (double), (size_t)nq * nbr, f) != (size_t)nq * nbr) {
    fprintf (stderr, "%s: '%s' truncated\n", owner, path);
    fclose (f);
    return 1;
  }
  fclose (f);
  for (i = 0; i < nq * nbr; i++)
    d->E[i] = sqrt (d->E[i] > 0 ? d->E[i] : 0) * MCVINE_HERTZ2MEV;
  /* Polarizations */
  snprintf (path, sizeof (path), "%s/Polarizations", dir);
  f = mcvine_fopen (path, "rb");
  if (!f || mcvine_idf_header (f, ft, &D2, &Nb2, &Nq2)) {
    fprintf (stderr, "%s: cannot read IDF '%s'\n", owner, path);
    if (f)
      fclose (f);
    return 1;
  }
  if (D2 != D || Nb2 != Nb || Nq2 != Nq) {
    fprintf (stderr, "%s: '%s' shape does not match Omega2\n", owner, path);
    fclose (f);
    return 1;
  }
  neps = (size_t)nq * nbr * Nb * 3 * 2;
  d->eps = (double*)mcvine_xmalloc (neps * sizeof (double));
  if (fread (d->eps, sizeof (double), neps, f) != neps) {
    fprintf (stderr, "%s: '%s' truncated\n", owner, path);
    fclose (f);
    return 1;
  }
  fclose (f);
  d->Emin = (double*)mcvine_xmalloc (nbr * sizeof (double));
  d->Emax = (double*)mcvine_xmalloc (nbr * sizeof (double));
  for (br = 0; br < nbr; br++) {
    d->Emin[br] = 1e300;
    d->Emax[br] = -1e300;
    for (i = 0; i < nq; i++) {
      double e = d->E[i * nbr + br];
      if (e < d->Emin[br])
        d->Emin[br] = e;
      if (e > d->Emax[br])
        d->Emax[br] = e;
    }
  }
  /* optional DOS */
  snprintf (path, sizeof (path), "%s/DOS", dir);
  f = mcvine_fopen (path, "rb");
  if (f) {
    fclose (f);
    d->has_dos = !mcvine_dos_load (&d->m_dos, path, 1, owner);
  }
  return 0;
}

/* fractional (reduced) coordinates of Q in the grid cell, then trilinear weights */
static void
mcvine_disp_locate (const mcvine_dispersion* d, const double* Q, int* id, double* fr) {
  int i;
  for (i = 0; i < 3; i++) {
    double c = d->a[i][0] * Q[0] + d->a[i][1] * Q[1] + d->a[i][2] * Q[2], r;
    c -= floor (c);
    r = c * (d->n[i] - 1);
    id[i] = (int)floor (r);
    if (id[i] >= d->n[i] - 1)
      id[i] = d->n[i] - 2;
    if (id[i] < 0)
      id[i] = 0;
    fr[i] = r - id[i];
  }
}
static double
mcvine_trilin (const double* f, const int* id, const double* fr, int n1, int n2, size_t stride, size_t off) {
  double s = 0;
  int a, b, c;
  for (a = 0; a < 2; a++)
    for (b = 0; b < 2; b++)
      for (c = 0; c < 2; c++) {
        double w = (a ? fr[0] : 1 - fr[0]) * (b ? fr[1] : 1 - fr[1]) * (c ? fr[2] : 1 - fr[2]);
        if (w != 0)
          s += w * f[((size_t)((id[0] + a) * n1 + id[1] + b) * n2 + id[2] + c) * stride + off];
      }
  return s;
}
double
mcvine_dispersion_energy (const mcvine_dispersion* d, int branch, const double* Q) {
  int id[3];
  double fr[3];
  mcvine_disp_locate (d, Q, id, fr);
  return mcvine_trilin (d->E, id, fr, d->n[1], d->n[2], d->nbranches, branch);
}
void
mcvine_dispersion_polarization (const mcvine_dispersion* d, int branch, int atom, const double* Q, double* re, double* im) {
  int id[3], c;
  double fr[3];
  size_t stride = (size_t)d->nbranches * d->natoms * 6;
  mcvine_disp_locate (d, Q, id, fr);
  for (c = 0; c < 3; c++) {
    size_t off = ((size_t)branch * d->natoms + atom) * 6 + c * 2;
    re[c] = mcvine_trilin (d->eps, id, fr, d->n[1], d->n[2], stride, off);
    im[c] = mcvine_trilin (d->eps, id, fr, d->n[1], d->n[2], stride, off + 1);
  }
}

/* phonon/scattering_length.icc : |sum_i b_i/sqrt(M_i) exp(iQ.d_i) (Q.eps_i)/|eps_i| |^2 */
static double
mcvine_norm_slsum (const mcvine_dispersion* d, const mcvine_atom* atoms, int natoms, int branch, const double* Q) {
  double sr = 0, si = 0;
  int i;
  for (i = 0; i < natoms; i++) {
    double er[3], ei[3], epslen, qer, qei, qd, c, s, amp;
    mcvine_dispersion_polarization (d, branch, i, Q, er, ei);
    epslen = sqrt (er[0] * er[0] + er[1] * er[1] + er[2] * er[2] + ei[0] * ei[0] + ei[1] * ei[1] + ei[2] * ei[2]);
    if (!(epslen > 0))
      continue;
    qer = (Q[0] * er[0] + Q[1] * er[1] + Q[2] * er[2]) / epslen;
    qei = (Q[0] * ei[0] + Q[1] * ei[1] + Q[2] * ei[2]) / epslen;
    qd = Q[0] * atoms[i].pos[0] + Q[1] * atoms[i].pos[1] + Q[2] * atoms[i].pos[2];
    c = cos (qd);
    s = sin (qd);
    amp = atoms[i].b_coh / sqrt (atoms[i].mass);
    sr += amp * (c * qer - s * qei);
    si += amp * (c * qei + s * qer);
  }
  return sr * sr + si * si;
}

/* ---- phonon kernels ---- */
/* phonon/IncoherentElastic.cc */
int
mcvine_S_Phonon_IncoherentElastic (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_Phonon_IncoherentElastic* k = (mcvine_kernel_Phonon_IncoherentElastic*)kk;
  double vi = mcvine_len3 (v), theta = MCVINE_PI * rand01 (), phi = 2 * MCVINE_PI * rand01 (), Q, e1[3], e2[3], e3[3], d[3];
  Q = V2K * vi * 2 * sin (theta / 2);
  mcvine_frame (v, e1, e2, e3);
  mcvine_dir_from_frame (e1, e2, e3, cos (theta), sin (theta), phi, d);
  v[0] = d[0] * vi;
  v[1] = d[1] * vi;
  v[2] = d[2] * vi;
  *p *= sin (theta) * (MCVINE_PI / 2) * exp (-k->m_dw_core * Q * Q);
  return 1;
}

/* phonon/IncoherentInelastic.cc and IncoherentInelastic_EnergyFocusing.cc */
int
mcvine_S_Phonon_IncoherentInelastic (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_Phonon_IncoherentInelastic* k = (mcvine_kernel_Phonon_IncoherentInelastic*)kk;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, Ef, e_range, omega, vf, d[3], Q[3], Ql, beta, DW, EQ;
  int i, is_small;
  mcvine_random_direction (d, _particle);
  if (k->m_focusing) {
    double Efmax = k->m_Ef + k->m_dEf / 2.;
    if (Ei - Efmax > k->m_max_omega || Ei - Efmax < 0)
      return 0;
    Ef = k->m_Ef + (rand01 () - 0.5) * k->m_dEf;
    e_range = k->m_dEf;
    if (Ef < 0)
      return 0;
  } else if (Ei > k->m_max_omega) {
    e_range = 2 * k->m_max_omega;
    Ef = Ei - k->m_max_omega + rand01 () * e_range;
  } else {
    e_range = Ei + k->m_max_omega;
    Ef = rand01 () * e_range;
  }
  omega = Ei - Ef;
  is_small = fabs (omega) < 1e-2 * k->m_max_omega;
  vf = SE2V * sqrt (Ef);
  for (i = 0; i < 3; i++) {
    Q[i] = V2K * (v[i] - vf * d[i]);
    v[i] = vf * d[i];
  }
  Ql = mcvine_len3 (Q);
  beta = 1. / (k->m_T * MCVINE_KELVIN2MEV);
  DW = exp (-k->m_dw_core * Ql * Ql);
  EQ = mcvine_k2E (Ql);
  *p *= e_range / k->m_mass * (vf / vi) * DW;
  if (is_small)
    *p *= k->m_dos.sod / beta * EQ;
  else
    *p *= mcvine_phonon_bose_factor (omega, k->m_T) * mcvine_dos_value (&k->m_dos, fabs (omega)) * EQ / fabs (omega);
  return 1;
}

/* phonon/CoherentInelastic_PolyXtal.cc */
int
mcvine_S_Phonon_CoherentInelastic_PolyXtal (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_Phonon_CoherentInelastic_PolyXtal* k = (mcvine_kernel_Phonon_CoherentInelastic_PolyXtal*)kk;
  const mcvine_dispersion* D = k->m_disp;
  double vi = mcvine_len3 (v), Ei = VS2E * vi * vi, Qmax, Q[3], vQ = 0, omega = 0, Ef = 0, vf = 0, Ql, ki, kf, cost, sint;
  double e1[3], e2[3], e3[3], d[3], q, x, Q1, vol, norm, dE;
  int branch, iter, ok = 0, i;
  branch = (int)floor (rand01 () * D->nbranches);
  if (branch >= D->nbranches)
    branch = D->nbranches - 1;
  *p *= D->nbranches;
  Qmax = mcvine_E2k (Ei) + mcvine_E2k (Ei + k->m_max_omega);
  /* pick_a_valid_Q_vector: MCViNE rejects until valid and uses an empirical
     accessible reciprocal volume; m_unbiased=1: one cube sample, weight
     uses the cube volume (2 Qmax)^3 exactly.                               */
  for (iter = 0; iter < (k->m_unbiased ? 1 : 100000); iter++) {
    for (i = 0; i < 3; i++)
      Q[i] = (2 * rand01 () - 1) * Qmax;
    vQ = K2V * mcvine_len3 (Q);
    omega = mcvine_dispersion_energy (D, branch, Q);
    if (omega < k->m_min_omega)
      continue;
    if (omega < Ei)
      Ef = (rand01 () >= 0.5) ? Ei + omega : Ei - omega; /* pick_Ef */
    else
      Ef = Ei + omega;
    vf = SE2V * sqrt (Ef);
    if (vQ >= fabs (vi - vf) && vQ <= vi + vf) {
      ok = 1;
      break;
    }
  }
  if (!ok)
    return 0;
  cost = (vi * vi + vf * vf - vQ * vQ) / (2 * vi * vf);
  if (cost * cost > 1)
    cost = cost > 0 ? 1 : -1;
  sint = sqrt (1 - cost * cost);
  mcvine_frame (v, e1, e2, e3);
  mcvine_dir_from_frame (e1, e2, e3, cost, sint, 2 * MCVINE_PI * rand01 (), d);
  for (i = 0; i < 3; i++)
    v[i] = d[i] * vf;
  ki = V2K * vi;
  kf = V2K * vf;
  Ql = V2K * vQ;
  dE = Ei - Ef;
  if (Ei > omega)
    *p *= 2.0; /* two choices of E_f (see README: uses phonon energy) */
  norm = mcvine_norm_slsum (D, k->m_atoms, k->m_natoms, branch, Q);
  norm *= 1e-30 * 4 * MCVINE_PI;
  norm /= (k->m_xs_coh_tot * 1e-28);
  *p *= MCVINE_KSQ2E * norm;
  *p /= fabs (dE);
  *p *= exp (-k->m_dw_core * Ql * Ql);
  *p *= kf / ki;
  *p *= mcvine_phonon_bose_factor (dE, k->m_T);
  *p *= 1 / ki / kf / Ql;
  q = mcvine_E2k (k->m_max_omega);
  x = ki / q; /* calc_AccessibleReciVol */
  Q1 = ki + mcvine_E2k (Ei + k->m_max_omega / (1 + 7.6 * pow (x, 4)));
  vol = k->m_unbiased ? 8 * Qmax * Qmax * Qmax : 4. / 3. * MCVINE_PI * Q1 * Q1 * Q1;
  *p *= vol;
  *p /= 8 * MCVINE_PI;
  return 1;
}

/* phonon/CoherentInelastic_SingleXtal.cc */
typedef struct {
  const mcvine_dispersion* D;
  int branch;
  double dir[3], vi[3], vil;
} mcvine_omq_ctx;
static double
mcvine_omq_f (double vf, void* c) { /* Omega_minus_deltaE.cc */
  mcvine_omq_ctx* x = (mcvine_omq_ctx*)c;
  double q[3];
  int i;
  for (i = 0; i < 3; i++)
    q[i] = V2K * (x->vi[i] - vf * x->dir[i]);
  return mcvine_dispersion_energy (x->D, x->branch, q) - VS2E * fabs (x->vil * x->vil - vf * vf);
}
int
mcvine_S_Phonon_CoherentInelastic_SingleXtal (void* kk, const double* r, double* v, double t, double* p, _class_particle* _particle) {
  mcvine_kernel_Phonon_CoherentInelastic_SingleXtal* k = (mcvine_kernel_Phonon_CoherentInelastic_SingleXtal*)kk;
  const mcvine_dispersion* D = k->m_disp;
  mcvine_omq_ctx c;
  double vil = mcvine_len3 (v), Ei = VS2E * vil * vil, roots[256], solid = 4 * MCVINE_PI, vf = 0, fac = 1;
  int good[1024], ngood = 0, br, iter, nf = 0, i;
  for (br = 0; br < D->nbranches && ngood < 1024; br++)
    if (D->Emin[br] < Ei * 1.5)
      good[ngood++] = br;
  if (!ngood)
    return 0;
  c.D = D;
  c.vil = vil;
  for (i = 0; i < 3; i++)
    c.vi[i] = v[i];
  /* MCViNE retries (up to 100 directions/branches) until omega(Q)=|Ei-Ef| has
     a solution, without correcting the weight for the success probability:
     absolute intensities come out too high by 1/P(success). m_unbiased=1
     makes a single attempt (unbiased; failed attempts get weight 0).        */
  for (iter = 0; iter < (k->m_unbiased ? 1 : 100); iter++) {
    if (k->m_target_radius > 0) {
      double tx = k->m_target[0] - r[0], ty = k->m_target[1] - r[1], tz = k->m_target[2] - r[2], n;
      randvec_target_circle (&c.dir[0], &c.dir[1], &c.dir[2], &solid, tx, ty, tz, k->m_target_radius);
      n = mcvine_len3 (c.dir);
      for (i = 0; i < 3; i++)
        c.dir[i] /= n;
    } else {
      mcvine_random_direction (c.dir, _particle);
      solid = 4 * MCVINE_PI;
    }
    i = (int)floor (rand01 () * ngood);
    if (i >= ngood)
      i = ngood - 1;
    c.branch = good[i];
    nf = mcvine_find_roots (mcvine_omq_f, &c, 0, 2 * vil, k->m_nsteps, k->m_xacc, roots, 256);
    if (nf > 0)
      break;
  }
  if (nf < 1)
    return 0;
  {
    double dv = k->m_deltaV_Jacobi * vil, f1, f2, J, Ef, omega, Q[3], ki, kf, norm;
    int idx;
    idx = (int)floor (rand01 () * nf);
    if (idx >= nf)
      idx = nf - 1;
    vf = roots[idx];
    fac *= nf;
    f1 = mcvine_omq_f (vf - dv, &c);
    f2 = mcvine_omq_f (vf + dv, &c);
    J = fabs (f2 - f1) / (2 * dv);
    if (!(J > 0))
      return 0;
    fac *= 2 * VS2E * vf / J;
    fac *= solid;
    fac *= ngood;
    Ef = VS2E * vf * vf;
    omega = Ei - Ef;
    for (i = 0; i < 3; i++)
      Q[i] = V2K * (v[i] - vf * c.dir[i]);
    ki = V2K * vil;
    kf = V2K * vf;
    norm = mcvine_norm_slsum (D, k->m_atoms, k->m_natoms, c.branch, Q);
    norm /= 1e30;
    norm /= (k->m_xs_coh_tot * 1e-28);
    fac *= MCVINE_KSQ2E * norm / fabs (omega);
    fac *= kf / ki;
    fac *= exp (-k->m_dw_core * (Q[0] * Q[0] + Q[1] * Q[1] + Q[2] * Q[2]));
    fac *= mcvine_phonon_bose_factor (omega, k->m_T);
    *p *= fac;
    for (i = 0; i < 3; i++)
      v[i] = c.dir[i] * vf;
  }
  return 1;
}

#endif /* MCVINE_LIB_C */
