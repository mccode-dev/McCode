#include "mcstas-basics.h"
#include "mccode-r.h"

/* Definitions a generated McStas instrument provides to the runtime and
   libraries, cut verbatim from mcstas --no-runtime --no-main output for an
   empty instrument "libcheck" (the resource-path #define MCSTAS left out). */

_class_particle _particle_global_randnbuse_var;
_class_particle* _particle = &_particle_global_randnbuse_var;

#ifdef MC_TRACE_ENABLED
int traceenabled = 1;
#else
int traceenabled = 0;
#endif
int   defaultmain         = 0;
char  instrument_name[]   = "libcheck";
char  instrument_source[] = "libcheck.instr";
char *instrument_exe      = NULL; /* will be set to argv[0] in main */
char  instrument_code[]   = "Instrument libcheck source code libcheck.instr is not embedded in this executable.\n  Use --source option when running mcstas.\n";

int numipar = 0;
struct mcinputtable_struct mcinputtable[] = {
  NULL, NULL, instr_type_double, ""
};

struct metadata_table_struct metadata_table[] = {
  "", "", "", ""
};
int num_metadata = 0;

/* defined by mccode-r.h only when the runtime is embedded */
FILE *siminfo_file = NULL;

int save(FILE *handle) { /* called by mccode_main for libcheck:SAVE */
  if (!handle) siminfo_init(NULL);

  /* call iteratively all components SAVE */
  if (!handle) siminfo_close(); 

  return(0);
} /* save */

/* *****************************************************************************
* instrument 'libcheck' and components FINALLY
***************************************************************************** */



int finally(void) { /* called by mccode_main for libcheck:FINALLY */
#pragma acc update host(_instrument_var)

  siminfo_init(NULL);
  save(siminfo_file); /* save data when simulation ends */

  /* call iteratively all components FINALLY */
  siminfo_close(); 

  return(0);
} /* finally */
