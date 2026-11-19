#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(ATLLARBARREL)

#----------------------------------------------------------------------------
# When built as part of AdePT_examples (add_subdirectory from examples/)
# AdePT transport is always enabled and the G4HepEm-only builds are off
#
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(WITH_ADEPT ON)
    set(WITH_G4HepEm OFF)
    set(WITH_G4HepEmTracking OFF)
    set(WITH_G4HepEmEM OFF)
    set(WITH_G4HepEmEM_MSC_MULTISTEPS OFF)
    message("Project CMAKE option: built inside AdePT_examples, forcing WITH_ADEPT=ON")
endif()

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

#----------------------------------------------------------------------------
# Option to keep the reference physics list (-p) and replace only its EM
# constructor with G4EmStandardPhysics + G4HepEmTrackingManager
#
option(WITH_G4HepEmEM "With WITH_G4HepEm: reference physics list with G4HepEm EM tracking" OFF)
if(WITH_G4HepEmEM)
    if(NOT WITH_G4HepEm)
        message(FATAL_ERROR "WITH_G4HepEmEM requires WITH_G4HepEm=ON")
    endif()
    add_compile_definitions(USE_G4HepEmEM)
    message("Project CMAKE option: building with USE_G4HepEmEM (reference list + G4HepEm EM tracking)")
endif()

# With WITH_G4HepEmEM: multiple MSC steps per transportation step (G4HepEm
# default). OFF by default, because AdePT does not use them on the GPU
#
option(WITH_G4HepEmEM_MSC_MULTISTEPS "With WITH_G4HepEmEM: G4HepEm multiple steps in MSC + transportation (G4HepEm default; AdePT uses OFF)" OFF)
if(WITH_G4HepEmEM_MSC_MULTISTEPS)
    if(NOT WITH_G4HepEmEM)
        message(FATAL_ERROR "WITH_G4HepEmEM_MSC_MULTISTEPS requires WITH_G4HepEmEM=ON")
    endif()
    add_compile_definitions(USE_G4HepEmEM_MSC_MULTISTEPS)
    message("Project CMAKE option: building with USE_G4HepEmEM_MSC_MULTISTEPS (G4HepEm default MSC stepping)")
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
    # AdePT requires CMake >= 3.25.2: stop here with a clear message rather than
    # inside AdePTConfig.cmake (a version check does not change the policies of the project)
    if(CMAKE_VERSION VERSION_LESS 3.25.2)
        message(FATAL_ERROR "WITH_ADEPT requires CMake >= 3.25.2 (found ${CMAKE_VERSION})")
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
# Option to build the original geometry, kept in legacy/src, instead of the
# one with the presampler cathode and thin-absorber overlap fixes
#
option(WITH_LEGACY_GEOMETRY "Build the original geometry (with the presampler and thin-absorber overlaps)" OFF)
if(WITH_LEGACY_GEOMETRY)
    list(FILTER sources EXCLUDE REGEX "/src/(LArBarrelConstruction|LArBarrelPresamplerConstruction)\\.cc$")
    file(GLOB legacy_sources ${PROJECT_SOURCE_DIR}/legacy/src/*.cc)
    list(APPEND sources ${legacy_sources})
    include_directories(${PROJECT_SOURCE_DIR}/src) # PresParameterDef.icc
    message("Project CMAKE option: building with the legacy geometry (legacy/src)")
endif()

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(ATLLArBarrel ATLLArBarrel.cc ${sources} ${headers})
if(WITH_G4HepEm)
    target_link_libraries(ATLLArBarrel ${Geant4_LIBRARIES} ${G4HepEm_LIBRARIES} hepemlib)
elseif(WITH_ADEPT)
    # AdePT-only sources (EM physics constructor), kept out of the src/ glob
    target_sources(ATLLArBarrel PRIVATE ${PROJECT_SOURCE_DIR}/adept/src/ATLLArBarrelEmPhysics_AdePT.cc)
    target_include_directories(ATLLArBarrel PRIVATE ${PROJECT_SOURCE_DIR}/adept/include)
    # the AdePT transport headers (included by AdePTTrackingManager.hh) use C++20 (std::span);
    # AdePT is built with C++20 but does not export the requirement
    target_compile_features(ATLLArBarrel PRIVATE cxx_std_20)
    # cuda_rdc_* (from VecGeom) keeps the CUDA device linking consistent with AdePT
    cuda_rdc_target_link_libraries(ATLLArBarrel PRIVATE ${Geant4_LIBRARIES} ${AdePT_LIBRARIES})
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
  run_adept.mac
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
