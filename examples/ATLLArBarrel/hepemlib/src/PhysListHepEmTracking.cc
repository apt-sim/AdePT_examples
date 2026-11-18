//**************************************************
// \file PhysListHepEmTracking.cc
// \brief: implementation of PhysListHepEmTracking
//         class for G4HepEm usage
// \source: This class is taken from G4HepEm
//          example
// \adaptation: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 20 February 2023
//**************************************************

//Includers from hepemlib files
//
#include "PhysListHepEmTracking.hh"

//Includers from G4HepEM
//
#include "G4HepEmTrackingManager.hh"

//Includers from Geant4
//
#include "G4EmParameters.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4ParticleDefinition.hh"
#include "G4Positron.hh"
#include "G4BosonConstructor.hh"  //particles
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

PhysListHepEmTracking::PhysListHepEmTracking(const G4String& name)
   :  G4VPhysicsConstructor(name) {

  G4EmParameters* param = G4EmParameters::Instance();
  param->SetDefaults();

  param->SetMscRangeFactor(0.04);
}

PhysListHepEmTracking::~PhysListHepEmTracking() {}

void PhysListHepEmTracking::ConstructParticle(){
    
    G4BosonConstructor  pBosonConstructor;
    pBosonConstructor.ConstructParticle();

    G4LeptonConstructor pLeptonConstructor;
    pLeptonConstructor.ConstructParticle();

    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();

    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();

    G4IonConstructor pIonConstructor;
    pIonConstructor.ConstructParticle();

    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();
}

void PhysListHepEmTracking::ConstructProcess() {

  // Register custom tracking manager for e-/e+ and gammas.
  auto* trackingManager = new G4HepEmTrackingManager;

  G4Electron::Definition()->SetTrackingManager(trackingManager);
  G4Positron::Definition()->SetTrackingManager(trackingManager);
  G4Gamma::Definition()->SetTrackingManager(trackingManager);
}

//**************************************************
