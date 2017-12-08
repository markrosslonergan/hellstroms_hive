#include "data_mc.h"

class eff_analysis{
	public:
		bdt_variable var;
		std::vector<bdt_file*> files;
		bdt_file * signal;

		int Nfiles;

		double title_size_ratio;
		double label_size_ratio;
		double title_offset_ratioX;
		double title_offset_ratioY;

		double title_size_upper;
		double label_size_upper;
		double title_offset_upper;
		std::vector<int> which_stack;	

		eff_analysis(bdt_file * insig, bdt_variable varin) : signal(insig), var(varin){	

			title_size_ratio=0.125;
			label_size_ratio=0.125;
			title_offset_ratioY = 0.3 ;
			title_offset_ratioX = 1.1;

			title_size_upper=0.2;
			label_size_upper=0.07;
			title_offset_upper = 1.45;

		}

		int plot(TFile *fout, double mva1, double mva2){
			//These will be better soon
			fout->cd();
			std::string const swtrigger = "passed_swtrigger ==1";
			std::string const pre_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140";
			std::string const base_selection = "passed_swtrigger == 1 && reco_asso_showers == 1";
			std::string const track_only = "reco_asso_tracks >0";
			std::string const no_track = "reco_asso_tracks == 0";
			std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";

			std::string bdt_cut_notrack = "((reco_asso_tracks == 0 && " + signal->friend_tree_name + ".mva > "+std::to_string(mva1)+"))";
			std::string bdt_cut_track = "((reco_asso_tracks > 0 && " + signal->friend_tree_name + ".mva > "+std::to_string(mva2)+"))";

			double plot_POT = 5e19;

			std::cout<<"Starting plotting routines"<<std::endl;
			TCanvas * c = new TCanvas((var.name+"can").c_str(), (var.name+"can").c_str(),1600,800);
			c->Divide(2,1);

		
			std::vector<std::string> whichtrack = {"notrack","sometrack"};
			std::vector<std::string> whichNtrack = {"1g0p","1gNp"};
			std::vector<std::string> whichtrack_cut = {no_track,track_only};
			std::vector<std::string> whichtrack_BDT = {bdt_cut_notrack, bdt_cut_track};
			//Track, notrack loop
			for(int i=0; i<2; i++){
			
				
				


				std::cout<<"Grabbing signal histograms"<<std::endl;
				TH1* h_signal_base   =	(TH1*)signal->getTH1(var, base_selection+"&&"+signal_definition+"&&"+whichtrack_cut.at(i) , "signal_"+whichtrack.at(i), plot_POT);
				TH1* h_signal_precut = 	(TH1*)signal->getTH1(var, base_selection+"&&"+signal_definition+"&&"+whichtrack_cut.at(i)+"&&"+pre_cut , "signal_"+whichtrack.at(i)+"_precut", plot_POT);
				TH1* h_signal_BDT = 	(TH1*)signal->getTH1(var, base_selection+"&&"+signal_definition+"&&"+whichtrack_cut.at(i)+"&&"+pre_cut + "&&"+ whichtrack_BDT.at(i), "signal_"+whichtrack.at(i)+"_notrack_BDT", plot_POT);


				double N_signal_base = h_signal_base->GetSumOfWeights();
				double N_signal_precut = h_signal_precut->GetSumOfWeights();
				double N_signal_BDT = h_signal_BDT->GetSumOfWeights();

				double to66 = 6.6e20/5e19;
				std::cout<<"For: "<<whichtrack.at(i)<<std::endl;
				std::cout<<"Starting 5e19 Signal: "<<N_signal_base<<" precuts: "<<N_signal_precut<<" BDT: "<<N_signal_BDT<<std::endl;
				std::cout<<"Starting 6.6e20 Signal: "<<to66*N_signal_base<<" precuts: "<<to66*N_signal_precut<<" BDT: "<<to66*N_signal_BDT<<std::endl;


				c->cd(i+1);          // Go back to the main canvas before defining pad0bot

				TLegend * leg_top  = new TLegend(0.61,0.61,0.89,0.89);
				leg_top->SetLineColor(kWhite);
				leg_top->SetHeader("6.6e20 POT | eff");
			
				TPad *pad1top = new TPad("pad1top", "pad1top", 0, 0.4, 1, 1.0);
				pad1top->SetBottomMargin(0); // Upper and lower plot are joined
				pad1top->Draw();             // Draw the upper pad: pad1top
				pad1top->cd();               // pad1top becomes the current pad

				h_signal_base->SetStats(0);

				h_signal_base->SetLineColor(kBlack);h_signal_base->SetLineWidth(2);
				h_signal_precut->SetLineColor(kRed-3);h_signal_precut->SetLineWidth(2);
				h_signal_BDT->SetLineColor(kBlue-3);h_signal_BDT->SetLineWidth(2);

				leg_top->AddEntry(h_signal_base,(whichNtrack.at(i)+": "+to_string_prec(to66*N_signal_base,3)+" | 100%").c_str(),"l");
				leg_top->AddEntry(h_signal_precut,("precuts: "+to_string_prec(to66*N_signal_precut,3)+" | "+ to_string_prec(N_signal_precut/N_signal_base*100,3)+"%").c_str(),"l");
				leg_top->AddEntry(h_signal_BDT,("BDT: "+to_string_prec(to66*N_signal_BDT,3)+" | " +to_string_prec(N_signal_BDT/N_signal_base*100,3)+"%").c_str(),"l");

				h_signal_base->SetTitle(whichtrack.at(i).c_str());
				h_signal_base->Draw();
				h_signal_precut->Draw("same");
				h_signal_BDT->Draw("same");

				h_signal_base->GetYaxis()->SetTitle("Verticies");
				h_signal_base->SetMinimum(h_signal_BDT->GetMinimum()*0.9);
				leg_top->Draw();

				c->cd(i+1);// go back to main canvas 
				TPad *pad0bot = new TPad("pad0bot", "pad0bot", 0, 0.05, 1, 0.4);
				pad0bot->SetTopMargin(0);
				pad0bot->SetBottomMargin(0.401);
				pad0bot->SetGridx(); // vertical grid
				pad0bot->Draw();
				pad0bot->cd();       // pad0bot becomes the current pad
				//pad0bot->SetLogy();


				TH1* rat_signal_precut = (TH1*)h_signal_precut->Clone("rat_signal_precut");
				TH1* rat_signal_BDT = (TH1*)h_signal_BDT->Clone("rat_signal_BDt");

				rat_signal_precut->SetStats(0);

				rat_signal_precut->Divide(h_signal_base);		
				rat_signal_BDT->Divide(h_signal_base);		

				rat_signal_precut->SetTitle("");
				rat_signal_precut->SetLineColor(kRed-3);
				rat_signal_precut->SetMarkerColor(kRed-3);
				rat_signal_precut->SetLineWidth(2);
				rat_signal_BDT->SetLineColor(kBlue-3);
				rat_signal_BDT->SetMarkerColor(kBlue-3);
				rat_signal_BDT->SetLineWidth(2);

				rat_signal_precut->Draw("E1");	
				rat_signal_BDT->Draw("E1 same");
				rat_signal_precut->SetMinimum(rat_signal_BDT->GetMinimum()*0.9);
				rat_signal_precut->GetXaxis()->SetTitle(var.unit.c_str());
				rat_signal_precut->GetYaxis()->SetTitle("Efficiency");

				TLine *line = new TLine(rat_signal_precut->GetXaxis()->GetXmin(),1.0,rat_signal_precut->GetXaxis()->GetXmax(),1.0 );
				line->Draw("same");

			}

			c->Write();


			/*
			   ratsw->SetMinimum(0.85);	
			   ratsw->SetMaximum(1.15);	
			   ratsw->SetTitle("");
			   ratsw->GetYaxis()->SetTitle("Ratio");
			   ratsw->GetYaxis()->SetTitleOffset(title_offset_ratioY);
			   ratsw->GetXaxis()->SetTitleOffset(title_offset_ratioX);

			   ratsw->GetYaxis()->SetTitleSize(title_size_ratio);
			   ratsw->GetXaxis()->SetTitleSize(title_size_ratio);
			   ratsw->GetYaxis()->SetLabelSize(label_size_ratio);
			   ratsw->GetXaxis()->SetTitle(var.unit.c_str());
			   ratsw->GetXaxis()->SetLabelSize(label_size_ratio);

			   var_swtrig.front()->GetYaxis()->SetRangeUser(0.1,ymax_sel);
			   var_swtrig.front()->GetYaxis()->SetTitle("Verticies");
			 */



		}	


};

int run_efficiency(){

	std::string MCFRIEND = "runtmva_merged_app.root";


	bdt_file *data = new bdt_file("../../../samples/data/", "merged.data5e19_v6.0.root","Data5e19","e1","LEEPhotonAnalysisData",MCFRIEND,"data",kBlue-4,true );
	bdt_file *ext = new bdt_file("../../../samples/data/", "merged.bnbext_v3.0.root","BNBext","hist","LEEPhotonAnalysisData", MCFRIEND,"dataext",kGreen-3,true);

	bdt_file *mc4 = new bdt_file("../../../samples/mcc84", "merged.bnbcosmic_v2.0.root","BNB+cosmicOverlay_8.4","hist","LEEPhoton", MCFRIEND,"bnb_cosmic",kRed-4,false);
	bdt_file *sig = new bdt_file("../../../samples/mcc84/", "merged.ncdeltacosmic_v1.0.root","NCDelta","hist","LEEPhoton", MCFRIEND,"ncdelta_cosmic",kYellow,false);
	//bdt_file *sig = new bdt_file("../../../samples/mcc82/", "runmv_sp_cosmic.root","LEE NCDelta","hist","LEEPhoton", MCFRIEND,"ncdelta_cosmic",kYellow,false);

	mc4->leg = "f";
	sig->leg = "f";
	ext->leg = "f";

	double N_off_events = 373820;//  136208;// v2.0;
	double N_on_events = 544479;

	data->setPOT(4.95e19);
	ext->setPOT(4.95e19);


	double data_scale_factor = 547616.0/N_on_events;
	double ext_scale_factor = 1.285*(382718.0/N_off_events);//*(N_on_events/547616.0);
	ext->scale(ext_scale_factor);
	//	sig->scale(2.1);

	std::vector<bdt_file*> vec_files = {data,ext,mc4,sig};
	std::vector<int> which_stack = {0,1,1,1};


	//(std::string inname, std::string inbin, bool intrack) : 
	std::vector<bdt_variable> vars;

	int level = 2;


	if(level>=0){

		vars.push_back(bdt_variable("reco_shower_dedx_plane2","(48,0,15)", "Shower dE/dx Collection Plane [MeV/cm]",false));
		vars.push_back(bdt_variable("summed_associated_helper_shower_energy","(25,0,0.5)","Reco Shower Energy [GeV]", false));
		vars.push_back(bdt_variable("longest_asso_track_displacement","(25,0,500)","Track Length [cm]", true));
		vars.push_back(bdt_variable("reco_asso_tracks","(5,0,4)","Number Reco Tracks",false));

	}if (level>=1){

		vars.push_back(bdt_variable("totalpe_ibg_sum","(25,0,2000)","Number of PE",false));
		vars.push_back(bdt_variable("reco_asso_showers","(6,0,5)","Number of Reco Showers",false));	
		vars.push_back(bdt_variable("closest_asso_shower_dist_to_flashzcenter","(25,0,1000)","Distance from shower to flashcenter [cm]",false));
		vars.push_back(bdt_variable("reco_nu_vtx_dist_to_closest_tpc_wall","(25,0,125)","Vertex-TPC Wall Distance [cm]",false));
		vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(25,0,10)","Photon Coversion Length [cm]" ,false));
		vars.push_back(bdt_variable("most_energetic_shower_bp_dist_to_tpc","(25,0,1000)","Back Projected Distance from shower to TPC wall [cm]",false));
		vars.push_back(bdt_variable("most_energetic_shower_reco_thetaxz","(25,-1.7,1.7)","Shower Angle xy [rad]",false));
		vars.push_back(bdt_variable("most_energetic_shower_reco_thetayz","(25,-1.7,1.7)","Shower Angle yz [rad]",false));
		vars.push_back(bdt_variable("longest_asso_track_thetayz","(25,-1.7,1.7)","Track Angle yz [rad]",true));
		vars.push_back(bdt_variable("longest_asso_track_thetaxz","(25,-1.7,1.7)","Track Angle xz [rad]",true));

	}if(level>=2){

		vars.push_back(bdt_variable("summed_associated_reco_shower_energy","(25,0,0.5)","Energy [GeV]",false));
		vars.push_back(bdt_variable("most_energetic_shower_reco_length","(25,0,125)","Distance [cm]",false));
		vars.push_back(bdt_variable("reco_nuvertx","(25,0,300)","Distance [cm]",false));
		vars.push_back(bdt_variable("reco_nuverty","(25,-300,+300)","Distance [cm]",false));
		vars.push_back(bdt_variable("reco_nuvertz","(25,0,1000)","Distance [cm]",false));
		vars.push_back(bdt_variable("reco_shower_dedx_plane0","(48,0,15)", "dE/dx [MeV/cm]",false));
		vars.push_back(bdt_variable("reco_shower_dedx_plane1","(48,0,15)", "dE/dx [MeV/cm]",false));
		vars.push_back(bdt_variable("totalpe_bbg_sum","(25,0,2000)","Num PE",false));
	}

	bdt_variable true_shower("delta_photon_energy","(25,0,1.0)","True Photon Energy [GeV]", false);


	TFile *fout = new TFile("eff_response.root","recreate");	
	eff_analysis eff(sig, true_shower);
	eff.plot(fout, 0.5, 0.5);

	eff_analysis eff_reco(sig, vars.at(1));
	eff_reco.plot(fout, 0.5, 0.5);

	fout->Close();

	return 0;
}



int data_mc(){
	return 0;//obsolete
}

