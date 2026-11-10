// SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
// SPDX-License-Identifier: MIT

//**************************************************
// \file ATLLArBarrelConstruction.cc
// \brief: implementation of
// ATLLArBarrelConstruction class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 3 August 2022
//**************************************************

// Includers from Geant4
//
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4GeomTestVolume.hh"
#include "G4GDMLParser.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"

// Includers from project files
//
#include "ATLLArBarrelConstruction.hh"
#include "ATLLArBarrelSensDet.hh"
#include "LArTBCryostatConstruction.hh"
#include "MaterialManager.hh"

ATLLArBarrelConstruction::ATLLArBarrelConstruction() : G4VUserDetectorConstruction(), fDumpGDML(false) {};

ATLLArBarrelConstruction::~ATLLArBarrelConstruction() {};

G4VPhysicalVolume *ATLLArBarrelConstruction::Construct()
{

  // Air is defined in MaterialManager
  //
  G4Material *Air = MaterialManager::GetInstance()->GetMaterial("Air");

  // The Experimental Hall
  //
  G4double s_experimentalHall_x = 14. * m;
  G4double s_experimentalHall_y = 14. * m;
  G4double s_experimentalHall_z = 16. * m;
  G4Box *experimentalHall_box =
      new G4Box("experimentalHall_box", s_experimentalHall_x, s_experimentalHall_y, s_experimentalHall_z);

  G4LogicalVolume *experimentalHall_log = new G4LogicalVolume(experimentalHall_box, Air, "experimentalHall_log");
  experimentalHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());

  G4VPhysicalVolume *experimentalHall_phys =
      new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), experimentalHall_log, "world", 0, false, 0);

  // insert test beam cryostat (calorimeter module is inserted inside cryostat)
  LArTBCryostatConstruction *cryostat  = new LArTBCryostatConstruction();
  G4LogicalVolume *tbcryostatmotherLog = cryostat->GetEnvelope(); // Em_log
  G4VPhysicalVolume *tbcryostatmotherPhys =
      new G4PVPlacement(0,
                        // G4ThreeVector(0.,0.,+2000.), //old position
                        // as ATLAS TB code
                        G4ThreeVector(0., 0., 0.), // new pos
                                                   // calo starts at z=0
                        "LAr::TBBarrel::Cryostat::MotherPhys", tbcryostatmotherLog, experimentalHall_phys, false, 0);
  tbcryostatmotherLog->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Check overlaps of experimentalHall_phys
  //(volume, tolerance, npoints, verbosity)
  //
  // G4GeomTestVolume* testVolume = new G4GeomTestVolume(experimentalHall_phys, 0.0, 1000, true);
  // testVolume->TestOverlapInTree();

  // Dump GDML file with experimentalHall_phys
  //
  if (fDumpGDML) {
    G4GDMLParser Parser;
    Parser.Write("experimentalHall.gdml", experimentalHall_phys);
  }

  // Return experimentalHall_phys
  //
  return experimentalHall_phys;
}

void ATLLArBarrelConstruction::ConstructSDandField()
{

  // Sensitive detectors
  //
  auto LArSD = new ATLLArBarrelSensDet("LArSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(LArSD);

  // Assign to logical volume
  //
  auto LVStore = G4LogicalVolumeStore::GetInstance();
  for (auto LV : *LVStore) {

    if (LV->GetName() == "LAr::EMB::STAC") LV->SetSensitiveDetector(LArSD);
  }

  // No fields
}

//**************************************************
