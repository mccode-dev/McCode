#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "mcstas-basics.h"
#include "mccode-r.h"
#include "read_table-lib.h"

static int failures = 0;

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

// mccode-dev/McCode#3016: Table_Value should find the bracketing rows with
// the constant-step lookup on constant-step tables and with the binary
// search on monotonic tables of more than 100 rows, not the linear search
//
// Test tables have two columns, x and y = table_y(x). Their x column is given
// by a row_x function: row_x(i, rows) is the x of row i in a table of rows rows.
static double x_constant(long i, long rows) { (void)rows; return 1 + 0.5 * i; }
static double x_quadratic(long i, long rows) { double u = (double)i / (rows - 1); return 0.5 + 19.5 * u * u; }
// mccode-dev/McCode#1968, #2565: the same tables, monotonically decreasing
static double x_constant_down(long i, long rows) { return x_constant(rows - 1 - i, rows); }
static double x_quadratic_down(long i, long rows) { return x_quadratic(rows - 1 - i, rows); }
static double table_y(double x) { return sin(0.37 * x) + 2; }

// writes the table to file, then reads it back with Table_Read (the only way
// to get its monotonic/constantstep flags set); mccode-libcheck runs each
// test in its own temporary directory
static void write_and_read_table(t_Table *t, const char *file, long rows, double (*row_x)(long, long))
{
  FILE *f = fopen(file, "w");
  long i;

  assert(f);
  for (i = 0; i < rows; i++)
    fprintf(f, "%.17g %.17g\n", row_x(i, rows), table_y(row_x(i, rows)));
  fclose(f);
  memset(t, 0, sizeof(*t));
  assert(Table_Read(t, (char *)file, 1) == 2 * rows);
}

static void check_flags(t_Table t, const char *name, int monotonic, int constantstep)
{
  if (t.monotonic != monotonic || t.constantstep != constantstep) {
    fprintf(stderr, "%s table: monotonic = %d, constantstep = %d, expected %d, %d\n",
            name, t.monotonic, t.constantstep, monotonic, constantstep);
    failures++;
  }
}

// a table point, an interval midpoint, and X beyond either end (the value at
// that end)
static void check_values(t_Table t, const char *name, double (*row_x)(long, long), long rows)
{
  long i = rows / 3;
  double x1 = row_x(i, rows), x2 = row_x(i + 1, rows);
  double y1 = table_y(x1), y2 = table_y(x2);
  double x_min = MIN(row_x(0, rows), row_x(rows - 1, rows)), x_max = MAX(row_x(0, rows), row_x(rows - 1, rows));

  if (fabs(Table_Value(t, x1, 1) - y1) > 1e-12 || fabs(Table_Value(t, 0.5 * (x1 + x2), 1) - 0.5 * (y1 + y2)) > 1e-12
      || Table_Value(t, x_max + 1, 1) != table_y(x_max) || Table_Value(t, x_min - 1, 1) != table_y(x_min)) {
    fprintf(stderr, "%s table: Table_Value returns wrong values\n", name);
    failures++;
  }
}

// every table point x[i] and interval midpoint, except the last x itself,
// must be bracketed correctly by the expected search: x[Index-1] <= X <
// x[Index] on an increasing table, x[Index-1] >= X > x[Index] on a decreasing one
static void check_path(t_Table t, const char *name, double (*row_x)(long, long), long rows, int expected)
{
  long i, wrong = 0, bad_bracket = 0, checked = 0;
  double first_wrong = 0;
  int first_path = 0;

  for (i = 0; i < 2 * (rows - 1); i++) {
    double X = (i % 2) ? 0.5 * (row_x(i / 2, rows) + row_x(i / 2 + 1, rows)) : row_x(i / 2, rows);
    double X1, X2;
    int path;
    long Index = Table_Value_Index(t, X, &X1, &X2, &path);

    checked++;
    if (!(Index >= 1 && Index < rows && X1 == row_x(Index - 1, rows) && X2 == row_x(Index, rows)
          && ((X1 <= X && X < X2) || (X2 < X && X <= X1))))
      bad_bracket++;
    if (path != expected) {
      if (!wrong) { first_wrong = X; first_path = path; }
      wrong++;
    }
  }
  if (wrong || bad_bracket) {
    fprintf(stderr, "%s table, %ld rows: %ld of %ld lookups not by path %d (first at X = %g, path %d), %ld wrong brackets\n",
            name, rows, wrong, checked, expected, first_wrong, first_path, bad_bracket);
    failures++;
  }
}

// Table_Rebin makes a decreasing table evenly spaced and increasing, with the
// values Table_Value gave before at the new x
static void check_rebin_decreasing(void)
{
  t_Table t;
  long i, at[] = {0, 7, 100, 1000};
  double step, x_min, before[4];
  int ok;

  write_and_read_table(&t, "rebin_down.dat", 50, x_quadratic_down);
  step = fabs(t.step_x);
  x_min = t.min_x;
  for (i = 0; i < 4; i++)
    before[i] = Table_Value(t, x_min + at[i] * step, 1);
  Table_Rebin(&t);
  ok = t.rows > 1000 && t.monotonic && t.constantstep && Table_Index(t, 0, 0) == x_min
       && Table_Index(t, 1, 0) > Table_Index(t, 0, 0);
  for (i = 0; ok && i < 4; i++)
    ok = fabs(Table_Value(t, x_min + at[i] * step, 1) - before[i]) < 1e-12;
  if (!ok) {
    fprintf(stderr, "decreasing table: Table_Rebin gives %ld rows, x from %g to %g, monotonic = %d, constantstep = %d\n",
            t.rows, Table_Index(t, 0, 0), Table_Index(t, t.rows - 1, 0), t.monotonic, t.constantstep);
    failures++;
  }
  Table_Free(&t);
}

int main(void)
{
  t_Table t;

  test_value2d_vectors();

  write_and_read_table(&t, "constant_step.dat", 1000, x_constant);
  check_flags(t, "constant_step", 1, 1);
  check_values(t, "constant_step", x_constant, 1000);
  check_path(t, "constant_step", x_constant, 1000, READ_TABLE_LOOKUP_CONSTANTSTEP);
  Table_Free(&t);

  write_and_read_table(&t, "monotonic.dat", 1000, x_quadratic);
  check_flags(t, "monotonic", 1, 0);
  check_values(t, "monotonic", x_quadratic, 1000);
  check_path(t, "monotonic", x_quadratic, 1000, READ_TABLE_LOOKUP_BINARY);
  Table_Free(&t);

  write_and_read_table(&t, "constant_step_down.dat", 1000, x_constant_down);
  check_flags(t, "decreasing constant_step", 1, 1);
  check_values(t, "decreasing constant_step", x_constant_down, 1000);
  check_path(t, "decreasing constant_step", x_constant_down, 1000, READ_TABLE_LOOKUP_CONSTANTSTEP);
  Table_Free(&t);

  write_and_read_table(&t, "monotonic_down.dat", 1000, x_quadratic_down);
  check_flags(t, "decreasing monotonic", 1, 0);
  check_values(t, "decreasing monotonic", x_quadratic_down, 1000);
  check_path(t, "decreasing monotonic", x_quadratic_down, 1000, READ_TABLE_LOOKUP_BINARY);
  Table_Free(&t);

  check_rebin_decreasing();

  assert(failures == 0);
  return 0;
}
