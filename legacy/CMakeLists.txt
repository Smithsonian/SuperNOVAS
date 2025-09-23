# Additional CMake configuration file for building the legacy programs of the 
# supernovas library.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_LEGACY=ON option.
#
# Author: Attila Kovacs


# Link against binaries in the local supernovas lib/ directory
link_directories(${CMAKE_BINARY_DIR}/lib)

# Build tools
add_executable(cio_file cio_file.c)
add_executable(cio_filegen EXCLUDE_FROM_ALL cio_filegen.c)

# Link against the supernovas library
target_link_libraries(cio_filegen PRIVATE
    supernovas
    $<$<BOOL:${HAVE_LIBM}>:m>
)

add_custom_target(generate-cio-data ALL
    COMMAND cio_filegen ${CMAKE_CURRENT_BINARY_DIR}/CIO_RA.TXT
)

# Install to bindir
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CIO_RA.TXT DESTINATION ${CMAKE_INSTALL_DATADIR})
