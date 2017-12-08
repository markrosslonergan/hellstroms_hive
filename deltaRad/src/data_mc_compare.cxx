#include "data_mc.h"

class bdt_response_analysis{
	public:
		std::vector<bdt_file*> files;
		std::vector<int> which_stack;	
		int Nfiles;

		bdt_response_analysis(std::vector<bdt_file*> vecin, std::vector<int> stackin) :which_stack(stackin), files(vecin){	
			Nfiles = vecin.size();
		}

		

		int plot_bdt_response(TFile *fout){
			//These will be better soon
			std::string const swtrigger = "passed_swtrigger ==1";
			std::string const pre_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140";
			std::string const base_selection = "passed_swtrigger == 1 && reco_asso_showers == 1";
			std::string const track_only = "reco_asso_tracks >0";
			std::string const no_track = "reco_asso_tracks == 0";
			std::string bdt_cut = "";
			double plot_POT = 5e19;


			fout->cd();
			TCanvas * cbdt = new TCanvas("BDT Response Stacked (Log)", "BDT Response Stacked (Log)",1600,1600);
			cbdt->Divide(2,2);

			std::vector<TH1*> bdt_response_track;
			std::vector<TH1*> bdt_response_notrack;

			std::vector<TGraph*> eff_bdt_response_track;
			std::vector<TGraph*> eff_bdt_response_notrack;

			TGraph * notrack_signif; std::vector<double> mva_notrack;
			TGraph * track_signif;  std::vector<double> mva_track;
			
			std::vector<std::vector<double>> n_verticies_afterbdt_track;
			std::vector<std::vector<double>> n_verticies_afterbdt_notrack;
			
			double best_sig_mva_track=-99;
			double best_sig_track=-99;

			double best_sig_mva_notrack=-99;
			double best_sig_notrack =-99;


			double mvastart = 0.28;
			double mvastop = 0.55;
			double mvastep = 0.0025;
			for(auto &file: files){
				std::cout<<"On file: "<<file->name<<" || "<<file->tag<<std::endl;
				//BDT starting plots	
				cbdt->cd(1);
				file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+"bdt_0(40,0.28,0.55)").c_str() ,(base_selection +"&&"+pre_cut +"&& reco_asso_tracks ==0").c_str(),"goff");
				bdt_response_notrack.push_back( (TH1*)gDirectory->Get((file->tag+"bdt_0").c_str() ));
				bdt_response_notrack.back()->Scale(file->scale_data*plot_POT/file->pot);

				cbdt->cd(2);
				file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+ "bdt_1(40,0.28,0.55)").c_str() ,(base_selection+"&&"+pre_cut +"&& reco_asso_tracks >0").c_str(),"goff");
				bdt_response_track.push_back( (TH1*)gDirectory->Get((file->tag+"bdt_1").c_str() ));
				bdt_response_track.back()->Scale(file->scale_data*plot_POT/file->pot);

				double n_verticies_after_precuts_notrack = bdt_response_notrack.back()->GetSumOfWeights();
				double n_verticies_after_precuts_track = bdt_response_track.back()->GetSumOfWeights();

				std::vector<double> vec_mva_notrack;
				std::vector<double> vec_mva_track;
				std::vector<double> eff_notrack;
				std::vector<double> eff_track;

				std::vector<double> n_verticies_file_track;
				std::vector<double> n_verticies_file_notrack;

				for(double mva_notrack =mvastart; mva_notrack<mvastop; mva_notrack+=mvastep){
					std::cout<<"NoTrack BDT efficienciys, on "<<mva_notrack<<std::endl;
					bdt_cut = "((reco_asso_tracks == 0 && " + file->friend_tree_name + ".mva >= "+std::to_string(mva_notrack)+")) ";
					vec_mva_notrack.push_back(mva_notrack);
					file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+std::to_string(mva_notrack)+ "bdt_0(40,0.28,0.55)").c_str() ,(base_selection+"&&"+pre_cut +"&& reco_asso_tracks == 0" +"&&"+ bdt_cut).c_str(),"goff");
					TH1* tmp = (TH1*)gDirectory->Get((file->tag+std::to_string(mva_notrack)+"bdt_0").c_str() );
					tmp->Scale(file->scale_data*plot_POT/file->pot);
					eff_notrack.push_back(tmp->GetSumOfWeights()/n_verticies_after_precuts_notrack );	
					
					n_verticies_file_notrack.push_back(tmp->GetSumOfWeights());
				}
				n_verticies_afterbdt_notrack.push_back(n_verticies_file_notrack);

				TGraph * geff_notrack = new TGraph(vec_mva_notrack.size(),&vec_mva_notrack[0],&eff_notrack[0]);
				

				//Same for tracks
				for(double mva_track =mvastart; mva_track<mvastop; mva_track+=mvastep){
					std::cout<<"Track BDT efficienciys, on "<<mva_track<<std::endl;
					bdt_cut = "((reco_asso_tracks > 0 && " + file->friend_tree_name + ".mva >= "+std::to_string(mva_track)+")) ";
					vec_mva_track.push_back(mva_track);
					file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+std::to_string(mva_track)+ "bdt_0(40,0.28,0.55)").c_str() ,(base_selection+"&&"+pre_cut +"&& reco_asso_tracks >0" +"&&"+ bdt_cut).c_str(),"goff");
					TH1* tmp = (TH1*)gDirectory->Get((file->tag+std::to_string(mva_track)+"bdt_0").c_str() );
					tmp->Scale(file->scale_data*plot_POT/file->pot);
					eff_track.push_back(tmp->GetSumOfWeights()/n_verticies_after_precuts_track );	
				
					n_verticies_file_track.push_back(tmp->GetSumOfWeights());
				}
				n_verticies_afterbdt_track.push_back(n_verticies_file_track);

				TGraph * geff_track = new TGraph(vec_mva_track.size(),&vec_mva_track[0],&eff_track[0]);


				eff_bdt_response_notrack.push_back(geff_notrack);
				eff_bdt_response_track.push_back(geff_track);

				mva_track = vec_mva_track;
				mva_notrack = vec_mva_notrack;
			}

			//Calculate significance, currently sig/sqrt(sig+bkg)
			std::vector<double> sig_notrack;
			std::vector<double> sig_track;

			std::vector<double> num_signal_track;
			std::vector<double> num_signal_notrack;

			for(int j=0; j<mva_notrack.size(); j++){
				double n_signal = n_verticies_afterbdt_notrack.at(3).at(j);
				double n_bkg = n_verticies_afterbdt_notrack.at(1).at(j)+n_verticies_afterbdt_notrack.at(2).at(j);
				if(n_signal == 0.0){
					sig_notrack.push_back(0.0);
				}else{
					sig_notrack.push_back(n_signal/sqrt(n_signal+n_bkg));
				}
				if(sig_notrack.back()>best_sig_notrack){
					best_sig_notrack = sig_notrack.back();
					best_sig_mva_notrack = mva_notrack.at(j);
				}
				num_signal_notrack.push_back(n_signal);

			}	
			for(int j=0; j<mva_track.size(); j++){
				double n_signal = n_verticies_afterbdt_track.at(3).at(j);
				double n_bkg = n_verticies_afterbdt_track.at(1).at(j)+n_verticies_afterbdt_track.at(2).at(j);
				
				if(n_signal == 0.0){
					sig_track.push_back(0.0);
				}else{
	
					sig_track.push_back(n_signal/sqrt(n_signal+n_bkg));
				}
				if(sig_track.back()>best_sig_track){
					best_sig_track = sig_track.back();
					best_sig_mva_track = mva_track.at(j);
				}
				num_signal_track.push_back(n_signal);
			}					
			

	
			std::cout<<"Best significance for notrack is "<<best_sig_notrack<<" at a cut of "<<best_sig_mva_notrack<<std::endl;
			std::cout<<"Best significance for track is "<<best_sig_track<<" at a cut of "<<best_sig_mva_track<<std::endl;
	
			notrack_signif = new TGraph(mva_notrack.size(), &mva_notrack[0], &sig_notrack[0] );
			track_signif = new TGraph(mva_track.size(), &mva_track[0], &sig_track[0]);

			fout->cd();
			std::cout<<"Making plots"<<std::endl;
			TPad*pbdt0 = (TPad*)cbdt->cd(1);
			pbdt0->SetLogy();

			THStack *bdt_stack_notrack = new THStack("bdt_notrack_stack","");
			bdt_response_notrack.at(0)->SetLineColor(kBlack);
			bdt_response_notrack.at(0)->SetStats(0);
			bdt_response_notrack.at(0)->SetLineWidth(3);
			bdt_response_notrack.at(0)->SetMarkerStyle(20);


			for(int t=0; t< Nfiles; t++){
				std::cout<<"On file: "<<t<<" of notrack "<<std::endl;
				if(which_stack.at(t)==0){
					bdt_response_notrack.at(t)->SetTitle("BDT Response (No Track)");
					bdt_response_notrack.at(t)->SetLineColor(files.at(t)->col);
					bdt_response_notrack.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
					bdt_response_notrack.at(t)->SetMinimum(0.01);
				}else{
					bdt_response_notrack.at(t)->SetFillColor(files.at(t)->col);
					bdt_response_notrack.at(t)->SetLineWidth(2);
					bdt_stack_notrack->Add(bdt_response_notrack.at(t));
				}
			}
			for(auto s:which_stack){
				if(s!=0){
					bdt_stack_notrack->Draw("same hist");
					bdt_response_notrack.at(0)->Draw("same");
					break;
				}
			}

			TPad*pbdt1 = (TPad*)cbdt->cd(2);
			pbdt1->SetLogy();

			THStack *bdt_stack_track = new THStack("bdt_track_stack","");
			bdt_response_track.at(0)->SetLineColor(kBlack);
			bdt_response_track.at(0)->SetStats(0);
			bdt_response_track.at(0)->SetLineWidth(3);
			bdt_response_track.at(0)->SetMarkerStyle(20);


			for(int t=0; t< Nfiles; t++){
				std::cout<<"On file: "<<t<<" of track "<<std::endl;
				if(which_stack.at(t)==0){
					bdt_response_track.at(t)->SetTitle("BDT Response (Track)");
					bdt_response_track.at(t)->SetLineColor(files.at(t)->col);
					bdt_response_track.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
					bdt_response_track.at(t)->SetMinimum(0.01);
				}else{
					bdt_response_track.at(t)->SetFillColor(files.at(t)->col);	
					bdt_response_track.at(t)->SetLineWidth(2);
					bdt_stack_track->Add(bdt_response_track.at(t));
				}
			}
			for(auto s:which_stack){
				if(s!=0){
					bdt_stack_track->Draw("same hist");
					bdt_response_track.at(0)->Draw("same");
					break;
				}
			}



			// Now plot TGraphs

			TLegend * l_eff_notrack = new TLegend(0.11,0.11,0.4,0.4);
			l_eff_notrack->SetFillStyle(0);
			TLegend * l_eff_track = new TLegend(0.11,0.11,0.4,0.4);
			l_eff_track->SetFillStyle(0);

			for(int t=0; t< Nfiles; t++){
				std::string plt = "lp";
				if(t==0) plt = "acp";
				
				cbdt->cd(3)->SetLogy();
				eff_bdt_response_notrack.at(t)->SetLineColor(files.at(t)->col);
				eff_bdt_response_notrack.at(t)->SetLineWidth(3);
				eff_bdt_response_notrack.at(t)->Draw(plt.c_str());
				eff_bdt_response_notrack.at(t)->SetMaximum(2);
				//eff_bdt_response_notrack.at(t)->SetMinimum(0.01);
				eff_bdt_response_notrack.at(t)->SetTitle("BDT Efficiencies, no track");


				l_eff_notrack->AddEntry(eff_bdt_response_notrack.at(t), files.at(t)->tag.c_str(),"l");

				cbdt->cd(4)->SetLogy();
				eff_bdt_response_track.at(t)->SetLineWidth(3);
				eff_bdt_response_track.at(t)->SetLineColor(files.at(t)->col);
				eff_bdt_response_track.at(t)->Draw(plt.c_str());
				eff_bdt_response_track.at(t)->SetMaximum(2);
				//eff_bdt_response_track.at(t)->SetMinimum(0.01);
				eff_bdt_response_track.at(t)->SetTitle("BDT Efficiencies, track");
				l_eff_track->AddEntry(eff_bdt_response_track.at(t), files.at(t)->tag.c_str(),"l");
			}
	
	
			cbdt->cd(3);
			notrack_signif->Draw("lp same");			
			l_eff_notrack->AddEntry(notrack_signif,"S/sqrt(S+B)","l");
			l_eff_notrack->Draw();
			
			cbdt->cd(4);
			track_signif->Draw("lp same");			
			l_eff_track->AddEntry(track_signif,"S/sqrt(S+B)","l");
			l_eff_track->Draw();

			cbdt->Write();

			TCanvas * cnum = new TCanvas("Number Signal Events", "Number Signal Events",1600,800);
			cnum->Divide(2,1);
			cnum->cd(1);
			TGraph * g_n_signal_notrack = new TGraph(mva_notrack.size(), &mva_notrack[0], &num_signal_notrack[0]);
			g_n_signal_notrack->Draw("alp");	
	
			cnum->cd(2);
			TGraph * g_n_signal_track = new TGraph(mva_track.size(), &mva_track[0], &num_signal_track[0]);
			g_n_signal_track->Draw("alp");	

			cnum->Write();	

		}


};


class compare_instance{
	public:
		bdt_variable var;
		std::vector<bdt_file*> files;

		int Nfiles;

		double title_size_ratio;
		double label_size_ratio;
		double title_offset_ratioX;
		double title_offset_ratioY;

		double title_size_upper;
		double label_size_upper;
		double title_offset_upper;
		std::vector<int> which_stack;	

		compare_instance(std::vector<bdt_file*> vecin, bdt_variable varin, std::vector<int> stackin) : files(vecin), var(varin), which_stack(stackin){	
			Nfiles = vecin.size();

			title_size_ratio=0.125;
			label_size_ratio=0.125;
			title_offset_ratioY = 0.3 ;
			title_offset_ratioX = 1.1;

			title_size_upper=0.2;
			label_size_upper=0.07;
			title_offset_upper = 1.45;

		}

		int plot(TFile *fout){
			//These will be better soon
			std::string const swtrigger = "passed_swtrigger ==1";
			std::string const pre_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140";
			std::string const base_selection = "passed_swtrigger == 1 && reco_asso_showers == 1";
			std::string const track_only = "reco_asso_tracks >0";
			std::string const no_track = "reco_asso_tracks == 0";
			std::string bdt_cut = "";
			double plot_POT = 5e19;
			//0.478399 Cut2: 0.478399
			//OLD: 0.529841 Cut2: 0.514321
			//curr 0.473046 Cut2: 0.469785

			// s/sqrt(s+b) 
			//0.486784 Cut2: 0.500177
			double mva1 = 0.535;
			double mva2 = 0.4925;//0.509117;

//Best significance for notrack is 0.170905 at a cut of 0.535
//Best significance for track is 0.162339 at a cut of 0.4925

			std::cout<<"Starting to make "<<var.name<<" plot."<<std::endl;
			TCanvas * c = new TCanvas((var.name+"can").c_str(), (var.name+"can").c_str(),1600,1600);
			c->Divide(2,2);
			std::vector<TH1*> var_swtrig;
			std::vector<TH1*> var_selec;
			std::vector<TH1*> var_precut;
			std::vector<TH1*> var_bdtcut;

			double ymax_sw=-999;
			double ymax_sel=-999;
			double ymax_pre=-999;
			double ymax_bdt=-999;

			std::vector<double> vertex_count_sw;
			std::vector<double> vertex_count_sel;
			std::vector<double> vertex_count_pre;
			std::vector<double> vertex_count_bdt;


			std::vector<TH1*> bdt_response_track;
			std::vector<TH1*> bdt_response_notrack;

			//This is where we fill all our nice histograms
			for(auto &file: files){

				bdt_cut = "((reco_asso_tracks == 0 && " + file->friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + file->friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
				std::cout<<"---> VERTEXCOUNT ---------------->"<<var.name<<"---"<<file->tag<<"<----------------------------------------"<<std::endl;
				std::cout<<"---> VERTEXCOUNT Base: "<<" "<<file->tvertex->GetEntries(base_selection.c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT Precut: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT BDT: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut).c_str())*5e19/file->pot<<std::endl;

				vertex_count_sw.push_back(file->tvertex->GetEntries(swtrigger.c_str())*5e19/file->pot);
				vertex_count_sel.push_back(file->tvertex->GetEntries(base_selection.c_str())*5e19/file->pot);
				vertex_count_pre.push_back(file->tvertex->GetEntries((base_selection+"&&"+pre_cut).c_str())*5e19/file->pot);
				vertex_count_bdt.push_back(file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut).c_str())*5e19/file->pot);

				std::cout<<"---> VERTEXCOUNT 0Tracks Base: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+no_track).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT 0Tracks Precut: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+no_track).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT 0Tracks BDT: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut+"&&"+no_track).c_str())*5e19/file->pot<<std::endl;

				std::cout<<"---> VERTEXCOUNT Tracks Base: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+track_only).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT Tracks Precut: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+track_only).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT Tracks BDT: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut+"&&"+track_only).c_str())*5e19/file->pot<<std::endl;


				std::string tcuts = base_selection;	
				std::string tcuts_sw = swtrigger;	
				if(var.is_track){
					tcuts = tcuts +"&&" + track_only;
					tcuts_sw = tcuts_sw +"&&" + track_only;
				}




				//-------------- Just SWtrigger -------------
				c->cd(1);	
				std::string tsw = "tmpsw_"+var.name+"_"+file->tag;
				std::cout<<tsw<<std::endl;		

				file->tvertex->Draw( (var.name+">>"+tsw+ var.binning).c_str() ,tcuts_sw.c_str(),"goff");
				std::cout<<"Done with Draw for "<<(var.name+">>"+tsw+ var.binning).c_str()<<std::endl;

				var_swtrig.push_back( (TH1*)gDirectory->Get(tsw.c_str()) );
				var_swtrig.back()->Scale(file->scale_data*plot_POT/file->pot);
				std::cout<<"Scaled selection by :"<<plot_POT/file->pot<<std::endl;
				ymax_sw = std::max(ymax_sw, var_swtrig.back()->GetBinContent(var_swtrig.back()->GetMaximumBin()) );


				//-------------- 1 RECO shower -------------
				c->cd(2);	
				std::string tsel = "tmpsel_"+var.name+"_"+file->tag;
				std::cout<<tsel<<std::endl;		

				file->tvertex->Draw( (var.name+">>"+tsel+ var.binning).c_str() ,tcuts.c_str(),"goff");
				std::cout<<"Done with Draw for "<<(var.name+">>"+tsel+ var.binning).c_str()<<std::endl;

				var_selec.push_back( (TH1*)gDirectory->Get(tsel.c_str()) );
				var_selec.back()->Scale(file->scale_data*plot_POT/file->pot);
				std::cout<<"Scaled selection by :"<<plot_POT/file->pot<<std::endl;
				ymax_sel = std::max(ymax_sel, var_selec.back()->GetBinContent(var_selec.back()->GetMaximumBin()) );

				//-------------- Precuts -------------
				c->cd(3);	
				std::string tpre = "tmppre_"+var.name+"_"+file->tag;
				file->tvertex->Draw((var.name+">>"+tpre+var.binning).c_str() ,(tcuts+"&&"+pre_cut).c_str(),"goff");
				std::cout<<"Done with draw from "<<tpre<<std::endl;

				var_precut.push_back(  (TH1*)gDirectory->Get(tpre.c_str()));
				var_precut.back()->Scale(file->scale_data*plot_POT/file->pot);;
				std::cout<<"Scaled precut by :"<<plot_POT/file->pot<<std::endl;
				ymax_pre = std::max(ymax_pre, var_precut.back()->GetBinContent(var_precut.back()->GetMaximumBin()) );

				/*
				//BDT starting plots	
				file->tvertex->Draw((file->friend_tree_name+".mva >>"+ tsel+"bdt_0(80,0.32,0.55)").c_str() ,(tcuts+"&&"+pre_cut +"&& reco_asso_tracks ==0").c_str(),"goff");
				bdt_response_notrack.push_back( (TH1*)gDirectory->Get((tsel+"bdt_0").c_str()) );
				bdt_response_notrack.back()->Scale(file->scale_data*plot_POT/file->pot);
				//eff_bdt_response_notrack.push_back((TH1*)var_precut.back()->Clone(("eff_bdt0_"+tsel).cstr());

				file->tvertex->Draw((file->friend_tree_name+".mva >>"+ tsel+"bdt_1(80,0.32,0.55)").c_str() ,(tcuts+"&&"+pre_cut +"&& reco_asso_tracks >0").c_str(),"goff");
				bdt_response_track.push_back( (TH1*)gDirectory->Get((tsel+"bdt_1").c_str()) );
				bdt_response_track.back()->Scale(file->scale_data*plot_POT/file->pot);
				*/


				//-------------- BDT-- -------------
				c->cd(4);			

				std::string tbdt = "tmpbdt_"+var.name+"_"+file->tag;
				file->tvertex->Draw((var.name+">>"+tbdt+var.binning).c_str() ,(tcuts+"&&"+pre_cut+"&&"+bdt_cut).c_str(),"goff");
				std::cout<<"Done with draw from "<<tbdt<<std::endl;

				if(file->tag == "Data5e19" ){
					std::cout<<file->name<<std::endl;
					file->tvertex->Scan("run_number:subrun_number:event_number:longest_asso_track_displacement:summed_associated_helper_shower_energy:reco_shower_dedx_plane2" ,(tcuts+"&&"+pre_cut+"&&"+bdt_cut).c_str() );
				}

				var_bdtcut.push_back(  (TH1*)gDirectory->Get(tbdt.c_str()));
				var_bdtcut.back()->Scale(file->scale_data*plot_POT/file->pot);;
				std::cout<<"Scaled bdtcut by :"<<plot_POT/file->pot<<std::endl;
				ymax_bdt = std::max(ymax_bdt, var_bdtcut.back()->GetBinContent(var_bdtcut.back()->GetMaximumBin()) );



			}

			/*
			TCanvas * cbdt = new TCanvas("BDT Response Stacked (Log)", "BDT Response Stacked (Log)",1600,800);
			cbdt->Divide(2,1);
			TPad*pbdt0 = (TPad*)cbdt->cd(1);
			pbdt0->SetLogy();

			THStack *bdt_stack_notrack = new THStack("bdt_notrack_stack","");
			bdt_response_notrack.at(0)->SetLineColor(kBlack);
			bdt_response_notrack.at(0)->SetStats(0);
			bdt_response_notrack.at(0)->SetLineWidth(3);
			bdt_response_notrack.at(0)->SetMarkerStyle(20);


			for(int t=0; t< Nfiles; t++){
				if(which_stack.at(t)==0){
					bdt_response_notrack.at(t)->SetTitle("BDT Response (No Track)");
					bdt_response_notrack.at(t)->SetLineColor(files.at(t)->col);
					bdt_response_notrack.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
				}else{
					bdt_response_notrack.at(t)->SetFillColor(files.at(t)->col);
					bdt_response_notrack.at(t)->SetLineWidth(2);
					bdt_stack_notrack->Add(bdt_response_notrack.at(t));
				}
			}
			for(auto s:which_stack){
				if(s!=0){
					bdt_stack_notrack->Draw("same hist");
					bdt_response_notrack.at(0)->Draw("same");
					break;
				}
			}

			TPad*pbdt1 = (TPad*)cbdt->cd(2);
			pbdt1->SetLogy();

			THStack *bdt_stack_track = new THStack("bdt_track_stack","");
			bdt_response_track.at(0)->SetLineColor(kBlack);
			bdt_response_track.at(0)->SetStats(0);
			bdt_response_track.at(0)->SetLineWidth(3);
			bdt_response_track.at(0)->SetMarkerStyle(20);


			for(int t=0; t< Nfiles; t++){
				if(which_stack.at(t)==0){
					bdt_response_notrack.at(t)->SetTitle("BDT Response (Track)");
					bdt_response_track.at(t)->SetLineColor(files.at(t)->col);
					bdt_response_track.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
				}else{
					bdt_response_track.at(t)->SetFillColor(files.at(t)->col);	
					bdt_response_track.at(t)->SetLineWidth(2);
					bdt_stack_track->Add(bdt_response_track.at(t));
				}
			}
			for(auto s:which_stack){
				if(s!=0){
					bdt_stack_track->Draw("same hist");
					bdt_response_track.at(0)->Draw("same");
					break;
				}
			}


			cbdt->Write();
			*/

			std::cout<<"---> VERTEXCOUNT ---------------->"<<var.name<<"---RATIO----------------------------------------"<<std::endl;
			std::cout<<"---> VERTEXCOUNT Ratio data/mc Base: "<<" "<<vertex_count_sel.at(0)/vertex_count_sel.at(1)<<std::endl;
			std::cout<<"---> VERTEXCOUNT Ratio data/mc Precut: "<<" "<<vertex_count_pre.at(0)/vertex_count_pre.at(1)<<std::endl;
			std::cout<<"---> VERTEXCOUNT Ratio data/mc BDT: "<<" "<<vertex_count_bdt.at(0)/vertex_count_bdt.at(1)<<std::endl;


			ymax_sw=1.25*ymax_sw;
			ymax_sel=1.25*ymax_sel;
			ymax_pre=1.25*ymax_pre;
			ymax_bdt=1.25*ymax_bdt;


			TLegend *lsw = new TLegend(0.6,0.6,0.89,0.89);lsw->SetLineColor(kBlack);lsw->SetFillStyle(0);
			TLegend *lsel = new TLegend(0.6,0.6,0.89,0.89);lsel->SetLineColor(kBlack);lsel->SetFillStyle(0);
			TLegend *lpre = new TLegend(0.6,0.6,0.89,0.89);lpre->SetLineColor(kBlack);lpre->SetFillStyle(0);
			TLegend *lbdt = new TLegend(0.6,0.6,0.89,0.89);lbdt->SetLineColor(kBlack);lbdt->SetFillStyle(0);





			//   cs->cd(1); hs->Draw(); T.DrawTextNDC(.5,.95,"Default drawing option");
			//  cs->cd(2); hs->Draw("nostack"); T.DrawTextNDC(.5,.95,"Option \"nostack\"");
			//cs->cd(3); hs->Draw("nostackb"); T.DrawTextNDC(.5,.95,"Option \"nostackb\"");
			// cs->cd(4); hs->Draw("lego1"); T.DrawTextNDC(.5,.95,"Option \"lego1\"");
			//Lets get some THStack set up!
			THStack *mc_stack_swtrig = new THStack("MCstack_swtrig","");

			c->cd(1);

			TPad *pad0top = new TPad("pad0top", "pad0top", 0, 0.35, 1, 1.0);
			pad0top->SetBottomMargin(0); // Upper and lower plot are joined
			pad0top->Draw();             // Draw the upper pad: pad0top
			pad0top->cd();               // pad0top becomes the current pad


			for(int t=0; t<Nfiles; t++){
				var_swtrig.at(t)->SetTitle(("SWtrigger Passed : "+var.name).c_str());
				var_swtrig.at(t)->SetLineColor(kBlack);
				var_swtrig.at(t)->SetStats(0);
				var_swtrig.at(t)->SetLineWidth(3);
				var_swtrig.at(t)->SetMarkerStyle(20);

				lsw->AddEntry(var_swtrig.at(t), (files.at(t)->tag+" : "+ to_string_prec( var_swtrig.at(t)->Integral(),1)).c_str()  , files.at(t)->leg.c_str());
				if(which_stack.at(t)==0){
					var_swtrig.at(t)->SetLineColor(files.at(t)->col);
					var_swtrig.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
					var_swtrig.at(t)->GetXaxis()->SetTitleSize(title_size_upper);
					var_swtrig.at(t)->GetXaxis()->SetLabelSize(label_size_upper);
					var_swtrig.at(t)->GetYaxis()->SetLabelSize(label_size_upper);
					var_swtrig.at(t)->GetYaxis()->SetTitleOffset(title_offset_upper);
				}else{
					var_swtrig.at(t)->SetFillColor(files.at(t)->col);
					mc_stack_swtrig->Add(var_swtrig.at(t));
				}
			}

			for(auto s:which_stack){
				if(s!=0){
					mc_stack_swtrig->Draw("same hist");
					var_swtrig.at(0)->Draw("same");
					break;
				}
			}


			c->cd(1);          // Go back to the main canvas before defining pad0bot
			TPad *pad0bot = new TPad("pad0bot", "pad0bot", 0, 0.05, 1, 0.35);
			pad0bot->SetTopMargin(0);
			pad0bot->SetBottomMargin(0.351);
			pad0bot->SetGridx(); // vertical grid
			pad0bot->Draw();
			pad0bot->cd();       // pad0bot becomes the current pad

			TH1* botsw = (TH1*)var_swtrig.at(1)->Clone("bot_sw");
			botsw->Add(var_swtrig.at(2));

			/*	for(int i = Nfiles-1; i > 1; i--){
				for(int j=0; j<=botsw->GetNbinsX(); j++){
				botsw->SetBinContent(i, botsw->GetBinContent(i)+var_swtrig.at(i)->GetBinContent(i));
				}
				}	
			 */
			TH1* ratsw = (TH1*)var_swtrig.front()->Clone("rat_sw");
			ratsw->Divide(botsw);		


			ratsw->SetFillColor(kYellow);
			ratsw->SetLineColor(kBlack);
			ratsw->Draw("E1");	

			TLine *line = new TLine(ratsw->GetXaxis()->GetXmin(),1.0,ratsw->GetXaxis()->GetXmax(),1.0 );
			line->Draw("same");

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



			pad0top->cd();
			lsw->Draw();


			//************************************************
			//Now the 1 reco shower
			//***********************************************
			THStack *mc_stack_selec = new THStack("MCstack_selec","");


			c->cd(2);

			TPad *pad1top = new TPad("pad1top", "pad1top", 0, 0.35, 1, 1.0);
			pad1top->SetBottomMargin(0); // Upper and lower plot are joined
			pad1top->Draw();             // Draw the upper pad: pad1top
			pad1top->cd();               // pad1top becomes the current pad


			for(int t=0; t<Nfiles; t++){
				var_selec.at(t)->SetTitle(("1 Reco Shower : "+var.name).c_str());
				var_selec.at(t)->SetLineColor(kBlack);
				var_selec.at(t)->SetStats(0);
				var_selec.at(t)->SetLineWidth(3);
				var_selec.at(t)->SetMarkerStyle(20);

				lsel->AddEntry(var_selec.at(t), (files.at(t)->tag+" : "+ to_string_prec( var_selec.at(t)->Integral(),1)).c_str()  , files.at(t)->leg.c_str());
				if(which_stack.at(t)==0){
					var_selec.at(t)->SetLineColor(files.at(t)->col);
					var_selec.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
					var_selec.at(t)->GetXaxis()->SetTitleSize(title_size_upper);
					var_selec.at(t)->GetXaxis()->SetLabelSize(label_size_upper);
					var_selec.at(t)->GetYaxis()->SetLabelSize(label_size_upper);
					var_selec.at(t)->GetYaxis()->SetTitleOffset(title_offset_upper);
				}else{
					var_selec.at(t)->SetFillColor(files.at(t)->col);
					mc_stack_selec->Add(var_selec.at(t));
				}
			}

			for(auto s:which_stack){
				if(s!=0){
					mc_stack_selec->Draw("same hist");
					var_selec.at(0)->Draw("same");
					break;
				}
			}


			c->cd(2);          // Go back to the main canvas before defining pad1bot
			TPad *pad1bot = new TPad("pad1bot", "pad1bot", 0, 0.05, 1, 0.35);
			pad1bot->SetTopMargin(0);
			pad1bot->SetBottomMargin(0.351);
			pad1bot->SetGridx(); // vertical grid
			pad1bot->Draw();
			pad1bot->cd();       // pad1bot becomes the current pad


			TH1* botsel = (TH1*)var_selec.at(1)->Clone("bot_sw");
			botsel->Add(var_selec.at(2));

			/*for(int i = Nfiles-1; i > 1; i--){
			  for(int j=0; j<=botsel->GetNbinsX(); j++){
			  botsel->SetBinContent(i, botsel->GetBinContent(i)+var_selec.at(i)->GetBinContent(i));
			  }
			  }*/	

			TH1* ratsel = (TH1*)var_selec.front()->Clone("rat_sel");
			ratsel->Divide(botsel);		

			ratsel->SetFillColor(kYellow);
			ratsel->SetLineColor(kBlack);
			ratsel->Draw("E1");	
			line->Draw("same");

			ratsel->SetTitle("");
			ratsel->GetYaxis()->SetTitle("Ratio");
			ratsel->GetYaxis()->SetTitleOffset(title_offset_ratioY);
			ratsel->GetXaxis()->SetTitleOffset(title_offset_ratioX);

			ratsel->SetMinimum(0.85);	
			ratsel->SetMaximum(1.15);
			ratsel->GetYaxis()->SetTitleSize(title_size_ratio);
			ratsel->GetXaxis()->SetTitleSize(title_size_ratio);
			ratsel->GetYaxis()->SetLabelSize(label_size_ratio);
			ratsel->GetXaxis()->SetTitle(var.unit.c_str());
			ratsel->GetXaxis()->SetLabelSize(label_size_ratio);

			var_selec.front()->GetYaxis()->SetRangeUser(0.1,ymax_sel);
			var_selec.front()->GetYaxis()->SetTitle("Verticies");

			pad1top->cd();
			lsel->Draw();


			//************************************************
			//Now the PRECUT
			//************************************************
			THStack *mc_stack_precut = new THStack("MCstack_precut","");


			c->cd(3);	
			TPad *pad2top = new TPad("pad2top", "pad2top", 0, 0.35, 1, 1.0);
			pad2top->SetBottomMargin(0); // Upper and lower plot are joined
			pad2top->Draw();             // Draw the upper pad: pad2top
			pad2top->cd();               // pad2top becomes the current pad

			for(int t=0; t<Nfiles; t++){
				var_precut.at(t)->SetTitle((" Precuts : "+var.name).c_str());
				var_precut.at(t)->SetLineColor(kBlack);
				var_precut.at(t)->SetStats(0);
				var_precut.at(t)->SetLineWidth(3);
				var_precut.at(t)->SetMarkerStyle(20);

				lpre->AddEntry(var_precut.at(t), (files.at(t)->tag+" : "+ to_string_prec( var_precut.at(t)->Integral(),1)).c_str()  , files.at(t)->leg.c_str());
				if(which_stack.at(t)==0){
					var_precut.at(t)->SetLineColor(files.at(t)->col);
					var_precut.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
					var_precut.at(t)->GetXaxis()->SetTitleSize(title_size_upper);
					var_precut.at(t)->GetXaxis()->SetLabelSize(label_size_upper);
					var_precut.at(t)->GetYaxis()->SetLabelSize(label_size_upper);
					var_precut.at(t)->GetYaxis()->SetTitleOffset(title_offset_upper);
				}else{
					var_precut.at(t)->SetFillColor(files.at(t)->col);
					mc_stack_precut->Add(var_precut.at(t));
				}
			}

			for(auto s:which_stack){
				if(s!=0){
					mc_stack_precut->Draw("same hist");
					var_precut.at(0)->Draw("same");
					break;
				}
			}




			c->cd(3);          // Go back to the main canvas before defining pad1bot
			TPad *pad2bot = new TPad("pad2bot", "pad2bot", 0, 0.05, 1, 0.35);
			pad2bot->SetTopMargin(0);
			pad2bot->SetBottomMargin(0.351);
			pad2bot->SetGridx(); // vertical grid
			pad2bot->Draw();
			pad2bot->cd();       // pad2bot becomes the current pad



			TH1* botpre = (TH1*)var_precut.at(1)->Clone("bot_pre");
			botpre->Add(var_precut.at(2));

			//	for(int i = Nfiles-1; i > 1; i--){
			//		for(int j=0; j<=botpre->GetNbinsX(); j++){
			//			botpre->SetBinContent(i, botpre->GetBinContent(i)+var_precut.at(i)->GetBinContent(i));
			//		}
			//	}	


			TH1* ratpre = (TH1*)var_precut.front()->Clone("rat_pre");
			ratpre->Divide(botpre);		
			ratpre->SetFillColor(kYellow);
			ratpre->Draw("E1");	
			line->Draw("same");


			ratpre->SetLineColor(kBlack);
			ratpre->SetTitle("");
			ratpre->GetYaxis()->SetTitle("Ratio");
			ratpre->GetXaxis()->SetTitleOffset(title_offset_ratioX);
			ratpre->GetYaxis()->SetTitleOffset(title_offset_ratioY);

			ratpre->SetMinimum(0.85);	
			ratpre->SetMaximum(1.15);
			ratpre->GetYaxis()->SetTitleSize(title_size_ratio);
			ratpre->GetXaxis()->SetTitleSize(title_size_ratio);
			ratpre->GetYaxis()->SetLabelSize(label_size_ratio);
			ratpre->GetXaxis()->SetLabelSize(label_size_ratio);
			ratpre->GetXaxis()->SetTitle(var.unit.c_str());

			var_precut.front()->GetYaxis()->SetRangeUser(0.1,ymax_pre);
			var_precut.front()->GetYaxis()->SetTitle("Verticies");

			pad2top->cd();
			lpre->Draw();

			//************************************************
			//Now the BDT
			//************************************************
			THStack *mc_stack_bdtcut = new THStack("MCstack_bdtcut","");



			c->cd(4);	
			TPad *pad3top = new TPad("pad3top", "pad3top", 0, 0.35, 1, 1.0);
			pad3top->SetBottomMargin(0); // Upper and lower plot are joined
			pad3top->Draw();             // Draw the upper pad: pad3top
			pad3top->cd();               // pad3top becomes the current pad

			for(int t=0; t<Nfiles; t++){
				var_bdtcut.at(t)->SetTitle((" BDT: "+var.name).c_str());
				var_bdtcut.at(t)->SetLineColor(kBlack);
				var_bdtcut.at(t)->SetStats(0);
				var_bdtcut.at(t)->SetLineWidth(3);
				var_bdtcut.at(t)->SetMarkerStyle(20);

				lbdt->AddEntry(var_bdtcut.at(t), (files.at(t)->tag+" : "+ to_string_prec( var_bdtcut.at(t)->Integral(),1)).c_str()  , files.at(t)->leg.c_str());
				if(which_stack.at(t)==0){
					var_bdtcut.at(t)->SetLineColor(files.at(t)->col);
					var_bdtcut.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
					var_bdtcut.at(t)->GetXaxis()->SetTitleSize(title_size_upper);
					var_bdtcut.at(t)->GetXaxis()->SetLabelSize(label_size_upper);
					var_bdtcut.at(t)->GetYaxis()->SetLabelSize(label_size_upper);
					var_bdtcut.at(t)->GetYaxis()->SetTitleOffset(title_offset_upper);
				}else{
					var_bdtcut.at(t)->SetFillColor(files.at(t)->col);
					mc_stack_bdtcut->Add(var_bdtcut.at(t));
				}
			}

			for(auto s:which_stack){
				if(s!=0){
					mc_stack_bdtcut->Draw("same hist");
					var_bdtcut.at(0)->Draw("same");
					break;
				}
			}




			c->cd(4);          // Go back to the main canvas before defining pad1bot
			TPad *pad3bot = new TPad("pad3bot", "pad3bot", 0, 0.05, 1, 0.35);
			pad3bot->SetTopMargin(0);
			pad3bot->SetBottomMargin(0.351);
			pad3bot->SetGridx(); // vertical grid
			pad3bot->Draw();
			pad3bot->cd();       // pad3bot becomes the current pad


			TH1* botbdt = (TH1*)var_bdtcut.at(1)->Clone("bot_bdt");
			botbdt->Add(var_bdtcut.at(2));
			//	for(int i = Nfiles-1; i > 1; i--){
			//		for(int j=0; j<=botbdt->GetNbinsX(); j++){
			//			botbdt->SetBinContent(j, botbdt->GetBinContent(j)+var_bdtcut.at(i)->GetBinContent(j) );
			//		}
			//	}	



			TH1* ratbdt = (TH1*)var_bdtcut.front()->Clone("rat_bdt");
			ratbdt->Divide(botbdt);		

			ratbdt->SetFillColor(kYellow);
			ratbdt->Draw();	
			ratbdt->SetLineColor(kBlack);
			ratbdt->SetTitle("");
			ratbdt->GetXaxis()->SetTitleOffset(title_offset_ratioX);
			ratbdt->GetYaxis()->SetTitle("Ratio");
			ratbdt->GetYaxis()->SetTitleOffset(title_offset_ratioY);
			line->Draw("same");

			ratbdt->GetYaxis()->SetTitleSize(title_size_ratio);
			ratbdt->GetXaxis()->SetTitleSize(title_size_ratio);
			ratbdt->GetYaxis()->SetLabelSize(label_size_ratio);
			ratbdt->GetXaxis()->SetLabelSize(label_size_ratio);
			ratbdt->GetXaxis()->SetTitle(var.unit.c_str());

			ratbdt->SetMinimum(0.85);	
			ratbdt->SetMaximum(1.15);

			var_bdtcut.front()->GetYaxis()->SetRangeUser(0.1,ymax_bdt);
			var_bdtcut.front()->GetYaxis()->SetTitle("Verticies");
			pad3top->cd();
			lbdt->Draw();



			fout->cd();
			std::cout<<"Writing out: "<<var.name<<std::endl;
			c->Write();

			TCanvas * ceff = new TCanvas((var.name+"can_eff").c_str(), (var.name+"can_eff").c_str(),1600,1600);
			ceff->Divide(2,2);
			/************************* EFFICIENCYi BIT *********************/

			std::vector<int> eff_col = {kRed-4,kBlue-4,kGreen-3};

			for(int j = 0; j<Nfiles; j++){

				TH1 * h_eff_base = (TH1D*)var_swtrig.at(j)->Clone("eff_base"); 
				TH1 * h_eff_pre = (TH1D*)var_precut.at(j)->Clone("eff_pre"); 
				TH1 * h_eff_sel = (TH1D*)var_selec.at(j)->Clone("eff_sel"); 
				TH1 * h_eff_bdt = (TH1D*)var_bdtcut.at(j)->Clone("eff_bdt"); 

				h_eff_pre->Divide(h_eff_base);
				h_eff_sel->Divide(h_eff_base);
				h_eff_bdt->Divide(h_eff_base);
				TPad *p = (TPad*)ceff->cd(j+1);


				double eff_sel = h_eff_sel->GetSumOfWeights()/h_eff_base->GetSumOfWeights();
				double eff_pre = h_eff_pre->GetSumOfWeights()/h_eff_base->GetSumOfWeights();
				double eff_bdt = h_eff_bdt->GetSumOfWeights()/h_eff_base->GetSumOfWeights();
				
				h_eff_sel->SetLineColor(eff_col.at(0) );
				h_eff_sel->SetMarkerColor(eff_col.at(0) );

				h_eff_pre->SetLineColor(eff_col.at(1) );
				h_eff_pre->SetMarkerColor(eff_col.at(1) );

				h_eff_bdt->SetLineColor(eff_col.at(2) );
				h_eff_bdt->SetMarkerColor(eff_col.at(2) );

				p->SetLogy();
				h_eff_bdt->Draw("E1");
				h_eff_bdt->SetTitle(files.at(j)->tag.c_str());
				h_eff_bdt->SetMaximum(1.0);
				if(j==0){
					double mi = h_eff_bdt->GetMinimum();
					if(mi>=0){
						h_eff_bdt->SetMinimum(1e-5);
					}else{ h_eff_bdt->SetMinimum(mi);
					}
				}

				h_eff_bdt->GetYaxis()->SetTitleSize(title_size_ratio*0.4);
				h_eff_bdt->GetXaxis()->SetTitleSize(title_size_ratio*0.4);
				h_eff_bdt->GetYaxis()->SetLabelSize(label_size_ratio*0.4);
				h_eff_bdt->GetXaxis()->SetLabelSize(label_size_ratio*0.4);
				h_eff_bdt->GetXaxis()->SetTitle(var.unit.c_str());
				h_eff_bdt->GetYaxis()->SetTitle("Efficiency");

				TLegend *leff = new TLegend(0.31,0.11,0.75,0.4);
				leff->AddEntry(h_eff_sel,("1 Shower and Vertex: "+std::to_string(eff_sel*100)+"%").c_str(),"lp");
				leff->AddEntry(h_eff_pre,("Precuts: "+std::to_string(eff_pre*100)+"%").c_str(),"lp");
				leff->AddEntry(h_eff_bdt,("BDT: "+std::to_string(eff_bdt*100)+"%").c_str(),"lp");
				leff->SetFillStyle(0);
				leff->SetLineColor(kWhite);
				leff->Draw();


				h_eff_sel->Draw("E1 same");
				h_eff_pre->Draw("E1 same");

			}

			ceff->Write();



		}



};

int new_data_mc(){
	
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
	sig->scale(2.1);

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
	TFile *fout0 = new TFile("bdt_response.root","recreate");	
	bdt_response_analysis bdt(vec_files,which_stack);
	bdt.plot_bdt_response(fout0);
	//return 0;

	TFile * fout = new TFile("data_mc_comparason.root","recreate");
	for(auto & v: vars){
		compare_instance c(vec_files, v, which_stack);
		c.plot(fout);
	}

	fout->Close();

	return 0;
}



int data_mc(){
	return 0;//obsolete
}

