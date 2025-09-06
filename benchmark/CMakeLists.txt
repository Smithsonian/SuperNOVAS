# List of example programs to build
set(BENCHMARK_PROGRAMS
    benchmark-nutation
    benchmark-place
)

link_directories(${CMAKE_BINARY_DIR}/lib)

# Build each example
foreach(BENCHMARK ${BENCHMARK_PROGRAMS})
    set(BENCHMARK_SOURCE ${BENCHMARK}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${BENCHMARK_SOURCE})
        add_executable(${BENCHMARK} ${BENCHMARK_SOURCE})

        # Link against the supernovas library
        target_link_libraries(${BENCHMARK} PRIVATE
            supernovas
            $<$<BOOL:${HAVE_LIBM}>:m>
        )

    else()
        message(WARNING "Source file ${BENCHMARK_SOURCE} not found - ${BENCHMARK} will not be built")
    endif()
endforeach()
