//**************************************************
// \file LArTBCryostatConstruction.hh
// \brief: definition of 
//         LArTBCryostatConstruction class
// \author: originally from ATLAS-G4
//          adaptation by 
//          Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 8 August 2022
//**************************************************

#ifndef LArTBCryostatConstruction_h
#define LArTBCruostatConstruction_h 1

//Includers from Geant4
//
#include "G4LogicalVolume.hh"

class LArTBCryostatConstruction{

    public:
        LArTBCryostatConstruction();
        ~LArTBCryostatConstruction();

        //Get the envelope containing this detector
        //
        G4LogicalVolume* GetEnvelope();

};

#endif //LArTBCryostatConstruction_h 1

//**************************************************
