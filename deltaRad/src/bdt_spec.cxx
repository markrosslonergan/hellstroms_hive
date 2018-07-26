#include "bdt_spec.h"

THStack* bdt_stack::getBDTStack(bdt_info whichbdt, int level, double cut1, double cut2){

	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());

	for(int t=0; t<stack.size(); t++){
		bdt_variable var = stack.at(t)->getBDTVariable(whichbdt);

		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "stack_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot);

		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);
		hist->SetFillStyle(stack.at(t)->fillstyle);

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Vertices");

		stacked->Add(hist);
	}

	return stacked;	
		
}


TH1* bdt_stack::getBDTSum(bdt_info whichbdt, int level, double cut1, double cut2){
	
	bdt_variable var = stack.at(0)->getBDTVariable(whichbdt);

	TH1* summed = (TH1*)stack.at(0)->getTH1(var, stack.at(0)->getStageCuts(level,cut1, cut2), "summed_"+stack.at(0)->tag+"_"+var.safe_name, plot_pot);

	for(int t=1; t<stack.size(); t++){
		bdt_variable varo = stack.at(t)->getBDTVariable(whichbdt);
		TH1* hist = (TH1*)stack.at(t)->getTH1(varo, stack.at(t)->getStageCuts(level,cut1, cut2), "summed_"+std::to_string(t)+"_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot);
		summed->Add(hist);
	}

		summed->SetTitle((this->name+"_"+var.name).c_str());
		summed->SetLineColor(kBlack);
		summed->SetStats(0);
		summed->SetLineWidth(1);

		summed->GetXaxis()->SetTitle(var.unit.c_str());
		summed->GetYaxis()->SetTitle("Vertices");



	return summed;	
		
}


int bdt_stack::makeSBNspec(std::string tagin, bdt_variable var, double c1, double c2, std::vector<std::string> hist_names){
	TFile *f = new TFile((tagin+".root").c_str(),"recreate");
	
	for(int t=0; t<stack.size(); t++){

		TH1D* hist = (TH1D*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(3,c1, c2), hist_names.at(t), plot_pot, 25);
		f->cd();	
		hist->Write();
	}	

	return 0;
}


TH1* bdt_stack::getSum(bdt_variable var, int level, double cut1, double cut2){


	TH1* summed = (TH1*)stack.at(0)->getTH1(var, stack.at(0)->getStageCuts(level,cut1, cut2), "summed_"+stack.at(0)->tag+"_"+var.safe_name, plot_pot);

	std::cout<<"Summed: "<<summed->GetSumOfWeights()<<std::endl;
	for(int t=1; t<stack.size(); t++){
		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "summed_"+std::to_string(t)+"_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot);
		summed->Add(hist);
		std::cout<<"Summed: "<<summed->Integral()<<std::endl;
	}

		summed->SetTitle((this->name+"_"+var.name).c_str());
		summed->SetLineColor(kBlack);
		summed->SetStats(0);
		summed->SetLineWidth(1);

		summed->GetXaxis()->SetTitle(var.unit.c_str());
		summed->GetYaxis()->SetTitle("Vertices");



	return summed;	
		
}


THStack* bdt_stack::getStack(bdt_variable var, int level, double cut1, double cut2){

	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());
	int stack_rebin = 1;
    std::cout << "GETTING STACK" << std::endl;
	//if(level ==2) stack_rebin=2;
	//if(level ==3) stack_rebin=4;

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
        std::cout << "IN GET STACK LOOP" << std::endl;
        std::cout << "t value: " << t << std::endl;
		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "stack_"+stack.at(t)->tag+"_"+var.safe_name, plot_pot,stack_rebin);
        std::cout << "DEFINED HIST" << std::endl;
		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);
		hist->SetFillStyle(stack.at(t)->fillstyle);
		hist->Scale();		

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Vertices");
		
		
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

    std::cout << "RETURNING STACK" << std::endl;
	return stacked;	
		
}

int bdt_stack::plotStacks(TFile *ftest, bdt_variable var,double c1, double c2){
		TCanvas *cobs = new TCanvas("","",1800,1600);
		cobs->Divide(2,2,0.0025,0.0000001);
		double plot_pot=6.6e20;

        std::cout << "GOT POT" << std::endl;
		ftest->cd();
        std::cout << "CD'ED SUCCESSFULLY" << std::endl;
		THStack* s0 = this->getStack(var,0,-9,-9);
        std::cout << "GOT FIRST STACK " << std::endl;
		THStack* s1 = this->getStack(var,1,-9,-9);
        std::cout << "GOT SECOND STACK " << std::endl;
		THStack* s2 = this->getStack(var,2,c1,-9);
        std::cout << "GOT THIRD STACK" << std::endl;
		THStack* s3 = this->getStack(var,3,c1, c2);
        std::cout << "GOT LAST STACK" << std::endl;

		std::cout<<"1"<<std::endl;

		cobs->cd(1);
		s0->Draw("hist");
		s0->SetTitle("All Vertices");
		s0->GetXaxis()->SetTitle(var.unit.c_str());
		s0->GetYaxis()->SetTitle("Vertices");
		s0->GetYaxis()->SetTitleOffset(1.5);
		s0->SetMaximum(s0->GetMaximum()*1.3);
		TLegend *l0 = new TLegend(0.11,0.72,0.89,0.89);
		std::vector<TH1F*> v0s;		

		for(auto &f: this->stack){
			double Nevents = f->GetEntries( f->getStageCuts(0,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			cobs->cd(1);
			v0s.push_back( new TH1F(("tmp1"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10));
			v0s.back()->SetFillColor(f->col);
			v0s.back()->SetLineColor(kBlack);
			l0->AddEntry(v0s.back(),("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l0->Draw();
		l0->SetLineColor(kWhite);
		l0->SetTextSize(0.03);
		l0->SetNColumns(2);
		
		std::cout<<"2"<<std::endl;
		cobs->cd(2);
		s1->Draw("hist");
		s1->SetTitle("Pre-Selection");
		s1->GetXaxis()->SetTitle(var.unit.c_str());
		s1->GetYaxis()->SetTitle("Vertices");
		s1->GetYaxis()->SetTitleOffset(1.5);
		s1->SetMaximum(s1->GetMaximum()*1.3);
		TLegend *l1 = new TLegend(0.11,0.72,0.89,0.89);
		for(auto &f: this->stack){
			double Nevents = f->GetEntries( f->getStageCuts(1,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			cobs->cd(2);
			auto h1 = new TH1F(("tmp2"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			l1->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l1->Draw("same");
		l1->SetLineColor(kWhite);
		l1->SetTextSize(0.03);
		l1->SetNColumns(2);


		std::cout<<"3"<<std::endl;


		cobs->cd(3);
		s2->Draw("hist");
		s2->SetTitle("Post Cosmic BDT cut");
		s2->GetXaxis()->SetTitle(var.unit.c_str());
		s2->GetYaxis()->SetTitle("Vertices");
		s2->GetYaxis()->SetTitleOffset(1.5);
		s2->SetMaximum(s2->GetMaximum()*1.3);
		TLegend *l2 = new TLegend(0.11,0.72,0.89,0.89);
		for(auto &f: this->stack){

			double Nevents = f->GetEntries( f->getStageCuts(2,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			cobs->cd(3);
			auto h1 = new TH1F(("tmp3"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			l2->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l2->Draw();
		l2->SetLineColor(kWhite);
		l2->SetTextSize(0.03);
		l2->SetNColumns(2);

	

		cobs->cd(4);
		s3->Draw("hist");
		s3->SetTitle("Post BNB BDT cut");
		s3->GetXaxis()->SetTitle(var.unit.c_str());
		s3->GetYaxis()->SetTitle("Vertices");
		s3->GetYaxis()->SetTitleOffset(1.5);
		s3->SetMaximum(s3->GetMaximum()*1.3);
		TLegend *l3 = new TLegend(0.11,0.72,0.89,0.89);
		for(auto &f: this->stack){
			double Nevents = f->GetEntries( f->getStageCuts(3,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;

			cobs->cd(4);
			auto h1 = new TH1F(("tmp4"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);
			l3->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l3->Draw();
		l3->SetLineColor(kWhite);
		l3->SetTextSize(0.03);
		l3->SetNColumns(2);


		std::cout<<"5"<<std::endl;
		

		cobs->Write();
		cobs->SaveAs(("stack/"+this->name+"_"+var.safe_unit+".pdf").c_str(),"pdf");
		//cobs->SaveAs(("stack/"+var.name+".png").c_str(),"png");

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


		bdt_variable var = this->stack.at(0)->getBDTVariable(whichbdt);


		std::cout<<"4"<<std::endl;
		TPad*p0 = (TPad*)cobs->cd(1);
		p0->SetLogy();
		s0->Draw("hist");
		s0->SetTitle("All Vertices");
		s0->GetXaxis()->SetTitle(var.unit.c_str());
		s0->GetYaxis()->SetTitle("Vertices");
		s0->GetYaxis()->SetTitleOffset(1.5);
		s0->SetMaximum(s0->GetMaximum()*10);
		TLegend *l0 = new TLegend(0.11,0.72,0.89,0.89);

		for(auto &f: this->stack){
			double Nevents = f->GetEntries( f->getStageCuts(0,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			auto h1 = new TH1F(("tmp1"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);
			l0->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
			
		}
		l0->Draw();
		l0->SetLineColor(kWhite);
		l0->SetTextSize(0.03);
		l0->SetNColumns(2);

	
		std::cout<<"1"<<std::endl;
		TPad*p1 = (TPad*)cobs->cd(2);
		p1->SetLogy();	
		s1->Draw("hist");
		s1->SetTitle("Pre-Selection");
		s1->GetXaxis()->SetTitle(var.unit.c_str());
		s1->GetYaxis()->SetTitle("Vertices");
		s1->GetYaxis()->SetTitleOffset(1.5);
		s1->SetMaximum(s1->GetMaximum()*50);
		s1->SetMinimum(1);
		TLegend *l1 = new TLegend(0.11,0.72,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp2"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->GetEntries( f->getStageCuts(1,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l1->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l1->Draw();
		l1->SetLineColor(kWhite);
		l1->SetTextSize(0.03);
		l1->SetNColumns(2);




		std::cout<<"2"<<std::endl;
	
		TPad*p2 = (TPad*)cobs->cd(3);
		p2->SetLogy();		
		s2->Draw("hist");
		s2->SetTitle("Post Cosmic BDT cut");
		s2->GetXaxis()->SetTitle(var.unit.c_str());
		s2->GetYaxis()->SetTitle("Vertices");
		s2->GetYaxis()->SetTitleOffset(1.5);
		s2->SetMaximum(s2->GetMaximum()*10);
		TLegend *l2 = new TLegend(0.11,0.72,0.89,0.89);
		for(auto &f: this->stack){

			auto h1 = new TH1F(("tmp3"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);


			double Nevents = f->GetEntries( f->getStageCuts(2,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l2->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l2->Draw();
		l2->SetLineColor(kWhite);
		l2->SetTextSize(0.03);
		l2->SetNColumns(2);


		std::cout<<"3"<<std::endl;

	
		TPad*p3 = (TPad*)cobs->cd(4);
		p3->SetLogy();		

		s3->Draw("hist");
		s3->SetTitle("Post BNB BDT cut");
		s3->GetXaxis()->SetTitle(var.unit.c_str());
		s3->GetYaxis()->SetTitle("Vertices");
		s3->GetYaxis()->SetTitleOffset(1.5);
		s3->SetMaximum(s3->GetMaximum()*10);
		TLegend *l3 = new TLegend(0.11,0.72,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp4"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->GetEntries( f->getStageCuts(3,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l3->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l3->Draw();
		l3->SetLineColor(kWhite);
		l3->SetTextSize(0.03);
		l3->SetNColumns(2);


		std::cout<<"4"<<std::endl;
		

		cobs->Write();
		cobs->SaveAs(("stack/"+this->name+ "_response_"+var.safe_unit+".pdf").c_str(),"pdf");

	return 0;
}
