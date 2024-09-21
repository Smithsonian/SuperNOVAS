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
      [ "Table of Contents", "index.html#autotoc_md28", null ],
      [ "Introduction", "index.html#autotoc_md30", [
        [ "Related links", "index.html#autotoc_md31", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md33", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md35", null ],
      [ "Building and installation", "index.html#autotoc_md37", [
        [ "Building your application with SuperNOVAS", "index.html#autotoc_md38", null ]
      ] ],
      [ "Example usage", "index.html#autotoc_md40", [
        [ "Note on alternative methodologies", "index.html#autotoc_md41", null ],
        [ "Calculating positions for a sidereal source", "index.html#autotoc_md42", [
          [ "Specify the object of interest", "index.html#autotoc_md43", null ],
          [ "Spefify the observer location", "index.html#autotoc_md44", null ],
          [ "Specify the time of observation", "index.html#autotoc_md45", null ],
          [ "Set up the observing frame", "index.html#autotoc_md46", null ],
          [ "Calculate an apparent place on sky", "index.html#autotoc_md47", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "index.html#autotoc_md48", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "index.html#autotoc_md49", null ],
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md50", null ],
        [ "Performance considerations", "index.html#autotoc_md51", null ],
        [ "Multi-threaded calculations", "index.html#autotoc_md52", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md54", null ],
      [ "SuperNOVAS specific features", "index.html#autotoc_md56", [
        [ "Newly added functionality", "index.html#autotoc_md57", [
          [ "Added in v1.1", "index.html#autotoc_md58", null ]
        ] ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md59", null ]
      ] ],
      [ "External Solar-system ephemeris data or services", "index.html#autotoc_md61", [
        [ "1. Universal ephemeris data / service integration", "index.html#autotoc_md62", null ],
        [ "2. Built-in support for (old) JPL major planet ephemerides", "index.html#autotoc_md63", [
          [ "2.1. Planets via <tt>eph_manager</tt>", "index.html#autotoc_md64", null ],
          [ "2.b. Planets via JPL's <tt>pleph</tt> FORTRAN interface", "index.html#autotoc_md65", null ]
        ] ],
        [ "3. Explicit linking of custom ephemeris functions", "index.html#autotoc_md66", null ]
      ] ],
      [ "Release schedule", "index.html#autotoc_md68", null ],
      [ "[Unreleased]", "md_CHANGELOG.html#autotoc_md1", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md2", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md3", null ]
      ] ],
      [ "[1.1.0] - 2024-08-04", "md_CHANGELOG.html#autotoc_md4", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md5", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md6", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md7", null ]
      ] ],
      [ "[1.0.1] - 2024-05-13", "md_CHANGELOG.html#autotoc_md8", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md9", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md10", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md11", null ]
      ] ],
      [ "[1.0.0] - 2024-03-01", "md_CHANGELOG.html#autotoc_md12", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md13", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md14", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md15", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md16", null ]
      ] ]
    ] ],
    [ "Contributing to SuperNOVAS", "md_CONTRIBUTING.html", null ],
    [ "SuperNOVAS: Astrometric Positions the Old Way", "md_LEGACY.html", [
      [ "Calculating positions for a sidereal source", "md_LEGACY.html#autotoc_md19", [
        [ "Specify the object of interest", "md_LEGACY.html#autotoc_md20", null ],
        [ "Spefify the observer location", "md_LEGACY.html#autotoc_md21", null ],
        [ "Specify the time of observation", "md_LEGACY.html#autotoc_md22", null ],
        [ "Specify Earth orientation parameters", "md_LEGACY.html#autotoc_md23", null ],
        [ "Calculate apparent positions on sky", "md_LEGACY.html#autotoc_md24", [
          [ "A. True apparent R.A. and declination", "md_LEGACY.html#autotoc_md25", null ],
          [ "B. Azimuth and elevation angles at the observing location", "md_LEGACY.html#autotoc_md26", null ]
        ] ]
      ] ],
      [ "Calculating positions for a Solar-system source", "md_LEGACY.html#autotoc_md27", null ]
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
"novas_8c.html#aed179ad664f4ff023deb540140f3c2ef",
"novas_8h.html#ace5e59ce1564bf6c61946c290bb05367a882f5d97444df44485d360ce298843b1",
"super_8c.html#aebad3c9b51f62cb7b31a686b5ec890a9"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';