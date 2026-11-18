//**************************************************
// \file LArBarrelConstruction.hh
// \brief: definition of 
//         LArBarrelConstruction class
// \author: originally from ATLAS-G4
//          adaptation by 
//          Lorenzo Pezzotti (CERN EP-SFT-sim) 
//          @lopezzot
// \start date: 10 August 2022
//**************************************************

#ifndef LArBarrelConstruction_h
#define LArBarrelConstruction_h 1

//Includers from Geant4
//
#include "G4LogicalVolume.hh"

class LArBarrelConstruction{

    public:
        LArBarrelConstruction();
        ~LArBarrelConstruction();

        //Get the envelope containing this detector
        //
        G4LogicalVolume* GetEnvelope();

    private: 
        double da, Da, de, De, Alfa;
        // Pointer to description of detector parameters ACCG, ACCA, ACMB, ACCO
        //LArVG4DetectorParameters* m_parameters;

        // Variables linked to the ACCG structure
        double Gama0, Psi;
        double Tgpb,Tgfe,Tggl,Thpb,Thfe,Thgl;
        double Thcu,Thfg;
        double Moth_Z_min, Moth_Z_max; 
        double Moth_inner_radius, Moth_outer_radius;
        int Nbrt, Nbrt1, Nplus;
        double Xel1f, Xtal, Xg10f, Xg10b;
        double Rhocen1, Rhocen15;
        double Bar_Eta_cut, Bar_Eta_cut1;
        double Rint;
        double Rhocen[15];
        double Phicen[15];
        double Delta[15];
        double Xtip_pb, Xtip_pc,  Xtip_gt, Xtip_gs;  

        // Variables linked to the ASAG structure
        double deltay[15];

        // Variables linked to the ACCA structure
        double Dx1, Dx2, Dy1, Dy2;
        double ClearancePS; 
        int NoOFcable;

        // Variables linked to the ACMB structure
        double ThMBcu, ThMBG10;
        double bdx, bdy, bdz; 
        int NoOFboard; 

        // Define logical variables for RUN conditions
        bool A_SAGGING;
        bool IflCur;
        bool IflMapTrans;

        // volumes that are private member variables:
        G4VPhysicalVolume *  Stac_phys1 ;

        // three functions to be used in calculations in the Accordion geometry
        // ( in the Construct() method ):
        inline G4double fx( G4double r, 
			G4double G, 
			const G4double Cenx[], 
			const G4double Ceny[] ) const;

        inline G4double fy( G4double r, 
			G4double G, 
			const G4double Cenx[], 
			const G4double Ceny[] ) const;

        inline double dely( const double gamfirstabs, double G ) const;
};

G4double LArBarrelConstruction::fx( G4double r, G4double G, const G4double Cenx[], const G4double Ceny[] ) const 
{
    G4int i = (int)rint(r-.1), j = (int)rint(r+.1) ;
    return  (cos(G)*(Cenx[i]+Cenx[j])/2-sin(G)*(Ceny[i]+Ceny[j])/2) ;
}

G4double LArBarrelConstruction::fy( G4double r, G4double G, const G4double Cenx[], const G4double Ceny[] ) const
{
    G4int i = (int)rint(r-.1), j = (int)rint(r+.1) ;
    return  (sin(G)*(Cenx[i]+Cenx[j])/2+cos(G)*(Ceny[i]+Ceny[j])/2) ;
}

G4double LArBarrelConstruction::dely( const G4double Gama0, G4double G ) const 
{
    return fabs(cos( G ) );
}

#endif //LArBarrelConstruction_h 1

//**************************************************