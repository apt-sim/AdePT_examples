// SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
// SPDX-License-Identifier: MIT

//**************************************************
// \file ATLLArBarrelTrackingAction.cc
// \brief: implementation of
//         ATLLArBarrelTrackingAction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 8 May 2023
//**************************************************

// Includers from project files
//
#include "ATLLArBarrelTrackingAction.hh"

// Includers from Geant4
//
#include "G4ParticleDefinition.hh"
#include "G4DynamicParticle.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"

ATLLArBarrelTrackingAction::ATLLArBarrelTrackingAction(ATLLArBarrelEventAction *EvtAction)
    : G4UserTrackingAction(), fEventAction(EvtAction)
{
}

ATLLArBarrelTrackingAction::~ATLLArBarrelTrackingAction() {}

void ATLLArBarrelTrackingAction::PreUserTrackingAction([[maybe_unused]] const G4Track *aTrack) {}

void ATLLArBarrelTrackingAction::PostUserTrackingAction(const G4Track *aTrack)
{

  // Skip it if the primary particle is not a baryon or meson
  // or the track is not a primary track
  //
  // Do not use GetParentID()==0 to identify primaries: with AdePT, unless the track data are
  // kept on the host, secondaries of nuclear reactions on GPU steps have ParentID 0 as well
  //(see Results.md, section 3d). Only real primaries have a G4PrimaryParticle attached.
  //
  const G4String &ParticleType = aTrack->GetParticleDefinition()->GetParticleType();
  if (!aTrack->GetDynamicParticle()->GetPrimaryParticle() || (ParticleType != "baryon" && ParticleType != "meson"))
    return;

  // Check it the primary baryon/meson had a nuclear breakupi (hadron inelastic process)
  // The process can be null if the track did not perform a regular step
  //
  const G4VProcess *Process = aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep();
  if (Process && Process->GetProcessSubType() == 121) { // HadInElastic process
    // the table of processes subtypes can be printed out with
    //  /run/particle/dumpOrderingParam
    // Also useful
    // aTrack->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->DumpInfo();
    fEventAction->SetHasHadronInteracted(true);
  }
}

//**************************************************
