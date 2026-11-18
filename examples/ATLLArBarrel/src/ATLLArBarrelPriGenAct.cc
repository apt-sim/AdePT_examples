//**************************************************
// \file ATLLArBarrelPriGenAct.cc
// \brief: implementation of ATLLArBarrelPriGenAct
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 8 August 2022
//**************************************************

//Includers from project files
//
#include "ATLLArBarrelPriGenAct.hh"

//Includers from Geant4
//
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

ATLLArBarrelPriGenAct::ATLLArBarrelPriGenAct()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun( nullptr ) {
    
      #ifdef USEGPS    
      fParticleGun = new G4GeneralParticleSource();
      auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle( "e-" );
      fParticleGun->SetParticleDefinition( particleDefinition );
      fParticleGun->SetParticlePosition( G4ThreeVector( 0.,0.,0.*m ) );
      #else
      fParticleGun = new G4ParticleGun(1); //set primary particle(s) to 1
      //default particle gun parameters (can be changed via UI)
      //
      auto particleDefinition = G4ParticleTable::GetParticleTable()->FindParticle( "e-" );
      fParticleGun->SetParticleDefinition( particleDefinition );
      //fParticleGun->SetParticleEnergy( 1.*GeV );
      //Set default primary particle position to origin (0,0,0) and
      //with atlas coordinates to eta=0.4 and ph=0.
      //
      //fParticleGun->SetParticleMomentumDirection( G4ThreeVector( 0.925007,0.,0.379949 ) );
      fParticleGun->SetParticlePosition( G4ThreeVector( 0.,0.,0.*m ) );
      #endif
}

ATLLArBarrelPriGenAct::~ATLLArBarrelPriGenAct() {
    delete fParticleGun;
}

void ATLLArBarrelPriGenAct::GeneratePrimaries( G4Event* event){
    fParticleGun->GeneratePrimaryVertex( event );
}

//**************************************************

