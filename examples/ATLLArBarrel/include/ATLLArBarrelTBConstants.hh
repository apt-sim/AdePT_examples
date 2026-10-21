//**************************************************
// \file ATLLArBarrelTBConstants.hh
// \brief: definition of ATLLArBarrelTBConstants 
//         namespace
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 5 May 2023
//**************************************************

#ifndef ATLLArBarrelTBConstants_h
#define ATLLArBarrelTBConstants_h 1

//Includers from Geant4
//
#include "G4SystemOfUnits.hh"
#include "G4Types.hh"

//Includers from STL
//
#include <ostream>

namespace ATLLArBarrelTBConstants{

    //explicit enum class for STAC sections
    //
    enum class STACSection : int {
        Front = 0,
        Middle = 1,
        Back = 2
    };
    std::ostream& operator<<(std::ostream& ostream, const STACSection& section);
 
    //Geometry parameters for hits and sensitive detector
    //
    constexpr G4double EtaMin = 0.;                        //We consider only half barrel (z>0)
    constexpr G4double EtaMax = 1.475;                     //end of half barrel (z>0)
    constexpr G4double EtaChange = 0.8;                    //at eta=0.8 the absorber changes
                                                           //the absorber is divided in
                                                           //A (eta<0.8) and B (eta>0.8) sides 
    constexpr G4double RMin = 1.4*m;                       //Internal radius of whole calo
    constexpr G4double RMinSTAC = 1.5*m;                   //Internal radius of STAC
    constexpr G4double STAClength_eta0 = 470.*mm;          //STAC length at eta=0
                                                           //At eta=0 the STAC is 22.3X0
                                                           //front (4.3X0) + middle (16.0X0) + back (2.0X0)
    constexpr G4double X0 = 470./22.3;                     //X0 in mm
    constexpr G4double FrontDepth = 4.3*X0;                //Front section depth (mm)
    constexpr G4double MiddleDepth = FrontDepth + 16.0*X0; //Middle section depth (mm)

    constexpr G4double BackDeltaEta = 0.05;                //(rad)DeltaEta at back section
                                                           //there are 16 eta bins in back section
                                                           //up to eta=0.8 (first half calo eta<0.8)
    constexpr G4double MiddleDeltaEta = BackDeltaEta/2.;   //(rad)Eta bins in middle section
    constexpr G4double FrontDeltaEta  = MiddleDeltaEta/8.; //(rad)Eta bins in front section

    constexpr G4double STACDeltaPhi     = 2.*M_PI/16;      //(rad)Delta Phi STAC
    constexpr G4double halfSTACDeltaPhi = STACDeltaPhi/2.;
    constexpr G4double FrontDeltaPhi  = STACDeltaPhi/4;    //(rad)Delta Phi front section
    constexpr G4double MiddleDeltaPhi = STACDeltaPhi/16;   //(rad)Delta Phi middle section
    constexpr G4double BackDeltaPhi   = MiddleDeltaPhi;    //(rad)Delta Phi back section

    constexpr G4int FrontHitNo  = 1024;                    //Number of hit to be allocated
    constexpr G4int MiddleHitNo = 512;                     //for front, middle and back
    constexpr G4int BackHitNo   = 256;                     //section (fist half calo eta<0.8)
    
    constexpr G4int BackEtasPerRow = 16;                   //Eta cells per row back section
    constexpr G4int MiddleEtasPerRow = BackEtasPerRow*2;   //Eta cells per row middle section
    constexpr G4int FrontEtasPerRow = MiddleEtasPerRow*8;  //Eta cells per row front section

    //Electronic noise parameters
    //
    constexpr G4double SampFrac = (2*GeV)/(10*GeV);        //At the moment, let's consider
                                                           //a simple 20% energy in LAr
                                                           //of total calo deposition
    constexpr G4double ElectNoiseSigma = (20*MeV)*SampFrac;//Sigma of 20 MeV equivalent
                                                           //electronic noise

} //ATLLArBarrelTBConstants

#endif //ATLLArBarrelTBConstants_h 1

//**************************************************

