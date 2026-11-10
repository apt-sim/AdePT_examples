# Results

Performance and validation comparisons of ATLLArBarrel with different EM transport
options (Geant4 CPU, G4HepEm, AdePT on GPU), plus geometry and material checks.
Measurements taken in October–November 2026. Build options are described in
[BUILD_OPTIONS.md](BUILD_OPTIONS.md).

## Environment

| Component | Version / setting |
|---|---|
| Host | EL9, 24 cores, glibc 2.34, gcc 13.3 (gcc-toolset-13) |
| GPU | NVIDIA GeForce RTX 2070 SUPER (compute capability 7.5, 8 GB), driver 590.48, also driving the desktop display |
| CUDA | 13.1 |
| Geant4 | 11.4.3, Release, multithreaded, builtin expat |
| G4HepEm | commit `780e798` (tracking-manager mode only), built with `G4HepEm_CUDA_BUILD=ON` and `G4HepEm_EARLY_TRACKING_EXIT=ON` |
| VecCore / VecGeom | 0.8.2 / 2.2.0 (CUDA, GDML, scalar backend, `VECGEOM_NAV=index` unless stated) |
| AdePT | 0.4.2 |
| GeoModel | 6.33.0 (section 4 only) |

All runs use Release builds with `-DWITH_GPS=OFF` and the macros in `runcards/benchmarks/`:
`/run/setCut 0.01 mm`, gun at the origin, direction eta = 0.2, phi = 0.01225.
Times are the event-loop times printed by Geant4 at the end of the run (initialisation excluded).
`EnergyLAr` is the total energy per event in the LAr sensitive detector (Birks-corrected,
including electronic noise), in MeV. Errors on means are standard errors of the mean.
The ntuple now also has `EnergyLArNoNoise` (Birks-corrected, no noise), `EnergyLArNoBirks`
(no Birks correction, no noise) and `EventID`. They were added after the measurements below and are
not used in them.

## How to reproduce

Build the three configurations in separate directories, as described in [BUILD_OPTIONS.md](BUILD_OPTIONS.md):
`build-cpu` (default), `build-hepem` (`-DWITH_G4HepEm=ON`) and `build-adept` (`-DWITH_ADEPT=ON`).
Run from the build directory, after sourcing `geant4.sh`:

| Section | Command |
|---|---|
| 1a | `./ATLLArBarrel -m ../runcards/benchmarks/gamma5GeV.mac -t 1` (in `build-cpu` and `build-hepem`) |
| 1b | `./ATLLArBarrel -m ../runcards/benchmarks/electron50GeV.mac -t 1` (in `build-cpu` and `build-hepem`) |
| 3a | `build-cpu`: `gamma5GeV.mac -t 1`; `build-adept`: `gamma5GeV_adept.mac -t 1` |
| 3b | `build-cpu`: `electron50GeV.mac -t 8`; `build-adept`: `electron50GeV_adept.mac -t 8` |
| 4 | `build-cpu`: `./ATLLArBarrel -m ../runcards/benchmarks/geometry_test.mac -t 1` |

Section 2 uses AdePT's own `example1` (`example1.mac`) built with `VECGEOM_NAV=tuple` and `index`.
The energy statistics are computed from the `EnergyLAr` branch of `ATLLArBarrelout_Run0.root`.
With the default (fixed) seeds, repeated runs give bit-identical energy distributions, both on CPU
and with AdePT.

## 1. G4HepEm vs Geant4 FTFP_BERT (CPU)

- **FTFP_BERT**: Geant4 reference physics list (EM + hadronic).
- **G4HepEm**: `-DWITH_G4HepEm=ON`, i.e. `PhysListHepEmTracking` from `hepemlib/`: only the
  `G4HepEmTrackingManager` for e-, e+ and gamma (no hadronic physics), `MscRangeFactor = 0.04`.

### 1a. 5 GeV gamma, 300 events, 1 thread, 3 sequential repetitions

| Physics | Time per event | EnergyLAr mean (MeV) |
|---|---|---|
| FTFP_BERT | 136.7 ± 0.2 ms | 977.2 ± 10.1 (rms 175) |
| G4HepEm | 95.0 ± 2.0 ms | 964.3 ± 10.6 (rms 184) |

Speed-up **1.44** (−30.5 % time per event). The deposited energies are compatible (0.9 sigma).

### 1b. 50 GeV electrons, 150 events, 1 thread, 3 repetitions run concurrently

| Physics | Time per event | EnergyLAr mean (MeV) |
|---|---|---|
| FTFP_BERT | 1407 ± 2 ms | 9912.7 ± 15.7 (rms 192) |
| G4HepEm | 975 ± 1 ms | 9938.4 ± 17.7 (rms 216) |

Speed-up **1.44** (−30.7 %). The deposited energies are compatible (1.1 sigma).
The speed-up is the same as at 5 GeV, because the time is dominated by EM shower tracking.

## 2. VecGeom navigation state: `tuple` vs `index`

AdePT `example1` with its default macro: CMS 2018 geometry, 1 event with 200 electrons of 10 GeV,
transported on the GPU. VecGeom recommends `VECGEOM_NAV=index` for this geometry (navigation table
about 264 MB, below its 512 MB threshold).

| `VECGEOM_NAV` | Run time | Total deposited energy |
|---|---|---|
| `tuple` (default) | 2.60 s (1 run) | 899.66 GeV |
| `index` | 2.42 s, 2.39 s (2 runs) | 899.41 GeV (identical in both runs) |

`index` is about **8 % faster**. The small energy difference comes from a different random sequence.

## 3. AdePT (GPU) vs Geant4 FTFP_BERT (CPU)

- **AdePT**: `-DWITH_ADEPT=ON`: FTFP_BERT with the EM constructor replaced by
  `G4EmStandardPhysics_AdePT`, equivalent to AdePT's `FTFP_BERT_AdePT`. e-, e+ and gamma are
  transported on the GPU in all regions (1 M track slots, 1 M hit slots). GPU steps are returned
  to the CPU and processed by the unchanged ATLLArBarrel sensitive detector.
- **CPU**: the default build, FTFP_BERT.

**The physics settings are not matched.** These measurements used the AdePT default of energy-loss
fluctuations off, while FTFP_BERT on the CPU has them on, so neither the energies nor the speed-up
compare the same physics. The `*_adept.mac` macros now set `/adept/SetEnergyLossFluctuation true`,
so running them no longer reproduces the AdePT numbers of this section.

### 3a. 5 GeV gamma, 300 events, 1 thread

| Setup | Time per event | CPU time / wall time | EnergyLAr mean (MeV) |
|---|---|---|---|
| CPU FTFP_BERT | 136.7 ± 0.2 ms (section 1a) | ~100 % | 977.2 ± 10.1 (rms 175) |
| AdePT | 142 ± 3 ms (3 runs) | ~26 % | 979.7 ± 9.9 (rms 171) |

There is no gain. With a single CPU thread and one 5 GeV shower at a time the GPU is underused,
and the CPU thread waits for it about 75 % of the time. A later CPU re-run gave 141.8 ms per event,
so run-to-run variation on this machine is about 4 %. The mean energies agree within the
statistical precision of this sample (±1.4 %).

### 3b. 50 GeV electrons, 150 events, 8 threads, 2 repetitions

| Setup | Wall time | Throughput (time per event) |
|---|---|---|
| CPU FTFP_BERT | 28.99 s, 28.46 s | 191.5 ± 2.5 ms |
| AdePT | 12.87 s, 12.19 s | 83.5 ± 3.2 ms |

AdePT is **2.3x faster** in throughput. An earlier run with 160 events gave 183 ms (CPU) and
81 ms (AdePT), the same ratio.

| EnergyLAr (MeV) | Mean | Median | rms | Min |
|---|---|---|---|---|
| CPU FTFP_BERT | 9912.7 ± 15.7 | 9925.2 | 192 | 9308.5 |
| AdePT | 9990.4 ± 35.6 | 10028.7 | 436 | 5386.7 |
| AdePT, without the event below 8 GeV | 10021.3 ± 17.9 | – | 219 | – |

**Open issue: the AdePT physics is not validated yet.** Two effects reproduce exactly in every run:
1. One event deposits only 5387 MeV, about half the typical energy. No CPU event is below 9300 MeV.
2. Without that event, AdePT deposits **+109 ± 24 MeV (+1.1 %, 4.6 sigma)** more than the CPU,
   with a similar width.

No warning about full buffers or dropped steps is printed. The 5 GeV comparison (3a) is not
precise enough to exclude a 1 % shift, so the effect is not shown to depend on the number of
threads or on the energy. For comparison, G4HepEm alone differs from FTFP_BERT by only
+0.3 % ± 0.2 % (section 1b). The AdePT timings should not be used for physics conclusions
until this is understood.

## 4. Geometry overlap checks

Tolerance 10 um, 200 random points per volume.

| Geometry checked | Tool | Overlaps |
|---|---|---|
| `GDMLs/experimentalHall_v0p1.gdml` converted with GeoModel `gdml2gm` | `gmclash -t 0.01 -n 200` | 3749 |
| `GDMLs/experimentalHall_v0p1.gdml` (original) | `gmclash -t 0.01 -n 200` | 1 |
| Current geometry (17808 volumes) | `geometry_test.mac` (Geant4 `/geometry/test/run`) | 1 (same volume) |

- 3748 of the 3749 overlaps in the converted geometry are artefacts of the GDML -> GeoModel
  conversion, all in the EMB accordion. Straight sections are reported about 1.56 m outside their
  mother `STAC`, and folds overlap straight sections by a constant 2.2 mm (absorbers) and 0.27 mm
  (electrodes). The converted geometry should not be used.
- The single real overlap: the first cathode `catho3` of presampler module `mod[1]` (electrodes
  tilted by −12°) protrudes by about **1.0 mm** from `LAr::Barrel::Presampler::Module`.
  The tilted cathode spans ±1.47 mm in the stacking direction, but its centre is only 0.44 mm from
  the module edge (margin `mod[1][5] = 0.3 mm` + half thickness + `epsil`). It is not fixed yet:
  the parameters come from the original ATLAS code.

## 5. Material definitions

The material table (`/material/g4/printMaterial all`) was compared before and after the
introduction of `MaterialManager` (NIST elements, no duplicated materials):

- All densities are unchanged. Duplicate-material warnings went from 8 to 0.
- Radiation lengths changed by less than 0.15 % because of the NIST elements (largest: `Cables`,
  5.825 -> 5.817 cm).

Two pre-existing bugs were then fixed. They change the physics, and all the measurements above
were taken after these fixes:

| Material | Before | After |
|---|---|---|
| `Gten` (unit `g/cm` instead of `g/cm3`) | 180 g/cm3, X0 = 2.2 mm | 1.8 g/cm3, X0 = 22.3 cm |
| `SiO2` (uses the Gten density) | 180 g/cm3 | 1.8 g/cm3 |
| `MBoards` (mixture with Gten) | 155 g/cm3, X0 = 2.5 mm | 2.85 g/cm3, X0 = 7.1 cm |
| `Cables` (mass fractions 62 % + 39 % = 101 %) | 3.003 g/cm3 | Kapton 38 %: 3.035 g/cm3 |

## Caveats

- In the runs above `HadInteraction` is always 0 because of a bug in `ATLLArBarrelTrackingAction`,
  fixed after these measurements: the condition `type != "baryon" || type != "meson"` was always
  true. These runs cannot be used to show that hadronic
  interactions are negligible. For the e-/gamma beams used here they are expected to be small,
  but this is not demonstrated. Even after the fix the flag only records inelastic interactions of
  a primary baryon or meson: it stays 0 by construction for e-/gamma beams and does not detect
  photonuclear or electronuclear interactions.
- Repetitions use the same random seed, so the timing errors reflect machine noise only, not
  event-to-event variation.
- The G4HepEm comparisons involve an EM-only physics list and a different MSC range factor (0.04),
  so they are not a matched control for the AdePT comparison of section 3.
- The GPU is a consumer card with low FP64 throughput (AdePT computes in double precision), and it
  also drives the desktop display. GPU results are not representative of data-centre GPUs.
