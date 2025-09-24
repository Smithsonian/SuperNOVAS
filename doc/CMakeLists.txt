# Additional CMake configuration file for building the documentation of the 
# SuperNOVAS library. The examples are also added to the test suite so they 
# may be checked for functionality.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_DOC=ON option.
#
# Author: Attila Kovacs

# ----------------------------------------------------------------------------
# Generate derivative documentation

# Generate headless README
add_executable(docedit EXCLUDE_FROM_ALL src/docedit.c)

add_custom_target(headless_readme ALL
    COMMAND docedit ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install Markdown documentation (Development)
file(GLOB MD_FILES doc/*.md)
install(FILES ${MD_FILES}
    COMPONENT Development
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

find_package(Doxygen)

set_package_properties(Doxygen PROPERTIES
    URL "https://doxygen.nl/"
    DESCRIPTION  "is a widely-used documentation generator tool in software development"
    TYPE OPTIONAL
    PURPOSE "Enables HTML documentation for supernovas"
)

if(Doxygen_FOUND)
    # Generate headless docs and run Doxygen on them.
    doxygen_add_docs(project_docs ALL CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    add_dependencies(project_docs headless_readme)
    
     # For html/ dir, list just the directory, not the individual files installed.
    install(CODE "message(STATUS \"Installing: <prefix>/${CMAKE_INSTALL_DOCDIR}/html/*\")")
    install(DIRECTORY html
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        MESSAGE_NEVER
        COMPONENT Development
    )

    # Install Doxygen tag, which downstream docs can link to.
    install(FILES supernovas.tag
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT Development
    )
else()
    message(WARNING "Doxygen not found -- Will skip generating HTML documentation")
endif()
    
    

   
