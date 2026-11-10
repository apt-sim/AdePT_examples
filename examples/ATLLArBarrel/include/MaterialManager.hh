// SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
// SPDX-License-Identifier: MIT

//**************************************************
// \file MaterialManager.hh
// \brief: definition of MaterialManager class
//         singleton holding all G4Materials of the
//         project, avoids duplicated definitions
// \author: Andrea Dell'Acqua
// \start date: 26 October 2026
//**************************************************

#ifndef MaterialManager_h
#define MaterialManager_h 1

// Includers from Geant4
//
#include "globals.hh"

// Forward declarations from Geant4
//
class G4Material;
class G4Element;

// Includers from STL
//
#include <map>

class MaterialManager {

public:
  static MaterialManager *GetInstance();

  MaterialManager(const MaterialManager &)            = delete;
  MaterialManager &operator=(const MaterialManager &) = delete;

  // Register a new material. If a material with the same name
  // is already registered, issue a warning and return the
  // registered one (the new one is not registered)
  //
  G4Material *AddMaterial(G4Material *material);

  // Retrieve a registered material (fatal exception if missing)
  //
  G4Material *GetMaterial(const G4String &name) const;

  // Check if a material is already registered
  //
  G4bool IsDefined(const G4String &name) const;

  // Retrieve an element from the NIST database by symbol (e.g. "Cu")
  //
  G4Element *GetElement(const G4String &symbol) const;

  // Print the names of all registered materials
  //
  void PrintMaterials() const;

private:
  MaterialManager();
  ~MaterialManager() = default;

  // Define the materials shared by several detector components
  //
  void DefineCommonMaterials();

  std::map<G4String, G4Material *> fMaterials;
};

#endif // MaterialManager_h 1

//**************************************************
