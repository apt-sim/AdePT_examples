//**************************************************
// \file ATLLArBarrelEmPhysics_AdePT.cc
// \brief: implementation of ATLLArBarrelEmPhysics_AdePT
//**************************************************

// Includers from project files
//
#include "ATLLArBarrelEmPhysics_AdePT.hh"

// Includers from AdePT and G4HepEm
//
#include <AdePT/g4integration/AdePTConfiguration.hh>
#include <AdePT/g4integration/AdePTTrackingManager.hh>
#include <G4HepEmConfig.hh>

// Includers from Geant4
//
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"

ATLLArBarrelEmPhysics_AdePT::ATLLArBarrelEmPhysics_AdePT(G4int ver)
    : G4EmStandardPhysics(ver, "ATLLArBarrelEmPhysics_AdePT"), fAdePTConfiguration(new AdePTConfiguration()),
      fTrackingManager(nullptr)
{
  // Project defaults (validated in Results.md, section 3)
  //
  fAdePTConfiguration->SetTrackInAllRegions(true);     // otherwise nothing goes to the GPU
  fAdePTConfiguration->SetEnergyLossFluctuation(true); // as G4EmStandardPhysics on the CPU
  fAdePTConfiguration->SetMillionsOfTrackSlots(1.);
  fAdePTConfiguration->SetMillionsOfHitSlots(1.);
  fAdePTConfiguration->SetCUDAStackLimit(8192);
}

ATLLArBarrelEmPhysics_AdePT::~ATLLArBarrelEmPhysics_AdePT()
{
  delete fAdePTConfiguration;
  // the tracking manager is deleted by Geant4
}

void ATLLArBarrelEmPhysics_AdePT::ConstructProcess()
{
  // Standard EM processes first (needed by the CPU part and by the tables)
  // Same as G4EmStandardPhysics_AdePT::ConstructProcess() of AdePT 0.4.2:
  // keep the two in sync when moving to a newer AdePT
  //
  G4EmStandardPhysics::ConstructProcess();

  // AdePT tracking manager for e-, e+ and gamma
  //
  fTrackingManager = new AdePTTrackingManager(fAdePTConfiguration, /*verbosity=*/0);

  // Pass the AdePT settings to G4HepEm, as G4EmStandardPhysics_AdePT does
  //
  auto g4hepemConfig = fTrackingManager->GetG4HepEmConfig();
  g4hepemConfig->SetMultipleStepsInMSCWithTransportation(
      fAdePTConfiguration->GetMultipleStepsInMSCWithTransportation());
  g4hepemConfig->SetEnergyLossFluctuation(fAdePTConfiguration->GetEnergyLossFluctuation());
  for (const auto &regionName : fAdePTConfiguration->GetWDTRegionNames()) {
    g4hepemConfig->SetWoodcockTrackingRegion(regionName);
  }
  g4hepemConfig->SetWDTEnergyLimit(fAdePTConfiguration->GetWDTKineticEnergyLimit());

  G4Electron::Definition()->SetTrackingManager(fTrackingManager);
  G4Positron::Definition()->SetTrackingManager(fTrackingManager);
  G4Gamma::Definition()->SetTrackingManager(fTrackingManager);
}

//**************************************************
