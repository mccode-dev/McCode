/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2009, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Kernel: memory.c
*
* %Identification
* Written by: K.N.
* Date: Jul  1, 1997
* Origin: Risoe
* Release: McStas CVS_090504
* Version: $Revision$
*
* Memory management functions.
*
*       $Id$
*
*******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>

#include "mccode.h"


/*******************************************************************************
* Allocate memory. This function never returns NULL; instead, the
* program is aborted if insufficient memory is available.
*******************************************************************************/
void *
mem(size_t size)
{
  void *p = calloc(1, size);    /* Allocate and clear memory. */
  if(p == NULL)
    fatal_error("memory exhausted during allocation of size %ld.", (long)size);
  return p;
}

/*******************************************************************************
* Free memory allocated with mem().
*******************************************************************************/
void memfree(void *p)
{
  if(p == NULL)
    debug(("memfree(): freeing NULL memory.\n"));
  else
    free(p);
}

/*******************************************************************************
* Allocate a new copy of a string.
*******************************************************************************/
char *
str_dup(char *string)
{
  char *s;

  s = mem(strlen(string) + 1);
  strcpy(s, string);
  return s;
}


/*******************************************************************************
* Allocate a new copy of initial N chars in a string.
*******************************************************************************/
char *
str_dup_n(char *string, int n)
{
  char *s;

  s = mem(n + 1);
  strncpy(s, string, n);
  s[n] = '\0';
  return s;
}


/*******************************************************************************
* Allocate a new string to hold the concatenation of given strings. Arguments
* are the strings to concatenate, terminated by NULL.
*******************************************************************************/
char *
str_cat(char *first, ...)
{
  char *s;
  va_list ap;
  int size;
  char *arg;

  size = 1;                     /* Count final '\0'. */
  va_start(ap, first);
  for(arg = first; arg != NULL; arg = va_arg(ap, char *))
    size += strlen(arg);        /* Calculate string size. */
  va_end(ap);
  s = mem(size);
  size = 0;
  va_start(ap, first);
  for(arg = first; arg != NULL; arg = va_arg(ap, char *))
  {
    strcpy(&(s[size]), arg);
    size += strlen(arg);
  }
  va_end(ap);
  return s;
}

/*******************************************************************************
* Allocate a new string holding the result of quoting the input string. The
* result is suitable for inclusion in C source code.
*******************************************************************************/
char *
str_quote(char *string)
{
  char *badchars = "\\\"\r\n\t";
  char *quotechars = "\\\"rnt";
  char *q=NULL, *res=NULL, *ptr;
  int len, pass;
  int c;
  char new[5];

  /* Loop over the string twice, first counting chars and afterwards copying
     them into an allocated buffer. */
  for(pass = 0; pass < 2; pass++)
  {
    char *p = string;

    if(pass == 0)
      len = 0;                  /* Prepare to compute length */
    else
      q = res = mem(len + 1);   /* Allocate buffer */
    /* Notice the cast to unsigned char; without it, the isprint(c) below will
       fail for characters with negative plain char values. */
    while((c = (unsigned char)(*p++)))
    {
      ptr = strchr(badchars, c);
      if(ptr != NULL)
        sprintf(new, "\\%c", quotechars[ptr - badchars]);
      else if(isprint(c))
        sprintf(new, "%c", c);
      else
        sprintf(new, "\\%03o", c);
      if(pass == 0)
        len += strlen(new);     /* Count in length */
      else
        for(ptr = new; (*q = *ptr) != 0; ptr++)
          q++;                  /* Copy over chars */
    }
  }
  return res;
}


/*******************************************************************************
* Free memory for a string.
*******************************************************************************/
void
str_free(char *string)
{
  memfree(string);
}
