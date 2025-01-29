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
      <name>NOVAS_DEGREE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6d9698dbaf6260d7fe8a531a0e96d2a8</anchor>
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
      <name>NOVAS_OBJECT_TYPES</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a60037f5a7c8fe2b7cf8893cf461598d1</anchor>
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
      <name>NOVAS_CALENDAR_OF_DATE</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa8cfc18bfdf8ebd0198065c9d07e3bbaa714288c1e87219a83b61acebb2341728</anchor>
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
      <anchor>a84516e14b76ec0b8ad5b9a9049a2db81</anchor>
      <arglist>(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>app_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abceec12c6cac2693b45b7778374455f4</anchor>
      <arglist>(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
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
      <anchor>a5ee352727a9c2d181d6bd7d097f965d6</anchor>
      <arglist>(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>astro_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a60ea97a77320c52b566f8e8fb5faf304</anchor>
      <arglist>(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>bary2obs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aada778ddcc75a355b6753f5372775094</anchor>
      <arglist>(const double *pos, const double *pos_obs, double *out, double *lighttime)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cal_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a410a61c67f5893584b09953fa4885fe5</anchor>
      <arglist>(double tjd, short *year, short *month, short *day, double *hour)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cel2ter</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a95f02377ce41d4e251654449dda82986</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class class, double xp, double yp, const double *in, double *out)</arglist>
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
      <anchor>a2ed632ae8df4a3737c200c96fe2362e2</anchor>
      <arglist>(double jd_tdb, long n_pts, ra_of_cio *cio)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_basis</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a07f83fbb8a57cf442c264ecdc979475e</anchor>
      <arglist>(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy, double *x, double *y, double *z)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_location</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a43141f0ea4598d6c87193c3ea594dd6c</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *ra_cio, short *loc_type)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_ra</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a94ffd7dee2728748af9159d92e1efb6c</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double *ra_cio)</arglist>
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
      <anchor>a6408979487e7fdf65fe7902b0400ce87</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *mobl, double *tobl, double *ee, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>ecl2equ</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afb3abc403e84a4821c3d00dc9186eed5</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat, double *ra, double *dec)</arglist>
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
      <anchor>a3221fff69d0ca52ae4694ec3ab0368ad</anchor>
      <arglist>(const double *jd_tdb, const object *body, enum novas_origin origin, enum novas_accuracy accuracy, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>equ2ecl</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a98e8713c3a4cc14363246a3da757ddcd</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec, double *elon, double *elat)</arglist>
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
      <anchor>a4afca520ede383556b549bbfb0d95c16</anchor>
      <arglist>(double ra, double dec, double *glon, double *glat)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>equ2hor</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5f229207d8a775119565a52f7e586cf8</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const on_surface *location, double ra, double dec, enum novas_refraction_model ref_option, double *zd, double *az, double *rar, double *decr)</arglist>
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
      <anchor>aaac686274606e79ba666f94306eece8b</anchor>
      <arglist>(double t, novas_delaunay_args *a)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gal2equ</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af4cce5aa6c74794a0b89176037305181</anchor>
      <arglist>(double glon, double glat, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>gcrs2equ</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a41f6507aa31cd35eff54f9c430ce78c0</anchor>
      <arglist>(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg, double *ra, double *dec)</arglist>
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
      <anchor>a581ef7bb3d2261a105d7086fc1768c97</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, double *pos, double *vel)</arglist>
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
      <anchor>aa798460894aa44f8564569d9ea4bf1ef</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, const novas_planet_bundle *planets, double *out)</arglist>
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
      <anchor>ae68d01655c4b37b5a3e1422f0062f94c</anchor>
      <arglist>(const double *pos_app, const double *pos_obs, const novas_planet_bundle *planets, double *out)</arglist>
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
      <anchor>ad290005b13558de3927e455775b32e0f</anchor>
      <arglist>(const on_surface *location, double az, double za, double *itrs)</arglist>
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
      <anchor>a296fcf0239f66eb19732f5072fed6d6e</anchor>
      <arglist>(const on_surface *location, const double *itrs, double *az, double *za)</arglist>
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
      <anchor>ac37988ea091479b766590e472293439b</anchor>
      <arglist>(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *pos_src_obs, double *tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>light_time2</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a8a93137b6201bedd8d47cc954684ebe3</anchor>
      <arglist>(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *p_src_obs, double *v_ssb, double *tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>limb_angle</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0e4364546c40854eb63211bd6cd0e70b</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, double *limb_ang, double *nadir_ang)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac036606c424c30a5600049b0cd38d868</anchor>
      <arglist>(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0f24eef523d0a8e3252002654389bd5c</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
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
      <anchor>a9950f7973fc3b5d40b0d0a9f4eceddac</anchor>
      <arglist>(const char *star_name, const char *catalog, long cat_num, double ra, double dec, double pm_ra, double pm_dec, double parallax, double rad_vel, cat_entry *star)</arglist>
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
      <anchor>a6802265b4ac76628f22260ba23ac2352</anchor>
      <arglist>(observer *obs)</arglist>
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
      <anchor>a00d9f6909923cd3e559c8f011ff9b794</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_on_surface</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9174488cd701172531174d85ed26819b</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, on_surface *loc)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_orbital_object</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3c5e215babeeba078ca7b3e9be42e555</anchor>
      <arglist>(const char *name, long num, const novas_orbital *orbit, object *body)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a618bbba887b9bc9e7df878234c66c215</anchor>
      <arglist>(enum novas_planet num, object *planet)</arglist>
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
      <anchor>a8bd9c6d2127483551c98300e264977f5</anchor>
      <arglist>(double jd_tt, double tra, double tdec, enum novas_accuracy accuracy, double *ira, double *idec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>mod_to_gcrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad9cc07f0e6af9339096e15a440b113e5</anchor>
      <arglist>(double jd_tdb, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>norm_ang</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a416fd587496210c45028352f409fd73f</anchor>
      <arglist>(double angle)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_app_to_geom</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a83db7db5a0ed0311610391f199b9f66c</anchor>
      <arglist>(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, double dist, double *geom_icrs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_app_to_hor</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae84debf8ab4d3b16feaaefdcb0ebbd08</anchor>
      <arglist>(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, RefractionModel ref_model, double *az, double *el)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_calendar_to_jd</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2e2c0999c4baf5aee06d8fad03640c22</anchor>
      <arglist>(enum novas_calendar_type calendar, short year, short month, short day, double hour)</arglist>
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
      <type>double</type>
      <name>novas_dms_degrees</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abf05d010ebf128f6300d4cc378616d90</anchor>
      <arglist>(const char *dms)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_e2h_offset</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>afd4387a6a7b3b5a8f6404ce27117adb3</anchor>
      <arglist>(double dra, double ddec, double pa, double *daz, double *del)</arglist>
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
      <name>novas_equ_sep</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4f7e06b2fa5b3145b37aec4f9c2d1602</anchor>
      <arglist>(double ra1, double dec1, double ra2, double dec2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_equ_track</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a407d7eda3fe3d9dac9fa88d90bf6c932</anchor>
      <arglist>(const object *source, const novas_frame *frame, double dt, novas_track *track)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_frame_lst</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4f05d543ba427bb850425a06850ee054</anchor>
      <arglist>(const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_posvel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae6f4d3710a277c8b284487fe175e4d49</anchor>
      <arglist>(const object *source, const novas_frame *frame, enum novas_reference_system sys, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_to_app</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ad412ac774327a4120237dc3b3110f9a0</anchor>
      <arglist>(const novas_frame *frame, const double *pos, enum novas_reference_system sys, sky_pos *out)</arglist>
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
      <anchor>ab4ea1b646b23a76936b2a6459b1bac6d</anchor>
      <arglist>(const novas_timespec *time, enum novas_timescale timescale, long *ijd)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_get_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa65f86f70b2ea6e77bf0799d6859ca05</anchor>
      <arglist>(const novas_timespec *time, enum novas_timescale timescale)</arglist>
    </member>
    <member kind="function">
      <type>time_t</type>
      <name>novas_get_unix_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa29034a9e6579d5982083c5c6ea8d773</anchor>
      <arglist>(const novas_timespec *time, long *nanos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_h2e_offset</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aacd8ca48294bf635e5adc60084db0f26</anchor>
      <arglist>(double daz, double del, double pa, double *dra, double *ddec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_hms_hours</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5f564fbf4c22367d7eed6a060a99cc37</anchor>
      <arglist>(const char *hms)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_to_app</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4849c7fca260cfb7c1d716566f4170ac</anchor>
      <arglist>(const novas_frame *frame, double az, double el, RefractionModel ref_model, enum novas_reference_system sys, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_track</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a891866f26535e79526ed85e52e2ee67c</anchor>
      <arglist>(const object *source, const novas_frame *frame, RefractionModel ref_model, novas_track *track)</arglist>
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
      <anchor>a93ac897d3f7a2f2a7f8d92f3e8cd9ea0</anchor>
      <arglist>(RefractionModel model, double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el0)</arglist>
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
      <anchor>a1b7e1fa279dc7502805cc76af2ae07fe</anchor>
      <arglist>(const novas_timespec *time, char *dst, int maxlen)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_jd_to_calendar</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6cd9d6258777678f126c60de9999cecd</anchor>
      <arglist>(double tjd, enum novas_calendar_type calendar, int *year, int *month, int *day, double *hour)</arglist>
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
      <name>novas_moon_angle</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6b829d25720d5bd8e6dae228b6fa690b</anchor>
      <arglist>(const object *source, const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_object_sep</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa12a28c250dbb1e452f1979317216baf</anchor>
      <arglist>(const object *source1, const object *source2, const novas_frame *frame)</arglist>
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
      <type>int</type>
      <name>novas_orbit_posvel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a445ad14faede9dc912601de27fb52bc0</anchor>
      <arglist>(double jd_tdb, const novas_orbital *orbit, enum novas_accuracy accuracy, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_date</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a48866c233886213c4d347efb5ce30dd1</anchor>
      <arglist>(const char *date, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_date_format</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab7b63f4a8cb2c50d4cbc972d2ffc4122</anchor>
      <arglist>(enum novas_calendar_type calendar, enum novas_date_format format, const char *date, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_dms</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3966d939b4bd0233f13ed1a9e8ae1f80</anchor>
      <arglist>(const char *str, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_hms</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ab731deea99a64dc57cd8d4ee55b4d60d</anchor>
      <arglist>(const char *str, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_planet</type>
      <name>novas_planet_for_name</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a92e869b101bd41e31ac6d3a2419bb0a3</anchor>
      <arglist>(const char *name)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_timescale</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a18bd698f5c504b5f0a581227e1649bd2</anchor>
      <arglist>(enum novas_timescale scale, char *buf)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_radio_refraction</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6b49f92f8f818f2272613e3432185a39</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_rises_above</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ae947cd79282aafd79436a1bee4b2fd8a</anchor>
      <arglist>(double el, const object *source, const novas_frame *frame, RefractionModel ref_model)</arglist>
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
      <name>novas_set_orbsys_pole</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aee0ed43252017745574fa1e4b84a7b11</anchor>
      <arglist>(enum novas_reference_system type, double ra, double dec, novas_orbital_system *sys)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_split_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a37c8f2d5e1ad2daed78d1bd7991fdba4</anchor>
      <arglist>(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1, novas_timespec *time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>ac272a018a213965f07b21ac6facb4a62</anchor>
      <arglist>(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_unix_time</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>abacd89aa380c85da2d3c19a7e2187df3</anchor>
      <arglist>(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *time)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sets_below</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a0c2383896d4bfba77a91272aa09d6a24</anchor>
      <arglist>(double el, const object *source, const novas_frame *frame, RefractionModel ref_model)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_sky_pos</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4e894a83f3065bbb706212d1267dd2ab</anchor>
      <arglist>(const object *object, const novas_frame *frame, enum novas_reference_system sys, sky_pos *out)</arglist>
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
      <name>novas_sun_angle</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a1a1323a247aa7164c73b16148eb32411</anchor>
      <arglist>(const object *source, const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_timescale</type>
      <name>novas_timescale_for_string</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5e447ec1ea688fe3fb688387ac669f7a</anchor>
      <arglist>(const char *str)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_timestamp</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9bc66e41d10e6a6b88cd6a352ab9da17</anchor>
      <arglist>(const novas_timespec *time, enum novas_timescale scale, char *dst, int maxlen)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_track_pos</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a24d627df1c1788bcaae4a768da63c18c</anchor>
      <arglist>(const novas_track *track, const novas_timespec *time, double *lon, double *lat, double *dist, double *z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_sky_pos</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a88972e93665bb1903795bfff264c14f6</anchor>
      <arglist>(const sky_pos *in, const novas_transform *transform, sky_pos *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_vector</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aadb7bc88119eed4ea54373a42e22a0d4</anchor>
      <arglist>(const double *in, const novas_transform *transform, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_v2z</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa54cd00024b294d38fa29433c017c65f</anchor>
      <arglist>(double vel)</arglist>
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
      <anchor>a0dc364ae1963da0b916ed5a9d0862d3e</anchor>
      <arglist>(double t, enum novas_accuracy accuracy, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_planets</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acbc29fe86915ef8a3701795dc9fd9b34</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *pos_obs, int pl_mask, novas_planet_bundle *planets)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_posvel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a5a62b6e1ef8840a1203c6d2a75d17152</anchor>
      <arglist>(double jd_tdb, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, const double *geo_pos, const double *geo_vel, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>place</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a263de1618d80fcc737984951fa97c5a3</anchor>
      <arglist>(double jd_tt, const object *source, const observer *location, double ut1_to_tt, enum novas_reference_system coord_sys, enum novas_accuracy accuracy, sky_pos *output)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_cirs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aebad3c9b51f62cb7b31a686b5ec890a9</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_gcrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acbcd90c9111cc5580a5c2bac404f7f00</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_icrs</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a4ef1b6d204e22608e964e60cf34a15ad</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_j2000</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa3da1221ec52e427f9571d12f6f914ae</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_mod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a3f055f3994889e3345d5267dac671a60</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a078b5e764d5cf3fdca600fada992b409</anchor>
      <arglist>(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt, enum novas_reference_system system, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_tod</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a2c8817db8a00764da4c285b32590ccfb</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
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
      <anchor>ade8207981b6f840d1f0a79dd0c72beb2</anchor>
      <arglist>(double jd_tdb_in, const double *pos, const double *vel, double jd_tdb_out, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rad_vel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a9f76e2386515e2023172e05891c3fa28</anchor>
      <arglist>(const object *source, const double *pos_src, const double *vel_src, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun, double *rv)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rad_vel2</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a191dcbb360afb9b6bf123d46858156d0</anchor>
      <arglist>(const object *source, const double *pos_emit, const double *vel_src, const double *pos_det, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec2vector</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acf86ce700b3d3509ebba13e5f16a4327</anchor>
      <arglist>(double ra, double dec, double dist, double *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a11d3da697a640c39d87cf201ac54ca7c</anchor>
      <arglist>(double jd_tt, const object *ss_body, const observer *obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *ra, double *dec, double *dis, double *rv)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aed179ad664f4ff023deb540140f3c2ef</anchor>
      <arglist>(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *ra, double *dec, double *rv)</arglist>
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
      <anchor>a151b74255ea04462abc5503c6afddcae</anchor>
      <arglist>(const on_surface *location, enum novas_refraction_model option, double zd_obs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>refract_astro</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa41408d8cbbe3cfc12930414a9c79fc7</anchor>
      <arglist>(const on_surface *location, enum novas_refraction_model option, double zd_astro)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_cio_locator_file</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a13c40b89c2cecd6a23720d181a800591</anchor>
      <arglist>(const char *filename)</arglist>
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
      <anchor>a3236e72771f2c363f342547d80f5d50f</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_equinox_type gst_type, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, double *gst)</arglist>
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
      <anchor>a98c5f0e28777954cec87abfedc8ca7a9</anchor>
      <arglist>(const cat_entry *star, double *pos, double *motion)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tdb2tt</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a45b1a5d3cf875521f3176bc301633d79</anchor>
      <arglist>(double jd_tdb, double *jd_tt, double *secdiff)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ter2cel</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a6b662ff99b35db19c9b6b24a1ae9264f</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class class, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>terra</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aa111f8d21d960677fb91139adc96ff17</anchor>
      <arglist>(const on_surface *location, double lst, double *pos, double *vel)</arglist>
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
      <anchor>a48cf19bbddaeeb698bb921575b15e23e</anchor>
      <arglist>(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>topo_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a221884a610d2866c7e7303cd7b94182c</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
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
      <name>unredshift_vrad</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>aea89203030ca831260dec18f83081d23</anchor>
      <arglist>(double vrad, double z)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>vector2radec</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>a882279981a7d5a67ed596fa0937a12c1</anchor>
      <arglist>(const double *pos, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_planet</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>af47d81c6aaa03c8af78eb251c8894bde</anchor>
      <arglist>(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_star</name>
      <anchorfile>novas_8h.html</anchorfile>
      <anchor>acd47f30801a6676afb5e0614eaefa569</anchor>
      <arglist>(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
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
    <name>novascon.h</name>
    <path>include/</path>
    <filename>novascon_8h.html</filename>
    <member kind="variable">
      <type>const double</type>
      <name>ANGVEL</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a088f463ef64dea2727d8ade9b1cd7192</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>AU</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a2b9e1ccc1a034cb26b9c794767315346</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>AU_KM</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a95419ca41731064148e6fcfb0e919171</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>AU_SEC</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>abcfdf85edc41c59ae55c3c45dc9c506a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>C</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a9e8a46a0e00368ad98642587ca4ebdbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>C_AUDAY</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a55379c7fece80070614c09bb218275b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>ERAD</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>aa7bbeb0cb618d36be058e8fd92133b4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>F</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a480d6423b1e9ca40c69c5a83458b49b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const short</type>
      <name>FN0</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a2d8f405dc66e829e1869bea6ffb86a44</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const short</type>
      <name>FN1</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>af275414414fd105a1702ca74374d1f70</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>GE</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a0ee5d7a271610d60ec10f180747886eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>GS</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a5060445c439eb63c5fb506fa625ca343</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>RMASS</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a19a7c107c3918c90c90cdad5ab310b1a</anchor>
      <arglist>[12]</arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>T0</name>
      <anchorfile>novascon_8h.html</anchorfile>
      <anchor>a179a958e126d46c9b952672120986924</anchor>
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
      <anchor>ab4481ae5926c3ba5810d2b9a9e010636</anchor>
      <arglist>)(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>iau2000a</name>
      <anchorfile>nutation_8h.html</anchorfile>
      <anchor>a63ac0d33e3f774cd89281a00e2918b98</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>iau2000b</name>
      <anchorfile>nutation_8h.html</anchorfile>
      <anchor>ae1a30706385dd901d96d28c816b51c87</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nu2000k</name>
      <anchorfile>nutation_8h.html</anchorfile>
      <anchor>ab17083c5bc84987d4dda1172a4dab843</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>solarsystem.h</name>
    <path>include/</path>
    <filename>solarsystem_8h.html</filename>
    <includes id="novas_8h" name="novas.h" local="no" import="no" module="no" objc="no">novas.h</includes>
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
      <anchor>ac2ae695519a5b600a105243f4e30c3ab</anchor>
      <arglist>)(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *origin, double *pos, double *vel)</arglist>
    </member>
    <member kind="typedef">
      <type>short(*</type>
      <name>novas_planet_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ae7eb9e64aa00ee757946488fa99351ec</anchor>
      <arglist>)(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="typedef">
      <type>short(*</type>
      <name>novas_planet_provider_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a85ed597fa94ecf72653e64f4afa7f833</anchor>
      <arglist>)(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
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
      <anchor>ab5d74f84f8e75e90dfc30ebde562d076</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>earth_sun_calc_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a3e6398e29968a48f9de090e82423750f</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
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
      <type>enum novas_planet</type>
      <name>naif_to_novas_planet</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a1c4b3f67a3b82808dd16ae43dfaf0f86</anchor>
      <arglist>(long id)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_helio_dist</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a13b1204e7ad069e62b0cb7024fbfee0d</anchor>
      <arglist>(double jd_tdb, const object *source, double *rate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_illum</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a3cdf3285614f58221cda45adb297112b</anchor>
      <arglist>(const object *source, const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_power</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ad03d69e61d1a2d5da8063fcd0c05ab7c</anchor>
      <arglist>(double jd_tdb, const object *source)</arglist>
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
      <anchor>a71d2bfef45818d56e359205c57db0694</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_eph_manager_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>acacfbde919107d0514da5c7d8508917b</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_ephem_provider</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a61c60adf57c00d797fac94bb5841c271</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_ephem_provider_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>ae19eb2bcaaa2deb9fe27fba8090724e1</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_jplint</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>aceb7bc6823b1823d8a8360517ff58f29</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_jplint_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a399a5363ddf93d662886e6e4692d03ad</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>double *</type>
      <name>readeph</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>abb70fdad016c3a2ea45990eaa16b9245</anchor>
      <arglist>(int mp, const char *name, double jd_tdb, int *error)</arglist>
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
      <anchor>a5e2d599b1dc7a0e87bfd29dbd6413398</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solarsystem_8h.html</anchorfile>
      <anchor>a0243c2489c22860de898115aed8ffc87</anchor>
      <arglist>(const double jd_tdb[2], short body, short origin, double *position, double *velocity)</arglist>
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
      <anchor>a83db7db5a0ed0311610391f199b9f66c</anchor>
      <arglist>(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, double dist, double *geom_icrs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_app_to_hor</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ae84debf8ab4d3b16feaaefdcb0ebbd08</anchor>
      <arglist>(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, RefractionModel ref_model, double *az, double *el)</arglist>
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
      <anchor>a407d7eda3fe3d9dac9fa88d90bf6c932</anchor>
      <arglist>(const object *source, const novas_frame *frame, double dt, novas_track *track)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_frame_lst</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a4f05d543ba427bb850425a06850ee054</anchor>
      <arglist>(const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_posvel</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ae6f4d3710a277c8b284487fe175e4d49</anchor>
      <arglist>(const object *source, const novas_frame *frame, enum novas_reference_system sys, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_geom_to_app</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ad412ac774327a4120237dc3b3110f9a0</anchor>
      <arglist>(const novas_frame *frame, const double *pos, enum novas_reference_system sys, sky_pos *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_to_app</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a4849c7fca260cfb7c1d716566f4170ac</anchor>
      <arglist>(const novas_frame *frame, double az, double el, RefractionModel ref_model, enum novas_reference_system sys, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_hor_track</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a891866f26535e79526ed85e52e2ee67c</anchor>
      <arglist>(const object *source, const novas_frame *frame, RefractionModel ref_model, novas_track *track)</arglist>
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
      <anchor>a6b829d25720d5bd8e6dae228b6fa690b</anchor>
      <arglist>(const object *source, const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_object_sep</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>aa12a28c250dbb1e452f1979317216baf</anchor>
      <arglist>(const object *source1, const object *source2, const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_rises_above</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>ae947cd79282aafd79436a1bee4b2fd8a</anchor>
      <arglist>(double el, const object *source, const novas_frame *frame, RefractionModel ref_model)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sets_below</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a0c2383896d4bfba77a91272aa09d6a24</anchor>
      <arglist>(double el, const object *source, const novas_frame *frame, RefractionModel ref_model)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_sky_pos</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a4e894a83f3065bbb706212d1267dd2ab</anchor>
      <arglist>(const object *object, const novas_frame *frame, enum novas_reference_system sys, sky_pos *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_illum</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a3cdf3285614f58221cda45adb297112b</anchor>
      <arglist>(const object *source, const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sun_angle</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a1a1323a247aa7164c73b16148eb32411</anchor>
      <arglist>(const object *source, const novas_frame *frame)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_track_pos</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a24d627df1c1788bcaae4a768da63c18c</anchor>
      <arglist>(const novas_track *track, const novas_timespec *time, double *lon, double *lat, double *dist, double *z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_sky_pos</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>a88972e93665bb1903795bfff264c14f6</anchor>
      <arglist>(const sky_pos *in, const novas_transform *transform, sky_pos *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_transform_vector</name>
      <anchorfile>frames_8c.html</anchorfile>
      <anchor>aadb7bc88119eed4ea54373a42e22a0d4</anchor>
      <arglist>(const double *in, const novas_transform *transform, double *out)</arglist>
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
    <name>novas.c</name>
    <path>src/</path>
    <filename>novas_8c.html</filename>
    <member kind="function">
      <type>int</type>
      <name>aberration</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a033f1bf4b01e0e21a0e05e13c19c02f6</anchor>
      <arglist>(const double *pos, const double *vobs, double lighttime, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>accum_prec</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ad1de5b481e41cf193c199bd0dc5084bd</anchor>
      <arglist>(double t)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>app_planet</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a84516e14b76ec0b8ad5b9a9049a2db81</anchor>
      <arglist>(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>app_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>abceec12c6cac2693b45b7778374455f4</anchor>
      <arglist>(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>astro_planet</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a5ee352727a9c2d181d6bd7d097f965d6</anchor>
      <arglist>(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>astro_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a60ea97a77320c52b566f8e8fb5faf304</anchor>
      <arglist>(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>bary2obs</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aada778ddcc75a355b6753f5372775094</anchor>
      <arglist>(const double *pos, const double *pos_obs, double *out, double *lighttime)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cal_date</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a410a61c67f5893584b09953fa4885fe5</anchor>
      <arglist>(double tjd, short *year, short *month, short *day, double *hour)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cel2ter</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a95f02377ce41d4e251654449dda82986</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class class, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cel_pole</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a8937180de2ee1545d19b8fc840f35fe2</anchor>
      <arglist>(double jd_tt, enum novas_pole_offset_type type, double dpole1, double dpole2)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_array</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a2ed632ae8df4a3737c200c96fe2362e2</anchor>
      <arglist>(double jd_tdb, long n_pts, ra_of_cio *cio)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_basis</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a07f83fbb8a57cf442c264ecdc979475e</anchor>
      <arglist>(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy, double *x, double *y, double *z)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_location</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a43141f0ea4598d6c87193c3ea594dd6c</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *ra_cio, short *loc_type)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>cio_ra</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a94ffd7dee2728748af9159d92e1efb6c</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double *ra_cio)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_gcrs</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ac0ba048cd1732d0d0398397396d158bc</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>d_light</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ad799864f637c483fb49491bde5a93e25</anchor>
      <arglist>(const double *pos_src, const double *pos_body)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>e_tilt</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6408979487e7fdf65fe7902b0400ce87</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, double *mobl, double *tobl, double *ee, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ecl2equ_vec</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6fa1eada41005ac70290d0bb5df1ec26</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>ee_ct</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>add7a260017fad0d59c943a0a8d935fcf</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ephemeris</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a3221fff69d0ca52ae4694ec3ab0368ad</anchor>
      <arglist>(const double *jd_tdb, const object *body, enum novas_origin origin, enum novas_accuracy accuracy, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>equ2ecl</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a98e8713c3a4cc14363246a3da757ddcd</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec, double *elon, double *elat)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>equ2ecl_vec</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aa05ecc76c1ccc0c3445f8b642d6c5311</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>equ2gal</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a4afca520ede383556b549bbfb0d95c16</anchor>
      <arglist>(double ra, double dec, double *glon, double *glat)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>equ2hor</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a5f229207d8a775119565a52f7e586cf8</anchor>
      <arglist>(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const on_surface *location, double ra, double dec, enum novas_refraction_model ref_option, double *zd, double *az, double *rar, double *decr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>era</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aef1f8daaad470353648de90576aefc66</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>frame_tie</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aa11d2253a44261370af8d0c32f0715e9</anchor>
      <arglist>(const double *in, enum novas_frametie_direction direction, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>fund_args</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aaac686274606e79ba666f94306eece8b</anchor>
      <arglist>(double t, novas_delaunay_args *a)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>gcrs2equ</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a41f6507aa31cd35eff54f9c430ce78c0</anchor>
      <arglist>(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_cirs</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a51ea4a016336dd3cf7783061c14d165c</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_j2000</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aeea4b5085c7267714492fae6108fb975</anchor>
      <arglist>(const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_mod</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a75de6cca190c5fbc3056db726f317c7f</anchor>
      <arglist>(double jd_tdb, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gcrs_to_tod</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a9c900b81a718a184281a8a0b733b4db7</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>geo_posvel</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a581ef7bb3d2261a105d7086fc1768c97</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>novas_ephem_provider</type>
      <name>get_ephem_provider</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aa6fad38297a49ba78a6bd49cedf889b1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>novas_planet_provider</type>
      <name>get_planet_provider</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a0f0a786b5614a788c02b37a462eae2d2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>novas_planet_provider_hp</type>
      <name>get_planet_provider_hp</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ae45badb450c1ca73920d982e12346c5b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>grav_def</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a48f90ac20950507fad9b880a7f7f2383</anchor>
      <arglist>(double jd_tdb, enum novas_observer_place unused, enum novas_accuracy accuracy, const double *pos_src, const double *pos_obs, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_planets</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aa798460894aa44f8564569d9ea4bf1ef</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, const novas_planet_bundle *planets, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_vec</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a08a008572e2746a70bec19739b5fb7e6</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, const double *pos_body, double rmass, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>ira_equinox</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aea0e1028baf14d16d9de800e36a17086</anchor>
      <arglist>(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>j2000_to_tod</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a02fa1ee652c20160055cbd2c7bd9ca63</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>julian_date</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a7cd85bc0a0248f84d399cbbce9fe9546</anchor>
      <arglist>(short year, short month, short day, double hour)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>light_time</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ac37988ea091479b766590e472293439b</anchor>
      <arglist>(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *pos_src_obs, double *tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>light_time2</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a8a93137b6201bedd8d47cc954684ebe3</anchor>
      <arglist>(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *p_src_obs, double *v_ssb, double *tlight)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>limb_angle</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a0e4364546c40854eb63211bd6cd0e70b</anchor>
      <arglist>(const double *pos_src, const double *pos_obs, double *limb_ang, double *nadir_ang)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_planet</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ac036606c424c30a5600049b0cd38d868</anchor>
      <arglist>(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>local_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a0f24eef523d0a8e3252002654389bd5c</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_cat_entry</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a9950f7973fc3b5d40b0d0a9f4eceddac</anchor>
      <arglist>(const char *star_name, const char *catalog, long cat_num, double ra, double dec, double pm_ra, double pm_dec, double parallax, double rad_vel, cat_entry *star)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_in_space</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ac0ca6f330e1795ce811df95986400632</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, in_space *loc)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_object</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a411f70fb77582d750c5269fcd64b0ce2</anchor>
      <arglist>(enum novas_object_type type, long number, const char *name, const cat_entry *star, object *source)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>make_observer</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a0f2096c7954b583fe749553b956ff9d2</anchor>
      <arglist>(enum novas_observer_place where, const on_surface *loc_surface, const in_space *loc_space, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_at_geocenter</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6802265b4ac76628f22260ba23ac2352</anchor>
      <arglist>(observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_in_space</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6c146e4d10152d5e1ffb978f2fc962be</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_observer_on_surface</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a00d9f6909923cd3e559c8f011ff9b794</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_on_surface</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a9174488cd701172531174d85ed26819b</anchor>
      <arglist>(double latitude, double longitude, double height, double temperature, double pressure, on_surface *loc)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_planet</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a618bbba887b9bc9e7df878234c66c215</anchor>
      <arglist>(enum novas_planet num, object *planet)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean_obliq</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>af182ee8cc4239f581f746aa974827d61</anchor>
      <arglist>(double jd_tdb)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>mean_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a8bd9c6d2127483551c98300e264977f5</anchor>
      <arglist>(double jd_tt, double tra, double tdec, enum novas_accuracy accuracy, double *ira, double *idec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>mod_to_gcrs</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ad9cc07f0e6af9339096e15a440b113e5</anchor>
      <arglist>(double jd_tdb, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>norm_ang</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a416fd587496210c45028352f409fd73f</anchor>
      <arglist>(double angle)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_calendar_to_jd</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a2e2c0999c4baf5aee06d8fad03640c22</anchor>
      <arglist>(enum novas_calendar_type calendar, short year, short month, short day, double hour)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>novas_case_sensitive</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a8f832afe92680a11c3489aaa224ac2e2</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>novas_debug</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a5f35633b872751df4dacd2fb2cbe4ac3</anchor>
      <arglist>(enum novas_debug_mode mode)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_debug_mode</type>
      <name>novas_get_debug_mode</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a29cad12337acfce5f1e370320dcefb79</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_jd_to_calendar</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6cd9d6258777678f126c60de9999cecd</anchor>
      <arglist>(double tjd, enum novas_calendar_type calendar, int *year, int *month, int *day, double *hour)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_los_to_xyz</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a601d246673742cf72a5dbe1449c97560</anchor>
      <arglist>(const double *los, double lon, double lat, double *xyz)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_orbit_posvel</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a445ad14faede9dc912601de27fb52bc0</anchor>
      <arglist>(double jd_tdb, const novas_orbital *orbit, enum novas_accuracy accuracy, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_xyz_to_los</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a14deb6fdce80ba213e1f1d5238306e17</anchor>
      <arglist>(const double *xyz, double lon, double lat, double *los)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z2v</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a63621cea3b4985c1bfcc4514a7aa3fb1</anchor>
      <arglist>(double z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nutation</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a69df5671751c09a7575963b94ad1151e</anchor>
      <arglist>(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nutation_angles</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a0dc364ae1963da0b916ed5a9d0862d3e</anchor>
      <arglist>(double t, enum novas_accuracy accuracy, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_planets</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>acbc29fe86915ef8a3701795dc9fd9b34</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *pos_obs, int pl_mask, novas_planet_bundle *planets)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>obs_posvel</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a5a62b6e1ef8840a1203c6d2a75d17152</anchor>
      <arglist>(double jd_tdb, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, const double *geo_pos, const double *geo_vel, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>place</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a263de1618d80fcc737984951fa97c5a3</anchor>
      <arglist>(double jd_tt, const object *source, const observer *location, double ut1_to_tt, enum novas_reference_system coord_sys, enum novas_accuracy accuracy, sky_pos *output)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a078b5e764d5cf3fdca600fada992b409</anchor>
      <arglist>(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt, enum novas_reference_system system, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>planet_lon</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aacc6a0daf29b8bdc18f46647a94694d3</anchor>
      <arglist>(double t, enum novas_planet planet)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>precession</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a9178358732325399f3a5e3582080ced3</anchor>
      <arglist>(double jd_tdb_in, const double *in, double jd_tdb_out, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>proper_motion</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ade8207981b6f840d1f0a79dd0c72beb2</anchor>
      <arglist>(double jd_tdb_in, const double *pos, const double *vel, double jd_tdb_out, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rad_vel</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a9f76e2386515e2023172e05891c3fa28</anchor>
      <arglist>(const object *source, const double *pos_src, const double *vel_src, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun, double *rv)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rad_vel2</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a191dcbb360afb9b6bf123d46858156d0</anchor>
      <arglist>(const object *source, const double *pos_emit, const double *vel_src, const double *pos_det, const double *vel_obs, double d_obs_geo, double d_obs_sun, double d_src_sun)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec2vector</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>acf86ce700b3d3509ebba13e5f16a4327</anchor>
      <arglist>(double ra, double dec, double dist, double *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_planet</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a11d3da697a640c39d87cf201ac54ca7c</anchor>
      <arglist>(double jd_tt, const object *ss_body, const observer *obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *ra, double *dec, double *dis, double *rv)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>radec_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aed179ad664f4ff023deb540140f3c2ef</anchor>
      <arglist>(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt, enum novas_reference_system sys, enum novas_accuracy accuracy, double *ra, double *dec, double *rv)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>refract</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a151b74255ea04462abc5503c6afddcae</anchor>
      <arglist>(const on_surface *location, enum novas_refraction_model option, double zd_obs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>refract_astro</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aa41408d8cbbe3cfc12930414a9c79fc7</anchor>
      <arglist>(const on_surface *location, enum novas_refraction_model option, double zd_astro)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_cio_locator_file</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a13c40b89c2cecd6a23720d181a800591</anchor>
      <arglist>(const char *filename)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_ephem_provider</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6097083c438a8d1872c2e00a66360030</anchor>
      <arglist>(novas_ephem_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_nutation_lp_provider</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a694288eb631d6a69ecf313b7b5b4a677</anchor>
      <arglist>(novas_nutation_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_planet_provider</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a9904c454bc7148b42d0f927b82ea3973</anchor>
      <arglist>(novas_planet_provider func)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>set_planet_provider_hp</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a418f45a11ce7f92ba0ef817558415640</anchor>
      <arglist>(novas_planet_provider_hp func)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>sidereal_time</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a3236e72771f2c363f342547d80f5d50f</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_equinox_type gst_type, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, double *gst)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>spin</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>af4090e519d7ce7ab1fcdb24b10126eb4</anchor>
      <arglist>(double angle, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>starvectors</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a98c5f0e28777954cec87abfedc8ca7a9</anchor>
      <arglist>(const cat_entry *star, double *pos, double *motion)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tdb2tt</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a45b1a5d3cf875521f3176bc301633d79</anchor>
      <arglist>(double jd_tdb, double *jd_tt, double *secdiff)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>ter2cel</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6b662ff99b35db19c9b6b24a1ae9264f</anchor>
      <arglist>(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, enum novas_equatorial_class class, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>terra</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aa111f8d21d960677fb91139adc96ff17</anchor>
      <arglist>(const on_surface *location, double lst, double *pos, double *vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_gcrs</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6871e7fecde44084c7b4e1b8dda5ac70</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_j2000</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a6205edb1361e56cc0c7aed0f088f7437</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>topo_planet</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a48cf19bbddaeeb698bb921575b15e23e</anchor>
      <arglist>(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>topo_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a221884a610d2866c7e7303cd7b94182c</anchor>
      <arglist>(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>transform_cat</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>abe0c6e6fc60a49efa88c365aef8cdc50</anchor>
      <arglist>(enum novas_transform_type option, double jd_tt_in, const cat_entry *in, double jd_tt_out, const char *out_id, cat_entry *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>transform_hip</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>af2c03d540de7a8bfafaed27eb9a753c1</anchor>
      <arglist>(const cat_entry *hipparcos, cat_entry *hip_2000)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tt2tdb</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a129acca41760da486e1179c93cb9e86d</anchor>
      <arglist>(double jd_tt)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>vector2radec</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a882279981a7d5a67ed596fa0937a12c1</anchor>
      <arglist>(const double *pos, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_planet</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>af47d81c6aaa03c8af78eb251c8894bde</anchor>
      <arglist>(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>virtual_star</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>acd47f30801a6676afb5e0614eaefa569</anchor>
      <arglist>(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>wobble</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>aead87744eeb3029ce7d5ffb1801ee652</anchor>
      <arglist>(double jd_tt, enum novas_wobble_direction direction, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>EPS_COR</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ac063b61c63e78a4864f2baa4606bab36</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>grav_bodies_full_accuracy</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a18647d39520d1a15a137661f698cb12a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>grav_bodies_reduced_accuracy</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a03a4df8961a0cd05f89aca478d2dcd24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>novas_inv_max_iter</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>ad2b933ce2f58e8b04bbe76ffbff99eb0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>PSI_COR</name>
      <anchorfile>novas_8c.html</anchorfile>
      <anchor>a103c28cd84e81dde365238ee3599f5b0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>novascon.c</name>
    <path>src/</path>
    <filename>novascon_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <includes id="novascon_8h" name="novascon.h" local="yes" import="no" module="no" objc="no">novascon.h</includes>
    <member kind="variable">
      <type>const double</type>
      <name>ANGVEL</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a088f463ef64dea2727d8ade9b1cd7192</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>AU</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a2b9e1ccc1a034cb26b9c794767315346</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>AU_KM</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a95419ca41731064148e6fcfb0e919171</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>AU_SEC</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>abcfdf85edc41c59ae55c3c45dc9c506a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>C</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a9e8a46a0e00368ad98642587ca4ebdbe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>C_AUDAY</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a55379c7fece80070614c09bb218275b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>ERAD</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>aa7bbeb0cb618d36be058e8fd92133b4d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>F</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a480d6423b1e9ca40c69c5a83458b49b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>GE</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a0ee5d7a271610d60ec10f180747886eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>GS</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a5060445c439eb63c5fb506fa625ca343</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>RMASS</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a19a7c107c3918c90c90cdad5ab310b1a</anchor>
      <arglist>[12]</arglist>
    </member>
    <member kind="variable">
      <type>const double</type>
      <name>T0</name>
      <anchorfile>novascon_8c.html</anchorfile>
      <anchor>a179a958e126d46c9b952672120986924</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>nutation.c</name>
    <path>src/</path>
    <filename>nutation_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="function">
      <type>int</type>
      <name>iau2000a</name>
      <anchorfile>nutation_8c.html</anchorfile>
      <anchor>a63ac0d33e3f774cd89281a00e2918b98</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>iau2000b</name>
      <anchorfile>nutation_8c.html</anchorfile>
      <anchor>ae1a30706385dd901d96d28c816b51c87</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>nu2000k</name>
      <anchorfile>nutation_8c.html</anchorfile>
      <anchor>ab17083c5bc84987d4dda1172a4dab843</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>refract.c</name>
    <path>src/</path>
    <filename>refract_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="function">
      <type>double</type>
      <name>novas_inv_refract</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>a93ac897d3f7a2f2a7f8d92f3e8cd9ea0</anchor>
      <arglist>(RefractionModel model, double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el0)</arglist>
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
      <type>double</type>
      <name>novas_standard_refraction</name>
      <anchorfile>refract_8c.html</anchorfile>
      <anchor>ab04bff3b97c132e358e11c3a5b7a3081</anchor>
      <arglist>(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el)</arglist>
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
      <anchor>a61c60adf57c00d797fac94bb5841c271</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_ephem_provider_hp</name>
      <anchorfile>solsys-ephem_8c.html</anchorfile>
      <anchor>ae19eb2bcaaa2deb9fe27fba8090724e1</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem</name>
      <anchorfile>solsys-ephem_8c.html</anchorfile>
      <anchor>a5e2d599b1dc7a0e87bfd29dbd6413398</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solsys-ephem_8c.html</anchorfile>
      <anchor>a0243c2489c22860de898115aed8ffc87</anchor>
      <arglist>(const double jd_tdb[2], short body, short origin, double *position, double *velocity)</arglist>
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
      <anchor>a71d2bfef45818d56e359205c57db0694</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_eph_manager_hp</name>
      <anchorfile>solsys1_8c.html</anchorfile>
      <anchor>acacfbde919107d0514da5c7d8508917b</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem</name>
      <anchorfile>solsys1_8c.html</anchorfile>
      <anchor>a5e2d599b1dc7a0e87bfd29dbd6413398</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solsys1_8c.html</anchorfile>
      <anchor>a0243c2489c22860de898115aed8ffc87</anchor>
      <arglist>(const double jd_tdb[2], short body, short origin, double *position, double *velocity)</arglist>
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
      <anchor>aceb7bc6823b1823d8a8360517ff58f29</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>planet_jplint_hp</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a399a5363ddf93d662886e6e4692d03ad</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a5e2d599b1dc7a0e87bfd29dbd6413398</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solsys2_8c.html</anchorfile>
      <anchor>a0243c2489c22860de898115aed8ffc87</anchor>
      <arglist>(const double jd_tdb[2], short body, short origin, double *position, double *velocity)</arglist>
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
      <anchor>ab5d74f84f8e75e90dfc30ebde562d076</anchor>
      <arglist>(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>earth_sun_calc_hp</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a3e6398e29968a48f9de090e82423750f</anchor>
      <arglist>(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enable_earth_sun_hp</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a279bca0519117cee9a3bd4382f38e7ad</anchor>
      <arglist>(int value)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a5e2d599b1dc7a0e87bfd29dbd6413398</anchor>
      <arglist>(double jd_tdb, short body, short origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>short</type>
      <name>solarsystem_hp</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>a0243c2489c22860de898115aed8ffc87</anchor>
      <arglist>(const double jd_tdb[2], short body, short origin, double *position, double *velocity)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>sun_eph</name>
      <anchorfile>solsys3_8c.html</anchorfile>
      <anchor>adc71aa5ed911942370abde4cf9b7c2dc</anchor>
      <arglist>(double jd, double *ra, double *dec, double *dis)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>super.c</name>
    <path>src/</path>
    <filename>super_8c.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>_DEFAULT_SOURCE</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a8fb447618db946a9e2a596d9ea18763f</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>app_to_cirs_ra</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a3fa57a154f2f423612736e5e3a5addbf</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double ra)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cirs_to_app_ra</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>af90d52a6527d9b62b8ac35d8c1ac7c6b</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, double ra)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_itrs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aa2768d89d499a7155f90a6fd8c176d8d</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cirs_to_tod</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a9cac4f3a18d4c14939519e7123b4d504</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>ecl2equ</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>afb3abc403e84a4821c3d00dc9186eed5</anchor>
      <arglist>(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gal2equ</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>af4cce5aa6c74794a0b89176037305181</anchor>
      <arglist>(double glon, double glat, double *ra, double *dec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>get_ut1_to_tt</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a4d7470ca705efa1953e8b4beab77c6b6</anchor>
      <arglist>(int leap_seconds, double dut1)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>get_utc_to_tt</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>ab7611704bab9ce717744a8b5575c5378</anchor>
      <arglist>(int leap_seconds)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>grav_redshift</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a3ce6b306bc183387d0aaac97abbd82e8</anchor>
      <arglist>(double M_kg, double r_m)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_undef</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aaae33249717912663bbb00be4fa4e6c3</anchor>
      <arglist>(double jd_tdb, enum novas_accuracy accuracy, const double *pos_app, const double *pos_obs, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>grav_undo_planets</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>ae68d01655c4b37b5a3e1422f0062f94c</anchor>
      <arglist>(const double *pos_app, const double *pos_obs, const novas_planet_bundle *planets, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>hor_to_itrs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>ad290005b13558de3927e455775b32e0f</anchor>
      <arglist>(const on_surface *location, double az, double za, double *itrs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_cirs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a12953f66c0f774284f0700ae785784d0</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_hor</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a296fcf0239f66eb19732f5072fed6d6e</anchor>
      <arglist>(const on_surface *location, const double *itrs, double *az, double *za)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>itrs_to_tod</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a3f6ae564cfac3766a21e0754905e5f7c</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>j2000_to_gcrs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a9d51ca5c972013024755c644c96d0586</anchor>
      <arglist>(const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_airborne_observer</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aa423a88cd3f434642fc89d47390184f4</anchor>
      <arglist>(const on_surface *location, const double *vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_cat_object</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>ab9b7a622486f2904615ceed1ba94cd8f</anchor>
      <arglist>(const cat_entry *star, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_ephem_object</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aa22303f98df2d63edf6d8452ff3a43b2</anchor>
      <arglist>(const char *name, long num, object *body)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_orbital_object</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a3c5e215babeeba078ca7b3e9be42e555</anchor>
      <arglist>(const char *name, long num, const novas_orbital *orbit, object *body)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_cat_entry</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a11aa3ce6bcdda5b7065b38d6cfaf63d3</anchor>
      <arglist>(const char *name, double ra, double dec, double z, cat_entry *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_redshifted_object</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>ae579a49d84696a913fd0d9f348bb5b11</anchor>
      <arglist>(const char *name, double ra, double dec, double z, object *source)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>make_solar_system_observer</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a9c3f1a1bc3fd9ac6d211806913d2d529</anchor>
      <arglist>(const double *sc_pos, const double *sc_vel, observer *obs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_dms_degrees</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>abf05d010ebf128f6300d4cc378616d90</anchor>
      <arglist>(const char *dms)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_e2h_offset</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>afd4387a6a7b3b5a8f6404ce27117adb3</anchor>
      <arglist>(double dra, double ddec, double pa, double *daz, double *del)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_epa</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a095ba0ee644dc00c11dcdc5616bc96b0</anchor>
      <arglist>(double ha, double dec, double lat)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_equ_sep</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a4f7e06b2fa5b3145b37aec4f9c2d1602</anchor>
      <arglist>(double ra1, double dec1, double ra2, double dec2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_h2e_offset</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aacd8ca48294bf635e5adc60084db0f26</anchor>
      <arglist>(double daz, double del, double pa, double *dra, double *ddec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_helio_dist</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a13b1204e7ad069e62b0cb7024fbfee0d</anchor>
      <arglist>(double jd_tdb, const object *source, double *rate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_hms_hours</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a5f564fbf4c22367d7eed6a060a99cc37</anchor>
      <arglist>(const char *hms)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_hpa</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a73fcb54e47b2be0aa92810b5f55f7a3c</anchor>
      <arglist>(double az, double el, double lat)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_lsr_to_ssb_vel</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aad80bf94be2efc36c340994829df8207</anchor>
      <arglist>(double epoch, double ra, double dec, double vLSR)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_dms</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aa9828ebb241e8a6500cac46f7e1e0e0c</anchor>
      <arglist>(const char *dms, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_hms</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>af9fc57f5df90c5beceb0b5c9c4fc15d6</anchor>
      <arglist>(const char *hms, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_planet</type>
      <name>novas_planet_for_name</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a92e869b101bd41e31ac6d3a2419bb0a3</anchor>
      <arglist>(const char *name)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_sep</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a9f33de2b26a19a5e20827e9e918b3375</anchor>
      <arglist>(double lon1, double lat1, double lon2, double lat2)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_orbsys_pole</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aee0ed43252017745574fa1e4b84a7b11</anchor>
      <arglist>(enum novas_reference_system type, double ra, double dec, novas_orbital_system *sys)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_solar_power</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>ad03d69e61d1a2d5da8063fcd0c05ab7c</anchor>
      <arglist>(double jd_tdb, const object *source)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_ssb_to_lsr_vel</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a99ed25baa029f3980924da8c8af54d1a</anchor>
      <arglist>(double epoch, double ra, double dec, double vLSR)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_v2z</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aa54cd00024b294d38fa29433c017c65f</anchor>
      <arglist>(double vel)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_xyz_to_uvw</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a87867e8e7fc7decaa44777a696465353</anchor>
      <arglist>(const double *xyz, double ha, double dec, double *uvw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z_add</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a493ab751eee5804db77157c37e94e5c3</anchor>
      <arglist>(double z1, double z2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_z_inv</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a1130ad2ce520e948c71f03a25a713075</anchor>
      <arglist>(double z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_cirs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aebad3c9b51f62cb7b31a686b5ec890a9</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_gcrs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>acbcd90c9111cc5580a5c2bac404f7f00</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_icrs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a4ef1b6d204e22608e964e60cf34a15ad</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_j2000</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aa3da1221ec52e427f9571d12f6f914ae</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_mod</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a3f055f3994889e3345d5267dac671a60</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>place_tod</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a2c8817db8a00764da4c285b32590ccfb</anchor>
      <arglist>(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>redshift_vrad</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>af56088995d42a0673e03bca1f3891544</anchor>
      <arglist>(double vrad, double z)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_cirs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>adb48b2acaf76cc3a1e3a1412cc42c232</anchor>
      <arglist>(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>tod_to_itrs</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>a4551e2ac083e34c1b2b61b3805efb9a3</anchor>
      <arglist>(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in, double *out)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>unredshift_vrad</name>
      <anchorfile>super_8c.html</anchorfile>
      <anchor>aea89203030ca831260dec18f83081d23</anchor>
      <arglist>(double vrad, double z)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>timescale.c</name>
    <path>src/</path>
    <filename>timescale_8c.html</filename>
    <includes id="novas_8h" name="novas.h" local="yes" import="no" module="no" objc="no">novas.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>_DEFAULT_SOURCE</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a8fb447618db946a9e2a596d9ea18763f</anchor>
      <arglist></arglist>
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
      <anchor>ab4ea1b646b23a76936b2a6459b1bac6d</anchor>
      <arglist>(const novas_timespec *time, enum novas_timescale timescale, long *ijd)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_get_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>aa65f86f70b2ea6e77bf0799d6859ca05</anchor>
      <arglist>(const novas_timespec *time, enum novas_timescale timescale)</arglist>
    </member>
    <member kind="function">
      <type>time_t</type>
      <name>novas_get_unix_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>aa29034a9e6579d5982083c5c6ea8d773</anchor>
      <arglist>(const novas_timespec *time, long *nanos)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_iso_timestamp</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a1b7e1fa279dc7502805cc76af2ae07fe</anchor>
      <arglist>(const novas_timespec *time, char *dst, int maxlen)</arglist>
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
      <anchor>a48866c233886213c4d347efb5ce30dd1</anchor>
      <arglist>(const char *date, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>novas_parse_date_format</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ab7b63f4a8cb2c50d4cbc972d2ffc4122</anchor>
      <arglist>(enum novas_calendar_type calendar, enum novas_date_format format, const char *date, char **tail)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_print_timescale</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a18bd698f5c504b5f0a581227e1649bd2</anchor>
      <arglist>(enum novas_timescale scale, char *buf)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_split_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a37c8f2d5e1ad2daed78d1bd7991fdba4</anchor>
      <arglist>(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1, novas_timespec *time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>ac272a018a213965f07b21ac6facb4a62</anchor>
      <arglist>(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *time)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_set_unix_time</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>abacd89aa380c85da2d3c19a7e2187df3</anchor>
      <arglist>(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *time)</arglist>
    </member>
    <member kind="function">
      <type>enum novas_timescale</type>
      <name>novas_timescale_for_string</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a5e447ec1ea688fe3fb688387ac669f7a</anchor>
      <arglist>(const char *str)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>novas_timestamp</name>
      <anchorfile>timescale_8c.html</anchorfile>
      <anchor>a9bc66e41d10e6a6b88cd6a352ab9da17</anchor>
      <arglist>(const novas_timespec *time, enum novas_timescale scale, char *dst, int maxlen)</arglist>
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
      <type>novas_matrix</type>
      <name>gcrs_to_cirs</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ab0781cc6d1fa84411eeca631bb39d615</anchor>
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
      <type>novas_matrix</type>
      <name>icrs_to_j2000</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>aae2b172beac21d0c2a5f256bc8ad40bc</anchor>
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
      <type>novas_matrix</type>
      <name>nutation</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>ac143b7ebb821de32278d5d4f3a4e75c3</anchor>
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
      <type>observer</type>
      <name>observer</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>aa792216b0b2601bb9af3b882c87274f8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>novas_planet_bundle</type>
      <name>planets</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a78ace4b8a99aace5313f4011a842d199</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>novas_matrix</type>
      <name>precession</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a1768e94e9ee04287bc18b3eb6a79f36c</anchor>
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
      <type>novas_timespec</type>
      <name>time</name>
      <anchorfile>structnovas__frame.html</anchorfile>
      <anchor>a430880330b471a12c587f77d1c73b49a</anchor>
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
      <type>novas_orbital_system</type>
      <name>system</name>
      <anchorfile>structnovas__orbital.html</anchorfile>
      <anchor>a2d7f51b64b1f15b7c85fdf1130d3ea87</anchor>
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
      <type>novas_observable</type>
      <name>accel</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>adb92e99ba304d8cba19dbe7f8dee77f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>novas_observable</type>
      <name>pos</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>a33e0ba55c500f6f927c7fb21c1710972</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>novas_observable</type>
      <name>rate</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>ab5aa5e48c9fcb9f6bd1031fe9feb80b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>novas_timespec</type>
      <name>time</name>
      <anchorfile>structnovas__track.html</anchorfile>
      <anchor>a430880330b471a12c587f77d1c73b49a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>novas_transform</name>
    <filename>structnovas__transform.html</filename>
    <member kind="variable">
      <type>novas_frame</type>
      <name>frame</name>
      <anchorfile>structnovas__transform.html</anchorfile>
      <anchor>aa4ab682491bbdc54f9fbf6a26c8245a1</anchor>
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
      <type>novas_matrix</type>
      <name>matrix</name>
      <anchorfile>structnovas__transform.html</anchorfile>
      <anchor>a7a4a2e27593c1af2ae14cfb323ba4aba</anchor>
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
      <type>novas_orbital</type>
      <name>orbit</name>
      <anchorfile>structobject.html</anchorfile>
      <anchor>a92255a8fae4e61f344eaf20e071bcb7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>cat_entry</type>
      <name>star</name>
      <anchorfile>structobject.html</anchorfile>
      <anchor>a0ead8b30742e9c76887f5ff76cabae7c</anchor>
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
      <type>in_space</type>
      <name>near_earth</name>
      <anchorfile>structobserver.html</anchorfile>
      <anchor>a447c33455c0da962abf8621638c994b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>on_surface</type>
      <name>on_surf</name>
      <anchorfile>structobserver.html</anchorfile>
      <anchor>a0ed422bd2a08c6b3870b545e16484501</anchor>
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
    <docanchor file="index.html">examples</docanchor>
    <docanchor file="index.html">methodologies</docanchor>
    <docanchor file="index.html">sidereal-example</docanchor>
    <docanchor file="index.html">solsys-example</docanchor>
    <docanchor file="index.html">tips</docanchor>
    <docanchor file="index.html">accuracy-notes</docanchor>
    <docanchor file="index.html">multi-threading</docanchor>
    <docanchor file="index.html">physical-units</docanchor>
    <docanchor file="index.html">precision</docanchor>
    <docanchor file="index.html">benchmarks</docanchor>
    <docanchor file="index.html">supernovas-features</docanchor>
    <docanchor file="index.html">added-functionality</docanchor>
    <docanchor file="index.html">debug-mode</docanchor>
    <docanchor file="index.html">api-changes</docanchor>
    <docanchor file="index.html">solarsystem</docanchor>
    <docanchor file="index.html">calceph-integration</docanchor>
    <docanchor file="index.html">cspice-integration</docanchor>
    <docanchor file="index.html">universal-ephemerides</docanchor>
    <docanchor file="index.html">builtin-ephem-readers</docanchor>
    <docanchor file="index.html">explicit-ephem-linking</docanchor>
    <docanchor file="index.html">debug-support</docanchor>
    <docanchor file="index.html">release-schedule</docanchor>
  </compound>
</tagfile>
