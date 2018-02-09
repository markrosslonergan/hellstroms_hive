#include "bdt_spec.h"


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
		TCanvas *cobs = new TCanvas("","",4*1200,4*800);
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
		TLegend *l0 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){
			double Nevents = f->tvertex->GetEntries( f->getStageCuts(0,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			auto h1 = new TH1F(("tmp1"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);
			l0->AddEntry(h1,(f->tag+" : "+to_string_prec(Nevents,2) ).c_str(),"f");
		}
		l0->Draw();

		
		cobs->cd(2);
		s1->Draw("hist");
		s1->SetTitle("Precuts");
		s1->GetXaxis()->SetTitle(var.unit.c_str());
		s1->GetYaxis()->SetTitle("Verticies");
		TLegend *l1 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp2"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->tvertex->GetEntries( f->getStageCuts(1,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l1->AddEntry(h1,(f->tag+" : "+to_string_prec(Nevents,2)).c_str(),"f");
		}
		l1->Draw();





		cobs->cd(3);
		s2->Draw("hist");
		s2->SetTitle("Cosmic BDT");
		s2->GetXaxis()->SetTitle(var.unit.c_str());
		s2->GetYaxis()->SetTitle("Verticies");
		TLegend *l2 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){

			auto h1 = new TH1F(("tmp3"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);


			double Nevents = f->tvertex->GetEntries( f->getStageCuts(2,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l2->AddEntry(h1,(f->tag+" : "+to_string_prec(Nevents,2)).c_str(),"f");
		}
		l2->Draw();




		cobs->cd(4);
		s3->Draw("hist");
		s3->SetTitle("BNB BDT");
		s3->GetXaxis()->SetTitle(var.unit.c_str());
		s3->GetYaxis()->SetTitle("Verticies");
		TLegend *l3 = new TLegend(0.49,0.49,0.89,0.89);
		for(auto &f: this->stack){
			auto h1 = new TH1F(("tmp4"+var.name+f->tag).c_str(),"TLegend Example",200,-10,10);
			h1->SetFillColor(f->col);
			h1->SetLineColor(kBlack);

			double Nevents = f->tvertex->GetEntries( f->getStageCuts(3,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
			l3->AddEntry(h1,(f->tag+" : "+to_string_prec(Nevents,2)).c_str(),"f");
		}
		l3->Draw();




		

		cobs->Write();
		cobs->SaveAs(("stack/"+var.name+".pdf").c_str(),"pdf");
		cobs->SaveAs(("stack/"+var.name+".png").c_str(),"png");

	return 0;
}
