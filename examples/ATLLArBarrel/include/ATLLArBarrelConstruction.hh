//**************************************************
// \file ATLLArBarrelConstruction.hh
// \brief: definition of ATLLArBarrelConstruction 
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 3 August 2022
//**************************************************

#ifndef ATLLArBarrelConstruction_h
#define ATLLArBarrelConstruction_h 1

//Includers from Geant4
//
#include "G4VUserDetectorConstruction.hh"

class ATLLArBarrelConstruction : public G4VUserDetectorConstruction {

    public:
        ATLLArBarrelConstruction();
        virtual ~ATLLArBarrelConstruction();

        virtual G4VPhysicalVolume* Construct();
        virtual void ConstructSDandField();
    
    private:
        G4bool fDumpGDML;

};

#endif //ATLLArBarrelConstruction_h 1

//**************************************************
