
#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/
#include "TruncMean.h" 

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TFriendElement.h"
#include "TGraph.h"
#include "TF1.h"



double bragg_fnc(double *x, double *par) {
	return par[0]*pow(x[0],par[1]);
}   

double bragg_fnc_fixD(double *x, double *par) {
	return par[0]*pow(x[0],-0.4);
}   


int trk_dEdx(std::string fnam, std::string namout){

	TFile * fin = new TFile(fnam.c_str(),"update");
	TTree * vertex_tree = (TTree*)fin->Get("LEEPhotonAnalysisData/vertex_tree");

	//Some branches for some basic info
	//	double asso_r
	int reco_asso_tracks = 0;
	int reco_asso_showers = 0;
	double longest_asso_track_displacement = 0;	

	vertex_tree->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
	vertex_tree->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
	vertex_tree->SetBranchAddress("longest_asso_track_displacement", &longest_asso_track_displacement);


	//Branches for the dEdx calo only!
	std::vector<double> *vtrk_dEdx = 0;
	std::vector<double> *vtrk_resrange = 0;

	TBranch *bdEdx = 0;
	TBranch *bresrange = 0;

	vertex_tree->SetBranchAddress("longest_asso_track_calo_dEdx",&vtrk_dEdx,&bdEdx);
	vertex_tree->SetBranchAddress("longest_asso_track_calo_resrange",&vtrk_resrange,&bresrange);


	// histograms of things I'm adding	
	TH1D *h_mean_dEdx = new TH1D("h_mean_dEdx","h_mean_dEdx",100,0,10);
	TH1D *h_mean_dEdx_start = new TH1D("h_mean_dEdx_start","h_mean_dEdx_start",100,0,20);
	TH1D *h_mean_dEdx_end = new TH1D("h_mean_dEdx_end","h_mean_dEdx_end",100,0,20);
	TH1D *h_mean_dEdx_ratio = new TH1D("h_mean_dEdx_ratio","h_mean_dEdx_ratio",100,0,5);

	TH1D *h_bragg_start_trunc_chi2 = new TH1D("h_bragg_start_trunc_chi2","h_brag_start_chi2",100,0,100);
	TH1D *h_bragg_start_parD = new TH1D("h_bragg_start_parD","h_brag_start_parD",100,-1.5,1);
	TH1D *h_bragg_start_fixD = new TH1D("h_bragg_start_fixD","h_brag_start_fixD",100,0,30);
	TH1D *h_bragg_start_parA = new TH1D("h_bragg_start_parA","h_brag_start_parA",100,0,30);
	TH1D *h_bragg_start_fixA = new TH1D("h_bragg_start_fixA","h_brag_start_fixA",100,-1.5,1);

	TH1D *h_bragg_start_trunc_parD = new TH1D("h_bragg_start_trunc_parD","h_brag_start_trunc_parD",100,-1.5,1);
	TH1D *h_bragg_start_trunc_parA = new TH1D("h_bragg_start_trunc_parA","h_brag_start_trunc_parA",100,0,30);
	
	TH1D *h_unit_dEdx = new TH1D("h_unit_dEdx","h_unit_dEdx",100,0,20);
	TH1D *h_range = new TH1D("h_range","h_range",100,0,500);

	//New Branches
	TFile *ftreeout = new TFile(namout.c_str(),"recreate");
	TTree * new_tree = new TTree("track_dEdx_tree","track_dEdx_tree");


	double v_mean_dEdx=0;
	double v_mean_dEdx_start=0;
	double v_mean_dEdx_end=0;
	double v_mean_dEdx_ratio=0;
	double v_bragg_start_parD=0;
	double v_bragg_start_parA=0;
	double v_bragg_start_trunc_parD=0;
	double v_bragg_start_trunc_parA=0;
	double v_bragg_start_trunc_chi2=0;

	double v_bragg_start_fixD=0;
	double v_bragg_start_fixA=0;
	double v_unit_dEdx = 0;
	double v_range=0;

	int v_reco_asso_tracks = 0;

	TBranch *b_reco_asso_tracks = new_tree->Branch("reco_asso_tracks",&v_reco_asso_tracks,"reco_asso_tracks/I");

	TBranch *b_mean_dEdx = new_tree->Branch("longest_asso_track_mean_dEdx",&v_mean_dEdx,"longest_asso_track_mean_dEdx/D");
	TBranch *b_mean_dEdx_start = new_tree->Branch("longest_asso_track_mean_dEdx_start",&v_mean_dEdx_start,"longest_asso_track_mean_dEdx_start/D");
	TBranch *b_mean_dEdx_end = new_tree->Branch("longest_asso_track_mean_dEdx_end",&v_mean_dEdx_end,"longest_asso_track_mean_dEdx_end/D");
	TBranch *b_mean_dEdx_ratio = new_tree->Branch("longest_asso_track_mean_dEdx_ratio",&v_mean_dEdx_ratio,"longest_asso_track_mean_dEdx_ratio/D");
	TBranch *b_bragg_start_parD = new_tree->Branch("longest_asso_track_bragg_start_parD",&v_bragg_start_parD,"longest_asso_track_bragg_start_parD/D");
	TBranch *b_bragg_start_trunc_chi2 = new_tree->Branch("longest_asso_track_bragg_start_trunc_chi2",&v_bragg_start_trunc_chi2,"longest_asso_track_bragg_start_trunc_chi2/D");
	TBranch *b_bragg_start_parA = new_tree->Branch("longest_asso_track_bragg_start_parA",&v_bragg_start_parA,"longest_asso_track_bragg_start_parA/D");
	TBranch *b_bragg_start_trunc_parD = new_tree->Branch("longest_asso_track_bragg_start_trunc_parD",&v_bragg_start_trunc_parD,"longest_asso_track_bragg_start_trunc_parD/D");
	TBranch *b_bragg_start_trunc_parA = new_tree->Branch("longest_asso_track_bragg_start_trunc_parA",&v_bragg_start_trunc_parA,"longest_asso_track_bragg_start_trunc_parA/D");

	TBranch *b_bragg_start_fixD = new_tree->Branch("longest_asso_track_bragg_start_fixD",&v_bragg_start_fixD,"longest_asso_track_bragg_start_fixD/D");
	TBranch *b_bragg_start_fixA = new_tree->Branch("longest_asso_track_bragg_start_fixA",&v_bragg_start_fixA,"longest_asso_track_bragg_start_fixA/D");
	
	TBranch *b_unit_dEdx = new_tree->Branch("longest_asso_track_unit_dEdx",&v_unit_dEdx,"longest_asso_track_unit_dEdx/D");
	TBranch *b_range = new_tree->Branch("longest_asso_track_range",&v_range,"longest_asso_track_range/D");

	TH2D * h2_dEdx_resrange = new TH2D("h2_dEdx_resrange","h2_dEdx_resrange",100,0,150,100,0,50);

	double max_realistic_dEdx = 1e4;
	double start_max = 50;//22.5; //in cm
	double end_max = 50;//22.5; //in cm

	std::vector<double> params = {20.0, -0.4};
	TF1 *bragg = new TF1("bragg",bragg_fnc,0.5,start_max, params.size());
	bragg->SetParameters(&params[0]);
	bragg->SetParNames("bragg_A","bragg_b");
	bragg->SetParLimits(0, 0, 30.0);
	bragg->SetParLimits(1, -1.5, 1);

	std::vector<double> params_fixD = {20.0};
	TF1 *bragg_fixD = new TF1("braggfixd",bragg_fnc_fixD,0.5,start_max, params_fixD.size());
	bragg_fixD->SetParameters(&params_fixD[0]);
	bragg_fixD->SetParNames("bragg_A");
	bragg_fixD->SetParLimits(0, 0, 30.0);


	//bragg->Draw();
	//return;


	for(int i=0; i< vertex_tree->GetEntries(); i++){
		//for(int i=0; i< 10; i++){
		vertex_tree->GetEntry(i);

		v_mean_dEdx=-999;
		v_mean_dEdx_start=-999;
		v_mean_dEdx_end=-999;
		v_mean_dEdx_ratio=-999;
		v_bragg_start_parD=-1.7;
		v_bragg_start_parA=-1;
		
		v_bragg_start_fixA = -1.7;
		v_bragg_start_fixD = -1;

		v_unit_dEdx = -999;
		v_range = -999;
		v_reco_asso_tracks = reco_asso_tracks;

		if(reco_asso_tracks >0){
			if (i%50000==0)std::cout<<i<<" "<<vtrk_dEdx->size()<<" "<<vtrk_resrange->size()<<" "<<reco_asso_tracks<<" "<<reco_asso_showers<<std::endl;

			double sum_dEdx =0;
			double sum_dEdx_start = 0;
			double sum_dEdx_end = 0;
			double num_start =0;
			double num_end =0;

			std::vector<float> start_dEdx;
			std::vector<float> start_resrange;

			std::vector<float> end_dEdx;
			std::vector<float> end_resrange;

			if(vtrk_dEdx->size() == 0 || std::min(vtrk_resrange->front(), vtrk_resrange->back()) > start_max){
				v_range = longest_asso_track_displacement;
				new_tree->Fill();
				continue;
			}

		//	TruncMean tm;
		//	std::vector<float> trun_dEdx;
		//	const std::vector<float> c_start_resrange = vtrk_resrange;
		//	const std::vector<float> c_start_dEdx = vtrk_dEdx;
		//	tm.CalcTruncMeanProfile(c_start_resrange,c_start_dEdx, out,nsigma);



			for(int j=0; j<vtrk_dEdx->size(); j++){
				h2_dEdx_resrange->Fill(vtrk_resrange->at(j), vtrk_dEdx->at(j));

				//std::cout<<"All dEdx: "<<vtrk_resrange->at(j)<<" "<<vtrk_dEdx->at(j)<<std::endl;	

				if(vtrk_dEdx->at(j) > max_realistic_dEdx){
					//	std::cout<<"LARGE dEdx: "<<vtrk_resrange->at(j)<<" "<<vtrk_dEdx->at(j)<<std::endl;	
				}
				if(vtrk_dEdx->at(j) < 0){
						std::cout<<"Negative dEdx: "<<vtrk_resrange->at(j)<<" "<<vtrk_dEdx->at(j)<<std::endl;	
				}



				sum_dEdx += vtrk_dEdx->at(j);
				

				if(vtrk_resrange->at(j) <= start_max){
					//std::cout<<"Start dEdx: "<<vtrk_resrange->at(j)<<" "<<vtrk_dEdx->at(j)<<std::endl;	
						sum_dEdx_start += vtrk_dEdx->at(j);
						num_start+=1.0;

						start_dEdx.push_back( vtrk_dEdx->at(j));
						start_resrange.push_back( vtrk_resrange->at(j));
				}				


				if(vtrk_resrange->at(j) >= vtrk_resrange->back()-end_max){
						sum_dEdx_end += vtrk_dEdx->at(j);
						num_end+=1.0;

						end_dEdx.push_back( vtrk_dEdx->at(j));
						end_resrange.push_back( vtrk_resrange->at(j));

				}	

			}

			if(start_dEdx.size() ==0){
				std::cout<<"ERROR: empty start run"<<std::endl;
				for(int j=0; j<vtrk_dEdx->size(); j++){
					std::cout<<j<<" "<<vtrk_resrange->at(j)<<" "<<vtrk_dEdx->at(j)<<std::endl;
				}
				std::cout<<"ERROR: empty start run. Skipping"<<std::endl;
				exit(EXIT_FAILURE);	
				continue;
			}

			TGraph * start_pts = new TGraph(start_dEdx.size(), &start_resrange[0],&start_dEdx[0] );
			TGraph * end_pts = new TGraph(end_dEdx.size(), &end_resrange[0],&end_dEdx[0] );

			TFitResultPtr fit_start_bragg = start_pts->Fit(bragg,"SQ");	
			h_bragg_start_parD->Fill(fit_start_bragg->Value(1));
			h_bragg_start_parA->Fill(fit_start_bragg->Value(0));
			v_bragg_start_parD = fit_start_bragg->Value(1);		
			v_bragg_start_parA = fit_start_bragg->Value(0);		
			//Truncated mean bit
			//void TruncMean::CalcTruncMeanProfile(const std::vector<float>& rr_v, const std::vector<float>& dq_v,  std::vector<float>& dq_trunc_v, const float& nsigma);
			TruncMean tm;
			std::vector<float> out;
			const std::vector<float> c_start_resrange = start_resrange;
			const std::vector<float> c_start_dEdx = start_dEdx;
			const float nsigma = 1;
			tm.CalcTruncMeanProfile(c_start_resrange,c_start_dEdx, out,nsigma);

			TGraph * trunc_start_pts = new TGraph(start_dEdx.size(), &start_resrange[0],&out[0] );
			TFitResultPtr fit_start_trunc_bragg = trunc_start_pts->Fit(bragg,"SQ");	
			h_bragg_start_trunc_parD->Fill(fit_start_trunc_bragg->Value(1));
			h_bragg_start_trunc_parA->Fill(fit_start_trunc_bragg->Value(0));
			h_bragg_start_trunc_chi2->Fill(fit_start_trunc_bragg->Chi2());
			v_bragg_start_trunc_parD = fit_start_trunc_bragg->Value(1);		
			v_bragg_start_trunc_parA = fit_start_trunc_bragg->Value(0);		
			v_bragg_start_trunc_chi2 = fit_start_trunc_bragg->Chi2();


			TFitResultPtr fit_start_bragg_fixD = start_pts->Fit(bragg_fixD,"SQ");	
			h_bragg_start_fixD->Fill(fit_start_bragg_fixD->Value(0));
			v_bragg_start_fixD = fit_start_bragg_fixD->Value(0);		
			/*
			bragg->ReleaseParameter(0);
			bragg->FixParameter(1,-0.4);
			TFitResultPtr fit_start_bragg_fixD = start_pts->Fit(bragg,"SQ");	
			h_bragg_start_fixD->Fill(fit_start_bragg_fixD->Value(0));
			v_bragg_start_fixD = fit_start_bragg_fixD->Value(0);		
			*/


			//Fill here some "fixed" bragg chi^2's


			h_mean_dEdx->Fill( sum_dEdx/(double)vtrk_resrange->size()  );
			h_mean_dEdx_start->Fill( sum_dEdx_start/num_start  );
			h_mean_dEdx_end->Fill( sum_dEdx_end/num_end  );
			h_mean_dEdx_ratio->Fill( (sum_dEdx_start/num_start)/(sum_dEdx_end/num_end)  );
			v_mean_dEdx =  sum_dEdx/(double)vtrk_resrange->size();
			v_mean_dEdx_start =  sum_dEdx_start/num_start ;
			v_mean_dEdx_end =  sum_dEdx_end/num_end ;
			v_mean_dEdx_ratio = (sum_dEdx_start/num_start)/(sum_dEdx_end/num_end) ;


			v_range = vtrk_resrange->front();		

			h_unit_dEdx->Fill( sum_dEdx/vtrk_resrange->front()  );
			v_unit_dEdx =  sum_dEdx/vtrk_resrange->front();

			if(v_range < 0){v_range = longest_asso_track_displacement;}

		}
		if(v_unit_dEdx!=v_unit_dEdx || v_unit_dEdx > max_realistic_dEdx ) v_unit_dEdx = -999;
		if(v_mean_dEdx!=v_mean_dEdx || v_mean_dEdx > max_realistic_dEdx ) v_mean_dEdx = -999;
		if(v_mean_dEdx_start!=v_mean_dEdx_start || v_mean_dEdx_start > max_realistic_dEdx  ) v_mean_dEdx_start = -999;
		if(v_mean_dEdx_end!=v_mean_dEdx_end || v_mean_dEdx > max_realistic_dEdx ) v_mean_dEdx_end = -999;
		if(v_mean_dEdx_ratio!=v_mean_dEdx_ratio || v_mean_dEdx_ratio > max_realistic_dEdx ) v_mean_dEdx_ratio = -999;
		if(v_bragg_start_parA!=v_bragg_start_parA || v_bragg_start_parA > max_realistic_dEdx ) v_bragg_start_parA = -1;
		if(v_bragg_start_parD!=v_bragg_start_parD || v_bragg_start_parD > max_realistic_dEdx ) v_bragg_start_parD = -1.7;



		//		std::cout<<v_mean_dEdx<<" "<<v_mean_dEdx_start<<" "<<v_mean_dEdx_end<<" "<<v_mean_dEdx_ratio<<" "<<v_unit_dEdx<<" "<<v_bragg_start_parD<<" "<<v_bragg_start_parA<<std::endl;
		//Fill branches
		/*b_bragg_start_parD->Fill();
		  b_bragg_start_parA->Fill();
		  b_mean_dEdx->Fill();
		  b_mean_dEdx_start->Fill();
		  b_mean_dEdx_end->Fill();
		  b_mean_dEdx_ratio->Fill();
		  b_unit_dEdx->Fill();
		 */
		new_tree->Fill();


	}
	ftreeout->cd();
	new_tree->Write();

	TFile *fout = new TFile("plots.root","recreate");
	fout->cd();

	h2_dEdx_resrange->Write();
	h_mean_dEdx->Write();
	h_mean_dEdx_end->Write();
	h_mean_dEdx_start->Write();
	h_mean_dEdx_ratio->Write();
	h_unit_dEdx->Write();

	h_range->Write();

	h_bragg_start_parD->Write();
	h_bragg_start_parA->Write();
	fout->Close();		



	TCanvas *ch2_dEdx_resrange = new TCanvas();
	ch2_dEdx_resrange->cd();

	h2_dEdx_resrange->Draw("colz");
	ch2_dEdx_resrange->SaveAs("2d.pdf","pdf");

	fin->Close();

	//	vertex_tree->Show();
	}




int main(){
			trk_dEdx("../samples/mcc86/merged.bnbcosmic_v3.0_mcc86_withcalo.root","trackdEdx_bnb_cosmics.root");
			trk_dEdx("../samples/mcc86/merged.ncsignal_cosmics_v2.0.root","trackdEdx_nsignal_cosmics.root");
			trk_dEdx("../samples/mcc86/merged.ncsignal_v2.0.root","trackdEdx_nsignal.root");

			trk_dEdx("../samples/mcc86/merged.bnb_v1.0.root","trackdEdx_bnb.root");
			trk_dEdx("../samples/mcc86/merged.intime_v1.0.root","trackdEdx_intime.root");
			trk_dEdx("../samples/mcc87/merged.data5e19_v10.root","trackdEdx_data5e19.root");
			trk_dEdx("../samples/mcc87/merged.bnbext_v7.0.root","trackdEdx_bnbext.root");
	return 0;
}


