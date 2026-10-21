//**************************************************
// \file ATLLArBarrelSensDet.hh
// \brief: definition of ATLLArBarrelSensDet class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 21 february 2023
//**************************************************

#ifndef ATLLArBarrelSensDet_h
#define ATLLArBarrelSensDet_h 1

//Includers from project files
//
#include "ATLLArBarrelHit.hh"
#include "ATLLArBarrelTBConstants.hh"

//Includers from Geant4
//
#include "G4VSensitiveDetector.hh"

//Forward declarations from Geant4
//
class G4Step;
class G4HCofThisEvent;

//Includers from STL
//
#include <vector>

class ATLLArBarrelSensDet : public G4VSensitiveDetector {

    public:
        ATLLArBarrelSensDet(const G4String& name);
        ~ATLLArBarrelSensDet() override = default;

        //methods from base class
        //
        void   Initialize(G4HCofThisEvent* hitCollection) override;
        G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
        void   EndOfEvent(G4HCofThisEvent* hitCollection) override;

        //This sensitive detector creates 3 hits collection 
        //(front, middle and back sections)
        //
        static const G4String fFrontHitsCollectionName;
        static const G4String fMiddleHitsCollectionName;
        static const G4String fBackHitsCollectionName;

    private:
        //Pointer to hits collections allocated ("new") at each event
        //
        ATLLArBarrelHitsCollection* fFrontHitsCollection;
        ATLLArBarrelHitsCollection* fMiddleHitsCollection;
        ATLLArBarrelHitsCollection* fBackHitsCollection;

        //Method to check if the HitID calculated in the ProcessHits method
        //is not outside the boundaries of hits per section
        //
        void CheckHitID(const ATLLArBarrelTBConstants::STACSection& Section, const G4int& HitID  ) const;

        //Method to apply Birks law taken as it is in the ATLAS ATHENA code on 8 May 2023
        //ATHENA LArG4BirksLaw class definition at
        //https://gitlab.cern.ch/atlas/athena/-/blob/master/LArCalorimeter/LArG4/LArG4Code/LArG4Code/LArG4BirksLaw.h
        //
        G4double ApplyBirks(G4double dEMeV, G4double dXCm/*, G4double EFieldKVPerCm*/) const;

};

//Inline definition of class methods
//
inline G4double ATLLArBarrelSensDet::ApplyBirks(G4double dE, G4double dX/*, G4double EField*/) const {

    /*Comments from ATLAS ATHENA reported as ATLASComment: ""*/

    //LAr density for Birks Law taken from constructor of LArG4BirksLaw class constructor at
    //https://gitlab.cern.ch/atlas/athena/-/blob/master/LArCalorimeter/LArG4/LArG4Barrel/src/LArBarrelCalculator.cxx#L85 
    //
    const G4double Birks_LAr_Density = 1.396;

    //Birks Law k parameter taken from constructor of LArCalculatorSvcImp class at
    //https://gitlab.cern.ch/atlas/athena/-/blob/master/LArCalorimeter/LArG4/LArG4Code/src/LArCalculatorSvcImp.cxx#L12
    //
    const G4double m_Birksk(0.05832);//ATLASComment:"value updated for G4 10.6.p03 - 1.2 times the previous value of 0.0486 used in all campaigns before MC21"
    
    //This value taken from ATHENA as passed to the actual m_BirksLaw function at
    //https://gitlab.cern.ch/atlas/athena/-/blob/master/LArCalorimeter/LArG4/LArG4Barrel/src/LArBarrelCalculator.cxx#L253
    //
    const G4double efield = 10.; //ATLASComment: "kV/cm, assume constant for now"
    
    //From here on taken from m_BirksLaw ATHENA implementation at
    //https://gitlab.cern.ch/atlas/athena/-/blob/master/LArCalorimeter/LArG4/LArG4Code/src/LArG4BirksLaw.cc#L7
    //
    if(dX < 1e-5) return dE; 
    //if(EField<1e-3) return 0.;

    G4double dEdX = dE/dX;

    const G4double kOverField = m_Birksk/efield;

    double dEcorr1 = dE * (1 + kOverField*1.51) / (1 + kOverField*dEdX/Birks_LAr_Density); //ATLASComment: "original corrections"

    if (dEdX > 12000.0) dEdX = 12000.0; //ATLASComment: "The experimental data is available only until dE/dX~12000 MeV/cm"

    double kHIP = (dEdX > 969.) ? 0.000754*dEdX+0.2692 : 1.0; //ATLASComment: "No corrections for dE/dX < 969 MeV/cm"

    return dEcorr1*kHIP;
}

inline void ATLLArBarrelSensDet::CheckHitID(const ATLLArBarrelTBConstants::STACSection& Section, const G4int& HitID) const{

    switch(Section){
        case ATLLArBarrelTBConstants::STACSection::Front:
            if(HitID > ATLLArBarrelTBConstants::FrontHitNo){
                G4ExceptionDescription msg;
                msg << "Wrong HitID "<<HitID<<" for section " << Section;
                G4Exception("CalorimeterSD::CheckHitID()",
                "MyCode0004", FatalException, msg);
            }
            break;
        case ATLLArBarrelTBConstants::STACSection::Middle:
            if(HitID > ATLLArBarrelTBConstants::MiddleHitNo){
                G4ExceptionDescription msg;
                msg << "Wrong HitID "<<HitID<<" for section " << Section;
                G4Exception("CalorimeterSD::CheckHitID()",
                "MyCode0004", FatalException, msg);
            }
            break;
        case ATLLArBarrelTBConstants::STACSection::Back:
            if(HitID > ATLLArBarrelTBConstants::BackHitNo){
                G4ExceptionDescription msg;
                msg << "Wrong HitID "<<HitID<<" for section " << Section;
                G4Exception("CalorimeterSD::CheckHitID()",
                "MyCode0004", FatalException, msg);
            }
            break;
        default:
            break;
    }
}

#endif

//**************************************************
