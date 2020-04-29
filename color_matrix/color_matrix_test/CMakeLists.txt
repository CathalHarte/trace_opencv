cmake_minimum_required(VERSION 3.12.0)
project( color_matrix_test )

set(REPO_ROOT "../..")

# Where to find other source files
add_subdirectory( .. color_matrix )
add_subdirectory( ${REPO_ROOT}/unit_test_helpers/cv_helpers cv_helpers )

# The target
add_executable( color_matrix_test color_matrix_test.cpp )

target_include_directories( color_matrix_test PRIVATE   
    ..
    ${REPO_ROOT}/unit_test_helpers                                                        
    ${REPO_ROOT}/unit_test_helpers/cv_helpers )

target_link_libraries( color_matrix_test color_matrix )
target_link_libraries( color_matrix_test libgtest.so libgtest_main.so libpthread.so )
target_link_libraries( color_matrix_test cv_helpers )
