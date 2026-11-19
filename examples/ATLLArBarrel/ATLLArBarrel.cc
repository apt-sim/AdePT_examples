//**************************************************
// \file ATLLArBarrel.cc
// \brief: main() of ATLLArBarrel
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
//          @lopezzot
// \start date: 3 August 2022
//**************************************************

// Includers from project files
//
#include "ATLLArBarrelConstruction.hh"
#include "ATLLArBarrelActIni.hh"

// Includers from Geant4
//
#ifndef G4MULTITHREADED
#include "G4RunManager.hh"
#else
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#endif
// #include "G4RunManagerFactory.hh" //only available from 10.7 on
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UIcommand.hh"

#ifdef USE_G4HepEm
#include "PhysicsList.hh" //it is built as a separate library in hepem
#include "G4EmStandardPhysics_G4HepEm.hh"
#endif

#ifdef USE_ADEPT
#include "ATLLArBarrelEmPhysics_AdePT.hh" // EM option 0 with AdePT and the project defaults
#include "G4SystemOfUnits.hh"
#endif

// Output helpers
//
namespace CLIOutputs {
void PrintHelp()
{
  G4cout << "Usage: ATLLArBarrel [OPTION...]\n\n"
         << "Options:\n"
         << "  -m MACRO        path to macro file to run\n"
         << "  -u UISESSION    string of the Geant4 UI session to use\n"
         << "  -t THREADS      number of threads to use in the simulation\n"
         << "  -p PHYSICSLIST  string of the physics list to use\n"
         << "  -h              print this help and exit\n"
         << G4endl;
}
void PrintError()
{
  G4cerr << "Wrong usage, see 'ATLLArBarrel -h' for more information" << G4endl;
}
} // namespace CLIOutputs

int main(int argc, char **argv)
{

  // CLI variables
  //
  G4String macro;
  G4String session;
  G4String custom_pl = "FTFP_BERT"; // default physics list
#ifdef G4MULTITHREADED
  G4int nThreads = G4Threading::G4GetNumberOfCores();
#endif

  // CLI parsing
  //
  for (G4int i = 1; i < argc; i = i + 2) {
    const G4String option = argv[i];
    if (option == "-h") {
      CLIOutputs::PrintHelp();
      return 0;
    }
    // all the other options take a value
    if (i + 1 >= argc) {
      CLIOutputs::PrintError();
      return 1;
    }
    if (option == "-m")
      macro = argv[i + 1];
    else if (option == "-u")
      session = argv[i + 1];
    else if (option == "-p")
      custom_pl = argv[i + 1];
#ifdef G4MULTITHREADED
    else if (option == "-t") {
      nThreads = G4UIcommand::ConvertToInt(argv[i + 1]);
    }
#endif
    else {
      CLIOutputs::PrintError();
      return 1;
    }
  }

  // Activate interaction mode if no macro card is provided and define UI session
  //
  G4UIExecutive *ui = nullptr;
  if (!macro.size()) { // if macro card is not passed
    ui = new G4UIExecutive(argc, argv, session);
  }

// Construct the run manager
//
#ifdef G4MULTITHREADED
  auto runManager = new G4MTRunManager;
  if (nThreads > 0) {
    runManager->SetNumberOfThreads(nThreads);
  }
#else
  auto runManager = new G4RunManager;
#endif

// Manadatory Geant4 classes
//

// Physics List
//
#ifndef USE_G4HepEm // Use Geant4 reference physics lists
  G4cout << "--> Using Geant4 reference physics list" << G4endl;
  auto physListFactory = new G4PhysListFactory();
  auto physicsList     = physListFactory->GetReferencePhysList(custom_pl);
#ifdef USE_ADEPT // transport e-, e+ and gamma on GPU with AdePT
  // The AdePT EM constructor uses EM option 0 (the only one implemented in G4HepEm):
  // refuse reference lists that use another EM option (suffixes of G4PhysListFactory;
  // "_EM0" is option 0 and is fine)
  for (const G4String suffix : {"_EMV", "_EMX", "_EMY", "_EMZ", "_LIV", "_PEN", "__GS", "__SS", "_WVI", "__LE"}) {
    if (G4StrUtil::ends_with(custom_pl, suffix)) {
      G4ExceptionDescription msg;
      msg << "Physics list " << custom_pl << " uses a non-default EM option, which "
          << "the AdePT EM constructor would replace with option 0. "
          << "Use a list without EM suffix (or with _EM0) when running with AdePT.";
      G4Exception("main()", "ATLLArBarrel001", FatalException, msg);
    }
  }
  G4cout << "--> Using AdePT for e-, e+ and gamma" << G4endl;
  physicsList->ReplacePhysics(new ATLLArBarrelEmPhysics_AdePT());
  // Production cut used for the validation, applied before the GPU tables are built
  // (a /run/setCut after /run/initialize does not reach the GPU, Results.md section 3c);
  // a /run/setCut before /run/initialize still overrides it
  physicsList->SetDefaultCutValue(0.01 * mm);
#endif
  runManager->SetUserInitialization(physicsList);
#endif

#ifdef USE_G4HepEmEM // reference list, only its EM constructor uses G4HepEm tracking
  G4cout << "--> Using Geant4 reference physics list " << custom_pl << " with G4HepEm EM tracking" << G4endl;
  auto physListFactory = new G4PhysListFactory();
  auto physicsList     = physListFactory->GetReferencePhysList(custom_pl);
#ifdef USE_G4HepEmEM_MSC_MULTISTEPS // G4HepEm default
  G4cout << "--> G4HepEm: multiple steps in MSC + transportation ON (G4HepEm default)" << G4endl;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_G4HepEm(true));
#else // as AdePT on GPU
  G4cout << "--> G4HepEm: multiple steps in MSC + transportation OFF (as AdePT on GPU)" << G4endl;
  physicsList->ReplacePhysics(new G4EmStandardPhysics_G4HepEm(false));
#endif
  runManager->SetUserInitialization(physicsList);
#elif defined(USE_G4HepEm)
#ifdef USE_G4HepEmTracking // use G4HepEm with specialized tracking
  G4cout << "--> Using G4HepEm with specialized tracking" << G4endl;
  runManager->SetUserInitialization(new PhysicsList("G4HepEmTracking"));
#else
  G4cout << "--> Using G4HepEm" << G4endl; // use G4HepEm
  runManager->SetUserInitialization(new PhysicsList("G4HepEm"));
#endif
#endif

  // Geometry and ActionInitialization
  //
  runManager->SetUserInitialization(new ATLLArBarrelConstruction());
  runManager->SetUserInitialization(new ATLLArBarrelActIni());

  // Visualization manager construction
  //
  auto visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  //
  auto UImanager = G4UImanager::GetUIpointer();

  G4int exitCode = 0;
  if (!ui) {
    // execute an argument macro file if exist (second parser argument)
    G4String command = "/control/execute ";
    UImanager->ApplyCommand("/process/em/verbose 0");  // avoid printing em processes
    UImanager->ApplyCommand("/process/had/verbose 0"); // avoid printing had processes
    const G4int macroStatus = UImanager->ApplyCommand(command + macro);
    if (macroStatus != 0) { // report a failed macro through the exit code
      G4cerr << "Macro " << macro << " failed with status " << macroStatus << G4endl;
      exitCode = 1;
    }
  } else {
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    if (ui->IsGUI()) {
      UImanager->ApplyCommand("/control/execute gui.mac");
    }
    // start interactive session
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program
  //
  delete visManager;
  delete runManager;

  return exitCode;
}

//**************************************************
