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
#include "TLine.h"
#include "TGraph.h"
#include "TText.h"

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
	int pot=20;
	/*************************************************************
	 *************************************************************
	 *		Command Line Argument Reading
	 ************************************************************
	 ************************************************************/

	const struct option longopts[] = 
	{
		{"xml", 		required_argument, 	0, 'x'},
		{"pot",			required_argument,	0, 'p'},
		{0,			no_argument, 		0,  0},
	};


	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "x:p:", longopts, &index);

		switch(iarg)
		{
			case 'x':
				xml = optarg;
				break;
			case 'p':
				pot = strtof(optarg,NULL);
				break;
			case '?':
			case 'h':
				std::cout<<"Allowed arguments:"<<std::endl;
				std::cout<<"\t-x\t--xml\t\tInput .xml file for SBNconfig"<<std::endl;
				return 0;
		}

	}
	double POTscale = 1.0;
	if(pot == 19){
		POTscale = 5e19/6.6e20;	
	}

	if(false){
	SBNspec delta_spec("../../delta_BDT_new", xml);
	delta_spec.calcFullVector();
	delta_spec.ScaleAll(POTscale);
	delta_spec.calcFullVector();

	SBNspec bkg_spec = delta_spec;
	bkg_spec.calcFullVector();
	bkg_spec.Scale("signal",0.0);
	bkg_spec.calcFullVector();


	TMatrixD Mempty(delta_spec.fullVec.size(), delta_spec.fullVec.size());
	SBNchi test_chi(bkg_spec, Mempty);
		
	//again create a SBNchi from this spectra

	std::cout<<"6 DOF: chi^2 "<<test_chi.CalcChi(delta_spec)<<std::endl;

	return 0;
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
	//SBNspec delta_spec("../../delta_BDT_onebin", xml);


	SBNspec delta_spec("../../delta_BDT_comb", xml);
	delta_spec.calcFullVector();
	delta_spec.Scale("signal",POTscale/3.1);
	delta_spec.calcFullVector();

	SBNspec bkg_spec = delta_spec;
	bkg_spec.calcFullVector();
	bkg_spec.Scale("signal",0.0);
	bkg_spec.calcFullVector();


	//again create a SBNchi from this spectra
	TMatrixD Mempty(delta_spec.fullVec.size(), delta_spec.fullVec.size());



	std::vector<double> ven={0};
	
	std::vector<std::vector<double>> vec_CLs;
	std::vector<std::vector<double>> vec_CLmine;
	for(int i=0; i<5;i++){
		vec_CLs.push_back(ven);
		vec_CLmine.push_back(ven);
	}
	

	int numMC = 60000;

	TRandom3 * rangen= new TRandom3(0);


	bool printed = false;

	double modmax=14;
	if(pot==19){
		modmax=62;
	}



	for (double true_mod = 0.0; true_mod< modmax; true_mod+=0.50){

		SBNspec sig_spec = delta_spec;	
		sig_spec.Scale("signal", true_mod); // this is now the "prediction" 

		double stot = sig_spec.getTotalEvents();
		double btot = bkg_spec.getTotalEvents();
	

		SBNchi test_chi(sig_spec, Mempty);
		//test_chi.setStatOnly(true);

		
		//step one, find median bkg only
		TH1D bkg_pdf = test_chi.toyMC_varyInput(&bkg_spec, numMC);
		std::vector<double> q = {0.1, 1-0.68, 0.5, 0.68, 0.9};
		std::vector<double> x(q.size());	
		bkg_pdf.ComputeIntegral(); // just a precaution
		bkg_pdf.GetQuantiles(q.size(), &x[0], &q[0]);
		std::cout<<true_mod<<" BKG Quantile: "<<x[0]<<" "<<x[1]<<" "<<x[2]<<std::endl;

		//Gotten medians ..etc..

		//std::vector<double> xrev = x;
		//std::reverse(std::begin(xrev), std::end(xrev));

		std::vector<double> pval = test_chi.toyMC_varyInput_getpval(&sig_spec, numMC, x);
		
		//lets do CLs
		for(int p=0; p<pval.size();p++){
			vec_CLmine.at(p).push_back(1-pval.at(p));
			vec_CLs.at(p).push_back(pval.at(p)/(1-q.at(p)) );
		}

		if(true_mod > 1){
		std::cout<<"NUMBER @ scale : "<<true_mod<<"\t"<< pval.at(0)/(1-q.at(0))<<"\tsig: "<<stot<<"\tbkg: "<<btot<<"\tsig_only "<<stot-btot<<"\t\ts/sqrt(s+b): "<<(stot-btot)/sqrt(stot)<<std::endl;
		}
		ven.push_back(true_mod);

	
		if(!printed &&  pval.at(0)/(1-q.at(0)) < 0.9){
			sig_spec.writeOut("printed.root");	
			printed = true;
			TFile * fp = new TFile("printed2.root","recreate");
			fp->cd();
			TCanvas *cp=new TCanvas();
			bkg_pdf.SetLineColor(kBlue-6);
			
			TH1D sig_pdf=test_chi.toyMC_varyInput(&sig_spec,numMC);
			sig_pdf.SetLineColor(kRed-6);
			sig_pdf.Draw();
		
			bkg_pdf.Draw("same");	
			cp->Write();	
			fp->Close();
			
			std::cout<<"PRINTED: Chi2: \t\t"<<x.at(0)<<"\t\t"<<x.at(1)<<"\t\t"<<x.at(2)<<"\t\t"<<x.at(3)<<"\t\t"<<x.at(4)<<std::endl;
			std::cout<<"PRINTED: Power: \t\t"<<q.at(0)<<"\t\t"<<q.at(1)<<"\t\t"<<q.at(2)<<"\t\t"<<q.at(3)<<"\t\t"<<q.at(4)<<std::endl;
			std::cout<<"PRINTED: Pval: \t\t"<<pval.at(0)<<"\t\t"<<pval.at(1)<<"\t\t"<<pval.at(2)<<"\t\t"<<pval.at(3)<<"\t\t"<<pval.at(4)<<std::endl;
		}
	

	}
	ven.push_back(modmax);
	for(int i=0; i< vec_CLs.size(); i++){
		vec_CLs.at(i).push_back(0);
		vec_CLmine.at(i).push_back(0);
	}

	TFile *f = new TFile("bound_90_root.root","recreate");
	
	//*************************************************
	//CLs

	std::vector<TGraph*> v_g_CLs;
	for(auto CL: vec_CLs){
		v_g_CLs.push_back( new TGraph(ven.size(), &ven[0], &CL[0]));
	}
	
	TCanvas *c0 = new TCanvas("CLs");
	TPad * p = (TPad*)c0->cd();
	p->SetLogy();
	
	int med = 2;
	
	v_g_CLs.at(0)->Draw("afp");
	v_g_CLs.at(0)->Draw("lp");
	v_g_CLs.at(0)->SetFillColor(kYellow-7);
	v_g_CLs.at(0)->SetTitle("CLs Exclusion Limit");


	v_g_CLs.at(med)->SetLineWidth(3);
	v_g_CLs.at(med)->SetLineStyle(2);

	v_g_CLs.at(1)->SetFillColor(kGreen+1);
	v_g_CLs.at(2)->SetFillColor(kGreen+1);


	v_g_CLs.at(3)->SetFillColor(kYellow-7);
	v_g_CLs.at(4)->SetFillColor(kWhite);

	for(int i=0; i<v_g_CLs.size(); i++){
	//for(int i=v_g_CLs.size()-1; i>=0; i--){
		v_g_CLs.at(i)->SetLineWidth(2);
		v_g_CLs.at(i)->Draw("fp same");
		v_g_CLs.at(i)->Draw("lp same");
	}



	TLine * l3 = new TLine(3.0, 0.001, 3.0,1);  l3->SetLineStyle(1);l3->SetLineColor(kBlack); 	
	l3->SetLineWidth(2);

	TText *t3 = new TText(2.6, 0.008,"LEE (x3)"); t3->SetTextColor(kBlack);//t90->SetTextSize(0.12);
	t3->SetTextAngle(-90);
	t3->Draw();
	l3->Draw();
	

	TLine * l90 = new TLine(1, 0.1, modmax-1,  0.1); l90->SetLineStyle(1);l90->SetLineColor(kRed);
	TLine * l95 = new TLine(1, 0.05, modmax-1, 0.05);l95->SetLineStyle(2);l95->SetLineColor(kRed);
	TLine * l99 = new TLine(1, 0.01, modmax-1, 0.01);l99->SetLineStyle(3);l99->SetLineColor(kRed);

	TText *t90 = new TText(modmax-3, 0.1*0.9,"90\% C.L"); t90->SetTextAlign(13); t90->SetTextColor(kRed);//t90->SetTextSize(0.12);
	TText *t95 = new TText(modmax-3, 0.05*0.9,"95\% C.L"); t95->SetTextAlign(13); t95->SetTextColor(kRed);     // t95->SetTextSize(0.12);
	TText *t99 = new TText(modmax-3, 0.01*0.9,"99\% C.L"); t99->SetTextAlign(13); t99->SetTextColor(kRed);    //t99->SetTextSize(0.12);

	t90->Draw();
	t99->Draw();
	t95->Draw();
	l90->Draw();
	l99->Draw();
	l95->Draw();

	v_g_CLs.front()->GetXaxis()->SetRangeUser(1,modmax-1);
	v_g_CLs.front()->GetXaxis()->SetTitle("#sigma_{1 #gamma} enhancement");
	v_g_CLs.front()->GetYaxis()->SetTitle("CLs");

	p->RedrawAxis();

	TLegend * l2 = new TLegend(0.6,0.7,0.89,0.89);
	l2->SetLineColor(kWhite);	


	l2->AddEntry(v_g_CLs.at(0),"95\% Experiments" , "f");
	l2->AddEntry(v_g_CLs.at(2), "Median Experiment", "l");
	l2->AddEntry(v_g_CLs.at(1), "68\% Experiments", "f");
	l2->Draw();


	c0->Write();


	//******************************************************

	//Normal


	std::vector<TGraph*> v_g_CLmine;
	for(auto CL: vec_CLmine){
		v_g_CLmine.push_back( new TGraph(ven.size(), &ven[0], &CL[0]));
	}
	
	TCanvas *c1 = new TCanvas("CL");
	TPad * p1 = (TPad*)c1->cd();
	//p1->SetLogy();
	
	
	v_g_CLmine.at(med)->Draw("afp");
	v_g_CLmine.at(med)->Draw("lp same");
	v_g_CLmine.at(med)->SetFillColor(kGreen+1);
	v_g_CLmine.at(med)->SetTitle("CL Exclusion Limit");

	v_g_CLmine.at(med)->SetLineWidth(3);
	v_g_CLmine.at(med)->SetLineStyle(2);
	
	v_g_CLmine.at(3)->SetFillColor(kGreen+1);
	v_g_CLmine.at(4)->SetFillColor(kYellow-7);

	v_g_CLmine.at(1)->SetFillColor(kYellow-7);
	v_g_CLmine.at(0)->SetFillColor(kWhite);

	for(int i=v_g_CLmine.size()-1; i>=0; i--){
		v_g_CLmine.at(i)->SetLineWidth(2);
		v_g_CLmine.at(i)->Draw("fp same");
		v_g_CLmine.at(i)->Draw("lp same");
	}



	TLine * lb90 = new TLine(1, 1-0.1, modmax-4,  1-0.1); lb90->SetLineStyle(1);lb90->SetLineColor(kRed);
	TLine * lb95 = new TLine(1, 1-0.05, modmax-4, 1-0.05);lb95->SetLineStyle(2);lb95->SetLineColor(kRed);
	TLine * lb99 = new TLine(1, 1-0.01, modmax-4, 1-0.01);lb99->SetLineStyle(3);lb99->SetLineColor(kRed);

	TText *tb90 = new TText(modmax-8, 1-0.1,"90\% C.L"); tb90->SetTextAlign(13); tb90->SetTextColor(kRed);//tb90->SetTextSize(0.12);
	TText *tb95 = new TText(modmax-8, 1-0.05,"95\% C.L"); tb95->SetTextAlign(13); tb95->SetTextColor(kRed);     // tb95->SetTextSize(0.12);
	TText *tb99 = new TText(modmax-8, 1-0.01,"99\% C.L"); tb99->SetTextAlign(13); tb99->SetTextColor(kRed);    //tb99->SetTextSize(0.12);

	tb90->Draw();
	tb99->Draw();
	tb95->Draw();
	lb90->Draw();
	lb99->Draw();
	lb95->Draw();

	v_g_CLmine.at(med)->GetXaxis()->SetRangeUser(1,modmax-3);
	v_g_CLmine.at(med)->GetYaxis()->SetRangeUser(0.8,1.11);
	v_g_CLmine.at(med)->GetXaxis()->SetTitle("#sigma_{1 #gamma} enhancement");
	v_g_CLmine.at(med)->GetYaxis()->SetTitle("CL Exclusion");

	p1->RedrawAxis();

	TLegend * l1 = new TLegend(0.15,0.7,0.15+0.29,0.89);
	l1->SetLineColor(kWhite);	

	
	TGraph * tmp = (TGraph*)v_g_CLmine.at(med)->Clone("tmp");
	tmp->SetLineStyle(1);

	l1->AddEntry(v_g_CLmine.at(1), "90\% Experiments", "f");
	l1->AddEntry(v_g_CLmine.at(med), "Median Experiment", "l"); //color fill has to be like this
	l1->AddEntry(tmp,"68\% Experiments" , "f");
	l1->Draw();

	c1->Write();




/*	
	TGraph *g = new TGraph(ven.size(), &ven[0], &vchi[0]);
	TGraph *g_high1 = new TGraph(ven.size(), &ven[0], &vchi_high1[0]);
	TGraph *g_high2 = new TGraph(ven.size(), &ven[0], &vchi_high2[0]);
	TGraph *g_low1 = new TGraph(ven.size(), &ven[0], &vchi_low1[0]);
	TGraph *g_low2 = new TGraph(ven.size(), &ven[0], &vchi_low2[0]);

	TCanvas *c = new TCanvas();
	std::vector<TGraph*> gvec;
	gvec.push_back(g_high2);
	gvec.push_back(g_high1);
	gvec.push_back(g_low1);
	gvec.push_back(g_low2);

	TLegend * l = new TLegend(0.5,0.11,0.89,0.5);
	l->SetLineColor(kWhite);	


	g_high2->SetLineColor(kBlack);
	g_high2->SetLineWidth(2);
	
	g->SetLineColor(kBlack);
	g->SetLineWidth(3);
	g->SetLineStyle(2);
	g->SetFillColor(kGreen+1);
	
	l->AddEntry(g_high2,"95\% Experiments" , "f");
	l->AddEntry(g, "Median Experiment", "l");
	l->AddEntry(g_high1, "68\% Experiments", "f");

	g_high1->SetFillColor(kGreen+1);
	g_high2->SetFillColor(kYellow-7);
	g_low2->SetFillColor(kYellow-7);
	g_low1->SetFillColor(kWhite);


	for(int i=0; i< gvec.size(); i++){
		gvec.at(i)->SetLineColor(kBlack);
		gvec.at(i)->SetLineWidth(2);
	}
	
	g_high2->Draw("alfp");
	g_high2->Draw("lp same");
	g_high2->SetTitle("Stats only");
	g_high2->GetXaxis()->SetTitle("#sigma enhancement over SM");
	g_high2->GetYaxis()->SetTitle("#chi^{2}");
	g_high2->GetXaxis()->SetRangeUser(1,18);

	g_high1->Draw("flp same");
	g_high1->Draw("lp same");
	g->Draw("fpl same");
	g->Draw("pl same");
	g_low2->Draw("flp same");
	g_low2->Draw("lp same");
	g_low1->Draw("flp same");
	g_low1->Draw("lp same");

	l->Draw();

	c->Write();	


*/
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
