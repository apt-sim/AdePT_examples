<!--
SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
SPDX-License-Identifier: MIT
-->

# ATLLArBarrel build options

This page describes the CMake options of ATLLArBarrel, how they combine, and what they
require at build time and at run time.

## Project options

| Option | Default | Effect | Requires |
| ------ | ------- | ------ | -------- |
| `WITH_GEANT4_UIVIS` | `ON` | Build with the Geant4 UI and visualization drivers (`ui_all vis_all`). Set to `OFF` for a batch-only executable. | Geant4 with UI/Vis drivers |
| `WITH_GPS` | `OFF` | Primary generator: `G4ParticleGun` (`/gun/` macro commands). With `ON`: `G4GeneralParticleSource` (`/gps/` macro commands). | – |
| `WITH_G4HepEm` | `OFF` | Use the physics list in `hepemlib/`: G4HepEm for e-, e+ and gamma only, no hadronic physics. | G4HepEm (`G4HepEm_DIR`) |
| `WITH_G4HepEmTracking` | `OFF` | With `WITH_G4HepEm`: use `G4HepEmTrackingManager` instead of `G4HepEmProcess`. Switched on automatically when the G4HepEm installation has no `G4HepEmProcess.hh` (recent G4HepEm versions). | `WITH_G4HepEm=ON` |
| `WITH_ADEPT` | `OFF` | Transport e-, e+ and gamma on the GPU with AdePT. The EM physics of the reference physics list (`-p`, default `FTFP_BERT`) is replaced by `G4EmStandardPhysics_AdePT`; hadronic physics stays on the CPU. | CUDA, AdePT, VecGeom and VecCore with CUDA, G4HepEm with CUDA |

### Combinations

| `WITH_G4HepEm` | `WITH_ADEPT` | Physics list | Particles on GPU |
| -------------- | ------------ | ------------ | ---------------- |
| `OFF` | `OFF` | Geant4 reference list (`-p`, default `FTFP_BERT`) | none |
| `ON` | `OFF` | `hepemlib` G4HepEm list (EM only) | none |
| `OFF` | `ON` | Reference list with `G4EmStandardPhysics_AdePT` | e-, e+, gamma |
| `ON` | `ON` | not allowed: CMake stops with an error, because AdePT already uses G4HepEm | – |

`WITH_GPS` and `WITH_GEANT4_UIVIS` can be combined with any of the above.

### Macros and `WITH_GPS`

| Macro | Commands | Needs |
| ----- | -------- | ----- |
| `runcards/run_gps.mac` | `/gps/` | `-DWITH_GPS=ON` |
| all the other macros with a primary generator | `/gun/` | `-DWITH_GPS=OFF` (default) |

A macro used with the wrong option stops the run with `COMMAND NOT FOUND`.

## Standard CMake variables

| Variable | Use |
| -------- | --- |
| `Geant4_DIR` | Path to `Geant4Config.cmake`, e.g. `<geant4-install>/lib64/cmake/Geant4` |
| `G4HepEm_DIR` | Path to `G4HepEmConfig.cmake` (`WITH_G4HepEm=ON`), e.g. `<g4hepem-install>/lib64/cmake/G4HepEm` |
| `CMAKE_PREFIX_PATH` | For `WITH_ADEPT=ON`: the installation prefixes of AdePT/VecGeom/VecCore and of G4HepEm, separated by `;` |
| `CMAKE_BUILD_TYPE` | `Release` is recommended for production and timing measurements |
| `CMAKE_CUDA_ARCHITECTURES` | For `WITH_ADEPT=ON`: compute capability of the GPU without the dot (e.g. `75` for an RTX 2070). Defaults to `native` (the GPU of the build machine). Use the same value as for VecGeom and AdePT. |
| `CMAKE_CUDA_COMPILER` | For `WITH_ADEPT=ON`, if `nvcc` is not in `PATH`, e.g. `/usr/local/cuda-13.1/bin/nvcc` |

## Build examples

CPU only, Geant4 reference physics list:
```sh
cmake -DGeant4_DIR=<geant4-install>/lib64/cmake/Geant4 -DCMAKE_BUILD_TYPE=Release \
      -DWITH_GPS=OFF ../ATLLArBarrel
```

G4HepEm (tracking manager):
```sh
cmake -DGeant4_DIR=<geant4-install>/lib64/cmake/Geant4 -DCMAKE_BUILD_TYPE=Release \
      -DG4HepEm_DIR=<g4hepem-install>/lib64/cmake/G4HepEm -DWITH_G4HepEm=ON \
      -DWITH_GPS=OFF ../ATLLArBarrel
```

AdePT:
```sh
cmake -DGeant4_DIR=<geant4-install>/lib64/cmake/Geant4 -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="<adept-install>;<g4hepem-install>" \
      -DCMAKE_CUDA_ARCHITECTURES=75 -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc \
      -DWITH_ADEPT=ON -DWITH_GPS=OFF ../ATLLArBarrel
```

Use a separate build directory for each configuration (e.g. `build/` and `build-adept/`).

## Running with AdePT

AdePT is configured with `/adept/` commands placed **before** `/run/initialize`. Without
`/adept/setTrackInAllRegions true` (or `/adept/addGPURegion <region>`), no particle is sent
to the GPU. Example, based on `run.mac`:

```
/adept/setVerbosity 0
/adept/setMillionsOfTrackSlots 1
/adept/setMillionsOfHitSlots 1
/adept/setCUDAStackLimit 8192
/adept/setTrackInAllRegions true
/adept/SetEnergyLossFluctuation true
/run/setCut 0.01 mm
/run/initialize
/gun/particle e-
/gun/energy 50 GeV
/gun/position 0. 0. 0. m
/gun/direction 0.980254 0.0120087 0.197375
/run/beamOn 100
```

Notes:
- The output shows `--> Using AdePT for e-, e+ and gamma`, then `AdePTTransport: initializing
  geometry and physics` and the GPU track slots allocated.
- The Geant4 geometry is converted to VecGeom automatically. The LAr sensitive detector
  (`LAr::EMB::STAC`) is unchanged: AdePT returns the GPU steps to the CPU and calls it.
- The GPU is used efficiently only with enough tracks in flight. Use several threads (`-t`)
  and/or high-energy showers: with 1 thread and 5 GeV gammas there is no gain over the CPU.
- AdePT does not support killing GPU tracks from CPU user code, or per-track state cached in
  shared members of user actions/sensitive detectors (see the AdePT documentation).
- AdePT switches energy-loss fluctuations off by default, while the Geant4 physics lists have them
  on. Set `/adept/SetEnergyLossFluctuation true` before `/run/initialize` to compare the same physics.
- Set `/run/setCut` **before** `/run/initialize`. If it comes after, the requested production cut is not applied on
  the GPU, and AdePT deposits about 1.4 % more energy in the LAr than the CPU.
- Validation status: with the cut before `/run/initialize`, AdePT agrees with the CPU to better than 0.2 % for
  50 GeV electrons and within the statistics for 5 GeV gamma. It has been compared only on these benchmarks. See
  [Results.md](Results.md), section 3.
- Benchmark macros with the AdePT settings are in `runcards/benchmarks/` (`*_adept.mac`).

## Other notes

- `cmake/FindEXPAT.cmake` drops the expat version requirement recorded by some Geant4
  installations (e.g. configured with Linuxbrew expat), so the system expat can be used.
  When building Geant4, `-DGEANT4_USE_SYSTEM_EXPAT=OFF` avoids the issue.
- With `WITH_G4HepEm=ON` and a G4HepEm built with CUDA (as needed by AdePT), the executable
  also depends on `libcudart`.
