#include "ncgamma_xsec.h"
#include "bdt_file.h"
using std::cout;
using std::endl;
//double energyCorr(double);

void make_2dHisto() {
    /////////////////////////////////////////////////////////
    ////////////// INITIALIZATION /////////////////////
    ////////////////////////////////////////////////////////

    bool targetC12 = true; //if true = c12, false = Ar40


    // Get signal file and TTree
    TString dir = "/pnfs/uboone/persistent/users/ksutton/ncgamma_genie/10_1_19/";

    TFile *fin;
    if (!targetC12){
        fin = new TFile(dir+"gntp.nu_coh_1GeV.gst.root", "READ");
    } else{
        fin = new TFile(dir+"gntp.nu_coh_C12_1GeV.gst.root", "READ");
    }
    TTree *t = (TTree*)fin->Get("gst");

    TFile *fxsec = new TFile(dir+"gxsec.root", "READ");
    TGraph *gxsec;
    if (targetC12) {
        gxsec = (TGraph*)fxsec->Get("nu_mu_C12/coh_nc");
    } else{
        gxsec = (TGraph*)fxsec->Get("nu_mu_Ar40/coh_nc");
    } //need to select correct target and nu/nubar get the coh nc component

    // Declare necessary tree variables and set branch address
    //final state
    double pxf[2] = {}; //vector of momenta for final state particles
    double pyf[2] = {};
    double pzf[2] = {};
    double pdgf[2] = {}; //pdg's of particles
    double Ef[2] = {}; //energy of particles
    int nf = 0;        //number of final state particles

    //probe
    double Ev = 0.;    //energy of initial neutrino
    double pxv= 0  ; //vector of initial momentum
    double pyv = 0. ;  
    double pzv = 0. ;


    TBranch *bpxf = 0;
    TBranch *bpyf = 0;
    TBranch *bpzf = 0;
    TBranch *bpdgf = 0;
    TBranch *bEf = 0;
    TBranch *bnf = 0; 
    TBranch *bEv = 0; 
    TBranch *bpxv = 0;
    TBranch *bpyv = 0;
    TBranch *bpzv = 0;

    t->SetBranchAddress("pxf", &pxf, &bpxf); //p of final state particles (photon and neutrino)
    t->SetBranchAddress("pyf", &pyf, &bpyf);
    t->SetBranchAddress("pzf", &pzf, &bpzf);
    t->SetBranchAddress("pdgf", &pdgf, &bpdgf); //pdg of "
    t->SetBranchAddress("Ef", &Ef, &bEf);//energies of "
    t->SetBranchAddress("nf", &nf, &bnf);//number of final state particles
    t->SetBranchAddress("Ev", &Ev, &bEv);//energy of probe (neutrino)
    t->SetBranchAddress("pxv", &pxv, &bpxv); //p of initial state particles (neutrino)
    t->SetBranchAddress("pyv", &pyv, &bpyv);
    t->SetBranchAddress("pzv", &pzv, &bpzv);


    // Define output file and histograms
    TFile *fout = new TFile("ncgamma_xsec_out.root", "RECREATE");
    //1D
    TH1D *h_Eg = new TH1D("h_Eg", "h_Eg", 100, 0, 1);
    TH1D *h_thetag = new TH1D("h_thetag", "h_thetag", 100, 0, 3.14);
    // TH1D *h_thetag = new TH1D("h_thetag", "h_thetag", 100, 0, 3.14/2);
    TH1D *h_phig = new TH1D("h_phig", "h_phig", 100, -3.14, 3.14);
    //TH1D *h_phil = new TH1D("h_phil", "h_phil", 100, -3.14, 3.14);
    TH1D *h_thetal = new TH1D("h_thetal", "h_thetal", 100, 0, 3.14/2);
    TGraph * xsec_copy = (TGraph*)gxsec->Clone();
    //2D
    TH2D *h_Egthetag = new TH2D("h_Egthetag", "h_Egthetag", 100, 0, 1, 100,  0, 3.14/2);
    TH2D *h_Egthetal = new TH2D("h_Egthetal", "h_Egthetal", 100, 0, 1, 100,  0, 3.14/2);
    TH2D *h_Egphig = new TH2D("h_Egphig", "h_Egphig", 100, 0, 1, 100,  -3.14, 3.14);
    TH2D *h_thetagthetal = new TH2D("h_thetagthetal", "h_thetagthetal", 100, 0, 3.14/2, 100,  0, 3.14/2);
    TH2D *h_thetagphig = new TH2D("h_thetagphig", "h_thetagphig", 100, 0, 3.14/2, 100,  -3.14, 3.14);
    TH2D *h_phigthetal = new TH2D("h_phigthetal", "h_phigthetal", 100, -3.14, 3.14, 100,  0, 3.14/2);


    /////////////////////////////////////////////////////////
    ////////////// HISTO FILL LOOP /////////////////////////
    ////////////////////////////////////////////////////////

    //we're interested in Eg, theta_l, theta_g, and phi_g
    //need to check which of the final particles is l and g since assuming coh nc gamma

    double Eg; //energy of photon fsp
    double theta_g; 
    double phi_g;
    double phi_l;
    double theta_l;
    double this_Ev; //assumes same Ev for a file

    TVector3 p3_probe;// = TVector3(pxv,pyv,pzv); //incoming neutrino
    TLorentzVector p4_probe;
    TLorentzVector p4_gamma; //outgoing photon
    TLorentzVector p4_lepton; //outgoing neutrino

    //   std::cout<<"the incoming neutrino momentum = ("<<  p3_probe.X()<<", "<< p3_probe.Y()<<", "<<  p3_probe.Z()<<")"<<std::endl;

    for (int i = 0; i < t->GetEntries(); i++) {
        //      std::cout<<"the incoming neutrino momentum = ("<<  p3_probe.X()<<", "<< p3_probe.Y()<<", "<<  p3_probe.Z()<<")"<<std::endl;

        t->GetEntry(i);
        if (nf != 2){
            std::cout<<"ERROR! expect only two particles, nf = "<<nf<<std::endl;
        }

        if (i ==0){
            this_Ev = Ev;
        }

        pxv = 0.2;
        p3_probe= TVector3(pxv,pyv,pzv);
        p4_probe = TLorentzVector(p3_probe, Ev);

        //the outgoing photon and lepton should be relative to the incoming neutrino direction 
        //check which of the fsp's is the photon
        TVector3 p3_gamma;
        TVector3 p3_lepton;
        if (pdgf[0] == 22){ 
            p3_gamma = TVector3(pxf[0],pyf[0],pzf[0]);
            p3_lepton = TVector3(pxf[1],pyf[1],pzf[1]);

            p4_gamma = TLorentzVector(p3_gamma,  Ef[0] );
            p4_lepton = TLorentzVector(p3_lepton,  Ef[1] );
            // p4_gamma = TLorentzVector(TVector3(pxf[0],pyf[0],pzf[0]), Ef[0] ); //4vector for g and l          
            //p4_lepton = TLorentzVector(TVector3(pxf[1],pyf[1],pzf[1]), Ef[1] ); //4vector for g and l          

        } else{
            p3_gamma = TVector3(pxf[1],pyf[1],pzf[1]);
            p3_lepton = TVector3(pxf[0],pyf[0],pzf[0]);

            p4_gamma = TLorentzVector(p3_gamma,  Ef[1] );
            p4_lepton = TLorentzVector(p3_lepton,  Ef[0] );

            //   p4_gamma = TLorentzVector(TVector3(pxf[1],pyf[1],pzf[1])- p3_probe, Ef[1] ); //4vector for g and l          
            //  p4_lepton = TLorentzVector(TVector3(pxf[0],pyf[0],pzf[0])- p3_probe, Ef[0] ); //4vector for g and l          


            // std::cout<<"lepton at n=0: pg(x, y, z) = "<<pxf[0]<<", "<<pyf[0]<<", "<< pzf[0]<<", El = "<<Ef[0]<<std::endl;
            // std::cout<<"photon at n=1: pg(x, y, z) = "<<pxf[1]<<", "<<pyf[1]<<", "<< pzf[1]<<", Ef = "<<Ef[1]<<std::endl;
        }

        /*
           std::cout<<"--------------------------- on event "<<i<<" ----------------------------------"<<std::endl;
           std::cout<<"photon: pg(x, y, z) = "<<  p4_gamma.Px()<<", "<< p4_gamma.Py()<<", "<< p4_gamma.Pz() <<", Eg = "<<p4_gamma.E()<<std::endl;
           std::cout<<"lepton: pl(x, y, z) = "<<  p4_lepton.Px()<<", "<< p4_lepton.Py()<<", "<< p4_lepton.Pz() <<", Eg = "<<p4_lepton.E()<<std::endl;
           */

        Eg = p4_gamma.E();

        //get angle between probe and z axis
        TVector3 z = TVector3(0, 0, 1);

        //make copy
        TVector3 p3_gamma_r = p3_gamma;
        TVector3 p3_lepton_r = p3_lepton;
        TVector3 p3_probe_r = p3_probe;

        double angle;
        TVector3 cross =  p4_probe.Vect().Cross(z);
        std::cout<<"angle between probe and z axis is "<<angle<<std::endl;

        std::cout<<"photon before rotation: pg(x, y, z) = "<<  p3_gamma.Px()<<", "<< p3_gamma.Py()<<", "<< p3_gamma.Pz() <<std::endl;
        std::cout<<"lepton before rotation: pl(x, y, z) = "<<  p3_lepton.Px()<<", "<< p3_lepton.Py()<<", "<< p3_lepton.Pz() <<std::endl;
        std::cout<<"lepton after rotation: pl(x, y, z) = "<<  p3_probe.Px()<<", "<< p3_probe.Py()<<", "<< p3_probe.Pz() <<std::endl;

        //if there is a rotation
        if (angle !=0.0){
            //rotate the gamma, lepton,probe
            p3_gamma_r.Rotate(angle,z);
            p3_lepton_r.Rotate(angle,z);  
            p3_probe_r.Rotate(angle,z);  
        }

        std::cout<<"photon after rotation: pg(x, y, z) = "<<  p3_gamma_r.Px()<<", "<< p3_gamma_r.Py()<<", "<< p3_gamma_r.Pz() <<std::endl;
        std::cout<<"lepton after rotation: pl(x, y, z) = "<<  p3_lepton_r.Px()<<", "<< p3_lepton_r.Py()<<", "<< p3_lepton_r.Pz() <<std::endl;
        std::cout<<"lepton after rotation: pl(x, y, z) = "<<  p3_probe_r.Px()<<", "<< p3_probe_r.Py()<<", "<< p3_probe_r.Pz() <<std::endl;


        //then get the delta phi 
        phi_g = p3_gamma_r.DeltaPhi(p3_probe_r);
        phi_l = p3_lepton_r.DeltaPhi(p3_probe_r);

        //angle between the photon and lepton and the probe
        theta_l = p4_lepton.Angle(p4_probe.Vect());
        theta_g = p4_gamma.Angle(p4_probe.Vect());

        h_Eg->Fill(Eg);
        h_thetag->Fill(theta_g);
        h_phig->Fill(phi_g);
        h_thetal->Fill(theta_l);
        //h_phil->Fill(phi_l);

        h_Egthetag->Fill(Eg, theta_g);
        h_Egthetal->Fill(Eg, theta_l);
        h_Egphig->Fill(Eg, phi_g);
        h_thetagthetal->Fill(theta_g, theta_l);
        h_phigthetal->Fill(phi_g, theta_l);
        h_thetagphig->Fill(theta_g,phi_g);


    }


    //1D
    //area normalize the histograms
    double norm = gxsec->Eval(this_Ev) * 1e-38; //read in total xsec for the energy
    std::cout<<"The total xsec for Ev= "<<this_Ev<<" is "<<norm<<std::endl;

    h_Eg->Scale(norm*(h_Eg->GetXaxis()->GetBinWidth(1))/h_Eg->Integral());
    h_thetag->Scale(norm*(h_thetag->GetXaxis()->GetBinWidth(1))/h_thetag->Integral());
    h_phig->Scale(norm*(h_phig->GetXaxis()->GetBinWidth(1))/h_phig->Integral());
    h_thetal->Scale(norm*(h_thetal->GetXaxis()->GetBinWidth(1))/h_thetal->Integral());
    //    h_phil->Scale(norm*(h_phil->GetXaxis()->GetBinWidth(1))/h_phil->Integral());



    /*    h_Eg->Scale(norm/(h_Eg->Integral()));
          h_thetag->Scale(norm/(h_thetag->Integral()));
          h_phig->Scale(norm/(h_phig->Integral()));
          h_thetal->Scale(norm/(h_thetal->Integral()));
          */

    h_Eg->SetOption("hist");
    h_thetag->SetOption("hist");
    h_phig->SetOption("hist");
    // h_phil->SetOption("hist");
    h_thetal->SetOption("hist");

    //x axis labels
    h_Eg->GetXaxis()->SetTitle("E_{#gamma} [GeV]");
    h_thetag->GetXaxis()->SetTitle("#theta_{#gamma} [rad]");
    //    h_phil->GetXaxis()->SetTitle("#phi_{l} [rad]");
    h_phig->GetXaxis()->SetTitle("#phi_{#gamma} [rad]");
    h_thetal->GetXaxis()->SetTitle("#theta_{l} [rad]");
    /*
       h_Eg->GetXaxis()-> SetTitleSize(0.05);
       h_thetag->GetXaxis()-> SetTitleSize(0.06);
       h_phig->GetXaxis()-> SetTitleSize(0.06);
       h_thetal->GetXaxis()-> SetTitleSize(0.06);
       */

    //y axis labels
    h_Eg->GetYaxis()->SetTitle("#frac{d#sigma}{dE_{#gamma}} [cm^{2}GeV^{-1}]");
    h_thetag->GetYaxis()->SetTitle("#frac{d#sigma}{d#theta_{#gamma}} [cm^{2}rad^{-1}]");
    h_phig->GetYaxis()->SetTitle("#frac{d#sigma}{d#phi_{#gamma}} [cm^{2}rad^{-1}]");
    //  h_phil->GetYaxis()->SetTitle("#frac{d#sigma}{d#phi_{l}} [cm^{2}rad^{-1}]");
    h_thetal->GetYaxis()->SetTitle("#frac{d#sigma}{d#theta_{l}} [cm^{2}rad^{-1}]");

    /*  h_Eg->GetYaxis()-> SetTitleSize(0.04);
        h_thetag->GetYaxis()-> SetTitleSize(0.04);
        h_phig->GetYaxis()-> SetTitleSize(0.04);
        h_thetal->GetYaxis()-> SetTitleSize(0.04);
        */

    //add title with neutrino energy and target
    std::string title;
    if (targetC12){
        title = "E_{#nu}= "+ to_string_prec(this_Ev, 1) + " GeV, ^{12}C";
    }else{
        title = "E_{#nu}= "+ to_string_prec(this_Ev, 1) + " GeV, ^{40}Ar";
    }


    h_Eg->SetTitle(title.c_str());
    h_thetag->SetTitle(title.c_str());
    h_phig->SetTitle(title.c_str());
    h_thetal->SetTitle(title.c_str());

    //2D
    h_Egthetag->SetOption("colz");
    h_Egthetal->SetOption("colz");
    h_Egphig->SetOption("colz");
    h_thetagthetal->SetOption("colz");
    h_thetagphig ->SetOption("colz");
    h_phigthetal->SetOption("colz");


    h_Egthetag->GetXaxis()->SetTitle("E_{#gamma} [GeV]");
    h_Egthetal->GetXaxis()->SetTitle("E_{#gamma} [GeV]");
    h_Egphig->GetXaxis()->SetTitle("E_{#gamma} [GeV]");
    h_thetagthetal->GetXaxis()->SetTitle("#theta_{#gamma}");
    h_thetagphig->GetXaxis()->SetTitle("#theta_{#gamma}");  
    h_phigthetal->GetXaxis()->SetTitle("#phi_{#gamma} [GeV]");

    h_Egthetag->GetYaxis()->SetTitle("#theta_{#gamma}");
    h_Egphig->GetYaxis()->SetTitle("#phi_{#gamma}");
    h_thetagphig->GetYaxis()->SetTitle("#phi_{#gamma}");
    h_phigthetal->GetYaxis()->SetTitle("#theta_{l}");
    h_thetagthetal->GetYaxis()->SetTitle("#theta_{l}");
    h_Egthetal->GetYaxis()->SetTitle("#theta_{l}");




    //Tgraph
    xsec_copy->GetXaxis()->SetRangeUser(0, 1.6);
    xsec_copy->GetYaxis()->SetRangeUser(0, 0.025);

    //   for (int i=0;i<xsec_copy->GetN();i++) xsec_copy->GetY()[i] *= 1e3;
    //convert to e-14 to match eduardo

    xsec_copy->Write("xsec");


    //close files
    fxsec -> Close();
    fin->Close();
    fout->Write();
    fout->Close();

}

int main() {
    make_2dHisto();
    return 0;
}

/*
// Energy correction functions
// Parameters extracted from fit performed in energy_cal.cc
double energyCorr(double energy) {
// MCC8
//double corr = 1.24476*energy + 0.015528;
// MCC9ish (v10)
double corr = 1.24288*energy + 8.64122;
return corr;
}
*/


