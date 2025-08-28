<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>novas-calceph.h</name>
    <path>include/</path>
    <filename>novas-calceph_8h.html</filename>
    <member kind="function">
      <type>int</type>
      <name>novas_calceph_use_ids</name>
      <anchorfile>novas-calceph_8h.html</anchorfile>
      <anchor>a5531b937a5bb5491cb475d2536346ad8</anchor>
      <arglist>(enum novas_id_type idtype)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_calceph</name>
      <anchorfile>novas-calceph_8h.html</anchorfile>
      <anchor>a35fc86a939602ec910dbdfde180d7f31</anchor>
      <arglist>(t_calcephbin *eph)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_calceph_planets</name>
      <anchorfile>novas-calceph_8h.html</anchorfile>
      <anchor>ab86ec03898e658c86c671a8b6399e331</anchor>
      <arglist>(t_calcephbin *eph)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>novas-cspice.h</name>
    <path>include/</path>
    <filename>novas-cspice_8h.html</filename>
    <member kind="function">
      <type>int</type>
      <name>cspice_add_kernel</name>
      <anchorfile>novas-cspice_8h.html</anchorfile>
      <anchor>ae02d26964099f56c375dc78172ad135b</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cspice_remove_kernel</name>
      <anchorfile>novas-cspice_8h.html</anchorfile>
      <anchor>a38e3b3155cad3b3822fbc3e553060ab1</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_cspice</name>
      <anchorfile>novas-cspice_8h.html</anchorfile>
      <anchor>a9b44218bca5fce1e9e5103494c15dde4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_cspice_ephem</name>
      <anchorfile>novas-cspice_8h.html</anchorfile>
      <anchor>afb078e1df9b5b08999d24e68d7b4ecdc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_cspice_planets</name>
      <anchorfile>novas-cspice_8h.html</anchorfile>
      <anchor>a943549e395b19f6a91064e3d6fa1c577</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>novas.h</name>
    <path>include/</path>
    <filename>novas_8h.html</filename>
    <includes id="nutation_8h" name="nutation.h" local="no" import="no" module="no" objc="no">nutation.h</includes>
    <includes id="solarsystem_8h" name="solarsystem.h" local="no" import="no" module="no" objc="no">solarsystem.h</includes>
    <class kind="struct">cat_entry</class>
    <class kind="struct">in_space</class>
    <class kind="struct">novas_delaunay_args</class>
    <class kind="struct">novas_frame</class>
    <class kind="struct">novas_matrix</class>
    <class kind="struct">novas_observable</class>
    <class kind="struct">novas_orbital</class>
    <class kind="struct">novas_orbital_system</class>
    <class kind="struct">novas_planet_bundle</class>
    <class kind="struct">novas_timespec</class>
    <class kind="struct">novas_track</class>
    <class kind="struct">novas_transform</class>
    <class kind="struct">object</class>
    <class kind="struct">observer</class>
    <class kind="struct">on_surface</class>
    <class kind="struct">ra_of_cio</class>
    <class kind="struct">sky_pos</class>
    <member kind="define">
      <type>#define</type>
      <name>ASEC2RAD</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8a0ef80e44076b86bc707e9414d85d76</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ASEC360</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a320b4181c798436981b4520175baad89</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>BARYC</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0c92a54a656043a455ea8ae0ace6c57e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CAT_ENTRY_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0a185da3a347343e5dd21d41a598d0fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DE405_AU</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af303bf91582509624aa40c6a87eb95ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFAULT_CIO_LOCATOR_FILE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4caf1f6e33a208827eb99df7e387d6d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFAULT_GRAV_BODIES_FULL_ACCURACY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4cb74da554b236c835302480688cfaac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEFAULT_GRAV_BODIES_REDUCED_ACCURACY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aec64bf67d5a03b2b4fb2583e5918f1de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>DEG2RAD</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af7e8592d0a634bd3642e9fd508ea8022</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>HELIOC</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5414c24d23ad004a73ec69b1aad9f3e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IN_SPACE_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a406038a5cbe32b7e0e7233f6145e4cf5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>M_PI</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae71449b1cc6e6250b91f539153a7a0d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_ARCMIN</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aacce04f99c506d5ff67ff4daf1ccda95</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_ARCSEC</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0fb993148083be267e8321f4f5227527</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_AU</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abc8f275a2d431e59a1ec9e276abaca1a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_AU_KM</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab94b49a9d44c6078c5ea12171029eb96</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_AU_SEC</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a610c7260c1879f89913bf7a5549b4649</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_C</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5fb4713ad9802152e40a83bf3c1ee242</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_C_AU_PER_DAY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7c474aa996c5ccc0c03b1006ea5b30e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_CIO_CACHE_SIZE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab7f2aaab5b9fe0be5d77d4cccc3745a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_DAY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0b0c2c5ccc61c8740de2e2ac277cd836</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_DEFAULT_WAVELENGTH</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4f7dee9d4c9db4cf201af8383c72d4db</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_DEGREE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6d9698dbaf6260d7fe8a531a0e96d2a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_DELAUNAY_ARGS_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abf34092276df6f7e9ddaab92a197179b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_EARTH_ANGVEL</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac50f27bc66238525124427551a956ec3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_EARTH_FLATTENING</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1830a25e886366263653167369213d8b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_EARTH_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abad802a863342aebe9a40587b4e47bab</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_EARTH_RADIUS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a139f16e134bf6119b6e04c8272c85668</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_EMB_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a209cb6db7acbde105772520150f238de</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_EQUATOR_TYPES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ade30c5c72c918b2428e803e3f4430ceb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_FRAME_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab1c90a8754103ec1bfcf241c75fe9371</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_G_EARTH</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af125f22b69933024d6a8513b205dca8b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_G_SUN</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6d955f313593b4a56fef202708d5722b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_GPS_TO_TAI</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6355388ff4a44836eda6eb1e97da60cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_GRS80_FLATTENING</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae283901f9a884acc6c5b88c6afb040b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_GRS80_RADIUS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5518e094e90ed1053aeb03c41d531452</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_HOURANGLE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aee2680f03d1dfa4c9fbeea2be806dbf7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_JD_B1900</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af03a1f0412f23cd6edb323297646eb30</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_JD_B1950</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3785083ae52bbbc1e749dbedd7e5d85e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_JD_HIP</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4100ecc5d9badfa07301024baffe9ded</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_JD_J2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a31bf4a08940bfa3e97ae6a1abee80422</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_JD_MJD0</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac266a346b8e22946f50079897b0baefa</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_JD_START_GREGORIAN</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a509afe7095f4d597125e9bfd6d0399ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_JUPITER_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8f05bb45f646e41b78bbe79175114d57</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_KM</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0a84af1a760ed2aaa01dd93e9e22bdf1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_KMS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7822b81ad3f75b448f446ed4da774e75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_MAJOR_VERSION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac73482ee628f676b44bb1476688f223e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_MARS_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a26aff9425f7df9a4559da8daac284cc5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_MATRIX_IDENTITY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a26fdd758cdef560688d38b20aacae68f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_MATRIX_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad10fc7f05b49bc1ac1110817dc25834e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_MERCURY_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab40e33a03ee368c2496b54f9b222fc89</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_MINOR_VERSION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af7a746fdca818e9d4c4aa27bf65a62be</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_MOON_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4cbd3c168b9357e3af2274252a6d1a5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_NEPTUNE_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a341603d54c8cfc1ae9bb23034ba3ef5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_OBJECT_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9980b82723819fa345030b9ddad48061</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_OBJECT_TYPES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a60037f5a7c8fe2b7cf8893cf461598d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_OBSERVABLE_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>affe559df5bc0f5de1e091037fabfda92</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_OBSERVER_PLACES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7af86c48f77394688f4ad43eb20760f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_ORBIT_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a85243a6b9eee259d3fb84ee1a587e065</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_ORBITAL_SYSTEM_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a61b4f45764a44517b867442de7749cd4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_ORIGIN_TYPES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aab57b6b4c8c96ed32414fe3964813e5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_PLANET_BUNDLE_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4cc558a8e366bfea3be40777ea0fcbc3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_PLANET_GRAV_Z_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0ae90838b446c7d4c1d3386ec8857410</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_PLANET_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad34a88c2c04b624f8f85fc86e7935967</anchor>
      <arglist>(num, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_PLANET_NAMES_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abbd19f57e31db2ac91cbe22f9a95f390</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_PLANETS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1853540eae9f8699b6aaff9c0b518e08</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_PLUTO_BARYCENTER_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aead51c753fb72d7c603298c654795ea1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_PLUTO_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a451602e14e6a5512d0e969e4d9878c34</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_REFERENCE_SYSTEMS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae55994629ca97e634b1bb97aafb3f49b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_REFRACTION_MODELS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1932e30210aa533136e5848089adf492</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_RMASS_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5ddc7a011476a8cffaec1866beaa0095</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SATURN_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2cd97d849a1f16d6b85b8d2976f56472</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SOLAR_CONSTANT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5e72489b5d24fa737d04cb9ec303b71e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SOLAR_RADIUS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae548ecdfc646914084936acb32e9c993</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SSB_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aebdfbe27bf764b69018291054ad9130d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SUN_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6afeda74c9df859ae6504eebfee01092</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SYSTEM_B1950</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaf00ed93616e93d09df832d485a0e115</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SYSTEM_FK4</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae7ab88078608cf4f4499cedcf4c0ca75</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SYSTEM_FK5</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abf795b4cd3cc6205b88508bcc77586c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SYSTEM_HIP</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a773bb459e1ec420cd7d93d064e70eb7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SYSTEM_ICRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9d170036a074eff7634bdb23e54871b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_SYSTEM_J2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae67fecc99f46afc9b4f12fc27b4fbe6c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_TAI_TO_TT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a68aa01f6e3e0f40bbe1b5ccab3144fda</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_TIMESCALES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7505fe27b163e97b60bcf4406eadffa7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_TIMESPEC_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>addf3ffbfe4ed05288b689e861600194f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_TRACK_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abd9e6922d311d47ee2a7a4faf6b67971</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_TRANSFORM_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6a255712d5e1dcc8c58e16d453501766</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_TRANSFORM_TYPES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab5988d52596448e8248d94a4f59d69a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_URANUS_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad873085559e1141614264d15c4885a87</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_VENUS_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af126344dc5cbee61df9e55bd92322839</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_VERSION_STRING</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afc8e7b02d26a13c380dcff4ec2d6530e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_WGS84_FLATTENING</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab0f64e76b53aa2c526d9b35930339e2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_WGS84_RADIUS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a46a8bebd4fc88bb5056eb820d961aa4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_WOBBLE_DIRECTIONS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3903b6d439f4a25a6482fd073996d9ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>OBSERVER_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8b1485a1563288c8d060d935c21e3a63</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ON_SURFACE_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad8e67e1b12b3699a361f87e95d333334</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ON_SURFACE_LOC</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4fe5e792ca262b3f45fe992b4c49e9b4</anchor>
      <arglist>(lon, lat, alt)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>RAD2DEG</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac5a945020d3528355cda82d383676736</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SIZE_OF_CAT_NAME</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af32c72297e36b77bead56044d23082f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SIZE_OF_OBJ_NAME</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a977a43db51d544f728ea8a22b4a53783</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SKY_POS_INIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a462bdac0f370f0c5b421ae5b3f3e35f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SUPERNOVAS_MAJOR_VERSION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5ab52d5db18c081b6313ed8b1993ebb7</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SUPERNOVAS_MINOR_VERSION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaed93e356acfc0d15d0f64ae0793b4f0</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SUPERNOVAS_PATCHLEVEL</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9b926427954233b94ebd6925d86f9f81</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SUPERNOVAS_RELEASE_STRING</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aadef42ba39811e058ee9efe2c6f7d151</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SUPERNOVAS_VERSION_STRING</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a44de9d1672290d153a1dfb0685886208</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TWOPI</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4912c64aec0c943b7985db6cb61ff83a</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>double(*</type>
      <name>RefractionModel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad4899a6116cfbbe17400e7ef0c8426a9</anchor>
      <arglist>)(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_accuracy</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3cba2735c32163fab062b1a904a83b56</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_FULL_ACCURACY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3cba2735c32163fab062b1a904a83b56aebffd24170ffab949609a8dcfc201660</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_REDUCED_ACCURACY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3cba2735c32163fab062b1a904a83b56a2047c8bae25010adb60d895a83079846</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_calendar_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa8cfc18bfdf8ebd0198065c9d07e3bba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ROMAN_CALENDAR</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa8cfc18bfdf8ebd0198065c9d07e3bbaa73cfd3e5c66bdfdf63a2d792251e0d13</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ASTRONOMICAL_CALENDAR</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa8cfc18bfdf8ebd0198065c9d07e3bbaa1b6339c57b7baba0957b935fd37cc8ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_GREGORIAN_CALENDAR</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa8cfc18bfdf8ebd0198065c9d07e3bbaa90f19bf1771802f4ad20cf14660c8322</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_cio_location_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aee3f2f5b3807612025e2ce25d368ff2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CIO_VS_GCRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aee3f2f5b3807612025e2ce25d368ff2fa9928da41a130a60eb1762cbe52e691ce</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CIO_VS_EQUINOX</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aee3f2f5b3807612025e2ce25d368ff2fae8c1b78646fefbd0ffdcae083b597241</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_date_format</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9f2b43ae4b048783dc09c2a3d9b46ca4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_YMD</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9f2b43ae4b048783dc09c2a3d9b46ca4ab4320d2be90ff55e278e6171f6516bab</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DMY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9f2b43ae4b048783dc09c2a3d9b46ca4a3f31b27c8e879d17b89c34395e278203</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_MDY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9f2b43ae4b048783dc09c2a3d9b46ca4ab13cba930f91e5e907bfc72489ffbc61</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_debug_mode</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9d8d2a1f5c06b488e4c817bc9877e3ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DEBUG_OFF</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9d8d2a1f5c06b488e4c817bc9877e3baa0b25a908023b92a2f4f42114736de9cf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DEBUG_ON</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9d8d2a1f5c06b488e4c817bc9877e3baadc10e15517b94fdd3d9eba3359909c65</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DEBUG_EXTRA</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9d8d2a1f5c06b488e4c817bc9877e3baaa03b175422e9603e87ab87f93a45c07c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_dynamical_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a61f6463d05c29930c667981fdcb1f341</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DYNAMICAL_MOD</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a61f6463d05c29930c667981fdcb1f341a5b6fd4adbb29dd7a90e5100730eda1e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DYNAMICAL_TOD</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a61f6463d05c29930c667981fdcb1f341a39ca3eead3c36784e06eff2a8598aea0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DYNAMICAL_CIRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a61f6463d05c29930c667981fdcb1f341a3bf824e91c250babd516eadef253d1fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_earth_rotation_measure</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9e882d9c9055f187d3a8dc9b019f0609</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>EROT_ERA</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9e882d9c9055f187d3a8dc9b019f0609ab07b6e5ed11fd50e7c2287b5b6626b46</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>EROT_GST</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9e882d9c9055f187d3a8dc9b019f0609a1d56a4fb59b567049ed1937bfcdf0f62</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_equator_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2eff9980cb9db96ab41fb564c1b5c014</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_MEAN_EQUATOR</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2eff9980cb9db96ab41fb564c1b5c014a90d1ef6cedd7a9fdfd7e538ecea5a040</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TRUE_EQUATOR</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2eff9980cb9db96ab41fb564c1b5c014acca4381f43619e421b26d79c1c77593f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_GCRS_EQUATOR</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2eff9980cb9db96ab41fb564c1b5c014a1248329939f130a4bef0d3ef9ed95c66</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_equatorial_class</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3789b2c39c26f01bb1bffe8ebc953467</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_REFERENCE_CLASS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3789b2c39c26f01bb1bffe8ebc953467ab580d06bb093ef496695125e2bef1798</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_DYNAMICAL_CLASS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3789b2c39c26f01bb1bffe8ebc953467aba4a23444c495f90061ec1a906585faa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_equinox_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab970f5de5d5e04874cb31865459b0b4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TRUE_EQUINOX</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab970f5de5d5e04874cb31865459b0b4fa1096785746d1a9bc678ea2c51fc21055</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_frametie_direction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae8b91a9b4fe4af63c7054d1eacdb996e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>J2000_TO_ICRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae8b91a9b4fe4af63c7054d1eacdb996ea7e5dc966849295dce29fb339b960da4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ICRS_TO_J2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae8b91a9b4fe4af63c7054d1eacdb996ea8557604d6604356e6620b42f78aec999</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_nutation_direction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae2d3d508d68d268bec838bdc013d0ada</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NUTATE_TRUE_TO_MEAN</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae2d3d508d68d268bec838bdc013d0adaa20ce1e5229410b6286bcc98a7ce69573</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NUTATE_MEAN_TO_TRUE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae2d3d508d68d268bec838bdc013d0adaacd57b0308467899ec6304aa26dcc8f4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_object_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1eecb357eeea0de4759e9c8d55af238f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_PLANET</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1eecb357eeea0de4759e9c8d55af238fad1eeaddaa96b028742a8ba424e311aa8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_EPHEM_OBJECT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1eecb357eeea0de4759e9c8d55af238fa58f22833170aee0959154d3a7be6c269</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_CATALOG_OBJECT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1eecb357eeea0de4759e9c8d55af238fa4f9ab9cec85498c41f90c33794d032e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ORBITAL_OBJECT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1eecb357eeea0de4759e9c8d55af238fa17562d2cd1b0b56d59d31f6ca51c759a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_observer_place</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6140c39d1526b137f827e2091ecc36b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_OBSERVER_AT_GEOCENTER</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6140c39d1526b137f827e2091ecc36b3aca54b3a0c7d5b31d09090a1f55ff72b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_OBSERVER_ON_EARTH</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6140c39d1526b137f827e2091ecc36b3aadb64b75d1bf5344760b942f241e52e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_OBSERVER_IN_EARTH_ORBIT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6140c39d1526b137f827e2091ecc36b3aca418efc57ac159cf541c324c008eb27</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_AIRBORNE_OBSERVER</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6140c39d1526b137f827e2091ecc36b3a1665bf146ca1a4d11380bf4d7d9d7058</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SOLAR_SYSTEM_OBSERVER</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6140c39d1526b137f827e2091ecc36b3a82474207bac139bc2a8826b9d317a42c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_origin</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a208120c5fa3e12853eb37cb99d3e27be</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_BARYCENTER</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a208120c5fa3e12853eb37cb99d3e27beade504a1c5ccf790ce9d8ad52797bb94c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_HELIOCENTER</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a208120c5fa3e12853eb37cb99d3e27bea13c0711a025b9aebd4186ab9127cbd7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SSB</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045aaeb39686d0e998c9188b47c31ee9c6b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_MERCURY</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a40678cd248bd2edca8f59ddfee20ab74</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_VENUS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a7c452cc654265f03e78801f9aead83de</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_EARTH</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045adc941f5a3613952d6d6102281e70e5c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_MARS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a479f3eb097c0aeb4ac7dc3ae2e6c376e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_JUPITER</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a994baf0c30caeaaee7b8b4d5a12921a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SATURN</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a03b12ea0f93a1a1045d7bcccd613c6f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_URANUS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a71d987d5d11d3a162807cdf0c994ad28</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_NEPTUNE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045af12d26e56aa375fd2aa0c0edfd1c9ae3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_PLUTO</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a1255a982524abb49bf076014d8d6acb5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SUN</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a170e1c704bd29c8d46f66d4d9a03d9c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_MOON</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a9fcf58133828600a062725ced448cfcf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_EMB</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045ad1785b619d72877c4eb78d61aedf9e1a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_PLUTO_BARYCENTER</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a219df36b21dc4476656e708d14d08045a3503bba5abad8c20c76a6ca87dccf677</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_pole_offset_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a70c3951615b1ecf42818c79893678543</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>POLE_OFFSETS_DPSI_DEPS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a70c3951615b1ecf42818c79893678543a2126557509f896a0aeb435795fe1da47</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>POLE_OFFSETS_X_Y</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a70c3951615b1ecf42818c79893678543ad304f43b1bf8becb63fc4d972f8b1f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_reference_ellipsoid</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5928ca4a09c2883b58e951a4582255b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_GRS80</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5928ca4a09c2883b58e951a4582255b3ab3062efc3b970895460dc617eec49b25</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_WGS84</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5928ca4a09c2883b58e951a4582255b3aeeba068882459151b93e0d9df7377e7a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_reference_plane</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9ee18ab5f8fdc009913c11f04026122f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ECLIPTIC_PLANE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9ee18ab5f8fdc009913c11f04026122fac0d0495aebd235ecd346b8932cc1943e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_EQUATORIAL_PLANE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9ee18ab5f8fdc009913c11f04026122fa8ab742cbe451963ba66f7bfbdbafb05b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_reference_system</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_GCRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651ea8c3e21e3d68b39b0b0ca28f99b8cacf4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TOD</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651eaa4bcd4fdb15ba77023241689a5608df3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_CIRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651eacbee2de517939c156e61fb5f9631e044</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ICRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651ea5bf6e497c776a4a8168b23db062e8a10</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_J2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651ead110200de7f232d9110467c0862733d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_MOD</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651ea5a2a55ff66f97310a9641d7ffa60b8dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TIRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651ea524041fc2059f8432b76fad1b8edb89f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ITRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ba038ea34eb901ccfb8f785708c651ea826bda7744c11bc1560f4db1848a17c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_refraction_model</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abe97e2d459a30db9d0f67d8c60af3b81</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_NO_ATMOSPHERE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abe97e2d459a30db9d0f67d8c60af3b81ace535f58f84fa374a973aa9d4e6a52e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_STANDARD_ATMOSPHERE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abe97e2d459a30db9d0f67d8c60af3b81a7ed64c314f06fdf9acfa72beb531e73d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_WEATHER_AT_LOCATION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abe97e2d459a30db9d0f67d8c60af3b81aaf3bef1a45c5d59df8e779da09cecb68</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_RADIO_REFRACTION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abe97e2d459a30db9d0f67d8c60af3b81a165a8cec5e4cd5aeb6f26530022a4761</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_WAVE_REFRACTION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abe97e2d459a30db9d0f67d8c60af3b81a75c80913155eddef17af869d6b0d62f6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_refraction_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8c77758e10b1d3dc8dbd3ab0b694e468</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_REFRACT_OBSERVED</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8c77758e10b1d3dc8dbd3ab0b694e468a29a320ab3459dafa0d18e7ebef3ae8fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_REFRACT_ASTROMETRIC</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8c77758e10b1d3dc8dbd3ab0b694e468a4255a64975ec93df2f228c63c38096a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_separator_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acb27ba6ec7f4ed7486f4419f8803f40e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SEP_COLONS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acb27ba6ec7f4ed7486f4419f8803f40ead0a359b222721a18e8c7c0260ffd7e50</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SEP_SPACES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acb27ba6ec7f4ed7486f4419f8803f40ea0164600609e97493df66ed70ca926c2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SEP_UNITS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acb27ba6ec7f4ed7486f4419f8803f40ea0def47a09a4c410ccb20b64cf204c76e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_SEP_UNITS_AND_SPACES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acb27ba6ec7f4ed7486f4419f8803f40ea9bf01302bc64e585197728c074d0bbee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_timescale</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TCB</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0ba8301aa6dff381beba373826e137c88c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TDB</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0baf5d992bdf01b4f3fd7b22e3e24c6974d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TCG</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0ba5935a299729a67971b12a78ee682f89a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TT</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0baadb0d1437b46022bca75d4e7ab0542c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_TAI</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0ba328f2523f4f064c5252f4ac858bddc4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_GPS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0bac29f535fdae4e29e1baeccacf8f0b98d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_UTC</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0ba01787464888b92c37b8340174c03cc8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_UT1</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa642c4ccc7195dc62fd0dc56567ebd0baa6fc4520426c4c9cbe497ab87f2ad583</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_transform_type</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ace5e59ce1564bf6c61946c290bb05367</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PROPER_MOTION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ace5e59ce1564bf6c61946c290bb05367a197e40a273d8132592b60efe3f87a065</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PRECESSION</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ace5e59ce1564bf6c61946c290bb05367a6833dbc8bfb3b8ba2cdd7a2da0b82353</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CHANGE_EPOCH</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ace5e59ce1564bf6c61946c290bb05367a882f5d97444df44485d360ce298843b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CHANGE_J2000_TO_ICRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ace5e59ce1564bf6c61946c290bb05367a84da44f7510d8c99353b1b1c6bb4faf8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CHANGE_ICRS_TO_J2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ace5e59ce1564bf6c61946c290bb05367a1b1e38f90bc3bc3c9f1c04aa2c959444</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_wobble_direction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aab27106a2d5c2c7fa64b2f208deff7f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WOBBLE_ITRS_TO_TIRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aab27106a2d5c2c7fa64b2f208deff7f5a88d1bc6832a20ed34c4dab5753864851</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WOBBLE_TIRS_TO_ITRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aab27106a2d5c2c7fa64b2f208deff7f5a4b792b9dcf18048019f1a4fb486b7c97</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WOBBLE_ITRS_TO_PEF</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aab27106a2d5c2c7fa64b2f208deff7f5ac1fc144f868517a2268a56343cc218a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>WOBBLE_PEF_TO_ITRS</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aab27106a2d5c2c7fa64b2f208deff7f5a6148979c7c2a121fc0852ec129eb4b10</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>aberration</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a033f1bf4b01e0e21a0e05e13c19c02f6</anchor>
      <arglist>(const double *pos, const double *vobs, double lighttime, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>accum_prec</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad1de5b481e41cf193c199bd0dc5084bd</anchor>
      <arglist>(double t)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>app_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac28dd9a362982cfce150751001a96a8a</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>app_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a906b17f06b42e31a29cafad4856f565f</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>app_to_cirs_ra</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3fa57a154f2f423612736e5e3a5addbf</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double ra)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>astro_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9ea79f60493a3a21a9228d048fa5a9a2</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>astro_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9530c770e83db2cbc5a31d8d1f8c36f4</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>bary2obs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1d58fd371e7604f75df315365672941e</anchor>
      <arglist>(const double *pos, const double *pos_obs, double *out, double *restrict lighttime)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cal_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a044f1359abcf0a039ac6452a95729b5a</anchor>
      <arglist>(double tjd, short *restrict year, short *restrict month, short *restrict day, double *restrict hour)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cel2ter</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6217852bfd7d3a58d3266d0daadfc6bc</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class coordType, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cel_pole</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8937180de2ee1545d19b8fc840f35fe2</anchor>
      <arglist>(double jd_tt, enum novas_pole_offset_type type, double dpole1, double dpole2)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_array</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afc894b55420495a727a90d3d85188ef4</anchor>
      <arglist>(double jd_tdb, long n_pts, ra_of_cio *restrict cio)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_basis</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a51bfbe9b491f50851326b2522dc2a29f</anchor>
      <arglist>(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy, double *restrict x, double *restrict y, double *restrict z)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_location</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a65ffaaa2b527ed03149eadc7568f3714</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *restrict ra_cio, short *restrict loc_type)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_ra</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acc331c69187b3077b98280bfb57f6670</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double *restrict ra_cio)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cirs_to_app_ra</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af90d52a6527d9b62b8ac35d8c1ac7c6b</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double ra)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_gcrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac0ba048cd1732d0d0398397396d158bc</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_itrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa2768d89d499a7155f90a6fd8c176d8d</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_tod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9cac4f3a18d4c14939519e7123b4d504</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>d_light</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad799864f637c483fb49491bde5a93e25</anchor>
      <arglist>(const double *pos_src, const double *pos_body)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>e_tilt</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab75165e2d69231da10c67de025ee05d8</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *restrict mobl, double *restrict tobl, double *restrict ee, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>ecl2equ</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a41ca0cfd79ad07c9e857ba02fa94928e</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ecl2equ_vec</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6fa1eada41005ac70290d0bb5df1ec26</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>ee_ct</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>add7a260017fad0d59c943a0a8d935fcf</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ephemeris</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a713982e1f2bae0a72b39d03007f00d23</anchor>
      <arglist>(const double *restrict jd_tdb, const object *restrict body, enum novas_origin origin, enum novas_accuracy accuracy, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>equ2ecl</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a035047336b16951b8b850066e74210a9</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec, double *restrict elon, double *restrict elat)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>equ2ecl_vec</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa05ecc76c1ccc0c3445f8b642d6c5311</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>equ2gal</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a288b49c03bc29704c8dd5acf8baca80e</anchor>
      <arglist>(double ra, double dec, double *restrict glon, double *restrict glat)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>equ2hor</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad932a8617973ef06e4a5c955b7897bb0</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const on_surface *restrict location, double ra, double dec, enum novas_refraction_model ref_option, double *restrict zd, double *restrict az, double *restrict rar, double *restrict decr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>era</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aef1f8daaad470353648de90576aefc66</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>frame_tie</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa11d2253a44261370af8d0c32f0715e9</anchor>
      <arglist>(const double *in, enum novas_frametie_direction direction, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>fund_args</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a312b5ab1a004b179d80463a07ca7da8f</anchor>
      <arglist>(double t, novas_delaunay_args *restrict a)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gal2equ</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>adcce501bd3f8f70974e7f55527162f83</anchor>
      <arglist>(double glon, double glat, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>gcrs2equ</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abd38071a8d4bdc342feaecfac2fc7856</anchor>
      <arglist>(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_cirs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a51ea4a016336dd3cf7783061c14d165c</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_j2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aeea4b5085c7267714492fae6108fb975</anchor>
      <arglist>(const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_mod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a75de6cca190c5fbc3056db726f317c7f</anchor>
      <arglist>(double jd_tdb, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_tod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9c900b81a718a184281a8a0b733b4db7</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>geo_posvel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ada5ca37110e66c8b65f623c9e26ae52e</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *restrict obs, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>novas_nutation_provider</type>
      <name>get_nutation_lp_provider</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a416a96426a0b3c52c1d5fc6452357366</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>get_ut1_to_tt</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4d7470ca705efa1953e8b4beab77c6b6</anchor>
      <arglist>(int leap_seconds, double dut1)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>get_utc_to_tt</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab7611704bab9ce717744a8b5575c5378</anchor>
      <arglist>(int leap_seconds)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>grav_def</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a48f90ac20950507fad9b880a7f7f2383</anchor>
      <arglist>(double jd_tdb, enum novas_observer_place unused, enum novas_accuracy accuracy, const double *pos_src, const double *pos_obs, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_planets</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae36bc31e1dad8a68f94f7a6414f0519f</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, const novas_planet_bundle *restrict planets, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>grav_redshift</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3ce6b306bc183387d0aaac97abbd82e8</anchor>
      <arglist>(double M_kg, double r_m)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_undef</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaae33249717912663bbb00be4fa4e6c3</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *pos_app, const double *pos_obs, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_undo_planets</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1b107a24b9fadc79cc5df8c6f5505d16</anchor>
      <arglist>(const double *pos_app, const double *pos_obs, const novas_planet_bundle *restrict planets, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_vec</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a08a008572e2746a70bec19739b5fb7e6</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, const double *pos_body, double rmass, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>hor_to_itrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a19f4f6c7d942dcba5ad2a4d2a60affad</anchor>
      <arglist>(const on_surface *restrict location, double az, double za, double *restrict itrs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>ira_equinox</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aea0e1028baf14d16d9de800e36a17086</anchor>
      <arglist>(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_cirs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a12953f66c0f774284f0700ae785784d0</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_hor</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aef3c1f8e4c443e51e65003018449768a</anchor>
      <arglist>(const on_surface *restrict location, const double *restrict itrs, double *restrict az, double *restrict za)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_tod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3f6ae564cfac3766a21e0754905e5f7c</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>j2000_to_gcrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9d51ca5c972013024755c644c96d0586</anchor>
      <arglist>(const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>j2000_to_tod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a02fa1ee652c20160055cbd2c7bd9ca63</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>julian_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7cd85bc0a0248f84d399cbbce9fe9546</anchor>
      <arglist>(short year, short month, short day, double hour)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>light_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afb2cc37c7bdcd34c5c167afe092aed68</anchor>
      <arglist>(double jd_tdb, const object *restrict body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *pos_src_obs, double *restrict tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>light_time2</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5c940b62732d74e6c6b1c844931aaa76</anchor>
      <arglist>(double jd_tdb, const object *restrict body, const double *restrict pos_obs, double tlight0, enum novas_accuracy accuracy, double *p_src_obs, double *restrict v_ssb, double *restrict tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>limb_angle</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a022957936bc7c5a3e9651776689c9380</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, double *restrict limb_ang, double *restrict nadir_ang)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>adcb9b273dfda5d97a1c01379a16997f9</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, double ut1_to_tt, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acd56c41e811a002d6280b3b315730fbd</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *restrict star, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_airborne_observer</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa423a88cd3f434642fc89d47390184f4</anchor>
      <arglist>(const on_surface *location, const double *vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_cat_entry</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a39b2e39d29f1b57c5f785284ba7ca43f</anchor>
      <arglist>(const char *restrict star_name, const char *restrict catalog, long cat_num, double ra, double dec, double pm_ra, double pm_dec, double parallax, double rad_vel, cat_entry *star)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_cat_object</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab9b7a622486f2904615ceed1ba94cd8f</anchor>
      <arglist>(const cat_entry *star, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_cat_object_sys</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5104d7db66ce2d8c61c7ad21599fb45e</anchor>
      <arglist>(const cat_entry *star, const char *restrict system, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_ephem_object</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa22303f98df2d63edf6d8452ff3a43b2</anchor>
      <arglist>(const char *name, long num, object *body)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_in_space</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac0ca6f330e1795ce811df95986400632</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, in_space *loc)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_object</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a859c6c09e05d95d047c92408454a1b1e</anchor>
      <arglist>(enum novas_object_type, long number, const char *name, const cat_entry *star, object *source)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_observer</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8c060b573c5054e77a9fd9df2d785838</anchor>
      <arglist>(enum novas_observer_place, const on_surface *loc_surface, const in_space *loc_space, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_at_geocenter</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aabb3ab0d99aa6e487cdcf6b454949b75</anchor>
      <arglist>(observer *restrict obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_in_space</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6c146e4d10152d5e1ffb978f2fc962be</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_on_surface</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5eb76fc6e7be571e228d23aee3823cb9</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, observer *restrict obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_on_surface</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a42c084465aedee75965f74e23e6571fe</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, on_surface *restrict loc)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4c7846be95299769f4d0377bf127bcf5</anchor>
      <arglist>(enum novas_planet num, object *restrict planet)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_cat_entry</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a11aa3ce6bcdda5b7065b38d6cfaf63d3</anchor>
      <arglist>(const char *name, double ra, double dec, double z, cat_entry *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_object</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae579a49d84696a913fd0d9f348bb5b11</anchor>
      <arglist>(const char *name, double ra, double dec, double z, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_object_sys</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a257abae9391aec835dc2b94d9788b9f9</anchor>
      <arglist>(const char *name, double ra, double dec, const char *restrict system, double z, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_solar_system_observer</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9c3f1a1bc3fd9ac6d211806913d2d529</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean_obliq</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af182ee8cc4239f581f746aa974827d61</anchor>
      <arglist>(double jd_tdb)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>mean_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaf51bf000e993807101a34adf423d36b</anchor>
      <arglist>(double jd_tt, double tra, double tdec, enum novas_accuracy accuracy, double *restrict ira, double *restrict idec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>mod_to_gcrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad9cc07f0e6af9339096e15a440b113e5</anchor>
      <arglist>(double jd_tdb, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_app_to_geom</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab7db8026c598e8e126a4725ac3b388bf</anchor>
      <arglist>(const novas_frame *restrict frame, enum novas_reference_system sys, double ra, double dec, double dist, double *restrict geom_icrs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_app_to_hor</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3cd947f2e48d6c5cc88ad7a1bab9b8ca</anchor>
      <arglist>(const novas_frame *restrict frame, enum novas_reference_system sys, double ra, double dec, RefractionModel ref_model, double *restrict az, double *restrict el)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_cartesian_to_geodetic</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abb9a26a4658c037b4721fb97cea08ed0</anchor>
      <arglist>(const double *restrict x, enum novas_reference_ellipsoid ellipsoid, double *restrict lon, double *restrict lat, double *restrict alt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>novas_case_sensitive</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8f832afe92680a11c3489aaa224ac2e2</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_change_observer</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2972b3eafa2d7f6c29cff34bad9fcf0d</anchor>
      <arglist>(const novas_frame *orig, const observer *obs, novas_frame *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abaac8a4ea28bc8b811277051e3168468</anchor>
      <arglist>(const char *restrict date)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_date_scale</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a66656c3f5a1256f4bfc3ed2e168c70c2</anchor>
      <arglist>(const char *restrict date, enum novas_timescale *restrict scale)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_day_of_week</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7ee576309aa1977e9b2a3824286f725c</anchor>
      <arglist>(double tjd)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_day_of_year</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae6eaa610037b0828d473062ab8722f47</anchor>
      <arglist>(double tjd, enum novas_calendar_type calendar, int *restrict year)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>novas_debug</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5f35633b872751df4dacd2fb2cbe4ac3</anchor>
      <arglist>(enum novas_debug_mode mode)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_diff_tcb</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4818ab9be2f0891091c719db01bcfa98</anchor>
      <arglist>(const novas_timespec *t1, const novas_timespec *t2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_diff_tcg</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab904df0f504d07bbf73f84e2a1c43e07</anchor>
      <arglist>(const novas_timespec *t1, const novas_timespec *t2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_diff_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abdcc8e194d79787838d1ffc064449932</anchor>
      <arglist>(const novas_timespec *t1, const novas_timespec *t2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_eop</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9fc52f6fd358463d0c1ca7aa651e65f6</anchor>
      <arglist>(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict xp, double *restrict yp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_eop_at_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a087d587e90b4e2138c0dba12d727696e</anchor>
      <arglist>(const novas_timespec *restrict time, double *restrict dxp, double *restrict dyp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_libration</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2576d1eb803afb956373e67fe9c438d1</anchor>
      <arglist>(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict xp, double *restrict yp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_ocean_tides</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abcd371629ee17cd1f1512ff0687db46e</anchor>
      <arglist>(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict xp, double *restrict yp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_dms_degrees</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aca89d2a0f49afad45dbddffc09c3adae</anchor>
      <arglist>(const char *restrict dms)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_e2h_offset</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a13cd0b80eaa7bc682013f7e8ffbdbd6f</anchor>
      <arglist>(double dra, double ddec, double pa, double *restrict daz, double *restrict del)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_epa</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a095ba0ee644dc00c11dcdc5616bc96b0</anchor>
      <arglist>(double ha, double dec, double lat)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_epoch</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af4e18885b7d173eb3fe5f18b06f0b4d4</anchor>
      <arglist>(const char *restrict system)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_equ_sep</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4f7e06b2fa5b3145b37aec4f9c2d1602</anchor>
      <arglist>(double ra1, double dec1, double ra2, double dec2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_equ_track</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a57f4a449754c88cfc87df658a6bc4cb2</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame, double dt, novas_track *restrict track)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_frame_lst</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a273fd76d83b8e04ced14558c69fd175c</anchor>
      <arglist>(const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_gast</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a348c4fd2a4a046e9768038075717b693</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geodetic_to_cartesian</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a908dd3e00d6b27aa73679afc32c31f24</anchor>
      <arglist>(double lon, double lat, double alt, enum novas_reference_ellipsoid ellipsoid, double *x)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_posvel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac6652ee3257621080dac342313d0a0a7</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame, enum novas_reference_system sys, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_to_app</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4817703c9357f702a27812584c91f87c</anchor>
      <arglist>(const novas_frame *restrict frame, const double *restrict pos, enum novas_reference_system sys, sky_pos *restrict out)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_debug_mode</type>
      <name>novas_get_debug_mode</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a29cad12337acfce5f1e370320dcefb79</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_get_split_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a32d8336084b65fac14d092ab6198c27d</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_timescale timescale, long *restrict ijd)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_get_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae30e3dd8e30bdcf4e8486cfce696fb98</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_timescale timescale)</arglist>
    </member>
    <member kind="function">
      <type>time_t</type>
      <name>novas_get_unix_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afa1a1d804ca70050418429f971c2045a</anchor>
      <arglist>(const novas_timespec *restrict time, long *restrict nanos)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_gmst</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a231f535f73e846d3867dbd914054d47b</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_h2e_offset</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7977de3e3edf8120b50cbd026c155f47</anchor>
      <arglist>(double daz, double del, double pa, double *restrict dra, double *restrict ddec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_hms_hours</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a696e44a2d1c788f9f335611efd7ed958</anchor>
      <arglist>(const char *restrict hms)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_to_app</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa57cbcae2001fece2ee4a2babfbc5f89</anchor>
      <arglist>(const novas_frame *restrict frame, double az, double el, RefractionModel ref_model, enum novas_reference_system sys, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_track</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1f29810393108d15fd8312a2a95158dc</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model, novas_track *restrict track)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_hpa</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a73fcb54e47b2be0aa92810b5f55f7a3c</anchor>
      <arglist>(double az, double el, double lat)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_inv_refract</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a81da80ae3ab1d0a444f39bde2af3a732</anchor>
      <arglist>(RefractionModel model, double jd_tt, const on_surface *restrict loc, enum novas_refraction_type type, double el0)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_invert_transform</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad2081e123024609d3071b33b183502b7</anchor>
      <arglist>(const novas_transform *transform, novas_transform *inverse)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_iso_timestamp</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4d6da26d0a7d7d904f25d2be00030b25</anchor>
      <arglist>(const novas_timespec *restrict time, char *restrict dst, int maxlen)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_itrf_transform</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afd98ed01148f1291018809d77785527f</anchor>
      <arglist>(int from_year, const double *restrict from_coords, const double *restrict from_rates, int to_year, double *to_coords, double *to_rates)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_itrf_transform_eop</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3c8c9b8f709d1e5a7aa3a915f8fafcbb</anchor>
      <arglist>(int from_year, double from_xp, double from_yp, double from_dut1, int to_year, double *restrict to_xp, double *restrict to_yp, double *restrict to_dut1)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_jd_from_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a54eee84c46900c80ca9966857c217cc8</anchor>
      <arglist>(enum novas_calendar_type calendar, int year, int month, int day, double hour)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_jd_to_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8ed2fb55bcf86d9f1f434a2f832356a2</anchor>
      <arglist>(double tjd, enum novas_calendar_type calendar, int *restrict year, int *restrict month, int *restrict day, double *restrict hour)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_los_to_xyz</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a601d246673742cf72a5dbe1449c97560</anchor>
      <arglist>(const double *los, double lon, double lat, double *xyz)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_lsr_to_ssb_vel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aad80bf94be2efc36c340994829df8207</anchor>
      <arglist>(double epoch, double ra, double dec, double vLSR)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_frame</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abfee69517c7432733c8382c88371c983</anchor>
      <arglist>(enum novas_accuracy accuracy, const observer *obs, const novas_timespec *time, double dx, double dy, novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_transform</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4d179c8573319fc25c775ce56ba055dc</anchor>
      <arglist>(const novas_frame *frame, enum novas_reference_system from_system, enum novas_reference_system to_system, novas_transform *transform)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_norm_ang</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a85334614a0f399e0d3cbde68075d0766</anchor>
      <arglist>(double angle)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_object_sep</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afa5f3a823a895bed01876df6b2a00370</anchor>
      <arglist>(const object *source1, const object *source2, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_offset_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a01f98b2b8033c8dbacaeab37e9198e91</anchor>
      <arglist>(const novas_timespec *time, double seconds, novas_timespec *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_optical_refraction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae4511a80a1defbb1e1fdc29c289e7de4</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4934da85b9722ef43a5b238f3e1971e4</anchor>
      <arglist>(const char *restrict date, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_date_format</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5ec9d054b7d363b3db1d065507a1ba41</anchor>
      <arglist>(enum novas_calendar_type calendar, enum novas_date_format format, const char *restrict date, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_degrees</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad3d04d065e76a5921995e96f67c24c41</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_dms</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa8d11652c5deb9e2ad78fd70ccac3378</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_hms</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a22e1bcf36379e0244b665c3e22d5fcba</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_hours</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aff8c2c3fae3be867d3fc73972a158514</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_iso_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad53b3be5b92ea244cfb0347feb3bec9f</anchor>
      <arglist>(const char *restrict date, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_timescale</type>
      <name>novas_parse_timescale</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a72245b3ea6099c573278f721a4bfd3d9</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_dms</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad778a440540ed687572b78b675bebd60</anchor>
      <arglist>(double degrees, enum novas_separator_type sep, int decimals, char *restrict buf, int len)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_hms</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a713869893ad7e58c53e44b37a3bcf768</anchor>
      <arglist>(double hours, enum novas_separator_type sep, int decimals, char *restrict buf, int len)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_timescale</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a084e5a2dcd20860af7176754d3a32202</anchor>
      <arglist>(enum novas_timescale scale, char *restrict buf)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_radio_refraction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6b49f92f8f818f2272613e3432185a39</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_refract_wavelength</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac673c4fef94847b796dc2aa2f4d0807d</anchor>
      <arglist>(double microns)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_rises_above</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaf38f64304fa0b00c9c56cead62043f3</anchor>
      <arglist>(double el, const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sep</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9f33de2b26a19a5e20827e9e918b3375</anchor>
      <arglist>(double lon1, double lat1, double lon2, double lat2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_split_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad47bcb1e3c5df56a0ffdf1a127e52598</anchor>
      <arglist>(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1, novas_timespec *restrict time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af4a1a5180905a7d8f76aeafe3f9a8ebb</anchor>
      <arglist>(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *restrict time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_unix_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9d54ea7c6cdbc75ee78cf552a7e1858d</anchor>
      <arglist>(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *restrict time)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sets_below</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a23af09cd8e5ec5ef4d0b03a347a4add0</anchor>
      <arglist>(double el, const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_sky_pos</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac365a1e015ae1f875dc5a66fd5435b2f</anchor>
      <arglist>(const object *restrict object, const novas_frame *restrict frame, enum novas_reference_system sys, sky_pos *restrict out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_ssb_to_lsr_vel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a99ed25baa029f3980924da8c8af54d1a</anchor>
      <arglist>(double epoch, double ra, double dec, double vLSR)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_standard_refraction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab04bff3b97c132e358e11c3a5b7a3081</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_str_degrees</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6d6ada07aab7ce09fb8ea7b12660ebcd</anchor>
      <arglist>(const char *restrict dms)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_str_hours</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3ed26b6af870f6dbc353f651563afd4a</anchor>
      <arglist>(const char *restrict hms)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_time_gst</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaf53fcad3fade814381cef3d06779a8c</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_time_lst</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9e99ea879ba0ddc1d9fbefc4b32b1ef1</anchor>
      <arglist>(const novas_timespec *restrict time, double lon, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_timescale</type>
      <name>novas_timescale_for_string</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab344242721ca00709d4dc482a7934e2a</anchor>
      <arglist>(const char *restrict str)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_timestamp</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac8e83c48005d5aabb3e7bbf8d9f8ed81</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_timescale scale, char *restrict dst, int maxlen)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_track_pos</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a225a2e33684d2c1fe769e5aa718a182a</anchor>
      <arglist>(const novas_track *track, const novas_timespec *time, double *restrict lon, double *restrict lat, double *restrict dist, double *restrict z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_sky_pos</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acfcfb90637cd1fdb25ef9afd1bda8e30</anchor>
      <arglist>(const sky_pos *in, const novas_transform *restrict transform, sky_pos *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_vector</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa77895c891e9eeb4b1541f03acfd154e</anchor>
      <arglist>(const double *in, const novas_transform *restrict transform, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_transit_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad04f1ebd7b2d39e5258b152fec231001</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_uvw_to_xyz</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a16c920eee105431247e9a1d9bf1c3021</anchor>
      <arglist>(const double *uvw, double ha, double dec, double *xyz)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_v2z</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa54cd00024b294d38fa29433c017c65f</anchor>
      <arglist>(double vel)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_wave_refraction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a7b9ad6dfbd86e4faa2d8068c7d64fac4</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_xyz_to_los</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a14deb6fdce80ba213e1f1d5238306e17</anchor>
      <arglist>(const double *xyz, double lon, double lat, double *los)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_xyz_to_uvw</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a87867e8e7fc7decaa44777a696465353</anchor>
      <arglist>(const double *xyz, double ha, double dec, double *uvw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z2v</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a63621cea3b4985c1bfcc4514a7aa3fb1</anchor>
      <arglist>(double z)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z_add</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a493ab751eee5804db77157c37e94e5c3</anchor>
      <arglist>(double z1, double z2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z_inv</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1130ad2ce520e948c71f03a25a713075</anchor>
      <arglist>(double z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nutation</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a69df5671751c09a7575963b94ad1151e</anchor>
      <arglist>(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nutation_angles</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6e9ced7b0aec544ad7b057b58f777ec4</anchor>
      <arglist>(double t, enum novas_accuracy accuracy, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_planets</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a07490a134a685fe0842b2b4816559292</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *restrict pos_obs, int pl_mask, novas_planet_bundle *restrict planets)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_posvel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0e4e5ff7c37e6521ca75ae62aaf4050e</anchor>
      <arglist>(double jd_tdb, double ut1_to_tt, enum novas_accuracy accuracy, const observer *restrict obs, const double *restrict geo_pos, const double *restrict geo_vel, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>place</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aca09ed0d2379ac4a4841c9e3edeeee4c</anchor>
      <arglist>(double jd_tt, const object *restrict source, const observer *restrict location, double ut1_to_tt, enum novas_reference_system coord_sys, enum novas_accuracy accuracy, sky_pos *restrict output)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_cirs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a49bd2b398f7eab3e98823cb1a7403ed5</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_gcrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aea583f6f785fa42f46710288e8a4e083</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_icrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad465eafe65e3d701fc4bd57efa793a12</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_j2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1ea9b377426b1800535e85fab47bfc1b</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_mod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac53d150d1f6b96c83d1516a66c699e63</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1504058672e6e3cf24ce6b7189ede005</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, const observer *restrict obs, double ut1_to_tt, enum novas_reference_system system, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_tod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab6908b0eee06b4a44242fba1fd9d506c</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>planet_lon</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aacc6a0daf29b8bdc18f46647a94694d3</anchor>
      <arglist>(double t, enum novas_planet planet)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>precession</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9178358732325399f3a5e3582080ced3</anchor>
      <arglist>(double jd_tdb_in, const double *in, double jd_tdb_out, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>proper_motion</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a24b69e1809a33113d4a041dd3e4082ad</anchor>
      <arglist>(double jd_tdb_in, const double *pos, const double *restrict vel, double jd_tdb_out, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rad_vel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afca24d914664bb60d20fb428177f329a</anchor>
      <arglist>(const object *restrict source, const double *restrict pos_src, const double *vel_src, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun, double *restrict rv)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rad_vel2</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a37f22e463cc06b4e381aa18659e6319d</anchor>
      <arglist>(const object *restrict source, const double *pos_emit, const double *vel_src, const double *pos_det, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec2vector</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae104afd1bc638def9bdbce1801e5054a</anchor>
      <arglist>(double ra, double dec, double dist, double *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a918c8244e44c69f1589d9f4bf5355d16</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, const observer *restrict obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis, double *restrict rv)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0f307c2a37d594a68e99c5c8af6d50c9</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, const observer *restrict obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict rv)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>redshift_vrad</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af56088995d42a0673e03bca1f3891544</anchor>
      <arglist>(double vrad, double z)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>refract</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac7ed2a1a5fb5aea3063d116d3d980eeb</anchor>
      <arglist>(const on_surface *restrict location, enum novas_refraction_model model, double zd_obs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>refract_astro</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afe1d4e96dd66676f5567a279e83e88d9</anchor>
      <arglist>(const on_surface *restrict location, enum novas_refraction_model model, double zd_astro)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_cio_locator_file</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ade20b8035266dfe5b082b12248f5affa</anchor>
      <arglist>(const char *restrict filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_nutation_lp_provider</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a694288eb631d6a69ecf313b7b5b4a677</anchor>
      <arglist>(novas_nutation_provider func)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>sidereal_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6eacba14d3c382d67307856a2ea290e3</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_equinox_type gst_type, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, double *restrict gst)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>spin</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af4090e519d7ce7ab1fcdb24b10126eb4</anchor>
      <arglist>(double angle, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>starvectors</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a27bae6c9efdab763dba9f1ae36a04669</anchor>
      <arglist>(const cat_entry *restrict star, double *restrict pos, double *restrict motion)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tdb2tt</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a90d5fae30728b51151c3daf355ca585b</anchor>
      <arglist>(double jd_tdb, double *restrict jd_tt, double *restrict secdiff)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ter2cel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac8c5ff3cc13126aba2ec5a6cbe187a31</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class coordType, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>terra</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8471f990ad6e3051a0aa584d9cb2d355</anchor>
      <arglist>(const on_surface *restrict location, double lst, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_cirs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>adb48b2acaf76cc3a1e3a1412cc42c232</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_gcrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6871e7fecde44084c7b4e1b8dda5ac70</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_itrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4551e2ac083e34c1b2b61b3805efb9a3</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_j2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6205edb1361e56cc0c7aed0f088f7437</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>topo_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaf3d450c87f545acfa7ab7e2b6c229d9</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, double ut1_to_tt, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>topo_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a86e4f52e832879a33aefc28803e2690f</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *restrict star, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>transform_cat</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a59caeca70d1fdd02e41ed62f20675e6c</anchor>
      <arglist>(enum novas_transform_type, double jd_tt_in, const cat_entry *in, double jd_tt_out, const char *out_id, cat_entry *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>transform_hip</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af2c03d540de7a8bfafaed27eb9a753c1</anchor>
      <arglist>(const cat_entry *hipparcos, cat_entry *hip_2000)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tt2tdb</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a129acca41760da486e1179c93cb9e86d</anchor>
      <arglist>(double jd_tt)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tt2tdb_fp</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad301a9c74bfbdb8b55513cd005787b22</anchor>
      <arglist>(double jd_tt, double limit)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tt2tdb_hp</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5709058076f4aea4eeaeeb000342cd94</anchor>
      <arglist>(double jd_tt)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>unredshift_vrad</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aea89203030ca831260dec18f83081d23</anchor>
      <arglist>(double vrad, double z)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>vector2radec</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a84f672dc1c37e3af374198f29cbcfba9</anchor>
      <arglist>(const double *restrict pos, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0dc172d82eec79f78c2d218c1a7b9cbe</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aaf187624a93349391518f9d6bb3051e5</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>wobble</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aead87744eeb3029ce7d5ffb1801ee652</anchor>
      <arglist>(double jd_tt, enum novas_wobble_direction direction, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>grav_bodies_full_accuracy</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a18647d39520d1a15a137661f698cb12a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>grav_bodies_reduced_accuracy</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a03a4df8961a0cd05f89aca478d2dcd24</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>nutation.h</name>
    <path>include/</path>
    <filename>nutation_8h.html</filename>
    <member kind="typedef">
      <type>int(*</type>
      <name>novas_nutation_provider</name>
      <anchorfile>nutation_8h.html</anchorfile>
      <anchor>a1f0e319630e7a47f17247bf51f7bf36c</anchor>
      <arglist>)(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>iau2000a</name>
      <anchorfile>nutation_8h.html</anchorfile>
      <anchor>a280cbf02b13ca57b3d77451be06e7dbc</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>iau2000b</name>
      <anchorfile>nutation_8h.html</anchorfile>
      <anchor>a6f9cddeb8c3dd906b9b0b5941249ad0c</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nu2000k</name>
      <anchorfile>nutation_8h.html</anchorfile>
      <anchor>abd066c2857d1b1cc06d8bd51b912d841</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solarsystem.h</name>
    <path>include/</path>
    <filename>solarsystem_8h.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_ID_TYPES</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a9a736e59fc9bfdc92b7245aa37d529ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int(*</type>
      <name>novas_ephem_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>aebb3d71006e8d93de242df61510374ad</anchor>
      <arglist>)(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *restrict origin, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="typedef">
      <type>short(*</type>
      <name>novas_planet_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>aae2b06af367d7b0a70a8b89c258b00b0</anchor>
      <arglist>)(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="typedef">
      <type>short(*</type>
      <name>novas_planet_provider_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ae8d7c10a8b9e69b1f0654ef4038482fd</anchor>
      <arglist>)(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>novas_id_type</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ac42df1db80fbcb6f7a72de80d9f9c531</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ID_NAIF</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ac42df1db80fbcb6f7a72de80d9f9c531ab3e6efc1ad89e02f915df25265ec32db</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NOVAS_ID_CALCEPH</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ac42df1db80fbcb6f7a72de80d9f9c531a074c723dfc1f03c82d52347c46ee0719</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>earth_sun_calc</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a580f5949c8a29cdd7bf4787aee891f2b</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>earth_sun_calc_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a338e3436ba86dadf6bd4d7390f9940ec</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enable_earth_sun_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a279bca0519117cee9a3bd4382f38e7ad</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="function">
      <type>novas_ephem_provider</type>
      <name>get_ephem_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>aa6fad38297a49ba78a6bd49cedf889b1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>novas_planet_provider</type>
      <name>get_planet_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a0f0a786b5614a788c02b37a462eae2d2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>novas_planet_provider_hp</type>
      <name>get_planet_provider_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ae45badb450c1ca73920d982e12346c5b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_orbital_object</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a3c5e215babeeba078ca7b3e9be42e555</anchor>
      <arglist>(const char *name, long num, const novas_orbital *orbit, object *body)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_planet</type>
      <name>naif_to_novas_planet</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a1c4b3f67a3b82808dd16ae43dfaf0f86</anchor>
      <arglist>(long id)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_approx_heliocentric</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a9ead627eed7fabd8583f1353e89f42e7</anchor>
      <arglist>(enum novas_planet id, double jd_tdb, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_approx_sky_pos</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a4cb01609f76b0ff8487817b9446a3a62</anchor>
      <arglist>(enum novas_planet id, const novas_frame *restrict frame, enum novas_reference_system sys, sky_pos *restrict out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_helio_dist</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>af4fa5359b8b0167a2780baff17b164a5</anchor>
      <arglist>(double jd_tdb, const object *restrict source, double *restrict rate)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_moon_orbit</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ad6e6c7f6dc2745030e3fd4d407e982bd</anchor>
      <arglist>(double jd_tdb, novas_orbital *restrict orbit)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_planet_orbit</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a63b094e813d8b752a6f3344419e80031</anchor>
      <arglist>(enum novas_planet id, double jd_tdb, novas_orbital *restrict orbit)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_moon_angle</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a121940d7f0ef41297d28fab8e6df5998</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_moon_phase</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a2dbdafd6b07f079815ae52d00e142861</anchor>
      <arglist>(double jd_tdb)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_next_moon_phase</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ad0bd6e212af549309b0fcb792ed8cb79</anchor>
      <arglist>(double phase, double jd_tdb)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_orbit_native_posvel</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a6b88d2403a3051ab32229244cf98d7ab</anchor>
      <arglist>(double jd_tdb, const novas_orbital *restrict orbit, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_orbit_posvel</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a4510a3235a693f122899983901211c30</anchor>
      <arglist>(double jd_tdb, const novas_orbital *restrict orbit, enum novas_accuracy accuracy, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_planet</type>
      <name>novas_planet_for_name</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a2cf7410aa68b980112734227b40890d6</anchor>
      <arglist>(const char *restrict name)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_orbsys_pole</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a706ecf8998b78e48051d2876efc6e01c</anchor>
      <arglist>(enum novas_reference_system type, double ra, double dec, novas_orbital_system *restrict sys)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_illum</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ac780bd907a12957995d36ad916dd4ec1</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_power</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a763f9a9dd2edb4645a5db05184ca7495</anchor>
      <arglist>(double jd_tdb, const object *restrict source)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sun_angle</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a2d1ef3bc6d99aa473511e45c6e0481e9</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>novas_to_dexxx_planet</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ac3eb54acf22dc155879163de89c6865e</anchor>
      <arglist>(enum novas_planet id)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>novas_to_naif_planet</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ab51ac9bdf7736e1c1582e0981046daf3</anchor>
      <arglist>(enum novas_planet id)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_eph_manager</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a841f04bf04e6dd8a5ae96f8ea729090d</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_eph_manager_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a8b471fd187aebc33b1f9fd4c2ff60c90</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_ephem_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a9d0f54c0ab00827768e61a353488b0ee</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_ephem_provider_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a93d0f96d81f3c52e05163643bd23eb7d</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_jplint</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a1300b6559a08ef90ae9b9d8d33a22220</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_jplint_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a2fe8cd1a87f96611f78edc36b4960d63</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>double *</type>
      <name>readeph</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>aa5f22db2fa785675372d45b88f2f1736</anchor>
      <arglist>(int mp, const char *restrict name, double jd_tdb, int *restrict error)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_ephem_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a6097083c438a8d1872c2e00a66360030</anchor>
      <arglist>(novas_ephem_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_planet_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a9904c454bc7148b42d0f927b82ea3973</anchor>
      <arglist>(novas_planet_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_planet_provider_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a418f45a11ce7f92ba0ef817558415640</anchor>
      <arglist>(novas_planet_provider_hp func)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a37c25dbc2683dc9e3b1d187e13f68330</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a17846f3fafcce208dbbca1384e3a7fb0</anchor>
      <arglist>(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>calendar.c</name>
    <path>src/</path>
    <filename>calendar_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>cal_date</name>
      <anchorfile>calendar_8c.html</anchorfile>
      <anchor>a044f1359abcf0a039ac6452a95729b5a</anchor>
      <arglist>(double tjd, short *restrict year, short *restrict month, short *restrict day, double *restrict hour)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>julian_date</name>
      <anchorfile>calendar_8c.html</anchorfile>
      <anchor>a7cd85bc0a0248f84d399cbbce9fe9546</anchor>
      <arglist>(short year, short month, short day, double hour)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_day_of_week</name>
      <anchorfile>calendar_8c.html</anchorfile>
      <anchor>a7ee576309aa1977e9b2a3824286f725c</anchor>
      <arglist>(double tjd)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_day_of_year</name>
      <anchorfile>calendar_8c.html</anchorfile>
      <anchor>ae6eaa610037b0828d473062ab8722f47</anchor>
      <arglist>(double tjd, enum novas_calendar_type calendar, int *restrict year)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_jd_from_date</name>
      <anchorfile>calendar_8c.html</anchorfile>
      <anchor>a54eee84c46900c80ca9966857c217cc8</anchor>
      <arglist>(enum novas_calendar_type calendar, int year, int month, int day, double hour)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_jd_to_date</name>
      <anchorfile>calendar_8c.html</anchorfile>
      <anchor>a8ed2fb55bcf86d9f1f434a2f832356a2</anchor>
      <arglist>(double tjd, enum novas_calendar_type calendar, int *restrict year, int *restrict month, int *restrict day, double *restrict hour)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>cio.c</name>
    <path>src/</path>
    <filename>cio_8c.html</filename>
    <member kind="function">
      <type>short</type>
      <name>cio_array</name>
      <anchorfile>cio_8c.html</anchorfile>
      <anchor>afc894b55420495a727a90d3d85188ef4</anchor>
      <arglist>(double jd_tdb, long n_pts, ra_of_cio *restrict cio)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_basis</name>
      <anchorfile>cio_8c.html</anchorfile>
      <anchor>a51bfbe9b491f50851326b2522dc2a29f</anchor>
      <arglist>(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy, double *restrict x, double *restrict y, double *restrict z)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_location</name>
      <anchorfile>cio_8c.html</anchorfile>
      <anchor>a65ffaaa2b527ed03149eadc7568f3714</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *restrict ra_cio, short *restrict loc_type)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_ra</name>
      <anchorfile>cio_8c.html</anchorfile>
      <anchor>acc331c69187b3077b98280bfb57f6670</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double *restrict ra_cio)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_cio_locator_file</name>
      <anchorfile>cio_8c.html</anchorfile>
      <anchor>ade20b8035266dfe5b082b12248f5affa</anchor>
      <arglist>(const char *restrict filename)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>earth.c</name>
    <path>src/</path>
    <filename>earth_8c.html</filename>
    <member kind="function">
      <type>double</type>
      <name>era</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>aef1f8daaad470353648de90576aefc66</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>geo_posvel</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>ada5ca37110e66c8b65f623c9e26ae52e</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *restrict obs, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>limb_angle</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a022957936bc7c5a3e9651776689c9380</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, double *restrict limb_ang, double *restrict nadir_ang)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_eop</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a3fe699281044873f4137344b28884cea</anchor>
      <arglist>(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict dxp, double *restrict dyp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_eop_at_time</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a087d587e90b4e2138c0dba12d727696e</anchor>
      <arglist>(const novas_timespec *restrict time, double *restrict dxp, double *restrict dyp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_libration</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a9ab448b9d09b5656cf74fe10ab855fd4</anchor>
      <arglist>(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict dxp, double *restrict dyp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_diurnal_ocean_tides</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a90862cafeee0a3534e9e52055a047286</anchor>
      <arglist>(double gmst, const novas_delaunay_args *restrict delaunay, double *restrict dxp, double *restrict dyp, double *restrict dut1)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_gast</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a348c4fd2a4a046e9768038075717b693</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_gmst</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a231f535f73e846d3867dbd914054d47b</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>sidereal_time</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a6eacba14d3c382d67307856a2ea290e3</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_equinox_type gst_type, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, double *restrict gst)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>terra</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>a8471f990ad6e3051a0aa584d9cb2d355</anchor>
      <arglist>(const on_surface *restrict location, double lst, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>wobble</name>
      <anchorfile>earth_8c.html</anchorfile>
      <anchor>aead87744eeb3029ce7d5ffb1801ee652</anchor>
      <arglist>(double jd_tt, enum novas_wobble_direction direction, double xp, double yp, const double *in, double *out)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>eph_manager.c</name>
    <path>src/</path>
    <filename>eph__manager_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="function">
      <type>short</type>
      <name>ephem_close</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a333da81ba7453316e26f32f628fefc0e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ephem_open</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>aabd581657809499fb433e4f9f56ace80</anchor>
      <arglist>(const char *ephem_name, double *jd_begin, double *jd_end, short *de_number)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>interpolate</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>aeda97db4dfac1a80c97ab2f3fb42b4a9</anchor>
      <arglist>(const double *buf, const double *t, long ncf, long na, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_ephemeris</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>ad36bc85b6f1cfb01ffc638b54552e15b</anchor>
      <arglist>(const double tjd[2], enum de_planet target, enum de_planet origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>split</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a50981955ef4594e094650db69472ec94</anchor>
      <arglist>(double tt, double *fr)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>state</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>af10ac2743bb9187d010e06722bb0cc75</anchor>
      <arglist>(const double *jed, enum de_planet target, double *target_pos, double *target_vel)</arglist>
    </member>
    <member kind="variable">
      <type>double *</type>
      <name>BUFFER</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>af1da4b8e59393e73ef0711920e920753</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>EM_RATIO</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>ad13d93611f33badca5f954f0b7c9c1a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>FILE *</type>
      <name>EPHFILE</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>ab690dba61a7445d8db2c104883fb0fe6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>IPT</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a3e671032c94fa64bb397d6166fef36f7</anchor>
      <arglist>[3][12]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>JPLAU</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a3827e48de0de48feb5b52444d03983d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>short</type>
      <name>KM</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a7c998bf57458a81960a20b58bec630d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>LPT</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>ad0644039a2a0b1fcf47bf8f97af4d537</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>NP</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>aa4ad9304a91f586963d80cf4805b9d76</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>NRL</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a1b3f642c5c66eb202b565cbd2ec70194</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>NV</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a4f5b18c8499bbc908974749c84eb9ece</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>PC</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a0fcf3bf4c37f78e5ef7557cb03029eff</anchor>
      <arglist>[18]</arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>RECORD_LENGTH</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a56a89d553bafe2d8c4a7e37f2997c300</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>SS</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>a20c09a36fbddf8e712b5f937f06b1b66</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>TWOT</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>ad741a4fdb6d243507e3be0028cdeea9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>VC</name>
      <anchorfile>eph__manager_8c.html</anchorfile>
      <anchor>afe4ec6caa67baf8fb5b184cc183a07e8</anchor>
      <arglist>[18]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>equinox.c</name>
    <path>src/</path>
    <filename>equinox_8c.html</filename>
    <member kind="function">
      <type>double</type>
      <name>accum_prec</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>ad1de5b481e41cf193c199bd0dc5084bd</anchor>
      <arglist>(double t)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cel_pole</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>a8937180de2ee1545d19b8fc840f35fe2</anchor>
      <arglist>(double jd_tt, enum novas_pole_offset_type type, double dpole1, double dpole2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>e_tilt</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>ab75165e2d69231da10c67de025ee05d8</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *restrict mobl, double *restrict tobl, double *restrict ee, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>ee_ct</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>add7a260017fad0d59c943a0a8d935fcf</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>fund_args</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>a312b5ab1a004b179d80463a07ca7da8f</anchor>
      <arglist>(double t, novas_delaunay_args *restrict a)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>ira_equinox</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>aea0e1028baf14d16d9de800e36a17086</anchor>
      <arglist>(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean_obliq</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>af182ee8cc4239f581f746aa974827d61</anchor>
      <arglist>(double jd_tdb)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_gmst_prec</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>a30f34cfb5b601fa3fe8533c4fb47d8b2</anchor>
      <arglist>(double jd_tdb)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nutation</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>a69df5671751c09a7575963b94ad1151e</anchor>
      <arglist>(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>planet_lon</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>aacc6a0daf29b8bdc18f46647a94694d3</anchor>
      <arglist>(double t, enum novas_planet planet)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>precession</name>
      <anchorfile>equinox_8c.html</anchorfile>
      <anchor>a9178358732325399f3a5e3582080ced3</anchor>
      <arglist>(double jd_tdb_in, const double *in, double jd_tdb_out, double *out)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>frames.c</name>
    <path>src/</path>
    <filename>frames_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>DA0</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a850d4988b6e1ee4e16274b2c723f93c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ETA0</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ac3263879bdfa579845d9bf404ab20208</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>XI0</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a9e131aa097cc3b5dd7d66533780aff3e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_app_to_geom</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ab7db8026c598e8e126a4725ac3b388bf</anchor>
      <arglist>(const novas_frame *restrict frame, enum novas_reference_system sys, double ra, double dec, double dist, double *restrict geom_icrs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_app_to_hor</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a3cd947f2e48d6c5cc88ad7a1bab9b8ca</anchor>
      <arglist>(const novas_frame *restrict frame, enum novas_reference_system sys, double ra, double dec, RefractionModel ref_model, double *restrict az, double *restrict el)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_change_observer</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a2972b3eafa2d7f6c29cff34bad9fcf0d</anchor>
      <arglist>(const novas_frame *orig, const observer *obs, novas_frame *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_equ_track</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a57f4a449754c88cfc87df658a6bc4cb2</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame, double dt, novas_track *restrict track)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_frame_lst</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a273fd76d83b8e04ced14558c69fd175c</anchor>
      <arglist>(const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_posvel</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ac6652ee3257621080dac342313d0a0a7</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame, enum novas_reference_system sys, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_to_app</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a4817703c9357f702a27812584c91f87c</anchor>
      <arglist>(const novas_frame *restrict frame, const double *restrict pos, enum novas_reference_system sys, sky_pos *restrict out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_to_app</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>aa57cbcae2001fece2ee4a2babfbc5f89</anchor>
      <arglist>(const novas_frame *restrict frame, double az, double el, RefractionModel ref_model, enum novas_reference_system sys, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_track</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a1f29810393108d15fd8312a2a95158dc</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model, novas_track *restrict track)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_invert_transform</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ad2081e123024609d3071b33b183502b7</anchor>
      <arglist>(const novas_transform *transform, novas_transform *inverse)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_frame</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>abfee69517c7432733c8382c88371c983</anchor>
      <arglist>(enum novas_accuracy accuracy, const observer *obs, const novas_timespec *time, double dx, double dy, novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_transform</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a4d179c8573319fc25c775ce56ba055dc</anchor>
      <arglist>(const novas_frame *frame, enum novas_reference_system from_system, enum novas_reference_system to_system, novas_transform *transform)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_moon_angle</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a121940d7f0ef41297d28fab8e6df5998</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_object_sep</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>afa5f3a823a895bed01876df6b2a00370</anchor>
      <arglist>(const object *source1, const object *source2, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_rises_above</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>aaf38f64304fa0b00c9c56cead62043f3</anchor>
      <arglist>(double el, const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sets_below</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a23af09cd8e5ec5ef4d0b03a347a4add0</anchor>
      <arglist>(double el, const object *restrict source, const novas_frame *restrict frame, RefractionModel ref_model)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_sky_pos</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ac365a1e015ae1f875dc5a66fd5435b2f</anchor>
      <arglist>(const object *restrict object, const novas_frame *restrict frame, enum novas_reference_system sys, sky_pos *restrict out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_illum</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ac780bd907a12957995d36ad916dd4ec1</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sun_angle</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a2d1ef3bc6d99aa473511e45c6e0481e9</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_track_pos</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a225a2e33684d2c1fe769e5aa718a182a</anchor>
      <arglist>(const novas_track *track, const novas_timespec *time, double *restrict lon, double *restrict lat, double *restrict dist, double *restrict z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_sky_pos</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>acfcfb90637cd1fdb25ef9afd1bda8e30</anchor>
      <arglist>(const sky_pos *in, const novas_transform *restrict transform, sky_pos *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_vector</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>aa77895c891e9eeb4b1541f03acfd154e</anchor>
      <arglist>(const double *in, const novas_transform *restrict transform, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_transit_time</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ad04f1ebd7b2d39e5258b152fec231001</anchor>
      <arglist>(const object *restrict source, const novas_frame *restrict frame)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>grav.c</name>
    <path>src/</path>
    <filename>grav_8c.html</filename>
    <member kind="function">
      <type>short</type>
      <name>grav_def</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>a48f90ac20950507fad9b880a7f7f2383</anchor>
      <arglist>(double jd_tdb, enum novas_observer_place unused, enum novas_accuracy accuracy, const double *pos_src, const double *pos_obs, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_planets</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>ae36bc31e1dad8a68f94f7a6414f0519f</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, const novas_planet_bundle *restrict planets, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>grav_redshift</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>a3ce6b306bc183387d0aaac97abbd82e8</anchor>
      <arglist>(double M_kg, double r_m)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_undef</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>aaae33249717912663bbb00be4fa4e6c3</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *pos_app, const double *pos_obs, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_undo_planets</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>a1b107a24b9fadc79cc5df8c6f5505d16</anchor>
      <arglist>(const double *pos_app, const double *pos_obs, const novas_planet_bundle *restrict planets, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_vec</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>a08a008572e2746a70bec19739b5fb7e6</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, const double *pos_body, double rmass, double *out)</arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>grav_bodies_full_accuracy</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>a18647d39520d1a15a137661f698cb12a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>grav_bodies_reduced_accuracy</name>
      <anchorfile>grav_8c.html</anchorfile>
      <anchor>a03a4df8961a0cd05f89aca478d2dcd24</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>itrf.c</name>
    <path>src/</path>
    <filename>itrf_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="function">
      <type>int</type>
      <name>novas_cartesian_to_geodetic</name>
      <anchorfile>itrf_8c.html</anchorfile>
      <anchor>abb9a26a4658c037b4721fb97cea08ed0</anchor>
      <arglist>(const double *restrict x, enum novas_reference_ellipsoid ellipsoid, double *restrict lon, double *restrict lat, double *restrict alt)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geodetic_to_cartesian</name>
      <anchorfile>itrf_8c.html</anchorfile>
      <anchor>a908dd3e00d6b27aa73679afc32c31f24</anchor>
      <arglist>(double lon, double lat, double alt, enum novas_reference_ellipsoid ellipsoid, double *x)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_itrf_transform</name>
      <anchorfile>itrf_8c.html</anchorfile>
      <anchor>afd98ed01148f1291018809d77785527f</anchor>
      <arglist>(int from_year, const double *restrict from_coords, const double *restrict from_rates, int to_year, double *to_coords, double *to_rates)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_itrf_transform_eop</name>
      <anchorfile>itrf_8c.html</anchorfile>
      <anchor>a3c8c9b8f709d1e5a7aa3a915f8fafcbb</anchor>
      <arglist>(int from_year, double from_xp, double from_yp, double from_dut1, int to_year, double *restrict to_xp, double *restrict to_yp, double *restrict to_dut1)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>naif.c</name>
    <path>src/</path>
    <filename>naif_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>__NOVAS_INTERNAL_API__</name>
      <anchorfile>naif_8c.html</anchorfile>
      <anchor>af46d8e51fc53d94b0d48c8201c3c84a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>enum novas_planet</type>
      <name>naif_to_novas_planet</name>
      <anchorfile>naif_8c.html</anchorfile>
      <anchor>a1c4b3f67a3b82808dd16ae43dfaf0f86</anchor>
      <arglist>(long id)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>novas_to_dexxx_planet</name>
      <anchorfile>naif_8c.html</anchorfile>
      <anchor>ac3eb54acf22dc155879163de89c6865e</anchor>
      <arglist>(enum novas_planet id)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>novas_to_naif_planet</name>
      <anchorfile>naif_8c.html</anchorfile>
      <anchor>ab51ac9bdf7736e1c1582e0981046daf3</anchor>
      <arglist>(enum novas_planet id)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>nutation.c</name>
    <path>src/</path>
    <filename>nutation_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>iau2000a</name>
      <anchorfile>nutation_8c.html</anchorfile>
      <anchor>a280cbf02b13ca57b3d77451be06e7dbc</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>iau2000b</name>
      <anchorfile>nutation_8c.html</anchorfile>
      <anchor>a6f9cddeb8c3dd906b9b0b5941249ad0c</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nu2000k</name>
      <anchorfile>nutation_8c.html</anchorfile>
      <anchor>abd066c2857d1b1cc06d8bd51b912d841</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nutation_angles</name>
      <anchorfile>nutation_8c.html</anchorfile>
      <anchor>a6e9ced7b0aec544ad7b057b58f777ec4</anchor>
      <arglist>(double t, enum novas_accuracy accuracy, double *restrict dpsi, double *restrict deps)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>observer.c</name>
    <path>src/</path>
    <filename>observer_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>aberration</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a033f1bf4b01e0e21a0e05e13c19c02f6</anchor>
      <arglist>(const double *pos, const double *vobs, double lighttime, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>bary2obs</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a1d58fd371e7604f75df315365672941e</anchor>
      <arglist>(const double *pos, const double *pos_obs, double *out, double *restrict lighttime)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>light_time</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>afb2cc37c7bdcd34c5c167afe092aed68</anchor>
      <arglist>(double jd_tdb, const object *restrict body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *pos_src_obs, double *restrict tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>light_time2</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a5c940b62732d74e6c6b1c844931aaa76</anchor>
      <arglist>(double jd_tdb, const object *restrict body, const double *restrict pos_obs, double tlight0, enum novas_accuracy accuracy, double *p_src_obs, double *restrict v_ssb, double *restrict tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_airborne_observer</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>aa423a88cd3f434642fc89d47390184f4</anchor>
      <arglist>(const on_surface *location, const double *vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_in_space</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>ac0ca6f330e1795ce811df95986400632</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, in_space *loc)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_observer</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a0f2096c7954b583fe749553b956ff9d2</anchor>
      <arglist>(enum novas_observer_place where, const on_surface *loc_surface, const in_space *loc_space, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_at_geocenter</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>aabb3ab0d99aa6e487cdcf6b454949b75</anchor>
      <arglist>(observer *restrict obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_in_space</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a6c146e4d10152d5e1ffb978f2fc962be</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_on_surface</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a5eb76fc6e7be571e228d23aee3823cb9</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, observer *restrict obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_on_surface</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a42c084465aedee75965f74e23e6571fe</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, on_surface *restrict loc)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_solar_system_observer</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a9c3f1a1bc3fd9ac6d211806913d2d529</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_e2h_offset</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a13cd0b80eaa7bc682013f7e8ffbdbd6f</anchor>
      <arglist>(double dra, double ddec, double pa, double *restrict daz, double *restrict del)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_epa</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a095ba0ee644dc00c11dcdc5616bc96b0</anchor>
      <arglist>(double ha, double dec, double lat)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_h2e_offset</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a7977de3e3edf8120b50cbd026c155f47</anchor>
      <arglist>(double daz, double del, double pa, double *restrict dra, double *restrict ddec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_hpa</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a73fcb54e47b2be0aa92810b5f55f7a3c</anchor>
      <arglist>(double az, double el, double lat)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_los_to_xyz</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a601d246673742cf72a5dbe1449c97560</anchor>
      <arglist>(const double *los, double lon, double lat, double *xyz)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_uvw_to_xyz</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a16c920eee105431247e9a1d9bf1c3021</anchor>
      <arglist>(const double *uvw, double ha, double dec, double *xyz)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_xyz_to_los</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a14deb6fdce80ba213e1f1d5238306e17</anchor>
      <arglist>(const double *xyz, double lon, double lat, double *los)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_xyz_to_uvw</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a87867e8e7fc7decaa44777a696465353</anchor>
      <arglist>(const double *xyz, double ha, double dec, double *uvw)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_planets</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a07490a134a685fe0842b2b4816559292</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *restrict pos_obs, int pl_mask, novas_planet_bundle *restrict planets)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_posvel</name>
      <anchorfile>observer_8c.html</anchorfile>
      <anchor>a0e4e5ff7c37e6521ca75ae62aaf4050e</anchor>
      <arglist>(double jd_tdb, double ut1_to_tt, enum novas_accuracy accuracy, const observer *restrict obs, const double *restrict geo_pos, const double *restrict geo_vel, double *restrict pos, double *restrict vel)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>orbital.c</name>
    <path>src/</path>
    <filename>orbital_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>novas_orbit_native_posvel</name>
      <anchorfile>orbital_8c.html</anchorfile>
      <anchor>a6b88d2403a3051ab32229244cf98d7ab</anchor>
      <arglist>(double jd_tdb, const novas_orbital *restrict orbit, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_orbit_posvel</name>
      <anchorfile>orbital_8c.html</anchorfile>
      <anchor>a4510a3235a693f122899983901211c30</anchor>
      <arglist>(double jd_tdb, const novas_orbital *restrict orbit, enum novas_accuracy accuracy, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_orbsys_pole</name>
      <anchorfile>orbital_8c.html</anchorfile>
      <anchor>a706ecf8998b78e48051d2876efc6e01c</anchor>
      <arglist>(enum novas_reference_system type, double ra, double dec, novas_orbital_system *restrict sys)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>parse.c</name>
    <path>src/</path>
    <filename>parse_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>MAX_DECIMALS</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>a93d0a993c2c1e6458c1e93fbd446b417</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_dms_degrees</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>aca89d2a0f49afad45dbddffc09c3adae</anchor>
      <arglist>(const char *restrict dms)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_epoch</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>af4e18885b7d173eb3fe5f18b06f0b4d4</anchor>
      <arglist>(const char *restrict system)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_hms_hours</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>a696e44a2d1c788f9f335611efd7ed958</anchor>
      <arglist>(const char *restrict hms)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_degrees</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>ad3d04d065e76a5921995e96f67c24c41</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_dms</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>ac651a7737d1f10e5267572f5c54fae92</anchor>
      <arglist>(const char *restrict dms, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_hms</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>a1aad2a903328080626db399450a7e297</anchor>
      <arglist>(const char *restrict hms, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_hours</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>aff8c2c3fae3be867d3fc73972a158514</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_dms</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>ad778a440540ed687572b78b675bebd60</anchor>
      <arglist>(double degrees, enum novas_separator_type sep, int decimals, char *restrict buf, int len)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_hms</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>a713869893ad7e58c53e44b37a3bcf768</anchor>
      <arglist>(double hours, enum novas_separator_type sep, int decimals, char *restrict buf, int len)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_str_degrees</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>a10a24fc28fa74efa75abf566bb61af5b</anchor>
      <arglist>(const char *restrict str)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_str_hours</name>
      <anchorfile>parse_8c.html</anchorfile>
      <anchor>ac69c17c646ddf3a5447557338a8490b4</anchor>
      <arglist>(const char *restrict str)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>place.c</name>
    <path>src/</path>
    <filename>place_8c.html</filename>
    <member kind="function">
      <type>short</type>
      <name>app_planet</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>ac28dd9a362982cfce150751001a96a8a</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>app_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a906b17f06b42e31a29cafad4856f565f</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>astro_planet</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a9ea79f60493a3a21a9228d048fa5a9a2</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>astro_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a9530c770e83db2cbc5a31d8d1f8c36f4</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ephemeris</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a713982e1f2bae0a72b39d03007f00d23</anchor>
      <arglist>(const double *restrict jd_tdb, const object *restrict body, enum novas_origin origin, enum novas_accuracy accuracy, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_planet</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>adcb9b273dfda5d97a1c01379a16997f9</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, double ut1_to_tt, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>acd56c41e811a002d6280b3b315730fbd</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *restrict star, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>mean_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>aaf51bf000e993807101a34adf423d36b</anchor>
      <arglist>(double jd_tt, double tra, double tdec, enum novas_accuracy accuracy, double *restrict ira, double *restrict idec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>place</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>aca09ed0d2379ac4a4841c9e3edeeee4c</anchor>
      <arglist>(double jd_tt, const object *restrict source, const observer *restrict location, double ut1_to_tt, enum novas_reference_system coord_sys, enum novas_accuracy accuracy, sky_pos *restrict output)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_cirs</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a49bd2b398f7eab3e98823cb1a7403ed5</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_gcrs</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>aea583f6f785fa42f46710288e8a4e083</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_icrs</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>ad465eafe65e3d701fc4bd57efa793a12</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_j2000</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a1ea9b377426b1800535e85fab47bfc1b</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_mod</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>ac53d150d1f6b96c83d1516a66c699e63</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a1504058672e6e3cf24ce6b7189ede005</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, const observer *restrict obs, double ut1_to_tt, enum novas_reference_system system, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_tod</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>ab6908b0eee06b4a44242fba1fd9d506c</anchor>
      <arglist>(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_planet</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a918c8244e44c69f1589d9f4bf5355d16</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, const observer *restrict obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis, double *restrict rv)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a0f307c2a37d594a68e99c5c8af6d50c9</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, const observer *restrict obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict rv)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>topo_planet</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>aaf3d450c87f545acfa7ab7e2b6c229d9</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, double ut1_to_tt, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>topo_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a86e4f52e832879a33aefc28803e2690f</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *restrict star, const on_surface *restrict position, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_planet</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>a0dc172d82eec79f78c2d218c1a7b9cbe</anchor>
      <arglist>(double jd_tt, const object *restrict ss_body, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_star</name>
      <anchorfile>place_8c.html</anchorfile>
      <anchor>aaf187624a93349391518f9d6bb3051e5</anchor>
      <arglist>(double jd_tt, const cat_entry *restrict star, enum novas_accuracy accuracy, double *restrict ra, double *restrict dec)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>planets.c</name>
    <path>src/</path>
    <filename>planets_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>novas_approx_heliocentric</name>
      <anchorfile>planets_8c.html</anchorfile>
      <anchor>a9ead627eed7fabd8583f1353e89f42e7</anchor>
      <arglist>(enum novas_planet id, double jd_tdb, double *restrict pos, double *restrict vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_approx_sky_pos</name>
      <anchorfile>planets_8c.html</anchorfile>
      <anchor>a4cb01609f76b0ff8487817b9446a3a62</anchor>
      <arglist>(enum novas_planet id, const novas_frame *restrict frame, enum novas_reference_system sys, sky_pos *restrict out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_moon_orbit</name>
      <anchorfile>planets_8c.html</anchorfile>
      <anchor>ad6e6c7f6dc2745030e3fd4d407e982bd</anchor>
      <arglist>(double jd_tdb, novas_orbital *restrict orbit)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_make_planet_orbit</name>
      <anchorfile>planets_8c.html</anchorfile>
      <anchor>a63b094e813d8b752a6f3344419e80031</anchor>
      <arglist>(enum novas_planet id, double jd_tdb, novas_orbital *restrict orbit)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_moon_phase</name>
      <anchorfile>planets_8c.html</anchorfile>
      <anchor>a2dbdafd6b07f079815ae52d00e142861</anchor>
      <arglist>(double jd_tdb)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_next_moon_phase</name>
      <anchorfile>planets_8c.html</anchorfile>
      <anchor>ad0bd6e212af549309b0fcb792ed8cb79</anchor>
      <arglist>(double phase, double jd_tdb)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>plugin.c</name>
    <path>src/</path>
    <filename>plugin_8c.html</filename>
    <member kind="function">
      <type>novas_ephem_provider</type>
      <name>get_ephem_provider</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>aa6fad38297a49ba78a6bd49cedf889b1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>novas_nutation_provider</type>
      <name>get_nutation_lp_provider</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>a416a96426a0b3c52c1d5fc6452357366</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>novas_planet_provider</type>
      <name>get_planet_provider</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>a0f0a786b5614a788c02b37a462eae2d2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>novas_planet_provider_hp</type>
      <name>get_planet_provider_hp</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>ae45badb450c1ca73920d982e12346c5b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_ephem_provider</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>a6097083c438a8d1872c2e00a66360030</anchor>
      <arglist>(novas_ephem_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_nutation_lp_provider</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>a694288eb631d6a69ecf313b7b5b4a677</anchor>
      <arglist>(novas_nutation_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_planet_provider</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>a9904c454bc7148b42d0f927b82ea3973</anchor>
      <arglist>(novas_planet_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_planet_provider_hp</name>
      <anchorfile>plugin_8c.html</anchorfile>
      <anchor>a418f45a11ce7f92ba0ef817558415640</anchor>
      <arglist>(novas_planet_provider_hp func)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>refract.c</name>
    <path>src/</path>
    <filename>refract_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>NOVAS_DEFAULT_WAVELENGTH</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>a4f7dee9d4c9db4cf201af8383c72d4db</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_inv_refract</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>a81da80ae3ab1d0a444f39bde2af3a732</anchor>
      <arglist>(RefractionModel model, double jd_tt, const on_surface *restrict loc, enum novas_refraction_type type, double el0)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_optical_refraction</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>ae4511a80a1defbb1e1fdc29c289e7de4</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_radio_refraction</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>a6b49f92f8f818f2272613e3432185a39</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_refract_wavelength</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>ac673c4fef94847b796dc2aa2f4d0807d</anchor>
      <arglist>(double microns)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_standard_refraction</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>ab04bff3b97c132e358e11c3a5b7a3081</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_wave_refraction</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>a7b9ad6dfbd86e4faa2d8068c7d64fac4</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>refract</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>ac7ed2a1a5fb5aea3063d116d3d980eeb</anchor>
      <arglist>(const on_surface *restrict location, enum novas_refraction_model model, double zd_obs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>refract_astro</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>afe1d4e96dd66676f5567a279e83e88d9</anchor>
      <arglist>(const on_surface *restrict location, enum novas_refraction_model model, double zd_astro)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solsys-calceph.c</name>
    <path>src/</path>
    <filename>solsys-calceph_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>novas_calceph_use_ids</name>
      <anchorfile>solsys-calceph_8c.html</anchorfile>
      <anchor>a5531b937a5bb5491cb475d2536346ad8</anchor>
      <arglist>(enum novas_id_type idtype)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_calceph</name>
      <anchorfile>solsys-calceph_8c.html</anchorfile>
      <anchor>a35fc86a939602ec910dbdfde180d7f31</anchor>
      <arglist>(t_calcephbin *eph)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_calceph_planets</name>
      <anchorfile>solsys-calceph_8c.html</anchorfile>
      <anchor>ab86ec03898e658c86c671a8b6399e331</anchor>
      <arglist>(t_calcephbin *eph)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solsys-cspice.c</name>
    <path>src/</path>
    <filename>solsys-cspice_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>cspice_add_kernel</name>
      <anchorfile>solsys-cspice_8c.html</anchorfile>
      <anchor>ae02d26964099f56c375dc78172ad135b</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cspice_remove_kernel</name>
      <anchorfile>solsys-cspice_8c.html</anchorfile>
      <anchor>a38e3b3155cad3b3822fbc3e553060ab1</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_cspice</name>
      <anchorfile>solsys-cspice_8c.html</anchorfile>
      <anchor>a9b44218bca5fce1e9e5103494c15dde4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_cspice_ephem</name>
      <anchorfile>solsys-cspice_8c.html</anchorfile>
      <anchor>afb078e1df9b5b08999d24e68d7b4ecdc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_use_cspice_planets</name>
      <anchorfile>solsys-cspice_8c.html</anchorfile>
      <anchor>a943549e395b19f6a91064e3d6fa1c577</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solsys-ephem.c</name>
    <path>src/</path>
    <filename>solsys-ephem_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="function">
      <type>short</type>
      <name>planet_ephem_provider</name>
      <anchorfile>solsys-ephem_8c.html</anchorfile>
      <anchor>a9d0f54c0ab00827768e61a353488b0ee</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_ephem_provider_hp</name>
      <anchorfile>solsys-ephem_8c.html</anchorfile>
      <anchor>a93d0f96d81f3c52e05163643bd23eb7d</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solsys1.c</name>
    <path>src/</path>
    <filename>solsys1_8c.html</filename>
    <member kind="function">
      <type>short</type>
      <name>planet_eph_manager</name>
      <anchorfile>solsys1_8c.html</anchorfile>
      <anchor>a841f04bf04e6dd8a5ae96f8ea729090d</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_eph_manager_hp</name>
      <anchorfile>solsys1_8c.html</anchorfile>
      <anchor>a8b471fd187aebc33b1f9fd4c2ff60c90</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem</name>
      <anchorfile>solsys1_8c.html</anchorfile>
      <anchor>a37c25dbc2683dc9e3b1d187e13f68330</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solsys1_8c.html</anchorfile>
      <anchor>a17846f3fafcce208dbbca1384e3a7fb0</anchor>
      <arglist>(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solsys2.c</name>
    <path>src/</path>
    <filename>solsys2_8c.html</filename>
    <member kind="function">
      <type>void</type>
      <name>jplihp_</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a14752a69e14c399d5ee82e83f22acf68</anchor>
      <arglist>(const double *jd_tdb, long *targ, long *cent, double *posvel, long *err_flg)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>jplint_</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>ab53652c0e203b166427464ee1691c236</anchor>
      <arglist>(const double *jd_tdb, long *targ, long *cent, double *posvel, long *err_flg)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_jplint</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a1300b6559a08ef90ae9b9d8d33a22220</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_jplint_hp</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a2fe8cd1a87f96611f78edc36b4960d63</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a37c25dbc2683dc9e3b1d187e13f68330</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a17846f3fafcce208dbbca1384e3a7fb0</anchor>
      <arglist>(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solsys3.c</name>
    <path>src/</path>
    <filename>solsys3_8c.html</filename>
    <member kind="function">
      <type>short</type>
      <name>earth_sun_calc</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a580f5949c8a29cdd7bf4787aee891f2b</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>earth_sun_calc_hp</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a338e3436ba86dadf6bd4d7390f9940ec</anchor>
      <arglist>(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin, double *restrict position, double *restrict velocity)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enable_earth_sun_hp</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a279bca0519117cee9a3bd4382f38e7ad</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>sun_eph</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a869ec4c9fe91668611c0cfcc6bf332cf</anchor>
      <arglist>(double jd, double *restrict ra, double *restrict dec, double *restrict dis)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>spectral.c</name>
    <path>src/</path>
    <filename>spectral_8c.html</filename>
    <member kind="function">
      <type>double</type>
      <name>novas_lsr_to_ssb_vel</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>aad80bf94be2efc36c340994829df8207</anchor>
      <arglist>(double epoch, double ra, double dec, double vLSR)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_ssb_to_lsr_vel</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>a99ed25baa029f3980924da8c8af54d1a</anchor>
      <arglist>(double epoch, double ra, double dec, double vLSR)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_v2z</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>aa54cd00024b294d38fa29433c017c65f</anchor>
      <arglist>(double vel)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z2v</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>a63621cea3b4985c1bfcc4514a7aa3fb1</anchor>
      <arglist>(double z)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z_add</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>a493ab751eee5804db77157c37e94e5c3</anchor>
      <arglist>(double z1, double z2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z_inv</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>a1130ad2ce520e948c71f03a25a713075</anchor>
      <arglist>(double z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rad_vel</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>afca24d914664bb60d20fb428177f329a</anchor>
      <arglist>(const object *restrict source, const double *restrict pos_src, const double *vel_src, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun, double *restrict rv)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rad_vel2</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>a37f22e463cc06b4e381aa18659e6319d</anchor>
      <arglist>(const object *restrict source, const double *pos_emit, const double *vel_src, const double *pos_det, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>redshift_vrad</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>af56088995d42a0673e03bca1f3891544</anchor>
      <arglist>(double vrad, double z)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>unredshift_vrad</name>
      <anchorfile>spectral_8c.html</anchorfile>
      <anchor>aea89203030ca831260dec18f83081d23</anchor>
      <arglist>(double vrad, double z)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>system.c</name>
    <path>src/</path>
    <filename>system_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>ecl2equ</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>a41ca0cfd79ad07c9e857ba02fa94928e</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ecl2equ_vec</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>a6fa1eada41005ac70290d0bb5df1ec26</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>equ2ecl</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>a035047336b16951b8b850066e74210a9</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec, double *restrict elon, double *restrict elat)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>equ2ecl_vec</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>aa05ecc76c1ccc0c3445f8b642d6c5311</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>equ2gal</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>a288b49c03bc29704c8dd5acf8baca80e</anchor>
      <arglist>(double ra, double dec, double *restrict glon, double *restrict glat)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>equ2hor</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>ad932a8617973ef06e4a5c955b7897bb0</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const on_surface *restrict location, double ra, double dec, enum novas_refraction_model ref_option, double *restrict zd, double *restrict az, double *restrict rar, double *restrict decr)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gal2equ</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>adcce501bd3f8f70974e7f55527162f83</anchor>
      <arglist>(double glon, double glat, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>hor_to_itrs</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>a19f4f6c7d942dcba5ad2a4d2a60affad</anchor>
      <arglist>(const on_surface *restrict location, double az, double za, double *restrict itrs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_hor</name>
      <anchorfile>system_8c.html</anchorfile>
      <anchor>aef3c1f8e4c443e51e65003018449768a</anchor>
      <arglist>(const on_surface *restrict location, const double *restrict itrs, double *restrict az, double *restrict za)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>target.c</name>
    <path>src/</path>
    <filename>target_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>_GNU_SOURCE</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a369266c24eacffb87046522897a570d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_cat_entry</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a39b2e39d29f1b57c5f785284ba7ca43f</anchor>
      <arglist>(const char *restrict star_name, const char *restrict catalog, long cat_num, double ra, double dec, double pm_ra, double pm_dec, double parallax, double rad_vel, cat_entry *star)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_cat_object</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>ab9b7a622486f2904615ceed1ba94cd8f</anchor>
      <arglist>(const cat_entry *star, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_cat_object_sys</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a5104d7db66ce2d8c61c7ad21599fb45e</anchor>
      <arglist>(const cat_entry *star, const char *restrict system, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_ephem_object</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>aa22303f98df2d63edf6d8452ff3a43b2</anchor>
      <arglist>(const char *name, long num, object *body)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_object</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a411f70fb77582d750c5269fcd64b0ce2</anchor>
      <arglist>(enum novas_object_type type, long number, const char *name, const cat_entry *star, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_orbital_object</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a3c5e215babeeba078ca7b3e9be42e555</anchor>
      <arglist>(const char *name, long num, const novas_orbital *orbit, object *body)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_planet</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a4c7846be95299769f4d0377bf127bcf5</anchor>
      <arglist>(enum novas_planet num, object *restrict planet)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_cat_entry</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a11aa3ce6bcdda5b7065b38d6cfaf63d3</anchor>
      <arglist>(const char *name, double ra, double dec, double z, cat_entry *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_object</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>ae579a49d84696a913fd0d9f348bb5b11</anchor>
      <arglist>(const char *name, double ra, double dec, double z, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_object_sys</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a257abae9391aec835dc2b94d9788b9f9</anchor>
      <arglist>(const char *name, double ra, double dec, const char *restrict system, double z, object *source)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>novas_case_sensitive</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a8f832afe92680a11c3489aaa224ac2e2</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_helio_dist</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>af4fa5359b8b0167a2780baff17b164a5</anchor>
      <arglist>(double jd_tdb, const object *restrict source, double *restrict rate)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_planet</type>
      <name>novas_planet_for_name</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a2cf7410aa68b980112734227b40890d6</anchor>
      <arglist>(const char *restrict name)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_power</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a763f9a9dd2edb4645a5db05184ca7495</anchor>
      <arglist>(double jd_tdb, const object *restrict source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>proper_motion</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a24b69e1809a33113d4a041dd3e4082ad</anchor>
      <arglist>(double jd_tdb_in, const double *pos, const double *restrict vel, double jd_tdb_out, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>starvectors</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>a27bae6c9efdab763dba9f1ae36a04669</anchor>
      <arglist>(const cat_entry *restrict star, double *restrict pos, double *restrict motion)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>transform_cat</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>abe0c6e6fc60a49efa88c365aef8cdc50</anchor>
      <arglist>(enum novas_transform_type option, double jd_tt_in, const cat_entry *in, double jd_tt_out, const char *out_id, cat_entry *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>transform_hip</name>
      <anchorfile>target_8c.html</anchorfile>
      <anchor>af2c03d540de7a8bfafaed27eb9a753c1</anchor>
      <arglist>(const cat_entry *hipparcos, cat_entry *hip_2000)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>timescale.c</name>
    <path>src/</path>
    <filename>timescale_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>_GNU_SOURCE</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a369266c24eacffb87046522897a570d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>get_ut1_to_tt</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a4d7470ca705efa1953e8b4beab77c6b6</anchor>
      <arglist>(int leap_seconds, double dut1)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>get_utc_to_tt</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ab7611704bab9ce717744a8b5575c5378</anchor>
      <arglist>(int leap_seconds)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_date</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>abaac8a4ea28bc8b811277051e3168468</anchor>
      <arglist>(const char *restrict date)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_date_scale</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a66656c3f5a1256f4bfc3ed2e168c70c2</anchor>
      <arglist>(const char *restrict date, enum novas_timescale *restrict scale)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_diff_tcb</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a4818ab9be2f0891091c719db01bcfa98</anchor>
      <arglist>(const novas_timespec *t1, const novas_timespec *t2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_diff_tcg</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ab904df0f504d07bbf73f84e2a1c43e07</anchor>
      <arglist>(const novas_timespec *t1, const novas_timespec *t2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_diff_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>abdcc8e194d79787838d1ffc064449932</anchor>
      <arglist>(const novas_timespec *t1, const novas_timespec *t2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_get_split_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a32d8336084b65fac14d092ab6198c27d</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_timescale timescale, long *restrict ijd)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_get_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ae30e3dd8e30bdcf4e8486cfce696fb98</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_timescale timescale)</arglist>
    </member>
    <member kind="function">
      <type>time_t</type>
      <name>novas_get_unix_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>afa1a1d804ca70050418429f971c2045a</anchor>
      <arglist>(const novas_timespec *restrict time, long *restrict nanos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_iso_timestamp</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a4d6da26d0a7d7d904f25d2be00030b25</anchor>
      <arglist>(const novas_timespec *restrict time, char *restrict dst, int maxlen)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_offset_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a01f98b2b8033c8dbacaeab37e9198e91</anchor>
      <arglist>(const novas_timespec *time, double seconds, novas_timespec *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_date</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a4934da85b9722ef43a5b238f3e1971e4</anchor>
      <arglist>(const char *restrict date, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_date_format</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a5ec9d054b7d363b3db1d065507a1ba41</anchor>
      <arglist>(enum novas_calendar_type calendar, enum novas_date_format format, const char *restrict date, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_iso_date</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ad53b3be5b92ea244cfb0347feb3bec9f</anchor>
      <arglist>(const char *restrict date, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_timescale</type>
      <name>novas_parse_timescale</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a72245b3ea6099c573278f721a4bfd3d9</anchor>
      <arglist>(const char *restrict str, char **restrict tail)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_timescale</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a084e5a2dcd20860af7176754d3a32202</anchor>
      <arglist>(enum novas_timescale scale, char *restrict buf)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_split_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ad47bcb1e3c5df56a0ffdf1a127e52598</anchor>
      <arglist>(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1, novas_timespec *restrict time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>af4a1a5180905a7d8f76aeafe3f9a8ebb</anchor>
      <arglist>(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *restrict time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_unix_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a9d54ea7c6cdbc75ee78cf552a7e1858d</anchor>
      <arglist>(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *restrict time)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_time_gst</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>aaf53fcad3fade814381cef3d06779a8c</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_time_lst</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a9e99ea879ba0ddc1d9fbefc4b32b1ef1</anchor>
      <arglist>(const novas_timespec *restrict time, double lon, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_timescale</type>
      <name>novas_timescale_for_string</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ab344242721ca00709d4dc482a7934e2a</anchor>
      <arglist>(const char *restrict str)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_timestamp</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ac8e83c48005d5aabb3e7bbf8d9f8ed81</anchor>
      <arglist>(const novas_timespec *restrict time, enum novas_timescale scale, char *restrict dst, int maxlen)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tdb2tt</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a90d5fae30728b51151c3daf355ca585b</anchor>
      <arglist>(double jd_tdb, double *restrict jd_tt, double *restrict secdiff)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tt2tdb</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a129acca41760da486e1179c93cb9e86d</anchor>
      <arglist>(double jd_tt)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tt2tdb_fp</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ad301a9c74bfbdb8b55513cd005787b22</anchor>
      <arglist>(double jd_tt, double limit)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tt2tdb_hp</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a5709058076f4aea4eeaeeb000342cd94</anchor>
      <arglist>(double jd_tt)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>transform.c</name>
    <path>src/</path>
    <filename>transform_8c.html</filename>
    <member kind="function">
      <type>double</type>
      <name>app_to_cirs_ra</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a3fa57a154f2f423612736e5e3a5addbf</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double ra)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cel2ter</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a6217852bfd7d3a58d3266d0daadfc6bc</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class coordType, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cirs_to_app_ra</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>af90d52a6527d9b62b8ac35d8c1ac7c6b</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double ra)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_gcrs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>ac0ba048cd1732d0d0398397396d158bc</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_itrs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>aa2768d89d499a7155f90a6fd8c176d8d</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_tod</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a9cac4f3a18d4c14939519e7123b4d504</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>frame_tie</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>aa11d2253a44261370af8d0c32f0715e9</anchor>
      <arglist>(const double *in, enum novas_frametie_direction direction, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>gcrs2equ</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>abd38071a8d4bdc342feaecfac2fc7856</anchor>
      <arglist>(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_cirs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a51ea4a016336dd3cf7783061c14d165c</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_j2000</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>aeea4b5085c7267714492fae6108fb975</anchor>
      <arglist>(const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_mod</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a75de6cca190c5fbc3056db726f317c7f</anchor>
      <arglist>(double jd_tdb, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_tod</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a9c900b81a718a184281a8a0b733b4db7</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_cirs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a12953f66c0f774284f0700ae785784d0</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_tod</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a3f6ae564cfac3766a21e0754905e5f7c</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>j2000_to_gcrs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a9d51ca5c972013024755c644c96d0586</anchor>
      <arglist>(const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>j2000_to_tod</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a02fa1ee652c20160055cbd2c7bd9ca63</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>mod_to_gcrs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>ad9cc07f0e6af9339096e15a440b113e5</anchor>
      <arglist>(double jd_tdb, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ter2cel</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>ac8c5ff3cc13126aba2ec5a6cbe187a31</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class coordType, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_cirs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>adb48b2acaf76cc3a1e3a1412cc42c232</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_gcrs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a6871e7fecde44084c7b4e1b8dda5ac70</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_itrs</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a4551e2ac083e34c1b2b61b3805efb9a3</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_j2000</name>
      <anchorfile>transform_8c.html</anchorfile>
      <anchor>a6205edb1361e56cc0c7aed0f088f7437</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>util.c</name>
    <path>src/</path>
    <filename>util_8c.html</filename>
    <member kind="function">
      <type>double</type>
      <name>d_light</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>ad799864f637c483fb49491bde5a93e25</anchor>
      <arglist>(const double *pos_src, const double *pos_body)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>novas_debug</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>a5f35633b872751df4dacd2fb2cbe4ac3</anchor>
      <arglist>(enum novas_debug_mode mode)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_equ_sep</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>a4f7e06b2fa5b3145b37aec4f9c2d1602</anchor>
      <arglist>(double ra1, double dec1, double ra2, double dec2)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_debug_mode</type>
      <name>novas_get_debug_mode</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>a29cad12337acfce5f1e370320dcefb79</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_norm_ang</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>a85334614a0f399e0d3cbde68075d0766</anchor>
      <arglist>(double angle)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sep</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>a9f33de2b26a19a5e20827e9e918b3375</anchor>
      <arglist>(double lon1, double lat1, double lon2, double lat2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_vlen</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>a0f41cefb8c872973b07e936b5026d143</anchor>
      <arglist>(const double *restrict v)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec2vector</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>ae104afd1bc638def9bdbce1801e5054a</anchor>
      <arglist>(double ra, double dec, double dist, double *restrict pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>spin</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>af4090e519d7ce7ab1fcdb24b10126eb4</anchor>
      <arglist>(double angle, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>vector2radec</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>a84f672dc1c37e3af374198f29cbcfba9</anchor>
      <arglist>(const double *restrict pos, double *restrict ra, double *restrict dec)</arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>novas_inv_max_iter</name>
      <anchorfile>util_8c.html</anchorfile>
      <anchor>ad2b933ce2f58e8b04bbe76ffbff99eb0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>cat_entry</name>
    <filename>structcat__entry.html</filename>
    <member kind="variable">
      <type>char</type>
      <name>catalog</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>a128c876f7e50045e1c469dbabb400c58</anchor>
      <arglist>[SIZE_OF_CAT_NAME]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>dec</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>ac4ae7989694321b540abb75c30eec690</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>parallax</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>a18f7f3626661dbf57ce9eaec51840eb8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>promodec</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>acf4bdbac6c1d059a4db3ed879f32f21c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>promora</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>ab9301cf1c67c13114b22950650b17789</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>ra</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>a713a0c71a86d92fa6892fdb2cb7c9422</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>radialvelocity</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>aa2b5e5ca3a5df1765b49a6c6b110a36a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char</type>
      <name>starname</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>ad26c9b08b1028d2a7f35c447841022b1</anchor>
      <arglist>[SIZE_OF_OBJ_NAME]</arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>starnumber</name>
      <anchorfile>structcat__entry.html</anchorfile>
      <anchor>a2cc0369c1acae73cf6f3e06f1a8ddab8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>in_space</name>
    <filename>structin__space.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>sc_pos</name>
      <anchorfile>structin__space.html</anchorfile>
      <anchor>abc3736b39d63bfabf73a25e3b03b91d8</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>sc_vel</name>
      <anchorfile>structin__space.html</anchorfile>
      <anchor>a9615dabeef0c31f47546c8083b768257</anchor>
      <arglist>[3]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_delaunay_args</name>
    <filename>structnovas__delaunay__args.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>D</name>
      <anchorfile>structnovas__delaunay__args.html</anchorfile>
      <anchor>ad8657a5ec76e12f3066fb4b4eb75ace9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>F</name>
      <anchorfile>structnovas__delaunay__args.html</anchorfile>
      <anchor>ae3520ebb3ff8d6feab3e6afb47ed4040</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>l</name>
      <anchorfile>structnovas__delaunay__args.html</anchorfile>
      <anchor>a59e80b8ba32c12c6d0a868f17a19ae48</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>l1</name>
      <anchorfile>structnovas__delaunay__args.html</anchorfile>
      <anchor>ad7a541093aca44890413459c6248db6c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>Omega</name>
      <anchorfile>structnovas__delaunay__args.html</anchorfile>
      <anchor>a1b5bf6735b7465aec5c931281d240737</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_frame</name>
    <filename>structnovas__frame.html</filename>
    <member kind="variable">
      <type>enum novas_accuracy</type>
      <name>accuracy</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ad9a827bf90bce73ad42f04ed2dd13a9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>beta</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a9424343761f8c4f4c1afe8f5b6bf471b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>deps0</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>afb1bb47af5a00647759b897f72a68b58</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>dpsi0</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a4445e3f8a201b14a5a7068c87507de72</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>dx</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a229d11aff11a7482259d1296b9b70b8a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>dy</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a9deb6f886b19d50e714d890c3c268efc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>earth_pos</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>aa3a3023bf2e8babb456b88391441c3dc</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>earth_vel</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a099283b966b6937d7e59e10c6ff69760</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>ee</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a622800443dfc347bd82d3752ec60c771</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>era</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>aa7f218ad801848e38dc25323b8f0197b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>gamma</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a72f9e01745b3a8203067ab84aefc9aea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_matrix</type>
      <name>gcrs_to_cirs</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a568001e7fc428917d58d1ac16fecb1af</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>gst</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>aef07d7946615098b170225edc81bd4a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_matrix</type>
      <name>icrs_to_j2000</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ad28815abec2847e4ab01a17dab76f0c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>mobl</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>adcbb032bae5aa01bff2501f40527943e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_matrix</type>
      <name>nutation</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>af75d534436e4c2442aae1fbb3572a372</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>obs_pos</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ac2558d2b29171b29bad2599c75d3cc6c</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>obs_vel</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a66f1e86da27c96c1c920dce442ce78c8</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>struct novas_observer</type>
      <name>observer</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a70a5703b12da2ee4c801250fdd5708d7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_planet_bundle</type>
      <name>planets</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a669cb67d7c5765600cb355d37e14f172</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_matrix</type>
      <name>precession</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ab0ba4960aa038a40165a7131684c6853</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>state</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ac8ddcc571efb6061ddf9dfbcfffb4cd1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>sun_pos</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a3d3fc00c5477b8f7a9b6593a748949d0</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>sun_vel</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ae949db92ce7c56e4b5ada1b8f80002d3</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>struct novas_timespec</type>
      <name>time</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a5a61c03607da83f2b44b05e68cc50e5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>tobl</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ad68cbc6741fea66373829a8f50307c8e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>v_obs</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a52e812c05f108858a4459d937a2115a4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_matrix</name>
    <filename>structnovas__matrix.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>M</name>
      <anchorfile>structnovas__matrix.html</anchorfile>
      <anchor>ac842f2eec8c8b880e2aca8792d63359b</anchor>
      <arglist>[3][3]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_observable</name>
    <filename>structnovas__observable.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>dist</name>
      <anchorfile>structnovas__observable.html</anchorfile>
      <anchor>accf93555161c9eedf006462a228af523</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>lat</name>
      <anchorfile>structnovas__observable.html</anchorfile>
      <anchor>a7972334534f68166121a6e51b0aac2d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>lon</name>
      <anchorfile>structnovas__observable.html</anchorfile>
      <anchor>aa96391e04b5977c50b96d77bea86a01d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>z</name>
      <anchorfile>structnovas__observable.html</anchorfile>
      <anchor>ab3e6ed577a7c669c19de1f9c1b46c872</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_orbital</name>
    <filename>structnovas__orbital.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>a</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a1031d0e0a97a340abfe0a6ab9e831045</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>apsis_period</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a18c1c30ab34602026b03269be48d36a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>e</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>ab17e17fb32b792781b807505e7f60c9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>i</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a5659a38afd08966e6799fa0fb40a882a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>jd_tdb</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a0aefc536698706b7f8a15f23168c3486</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>M0</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>adc4cda6fcbff8922621b0a08f911957d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>n</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>abe63e991a7bf5d666068b15c9064428d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>node_period</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>abc96d50a65bae0b26750df2d93492fea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>omega</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a98ecc32b7ac0cf654d9f883cbe5cab35</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>Omega</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a1b5bf6735b7465aec5c931281d240737</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_orbital_system</type>
      <name>system</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a7ccd081fe6b1424bd644317068578775</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_orbital_system</name>
    <filename>structnovas__orbital__system.html</filename>
    <member kind="variable">
      <type>enum novas_planet</type>
      <name>center</name>
      <anchorfile>structnovas__orbital__system.html</anchorfile>
      <anchor>ac3165061a779934bfe04da14ad5555d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>obl</name>
      <anchorfile>structnovas__orbital__system.html</anchorfile>
      <anchor>ae5014f0e96e7c220c55eb82d11c4133a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>Omega</name>
      <anchorfile>structnovas__orbital__system.html</anchorfile>
      <anchor>a1b5bf6735b7465aec5c931281d240737</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum novas_reference_plane</type>
      <name>plane</name>
      <anchorfile>structnovas__orbital__system.html</anchorfile>
      <anchor>a2c38f985781586b7df387b437720174d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum novas_reference_system</type>
      <name>type</name>
      <anchorfile>structnovas__orbital__system.html</anchorfile>
      <anchor>adaabf923501a3a0a47a8958b6a33c6b3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_planet_bundle</name>
    <filename>structnovas__planet__bundle.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>mask</name>
      <anchorfile>structnovas__planet__bundle.html</anchorfile>
      <anchor>ab77cc972f3ee899689ba053015472ccd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>pos</name>
      <anchorfile>structnovas__planet__bundle.html</anchorfile>
      <anchor>afa2104ef635031a82766fd018c9ee9dd</anchor>
      <arglist>[NOVAS_PLANETS][3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>vel</name>
      <anchorfile>structnovas__planet__bundle.html</anchorfile>
      <anchor>af2bad6c1e0659e0e508492669eb67035</anchor>
      <arglist>[NOVAS_PLANETS][3]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_timespec</name>
    <filename>structnovas__timespec.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>dut1</name>
      <anchorfile>structnovas__timespec.html</anchorfile>
      <anchor>a281e93817d681ec4a6cea2601a15c566</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>fjd_tt</name>
      <anchorfile>structnovas__timespec.html</anchorfile>
      <anchor>a696e598955615f729a92870c8213a10c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>ijd_tt</name>
      <anchorfile>structnovas__timespec.html</anchorfile>
      <anchor>ab1b769234049ec8d7cdbb22800116184</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>tt2tdb</name>
      <anchorfile>structnovas__timespec.html</anchorfile>
      <anchor>acd6082033a111e30cda6bab81b7a32ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>ut1_to_tt</name>
      <anchorfile>structnovas__timespec.html</anchorfile>
      <anchor>a2b381954218c16fad4ae585fb90fddbe</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_track</name>
    <filename>structnovas__track.html</filename>
    <member kind="variable">
      <type>struct novas_observable</type>
      <name>accel</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>a8e9a1a492f74f76fb441e62bdd0df852</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_observable</type>
      <name>pos</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>ab572f04f2fbe8dd7174a07d4ae914706</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_observable</type>
      <name>rate</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>a08de80b2751625d418f907fe7ce0c2e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_timespec</type>
      <name>time</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>a5a61c03607da83f2b44b05e68cc50e5b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_transform</name>
    <filename>structnovas__transform.html</filename>
    <member kind="variable">
      <type>struct novas_frame</type>
      <name>frame</name>
      <anchorfile>structnovas__transform.html</anchorfile>
      <anchor>a89ed0a18f08c2125a0de2c5825c2bb0c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum novas_reference_system</type>
      <name>from_system</name>
      <anchorfile>structnovas__transform.html</anchorfile>
      <anchor>a5fe73fdfdc0f8933168a1c50df2db4ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_matrix</type>
      <name>matrix</name>
      <anchorfile>structnovas__transform.html</anchorfile>
      <anchor>ac528fe7349274fecff6a0c7742414308</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum novas_reference_system</type>
      <name>to_system</name>
      <anchorfile>structnovas__transform.html</anchorfile>
      <anchor>a8dac6a5c4bdc0e667bdc9eb7c7bb7094</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>object</name>
    <filename>structobject.html</filename>
    <member kind="variable">
      <type>char</type>
      <name>name</name>
      <anchorfile>structobject.html</anchorfile>
      <anchor>ae73ef5f04701dc9d4e7cb1b1c3fa9c12</anchor>
      <arglist>[SIZE_OF_OBJ_NAME]</arglist>
    </member>
    <member kind="variable">
      <type>long</type>
      <name>number</name>
      <anchorfile>structobject.html</anchorfile>
      <anchor>a7d578acc2a3e88ef2435fe0e88d01a74</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_orbital</type>
      <name>orbit</name>
      <anchorfile>structobject.html</anchorfile>
      <anchor>a43225af0345622f8c049032d75a1a3da</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_cat_entry</type>
      <name>star</name>
      <anchorfile>structobject.html</anchorfile>
      <anchor>a499924f8241c3fbb3bf34c5e16f1c7de</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum novas_object_type</type>
      <name>type</name>
      <anchorfile>structobject.html</anchorfile>
      <anchor>a2cc80aad5c0303449e2412b122504a10</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>observer</name>
    <filename>structobserver.html</filename>
    <member kind="variable">
      <type>struct novas_in_space</type>
      <name>near_earth</name>
      <anchorfile>structobserver.html</anchorfile>
      <anchor>a1ab1241c3bb4d3afc4710e02e948c0cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>struct novas_on_surface</type>
      <name>on_surf</name>
      <anchorfile>structobserver.html</anchorfile>
      <anchor>a45fe4eb871c346c1b8315c4c6a41e96d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum novas_observer_place</type>
      <name>where</name>
      <anchorfile>structobserver.html</anchorfile>
      <anchor>a11f2eb69df64b674d938b34bbb842ab1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>on_surface</name>
    <filename>structon__surface.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>height</name>
      <anchorfile>structon__surface.html</anchorfile>
      <anchor>a89f6abd564014faeff7cd20c340a9c7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>humidity</name>
      <anchorfile>structon__surface.html</anchorfile>
      <anchor>a987616dbcfdfc936af3e5874ef5a41e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>latitude</name>
      <anchorfile>structon__surface.html</anchorfile>
      <anchor>a76714bdbc5c536fa77dfb14533ff82a9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>longitude</name>
      <anchorfile>structon__surface.html</anchorfile>
      <anchor>ac155e35fdeebafc89723a51520fb9fe6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>pressure</name>
      <anchorfile>structon__surface.html</anchorfile>
      <anchor>aee1c5d07ac79c5c036195858afe33405</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>temperature</name>
      <anchorfile>structon__surface.html</anchorfile>
      <anchor>ab4b11c8d9c758430960896bc3fe82ead</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>ra_of_cio</name>
    <filename>structra__of__cio.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>jd_tdb</name>
      <anchorfile>structra__of__cio.html</anchorfile>
      <anchor>a0aefc536698706b7f8a15f23168c3486</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>ra_cio</name>
      <anchorfile>structra__of__cio.html</anchorfile>
      <anchor>aba4253d16ca8c644b29da1e727d787aa</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>sky_pos</name>
    <filename>structsky__pos.html</filename>
    <member kind="variable">
      <type>double</type>
      <name>dec</name>
      <anchorfile>structsky__pos.html</anchorfile>
      <anchor>ac4ae7989694321b540abb75c30eec690</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>dis</name>
      <anchorfile>structsky__pos.html</anchorfile>
      <anchor>aeedff25e8c80502a891e8af33d35b3c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>r_hat</name>
      <anchorfile>structsky__pos.html</anchorfile>
      <anchor>a5fd669be4d5224aa192afa368a6e3173</anchor>
      <arglist>[3]</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>ra</name>
      <anchorfile>structsky__pos.html</anchorfile>
      <anchor>a713a0c71a86d92fa6892fdb2cb7c9422</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>rv</name>
      <anchorfile>structsky__pos.html</anchorfile>
      <anchor>ad9e8b5daff39072d109a6f4f1fa61f55</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>md_CHANGELOG</name>
    <title>Changelog</title>
    <filename>md_CHANGELOG.html</filename>
  </compound>
  <compound kind="page">
    <name>md_CONTRIBUTING</name>
    <title>Contributing to SuperNOVAS</title>
    <filename>md_CONTRIBUTING.html</filename>
  </compound>
  <compound kind="page">
    <name>md_LEGACY</name>
    <title>Astrometric Positions the Old Way</title>
    <filename>md_LEGACY.html</filename>
    <docanchor file="md_LEGACY.html">old-sidereal-example</docanchor>
    <docanchor file="md_LEGACY.html">old-solsys-example</docanchor>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>SuperNOVAS</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md_README-orig</docanchor>
    <docanchor file="index.html">introduction</docanchor>
    <docanchor file="index.html">fixed-issues</docanchor>
    <docanchor file="index.html">compatibility</docanchor>
    <docanchor file="index.html">installation</docanchor>
    <docanchor file="index.html">integration</docanchor>
    <docanchor file="index.html">methodologies</docanchor>
    <docanchor file="index.html">examples</docanchor>
    <docanchor file="index.html">sidereal-example</docanchor>
    <docanchor file="index.html">specify-object</docanchor>
    <docanchor file="index.html">specify-observer</docanchor>
    <docanchor file="index.html">specify-time</docanchor>
    <docanchor file="index.html">observing-frame</docanchor>
    <docanchor file="index.html">apparent-place</docanchor>
    <docanchor file="index.html">horizontal-place</docanchor>
    <docanchor file="index.html">rise-set-transit</docanchor>
    <docanchor file="index.html">solsys-example</docanchor>
    <docanchor file="index.html">transforms</docanchor>
    <docanchor file="index.html">solarsystem</docanchor>
    <docanchor file="index.html">calceph-integration</docanchor>
    <docanchor file="index.html">cspice-integration</docanchor>
    <docanchor file="index.html">universal-ephemerides</docanchor>
    <docanchor file="index.html">builtin-ephem-readers</docanchor>
    <docanchor file="index.html">explicit-ephem-linking</docanchor>
    <docanchor file="index.html">precision</docanchor>
    <docanchor file="index.html">tips</docanchor>
    <docanchor file="index.html">cpp-headers</docanchor>
    <docanchor file="index.html">accuracy-notes</docanchor>
    <docanchor file="index.html">multi-threading</docanchor>
    <docanchor file="index.html">physical-units</docanchor>
    <docanchor file="index.html">string-times-and-angles</docanchor>
    <docanchor file="index.html">string-dates</docanchor>
    <docanchor file="index.html">debug-support</docanchor>
    <docanchor file="index.html">benchmarks</docanchor>
    <docanchor file="index.html">supernovas-features</docanchor>
    <docanchor file="index.html">added-functionality</docanchor>
    <docanchor file="index.html">api-changes</docanchor>
    <docanchor file="index.html">release-schedule</docanchor>
  </compound>
</tagfile>
