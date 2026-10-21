//**************************************************
// \file PhysicsList.hh
// \brief: definition of PhysicsList class
//         for G4HepEm usage
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 20 February 2023
//**************************************************

//This class is used to handle G4HepEm usage
//in ATLLarBarrel.
//
#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class PhysicsListMessenger;
class G4VPhysicsConstructor;

class PhysicsList: public G4VModularPhysicsList {

    public:

        PhysicsList(const G4String& G4HepEmType);
        ~PhysicsList();
    
    private:
        G4String fG4HepEmType;

};

#endif

//**************************************************
