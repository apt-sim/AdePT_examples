#include <iostream>
#include <algorithm>
#include <vector>

void ATLLArBarrelAnalysis(){
    
    //Open file and get tree
    //
    std::string filename = "run_gps_output/ATLLArBarrelout_Run0.root";
    TFile* file = TFile::Open( filename.c_str(), "READ");
    TTree* tree = static_cast<TTree*>(file->Get("ATLLArBarrelout"));

    const int MiddleEtasPerRow = 16*2;
 
    //Set branch adresses
    //
    std::vector<double>* Front = nullptr; 
    std::vector<double>* Middle = nullptr; 
    std::vector<double>* Back = nullptr; 
    tree->SetBranchAddress( "FrontHitsEdep", &Front );
    tree->SetBranchAddress( "MiddleHitsEdep",&Middle );
    tree->SetBranchAddress( "BackHitsEdep",  &Back );

    //Allocate histograms
    //
    TH1F* RPhiH1 = new TH1F("RPhi_H1","RPhi",40,0.7,1.1);
    TH1F* REtaH1 = new TH1F("REta_H1","REta",40,0.7,1.1);
    TH1F* F1H1 = new TH1F("F1_H1","F1",55,0.0,1.1);
    TH1F* F2H1 = new TH1F("F2_H1","F2",55,0.0,1.1);

    //Allocate graphs
    //
    const int piFilesNo = 5;
    //TGraph* HadIntGr = new TGraph(5);
    double piEnergies[piFilesNo]{20.,50.,100.,150.,200.}; //GeV
    double piInt[piFilesNo]{0.,0.,0.,0.,0.};

    TRandom rndm;

    //Loop over events of gamma run
    //
    for(int evt=0; evt<tree->GetEntries(); evt++){
        tree->GetEntry(evt);
 
        double RPhi = 0.; //RPhi variable E3x3/E3x7 (middle layer)
        double REta = 0.; //REta variable E3x7/E7x7 (middle layer)
        double F1 = 0.;
        double F2 = 0.;
 
        //Find index of vector max (middle layer)
        //
        auto MiddleMax = std::distance(Middle->begin(),(std::max_element(Middle->begin(),Middle->end())));
        auto seed = rndm.Rndm();
        if(seed<0.03){
            MiddleMax=MiddleMax-1*MiddleEtasPerRow;
        }
        if(0.03<seed<0.06){
            MiddleMax=MiddleMax-1;
        }
        //std::cout<<MiddleMax<<std::endl;
        //if(MiddleMax!=271) continue; //exclude events where max is not centered in hit 271

        int MMRawM1 = MiddleMax - 1*MiddleEtasPerRow;
        int MMRawM2 = MiddleMax - 2*MiddleEtasPerRow;
        int MMRawM3 = MiddleMax - 3*MiddleEtasPerRow;
        int MMRawP1 = MiddleMax + 1*MiddleEtasPerRow;
        int MMRawP2 = MiddleMax + 2*MiddleEtasPerRow;
        int MMRawP3 = MiddleMax + 3*MiddleEtasPerRow;
 
        double E3x3 = Middle->at(MiddleMax)+Middle->at(MiddleMax-1)+Middle->at(MiddleMax+1)+
                      Middle->at(MMRawM1)+Middle->at(MMRawM1-1)+Middle->at(MMRawM1+1)+
                      Middle->at(MMRawP1)+Middle->at(MMRawP1-1)+Middle->at(MMRawP1+1);
 
        double E3x7 = Middle->at(MiddleMax)+Middle->at(MiddleMax-1)+Middle->at(MiddleMax+1)+
                      Middle->at(MMRawM1)+Middle->at(MMRawM1-1)+Middle->at(MMRawM1+1)+
                      Middle->at(MMRawM2)+Middle->at(MMRawM2-1)+Middle->at(MMRawM2+1)+
                      Middle->at(MMRawM3)+Middle->at(MMRawM3-1)+Middle->at(MMRawM3+1)+
                      Middle->at(MMRawP1)+Middle->at(MMRawP1-1)+Middle->at(MMRawP1+1)+
                      Middle->at(MMRawP2)+Middle->at(MMRawP2-1)+Middle->at(MMRawP2+1)+
                      Middle->at(MMRawP3)+Middle->at(MMRawP3-1)+Middle->at(MMRawP3+1);

        double E7x7 = Middle->at(MiddleMax)+Middle->at(MiddleMax-1)+Middle->at(MiddleMax+1)+Middle->at(MiddleMax-2)+Middle->at(MiddleMax+2)+Middle->at(MiddleMax-3)+Middle->at(MiddleMax+3)+
                      Middle->at(MMRawM1)+Middle->at(MMRawM1-1)+Middle->at(MMRawM1+1)+Middle->at(MMRawM1-2)+Middle->at(MMRawM1+2)+Middle->at(MMRawM1-3)+Middle->at(MMRawM1+3)+
                      Middle->at(MMRawM2)+Middle->at(MMRawM2-1)+Middle->at(MMRawM2+1)+Middle->at(MMRawM2-2)+Middle->at(MMRawM2+2)+Middle->at(MMRawM2-3)+Middle->at(MMRawM2+3)+
                      Middle->at(MMRawM3)+Middle->at(MMRawM3-1)+Middle->at(MMRawM3+1)+Middle->at(MMRawM3-2)+Middle->at(MMRawM3+2)+Middle->at(MMRawM3-3)+Middle->at(MMRawM3+3)+
                      Middle->at(MMRawP1)+Middle->at(MMRawP1-1)+Middle->at(MMRawP1+1)+Middle->at(MMRawP1-2)+Middle->at(MMRawP1+2)+Middle->at(MMRawP1-3)+Middle->at(MMRawP1+3)+
                      Middle->at(MMRawP2)+Middle->at(MMRawP2-1)+Middle->at(MMRawP2+1)+Middle->at(MMRawP2-2)+Middle->at(MMRawP2+2)+Middle->at(MMRawP2-3)+Middle->at(MMRawP2+3)+
                      Middle->at(MMRawP3)+Middle->at(MMRawP3-1)+Middle->at(MMRawP3+1)+Middle->at(MMRawP3-2)+Middle->at(MMRawP3+2)+Middle->at(MMRawP3-3)+Middle->at(MMRawP3+3);
        
        double TotHits = std::accumulate(Front->begin(), Front->end(), 0.)+
                         std::accumulate(Middle->begin(), Middle->end(), 0.)+
                         std::accumulate(Back->begin(), Back->end(), 0.);

        F1 = std::accumulate(Front->begin(), Front->end(), 0.) / TotHits;
        F2 = std::accumulate(Middle->begin(), Middle->end(), 0.) / TotHits;

        RPhi = E3x3/E3x7;
        REta = E3x7/E7x7;
        RPhiH1->Fill(RPhi);
        REtaH1->Fill(REta);
        F1H1->Fill(F1);
        F2H1->Fill(F2);

    } //end for loop over events of gamma file
 
    //Get HadInteraction values
    //
    double HadIntAvg = 0;
    for(int i=1; i<=piFilesNo;i++){
        std::string pifilename = "run_gps_output/ATLLArBarrelout_Run"+std::to_string(i)+".root";
        TFile* pifile = TFile::Open( pifilename.c_str(), "READ");
        TTree* pitree = static_cast<TTree*>(pifile->Get("ATLLArBarrelout"));
        int HadInt = 0;
        HadIntAvg = 0.;
        pitree->SetBranchAddress("HadInteraction",&HadInt);
        for(int entry=0; entry<pitree->GetEntries(); entry++){
            pitree->GetEntry(entry);
            HadIntAvg+=HadInt;
        }
        HadIntAvg = HadIntAvg/pitree->GetEntries();
        piInt[i-1]= HadIntAvg;
    }

    //Open output file and write histograms and graphs
    //
    TFile* OutFile = TFile::Open("OutputAnalysisBarrel.root", "RECREATE");
    //RPhiH1->ResetStats(); //this reset the stat computation using bin content
    std::cout<<"Rphi mean: "<<RPhiH1->GetMean()<<"+-"<<RPhiH1->GetMean(11)<<" std: "<<RPhiH1->GetStdDev()<<"+-"<<RPhiH1->GetStdDev(11)<<std::endl;
    RPhiH1->GetXaxis()->SetTitle("Rphi");
    RPhiH1->GetYaxis()->SetTitle("Entries");
    RPhiH1->Write();
    //REtaH1->ResetStats();
    std::cout<<"Reta mean: "<<REtaH1->GetMean()<<"+-"<<RPhiH1->GetMean(11)<<" std: "<<REtaH1->GetStdDev()<<"+-"<<RPhiH1->GetStdDev(11)<<std::endl;
    REtaH1->GetXaxis()->SetTitle("Reta");
    REtaH1->GetYaxis()->SetTitle("Entries");
    REtaH1->Write();
    F1H1->GetXaxis()->SetTitle("F1");
    F1H1->GetYaxis()->SetTitle("Entries");
    F1H1->Write();
    F2H1->GetXaxis()->SetTitle("F2");
    F2H1->GetYaxis()->SetTitle("Entries");
    F2H1->Write();
    TGraph* HadIntGr = new TGraph(piFilesNo, piEnergies, piInt);
    HadIntGr->SetTitle("");
    HadIntGr->SetMarkerStyle(8);
    HadIntGr->GetYaxis()->SetRangeUser(0.7,1.0);
    HadIntGr->GetXaxis()->SetTitle("Beam Energy [GeV]");
    HadIntGr->GetYaxis()->SetTitle("Nuclear Inelastic Probability");
    HadIntGr->Write();

    //Create and write canvases
    //
    TCanvas HadIntcanvas{"HadInt_canvas","HadInt canvas",600,600}; 
    HadIntcanvas.cd();
    HadIntGr->Draw();
    HadIntcanvas.SetLeftMargin(0.15);
    HadIntcanvas.Write();
    TCanvas RPhicanvas{"Rphi_canvas","Rphi canvas",600,600}; 
    RPhicanvas.cd();
    RPhiH1->Draw();
    RPhicanvas.SetLeftMargin(0.15);
    RPhicanvas.Write();
    TCanvas REtacanvas{"Reta_canvas","Reta canvas",600,600}; 
    REtacanvas.cd();
    REtaH1->Draw();
    REtacanvas.SetLeftMargin(0.15);
    REtacanvas.Write();
    TCanvas F1canvas{"F1_canvas","F1 canvas",600,600}; 
    F1canvas.cd();
    F1H1->Draw();
    F1canvas.SetLeftMargin(0.15);
    F1canvas.Write();
    TCanvas F2canvas{"F2_canvas","F2 canvas",600,600}; 
    F2canvas.cd();
    F2H1->Draw();
    F2canvas.SetLeftMargin(0.15);
    F2canvas.Write();

    OutFile->Close();
}
