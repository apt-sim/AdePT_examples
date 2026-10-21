//**************************************************
// \file ATLLArBarrelHit.hh
// \brief: definition of ATLLArBarrelHit 
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 7 May 2022
//**************************************************

#ifndef ATLLArBarrelHit_h
#define ATLLArBarrelHit_h 1

//Includers from Geant4
//
#include "G4VHit.hh"
#include "G4THitsCollection.hh"

class ATLLArBarrelHit : public G4VHit {
  
    public:
        ATLLArBarrelHit();
        ATLLArBarrelHit( const ATLLArBarrelHit& ) = default;
        virtual ~ATLLArBarrelHit();

        //Operators (= and ==)
        //
        ATLLArBarrelHit& operator=( const ATLLArBarrelHit& ) = default;
        //G4bool operator==( const ATLLArBarrelHit& ) const;

        //Methods from base class
        //
        virtual void Draw() override{}
        virtual void Print()override;

        //Methods to handle data
        //
        void AddEdep( G4double dEdep );
        void SetEta( G4double Eta );
        void SetPhi( G4double Phi );
        void SetPositionAllocated( G4bool PosAllocated );

        //Get methods
        //
        G4double GetEdep() const {return fEdep;};
        G4bool HasPositionBeenAllocated() const {return fPositionAllocated;};

    private:
        //Total energy deposition in the cell
        G4double fEdep;
        
        //Eta and Phi cell values
        G4double fHitEta, fHitPhi;
        //Bolean variable to indicate it Eta, Phi
        //have already been assigned
        G4bool fPositionAllocated;

};

using ATLLArBarrelHitsCollection = G4THitsCollection<ATLLArBarrelHit>;

inline void ATLLArBarrelHit::AddEdep(G4double dEdep) { fEdep += dEdep; }

inline void ATLLArBarrelHit::SetPositionAllocated(G4bool PosAllocated) { fPositionAllocated = PosAllocated; }

inline void ATLLArBarrelHit::SetEta(G4double Eta) { fHitEta = Eta; }

inline void ATLLArBarrelHit::SetPhi(G4double Phi) { fHitPhi = Phi; }

inline void ATLLArBarrelHit::Print() {

    G4cout<<"Hit at Phi "<<fHitPhi<<" Eta "<<fHitEta<<" Edep "<<fEdep<<G4endl;
}

#endif //ATLLArBarrelHit_h 1

//**************************************************
