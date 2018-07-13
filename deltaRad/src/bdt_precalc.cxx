#include "bdt_precalc.h"
double bragg_fnc(double *x, double *par) {
	return par[0]*pow(x[0],par[1]);
}



int addPreFriends(bdt_file* filein,std::string which){
	filein->addFriend( which+"_info"  , filein->tag+"_friends.root");
    std::cout << "Added " << which+"_info" << " from " << filein->tag+"_friends.root" << std::endl;
	return 0;
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

				//Hard kill any silly dEdx, Dropping first and last point
				if(vtrk_resrange->at(s).size()>1){
				for(int k=1; k < vtrk_resrange->at(s).size()-1; k++){

					bool is_sensible =vtrk_dEdx->at(s).at(k) < max_realistic_dEdx; 
					bool is_nan =vtrk_dEdx->at(s).at(k) != vtrk_dEdx->at(s).at(k); 
					bool is_inf = std::isinf(vtrk_dEdx->at(s).at(k));
					bool is_nonzero = vtrk_dEdx->at(s).at(k) > min_dEdx;

					if(is_sensible && !is_nan && !is_inf && is_nonzero){
						c_resrange.push_back(vtrk_resrange->at(s).at(k));
						c_dEdx.push_back(vtrk_dEdx->at(s).at(k));
					}
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
                    delete gb;
                    delete ga;
                    delete c;


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

				if(brag_c_resrange.size()>1){

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
						for(int m=0;m<c_resrange.size(); m++){

						std::cout<<c_resrange.at(m)<<" "<<trunc_dEdx.at(m)<<std::endl;
						}		

						exit(EXIT_FAILURE);
					}

                    delete bragg;
                    delete pts;

				}else{
					v_track_good_calo.push_back(0);
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
    delete gb;
    delete ga;
    delete c;


	return 0;
}

// Function to calculate the pi0 -> 2gamma decay angle, relative to boost vector
int bdt_precalc::genPi0BoostAngle() {
    TTree *friend_tree = new TTree("pi0_info", "pi0_info");
    std::size_t const N = 30;
    Int_t most_energetic_shower_index = 0;
    Int_t second_most_energetic_shower_index = 0;
    Int_t ccnc = 0, reco_asso_showers = 0, reco_asso_tracks = 0;
    Double_t reco_shower_helper_energy[N] = {0.};
    Double_t reco_shower_dirx[N] = {0.};
    Double_t reco_shower_diry[N] = {0.};
    Double_t reco_shower_dirz[N] = {0.};
    Double_t gamma_opening_angle_lab = 0.;
    Double_t gamma_decay_angle_forward = 0.;
    Double_t gamma_decay_angle_backward = 0.;
    Double_t gamma_z_angle_forward = 0.;
    Double_t gamma_z_angle_backward = 0.;
    Double_t gamma_decay_angle_same = 0.;
    Double_t gamma_decay_angle_opp = 0.;
    Double_t gamma_pi_least_angle = 0.;
    Double_t z_gamma_least_angle = 0.;
    //Double_t gamma_opening_angle_cm = 0.;

    TBranch *breco_shower_energy = 0;
    TBranch *breco_shower_dirx = 0;
    TBranch *breco_shower_diry = 0;
    TBranch *breco_shower_dirz = 0;

    // Get necessary info from BDT files
    file->tvertex->SetBranchAddress("most_energetic_shower_index", &most_energetic_shower_index);
    file->tvertex->SetBranchAddress("second_most_energetic_shower_index", &second_most_energetic_shower_index);
    file->tvertex->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
    file->tvertex->SetBranchAddress("reco_shower_helper_energy", &reco_shower_helper_energy, &breco_shower_energy);
    file->tvertex->SetBranchAddress("reco_shower_dirx", &reco_shower_dirx, &breco_shower_dirx);
    file->tvertex->SetBranchAddress("reco_shower_diry", &reco_shower_diry, &breco_shower_diry);
    file->tvertex->SetBranchAddress("reco_shower_dirz", &reco_shower_dirz, &breco_shower_dirz);

    // Friend tree branch
    TBranch *b_gamma_opening_angle_lab = friend_tree->Branch("gamma_opening_angle_lab", &gamma_opening_angle_lab);
    TBranch *b_gamma_decay_angle_forward = friend_tree->Branch("reco_gamma_decay_angle_forward", &gamma_decay_angle_forward);
    TBranch *b_gamma_decay_angle_backward = friend_tree->Branch("reco_gamma_decay_angle_backward", &gamma_decay_angle_backward);
    TBranch *b_gamma_decay_angle_same = friend_tree->Branch("gamma_decay_angle_same", &gamma_decay_angle_same);
    TBranch *b_gamma_decay_angle_opp = friend_tree->Branch("gamma_decay_angle_opp", &gamma_decay_angle_opp);
    TBranch *b_gamma_z_angle_forward = friend_tree->Branch("gamma_z_angle_forward", &gamma_z_angle_forward);
    TBranch *b_gamma_z_angle_backward = friend_tree->Branch("gamma_z_angle_backward", &gamma_z_angle_backward);
    TBranch *b_gamma_pi_least_angle = friend_tree->Branch("gamma_pi_least_angle", &gamma_pi_least_angle);
    TBranch *b_z_gamma_least_angle = friend_tree->Branch("z_gamma_least_angle", &z_gamma_least_angle);
    //TBranch *b_gamma_opening_angle_cm = friend_tree->Branch("gamma_opening_angle_cm", &gamma_opening_angle_cm);

    //std::ofstream myfile;
    //myfile.open("output_pi0Boost.txt");
	int NN = file->tvertex->GetEntries();
	for(int i=0; i< file->tvertex->GetEntries(); i++) {
		if (i%10000==0)std::cout<<i<<"/"<<NN<<" "<<file->tag<<" "<<std::endl;
		file->tvertex->GetEntry(i);
        gamma_decay_angle_forward = -999.;
        gamma_decay_angle_backward = -999.;
        gamma_opening_angle_lab = -999;
        //gamma_opening_angle_cm = -999;
        gamma_decay_angle_same = -999;
        gamma_decay_angle_opp = -999;
        gamma_z_angle_forward = -999;
        gamma_z_angle_backward = -999;
        gamma_pi_least_angle = -999;
        z_gamma_least_angle = -999;
        // Check for signal
        bool hasTwoShowers = (reco_asso_showers == 2);
        bool goodIndex1 = (most_energetic_shower_index >= 0 && most_energetic_shower_index < 3);
        bool goodIndex2 = (second_most_energetic_shower_index >= 0 && second_most_energetic_shower_index < 3);
        if (hasTwoShowers && goodIndex1 && goodIndex2) { 
            double px = 0., py = 0., pz = 0.; 
            double px1 = 0., py1 = 0., pz1 = 0.; 
            double px2 = 0., py2 = 0., pz2 = 0.;
            double E1 = 0., E2 = 0., E_pi = 0.;
            E1 = reco_shower_helper_energy[most_energetic_shower_index]; 
            E2 = reco_shower_helper_energy[second_most_energetic_shower_index]; 
            E_pi = E1 + E2; 

            px1 = E1*reco_shower_dirx[most_energetic_shower_index]; 
            py1 = E1*reco_shower_diry[most_energetic_shower_index];
            pz1 = E1*reco_shower_dirz[most_energetic_shower_index];
            px2 = E2*reco_shower_dirx[second_most_energetic_shower_index]; 
            py2 = E2*reco_shower_diry[second_most_energetic_shower_index];
            pz2 = E2*reco_shower_dirz[second_most_energetic_shower_index]; 

            TLorentzVector gamma1(px1, py1, pz1, E1);
            TLorentzVector gamma2(px2, py2, pz2, E2);
            gamma_opening_angle_lab = gamma1.Vect().Angle(gamma2.Vect());
            /*
            myfile << "gamma1 coordinates, pre-boost:\n" << "\t(" << gamma1.X() 
                                              << ", "  << gamma1.Y()
                                              << ", "  << gamma1.Z()
                                              << ")" << std::endl;

            myfile << "gamma2 coordinates, pre-boost:\n" << "\t(" << gamma2.X() 
                                              << ", "  << gamma2.Y()
                                              << ", "  << gamma2.Z()
                                              << ")" << std::endl;
            */
            TLorentzVector p_pi = gamma1 + gamma2;
            TVector3 preBoost(p_pi.X(), p_pi.Y(), p_pi.Z());
            
            TVector3 boostVec = p_pi.BoostVector(); 
            /*
            myfile << "Boost vector:\n"  << "\t(" << boostVec.X() 
                                         << ", "  << boostVec.Y()
                                         << ", "  << boostVec.Z()
                                         << ")"   << std::endl;
            */
            p_pi.Boost(-boostVec);
            gamma1.Boost(-boostVec);
            gamma2.Boost(-boostVec);
            
            /*
            myfile << "gamma1 coordinates, post-boost:\n" << "\t(" << gamma1.X() 
                                              << ", " << gamma1.Y()
                                              << ", " << gamma1.Z()
                                              << ")"  << std::endl;

            myfile << "gamma2 coordinates, post-boost:\n" << "\t(" << gamma2.X() 
                                              << ", " << gamma2.Y()
                                              << ", " << gamma2.Z()
                                              << ")"  << std::endl;
            */
            TVector3 forwardShower;
            TVector3 backwardShower;
            TVector3 pi0SameDirShower;
            TVector3 pi0OppDirShower;
            // Define new coordinates with 
            if (gamma1.Z() > 0 && gamma2.Z() > 0) {
                //myfile << "Both showers forward" << std::endl;
                continue;
            }
            else if (gamma1.Z() < 0 && gamma2.Z() < 0) {
                //myfile << "Both showers backward" << std::endl;
                continue;
            }
            else if (gamma1.Z() > 0 && gamma2.Z() < 0 ) {
                //myfile << "Shower 1 forward" << std::endl;
                forwardShower.SetXYZ(gamma1.X(), gamma1.Y(), gamma1.Z());
                backwardShower.SetXYZ(gamma2.X(), gamma2.Y(), gamma2.Z());
            }
            else if (gamma1.Z() < 0 && gamma2.Z() > 0 ) {
                //myfile << "Shower 2 forward" << std::endl;
                forwardShower.SetXYZ(gamma2.X(), gamma2.Y(), gamma2.Z());
                backwardShower.SetXYZ(gamma1.X(), gamma1.Y(), gamma1.Z());
            }
            else {
                //myfile << "Uhhhh???" << std::endl;
                continue;
            }
            if ((gamma1.Z() > 0) == (boostVec.Z() > 0)) {
                //myfile << "Shower 1 in same direction as pi0" << std::endl;
                pi0SameDirShower.SetXYZ(gamma1.X(), gamma1.Y(), gamma1.Z());
                pi0OppDirShower.SetXYZ(gamma2.X(), gamma2.Y(), gamma2.Z());
            }
            else if ((gamma2.Z() > 0) == (boostVec.Z() > 0)) {
                //myfile << "Shower 2 in same direction as pi0" << std::endl;
                pi0SameDirShower.SetXYZ(gamma2.X(), gamma2.Y(), gamma2.Z());
                pi0OppDirShower.SetXYZ(gamma1.X(), gamma1.Y(), gamma1.Z());
            }
            else {
                //myfile << "Neither shower in same direction???" << std::endl;
                continue;
            }
            
            /*
            myfile << "Forward shower:\n" << "\t(" << forwardShower.X() 
                                          << ", "  << forwardShower.Y()
                                          << ", "  << forwardShower.Z()
                                          << ")"   << std::endl;
            myfile << "Backward shower:\n" << "\t("<< backwardShower.X() 
                                          << ", "  << backwardShower.Y()
                                          << ", "  << backwardShower.Z()
                                          << ")"   << std::endl;

            */
            TVector3 zUnit(0., 0., 1.);
            if (zUnit.Angle(forwardShower) <= zUnit.Angle(backwardShower)) {
                z_gamma_least_angle = cos(zUnit.Angle(forwardShower));
            }
            else if (zUnit.Angle(forwardShower) > zUnit.Angle(backwardShower)) {
                z_gamma_least_angle = cos(zUnit.Angle(backwardShower));
            }
            else z_gamma_least_angle = -999;

            if (boostVec.Angle(forwardShower) <= boostVec.Angle(backwardShower)) {
                gamma_pi_least_angle = gamma_decay_angle_forward;
            }
            else {
                gamma_pi_least_angle = gamma_decay_angle_backward;
            }
            
            gamma_decay_angle_forward = cos(boostVec.Angle(forwardShower));
            gamma_decay_angle_backward = cos(boostVec.Angle(backwardShower));
            gamma_decay_angle_same = cos(boostVec.Angle(pi0SameDirShower));
            gamma_decay_angle_opp = cos(boostVec.Angle(pi0OppDirShower));
            gamma_z_angle_forward = cos(zUnit.Angle(forwardShower));
            gamma_z_angle_backward = cos(zUnit.Angle(backwardShower));

            /*
            gamma_decay_angle_forward = boostVec.Angle(forwardShower);
            gamma_decay_angle_backward = boostVec.Angle(backwardShower);
            gamma_decay_angle_same = boostVec.Angle(pi0SameDirShower);
            gamma_decay_angle_opp = boostVec.Angle(pi0OppDirShower);
            gamma_z_angle_forward = zUnit.Angle(forwardShower);
            gamma_z_angle_backward = zUnit.Angle(backwardShower);
            */
            
            //gamma_opening_angle_cm = cos(gamma1.Vect().Angle(gamma2.Vect()));

            //myfile << "Decay angle = " << gamma_decay_angle_forward << std::endl;
            //myfile << "Opening angle, lab = " << gamma_opening_angle_lab << std::endl;
            //myfile << "Opening angle, cm = " << gamma_opening_angle_cm << std::endl;
            //myfile << "-----------------------------------------------------------" << std::endl;
        }
        
		friend_tree->Fill();
        
        std::fill_n(reco_shower_helper_energy, N, 0);
        std::fill_n(reco_shower_dirx, N, 0);
        std::fill_n(reco_shower_diry, N, 0);
        std::fill_n(reco_shower_dirz, N, 0);
        
	}

	friend_file_out->cd();
	friend_tree->Write();

    return 0;
}

int bdt_precalc::genPi0Info(){
	TTree * friend_tree = new TTree("pi0_info","pi0_info");

	// Some branches for some basic info
	// double asso_r
	int reco_asso_tracks = 0;
	int reco_asso_showers = 0;
	int pi0_class_number = 0;
	std::vector<double> *vall_reco_tracks = 0;
	std::vector<double> *vall_reco_showers = 0;

	TBranch *ballt = 0;
	TBranch *balls = 0;

    // Get variables from BDT files (name of branch, pointer to some variable)
	file->tvertex->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
	file->tvertex->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
	file->tvertex->SetBranchAddress("all_reco_tracks_dist_from_vertex",&vall_reco_tracks,&ballt);
	file->tvertex->SetBranchAddress("all_reco_showers_dist_from_vertex",&vall_reco_showers,&balls);
    //file->tvertex->SetBranchAddress("all_gamma_decay_angles", &vall_gamma_decays);
	file->tvertex->SetBranchAddress("pi0_class_number",&pi0_class_number);

	// New branches for FRIEND TREE 
	double v_pi0_class_number=0;
	std::vector<int> vec_reco_showers_within;
	std::vector<int> vec_reco_tracks_within;
    std::vector<double> vec_gamma_decay_angles;

	int reco_showers_within_10 = 0;
	int reco_showers_within_20 = 0;
	int reco_showers_within_30 = 0;

	int reco_tracks_within_10 = 0;
	int reco_tracks_within_20 = 0;
	int reco_tracks_within_30 = 0;

	TBranch *b_vec_showers = friend_tree->Branch("num_reco_showers_within_Xcm_vertex",&vec_reco_showers_within);
	TBranch *b_vec_tracks = friend_tree->Branch("num_reco_tracks_within_Xcm_vertex",&vec_reco_tracks_within);
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

		for(double c=1; c<32.0; c=c+2.0){
			vec_reco_showers_within.push_back(0);
			vec_reco_tracks_within.push_back(0);


			for(int j=0; j< vall_reco_tracks->size(); j++){

				if(vall_reco_tracks->at(j)<c){
					vec_reco_tracks_within.back()++;
				}
			}


			for(int j=0; j< vall_reco_showers->size(); j++){

				if(vall_reco_showers->at(j)<c){
					vec_reco_showers_within.back()++;
				}	
			}
		}



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

		vec_reco_showers_within.clear();
		vec_reco_tracks_within.clear();

	}
	friend_file_out->cd();
	friend_tree->Write();


	return 0;
}



int bdt_precalc::genBNBcorrectionInfo(){

	TTree * friend_tree = new TTree("bnbcorrection_info","bnbcorrection_info");
	int nu_pdg = 0;
	double true_nu_E = 0;
	file->tvertex->SetBranchAddress("nu_pdg", &nu_pdg);
	file->tvertex->SetBranchAddress("true_nu_E", &true_nu_E);
	// New branches for FRIEND TREEEE 
	double weight=0;
	TBranch *b_weight = friend_tree->Branch("weight",&weight);

	TFile *f_bnbcorr = new TFile("bnbcorr.root","recreate");


	TFile *f_old = new TFile("../../bnbcorrection/bnb_oldflux_volAVTPC.root","read");
	TH1F* h_mu_nue_old = (TH1F*)f_old->Get("h_mu_nue");
	TH1F* h_mu_nuebar_old = (TH1F*)f_old->Get("h_mu_nuebar");
	TH1F* h_mu_numu_old = (TH1F*)f_old->Get("h_mu_numu");
	TH1F* h_mu_numubar_old = (TH1F*)f_old->Get("h_mu_numubar");

	TH1F* h_pi_nue_old = (TH1F*)f_old->Get("h_pi_nue");
	TH1F* h_pi_nuebar_old = (TH1F*)f_old->Get("h_pi_nuebar");
	TH1F* h_pi_numu_old = (TH1F*)f_old->Get("h_pi_numu");
	TH1F* h_pi_numubar_old = (TH1F*)f_old->Get("h_pi_numubar");

	TH1F* h_k0_nue_old = (TH1F*)f_old->Get("h_k0_nue");
	TH1F* h_k0_nuebar_old = (TH1F*)f_old->Get("h_k0_nuebar");
	TH1F* h_k0_numu_old = (TH1F*)f_old->Get("h_k0_numu");
	TH1F* h_k0_numubar_old = (TH1F*)f_old->Get("h_k0_numubar");

	TH1F* h_k_nue_old = (TH1F*)f_old->Get("h_k_nue");
	TH1F* h_k_nuebar_old = (TH1F*)f_old->Get("h_k_nuebar");
	TH1F* h_k_numu_old = (TH1F*)f_old->Get("h_k_numu");
	TH1F* h_k_numubar_old = (TH1F*)f_old->Get("h_k_numubar");

	TH1F* h_nue_old = (TH1F*)h_mu_nue_old->Clone("h_nue_old");
	h_nue_old->Add(h_pi_nue_old);
	h_nue_old->Add(h_k0_nue_old);
	h_nue_old->Add(h_k_nue_old);

	TH1F* h_nuebar_old = (TH1F*)h_mu_nuebar_old->Clone("h_nuebar_old");
	h_nuebar_old->Add(h_pi_nuebar_old);
	h_nuebar_old->Add(h_k0_nuebar_old);
	h_nuebar_old->Add(h_k_nuebar_old);

	TH1F* h_numu_old = (TH1F*)h_mu_numu_old->Clone("h_numu_old");
	h_numu_old->Add(h_pi_numu_old);
	h_numu_old->Add(h_k0_numu_old);
	h_numu_old->Add(h_k_numu_old);

	TH1F* h_numubar_old = (TH1F*)h_mu_numubar_old->Clone("h_numubar_old");
	h_numubar_old->Add(h_pi_numubar_old);
	h_numubar_old->Add(h_k0_numubar_old);
	h_numubar_old->Add(h_k_numubar_old);




	TFile *f_new = new TFile("../../bnbcorrection/bnb_newflux_volAVTPC.root","read");
	TH1F* h_mu_nue_new = (TH1F*)f_new->Get("h_mu_nue");
	TH1F* h_mu_nuebar_new = (TH1F*)f_new->Get("h_mu_nuebar");
	TH1F* h_mu_numu_new = (TH1F*)f_new->Get("h_mu_numu");
	TH1F* h_mu_numubar_new = (TH1F*)f_new->Get("h_mu_numubar");

	TH1F* h_pi_nue_new = (TH1F*)f_new->Get("h_pi_nue");
	TH1F* h_pi_nuebar_new = (TH1F*)f_new->Get("h_pi_nuebar");
	TH1F* h_pi_numu_new = (TH1F*)f_new->Get("h_pi_numu");
	TH1F* h_pi_numubar_new = (TH1F*)f_new->Get("h_pi_numubar");

	TH1F* h_k0_nue_new = (TH1F*)f_new->Get("h_k0_nue");
	TH1F* h_k0_nuebar_new = (TH1F*)f_new->Get("h_k0_nuebar");
	TH1F* h_k0_numu_new = (TH1F*)f_new->Get("h_k0_numu");
	TH1F* h_k0_numubar_new = (TH1F*)f_new->Get("h_k0_numubar");

	TH1F* h_k_nue_new = (TH1F*)f_new->Get("h_k_nue");
	TH1F* h_k_nuebar_new = (TH1F*)f_new->Get("h_k_nuebar");
	TH1F* h_k_numu_new = (TH1F*)f_new->Get("h_k_numu");
	TH1F* h_k_numubar_new = (TH1F*)f_new->Get("h_k_numubar");

	TH1F* h_nue_new = (TH1F*)h_mu_nue_new->Clone("h_nue_new");
	h_nue_new->Add(h_pi_nue_new);
	h_nue_new->Add(h_k0_nue_new);
	h_nue_new->Add(h_k_nue_new);

	TH1F* h_nuebar_new = (TH1F*)h_mu_nuebar_new->Clone("h_nuebar_new");
	h_nuebar_new->Add(h_pi_nuebar_new);
	h_nuebar_new->Add(h_k0_nuebar_new);
	h_nuebar_new->Add(h_k_nuebar_new);

	TH1F* h_numu_new = (TH1F*)h_mu_numu_new->Clone("h_numu_new");
	h_numu_new->Add(h_pi_numu_new);
	h_numu_new->Add(h_k0_numu_new);
	h_numu_new->Add(h_k_numu_new);

	TH1F* h_numubar_new = (TH1F*)h_mu_numubar_new->Clone("h_numubar_new");
	h_numubar_new->Add(h_pi_numubar_new);
	h_numubar_new->Add(h_k0_numubar_new);
	h_numubar_new->Add(h_k_numubar_new);



	TH1F * h_nue_ratio = (TH1F*)h_nue_new->Clone("h_nue_ratio");
	h_nue_ratio->Divide(h_nue_old);

	TH1F * h_nuebar_ratio = (TH1F*)h_nuebar_new->Clone("h_nuebar_ratio");
	h_nuebar_ratio->Divide(h_nuebar_old);

	TH1F * h_numu_ratio = (TH1F*)h_numu_new->Clone("h_numu_ratio");
	h_numu_ratio->Divide(h_numu_old);

	TH1F * h_numubar_ratio = (TH1F*)h_numubar_new->Clone("h_numubar_ratio");
	h_numubar_ratio->Divide(h_numubar_old);



	



	int NN = file->tvertex->GetEntries();
	for(int i=0; i< NN; i++){
		file->tvertex->GetEntry(i);
		if (i%10000==0)std::cout<<i<<"/"<<NN<<" "<<file->tag<<" "<<std::endl;

		weight = 1.0;



		if(nu_pdg == 12){
			int thisbin = h_nue_ratio->GetXaxis()->FindBin(true_nu_E);	
			weight = h_nue_ratio->GetBinContent(thisbin);
		}else if(nu_pdg == -12){
			int thisbin = h_nuebar_ratio->GetXaxis()->FindBin(true_nu_E);	
			weight = h_nuebar_ratio->GetBinContent(thisbin);
		}else if(nu_pdg == 14){
			int thisbin = h_numu_ratio->GetXaxis()->FindBin(true_nu_E);	
			weight = h_numu_ratio->GetBinContent(thisbin);
		}else if(nu_pdg == -14){
			int thisbin = h_numubar_ratio->GetXaxis()->FindBin(true_nu_E);	
			weight = h_numubar_ratio->GetBinContent(thisbin);
		}else{
			weight = 1.0;
		}



		friend_tree->Fill();


	}

	friend_file_out->cd();
	friend_tree->Write();



	f_bnbcorr->cd();
	h_nue_ratio->Write();
	h_nuebar_ratio->Write();
	h_numu_ratio->Write();
	h_numubar_ratio->Write();
	f_bnbcorr->Close();


	return 0;
}




