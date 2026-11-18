//**************************************************
// \file ATLLArBarrelActIni.hh
// \brief: definition of ATLLArBarrelActIni class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 8 August 2022
//**************************************************

#ifndef ATLLArBarrelActIni_hh
#define ATLLArBarrelActIni_hh 1

//Includers from Geant4
//
#include "G4VUserActionInitialization.hh"

class ATLLArBarrelActIni : public G4VUserActionInitialization {
    
    public:
        ATLLArBarrelActIni( );
        virtual ~ATLLArBarrelActIni();
        
        virtual void BuildForMaster() const;
        virtual void Build() const;

};

#endif //ATLLArBarrelActIni_hh 1

//**************************************************

