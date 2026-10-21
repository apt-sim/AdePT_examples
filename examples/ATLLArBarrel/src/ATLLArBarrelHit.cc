//**************************************************
// \file ATLLArBarrelHit.cc
// \brief: implementation of ATLLArBarrelHit 
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 7 May 2022
//**************************************************

//Includers from project files
//
#include "ATLLArBarrelHit.hh"

//Constructor
//
ATLLArBarrelHit::ATLLArBarrelHit()
    : G4VHit(),
    fEdep(0.),
    fHitEta(0.),
    fHitPhi(0.),
    fPositionAllocated(false){}   

//Destructor
//
ATLLArBarrelHit::~ATLLArBarrelHit(){}

//**************************************************
