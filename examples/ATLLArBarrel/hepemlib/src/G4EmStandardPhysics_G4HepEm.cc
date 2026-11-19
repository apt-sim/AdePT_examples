//**************************************************
// \file G4EmStandardPhysics_G4HepEm.cc
// \brief: implementation of G4EmStandardPhysics_G4HepEm
//**************************************************

#include "G4EmStandardPhysics_G4HepEm.hh"

#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4HepEmConfig.hh"
#include "G4HepEmTrackingManager.hh"
#include "G4Positron.hh"

G4EmStandardPhysics_G4HepEm::G4EmStandardPhysics_G4HepEm(G4bool multipleStepsMSC, G4int ver, const G4String &name)
    : G4EmStandardPhysics(ver, name), fMultipleStepsMSC(multipleStepsMSC)
{
}

void G4EmStandardPhysics_G4HepEm::ConstructProcess()
{
  // Register the standard EM processes first (needed for the other particles
  // and for the tables), then give e-, e+ and gamma to G4HepEm
  //
  G4EmStandardPhysics::ConstructProcess();

  auto *trackingManager = new G4HepEmTrackingManager;
  trackingManager->GetConfig()->SetMultipleStepsInMSCWithTransportation(fMultipleStepsMSC);
  G4Electron::Definition()->SetTrackingManager(trackingManager);
  G4Positron::Definition()->SetTrackingManager(trackingManager);
  G4Gamma::Definition()->SetTrackingManager(trackingManager);
}
