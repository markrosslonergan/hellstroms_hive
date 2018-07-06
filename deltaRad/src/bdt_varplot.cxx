#include "bdt_varplot.h"


int plot_bdt_variable(bdt_file * signal_pure, bdt_file * background_pure, bdt_variable v,bdt_info input_bdt_info){

	std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};


	for(int j=0; j<2;j++){	

		std::string cut_signal = signal_pure->getStageCuts(j,-9,-9); 
		std::string cut_background_pure = background_pure->getStageCuts(j,-9,-9); 

		TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_var" ,1.0);
		TH1* bkg = background_pure->getTH1(v,cut_background_pure.c_str(),v.safe_name+"_bkg_var" ,1.0);

		sig->Scale(1.0/sig->Integral());			
		bkg->Scale(1.0/bkg->Integral());			
		sig->SetLineColor(kRed-7);
		bkg->SetLineColor(kGreen-3);
		sig->SetLineWidth(2);
		bkg->SetLineWidth(2);
		TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_cosmo").c_str(), ("cvar_"+v.name+"_cosmo").c_str(),1200,1200);
		c_var->cd();


		sig->SetFillColor(kRed-7);
		bkg->SetFillColor(kGreen-3);
		sig->SetFillStyle(3445);
		bkg->SetFillStyle(3454);

		if(j!=1){
			sig->SetTitle(title.at(j).c_str());
		}else{
			sig->SetTitle(" ");
		}

		c_var->cd();			

		sig->Draw("hist");
		bkg->Draw("hist same");
		//sig->GetXaxis()->SetTitle(v.unit.c_str());
		sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
		sig->GetYaxis()->SetTitleOffset(1.5);

		TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
		l->SetLineColor(kWhite);
		l->SetFillStyle(0);
		l->SetNColumns(2);

		l->AddEntry(sig,"NC #Delta #gamma Signal","lf");	
		l->AddEntry(bkg,"Intime Cosmic Background","lf");	
		l->Draw();

		TText *pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulation Preliminary");
		pre->Draw();

		TLatex latex;
		latex.SetTextSize(0.06);
		latex.SetTextAlign(13);  //align at top
		latex.SetNDC();
		latex.DrawLatex(.7,.71, input_bdt_info.topo_name.c_str());


		double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
		sig->SetMaximum(max_height*1.3);


		c_var->Print(("var/"+input_bdt_info.identifier+"_"+v.safe_unit+"_stage_"+std::to_string(j)+".pdf").c_str(),"pdf");
	}


	return 0;
}
