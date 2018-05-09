#include "bdt_boxcut.h"

int bdt_boxcut::doCuts(TFile *ftest){
	double plot_pot = 6.6e20;
	ftest->cd();

	

	for(int i=0; i< vars.size(); i++){

		
		TCanvas *c = new TCanvas(vars.at(i).safe_name.c_str());
		c->Divide(2,1);
		c->cd(1);
		TLegend *l0 = new TLegend(0.11,0.72,0.89,0.89);
		l0->SetNColumns(2);


		for(auto &f: mc_stack->stack){
			std::string allcuts = this->core_cuts+"&&"+ f->getStageCuts(0,-9,-9);
			for(int i=0; i<boxcuts.size(); i++){
				allcuts+="&&"+ boxcuts.at(i);
			}


			TH1* htmp= (TH1*)f->getTH1(vars.at(i),allcuts, f->tag+"_"+vars.at(i).safe_name, plot_pot );
			std::cout<<f->tag<<" "<<htmp->GetSumOfWeights()<<std::endl;
			c->cd(1);
			htmp->Draw("same hist");
			htmp->SetMaximum( htmp->GetMaximum()*1.4);
			
			l0->AddEntry(htmp,("#splitline{"+f->tag+"}{"+to_string_prec(htmp->Integral(),2)+"}").c_str(),"lp");	
			c->cd(2);	
			TH1* htmp_scale = (TH1*)htmp->Clone(("scale_"+ f->tag+"_"+vars.at(i).safe_name).c_str());	
			htmp_scale->Scale(1.0/htmp_scale->Integral());	
			c->cd(2);
			htmp_scale->Draw("same hist");
		}


		l0->Draw();
		l0->SetLineWidth(0);
		l0->SetLineColor(0);
		l0->SetFillStyle(0);
		l0->SetTextSize(0.03);
		ftest->cd();
		c->Write();
		c->SaveAs(("boxcut/"+tag+"_"+vars.at(i).safe_unit+".pdf").c_str(),"pdf");



	}

	return 0;
}
