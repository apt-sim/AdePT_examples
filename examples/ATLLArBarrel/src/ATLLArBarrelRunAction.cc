//**************************************************
// \file ATLLArBarrelRunAction.cc
// \brief: implementation of ATLLArBarrelRunAction
//         class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 31 October 2022
//**************************************************

//Includers from project files
//
#include "ATLLArBarrelRunAction.hh"

//Includers from Geant4
//
#include "G4Version.hh"
#if G4VERSION_NUMBER < 1100
#include "g4root.hh"  // replaced by G4AnalysisManager.h  in G4 v11 and up
#else
#include "G4AnalysisManager.hh"
#endif

ATLLArBarrelRunAction::ATLLArBarrelRunAction(ATLLArBarrelEventAction* evtAction)
    : G4UserRunAction(),
      fEventAction(evtAction){
    
    //Set the output file (.root) structure
    //
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetNtupleMerging(true);

    //This (as far as I can investigate such a stupid thing) is only needed
    //in g410.5(and patches)
    //
    #if G4VERSION_NUMBER > 1050
    #if G4VERSION_NUMBER < 1060
    analysisManager->SetNtupleRowWise(false);
    #endif
    #endif
  
    // Creating ntuple
    //
    analysisManager->CreateNtuple("ATLLArBarrelout", "ATLLArBarreloutput");
    analysisManager->CreateNtupleIColumn("HadInteraction");
    analysisManager->CreateNtupleDColumn("EnergyLAr");
    analysisManager->CreateNtupleDColumn("FrontHitsEdep", fEventAction->GetFrontHitsEdepVector());
    analysisManager->CreateNtupleDColumn("MiddleHitsEdep",fEventAction->GetMiddleHitsEdepVector());
    analysisManager->CreateNtupleDColumn("BackHitsEdep",  fEventAction->GetBackHitsEdepVector());
    analysisManager->FinishNtuple();
}

ATLLArBarrelRunAction::~ATLLArBarrelRunAction(){

    #if G4VERSION_NUMBER < 1100
    delete G4AnalysisManager::Instance();  // not needed for G4 v11 and up
    #endif
}

void ATLLArBarrelRunAction::BeginOfRunAction(const G4Run* run){
    fTimer.Start();
    
    //Save random number seed
    //
    //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
 
    //Open output file
    //
    auto analysisManager = G4AnalysisManager::Instance();
    std::string runnumber = std::to_string( run->GetRunID() );
    G4String fileName = "ATLLArBarrelout_Run" + runnumber + ".root";
    analysisManager->OpenFile(fileName);
}

void ATLLArBarrelRunAction::EndOfRunAction([[maybe_unused]] const G4Run* aRun){
 
    //Write evt info and close output file
    //
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();

    //Stop Time and printout time
    //
    fTimer.Stop();
    G4int events = aRun->GetNumberOfEvent();
    G4cout << " ====================================================================== " << G4endl;
    G4cout << "  Run terminated, " << events << " events transported" << G4endl;
    G4cout << "  Time: " << fTimer << G4endl;
    G4cout << " ====================================================================== " << G4endl;
}

//**************************************************
