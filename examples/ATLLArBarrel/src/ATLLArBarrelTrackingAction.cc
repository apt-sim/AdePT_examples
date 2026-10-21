//**************************************************
// \file ATLLArBarrelTrackingAction.cc
// \brief: implementation of
//         ATLLArBarrelTrackingAction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 8 May 2023
//**************************************************

//Includers from project files
//
#include "ATLLArBarrelTrackingAction.hh"

//Includers from Geant4
//
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

ATLLArBarrelTrackingAction::ATLLArBarrelTrackingAction(ATLLArBarrelEventAction* EvtAction)
    : G4UserTrackingAction(),
      fEventAction(EvtAction) {}

ATLLArBarrelTrackingAction::~ATLLArBarrelTrackingAction(){}

void ATLLArBarrelTrackingAction::PreUserTrackingAction([[maybe_unused]] const G4Track* aTrack) {}

void ATLLArBarrelTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
    
    //Skip it if the primary particle is not a baryon or meson
    //or the track is not a primary track
    //
    //if(0!=aTrack->GetParentID())return;
    if(0!=aTrack->GetParentID() || aTrack->GetParticleDefinition()->GetParticleType()!="baryon"
       || aTrack->GetParticleDefinition()->GetParticleType()!="meson") return;

    //Check it the primary baryon/meson had a nuclear breakupi (hadron inelastic process)
    //
    if(aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType()==121) {//HadInElastic process
            //the table of processes subtypes can be printed out with
            // /run/particle/dumpOrderingParam
            //Also useful
            //aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->DumpInfo();
            fEventAction->SetHasHadronInteracted(true); 
    }

}

//**************************************************
