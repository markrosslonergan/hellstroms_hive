#include "bdt_varplot.h"






int plot_bdt_variables(bdt_file * signal_pure, bdt_file * background_pure, std::vector<bdt_variable> vars, bdt_info input_bdt_info){

	std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

	for(int j=0; j<2;j++){	


		signal_pure->setStageEntryList(j);
		background_pure->setStageEntryList(j);

		for(auto &v: vars){

			//std::string cut_signal = signal_pure->getStageCuts(j,-9,-9); 
			//std::string cut_background_pure = background_pure->getStageCuts(j,-9,-9); 

			//		TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_var" ,1.0);
			//		TH1* bkg = background_pure->getTH1(v,cut_background_pure.c_str(),v.safe_name+"_bkg_var" ,1.0);

			TH1* sig = signal_pure->getTH1(v,"1",v.safe_name+"_sig_var" ,1.0);
			TH1* bkg = background_pure->getTH1(v,"1",v.safe_name+"_bkg_var" ,1.0);

			sig->Scale(1.0/sig->Integral());			
			bkg->Scale(1.0/bkg->Integral());			
			sig->SetLineColor(signal_pure->col);
			bkg->SetLineColor(background_pure->col);
			sig->SetLineWidth(2);
			bkg->SetLineWidth(2);
			TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_"+input_bdt_info.identifier).c_str(), ("cvar_"+v.name+"_"+input_bdt_info.identifier).c_str(),1200,1200);
			c_var->cd();


			sig->SetFillColor(signal_pure->col);
			bkg->SetFillColor(background_pure->col);
			sig->SetFillStyle(3445);
			bkg->SetFillStyle(3454);

			if(j!=1){
				sig->SetTitle(title.at(j).c_str());
			}else{
				sig->SetTitle(" ");
			}

			c_var->cd();			

			sig->Draw("hist");
		sig->SetMinimum(0);
			bkg->Draw("hist same");
			//sig->GetXaxis()->SetTitle(v.unit.c_str());
			sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
			sig->GetYaxis()->SetTitleOffset(1.5);

			TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
			l->SetLineColor(kWhite);
			l->SetFillStyle(0);
			l->SetNColumns(2);

			std::string bkg_legend_name = "Intime Cosmic Background";
			if(background_pure->tag == "BNBPure") bkg_legend_name = "BNB Backgrounds";

			l->AddEntry(sig,"NC #Delta #gamma Signal","lf");	
			l->AddEntry(bkg, bkg_legend_name.c_str(),"lf");	
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

			delete sig;
			delete bkg;
			delete c_var;
		}

	}


	return 0;
}


int plot_bdt_variable(bdt_file * signal_pure, bdt_file * background_pure, bdt_variable v,bdt_info input_bdt_info){

	std::vector<std::string> title = {"All Verticies","Pre-Selection Cuts"};

	std::cout<<"Starting to plot "<<v.name<<std::endl;
	for(int j=0; j<2;j++){	
		
		std::cout<<"On Stage "<<j<<std::endl;
		signal_pure->setStageEntryList(j);
		background_pure->setStageEntryList(j);

		
		std::cout<<"Set Stage Entries Done "<<j<<std::endl;
		

		//std::string cut_signal = signal_pure->getStageCuts(j,-9,-9); 
		//std::string cut_background_pure = background_pure->getStageCuts(j,-9,-9); 

		//		TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_var" ,1.0);
		//		TH1* bkg = background_pure->getTH1(v,cut_background_pure.c_str(),v.safe_name+"_bkg_var" ,1.0);

		TH1* sig = signal_pure->getTH1(v,"1",v.safe_name+"_sig_var" ,1.0);
		TH1* bkg = background_pure->getTH1(v,"1",v.safe_name+"_bkg_var" ,1.0);
	
		std::cout<<"Gotten all TH1's"<<j<<std::endl;
		


		sig->Scale(1.0/sig->Integral());			
		bkg->Scale(1.0/bkg->Integral());			
		sig->SetLineColor(signal_pure->col);
		bkg->SetLineColor(background_pure->col);
		sig->SetLineWidth(2);
		bkg->SetLineWidth(2);
		TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_"+input_bdt_info.identifier).c_str(), ("cvar_"+v.name+"_"+input_bdt_info.identifier).c_str(),1200,1200);
		c_var->cd();


		sig->SetFillColor(signal_pure->col);
		bkg->SetFillColor(background_pure->col);
		sig->SetFillStyle(3445);
		bkg->SetFillStyle(3454);

		if(j!=1){
			sig->SetTitle(title.at(j).c_str());
		}else{
			sig->SetTitle(" ");
		}

		c_var->cd();			

		sig->Draw("hist");
		sig->SetMinimum(0);
		bkg->Draw("hist same");
		//sig->GetXaxis()->SetTitle(v.unit.c_str());
		sig->GetYaxis()->SetTitle("Verticies [Area Normalized]");
		sig->GetYaxis()->SetTitleOffset(1.5);

		TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
		l->SetLineColor(kWhite);
		l->SetFillStyle(0);
		l->SetNColumns(2);

		std::string bkg_legend_name = "Intime Cosmic Background";
		if(background_pure->tag == "BNBPure") bkg_legend_name = "BNB Backgrounds";

		l->AddEntry(sig,"NC #Delta #gamma Signal","lf");	
		l->AddEntry(bkg, bkg_legend_name.c_str(),"lf");	
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

		delete sig;
		delete bkg;
		delete c_var;
	}


	return 0;
}
