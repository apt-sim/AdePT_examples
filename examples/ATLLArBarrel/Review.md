# ATLLArBarrel – Review

Independent review of ATLLArBarrel at commit `d91769b` (`master` of the original ATLLArBarrel
repository, imported here; 2026-11-10): code review, geometry checks, builds with Geant4 alone and with G4HepEm, speed tests, and the changes needed to run with
AdePT on a GPU. Review date: 2026-11-17.

Companion documents in this repository: [README.md](README.md), [BUILD_OPTIONS.md](BUILD_OPTIONS.md),
[Results.md](Results.md). Where this review repeats a measurement of `Results.md` on a different
machine, both numbers are given.

## Contents

1. [Summary](#1-summary)
2. [Environment](#2-environment)
3. [Code review](#3-code-review)
4. [Geometry checks](#4-geometry-checks)
5. [Builds](#5-builds)
6. [Speed tests: Geant4 vs G4HepEm](#6-speed-tests-geant4-vs-g4hepem)
7. [Running with AdePT on GPU: code and build changes](#7-running-with-adept-on-gpu-code-and-build-changes)
8. [How to reproduce](#8-how-to-reproduce)
9. [Appendix A: patch for the matched G4HepEm configuration](#appendix-a-patch-for-the-matched-g4hepem-configuration)

---

## 1. Summary

- **Builds.** The CPU-only (`FTFP_BERT`) and G4HepEm builds configure and compile cleanly on macOS
  (Apple Clang 17, Geant4 11.4.3, G4HepEm `20261106`), with no warnings at the default warning level.
  With `-Wall -Wextra -Wshadow` there are about 70 warnings, almost all unused variables in the
  geometry code inherited from ATLAS; none points to a bug (section 3.3).
- **Geometry.** The Geant4 overlap check confirms the one known overlap: the first presampler cathode
  `catho3` protrudes by **1.03 mm** from its module. At higher sampling density (2000 and 20000 points
  per volume) the check also finds **6–30 µm protrusions of five `LAr::EMB::ThinAbs::Straight` volumes**
  from `LAr::EMB::STAC`, at the high-z end of the accordion. They were not visible with the
  200 points used in `Results.md`. Both are explained in section 4. **Both are now fixed** (4.6); the
  original geometry is kept in `legacy/src` and can be built with `-DWITH_LEGACY_GEOMETRY=ON`.
- **Speed.** On a 6-core Intel i5-9600K, G4HepEm makes the simulation **1.40–1.51× faster** than
  Geant4 `FTFP_BERT`, for 5 GeV photons and 50 GeV electrons, with 1 and 6 threads. This agrees with
  the 1.44× of `Results.md` on a 24-core EL9 machine. Multithreaded scaling on 6 cores is about 5.2×
  for all configurations.
- **Physics of the G4HepEm comparison (main finding).** The G4HepEm option of the project
  (`-DWITH_G4HepEm=ON`) is an **EM-only physics list**: it has no photo-, electro- or positron-nuclear
  processes. At 50 GeV it deposits **+0.4 % more energy in the LAr than `FTFP_BERT` (3.1–3.6 σ)**, while
  a **matched configuration** (FTFP_BERT with only its EM constructor using the G4HepEm tracking
  manager, Appendix A) agrees with `FTFP_BERT` within 0.1 % and is just as fast. I recommend adding
  this matched configuration to the project as the reference for G4HepEm, and for comparisons with AdePT
  (added on 2026-11-19 as `-DWITH_G4HepEmEM=ON`).
- **AdePT.** The project already contains a working AdePT integration (`WITH_ADEPT`), validated on an
  NVIDIA RTX 2070 SUPER with AdePT 0.4.2 (`Results.md`, section 3). Checked against the AdePT
  sources (v0.4.2 and v0.4.3, released today), the integration follows the documented pattern. Section 7
  lists everything that is needed to run on a GPU, what is already in place, and the changes still
  recommended: CMake minimum version, guard against physics lists with a non-default EM option (since added
  to the project and tested on the GPU, finding 4), dependency versions required by AdePT 0.4.3, and a few smaller points. AdePT cannot run on this
  machine (no NVIDIA GPU, no CUDA on macOS), so these changes were checked against the sources but not run.
  The geometry fixes of section 4.6 were later checked with AdePT on the GPU machine: the LAr energy
  does not change within statistics (section 4.6). Against the matched CPU reference, now part of the project
  (`WITH_G4HepEmEM`, with G4HepEm's multiple MSC steps off as on the GPU), AdePT agrees to 0.01 % at 50 GeV and
  0.06 % at 5 GeV with 10000 events, also in the longitudinal shower shape (`Results.md`, section 3e).

---

## 2. Environment

| Component | Version / setting |
|---|---|
| Host | Intel Core i5-9600K, 6 cores (no SMT), 64 GB RAM, macOS 15.7.1 |
| Compiler | Apple Clang 17.0.0 (`/usr/bin/c++`) |
| CMake | 4.2.3 |
| Geant4 | 11.4.3 (geant4-11-04-patch-03), Release, multithreaded, system expat, no VecGeom/USolids |
| G4HepEm | tag `20261106`, Release, CPU only (`G4HepEm_CUDA_BUILD=OFF`) |
| AdePT (sources only) | v0.4.2 and master `d7cc1e6` (= v0.4.3, 2026-11-17) |
| GPU | none |

All runs are Release builds with `-DWITH_GPS=OFF`. The project sources were not modified. Builds and
outputs are in `~/atllarbarrel-review/` (section 8).

---

## 3. Code review

The code is well organised and readable, follows the usual Geant4 example structure, and is well
documented (README, BUILD_OPTIONS, Results). The recent work (MaterialManager, material fixes,
`HadInteraction` fix, AdePT integration and validation) is careful. The points below are ordered by
importance.

### 3.1 Findings

| # | Severity | Where | Finding | Recommendation |
|---|---|---|---|---|
| 1 | **Medium** | `hepemlib/src/PhysicsList.cc`, `PhysListHepEmTracking.cc` | The G4HepEm configuration is an EM-only list: no hadronic physics, no photo/electro-nuclear processes, `SetDefaults()` + `MscRangeFactor 0.04`. It is therefore not a like-for-like comparison with `FTFP_BERT`, and it is not the physics that AdePT uses (`G4EmStandardPhysics_AdePT` keeps the nuclear processes of FTFP_BERT). Measured effect: +0.4 % LAr energy at 50 GeV (section 6.3), and no low-energy tail from photonuclear events. | Add the matched configuration of Appendix A (`-DWITH_G4HepEmEM=ON`: reference list from `-p`, EM constructor replaced by `G4EmStandardPhysics` + `G4HepEmTrackingManager`). Keep the EM-only list for EM-only studies and label it as such in README/Results. **Fixed**: `WITH_G4HepEmEM` is now part of the project (Appendix A), and README/BUILD_OPTIONS label the `hepemlib/` list as EM only. |
| 2 | **Medium** | `src/LArBarrelPresamplerConstruction.cc:346, 411–416` | Known overlap: first `catho3` of the tilted module (`mod[1]`, −12°) protrudes 1.03 mm from `LAr::Barrel::Presampler::Module`. | See 4.2: the electrode pattern of this module does not fit in the module length, so a shift does not work; clip the first cathode. **Fixed** (4.6). |
| 3 | Low–Medium | `src/LArBarrelConstruction.cc` (thin-absorber straights, ~l.1129–1160, 1960–1993) | New: 5 `LAr::EMB::ThinAbs::Straight` copies protrude 6–30 µm from `LAr::EMB::STAC` at z ≈ 3112 mm (the end of STAC). | See 4.3: apply the same 50 µm `safety_zlen` used for other accordion pieces to the z half-length of the thin-absorber straights at the last fold. **Fixed** with a larger margin at the η cut (4.3, 4.6). |
| 4 | Low–Medium | `ATLLArBarrel.cc:112–116` (now 128–141) | With `WITH_ADEPT`, `ReplacePhysics(new G4EmStandardPhysics_AdePT())` silently replaces the EM constructor of *any* list given with `-p`. For lists with another EM option (suffixes `_EMV`, `_EMX`, `_EMY`, `_EMZ`, `_LIV`, `_PEN`, `__GS`, `__SS`, `_WVI`, `__LE`) the GPU then runs option-0 physics while the user believes otherwise. AdePT documents that `G4EmStandardPhysics_AdePT` is only for option 0 (its case 2). | Refuse or warn for lists whose name has an EM suffix; for those, AdePT recommends registering `AdePTTrackingManager` directly (its case 3). See 7.3. **Fixed**: the guard below is now in `ATLLArBarrel.cc` (lines 128–139); such lists stop with the fatal exception `ATLLArBarrel001`. Other EM options on the GPU (case 3) are not supported. |
| 5 | Low | `ATLLArBarrel.cc:69–83` | The command-line loop reads `argv[i+1]` without checking `i+1 < argc`: `./ATLLArBarrel -m` reads past the end of `argv` (undefined behaviour, crash). | Check `i+1 < argc` before reading the value and print the help otherwise. **Fixed**: an option without its value now prints the usage error and exits with status 1. |
| 6 | Low | `CMakeLists.txt:3` | `cmake_minimum_required(VERSION 3.8...3.18)`. `CMAKE_CUDA_ARCHITECTURES=native` (the project default for AdePT) needs CMake ≥ 3.24, and AdePT itself needs ≥ 3.25.2. Also, with CMake 4 the `...3.18` upper bound turns off all policies newer than 3.18. | Raise to `3.16...3.31` and, inside `if(WITH_ADEPT)`, add `cmake_minimum_required(VERSION 3.25.2)` (see 7.2). **Partly fixed**: `WITH_ADEPT` now stops with a clear message below CMake 3.25.2; the `cmake_minimum_required` range is unchanged. |
| 7 | Low | `GNUMakeFile` | Legacy Geant4 GNUmake file. The GNUmake system (`config/binmake.gmk`) is no longer shipped with Geant4 11, the file knows nothing about `hepemlib`, G4HepEm or AdePT, and its name has the wrong case for case-sensitive file systems (`GNUmakefile`). | Remove it; CMake is the only supported build. |
| 8 | Low | `CMakeLists.txt:95–113` | Directory-wide `include(${Geant4_USE_FILE})` and `include_directories(...)`; `${G4HepEm_LIBRARIES}` is linked also in the default build, where it is empty. | Use target-based `target_include_directories` / `target_link_libraries(ATLLArBarrel PRIVATE ...)` and link G4HepEm only when it is used. Not urgent. |
| 9 | Low | `src/LArBarrelConstruction.cc`, `src/LArTBCryostatConstruction.cc` | 206 `std::cout` (not `G4cout`) in the geometry code; in MT runs the output is interleaved and cannot be silenced with `/control/cout/...`. | Replace with `G4cout`, or guard the debug prints with the existing `DEBUG_GEO` macro. |
| 10 | Low | `src/ATLLArBarrelEventAction.cc:92–94` | `GetCollectionID` (string lookups) is called at every event, although the SD already caches the IDs. | Cache the three IDs in the event action at the first event. Negligible cost today. |
| 11 | Low | `ATLLArBarrel.cc:112` (now 125), `src/ATLLArBarrelActIni.cc:30` | `G4PhysListFactory` and the master `ATLLArBarrelEventAction` (created only to describe the ntuple columns) are never deleted. | Use a stack `G4PhysListFactory`; let the master run action own its event action. Cosmetic. |
| 12 | Info | `runcards/benchmarks/gamma5GeV.mac:4`, `electron50GeV.mac:4` | The CPU macros set `/run/setCut` after `/run/initialize`, the AdePT macros before. Both are correct for their configuration, but the difference is easy to copy wrongly. | Use the "before `/run/initialize`" order in all macros. |
| 13 | Info | `include/LArBarrelConstruction.hh:98` | `dely(Gama0, G)` ignores its first argument, which also shadows the member `Gama0`. It is only used when `A_SAGGING` is set. | Leave as in ATLAS, or remove the unused parameter. |

### 3.2 Sensitive detector, hits and user actions

- **`ATLLArBarrelSensDet`** – the cell index arithmetic is correct: the front section has 256 η × 4 φ =
  1024 cells, the middle 32 × 16 = 512 and the back 16 × 16 = 256, matching `FrontHitNo`, `MiddleHitNo`
  and `BackHitNo`. The guards `Eta < 0.8` and `|Phi| < ΔΦ_STAC/2` keep `HitID` in range, and
  `CheckHitID` catches anything else. The Birks law is a faithful copy of ATLAS `LArG4BirksLaw` with the
  right units (step length converted to cm, energy in MeV).
- `ProcessHits` uses only the pre-step position, the total energy deposit and the step length, and keeps no
  per-track state. This is exactly what AdePT can provide for steps returned from the GPU (`Results.md`,
  section 3d, verified this with 95 million steps).
- **Electronic noise** (20 MeV equivalent per cell, 1792 Gaussian numbers per event) is added at the end of
  the event in the event action, after the noise-free sums. This is correct, and keeping the
  `EnergyLArNoNoise` column makes the physics comparisons much more precise (the noise dominates the rms
  at 5 GeV).
- **`ATLLArBarrelTrackingAction`** – the fix that identifies primaries through
  `GetDynamicParticle()->GetPrimaryParticle()` instead of `ParentID == 0` is correct and also works with
  AdePT. The flag is only meaningful for hadron beams, as `Results.md` already states.
- **Reproducibility** – with the default seeds every repeated run (1 and 6 threads) gave bit-identical
  energy distributions. The CPU result at 5 GeV, 970.7 ± 2.9 MeV without noise, is identical to the
  value in `Results.md` measured on Linux. The geometry, physics and random sequence are therefore
  platform-independent, which is a good property for regression testing.

### 3.3 Compiler warnings

With `-Wall -Wextra -Wshadow -Wpedantic -Wconversion` the project compiles with about 70 warnings:

| Kind | Where | Comment |
|---|---|---|
| Unused variables (mostly `G4VPhysicalVolume*` returned by `new G4PVPlacement`) | geometry code | Harmless; drop the variable names. |
| Unused parameter / shadowing (`dely`, `elec_rot` declared twice, `th` in `ProcessHits`) | `LArBarrelConstruction.hh`, `LArBarrelPresamplerConstruction.cc:364/406`, `ATLLArBarrelSensDet.cc:89` | Harmless. |
| Float → int conversions (`std::floor` assigned to `G4int`) | `ATLLArBarrelSensDet.cc:151–156` | Correct as written; make them explicit with `static_cast<G4int>`. |

---

## 4. Geometry checks

### 4.1 Overlap checks with Geant4

`/geometry/test/run` on the full geometry (8904 placed volumes checked, macro
`runcards/benchmarks/geometry_test.mac`, one report per volume):

| Tolerance | Points per volume | Volumes with overlaps | Overlaps found | Time |
|---|---|---|---|---|
| 10 µm | 200 (as in `Results.md`) | 1 | `catho3` in `LAr::Barrel::Presampler::Module`, 0.98–1.01 mm | 10 s |
| 1 µm | 2000 | 2 | `catho3` 1.03 mm (659–678 points); `ThinAbs::Straight:17` in `STAC`, 30.0 µm (1 point) | 9 s |
| 1 µm | 20000 | 6 | `catho3` 1.03 mm (≈6800 points); `ThinAbs::Straight` copies 7, 33, 44, 50, 63 in `STAC`: 6.6, 15.4, 19.0, 19.5, 11.9 µm (1 point each) | 55 s |

The checks run on the master and on the worker thread; the table counts the master only.
No overlap between daughters (only protrusions from the mother) was found.

### 4.2 Presampler cathode `catho3` (1.03 mm)

Module 2 (`mod[1]`) has its 64 cathodes and 64 anodes tilted by −12°. The electrodes are placed from
`YStartC = -mod_leng/2 + (mod[1][5] + cathode_th/2)` with `mod[1][5] = 0.3 mm`
(`LArBarrelPresamplerConstruction.cc:346`). A tilted cathode of height 12.86 mm spans
±(0.5·12.86·sin 12° + 0.5·0.27·cos 12°) = ±1.47 mm along the stacking direction, but its centre is only
0.44 mm from the module edge, hence the 1.03 mm protrusion.

Shifting the pattern does not help. The pattern needs 1.47 + 63 × 4.621 + 2.31 + 1.50 ≈ 296.4 mm
(first cathode half-span + pitch + cathode–anode offset + last anode half-span), but the module is
295.74 mm long. Moving all electrodes by +1.04 mm would make the last anode protrude by about 0.65 mm
at the other end. In ATLAS the tilted electrodes are cut by the module walls. Options, in order of preference:

1. **Clip the first cathode of the tilted module:** build it as a `G4IntersectionSolid` of the tilted
   `G4Trd` with the module box, and place the clipped solid for `j == 0` of `mod[1]` only.
2. Shorten `heig_elec3` for that one electrode so that it fits (simpler, but changes its shape).
3. Lengthen module 2 by about 0.7 mm, if the sector has room. Check this against the sector length,
   which already uses `9*epsil` of clearance.

The effect on the physics is negligible (0.27 mm of cathode material in the presampler, outside the
sensitive `STAC`), but Geant4 navigation in overlapping volumes is undefined, and AdePT/VecGeom on the
GPU may behave differently from the CPU there.

### 4.3 Thin-absorber straight sections at the end of STAC (6–30 µm)

The protrusions are all at local z ≈ 3112 mm, the high-z face of the `STAC` polycone, and each is found
in a single point out of 20000. So they are corners of the `G4Trap` straight sections of the thin
absorber (η > 0.8 part of the accordion), cut at `Zmax` with half-lengths `dz0`/`dza` computed at the two
ends (`LArBarrelConstruction.cc:1129–1132`). Other accordion pieces already subtract a 50 µm
`safety_zlen` (l.1300–1302, 1374–1375, 1776–1777), but these straights do not.
**Recommendation:** subtract `safety_zlen` from the z half-length of the thin-absorber straights at the
last fold, as for the other pieces. The volumes are outside the readout region (`Eta < 0.8`), so the
energy is not affected, but very small overlaps at a mother boundary are a typical source of stuck
tracks on the GPU navigator.

**Correction (see 4.6).** A closer look shows that the straights *do* subtract `safety_zlen`
(`tl1`, `bl1` and the position `Zcd`). The protrusions are at the η = 1.475 cut, not at `Zmax`: the
z lengths `Zcp2l`/`Zcp2h` are computed on the centre line of the absorber, and the corners of the 2.2 mm
thick absorber cross the inner cone of `STAC` by up to 30 µm, more than the 50 µm z margin covers.

### 4.4 GDML file

`GDMLs/experimentalHall_v0p1.gdml` is an older export (v0.1). `Results.md` shows that its GeoModel
conversion (`gdml2gm`) is not usable: 3748 artefacts in the accordion. I recommend regenerating the GDML
from the current geometry (`fDumpGDML = true` in `ATLLArBarrelConstruction`, or a `/control` command),
because AdePT can also take the VecGeom geometry from a GDML (`/adept/setVecGeomGDML`), and a stale file
would silently use different materials (the Gten and Cables fixes of October 2026).

### 4.5 Materials

`MaterialManager` removes all duplicated materials, and the fixes of `Gten`, `SiO2`, `MBoards` and `Cables`
described in `Results.md`, section 5, are correct. No further material problem was found.

### 4.6 Fixes applied

Both overlaps are fixed in `src/`. The original files are kept unchanged in `legacy/src/`;
`-DWITH_LEGACY_GEOMETRY=ON` builds them instead of the fixed ones.

- **`catho3`** (`src/LArBarrelPresamplerConstruction.cc`): option 1 of 4.2. In a tilted module
  (`mod[i][3] != 0`, i.e. module 2) the first cathode is a `G4IntersectionSolid` of the tilted `G4Trd`
  with the module envelope, shrunk by `epsil` (7 µm) along the stacking direction to avoid coincident
  surfaces. The clipped cathode is placed at the module origin with the same name `catho3`.
  The other 63 cathodes and all anodes are unchanged.
- **Thin-absorber straights** (`src/LArBarrelConstruction.cc`): where the straight is cut by the
  η = 1.475 line, the thin absorber (not the electrode) is shortened by 0.2 mm at its high-z end
  (`safety_zlen_abs = 0.1 mm` subtracted from `tl1`/`bl1`). Its low-z end and the thick absorber
  inside it do not move.

Checks with Geant4 11.4.3 (tolerance 1 µm, `/geometry/test/run`):

| Geometry | 20000 points per volume | 200000 points per volume |
|---|---|---|
| `legacy/src` (original) | `catho3` 1.03 mm; `ThinAbs::Straight` 7, 33, 44, 50, 63: 6.6–19.5 µm (as in 4.1) | not run |
| `src` (fixed) | **no overlaps** (8904 volumes) | **no overlaps** |

Physics: 400 events of 10 GeV e- at η = 0.2 (FTFP_BERT, cut 0.01 mm) give
⟨E<sub>LAr</sub>⟩ = 1922.5 ± 10.1 MeV with the fixed geometry and 1914.6 ± 9.9 MeV with the original one,
a difference of 0.6σ. The removed material is 0.27 mm of cathode outside `STAC` and 0.2 mm of lead
at the η = 1.475 edge, outside the readout region.

**AdePT on GPU (2026-11-19).** The two AdePT benchmarks of `Results.md`, section 3, were re-run on the
machine of `Results.md` (EL9, NVIDIA RTX 2070 SUPER, CUDA 13.1, Geant4 11.4.3, AdePT 0.4.2,
`CMAKE_CUDA_ARCHITECTURES=75`) with two builds from the current sources:

- **fixed**: `src/`, built from the top of AdePT_examples (`examples/CMakeLists.txt`, `WITH_ADEPT` forced on);
- **legacy**: `legacy/src`, standalone build with `-DWITH_ADEPT=ON -DWITH_LEGACY_GEOMETRY=ON`.

Both use the unmodified `runcards/benchmarks/*_adept.mac` (fluctuations on, cut before init).
Energies in MeV, errors are standard errors of the mean. "Paired" is the mean of the event-by-event
difference fixed − legacy (same `EventID`).

5 GeV γ, 300 events, 1 thread (`gamma5GeV_adept.mac`):

| Geometry | EnergyLAr | EnergyLArNoNoise | Time per event |
|---|---|---|---|
| `Results.md` 3a (cut before init) | 960.5 ± 10.3 (rms 179) | 973.4 ± 2.6 (rms 45.2, min 834.5) | 232.0 ms |
| legacy, re-run | 960.5 ± 10.3 (rms 179.0) | 973.4 ± 2.6 (rms 45.2, min 834.5) | 227 ms |
| **fixed** | 961.3 ± 10.2 (rms 177.0) | **973.8 ± 2.6** (rms 44.4, min 835.5) | 224 ms |
| fixed − legacy (paired) | | +0.4 ± 0.6 | |

50 GeV e⁻, 150 events, 8 threads (`electron50GeV_adept.mac`):

| Geometry | EnergyLAr | EnergyLArNoNoise | Wall time |
|---|---|---|---|
| `Results.md` 3b (cut before init) | 9899.0 ± 19.0 (rms 232, min 8374.2) | 9920.7 ± 13.2 (rms 161, min 8472.5) | 20.31–20.45 s |
| legacy, re-run | 9899.0 ± 19.0 (rms 232.3, min 8374.2) | 9920.7 ± 13.2 (rms 161.3, min 8472.5) | 20.2 s |
| **fixed** | 9895.7 ± 19.8 (rms 242.7, min 8168.3) | **9919.4 ± 13.3** (rms 162.8, min 8446.4) | 19.9 s |
| fixed − legacy (paired) | | −1.3 ± 0.8 | |

- The legacy build reproduces `Results.md` digit for digit, so the software stack is unchanged and the
  comparison isolates the geometry fix.
- With the fixed geometry the noise-free LAr energy changes by +0.04 % (5 GeV) and −0.01 % (50 GeV),
  compatible with zero. The Front, Middle and Back compartment energies agree within their errors, and
  `HadInteraction` is 0 in all runs.
- 119/300 (5 GeV) and 32/150 (50 GeV) events are bit-identical between the two geometries. The others
  differ because tracks crossing the modified volumes change the random sequence, as expected.
- No `G4Exception`, no overlap message and no stuck or lost tracks in any of the four runs; the time per
  event is the same for both geometries.

---

## 5. Builds

Three configurations were built from the unmodified sources, plus one from a copy with the patch of
Appendix A:

| Build directory | Options | Physics | Result |
|---|---|---|---|
| `build-cpu` | default | `FTFP_BERT` (Geant4 reference list) | OK, 0 warnings |
| `build-hepem` | `-DWITH_G4HepEm=ON` | hepemlib EM-only list + `G4HepEmTrackingManager` | OK, 0 warnings. CMake switches to tracking mode automatically because G4HepEm `20261106` has no `G4HepEmProcess` |
| `build-hepemem` (patched copy) | `-DWITH_G4HepEm=ON -DWITH_G4HepEmEM=ON` | `FTFP_BERT` with `G4EmStandardPhysics` + `G4HepEmTrackingManager` for e±, γ | OK |
| `build-warn` | `-DWITH_G4HepEm=ON` + extra warning flags | – | OK, ~70 warnings (3.3) |

```sh
source <geant4-install>/bin/geant4.sh
cmake -S ATLLArBarrel -B build-cpu   -DCMAKE_BUILD_TYPE=Release -DGeant4_DIR=<geant4-install>/lib/cmake/Geant4
cmake -S ATLLArBarrel -B build-hepem -DCMAKE_BUILD_TYPE=Release -DGeant4_DIR=<geant4-install>/lib/cmake/Geant4 \
      -DG4HepEm_DIR=<g4hepem-install>/lib/cmake/G4HepEm -DWITH_G4HepEm=ON
cmake --build build-cpu -j6 && cmake --build build-hepem -j6
```

At run time the output shows `--> Using Geant4 reference physics list`, `--> Using G4HepEm with
specialized tracking` and, for the patched build, `--> Using Geant4 reference physics list FTFP_BERT with
G4HepEm EM tracking`. In the patched build G4HepEm reports that it found the `photonNuclear`,
`electronNuclear` and `positronNuclear` processes of FTFP_BERT and runs them as extra processes.

---

## 6. Speed tests: Geant4 vs G4HepEm

### 6.1 Method

- Benchmark macros of the project: `runcards/benchmarks/gamma5GeV.mac` (300 photons of 5 GeV) and
  `electron50GeV.mac` (150 electrons of 50 GeV), gun at the origin towards η = 0.2, φ = 0.01225,
  production cut 0.01 mm. For 6 threads the 50 GeV macro was run with 300 events.
- Time = event-loop wall time printed by the master run action (`Run terminated ... Time: Real=`),
  initialisation excluded. Time per event = wall time / events. With 6 threads this is the throughput.
- All runs sequential on an otherwise idle machine; 3 repetitions at 5 GeV, 2 at 50 GeV. Errors are the
  spread of the repetitions (standard error), i.e. machine noise only, because the seeds are fixed.
- Energies are computed from the `EnergyLArNoNoise` branch (Birks-corrected, no electronic noise), mean ±
  standard error.
- Configurations: **CPU** = `FTFP_BERT`; **G4HepEm (EM-only)** = the project's `-DWITH_G4HepEm=ON`;
  **G4HepEm (matched)** = `FTFP_BERT` with G4HepEm EM tracking (Appendix A).

### 6.2 Timing

| Benchmark | Threads | CPU FTFP_BERT | G4HepEm (EM-only) | G4HepEm (matched) | Speed-up EM-only | Speed-up matched |
|---|---|---|---|---|---|---|
| 5 GeV γ, 300 events | 1 | 145.1 ± 2.5 ms | 102.3 ± 0.8 ms | 96.4 ± 0.1 ms | **1.42** | **1.51** |
| 50 GeV e⁻, 150 events | 1 | 1318 ± 9 ms | 928 ± 6 ms | 940 ± 1 ms | **1.42** | **1.40** |
| 50 GeV e⁻, 300 events | 6 | 254.0 ± 2.8 ms | 180.1 ± 1.2 ms | 176.2 ± 0.6 ms | **1.41** | **1.44** |

Individual wall times (s): 5 GeV: CPU 42.86 / 45.04 / 42.72, EM-only 30.21 / 30.92 / 30.97,
matched 28.89 / 28.99 / 28.91. 50 GeV, 1 thread: CPU 196.37 / 199.14, EM-only 138.35 / 140.12,
matched 141.03 / 140.97. 50 GeV, 6 threads: CPU 75.38 / 77.03, EM-only 53.66 / 54.40, matched 52.69 / 53.06.

- **G4HepEm is 1.40–1.51× faster** than Geant4, with the same speed-up at 5 and 50 GeV and with 1 or 6
  threads. The time is dominated by the tracking of e± and γ in the accordion, as `Results.md` also
  concluded (1.44× on its EL9 machine). This machine is ~6 % slower per event than the one of
  `Results.md` for the CPU (145 ms vs 137 ms at 5 GeV).
- Keeping the hadronic and nuclear physics of FTFP_BERT costs nothing measurable: the matched
  configuration is as fast as the EM-only list (faster at 5 GeV, 1 % slower at 50 GeV, 2 % faster with
  6 threads).
- **Multithreaded scaling** on 6 physical cores: 1318/254 = **5.19×** for the CPU, 5.15× for EM-only
  and 5.33× for matched G4HepEm (efficiency 86–89 %), so G4HepEm does not change the MT behaviour.

### 6.3 Physics agreement

| Benchmark | CPU FTFP_BERT (MeV) | G4HepEm EM-only (MeV) | Δ EM-only | G4HepEm matched (MeV) | Δ matched |
|---|---|---|---|---|---|
| 5 GeV γ, 300 ev. | 970.7 ± 2.9 (rms 49.7) | 974.7 ± 2.7 (rms 46.6) | +4.0 ± 4.0 (+0.4 %, 1.0 σ) | 971.0 ± 2.7 (rms 47.6) | +0.3 ± 4.0 (0.0 %) |
| 50 GeV e⁻, 150 ev., 1 thr. | 9924.2 ± 9.4 (rms 115) | 9966.9 ± 10.3 (rms 126) | **+42.7 ± 13.9 (+0.43 %, 3.1 σ)** | 9923.6 ± 9.4 (rms 115) | −0.6 ± 13.3 (0.0 %) |
| 50 GeV e⁻, 300 ev., 6 thr. | 9923.0 ± 7.6 (rms 132) | 9961.2 ± 7.2 (rms 125) | **+38.2 ± 10.5 (+0.38 %, 3.6 σ)** | 9913.0 ± 7.4 (rms 120)¹ | −10.0 ± 10.6 (−0.1 %, 0.9 σ) |

¹ 299 events: one event deposits 4594 MeV (all 300 events: 9895.3 ± 19.2 MeV, rms 332). It is the kind of
photonuclear event described in `Results.md`, section 3b (a 30–35 GeV photon converted into hadrons), and
it can only occur when the nuclear processes are present.

With the electronic noise included (`EnergyLAr`) the conclusions are the same, with errors about 2× larger.

**Interpretation.** The EM-only list deposits about 0.4 % more energy at 50 GeV, with 3–3.6 σ
significance in two independent samples. The matched configuration agrees with FTFP_BERT, so the excess
comes from the missing photo/electro-nuclear processes, which in FTFP_BERT move part of the shower energy
into hadrons and neutrons that deposit little in the LAr. It is not caused by G4HepEm itself. This also
explains why the G4HepEm comparison of `Results.md`, section 1, is (as its Caveats section already
warns) not a matched control for AdePT. AdePT's `G4EmStandardPhysics_AdePT` does keep the nuclear
processes, so **the matched configuration is the right CPU reference for G4HepEm and AdePT**.

---

## 7. Running with AdePT on GPU: code and build changes

AdePT transports e⁻, e⁺ and γ on an NVIDIA GPU. It uses G4HepEm physics compiled for CUDA and the
VecGeom geometry, converted automatically from the Geant4 geometry by G4VG. All other particles stay on
the CPU, and the steps in sensitive volumes are returned to the CPU, where the normal Geant4 sensitive
detector processes them.

A step-by-step migration guide from the original CPU application, with the complete code of a project
AdePT EM constructor and the converted macros, is in [AdePT_migration.md](AdePT_migration.md).

**Most of what is needed is already in the repository** (commit `8099cac` and follow-ups, validated in
`Results.md`, section 3). Section 7.1 lists all the changes, marking what exists and what is still
recommended. The items still to do (7.2–7.5) were checked against the AdePT sources but not built,
because this machine has no NVIDIA GPU.

### 7.1 Overview of the changes

| Area | Change | Status |
|---|---|---|
| CMake | `WITH_ADEPT` option, `enable_language(CUDA)`, `CMAKE_CUDA_ARCHITECTURES` default, `find_package(AdePT)`, `USE_ADEPT` definition, `cuda_rdc_target_link_libraries` | ✅ present (`CMakeLists.txt:64–79, 110–111`) |
| CMake | Mutual exclusion `WITH_ADEPT` / `WITH_G4HepEm` | ✅ present |
| CMake | Minimum CMake version for CUDA/AdePT, `PRIVATE` link scope | ✅ added (7.2) |
| Physics | `ReplacePhysics(new ATLLArBarrelEmPhysics_AdePT())` on the reference list: same physics as `G4EmStandardPhysics_AdePT`, project defaults in code; default cut 0.01 mm | ✅ present (`ATLLArBarrel.cc`, `adept/`; AdePT_migration.md option B) |
| Physics | Guard for lists with non-default EM options | ✅ present (`ATLLArBarrel.cc:128–139`, finding 4) |
| Physics | Direct `AdePTTrackingManager` for other EM setups (case 3) | not needed for the validated `FTFP_BERT` (7.3) |
| User code | Sensitive detector compatible with returned GPU steps | ✅ verified (`Results.md` 3d, section 3.2 here) |
| User code | Primary identification without `ParentID == 0` | ✅ fixed (`ATLLArBarrelTrackingAction.cc`) |
| Macros | `/adept/*` before `/run/initialize`, `/run/setCut` before `/run/initialize`, energy-loss fluctuations on, GPU in all regions | ✅ present (`runcards/benchmarks/*_adept.mac`) |
| Geometry | Fix the two overlaps before GPU production runs | ✅ fixed (section 4.6; original in `legacy/src`) |
| Dependencies | CUDA, VecCore, VecGeom (CUDA), G4HepEm (CUDA), G4VG, AdePT; versions for AdePT 0.4.3 | ⚠️ updated list (7.4) |
| Docs | Matched CPU reference for AdePT | ✅ added (`-DWITH_G4HepEm=ON -DWITH_G4HepEmEM=ON`, Appendix A) |

### 7.2 CMake changes

What the repository has (correct for AdePT 0.4.2 and 0.4.3):

```cmake
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
endif()
...
elseif(WITH_ADEPT)
    cuda_rdc_target_link_libraries(ATLLArBarrel ${Geant4_LIBRARIES} ${AdePT_LIBRARIES})
```

- `find_package(AdePT)` pulls in VecCore (CUDA backend), VecGeom, G4HepEm and G4VG through
  `AdePTConfig.cmake`. It stops with an error if G4HepEm was built without CUDA, and it sets
  `AdePT_LIBRARIES = AdePT::G4Integration`, whose include directories propagate the AdePT headers.
- `cuda_rdc_target_link_libraries` comes from VecGeom's CUDA-RDC helpers and is **required**. With plain
  `target_link_libraries` the device code of VecGeom/AdePT is not linked consistently ("relocatable device
  code"), and the executable fails at link or load time.
- `AdePTConfig.cmake` adds `-Wl,--disable-new-dtags` to `CMAKE_EXE_LINKER_FLAGS`. This is a GNU ld flag,
  so the AdePT build is Linux-only in practice, which is also the only platform with CUDA.

Recommended additions (the version check and `PRIVATE` are now in `CMakeLists.txt`; the
`cmake_minimum_required` range is unchanged):

```cmake
# top of the file: CMake 4 compatible range, keeps modern policies
cmake_minimum_required(VERSION 3.16...3.31)

if(WITH_ADEPT)
    # CUDA_ARCHITECTURES=native needs 3.24; AdePT itself requires 3.25.2
    if(CMAKE_VERSION VERSION_LESS 3.25.2)
        message(FATAL_ERROR "WITH_ADEPT requires CMake >= 3.25.2 (found ${CMAKE_VERSION})")
    endif()
    ...
endif()

# explicit scope, as in the AdePT integration documentation
cuda_rdc_target_link_libraries(ATLLArBarrel PRIVATE ${Geant4_LIBRARIES} ${AdePT_LIBRARIES})
```

`CMAKE_CUDA_ARCHITECTURES` must be the same for VecGeom, G4HepEm, AdePT and ATLLArBarrel (e.g. `75`
for an RTX 2070, `80` for A100, `90` for H100). `native` is only correct when building on the machine
that will run the job.

### 7.3 Source code changes

**Physics list** (`ATLLArBarrel.cc`), as in the repository:

```cpp
#ifdef USE_ADEPT
#include <AdePT/g4integration/G4EmStandardPhysics_AdePT.hh>
#endif
...
auto physicsList = physListFactory->GetReferencePhysList( custom_pl );
#ifdef USE_ADEPT
physicsList->ReplacePhysics( new G4EmStandardPhysics_AdePT() );
#endif
runManager->SetUserInitialization(physicsList);
```

`G4EmStandardPhysics_AdePT` derives from `G4EmStandardPhysics` (option 0). It registers the standard
processes, then attaches an `AdePTTrackingManager` to e⁻, e⁺ and γ. This is "case 2" of the AdePT
integration guide, and it is equivalent to AdePT's `FTFP_BERT_AdePT` when `-p FTFP_BERT`.

**Update (2026-11-19).** The project now uses its own constructor `ATLLArBarrelEmPhysics_AdePT` (`adept/`,
`AdePT_migration.md` step 2, option B): the same `ConstructProcess()`, with the validated AdePT settings as
defaults, and `physicsList->SetDefaultCutValue(0.01*mm)` in `main()`. The code below shows the guard with the
current names.

Guard (finding 4, now in `ATLLArBarrel.cc`), so that a list with a different EM option is not silently
changed to option 0. Tested with the AdePT build on the GPU machine (2026-11-19): `FTFP_BERT_EMZ` and
`QGSP_BIC__GS` stop with `ATLLArBarrel001`, `FTFP_BERT` and `FTFP_BERT_EM0` run. As in the project:

```cpp
#ifdef USE_ADEPT
// The AdePT EM constructor uses EM option 0 (the only one implemented in G4HepEm):
// refuse reference lists that use another EM option (suffixes of G4PhysListFactory;
// "_EM0" is option 0 and is fine)
for (const G4String suffix : {"_EMV", "_EMX", "_EMY", "_EMZ", "_LIV", "_PEN", "__GS", "__SS", "_WVI", "__LE"}) {
  if (G4StrUtil::ends_with(custom_pl, suffix)) {
    G4ExceptionDescription msg;
    msg << "Physics list " << custom_pl << " uses a non-default EM option, which "
        << "the AdePT EM constructor would replace with option 0. "
        << "Use a list without EM suffix (or with _EM0) when running with AdePT.";
    G4Exception("main()", "ATLLArBarrel001", FatalException, msg);
  }
}
G4cout << "--> Using AdePT for e-, e+ and gamma" << G4endl;
physicsList->ReplacePhysics(new ATLLArBarrelEmPhysics_AdePT());
physicsList->SetDefaultCutValue(0.01 * mm);
#endif
```

To run AdePT with another EM option ("case 3" of the AdePT guide), write a physics constructor that
derives from the wanted `G4EmStandardPhysics_optionN`, calls its `ConstructProcess()`, then creates an
`AdePTConfiguration` and an `AdePTTrackingManager`, sets the G4HepEm parameters that mirror that option
through `GetG4HepEmConfig()`, and attaches the manager to `G4Electron`, `G4Positron` and `G4Gamma`. This
is the same structure as `G4EmStandardPhysics_G4HepEm` in Appendix A, with `AdePTTrackingManager` in
place of `G4HepEmTrackingManager`.

**User code requirements** (all satisfied by the current code; keep them in mind for changes):

- Sensitive detectors may use only what AdePT copies back from the GPU (`GPUStep` in
  `AdePT/transport/steps/GPUStep.hh`): pre/post-step position, momentum direction, kinetic energy and
  navigation state (volume), total energy deposit, step length, global/local/proper time, weight,
  particle type, track/parent ID, number of secondaries and the step-limiting process. The non-ionising
  energy deposit is **not** copied (the field is commented out in 0.4.3), so `GetNonIonizingEnergyDeposit()`
  would always be 0 for GPU steps. ATLLArBarrel uses pre-step position, energy deposit and step length.
- No per-track state in shared members of sensitive detectors or user actions, and no killing of GPU
  tracks from CPU code.
- Do not identify primaries with `ParentID == 0`: with the default settings the secondaries of nuclear
  reactions on GPU steps also have ParentID 0. Use `GetPrimaryParticle()`, as the current tracking action
  does, or run with `/adept/returnFirstAndLastStep true`.
- User tracking and stepping actions are **not** called for GPU tracks unless
  `/adept/CallUserTrackingAction true` / `/adept/CallUserSteppingAction true` are set (slower). The
  current tracking action only looks at primary hadrons, which stay on the CPU, so it needs neither.
- A magnetic field would need AdePT built with `ADEPT_USE_EXT_BFIELD` (covfie field map,
  `/adept/setCovfieBfieldFile`). The test beam setup has no field, so nothing is needed.

### 7.4 Dependencies and build on the GPU machine

Requirements of AdePT 0.4.3 (master, 2026-11-17), from its `CMakeLists.txt` and installation guide:

| Package | Version | Build options |
|---|---|---|
| CMake | ≥ 3.25.2 | |
| Compiler | C++20 | |
| CUDA | 12 or newer recommended | |
| Geant4 | > 11.0 | `GEANT4_USE_GDML=ON`, `GEANT4_USE_SYSTEM_EXPAT=OFF` recommended, `GEANT4_USE_USOLIDS=OFF` |
| VecCore | ≥ 0.5.2 (0.8.2 used in `Results.md`) | |
| VecGeom | **≥ 2.2.1** (0.4.2 had no minimum) | `VECGEOM_ENABLE_CUDA=ON`, `VECGEOM_GDML=ON`, `BACKEND=Scalar`, `VECGEOM_NAV=index` for this geometry (`Results.md` section 2), same `CMAKE_CUDA_ARCHITECTURES` |
| G4HepEm | **≥ tag `20261106`** | `G4HepEm_CUDA_BUILD=ON`, `G4HepEm_EARLY_TRACKING_EXIT=ON` |
| G4VG | 1.0.7 | fetched by AdePT if not installed |
| AdePT | 0.4.2 (validated in `Results.md`) or 0.4.3 | `CMAKE_CUDA_ARCHITECTURES` as above |

The validated stack of `Results.md` (AdePT 0.4.2, VecGeom 2.2.0, G4HepEm `780e798`) does **not** satisfy
AdePT 0.4.3. To move to 0.4.3, update VecGeom to 2.2.1 and G4HepEm to `20261106` (the tag used in this
review for the CPU builds), then repeat the validation of `Results.md`, section 3. The UI commands used by
the project's macros are unchanged between 0.4.2 and 0.4.3 (0.4.3 only adds `/adept/validateHandoff`).

Build commands on the GPU machine:

```sh
source <geant4-install>/bin/geant4.sh
cmake -S ATLLArBarrel -B build-adept -DCMAKE_BUILD_TYPE=Release \
      -DGeant4_DIR=<geant4-install>/lib64/cmake/Geant4 \
      -DCMAKE_PREFIX_PATH="<adept-install>;<vecgeom-install>;<veccore-install>;<g4hepem-install>" \
      -DCMAKE_CUDA_ARCHITECTURES=75 -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc \
      -DWITH_ADEPT=ON -DWITH_GPS=OFF
cmake --build build-adept -j
```

An alternative inside the CERN network is the LCG view
`/cvmfs/sft.cern.ch/lcg/views/devAdePT/latest/x86_64-el9-gcc13-opt/setup.sh`, which provides the whole
stack, including CUDA through `/cvmfs/projects.cern.ch`.

### 7.5 Run-time configuration (macros)

All `/adept/` commands, **and `/run/setCut`**, must come before `/run/initialize`:

```
/adept/setVerbosity 0
/adept/setMillionsOfTrackSlots 1
/adept/setMillionsOfHitSlots 1
/adept/setCUDAStackLimit 8192
/adept/setTrackInAllRegions true        # without this (or /adept/addGPURegion) nothing goes to the GPU
/adept/SetEnergyLossFluctuation true    # AdePT default is off, Geant4 lists have it on
/adept/setSeed 1234                     # optional, for reproducible GPU random numbers
/run/setCut 0.01 mm                     # after /run/initialize the GPU keeps the default cut (Results.md 3c)
/run/initialize
```

Notes:

- `/run/setCut` after `/run/initialize` produced a +1.4 % LAr energy excess on the GPU (`Results.md`, 3c).
  This is the most important pitfall of the integration and should be stated prominently in the README.
- Track and hit slots: 1 M each was enough for 50 GeV electrons with 8 threads on an 8 GB card.
  Increase `setMillionsOfHitSlots` for higher energies or more threads.
- The GPU is used efficiently only with many tracks in flight. Use several CPU threads (`-t`) and
  high-energy showers: with 1 thread and 5 GeV photons AdePT was 1.7× *slower* than the CPU, with 8 threads
  and 50 GeV electrons 1.4× faster (`Results.md`, 3a–3b).
- For a fair timing comparison, use the matched G4HepEm configuration (Appendix A) as the CPU reference.
  It has the same physics as `G4EmStandardPhysics_AdePT`, but on the CPU.

### 7.6 Validation checklist on the GPU machine

1. Output shows `--> Using AdePT for e-, e+ and gamma` and the allocation of GPU track/hit slots.
2. The G4HepEm configuration printed at initialisation shows energy-loss fluctuations on and the
   production cut of 0.01 mm.
3. `electron50GeV_adept.mac` with 8 threads: `EnergyLArNoNoise` within 0.2 % of the CPU (`Results.md`: +0.15 ± 0.06 %).
   ✅ Done on 2026-11-19 with the fixed geometry (`Results.md` 3e), 10000 events: +0.09 ± 0.02 % against FTFP_BERT
   and +0.01 ± 0.02 % against the matched CPU reference (`-DWITH_G4HepEm=ON -DWITH_G4HepEmEM=ON`).
4. `gamma5GeV_adept.mac`: agreement within statistics. ✅ Done (`Results.md` 3e), 10000 events: +0.07 ± 0.07 %
   against FTFP_BERT, −0.06 ± 0.07 % against the matched reference. The longitudinal shape (Front, Middle, Back)
   also agrees.
5. `HadInteraction` = 0 for e⁻/γ beams, and the same value as on the CPU for a pion beam.
6. Repeat after the geometry fixes of section 4: the energy should not change. ✅ Done on 2026-11-19
   for `gamma5GeV_adept.mac` and `electron50GeV_adept.mac` (section 4.6): no change within statistics.

---

## 8. How to reproduce

All review builds and outputs are in `~/atllarbarrel-review/`:

| Path | Content |
|---|---|
| `build-cpu/`, `build-hepem/`, `build-hepemem/`, `build-warn/` | builds of section 5 |
| `src-matched/` | copy of the repository with the patch of Appendix A (`g4hepem_em_variant.patch`) |
| `geomtest.log`, `geomtest_hires.log`, `geomtest_vhires.log` | overlap checks of 4.1 (200, 2000, 20000 points) |
| `bench.sh`, `bench2.sh` | benchmark scripts (sequential runs) |
| `runs/<benchmark>_<threads>_<config>_r<rep>/` | `run.log` and `ATLLArBarrelout_Run0.root` of each run |
| `bench_summary.txt`, `bench_summary2.txt` | timings |
| `estats.py` | energy statistics from the ROOT files (needs `uproot` and `numpy`) |
| `adept-v0.4.2/`, `adept-master/` | AdePT sources used for section 7 |

```sh
source ~/Geant4/install-v11.4.3/bin/geant4.sh
cd ~/atllarbarrel-review
./bench.sh       # CPU and G4HepEm EM-only, ~35 min on 6 cores
./bench2.sh      # matched G4HepEm, ~15 min
python3 estats.py runs/*/ATLLArBarrelout_Run0.root
```

The ROOT 6.32 installation on this Mac does not start: its precompiled modules no longer match the
updated Xcode SDK. The ROOT files were therefore read with `uproot` from a Python virtual environment.

---

## Appendix A: patch for the matched G4HepEm configuration

**Applied to the project on 2026-11-19**: the patch below is kept for reference and no longer needs to
be applied (`git apply` now fails, because the changes are already there). The project version adds one
setting: G4HepEm's multiple steps in MSC + transportation are disabled by default, as AdePT does on the GPU, and
`-DWITH_G4HepEmEM_MSC_MULTISTEPS=ON` restores the G4HepEm default used by the patch below and by the matched
results of section 6 (see `Results.md`, section 3e).

Adds the CMake option `WITH_G4HepEmEM` (requires `WITH_G4HepEm=ON`): the reference physics list chosen with
`-p` is kept, and only its EM constructor is replaced by `G4EmStandardPhysics` with the
`G4HepEmTrackingManager` for e⁻, e⁺ and γ. Nuclear processes of e⁻, e⁺ and γ are kept and run by G4HepEm
as extra processes. Same structure as AdePT's `G4EmStandardPhysics_HepEm`.

```sh
cmake ... -DG4HepEm_DIR=<g4hepem-install>/lib/cmake/G4HepEm -DWITH_G4HepEm=ON -DWITH_G4HepEmEM=ON
./ATLLArBarrel -m ../runcards/benchmarks/electron50GeV.mac -t 6 -p FTFP_BERT
```

```diff
diff --git a/examples/ATLLArBarrel/ATLLArBarrel.cc b/examples/ATLLArBarrel/ATLLArBarrel.cc
index 862d6ad..c400036 100644
--- a/examples/ATLLArBarrel/ATLLArBarrel.cc
+++ b/examples/ATLLArBarrel/ATLLArBarrel.cc
@@ -28,6 +28,7 @@
 
 #ifdef USE_G4HepEm
 #include "PhysicsList.hh" //it is built as a separate library in hepem
+#include "G4EmStandardPhysics_G4HepEm.hh"
 #endif
 
 #ifdef USE_ADEPT
@@ -130,7 +131,13 @@ int main(int argc, char **argv)
   runManager->SetUserInitialization(physicsList);
 #endif
 
-#ifdef USE_G4HepEm
+#ifdef USE_G4HepEmEM // reference list, only its EM constructor uses G4HepEm tracking
+  G4cout << "--> Using Geant4 reference physics list " << custom_pl << " with G4HepEm EM tracking" << G4endl;
+  auto physListFactory = new G4PhysListFactory();
+  auto physicsList     = physListFactory->GetReferencePhysList(custom_pl);
+  physicsList->ReplacePhysics(new G4EmStandardPhysics_G4HepEm());
+  runManager->SetUserInitialization(physicsList);
+#elif defined(USE_G4HepEm)
 #ifdef USE_G4HepEmTracking // use G4HepEm with specialized tracking
   G4cout << "--> Using G4HepEm with specialized tracking" << G4endl;
   runManager->SetUserInitialization(new PhysicsList("G4HepEmTracking"));
diff --git a/examples/ATLLArBarrel/CMakeLists.txt b/examples/ATLLArBarrel/CMakeLists.txt
index 62a4e94..f42398d 100644
--- a/examples/ATLLArBarrel/CMakeLists.txt
+++ b/examples/ATLLArBarrel/CMakeLists.txt
@@ -62,6 +62,19 @@ if(WITH_G4HepEm)
     add_subdirectory(hepemlib)
 endif()
 
+#----------------------------------------------------------------------------
+# Option to keep the reference physics list (-p) and replace only its EM
+# constructor with G4EmStandardPhysics + G4HepEmTrackingManager
+#
+option(WITH_G4HepEmEM "With WITH_G4HepEm: reference physics list with G4HepEm EM tracking" OFF)
+if(WITH_G4HepEmEM)
+    if(NOT WITH_G4HepEm)
+        message(FATAL_ERROR "WITH_G4HepEmEM requires WITH_G4HepEm=ON")
+    endif()
+    add_compile_definitions(USE_G4HepEmEM)
+    message("Project CMAKE option: building with USE_G4HepEmEM (reference list + G4HepEm EM tracking)")
+endif()
+
 if(WITH_G4HepEmTracking)
     add_compile_definitions(USE_G4HepEmTracking)
     message("Project CMAKE option: building with USE_G4HepEm with tracking")
diff --git a/examples/ATLLArBarrel/hepemlib/CMakeLists.txt b/examples/ATLLArBarrel/hepemlib/CMakeLists.txt
index 774cd86..6893855 100644
--- a/examples/ATLLArBarrel/hepemlib/CMakeLists.txt
+++ b/examples/ATLLArBarrel/hepemlib/CMakeLists.txt
@@ -3,6 +3,8 @@ add_library(hepemlib
     include/PhysListHepEmTracking.hh
     src/PhysicsList.cc
     src/PhysListHepEmTracking.cc
+    include/G4EmStandardPhysics_G4HepEm.hh
+    src/G4EmStandardPhysics_G4HepEm.cc
 )
 
 # G4HepEmProcess is only available in older G4HepEm versions
diff --git a/examples/ATLLArBarrel/hepemlib/include/G4EmStandardPhysics_G4HepEm.hh b/examples/ATLLArBarrel/hepemlib/include/G4EmStandardPhysics_G4HepEm.hh
new file mode 100644
index 0000000..d1eab8b
--- /dev/null
+++ b/examples/ATLLArBarrel/hepemlib/include/G4EmStandardPhysics_G4HepEm.hh
@@ -0,0 +1,22 @@
+//**************************************************
+// \file G4EmStandardPhysics_G4HepEm.hh
+// \brief: G4EmStandardPhysics with e-, e+ and gamma
+//         tracked by the G4HepEmTrackingManager
+//         (same structure as AdePT's G4EmStandardPhysics_HepEm)
+//**************************************************
+
+#ifndef G4EmStandardPhysics_G4HepEm_h
+#define G4EmStandardPhysics_G4HepEm_h 1
+
+#include "G4EmStandardPhysics.hh"
+
+class G4EmStandardPhysics_G4HepEm : public G4EmStandardPhysics {
+
+public:
+  explicit G4EmStandardPhysics_G4HepEm(G4int ver = 1, const G4String &name = "");
+  ~G4EmStandardPhysics_G4HepEm() override = default;
+
+  void ConstructProcess() override;
+};
+
+#endif // G4EmStandardPhysics_G4HepEm_h
diff --git a/examples/ATLLArBarrel/hepemlib/src/G4EmStandardPhysics_G4HepEm.cc b/examples/ATLLArBarrel/hepemlib/src/G4EmStandardPhysics_G4HepEm.cc
new file mode 100644
index 0000000..76c18d7
--- /dev/null
+++ b/examples/ATLLArBarrel/hepemlib/src/G4EmStandardPhysics_G4HepEm.cc
@@ -0,0 +1,29 @@
+//**************************************************
+// \file G4EmStandardPhysics_G4HepEm.cc
+// \brief: implementation of G4EmStandardPhysics_G4HepEm
+//**************************************************
+
+#include "G4EmStandardPhysics_G4HepEm.hh"
+
+#include "G4Electron.hh"
+#include "G4Gamma.hh"
+#include "G4HepEmTrackingManager.hh"
+#include "G4Positron.hh"
+
+G4EmStandardPhysics_G4HepEm::G4EmStandardPhysics_G4HepEm(G4int ver, const G4String &name)
+    : G4EmStandardPhysics(ver, name)
+{
+}
+
+void G4EmStandardPhysics_G4HepEm::ConstructProcess()
+{
+  // Register the standard EM processes first (needed for the other particles
+  // and for the tables), then give e-, e+ and gamma to G4HepEm
+  //
+  G4EmStandardPhysics::ConstructProcess();
+
+  auto *trackingManager = new G4HepEmTrackingManager;
+  G4Electron::Definition()->SetTrackingManager(trackingManager);
+  G4Positron::Definition()->SetTrackingManager(trackingManager);
+  G4Gamma::Definition()->SetTrackingManager(trackingManager);
+}
```

Before it was added to the project, it applied with `git apply` in the root of AdePT_examples. It was regenerated on 2026-11-19 from the
current sources (after clang-format and the fixes of section 4.6), checked with clang-format-19, and
built with 0 warnings against Geant4 11.4.3 and G4HepEm `780e798` (EL9, gcc 13.3). With 150 electrons
of 50 GeV and 8 threads (`electron50GeV.mac`) it gives EnergyLArNoNoise = 9902.0 ± 10.9 MeV
(rms 133.5), compatible with FTFP_BERT on the same machine (`Results.md` 3b: 9913.8 ± 9.9 MeV,
−11.8 ± 14.7 MeV, 0.8σ), and the log confirms that `photonNuclear` and `electronNuclear` are kept as
extra G4HepEm processes. The earlier version of this patch, written against the unformatted sources,
had wrong hunk headers and did not apply.
