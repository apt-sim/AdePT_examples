//**************************************************
// \file LArTBCryostatConstruction.cc
// \brief: implementation of 
//         LArTBCryostatConstruction class
// \author: originally from ATLAS-G4
//          adaptation by 
//          Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 8 August 2022
//**************************************************

//Includers from project files
//
#include "LArTBCryostatConstruction.hh"
#include "LArBarrelConstruction.hh"
#include "LArBarrelPresamplerConstruction.hh"

//Includers from Geant4
//
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

LArTBCryostatConstruction::LArTBCryostatConstruction(){}
LArTBCryostatConstruction::~LArTBCryostatConstruction(){}

G4LogicalVolume* LArTBCryostatConstruction::GetEnvelope() {

    G4cout<<"->in LArTBCryostatConstruction::GetEnvelope()"<<G4endl;
    G4String name,symbol;
    G4double a,z,density,fractionmass;
    G4double X0,IntL;
    G4int ncomponents,natoms;

    // Hydrogen
    //
    a= 1.01*g/mole;
    G4Element *elH = new G4Element(name="Hydrogen", symbol="H", z=1., a);
 
    // Carbon
    //
    a= 12.01*g/mole;
    G4Element *elC = new G4Element(name="Carbon", symbol="C", z=6., a);
    
    // Oxygen
    //
    a = 16.00*g/mole;
    G4Element *elO = new G4Element(name="Oxygen", symbol="O", z=8., a);

    // Nitrogen
    //
    a= 14.01*g/mole;
    G4Element *elN = new G4Element(name="Nitrogen", symbol="N", z=7., a);

    // Aluminium
    //
    a = 26.98*g/mole;
    density = 2.70*g/cm3;
    G4Material * Aluminium = new G4Material(name="Aluminium",13.,a, density);
    X0 = Aluminium->GetRadlen();
    IntL = Aluminium->GetNuclearInterLength();
    std::cout << "Alu  X0/lambda " << X0 << " " << IntL << std::endl;

    // Iron
    //
    a = 55.845*g/mole;
    density = 7.87*g/cm3;
    G4Material * Iron = new G4Material(name="Iron",26.,a,density);

    // Define Material FOAM  ( C5H8O2 )
    // latest density value from P.Puzo (october 2003)
    //
    density = 0.058*g/cm3;
    G4Material * Foam = new G4Material(name="Foam", density, ncomponents=3);
    Foam->AddElement(elH,natoms=8);
    Foam->AddElement(elC,natoms=5);
    Foam->AddElement(elO,natoms=2);
    X0 = Foam->GetRadlen();
    IntL = Foam->GetNuclearInterLength();
    std::cout << "Foam X0/lambda " << X0 << " " << IntL << std::endl;

    // Air
    //
    density = 1.29e-03*g/cm3;
    G4Material * Air = new G4Material(name="Air", density, ncomponents=2);
    Air->AddElement(elN, fractionmass=.8);
    Air->AddElement(elO, fractionmass=.2);

    // liquid Argon
    //
    a = 39.948*g/mole;
    density = 1.396*g/cm3;
    G4Material * LAr = new G4Material(name="LiquidArgon",18.,a, density);

    // Vacuum
    //
    density = CLHEP::universe_mean_density;  //from PhysicalConstants.h
    G4double pressure = 3.e-18*pascal;
    G4double temperature = 2.73*kelvin;
    G4Material *
    Vacuum =
        new G4Material(name="Vacuum", z=1., a=1.01*g/mole, density,
                       kStateGas,temperature,pressure);


    //--------------------------------------------------
    //Define Geometries
    //--------------------------------------------------

    //Big mother volume to be included in transparent way in Calo Ctb envelope
    //
    G4double zp[3]   ={-1050.,0.,3810.};
    G4double ri[3]   ={965.,965.,965.};
    G4double ro[3]   ={2270.,2270.,2270.};
/*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " Pcone volume for overall envelope " << std::endl;
    std::cout << " phi0=-25deg, Dphi=50deg " << std::endl;
    for (int i=0;i<3;i++) {
     std::cout << " Plane zp/ri/ro " << zp[i] << " " << ri[i] << " " << ro[i] << std::endl;
    }
#endif*/
    G4Polycone* Em_pcone = new G4Polycone("LAr::TBBarrel::Cryostat::Envelope",
                                          -25.*deg,50.*deg,3,zp,ri,ro);
    G4LogicalVolume* Em_log = new G4LogicalVolume(Em_pcone,Air,
                                    "LAr::TBBarrel::Cryostat::Envelope");
    //Em_log is returned by GetEnvelope() method
    //g4sdc->SetSD(Em_log);                                              
     
    //
    // Cryostat geometry
    //

    G4double  Cryo_Distz = 483.5*cm;   // total size in z
    G4double  Cryo_z0 = 103.0*cm;       // eta=0 position wrt cryosta edge at z<0

    G4double  DeltaR_cold = 4.1*cm;    // thickness cold vessel before calo
    G4double  DeltaRout_cold = 5.0*cm;    // thickness cold vessel after calo

    G4double  DeltaR_warm= 3.86*cm;    // thickness warm vessel before calo
    G4double  DeltaRout_warm = 4.0*cm;    // thickness warm vessel after calo

    G4double  DeltaRout_vac = 3.0*cm;  // vacuum space cryo after calo

    G4double Dz_end_warm = 7.0*cm;   // thickness of end plate at high z
    G4double Dz_end_vac  = 8.0*cm;   
    G4double Dz_end_cold = 7.0*cm;

    G4double Dz_end_tot = Dz_end_warm + Dz_end_vac + Dz_end_cold;

    // position of center of curvature of cryostat before calo
    G4double  Cryo_Xcent = 3363.;

    G4double  Cryo_Rmax_C = 2326.;
    G4double  Cryo_Rmin_C  = Cryo_Rmax_C - DeltaR_cold;     

    G4double  Cryo_Rmax_W = 2396.;
    G4double  Cryo_Rmin_W =  Cryo_Rmax_W - DeltaR_warm;

    // recompute vacuum space cryoastat before calo
    //G4double  DeltaR_vac = Cryo_Rmin_W - Cryo_Rmax_C; //unused variable rm lorenzo  

    G4double  Rmin_mother = Cryo_Xcent-Cryo_Rmax_W;
    G4double  Rmax_mother = 2270.;

    G4double Phi_Min = -5.0 * deg;
    G4double Phi_Span = 32.5 * deg;
    // GU 10/09/2004
    // For cryostat mother volume, sligthly larger phi range
    //  to avoid clash with front cryostat
    G4double Phi_Min_Moth=-9.0*deg;
    G4double Phi_Span_Moth=38.5*deg;

    // -----------------------------------------------------------------
    // Mother volume for Cryostat, filled with foam
    // ------------------------------------------------------------------

/*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << "** Mother Volume for TB cryostat (Tubs) " << std::endl;
    std::cout << "   (matter = foam) " << std::endl;
    std::cout << " Rmin/Rmax " << Rmin_mother << " " << Rmax_mother << std::endl;
    std::cout << " Dz/2 " << Cryo_Distz/2. << std::endl;
    std::cout << " PhiMin, Span " << Phi_Min_Moth/deg << " "
              << Phi_Span_Moth/deg << std::endl;
#endif*/

    G4Tubs *Cent_tube = new G4Tubs("LAr::TBBarrel::Cryostat::Mother",
                                   Rmin_mother,
                                   Rmax_mother,
                                   Cryo_Distz/2.,
                                   Phi_Min_Moth,
                                   Phi_Span_Moth);

    G4LogicalVolume *Cent_log = new G4LogicalVolume(Cent_tube,
                                                    Foam,
                                   "LAr::TBBarrel::Cryostat::Mother");
    Cent_log->SetVisAttributes( G4VisAttributes::GetInvisible() );

    //g4sdc->SetSD(Cent_log);

    // position in Pcon mother envelope (which has Atlas reference frame)
    G4double zpos = Cryo_Distz/2.-Cryo_z0;
    G4double phi  = -1.*360.*deg/16/2.;   // to have x axis in middle of volume
/*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " Position mother in EM CTb envelope z= " << zpos  
              << "  phi rotation= " << phi/deg << std::endl;
#endif*/
    G4RotationMatrix* rotm=new G4RotationMatrix();
    rotm->rotateZ(-phi);
    
    G4VPhysicalVolume* Cent_phys = new G4PVPlacement(rotm,G4ThreeVector(0.,0.,zpos),
                                   Cent_log,"LAr::TBBarrel::Cryostat::Mother",
                                   Em_log,false,1);
                                

    // ----------------------------------------------------------------------
    // Cryostat before calo
    // ----------------------------------------------------------------------
//#ifdef BUILD_TBCRYO1

// Warm vessel

/*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " ** Cryostat before LAr (shape=Tubs)" << std::endl;
    std::cout << " center in x = " << Cryo_Xcent  << std::endl;
    std::cout << " angle 180-11 deg, span = 14 deg" << std::endl;
    std::cout << " R warm vessel " << Cryo_Rmin_W << " "
                                   << Cryo_Rmax_W << std::endl;
    std::cout << " R vacuum      " << Cryo_Rmax_C << " "
                                   << Cryo_Rmin_W << std::endl;
    std::cout << " R cold vessel " << Cryo_Rmin_C << " "
                                   << Cryo_Rmax_C << std::endl;
    std::cout << " Half size in z " << (Cryo_Distz-Dz_end_tot)/2. << std::endl;
    std::cout << " position in z in mother " <<  -Dz_end_tot/2. << std::endl;
#endif*/

    G4Tubs *CryoW_tube = new G4Tubs("LAr::TBBarrel::Cryostat::WarmTube",
                                    Cryo_Rmin_W,
                                    Cryo_Rmax_W,
                                    (Cryo_Distz-Dz_end_tot)/2.,
                                    (180.-11.)*deg,
                                    14.*deg);

    G4LogicalVolume *CryoW_log = new G4LogicalVolume(CryoW_tube,
                                                     Aluminium,
                             "LAr::TBBarrel::Cryostat::WarmTube");
    //g4vis->SetVis(CryoW_log);
    //g4sdc->SetSD(CryoW_log);

    G4VPhysicalVolume *CryoW_phys;
             CryoW_phys = new G4PVPlacement(0,
                          G4ThreeVector(Cryo_Xcent, 0., -Dz_end_tot/2.),
                          "LAr::TBBarrel::Cryostat::WarmTube",
                          CryoW_log,
                          Cent_phys,
                          false,
                          0);

    // Waccum between warm and cold vessels
    //
    G4Tubs *CryoV_tube = new G4Tubs("LAr::TBBarrel::Cryostat::VacTube",
                                    Cryo_Rmax_C,
                                    Cryo_Rmin_W,
                                    (Cryo_Distz-Dz_end_tot)/2.,
                                    (180.-11.)*deg,
                                    14.*deg);

    G4LogicalVolume* CryoV_log = new G4LogicalVolume(CryoV_tube,
                                                     Vacuum,
                             "LAr::TBBarrel::Cryostat::VacTube");
    //g4sdc->SetSD(CryoV_log);

    G4VPhysicalVolume* CryoV_phys = new G4PVPlacement(0,
                             G4ThreeVector(Cryo_Xcent,0.,-Dz_end_tot/2.),
                             "LAr::TBBarrel::Cryostat::VacTube",
                             CryoV_log,
                             Cent_phys,
                             false,
                             0);
    // Cold vessel

    G4Tubs *CryoC_tube = new G4Tubs("LAr::TBBarrel::Cryostat::ColdTube",
                                    Cryo_Rmin_C,
                                    Cryo_Rmax_C,
                                    (Cryo_Distz-Dz_end_tot)/2.,
                                    (180.-11.)*deg,
                                    14.*deg);

    G4LogicalVolume *CryoC_log = new G4LogicalVolume(CryoC_tube,
                                                     Aluminium,
                                     "LAr::TBBarrel::Cryostat::ColdTube");
    //g4sdc->SetSD(CryoC_log);
    //g4vis->SetVis(CryoC_log);


    G4VPhysicalVolume *CryoC_phys;
             CryoC_phys = new G4PVPlacement(0,
                          G4ThreeVector(Cryo_Xcent,0., -Dz_end_tot/2.),
                          "LAr::TBBarrel::Cryostat::ColdTube",
                          CryoC_log,
                          Cent_phys,
                          false,
                          0);
    //#endif
    
    // ----------------------------------------------------------------------
    // ACCB  Mother volume for all the LAr 
    // including PS + Accordion
    // the LAr start at r=1410mm (end of foam) and go to the beginning of the
    // cold vessel of the cryostat after calo
    //-----------------------------------------------------------------------

    G4double LAr_inner_radius=141.00*cm;   // min radius of PS
    G4double LAr_outer_radius=Rmax_mother-DeltaRout_warm-DeltaRout_cold
                                         -DeltaRout_vac;

    G4double LAr_z_max = Cryo_Distz-Dz_end_tot;

/*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " *** LAr volume (tubs put in foam)" << std::endl;
    std::cout << "Rmin/Rmax " << LAr_inner_radius << " "
                              << LAr_outer_radius << std::endl;
    std::cout << "PhiMin,Span " << Phi_Min/deg << " "
                                << Phi_Span/deg << std::endl;
    std::cout << "DeltaZ/2 " << LAr_z_max/2. << std::endl;
    std::cout << "Position in z in mother " << (LAr_z_max-Cryo_Distz)/2. << std::endl;
#endif*/

    // ACCB mother is a tube
    G4Tubs *moth_tube = new G4Tubs("LAr::TBBarrel::Cryostat::LAr",
                                   LAr_inner_radius,
                                   LAr_outer_radius,
                                   LAr_z_max/2.,
                                   Phi_Min,
                                   Phi_Span);

    G4LogicalVolume *moth_log = new G4LogicalVolume(moth_tube,
                                                    LAr,
                                  "LAr::TBBarrel::Cryostat::LAr");

    auto LArVisAttributes = new G4VisAttributes();
    LArVisAttributes->SetForceSolid( true );
    LArVisAttributes->SetColour( G4Color::Cyan().GetRed(), G4Color::Cyan().GetGreen(), G4Color::Cyan().GetBlue(), 0.2);
    moth_log->SetVisAttributes( LArVisAttributes );

    //moth_log->SetVisAttributes( VisAttributes::LArVisAttributes );
    //g4sdc->SetSD(moth_log);

    // Physical volume
    G4VPhysicalVolume *moth_phys = new G4PVPlacement(0,
                          G4ThreeVector(0., 0.,(LAr_z_max-Cryo_Distz)/2.),
                          "LAr::TBBarrel::Cryostat::LAr", 
                          moth_log,
                          Cent_phys,
                          false,
                          0);

    // Adjust LAR volumes at the end of foam, such as to have 
    //   18.5mm before PS at top
    //   12.5mm before PS at mid top
    //   14.5mm before PS at mid
    //    7.5mm before PS at mid bottom
    //    9.5mm before PS at bottom
    // takes into account that for 1 PS sector r at the middle is 1412 and
    //  at the edges is 1419.  (middle of PS sectors=mid top and mid bottom)
    //  therefore compared to r=1410mm (LAr above) to get the correct Ar space
    //  one needs to add  ~9.5mm at the top, 10mm at mid top, 5.5mm at mid,
    //    5.5mm at mid bottom and 0.5 mm at bottom

    //#ifdef BUILD_LARFOAM

    G4double delta_LAr[5]={0.5*mm,5.5*mm,5.5*mm,10.*mm,9.5*mm};
    for (int ilar=0;ilar<5;ilar++) {
      G4double r1=LAr_inner_radius-delta_LAr[ilar];
      G4double r2=LAr_inner_radius;
      G4double Phi1=22.5*deg/5.*((double) ilar);
      G4double Delta_phi = 22.5*deg/5.;

/*#ifdef DEBUG_GEO
      std::cout << " Ar additionnal volume before PS " << r1 << " "
                << r2 << " " << Phi1/deg << " " << Delta_phi/deg << std::endl;
#endif*/

      G4Tubs *lar_tube = new G4Tubs("LAr::TBBarrel::Cryostat::LAr2",
                                   r1,
                                   r2,
                                   LAr_z_max/2.,
                                   Phi1,
                                   Delta_phi);

      G4LogicalVolume *lar_log = new G4LogicalVolume(lar_tube,
                                                    LAr,
                                  "LAr::TBBarrel::Cryostat::LAr2");
      //g4sdc->SetSD(lar_log);

      G4VPhysicalVolume *lar_phys =
        new G4PVPlacement(0,
                          G4ThreeVector(0., 0.,(LAr_z_max-Cryo_Distz)/2.),
                          "LAr::TBBarrel::Cryostat::LAr2",
                          lar_log,
                          Cent_phys,
                          false,
                          0);
    }
//#endif


    // Outer support rings: 6 steel rings, starting just
    //   after Barrel volume (G10 bars) (r=2003.6)
    //   DZ=80mm for DR=12mm, then DZ=10mm for DR=757mm then DZ=80mm for DR=12mm
    //   at locations z=397,805,1255,1750,2316,2868 mm
    //#ifdef BUILD_SUPPORTRING

     G4double R_ring = 2003.6;
     G4double DeltaR1_ring = 12.0;
     G4double DeltaZ1_ring = 80.0;
     G4double DeltaR2_ring = 75.7;
     G4double DeltaZ2_ring = 10.0;
     G4double DeltaR3_ring = 12.0;
     G4double DeltaZ3_ring = 80.0;

/*#ifdef DEBUG_GEO
     std::cout << " " << std::endl;
     std::cout << " *** Support Ring1: R/DR/DZ " << R_ring << " "
               << DeltaR1_ring << " " << DeltaZ1_ring << std::endl;
     std::cout << "             Ring2: R/DR/DZ " << R_ring+DeltaR1_ring << " "
               << DeltaR2_ring << " " << DeltaZ2_ring << std::endl;
     std::cout << "             Ring3: R/DR/DZ " << R_ring+DeltaR1_ring+DeltaR2_ring << " "
               << DeltaR3_ring << " " << DeltaZ3_ring << std::endl;
#endif*/

     auto ringVisAttributes = new G4VisAttributes();
     ringVisAttributes->SetForceSolid( true );
     ringVisAttributes->SetColor( G4Color::Grey() );
     ringVisAttributes->SetDaughtersInvisible( true );

     G4Tubs * ring1_shape = new G4Tubs("LAr::TBBarrel::Cryostat::Ring1",
                                 R_ring,
                                 R_ring+DeltaR1_ring,
                                 DeltaZ1_ring/2.,
                                 Phi_Min,
                                 Phi_Span);

     G4LogicalVolume *ring1_log = new G4LogicalVolume(ring1_shape,
                                                     Iron,
                                   "LAr::TBBarrel::Cryostat::Ring1");
     ring1_log->SetVisAttributes( ringVisAttributes );

     //g4sdc->SetSD(ring1_log);

     G4Tubs * ring2_shape = new G4Tubs("LAr::TBBarrel::Cryostat::Ring2",
                                 R_ring+DeltaR1_ring,
                                 R_ring+DeltaR1_ring+DeltaR2_ring,
                                 DeltaZ2_ring/2.,
                                 Phi_Min,
                                 Phi_Span);

     G4LogicalVolume *ring2_log = new G4LogicalVolume(ring2_shape,
                                                     Iron,
                                   "LAr::TBBarrel::Cryostat::Ring2");
     ring2_log->SetVisAttributes( ringVisAttributes );
     //g4sdc->SetSD(ring2_log);

     G4Tubs * ring3_shape = new G4Tubs("LAr::TBBarrel::Cryostat::Ring3",
                                 R_ring+DeltaR1_ring+DeltaR2_ring,
                                 R_ring+DeltaR1_ring+DeltaR2_ring+DeltaR3_ring,
                                 DeltaZ3_ring/2.,
                                 Phi_Min,
                                 Phi_Span);

     G4LogicalVolume *ring3_log = new G4LogicalVolume(ring3_shape,
                                                     Iron,
                                   "LAr::TBBarrel::Cryostat::Ring3");
     ring3_log->SetVisAttributes( ringVisAttributes );
     //g4sdc->SetSD(ring3_log);

     static double zring[6] = {397.,805.,1255.,1750.,2316.,2868.};
     for (int iring=0; iring < 6; iring++) 
     {
       G4double Zcd =  zring[iring]-LAr_z_max/2.+Cryo_z0;
/*#ifdef DEBUG_GEO
       std::cout << " Position ring in LAr mother volume at z = "
                 << Zcd << " (z atlas= " << zring[iring] << std::endl;
#endif*/
       G4VPhysicalVolume *ring1_phys =
         new G4PVPlacement(0,
                           G4ThreeVector(0.,0.,Zcd),
                           "LAr::TBBarrel::Cryostat::Ring1",
                           ring1_log,
                           moth_phys,
                           false,
                           iring); 

       G4VPhysicalVolume *ring2_phys =
         new G4PVPlacement(0,
                           G4ThreeVector(0.,0.,Zcd),
                           "LAr::TBBarrel::Cryostat::Ring2",
                           ring2_log,
                           moth_phys,
                           false,
                           iring);

       G4VPhysicalVolume *ring3_phys =
         new G4PVPlacement(0,
                           G4ThreeVector(0.,0.,Zcd),
                           "LAr::TBBarrel::Cryostat::Ring3",
                           ring3_log,
                           moth_phys,
                           false,
                           iring);
     }
//#endif


// -----------------------------------------------------------------
//  Cryostat after LAr
// -----------------------------------------------------------------
//#ifdef BUILD_TBCRYO2
     G4double rmin,rmax;

// Cold vessel 
     rmin = LAr_outer_radius;
     rmax = LAr_outer_radius + DeltaRout_cold;
/*#ifdef DEBUG_GEO
     std::cout << " " << std::endl;
     std::cout << "** Cryostat after calo " << std::endl;
     std::cout << "cold vessel from " << rmin << " to " << rmax << std::endl;
     
     std::cout << " position in mother in z " << -Dz_end_tot/2. << std::endl;
#endif*/
     G4Tubs* CryoC2_tube = new G4Tubs("LAr::TBBarrel::Cryostat::ColdTube2",
                                     rmin,
                                     rmax,
                                     (Cryo_Distz-Dz_end_tot)/2.,
                                     Phi_Min,
                                     Phi_Span);

    G4LogicalVolume *CryoC2_log = new G4LogicalVolume(CryoC2_tube,
                                                     Aluminium,
                                     "LAr::TBBarrel::Cryostat::ColdTube2");
    //g4sdc->SetSD(CryoC2_log);

    G4VPhysicalVolume *CryoC2_phys =  new G4PVPlacement(0,
                          G4ThreeVector(0., 0.,-Dz_end_tot/2.),
                          "LAr::TBBarrel::Cryostat::ColdTube2",
                          CryoC2_log,
                          Cent_phys,
                          false,
                          0);

     // vacuum between warn and cold vessel
     rmin = rmax;
     rmax = rmin + DeltaRout_vac;
/*#ifdef DEBUG_GEO
    std::cout << "vacuum from " << rmin << " to " << rmax << std::endl;
#endif*/
     G4Tubs* CryoV2_tube = new G4Tubs("LAr::TBBarrel::Cryostat::VacTube2",
                                     rmin,
                                     rmax,
                                     (Cryo_Distz-Dz_end_tot)/2.,
                                     Phi_Min,
                                     Phi_Span);

    G4LogicalVolume *CryoV2_log = new G4LogicalVolume(CryoV2_tube,
                                                     Vacuum,
                                     "LAr::TBBarrel::Cryostat::VacTube2");
    //g4sdc->SetSD(CryoV2_log);

    G4VPhysicalVolume *CryoV2_phys =  new G4PVPlacement(0,
                          G4ThreeVector(0., 0., -Dz_end_tot/2.),
                          "LAr::TBBarrel::Cryostat::VacTube2",
                          CryoV2_log,
                          Cent_phys,
                          false,
                          0);

    // warm vessel
     rmin = rmax;
     rmax = rmin + DeltaRout_warm;
/*#ifdef DEBUG_GEO
    std::cout << "warm vessel from " << rmin << " to " << rmax << std::endl;
#endif*/
     G4Tubs* CryoW2_tube = new G4Tubs("LAr::TBBarrel::Cryostat::WarmTube2",
                                     rmin,
                                     rmax,
                                     (Cryo_Distz-Dz_end_tot)/2.,
                                     Phi_Min,
                                     Phi_Span);

    G4LogicalVolume *CryoW2_log = new G4LogicalVolume(CryoW2_tube,
                                                     Aluminium,
                                     "LAr::TBBarrel::Cryostat::WarmTube2");
    //g4vis->SetVis(CryoW2_log);
    //g4sdc->SetSD(CryoW2_log);

    G4VPhysicalVolume *CryoW2_phys =  new G4PVPlacement(0,
                          G4ThreeVector(0., 0., -Dz_end_tot/2.),
                          "LAr::TBBarrel::Cryostat::WarmTube2",
                          CryoW2_log,
                          Cent_phys,
                          false,
                          0);

//#endif

// Cryostat end plates at high z
//#ifdef BUILD_ENDCRYO


// warm vessel
/*#ifdef DEBUG_GEO
   std::cout << "End plate for warm vessel  Rmin,Rmax " << Rmin_mother << " "
             << Rmax_mother << std::endl;
   std::cout << " half size in z " << Dz_end_warm/2. << std::endl;
#endif*/

     G4Tubs* CryoEndW_tube = new G4Tubs("LAr::TBBarrel::Cryostat::EndWarm",
                                     Rmin_mother,
                                     Rmax_mother,
                                     Dz_end_warm/2.,
                                     Phi_Min,
                                     Phi_Span);

    G4LogicalVolume *CryoEndW_log = new G4LogicalVolume(CryoEndW_tube,
                                                     Aluminium,
                                     "LAr::TBBarrel::Cryostat::EndWarm");
    //g4vis->SetVis(CryoEndW_log);
    //g4sdc->SetSD(CryoEndW_log);

    G4double zwarm = Cryo_Distz/2. - Dz_end_warm/2.;
/*#ifdef DEBUG_GEO
    std::cout << " position in mother at z " << zwarm << std::endl;
#endif*/
    G4VPhysicalVolume *CryoEndW_phys =  new G4PVPlacement(0,
                          G4ThreeVector(0., 0., zwarm),
                          "LAr::TBBarrel::Cryostat::EndWarm",
                          CryoEndW_log,
                          Cent_phys,
                          false,
                          0);


// vaccum part
/*#ifdef DEBUG_GEO
   std::cout << "End plate for vacuum       Rmin,Rmax " << Rmin_mother << " "
             << Rmax_mother << std::endl;
   std::cout << " half size in z " << Dz_end_vac/2. << std::endl;
#endif*/

     G4Tubs* CryoEndV_tube = new G4Tubs("LAr::TBBarrel::Cryostat::EndVac",
                                     Rmin_mother,
                                     Rmax_mother,
                                     Dz_end_vac/2.,
                                     Phi_Min,
                                     Phi_Span);

    G4LogicalVolume *CryoEndV_log = new G4LogicalVolume(CryoEndV_tube,
                                                     Vacuum,
                                     "LAr::TBBarrel::Cryostat::EndVac");
    //g4sdc->SetSD(CryoEndV_log);

    G4double zvac = Cryo_Distz/2. - Dz_end_warm - Dz_end_vac/2.;
/*#ifdef DEBUG_GEO
    std::cout << " position in mother at z " << zvac << std::endl;
#endif*/
    G4VPhysicalVolume *CryoEndV_phys =  new G4PVPlacement(0,
                          G4ThreeVector(0., 0., zvac),
                          "LAr::TBBarrel::Cryostat::EndVac",
                          CryoEndV_log,
                          Cent_phys,
                          false,
                          0);
// cold vessel
/*#ifdef DEBUG_GEO
   std::cout << "End plate for cold vessel  Rmin,Rmax " << Rmin_mother << " "
             << Rmax_mother << std::endl;
   std::cout << " half size in z " << Dz_end_cold/2. << std::endl;
#endif*/

     G4Tubs* CryoEndC_tube = new G4Tubs("LAr::TBBarrel::Cryostat::EndCold",
                                     Rmin_mother,
                                     Rmax_mother,
                                     Dz_end_cold/2.,
                                     Phi_Min,
                                     Phi_Span);

    G4LogicalVolume *CryoEndC_log = new G4LogicalVolume(CryoEndC_tube,
                                                     Aluminium,
                                     "LAr::TBBarrel::Cryostat::EndCold");

    //g4vis->SetVis(CryoEndC_log);
    //g4sdc->SetSD(CryoEndC_log);

    G4double zcold = Cryo_Distz/2. - Dz_end_warm - Dz_end_vac -Dz_end_cold/2.;
/*#ifdef DEBUG_GEO
    std::cout << " position in mother at z " << zcold << std::endl;
#endif*/
    G4VPhysicalVolume *CryoEndC_phys =  new G4PVPlacement(0,
                          G4ThreeVector(0., 0., zcold),
                          "LAr::TBBarrel::Cryostat::EndCold",
                          CryoEndC_log,
                          Cent_phys,
                          false,
                          0);



//#endif

    // --------------------------------------------------------------------
    // Place the barrel test module inside the LAR volume (moth_phys)
    // --------------------------------------------------------------------

    //#ifdef BUILD_LARMODULE
    LArBarrelConstruction * barrel = new LArBarrelConstruction();
    //g4vis->AddConsultant( new LArBarrelVisualConsultant() );
    //g4sdc->AddConsultant( new LArBarrelSDConsultant() );

    G4cout << "->before GetEnvelope barrel " << G4endl;

    G4LogicalVolume* barrelPosEnvelope = barrel->GetEnvelope();

    //z=0 of ECAM is z=0 of Atlas
    //z=0 of moth_phys is at + LAr_z_max/2.-Cryo_z0 in atlas frame

    double Zcd = -LAr_z_max/2.+Cryo_z0;

    /*#ifdef DEBUG_GEO
    std::cout << " " << std::endl;
    std::cout << " Position ECAM volume in mother LAr at z " << Zcd << std::endl;
    #endif*/
    G4VPhysicalVolume* barrelPosPhysical =
         new G4PVPlacement(0,                          // Rotation matrix
                           G4ThreeVector(0.,0.,Zcd),   // Translation
                           barrelPosEnvelope->GetName(),     // Name
                           barrelPosEnvelope,          // Logical volume
                           moth_phys,                  // Mother volume
                           false,                      // Boolean volume?
                           0);                         // Copy number
    
    //#endif


    // ------------------------------------------------------------------------
    // Place the Presampler test module inside the LAr volume (moth_phys)
    // ------------------------------------------------------------------------
    //#ifdef BUILD_PRESAMPLER

    //atlas uses "ps", changed to PreSampler to avoid redefinition of CLHEP::ps lorenzo
    LArBarrelPresamplerConstruction * PreSampler = new LArBarrelPresamplerConstruction(1);
    //g4vis->AddConsultant( new LArBarrelPresamplerVisualConsultant() );
    //g4sdc->AddConsultant( new LArBarrelPresamplerSDConsultant() );

    G4LogicalVolume* barrelPSEnvelope = PreSampler->GetEnvelope();

    // PS lenght = 2*1582.5
    // start should be a z=0 in Atlas  => z = -LAr_z_max/2.+Cryo_z0 in moth_phys
    // center of PS in moth phys should be at 1582.5-Cryo_Distz+Cryo_z0 in moth_phys

    //   Zcd = 1582.5-LAr_z_max/2.+Cryo_z0;
    // new value of PS mother lenght
    Zcd = 1550.0*mm-LAr_z_max/2.+Cryo_z0;

    //#ifdef DEBUG_GEO
    //std::cout << " " << std::endl;
    //std::cout << " Position PS volume in mother LAr at z " << Zcd << std::endl;
    //#endif

    G4VPhysicalVolume * barrelPSPhysical =
        new G4PVPlacement(0,
                         G4ThreeVector(0.,0.,Zcd),
                         barrelPSEnvelope->GetName(),
                         barrelPSEnvelope,
                         moth_phys,
                         false,
                         0);
    //#endif

    return Em_log;
}

//**************************************************
