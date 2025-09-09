# Additional CMake configuration file for building the regression testing programs
# of the supernovas library. The programs are added to the test suite.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_TESTING=ON option.
#
# Author: Attila Kovacs

# List of test programs to build
set(TEST_PROGRAMS
    test-compat
    test-super
    test-errors
)

if(ENABLE_CALCEPH)
    list(APPEND TEST_PROGRAMS test-calceph)
endif()

if(ENABLE_CSPICE)
    list(APPEND TEST_PROGRAMS test-cspice)
endif()

# Look for headers in the supernovas include/ directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Link against binaries in the local supernovas lib/ directory
link_directories(${CMAKE_BINARY_DIR}/lib)

# Directory where ephemeris data for the tests are found
set(EPHEM_DIR ${PROJECT_SOURCE_DIR}/test/ephem)

# Directory where the CIO locator data (ASCII and binary) are found
set(CIO_DIR ${PROJECT_SOURCE_DIR}/data)

# Create a data/ directory for the compat test output
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)

# We'll build and use the cio_file program just to use here..
add_executable(cio_file EXCLUDE_FROM_ALL ${PROJECT_SOURCE_DIR}/src/cio_file.c)

# Generate the binary CIO locator data for the tests
add_custom_target(cio_bin_data
    DEPENDS cio_file
    COMMAND cio_file ${PROJECT_SOURCE_DIR}/data/CIO_RA.TXT ${CIO_DIR}/cio_ra.bin
)

# Build each example
foreach(TEST ${TEST_PROGRAMS})
    set(TEST_SOURCE src/${TEST}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_SOURCE})
        add_executable(${TEST} ${TEST_SOURCE})
        add_dependencies(${TEST} cio_bin_data)
              
        target_include_directories(${TEST} PRIVATE
            ${PROJECT_SOURCE_DIR}/include
        )
        
        # Link against the supernovas library
        target_link_libraries(${TEST} PRIVATE
            supernovas
            $<$<BOOL:${HAVE_LIBM}>:m>
        )

        # Special handling for CALCEPH example
        if(${TEST} STREQUAL "test-calceph")
            target_link_libraries(${TEST} PRIVATE
                solsys-calceph
                calceph
            )
     	    add_test(NAME ${TEST} COMMAND ${TEST} ${EPHEM_DIR})
     	    
        # Special handling for CSPICE example
        elseif(${TEST} STREQUAL "test-cspice")
            target_link_libraries(${TEST} PRIVATE
                solsys-cspice
                cspice
            )
            add_test(NAME ${TEST} COMMAND ${TEST} ${EPHEM_DIR})
        
        # All other examples
     	else()
     	    add_test(NAME ${TEST} COMMAND ${TEST} ${CIO_DIR})
        endif()
        
    else()
        message(WARNING "Source file ${TEST_SOURCE} not found - ${TEST} will not be built")
    endif()
endforeach()

