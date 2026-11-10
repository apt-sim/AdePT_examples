# SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
# SPDX-License-Identifier: MIT

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(ATLLARBARREL)

#----------------------------------------------------------------------------
# Project CMake modules (FindEXPAT wrapper without version requirement)
#
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Option to enable G4HepEm usage
#
option(WITH_G4HepEm "Build for G4HepEm usage" OFF)

#----------------------------------------------------------------------------
# Option to enable G4HepEm specialized trasnportation usage
#
option(WITH_G4HepEmTracking "Build for G4HepEm usage with tracking" OFF)

if(WITH_G4HepEm)
    find_package(G4HepEm REQUIRED)
    add_compile_definitions(USE_G4HepEm)
    message("Project CMAKE option: building with USE_G4HepEm")
    # Recent G4HepEm versions removed G4HepEmProcess and only provide
    # the G4HepEmTrackingManager: switch to tracking if needed
    if(EXISTS ${G4HepEm_INCLUDE_DIR}/G4HepEmProcess.hh)
        set(G4HepEm_HAS_PROCESS ON)
    else()
        set(G4HepEm_HAS_PROCESS OFF)
        if(NOT WITH_G4HepEmTracking)
            message("Project CMAKE option: G4HepEmProcess not available in this G4HepEm, switching to WITH_G4HepEmTracking")
            set(WITH_G4HepEmTracking ON)
        endif()
    endif()
    add_subdirectory(hepemlib)
endif()

if(WITH_G4HepEmTracking)
    add_compile_definitions(USE_G4HepEmTracking)
    message("Project CMAKE option: building with USE_G4HepEm with tracking")
endif()

#----------------------------------------------------------------------------
# Option to enable AdePT (GPU transport of e-, e+ and gamma)
# The EM physics of the reference physics list is replaced by
# G4EmStandardPhysics_AdePT, which already includes G4HepEm
#
option(WITH_ADEPT "Build with AdePT GPU transport of e-, e+ and gamma" OFF)
if(WITH_ADEPT)
    if(WITH_G4HepEm)
        message(FATAL_ERROR "WITH_ADEPT and WITH_G4HepEm cannot be used together: AdePT already uses G4HepEm")
    endif()
    if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        set(CMAKE_CUDA_ARCHITECTURES native)
    endif()
    enable_language(CUDA)
    find_package(AdePT REQUIRED)
    add_compile_definitions(USE_ADEPT)
    message("Project CMAKE option: building with USE_ADEPT (CUDA architectures: ${CMAKE_CUDA_ARCHITECTURES})")
endif()

#----------------------------------------------------------------------------
# Option to use G4GeneralParticleSource (/gps/ commands) instead of
# G4ParticleGun (/gun/ commands) as primary generator
#
option(WITH_GPS "Build with G4GeneralParticleSource as primary generator" OFF)
if(WITH_GPS)
    add_compile_definitions(USEGPS)
    message("Project CMAKE option: building with USEGPS (G4GeneralParticleSource)")
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(ATLLArBarrel ATLLArBarrel.cc ${sources} ${headers})
if(WITH_G4HepEm)
    target_link_libraries(ATLLArBarrel ${Geant4_LIBRARIES} ${G4HepEm_LIBRARIES} hepemlib)
elseif(WITH_ADEPT)
    # cuda_rdc_* (from VecGeom) keeps the CUDA device linking consistent with AdePT
    cuda_rdc_target_link_libraries(ATLLArBarrel ${Geant4_LIBRARIES} ${AdePT_LIBRARIES})
else()
    target_link_libraries(ATLLArBarrel ${Geant4_LIBRARIES} ${G4HepEm_LIBRARIES})
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(ATLLArBarrel_SCRIPT
  gui.mac
  init.mac
  init_vis.mac
  vis.mac
  icons.mac
  run.mac
  single.mac
  )

foreach(_script ${ATLLArBarrel_SCRIPT})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Add program to the project targets
# (this avoids the need of typing the program name after make)
#
add_custom_target(ATLASBARREL DEPENDS ATLLArBarrel)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS ATLLArBarrel DESTINATION bin)
