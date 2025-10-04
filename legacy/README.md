# Legacy NOVAS C support files

This folder contains additional files and tools that were part of the original NOVAS C 3.1 distribution, but are not 
integral to the current version of __SuperNOVAS__. They have been modified to follow best coding practices, but 
otherwise provide the same function as in NOVAS C. Here is a summary of the contents of this folder:

 - `novascon.c` / `novascon.h` -- These are legacy constant provided by NOVAS. They were problematic for two reasons.
   First because they were declared as constant storage types, not macros, which meant that one could neither check if 
   the symbols were defined, or change their values for the application. And second, because these constants had 
   simplistic names (such as `C`, `F`, or `AU`), which results in an elevated chance of namespace conflicts with other 
   dependencies of an application -- aggrevated by the first problem of not being able to define these conditionally. 
   Many of these constants are also available as macros in __SuperNOVAS__, with more distinct names (usually with just 
   `NOVAS_` prepended, such as `C` &rarr; `NOVAS_C`). However, if your existing NOVAS C application relied on the 
   definitions of `novascon.h`, the header and source are provided for you to compile and link against if need be.
   
 - `nutation.h` -- These definitions are now part of `novas.h`, but they are made available here spearately also for
   any application, which may have referenced these definitions directly.
   
 - `readeph0.c` -- A dummy implementation for the NOVAS C `readeph()` function. There is really no reason why you 
   should ever need this, since it does not do anything useful, but since it was offered by the NOVAS C distribution, 
   we offer it also.
   
 - `solarsystem.h` Legacy NOVAS C header with prototypes for `solarsystem()` / `solarsystem_hp()` functions, e.g.
   for `solsys1.c` and `solsys2.c`.
   
 - `solsys1.c / .h`, `eph_manager.c / .h` -- A self-contained NOVAS C `solarsystem()` / `solarsystem_hp()` 
   implementation that works directly with older JPL ephemeris data (DE200 -- DE421). If your legacy NOVAS C 
   application relies on it, you can build __SuperNOVAS__ with the `SOLSYS_SOURCE` set to these source files (e.g. 
   `legacy/solsys1.c legacy/eph_manager.c`, and adding the folder containing them to the compiler include directories,
   e.g. via adding `-Ilegacy` to `CPPFLAGS`.
   
 - `solsys2.c / .h`, `jplint.f` -- A NOVAS C `solarsystem()` / `solarsystem_hp()` implementation interfacing to 
   JPL's Fortran PLEPH library. `solsys2.c` calls `jplint()` / `jplihp()`, which in turn provides a Fortran adapter 
   interface to PLEPH. In __SuperNOVAS__ you are much better off using the CALCEPH or CSPICE plugins, which provide 
   more modern and more versatile support for ephemeris data. However, if you have an old NOVAS C application, which 
   uses  `solsys2.c` with the stock (unmodified) `jplint.f` interface module, then you may use the source provided 
   here to build that application against __SuperNOVAS__ as is. Before that you must build __SuperNOVAS__ with the 
   `SOLSYS_SOURCE` set to e.g. `legacy/solsys1.c`, modifying (as needed) `jplint.f` and then compiling it, and finally 
   linking `supernovas` along with `jplint.o` and the PLEPH library.
   
 - `cio_file.c` -- A NOVAS C tool for converting the CIO vs GCRS locator table `CIO_RA.TXT` to the binary 
   `cio_ra.bin`. __SuperNOVAS__ no longer uses such a generated table, but the source code of the tool is provided
   in case you want it for some other purpose.

-----------------------------------------------------------------------------
Copyright (C) 2025 Attila Kovács
