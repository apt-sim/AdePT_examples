//**************************************************
// \file G4EmStandardPhysics_G4HepEm.hh
// \brief: G4EmStandardPhysics with e-, e+ and gamma
//         tracked by the G4HepEmTrackingManager
//         (same structure as AdePT's G4EmStandardPhysics_HepEm)
//         By default the combined MSC + transportation
//         multiple steps are disabled, as in AdePT on GPU
//**************************************************

#ifndef G4EmStandardPhysics_G4HepEm_h
#define G4EmStandardPhysics_G4HepEm_h 1

#include "G4EmStandardPhysics.hh"

class G4EmStandardPhysics_G4HepEm : public G4EmStandardPhysics {

public:
  // multipleStepsMSC: let G4HepEm do several MSC steps per transportation step
  // (the G4HepEm default). AdePT does not do this on the GPU, so the default
  // here is false, to compare with AdePT.
  explicit G4EmStandardPhysics_G4HepEm(G4bool multipleStepsMSC = false, G4int ver = 1, const G4String &name = "");
  ~G4EmStandardPhysics_G4HepEm() override = default;

  void ConstructProcess() override;

private:
  G4bool fMultipleStepsMSC;
};

#endif // G4EmStandardPhysics_G4HepEm_h
