#include "bdt_spec.h"


THStack* bdt_stack::getBDTStack(bdt_info whichbdt, std::string binning, int level, double cut1, double cut2){

	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());

	for(int t=0; t<stack.size(); t++){
		bdt_variable var = stack.at(t)->getBDTVariable(whichbdt, binning);

		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "stack_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot);

		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);
		hist->SetFillStyle(stack.at(t)->fillstyle);

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Verticies");

		stacked->Add(hist);
	}

	return stacked;	

}


THStack* bdt_stack::getBDTStack(bdt_info whichbdt, int level, double cut1, double cut2){
	return this->getBDTStack(whichbdt, whichbdt.binning, level, cut1, cut2);
}

TH1* bdt_stack::getBDTSum(bdt_info whichbdt, std::string binning, int level, double cut1, double cut2){

	bdt_variable var = stack.at(0)->getBDTVariable(whichbdt, binning);

	TH1* summed = (TH1*)stack.at(0)->getTH1(var, stack.at(0)->getStageCuts(level,cut1, cut2), "summed_"+stack.at(0)->tag+"_"+var.safe_name, plot_pot);

	for(int t=1; t<stack.size(); t++){
		bdt_variable varo = stack.at(t)->getBDTVariable(whichbdt,binning);
		TH1* hist = (TH1*)stack.at(t)->getTH1(varo, stack.at(t)->getStageCuts(level,cut1, cut2), "summed_"+std::to_string(t)+"_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot);
		summed->Add(hist);
	}

	summed->SetTitle((this->name+"_"+var.name).c_str());
	summed->SetLineColor(kBlack);
	summed->SetStats(0);
	summed->SetLineWidth(1);
	summed->SetFillStyle(3002);
	summed->SetFillColor(kGray+3);

	summed->GetXaxis()->SetTitle(var.unit.c_str());
	summed->GetYaxis()->SetTitle("Verticies");



	return summed;	

}

TH1* bdt_stack::getBDTSum(bdt_info whichbdt, int level, double cut1, double cut2){
	return this->getBDTSum(whichbdt, whichbdt.binning, level, cut1, cut2);

}


int bdt_stack::makeSBNspec(std::string tagin, bdt_variable var, double c1, double c2, std::vector<std::string> hist_names){
	TFile *f = new TFile((tagin+".root").c_str(),"recreate");

	for(int t=0; t<stack.size(); t++){

		TH1D* hist = (TH1D*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(3,c1, c2), hist_names.at(t), plot_pot, 12);
		f->cd();	
		hist->Write();
	}	

	return 0;
}

TH1* bdt_stack::getEntrySum(bdt_variable var){
	return getEntrySum(var, 0);
}


TH1* bdt_stack::getEntrySum(bdt_variable var,int level){
	int stack_rebin = 1;
	if(level ==2) stack_rebin=2;
	if(level ==3) stack_rebin=2;


	TH1* summed = (TH1*)stack.at(0)->getTH1(var, "1", "summed_"+stack.at(0)->tag+"_"+var.safe_name, plot_pot);

	std::cout<<"Summed: "<<summed->GetSumOfWeights()<<std::endl;

	for(int t=1; t<stack.size(); t++){
		TH1* hist = (TH1*)stack.at(t)->getTH1(var, "1", "summed_"+std::to_string(t)+"_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot, stack_rebin);
		summed->Add(hist);
		std::cout<<"Summed: "<<summed->Integral()<<std::endl;
	}

	summed->SetTitle((this->name+"_"+var.name).c_str());
	summed->SetLineColor(kBlack);
	summed->SetFillStyle(3002);
	summed->SetFillColor(kGray+3);
	summed->SetStats(0);
	summed->SetLineWidth(1);

	summed->GetXaxis()->SetTitle(var.unit.c_str());
	summed->GetYaxis()->SetTitle("Verticies");



	return summed;	

}




TH1* bdt_stack::getSum(bdt_variable var, int level, double cut1, double cut2){

	int stack_rebin = 1;
	if(level ==2) stack_rebin=2;
	if(level ==3) stack_rebin=2;


	TH1* summed = (TH1*)stack.at(0)->getTH1(var, stack.at(0)->getStageCuts(level,cut1, cut2), "summed_"+stack.at(0)->tag+"_"+var.safe_name, plot_pot);

	std::cout<<"Summed: "<<summed->GetSumOfWeights()<<std::endl;

	for(int t=1; t<stack.size(); t++){
		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "summed_"+std::to_string(t)+"_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot, stack_rebin);
		summed->Add(hist);
		std::cout<<"Summed: "<<summed->Integral()<<std::endl;
	}

	summed->SetTitle((this->name+"_"+var.name).c_str());
	summed->SetLineColor(kBlack);
	summed->SetFillStyle(3002);
	summed->SetFillColor(kGray+3);
	summed->SetStats(0);
	summed->SetLineWidth(1);

	summed->GetXaxis()->SetTitle(var.unit.c_str());
	summed->GetYaxis()->SetTitle("Verticies");



	return summed;	

}

THStack* bdt_stack::getEntryStack(bdt_variable var){
	return getEntryStack(var, 0);

}
THStack* bdt_stack::getEntryStack(bdt_variable var, int level){
	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());
	int stack_rebin = 1;
	if(level ==2) stack_rebin=2;
	if(level ==3) stack_rebin=2;

	/*

	   to_sort.push_back(&h);
	   l_to_sort.push_back(tmp);
	   integral_sorter.push_back(total_events);	

	   for (int i: sort_indexes(integral_sorter)) {
	   hs->Add(to_sort.at(i));	
	   legStack.AddEntry(to_sort.at(i), l_to_sort.at(i).c_str(),"f");
	   }

*/



	std::vector<TH1*> to_sort;
	std::vector<double> integral_sorter;

	for(int t=0; t<stack.size(); t++){
		std::cout<<"Stack "<<stack.at(t)->tag<<" level "<<t<<std::endl;

		TH1* hist = (TH1*)stack.at(t)->getTH1(var, "1", "stack_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot,stack_rebin);
		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);
		hist->SetFillStyle(stack.at(t)->fillstyle);
		hist->Scale();		

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Verticies");


		to_sort.push_back(hist);
		integral_sorter.push_back(hist->GetSumOfWeights());

		stacked->Add(hist);

		//std::cout<<"HAT: "<<level<<" "<<stack.at(t)->tag<<std::endl;
		//for(int k=1; k< hist->GetNbinsX(); k++){
		//	std::cout<<hist->GetBinContent(k)<<" ";
		//}

	}

	for (int i: sort_indexes(integral_sorter)) {
		//stacked->Add(to_sort.at(i));	
		//legStack.AddEntry(to_sort.at(i), l_to_sort.at(i).c_str(),"f");
	}


	return stacked;	


}



THStack* bdt_stack::getStack(bdt_variable var, int level, double cut1, double cut2){

	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());
	int stack_rebin = 1;
	if(level ==2) stack_rebin=2;
	if(level ==3) stack_rebin=2;

	/*

	   to_sort.push_back(&h);
	   l_to_sort.push_back(tmp);
	   integral_sorter.push_back(total_events);	

	   for (int i: sort_indexes(integral_sorter)) {
	   hs->Add(to_sort.at(i));	
	   legStack.AddEntry(to_sort.at(i), l_to_sort.at(i).c_str(),"f");
	   }

*/



	std::vector<TH1*> to_sort;
	std::vector<double> integral_sorter;

	for(int t=0; t<stack.size(); t++){
		std::cout<<"Stack "<<stack.at(t)->tag<<" level "<<t<<std::endl;

		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "stack_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot,stack_rebin);
		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);
		hist->SetFillStyle(stack.at(t)->fillstyle);
		hist->Scale();		

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Verticies");


		to_sort.push_back(hist);
		integral_sorter.push_back(hist->GetSumOfWeights());

		stacked->Add(hist);

		//std::cout<<"HAT: "<<level<<" "<<stack.at(t)->tag<<std::endl;
		//for(int k=1; k< hist->GetNbinsX(); k++){
		//	std::cout<<hist->GetBinContent(k)<<" ";
		//}

	}

	for (int i: sort_indexes(integral_sorter)) {
		//stacked->Add(to_sort.at(i));	
		//legStack.AddEntry(to_sort.at(i), l_to_sort.at(i).c_str(),"f");
	}


	return stacked;	

}


int bdt_stack::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, double c1, double c2){

	ftest->cd();

	std::vector<std::string> stage_names = {"All verticies","Pre-Selection Cuts","Cosmic BDT Cut","BNB BDT cut"};
	//Loop over all stages
	for(int s = 0; s< 4; s++){
		std::cout<<"On stage: "<<s<<std::endl;
		//First set the files at this stage
		for(auto &f: this->stack){
			std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
			if(s==2) f->calcCosmicBDTEntryList(c1, c2);
			if(s==3) f->calcBNBBDTEntryList(c1, c2);
			std::cout<<"Setting up EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
			f->setStageEntryList(s);

            //Also usable on BNBCosmics
			if(s==3 && false && f->tag == "NCDeltaRadCosmics") f->tvertex->Scan("run_number:subrun_number:event_number:reco_nuvertx:reco_nuverty:reco_nuvertz:reco_track_vertdirx[0]:reco_track_vertdiry[0]:reco_track_vertdirz[0]:reco_shower_startx[0]:reco_shower_starty[0]:reco_shower_startz[0]:reco_shower_dirx[0]:reco_shower_diry[0]:reco_shower_dirz[0]");
		}	
		std::cout<<"Done with computations on TTrees and bdt_stacks"<<std::endl;


		//And all variables in the vector var
		for(auto &var: vars){
			std::cout<<"Starting on variable "<<var.name<<std::endl;
			TCanvas *cobs = new TCanvas(("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(),1800,1600);
			//cobs->Divide(2,2,0.0025,0.0000001);

			THStack *stk = (THStack*)this->getEntryStack(var,s);
			TH1 * tsum = (TH1*)this->getEntrySum(var,s);

			tsum->SetMarkerSize(0);

			cobs->cd();
			stk->Draw("hist");
			stk->SetTitle("");
			stk->GetXaxis()->SetTitle(var.unit.c_str());
			stk->GetYaxis()->SetTitle("Verticies");
			stk->GetYaxis()->SetTitleOffset(1.5);
			stk->SetMaximum(stk->GetMaximum()*1.35);
			TLegend *l3 = new TLegend(0.11,0.72,0.89,0.89);
			tsum->DrawCopy("Same E2"); tsum->SetFillStyle(0);tsum->Draw("hist same");
			for(auto &f: this->stack){
				double Nevents = f->GetEntries()*(plot_pot/f->pot )*f->scale_data;

				cobs->cd(s);
				auto h1 = new TH1F(("tmp_"+std::to_string(s)+"_"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
				h1->SetFillStyle(f->fillstyle);
				h1->SetFillColor(f->col);
				h1->SetLineColor(kBlack);
				l3->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
			}
			l3->Draw();
			l3->SetLineColor(kWhite);
			l3->SetLineWidth(0);
			l3->SetFillStyle(0);
			l3->SetTextSize(0.03);
			l3->SetNColumns(2);

			TLatex latexbdt2;
			latexbdt2.SetTextSize(0.05);
			latexbdt2.SetTextAlign(13);  //align at top
			latexbdt2.SetNDC();
			latexbdt2.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
			TLatex pottenbdt2;
			pottenbdt2.SetTextSize(0.05);
			pottenbdt2.SetTextAlign(13);  //align at top
			pottenbdt2.SetNDC();
			std::string pot_draw_bdt2 = to_string_prec(plot_pot/1e20,1)+"e20 POT";
			pottenbdt2.DrawLatex(.66,.75, pot_draw_bdt2.c_str());

			TText *tbdt2 = drawPrelim(0.11,0.91,0.035,"MicroBooNE Simulation Preliminary");
			tbdt2->Draw();


			TText *titbdt2 = drawPrelim(0.89, 0.91, 0.035, stage_names.at(s).c_str());
			titbdt2->SetTextAlign(30);
			titbdt2->Draw();

			//cobs->Write();
			cobs->SaveAs(("stack/"+this->name+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".pdf").c_str(),"pdf");

			delete cobs;
			delete stk;
			delete tsum;
			delete l3;



		}
	}



	return 0;
}






int bdt_stack::plotStacks(TFile *ftest, bdt_variable var,double c1, double c2){
	TCanvas *cobs = new TCanvas("","",1800,1600);
	cobs->Divide(2,2,0.0025,0.0000001);
	double plot_pot=6.6e20;

	ftest->cd();
	std::cout<<"bdt_stack::plotStacks || Starting to get Stacks"<<std::endl;	
	THStack* s0 = this->getStack(var,0,-9,-9);
	THStack* s1 = this->getStack(var,1,-9,-9);
	THStack* s2 = this->getStack(var,2,c1,-9);
	THStack* s3 = this->getStack(var,3,c1, c2);
	std::cout<<"bdt_stack::plotStacks || Gotten all Stacks"<<std::endl;	
	std::cout<<"bdt_stack::plotStacks || Starting to get Sums"<<std::endl;	

	TH1* t0 = this->getSum(var,0,-9,-9);
	TH1* t1 = this->getSum(var,1,-9,-9);
	TH1* t2 = this->getSum(var,2,c1,-9);
	TH1* t3 = this->getSum(var,3,c1, c2);
	std::cout<<"bdt_stack::plotStacks || Gotten all Sums"<<std::endl;	


	std::cout<<t0<<std::endl;
	t0->SetMarkerSize(0);
	std::cout<<"bdt_stack::plotStacks || TEST"<<std::endl;	
	t1->SetMarkerSize(0);
	t2->SetMarkerSize(0);
	t3->SetMarkerSize(0);

	/*		t0->SetLineWidth(1);
			t1->SetLineWidth(0.075);
			t2->SetLineWidth(0.05);
			t3->SetLineWidth(0.025);
			*/	

	std::cout<<"1"<<std::endl;

	cobs->cd(1);
	s0->Draw("hist");
	s0->SetTitle("");
	s0->GetXaxis()->SetTitle(var.unit.c_str());
	s0->GetYaxis()->SetTitle("Verticies");
	s0->GetYaxis()->SetTitleOffset(1.5);
	s0->SetMaximum(s0->GetMaximum()*1.3);
	TLegend *l0 = new TLegend(0.11,0.72,0.89,0.89);
	l0->SetFillStyle(0);

	std::vector<TH1F*> v0s;		
	t0->DrawCopy("Same E2"); t0->SetFillStyle(0);t0->Draw("hist same");

	for(auto &f: this->stack){
		double Nevents = f->GetEntries( f->getStageCuts(0,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
		cobs->cd(1);
		v0s.push_back( new TH1F(("tmp1"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10));
		v0s.back()->SetFillColor(f->col);
		v0s.back()->SetFillStyle(f->fillstyle);
		v0s.back()->SetLineColor(kBlack);
		l0->AddEntry(v0s.back(),("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
	}
	l0->Draw();
	l0->SetLineColor(kWhite);
	l0->SetTextSize(0.03);
	l0->SetNColumns(2);

	TLatex latexsel;
	latexsel.SetTextSize(0.05);
	latexsel.SetTextAlign(13);  //align at top
	latexsel.SetNDC();
	latexsel.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottensel;
	pottensel.SetTextSize(0.05);
	pottensel.SetTextAlign(13);  //align at top
	pottensel.SetNDC();
	std::string pot_draw_sel = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottensel.DrawLatex(.66,.75, pot_draw_sel.c_str());


	TText *tsel = drawPrelim(0.16,0.91,0.035,"MicroBooNE Simulation Preliminary");
	tsel->Draw();

	TText *titsel = drawPrelim(0.89, 0.91, 0.035, "All Vertices");
	titsel->SetTextAlign(30);
	titsel->Draw();




	std::cout<<"2"<<std::endl;
	cobs->cd(2);
	s1->Draw("hist");
	s1->SetTitle("");
	s1->GetXaxis()->SetTitle(var.unit.c_str());
	s1->GetYaxis()->SetTitle("Verticies");
	s1->GetYaxis()->SetTitleOffset(1.5);
	s1->SetMaximum(s1->GetMaximum()*1.3);
	TLegend *l1 = new TLegend(0.11,0.72,0.89,0.89);
	l1->SetFillStyle(0);
	t1->DrawCopy("Same E2"); t1->SetFillStyle(0);t1->Draw("hist same");

	for(auto &f: this->stack){
		double Nevents = f->GetEntries( f->getStageCuts(1,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
		cobs->cd(2);
		auto h1 = new TH1F(("tmp2"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
		h1->SetFillColor(f->col);
		h1->SetLineColor(kBlack);
		h1->SetFillStyle(f->fillstyle);

		h1->SetFillStyle(f->fillstyle);
		l1->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
	}
	l1->Draw("same");
	l1->SetLineColor(kWhite);
	l1->SetLineWidth(0);
	l1->SetTextSize(0.03);
	l1->SetNColumns(2);


	TLatex latexpre;
	latexpre.SetTextSize(0.05);
	latexpre.SetTextAlign(13);  //align at top
	latexpre.SetNDC();
	latexpre.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottenpre;
	pottenpre.SetTextSize(0.05);
	pottenpre.SetTextAlign(13);  //align at top
	pottenpre.SetNDC();
	std::string pot_draw_pre = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottenpre.DrawLatex(.66,.75, pot_draw_pre.c_str());




	TText *tpre = drawPrelim(0.11,0.91,0.035,"MicroBooNE Simulation Preliminary");
	tpre->Draw();

	TText *titpre = drawPrelim(0.89, 0.91, 0.035, "Pre-Selection Cuts");
	titpre->SetTextAlign(30);
	titpre->Draw();





	std::cout<<"3"<<std::endl;


	cobs->cd(3);
	s2->Draw("hist");
	s2->SetTitle("");
	s2->GetXaxis()->SetTitle(var.unit.c_str());
	s2->GetYaxis()->SetTitle("Verticies");
	s2->GetYaxis()->SetTitleOffset(1.5);
	s2->SetMaximum(s2->GetMaximum()*1.3);
	TLegend *l2 = new TLegend(0.11,0.72,0.89,0.89);
	t2->DrawCopy("Same E2"); t2->SetFillStyle(0);t2->Draw("hist same");

	for(auto &f: this->stack){

		double Nevents = f->GetEntries( f->getStageCuts(2,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
		cobs->cd(3);
		auto h1 = new TH1F(("tmp3"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
		h1->SetFillColor(f->col);
		h1->SetLineColor(kBlack);
		h1->SetFillStyle(f->fillstyle);
		l2->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
	}
	l2->Draw();
	l2->SetLineColor(kWhite);
	l2->SetTextSize(0.03);
	l2->SetNColumns(2);


	TLatex latexbdt1;
	latexbdt1.SetTextSize(0.05);
	latexbdt1.SetTextAlign(13);  //align at top
	latexbdt1.SetNDC();
	latexbdt1.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottenbdt1;
	pottenbdt1.SetTextSize(0.05);
	pottenbdt1.SetTextAlign(13);  //align at top
	pottenbdt1.SetNDC();
	std::string pot_draw_bdt1 = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottenbdt1.DrawLatex(.66,.75, pot_draw_bdt1.c_str());


	TText *tbdt1 = drawPrelim(0.11,0.91,0.035,"MicroBooNE Simulation Preliminary");
	tbdt1->Draw();

	TText *titbdt1 = drawPrelim(0.89, 0.91, 0.035, "Cosmic BDT Cut");
	titbdt1->SetTextAlign(30);
	titbdt1->Draw();







	cobs->cd(4);
	s3->Draw("hist");
	s3->SetTitle("");
	s3->GetXaxis()->SetTitle(var.unit.c_str());
	s3->GetYaxis()->SetTitle("Verticies");
	s3->GetYaxis()->SetTitleOffset(1.5);
	s3->SetMaximum(s3->GetMaximum()*1.35);
	TLegend *l3 = new TLegend(0.11,0.72,0.89,0.89);
	t3->DrawCopy("Same E2"); t3->SetFillStyle(0);t3->Draw("hist same");
	for(auto &f: this->stack){
		double Nevents = f->GetEntries( f->getStageCuts(3,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;

		cobs->cd(4);
		auto h1 = new TH1F(("tmp4"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
		h1->SetFillStyle(f->fillstyle);
		h1->SetFillColor(f->col);
		h1->SetLineColor(kBlack);
		l3->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
	}
	l3->Draw();
	l3->SetLineColor(kWhite);
	l3->SetLineWidth(0);
	l3->SetFillStyle(0);
	l3->SetTextSize(0.03);
	l3->SetNColumns(2);

	TLatex latexbdt2;
	latexbdt2.SetTextSize(0.05);
	latexbdt2.SetTextAlign(13);  //align at top
	latexbdt2.SetNDC();
	latexbdt2.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottenbdt2;
	pottenbdt2.SetTextSize(0.05);
	pottenbdt2.SetTextAlign(13);  //align at top
	pottenbdt2.SetNDC();
	std::string pot_draw_bdt2 = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottenbdt2.DrawLatex(.66,.75, pot_draw_bdt2.c_str());

	TText *tbdt2 = drawPrelim(0.11,0.91,0.035,"MicroBooNE Simulation Preliminary");
	tbdt2->Draw();


	TText *titbdt2 = drawPrelim(0.89, 0.91, 0.035, "BNB BDT Cut");
	titbdt2->SetTextAlign(30);
	titbdt2->Draw();






	std::cout<<"5"<<std::endl;


	cobs->Write();
	cobs->SaveAs(("stack/"+this->name+"_"+var.safe_unit+".pdf").c_str(),"pdf");
	//cobs->SaveAs(("stack/"+var.name+".png").c_str(),"png");


	delete cobs;
	delete s0; delete s1; delete s2; delete s3;		 
	delete t0; delete t1; delete t2; delete t3;	

	return 0;
}





int bdt_stack::plotBDTStacks(TFile *ftest, bdt_info whichbdt,double c1, double c2){
	TCanvas *cobs = new TCanvas("","",1800,1600);
	cobs->Divide(2,2,0.0025,0.0000001);
	double plot_pot=6.6e20;

	ftest->cd();
	THStack* s0 = this->getBDTStack(whichbdt,0,-9,-9);
	THStack* s1 = this->getBDTStack(whichbdt,1,-9,-9);
	THStack* s2 = this->getBDTStack(whichbdt,2,c1,-9);
	THStack* s3 = this->getBDTStack(whichbdt,3,c1, c2);

	TH1* t0 = this->getBDTSum(whichbdt,0,-9,-9);
	TH1* t1 = this->getBDTSum(whichbdt,1,-9,-9);
	TH1* t2 = this->getBDTSum(whichbdt,2,c1,-9);
	TH1* t3 = this->getBDTSum(whichbdt,3,c1, c2);

	t0->SetMarkerSize(0);
	t1->SetMarkerSize(0);
	t2->SetMarkerSize(0);
	t3->SetMarkerSize(0);



	bdt_variable var = this->stack.at(0)->getBDTVariable(whichbdt);


	std::cout<<"4"<<std::endl;
	TPad*p0 = (TPad*)cobs->cd(1);
	p0->SetLogy();
	s0->Draw("hist");
	s0->SetTitle("All Verticies");
	s0->GetXaxis()->SetTitle(var.unit.c_str());
	s0->GetYaxis()->SetTitle("Verticies");
	s0->GetYaxis()->SetTitleOffset(1.5);
	s0->SetMaximum(s0->GetMaximum()*10);
	TLegend *l0 = new TLegend(0.11,0.72,0.89,0.89);

	t0->DrawCopy("Same E2"); t0->SetFillStyle(0);t0->Draw("hist same");
	for(auto &f: this->stack){
		double Nevents = f->GetEntries( f->getStageCuts(0,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
		auto h1 = new TH1F(("tmp1"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
		h1->SetFillColor(f->col);
		h1->SetFillStyle(f->fillstyle);
		h1->SetLineColor(kBlack);
		l0->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");

	}
	l0->Draw();
	l0->SetLineColor(kWhite);
	l0->SetTextSize(0.03);
	l0->SetNColumns(2);

	TLatex latexsel;
	latexsel.SetTextSize(0.05);
	latexsel.SetTextAlign(13);  //align at top
	latexsel.SetNDC();
	latexsel.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottensel;
	pottensel.SetTextSize(0.05);
	pottensel.SetTextAlign(13);  //align at top
	pottensel.SetNDC();
	std::string pot_draw_sel = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottensel.DrawLatex(.66,.75, pot_draw_sel.c_str());






	std::cout<<"1"<<std::endl;
	TPad*p1 = (TPad*)cobs->cd(2);
	p1->SetLogy();	
	s1->Draw("hist");
	s1->SetTitle("Pre-Selection Cuts");
	s1->GetXaxis()->SetTitle(var.unit.c_str());
	s1->GetYaxis()->SetTitle("Verticies");
	s1->GetYaxis()->SetTitleOffset(1.5);
	s1->SetMaximum(s1->GetMaximum()*50);
	s1->SetMinimum(1);
	t1->DrawCopy("Same E2"); t1->SetFillStyle(0);t1->Draw("hist same");
	TLegend *l1 = new TLegend(0.11,0.72,0.89,0.89);
	for(auto &f: this->stack){
		auto h1 = new TH1F(("tmp2"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
		h1->SetFillColor(f->col);
		h1->SetLineColor(kBlack);
		h1->SetFillStyle(f->fillstyle);
		double Nevents = f->GetEntries( f->getStageCuts(1,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
		l1->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
	}
	l1->Draw();
	l1->SetLineColor(kWhite);
	l1->SetTextSize(0.03);
	l1->SetNColumns(2);



	TLatex latexpre;
	latexpre.SetTextSize(0.05);
	latexpre.SetTextAlign(13);  //align at top
	latexpre.SetNDC();
	latexpre.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottenpre;
	pottenpre.SetTextSize(0.05);
	pottenpre.SetTextAlign(13);  //align at top
	pottenpre.SetNDC();
	std::string pot_draw_pre = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottenpre.DrawLatex(.66,.75, pot_draw_pre.c_str());






	std::cout<<"2"<<std::endl;

	TPad*p2 = (TPad*)cobs->cd(3);
	p2->SetLogy();		
	s2->Draw("hist");
	s2->SetTitle("Cosmic BDT cut");
	s2->GetXaxis()->SetTitle(var.unit.c_str());
	s2->GetYaxis()->SetTitle("Verticies");
	s2->GetYaxis()->SetTitleOffset(1.5);
	s2->SetMaximum(s2->GetMaximum()*10);
	TLegend *l2 = new TLegend(0.11,0.72,0.89,0.89);
	t2->DrawCopy("Same E2"); t2->SetFillStyle(0);t2->Draw("hist same");
	for(auto &f: this->stack){

		auto h1 = new TH1F(("tmp3"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
		h1->SetFillColor(f->col);
		h1->SetLineColor(kBlack);		h1->SetFillStyle(f->fillstyle);


		double Nevents = f->GetEntries( f->getStageCuts(2,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
		l2->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
	}
	l2->Draw();
	l2->SetLineColor(kWhite);
	l2->SetTextSize(0.03);
	l2->SetNColumns(2);

	TLatex latexbdt1;
	latexbdt1.SetTextSize(0.05);
	latexbdt1.SetTextAlign(13);  //align at top
	latexbdt1.SetNDC();
	latexbdt1.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottenbdt1;
	pottenbdt1.SetTextSize(0.05);
	pottenbdt1.SetTextAlign(13);  //align at top
	pottenbdt1.SetNDC();
	std::string pot_draw_bdt1 = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottenbdt1.DrawLatex(.66,.75, pot_draw_bdt1.c_str());






	std::cout<<"3"<<std::endl;


	TPad*p3 = (TPad*)cobs->cd(4);
	p3->SetLogy();		

	s3->Draw("hist");
	s3->SetTitle("BNB BDT cut");
	s3->GetXaxis()->SetTitle(var.unit.c_str());
	s3->GetYaxis()->SetTitle("Verticies");
	s3->GetYaxis()->SetTitleOffset(1.5);
	s3->SetMaximum(s3->GetMaximum()*10);
	TLegend *l3 = new TLegend(0.11,0.72,0.89,0.89);
	t3->DrawCopy("Same E2"); t3->SetFillStyle(0);t3->Draw("hist same");
	for(auto &f: this->stack){
		auto h1 = new TH1F(("tmp4"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
		h1->SetFillColor(f->col);
		h1->SetLineColor(kBlack);
		h1->SetFillStyle(f->fillstyle);
		double Nevents = f->GetEntries( f->getStageCuts(3,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
		l3->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
	}
	l3->Draw();
	l3->SetLineColor(kWhite);
	l3->SetTextSize(0.03);
	l3->SetNColumns(2);


	std::cout<<"4"<<std::endl;

	TLatex latexbdt2;
	latexbdt2.SetTextSize(0.05);
	latexbdt2.SetTextAlign(13);  //align at top
	latexbdt2.SetNDC();
	latexbdt2.DrawLatex(.66,.80,this->stack.at(0)->topo_name.c_str());
	TLatex pottenbdt2;
	pottenbdt2.SetTextSize(0.05);
	pottenbdt2.SetTextAlign(13);  //align at top
	pottenbdt2.SetNDC();
	std::string pot_draw_bdt2 = to_string_prec(plot_pot/1e20,1)+"e20 POT";
	pottenbdt2.DrawLatex(.66,.75, pot_draw_bdt2.c_str());






	cobs->Write();
	cobs->SaveAs(("stack/"+this->name+ "_response_"+var.safe_unit+".pdf").c_str(),"pdf");

	return 0;
}
