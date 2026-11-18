//**************************************************
// \file PhysListHepEm.hh
// \brief: definition of PhysListHepEm class
//         for G4HepEm usage
// \source: This class is taken from G4HepEm
//          example
// \adaptation: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 20 February 2023
//**************************************************

#ifndef PhysListHepEm_h
#define PhysListHepEm_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"


class PhysListHepEm : public G4VPhysicsConstructor {
  public:
     PhysListHepEm (const G4String& name = "G4HepEm-physics-list");
    ~PhysListHepEm();

  public:
    void ConstructParticle() override;

    // This method will be invoked in the Construct() method.
    // each physics process will be instantiated and
    // registered to the process manager of each particle type
    void ConstructProcess() override;
};

#endif

//**************************************************
