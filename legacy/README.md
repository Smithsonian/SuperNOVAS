# Legacy NOVAS C support files

This folder contains additional files and tools that were part of the original NOVAS C 3.1 distribution, but are not 
integral to the current version of __SuperNOVAS__. Here is a summary of the contents of this folder:

 - `novascon.c` / `novascon.h` -- These are legacy constant provided by NOVAS. They were problematic for two reasons.
   First because they were declated as constant storage types, not macros, which meant that one could neither check if 
   the symbols were already defined, or change their values for the application. And second, because these constants 
   had too simplistic names (such as `C`, `F`, or `AU`), which results in an elevated chance of namespace conflicts
   with other dependencies of an application -- aggrevated by the first problem of not being able to define these
   confitionally. Many of the constants are also available as macros in __SuperNOVAS__, with more distinct names 
   (usually with just `NOVAS_` prepended, such as `C` &rarr; `NOVAS_C`). However, if your existing NOVAS C application
   relied on the definitions of `novascon.h`, the header and source are provided for you to compile and link against
   if need be.
   
 - `readeph0.c` -- NOVAS_C required a fixed implementation for the `readeph()` function. This source module provided a
   dummy implemetation, which did nothing other than satisfy linking dependencies. __SuperNOVAS__ allows to configure 
   ephemeris providers at runtime, making this module unnecessary. However, if you have a legacy NOVAS C application,
   which relies on `readeph0.c`, you can use the provided module, and build __SuperNOVAS__ with the `DEFAULT_READEPH`
   pointing to this source. (See `config.mk` in the main distribution directory).
   
 - `jplint.f` -- The NOVAS C `solsys2.c` solar-system provider module provides an interface to JPL's Fortran PLEPH
   library. `solsys2.c` calls `jplint()` / `jplihp()`, which provide a Fotran adapter interface to PLEPH. In 
   __SuperNOVAS__ you are much better off using the CALCEPH or CSPICE plugins, which provide more modern and more
   versatile support for epehemeris data. However, if you have a NOVAS C application, which uses `solsys2` with the
   stock (unmodified) `jplint.f` interface module, then you may use the source provided here to build that 
   application against __SuperNOVAS__.

-----------------------------------------------------------------------------
Copyright (C) 2025 Attila Kovács
