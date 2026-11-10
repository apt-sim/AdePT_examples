// SPDX-FileCopyrightText: 2022-2026 Lorenzo Pezzotti and ATLLArBarrel contributors
// SPDX-License-Identifier: MIT

//**************************************************
// \file MaterialManager.cc
// \brief: implementation of MaterialManager class
// \author: Andrea Dell'Acqua
// \start date: 26 October 2026
//**************************************************

// Includers from project files
//
#include "MaterialManager.hh"

// Includers from Geant4
//
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ios.hh"

MaterialManager *MaterialManager::GetInstance()
{
  static MaterialManager instance;
  return &instance;
}

MaterialManager::MaterialManager()
{
  DefineCommonMaterials();
}

G4Material *MaterialManager::AddMaterial(G4Material *material)
{

  const G4String &name = material->GetName();
  auto it              = fMaterials.find(name);
  if (it == fMaterials.end()) {
    fMaterials[name] = material;
    return material;
  }

  // Same pointer registered twice: nothing to do
  //
  G4Material *registered = it->second;
  if (registered == material) return registered;

  // Duplicated name: warn, with more details if definitions differ
  //
  G4ExceptionDescription msg;
  msg << "Material " << name << " already defined, keeping the first definition.";
  if (registered->GetDensity() != material->GetDensity() ||
      registered->GetNumberOfElements() != material->GetNumberOfElements()) {
    msg << G4endl << "Definitions differ: density " << registered->GetDensity() / (g / cm3) << " vs "
        << material->GetDensity() / (g / cm3) << " g/cm3, elements " << registered->GetNumberOfElements() << " vs "
        << material->GetNumberOfElements() << ". Use a different name for the new definition.";
  }
  G4Exception("MaterialManager::AddMaterial()", "MatMgr001", JustWarning, msg);
  return registered;
}

G4Material *MaterialManager::GetMaterial(const G4String &name) const
{
  auto it = fMaterials.find(name);
  if (it == fMaterials.end()) {
    G4ExceptionDescription msg;
    msg << "Material " << name << " not defined in MaterialManager.";
    G4Exception("MaterialManager::GetMaterial()", "MatMgr002", FatalException, msg);
    return nullptr;
  }
  return it->second;
}

G4bool MaterialManager::IsDefined(const G4String &name) const
{
  return fMaterials.find(name) != fMaterials.end();
}

G4Element *MaterialManager::GetElement(const G4String &symbol) const
{
  G4Element *element = G4NistManager::Instance()->FindOrBuildElement(symbol);
  if (!element) {
    G4ExceptionDescription msg;
    msg << "Element " << symbol << " not found in NIST database.";
    G4Exception("MaterialManager::GetElement()", "MatMgr003", FatalException, msg);
  }
  return element;
}

void MaterialManager::PrintMaterials() const
{
  G4cout << "MaterialManager: " << fMaterials.size() << " materials defined" << G4endl;
  for (const auto &entry : fMaterials) {
    G4cout << "  " << entry.first << "  density " << entry.second->GetDensity() / (g / cm3) << " g/cm3" << G4endl;
  }
}

void MaterialManager::DefineCommonMaterials()
{

  G4double a, density;

  // Air
  //
  density  = 1.29e-03 * g / cm3;
  auto Air = new G4Material("Air", density, 2);
  Air->AddElement(GetElement("N"), .8);
  Air->AddElement(GetElement("O"), .2);
  AddMaterial(Air);

  // Vacuum
  //
  density              = universe_mean_density;
  G4double pressure    = 3.e-18 * pascal;
  G4double temperature = 2.73 * kelvin;
  AddMaterial(new G4Material("Vacuum", 1., 1.01 * g / mole, density, kStateGas, temperature, pressure));

  // Aluminium
  //
  a       = 26.98 * g / mole;
  density = 2.70 * g / cm3;
  AddMaterial(new G4Material("Aluminium", 13., a, density));

  // Iron
  //
  a       = 55.845 * g / mole;
  density = 7.87 * g / cm3;
  AddMaterial(new G4Material("Iron", 26., a, density));

  // Copper
  //
  a       = 63.55 * g / mole;
  density = 8.96 * g / cm3;
  AddMaterial(new G4Material("Copper", 29., a, density));

  // Lead
  //
  a       = 207.19 * g / mole;
  density = 11.35 * g / cm3;
  AddMaterial(new G4Material("Lead", 82., a, density));

  // liquid Argon
  //
  a       = 39.948 * g / mole;
  density = 1.396 * g / cm3;
  AddMaterial(new G4Material("LiquidArgon", 18., a, density));

  // Kapton (C22 H10 O5 N2)
  //
  density     = 1.46 * g / cm3;
  auto Kapton = new G4Material("Kapton", density, 4);
  Kapton->AddElement(GetElement("H"), 10);
  Kapton->AddElement(GetElement("O"), 5);
  Kapton->AddElement(GetElement("C"), 22);
  Kapton->AddElement(GetElement("N"), 2);
  AddMaterial(Kapton);
}

//**************************************************
