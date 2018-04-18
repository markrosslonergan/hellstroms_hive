#include "bdt_precalc.h"
double bragg_fnc(double *x, double *par) {
	return par[0]*pow(x[0],par[1]);
}



int addPreFriends(bdt_file* filein,std::string which){
	filein->addFriend( which+"_info"  , filein->tag+"_friends.root");
	return 0;
}



int bdt_precalc::gen(std::string which ){



}

int bdt_precalc::genTrackInfo(){

	//Some constants, not chosen for any huge reason
	double max_realistic_dEdx = 25;
	double min_realistic_hits = 5;
	double min_dEdx = 0.01;


	TTree * friend_tree = new TTree("track_info","track_info");

	//Some branches for some basic info
	//	double asso_r
	int reco_asso_tracks = 0;
	int reco_asso_showers = 0;
	//std::vector<double> longest_asso_track_displacement = 0;	
	//Branches for the dEdx calo only!
	std::vector<std::vector<double>> *vtrk_dEdx = 0;
	std::vector<std::vector<double>> *vtrk_resrange = 0;
	std::vector<double>* true_track_pdg=0;

	TBranch *bdEdx = 0;
	TBranch *bresrange = 0;
	TBranch *btrackpdg = 0;

	file->tvertex->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
	file->tvertex->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
	file->tvertex->SetBranchAddress("reco_track_calo_dEdx",&vtrk_dEdx,&bdEdx);
	file->tvertex->SetBranchAddress("reco_track_calo_resrange",&vtrk_resrange,&bresrange);
	file->tvertex->SetBranchAddress("true_track_pdg",&true_track_pdg,&btrackpdg);

	// New branches for FRIEND TREEEE 
	double v_bragg_parA=0;
	double v_bragg_parD=0;
	int v_reco_asso_tracks = 0;


	std::vector<double> v_mean_dEdx;	
	std::vector<double> v_start_mean_dEdx;	
	std::vector<double> v_end_mean_dEdx;	
	std::vector<double> v_track_range;
	std::vector<double> v_track_PIDA;
	std::vector<double> v_track_braggA;
	std::vector<double> v_track_good_calo;
	std::vector<double> v_track_kinetic;

	TBranch *b_num_tracks = friend_tree->Branch("reco_asso_tracks",&v_reco_asso_tracks);

	TBranch *b_mean_dEdx = friend_tree->Branch("reco_track_mean_dEdx",&v_mean_dEdx);
	TBranch *b_start_mean_dEdx = friend_tree->Branch("reco_track_end_mean_dEdx",&v_end_mean_dEdx);
	TBranch *b_end_mean_dEdx = friend_tree->Branch("reco_track_start_mean_dEdx",&v_start_mean_dEdx);
	TBranch *b_track_length = friend_tree->Branch("reco_track_range",&v_track_range);
	TBranch *b_track_PIDA = friend_tree->Branch("reco_track_PIDA",&v_track_PIDA);
	TBranch *b_bragg_parA = friend_tree->Branch("reco_track_braggA",&v_track_braggA);
	TBranch *b_kinetic = friend_tree->Branch("reco_track_kinetic",&v_track_kinetic);
	TBranch *b_good_calo = friend_tree->Branch("reco_track_good_calo",&v_track_good_calo);



	//Truncated Mean Testing
	TFile* ftest = new TFile("test_trunc.root","recreate");
	bool done = false;
	std::vector<double> bx;
	std::vector<double> by;
	std::vector<double> ay;

	int dra =0;
	int NN = file->tvertex->GetEntries();
	for(int i=0; i< file->tvertex->GetEntries(); i++){

		file->tvertex->GetEntry(i);

		v_bragg_parA = -999;
		v_bragg_parD = -999;		


		v_reco_asso_tracks = reco_asso_tracks;
		//if(i>5000) break;

		if(reco_asso_tracks == 0){
			//welp nothing to do here
		}else if(reco_asso_tracks >0){

			for(int s=0; s<reco_asso_tracks; s++){

				if (i%2000==0)std::cout<<i<<"/"<<NN<<" "<<file->tag<<" "<<vtrk_dEdx->at(s).size()<<" "<<vtrk_resrange->at(s).size()<<" "<<reco_asso_tracks<<" "<<reco_asso_showers<<std::endl;

				double sum_dEdx =0;
				double sum_dEdx_start = 0;
				double sum_dEdx_end = 0;
				double num_start =0;
				double num_end =0;

				std::vector<float> start_dEdx;
				std::vector<float> start_resrange;

				std::vector<float> end_dEdx;
				std::vector<float> end_resrange;



				TruncMean tm;
				std::vector<double> trunc_dEdx;
				std::vector<double> c_resrange;
				std::vector<double> c_dEdx;



				//Hard kill any silly dEdx
				for(int k=0; k < vtrk_resrange->at(s).size(); k++){

					bool is_sensible =vtrk_dEdx->at(s).at(k) < max_realistic_dEdx; 
					bool is_nan =vtrk_dEdx->at(s).at(k) != vtrk_dEdx->at(s).at(k); 
					bool is_inf = std::isinf(vtrk_dEdx->at(s).at(k));
					bool is_nonzero = vtrk_dEdx->at(s).at(k) > min_dEdx;

					if(is_sensible && !is_nan && !is_inf && is_nonzero){
						c_resrange.push_back(vtrk_resrange->at(s).at(k));
						c_dEdx.push_back(vtrk_dEdx->at(s).at(k));
					}
				}

				//If we dont have enough hits, move to next track and tag this track as "BAD"
				if(c_dEdx.size() < min_realistic_hits ){
					v_track_range.push_back(-999);
					v_track_PIDA.push_back(-999);
					v_track_braggA.push_back(-999);
					v_start_mean_dEdx.push_back(-999);	
					v_end_mean_dEdx.push_back(-999);	
					v_mean_dEdx.push_back(-999);	
					v_track_good_calo.push_back(-999);
					v_track_kinetic.push_back(-999);
					continue;
				}

				//otherwise track is OK	
				v_track_good_calo.push_back(1);


				//Calculate appriopiate radius
				double tenth_track = std::max(c_resrange.front(), c_resrange.back())/20.0;
				double min_dx = 999;
				for(int j = c_resrange.size()-1; j>1; j--){
					double dx = fabs(c_resrange.at(j)-c_resrange.at(j-1));
					if(dx < min_dx) min_dx = dx;
				}

				double rad = std::max( min_dx*2, tenth_track); 
				tm.setRadius( rad  );


				tm.CalcTruncMeanProfile(c_resrange,c_dEdx, trunc_dEdx);			

				if(false && trunc_dEdx.size()> 150 && trunc_dEdx.back() >  trunc_dEdx.front() && true_track_pdg->at(s) == 13 && dra < 100){
					bx = c_resrange;
					by = c_dEdx;
					ay = trunc_dEdx;
					std::vector<double> ax = bx;
					ay.erase(ay.begin());
					ax.erase(ax.begin());
					

					done = true;
					std::cout<<"Got Done! "<<dra<<std::endl;
					//Some Truncated Mean Testing
					
					TGraph * gb = new TGraph(bx.size(),&bx[0], &by[0]);
					TGraph * ga = new TGraph(ax.size(),&ax[0], &ay[0]);
					TCanvas *c = new TCanvas();
					gb->Draw("ap");
					gb->SetMarkerStyle(20);
					gb->SetMarkerSize(1.5);


					ga->Draw("l");
					ga->SetLineWidth(2);
					ga->SetLineColor(kRed-7);

					gb->GetXaxis()->SetTitle("Residual Range [cm]");
					gb->GetYaxis()->SetTitle("dE/dx [MeV/cm]");

					dra++;
	
					c->SaveAs(("truncpics/muon_"+std::to_string(dra)+".png").c_str(),"png");


				}

				//Check to make sure the vectors are filled in the right order. (Just for bragg variable!)
				std::vector<double> brag_c_resrange = c_resrange;	
				std::vector<double> brag_trunc_dEdx = trunc_dEdx;	
			
				if(brag_c_resrange.front() < brag_c_resrange.back()){
					std::reverse(brag_c_resrange.begin(),brag_c_resrange.end());
					std::reverse(brag_trunc_dEdx.begin(),brag_trunc_dEdx.end());
				}
				for(int j=0; j< brag_trunc_dEdx.size(); j++){
				//		if(brag_c_resrange.at(j) ==0) brag_c_resrange.at(j)=0.001;
				}
				while( brag_c_resrange.back()==0){
					brag_c_resrange.pop_back();
					brag_trunc_dEdx.pop_back();

				}


				std::vector<double> params = {15.0, -0.4};
				TF1 *bragg = new TF1("bragg", bragg_fnc, brag_c_resrange.front(), brag_c_resrange.back(), params.size());
				bragg->SetParameters(&params[0]);
				bragg->SetParNames("bragg_A","bragg_b");
				bragg->SetParLimits(0, 0.01, 30.0);
				//bragg->SetParLimits(1, -1.5, 1);
				bragg->FixParameter(1,-0.42);

				TGraph * pts = new TGraph(brag_trunc_dEdx.size(), &brag_c_resrange[0],&brag_trunc_dEdx[0] );
	//			TFitResultPtr fit_bragg = pts->Fit(bragg,"SQ");	
				TFitResultPtr fit_bragg = pts->Fit(bragg,"SWNQ");	
				v_bragg_parD = fit_bragg->Value(1);		
				v_bragg_parA = fit_bragg->Value(0);		
				
			
				if(!fit_bragg->IsValid()){
					std::cout<<"ERROR: fit result is not valid: status code: "<<fit_bragg->Status()<<" "<<std::endl;
					std::cout<<c_resrange.size()<<" "<<trunc_dEdx.size()<<std::endl;
					exit(EXIT_FAILURE);
				}

				double pida_sum=0;

				for(int j=0; j<trunc_dEdx.size(); j++){

					//Some error checking
					if( trunc_dEdx.at(j) != trunc_dEdx.at(j) || std::isinf(trunc_dEdx.at(j))){
						std::cout<<"Truncated dedx is either inf or nan @ "<<j<<" "<<trunc_dEdx.at(j)<<std::endl;
						std::cout<<"Vector Length : "<<trunc_dEdx.size()<<std::endl;
						std::cout<<"i\t range \t dedx \t trunc dedx"<<std::endl;
						for(int m=0; m<trunc_dEdx.size(); m++){
							std::cout<<m<<"\t"<<c_resrange.at(m)<<"  "<<c_dEdx.at(m)<<"  "<<trunc_dEdx.at(m)<<std::endl;
						}
						std::cout<<"Using Radius: "<<rad<<std::endl;
						exit(EXIT_FAILURE);
					}


					if(trunc_dEdx.at(j) > 1000*max_realistic_dEdx){
						//	std::cout<<"LARGE dEdx: "<<c_resrange.at(j)<<" "<<trunc_dEdx.at(j)<<" size: "<<trunc_dEdx.size()<<" pretrunc: "<<vtrk_dEdx->at(s).at(j)<<std::endl;	
					}
					if(trunc_dEdx.at(j) < 0){
						std::cout<<"Negative dEdx: "<<c_resrange.at(j)<<" "<<trunc_dEdx.at(j)<<std::endl;	
					}


					pida_sum += trunc_dEdx.at(j)/(pow(c_resrange.at(j),-0.42));
					sum_dEdx += trunc_dEdx.at(j);

					if( j < trunc_dEdx.size()/2){
						sum_dEdx_start += trunc_dEdx.at(j);
						num_start+=1.0;
					}

					if( j >= trunc_dEdx.size()/2){
						sum_dEdx_end += trunc_dEdx.at(j);
						num_end+=1.0;
					}

				}


				double previous_resrange = c_resrange.front();
				double energy_sum = 0;
				for(int j=0; j< c_resrange.size(); j++){
					double current_resrange = 0;
					if(j+1 < c_resrange.size()) current_resrange = c_resrange.at(j+1);
					double dx = fabs(previous_resrange-current_resrange);
					energy_sum += trunc_dEdx.at(j)*dx/1000.0;	

					if(trunc_dEdx.at(j)<0) std::cout<<"Negative dEdx!! "<<trunc_dEdx.at(j)<<std::endl;
					if(dx<0) std::cout<<"Negative dx!! "<<dx<<" "<<j<<" "<<previous_resrange<<" "<<current_resrange<<std::endl;

					previous_resrange = current_resrange;	
				}	

				v_track_range.push_back(std::max(vtrk_resrange->at(s).front(), vtrk_resrange->at(s).back()));
				v_track_kinetic.push_back(energy_sum);
				v_track_braggA.push_back(v_bragg_parA);
				v_track_PIDA.push_back(pida_sum/(double)c_resrange.size());
				v_mean_dEdx.push_back(sum_dEdx/(double)c_resrange.size());
				v_start_mean_dEdx.push_back(sum_dEdx_start/num_start);
				v_end_mean_dEdx.push_back(sum_dEdx_end/num_end);

			}

		}

		// OK now some error checking!..
		for(int j=0; j<v_mean_dEdx.size();j++){
			if(v_mean_dEdx.at(j) != v_mean_dEdx.at(j)) 	{
				std::cout<<"Yarp: "<<" "<<v_mean_dEdx.at(j)<<" "<<v_start_mean_dEdx.at(j)<<std::endl;
				v_mean_dEdx.at(j) =-999;
			}
			if(v_start_mean_dEdx.at(j) != v_start_mean_dEdx.at(j)) v_start_mean_dEdx.at(j) =-999;
		}


		friend_tree->Fill();
		v_track_range.clear();
		v_track_kinetic.clear();
		v_track_PIDA.clear();
		v_track_braggA.clear();
		v_start_mean_dEdx.clear();	
		v_end_mean_dEdx.clear();	
		v_mean_dEdx.clear();	
		v_track_good_calo.clear();

	}
	friend_file_out->cd();
	friend_tree->Write();


	//Some Truncated Mean Testing
	ftest->cd();
	TGraph * gb = new TGraph(bx.size(),&bx[0], &by[0]);
	TGraph * ga = new TGraph(bx.size(),&bx[0], &ay[0]);
	TCanvas *c = new TCanvas();
	gb->Draw("ap");
	gb->SetMarkerStyle(20);
	gb->SetMarkerSize(2);

	ga->Draw("l");
	ga->SetLineColor(kRed-7);

	c->Write();


	return 0;
}



int bdt_precalc::genPi0Info(){


	TTree * friend_tree = new TTree("pi0_info","pi0_info");

	//Some branches for some basic info
	//	double asso_r
	int reco_asso_tracks = 0;
	int reco_asso_showers = 0;
	int pi0_class_number = 0;
	//std::vector<double> longest_asso_track_displacement = 0;	
	std::vector<double> *vall_reco_tracks= 0;
	std::vector<double> *vall_reco_showers= 0;

	TBranch *ballt = 0;
	TBranch *balls = 0;

	file->tvertex->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
	file->tvertex->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
	file->tvertex->SetBranchAddress("all_reco_tracks_dist_from_vertex",&vall_reco_tracks,&ballt);
	file->tvertex->SetBranchAddress("all_reco_showers_dist_from_vertex",&vall_reco_showers,&balls);
	file->tvertex->SetBranchAddress("pi0_class_number",&pi0_class_number);
	// New branches for FRIEND TREEEE 


	double v_pi0_class_number=0;
	int reco_showers_within_10 = 0;
	int reco_showers_within_20 = 0;
	int reco_showers_within_30 = 0;
	
	int reco_tracks_within_10 = 0;
	int reco_tracks_within_20 = 0;
	int reco_tracks_within_30 = 0;

	TBranch *b_reco_showers_within_10 = friend_tree->Branch("num_reco_showers_within_10cm_vertex",&reco_showers_within_10);
	TBranch *b_reco_showers_within_20 = friend_tree->Branch("num_reco_showers_within_20cm_vertex",&reco_showers_within_20);
	TBranch *b_reco_showers_within_30 = friend_tree->Branch("num_reco_showers_within_30cm_vertex",&reco_showers_within_30);
	TBranch *b_reco_tracks_within_10 = friend_tree->Branch("num_reco_tracks_within_10cm_vertex",&reco_tracks_within_10);
	TBranch *b_reco_tracks_within_20 = friend_tree->Branch("num_reco_tracks_within_20cm_vertex",&reco_tracks_within_20);
	TBranch *b_reco_tracks_within_30 = friend_tree->Branch("num_reco_tracks_within_30cm_vertex",&reco_tracks_within_30);

	TBranch *b_pi0_class_number = friend_tree->Branch("pi0_class_number",&v_pi0_class_number);
	
	int NN = file->tvertex->GetEntries();
	for(int i=0; i< file->tvertex->GetEntries(); i++){

		if (i%10000==0)std::cout<<i<<"/"<<NN<<" "<<file->tag<<" "<<std::endl;
		reco_showers_within_10 = 0;
		reco_showers_within_20 = 0;
		reco_showers_within_30 = 0;

		reco_tracks_within_10 = 0;
		reco_tracks_within_20 = 0;
		reco_tracks_within_30 = 0;

		file->tvertex->GetEntry(i);
		v_pi0_class_number = pi0_class_number;

		for(int j=0; j< vall_reco_tracks->size(); j++){
			if(vall_reco_tracks->at(j) < 10){
				reco_tracks_within_10 ++;
				reco_tracks_within_20 ++;
				reco_tracks_within_30 ++;
			}else if(vall_reco_tracks->at(j) < 20){
				reco_tracks_within_20 ++;
				reco_tracks_within_30 ++;
			}else if(vall_reco_tracks->at(j) < 30){
				reco_tracks_within_30 ++;
			}	
		}
		
		for(int j=0; j< vall_reco_showers->size(); j++){
			if(vall_reco_showers->at(j) < 10){
				reco_showers_within_10 ++;
				reco_showers_within_20 ++;
				reco_showers_within_30 ++;
			}else if(vall_reco_showers->at(j) < 20){
				reco_showers_within_20 ++;
				reco_showers_within_30 ++;
			}else if(vall_reco_showers->at(j) < 30){
				reco_showers_within_30 ++;
			}	
		}
		

		friend_tree->Fill();

	}
	friend_file_out->cd();
	friend_tree->Write();


	return 0;
}



