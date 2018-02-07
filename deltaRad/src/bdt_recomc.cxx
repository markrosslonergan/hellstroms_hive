#include "bdt_recomc.h"


template <typename T>
std::string to_string_prec(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out <<std::fixed<< std::setprecision(n) << a_value;
	return out.str();
}




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

	double N_bdt_cosmic;
	double N_bdt_bnb;

	std::cout<<"Selection: "<<cut.base_cuts<<std::endl;

	//REWRITE THIS USING file->getStageCuts

	//-------------- Selection -------------
	reco_mc_vec_sel = file->getRecoMCTH1(var, file->flow.base_cuts, "sel_"+file->tag+"_"+var.name, plot_pot);
	reco_mc_all_sel = (TH1*)file->getTH1(var, file->flow.base_cuts, "all_sel_"+file->tag+"_"+var.name, plot_pot);


	//-------------- Precuts -------------
	reco_mc_vec_pre = file->getRecoMCTH1(var, file->flow.base_cuts+"&&"+ file->flow.pre_cuts, "pre_"+file->tag+"_"+var.name, plot_pot);
	reco_mc_all_pre = (TH1*)file->getTH1(var , file->flow.base_cuts+"&&"+ file->flow.pre_cuts,"all_pre_"+file->tag+"_"+var.name, plot_pot);

	//-------------- Cosmic BDT -------------
	bdt_variable cosmicvar = file->getBDTVariable("cosmic_track");
	std::string cosmiccut = file->flow.base_cuts+"&&"+ file->flow.pre_cuts+"&&"+ cosmicvar.name + ">" +std::to_string(cut_cosmic_val);

	reco_mc_vec_bdt1 = file->getRecoMCTH1(var, cosmiccut, "bdt1_"+file->tag+"_"+var.name, plot_pot);
	reco_mc_all_bdt1 = (TH1*)file->getTH1(var , cosmiccut  ,"all_bdt1_"+file->tag+"_"+var.name, plot_pot);
	N_bdt_cosmic = file->tvertex->GetEntries(cosmiccut.c_str())*plot_pot/file->pot*file->scale_data;


	//-------------- BNB BDT -------------
	bdt_variable bnbvar = file->getBDTVariable("bnb_track");
	std::string bnbcut = cosmiccut+"&&"+ file->flow.pre_cuts+"&&"+ bnbvar.name + ">" +std::to_string(cut_bnb_val);

	reco_mc_vec_bdt2 = file->getRecoMCTH1(var, bnbcut , "bdt2_"+file->tag+"_"+var.name, plot_pot);
	reco_mc_all_bdt2 = (TH1*)file->getTH1(var , bnbcut  ,"all_bdt2_"+file->tag+"_"+var.name, plot_pot);
	N_bdt_bnb = file->tvertex->GetEntries(bnbcut.c_str())*plot_pot/file->pot*file->scale_data;



	/**********************************************************************
	 *			Reco-Truth Matching section
	 *
	 **********************************************************************/

		fout->cd();

		TCanvas *c_reco_truth = new TCanvas(("recomc_truth_"+var.name+"_"+file->tag).c_str(), ("recomc_truth_"+var.name+"_"+file->tag).c_str(),1600,1600);
		c_reco_truth->Divide(2,2);

		//******************* Selection	*************************
		c_reco_truth->cd(1);

		THStack * s_reco_truth_sel = new THStack("Selection","Selection");		
		TLegend * l_reco_truth_sel = new TLegend(0.51,0.51,0.89,0.89);


		int iv=0;
		for(auto &v: reco_mc_vec_sel){
			std::cout<<"SWtrigger, on hist #: "<<iv<<". Add to stack."<<std::endl;
			s_reco_truth_sel->Add(v);
			std::cout<<"SWtrigger, Calc integral."<<std::endl;
			double n = v->Integral();
			std::cout<<"SWtrigger, and calc percentage."<<std::endl; 
			double per = n/reco_mc_all_sel->Integral()*100.0;
			std::cout<<"SWtrigger, and add legend"<<std::endl;
			l_reco_truth_sel->AddEntry(v   ,(file->recomc_names.at(iv)+" |\t\t "+to_string_prec(n,1)+" \t("+to_string_prec(per,1)+"%)"  ).c_str(),"f");
			iv++;
		}	

		s_reco_truth_sel->Draw("hist");
		l_reco_truth_sel->Draw();

		std::cout<<"Drawn."<<std::endl;

		s_reco_truth_sel->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_sel->GetYaxis()->SetTitle("Verticies");


		//******************* pre	*************************
		c_reco_truth->cd(2);

		THStack * s_reco_truth_pre = new THStack("Precuts","Precuts");		
		TLegend * l_reco_truth_pre = new TLegend(0.51,0.51,0.89,0.89);

		int ipre=0;
		for(auto v: reco_mc_vec_pre){
			s_reco_truth_pre->Add(v);
			double n = v->Integral();
			double per = n/reco_mc_all_pre->Integral()*100.0;
			l_reco_truth_pre->AddEntry(v,(file->recomc_names.at(ipre)+" |\t\t "+to_string_prec(n,1)+" \t("+to_string_prec(per,1)+"%)"  ).c_str(),"f");
			ipre++;
		}	

		s_reco_truth_pre->Draw("hist");
		l_reco_truth_pre->Draw();

		s_reco_truth_pre->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_pre->GetYaxis()->SetTitle("Verticies");


		//******************* cosmic bdt	*************************
		c_reco_truth->cd(3);

		THStack * s_reco_truth_bdt1 = new THStack("Cosmic BDT",("Cosmic BDT: Cut @ "+to_string_prec(cut_cosmic_val,3)+" #: "+to_string_prec(N_bdt_cosmic,3)).c_str());		
		TLegend * l_reco_truth_bdt1 = new TLegend(0.51,0.51,0.89,0.89);

		int ibdt1=0;
		for(auto v: reco_mc_vec_bdt1){
			s_reco_truth_bdt1->Add(v);
			double n = v->Integral();
			double per = n/reco_mc_all_bdt1->Integral()*100.0;
			l_reco_truth_bdt1->AddEntry(v,(file->recomc_names.at(ibdt1)+" |\t\t "+to_string_prec(n,1)+" \t("+to_string_prec(per,1)+"%)"  ).c_str(),"f");
			ibdt1++;
		}	

		s_reco_truth_bdt1->Draw("hist");
		l_reco_truth_bdt1->Draw();

		s_reco_truth_bdt1->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_bdt1->GetYaxis()->SetTitle("Verticies");


		//******************* BNB bdt	*************************
		c_reco_truth->cd(4);

		THStack * s_reco_truth_bdt2 = new THStack("BNB BDT",("BNB: Cut @ "+to_string_prec(cut_bnb_val,3)+ " #: "+to_string_prec(N_bdt_bnb,3)).c_str());		
		TLegend * l_reco_truth_bdt2 = new TLegend(0.51,0.51,0.89,0.89);

		int ibdt2=0;
		for(auto v: reco_mc_vec_bdt2){
			s_reco_truth_bdt2->Add(v);
			double n = v->Integral();
			double per = n/reco_mc_all_bdt2->Integral()*100.0;
			l_reco_truth_bdt2->AddEntry(v,(file->recomc_names.at(ibdt2)+" |\t\t "+to_string_prec(n,1)+" \t("+to_string_prec(per,1)+"%)"  ).c_str(),"f");
			ibdt2++;
		}	

		s_reco_truth_bdt2->Draw("hist");
		l_reco_truth_bdt2->Draw();

		s_reco_truth_bdt2->GetXaxis()->SetTitle(var.unit.c_str());
		s_reco_truth_bdt2->GetYaxis()->SetTitle("Verticies");


		c_reco_truth->Write();
		c_reco_truth->Print(("unit/"+var.name+"_"+file->tag+"_recotruth.pdf").c_str(),"pdf");

	return 0;
}



