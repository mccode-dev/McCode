/* In Emacs, please make this -*-c-*- mode. Thanks. */

/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2007, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Kernel: instrument.l
*
* %Identification
* Written by: K.N.
* Date: Jul  1, 1997
* Origin: Risoe
* Release: McStas 1.6
* Version: $Revision$
*
* Flex scanner for instrument definition files.
*
* Overview (how the pieces fit together):
*   - instrument.y (the bison parser) pulls tokens from yylex() defined here.
*     Keywords, identifiers, numbers, strings and "any other C token"
*     (TOK_CTOK) are returned one by one; the parser glues C tokens back
*     together into expressions (CExp, see cexp.c).
*   - %{ ... %} blocks are NOT tokenised: the scanner switches to the <ccode>
*     start condition and hands every source line verbatim to the parser as a
*     TOK_CODE_LINE (newline included). The parser collects them into a
*     struct code_block, which cogen.c later prints unchanged.
*   - Files can nest: %include (in instrument/component text or inside a C
*     block) and component autoloading (read_component() in instrument.y)
*     both push the current flex buffer on file_stack and continue scanning
*     the new file. <<EOF>> pops back to the including file.
*   - The very first token of every parse is synthetic: TOK_GENERAL for an
*     instrument file, TOK_RESTRICTED for an autoloaded .comp file. This lets
*     one grammar serve both cases (see rule `main' in instrument.y).
*   - instr_current_line/instr_current_filename track the position for error
*     messages; every rule that consumes an end-of-line increments the line.
*
*******************************************************************************/


/* Definition section. */

/* Do not use the `yywrap feature' - only scan a single file (see Flex manual). */
%option noyywrap


%{
#include <string.h>
#include <math.h>
#include <stdio.h>

// On Windows and using cl.exe, avoid
// unistd.h and map popen/pclose to fallback _ versions
#ifdef _MSC_EXTENSIONS
#define YY_NO_UNISTD_H
#define popen _popen
#define pclose _pclose
#endif

#include "mccode.h"
#include "instrument.tab.h"

/* Fix things for bison option %pure_parser. */
#define YY_DECL int yylex(YYSTYPE *yylvalp)
#define yylval (*yylvalp)


/* Structure to hold the state of a file being parsed. One entry is pushed on
   file_stack for every %include / autoloaded component, and popped at EOF. */
struct file_state
  {
    YY_BUFFER_STATE buffer;
    char *filename;
    char *switch_line; /* pending "lib.c" to include after "lib.h" (see cinclname) */
    int line;
    int oldstate;   /* Saved lexer start condition. */
    int visible_eof;    /* If true, tell parser about end-of-file. */
  };

#define MAX_INCLUDE 256
static struct file_state file_stack[MAX_INCLUDE + 1];
static int file_stack_ptr = 0;
/* When %include "lib" (no extension) is used inside a C block, lib.h is
   embedded first and, with the runtime embedded, lib.c must follow it. The
   .c name is parked here, saved with the .h file's stack entry, and picked up
   by the <<EOF>> rule when lib.h ends. */
static char *switch_line = NULL;

static void push_include(char *name);

%}


/* Lexer states. */

/* The state ccomment is used for scanning c-style comments. The state ccode
   is used when scanning embedded C code blocks. */
%x ccomment
%x ccode

/* The state initial_token is only used to output an initial token to
   discriminate between parsing general instrument definitions and autoloaded
   component definitions. */
%x initial_token

/* Get file name in %include. */
%x inclname

/* Get full %include line within C code blocks.
   The line is re-scanned from its start (yyless(0)), so leading blanks are
   skipped there first. */
%x cfullincl

/* Get file name in %include within C code blocks. */
%x cinclname


/* Abbreviations. */
DIGIT   [0-9]
ALPHA   [A-Za-z]
ALPHANUM  {ALPHA}|{DIGIT}|"_"

NUMBER    -?({DIGIT}*".")?{DIGIT}+([Ee][+-]?{DIGIT}+)?
ID    {DIGIT}*{ALPHA}{ALPHANUM}*
EOL             (\r\n|\n|\r)
INCLUDE "%include"

%%

 /* Initially, output a single token to the parser to tell it whether to parse
    general instrument definitions or autoloaded component definitions.
    yyless(0) gives back the character just matched, so nothing is consumed. */
<initial_token>.|\n |
<initial_token><<EOF>> {
      yyless(0);
      BEGIN(INITIAL);
      return parse_restricted ? TOK_RESTRICTED : TOK_GENERAL;
    }

ABSOLUTE      return TOK_ABSOLUTE;
AT            return TOK_AT;
COMPONENT     return TOK_COMPONENT;
DECLARE       return TOK_DECLARE;
USERVARS      return TOK_USERVARS;
DEFINE        return TOK_DEFINE;
DEFINITION    return TOK_DEFINITION;
END           return TOK_END;
MCDISPLAY     return TOK_DISPLAY;
DISPLAY       return TOK_DISPLAY; /* DISPLAY is alias to MCDISPLAY */
FINALLY       return TOK_FINALLY;
INITIALIZE    return TOK_INITIALISE;
INITIALISE    return TOK_INITIALISE;  /* INITIALISE is alias to INITIALIZE */
INSTRUMENT    return TOK_INSTRUMENT;
OUTPUT        return TOK_PRIVATE;
PRIVATE       return TOK_PRIVATE; /* PRIVATE is alias to OUTPUT */
PARAMETERS    return TOK_PARAMETERS;
RELATIVE      return TOK_RELATIVE;
ROTATED       return TOK_ROTATED;
PREVIOUS      return TOK_PREVIOUS;
SETTING       return TOK_SETTING;
STATE         return TOK_STATE; /* obsolete -> error */
POLARISATION  return TOK_POL;   /* obsolete -> error */
TRACE         return TOK_TRACE;
SHARE         return TOK_SHARE;
EXTEND        return TOK_EXTEND;
GROUP         return TOK_GROUP;         /* extended McStas grammar */
SAVE          return TOK_SAVE;
JUMP          return TOK_JUMP;          /* extended McStas grammar */
WHEN          return TOK_WHEN;          /* extended McStas grammar */
NEXT          return TOK_NEXT;          /* extended McStas grammar */
ITERATE       return TOK_ITERATE;       /* extended McStas grammar */
MYSELF        return TOK_MYSELF;        /* extended McStas grammar */
INHERIT       return TOK_INHERIT;       /* extended McStas COMP grammar */
COPY          return TOK_COPY;          /* extended McStas INSTR grammar */
SPLIT         return TOK_SPLIT;         /* extended McStas grammar */
REMOVABLE     return TOK_REMOVABLE;     /* extended McStas grammar with include */
CPU           return TOK_CPUONLY;       /* extended McStas grammar with GPU-CPU support */
NOACC         return TOK_NOACC; 
DEPENDENCY    return TOK_DEPENDENCY;
SHELL         return TOK_SHELL;         /* extended McStas grammar with SHELL commands pre-cogen */
SEARCH        return TOK_SEARCH;        /* additional Instrument/Component search directories */
METADATA      return TOK_METADATA;


"("|")"|"["|"]"|"{"|"}"|"," return yytext[0]; /* Punctuation. */
"="|"*"       return yytext[0]; /* Operator. */
"/"  return yytext[0]; /* special name, unit separator */

{NUMBER}  yylval.number = str_dup(yytext); return TOK_NUMBER;
 /* Note: Since ID overlaps with NUMBER (eg. "2E3"), ID must come
    after NUMBER */
{ID}    yylval.string = str_dup(yytext); return TOK_ID;

 /* Scanning all other C tokens used in expressions for component
 * actual parameters.
 * IMPORTANT!: Whenever a token is removed from here to make an independent
 * separate token, the new token must be added to the parser rules for
 * genatexp/topatexp.
 * Flex picks the longest match, and the earliest rule on a tie. So "*" is
 * returned as '*' by the operator rule above, and "sizeof" as TOK_ID by {ID};
 * those alternatives (and the duplicate "+"/"-") never fire here.
 */
"->"|"."|"!"|"~"|"++"|"--"|"+"|"-"|"&"|"sizeof"|"*"|"%"|"+"|"-"|"<<"|">>"|"<"|"<="|">"|">="|"=="|"!="|"^"|"|"|"&&"|"||"|"?"|":"|"+="|"-="|"*="|"/="|"%="|"&="|"^="|"|="|"<<="|">>="|"'" {
      yylval.string = str_dup(yytext);
      return TOK_CTOK;
\
    }

  /* Scanning embedded C code.
     %{ must be alone on its line. The token carries the line number of the
     %{ so the code block can be traced back to its source (#line). */

"%""{"[\t ]*{EOL} {
      yylval.linenum = instr_current_line;
      instr_current_line++;
      BEGIN(ccode);
      return TOK_CODE_START;
    }
"%""{"[^\n]*{EOL} {
      instr_current_line++;
      print_error("%%{ token not on a line by itself "
                  "on line %d of file '%s': %s.\n",
              instr_current_line, instr_current_filename, yytext);
      return TOK_INVALID;
    }

<ccode>{
    /* normal %} symbol to end C code block */
    [\t ]*"%""}"[\t ]*{EOL} instr_current_line++; BEGIN(INITIAL); return TOK_CODE_END;
    /* %} symbol surrounded by some unrelevant stuff: the whole line is
       dropped with a warning and the scanner STAYS in <ccode>, i.e. this
       does not close the block. */
    [^\n]*"%""}"[^\n]*{EOL} {
        instr_current_line++;
        print_warn(NULL, "%%} terminator not on a line by itself "
                    "on line %d of file '%s': %s.\n",
              instr_current_line, instr_current_filename, yytext);
      }
    /* %include full line -> jump to cfullincl state */
    [\t ]*{INCLUDE}[^\n]*{EOL} {
      yyless(0);  /* re-use the current line, but within cfullincl state */
      BEGIN(cfullincl);
      }
    /* full line as C code */
    [^\n]*{EOL}   {
        instr_current_line++;
        yylval.string = str_dup(yytext);
        return TOK_CODE_LINE;
      }
    } /* end ccode */

 /* C character literals, e.g. 'a' or '\n', kept verbatim as a single token in
    expressions (otherwise ' would be scanned as a token of its own, and 'a'
    would become ' a ' in the generated code). */
\'([^\'\\\n]|\\[^\n])+\'  {
        yylval.string = str_dup(yytext);
        return TOK_CTOK;
      }

 /* Quoted strings. Simple version: no escape sequences. */
\"[^\"\n\\]*\"    {
        yylval.string =
          str_dup_n(&yytext[1], strlen(yytext) - 2);
        return TOK_STRING;
      }
 /* Quoted strings with escape sequence (e.g Win32 path): preserve all chars */
\"[^\"\n]*\\[^\"\n]*\"  {
        yylval.string = str_dup_n(&yytext[1], strlen(yytext) - 2);
        return TOK_STRING;
      }
\"[^\n\"]*{EOL}   {
        print_error("Error: Unterminated string "
                    "on line %d of file '%s': %s.\n",
              instr_current_line, instr_current_filename, yytext);
      }

 /* %-style comments - ignore everything to end of line.
    Only "%" followed by a blank or end-of-line is a comment; e.g. "%foo"
    falls through to the invalid-character rule. */
"%"{EOL}    instr_current_line++; /* Ignore comment. */
"% "[^\n]*{EOL} instr_current_line++; /* Ignore comment. */

 /* Include files for McStas comp/instr (INITIAL state). */
 /* then next token is the file name */
{INCLUDE}[ \t]+\" BEGIN(inclname);
<inclname>{
  /* name ends with a quote char -> include as INITIAL state */
  [^\"\n]+\" {
        yytext[yyleng - 1] = '\0';
        BEGIN(INITIAL);
        if (verbose) fprintf(stderr, "Embedding %s\n", yytext);
        push_include(yytext);
      }
  /* name contains char, including quote and \n -> not valid */
  [\"\n].*  {
        print_error("Error in %%include statement "
                    "on line %d of file '%s': %s.\n",
              instr_current_line, instr_current_filename, yytext);
        BEGIN(INITIAL);
      }
  } /* end inclname */
 /* Include files within C code blocks (ccode state)*/
 /* next token is full line, regenerated by yyless(0) */
<cfullincl>[ \t]+  /* skip indentation (else flex would echo it) */
<cfullincl>{INCLUDE}[ \t]+\"  BEGIN(cinclname);
<cinclname>{
  /* name ends with a quote char, with extension ("x.h", "x.hpp", "dir/x.c")
     -> include as ccode state. Both rules match the same text, so this one
     wins by being first. */
  [^\"\n]+\.[A-Za-z0-9]+\"  {
        yytext[yyleng - 1] = '\0';
        BEGIN(ccode);
        if (verbose) fprintf(stderr, "Embedding file      %s\n", yytext);
        push_include(yytext);
      }
  /* name ends with a quote char, but no ext -> include as ccode state
   * this occurs when importing a library .h/.c The .c is only included
   * when instr->runtime option is true
   * Each library is embedded once per run (tracked in lib_instances).
   */
  [^\"\n]+\"  {
        char *tmp0, *tmp1;
        yytext[yyleng - 1] = '\0';
        tmp0 = str_dup(yytext);
        if (!symtab_lookup(lib_instances, tmp0))
        {
          tmp1 = str_cat(tmp0, ".h", NULL);
          if (instrument_definition->include_runtime)
          {
            switch_line = str_cat(tmp0, ".c", NULL);
          }
          else
            fprintf(stderr,"Dependency: %s.o\n", tmp0);

          BEGIN(ccode);
          if (verbose) fprintf(stderr, "Embedding library   %s\n", tmp1);

          push_include(tmp1);
          symtab_add(lib_instances, tmp0, NULL);
          str_free(tmp1);
        }
        else
        {
          BEGIN(ccode);  /* library was previously embedded */
        }
        str_free(tmp0);
      }
  /* name contains char, including quote and \n -> not valid */
  [\"\n].*  {
        print_error("Error in %%include statement "
                    "on line %d of file '%s': %s.\n",
              instr_current_line, instr_current_filename, yytext);
        BEGIN(ccode);
      }
  } /* end  cinclname */

 /* C++ "//"-style comments - ignore everything to end of line. */
"//"[^\n]*{EOL} instr_current_line++; /* Ignore comment. */

 /* C-style comments. */
"/*"    BEGIN(ccomment);
<ccomment>{
 [^*\n]*  /* Ignore comment. */
 [^*\n]*{EOL} instr_current_line++; /* Ignore comment. */
 "*"+[^*/\n]* /* Ignore comment. */
 "*"+[^*/\n]*{EOL}  instr_current_line++; /* Ignore comment. */
 "*"+"/"  BEGIN(INITIAL); /* End of comment. */
}

[ \t]+      /* Ignore whitespace. */
[ \t]*{EOL}   instr_current_line++; /* Ignore whitespace. */

 /* End of a file: either the end of everything, or pop back to the file that
    included it. Three cases on pop:
    - a "lib.c" is pending after "lib.h" (switch_line): include it right away;
    - an autoloaded component ended (visible_eof): end this nested yyparse()
      call, so read_component() regains control;
    - a plain %include ended: resume scanning the includer silently. */
<INITIAL,ccode><<EOF>> {
      if(file_stack_ptr <= 0)
      {
        /* EOF on main instrument file. */
        yyterminate();
      }
      else
      {
        --file_stack_ptr;
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(file_stack[file_stack_ptr].buffer);
        instr_current_filename = file_stack[file_stack_ptr].filename;
        instr_current_line = file_stack[file_stack_ptr].line;
        switch_line        = file_stack[file_stack_ptr].switch_line;
        if (switch_line)
        {
          char *tmp0;
          tmp0 = str_dup(switch_line);
          str_free(switch_line);
          switch_line = NULL;
          BEGIN(ccode);
          if (verbose) fprintf(stderr, "Embedding library   %s\n", tmp0);
          push_include(tmp0);
          str_free(tmp0);
        }
        else
        {
          BEGIN(file_stack[file_stack_ptr].oldstate);
          if(file_stack[file_stack_ptr].visible_eof)
            yyterminate();
        }

      }
    }

.               {
      print_error("Invalid character `%s' "
                  "on line %d of file '%s'.\n",
            yytext, instr_current_line, instr_current_filename);
      return TOK_INVALID;
    }



%%

/* User code section. */

/* This flag is set when autoloading component definitions to make the lexer
   output the special initial token to switch the parser to restricted
   mode. */
int parse_restricted = FALSE;

/* Prepare to run lexical analysis on new file. */
void
lex_new_file(FILE *file)
{
  parse_restricted = FALSE;
  BEGIN(initial_token);
  yyrestart(file);
}


/* This handles the details of switching the lexer to a new file. */
static void
push_file(FILE *file, int restricted, int visible_eof)
{
  if(file_stack_ptr >= MAX_INCLUDE)
    fatal_error("Too deeply nested includes "
                "on line %d of file '%s'.\n",
              instr_current_line, instr_current_filename);
  file_stack[file_stack_ptr].buffer = YY_CURRENT_BUFFER;
  file_stack[file_stack_ptr].filename = instr_current_filename;
  file_stack[file_stack_ptr].line = instr_current_line;
  file_stack[file_stack_ptr].oldstate = YY_START;
  file_stack[file_stack_ptr].visible_eof = visible_eof;
  file_stack[file_stack_ptr].switch_line = switch_line;
  file_stack_ptr++;

  instr_current_line = 1;

  yy_switch_to_buffer(yy_create_buffer(file, YY_BUF_SIZE));
  parse_restricted = restricted;
}

/* Handle a new %include file. The include is transparent to the parser: it
   just sees more tokens. */
void
push_include(char *name)
{
  FILE *file;

  file = open_file_search(name);
  if(file == NULL)
    fatal_error("Cannot open include file '%s' "
                "on line %d of file '%s'.\n",
              name, instr_current_line, instr_current_filename);
  push_file(file, FALSE, FALSE);
  /* copy: callers pass yytext or temporaries, but the name is kept in
     code blocks and error messages long after */
  instr_current_filename = str_dup(name);
  instr_current_line = 1;
}

/* Handle a new autoincluded file (uses recursive parser call).
   Restricted mode + initial_token makes the nested parse start with
   TOK_RESTRICTED (component grammar only); visible_eof makes the EOF of the
   .comp file end that nested parse. */
void
push_autoload(FILE *file)
{
  push_file(file, TRUE, TRUE);
  BEGIN(initial_token);
}
