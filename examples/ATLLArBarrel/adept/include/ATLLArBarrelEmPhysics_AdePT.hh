//**************************************************
// \file ATLLArBarrelEmPhysics_AdePT.hh
// \brief: EM physics constructor with AdePT GPU
//         transport of e-, e+ and gamma
//**************************************************

#ifndef ATLLArBarrelEmPhysics_AdePT_h
#define ATLLArBarrelEmPhysics_AdePT_h 1

// Includers from Geant4
//
#include "G4EmStandardPhysics.hh"

class AdePTConfiguration;
class AdePTTrackingManager;

// Same physics as G4EmStandardPhysics_AdePT (EM option 0, G4HepEm models
// on CPU and GPU), with the project defaults set in code. The /adept/
// commands of a macro, executed before /run/initialize, still override them.
//
class ATLLArBarrelEmPhysics_AdePT : public G4EmStandardPhysics {

public:
  explicit ATLLArBarrelEmPhysics_AdePT(G4int ver = 1);
  ~ATLLArBarrelEmPhysics_AdePT() override;

  void ConstructProcess() override;

private:
  AdePTConfiguration *fAdePTConfiguration;
  AdePTTrackingManager *fTrackingManager;
};

#endif // ATLLArBarrelEmPhysics_AdePT_h

//**************************************************
