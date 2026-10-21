#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(ATLLARBARREL)

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
if(WITH_G4HepEm)
    find_package(G4HepEm REQUIRED)
    add_compile_definitions(USE_G4HepEm)
    message("Project CMAKE option: building with USE_G4HepEm")
    add_subdirectory(hepemlib)
endif()

#----------------------------------------------------------------------------
# Option to enable G4HepEm specialized trasnportation usage
#
option(WITH_G4HepEmTracking "Build for G4HepEm usage with tracking" OFF)
if(WITH_G4HepEmTracking)
    add_compile_definitions(USE_G4HepEmTracking)
    message("Project CMAKE option: building with USE_G4HepEm with tracking")
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
