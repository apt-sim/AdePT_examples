# SPDX-FileCopyrightText: 2025 CERN for the benefit of the AdePT project
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.25)

project(AdePTExamples
  VERSION 0.1.0
  LANGUAGES CXX CUDA)

# Options for later when you start adding examples
option(ADEPT_EXAMPLES_BUILD_TESTS "Build tests for AdePT examples" ON)
option(ADEPT_EXAMPLES_ENABLE_CUDA "Enable CUDA builds (required for most AdePT examples)" ON)

# Find AdePT (and, transitively, VecGeom / VecCore / G4HepEm)
#
# You typically provide CMAKE_PREFIX_PATH so that CMake can locate AdePT’s config.
# e.g.:
#   cmake -S. -Bbuild \
#     -DCMAKE_PREFIX_PATH="<path_to_AdePT_installation>;<other_deps>"
#
find_package(AdePT REQUIRED)

message(STATUS "Found AdePT include dirs: ${AdePT_INCLUDE_DIRS}")
message(STATUS "Found AdePT libraries   : ${AdePT_LIBRARIES}")

# Later: add actual examples under examples/
add_subdirectory(examples)

# Optionally, you can expose a meta-target for all examples once they exist
# add_custom_target(adept_examples_all DEPENDS <example-targets-go-here>)
