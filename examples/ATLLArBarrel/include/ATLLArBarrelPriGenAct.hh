//**************************************************
// \file ATLLArBarrelPriGenAct.hh
// \brief: definition of ATLLArBarrelPriGenAct class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 8 August 2022
//**************************************************

#ifndef ATLLArBarrel_h
#define ATLLArBarrel_h 1

//Includers from Geant4
//
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

//macros
//
#define USEGPS

class G4ParticleGun;
#ifdef USEGPS
class G4GeneralParticleSource;
#endif
class G4Event;

class ATLLArBarrelPriGenAct : public G4VUserPrimaryGeneratorAction {

    public:
        ATLLArBarrelPriGenAct();
        virtual ~ATLLArBarrelPriGenAct();

        virtual void GeneratePrimaries( G4Event* event );

    private:
        #ifdef USEGPS
        G4GeneralParticleSource* fParticleGun;
        #else
        G4ParticleGun* fParticleGun;
        #endif
};

#endif //ATLLArBarrel_h 1

//**************************************************

