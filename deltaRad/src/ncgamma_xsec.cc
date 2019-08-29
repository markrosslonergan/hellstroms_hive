#include "ncgamma_xsec.h"
using std::cout;
using std::endl;
//double energyCorr(double);

void make_2dHisto() {
    /////////////////////////////////////////////////////////
    ////////////// INITIALIZATION /////////////////////
    ////////////////////////////////////////////////////////

    // Get signal file and TTree
    TString dir = "/pnfs/uboone/persistent/users/ksutton/ncgamma_genie/";
    TFile *fin = new TFile(dir+"gntp.nu_coh_1GeV.gst.root", "READ");
    TTree *t = (TTree*)fin->Get("gst");

    // Declare necessary tree variables and set branch address
    double pxf[2] = {}; //vector of momenta for final state particles
    double pyf[2] = {};
    double pzf[2] = {};
    double pdgf[2] = {}; //pdg's of particles
    double Ef[2] = {}; //energy of particles
    int nf = 0;        //number of final state particles

    TBranch *bpxf = 0;
    TBranch *bpyf = 0;
    TBranch *bpzf = 0;
    TBranch *bpdgf = 0;
    TBranch *bEf = 0;
    TBranch *bnf = 0;  

    t->SetBranchAddress("pxf", &pxf, &bpxf);
    t->SetBranchAddress("pyf", &pyf, &bpyf);
    t->SetBranchAddress("pzf", &pzf, &bpzf);
    t->SetBranchAddress("pdgf", &pdgf, &bpdgf);
    t->SetBranchAddress("Ef", &Ef, &bEf);
    t->SetBranchAddress("nf", &nf, &bnf);


    // Define output file and histograms
    TFile *fout = new TFile("ncgamma_xsec_out.root", "RECREATE");
    TH1D *h_Eg = new TH1D("h_Eg", "h_Eg", 100, 0, 1);
    TH1D *h_thetag = new TH1D("h_thetag", "h_thetag", 100, 0, 3.14);
    TH1D *h_phig = new TH1D("h_phig", "h_phig", 100, -3.14, 3.14);
    TH1D *h_thetal = new TH1D("h_thetal", "h_thetal", 100, 0, 3.14);


    /////////////////////////////////////////////////////////
    ////////////// HISTO FILL LOOP /////////////////////////
    ////////////////////////////////////////////////////////

    //we're interested in Eg, theta_l, theta_g, and phi_g
    //need to check which of the final particles is l and g since assuming coh nc gamma

    double Eg; //energy of photon fsp
    double theta_g; 
    double phi_g;
    double theta_l;

    TLorentzVector p4_gamma;
    TLorentzVector p4_lepton;

    for (int i = 0; i < t->GetEntries(); i++) {

        t->GetEntry(i);
        if (nf != 2){
            std::cout<<"ERROR! expect only two particles, nf = "<<nf<<std::endl;
        }

        //  std::cout<<"lepton at n=0: pg(x, y, z) = "<<pxf[0]<<", "<<pyf[0]<<", "<< pzf[0]<<", El = "<<Ef[0]<<std::endl;
        //std::cout<<"photon at n=1: pg(x, y, z) = "<<pxf[1]<<", "<<pyf[1]<<", "<< pzf[1]<<", Ef = "<<Ef[1]<<std::endl;

        //check which of the fsp's is the photon
        if (pdgf[0] == 22){ 
            p4_gamma = TLorentzVector(TVector3(pxf[0],pyf[0],pzf[0]), Ef[0] ); //4vector for g and l          
            p4_lepton = TLorentzVector(TVector3(pxf[1],pyf[1],pzf[1]), Ef[1] ); //4vector for g and l          

            //std::cout<<"photon at n=0: pg(x, y, z) = "<<pxf[0]<<", "<<pyf[0]<<", "<< pzf[0]<<", Eg = "<<Eg<<std::endl;
        } else{
            p4_gamma = TLorentzVector(TVector3(pxf[1],pyf[1],pzf[1]), Ef[1] ); //4vector for g and l          
            p4_lepton = TLorentzVector(TVector3(pxf[0],pyf[0],pzf[0]), Ef[0] ); //4vector for g and l          


            // std::cout<<"lepton at n=0: pg(x, y, z) = "<<pxf[0]<<", "<<pyf[0]<<", "<< pzf[0]<<", El = "<<Ef[0]<<std::endl;
            // std::cout<<"photon at n=1: pg(x, y, z) = "<<pxf[1]<<", "<<pyf[1]<<", "<< pzf[1]<<", Ef = "<<Ef[1]<<std::endl;
        }
        /*
           std::cout<<"--------------------------- on event "<<i<<" ----------------------------------"<<std::endl;
           std::cout<<"photon: pg(x, y, z) = "<<  p4_gamma.Px()<<", "<< p4_gamma.Py()<<", "<< p4_gamma.Pz() <<", Eg = "<<p4_gamma.E()<<std::endl;
           std::cout<<"lepton: pl(x, y, z) = "<<  p4_lepton.Px()<<", "<< p4_lepton.Py()<<", "<< p4_lepton.Pz() <<", Eg = "<<p4_lepton.E()<<std::endl;
           */
        Eg = p4_gamma.E();
        theta_g = p4_gamma.Theta();
        phi_g = p4_gamma.Phi();
        theta_l = p4_lepton.Theta();

        h_Eg->Fill(Eg);
        h_thetag->Fill(theta_g);
        h_phig->Fill(phi_g);
        h_thetal->Fill(theta_l);

    }

    //area normalize the histogramsi
    double norm = 1.0;

    h_Eg->Scale(norm/(h_Eg->Integral()));
    h_thetag->Scale(norm/(h_thetag->Integral()));
    h_phig->Scale(norm/(h_phig->Integral()));
    h_thetal->Scale(norm/(h_thetal->Integral()));

    h_Eg->SetOption("hist");
    h_thetag->SetOption("hist");
    h_phig->SetOption("hist");
    h_thetal->SetOption("hist");



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


