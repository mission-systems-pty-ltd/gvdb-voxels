# Copyright 2017 NVIDIA Corporation
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.10) # Use Modern CMake

# Here's what the dependency graph of this project looks like:
#      .---gvdbCopy--. (copies files)
#      |             |
#      V             V
#     gvdb        gvdbPTX
# (builds lib)  (builds PTX)

project(gvdb LANGUAGES CUDA CXX C)

cmake_policy(SET CMP0072 NEW) # Prefer GLVND by default when available (CMake 3.11+)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(CUDAToolkit REQUIRED)
# Finds GLEW
# include("cmake/FindGLEW.cmake")
find_package(GLEW REQUIRED)

# Print warning if not a 64-bit build
if(NOT (CMAKE_SIZEOF_VOID_P EQUAL 8))
    message(WARNING "gvdb_library: This project requires a 64-bit build. Subsequent configuration steps may fail.")
endif()

# Set the global minimum C++ standard for the project (this is C++11, since we use nullptr)
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Set C++ standard required for CUDA
if(NOT DEFINED CMAKE_CUDA_STANDARD)
    set(CMAKE_CUDA_STANDARD 11)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
endif()

# Compile a couple of source files into a PTX object library.
# Creates an object library target named gvdbPTX.
set(GVDB_PTX_SOURCE_FILES "kernels/cuda_gvdb_copydata.cu" "kernels/cuda_gvdb_module.cu"
"${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_dda.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_geom.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_nodes.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_operators.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_particles.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_raycast.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_scene.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_math.cuh"
)
add_library(gvdbPTX OBJECT ${GVDB_PTX_SOURCE_FILES})
# Make sure it compiles to a PTX file.
set_target_properties(gvdbPTX PROPERTIES CUDA_PTX_COMPILATION ON)
# set_target_properties(gvdbPTX PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

# Private and public sources are compiled into the target.
# Public and interface sources are published into the list of sources for
# consuming targets
# Another way of thinking about this is that
# public = used for target + used from targets.
# target_sources(gvdbPTX
#     PUBLIC  "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_dda.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_geom.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_nodes.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_operators.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_particles.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_raycast.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_scene.cuh"
#             "${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_math.cuh")

# Enable the --use_fast_math CUDA compiler flag.
target_compile_options(gvdbPTX
    PRIVATE --use_fast_math)

# Export our additional target include directories.
target_include_directories(gvdbPTX
    INTERFACE   $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/kernels>
                $<INSTALL_INTERFACE:include>) # expands to _output/include - note that these must be un-quoted!


# Create our main GVDB shared library (will output to libgvdb.so on Unix and gvdb.dll on Windows)
# add_library requires at least one file to be listed in CMake 3.10:
file(WRITE "${CMAKE_CURRENT_LIST_DIR}/dummy.cpp" "")

# add_library(gvdb SHARED "${CMAKE_CURRENT_LIST_DIR}/dummy.cpp")


include_directories(${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

add_library(gvdb SHARED 
            src/app_perf.cpp
            src/gvdb_allocator.cpp
            src/gvdb_camera.cpp
            src/gvdb_cutils.cu
            src/gvdb_model.cpp
            src/gvdb_node.cpp
            src/gvdb_render_opengl.cpp
            src/gvdb_scene.cpp
            src/gvdb_types.cpp
            src/gvdb_vec.cpp
            src/gvdb_volume_3D.cpp
            src/gvdb_volume_base.cpp
            src/gvdb_volume_gvdb.cpp
            src/loader_ObjarReader.cpp
            src/loader_OBJReader.cpp
            src/loader_Parser.cpp
            src/string_helper.cpp
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/app_perf.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_allocator.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_camera.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_cutils.cuh"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_model.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_node.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_render.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_scene.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_types.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_vec.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_3D.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_base.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_gvdb.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/loader_ObjarReader.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/loader_OBJReader.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/loader_Parser.h"
            "${CMAKE_CURRENT_LIST_DIR}/src/string_helper.h" 
) 
set_target_properties(gvdb PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(gvdb PROPERTIES CUDA_RUNTIME_LIBRARY Shared)

message("OPENGL_LIBRARIES = ${OPENGL_LIBRARIES}")
message("X11_LIBRARIES = ${X11_LIBRARIES}")
target_link_libraries(gvdb  PUBLIC
    # CUDA::cudart 
    ${X11_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES} # Link GLEW library
    # /usr/local/cuda/lib64/libcudart.so
)
# include cuda headers
message("CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES = ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")
target_include_directories(gvdb PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
target_compile_definitions(gvdb PRIVATE BUILD_OPENGL GVDB_EXPORTS)
message("BUILD_OPENGL = ${BUILD_OPENGL}")

# # Allow GVDB to use e.g. CUDA_GVDB_COPYDATA as a macro for "cuda_gvdb_copydata.ptx"
set(GVDB_PTX_INSTALL_PREFIX_MACRO "${CMAKE_INSTALL_PREFIX}/lib/CMakeFiles/gvdbPTX.dir/kernels")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
message("GVDB_PTX_INSTALL_PREFIX_MACRO = ${GVDB_PTX_INSTALL_PREFIX_MACRO}")
target_compile_definitions(gvdb PUBLIC CUDA_GVDB_COPYDATA_PTX="${GVDB_PTX_INSTALL_PREFIX_MACRO}/cuda_gvdb_copydata.ptx")
target_compile_definitions(gvdb PUBLIC CUDA_GVDB_MODULE_PTX="${GVDB_PTX_INSTALL_PREFIX_MACRO}/cuda_gvdb_module.ptx")

# Compute binary, include, and library install paths.
get_filename_component(BIN_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/bin REALPATH)
get_filename_component(INCLUDE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/include REALPATH)
get_filename_component(LIB_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/lib REALPATH)

# libraries etc.
install(DIRECTORY "$<TARGET_FILE_DIR:gvdb>/" DESTINATION ${LIB_INSTALL_PATH} FILES_MATCHING PATTERN "*.dll" PATTERN "*.glsl" PATTERN "*.ptx" PATTERN "*.so")
install(DIRECTORY "$<TARGET_FILE_DIR:gvdb>/" DESTINATION ${LIB_INSTALL_PATH} FILES_MATCHING PATTERN "*.lib" PATTERN "*.exp" PATTERN "*.ilk" PATTERN "*.pdb")

install(TARGETS gvdb DESTINATION ${LIB_INSTALL_PATH})
# glob .h files for gvdb
file(GLOB GVDB_HEADERS "${CMAKE_CURRENT_LIST_DIR}/src/*.h")
install(FILES ${GVDB_HEADERS} DESTINATION ${INCLUDE_INSTALL_PATH})
# Create a CMake config file for gvdb
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/gvdbConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/gvdbConfig.cmake"
    INSTALL_DESTINATION ${LIB_INSTALL_PATH}/cmake/gvdb
    PATH_VARS INCLUDE_INSTALL_PATH LIB_INSTALL_PATH
)
set(PROJECT_VERSION 0.1.0)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/gvdbConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/gvdbConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cmake/gvdbConfigVersion.cmake" DESTINATION ${LIB_INSTALL_PATH}/cmake/gvdb)


install(TARGETS gvdbPTX DESTINATION ${GVDB_PTX_INSTALL_PREFIX})
