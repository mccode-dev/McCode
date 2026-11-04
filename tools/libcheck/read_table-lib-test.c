#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "mcstas-basics.h"
#include "mccode-r.h"
#include "read_table-lib.h"

static int failures = 0;
static volatile double sink;

// mccode-dev/McCode#3017: Table_Value2d on one-row and one-column tables
static void check_value2d(t_Table t, const char *name, double X, double Y, double expected)
{
  double got = Table_Value2d(t, X, Y);
  if (fabs(got - expected) > 1e-12) {
    fprintf(stderr, "#3017: Table_Value2d(%s, %g, %g) = %g, expected %g\n", name, X, Y, got, expected);
    failures++;
  }
}

static void test_value2d_vectors(void)
{
  t_Table R, C;
  int i;

  memset(&R, 0, sizeof(R));
  memset(&C, 0, sizeof(C));
  Table_Init(&R, 1, 5);
  Table_Init(&C, 5, 1);
  for (i = 0; i < 5; i++) {
    Table_SetElement(&R, 0, i, 10 * i);
    Table_SetElement(&C, i, 0, 10 * i);
  }

  check_value2d(R, "R", 0, 1.5, 15);
  check_value2d(R, "R", 0.5, 1, 10);
  check_value2d(R, "R", 0.5, 1.5, 15);
  check_value2d(R, "R", 2, 1, 10);
  check_value2d(R, "R", -1, 1, 10);
  check_value2d(C, "C", 1, 0.5, 10);
  check_value2d(C, "C", 1.5, 0.5, 15);

  Table_Free(&R);
  Table_Free(&C);
}

// mccode-dev/McCode#3016: Table_Value falls back to the linear search on
// constant-step and large monotonic tables, so a lookup costs O(rows)
static double x_constant(long i, long rows) { (void)rows; return 1 + 0.5 * i; }
static double x_quadratic(long i, long rows) { double u = (double)i / (rows - 1); return 0.5 + 19.5 * u * u; }
static double y_of(double x) { return sin(0.37 * x) + 2; }

static void load_table(t_Table *t, const char *file, long rows, double (*x_of)(long, long))
{
  FILE *f = fopen(file, "w");
  long i;

  assert(f);
  for (i = 0; i < rows; i++)
    fprintf(f, "%.17g %.17g\n", x_of(i, rows), y_of(x_of(i, rows)));
  fclose(f);
  memset(t, 0, sizeof(*t));
  assert(Table_Read(t, (char *)file, 1) == 2 * rows);
}

static double ns_per_lookup(t_Table t)
{
  double span = t.max_x - t.min_x;
  long n = 0;
  int i;
  clock_t start, now;

  srand(1);
  start = clock();
  do {
    for (i = 0; i < 1000; i++)
      sink += Table_Value(t, t.min_x + span * rand() / ((double)RAND_MAX + 1), 1);
    n += 1000;
    now = clock();
  } while (now - start < CLOCKS_PER_SEC / 20);
  return 1e9 * (double)(now - start) / CLOCKS_PER_SEC / n;
}

static void check_values(t_Table t, const char *name, double (*x_of)(long, long), long rows)
{
  long i = rows / 3;
  double x1 = x_of(i, rows), x2 = x_of(i + 1, rows);
  double y1 = y_of(x1), y2 = y_of(x2);

  if (fabs(Table_Value(t, x1, 1) - y1) > 1e-12 || fabs(Table_Value(t, 0.5 * (x1 + x2), 1) - 0.5 * (y1 + y2)) > 1e-12) {
    fprintf(stderr, "#3016: Table_Value on %s returns wrong values\n", name);
    failures++;
  }
}

static void check_scaling(const char *name, double (*x_of)(long, long), long small, long big)
{
  t_Table s, b;
  double ns_small, ns_big;
  char file[64];

  snprintf(file, sizeof(file), "%s_%ld.dat", name, small);
  load_table(&s, file, small, x_of);
  snprintf(file, sizeof(file), "%s_%ld.dat", name, big);
  load_table(&b, file, big, x_of);

  check_values(b, name, x_of, big);
  ns_small = ns_per_lookup(s);
  ns_big = ns_per_lookup(b);
  if (ns_big > 10 * ns_small) {
    fprintf(stderr, "#3016: Table_Value on %s: %.0f ns/lookup at %ld rows, %.0f ns/lookup at %ld rows\n",
            name, ns_small, small, ns_big, big);
    failures++;
  }

  Table_Free(&s);
  Table_Free(&b);
}

int main(void)
{
  t_Table t;

  test_value2d_vectors();

  load_table(&t, "probe.dat", 1000, x_quadratic);
  assert(t.monotonic && !t.constantstep);
  Table_Free(&t);
  load_table(&t, "probe_c.dat", 1000, x_constant);
  assert(t.constantstep);
  Table_Free(&t);

  check_scaling("constant_step", x_constant, 100, 100000);
  check_scaling("monotonic", x_quadratic, 1000, 100000);

  assert(failures == 0);
  return 0;
}
