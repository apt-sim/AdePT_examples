//**************************************************
// \file LArBarrelPresamplerConstruction.hh
// \brief: definition of 
//         LArBarrelConstruction class
// \author: originally from ATLAS-G4
//          adaptation by 
//          Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 27 October 2022
//**************************************************

#ifndef LArBarrelPresamplerConstruction_h
#define LArBarrelPresamplerConstruction_h 1

//Includers from Geant4
//
#include "globals.hh"

//Forward declarations from Geant4
//
class G4LogicalVolume;

class LArBarrelPresamplerConstruction {
    
    public:
        LArBarrelPresamplerConstruction();
        LArBarrelPresamplerConstruction(int itb); //for test beam case
        ~LArBarrelPresamplerConstruction();

        //return envelope with presampler
        //
        G4LogicalVolume* GetEnvelope();

    private:
        G4double cmm;
        G4double smallLength,bigLength;
 
        G4double anode_th,cathode_th,larheight;

        G4double mod[8][6];

        G4double prep1_th,prep2_th, shell_th,prot_th;
 
        G4double  mech_clear,rail_th,rail_pos,rail_width;

        G4double mb_th,mb_width,mb_length;

        G4double widthFront,heightIn,heightOut;

        G4double rMinPresamplerMother,rMaxPresamplerMother,PresamplerMother_length;

        //GU add phimin and phi span as parameters

        G4double Phi_min,Phi_span;
  
        G4int nsectors,nbsectors;
};

#endif

//**************************************************
