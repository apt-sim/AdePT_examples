//**************************************************
// \file ATLLArBarrelActIni.cc
// \brief: implementation of ATLLArBarrelActIni class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 8 August 2022
//**************************************************

//Includers from project files
//
#include "ATLLArBarrelActIni.hh"
#include "ATLLArBarrelPriGenAct.hh"
#include "ATLLArBarrelRunAction.hh"
#include "ATLLArBarrelEventAction.hh"
#include "ATLLArBarrelTrackingAction.hh"

ATLLArBarrelActIni::ATLLArBarrelActIni( )
    : G4VUserActionInitialization(){}

ATLLArBarrelActIni::~ATLLArBarrelActIni() {}

//Define Build() and BuildForMaster() methods
//
void ATLLArBarrelActIni::BuildForMaster() const {

    //From the base class comments:"The user should not use
    // this method to instantiate user action classes except for user
    // run action."
    //
    ATLLArBarrelEventAction* EvtAction = new ATLLArBarrelEventAction();
    SetUserAction(new ATLLArBarrelRunAction(EvtAction));
}

void ATLLArBarrelActIni::Build() const {
    ATLLArBarrelEventAction* EventAction = new ATLLArBarrelEventAction();
    SetUserAction( new ATLLArBarrelPriGenAct );
    SetUserAction( new ATLLArBarrelRunAction(EventAction));
    SetUserAction( new ATLLArBarrelTrackingAction(EventAction) );
    SetUserAction( EventAction );
}

//**************************************************
