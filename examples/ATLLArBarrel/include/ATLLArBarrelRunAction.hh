//**************************************************
// \file ATLLArBarrelRunAction.hh
// \brief: definition of ATLLArBarrelRunAction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 31 October 2022
//**************************************************

#ifndef ATLLArBarrelRunAction_h
#define ATLLArBarrelRunAction_h 1

//Includers from project files
//
#include "ATLLArBarrelEventAction.hh"

//Includers from Geant4
//
#include "G4UserRunAction.hh"
#include "G4Timer.hh"
#include "G4Run.hh"

class ATLLArBarrelRunAction : public G4UserRunAction {

    public:
        ATLLArBarrelRunAction(ATLLArBarrelEventAction* evtAction);
        ~ATLLArBarrelRunAction();
        
        //Methods from base class
        //
        void BeginOfRunAction(const G4Run*) override;
        void EndOfRunAction(const G4Run* aRun) override;

    private:
        G4Timer fTimer;
        ATLLArBarrelEventAction* fEventAction;
};

#endif //ATLLArBarrelRunAction_h 1

//**************************************************
