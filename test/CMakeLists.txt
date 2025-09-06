
# List of example programs to build
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

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${CMAKE_BINARY_DIR}/lib)

add_executable(cio_file ${PROJECT_SOURCE_DIR}/src/cio_file.c)

add_custom_target(cio_bin_data
    DEPENDS cio_file
    COMMAND cio_file ${PROJECT_SOURCE_DIR}/data/CIO_RA.TXT ${PROJECT_SOURCE_DIR}/data/cio_ra.bin
)

set(EPHEM_DIR ${PROJECT_SOURCE_DIR}/test/ephem)
set(CIO_DIR ${PROJECT_SOURCE_DIR}/data)

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
        
        message(STATUS "Added test: ${TEST}")
    else()
        message(WARNING "Source file ${TEST_SOURCE} not found - ${TEST} will not be built")
    endif()
endforeach()
