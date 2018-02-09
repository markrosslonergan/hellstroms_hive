#include "bdt_spec.h"

THStack* bdt_stack::getBDTStack(std::string whichbdt, int level, double cut1, double cut2){

	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());

	for(int t=0; t<stack.size(); t++){
		bdt_variable var = stack.at(t)->getBDTVariable(whichbdt);

		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "stack_"+stack.at(t)->tag+"_"+var.name, plot_pot);

		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Verticies");

		stacked->Add(hist);
	}

	return stacked;	
		
}


THStack* bdt_stack::getStack(bdt_variable var, int level, double cut1, double cut2){

	THStack *stacked = new THStack((this->name+"_stack").c_str(), (this->name+"_stack").c_str());

	for(int t=0; t<stack.size(); t++){
		TH1* hist = (TH1*)stack.at(t)->getTH1(var, stack.at(t)->getStageCuts(level,cut1, cut2), "stack_"+stack.at(t)->tag+"_"+var.name, plot_pot);

		hist->SetTitle((this->name+"_"+var.name).c_str());
		hist->SetLineColor(kBlack);
		hist->SetStats(0);
		hist->SetLineWidth(1);
		//hist->SetMarkerStyle(20);
		hist->SetFillColor(stack.at(t)->col);

		hist->GetXaxis()->SetTitle(var.unit.c_str());
		hist->GetYaxis()->SetTitle("Verticies");

		stacked->Add(hist);
	}

	return stacked;	
		
}

int bdt_stack::plotStacks(TFile *ftest, bdt_variable var,double c1, double c2){
		TCanvas *cobs = new TCanvas("","",1800,1600);
		cobs->Divide(2,2,0.0025,0.0000001);
		double plot_pot=6.6e20;

		ftest->cd();
		THStack* s0 = this->getStack(var,0,-9,-9);
		THStack* s1 = this->getStack(var,1,-9,-9);
		THStack* s2 = this->getStack(var,2,c1,-9);
		THStack* s3 = this->getStack(var,3,c1, c2);

		cobs->cd(1);
		s0->Draw("hist");
		s0->SetTitle("Selection");
		s0->GetXaxis()->SetTitle(var.unit.c_str());
		s0->GetYaxis()->SetTitle("Verticies");
		s0->GetYaxis()->SetTitleOffset(1.5);
		TLegend *l0 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){
			double Nevents = f->tvertex->GetEntries( f->getStageCuts(0,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			auto h1 = new TH1F(("tmp1"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);
			l0->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l0->Draw();
		l0->SetLineColor(kWhite);
		l0->SetTextSize(0.03);
		
		cobs->cd(2);
		s1->Draw("hist");
		s1->SetTitle("Precuts");
		s1->GetXaxis()->SetTitle(var.unit.c_str());
		s1->GetYaxis()->SetTitle("Verticies");
		s1->GetYaxis()->SetTitleOffset(1.5);
		TLegend *l1 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp2"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->tvertex->GetEntries( f->getStageCuts(1,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l1->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l1->Draw();
		l1->SetLineColor(kWhite);
		l1->SetTextSize(0.03);




		cobs->cd(3);
		s2->Draw("hist");
		s2->SetTitle("Cosmic BDT");
		s2->GetXaxis()->SetTitle(var.unit.c_str());
		s2->GetYaxis()->SetTitle("Verticies");
		s2->GetYaxis()->SetTitleOffset(1.5);
		TLegend *l2 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){

			auto h1 = new TH1F(("tmp3"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);


			double Nevents = f->tvertex->GetEntries( f->getStageCuts(2,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l2->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l2->Draw();
		l2->SetLineColor(kWhite);
		l2->SetTextSize(0.03);




		cobs->cd(4);
		s3->Draw("hist");
		s3->SetTitle("BNB BDT");
		s3->GetXaxis()->SetTitle(var.unit.c_str());
		s3->GetYaxis()->SetTitle("Verticies");
		s3->GetYaxis()->SetTitleOffset(1.5);
		TLegend *l3 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp4"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->tvertex->GetEntries( f->getStageCuts(3,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l3->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l3->Draw();
		l3->SetLineColor(kWhite);
		l3->SetTextSize(0.03);



		

		cobs->Write();
		cobs->SaveAs(("stack/"+var.name+".pdf").c_str(),"pdf");
		cobs->SaveAs(("stack/"+var.name+".png").c_str(),"png");

	return 0;
}





int bdt_stack::plotBDTStacks(TFile *ftest, std::string whichbdt,double c1, double c2){
		TCanvas *cobs = new TCanvas("","",1800,1600);
		cobs->Divide(2,2,0.0025,0.0000001);
		double plot_pot=6.6e20;

		ftest->cd();
		THStack* s0 = this->getBDTStack(whichbdt,0,-9,-9);
		THStack* s1 = this->getBDTStack(whichbdt,1,-9,-9);
		THStack* s2 = this->getBDTStack(whichbdt,2,c1,-9);
		THStack* s3 = this->getBDTStack(whichbdt,3,c1, c2);


		bdt_variable var = this->stack.at(0)->getBDTVariable(whichbdt);


		TPad*p0 = (TPad*)cobs->cd(1);
		p0->SetLogy();
		s0->Draw("hist");
		s0->SetTitle("Selection");
		s0->GetXaxis()->SetTitle(var.unit.c_str());
		s0->GetYaxis()->SetTitle("Verticies");
		s0->GetYaxis()->SetTitleOffset(1.5);
		s0->SetMaximum(s0->GetMaximum()*10);
		TLegend *l0 = new TLegend(0.49,0.49,0.89,0.89);

		for(auto &f: this->stack){
			double Nevents = f->tvertex->GetEntries( f->getStageCuts(0,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			auto h1 = new TH1F(("tmp1"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);
			l0->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
			
		}
		l0->Draw();
		l0->SetLineColor(kWhite);
		l0->SetTextSize(0.03);
	
		TPad*p1 = (TPad*)cobs->cd(2);
		p1->SetLogy();	
		s1->Draw("hist");
		s1->SetTitle("Precuts");
		s1->GetXaxis()->SetTitle(var.unit.c_str());
		s1->GetYaxis()->SetTitle("Verticies");
		s1->GetYaxis()->SetTitleOffset(1.5);
		s1->SetMaximum(s1->GetMaximum()*50);
		s1->SetMinimum(1);
		TLegend *l1 = new TLegend(0.11,0.71,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp2"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->tvertex->GetEntries( f->getStageCuts(1,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l1->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l1->Draw();
		l1->SetLineColor(kWhite);
		l1->SetTextSize(0.03);
		l1->SetNColumns(3);




	
		TPad*p2 = (TPad*)cobs->cd(3);
		p2->SetLogy();		
		s2->Draw("hist");
		s2->SetTitle("Cosmic BDT");
		s2->GetXaxis()->SetTitle(var.unit.c_str());
		s2->GetYaxis()->SetTitle("Verticies");
		s2->GetYaxis()->SetTitleOffset(1.5);
		s2->SetMaximum(s2->GetMaximum()*10);
		TLegend *l2 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){

			auto h1 = new TH1F(("tmp3"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);


			double Nevents = f->tvertex->GetEntries( f->getStageCuts(2,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l2->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l2->Draw();
		l2->SetLineColor(kWhite);
		l2->SetTextSize(0.03);



	
		TPad*p3 = (TPad*)cobs->cd(4);
		p3->SetLogy();		

		s3->Draw("hist");
		s3->SetTitle("BNB BDT");
		s3->GetXaxis()->SetTitle(var.unit.c_str());
		s3->GetYaxis()->SetTitle("Verticies");
		s3->GetYaxis()->SetTitleOffset(1.5);
		s3->SetMaximum(s3->GetMaximum()*10);
		TLegend *l3 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp4"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->tvertex->GetEntries( f->getStageCuts(3,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l3->AddEntry(h1,("#splitline{"+f->tag+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
		}
		l3->Draw();
		l3->SetLineColor(kWhite);
		l3->SetTextSize(0.03);



		

		cobs->Write();
		cobs->SaveAs(("stack/response_"+var.name+".pdf").c_str(),"pdf");

	return 0;
}
