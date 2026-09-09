# Union libraries and globals handled by code generator

## Status

This is a proposed change

## Context

The original Union components were developed under McStas 2.X and used global variables for a number 
of lists that were used to transfer imformation between components. 
In McStas 3.X that was no longer possible, so an Union_init and Union_stop method was added, this 
were required to surround all the Union components used in an instrument. The init part loaded the
required libraries and set up the global variables, then all other Union components would read the
variables from that component, needing the name on it as an input (standard convetion is init), so
almost always omitted. Stop added switch statements for process functions, avoiding cases that are
not actually in the current instrument using the preprocessor. The included files are in share and
do need updating when new processes / geometries are added. They are called union-init.c, union-lib.c
and union-suffix.c.

## Decision

Let the code generator insert the union-init.c, union-lib.c and union-suffix.c files at the
appropriate positions if a Union_master or a Union_master_GPU is detected.

Keep the Union_init and Union_stop components for a transition period, but they are empty and
only show a deprication warning.

Remove the init input on all Union components.

Add errors on all non master Union components to let the user know a master is necessary to compile
the instrument.

## Consequences

It becomes easier to use the Union components as there is no need to place Union_init or Union_stop.

Existing instruments that do not use the init parameter for individual components still work and only
show the deprication warnings. Using mismatch between version of the components and code generator
could lead to problems, but the same preprocessor guards are used. Main issue would be an instrument
using a folder of new Union components with an old code generator, in that case libraries would not
be loaded.

Still easy for developers to find the .c files in share to update them.

## Behaviour

Simplifies use of Union components by avoiding Union_init and Union_stop.
