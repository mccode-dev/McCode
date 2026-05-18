/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2009, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Kernel: port.c
*
* %Identification
* Written by: K.N.
* Date: Nov 11, 1998
* Origin: Risoe
* Release: McStas CVS_090602
* Version: $Revision$
*
* Code to handle portability issues (mainly Win32 stuff).
*
* $Id$
*
*******************************************************************************/

#include "port.h"

#ifndef HAVE_STRCASECMP
#include <ctype.h>
int strcasecmp(char *s1, char *s2)
{
  int c, c1, c2;
  do {
    c1 = toupper(*s1++);
    c2 = toupper(*s2++);
    c = c1 - c2;
  } while(c1 != 0 && c2 != 0 && c == 0);
  return c;
}
#endif /* HAVE_STRCASECMP */

#ifndef HAVE_FDOPEN
FILE *fdopen(int descr, const char *mode)
{
  fprintf(stderr, "The '-' argument for standard input and output is not supported"
	      " on this system.");
  return(NULL);
}
#endif /* HAVE_FDOPEN */

#ifndef HAVE_STRCASESTR
#include <ctype.h>

char *
strcasestr (char *haystack, char *needle)
{
	char *p, *startn = 0, *np = 0;

	for (p = haystack; *p; p++) {
		if (np) {
			if (toupper(*p) == toupper(*np)) {
				if (!*++np)
					return startn;
			} else
				np = 0;
		} else if (toupper(*p) == toupper(*needle)) {
			np = needle + 1;
			startn = p;
		}
	}

	return 0;
}
#endif /* HAVE_STRCASESTR */

