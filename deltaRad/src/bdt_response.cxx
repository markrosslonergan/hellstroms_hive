#include "bdt_response.h"

int bdt_response::plot_bdt_response(TFile *fout){
	if (!fout) {
		std::cout << "Bad file in BDT response" << std::endl;
		return 1;
	}
	fout->cd();

	double title_size_ratio=0.1;
	double label_size_ratio=0.1;
	double title_offset_ratioY = 0.3 ;
	double title_offset_ratioX = 1.1;

	double title_size_upper=0.15;
	double label_size_upper=0.05;
	double title_offset_upper = 1.45;




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

	std::vector<TH1*> h_bdt;
	std::vector<TH1*> h_bdt_log;

	int i=0;

	for(auto &file: files){
		std::cout<<"On file: "<<file->name<<" || "<<file->tag<<std::endl;
		file->setStageEntryList(1);

		bdt_variable bdtvar = file->getBDTVariable(info);
		std::string saf = file->tag+"_"+bdtvar.safe_name+"_"+bdt_type;
		std::cout<<"saf: "<<saf<<std::endl;
		fout->cd();
		std::cout<<"About to get TH1"<<std::endl;

		//std::cout<<"EVENTS: "<<h_bdt.back()->Integral()<<std::endl;
		TH1 * ttmp = (TH1*)file->getTH1(bdtvar, "1", file->tag+"_"+bdtvar.safe_unit+"_"+bdt_type ,1.0);
		h_bdt.push_back(ttmp);
		h_bdt.back()->SetDirectory(0);	

		std::cout << "Variable: " << bdtvar.name << std::endl;
		std::cout << "Sum of weights: " << ttmp->GetSumOfWeights() << std::endl;
		std::cout << "No. entries: " << ttmp->GetEntries() << std::endl;
		c->cd(1);
	
		h_bdt.back()->SetTitle(info.name.c_str());
		h_bdt.back()->SetLineWidth(2);
		std::cout << "Integral: " << h_bdt.back()->Integral() << std::endl;
		h_bdt.back()->Scale(1.0/h_bdt.back()->Integral() );
		std::cout << "Sum of weights after scaling: " << h_bdt.back()->GetSumOfWeights() << std::endl;
		h_bdt.back()->Draw("hist same");
		h_bdt.back()->Write();
		std::cout<<h_bdt.back()->GetSumOfWeights()<<" "<<bdtvar.name<<std::endl;
		h_bdt.back()->GetXaxis()->SetTitle(bdtvar.unit.c_str());
		h_bdt.back()->GetYaxis()->SetTitle("Verticies [Unit Normalized]");
		h_bdt.back()->GetYaxis()->SetTitleOffset(1.25);
		h_bdt.back()->GetYaxis()->SetTitleSize(0.035);

		std::cout<<"On file "<<file->tag<<" is color "<<file->col<<std::endl;
		h_bdt.back()->SetFillColor(file->col);
		if(i==0)h_bdt.back()->SetFillStyle(3445);
		if(i==1)h_bdt.back()->SetFillStyle(3454);
		h_bdt.back()->SetMaximum( h_bdt.back()->GetMaximum()*1.3 );
		//p1->SaveAs(("response/BDT_response_nonlog_"+info.identifier+".pdf").c_str(),"pdf");

		c->cd(2);
		p2->SetLogy();
		h_bdt_log.push_back( (TH1*)h_bdt.back()->Clone(("log"+file->tag).c_str()));
		h_bdt_log.back()->Draw("hist same");
        h_bdt_log.back()->SetMaximum( h_bdt_log.back()->GetMaximum()*7 );
		//p2->SaveAs(("response/BDT_response_log_"+info.identifier+".pdf").c_str(),"pdf");


		i++;
		std::cout << "Done loop with " << file->name << " and " << bdtvar.name << std::endl;

	}
	for(int i=files.size()-1; i>=0; i--){
		leg->AddEntry(h_bdt.at(i), files.at(i)->plot_name.c_str(),"lf");
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

	TH1* sig = (TH1*)h_bdt.at(0)->Clone("sig");
	TH1* bkg = (TH1*)h_bdt.at(1)->Clone("bkg");

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


	

	TCanvas *c_simple = new TCanvas(info.identifier.c_str(),info.identifier.c_str(),1600,1300);
	c_simple->cd();
	TPad *pad0top = new TPad(("pad0top_"), ("pad0top_"), 0, 0.35, 1, 1.0);
	pad0top->SetBottomMargin(0); // Upper and lower plot are joined
	pad0top->Draw();             // Draw the upper pad: pad2top
	pad0top->cd();               // pad2top becomes the current pad
	pad0top->SetLogy();
	
	h_bdt_log.front()->Draw("hist same");
	h_bdt_log.back()->Draw("hist same");
	h_bdt_log.front()->SetMinimum(1.1*1e-4);
	h_bdt_log.front()->SetMinimum(1.1*1e-4);
	leg->Draw();



	c_simple->cd();
	TPad *pad0bot = new TPad(("padbot_"),("padbot_"), 0, 0.05, 1, 0.35);
	pad0bot->SetTopMargin(0);
	pad0bot->SetBottomMargin(0.21);
	pad0bot->SetGridy();
pad0bot->SetGridx(); // vertical grid
	pad0bot->Draw();
	pad0bot->cd();       // pad0bot becomes the current pad
	pad0bot->SetLogy();
	g_sig_eff->Draw("al");
	g_sig_eff->SetTitle("");
	g_sig_eff->GetXaxis()->SetTitle("BDT Response");
	g_sig_eff->SetMaximum(1.0);
	g_sig_eff->SetMinimum(0.0001);
	g_bkg_eff->Draw("l");
	//leff->Draw();

	g_sig_eff->GetXaxis()->SetTitleOffset(title_offset_ratioX);
	g_sig_eff->GetYaxis()->SetTitleOffset(title_offset_ratioY*1.2);
	g_sig_eff->GetYaxis()->SetTitleSize(title_size_ratio);
	g_sig_eff->GetXaxis()->SetTitleSize(title_size_ratio);
	g_sig_eff->GetYaxis()->SetLabelSize(label_size_ratio);
	g_sig_eff->GetXaxis()->SetLabelSize(label_size_ratio);
	
	TLatex latex;
	latex.SetTextSize(0.24);
	latex.SetTextAlign(13);  //align at top
	latex.SetNDC();
	latex.DrawLatex(.15,.433,bdt_sig->topo_name.c_str());

	TText *pre = drawPrelim(0.13,0.86,0.1,"MicroBooNE Simulation Preliminary");
	pre->Draw();



	c_simple->SaveAs(("response/BDT_response_SIMPLE_"+info.identifier+".pdf").c_str(), "pdf");



	return 0;
}
