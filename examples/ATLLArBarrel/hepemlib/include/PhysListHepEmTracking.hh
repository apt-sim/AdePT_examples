//**************************************************
// \file PhysListHepEmTracking.hh
// \brief: definition of PhysListHepEmTracking class
//         for G4HepEm usage
// \source: This class is taken from G4HepEm
//          example
// \adaptation: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 20 February 2023
//**************************************************

#ifndef PhysListHepEmTracking_h
#define PhysListHepEmTracking_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

class PhysListHepEmTracking : public G4VPhysicsConstructor {

  public: 
     PhysListHepEmTracking(const G4String& name = "HepEmTracking");
    ~PhysListHepEmTracking();

  public: 
    void ConstructParticle() override;

    void ConstructProcess() override;
};

#endif

//**************************************************
