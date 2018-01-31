#ifndef EFFICIENCY_H
#define EFFICIENCY_H

#include "data_mc_testing_suite.h"

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

			title_size_ratio=0.125*0.6;
			label_size_ratio=0.125*0.6;
			title_offset_ratioY = 0.45 ;
			title_offset_ratioX = 1.45;

			title_size_upper=0.125*0.5;
			label_size_upper=0.125*0.5;
			title_offset_upper = 0.75;

		}

		int plot(TFile *fout, double mva1, double mva2){
			//These will be better soon
			fout->cd();
			std::string const swtrigger = "passed_swtrigger ==1";
			std::string const pre_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140";
			std::string const base_selection = "passed_swtrigger == 1 && reco_asso_showers == 1 ";
			std::string const track_only = "reco_asso_tracks >0";
			std::string const no_track = "reco_asso_tracks == 0";
			std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";

			std::string bdt_cut_notrack = "((reco_asso_tracks == 0 && " + signal->friend_tree_name + ".mva > "+std::to_string(mva1)+"))";
			std::string bdt_cut_track = "((reco_asso_tracks > 0 && " + signal->friend_tree_name + ".mva > "+std::to_string(mva2)+"))";

			double plot_POT = 5e19;

			std::cout<<"Starting plotting routines"<<std::endl;
			TCanvas * c = new TCanvas((var.name+"can").c_str(), (var.name+"can").c_str(),1600,800);
			c->Divide(2,1);

			std::vector<std::string> event_track = {"exiting_proton_number == 0", "exiting_proton_number >0"};	
			std::vector<std::string> whichtrack = {"notrack","sometrack"};
			std::vector<std::string> whichNtrack = {"1g0p","1gNp"};
			whichtrack = whichNtrack;
			std::vector<std::string> whichtrack_cut = {no_track,track_only};
			std::vector<std::string> whichtrack_BDT = {bdt_cut_notrack, bdt_cut_track};
			//Track, notrack loop
			for(int i=0; i<2; i++){
			
				
				


				std::cout<<"Grabbing signal histograms"<<std::endl;
				TH1* h_signal_base   =  (TH1*)signal->getEventTH1(var,signal_definition+"&&"+event_track.at(i),"signal_"+whichtrack.at(i), plot_POT );
				TH1* h_signal_vertex =	(TH1*)signal->getTH1(var, base_selection+"&&"+signal_definition+"&&"+whichtrack_cut.at(i) , "signal_"+whichtrack.at(i)+"_vertex", plot_POT);
				h_signal_base =h_signal_vertex;
				TH1* h_signal_precut = 	(TH1*)signal->getTH1(var, base_selection+"&&"+signal_definition+"&&"+whichtrack_cut.at(i)+"&&"+pre_cut , "signal_"+whichtrack.at(i)+"_precut", plot_POT);
				TH1* h_signal_BDT = 	(TH1*)signal->getTH1(var, base_selection+"&&"+signal_definition+"&&"+whichtrack_cut.at(i)+"&&"+pre_cut + "&&"+ whichtrack_BDT.at(i), "signal_"+whichtrack.at(i)+"_notrack_BDT", plot_POT);


				if(signal->tag == "NCDeltaCosmics"){
					std::cout<<"MARKTEST: "<<signal->tag<<" "<<base_selection<<" && "<<signal_definition<<" && "<<whichtrack_cut.at(i)<<" && "<<pre_cut<<" N: "<<h_signal_precut->Integral()<<std::endl;
				}



				double N_signal_base = h_signal_base->GetSumOfWeights();
				double N_signal_vertex = h_signal_vertex->GetSumOfWeights();
				double N_signal_precut = h_signal_precut->GetSumOfWeights();
				double N_signal_BDT = h_signal_BDT->GetSumOfWeights();

				double to66 = 6.6e20/5e19;
				std::cout<<"For: "<<whichtrack.at(i)<<std::endl;
				std::cout<<"Starting 5e19 Signal: "<<N_signal_base<<" precuts: "<<N_signal_precut<<" BDT: "<<N_signal_BDT<<std::endl;
				std::cout<<"Starting 6.6e20 Signal: "<<to66*N_signal_base<<" precuts: "<<to66*N_signal_precut<<" BDT: "<<to66*N_signal_BDT<<std::endl;


				c->cd(i+1);          // Go back to the main canvas before defining pad0bot

				TLegend * leg_top  = new TLegend(0.51,0.51,0.89,0.89);
				leg_top->SetLineColor(kWhite);
				leg_top->SetHeader("6.6e20 POT | effciency (%)");
			
				TPad *pad1top = new TPad("pad1top", "pad1top", 0, 0.4, 1, 1.0);
				pad1top->SetBottomMargin(0); // Upper and lower plot are joined
				pad1top->Draw();             // Draw the upper pad: pad1top
				pad1top->cd();               // pad1top becomes the current pad

				h_signal_base->SetStats(0);

				h_signal_base->SetLineColor(kBlack);h_signal_base->SetLineWidth(2);
				h_signal_vertex->SetLineColor(kGreen+1);h_signal_vertex->SetLineWidth(2);
				h_signal_precut->SetLineColor(kRed-3);h_signal_precut->SetLineWidth(2);
				h_signal_BDT->SetLineColor(kBlue-3);h_signal_BDT->SetLineWidth(2);

				//leg_top->AddEntry(h_signal_base,(whichNtrack.at(i)+": "+to_string_prec(to66*N_signal_base,3)+" | 100%").c_str(),"l");
				leg_top->AddEntry(h_signal_vertex,("Vertexing: "+to_string_prec(to66*N_signal_vertex,3)+" | "+ to_string_prec(N_signal_vertex/N_signal_base*100,3)+"%").c_str(),"l");
				leg_top->AddEntry(h_signal_precut,("Precuts: "+to_string_prec(to66*N_signal_precut,3)+" | "+ to_string_prec(N_signal_precut/N_signal_base*100,3)+"%").c_str(),"l");
				leg_top->AddEntry(h_signal_BDT,("BDT: "+to_string_prec(to66*N_signal_BDT,3)+" | " +to_string_prec(N_signal_BDT/N_signal_base*100,3)+"%").c_str(),"l");

				h_signal_base->SetTitle(whichtrack.at(i).c_str());
				h_signal_base->Draw();
				h_signal_vertex->Draw("same");
				h_signal_precut->Draw("same");
				h_signal_BDT->Draw("same");

				h_signal_base->GetYaxis()->SetTitle("Verticies [au]");
				h_signal_base->SetMinimum(0.001);
				leg_top->Draw();

				   h_signal_base->GetYaxis()->SetTitleOffset(title_offset_upper);
				   h_signal_base->GetXaxis()->SetTitleOffset(title_offset_upper);

                   				 h_signal_base->GetXaxis()->SetTitle("");
				   h_signal_base->GetYaxis()->SetTitleSize(title_size_upper);
				   h_signal_base->GetXaxis()->SetTitleSize(title_size_upper);
				   h_signal_base->GetYaxis()->SetLabelSize(label_size_upper);
				   h_signal_base->GetXaxis()->SetTitle(var.unit.c_str());
				   h_signal_base->GetXaxis()->SetLabelSize(label_size_upper);




				c->cd(i+1);// go back to main canvas 
				TPad *pad0bot = new TPad("pad0bot", "pad0bot", 0, 0.05, 1, 0.4);
				pad0bot->SetTopMargin(0);
				pad0bot->SetBottomMargin(0.401);
				pad0bot->SetGridx(); // vertical grid
				pad0bot->Draw();
				pad0bot->cd();       // pad0bot becomes the current pad
				//pad0bot->SetLogy();

				TH1* rat_signal_precut = (TH1*)h_signal_precut->Clone("rat_signal_precut");
				TH1* rat_signal_vertex = (TH1*)h_signal_vertex->Clone("rat_signal_vertex");
				TH1* rat_signal_BDT = (TH1*)h_signal_BDT->Clone("rat_signal_BDt");

				rat_signal_vertex->SetStats(0);

				rat_signal_precut->Divide(h_signal_base);		
				rat_signal_vertex->Divide(h_signal_base);		
				rat_signal_BDT->Divide(h_signal_base);		

				rat_signal_vertex->SetTitle("");
				rat_signal_vertex->SetLineColor(kRed-3);
				rat_signal_vertex->SetMarkerColor(kRed-3);
				rat_signal_vertex->SetLineWidth(2);
	
				rat_signal_precut->SetLineColor(kRed-3);
				rat_signal_precut->SetMarkerColor(kRed-3);
				rat_signal_precut->SetLineWidth(2);
				rat_signal_BDT->SetLineColor(kBlue-3);
				rat_signal_BDT->SetMarkerColor(kBlue-3);
				rat_signal_BDT->SetLineWidth(2);

				rat_signal_precut->Draw("E1");	
				rat_signal_precut->SetTitle("");
			//	rat_signal_vetex->Draw("E1 same");	
				rat_signal_BDT->Draw("E1 same");
				rat_signal_precut->SetMinimum(rat_signal_BDT->GetMinimum()*0.9);
				rat_signal_precut->SetMaximum(1.199);

				TLine *line = new TLine(rat_signal_precut->GetXaxis()->GetXmin(),1.0,rat_signal_precut->GetXaxis()->GetXmax(),1.0 );
				line->Draw("same");

				   rat_signal_precut->GetYaxis()->SetTitle("Efficiency");
				   rat_signal_precut->GetYaxis()->SetTitleOffset(title_offset_ratioY);
				   rat_signal_precut->GetXaxis()->SetTitleOffset(title_offset_ratioX);

				   rat_signal_precut->GetYaxis()->SetTitleSize(title_size_ratio);
				   rat_signal_precut->GetXaxis()->SetTitleSize(title_size_ratio);
				   rat_signal_precut->GetYaxis()->SetLabelSize(label_size_ratio);
				   rat_signal_precut->GetXaxis()->SetTitle(var.unit.c_str());
				   rat_signal_precut->GetXaxis()->SetLabelSize(label_size_ratio);


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


			return 0;
		}	


};


#endif
