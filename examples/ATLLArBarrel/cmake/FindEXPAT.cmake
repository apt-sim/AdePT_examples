# SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
# SPDX-License-Identifier: MIT

#----------------------------------------------------------------------------
# Wrapper around CMake's FindEXPAT that drops the version requirement.
# Geant4 may record a minimum EXPAT version taken from a non-system expat
# (e.g. Linuxbrew) at its own configure time; the system expat is enough.
#
unset(EXPAT_FIND_VERSION)
include(${CMAKE_ROOT}/Modules/FindEXPAT.cmake)
