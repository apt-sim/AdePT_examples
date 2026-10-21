//**************************************************
// \file LArBarrelPresamplerConstruction.cc
// \brief: implementation of 
//         LArBarrelPresamplerConstruction class
// \author: originally from ATLAS-G4
//          adaptation by 
//          Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 27 October 2022
//**************************************************

//Includers from project files
//
#include "LArBarrelPresamplerConstruction.hh"

//Includers from Geant4
//
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"

//Includers from c++
//
#include <cmath>

LArBarrelPresamplerConstruction::LArBarrelPresamplerConstruction(){
    #include "PresParameterDef.icc"
}

LArBarrelPresamplerConstruction::LArBarrelPresamplerConstruction(int itb){
    #include "PresParameterDef.icc"
    if (itb==1) {
        rMinPresamplerMother=1410.;   
        Phi_min=-0.5*CLHEP::deg;
        Phi_span=23.5*CLHEP::deg;
        nbsectors = 2;
    }
}

LArBarrelPresamplerConstruction::~LArBarrelPresamplerConstruction(){}

G4LogicalVolume* LArBarrelPresamplerConstruction::GetEnvelope(){

    
    G4String name,symbol;
    G4double a,z,density,fractionmass;
    G4int ncomponents,natoms;

    a = 12.011*CLHEP::g/CLHEP::mole;
    G4Element* Carbon = new G4Element(name="Carbon", symbol = "C" , z=6, a);

    a = 14.0067*CLHEP::g/CLHEP::mole;
    G4Element* Nitrogen = new G4Element(name="Nitrogen", symbol = "N" , z=7, a);

    a = 63.546*CLHEP::g/CLHEP::mole;
    G4Element* Copper = new G4Element(name="Copper", symbol = "Cu" , z=29, a);

    a = 1.00797*CLHEP::g/CLHEP::mole;
    G4Element* Hydrogen = new G4Element(name="Hydrogen", symbol = "H" , z=1, a);

    a = 39.948*CLHEP::g/CLHEP::mole;
    density = 1.396*CLHEP::g/CLHEP::cm3;
    G4Material* LAr = new G4Material(name="LiquidArgon",18.,a, density);

    a = 15.9995*CLHEP::g/CLHEP::mole;
    G4Element* Oxygen = new G4Element(name="Oxygen", symbol = "O" , z=8, a);

    density = 1.9*CLHEP::g/CLHEP::cm3;
    G4Material* FR4 = new G4Material(name="FR4",density,ncomponents=3);
    FR4->AddElement(Hydrogen, natoms=14);
    FR4->AddElement(Carbon, natoms=8);
    FR4->AddElement(Oxygen, natoms=4);

    density = 1.42*CLHEP::g/CLHEP::cm3;
    G4Material* Kapton = new G4Material(name="Kapton",density,ncomponents=4);
    Kapton->AddElement(Carbon, natoms=22);
    Kapton->AddElement(Hydrogen, natoms=10);
    Kapton->AddElement(Oxygen, natoms=5);
    Kapton->AddElement(Nitrogen, natoms=2);

    density = 2.24*CLHEP::g/CLHEP::cm3;
    G4Material* MBMat = new G4Material(name="MBMat",density,ncomponents=2);
    MBMat->AddMaterial(FR4, fractionmass=80.75*CLHEP::perCent);
    MBMat->AddElement(Copper, fractionmass=19.25*CLHEP::perCent);

    density = 3.06*CLHEP::g/CLHEP::cm3;
    G4Material* AnodeMat = new G4Material(name="AnodeMat",density,ncomponents=2);
    AnodeMat->AddElement(Copper, fractionmass=48.1*CLHEP::perCent);
    AnodeMat->AddMaterial(FR4, fractionmass=51.9*CLHEP::perCent);

    density = 3.73*CLHEP::g/CLHEP::cm3;
    G4Material* CathodeMat = new G4Material(name="CathodeMat",density,ncomponents=2);
    CathodeMat->AddElement(Copper, fractionmass=62.27*CLHEP::perCent);
    CathodeMat->AddMaterial(FR4, fractionmass=37.73*CLHEP::perCent);

    density = 3.98*CLHEP::g/CLHEP::cm3;
    G4Material* ConnecMat = new G4Material(name="ConnecMat",density,ncomponents=3);
    ConnecMat->AddElement(Copper, fractionmass=54.2*CLHEP::perCent);
    ConnecMat->AddMaterial(Kapton, fractionmass=38.7*CLHEP::perCent);
    ConnecMat->AddMaterial(LAr, fractionmass=7.1*CLHEP::perCent);

    //Use a contraction factor 
    //G4double cmm;
    //cmm=(1-contract)*mm;
    G4double epsil = 0.007*CLHEP::mm;
    G4int i=0;
    G4int j=0;

    // Generic name strings.
    G4String dname;

    //---------------- Electrodes  -----------------------------
    //
    G4double heig_elec1 =
        (larheight/cos(-mod[0][3]*CLHEP::deg)-0.5*anode_th/cos(mod[0][3]*CLHEP::deg))*cmm;
    G4double heig_elec3 = (larheight-0.5*cathode_th/cos(mod[1][3]*CLHEP::deg))*cmm;

    name = "LAr::Barrel::Presampler::";
    dname = name + "Cathode1";
    G4Trd* catho1
        = new G4Trd(dname,smallLength/2 *cmm,bigLength/2 *cmm,
                cathode_th/2 *cmm,cathode_th/2 *cmm,heig_elec1/2 *cmm);
    G4LogicalVolume* LV_catho1
        = new G4LogicalVolume(catho1,CathodeMat,dname,0,0,0);
    //g4vis->SetVis( LV_catho1 );
    //g4sdc->SetSD( LV_catho1 );

    dname = name + "Cathode3";
    G4Trd* catho3
        = new G4Trd(dname,smallLength/2 *cmm,bigLength/2 *cmm,
		cathode_th/2 *cmm,cathode_th/2 *cmm,heig_elec3/2);
    G4LogicalVolume* LV_catho3
        = new G4LogicalVolume(catho3,CathodeMat,dname,0,0,0);
    //g4vis->SetVis( LV_catho3 );
    //g4sdc->SetSD( LV_catho3 );

    dname = name + "Anode1";
    G4Trd* ano1
        = new G4Trd(dname,smallLength/2 *cmm,bigLength/2 *cmm,
		anode_th/2 *cmm,anode_th/2 *cmm,heig_elec1/2);
    G4LogicalVolume* LV_ano1
        = new G4LogicalVolume(ano1,AnodeMat,dname,0,0,0);
    //g4vis->SetVis( LV_ano1 );
    //g4sdc->SetSD( LV_ano1 );

    dname = name + "Anode3";
    G4Trd* ano3
        = new G4Trd(dname,smallLength/2 *cmm,bigLength/2 *cmm,
		anode_th/2 *cmm,anode_th/2 *cmm,heig_elec3/2);
    G4LogicalVolume* LV_ano3
        = new G4LogicalVolume(ano3,AnodeMat,dname,0,0,0);
    //g4vis->SetVis( LV_ano3 );
    //g4sdc->SetSD( LV_ano3 );


    //--------------- Prepreg. plates ---------------------------------------

    G4double prep1_height = (smallLength/2+1.)*cmm;
    G4double prep2_height = (bigLength/2+1.)*cmm;
    G4double prep1_z = (prep1_th/2)*cmm;
    G4double prep2_z = (prep2_th/2)*cmm;
    G4double prep_length[8];

    for( i=0; i<8; i++ ) 
        { 
        prep_length[i] = (mod[i][0]) *cmm;
        } 

    G4Box* Prep1[8];
    G4Box* Prep2[8];
    G4LogicalVolume* LV_Prep1[8];
    G4LogicalVolume* LV_Prep2[8];

    for( i=0; i<8; i++ )
        {
        dname = name + "Prep1";
        Prep1[i]
            = new G4Box(dname, prep1_height,prep_length[i]/2,prep1_z);
        LV_Prep1[i]
	    = new G4LogicalVolume(Prep1[i],FR4,dname,0,0,0);
        //g4vis->SetVis( LV_Prep1[i] );
      //g4sdc->SetSD( LV_Prep1[i] );
      
        dname = name + "Prep2";
        Prep2[i]
            = new G4Box(dname, prep2_height,prep_length[i]/2,prep2_z);
        LV_Prep2[i]
	    = new G4LogicalVolume(Prep2[i],FR4,dname,0,0,0);
        //g4vis->SetVis( LV_Prep2[i] );
        //g4sdc->SetSD( LV_Prep2[i] );
    }
    
    // define mother volumes (envelopes in AGDD)
    // envelopes of PS modules (trapezoids)
  
    G4double mod_xm;
    G4double mod_xp;
    G4double mod_leng[8];
    G4double mod_heig[8];
    G4double larheight2;
    larheight2 = larheight*cos(-mod[1][3]*CLHEP::deg)*CLHEP::mm;
    mod_xm = prep1_height+epsil;
    mod_xp = (bigLength/2+1.+prep2_th*tan((360./(2*nsectors))*CLHEP::deg))*cmm;

    G4Trd* env_module[8];
    G4LogicalVolume* PsModuleLog[8];

    dname = name + "Module";

    for( i=0; i<8; i++ ) mod_leng[i]=mod[i][0]*cmm+2*epsil;
    mod_heig[0]= (larheight+prep1_th+prep2_th)*cmm+4*epsil;
    mod_heig[1]= (larheight2+prep1_th+prep2_th)*cmm+5.*epsil;
    for( i=2; i<8; i++ ) mod_heig[i] = mod_heig[0];
    for( i=0; i<8; i++ )
        {
        env_module[i]
	    = new G4Trd(dname,mod_xm,mod_xp,mod_leng[i]/2,mod_leng[i]/2,mod_heig[i]/2);
      
        PsModuleLog[i]
	    = new G4LogicalVolume(env_module[i],LAr,dname,0,0,0);
        //g4vis->SetVis( PsModuleLog[i] );
        //g4sdc->SetSD( PsModuleLog[i] );
    }
  
    // sector envelope (trapezoid)

    G4double sector_length = mb_length*cmm +9.*epsil;
    G4double sector_height =
        mod_heig[0]+(shell_th+rail_th)*cmm+mech_clear*CLHEP::mm+3*epsil; 
    G4double sect_xm = mod_xm+epsil;
    G4double sect_xp = sect_xm+sector_height*tan((360./(2*nsectors))*CLHEP::deg);	    

    dname = name + "Sector";

    G4Trd* env_PSSECTOR
        = new G4Trd(dname,sect_xm,sect_xp,sector_length/2,sector_length/2,sector_height/2); 
    G4LogicalVolume* PsSectorLog
        = new G4LogicalVolume(env_PSSECTOR,LAr,dname,0,0,0);
    //g4vis->SetVis( PsSectorLog );
    //g4sdc->SetSD( PsSectorLog );

    //--------------------------------------------------------------------
    // Mother boards, connectics, protection plates ....

    dname = name + "MotherBoard";

    G4Box* MB
        = new G4Box(dname, (mb_width/2)*cmm,(mb_length/2)*cmm,(mb_th/2)*cmm);
    G4LogicalVolume* LV_MB
        = new G4LogicalVolume(MB,MBMat,dname,0,0,0);
    //g4vis->SetVis( LV_MB );
    //g4sdc->SetSD ( LV_MB );

    G4double shell_leng = 0.;
    for( i=0; i<8; i++) 
        {
        shell_leng += mod[i][0];   
        }
 
    dname = name + "ProtectionShell";

    G4Box* Prot_Shell
        = new G4Box(dname, (smallLength/2+1.)*cmm,
		(shell_leng/2)*cmm,(shell_th/2)*cmm);
    G4LogicalVolume* LV_Prot_Shell
        = new G4LogicalVolume(Prot_Shell,FR4,dname,0,0,0);
    //g4vis->SetVis( LV_Prot_Shell );
    //g4sdc->SetSD( LV_Prot_Shell );

    G4double conn_xm = (widthFront/2)*cmm;
    G4double conn_xp = (mb_width/2)*cmm;
    G4double conn_ym= (heightIn/2)*cmm;
    G4double conn_yp =(heightOut/2)*cmm;
    G4double conn_leng = (mb_length/2)*cmm;
    dname = name + "Connectics";
    G4Trd* connectics
        = new G4Trd(dname,conn_xm,conn_xp,conn_ym,conn_yp,conn_leng);
    G4LogicalVolume* LV_connectics
        = new G4LogicalVolume(connectics,ConnecMat,dname,0,0,0);
    //g4vis->SetVis( LV_connectics );
    //g4sdc->SetSD( LV_connectics );

    G4double prot_x = (bigLength/2+1.-rail_pos-rail_width+epsil)*cmm;
    G4double prot_y = (shell_leng/2)*cmm;
    G4double prot_z = (prot_th/2)*cmm;
    dname = name + "ProtectionPlate";
    G4Box* prot_plate
        = new G4Box(dname, prot_x,prot_y,prot_z);
    G4LogicalVolume* LV_prot_plate
        = new G4LogicalVolume(prot_plate,FR4,dname,0,0,0);
    //g4vis->SetVis( LV_prot_plate );
    //g4sdc->SetSD( LV_prot_plate );

    dname = name + "Rail";
    G4Box* rail
        = new G4Box(dname, (rail_width/2)*cmm,(shell_leng/2)*cmm,(rail_th/2)*cmm);
    G4LogicalVolume* LV_rail
        = new G4LogicalVolume(rail,FR4,dname,0,0,0);   
    //g4vis->SetVis( LV_rail );
    //g4sdc->SetSD( LV_rail );

    //---------------------------------------------------------------
    //
    // electrodes + prepreg plates ----> PS modules

    G4VPhysicalVolume* prep1_phys[8];
    G4VPhysicalVolume* prep2_phys[8];

    G4double prep1_pos[8];
    G4double prep2_pos[8];
    G4double elec_trans = -2*prep2_z+mod_heig[0]/2-(larheight/2)*cmm-3*epsil; 
    for( i=0; i<8; i++ )
        {
        prep1_pos[i] = prep1_z-mod_heig[i]/2+epsil;
        prep2_pos[i] = -prep2_z+mod_heig[i]/2-epsil;
        }

    G4double YStartA[8],YStartC[8];

    for( i=0; i<8; i++ ) 
        {
        YStartC[i] = -mod_leng[i]/2+(mod[i][5]+cathode_th/2)*cmm;
        YStartA[i] = YStartC[i]+(mod[i][4]/2)*cmm;
        }   
   
    // construct module(1)

    prep1_phys[0] = new G4PVPlacement(0,
				    G4ThreeVector(0*CLHEP::mm,0*CLHEP::mm,prep1_pos[0]),
				    LV_Prep1[0],
				    LV_Prep1[0]->GetName(),
				    PsModuleLog[0],false,0);

    prep2_phys[0] = new G4PVPlacement(0,
				    G4ThreeVector(0*CLHEP::mm,0*CLHEP::mm,prep2_pos[0]),
				    LV_Prep2[0],
				    LV_Prep2[0]->GetName(),
				    PsModuleLog[0],false,0);
			
    G4RotationMatrix* elec_rot = new G4RotationMatrix; 
    elec_rot-> rotateX(mod[0][3]*CLHEP::deg);		

    for( j=0; j<mod[0][2]; j++ )

        {
            G4VPhysicalVolume* cath_phys = new
	    G4PVPlacement(elec_rot,
		      G4ThreeVector(0.*CLHEP::mm,YStartC[0]+j*mod[0][4]*cmm,elec_trans),
		      LV_catho1,
		      LV_catho1->GetName(),
		      PsModuleLog[0],false,0);
    }

    for( j=0; j<mod[0][1]; j++ )

        {
        G4VPhysicalVolume* ano_phys = new
	    G4PVPlacement(elec_rot,
		      G4ThreeVector(0.*CLHEP::mm,YStartA[0]+j*mod[0][4]*cmm,elec_trans),
		      LV_ano1,
		      LV_ano1->GetName(),
		      PsModuleLog[0],false,0);

        }

    // modules 2--->8 - i : 1 --> 7 

    for( i=1; i<8; i++ )
        {
        prep1_phys[i] = new G4PVPlacement(0,
					G4ThreeVector(0*CLHEP::mm,0*CLHEP::mm,prep1_pos[i]),
					LV_Prep1[i],
					LV_Prep1[i]->GetName(),
					PsModuleLog[i],false,0);

        prep2_phys[i] = new G4PVPlacement(0,
					G4ThreeVector(0*CLHEP::mm,0*CLHEP::mm,prep2_pos[i]),
					LV_Prep2[i],
					LV_Prep2[i]->GetName(),
					PsModuleLog[i],false,0);

        G4RotationMatrix* elec_rot = new G4RotationMatrix; 
        elec_rot-> rotateX(mod[i][3]*CLHEP::deg);
        for( j=0; j<mod[i][2]; j++ )

	    {
	    G4VPhysicalVolume* cath_phys = new
	    G4PVPlacement(elec_rot,
			  G4ThreeVector(0.*CLHEP::mm,YStartC[i]+j*mod[i][4]*cmm,elec_trans),
			  LV_catho3,
			  "catho3",
			  PsModuleLog[i],false,0);
	    }

        for( j=0; j<mod[i][1]; j++ )

	    {
	    G4VPhysicalVolume* ano_phys = new
	    G4PVPlacement(elec_rot,
			  G4ThreeVector(0.*CLHEP::mm,YStartA[i]+j*mod[i][4]*cmm,elec_trans),
			  LV_ano3,
			  LV_ano3->GetName(),
			  PsModuleLog[i],false,0);

	    }
        }	      



    //------------------------------------------------------------------------
    //
    // modules + connectics + MB +  ... ------> PS sectors

    G4double modY[8],modZ[8];
    modY[0] = -sector_length/2+mod_leng[0]/2+epsil;
    modZ[0] = -sector_height/2+shell_th*cmm+mech_clear+mod_heig[0]/2+epsil;
    modZ[1] = modZ[0]+(mod_heig[0]-mod_heig[1])/2;
    for( i=1; i<8; i++)
    {
      modY[i] = modY[i-1]+mod_leng[i-1]/2+mod_leng[i]/2+epsil;
    }      
    for( i=2; i<8; i++) modZ[i]= modZ[0];

    G4VPhysicalVolume* PsModulePhys[8];

    for( i=0; i<8; i++)
    {      
      PsModulePhys[i]= new G4PVPlacement(0,
					 G4ThreeVector(0.*CLHEP::mm,modY[i],modZ[i]),
					 PsModuleLog[i],
					 PsModuleLog[i]->GetName(),
					 PsSectorLog,false,0);			
    }			

    G4double glX = 0.*CLHEP::mm;
    G4double glY = -sector_length/2+prot_y+epsil;
    G4double glZ = -sector_height/2+(shell_th/2)*cmm+epsil;

    G4VPhysicalVolume* Prot_Shell_phys = new G4PVPlacement(0,
							 G4ThreeVector(glX,glY,glZ),
							 LV_Prot_Shell,
							 LV_Prot_Shell->GetName(),
							 PsSectorLog,false,0);

    G4double mbZ = modZ[0]+mod_heig[0]/2+(mb_th/2)*cmm+epsil;

    G4VPhysicalVolume* mb_phys = new G4PVPlacement(0,
						 G4ThreeVector(0.*CLHEP::mm,0.*CLHEP::mm,mbZ),
						 LV_MB,
						 LV_MB->GetName(),
						 PsSectorLog,false,0);

    G4double protZ= mbZ+(mb_th/2+heightOut+prot_th/2)*cmm+2*epsil;
    G4VPhysicalVolume* prot_plate_phys = new G4PVPlacement(0,
							 G4ThreeVector(glX,glY,protZ),			   
							 LV_prot_plate,
							 LV_prot_plate->GetName(),
							 PsSectorLog,false,0);

    G4double connZ = mbZ+(mb_th/2+heightOut/2)*cmm+epsil;
    G4RotationMatrix* matrix = new G4RotationMatrix;
    matrix->rotateX (90.*CLHEP::deg);

    G4VPhysicalVolume* connectics_phys = new G4PVPlacement(matrix,
							 G4ThreeVector(0.*CLHEP::mm,0.*CLHEP::mm,connZ),
							 LV_connectics,
							 LV_connectics->GetName(),
							 PsSectorLog,false,0);

    G4double railX = (bigLength/2+1-rail_pos-rail_width/2)*cmm+epsil;
    G4double railZ = modZ[0]+mod_heig[0]/2+(rail_th/2)*cmm+epsil;

    G4VPhysicalVolume* rail1_phys = new G4PVPlacement(0,
						    G4ThreeVector(railX,glY,railZ),
						    LV_rail,
						    LV_rail->GetName(),
						    PsSectorLog,false,0);

    G4VPhysicalVolume* rail2_phys = new G4PVPlacement(0,
						    G4ThreeVector(-railX,glY,railZ),
						    LV_rail,
						    LV_rail->GetName(),
						    PsSectorLog,false,0);

    // construct a half barrel 

    name = "LAr::Barrel::Presampler";
    G4Tubs* env_BarrelPres
        = new G4Tubs(name, rMinPresamplerMother, rMaxPresamplerMother, 
		 PresamplerMother_length, Phi_min, Phi_span);

    G4LogicalVolume* BarrelPresLog
        = new G4LogicalVolume(env_BarrelPres,LAr,name,0,0,0);
    //g4vis->SetVis( BarrelPresLog );
    //g4sdc->SetSD( BarrelPresLog );

    G4double rpres = 1426.*CLHEP::mm;
    //  G4double zpres = 0.*CLHEP::mm;
    G4double zpres = -PresamplerMother_length+sector_length/2+epsil+2.97*CLHEP::mm;
    G4double PhiPres;
    G4double xpres;
    G4double ypres;
    G4double Phi0=360./(2*nsectors) *CLHEP::deg;
    G4double dPhi=(360./nsectors)*CLHEP::deg;

    for (G4int copy=0; copy < nbsectors; ++copy)
    {
      PhiPres = copy*dPhi+Phi0;
      xpres = rpres*cos(PhiPres);
      ypres = rpres*sin(PhiPres);
      
      G4RotationMatrix* sector_rot = new G4RotationMatrix;
      sector_rot->rotateZ(-PhiPres);
      sector_rot->rotateZ (-90.*CLHEP::deg);
      sector_rot->rotateX (-90.*CLHEP::deg);
		
      G4VPhysicalVolume* PSSECTOR_phys = new G4PVPlacement(sector_rot,
							   G4ThreeVector(xpres,ypres,zpres),
							   PsSectorLog,
							   PsSectorLog->GetName(),
							   BarrelPresLog,false,0);			
	
    }				

    //------------------- Sensitive Detectors-----------------------------------

    // Get the pointer to the singleton sensitive detector consultant.
    //  LArG4SensitiveDetectorConsultant* g4sdc = 
    //  LArG4SensitiveDetectorConsultant::GetInstance();

    //  for (i=0; i<8; i++ ) 
    //    {
      // Set the sensitive detector from the consultant.
    //  g4sdc->SetSD( PsModuleLog[i] );
    //  } 

    //-------------------end of sensitive detectors --------------------------------  

    return BarrelPresLog;

}

//**************************************************
