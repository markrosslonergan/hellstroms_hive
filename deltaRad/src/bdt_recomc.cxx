#include "bdt_recomc.h"

int bdt_recomc::plot_recomc(TFile *fout, bdt_file* file, bdt_variable var, double cut_cosmic_val, double cut_bnb_val){

	double title_size_ratio=0.1;
	double label_size_ratio=0.1;
	double title_offset_ratioY = 0.3 ;
	double title_offset_ratioX = 1.1;

	double title_size_upper=0.15;
	double label_size_upper=0.05;
	double title_offset_upper = 1.45;

	double plot_pot = 6.6e20;

	//The reco MC vectors (one for each file) currently only bnbcosmic has truth sooo
	std::vector<TH1*> reco_mc_vec_sel;
	std::vector<TH1*> reco_mc_vec_pre;
	std::vector<TH1*> reco_mc_vec_bdt1;
	std::vector<TH1*> reco_mc_vec_bdt2;

	TH1* reco_mc_all_sel;
	TH1* reco_mc_all_pre;
	TH1* reco_mc_all_bdt1;
	TH1* reco_mc_all_bdt2;

	double N_selection;
	double N_precuts;
	double N_bdt_cosmic;
	double N_bdt_bnb;

	//REWRITE THIS USING file->getStageCuts
	file->recomc_cols = recomc_cols;
	file->recomc_cuts = recomc_cuts;
	file->recomc_names = recomc_names;
	
	//-------------- All Verticies -------------
	std::string selection = file->getStageCuts(0, -9, -9);
	
	reco_mc_vec_sel = file->getRecoMCTH1(var, selection, "sel_"+file->tag+"_"+var.safe_name, plot_pot);
	std::cout<<"TEST "<<reco_mc_vec_sel.size()<<std::endl;

	reco_mc_all_sel = (TH1*)file->getTH1(var, selection, "all_sel_"+file->tag+"_"+var.safe_name, plot_pot);
	N_selection = file->GetEntries((selection).c_str())*plot_pot/file->pot*file->scale_data;

	//-------------- Pre-Selection -------------
	std::string precut = file->getStageCuts(1, -9, -9);

	reco_mc_vec_pre = file->getRecoMCTH1(var, precut , "pre_"+file->tag+"_"+var.safe_name, plot_pot);
	reco_mc_all_pre = (TH1*)file->getTH1(var , precut ,"all_pre_"+file->tag+"_"+var.safe_name, plot_pot);
	N_precuts = file->GetEntries((precut).c_str())*plot_pot/file->pot*file->scale_data;

	//-------------- Cosmic BDT ------------- 
	int cosmic_rebin = 1;
	if(do_rebin) cosmic_rebin = 2;

	std::string cosmiccut = file->getStageCuts(2, cut_cosmic_val, -9);
	reco_mc_all_bdt1 = (TH1*)file->getTH1(var , cosmiccut  ,"all_bdt1_"+file->tag+"_"+var.safe_name, plot_pot);
	reco_mc_vec_bdt1 = file->getRecoMCTH1(var, cosmiccut, "bdt1_"+file->tag+"_"+var.safe_name, plot_pot,cosmic_rebin);
	N_bdt_cosmic = file->GetEntries(cosmiccut.c_str())*plot_pot/file->pot*file->scale_data;


	//-------------- BNB BDT -------------
	int bnb_rebin=1;
	if(do_rebin) bnb_rebin=4;

	std::string bnbcut = file->getStageCuts(3, cut_cosmic_val, cut_bnb_val);
	reco_mc_vec_bdt2 = file->getRecoMCTH1(var, bnbcut , "bdt2_"+file->tag+"_"+var.safe_name, plot_pot,bnb_rebin);
	reco_mc_all_bdt2 = (TH1*)file->getTH1(var , bnbcut  ,"all_bdt2_"+file->tag+"_"+var.safe_name, plot_pot);
	N_bdt_bnb = file->GetEntries(bnbcut.c_str())*plot_pot/file->pot*file->scale_data;

	if(false){
		file->tvertex->Scan("run_number:subrun_number:event_number:reco_shower_dedx_plane2[0]:reco_shower_helper_energy[0]:reco_track_displacement[0]",bnbcut.c_str());
	}



	/**********************************************************************
	 *			Reco-Truth Matching section
	 *
	 **********************************************************************/

		fout->cd();

		TCanvas *c_reco_truth = new TCanvas(("recomc_truth_"+var.name+"_"+file->tag).c_str(), ("recomc_truth_"+var.name+"_"+file->tag).c_str(),2000,1600);
		c_reco_truth->Divide(2,2);

		//******************* All Verticies	*************************
		c_reco_truth->cd(1);

   		TPad *padsel = new TPad("padsel", "padsel", 0, 0, 0.7, 1.0);
   		padsel->SetRightMargin(0); // Upper and lower plot are joined
   		padsel->Draw();             // Draw the upper pad: padsel
  		padsel->cd();               // padsel becomes the current pad


		THStack * s_reco_truth_sel = new THStack("All Verticies",("All Verticies Total:"+to_string_prec(N_selection,3)).c_str());		
		TLegend * l_reco_truth_sel = new TLegend(0.11,0.11,0.89,0.89);


		int iv=0;
		for(auto &v: reco_mc_vec_sel){
			std::cout<<"SWtrigger, on hist #: "<<iv<<". Add to stack."<<std::endl;
			s_reco_truth_sel->Add(v);
			std::cout<<"SWtrigger, Calc integral."<<std::endl;
			double n = v->Integral();
			std::cout<<"SWtrigger, and calc percentage."<<std::endl; 
			double per = n/reco_mc_all_sel->Integral()*100.0;
			std::cout<<"SWtrigger, and add legend"<<std::endl;
			l_reco_truth_sel->AddEntry(v,("#splitline{"+ recomc_names.at(iv)+"}{#bf{"+to_string_prec(n,2)+"}     ("+to_string_prec(per,1)+"%)}"  ).c_str(),"f");
			iv++;
		}	

		s_reco_truth_sel->Draw("hist");
		std::cout<<"Drawn."<<std::endl;

		
std::cout<<"1"<<std::endl;
		s_reco_truth_sel->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_sel->GetYaxis()->SetTitle("Verticies");
		s_reco_truth_sel->GetYaxis()->SetTitleOffset(1.5);

		c_reco_truth->cd(1);          // Go back to the main canvas before defining pad2
	        TPad *padsell = new TPad("padsell", "padsell", 0.7, 0, 1, 1);
   		padsell->SetBottomMargin(0.2);
   		padsell->Draw();
  		padsell->cd();       // padsell becomes the current pad
		l_reco_truth_sel->SetTextSize(0.0775);
		l_reco_truth_sel->Draw();
		l_reco_truth_sel->SetLineColor(kWhite);
		l_reco_truth_sel->SetLineWidth(0);
	

std::cout<<"2"<<std::endl;

		//******************* pre	*************************
		c_reco_truth->cd(2);

   		TPad *padpre = new TPad("padpre", "padpre", 0, 0, 0.7, 1.0);
   		padpre->SetRightMargin(0); // Upper and lower plot are joined
   		padpre->Draw();             // Draw the upper pad: padpre
  		padpre->cd();               // padpre becomes the current pad

		THStack * s_reco_truth_pre = new THStack("Pre-Selection",("Pre-Selection Total: "+ to_string_prec(N_precuts,3)).c_str());		
		TLegend * l_reco_truth_pre = new TLegend(0.11,0.11,0.89,0.89);

		int ipre=0;
		for(auto &v: reco_mc_vec_pre){
			s_reco_truth_pre->Add(v);
			double n = v->Integral();
			double per = n/reco_mc_all_pre->Integral()*100.0;
			l_reco_truth_pre->AddEntry(v,("#splitline{"+ recomc_names.at(ipre)+"}{#bf{"+to_string_prec(n,2)+"}     ("+to_string_prec(per,1)+"%)}"  ).c_str(),"f");
			ipre++;
		}	

		s_reco_truth_pre->Draw("hist");
		s_reco_truth_pre->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_pre->GetYaxis()->SetTitle("Verticies");
		s_reco_truth_pre->GetYaxis()->SetTitleOffset(1.5);
	
   		c_reco_truth->cd(2);          // Go back to the main canvas before defining pad2
	        TPad *padprel = new TPad("padprel", "padprel", 0.7, 0, 1, 1);
   		padprel->SetBottomMargin(0.2);
   		padprel->Draw();
  		padprel->cd();       // padprel becomes the current pad
		l_reco_truth_pre->SetTextSize(0.0775);
		l_reco_truth_pre->Draw();
		l_reco_truth_pre->SetLineColor(kWhite);
		l_reco_truth_pre->SetLineWidth(0);
	

std::cout<<"3"<<std::endl;

		//******************* cosmic bdt	*************************
		c_reco_truth->cd(3);

   		TPad *padbdt1 = new TPad("padbdt1", "padbdt1", 0, 0, 0.7, 1.0);
   		padbdt1->SetRightMargin(0); // Upper and lower plot are joined
   		padbdt1->Draw();             // Draw the upper pad: padbdt1
  		padbdt1->cd();               // padbdt1 becomes the current pad


		THStack * s_reco_truth_bdt1 = new THStack("Post Cosmic BDT",("Post Cosmic BDT: Cut @ "+to_string_prec(cut_cosmic_val,2)+" Total: "+to_string_prec(N_bdt_cosmic,3)).c_str());		
		TLegend * l_reco_truth_bdt1 = new TLegend(0.11,0.11,0.89,0.89);

		int ibdt1=0;
		for(auto &v: reco_mc_vec_bdt1){
			s_reco_truth_bdt1->Add(v);
			double n = v->Integral();
			double per = n/reco_mc_all_bdt1->Integral()*100.0;
			l_reco_truth_bdt1->AddEntry(v,("#splitline{"+ recomc_names.at(ibdt1)+"}{#bf{"+to_string_prec(n,2)+"}     ("+to_string_prec(per,1)+"%)}"  ).c_str(),"f");
			ibdt1++;
		}	

		s_reco_truth_bdt1->Draw("hist");

		s_reco_truth_bdt1->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_bdt1->GetYaxis()->SetTitle("Verticies");
		s_reco_truth_bdt1->GetYaxis()->SetTitleOffset(1.5);
	
		c_reco_truth->cd(3);          // Go back to the main canvas before defining pad2
	        TPad *padbdt1l = new TPad("padbdt1l", "padbdt1l", 0.7, 0, 1, 1);
   		padbdt1l->SetBottomMargin(0.2);
   		padbdt1l->Draw();
  		padbdt1l->cd();       // padbdt1l becomes the current pad
		l_reco_truth_bdt1->SetTextSize(0.0775);
		l_reco_truth_bdt1->Draw();
		l_reco_truth_bdt1->SetLineColor(kWhite);
		l_reco_truth_bdt1->SetLineWidth(0);
	



		//******************* BNB bdt	*************************
		c_reco_truth->cd(4);

   		TPad *padbdt2 = new TPad("padbdt2", "padbdt2", 0, 0, 0.7, 1.0);
   		padbdt2->SetRightMargin(0); // Upper and lower plot are joined
   		padbdt2->Draw();             // Draw the upper pad: padbdt2
  		padbdt2->cd();               // padbdt2 becomes the current pad


		THStack * s_reco_truth_bdt2 = new THStack("Post BNB BDT",("Post BNB BDT: Cut @ "+to_string_prec(cut_bnb_val,2)+ " Total: "+to_string_prec(N_bdt_bnb,3)).c_str());		
		TLegend * l_reco_truth_bdt2 = new TLegend(0.11,0.11,0.89,0.89);

		int ibdt2=0;
		for(auto &v: reco_mc_vec_bdt2){
			s_reco_truth_bdt2->Add(v);
			double n = v->Integral();
			double per = n/reco_mc_all_bdt2->Integral()*100.0;
			l_reco_truth_bdt2->AddEntry(v,("#splitline{"+ recomc_names.at(ibdt2)+"}{#bf{"+to_string_prec(n,2)+"}     ("+to_string_prec(per,1)+"%)}"  ).c_str(),"f");
			ibdt2++;
		}	

std::cout<<"5"<<std::endl;
		s_reco_truth_bdt2->Draw("hist");

		s_reco_truth_bdt2->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_bdt2->GetYaxis()->SetTitle("Verticies");
		s_reco_truth_bdt2->GetYaxis()->SetTitleOffset(1.5);

	
		c_reco_truth->cd(4);          // Go back to the main canvas before defining pad2
	        TPad *padbdt2l = new TPad("padbdt2l", "padbdt2l", 0.7, 0, 1, 1);
   		padbdt2l->SetBottomMargin(0.2);
   		padbdt2l->Draw();
  		padbdt2l->cd();       // padbdt2l becomes the current pad
		l_reco_truth_bdt2->SetTextSize(0.0775);
		l_reco_truth_bdt2->Draw();
		l_reco_truth_bdt2->SetLineColor(kWhite);
		l_reco_truth_bdt2->SetLineWidth(0);
	




		c_reco_truth->Write();
		c_reco_truth->Print(("recomc/"+tag+"_"+var.safe_unit+"_"+file->tag+"_recotruth.pdf").c_str(),"pdf");

	return 0;
}



