//**************************************************
// \file ATLLArBarrelEventAction.hh
// \brief: definition of ATLLArBarrelEventAction
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 6 May 2023
//**************************************************

#ifndef ATLLArBarrelEventAction_h
#define ATLLArBarrelEventAction_h 1

//Includers from project files
//
#include "ATLLArBarrelHit.hh"

//Includers from Geant4
//
#include "G4UserEventAction.hh"
#include "G4Types.hh"

//Includers from STL
//
#include <vector>

class ATLLArBarrelEventAction : public G4UserEventAction {
    
    public:
        ATLLArBarrelEventAction();
        virtual ~ATLLArBarrelEventAction();

        //Methods from base class to be implemented
        //
        virtual void BeginOfEventAction( const G4Event* event );
        virtual void EndOfEventAction( const G4Event* event );

        //Methods to be called by G4AnalysisManager at the AddNtupleRow() call
        //
        std::vector<G4double>& GetFrontHitsEdepVector() { return fFrontHitsEdepVector; };
        std::vector<G4double>& GetMiddleHitsEdepVector(){ return fMiddleHitsEdepVector; };
        std::vector<G4double>& GetBackHitsEdepVector()  { return fBackHitsEdepVector; };

        //Method to modify and get fHasHadronInteracted
        //
        void SetHasHadronInteracted(G4bool interaction){ fHasHadronInteracted = interaction; };
        G4bool GetHasHadronInteracted(){ return fHasHadronInteracted; };

    private:
        //This method is a wrapper around the kernel method event->GetHCofThisEvent()->GetHC(hcID)
        //
        ATLLArBarrelHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;

        //Vector for permanent storage of energy deposits in Hits (front, middle and back sections)
        //
        std::vector<G4double> fFrontHitsEdepVector;
        std::vector<G4double> fMiddleHitsEdepVector;
        std::vector<G4double> fBackHitsEdepVector;

        //Boolean to store if the primary hadron has interacted
        //
        G4bool fHasHadronInteracted;
};
                     
#endif //ATLLArBarrelEventAction_h

//**************************************************

