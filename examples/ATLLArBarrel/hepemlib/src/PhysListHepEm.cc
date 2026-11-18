//**************************************************
// \file PhysListHepEm.cc
// \brief: implementation of PhysListHepEm class
//         for G4HepEm usage
// \source: This class is taken from G4HepEm
//          example
// \adaptation: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 20 February 2023
//**************************************************

//Includers from hepemlib files
//
#include "PhysListHepEm.hh"

// include the G4HepEmProcess from the G4HepEm lib.
//
#include "G4HepEmProcess.hh"

//Includers from Geant4
//
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4PhysicsListHelper.hh"
#include "G4EmParameters.hh"
#include "G4BuilderType.hh"
#include "G4SystemOfUnits.hh"
#include "G4BosonConstructor.hh"  //particles
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

PhysListHepEm::PhysListHepEm(const G4String& name)
    : G4VPhysicsConstructor(name) {

    G4EmParameters* param = G4EmParameters::Instance();
    param->SetDefaults();
    
    param->SetMscRangeFactor(0.04);

    SetPhysicsType(bElectromagnetic);
}

PhysListHepEm::~PhysListHepEm(){}

void PhysListHepEm::ConstructParticle(){
    
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

void PhysListHepEm::ConstructProcess(){

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // creae the only one G4HepEm process that will be assigned to e-/e+ and gamma
  G4HepEmProcess* hepEmProcess = new G4HepEmProcess();

  // Add standard EM Processes
  //
  auto aParticleIterator = GetParticleIterator();
  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {

      // Add G4HepEm process to gamma: includes Conversion, Compton and photoelectric effect.
      particle->GetProcessManager()->AddProcess(hepEmProcess, -1, -1, 1);

    } else if (particleName == "e-") {

      // Add G4HepEm process to e-: includes Ionisation, Bremsstrahlung, MSC for e-
     particle->GetProcessManager()->AddProcess(hepEmProcess, -1, -1, 1);

    } else if (particleName == "e+") {

      // Add G4HepEm process to e+: includes Ionisation, Bremsstrahlung, MSC and e+e-
      // annihilation into 2 gamma interactions for e+
      particle->GetProcessManager()->AddProcess(hepEmProcess, -1, -1, 1);

    }
  }

}

//**************************************************
