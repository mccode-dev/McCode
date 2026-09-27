# Union libraries and globals handled by code generator

## Status

This is a proposed change

## Context

The original Union components were developed under McStas 2.X and used global variables for a number 
of lists that were used to transfer information between components. 
In McStas and McXtrace 3.X that was no longer possible, so the `Union_init` and `Union_stop` component were 
added. They were required to surround all the Union components used in an instrument. The `Union_init` 
loaded the required libraries and set up the global variables, then all other Union components would read the
variables from that component, needing the name on it as the init parameter. The init parameter had a default
of `init` so if the `Union_init` instance was given that name, there was no need to specify that parameter. 
`Union_stop` component added switch statements for process functions, avoiding cases that are not actually in
the current instrument using the preprocessor. The included files are in share and do need updating when new 
processes / geometries are added. They are called `union-init.c`, `union-lib.c` and `union-suffix.c`.

## Decision

Let the code generator insert the `union-init.c`, `union-lib.c` and `union-suffix.c` files at the
appropriate positions if a `Union_master` or a `Union_master_GPU` is detected. It works by searching
for a `Union_master`/`Union_master_GPU` component, if found it adds `read_table-lib.h`, `union-lib.c` 
and `union-init.c` before all component SHARE sections and `union-suffix.c` after all component SHARE
sections. It also creates a Union preprocessor macro that can be used to detect whether Union is active 
in this instrument.

All Union components emit compile errors if the Union preprocessor macro is not defined, this is done 
in each component. 

Keep the `Union_init` and `Union_stop` components for a transition period, but they now only contain
the code emitting the deprecation warning.

Remove the init input on all Union components.

## Consequences

It becomes easier to use the Union components as there is no need to place `Union_init` or `Union_stop`.

Instruments using the `init` parameter in any Union component will need that removed as the parameter
was removed.

Existing instruments that do not use the init parameter for individual components but have the `Union_init`
and/or `Union_stop` components still work and only show the deprecation warnings. A mismatch between 
version of the components and code generator could lead to problems, but the same preprocessor guards are used.
Main issue would be an instrument using a folder of new Union components with an old code generator, in that case 
the intentional compile errors would be triggered.

No change the location of .c files in share that needs to be easily accesible to developers.

Cogen would need to be updated if `Union_master` or `Union_master_GPU` name is changed.

## Behaviour

- In valid use, Union systems no longer need the `Union_init` and `Union_stop`
- Using the `Union_init` and `Union_stop` won't break instruments, but show deprecation warnings
- Using the init parameter in Union components will fail as the parameter is removed
- Compilation fails if any Union component is used without a `Union_master` or `Union_master_GPU`



