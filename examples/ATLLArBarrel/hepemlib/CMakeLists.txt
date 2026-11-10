# SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
# SPDX-License-Identifier: MIT

add_library(hepemlib
    include/PhysicsList.hh
    include/PhysListHepEmTracking.hh
    src/PhysicsList.cc
    src/PhysListHepEmTracking.cc
)

# G4HepEmProcess is only available in older G4HepEm versions
if(G4HepEm_HAS_PROCESS)
    target_sources(hepemlib PRIVATE include/PhysListHepEm.hh src/PhysListHepEm.cc)
    target_compile_definitions(hepemlib PRIVATE G4HepEm_HAS_PROCESS)
endif()

target_include_directories(hepemlib PUBLIC include)
target_link_libraries(hepemlib ${Geant4_LIBRARIES} ${G4HepEm_LIBRARIES})
