//**************************************************
// \file PhysicsList.cc
// \brief: implementation of PhysicsList class
//         for G4HepEm usage
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 20 February 2023
//**************************************************

//Includers from hepemlib files
//
#include "PhysicsList.hh"
#include "PhysListHepEm.hh"
#include "PhysListHepEmTracking.hh"

//Includers from Geant4
//
#include "G4LossTableManager.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList(const G4String& G4HepEmType)
    : G4VModularPhysicsList(),
      fG4HepEmType(G4HepEmType) {
    
    //Set default cuts
    //
    G4LossTableManager::Instance();
    SetDefaultCutValue(0.7*mm);

    if (fG4HepEmType=="G4HepEmTracking") RegisterPhysics( new PhysListHepEmTracking );

    else if (fG4HepEmType=="G4HepEm") RegisterPhysics( new PhysListHepEm );

    else {
    
        G4ExceptionDescription msg;
        msg << "Wrong G4HepEM constructor type ";
        G4Exception("PhysicsList::PhysicsList()",
        "MyCode0004", FatalException, msg);
    }
}

PhysicsList::~PhysicsList(){}

//**************************************************
