# Migrating ATLLArBarrel to AdePT (GPU transport of e⁻, e⁺ and γ)

This guide lists every step needed to take the **original CPU-only application** to a build that transports
electrons, positrons and photons on an NVIDIA GPU with [AdePT](https://github.com/apt-sim/AdePT).

- **Starting point:** the application as it was before AdePT, commit `fb8413e` ("Support recent G4HepEm
  versions without G4HepEmProcess").
- **Target:** AdePT 0.4.3, released 2026-11-17. AdePT 0.4.2 also works with the same code. It is the
  version validated on the GPU machine in `Results.md`, section 3.
- **What the current repository already has:** `master` (`a22047d`) contains a working minimal migration
  (commits `d5b2d67`, `2d4a569`, `3f257b6`, `76ae120`). Each step below says whether it is already applied
  (✅) or still to do (⚠️), so the guide can be followed either from the original code or from `master`.
- **How it was checked:** this guide was written on a Mac without an NVIDIA GPU, against the AdePT 0.4.3 and
  0.4.2 sources. The new code of steps 2 and 3 was only syntax-checked: against Geant4 11.4.3 and G4HepEm
  `20261106`, with stub AdePT headers that have the 0.4.3 signatures. It must be built and validated on the
  GPU machine (step 8).

See also `Review.md`, section 7, and `BUILD_OPTIONS.md`, "Running with AdePT".

---

## Contents

0. [How AdePT works in this application](#0-how-adept-works-in-this-application)
1. [Software stack on the GPU machine](#1-software-stack-on-the-gpu-machine)
2. [Step 1 – CMakeLists.txt](#step-1--cmakeliststxt)
3. [Step 2 – New EM physics constructor](#step-2--new-em-physics-constructor)
4. [Step 3 – main(): ATLLArBarrel.cc](#step-3--main-atllarbarrelcc)
5. [Step 4 – User actions and sensitive detector](#step-4--user-actions-and-sensitive-detector)
6. [Step 5 – Geometry](#step-5--geometry)
7. [Step 6 – Macro files](#step-6--macro-files)
8. [Step 7 – Build and run](#step-7--build-and-run)
9. [Step 8 – Validation](#step-8--validation)
10. [Pitfalls](#pitfalls)
11. [Limitations](#limitations)
12. [Summary of the files to change](#summary-of-the-files-to-change)

---

## 0. How AdePT works in this application

```
            G4 stack (CPU, one per worker thread)
                         │
        ┌────────────────┴──────────────────┐
   hadrons, μ, ν, ions...               e⁻, e⁺, γ
   standard Geant4 tracking         AdePTTrackingManager
   (FTFP_BERT, CPU)                         │
                              ┌─────────────┴─────────────┐
                          GPU region                  CPU region
                   AdePT transport on GPU        G4HepEmTrackingManager
                   (G4HepEm physics + VecGeom)        (CPU)
                              │
          steps in sensitive volumes copied back to the CPU
          → ATLLArBarrelSensDet::ProcessHits() (unchanged)
          secondaries of γ/e-nuclear reactions → back to the CPU stack
```

- AdePT is a `G4VTrackingManager` attached to e⁻, e⁺ and γ. Every other particle is tracked by Geant4 on
  the CPU as before.
- On the GPU the physics is **G4HepEm**: Geant4 EM option-0 models, compiled for CUDA. The geometry is
  **VecGeom**. G4VG converts it automatically from the Geant4 geometry built by
  `ATLLArBarrelConstruction`, so no GDML file is needed.
- Steps in sensitive volumes are copied back to the CPU, and the normal Geant4 sensitive detector is
  called with a reconstructed `G4Step`. The hits, the ntuple and the analysis therefore do not change.
- Physics-wise, the migration replaces the EM constructor `G4EmStandardPhysics` of the reference list (for
  example FTFP_BERT) with an AdePT one. Hadronic physics, decays, G4EmExtraPhysics (γ/e-nuclear) etc. are
  unchanged.

---

## 1. Software stack on the GPU machine

| Package | Version | Required build options |
|---|---|---|
| NVIDIA driver + CUDA toolkit | CUDA ≥ 12 (13.1 used in `Results.md`) | |
| CMake | **≥ 3.25.2** | |
| C++ compiler | C++20, supported by `nvcc` (gcc 13 used) | |
| Geant4 | > 11.0 (11.4.3 used), multithreaded | `GEANT4_USE_GDML=ON`, `GEANT4_USE_SYSTEM_EXPAT=OFF` (recommended), `GEANT4_USE_USOLIDS=OFF` |
| VecCore | ≥ 0.5.2 (0.8.2 used) | default build (header-only; the CUDA backend is selected by AdePT) |
| VecGeom | **≥ 2.2.1** for AdePT 0.4.3 (2.2.0 is enough for 0.4.2) | `VECGEOM_ENABLE_CUDA=ON`, `VECGEOM_GDML=ON`, `BACKEND=Scalar`, `VECGEOM_NAV=index` (8 % faster for this kind of geometry, `Results.md` section 2) |
| G4HepEm | **≥ tag `20261106`** for 0.4.3 (`780e798` was used with 0.4.2) | `G4HepEm_CUDA_BUILD=ON`, `G4HepEm_EARLY_TRACKING_EXIT=ON` |
| G4VG | 1.0.7 | fetched by AdePT if it is not installed |
| AdePT | 0.4.3 (or 0.4.2) | |

`CMAKE_CUDA_ARCHITECTURES` **must be the same** for VecGeom, G4HepEm, AdePT and ATLLArBarrel. Examples:
`75` for an RTX 2070 (the validation machine), `80` for an A100, `90` for an H100. `native` is only correct when
the code is built on the machine that will run it.

Example build of the stack. Paths are placeholders; the installation guide of AdePT, `docs/installation`, is the
reference.

```sh
export CUDAARCH=75
export PREFIX=$HOME/adept-stack
source <geant4-install>/bin/geant4.sh

# VecCore
cmake -S VecCore -B build-veccore -DCMAKE_INSTALL_PREFIX=$PREFIX
cmake --build build-veccore -j --target install

# VecGeom
cmake -S VecGeom -B build-vecgeom -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX \
      -DCMAKE_PREFIX_PATH=$PREFIX -DCMAKE_CUDA_ARCHITECTURES=$CUDAARCH \
      -DVECGEOM_ENABLE_CUDA=ON -DVECGEOM_GDML=ON -DBACKEND=Scalar -DVECGEOM_NAV=index
cmake --build build-vecgeom -j --target install

# G4HepEm (tag 20261106 or newer)
cmake -S g4hepem -B build-g4hepem -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX \
      -DCMAKE_PREFIX_PATH=$PREFIX -DCMAKE_CUDA_ARCHITECTURES=$CUDAARCH \
      -DG4HepEm_CUDA_BUILD=ON -DG4HepEm_EARLY_TRACKING_EXIT=ON
cmake --build build-g4hepem -j --target install

# AdePT
cmake -S AdePT -B build-adept -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$PREFIX \
      -DCMAKE_PREFIX_PATH=$PREFIX -DCMAKE_CUDA_ARCHITECTURES=$CUDAARCH
cmake --build build-adept -j --target install
```

Inside the CERN network you can skip these builds: the LCG view
`/cvmfs/sft.cern.ch/lcg/views/devAdePT/latest/x86_64-el9-gcc13-opt/setup.sh` provides the whole stack.

AdePT is in practice **Linux only**. `AdePTConfig.cmake` adds the GNU ld flag `-Wl,--disable-new-dtags`, and
CUDA does not exist on recent macOS. On the Mac, keep developing with the G4HepEm CPU build, which has the same
EM physics as the GPU (see step 8).

---

## Step 1 – CMakeLists.txt

### 1a. AdePT option ✅ (already in `master`)

Add this after the G4HepEm options and before `include(${Geant4_USE_FILE})`:

```cmake
#----------------------------------------------------------------------------
# Option to enable AdePT (GPU transport of e-, e+ and gamma)
# The EM physics of the reference physics list is replaced by
# an AdePT EM constructor, which already includes G4HepEm
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
```

- `CMAKE_CUDA_ARCHITECTURES` must be set **before** `enable_language(CUDA)`.
- `find_package(AdePT)` also finds VecCore, VecGeom, G4HepEm and G4VG through `AdePTConfig.cmake`. It stops if
  G4HepEm was built without CUDA, and it sets `AdePT_LIBRARIES` to `AdePT::G4Integration`, which carries the
  include directories of AdePT, G4HepEm and VecGeom.
- `WITH_G4HepEm` must stay off. AdePT brings its own G4HepEm, and the project `hepemlib` would register a
  second tracking manager for the same particles.

### 1b. Link with the CUDA-RDC helper ✅ (in `master`, add `PRIVATE` ⚠️)

```cmake
add_executable(ATLLArBarrel ATLLArBarrel.cc ${sources} ${headers})
if(WITH_G4HepEm)
    target_link_libraries(ATLLArBarrel ${Geant4_LIBRARIES} ${G4HepEm_LIBRARIES} hepemlib)
elseif(WITH_ADEPT)
    # AdePT-only sources (step 2), compiled only in this configuration
    target_sources(ATLLArBarrel PRIVATE ${PROJECT_SOURCE_DIR}/adept/src/ATLLArBarrelEmPhysics_AdePT.cc)
    target_include_directories(ATLLArBarrel PRIVATE ${PROJECT_SOURCE_DIR}/adept/include)
    # cuda_rdc_* (from VecGeom) keeps the CUDA device linking consistent with AdePT
    cuda_rdc_target_link_libraries(ATLLArBarrel PRIVATE ${Geant4_LIBRARIES} ${AdePT_LIBRARIES})
else()
    target_link_libraries(ATLLArBarrel ${Geant4_LIBRARIES} ${G4HepEm_LIBRARIES})
endif()
```

- `cuda_rdc_target_link_libraries` is **mandatory**. VecGeom and AdePT use relocatable device code. With a
  plain `target_link_libraries`, the device code is not linked consistently, and the executable fails at
  link time or at start-up. The function comes from VecGeom's `CudaRdcUtils.cmake`, which
  `find_package(AdePT)` loads.
- The new sources live in a separate `adept/` directory. The `file(GLOB ... src/*.cc)` of the project must not
  pick them up, because the CPU and G4HepEm builds have no AdePT headers.
- With the minimal migration (physics option A of step 2), only the `cuda_rdc_target_link_libraries` line is
  needed.

### 1c. CMake version ⚠️

The original `cmake_minimum_required(VERSION 3.8...3.18)` stays valid for the CPU builds. AdePT itself requires
3.25.2, and `CMAKE_CUDA_ARCHITECTURES=native` requires 3.24. Add an explicit check at the top of the `WITH_ADEPT`
block, so that an old CMake gives a clear message instead of an error deep inside `AdePTConfig.cmake`:

```cmake
if(WITH_ADEPT)
    if(CMAKE_VERSION VERSION_LESS 3.25.2)
        message(FATAL_ERROR "WITH_ADEPT requires CMake >= 3.25.2 (found ${CMAKE_VERSION})")
    endif()
    ...
```

A version check is better than a second `cmake_minimum_required()` inside the `if`, because it does not change
the policy settings of the rest of the project.

### 1d. Copy the new macro ⚠️

Add the macro of step 6 to the scripts copied to the build directory:

```cmake
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
```

### 1e. `WITH_GPS` default ✅

The original default was `WITH_GPS=ON`, but `run.mac` and the benchmarks use `/gun/` commands. `master`
changed the default to `OFF`. This is not specific to AdePT, which works with both generators.

---

## Step 2 – New EM physics constructor

There are two ways to plug AdePT into the reference physics list. Both give exactly the same physics:
G4EmStandardPhysics option 0 on the CPU, and G4HepEm (option-0 models) for e⁻/e⁺/γ on the CPU and GPU.

### Option A – AdePT's own constructor ✅ (what `master` does)

```cpp
#include <AdePT/g4integration/G4EmStandardPhysics_AdePT.hh>
...
physicsList->ReplacePhysics( new G4EmStandardPhysics_AdePT() );
```

This needs no new file, and it is "case 2" of the AdePT integration guide. With `-p FTFP_BERT` it is equivalent
to AdePT's `FTFP_BERT_AdePT`. All AdePT settings must come from `/adept/` macro commands, and the AdePT defaults
are **not** the ones this project needs: nothing is sent to the GPU, and energy-loss fluctuations are off. A
macro without the `/adept/` lines therefore runs silently on the CPU, or with different physics (pitfalls 1 and 2).

### Option B – project physics constructor ⚠️ (recommended)

This option follows "case 3" of the AdePT integration guide. A project-owned constructor sets the validated
defaults in code, so that a macro without `/adept/` lines gives the correct physics. The `/adept/` commands of
a macro, executed before `/run/initialize`, still override these defaults: the constructor runs in `main()`,
before any macro.

The same class is also the place for later customisations, e.g. Woodcock tracking regions, G4HepEm step-limit
parameters, or GPU-only regions.

**New file `adept/include/ATLLArBarrelEmPhysics_AdePT.hh`:**

```cpp
//**************************************************
// \file ATLLArBarrelEmPhysics_AdePT.hh
// \brief: EM physics constructor with AdePT GPU
//         transport of e-, e+ and gamma
//**************************************************

#ifndef ATLLArBarrelEmPhysics_AdePT_h
#define ATLLArBarrelEmPhysics_AdePT_h 1

//Includers from Geant4
//
#include "G4EmStandardPhysics.hh"

class AdePTConfiguration;
class AdePTTrackingManager;

//Same physics as G4EmStandardPhysics_AdePT (EM option 0, G4HepEm models
//on CPU and GPU), with the project defaults set in code. The /adept/
//commands of a macro, executed before /run/initialize, still override them.
//
class ATLLArBarrelEmPhysics_AdePT : public G4EmStandardPhysics {

    public:
        explicit ATLLArBarrelEmPhysics_AdePT(G4int ver = 1);
        ~ATLLArBarrelEmPhysics_AdePT() override;

        void ConstructProcess() override;

    private:
        AdePTConfiguration* fAdePTConfiguration;
        AdePTTrackingManager* fTrackingManager;

};

#endif //ATLLArBarrelEmPhysics_AdePT_h

//**************************************************
```

**New file `adept/src/ATLLArBarrelEmPhysics_AdePT.cc`:**

```cpp
//**************************************************
// \file ATLLArBarrelEmPhysics_AdePT.cc
// \brief: implementation of ATLLArBarrelEmPhysics_AdePT
//**************************************************

//Includers from project files
//
#include "ATLLArBarrelEmPhysics_AdePT.hh"

//Includers from AdePT and G4HepEm
//
#include <AdePT/g4integration/AdePTConfiguration.hh>
#include <AdePT/g4integration/AdePTTrackingManager.hh>
#include <G4HepEmConfig.hh>

//Includers from Geant4
//
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

ATLLArBarrelEmPhysics_AdePT::ATLLArBarrelEmPhysics_AdePT(G4int ver)
    : G4EmStandardPhysics(ver, "ATLLArBarrelEmPhysics_AdePT"),
    fAdePTConfiguration(new AdePTConfiguration()),
    fTrackingManager(nullptr){

    //Project defaults (validated in Results.md, section 3)
    //
    fAdePTConfiguration->SetTrackInAllRegions(true);      //otherwise nothing goes to the GPU
    fAdePTConfiguration->SetEnergyLossFluctuation(true);  //as G4EmStandardPhysics on the CPU
    fAdePTConfiguration->SetMillionsOfTrackSlots(1.);
    fAdePTConfiguration->SetMillionsOfHitSlots(1.);
    fAdePTConfiguration->SetCUDAStackLimit(8192);
}

ATLLArBarrelEmPhysics_AdePT::~ATLLArBarrelEmPhysics_AdePT(){
    delete fAdePTConfiguration;
    //the tracking manager is deleted by Geant4
}

void ATLLArBarrelEmPhysics_AdePT::ConstructProcess(){

    //Standard EM processes first (needed by the CPU part and by the tables)
    //
    G4EmStandardPhysics::ConstructProcess();

    //AdePT tracking manager for e-, e+ and gamma
    //
    fTrackingManager = new AdePTTrackingManager(fAdePTConfiguration, /*verbosity=*/0);

    //Pass the AdePT settings to G4HepEm, as G4EmStandardPhysics_AdePT does
    //
    auto g4hepemConfig = fTrackingManager->GetG4HepEmConfig();
    g4hepemConfig->SetMultipleStepsInMSCWithTransportation(
        fAdePTConfiguration->GetMultipleStepsInMSCWithTransportation());
    g4hepemConfig->SetEnergyLossFluctuation(fAdePTConfiguration->GetEnergyLossFluctuation());
    for (const auto& regionName : fAdePTConfiguration->GetWDTRegionNames()) {
        g4hepemConfig->SetWoodcockTrackingRegion(regionName);
    }
    g4hepemConfig->SetWDTEnergyLimit(fAdePTConfiguration->GetWDTKineticEnergyLimit());

    G4Electron::Definition()->SetTrackingManager(fTrackingManager);
    G4Positron::Definition()->SetTrackingManager(fTrackingManager);
    G4Gamma::Definition()->SetTrackingManager(fTrackingManager);
}

//**************************************************
```

Notes:

- `ConstructProcess()` follows `G4EmStandardPhysics_AdePT::ConstructProcess()` of AdePT 0.4.3 line by line. Only
  the defaults in the constructor are new. When moving to a newer AdePT, compare it with
  `src/g4integration/physics_constructors/G4EmStandardPhysics_AdePT.cc` and keep the two in sync.
- The class derives from `G4EmStandardPhysics`, so `ReplacePhysics()` recognises it as the EM constructor
  (`bElectromagnetic`) and removes the original one.
- Settings that must mirror an EM option go after `GetG4HepEmConfig()`. Examples are
  `SetEnergyLossStepLimitFunctionParameters`, `SetMSCRangeFactor`, and the tracking cuts in
  `GetG4HepEmParameters()`.
- **Do not** derive from `G4EmStandardPhysics_option3/4`, `_Livermore` or `_Penelope` to get "AdePT with option 4"
  etc. G4HepEm implements only the option-0 models (Urban MSC, Seltzer-Berger/relativistic bremsstrahlung,
  Møller/Bhabha, Klein-Nishina, Bethe-Heitler...). The e⁻/e⁺/γ would then use option-0 models on the GPU and on
  the CPU alike, and only the processes of other particles would follow option 4. This is why `main()` refuses
  these lists (step 3).

### CPU reference with identical code ⚠️ (recommended)

AdePT also installs `G4EmStandardPhysics_HepEm`: the same construction, with a `G4HepEmTrackingManager` and no
GPU. The project patch in `Review.md`, Appendix A (`WITH_G4HepEmEM`) builds the same thing without AdePT, and it
can be used on the Mac. Use one of the two as the CPU reference of every GPU validation. Both have the same EM
physics as the GPU and the full hadronic list, so any remaining difference comes from the GPU transport. The
original `WITH_G4HepEm` physics list has no γ/e-nuclear processes and is 0.4 % off at 50 GeV
(`Review.md` 6.3), so it is **not** a valid reference for AdePT.

---

## Step 3 – main(): `ATLLArBarrel.cc`

Changes relative to the original `main()`. Lines marked `+` are new; the guard and the default cut are still to
do in `master` ⚠️.

```cpp
 #ifdef USE_G4HepEm
 #include "PhysicsList.hh" //it is built as a separate library in hepem
 #endif

+#ifdef USE_ADEPT
+#include "ATLLArBarrelEmPhysics_AdePT.hh"          //option B (option A: <AdePT/g4integration/G4EmStandardPhysics_AdePT.hh>)
+#include "G4SystemOfUnits.hh"
+#endif
 ...
     //Physics List
     //
     #ifndef USE_G4HepEm //Use Geant4 reference physics lists
     G4cout<<"--> Using Geant4 reference physics list"<<G4endl;
     auto physListFactory = new G4PhysListFactory();
     auto physicsList = physListFactory->GetReferencePhysList( custom_pl );
+    #ifdef USE_ADEPT //transport e-, e+ and gamma on GPU with AdePT
+    //The AdePT EM constructor uses EM option 0 (the only one implemented in G4HepEm):
+    //refuse reference lists with another EM option (suffixes of G4PhysListFactory;
+    //"_EM0" is option 0 and is fine)
+    for (const G4String suffix : {"_EMV", "_EMX", "_EMY", "_EMZ", "_LIV", "_PEN", "__GS", "__SS", "_WVI", "__LE"}) {
+        if (G4StrUtil::ends_with(custom_pl, suffix)) {
+            G4ExceptionDescription msg;
+            msg << "Physics list " << custom_pl << " uses a non-default EM option, which "
+                << "the AdePT EM constructor would replace with option 0.";
+            G4Exception("main()", "ATLLArBarrel001", FatalException, msg);
+        }
+    }
+    G4cout<<"--> Using AdePT for e-, e+ and gamma"<<G4endl;
+    physicsList->ReplacePhysics( new ATLLArBarrelEmPhysics_AdePT() );
+    //Production cut used for the validation, applied before the GPU tables are built
+    //(a /run/setCut after /run/initialize does not reach the GPU, Results.md section 3c)
+    physicsList->SetDefaultCutValue( 0.01*mm );
+    #endif
     runManager->SetUserInitialization(physicsList);
     #endif
```

- The physics list must be complete **before** `runManager->SetUserInitialization(physicsList)`, as above.
  AdePT's tracking manager is created in `ConstructProcess()`, during `/run/initialize`.
- The guard was compiled and tested with Geant4 11.4.3 (`Review.md` 7.3). `FTFP_BERT`, `QGSP_BERT_HP`,
  `FTFP_BERT_EM0` and `Shielding_LEND` are accepted; `FTFP_BERT_EMZ` and `QGSP_BIC__GS` are refused.
  `G4StrUtil` comes with `G4String.hh`, and `G4Exception` is already available through `globals.hh`.
- `SetDefaultCutValue(0.01*mm)` makes the validated production cut the default of the AdePT build. A
  `/run/setCut` placed **before** `/run/initialize` still overrides it. It removes the most dangerous pitfall:
  +1.4 % energy in the LAr with the cut after initialisation. Without this line the default cut would be the
  FTFP_BERT 0.7 mm. If the CPU builds should have the same default, move the line out of `#ifdef USE_ADEPT`.
  This changes the original CPU behaviour for macros that do not set a cut.
- Nothing else in `main()` changes. `G4MTRunManager` is fine: AdePT is designed for multithreaded runs and
  shares one GPU between all worker threads. The thread count (`-t`) is one of the main performance knobs
  (step 8).
- Optional, already in `master`: return a non-zero exit code when the macro fails. This is useful for batch jobs
  on the GPU node.

---

## Step 4 – User actions and sensitive detector

AdePT calls the user code on the CPU with steps copied from the GPU (`GPUStep`, in
`AdePT/transport/steps/GPUStep.hh`). The rules for user code are in `docs/user/gpu_tracking.md` of AdePT. The
table below reviews every class of the original application against them.

| File | Needs a change? | Why |
|---|---|---|
| `src/ATLLArBarrelTrackingAction.cc` | **Yes, required** ✅ | see 4a |
| `src/ATLLArBarrelSensDet.cc`, `include/ATLLArBarrelSensDet.hh` | Recommended ✅ | see 4b |
| `src/ATLLArBarrelConstruction.cc` | No (but see step 5) | geometry converted automatically by G4VG; the SD is attached to `LAr::EMB::STAC` in `ConstructSDandField()`, once per worker as needed |
| `src/ATLLArBarrelEventAction.cc`, `RunAction.cc`, `Hit.cc` | No | they work on hits collections, filled in the same way; AdePT drains the GPU steps before `EndOfEventAction` |
| `src/ATLLArBarrelPrimaryGenerator*.cc` | No | primaries are created on the CPU; both G4ParticleGun and GPS work |
| `src/ATLLArBarrelActIni.cc` | No | |
| `hepemlib/` | No | not used with `WITH_ADEPT` |

### 4a. Primary identification (required) ✅

The original `PostUserTrackingAction` identified the primary with `GetParentID()==0`. By default AdePT does not
keep the GPU track data on the host. Hadrons from γ/e-nuclear reactions on GPU steps are then created with
parent ID 0, the dummy ID of the reconstructed GPU track, and they look like primaries. `HadInteraction` was set
in 135 out of 150 events of 50 GeV electrons with AdePT, against 0 on the CPU (`Results.md` 3d). The original
condition also had a logic error (`!= "baryon" || != "meson"` is always true), so the flag was never set.

Replace it with an identification that does not depend on track IDs. Also guard against a null process for
tracks that made no regular step:

```cpp
void ATLLArBarrelTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {

    //Do not use GetParentID()==0 to identify primaries: with AdePT, unless the track data are
    //kept on the host, secondaries of nuclear reactions on GPU steps have ParentID 0 as well
    //(see Results.md, section 3d). Only real primaries have a G4PrimaryParticle attached.
    //
    const G4String& ParticleType = aTrack->GetParticleDefinition()->GetParticleType();
    if(!aTrack->GetDynamicParticle()->GetPrimaryParticle() || (ParticleType!="baryon" && ParticleType!="meson")) return;

    //Check it the primary baryon/meson had a nuclear breakupi (hadron inelastic process)
    //The process can be null if the track did not perform a regular step
    //
    const G4VProcess* Process = aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep();
    if(Process && Process->GetProcessSubType()==121) {//HadInElastic process
            fEventAction->SetHasHadronInteracted(true);
    }
}
```

Alternatively, `/adept/returnFirstAndLastStep true` keeps real track and parent IDs. It costs little (20.5 s
against 20.4 s for 150 electrons), but it makes the code depend on a run-time setting. The code fix is preferred.

The tracking action only acts on primary hadrons, which are never transported on the GPU. It therefore does not
need `/adept/CallUserTrackingAction`. The application has no stepping action.

### 4b. Sensitive detector (recommended) ✅

`ATLLArBarrelSensDet::ProcessHits()` already uses only quantities that AdePT copies back:

| Used by ProcessHits | Copied from the GPU? |
|---|---|
| `GetTotalEnergyDeposit()` | yes |
| pre-step point position | yes |
| `GetStepLength()` (Birks law) | yes |
| touchable / volume of the pre-step point | yes (navigation state) |
| `GetNonIonizingEnergyDeposit()` | **no**, always 0 for GPU steps: do not start using it |

`Results.md` 3d checked that every step in the sensitive volume reaches the SD (95 million calls), with identical
energy per event.

The AdePT rule "no per-track or shared mutable state" required one change. The original `Initialize()` kept the
hits-collection IDs in function-`static` variables, which are shared by all worker threads. Make them members of
the SD, which exists once per thread:

```cpp
//ATLLArBarrelSensDet.hh, private members
G4int fFrontHCID;
G4int fMiddleHCID;
G4int fBackHCID;

//ATLLArBarrelSensDet.cc, Initialize()
if(fFrontHCID<0){ //prevent doing it every event -> HCIDs are identical for every event
    fFrontHCID  = G4SDManager::GetSDMpointer()->GetCollectionID(fFrontHitsCollectionName);
    fMiddleHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fMiddleHitsCollectionName);
    fBackHCID   = G4SDManager::GetSDMpointer()->GetCollectionID(fBackHitsCollectionName);
}
HCE->AddHitsCollection( fFrontHCID, fFrontHitsCollection );
HCE->AddHitsCollection( fMiddleHCID, fMiddleHitsCollection );
HCE->AddHitsCollection( fBackHCID, fBackHitsCollection );
```

Initialise the three members to `-1` in the constructor. The other SD fixes of commit `2d4a569` (`>=` in
`CheckHitID` and in the η/φ boundaries) are not AdePT-specific. They avoid out-of-range hit indices, which the
larger number of GPU steps makes more likely to show up.

Two more rules from the AdePT documentation for future changes to user code:

- Do not kill e⁻/e⁺/γ tracks from the SD or user actions. GPU tracks cannot be killed from the CPU.
- Steps arrive in a different order from CPU tracking: steps of different tracks are interleaved. Code must not
  assume that consecutive `ProcessHits` calls belong to the same track.

### 4c. Optional: noise-free energy columns ✅

Not required for AdePT, but very useful for its validation. `master` adds `EnergyLArNoNoise`, `EnergyLArNoBirks`
and `EventID` to the ntuple (`RunAction`, `EventAction`, `Hit`). The electronic noise (20 MeV per event) otherwise
hides differences of 0.1 %.

---

## Step 5 – Geometry

No code change is required: G4VG converts the Geant4 geometry to VecGeom at `/run/initialize`. The accordion, the
presampler and the cryostat use solids supported by VecGeom, and the conversion was validated on the GPU
(`Results.md` section 3).

Fixed (`Review.md` section 4.6) ✅. The original geometry is kept in `legacy/src` (`-DWITH_LEGACY_GEOMETRY=ON`):

- **Presampler `catho3`** overlapped its mother by 1.03 mm. The first cathode of the tilted module is now
  clipped with a `G4IntersectionSolid`. Check that G4VG converts this boolean solid when AdePT builds the
  VecGeom geometry.
- **Five `ThinAbs::Straight` sections** protruded 6–30 µm from `STAC` at the η = 1.475 cut. The thin
  absorber is now 0.2 mm shorter at that end.

VecGeom navigation is more sensitive to overlaps than Geant4. An overlap can produce stuck tracks or lost energy
only on the GPU. After the fixes, repeat the validation of step 8: the energy should not change.

`VECGEOM_NAV=index` is recommended for this geometry. It is a VecGeom build option, not an application one.

---

## Step 6 – Macro files

### Rules

1. **All `/adept/` commands go before `/run/initialize`.** Afterwards they are refused or ignored, because AdePT
   allocates the GPU memory and copies the geometry and the physics tables at initialisation.
2. **`/run/setCut` goes before `/run/initialize`.** This was the most important finding of the GPU validation.
   With the original order (`/run/initialize`, then `/run/setCut 0.01 mm`), the GPU kept the default cut, and
   AdePT deposited **+1.4 %** more energy in the LAr (`Results.md` 3c). With the cut first, AdePT agrees with the
   CPU to better than 0.2 %. The CPU builds give the same result with both orders, so changing the existing macros
   is harmless.
3. Without option B of step 2, every AdePT macro needs `/adept/setTrackInAllRegions true` (or `/adept/addGPURegion`)
   and `/adept/SetEnergyLossFluctuation true`. Otherwise nothing goes to the GPU, or the energy-loss fluctuations
   are off while FTFP_BERT has them on.
4. Commands that configure the Geant4 EM tracking of e⁻/e⁺/γ have **no effect** on these particles with AdePT,
   because G4HepEm/AdePT do their tracking. This applies to `/process/em/transportationWithMsc`
   (`runcards/run_msc.mac`), `/process/em/UseGeneralProcess` (`runcards/run_nogammageneral.mac`), and
   `/process/eLoss/fluct` and `/process/msc/*` for e±. The AdePT equivalents are
   `/adept/SetMultipleStepsInMSCWithTransportation` and `/adept/SetEnergyLossFluctuation`.

### New macro `run_adept.mac` ⚠️

This is the original `run.mac` (5 GeV γ, η = 0.2) converted to AdePT. Put it next to `run.mac` and add it to
`ATLLArBarrel_SCRIPT` (step 1d):

```
#AdePT version of run.mac, requires -DWITH_ADEPT=ON
#All /adept/ commands and /run/setCut must precede /run/initialize
#
/adept/setVerbosity 0
/adept/setMillionsOfTrackSlots 1
/adept/setMillionsOfHitSlots 1
/adept/setCUDAStackLimit 8192
/adept/setTrackInAllRegions true
/adept/SetEnergyLossFluctuation true
#/adept/setSeed 1234                  #optional, reproducible GPU random numbers
/run/setCut 0.01 mm
/run/initialize
/run/printProgress 10
/gun/particle gamma
/gun/energy 5 GeV
/gun/position 0. 0. 0. m

#eta 0.2 phi 0.01225 (0.0245/2)
/gun/direction 0.980254 0.0120087 0.197375

/run/beamOn 100
```

With option B the six `/adept/` lines repeat the defaults set in code. Keep them anyway, because they document
the run and stay valid with option A.

### Conversion of the existing macros

| Macro | Change for AdePT |
|---|---|
| `run.mac` | use `run_adept.mac` above (cut moved before init, `/adept/` block) |
| `runcards/benchmarks/electron50GeV.mac`, `gamma5GeV.mac` | ✅ `*_adept.mac` versions exist |
| `runcards/run.mac`, `single.mac`, `init.mac` | add the `/adept/` block before `/run/initialize`; no cut is set, so the default applies (0.01 mm with step 3, otherwise 0.7 mm) |
| `runcards/run_gps.mac` | add the `/adept/` block and move `/run/setCut 0.1 mm` before `/run/initialize`; needs `-DWITH_GPS=ON`. The π⁻ runs work as well: the hadronic shower stays on the CPU, and its EM component goes to the GPU |
| `runcards/run_msc.mac`, `run_nogammageneral.mac` | their EM commands have no effect on e⁻/e⁺/γ with AdePT (rule 4); do not use them for AdePT studies |
| `init_vis.mac`, `vis.mac`, `gui.mac` (interactive) | possible, but GPU tracks are not drawn as trajectories unless `/adept/returnAllSteps true` and `/adept/CallUserTrackingAction true` are set (slow); use the CPU build for visualisation |

### Useful run-time commands

| Command | Use |
|---|---|
| `/adept/setMillionsOfTrackSlots N`, `/adept/setMillionsOfHitSlots N` | GPU buffers. 1 + 1 was enough for 50 GeV e⁻ with 8 threads on 8 GB; increase for higher energies or more threads |
| `/adept/setCUDAStackLimit 8192` | needed by the VecGeom navigation |
| `/adept/addGPURegion R`, `/adept/removeGPURegion R` | send only some regions to the GPU |
| `/adept/setSeed N` | reproducible GPU random numbers |
| `/adept/returnFirstAndLastStep true`, `/adept/returnAllSteps true` | keep track IDs or all steps on the host (debugging, step 4a) |
| `/adept/CallUserTrackingAction`, `/adept/CallUserSteppingAction` | call user actions for GPU tracks (slower; not needed here) |
| `/adept/FinishLastNParticlesOnCPU N` | finish the tail of an event on the CPU (tuning) |
| `/adept/validateHandoff true` | new in 0.4.3: checks the tracks returned to Geant4 |
| `/adept/setVerbosity N` | AdePT printout |

---

## Step 7 – Build and run

```sh
source <geant4-install>/bin/geant4.sh
cmake -S ATLLArBarrel -B build-adept -DCMAKE_BUILD_TYPE=Release \
      -DGeant4_DIR=<geant4-install>/lib64/cmake/Geant4 \
      -DCMAKE_PREFIX_PATH="$HOME/adept-stack" \
      -DCMAKE_CUDA_ARCHITECTURES=75 \
      -DCMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc \
      -DWITH_ADEPT=ON -DWITH_GPS=OFF
cmake --build build-adept -j

cd build-adept
./ATLLArBarrel -m run_adept.mac -t 8
./ATLLArBarrel -m ../runcards/benchmarks/electron50GeV_adept.mac -t 8
```

Expected configuration output: `Project CMAKE option: building with USE_ADEPT (CUDA architectures: 75)`.

Expected run output: `--> Using AdePT for e-, e+ and gamma`, then the AdePT initialisation (geometry conversion,
GPU track and hit slots), and the G4HepEm configuration printout.

Performance notes (`Results.md` 3a–3b):

- The GPU pays off only with many tracks in flight. With 1 thread and 5 GeV γ, AdePT was 1.7× *slower* than the
  CPU. With 8 threads and 50 GeV e⁻ it was 1.4× faster. Use several threads and high-energy showers.
- On the validation machine the GPU also drives the desktop display. A dedicated GPU will give better and more
  stable timings.

---

## Step 8 – Validation

Repeat these checks after every change of AdePT, VecGeom, G4HepEm or geometry version.

| # | Check | Expected |
|---|---|---|
| 1 | Run output | `--> Using AdePT for e-, e+ and gamma`, GPU slots allocated |
| 2 | G4HepEm configuration printout | energy-loss fluctuations **on**, production cut 0.01 mm (energy thresholds in LAr and Pb match the CPU run) |
| 3 | `electron50GeV_adept.mac`, 8 threads, against the CPU | `EnergyLArNoNoise` within 0.2 % (`Results.md`: +0.15 ± 0.06 % against FTFP_BERT) |
| 4 | Same, against the G4HepEm CPU reference with identical EM physics (step 2) | compatible within statistics |
| 5 | `gamma5GeV_adept.mac` | compatible within statistics |
| 6 | `HadInteraction` | 0 for e⁻/γ beams; same rate as on the CPU for a π⁻ beam (253/300 at 20 GeV) |
| 7 | Steps per event and SD calls | close to the G4HepEm CPU reference (≈ 0.79 M steps per 50 GeV event) |
| 8 | Rerun with `/adept/setSeed` fixed | identical results |
| 9 | After the geometry fixes of step 5 | energy unchanged |

The analysis can use `estats.py` of the review (`~/atllarbarrel-review`, needs `uproot` and `numpy`) or the
macros in `analysis/`.

---

## Pitfalls

Ordered by impact, all observed in this project:

1. **`/run/setCut` after `/run/initialize`:** +1.4 % energy in the LAr on the GPU, and no warning. Fixed by the
   macro order and by `SetDefaultCutValue` (step 3).
2. **Missing `/adept/setTrackInAllRegions true`:** nothing runs on the GPU. The run looks normal, just slow.
   Fixed by option B (step 2).
3. **Energy-loss fluctuations off by default in AdePT:** different physics from the CPU lists. Fixed by option B or
   by `/adept/SetEnergyLossFluctuation true`.
4. **`ParentID == 0` for nuclear secondaries of GPU tracks:** wrong primary-based logic (`HadInteraction`). Fixed
   in step 4a.
5. **Plain `target_link_libraries`:** link or load failures of the CUDA device code. Use
   `cuda_rdc_target_link_libraries` (step 1b).
6. **Different `CMAKE_CUDA_ARCHITECTURES` in the stack:** "no kernel image available" or invalid device function
   at run time.
7. **Physics list with another EM option (`_EMZ`, `_LIV`...):** silently replaced by option 0. Fixed by the guard
   (step 3).
8. **Comparing with the original `WITH_G4HepEm` list:** a 0.4 % bias from the missing γ/e-nuclear processes,
   unrelated to the GPU. Use the matched reference (step 2).

---

## Limitations

- **EM physics:** only option-0 models (G4HepEm). Option 3/4, Livermore and Penelope cannot be run on the GPU.
- **Non-ionising energy deposit:** not copied back from the GPU (always 0).
- **Magnetic field:** none in this test-beam setup. A field would need AdePT built with `ADEPT_USE_EXT_BFIELD`, and
  a covfie field map (`/adept/setCovfieBfieldFile`).
- **Fast simulation / parameterisations:** not available on the GPU. Keep such regions on the CPU with
  `/adept/removeGPURegion`.
- **Visualisation of GPU tracks:** only with all steps returned and user tracking actions called (slow).
- **Platform:** Linux with an NVIDIA GPU. The macOS development machine can only build the CPU and G4HepEm
  configurations.

---

## Summary of the files to change

| File | Change | Status in `master` |
|---|---|---|
| `CMakeLists.txt` | `WITH_ADEPT` option, CUDA, `find_package(AdePT)`, `USE_ADEPT`, `cuda_rdc_target_link_libraries` | ✅ |
| `CMakeLists.txt` | CMake ≥ 3.25.2 check, `PRIVATE`, `adept/` sources, `run_adept.mac` copy | ⚠️ |
| `adept/include/ATLLArBarrelEmPhysics_AdePT.hh`, `adept/src/ATLLArBarrelEmPhysics_AdePT.cc` | new EM constructor with project defaults (option B) | ⚠️ new |
| `ATLLArBarrel.cc` | `#include`, `ReplacePhysics(...AdePT...)` | ✅ (option A) |
| `ATLLArBarrel.cc` | EM-option guard, `SetDefaultCutValue(0.01*mm)`, option B constructor | ⚠️ |
| `src/ATLLArBarrelTrackingAction.cc` | primaries via `GetPrimaryParticle()`, null-process check | ✅ |
| `src/ATLLArBarrelSensDet.cc`, `include/ATLLArBarrelSensDet.hh` | per-thread HC IDs (no statics), index bounds | ✅ |
| `src/ATLLArBarrelEventAction.cc`, `RunAction.cc`, `Hit.cc/.hh` | noise-free validation columns (optional) | ✅ |
| `src/ATLLArBarrelConstruction.cc` (and helpers) | fix `catho3` and ThinAbs overlaps | ✅ done (`src/LAr*Construction.cc`; original in `legacy/src`) |
| `run_adept.mac` | new macro (step 6) | ⚠️ new |
| `runcards/benchmarks/*_adept.mac` | AdePT benchmark macros, cut before init | ✅ |
| other `runcards/*.mac` | `/adept/` block and cut before init, when used with AdePT | ⚠️ |
| `hepemlib/` + `WITH_G4HepEmEM` | matched CPU reference (`Review.md` Appendix A) | ⚠️ |
| `README.md`, `BUILD_OPTIONS.md` | document `WITH_ADEPT`, macro rules, pitfalls 1–3 | ✅ partly; add option B and the cut default |
