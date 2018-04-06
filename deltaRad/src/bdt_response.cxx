#include "bdt_response.h"

int bdt_response::plot_bdt_response(TFile *fout){
    if (!fout) {
        std::cout << "Bad file in BDT response" << std::endl;
        return 1;
    }
	fout->cd();
	
	
	TCanvas *c = new TCanvas(info.identifier.c_str(),info.identifier.c_str(),1600,1600);
	c->Divide(2,2);
	TPad *p1 = (TPad*)c->cd(1);
	TPad *p2 = (TPad*)c->cd(2);

	TLegend *leg = new TLegend(0.13, 0.79, 0.89,0.89);
	leg->SetFillStyle(0);
	leg->SetLineColor(kWhite);
	leg->SetLineWidth(0);
	leg->SetNColumns(2);

	std::vector<bdt_file*> files= {bdt_sig, bdt_bkg};
	std::vector<std::string> which_cuts = {bdt_sig->getStageCuts(0,-9,-9), bdt_bkg->getStageCuts(0,-9,-9)};
//	std::vector<std::string> which_cuts = {"1", bdt_bkg->getStageCuts(0,-9,-9)};
	std::vector<TH1*> h_bdt;
	std::vector<TH1*> h_bdt_log;

	std::cout<<"CUTS: "<<which_cuts.size()<<" "<<which_cuts.at(0)<<" "<<which_cuts.at(1)<<std::endl;
	int i=0;
	for(auto &file: files){
		std::cout<<"On file: "<<file->name<<" || "<<file->tag<<std::endl;

		bdt_variable bdtvar = file->getBDTVariable(info);
		std::string saf = file->tag+"_"+bdtvar.safe_name+"_"+bdt_type;
		std::cout<<"saf: "<<saf<<std::endl;
		fout->cd();
          	std::cout<<"About to get TH1"<<std::endl;
		h_bdt.push_back( file->getTH1(bdtvar, which_cuts.at(i), saf ,1.0));
		h_bdt.back()->SetDirectory(0);	

		std::cout<<"EVENTS: "<<h_bdt.back()->Integral()<<std::endl;
           TH1 * ttmp = file->getTH1(bdtvar, which_cuts.at(i), file->tag+"_"+bdtvar.safe_unit+"_"+bdt_type ,1.0);
		h_bdt.push_back(ttmp);
		h_bdt.back()->SetDirectory(0);	

		c->cd(1);
		h_bdt.back()->SetTitle(info.name.c_str());
		h_bdt.back()->SetLineWidth(2);
		h_bdt.back()->Scale(1.0/h_bdt.back()->Integral() );
		h_bdt.back()->Draw("hist same");
		h_bdt.back()->Write();
		std::cout<<h_bdt.back()->GetSumOfWeights()<<" "<<bdtvar.name<<std::endl;
		h_bdt.back()->GetXaxis()->SetTitle(bdtvar.unit.c_str());
		h_bdt.back()->GetYaxis()->SetTitle("Verticies [Unit Normalized]");
		h_bdt.back()->GetYaxis()->SetTitleOffset(1.5);

		h_bdt.back()->SetFillColor(file->col);
		if(i==0) h_bdt.back()->SetFillStyle(3445);
		if(i==1)h_bdt.back()->SetFillStyle(3454);
		h_bdt.back()->SetMaximum( h_bdt.back()->GetMaximum()*1.3 );


		c->cd(2);
		p2->SetLogy();
		h_bdt_log.push_back( (TH1*)h_bdt.back()->Clone(("log"+file->tag).c_str()));
		h_bdt_log.back()->Draw("hist same");
		h_bdt_log.back()->SetMaximum( h_bdt_log.back()->GetMaximum()*10 );

		i++;
        std::cout << "Done loop with " << file->name << " and " << bdtvar.name << std::endl;
	}
	for(int i=files.size()-1; i>=0; i--){
		leg->AddEntry(h_bdt.at(i),files.at(i)->plot_name.c_str(),"lf");
	}


	std::cout<<"DONE:"<<std::endl;
	c->cd(1);
	leg->Draw();
	c->cd(2);
	leg->Draw();

	fout->cd();
//	c->Write();
//	c->Print(("response/BDT_response_"+info.identifier+".pdf").c_str(),"pdf");
	//c->Print(("response/BDT_response_"+info.identifier+".png").c_str(),"png");
	//return 0;


	std::vector<double> mva;
	std::vector<double> bkg_eff;
	std::vector<double> sig_eff;
	std::vector<double> sig_purity;

	TH1* sig = h_bdt.at(0);
	TH1* bkg = h_bdt.at(1);
	
	double bkgleft = 0.0;
	double sigleft = 0.0;

	double sigint = sig->Integral();
	double bkgint = bkg->Integral();
	
	// simple efficiency calculation
	for(int i=1; i<=sig->GetNbinsX(); i++){
		mva.push_back(sig->GetBinCenter(i));
		bkgleft+=bkg->GetBinContent(i);
		sigleft+=sig->GetBinContent(i);

		bkg_eff.push_back(1.0-bkgleft/bkgint);
		sig_eff.push_back(1.0-sigleft/sigint);
	}


	TPad *p = (TPad*)c->cd(3);
	p->SetLogy();
//	p->SetLogx();
	p->SetGridx();
	p->SetGridy();


	TGraph * g_sig_eff = new TGraph( mva.size(), &mva[0], &sig_eff[0]);
	TGraph * g_bkg_eff = new TGraph( mva.size(), &mva[0], &bkg_eff[0]);
	g_sig_eff->Draw("al");	
	g_bkg_eff->Draw("l");	

	g_sig_eff->SetLineWidth(2);
	g_bkg_eff->SetLineWidth(2);

	g_sig_eff->SetLineColor(files.at(0)->col);
	g_bkg_eff->SetLineColor(files.at(1)->col);

	g_sig_eff->GetXaxis()->SetTitle("BDT Response Cut");
	g_sig_eff->GetYaxis()->SetTitle("Efficiency");
	g_sig_eff->SetTitle("BDT Efficiencies");

	TLegend *leff = new TLegend(0.12,0.12,0.49,0.39);
	leff->AddEntry(g_sig_eff,"Signal Efficiency","l");
	leff->AddEntry(g_bkg_eff,"Background Efficiency","l");
	leff->Draw();


	fout->cd();
	c->Write();
	c->SaveAs(("response/BDT_response_"+info.identifier+".pdf").c_str(),"pdf");
//	c->SaveAs(("response/BDT_response_"+info.identifier+".png").c_str(),"png");

	return 0;
/*



	//These will be better soon
	std::string const swtrigger = "passed_swtrigger ==1";
	std::string const pre_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140";
	std::string const base_selection = "passed_swtrigger == 1 && reco_asso_showers == 1";
	std::string const track_only = "reco_asso_tracks >0";
	std::string const no_track = "reco_asso_tracks == 0";
	std::string const signal_definition = "is_delta_rad == 1 && true_nu_vtx_fid_contained == 1";
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

		std::string cuts = base_selection+"&&"+pre_cut;
		if(file->tag == "NCDelta" || file->tag == "NCDeltaCosmics"){
			cuts = cuts + "&&"+signal_definition;
		}

		//BDT starting plots	
		cbdt->cd(1);
		file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+"bdt_0(40,0.28,0.55)").c_str() ,(cuts +"&& reco_asso_tracks ==0").c_str(),"goff");
		bdt_response_notrack.push_back( (TH1*)gDirectory->Get((file->tag+"bdt_0").c_str() ));
		bdt_response_notrack.back()->Scale(file->scale_data*plot_POT/file->pot);

		cbdt->cd(2);
		file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+ "bdt_1(40,0.28,0.55)").c_str() ,(cuts +"&& reco_asso_tracks >0").c_str(),"goff");
		bdt_response_track.push_back( (TH1*)gDirectory->Get((file->tag+"bdt_1").c_str() ));
		bdt_response_track.back()->Scale(file->scale_data*plot_POT/file->pot);


		double n_verticies_after_precuts_notrack = bdt_response_notrack.back()->GetSumOfWeights();
		double n_verticies_after_precuts_track = bdt_response_track.back()->GetSumOfWeights();

		std::cout<<"RATES: "<<file->tag<<" NoTrack: "<<n_verticies_after_precuts_notrack<<" track: "<<n_verticies_after_precuts_track<<std::endl;

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
			file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+std::to_string(mva_notrack)+ "bdt_0(40,0.28,0.55)").c_str() ,(cuts +"&& reco_asso_tracks == 0" +"&&"+ bdt_cut).c_str(),"goff");
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
			file->tvertex->Draw((file->friend_tree_name+".mva >>"+ file->tag+std::to_string(mva_track)+ "bdt_0(40,0.28,0.55)").c_str() ,(cuts +"&& reco_asso_tracks >0" +"&&"+ bdt_cut).c_str(),"goff");
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

	TLegend *lnotrack = new TLegend(0.5,0.5,0.89,0.89);
	lnotrack->SetLineColor(kWhite);

	THStack *bdt_stack_notrack = new THStack("bdt_notrack_stack","");
	bdt_response_notrack.at(0)->SetLineColor(kBlack);
	bdt_response_notrack.at(0)->SetStats(0);
	bdt_response_notrack.at(0)->SetLineWidth(1);
	bdt_response_notrack.at(0)->SetMarkerStyle(20);
	bdt_response_notrack.at(0)->GetXaxis()->SetTitle("BDT Response");
	bdt_response_notrack.at(0)->GetYaxis()->SetTitle("Verticies");

	for(int t=Nfiles-1; t>= 0; t--){
		std::cout<<"On file: "<<t<<" of notrack "<<std::endl;
		lnotrack->AddEntry(bdt_response_notrack.at(t),files.at(t)->tag.c_str(),files.at(t)->leg.c_str());
		if(which_stack.at(t)==0){
			bdt_response_notrack.at(t)->SetTitle("BDT Response (No Track)");
			bdt_response_notrack.at(t)->SetLineColor(files.at(t)->col);
			bdt_response_notrack.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
			bdt_response_notrack.at(t)->SetMinimum(0.01);
		}else{
			bdt_response_notrack.at(t)->SetFillColor(files.at(t)->col);
			bdt_response_notrack.at(t)->SetLineWidth(1);
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
	lnotrack->Draw();
	TPad*pbdt1 = (TPad*)cbdt->cd(2);
	pbdt1->SetLogy();

	TLegend *ltrack = new TLegend(0.5,0.5,0.89,0.89);
	ltrack->SetLineColor(kWhite);

	THStack *bdt_stack_track = new THStack("bdt_track_stack","");
	bdt_response_track.at(0)->SetLineColor(kBlack);
	bdt_response_track.at(0)->SetStats(0);
	bdt_response_track.at(0)->SetLineWidth(1);
	bdt_response_track.at(0)->SetMarkerStyle(20);
	bdt_response_track.at(0)->GetXaxis()->SetTitle("BDT Response");
	bdt_response_track.at(0)->GetYaxis()->SetTitle("Verticies");



	for(int t=Nfiles-1; t>=0; t--){
		ltrack->AddEntry(bdt_response_track.at(t),files.at(t)->tag.c_str(),files.at(t)->leg.c_str());

		std::cout<<"On file: "<<t<<" of track "<<std::endl;
		if(which_stack.at(t)==0){
			bdt_response_track.at(t)->SetTitle("BDT Response (Track)");
			bdt_response_track.at(t)->SetLineColor(files.at(t)->col);
			bdt_response_track.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
			bdt_response_track.at(t)->SetMinimum(0.01);
		}else{
			bdt_response_track.at(t)->SetFillColor(files.at(t)->col);	
			bdt_response_track.at(t)->SetLineWidth(1);
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

	ltrack->Draw();

	// Now plot TGraphs

	TLegend * l_eff_notrack = new TLegend(0.51,0.11,0.89,0.5);
	l_eff_notrack->SetFillStyle(0);
	TLegend * l_eff_track = new TLegend(0.51,0.11,0.89,0.5);
	l_eff_track->SetFillStyle(0);

	for(int t=0; t< Nfiles; t++){
		std::string plt = "lp";
		if(t==0) plt = "acp";

		cbdt->cd(3)->SetLogy();
		eff_bdt_response_notrack.at(t)->SetLineColor(files.at(t)->col);
		eff_bdt_response_notrack.at(t)->SetLineWidth(1);
		eff_bdt_response_notrack.at(t)->Draw(plt.c_str());
		eff_bdt_response_notrack.at(t)->SetMaximum(2);
		//eff_bdt_response_notrack.at(t)->SetMinimum(0.01);
		eff_bdt_response_notrack.at(t)->SetTitle("BDT Efficiencies, no track");
		eff_bdt_response_notrack.at(t)->GetXaxis()->SetTitle("BDT Response Cut");
		eff_bdt_response_notrack.at(t)->GetYaxis()->SetTitle("Efficiency");


		l_eff_notrack->AddEntry(eff_bdt_response_notrack.at(t), files.at(t)->tag.c_str(),"l");

		cbdt->cd(4)->SetLogy();
		eff_bdt_response_track.at(t)->SetLineWidth(1);
		eff_bdt_response_track.at(t)->SetLineColor(files.at(t)->col);
		eff_bdt_response_track.at(t)->Draw(plt.c_str());
		eff_bdt_response_track.at(t)->SetMaximum(2);
		//eff_bdt_response_track.at(t)->SetMinimum(0.01);
		eff_bdt_response_track.at(t)->SetTitle("BDT Efficiencies, track");
		eff_bdt_response_track.at(t)->GetXaxis()->SetTitle("BDT Response Cut");
		eff_bdt_response_track.at(t)->GetYaxis()->SetTitle("Efficiency");
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
	return 0;



return 0;
*/
}
