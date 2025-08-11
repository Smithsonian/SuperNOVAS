/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "SuperNOVAS", "index.html", [
    [ "Changelog", "md_CHANGELOG.html", [
      [ "Table of Contents", "index.html#autotoc_md50", null ],
      [ "Introduction", "index.html#autotoc_md52", [
        [ "Related links", "index.html#autotoc_md53", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md55", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md57", null ],
      [ "Building and installation", "index.html#autotoc_md59", null ],
      [ "Building your application with SuperNOVAS", "index.html#autotoc_md61", [
        [ "Legacy linking <tt>solarsystem()</tt> and <tt>readeph()</tt> modules", "index.html#autotoc_md62", null ],
        [ "Legacy modules: a better way...", "index.html#autotoc_md63", null ]
      ] ],
      [ "Celestial coordinate systems (old vs. new)", "index.html#autotoc_md65", null ],
      [ "Example usage", "index.html#autotoc_md67", [
        [ "Calculating positions for a sidereal source", "index.html#autotoc_md68", [
          [ "Specify the object of interest", "index.html#autotoc_md69", null ],
          [ "Specify the observer location", "index.html#autotoc_md70", null ],
          [ "Specify the time of observation", "index.html#autotoc_md71", null ],
          [ "Set up the observing frame", "index.html#autotoc_md72", null ],
          [ "Calculate an apparent place on sky", "index.html#autotoc_md73", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "index.html#autotoc_md74", null ],
          [ "Calculate rise, set, and transit times", "index.html#autotoc_md75", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "index.html#autotoc_md76", null ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "index.html#autotoc_md77", null ]
      ] ],
      [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md79", [
        [ "CALCEPH integration", "index.html#autotoc_md80", null ],
        [ "NAIF CSPICE toolkit integration", "index.html#autotoc_md81", null ],
        [ "Universal ephemeris data / service integration", "index.html#autotoc_md82", null ],
        [ "Legacy support for (older) JPL major planet ephemerides", "index.html#autotoc_md83", [
          [ "Planets via <tt>eph_manager</tt>", "index.html#autotoc_md84", null ],
          [ "Planets via JPL's <tt>pleph</tt> FORTRAN interface", "index.html#autotoc_md85", null ]
        ] ],
        [ "Legacy linking of custom ephemeris functions", "index.html#autotoc_md86", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md88", [
        [ "Prerequisites to precise results", "index.html#autotoc_md89", null ]
      ] ],
      [ "Tips and tricks", "index.html#autotoc_md91", [
        [ "SuperNOVAS and C++", "index.html#autotoc_md92", null ],
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md93", null ],
        [ "Multi-threaded calculations", "index.html#autotoc_md94", null ],
        [ "Physical units", "index.html#autotoc_md95", null ],
        [ "String times and angles", "index.html#autotoc_md96", null ],
        [ "String dates", "index.html#autotoc_md97", null ]
      ] ],
      [ "Runtime debug support", "index.html#autotoc_md99", null ],
      [ "Representative benchmarks", "index.html#autotoc_md101", null ],
      [ "SuperNOVAS added features", "index.html#autotoc_md103", [
        [ "New functionality highlights", "index.html#autotoc_md104", [
          [ "New in v1.0", "index.html#autotoc_md105", null ],
          [ "New in v1.1", "index.html#autotoc_md106", null ],
          [ "New in v1.2", "index.html#autotoc_md107", null ],
          [ "New in v1.3", "index.html#autotoc_md108", null ],
          [ "New in v1.4", "index.html#autotoc_md109", null ]
        ] ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md110", null ]
      ] ],
      [ "Release schedule", "index.html#autotoc_md112", null ],
      [ "[1.4.2-rc1] - 2025-08-11", "md_CHANGELOG.html#autotoc_md1", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md2", null ]
      ] ],
      [ "[1.4.1] - 2025-07-21", "md_CHANGELOG.html#autotoc_md3", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md4", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md5", null ]
      ] ],
      [ "[1.4.0] - 2025-06-02", "md_CHANGELOG.html#autotoc_md6", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md7", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md8", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md9", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md10", null ]
      ] ],
      [ "[1.3.1] - 2025-05-07", "md_CHANGELOG.html#autotoc_md11", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md12", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md13", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md14", null ]
      ] ],
      [ "[1.3.0] - 2025-04-15", "md_CHANGELOG.html#autotoc_md15", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md16", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md17", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md18", null ]
      ] ],
      [ "[1.2.0] - 2025-01-15", "md_CHANGELOG.html#autotoc_md19", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md20", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md21", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md22", null ]
      ] ],
      [ "[1.1.1] - 2024-10-28", "md_CHANGELOG.html#autotoc_md23", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md24", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md25", null ]
      ] ],
      [ "[1.1.0] - 2024-08-04", "md_CHANGELOG.html#autotoc_md26", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md27", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md28", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md29", null ]
      ] ],
      [ "[1.0.1] - 2024-05-13", "md_CHANGELOG.html#autotoc_md30", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md31", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md32", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md33", null ]
      ] ],
      [ "[1.0.0] - 2024-03-01", "md_CHANGELOG.html#autotoc_md34", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md35", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md36", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md37", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md38", null ]
      ] ]
    ] ],
    [ "Contributing to SuperNOVAS", "md_CONTRIBUTING.html", null ],
    [ "Astrometric Positions the Old Way", "md_LEGACY.html", [
      [ "Calculating positions for a sidereal source", "md_LEGACY.html#autotoc_md41", [
        [ "Specify the object of interest", "md_LEGACY.html#autotoc_md42", null ],
        [ "Spefify the observer location", "md_LEGACY.html#autotoc_md43", null ],
        [ "Specify the time of observation", "md_LEGACY.html#autotoc_md44", null ],
        [ "Specify Earth orientation parameters", "md_LEGACY.html#autotoc_md45", null ],
        [ "Calculate apparent positions on sky", "md_LEGACY.html#autotoc_md46", [
          [ "A. True apparent R.A. and declination", "md_LEGACY.html#autotoc_md47", null ],
          [ "B. Azimuth and elevation angles at the observing location", "md_LEGACY.html#autotoc_md48", null ]
        ] ]
      ] ],
      [ "Calculating positions for a Solar-system source", "md_LEGACY.html#autotoc_md49", null ]
    ] ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"md_LEGACY.html#autotoc_md41",
"novas_8h.html#a918c8244e44c69f1589d9f4bf5355d16",
"parse_8c.html#a10a24fc28fa74efa75abf566bb61af5b",
"structon__surface.html#a76714bdbc5c536fa77dfb14533ff82a9"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';