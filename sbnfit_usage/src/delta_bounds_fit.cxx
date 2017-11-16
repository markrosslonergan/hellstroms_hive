#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <unistd.h>
#include <getopt.h>
#include <cstring>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TString.h"
#include "TNtuple.h"
#include "TChain.h"
#include "TMath.h"
#include "TSystem.h"
#include "TMatrixT.h"
#include "TRandom.h"
#include "TError.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TGraph.h"

#include "prob.h"
#include "params.h"
#include "SBNconfig.h"
#include "SBNchi.h"
#include "SBNdet.h"
#include "SBNspec.h"
#include "SBNosc.h"
#include "SBgeN.h"
#include "SBNfit.h"
#include "SBNfit3pN.h"

#define no_argument 0
#define required_argument 1
#define optional_argument 2

using namespace sbn;


/*************************************************************
 *************************************************************
 *		BEGIN example.cxx
 ************************************************************
 ************************************************************/
int main(int argc, char* argv[])
{




	std::string xml = "default.xml";
	int iarg = 0;
	opterr=1;
	int index; 
	int test_mode=0;
	/*************************************************************
	 *************************************************************
	 *		Command Line Argument Reading
	 ************************************************************
	 ************************************************************/

	const struct option longopts[] = 
	{
		{"xml", 		required_argument, 	0, 'x'},
		{"test",		required_argument,	0, 't'},
		{0,			no_argument, 		0,  0},
	};


	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "x:t:", longopts, &index);

		switch(iarg)
		{
			case 'x':
				xml = optarg;
				break;
			case 't':
				test_mode = strtof(optarg,NULL);
				break;
			case '?':
			case 'h':
				std::cout<<"Allowed arguments:"<<std::endl;
				std::cout<<"\t-x\t--xml\t\tInput .xml file for SBNconfig"<<std::endl;
				return 0;
		}

	}


	/*************************************************************
	 *************************************************************
	 *		Example 3: Sterile neutrino, 3+1 sensitivity
	 ************************************************************
	 ************************************************************/

	//Time for a more model dependant example
	//using just SBNspec does not give you a huge amount of power. The idea is to make your own model dependant classes from this
	//such as here, SBNosc, a SBNspec precoded to do oscillation phyiscs (at SBL)

	//Load up your background, uboone was wierdly scaled in the precomp so fix here
	SBNspec delta_spec("../../delta_BDT", xml);
	delta_spec.calcFullVector();

	SBNspec bkg_spec = delta_spec;

	bkg_spec.Scale("signal",0.0);


	//again create a SBNchi from this spectra
	TMatrixD Mempty(delta_spec.fullVec.size(), delta_spec.fullVec.size());


	std::vector<double> ven;
	std::vector<double> vchi;


	TFile *f = new TFile("bound.root","recreate");

	int numMC = 5000;

	TRandom3 * rangen= new TRandom3(0);

	for (double true_mod = 1.0; true_mod< 30; true_mod+=2){
		SBNspec true_spec = delta_spec;	
		true_spec.Scale("signal", true_mod); // this is now the "prediction" 

		//SBNchi test_chi(true_spec, Mempty);
		//test_chi.setStatOnly(true);


			//now generate some random data of the test_spec..

			TH1D tmp("","",100,0,100);
			tmp.GetXaxis()->SetCanExtend(kTRUE);

			for(int i =0; i< numMC; i++){
				SBNspec test_spec = delta_spec;
				SBNspec mcdata_spec = true_spec;
				mcdata_spec.poissonScale(rangen);
					

				//Set up a SBNfit with background and signal, and number of paramaters
				SBNfit myfit(mcdata_spec, test_spec, Mempty, 2);
				myfit.setStatOnly(true);

				//SBNfit contains a simple routine for scaling any number of historgrams by a parameter, and minimizing over them
				//so make a vector of pairs, saying which hists to scale with which parameter
				std::vector<std::pair<std::string, int>> myin;
				myin.push_back(std::make_pair("signal",0) );
				myin.push_back(std::make_pair("bkg",1) );

				myfit.initialize_norm(myin);

				//We initilise parametrs and give them names
				std::vector<double> init  {true_mod*0.8, 1.0};
				std::vector<double> low  {0.5,0};
				std::vector<double> up  {40,1};
				std::vector<double> step  {0.01,0.01};
				std::vector<int> isfixed  {0,1};
				std::vector<std::string> nam  {"mod","fixed"};

				myfit.setInitialValues(init);
				myfit.setNames(nam);
				myfit.setLowerValues(low);
				myfit.setUpperValues(up);
				myfit.setStepSizes(step);
				myfit.setFixed(isfixed);

				//and minimize, obviously as we are comparing it to itself, we hope that it find that no scaling is best!

				myfit.Minimize();
				std::cout<<"Minimized! chi^2: "<<myfit.bf_chi<<" mod: "<<myfit.bf_params[0]<<" p2: "<<" #:"<<myfit.num_func_calls<<std::endl;

				tmp.Fill(myfit.bf_params[0]);

			}
			
			//TH1D pdf = test_chi.toyMC_varyCore(&test_spec , numMC);
		
			tmp.SetName( (std::to_string(true_mod)+"_true").c_str());
			tmp.Write();
			
	
			double chi =0;
			std::cout<<"Enhance on: "<<true_mod<<" chi^2: "<<chi<<std::endl;
			//ven.push_back(true_mod); vchi.push_back(test_mod);
	}

	TGraph *g = new TGraph(ven.size(), &ven[0], &vchi[0]);
	TCanvas *c = new TCanvas();



	g->Draw("alp");
	g->SetTitle("Stats only ");
	g->GetXaxis()->SetTitle("#sigma enhancement over SM");
	g->GetYaxis()->SetTitle("#chi^{2}");


	c->Write();	
	f->Close();

	return 0;
	/*
	   SBNchi test_chi(bkg_spec, Mempty);
	   test_chi.setStatOnly(true);

	//Now create a oscillation spectra, constructed the same.
	SBNosc oscSig("../../data/precomp/SBN_BKG",xml);

	//Say we want to just look at apperance mode (as its easiest to plot 2d!)
	oscSig.Scale("uBooNE",uboonepot);		
	oscSig.setBothMode();

	//so varying over all Dela M and sin^2 2 theta
	double um4min = -4.0;
	double um4max = 0;
	double um4step =0.05;

	//Want to contour plot sensitivity eventually so som standard root
	TCanvas *c1 = new TCanvas("c1","c1",600,400);
	TH2F *hcontz = new TH2F("hcontz","MicroBooNE 3+1 90\% C.L ",(um4max-um4min)/um4step,um4min,um4max, 100,-2,2);
	hcontz->GetXaxis()->SetTitle("U_{#mu 4}^2");
	hcontz->GetYaxis()->SetTitle("#Delta m^{2}_{41} (eV^{2})");

	for(double m = -2.00; m <2.00; m=m+0.04){
	for(double um4 = um4max ; um4 >= um4min; um4 = um4 - um4step){

	//always work in proper UPMNS elements!!
	double uei = 0.0;
	double umi = pow(10,um4);

	//This is where you can set up 3+N
	double imn[3] = {sqrt(pow(10,m)),0,0};
	double iue[3] = {uei,0,0};
	double ium[3] = {umi,0,0};
	double iph[3] = {0,0,0};

	//construct a signalModel
	neutrinoModel signalModel(imn,iue,ium,iph);
	signalModel.numsterile = 1; //this isnt really necessary as it can tell from imn, but nice for reading

	//And load thus model into our spectra. At this point its comuted all the necessary mass-splittins and which frequencies they are
	oscSig.load_model(signalModel);

	//And apply this oscillaion! Adding to it the bkgSpec that it was initilised with.
	std::vector<double> ans = oscSig.Oscillate();


	//std::cout<<"HERE: "<<ans.size()<<" oscSig.num_bins_total_compressed: "<<oscSig.num_bins_total_compressed<<std::endl;

	//Then calculate a chu^2
	double tchi=test_chi.CalcChi(ans); 

	std::cout<<"Dm^2: "<<m<<" um4^2: "<<2*um4<<" chi^2: "<<tchi<<std::endl;
	//and save wherever you like , this si just a quick hodge podge example
	int bb= 1+floor((-um4min-um4)/um4step+0.00001); 
	bb = hcontz->GetXaxis()->FindBin(um4);
	//std::cout<<"Bin: "<<bb<<"/"<<hcontz->GetNbinsX()<<std::endl; 
	hcontz->SetBinContent(bb, 1+floor(-(-2.00-m)/0.04+0.00001), tchi);


	}
	}
	Double_t contours[1];
	contours[0] = 1.64;
	hcontz->SetContour(1, contours);

	c1->cd();

	hcontz->Draw("CONT3");
	TFile * ff = new TFile("example_3.root","RECREATE");
	ff->cd();

	c1->Write();
	ff->Close();

	return 0;

	*/
}
