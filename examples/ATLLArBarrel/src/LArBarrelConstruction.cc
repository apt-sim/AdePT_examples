//**************************************************
// \file LArBarrelConstruction.cc
// \brief: implementation of 
//         LArBarrelConstruction class
// \author: originally from ATLAS-G4
//          adaptation by 
//          Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 10 August 2022
//**************************************************

//Includers from project files
//
#include "LArBarrelConstruction.hh"

//Includers from Geant4
//
#include "G4VPhysicalVolume.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
#include "G4Trd.hh"
#include "G4Polycone.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "globals.hh"

#include <iostream>
#include <cmath>

//#define FRONT_STEEL
#define BUILD_FRONT_ELECTRONICS
#define BUILD_HIGHETA_ELECTRONICS
#define BUILD_FRONT_G10
#define BUILD_BACK_G10
#define BUILD_STAC
#define BUILD_ACCORDION_PLATES

// Define this macro to use the original geometry construction, taken from
// ATLAS. For example, this creates all "Straight" volumes as G4Traps, even
// though many of them can actually be created as G4Box.
//#define ORIGINAL_ATLAS

LArBarrelConstruction::LArBarrelConstruction()
    : A_SAGGING(false),
      IflCur(false),
      IflMapTrans(true) {}

LArBarrelConstruction::~LArBarrelConstruction(){}

G4LogicalVolume* LArBarrelConstruction::GetEnvelope() {

    //--------------------------------------------------------------------------- 
    // Elements , Mixtures and Materials 
    //---------------------------------------------------------------------------
  
    G4String name,symbol;
    G4double a,z,density,fractionmass;
    G4double dalu,dlead,dlar,dglue,dfe,dcu,dkapton,dGten,dmixelec;
    G4int ncomponents,natoms;

    /*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** Material list " << std::endl;
    #endif*/

    // Vacuum
    density = CLHEP::universe_mean_density;  //from PhysicalConstants.h
    G4double pressure = 3.e-18*CLHEP::pascal;
    G4double temperature = 2.73*CLHEP::kelvin;
    G4Material * Vacuum =
    	new G4Material(name="Vacuum", z=1., a=1.01*CLHEP::g/CLHEP::mole, density,
    		       kStateGas,temperature,pressure); 

    /*#ifdef DEBUG_GEO
    std::cout << " Vacuum X0 = " << Vacuum->GetRadlen() << std::endl;
    #endif*/

    // Copper
    a = 63.55*CLHEP::g/CLHEP::mole;
    density = 8.96*CLHEP::g/CLHEP::cm3;
    G4Material * Copper = new G4Material(name="Copper",29.,a, density);
    /*#ifdef DEBUG_GEO
    std::cout << " Copper X0 = " << Copper->GetRadlen() << std::endl;
    #endif*/

    // Aluminium
    a = 26.98*CLHEP::g/CLHEP::mole;
    density = 2.70*CLHEP::g/CLHEP::cm3;
    dalu = density;
    G4Material *  Aluminium = new G4Material(name="Aluminium",13.,a, density);	
    /*#ifdef DEBUG_GEO
    std::cout << " Alumin X0/lambda = " << Aluminium->GetRadlen() 
            << " " << Aluminium->GetNuclearInterLength() << std::endl;
    #endif*/
  
    // Lead
    a = 207.19*CLHEP::g/CLHEP::mole;
    density = 11.35*CLHEP::g/CLHEP::cm3;
    dlead = density;
    G4Material * Lead = new G4Material(name="Lead",82.,a, density);
    /*#ifdef DEBUG_GEO
    std::cout << " Lead   X0/lambda = " << Lead->GetRadlen() 
            << " " << Lead->GetNuclearInterLength() << std::endl;
    #endif*/

    // liquid Argon
    a = 39.948*CLHEP::g/CLHEP::mole;
    density = 1.396*CLHEP::g/CLHEP::cm3;
    dlar = density;
    G4Material *  LAr = new G4Material(name="LiquidArgon",18.,a, density);
    /*#ifdef DEBUG_GEO
    std::cout << " LAr    X0/lambda = " << LAr->GetRadlen() 
            << " " << LAr->GetNuclearInterLength() << std::endl;
    #endif*/

    // Argon
    a = 39.95*CLHEP::g/CLHEP::mole;
    G4Element *elAr = new G4Element(name="Argon", symbol="Ar", z=18., a);

    // Oxygen
    a = 16.00*CLHEP::g/CLHEP::mole;
    G4Element *elO = new G4Element(name="Oxygen", symbol="O", z=8., a);
 
    // Nitrogen 
    a= 14.01*CLHEP::g/CLHEP::mole;
    G4Element *elN = new G4Element(name="Nitrogen", symbol="N", z=7., a);

    // Air
    density = 1.29e-03*CLHEP::g/CLHEP::cm3;
    G4Material *  Air = new G4Material(name="Air", density, ncomponents=2);
    Air->AddElement(elN, fractionmass=.8); 
    Air->AddElement(elO, fractionmass=.2);
    /*#ifdef DEBUG_GEO
    std::cout << " Air    X0 = " << Air->GetRadlen() << std::endl;
    #endif*/
 
    // Hydrogen
    a= 1.01*CLHEP::g/CLHEP::mole;
    G4Element *elH = new G4Element(name="Hydrogen", symbol="H", z=1., a);

    // Carbon
    a= 12.01*CLHEP::g/CLHEP::mole;
    G4Element *elC = new G4Element(name="Carbon", symbol="C", z=6., a);

    // Silicium
    a= 28.09*CLHEP::g/CLHEP::mole;
    G4Element *elSi = new G4Element(name="Silicium", symbol="Si", z=14., a);

    // Glue  raw formula  C5 H8 O4 Si
    density = 1.69*CLHEP::g/CLHEP::cm3;
    dglue = density;
    G4Material *Glue = new G4Material(name="Glue", density, ncomponents=4);
    Glue->AddElement(elH,natoms=8); 
    Glue->AddElement(elO,natoms=4);
    Glue->AddElement(elC,natoms=5);
    Glue->AddElement(elSi,natoms=1);
    /*#ifdef DEBUG_GEO
    std::cout << " Glue   X0/Lambda = " << Glue->GetRadlen() 
            << " " << Glue->GetNuclearInterLength() << std::endl;
    #endif*/

    // Define Material Kapton (C22 H10 O5 N2)
    density = 1.46*CLHEP::g/CLHEP::cm3;
    dkapton = density;
    G4Material* Kapton = new G4Material(name="Kapton",density,ncomponents=4);
    Kapton->AddElement(elH,natoms=10);
    Kapton->AddElement(elO,natoms=5);
    Kapton->AddElement(elC,natoms=22);
    Kapton->AddElement(elN,natoms=2);
    /*#ifdef DEBUG_GEO
    std::cout << " Kapton X0/Lambda = " << Kapton->GetRadlen() 
            << " " << Kapton->GetNuclearInterLength() << std::endl;
    #endif*/

    // Iron
    a = 55.85*CLHEP::g/CLHEP::mole;
    density = 7.87*CLHEP::g/CLHEP::cm3;
    dfe = density;
    G4Element *elFe = new G4Element(name="Iron", symbol="Fe", z=26., a);

    G4Material *Iron = new G4Material(name="Iron",26.,a, density);

    // Copper
    a = 63.64*CLHEP::g/CLHEP::mole;
    density = 8.96*CLHEP::g/CLHEP::cm3;
    dcu = density;
    G4Element *elCu = new G4Element(name="Coper", symbol="Cu", z=29., a);

    // Define THIN and THICK lead absorbers
    G4double Totalthick;
    G4double Totalmass,Fracpb,Fracfe,Fracgl;
    G4double Totalthicke;
    G4double Totalmasse,FracCu,FracKap;

    // THIN absorber
    //Tggl = m_parameters->GetValue("LArEMBThinAbsGlue");
    Tggl = 0.0679*cm; //from atlas db and MySqlEMB.cxx
    //Tgfe = m_parameters->GetValue("LArEMBThinAbsIron");
    Tgfe = 0.04*cm; //from atlas db and MySqlEMB.cxx
    //Tgpb = m_parameters->GetValue("LArEMBThinAbsLead");
    Tgpb = 0.1131*cm; //from atlas db and MySqlEMB.cxx
    Totalthick = Tggl+Tgfe+Tgpb;
    Totalmass = (Tgpb*dlead+Tgfe*dfe+Tggl*dglue);
    Fracpb = (Tgpb*dlead)/Totalmass;
    Fracfe = (Tgfe*dfe)/Totalmass;
    Fracgl = (Tggl*dglue)/Totalmass;
    density = Totalmass/Totalthick;
    /*#ifdef DEBUG_GEO
    std::cout<<"\n ---- THIN absorber characteristics: ----" << std::endl;
    std::cout<<"  Fraction pb,fe,gl: "<<Fracpb<<","<<Fracfe<<","<<Fracgl<< std::endl;
    std::cout<<"  Total mass, Thickness: "<<Totalmass<<" ,"<<Totalthick<< std::endl;
    std::cout<<"  Thinabs Density =  "<< density/(g/cm3) << "\n" << std::endl;
    #endif*/
    G4Material * Thin_abs = new G4Material(name="Thinabs",density,ncomponents=3);
    Thin_abs->AddMaterial(Lead,Fracpb);
    Thin_abs->AddElement(elFe,Fracfe);
    Thin_abs->AddMaterial(Glue,Fracgl);
    /*#ifdef DEBUG_GEO
    std::cout << " Thin absorber  X0/lambda = " << Thin_abs->GetRadlen() 
            << " " << Thin_abs->GetNuclearInterLength() << std::endl;
    #endif*/

    // THICK absorber
    //Thgl = m_parameters->GetValue("LArEMBThickAbsGlue");
    Thgl = 0.0278*cm; //from atlas fb and MySqlEMB.cxx
    //Thfe = m_parameters->GetValue("LArEMBThickAbsIron");
    Thfe = 0.04*cm; //from athena db and MySqlEMB.cxx
    //Thpb = m_parameters->GetValue("LArEMBThickAbsLead");
    Thpb = 0.1532*cm; //from athena db and MySqlEMB.cxx
    Totalthick = Thgl+Thfe+Thpb;
    Totalmass = (Thpb*dlead+Thfe*dfe+Thgl*dglue);
    Fracpb = (Thpb*dlead)/Totalmass;
    Fracfe = (Thfe*dfe)/Totalmass;
    Fracgl = (Thgl*dglue)/Totalmass;
    density = Totalmass/Totalthick;
    /*#ifdef DEBUG_GEO
    std::cout<<"\n ---- THICK absorber characteristics: ----" << std::endl;
    std::cout<<"  Fraction pb,fe,gl: "<<Fracpb<<","<<Fracfe<<","<<Fracgl<< std::endl;
    std::cout<<"  Total mass, Thickness: "<<Totalmass<<" ,"<<Totalthick<< std::endl;
    std::cout<<"  Thickabs Density =  " << density/(g/cm3) << " \n" << std::endl;
    #endif*/
    G4Material * Thick_abs = new G4Material(name="Thickabs",density,ncomponents=3);
    Thick_abs->AddMaterial(Lead,Fracpb);
    Thick_abs->AddElement(elFe,Fracfe);
    Thick_abs->AddMaterial(Glue,Fracgl);
    /*#ifdef DEBUG_GEO
    std::cout << " Thick absorber X0/lambda = " << Thick_abs->GetRadlen() 
            << " " << Thick_abs->GetNuclearInterLength() << std::endl;
    #endif*/


    // Define electrode as a mixture Kapton+Cu
    // Note:
    // There is, of course, glue between the Cu and Kapton
    // layers, but since the glue material is similar to
    // Kapton, we use Kapton for the whole "Kapton+glue" part
    //Thcu = m_parameters->GetValue("LArEMBThickElecCopper");
    Thcu = 0.0105*cm; //from athena db and MySqlEMB.cxx
    //Thfg = m_parameters->GetValue("LArEMBThickElecKapton");
    Thfg = 0.017*cm; //from athena db and MySqlEMB.cxx 
    Totalthicke = Thcu+Thfg;
    Totalmasse = (Thcu*dcu+Thfg*dkapton);
    FracCu = (Thcu*dcu)/Totalmasse;
    FracKap = (Thfg*dkapton)/Totalmasse;
    density = Totalmasse/Totalthicke;
    /*#ifdef DEBUG_GEO
    std::cout<<"\n ---- Electrode characteristics: ----" << std::endl;
    std::cout<<"  Fraction Cu, Kapton: " << FracCu << "," << FracKap << std::endl;
    std::cout<<"  Total mass, Thickness:"<<Totalmasse<<" ,"<<Totalthicke<< std::endl;
    std::cout<<"  Electrode Density =  " << density/(g/cm3) << "\n " << std::endl;
    #endif*/
    G4Material * Kapton_Cu = new G4Material(name="KaptonC",density,ncomponents=2);
    Kapton_Cu->AddElement(elCu,FracCu);
    Kapton_Cu->AddMaterial(Kapton,FracKap);
    /*#ifdef DEBUG_GEO
    std::cout << " Electrode     X0/Lambda = " << Kapton_Cu->GetRadlen() 
            << " " << Kapton_Cu->GetNuclearInterLength() << std::endl;
    #endif*/

    // Define Material GTEN_epoxy  ( C8 H14 O4 )
    //density = m_parameters->GetValue("LArEMBEpoxyVolumicMass");
    density = 1.8*g/cm; //from atlas fb and MySqlEMB.cxx
    dGten = density;
    G4Material * Gten = new G4Material(name="Gten", density, ncomponents=3);
    Gten->AddElement(elH,natoms=14); 
    Gten->AddElement(elO,natoms=4);
    Gten->AddElement(elC,natoms=8);
    /*#ifdef DEBUG_GEO
    std::cout << " G10           density   = " << density/(CLHEP::g/CLHEP::cm3) << std::endl;
    std::cout << " G10           X0/Lambda = " << Gten->GetRadlen() 
            << " " << Gten->GetNuclearInterLength() << std::endl;
    #endif*/

    // Define Material SiO2
    G4Material *SiO2 = new G4Material(name="SiO2",density,ncomponents=2);
    SiO2->AddElement(elSi,natoms=1);
    SiO2->AddElement(elO,natoms=2);

    // Define Material mixture of GTEN_eopxy and SiO2 for the Bars
    density = 1.72*CLHEP::g/CLHEP::cm3;
    G4Material *Gten_bar = new G4Material(name="Gten_bar",density,ncomponents=2);
    Gten_bar->AddMaterial(Gten,fractionmass=0.38);
    Gten_bar->AddMaterial(SiO2,fractionmass=0.62);
    /*#ifdef DEBUG_GEO
    std::cout << " G10_bar       density   = " << density/(g/cm3) << std::endl;
    std::cout << " G10_bar       X0/Lambda = " << Gten_bar->GetRadlen()
            << " " << Gten_bar->GetNuclearInterLength() << std::endl;
    #endif*/


    // Define Material ELECTRONICS:
    //
    // new (refined) version : mixture of
    //                                C22 H10 O5 N2 (for Kapton)
    //                            and Copper 
    // GU 4/6/2004 compute density from input parameters
    //  density = 2.440*g/cm3;
    //double frmassCu = m_parameters->GetValue("LArEMBmasspercentCu");
    double frmassCu = 62; //from atlas db and MySqlEMB.cxx
    //double frmassKap = m_parameters->GetValue("LArEMBmasspercentKap");
    double frmassKap = 39; //from atlas fb and MySqlEMB.cxx
    density = dcu*(1.+frmassKap/frmassCu)/(1.+frmassKap/frmassCu*dcu/dkapton);
    G4Material * Cable_elect = new G4Material(name="Cables",density,ncomponents=2);
    Cable_elect->AddElement(elCu, fractionmass=frmassCu*CLHEP::perCent);
    Cable_elect->AddMaterial(Kapton, fractionmass=frmassKap*CLHEP::perCent);
    /*#ifdef DEBUG_GEO
    std::cout << " Cable    X0/lambda= " << Cable_elect->GetRadlen() 
            << " " << Cable_elect->GetNuclearInterLength() << std::endl;
    #endif*/

    //
    // Define Material Electronics MOTHER_BOARDS
    //
    // Mother_board is defined as a mixture of epox_G10 (C8 H14 O4) and Copper
    //ThMBcu = m_parameters->GetValue("LArEMBCuThickness");
    ThMBcu = 0.063*cm; //from atlas db and MySqlEMB.cxx
    //ThMBG10 = m_parameters->GetValue("LArEMBG10Thickness");
    ThMBG10 = 0.367*cm; //from atlas db and MySqlEMB.cxx
    G4double TotalthickMBe = ThMBcu+ThMBG10;
    G4double TotalmassMBe = (ThMBcu*dcu+ThMBG10*dGten);
    G4double FracMBCu = (ThMBcu*dcu)/TotalmassMBe;
    G4double FracMBG10 = (ThMBG10*dGten)/TotalmassMBe;
    density = TotalmassMBe/TotalthickMBe;
    /*#ifdef DEBUG_GEO
    std::cout<<"\n ---- Mother Board characteristics: ----" << std::endl;
    std::cout<<"  Fraction Cu, G10: " << FracMBCu << "," << FracMBG10 << std::endl;
    std::cout<<"  Total mass, Thickness:"
         << TotalmassMBe <<" ," <<TotalthickMBe<< std::endl;
    std::cout<<"  M_board Density =  "<<density/(CLHEP::g/CLHEP::cm3)<< "\n " << std::endl;
    #endif*/
    G4Material * Moth_elect = new G4Material(name="MBoards",density,ncomponents=2);
    // GU use fractions from NOVA ??
    //  Moth_elect->AddElement(elC,natoms=8);
    //  Moth_elect->AddElement(elH,natoms=14);
    //  Moth_elect->AddElement(elO,natoms=4);
    //  Moth_elect->AddElement(elCu,natoms=1); 
    Moth_elect->AddMaterial(Gten,fractionmass=FracMBG10);
    Moth_elect->AddElement(elCu,fractionmass= FracMBCu);
    /*#ifdef DEBUG_GEO
    std::cout << " Mother board  X0/Lambda = " << Moth_elect->GetRadlen() 
            << " " << Moth_elect->GetNuclearInterLength() << std::endl;
    #endif*/

    // GU 4/6/2004  material for the effective pin+summing board effect
    G4double ThSBCu = 0.28*CLHEP::mm;
    G4double ThSBAr = 9.72*CLHEP::mm;
    G4double TotalthickSB = ThSBCu+ThSBAr;
    G4double TotalmassSB = (ThSBCu*dcu+ThSBAr*dlar);
    G4double FracSBCu = (ThSBCu*dcu)/TotalmassSB;
    G4double FracSBAr = (ThSBAr*dlar)/TotalmassSB;
    density = TotalmassSB/TotalthickSB;
    /*#ifdef DEBUG_GEO
    G4cout<<"\n ---- Pin+Summing Board characteristics: ----" << G4endl;
    G4cout<<"  Fraction Cu, Ar: " << FracSBCu << "," << FracSBAr << G4endl;
    G4cout<<"  Total mass, Thickness:"
        << TotalmassSB <<" ," <<TotalthickSB<< G4endl;
    G4cout<<"  Density =  "<<density/(g/cm3)<< "\n " << G4endl;
    #endif*/
    G4Material * Summing_board = new G4Material(name="SBoards",density,ncomponents=2);
    Summing_board->AddElement(elAr,fractionmass=FracSBAr);
    Summing_board->AddElement(elCu,fractionmass=FracSBCu);
    /*#ifdef DEBUG_GEO
    std::cout << " Summing board  X0/Lambda = " << Summing_board->GetRadlen()
            << " " << Summing_board->GetNuclearInterLength() << std::endl;
    #endif*/

    // End of MATERIALS implementation. 


    //-------------------------------------------------------------------------
    // Detector geometry
    //-------------------------------------------------------------------------

    // Define some color constants and visibility attributes.

    //LArG4VisualConsultant* g4vis = LArG4VisualConsultant::GetInstance();

    // Get the pointer to the singleton sensitive detector consultant.
    
    //LArG4SensitiveDetectorConsultant* g4sdc = LArG4SensitiveDetectorConsultant::GetInstance();

    // The base prefix for all volume names.
    G4String baseName = "LAr::EMB::";

    // Phi coordinate for the first absorber neutral fiber
    //Gama0 = m_parameters->GetValue("LArEMBAbsPhiFirst");
    Gama0 = 0.003068*deg; //from atlas db and MySqlEMB.cxx
    /*#ifdef DEBUG_GEO
    std::cout << "Phi angle of first absorber " << Gama0/deg << std::endl;
    #endif*/

    //Moth_Z_min = m_parameters->GetValue("LArEMBMotherZmin");
    Moth_Z_min = 0.3*cm; //from atlas db and MySqlEMB.cxx
    // Moth_Z_max as the maximum Z value for the  Barrel  Volume is 3165mm
    //Moth_Z_max = m_parameters->GetValue("LArEMBMotherZmax");
    Moth_Z_max = 316.5*cm; //from atlas db and MySqlEMB.cxx
    // Moth_Z_max as the maximum Z value for the FIDUCIAL Volume is 3150mm
    //Moth_Z_max = m_parameters->GetValue("LArEMBfiducialMothZmax");
    Moth_Z_max = 315.0*cm;
    /*#ifdef DEBUG_GEO
     std::cout << "ECAM Zmin and Zmax " << Moth_Z_min
                << " " << Moth_Z_max << std::endl;
    #endif*/

    // ----------------------------------------------------------------------
    // ACCB  Mother volume for the mother for the Accordion 
    // to be implemented  in ELAM
    //-----------------------------------------------------------------------

    //Moth_inner_radius = m_parameters->GetValue("LArEMBMotherRmin"); // 1447.0
    Moth_inner_radius = 144.7*cm; //from atlas db and MySqlEMB.cxx
    //Moth_outer_radius = m_parameters->GetValue("LArEMBMotherRmax");  // 2003.5
    Moth_outer_radius = 200.35*cm; //from atlas db and MySqlEMB.cxx 

    /*#ifdef DEBUG_GEO
    std::cout << "ECAM inner and outer R " << Moth_inner_radius
            << " " << Moth_outer_radius << std::endl;
    #endif*/

    // WARNING : for the time being those two parameters don't exist in NOVA
    G4double Moth_Phi_Min = 0.0*deg; 
          //LArBarrelCalculator::s_phiMinBarrel * CLHEP::deg;//0.0 * deg;
    G4double Moth_Phi_Max = 24*deg; //from MySqlEMB.cxx this number is set to 360deg i.e. 4pi -> to be changed
                                    //correction ad hoc by lorenzo pezzotti
           //m_parameters->GetValue("LArEMBphiMaxBarrel")*CLHEP::deg;
    /*#ifdef DEBUG_GEO
    std::cout << "phi0 and Dphi " << Moth_Phi_Min
                << " " << Moth_Phi_Max << std::endl;
    #endif*/

    // Construct the container volume (Accordion plates, G10 bars, etc...) 
    // for the LAr in the Barrel 
    // That is the volume ECAM (to be placed in the ACCB volume)
    //

    // Number of Zig's +1 for the Accordion
    //Nbrt = (int) ( m_parameters->GetValue("LArEMBnoOFAccZigs") ); // =14
    Nbrt = (int) 14.0; //from atlas db and MySqlEMB.cxx
    Nbrt1 = Nbrt+1;

    // G10 and electronics in front/back of the barrel
    //Xel1f = m_parameters->GetValue("LArEMBInnerElectronics"); // = 23mm
    Xel1f = 23*mm;
    //Xtal = m_parameters->GetValue("LArEMBLArGapTail");        // = 13mm
    Xtal = 13*mm + 0.1*CLHEP::mm;
    //Xg10f = m_parameters->GetValue("LArEMBG10SupportBarsIn");  // = 20mm
    Xg10f = 20*mm;
    //Xg10b = m_parameters->GetValue("LArEMBG10SupportBarsOut");  // =20mm
    Xg10b = 20*mm;

    // Z coordinates for half barrel in ACCB frame.
    G4double Bar_Z_min = Moth_Z_min;
    G4double Bar_Z_max = Moth_Z_max;

    // Radius of the first fold
    //Rhocen1 = m_parameters->GetValue("LArEMBRadiusAtCurvature",0);
    Rhocen1 = 150.002*cm; //from atlas db
    //Rhocen15 = m_parameters->GetValue("LArEMBRadiusAtCurvature",14);
    Rhocen15 = 197.948*cm;

    // R and Z coordinates of ETA_cuts.
    //Bar_Eta_cut = (double) (m_parameters->GetValue("LArEMBMaxEtaAcceptance")); 
    Bar_Eta_cut = (double) 1.475; //from atlas db
    //Bar_Eta_cut1 = (double) (m_parameters->GetValue("LArEMBThickEtaAcceptance"));
    Bar_Eta_cut1 = (double) 0.8; //from atlas db
    /*#ifdef DEBUG_GEO
    std::cout << " Bar_Eta_cut " << Bar_Eta_cut << std::endl;
    std::cout << " Bar_Eta_cut1 " << Bar_Eta_cut1 << std::endl;
    #endif*/

    G4double Bar_Z_cut,Bar_Z_cut1, Tetac, Tetac1 , Bar_Rcmx ;
    Tetac = 2.*atan(exp(-Bar_Eta_cut));
    Bar_Z_cut = Rhocen1/tan(Tetac);
    Bar_Rcmx  = Bar_Z_max*tan(Tetac);
    Tetac1 = 2.*atan(exp(-Bar_Eta_cut1));
    Bar_Z_cut1 = Rhocen1/tan(Tetac1); 
    G4double Zp0c; 
    G4double Zp0 = 0.;

    // add now safety on Rhocen1 
    Rhocen1 = Rhocen1  - 0.040*CLHEP::mm;

    G4double Zhalf,Zhalfc;
    // Half-length of this Half barrel
    Zhalf = (Bar_Z_max-Bar_Z_min)/2.;
    // Half-length of the half barrel up to Zcut
    Zhalfc = (Bar_Z_cut-Bar_Z_min)/2.;
    G4double Rmini,Rmaxi,DeltaZ,Zpos;
    G4double Rmini1,Rmaxi1,Rmini2,Rmaxi2;

    // Accordion shape parameters:
    // rho phi of curvature centers and delta's 
    // IN 2D_transverse LOCAL framework of a layer and symetry axis as X_axis 
    // delta's are GEOMETRICAL angles of straight parts w.r. the Y_axis )
 
    //Take values directly from atlas db, no for loop
    //
    double Rhocen_tmp[15] = {150.002*cm,152.1*cm,155.966*cm,159.72*cm,163.457*cm,167.102*cm,170.743*cm,174.307*cm,177.876*cm,181.375*cm,184.887*cm,188.336*cm,191.802*cm,195.21*cm,197.048*cm};
    double Phicen_tmp[15] = {0.106187*deg,0.569751*deg,-0.573092*deg,0.576518*deg,-0.579943*deg,0.582296*deg,-0.585638*deg,0.588207*deg,-0.590596*deg,0.59285*deg,-0.595587*deg,0.59744*deg,-0.599714*deg,0.601911*deg,0.0811661*deg};
    double Delta_tmp[15] = {46.2025*deg,45.0574*deg,43.3446*deg,42.4478*deg,40.9436*deg,40.2251*deg,38.8752*deg,38.2915*deg,37.0608*deg,36.5831*deg,35.4475*deg,35.0556*deg,33.9977*deg,33.6767*deg,90.*deg};
    double deltay_tmp[15] = {0.*cm,0.017*cm,0.03*cm,0.063*cm,0.078*cm,0.106*cm,0.109*cm,0.121*cm,0.107*cm,0.103*cm,0.074*cm,0.061*cm,0.027*cm,0.02*cm,0.*cm};
    for (G4int idat = 0; idat < 15 ; idat++) {
        //Rhocen[idat] = (double) (m_parameters->GetValue("LArEMBRadiusAtCurvature",idat));
        Rhocen[idat] = Rhocen_tmp[idat];
        //Phicen[idat] = (double) (m_parameters->GetValue("LArEMBPhiAtCurvature",idat));
        Phicen[idat] = Phicen_tmp[idat];
        //Delta[idat]  = (double) (m_parameters->GetValue("LArEMBDeltaZigAngle",idat));
        //if(idat == 14) Delta[idat]  = (90.0) * CLHEP::deg;
        Delta[idat] = Delta_tmp[idat];
        //deltay[idat] = (double) (m_parameters->GetValue("LArEMBSaggingAmplitude",idat));
        deltay[idat] = deltay_tmp[idat];

        /*#ifdef DEBUG_GEO
        std::cout << "idat " << idat << " Rhocen/Phice/Delta/deltay "
        << Rhocen[idat] << " " << Phicen[idat]/CLHEP::deg << " "
        << Delta[idat]/CLHEP::deg << " " << deltay[idat] << std::endl;
        #endif*/
    }
   
    //
    // The volume ECAM (Barrel volume)
    //
    G4double Zplan[] = {Bar_Z_min-Zp0,Bar_Z_cut-Zp0,Bar_Z_max-Zp0};
    G4double Riacc[] = {Moth_inner_radius,Moth_inner_radius, Rhocen1};
    G4double Roacc[] = {Moth_outer_radius,Moth_outer_radius,Moth_outer_radius};  
    G4int ecamArraySize = sizeof(Zplan) / sizeof(G4double);

    // Define the volume into which we'll place all other EMB elements.
    G4String ecamName = baseName + "ECAM";
    // ==GU 11/06/2003
    //  in test beam case need to have STAC between -1.055 and 24.61 degrees
    //  (i.e. a little bit wider than one calorimeter module)
    // =GU 10/09/04
    //  have ECAM volume sligthly wider than STAC to avoid G4 tracking becoming
    //   very confused at the common surface between ECAM and STAC
    //int Ncell = (int) (m_parameters->GetValue("LArEMBnoOFPhysPhiCell"));
    int Ncell = 64; //attempt for test-beam geometry
    G4double Moth_Phi_Min2;
    G4double Moth_Phi_Max2;
    G4double Moth_Phi_Min3;
    G4double Moth_Phi_Max3;
    if (Ncell == 64) {              // test beam case: One module
        Moth_Phi_Min2 = -1.055*CLHEP::deg;
        Moth_Phi_Max2 = 24.61*CLHEP::deg;
        Moth_Phi_Min3 = -1.555*CLHEP::deg;
        Moth_Phi_Max3 = 25.61*CLHEP::deg;
    }
    else {                         // all other cases = full Atlas
        Moth_Phi_Min2 = 0.;
        Moth_Phi_Max2 = 2*M_PI;
        Moth_Phi_Min3 = 0.;
        Moth_Phi_Max3 = 2*M_PI;
    }

    /*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << "*** ecam Parameters " << std::endl;
    std::cout << "    Phi0, Dphi " << Moth_Phi_Min2 << " "
                                 << Moth_Phi_Max2 << std::endl;
    for (int i=0; i<ecamArraySize;i++) {
        std::cout << "   Z/Rin/Rout " << Zplan[i] << " "
             << Riacc[i] << " "  << Roacc[i] << std::endl;
    }
    #endif*/

    G4Polycone* ecamShape =   new G4Polycone(ecamName, 
		                            Moth_Phi_Min3, 
		                            Moth_Phi_Max3, 
		                            ecamArraySize, 
		                            Zplan, 
		                            Riacc, 
		                            Roacc);
    G4LogicalVolume* ecamLogical = new G4LogicalVolume(ecamShape, LAr, ecamName);
    //g4vis->SetVis( ecamLogical );
    //g4sdc->SetSD( ecamLogical );

    //Skipping electronics for the moment
    //G4double clearance = 1.3*CLHEP::mm; //copied from electronic part
    
    #ifdef BUILD_FRONT_ELECTRONICS

    //        Front Electronics are in a TUBE

    // 1) The TUBE
    name = baseName + "TELF";

    // WARNING : this "hard_coded" 0.010*CLHEP::mm is a "security" to avoid
    //           fake "overlapping" diagnostics with "DAVID" 
    Rmini =  Moth_inner_radius + 0.010*CLHEP::mm;
    Rmaxi = Rmini+Xel1f - 0.010*CLHEP::mm;
    DeltaZ = Zhalfc;

    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** TELF volume " << std::endl;
    std::cout << " Rmin/Rmax/DeltaZ" << Rmini << " "
                << Rmaxi << " " << DeltaZ << std::endl;
    #endif
    G4Tubs *Elnicsf_tub = new G4Tubs(name,
				     Rmini,
				     Rmaxi,
				     DeltaZ,
				     Moth_Phi_Min,
                                     Moth_Phi_Max);

    G4LogicalVolume *Elnicsf_log = new G4LogicalVolume(Elnicsf_tub,
						       LAr, 
						       name);

    //         Attributes for Front ELECTRONICS
    //g4vis->SetVis( Elnicsf_log );
    //g4sdc->SetSD( Elnicsf_log );
  
    Zpos = Zhalfc+Bar_Z_min; // 08-Jan-2002 ML: Position relative to LAr G4Polycone
    #ifdef DEBUG_GEO
    std::cout << " position of TELF in ECAM " << Zpos << std::endl;
    #endif
    G4VPhysicalVolume *Elnicsf_phys = 	new G4PVPlacement(0,
			  G4ThreeVector(0.,0.,Zpos),
			  Elnicsf_log, 
			  name, 
			  ecamLogical, 
			  false, 
			  1);

    //GU 3/6/2004:  Effective mixture of Cu+Ar to represent ping+summing
    // board effect (mixture described in Pascal's note)
    // 1cm thick tube
    G4double ThickSum=10.*CLHEP::mm;
    name = baseName + "SUMB";
    Rmini = Moth_inner_radius+Xel1f-ThickSum;
    Rmaxi = Moth_inner_radius+Xel1f-0.020*CLHEP::mm;  // safety margin
    DeltaZ = Zhalfc;
    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** Pin/ Summing board volume " << std::endl;
    std::cout << " Rmin/Rmax/DeltaZ" << Rmini << " "
              << Rmaxi << " " << DeltaZ << std::endl;
    #endif

    G4Tubs *Summing_tub = new G4Tubs(name,Rmini,Rmaxi,DeltaZ,
                                   Moth_Phi_Min,Moth_Phi_Max);

    G4LogicalVolume *Summing_log = new G4LogicalVolume(Summing_tub,
                                                     Summing_board,
                                                     name);
    G4VPhysicalVolume *Summing_phys =  
           new G4PVPlacement(0,G4ThreeVector(0.,0.,0.),name,
                             Summing_log,Elnicsf_phys,false,1);

    //g4sdc->SetSD( Summing_log );


    // 2) Solids and Logical Volumes for Mother_Boards and cables
     
    // 2.1  Mother_board

    name = baseName + "MOAC";
    //bdx = .5*(m_parameters->GetValue("LArEMBMoBoTchickness"));
    bdx = 0.5*0.43*cm; //from atlas db
    //bdy = .5*(m_parameters->GetValue("LArEMBMoBoHeight"));
    bdy = 0.5*7.23*cm; //from atlas db
    bdz = Zhalfc - 0.007*CLHEP::mm;

    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " *** MOAC volume (Mother boards) " << std::endl;
    std::cout << " dx/dy/dz " << bdx << " "
            << bdy << " " << bdz << std::endl;
    #endif

    G4Box *elboardf_box = new G4Box(name,bdx,bdy,bdz);

    G4LogicalVolume *elboardf_log = new G4LogicalVolume(elboardf_box,
							Moth_elect,
							name);
    //g4vis->SetVis( elboardf_log );
    //g4sdc->SetSD( elboardf_log );
    
    // 2.2  Cables
    
    name = baseName + "CAAC";

    G4double Dzc = Zhalfc - 0.007*CLHEP::mm;
    //Dx1 = .5*(m_parameters->GetValue("LArEMBCablethickat0"));
    Dx1 = 0.5*0.1*cm; //from atlas db
    //Dx2 = .5*Bar_Eta_cut*(m_parameters->GetValue("LArEMBthickincrfac"));
    Dx2 = 0.5*Bar_Eta_cut*0.517*cm; //from atlas db
    //Dy1 = .5*(m_parameters->GetValue("LArEMBCableEtaheight"));
    Dy1 = 0.5*7.*cm; //from atlas db
    Dy2 = Dy1;

    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " *** CAAC volume (cables) " << std::endl;
    std::cout << " half z " << Dzc << std::endl;
    std::cout << " Dx1,Dy1 " << Dx1 << " " << Dy1 << std::endl;
    std::cout << " Dx2,Dy2 " << Dx2 << " " << Dy2 << std::endl;
    #endif
  
    G4Trap *elcablesf_trap = new G4Trap(name,
					Dx1, 
					Dx2, 
					Dy1, 
					Dy2, 
					Dzc);
 
    G4LogicalVolume *elcablesf_log = new G4LogicalVolume(elcablesf_trap,
							 Cable_elect,
							 name);

    //g4vis->SetVis( elcablesf_log );
    //g4sdc->SetSD( elcablesf_log );

    // 3) Physical Volumes -> BOARDS and CABLES
    G4double PhiOrig = 0.;
    G4double twopi64 = M_PI/32.;
    G4double twopi32 = 2.*twopi64;
    G4double PhiPos0, PhiPos00, PhiPos1, PhiPos2, PhiPos, Xpos, Ypos, Zpose;
      
    // 3.1  Mother_Boards
    //NoOFboard = (int) m_parameters->GetValue("LArEMBnoOFmothboard"); //32
    NoOFboard = (int) 2; //from atlas db (32 for 4pi geometry)
    //ClearancePS = m_parameters->GetValue("LArEMBMoBoclearfrPS");
    ClearancePS = 0.6*cm;
    G4double RhoPosB = Moth_inner_radius + ClearancePS; 
    PhiPos0 = twopi64 + PhiOrig;
    
    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << "position Mother board in TELF " << std::endl;
    std::cout << "number of copies " << NoOFboard << std::endl;
    std::cout << "Radius " << RhoPosB << std::endl;
    std::cout << "First phi " << PhiPos0/deg << " every " << twopi32/deg << std::endl;
    #endif

    for (int bcopy = 0; bcopy<NoOFboard; bcopy++)
    {
        PhiPos = PhiPos0 + bcopy*twopi32;
        Xpos = cos(PhiPos)*RhoPosB;      
        Ypos = sin(PhiPos)*RhoPosB;
        Zpose = 0.;
        G4RotationMatrix *rmb = new G4RotationMatrix;
        rmb-> rotateZ(-PhiPos);
    
        G4VPhysicalVolume *elboardf_phys = 
	        new G4PVPlacement(rmb, 
			      G4ThreeVector(Xpos, Ypos, Zpose),
			      baseName + "MOAC", 
			      elboardf_log, 
			      Elnicsf_phys,
			      false, 
			      bcopy);   
    }
    
    // 3.2  Cables
    //NoOFcable = (int) m_parameters->GetValue("LArEMBnoOFcableBundle"); //64
    NoOFcable = (int) 4; //from atlas db
    //ClearancePS = m_parameters->GetValue("LArEMBCablclearfrPS");
    ClearancePS = 0.6*cm; //from atlas db
    G4double RhoPosC = Moth_inner_radius + ClearancePS;
    PhiPos0 = (twopi64 - asin(bdy/RhoPosB))/2.;
    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << "position cables       in TELF " << std::endl;
    std::cout << "number of copies " << NoOFcable << std::endl;
    std::cout << "Radius " << RhoPosC << std::endl;
    #endif
    for (int ccopy = 0; ccopy<NoOFcable; ccopy++)
    {
        PhiOrig = 22.5*CLHEP::deg * int(ccopy/4);
        PhiPos1 = PhiPos0 + PhiOrig;
        PhiPos2 = twopi32 - PhiPos0 +PhiOrig;
        PhiPos00 = PhiPos1;
        if( ((ccopy%4) == 2) || ((ccopy%4) == 3) ) PhiPos00 = PhiPos2;
        PhiPos = PhiPos00 + (ccopy%2)*twopi32;
        #ifdef DEBUG_GEO
        std::cout << "copy/phi " << ccopy << " " << PhiPos/deg << std::endl;
        #endif
        Xpos = cos(PhiPos)*RhoPosC;
        Ypos = sin(PhiPos)*RhoPosC;
        Zpose = 0.;
        G4RotationMatrix *rmc = new G4RotationMatrix;
        rmc-> rotateZ(-PhiPos);
        G4VPhysicalVolume *elcablesf_phys = 
	        new G4PVPlacement(rmc, 
			      G4ThreeVector(Xpos, Ypos, Zpose), 
			      baseName + "CAAC", 
			      elcablesf_log, 
			      Elnicsf_phys,
			      false, 
			      ccopy);  
    }    
    #endif // BUILD_FRONT_ELECTRONICS

    // add 1.3 mm in z to allow cleareance for absorber with non
    // 0 thickness, at eta=1.475, low r part of the barrel
    // this affects STAC and TELB volumes
    G4double clearance = 1.3*CLHEP::mm;

    #ifdef BUILD_HIGHETA_ELECTRONICS


    //        Back Electronics are in an extruded CONE

    name = baseName + "TELB";

    Rmini1 = Rhocen[0] - .030*CLHEP::mm;
    Rmaxi1 = Rhocen[0] - .020*CLHEP::mm;
    Rmini2 = Rhocen[0] - .030*CLHEP::mm;
    Rmaxi2 = Bar_Rcmx - clearance - .020*CLHEP::mm;
    // GU fix of TELB
    //  G4double transl = 3.5*CLHEP::mm;
    G4double transl = 0.;
    DeltaZ = Zhalf-Zhalfc-clearance/2. -transl/2. -0.001*CLHEP::mm;  // 1 micron for safety

    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** TELB volume (Cons)" << std::endl;
    std::cout << "phi0,Dphi " << Moth_Phi_Min << " " << Moth_Phi_Max << std::endl;
    std::cout << "halfZ " << DeltaZ << std::endl;
    std::cout << "Rmini1/Rmaxi1 " << Rmini1 << " " << Rmaxi1 << std::endl;
    std::cout << "Rmini2/Rmaxi2 " << Rmini2 << " " << Rmaxi2 << std::endl;
    #endif

    G4Cons *Elnicsb_con = new G4Cons(name,
                 		     Rmini1,
				     Rmaxi1,
				     Rmini2,
				     Rmaxi2,
				     DeltaZ,
				     Moth_Phi_Min,
				     Moth_Phi_Max);

    G4LogicalVolume *Elnicsb_log = new G4LogicalVolume(Elnicsb_con,
						       Cable_elect, 
						       name);

    //          Attributes for Back ELECTRONICS
    //g4vis->SetVis( Elnicsb_log );
    //g4sdc->SetSD( Elnicsb_log );
  
    Zpos = Zhalfc + transl;
    Zpos = Bar_Z_cut+DeltaZ+clearance; // 04-Jan-2002 WGS: Position relative to LAr G4Polycone
    #ifdef DEBUG_GEO
    std::cout << "Position TELB in Ecam Zpos = " << Zpos << std::endl;
    #endif
    G4VPhysicalVolume *Elnicsb_phys = 
        	new G4PVPlacement(0,
			  G4ThreeVector(0.,0.,Zpos),
			  Elnicsb_log, 
			  name, 
			  ecamLogical, 
			  false, 
			  1);
    #endif // BUILD_HIGHETA_ELECTRONICS



    // Front and back G10 : TUBES in ECAM

    #ifdef BUILD_FRONT_G10

    //   FRONT G10
           
    name = baseName + "GTENF";

    Rmini = Moth_inner_radius + Xel1f;
    Rmaxi = Rmini+Xg10f;
    DeltaZ = Zhalfc;
    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** GTENF volume (Tubs) front G10 " << std::endl;
    std::cout << "HalfZ " << DeltaZ << std::endl;
    std::cout << "Rmini,Rmaxi " << Rmini << " " << Rmaxi << std::endl;
    #endif
    G4Tubs *G10f_tub = new G4Tubs(name,
				  Rmini,
				  Rmaxi,
				  DeltaZ,
				  Moth_Phi_Min,
                                  Moth_Phi_Max);

    G4LogicalVolume *G10f_log = new G4LogicalVolume(G10f_tub,
						    Gten_bar, 
						    name);

    //       Attributes for Front G10
    //g4vis->SetVis( G10f_log );
    //g4sdc->SetSD( G10f_log );
  
    Zpos = Zhalfc+Bar_Z_min; // 04-Jan-2002 WGS: Position relative to LAr G4Polycone
    #ifdef DEBUG_GEO
    std::cout << "Position GTENF in ECAM Zpos = " << Zpos << std::endl;
    #endif
    G4VPhysicalVolume *G10f_phys = 
        	new G4PVPlacement(0,
			  G4ThreeVector(0.,0.,Zpos),
			  G10f_log, 
			  name, 
			  ecamLogical, 
			  false, 
			  1);

    #endif // BUILD_FRONT_G10

    #ifdef BUILD_BACK_G10

    //   BACK G10

    name = baseName +"GTENB";

    Rmini = Rhocen[Nbrt]+Xtal; // Why not Rhocen[Nbrt+1] + Xtal ?
    // GU to be sure that GTENB does not go outside mother ECAM volume
    //  Rmaxi = Rmini+Xg10b;
    Rmaxi = Moth_outer_radius-0.01*CLHEP::mm;   // 10 microns for more safety..
    DeltaZ = Zhalf;
    #ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** GTENB volume (Tubs) back  G10 " << std::endl;
    std::cout << "HalfZ " << DeltaZ << std::endl;
    std::cout << "Rmini,Rmaxi " << Rmini << " " << Rmaxi << std::endl;
    #endif
    G4Tubs *G10b_tub = new G4Tubs(name,
				  Rmini,
				  Rmaxi,
				  DeltaZ,
				  Moth_Phi_Min,
                                  Moth_Phi_Max);

    G4LogicalVolume *G10b_log = new G4LogicalVolume(G10b_tub,
						    Gten_bar, 
						    name);

    //       Attributes for Back G10
    //g4vis->SetVis( G10b_log );
    //g4sdc->SetSD( G10b_log );
    
    Zpos = Zhalf+Bar_Z_min; // 08-Jan-2002 ML: Position relative to LAr G4Polycone
    #ifdef DEBUG_GEO
    std::cout << "Position GTENB in ECAM Zpos = " << Zpos << std::endl;
    #endif
    G4VPhysicalVolume *G10b_phys = 
        	new G4PVPlacement(0,
			  G4ThreeVector(0.,0.,Zpos),
			  G10b_log, 
			  name, 
			  ecamLogical, 
			  false, 
			  1);

    #endif // BUILD_BACK_G10
    










    // Put in place the volume STAC (ACCORDION) in the ECAM  volume     
    //#ifdef BUILD_STAC
    name = baseName + "STAC";

    //Psi = m_parameters->GetValue("LArEMBPhiGapAperture");
    Psi = 0.3515625*deg; //from atlas db
    /*#ifdef DEBUG_GEO
    std::cout << " Dphi between two absorbers " << Psi/deg << std::endl;
    #endif*/
    G4double Zplan1[] = {Bar_Z_min,Bar_Z_cut+clearance,Bar_Z_max};
    G4double Riacc1[] = {Rhocen[0],Rhocen[0], Bar_Rcmx-clearance};
    G4double Roacc1[] = {Rhocen[Nbrt],Rhocen[Nbrt],Rhocen[Nbrt]};
    /*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** Volume Stac (Pcon) " << std::endl;
    std::cout << "Phi0/Dphi " << Moth_Phi_Min2 << " " << Moth_Phi_Max2 << std::endl;
    for (int i=0;i<3;i++) {
        std::cout << "Plane Z/Ri/Ro " << Zplan1[i] << " "
             << Riacc1[i] << " " << Roacc1[i] << std::endl;
    }
    #endif*/
  
    G4Polycone *Stac_pcone = new G4Polycone(name,
					    Moth_Phi_Min2,
                                            Moth_Phi_Max2, 
					    3, 
					    Zplan1,
					    Riacc1,
					    Roacc1);

    G4LogicalVolume *Stac_log = new G4LogicalVolume(Stac_pcone,
						    LAr, 
						    name);
    // Attributes for the ACCORDION volume
    //g4vis->SetVis( Stac_log );
    //g4sdc->SetSD( Stac_log );

    // Reduce the memory usage with the help of voxels
    // NOTE: 
    //   STAC is the volume with the most subvolumes contained directely in it

    Stac_log->SetSmartless(0.2);

    /*#ifdef DEBUG_GEO
    std::cout << "Position STAC in ECAM " << -Zp0 << std::endl;
    #endif*/
    Stac_phys1 = 
            new G4PVPlacement(0, 
	    		  G4ThreeVector(0., 0.,-Zp0), 
			  Stac_log, 
			  name, 
			  ecamLogical, 
			  false, 
			  1);

    //#ifdef BUILD_ACCORDION_PLATES
    // Construct the straight and the corner parts of the Accordion plates
 
    //Rint = m_parameters->GetValue("LArEMBNeutFiberRadius");
    Rint = 0.278*cm; //from atlas db
    //Xtip_pb = m_parameters->GetValue("LArEMBLeadTipThickFront");
    Xtip_pb = 0.2*cm; //from atlas db
    //Xtip_pc = m_parameters->GetValue("LArEMBLeadTipThickEnd");
    Xtip_pc = 1.3*cm; //from atlas db
    //Xtip_gt = m_parameters->GetValue("LArEMBG10TipThickFront");
    Xtip_gt = 0.8*cm; //from atlas db
    //Xtip_gs = m_parameters->GetValue("LArEMBG10TipThickEnd");  
    Xtip_gs = 0.*cm; //from atlas db


    /*#ifdef DEBUG_GEO
    std::cout << "Rint (NeutFiberRadius) " << Rint << std::endl;
    std::cout << "Xtip_pb (LArEMBLeadTipThickFront) " << Xtip_pb << std::endl;
    std::cout << "Xtip_pc (LArEMBLeadTipThickEnd  ) " << Xtip_pc << std::endl;
    std::cout << "Xtip_gt (LArEMBG10TipThickFront)  " << Xtip_gt << std::endl;
    std::cout << "Xtip_gs (LArEMBG10TipThickEnd  )  " << Xtip_gs << std::endl;
    #endif*/
    G4double Xhalfb,Zhalfb,radius;
    G4double Xhalfbe,Zhalfbe,radiuse;
    G4double Xhalfbg,Zhalfbg,radiusg;
    G4double Thce = Thpb+Thgl+Thfe;
    G4double Thel = Thcu+Thfg;
    /*#ifdef DEBUG_GEO
    std::cout << "total thickness of Absorber  " << Thce << std::endl;
    std::cout << "total thickness of electrode " << Thel << std::endl;
    #endif*/
    G4double Xtips = Xtip_pb + Xtip_gt;
    G4double Xtipt = Xtip_pc + Xtip_gs;
    G4double Zcon1,Zcon2,dz0,dza,dz01,dza1;
    Alfa = Psi;
    G4double Gama,Game;
    G4double Zmin = Bar_Z_min;
    G4double Zmax = Bar_Z_max;

    // Zcp1 = z max for eta=0.8 at the radius of the middle of the fold
    // Zcp2 = z max for eta=1.475 at the radius of the middle of the fold
    // Along = lenght of the straight sections
    G4double Zcp1[15],Zcp2[15],Along[14],Adisc2,Along2;

    // Rhol = radius at the beginning of the straight section
    // Rhoh = radius at the end of the straight section
    // Zcp1l = z max for eta=0.8 at the beginning of the straight section
    // Zcp1l = z max for eta=0.8 at the end of the straight section
    // Zcp2l = z max for eta=1.475 at the beginning of the straight section
    // Zcp2l = z max for eta=1.475 at the end of the straight section
    G4double Zcp1l[14],Zcp1h[14],Zcp2l[14],Zcp2h[14];
    G4double Rhol[14],Rhoh[14];

    G4double safety_along = 0.007*CLHEP::mm;
    
    //Nplus = (int) (m_parameters->GetValue("LArEMBnoOFPhysPhiCell"));
    Nplus = (int) 64; //from atlas db
    /*#ifdef DEBUG_GEO
    std::cout << "total number of phicell " << Nplus << std::endl;
    #endif*/
    
    // Compute centers of curvature coordinates in a local frame.

    G4double Cenx[15], Ceny[15] ;
    for (int jf=0; jf<Nbrt1; jf++)
    {
        Cenx[jf] = Rhocen[jf]*cos(Phicen[jf]); // Phicen[] already in radians
        Ceny[jf] = Rhocen[jf]*sin(Phicen[jf]);
        Zcp1[jf] = Rhocen[jf]/tan(Tetac1);
        Zcp2[jf] = Rhocen[jf]/tan(Tetac);
        /*#ifdef DEBUG_GEO
        std::cout << " jf " << jf
        << "Cenx/Ceny " << Cenx[jf] << " " << Ceny[jf] << " "
        << "Zcp1/Zcp2 " << Zcp1[jf] << " " << Zcp2[jf] << std::endl;
        #endif*/
    }
    /*#ifdef DEBUG_GEO
    std::cout<<" Zcut =  "<<Bar_Z_cut<<"  "<<" Zcut1 =  "<<Bar_Z_cut1<< std::endl;
    #endif*/

    // Compute staight lengths of folds
    for (int jl=0; jl<Nbrt; jl++)
    {
        Adisc2 = (Cenx[jl+1]-Cenx[jl])*(Cenx[jl+1]-Cenx[jl])+
                 (Ceny[jl+1]-Ceny[jl])*(Ceny[jl+1]-Ceny[jl]);
        Along2 = Adisc2 - 4.*Rint*Rint;
        Along[jl] = sqrt(Along2);
        /*#ifdef DEBUG_GEO
        std::cout << "jl " << jl
                << "straight length " << Along[jl] << std::endl;
        #endif*/
        G4double ddelta = M_PI/2.-Delta[jl];
        if (jl%2==1) ddelta=-1.*ddelta;
        G4double xlong=Along[jl]-2.*safety_along;
        if(A_SAGGING) xlong=Along[jl]-2.*safety_along;
        G4double x2=0.5*(Cenx[jl+1]+Cenx[jl])+0.5*xlong*cos(ddelta);
        G4double y2=0.5*(Ceny[jl+1]+Ceny[jl])+0.5*xlong*sin(ddelta);
        G4double x1=0.5*(Cenx[jl+1]+Cenx[jl])-0.5*xlong*cos(ddelta);
        G4double y1=0.5*(Ceny[jl+1]+Ceny[jl])-0.5*xlong*sin(ddelta);
        Rhol[jl] = sqrt(x1*x1+y1*y1);
        Rhoh[jl] = sqrt(x2*x2+y2*y2);
        Zcp1l[jl] = Rhol[jl]/tan(Tetac1);
        Zcp1h[jl] = Rhoh[jl]/tan(Tetac1);
        Zcp2l[jl] = Rhol[jl]/tan(Tetac);
        Zcp2h[jl] = Rhoh[jl]/tan(Tetac);
        /*#ifdef DEBUG_GEO
        std::cout << "x1,y1,x2,y2 " << x1 << " " << y1 << " " << x2 << " "
                  << y2 << std::endl;
        std::cout << "  lower/upper radius " << Rhol[jl] << " " << Rhoh[jl]
                  << " Z for eta0.8 " << Zcp1l[jl] << " " << Zcp1h[jl]
               << " Z for etamax " << Zcp2l[jl] << " " << Zcp2h[jl] << std::endl;
        #endif*/
    }

    // Inner&outer radii of the tubes describing the corner(Fold) parts of 
    // the Accordion plates
    // Rayons internes et externes pour les tubes des coudes
    G4double Rcmin,Rcmax;
    G4double Rcmine,Rcmaxe;
    Rcmin=Rint-Thce/2.;
    Rcmax=Rint+Thce/2.;
    Rcmine=Rint-Thel/2.;
    Rcmaxe=Rint+Thel/2.;
 
    // Variables position 
    G4double Xcd,Ycd,Zcd;
    G4double Xcde,Ycde,Zcde;
    G4double Dz,h1,bl1,tl1,alp1,Dze;
    G4double Zx0,Xb1,Xt1,Xtrans;


    // Loop through the straight and corner(Fold) parts of the Accordion plates
    // Repeat each part around Phi, then move to the next, towards outer radii

    // Define some commonly-used volume names.
    G4String fbName         = baseName + "FrontBack::Absorber";
    G4String fbName2        = baseName + "FrontBack::Absorber2";
    G4String fbeName        = baseName + "FrontBack::Electrode";
    G4String fbgName        = baseName + "FrontBack::G10";
    G4String fbsName        = baseName + "FrontBack::Steel";
    G4String corndwName     = baseName + "ThinAbs::CornerDownFold";
    G4String corndwtName    = baseName + "ThickAbs::CornerDownFold";
    G4String corndweName    = baseName + "Electrode::CornerDownFold";
    G4String cornupName     = baseName + "ThinAbs::CornerUpFold";
    G4String cornuptName    = baseName + "ThickAbs::CornerUpFold";
    G4String cornupeName    = baseName + "Electrode::CornerUpFold";
    G4String straightName   = baseName + "ThinAbs::Straight";
    G4String straighttName  = baseName + "ThickAbs::Straight";
    G4String straighteName  = baseName + "Electrode::Straight";

    for (int irl=0; irl<Nbrt; irl++) {
        #ifdef DEBUG_GEO
        std::cout << " " << std::endl;
        std::cout << " ===== irl = " << irl << std::endl;
        #endif

        // 12-Feb-2003 WGS: For those volumes that support it, supply a
        // number (0 < ratio < 1) that's a function of fold number.
        // This causes the barrel's color to vary as a function of
        // radius.  (This is pretty; whether or not it's useful I leave
        // for others to decide.)
        G4float ratio = G4float(irl) / G4float(Nbrt);

        G4int iparit;
        iparit = (1-2*(irl%2));
        double newalf1, newalf1e, newalpha, newalphe, cosalfa, sinalfa;
        G4Tubs *Corndw_ptub;
        G4Tubs *Corndwt_ptub;
        G4Tubs *Cornup_ptub;
        G4Tubs *Cornupt_ptub;
        G4Tubs *Cornup_etub;
        G4Tubs *Corndw_etub;
        G4VSolid *Straight_solid;
        G4VSolid *Straight_solidt;
        G4VSolid *Straight_esolid;
        G4Box *Frtbck_box;
        G4Box *Frtbck2_box;
        G4Box *Frtbcke_box;
        G4Box *Frtbckg_box;
        G4Box *Frtbcks_box;
        G4LogicalVolume *Frtbck_log;
        G4LogicalVolume *Frtbck2_log;
        G4LogicalVolume *Frtbckg_log;
        G4LogicalVolume *Frtbcke_log;
        G4LogicalVolume *Frtbcks_log;
        G4LogicalVolume *Straight_log;
        G4LogicalVolume *Straight_elog;

        Zcon1 = Zcp2[irl];
        Zcon2 = Zcp2[irl+1];
        // if the radius at the lower edge of this fold is smaller than
        // Bar_rcmx, take the lenght at this radius instead of the radius
        // of the fold to avoid going outside the mother volume
        if (irl>0 && Rhoh[irl-1] < Bar_Rcmx) Zcon1=Zcp2h[irl-1];
        if (Rhoh[irl] < Bar_Rcmx) Zcon2=Zcp2h[irl];
        dz0 = (std::min(Zcon1,Zmax)-Zmin)/2.;  // half lenght in z at irl
        dza = (std::min(Zcon2,Zmax)-Zmin)/2.;
        dz01 = (std::min(Zcp1[irl],Zmax)-Zmin)/2.;  // half lenght for thick lead
        dza1 = (std::min(Zcp1[irl+1],Zmax)-Zmin)/2.;

        #ifdef DEBUG_GEO
        std::cout << "Zcon1,Zcon2 " << Zcon1 << "  " << Zcon2 << std::endl;
        #endif

        // Creation of the straight absorber parts. Front (TIPB) & Back (TIPC)
        // Creation of the straight electrode parts. Front (TIPK) & Back (TIPL)

        if (irl ==0 || irl == Nbrt-1) {
            // GU: add 2 micron safety to avoid overlap due to rounding errors
            Xhalfb = Xtip_pb/2 - .002*CLHEP::mm;
            Zhalfb = (Bar_Z_cut-Zmin)/2 ;
            Xhalfbe = Xtips/2 - .002*CLHEP::mm;
            Xhalfbg = Xtip_gt/2 - .002*CLHEP::mm;
            if (irl ==Nbrt-1) { 
                //	Xhalfb = Xtip_pc/2 - .001*CLHEP::mm;
                //	Zhalfb = (Zmax - Zmin)/2;
                //	Xhalfbe = Xtipt/2 - .001*CLHEP::mm;
                //	Xhalfbg = Xtip_gs/2 - .001*CLHEP::mm;

                // GU 29-09-2003   No G10 at the end of absorber, lead until the end
                // GU 09-06-2004  increase safety margin to 4 microns to avoid overlapping due to
                // rounding errors when positionning all volumes in phi
                Xhalfbe = Xtipt/2 - .004*CLHEP::mm;    // electrode
                Xhalfb  = Xhalfbe;              // absorber
                Zhalfb =  (Zmax - Zmin)/2;
            }
            Zhalfbe = Zhalfb;
            Zhalfbg = Zhalfb;
            #ifdef DEBUG_GEO
            std::cout << " " << std::endl;
            std::cout << " ** Volume FrontBack::Absorber (thin_abs) " << std::endl;
            std::cout << "Box half sizes " << Xhalfb << " "
                      << Thce/2. << " " << Zhalfb << std::endl;
            std::cout << " ** Volume FrontBack::Absorber (thick_abs)" << std::endl;
            std::cout << "Box half sizes " << Xhalfb << " "
                    << Thce/2. << " " << dz01 << std::endl;
            std::cout << " ** Volume FrontBack::Electrode " << std::endl;
            std::cout << "Box half sizes " << Xhalfbe << " "
                    << Thel/2. << " " << Zhalfbe << std::endl;
            if (irl==0) {
                std::cout << " ** Volume FrontBack::GTEN      " << std::endl;
                std::cout << "Box half sizes " << Xhalfbg << " "
                      << Thce/2. << " " << Zhalfbg << std::endl;
                std::cout << " ** Volume FrontBack::Steel     " << std::endl,
                std::cout << "Box half sizes " << Xhalfbg << " "
                      << Tgfe/4. << " " << Zhalfbg << std::endl;
            }
            #endif
            if (irl==0) {
                Frtbck_box = new G4Box(fbName,Xhalfb,Thce/2.,Zhalfb);
                Frtbck2_box = new G4Box(fbName2,Xhalfb,Thce/2.,dz01);
                Frtbcke_box = new G4Box(fbeName,Xhalfbe,Thel/2.,Zhalfbe);
                Frtbckg_box = new G4Box(fbgName, Xhalfbg,Thce/2.,Zhalfbg);
            } else {
                Frtbck_box = new G4Box(fbName,Xhalfb,Thce/2.,Zhalfb);
                Frtbck2_box = new G4Box(fbName2,Xhalfb,Thce/2.,dz01);
                Frtbcke_box = new G4Box(fbeName,Xhalfbe,Thel/2.,Zhalfbe);
            }

            // For front part, Steel piece to be include in G10 volume
            #ifdef FRONT_STEEL
            if(irl==0) Frtbcks_box = new G4Box(fbsName,Xhalfbg,Tgfe/4.,Zhalfbg);
            #endif
	
            Frtbck_log = new G4LogicalVolume(Frtbck_box,Thin_abs,fbName);
            //g4vis->SetVis( Frtbck_log,ratio );
            //g4sdc->SetSD( Frtbck_log );

            Frtbck2_log = new G4LogicalVolume(Frtbck2_box,Thick_abs,fbName2);
            //g4vis->SetVis( Frtbck2_log,ratio );
            //g4sdc->SetSD( Frtbck2_log );
      
            Frtbcke_log = new G4LogicalVolume(Frtbcke_box,Kapton_Cu,fbeName);
            //g4vis->SetVis( Frtbcke_log,ratio );
            //g4sdc->SetSD( Frtbcke_log );

            if (irl==0) {
                Frtbckg_log = new G4LogicalVolume(Frtbckg_box,Gten_bar,fbgName);
                //g4vis->SetVis( Frtbckg_log,ratio );
                //g4sdc->SetSD( Frtbckg_log );
            }


            // Include thick abs part into thin abs logical volume

            #ifdef DEBUG_GEO
            std::cout << "position thick abs into thin z = " 
                   << dz01-Zhalfb << std::endl;
            #endif
            G4VPhysicalVolume *Frtbck2_phys =
                new G4PVPlacement(0,
                                  G4ThreeVector(0,0,dz01-Zhalfb),
                                  Frtbck2_log,
                                  fbName2,
                                  Frtbck_log,
                                  false,
                                  irl); // use copy#

            // For front part, include in G10 piece (8mm radial size*2.13mm thick)
            //  the steel pieces on each side (2*0.2mm thickness)

            if (irl==0) {

                #ifdef FRONT_STEEL
                Frtbcks_log =
                    new G4LogicalVolume(Frtbcks_box,Iron,fbsName);
                //g4vis->SetVis( Frtbcks_log,ratio );
                //g4sdc->SetSD( Frtbcks_log );
                #ifdef DEBUG_GEO
                std::cout << "position steel border into G10 front part " 
                          << "y = +- " << 0.5*(+Thce-Tgfe/2.)<< std::endl;
                #endif
                G4VPhysicalVolume *Frtbcks1_phys =
                    new G4PVPlacement(0,
                              G4ThreeVector(0.,0.5*(-Thce+Tgfe/2.),0.),
                              Frtbcks_log,
                              fbsName,
                              Frtbckg_log,
                              false,
                              0);
                G4VPhysicalVolume *Frtbcks2_phys =
                    new G4PVPlacement(0,
                              G4ThreeVector(0.,0.5*(+Thce-Tgfe/2.),0.),
                              Frtbcks_log,
                              fbsName,
                              Frtbckg_log,
                              false,
                              1);
                #endif
            }   // irl==0

        } //irl==0 or Nbrt-1    
        
        // Solid and Logical Volumes for FOLDS (absorbers, then electrodes)
        // Distinguish DOWN folds and UP folds

        //
        //----------    DOWN folds  --------------------
        double phi0,Dphi;
        double ddz0=dz0;
        double ddz01=dz01;
        double phi0_safety=0.003;

        if (irl ==0)    // first down half fold
        {
            phi0=M_PI/2. + phi0_safety;
            Dphi=M_PI/2.-Delta[irl]-phi0_safety;
        }
        else if (irl%2 == 0 &&  irl !=0)    // normal down fold
        {
        phi0=Delta[irl-1];
        Dphi=M_PI-Delta[irl]-Delta[irl-1];
        }
        else if (irl == Nbrt-1 && Nbrt%2 == 0)    // last (half) fold is a down
        {
            phi0=Delta[irl];
            Dphi=M_PI/2.-Delta[irl];
            ddz0=dza;
            ddz01=dza1;
        }
        else
        {
            phi0=Delta[irl];
            Dphi=M_PI-Delta[irl+1]-Delta[irl];
        }
        // GU 09/06/2004 add some safety in z size
        G4double safety_zlen=0.050*CLHEP::mm;
        ddz0 = ddz0 - safety_zlen;
        ddz01 = ddz01 - safety_zlen;
        #ifdef DEBUG_GEO
        std::cout << " " << std::endl;
        std::cout << " ** DOWN FOLDS " << std::endl;
        std::cout << "phi0,Dphi   " << phi0 << " " << Dphi << std::endl;
        std::cout << "Rmin/Rmax   " << Rcmin << " " << Rcmax << std::endl;
        std::cout << "Rmine/Rmaxe " << Rcmine << " " << Rcmaxe << std::endl;
        std::cout << "half len:dz0,dz01 " << ddz0 << " " << ddz01 << std::endl;
        #endif

        Corndw_ptub = new G4Tubs(corndwName,Rcmin,Rcmax,ddz0,phi0,Dphi); 
        Corndw_etub = new G4Tubs(corndweName,Rcmine,Rcmaxe,ddz0,phi0,Dphi); 
        Corndwt_ptub = new G4Tubs(corndwtName,Rcmin,Rcmax,ddz01,phi0,Dphi); 

        G4LogicalVolume *Corndw_log = new G4LogicalVolume(Corndw_ptub,
	    						  Thin_abs, 
							  corndwName);
        //g4vis->SetVis( Corndw_log,ratio );
        //g4sdc->SetSD( Corndw_log );

        G4LogicalVolume *Corndw_elog = new G4LogicalVolume(Corndw_etub,
							   Kapton_Cu, 
							   corndweName);
        //g4vis->SetVis( Corndw_elog,ratio );
        //g4sdc->SetSD( Corndw_elog );

        G4LogicalVolume *Corndw_logt = new G4LogicalVolume(Corndwt_ptub,
                                                           Thick_abs,
                                                           corndwtName);
        //g4vis->SetVis( Corndw_logt,ratio );
        //g4sdc->SetSD( Corndw_logt );

        // Put thick abs in the logical volume of thin abs

        #ifdef DEBUG_GEO
        std::cout << "Position thick down absorber in thin logical volume z= "
                  << ddz01-ddz0 << std::endl;
        #endif

        G4VPhysicalVolume *corndw_physt =
                new G4PVPlacement(0,
                              G4ThreeVector(0,0,ddz01-ddz0),
                              Corndw_logt,
                              corndwtName,
                              Corndw_log,
                              false,
                              irl); // use copy#

        // --------------- UP folds ( called for ODD irl's ) --------------
        // The first "IF" deals only with the case the number of Accordion
        // straight parts is even, i.e., when Nbrt is odd

        ddz0=dz0;
        ddz01=dz01;
        if (irl == Nbrt-1 && Nbrt%2 !=0) {
            phi0=Delta[irl];
            Dphi=M_PI/2.-Delta[irl];
            ddz0=dza;
            ddz01=dza1;
        }
        else if (irl%2 == 0)
        {
            phi0=Delta[irl+1];
            Dphi=M_PI-Delta[irl]-Delta[irl+1];
        }
        else     // normal up folds for odd irl
        {
            phi0=Delta[irl];
            Dphi=M_PI-Delta[irl-1]-Delta[irl];
        }

        // GU 09/06/2004 add some safety in z size
        ddz0 = ddz0 - safety_zlen;
        ddz01 = ddz01 - safety_zlen;

        #ifdef DEBUG_GEO
        std::cout << " " << std::endl;
        std::cout << " ** UP   FOLDS " << std::endl;
        std::cout << "phi0,Dphi   " << phi0 << " " << Dphi << std::endl;
        std::cout << "Rmin/Rmax   " << Rcmin << " " << Rcmax << std::endl;
        std::cout << "Rmine/Rmaxe " << Rcmine << " " << Rcmaxe << std::endl;
        std::cout << "half len:dz0,dz01 " << ddz0 << " " << ddz01 << std::endl;
        #endif

        Cornup_ptub =  new G4Tubs(cornupName,Rcmin,Rcmax,ddz0,phi0,Dphi);
        Cornup_etub =  new G4Tubs(cornupeName,Rcmine,Rcmaxe,ddz0,phi0,Dphi);
        Cornupt_ptub = new G4Tubs(cornuptName,Rcmin,Rcmax,ddz01,phi0,Dphi);

        G4LogicalVolume *Cornup_log = new G4LogicalVolume(Cornup_ptub,
  	            					  Thin_abs, 
							  cornupName);
        //g4vis->SetVis( Cornup_log,ratio );
        //g4sdc->SetSD( Cornup_log );


        G4LogicalVolume *Cornup_elog = new G4LogicalVolume(Cornup_etub,
        	    					   Kapton_Cu, 
						           cornupeName);
        //g4vis->SetVis( Cornup_elog,ratio );
        //g4sdc->SetSD( Cornup_elog );


        G4LogicalVolume *Cornup_logt = new G4LogicalVolume(Cornupt_ptub,
							   Thick_abs, 
							   cornuptName);
        //g4vis->SetVis( Cornup_logt,ratio );
        //g4sdc->SetSD( Cornup_logt );
     

        // Put thick abs in the logical volume of the thin abs

        #ifdef DEBUG_GEO
        std::cout << "Position thick up   absorber in thin logical volume z= "
                  << ddz01-ddz0 << std::endl;
        #endif
  
        G4VPhysicalVolume *cornup_physt = new G4PVPlacement(0, 
			                                    G4ThreeVector(0,0,ddz01-ddz0), 
			                                    Cornup_logt,
			                                    cornuptName,
			                                    Cornup_log, 
			                                    false, 
			                                    irl); // use copy#

        // Start loop over phi
        // (note it is not possible to create the geometry of the straight
        //  absorbers and electrodes outside the loop as we did for the folds because
        //  of sagging.)
        //

        int Nloop = Nplus;

        // GU in test beam case, should put one more absorber than
        // electrode as we don't have a full 2pi geometry
        //    in full atlas case, should have 1024 absorbers and electrodes

        if(Ncell == 64) Nloop++;
        for(int icopy=0; icopy<Nloop; icopy++)          // Loop over phi
        {   

        // 12-Feb-2003 WGS: Define a logical condition to suppress
        // the display of some volumes.  The following condition
        // causes us to display every 16th stack (at least, for
        // those volumes that support a partial view, and whose
        // logical volumes are defined within this loop).
        G4bool partial = icopy % 64;

        // Physical Volume Index for intrinsic access (SAGGING methods)
        G4int icopytot = 10000*irl + icopy; 

        // Starting angle for absorber rotation
        Gama = Gama0 + Alfa * icopy; // 1st absorber starts at Gama0    
	// Starting angle for electrode rotation
        Game= Gama + Psi/2;
	
        // Front and back Straight parts :  mother ECAM (bar_phys1)
        // Absorber+G10 and electrode front back straight parts
        //                            are for irl=0 or irl=Nbrt-1
        Zcd = Zmin+dz0; // Fix added 04-Oct-2002 Sylvain Negroni
        if (irl ==0 || irl == Nbrt-1)
	{
	  radius = Cenx[irl] - Xtip_pb/2;
	  radiuse = Cenx[irl] - Xtips/2;
	  radiusg = radius - Xtips/2;
	  if(irl == Nbrt-1)
	  {
        //radius = Cenx[Nbrt] + Xtip_pc/2;
	    radius  = Cenx[Nbrt] + Xtipt/2;
	    radiuse = Cenx[Nbrt] + Xtipt/2;
  	  }

	  G4RotationMatrix *rmfb = new G4RotationMatrix;
	  rmfb-> rotateZ(-Gama);
	  G4RotationMatrix *rmfe = new G4RotationMatrix;
	  rmfe-> rotateZ(-Game);

	  Xcd = radius*cos(Gama);
	  Ycd = radius*sin(Gama);

          #ifdef DEBUG_GEO
          if (icopy==0) {
           std::cout << " " << std::endl;
           std::cout << "position front/back straight part in ECAM" << std::endl;
           std::cout << "angle absborber   " << Gama/deg << std::endl;
           std::cout << "angle electrode   " << Game/deg << std::endl;
           std::cout << "rad for absborber " << radius << std::endl;
           if(irl==0) std::cout << "rad for G10       " << radiusg << std::endl;
           std::cout << "rad for electrode " << radiuse << std::endl;
          }
          #endif

	  G4VPhysicalVolume *Frtbck_phys = new G4PVPlacement(rmfb,
				      G4ThreeVector(Xcd,Ycd,Zcd),
				      Frtbck_log,
				      fbName,
				      ecamLogical,
				      false,
				      icopytot); // use copy#

          // G10 in absorber is only in the front 
          if (irl==0) {
	    Xcd = radiusg*cos(Gama);
	    Ycd = radiusg*sin(Gama);

	    G4VPhysicalVolume *Frtbckg_phys= 
		    new G4PVPlacement(rmfb,
				      G4ThreeVector(Xcd,Ycd,Zcd),
				      Frtbckg_log,
				      fbgName,
				      ecamLogical,
				      false,
				      icopytot);
          }

	  if(icopy != Nplus)
	  {
            Xcd = radiuse*cos(Game);
	    Ycd = radiuse*sin(Game);
 	
	    G4VPhysicalVolume *Frtbcke_phys= 
			new G4PVPlacement(rmfe,
					  G4ThreeVector(Xcd,Ycd,Zcd),
					  Frtbcke_log,
					  fbeName,
					  ecamLogical,
					  false,
					  icopytot);
	  }	
        }             // end of front-back straight part


        // FIRST : PHYSICAL VOLUMES for FOLDS (absorber and electrode)
        //   This to account for a possible SAGGING

        G4RotationMatrix *rmcdd = new G4RotationMatrix;
	rmcdd-> rotateZ(-Gama+180.*CLHEP::deg);
	G4RotationMatrix *rmeld = new G4RotationMatrix;
	rmeld-> rotateZ(-Game+180.*CLHEP::deg);
	G4RotationMatrix *rmcdu = new G4RotationMatrix;
	rmcdu-> rotateZ(-Gama);
	G4RotationMatrix *rmelu = new G4RotationMatrix;
	rmelu-> rotateZ(-Game);


        // translation
        Xcd=fx(irl+0., Gama, Cenx, Ceny);
        Ycd=fy(irl+0., Gama, Cenx, Ceny);
        if(A_SAGGING) Ycd -= deltay[irl]*dely( Gama0, Gama);
        Zcd=Zmin+dz0;
        Xcde=fx(irl+0., Game, Cenx, Ceny);
        Ycde=fy(irl+0., Game, Cenx, Ceny);
        if(A_SAGGING) Ycde -= deltay[irl]*dely( Gama0, Game);
        Zcde=Zmin+dz0;
	
        if (irl%2 ==0) 
	{
        // Absorber DOWN corner  
        #ifdef DEBUG_GEO
          if (icopy==0) {
           std::cout << " " << std::endl;
           std::cout << "Position Absorber down corner in STAC " << std::endl;
           std::cout << "rotation " << Gama/deg-180. << std::endl;
           std::cout << "position " << Xcd << " " << Ycd
                     << " " << Zcd << std::endl;
          }
        #endif

	  G4VPhysicalVolume *corndw_phys = 
		    new G4PVPlacement(rmcdd,
				      G4ThreeVector(Xcd,Ycd,Zcd), 
				      corndwName, 
				      Corndw_log, 
				      Stac_phys1, 
				      false,
				      icopytot);

	  if(icopy != Nplus)
	  {
            G4VPhysicalVolume *corndw_ephys = 
			new G4PVPlacement(rmeld,G4ThreeVector(Xcde,Ycde,Zcde), 
					  corndweName, 
					  Corndw_elog, 
					  Stac_phys1,
					  false,
					  icopytot);
          }
        }

	if (irl%2 !=0 ) 
	{
        // Absorber UP corner
        #ifdef DEBUG_GEO
          if (icopy==0) {
           std::cout << " " << std::endl;
           std::cout << "Position Absorber up corner in STAC " << std::endl;
           std::cout << "rotation " << Gama/deg << std::endl;
           std::cout << "position " << Xcd << " " << Ycd
                     << " " << Zcd << std::endl;
          }
        #endif
  
	  G4VPhysicalVolume *cornup_phys = 
		    new G4PVPlacement(rmcdu, 
				      G4ThreeVector(Xcd,Ycd,Zcd), 
				      cornupName, 
				      Cornup_log, 
				      Stac_phys1, 
				      false,
				      icopytot);

	  if(icopy != Nplus)
	  {
        #ifdef DEBUG_GEO
            if (icopy==0) {
             std::cout << " " << std::endl;
             std::cout << "Position Electrode up corner in STAC" << std::endl;
             std::cout << "rotation " << Game/deg << std::endl;
             std::cout << "position " << Xcd << " " << Ycd
                       << " " << Zcd << std::endl;
            }
        #endif

	    G4VPhysicalVolume *cornup_ephys = 
			new G4PVPlacement(rmelu, 
					  G4ThreeVector(Xcde,Ycde,Zcde), 
					  cornupeName, 
					  Cornup_elog, 
					  Stac_phys1, 
					  false,
					  icopytot);
  	  }
        }
	
        if (irl == Nbrt-1) 
        {
          G4int icopytott = 10000*Nbrt + icopy;	  
        // Last absorber DOWN corner
	  Xcd=fx(irl+1., Gama, Cenx, Ceny);
	  Ycd=fy(irl+1., Gama, Cenx, Ceny);
	  if(A_SAGGING) Ycd -= deltay[irl+1]*dely( Gama0, Gama);
	  Zcd=Zmin+dz0;
        #ifdef DEBUG_GEO
          if (icopy==0) {
            std::cout << " " << std::endl;
            std::cout << "Position last Absorber down fold in STAC" << std::endl;
            std::cout << "rotation " << Gama/deg-180. << std::endl;
            std::cout << "position " << Xcd << " " << Ycd
                      << " " << Zcd << std::endl;
          }
        #endif

	  G4VPhysicalVolume *corndw_phys = 
		    new G4PVPlacement(rmcdd, 
				      G4ThreeVector(Xcd,Ycd,Zcd), 
				      corndwName, 
				      Corndw_log, 
				      Stac_phys1, 
				      false,
				      icopytott);

	  if(icopy != Nplus)
	  {
        // Last electrode DOWN corner
  	    Xcde=fx(irl+1., Game, Cenx, Ceny);
	    Ycde=fy(irl+1., Game, Cenx, Ceny);
	    if(A_SAGGING) Ycde -= deltay[irl+1]*dely( Gama0, Game);
	    Zcde=Zmin+dz0;
        #ifdef DEBUG_GEO
            if (icopy==0) {
             std::cout << " " << std::endl;
             std::cout << "Position last Electrode fold in STAC" << std::endl;
             std::cout << "rotation " << Game/deg-180. << std::endl;
             std::cout << "position " << Xcd << " " << Ycd
                       << " " << Zcd << std::endl;
            }
        #endif
  	    G4VPhysicalVolume *corndw_ephys = 
			new G4PVPlacement(rmeld, 
					  G4ThreeVector(Xcde,Ycde,Zcde), 
					  corndweName, 
					  Corndw_elog, 
					  Stac_phys1, 
					  false,
					  icopytott);
	  }
        }


        // THEN the straight parts : absorbers and electrodes
        //   below three steps A, B and C
        //

        // -A- FIRST straight solids parameters (absorbers, then electrodes)
        //

	double dx, dy;
	double x1a, x2a, y1a, y2a;
	double x1e, x2e, y1e, y2e;

        // thickness of absorber
	Dz = Thce/2.;
        // thickness of electrode
	Dze = Thel/2.;

        if(A_SAGGING)
        {
            // For absorbers 
	    x1a = fx(irl+0., Gama, Cenx, Ceny);
            x2a = fx(irl+1., Gama, Cenx, Ceny);
	    y1a = fy(irl+0., Gama, Cenx, Ceny);
	    y1a -= deltay[irl]*dely( Gama0, Gama);
            y2a = fy(irl+1., Gama, Cenx, Ceny);
	    y2a -= deltay[irl+1]*dely( Gama0, Gama);
            dx = x2a - x1a;
            dy = y2a - y1a;

            // Da the two fold centers distance, da straight part length
            Da = sqrt ( dx*dx + dy*dy );
            da = sqrt ( (Da - 2.*Rint)*(Da + 2.*Rint) ); 

            // newalpha (slant angle) value of the rotation angle around Z_axis
            // used for SAGGING see below PHYSICAL Straight parts 
            cosalfa = (da*dx -iparit*2.*Rint*dy)/Da/Da;
            sinalfa = (da*dy +iparit*2.*Rint*dx)/Da/Da;
	    newalpha = atan2(sinalfa,cosalfa);  

            if(icopy != Nplus)
            {
               // For electrodes
	       x1e = fx(irl+0., Game, Cenx, Ceny);        
               x2e = fx(irl+1., Game, Cenx, Ceny);
	       y1e = fy(irl+0., Game, Cenx, Ceny);
	       y1e -= deltay[irl]*dely( Gama0, Game);
               y2e = fy(irl+1., Game, Cenx, Ceny);
	       y2e -= deltay[irl+1]*dely( Gama0, Game);
               dx = x2e - x1e;
               dy = y2e - y1e;
               // De the two fold centers distance, de straight part length
               De = sqrt ( dx*dx + dy*dy );
               de = sqrt ( (De - 2.*Rint)*(De + 2.*Rint) );

               //newalphe (slant angle) value of the rotation angle around Z_axis
               // used for SAGGING see below PHYSICAL Straight parts 
               cosalfa = (de*dx -iparit*2.*Rint*dy)/De/De;
               sinalfa = (de*dy +iparit*2.*Rint*dx)/De/De;
	       newalphe = atan2(sinalfa,cosalfa);
            } 
	}


        //
        // -B- straight Solid and Logical volumes (absorbers, electrodes)
        //
	h1 = Along[irl]/2. - safety_along;
        if(A_SAGGING) h1 = da/2. - safety_along;

        //      if (Rhocen[irl+1] <= Bar_Rcmx )
        //      {
        //        bl1 = (Zcp2[irl]-Zmin)/2.;
        //        tl1 = (Zcp2[irl+1]-Zmin)/2.;
        //      } 
        //      else
        //      {
        //          tl1 = (Zmax-Zmin)/2.;
        //          bl1 = tl1;
        //          if(Rhocen[irl] <= Bar_Rcmx ) bl1=(Zcp2[irl]-Zmin)/2.;
        //      }

        // more correct computation with z lenght computed exactly at the
        // radius of the end and the beginning of the straight sections
        bl1 = (std::min(Zcp2l[irl],Zmax)-Zmin)/2.;
        tl1 = (std::min(Zcp2h[irl],Zmax)-Zmin)/2.;

        //GU 09/06/2004 add small safety in size tl1 and bl1
        tl1=tl1-safety_zlen;
        bl1=bl1-safety_zlen;

        #ifdef DEBUG_GEO
        if (icopy==0){
         std::cout << " " << std::endl;
         std::cout << "Parameters for straight absorber " << std::endl;
         std::cout << "half lenght " << Dz << std::endl;
         std::cout << "h1 (halfY), tl1,bl1 (halfX) " << h1
                   << " " << tl1 << " " << bl1 << std::endl;
        }
        #endif
#ifndef ORIGINAL_ATLAS
	if (tl1 == bl1) {
		Straight_solid = new G4Box(straightName, tl1, h1, Dz);
	} else
#endif
		Straight_solid = new G4Trap(straightName,
					    2*Dz,
					    2*h1,
					    2*tl1,
					    2*bl1);

        // The same for electrodes.
        if(A_SAGGING) h1 = de/2. - .007*CLHEP::mm;
        #ifdef DEBUG_GEO
        if (icopy==0){
         std::cout << " " << std::endl;
         std::cout << "Parameters for straight electrode " << std::endl;
         std::cout << "half lenght " << Dze << std::endl;
         std::cout << "h1 (halfY), tl1,bl1 (halfX) " << h1
                   << " " << tl1 << " " << bl1 << std::endl;
        }
        #endif
#ifndef ORIGINAL_ATLAS
	if (tl1 == bl1) {
		Straight_esolid = new G4Box(straighteName, tl1, h1, Dze);
	} else
#endif
		Straight_esolid = new G4Trap(straighteName,
					     2*Dze,
					     2*h1,
					     2*tl1,
					     2*bl1);


        // Related straight logical volumes (absorbers, electrodes)


        Straight_log = new G4LogicalVolume(Straight_solid,
					       Thin_abs,
					       straightName);

        Straight_elog = new G4LogicalVolume(Straight_esolid,
						Kapton_Cu,
						straighteName);

	//g4vis->SetVis( Straight_log,ratio,partial );
        //g4sdc->SetSD( Straight_log );

	//g4vis->SetVis( Straight_elog,ratio,partial );
        //g4sdc->SetSD( Straight_elog );

        // Account for THICK STRAIGHT ABSORBERS : 
        // They are put into the previous thin absorbers for
        //          abs(eta) < .8  i.e. abs(Z)< Zcut2


        Zx0 = (tl1+bl1)/2.;
        //        Xb1 = (Zcp1[irl]-Zmin)/2.;
        //        Xt1 = (Zcp1[irl+1]-Zmin)/2.;
        // more correct computation with z lenght computed exactly at the
        // radius of the end and the beginning of the straight sections
        Xb1 = (Zcp1l[irl]-Zmin)/2.;
        Xt1 = (Zcp1h[irl]-Zmin)/2.;
        // translation in x to include thick absorber into thin absorber
	Xtrans = (Xb1+Xt1)/2.-Zx0 + .007*CLHEP::mm;
        if(A_SAGGING) h1 = da/2. - .007*CLHEP::mm;
        #ifdef DEBUG_GEO
        if (icopy==0){
         std::cout << " " << std::endl;
         std::cout << "Parameters for straight thick absorber " << std::endl;
         std::cout << "half lenght " << Dz << std::endl;
         std::cout << "h1 (halfY), xt1,Xb1 (halfX) " << h1
                   << " " << Xt1 << " " << Xb1 << std::endl;
        }
        #endif
	Straight_solidt = new G4Trap(straighttName,
				     2*Dz,
				     2*h1,
				     2*Xt1,
				     2*Xb1);

	G4LogicalVolume *Straight_logt = new G4LogicalVolume(Straight_solidt,
							     Thick_abs,
							     straighttName);
	//g4vis->SetVis( Straight_logt,ratio,partial );
        //g4sdc->SetSD( Straight_logt );
     

        // 2) put the thick absorber in the logical thin absorber volume
        //
        #ifdef DEBUG_GEO
        if (icopy==0) {
         std::cout << "Position thick abs into thin Xtrans= "
                   << Xtrans << std::endl;
        }
        #endif
	G4VPhysicalVolume *straight_physt = 
	    new G4PVPlacement(0, 
			      G4ThreeVector(Xtrans,0,0), 
			      Straight_logt,
			      straightName, 
			      Straight_log, 
			      false, 
			      irl); // use copy#


        //
        // -C- PHYSICAL VOLUMES for straight parts (absorbers & electrodes)
        //
        //
        // First Rotations
    
	G4double alfrot,alf1,alfrote,alf1e;

        // For absorber
        if(A_SAGGING) {

        // newalpha is already computed angle wrt z axis
        // P/2 rotation is to get absorber aligned along local x axis
        // instead of y, then rotate with angle newalpha

            alfrot =  -M_PI/2. - newalpha;
        }
        else
        // no sagging, compute rotation angle from scratch
        {
            alf1 = (Delta[irl])*iparit;
        // First fold is 0, then 1 etc
        // Even TRAP's have to be put upside down for a good thickabs matching
            if(iparit == 1) alf1 = alf1 + 180.*CLHEP::deg;
            alfrot = (alf1-Gama);
        }

        // For electrode

        if(A_SAGGING) {

        // newalphae is already computed angle wrt z axis
        // P/2 rotation is to get absorber aligned along local x axis
        // instead of y, then rotate with angle newalpha

            alfrote = -M_PI/2. - newalphe;
        }
        else
        {
        // no sagging, compute rotation angle from scratch
            alf1e = (Delta[irl])*iparit;
        // First fold is 0, then 1 etc
        // Even TRAP's have to be put upside down for a good thickabs matching
            if(iparit == 1) alf1e = alf1e + 180.*CLHEP::deg;
            alfrote = (alf1e-Game);
        }


        G4RotationMatrix *rms = new G4RotationMatrix;
	rms-> rotateZ(alfrot);
	G4RotationMatrix *rmse = new G4RotationMatrix;
	rmse-> rotateZ(alfrote);
        rms-> rotateY(90.*CLHEP::deg);
	rmse-> rotateY(90.*CLHEP::deg);
	    
        // Then  Position of the straight part centers and Physical volumes
        if(A_SAGGING)
        {
          Xcd = (x1a + x2a)/2.;
          Ycd = (y1a + y2a)/2.;
        }
        else
        {
          Xcd=fx(irl+.5, Gama, Cenx, Ceny);
          Ycd=fy(irl+.5, Gama, Cenx, Ceny);
        }
        Zcd=Zmin+(tl1+bl1)/2.+safety_zlen;

        #ifdef DEBUG_GEO
        if (icopy==0 ) {
         std::cout << " " << std::endl;
         std::cout << "position straight absorber in STAC " << std::endl;
         std::cout << "angle along Z " << -alfrot/deg << std::endl;
         std::cout << "angle along Y  90.  " << std::endl;
         std::cout << "Position " << Xcd << " " << Ycd
                   << " " << Zcd << std::endl;
        }
        #endif
	    
	G4VPhysicalVolume *straight_phys = 
		new G4PVPlacement(rms, 
				  G4ThreeVector(Xcd,Ycd,Zcd), 
				  straightName, 
				  Straight_log, 
				  Stac_phys1, 
				  false,
				  icopytot);

        // position for electrodes
        if(A_SAGGING)
        {
          Xcd = (x1e + x2e)/2.;
          Ycd = (y1e + y2e)/2.;
        }
        else
        {
          Xcd=fx(irl+.5, Game, Cenx, Ceny);
          Ycd=fy(irl+.5, Game, Cenx, Ceny);
        }
        Zcd=Zmin+(tl1+bl1)/2.+safety_zlen;
	  
	if(icopy != Nplus)
	{
          #ifdef DEBUG_GEO
          if (icopy==0) {
           std::cout << " " << std::endl;
           std::cout << "position straight electrode in STAC " << std::endl;
           std::cout << "angle along Z " << -alfrote/deg << std::endl;
           std::cout << "angle along Y  90.  " << std::endl;
           std::cout << "Position " << Xcd << " " << Ycd
                   << " " << Zcd << std::endl;
          }
         #endif
	  G4VPhysicalVolume *straight_ephys = 
		    new G4PVPlacement(rmse, 
				      G4ThreeVector(Xcd,Ycd,Zcd), 
				      straighteName, 
				      Straight_elog, 
				      Stac_phys1, 
				      false,
				      icopytot);

	}
	
    }             // END of loop over phi
	
  } // END of the loop over the Accordeon Phi_Crowns - end of Accordeon Geom.


//#endif // BUILD_ACCORDION_PLATES

//#endif  // BUILD_STAC

    return ecamLogical;




}

//**************************************************
