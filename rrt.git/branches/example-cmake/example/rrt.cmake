# This file is a part of the rrt library at:
# https://github.com/robojackets/rrt
#
# Directions for use:
# To use the rrt library in another CMake project, do the following:
# * git submodule add git://github.com/robojackets/rrt
# Copy this CMake file into your project
# Open up the appropriate CMakeLists.txt file and add:
#   include(path/to/rrt.cmake)
# Then link with the appropriate target
#   target_link_libraries(my-awesome-project rrt_lib)

include(ExternalProject)
ExternalProject_Add(rrt_lib
    GIT_REPOSITORY ${PROJECT_SOURCE_DIR}/external/rrt
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
)
set_target_properties(rrt_lib PROPERTIES EXCLUDE_FROM_ALL TRUE)

# specify include dir
# ExternalProject_Get_Property(rrt_lib source_dir)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# specify link libraries
# ExternalProject_Get_Property(rrt_lib binary_dir)
set(rrt_LIBRARY
    ${CMAKE_CURRENT_BINARY_DIR}/lib/librrt.a
)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib)

add_custom_command(OUTPUT ${rrt_LIBRARY}
    DEPENDS rrt_lib
)
