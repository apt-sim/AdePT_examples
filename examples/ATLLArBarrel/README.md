<!--
SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
SPDX-License-Identifier: CC-BY-4.0
-->

# ATLLArBarrel: ATLAS LAr barrel beam test with AdePT

**Location:** `examples/ATLLArBarrel/`  
**Category:** Full Application  
**AdePT version tested:** 0.4.2  
**Status:** experimental

---

## 1. Purpose

A Geant4 simulation of the ATLAS LAr barrel beam test setup, in which electrons, positrons and gammas
are transported on the GPU with AdePT. The accordion geometry, with thousands of volumes, and the
sensitive detector are the unchanged Geant4 application. It shows how a realistic sampling calorimeter
is integrated with AdePT and how to validate it against Geant4 on the CPU.

The description of the original application is in [`PROJECT_README.md`](PROJECT_README.md).

---

## 2. Features Demonstrated

- Replacing the EM constructor of a reference physics list with `G4EmStandardPhysics_AdePT`
- Automatic conversion of the Geant4 geometry to VecGeom
- GPU steps returned to the CPU and processed by an unchanged sensitive detector
- `/adept/` configuration through macros
- Comparison with FTFP_BERT on the CPU and with G4HepEm

---

## 3. Prerequisites

- AdePT installed and discoverable via `CMAKE_PREFIX_PATH`
- Geant4 built with multithreading and GDML (tested with 11.4.3)
- CUDA toolkit (version required by AdePT; tested with 13.1)
- Dependencies found through AdePT (VecGeom, VecCore, G4HepEm, G4VG)

---

## 4. Build Instructions

From the top of this repository, setting the architecture of your GPU (`75` for an RTX 2070 SUPER):

```bash
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH="<AdePT installation prefix>;<G4HepEm installation prefix>" \
  -DCMAKE_CUDA_ARCHITECTURES=75
cmake --build build --target ATLLArBarrel -- -j
```

The example is built with `WITH_ADEPT=ON` in this repository. See [`BUILD_OPTIONS.md`](BUILD_OPTIONS.md) for
the CPU and G4HepEm configurations of the standalone project.

---

## 5. How to Run

From `build/examples/ATLLArBarrel/`:

```bash
./ATLLArBarrel -m <repository>/examples/ATLLArBarrel/runcards/benchmarks/electron50GeV_adept.mac -t 8
```

- `-m` is the macro, `-t` the number of threads, and `-p` the reference physics list.
- `runcards/benchmarks/gamma5GeV_adept.mac` is the 5 GeV gamma benchmark, to be run with `-t 1`.
- The GPU is used efficiently only with enough tracks in flight: use several threads and/or high-energy showers.

---

## 6. Expected Output

The run ends with the Geant4 summary, for instance:

```yaml
Run terminated, 150 events transported
```

The ntuple `ATLLArBarrelout_Run0.root` has one row per event. The main columns are `EnergyLAr` (Birks-corrected,
with electronic noise), `EnergyLArNoNoise`, `EnergyLArNoBirks` and `EventID`. Energies are in MeV.

---

## 7. Validation or Checks

Reference results and figures are in [`Results.md`](Results.md). With 50 GeV electrons and the benchmark macros, the
mean of `EnergyLArNoNoise` for 150 events is 9920.7 ± 13.2 MeV with AdePT and 9913.8 ± 9.9 MeV for FTFP_BERT on the
CPU. With 1000 events and without events containing a hard photonuclear interaction, AdePT is +0.15 % ± 0.06 % above
FTFP_BERT, the same as G4HepEm on the CPU.

---

## 8. File structure

```objectivec
examples/ATLLArBarrel/
  README.md
  PROJECT_README.md    # description of the original application
  BUILD_OPTIONS.md     # CPU, G4HepEm and AdePT configurations
  Results.md           # performance and validation results
  CMakeLists.txt
  ATLLArBarrel.cc
  src/  include/       # application
  runcards/benchmarks/ # benchmark macros
  hepemlib/            # physics lists for the G4HepEm configuration
  analysis/  scripts/  # analysis and helper scripts
  GDMLs/               # dump of the geometry, not read at run time
  images/
```

---

## 9. Notes / Troubleshooting

- **Set `/run/setCut` before `/run/initialize`.** If it comes after, the requested production cut is not applied on
  the GPU, and the energy in the LAr is about 1.4 % higher than on the CPU.
- AdePT switches energy-loss fluctuations off by default. The benchmark macros set
  `/adept/SetEnergyLossFluctuation true` before `/run/initialize` to match the Geant4 physics lists.
- **Do not identify primaries with `GetParentID()==0`.** Unless the host keeps the track data
  (`/adept/returnFirstAndLastStep true` or `/adept/returnAllSteps true`), hadrons created by nuclear reactions on GPU
  steps also have ParentID 0. This example tests `GetDynamicParticle()->GetPrimaryParticle()` instead.
- With 1 thread and 5 GeV gammas AdePT is slower than the CPU, as the GPU is underused.

---

## 10. License

The code of this example comes from the ATLLArBarrel project and is licensed under the MIT license
(see [`LICENSE`](LICENSE)), as stated in the SPDX header of each file. This document is licensed under CC-BY-4.0.
