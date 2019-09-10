#include "bdt_eff.h"

bdt_efficiency::bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT) : file(filein){

    double conversion = filein->scale_data*plot_POT/filein->pot;
    double n_starting_events = 0;
    denominator = "";

    std::cout<<"File has  "<<filein->GetEntries("1")*conversion<<" events when scaled to "<<plot_POT<<std::endl;


    for(int i=0; i<v_denomin.size();i++){
            std::cout<<" On Cut "<<v_denomin[i]<<std::endl;
            if(i==0){
                denominator = v_denomin[i];
            }else{
                denominator += "&&"+v_denomin[i];
            }
            double tmp_events =  filein->GetEntries(denominator)*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<std::endl;
            n_starting_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR is "<<n_starting_events<<std::endl;

    std::cout<<"-----------------------------------------"<<std::endl;

    std::string topocuts = "";
    double n_topo_events = 0;
    for(int i=0; i<v_topo.size();i++){
            std::cout<<" On topo: "<<v_topo[i]<<std::endl;
            if(i==0){
                topocuts = v_topo[i];
            }else{
                topocuts += "&&"+v_topo[i];
            }
            double tmp_events =  filein->GetEntries(denominator+"&&"+topocuts)*conversion;
            double tmp_events_just_this =  filein->GetEntries(denominator+"&&"+v_topo[i])*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<" which on its own is a ("<<tmp_events_just_this/n_starting_events*100.0<<"%) effect"<<std::endl;
            n_topo_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR + TOPOLOGICAL is "<<n_topo_events<<std::endl;
    std::cout<<"So total Pandora Reco Efficiency is "<<n_topo_events/n_starting_events*100.0<<"%"<<std::endl;
    
    std::cout<<"-----------------------------------------"<<std::endl;

    std::string precuts = "";
    double n_precut_events = 0;
    for(int i=0; i<v_precuts.size();i++){
            std::cout<<" On precut: "<<v_precuts[i]<<std::endl;
            if(i==0){
                precuts = v_precuts[i];
            }else{
                precuts += "&&"+v_precuts[i];
            }
            double tmp_events =  filein->GetEntries(denominator+"&&"+topocuts+"&&"+precuts)*conversion;
            double tmp_events_just_this =  filein->GetEntries(denominator+"&&"+topocuts+"&&"+v_precuts[i])*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<" which on its own is a ("<<tmp_events_just_this/n_topo_events*100.0<<"%) effect relative to topo"<<std::endl;
            n_precut_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR + TOPOLOGICAL + PRECUTS is "<<n_precut_events<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_starting_events*100.0<<"% relative to denom"<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_topo_events*100.0<<"% relative to topo"<<std::endl;
 
    std::cout<<"-----------------------------------------"<<std::endl;
    
    double n_final_events = 0;
	for(int s=2; s<bdt_cuts.size()+2; s++){
		file->calcBDTEntryList(s,bdt_cuts);
		file->setStageEntryList(s);

		double stage_entries = file->GetEntries(denominator+"&&"+topocuts+"&&"+precuts)*conversion;
		std::cout<<"Stage: "<<s<<" "<<stage_entries<<" Efficiency relative to denom: "<<stage_entries/n_starting_events*100.0<<"%"<<std::endl;
		std::cout<<"Stage: "<<s<<" "<<stage_entries<<" Efficiency relative to topo: "<<stage_entries/n_topo_events*100.0<<"%"<<std::endl;
		std::cout<<"Stage: "<<s<<" "<<stage_entries<<" Efficiency relative to precuts: "<<stage_entries/n_precut_events*100.0<<"%"<<std::endl;
    }


}


bdt_efficiency::bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts ,double c1, double c2, double plot_POT) : file(filein){

    double conversion = filein->scale_data*plot_POT/filein->pot;
    double n_starting_events = 0;
    denominator = "";

    std::cout<<"File has  "<<filein->GetEntries("1")*conversion<<" events when scaled to "<<plot_POT<<std::endl;


    for(int i=0; i<v_denomin.size();i++){
            std::cout<<" On Cut "<<v_denomin[i]<<std::endl;
            if(i==0){
                denominator = v_denomin[i];
            }else{
                denominator += "&&"+v_denomin[i];
            }
            double tmp_events =  filein->GetEntries(denominator)*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<std::endl;
            n_starting_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR is "<<n_starting_events<<std::endl;

    std::cout<<"-----------------------------------------"<<std::endl;

    std::string topocuts = "";
    double n_topo_events = 0;
    for(int i=0; i<v_topo.size();i++){
            std::cout<<" On topo: "<<v_topo[i]<<std::endl;
            if(i==0){
                topocuts = v_topo[i];
            }else{
                topocuts += "&&"+v_topo[i];
            }
            double tmp_events =  filein->GetEntries(denominator+"&&"+topocuts)*conversion;
            double tmp_events_just_this =  filein->GetEntries(denominator+"&&"+v_topo[i])*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<" which on its own is a ("<<tmp_events_just_this/n_starting_events*100.0<<"%) effect"<<std::endl;
            n_topo_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR + TOPOLOGICAL is "<<n_topo_events<<std::endl;
    std::cout<<"So total Pandora Reco Efficiency is "<<n_topo_events/n_starting_events*100.0<<"%"<<std::endl;
    
    std::cout<<"-----------------------------------------"<<std::endl;

    std::string precuts = "";
    double n_precut_events = 0;
    for(int i=0; i<v_precuts.size();i++){
            std::cout<<" On precut: "<<v_precuts[i]<<std::endl;
            if(i==0){
                precuts = v_precuts[i];
            }else{
                precuts += "&&"+v_precuts[i];
            }
            double tmp_events =  filein->GetEntries(denominator+"&&"+topocuts+"&&"+precuts)*conversion;
            double tmp_events_just_this =  filein->GetEntries(denominator+"&&"+topocuts+"&&"+v_precuts[i])*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<" which on its own is a ("<<tmp_events_just_this/n_topo_events*100.0<<"%) effect relative to topo"<<std::endl;
            n_precut_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR + TOPOLOGICAL + PRECUTS is "<<n_precut_events<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_starting_events*100.0<<"% relative to denom"<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_topo_events*100.0<<"% relative to topo"<<std::endl;
 
    std::cout<<"-----------------------------------------"<<std::endl;
    
    double n_final_events = 0;
	for(int s=2; s<4; s++){
		if(s==2) file->calcCosmicBDTEntryList(c1, c2);
		if(s==3) file->calcBNBBDTEntryList(c1, c2);
		file->setStageEntryList(s);

		double stage_entries = file->GetEntries(denominator+"&&"+topocuts+"&&"+precuts)*conversion;
		std::cout<<"Stage: "<<s<<" "<<stage_entries<<" Efficiency relative to denom: "<<stage_entries/n_starting_events*100.0<<"%"<<std::endl;
		std::cout<<"Stage: "<<s<<" "<<stage_entries<<" Efficiency relative to topo: "<<stage_entries/n_topo_events*100.0<<"%"<<std::endl;
		std::cout<<"Stage: "<<s<<" "<<stage_entries<<" Efficiency relative to precuts: "<<stage_entries/n_precut_events*100.0<<"%"<<std::endl;
    }


}


bdt_efficiency::bdt_efficiency(bdt_file* filein, std::string denomin, double c1, double c2) : file(filein), denominator(denomin){


	//First step, find event entrylist. In future we must actually track this from the event_tree
	int  event_number=0;
	int  run_number=0;
	int  subrun_number=0;
	double wei=0;
	int pass_denom = 0;

	double weighted_num = 0;
	double base_num = 0;

	file->tvertex->ResetBranchAddresses();

	file->tvertex->SetBranchAddress("event_number",&event_number);
	file->tvertex->SetBranchAddress("bnbcorrection_info.weight",&wei);

	//TPCActive
	//Z: 0 to 1036.8
	//X: 0 to 256.35
	//Y: -116.5 to 116.5

	std::set<int> eventIDs;
	event_entry_list = new TEntryList(file->tvertex);

	for(int k=0; k< file->tvertex->GetEntries();k++){
		file->tvertex->GetEntry(k);
		if(k%10000==0) std::cout<<k<<std::endl;	
		if(eventIDs.count(event_number)==0){
			eventIDs.insert(event_number);
			event_entry_list->Enter(k,file->tvertex);
			weighted_num+=wei;
			base_num++;
		}	
	}

	double MOD =file->scale_data*6.6e20/file->pot;
	double volCryo = 199668.427885;
	double volTPC =  101510.0;
	double volTPCActive=  86698.6;

	file->tvertex->SetEntryList(event_entry_list);


	double Ndenominator = file->GetEntries(denominator.c_str());

	std::cout<<"====================Raw Numbers of Events==================="<<std::endl;
	std::cout<<"1: Number of events in FillLightFiles : "<<file->numberofevents_raw<<std::endl;
	std::cout<<"2: Number of events in FillLightFiles scaled to TPCActive: "<<file->numberofevents_raw*volTPCActive/volTPC<<std::endl;
	std::cout<<"3: Number of unique events in vertexed_files : "<<base_num<<std::endl;
	std::cout<<"4: Number of unique events in vertexed_files with BNB_correction : "<<weighted_num<<std::endl;
	std::cout<<"5: Number of approximated events in FillLightFiles with BNB_correction : "<<file->numberofevents_raw*weighted_num/base_num<<std::endl;
	std::cout<<"6: Same scaled to TPCActive: "<<file->numberofevents_raw*weighted_num/base_num*volTPCActive/volTPC<<std::endl;
	double vertex_eff = (weighted_num)/(file->numberofevents_raw*weighted_num/base_num*volTPCActive/volTPC);
	std::cout<<"7: Ratio of (6) and (4): This is a measure of vertexing efficiency : "<<vertex_eff<<std::endl;

	std::cout<<"==================== Denominator events (Here on x3.1) ==================="<<std::endl;
	std::cout<<"Number of events in vertexed_files that pass denominator with BNB_correction : "<<Ndenominator<<std::endl;
	std::cout<<"Number of events in vertexed_files that pass denominator with BNB_correction with vertex eff removed : "<<Ndenominator/vertex_eff<<std::endl;
	double true_denominator = Ndenominator/vertex_eff*MOD;
	std::cout<<"Same scaled to 6.6e20 "<<true_denominator<<std::endl; 



	double nverticies;
	for(int s=0; s<4; s++){
		if(s==2) file->calcCosmicBDTEntryList(c1, c2);
		if(s==3) file->calcBNBBDTEntryList(c1, c2);
		file->setStageEntryList(s);

		double stage_entries = file->GetEntries("1")*MOD;
		if(s==0) nverticies = stage_entries;
		std::cout<<"Stage: "<<s<<" Verticies: "<<stage_entries<<" Efficiency: "<<stage_entries/true_denominator<<std::endl;
	}

	std::cout<<"==================== Precuts - Individual  ==================="<<std::endl;
	file->setStageEntryList(0);

	for(int m=0; m< file->flow.vec_pre_cuts.size(); m++){
		std::string tmpcut = file->flow.vec_pre_cuts.at(m);
		double np = file->GetEntries(tmpcut.c_str())*MOD;
		std::cout<<" + "<<file->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/nverticies*100<<")\%"<<std::endl;
	}




	std::cout<<"==================== Precuts - One by One  ==================="<<std::endl;
	file->setStageEntryList(0);

	std::string thiscut = "1";
	for(int m=0; m< file->flow.vec_pre_cuts.size(); m++){
		thiscut += "&&"+ file->flow.vec_pre_cuts.at(m);
		double np = file->GetEntries(thiscut.c_str())*MOD;
		std::cout<<" + "<<file->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/nverticies*100<<")\%"<<std::endl;
	}




}



bdt_efficiency::bdt_efficiency(bdt_file* filein, std::string denomin, double c1, double c2,bool in) : file(filein), denominator(denomin){


	TFile * feff = new TFile("eff.1g1p.root","recreate");
	//First step, find event entrylist. In future we must actually track this from the event_tree
	int  event_number=0;
	int  run_number=0;
	int  subrun_number=0;
	double wei=0;
	int pass_denom = 0;

	double weighted_num = 0;
	double base_num = 0;

	file->tvertex->ResetBranchAddresses();

	file->tvertex->SetBranchAddress("event_number",&event_number);
	file->tvertex->SetBranchAddress("bnbcorrection_info.weight",&wei);

	//TPCActive
	//Z: 0 to 1036.8
	//X: 0 to 256.35
	//Y: -116.5 to 116.5

	std::set<int> eventIDs;
	event_entry_list = new TEntryList(file->tvertex);

	for(int k=0; k< file->tvertex->GetEntries();k++){
		file->tvertex->GetEntry(k);
		if(k%10000==0) std::cout<<k<<std::endl;	
		if(eventIDs.count(event_number)==0){
			eventIDs.insert(event_number);
			event_entry_list->Enter(k,file->tvertex);
			weighted_num+=wei;
			base_num++;
		}	
	}

	double MOD =file->scale_data*6.6e20/file->pot;
	double volCryo = 199668.427885;
	double volTPC =  101510.0;
	double volTPCActive=  86698.6;

	file->tvertex->SetEntryList(event_entry_list);


	double Ndenominator = file->GetEntries(denominator.c_str());

	std::cout<<"====================Raw Numbers of Events==================="<<std::endl;
	std::cout<<"1: Number of events in FillLightFiles : "<<file->numberofevents_raw<<std::endl;
	std::cout<<"2: Number of events in FillLightFiles scaled to TPCActive: "<<file->numberofevents_raw*volTPCActive/volTPC<<std::endl;
	std::cout<<"3: Number of unique events in vertexed_files : "<<base_num<<std::endl;
	std::cout<<"4: Number of unique events in vertexed_files with BNB_correction : "<<weighted_num<<std::endl;
	std::cout<<"5: Number of approximated events in FillLightFiles with BNB_correction : "<<file->numberofevents_raw*weighted_num/base_num<<std::endl;
	std::cout<<"6: Same scaled to TPCActive: "<<file->numberofevents_raw*weighted_num/base_num*volTPCActive/volTPC<<std::endl;
	double vertex_eff = (weighted_num)/(file->numberofevents_raw*weighted_num/base_num*volTPCActive/volTPC);
	std::cout<<"7: Ratio of (6) and (4): This is a measure of vertexing efficiency : "<<vertex_eff<<std::endl;

	std::cout<<"==================== Denominator events (Here on x3.1) ==================="<<std::endl;
	std::cout<<"Number of events in vertexed_files that pass denominator with BNB_correction : "<<Ndenominator<<std::endl;
	std::cout<<"Number of events in vertexed_files that pass denominator with BNB_correction with vertex eff removed : "<<Ndenominator/vertex_eff<<std::endl;
	double true_denominator = Ndenominator/vertex_eff*MOD;
	std::cout<<"Same scaled to 6.6e20 "<<true_denominator<<std::endl; 


	bdt_variable true_photon("delta_photon_energy","(20, 0 , 1.0)","True Photon Energy [GeV]",false,"d");
	bdt_variable true_proton("delta_proton_energy-0.938272","(20, 0 , 1.0)","True Proton Kinetic Energy [GeV]",false,"d");

	TH1* h_true_photon_denom = (TH1*)file->getTH1(true_photon, denominator, "photon_true_denom", 6.6e20);
	TH1* h_true_proton_denom = (TH1*)file->getTH1(true_proton, denominator, "proton_true_denom", 6.6e20);

	TH2* h2_true_photon_proton_denom = (TH2*)file->getTH2(true_proton, true_photon, denominator,"2d proton photon denom",6.6e20);

	TH1* h_true_photon_numer;
	TH1* h_true_proton_numer;
	TH2* h2_true_photon_proton_numer;

	double nverticies;
	double finaleff;
	for(int s=0; s<4; s++){
		if(s==2) file->calcCosmicBDTEntryList(c1, c2);
		if(s==3) file->calcBNBBDTEntryList(c1, c2);
		file->setStageEntryList(s);

		double stage_entries = file->GetEntries("1")*MOD;
		if(s==0) nverticies = stage_entries;
		std::cout<<"Stage: "<<s<<" Verticies: "<<stage_entries<<" Efficiency: "<<stage_entries/true_denominator<<std::endl;

		if(s==3){
			std::cout<<"Getting true photon and proton energues"<<std::endl;
			h_true_photon_numer = (TH1*)file->getTH1(true_photon, denominator, "photon_true_numer", 6.6e20);
			h_true_proton_numer = (TH1*)file->getTH1(true_proton, denominator, "proton_true_numer", 6.6e20);
			h2_true_photon_proton_numer = (TH2*)file->getTH2(true_proton, true_photon, denominator,"2d proton photon numer",6.6e20);
			finaleff = stage_entries/true_denominator*100.0;

		}

	}

	h_true_photon_denom->Scale(MOD/vertex_eff);
	h_true_proton_denom->Scale(MOD/vertex_eff);
	h_true_photon_numer->Scale(MOD/vertex_eff);
	h_true_proton_numer->Scale(MOD/vertex_eff);

	h2_true_photon_proton_numer->Scale(MOD/vertex_eff);
	h2_true_photon_proton_denom->Scale(MOD/vertex_eff);
	std::cout<<"Writing"<<std::endl;

	feff->cd();
	h_true_photon_numer->Write();
	h_true_proton_numer->Write();
	h_true_photon_denom->Write();
	h_true_proton_denom->Write();
	h2_true_photon_proton_numer->Write();
	h2_true_photon_proton_denom->Write();


	TH1* h_true_photon_ratio = (TH1*)h_true_photon_numer->Clone("h_true_photon_ratio");
	TH1* h_true_proton_ratio = (TH1*)h_true_proton_numer->Clone("h_true_proton_ratio");
	TH2* h2_true_photon_proton_ratio = (TH2*)h2_true_photon_proton_numer->Clone("h2_true_photon_proton_ratio");

	h_true_photon_ratio->Divide(h_true_photon_denom);
	h_true_proton_ratio->Divide(h_true_proton_denom);
	h2_true_photon_proton_ratio->Divide(h2_true_photon_proton_denom);
	feff->cd();

	h_true_photon_ratio->Write();
	h_true_proton_ratio->Write();
	h2_true_photon_proton_ratio->Write();


	//Std::plotting
	//
	TCanvas * c = new TCanvas();
	c->cd();

	h_true_photon_ratio->Scale(100);
	h_true_proton_ratio->Scale(100);

	h_true_photon_ratio->Draw("E1");

	h_true_photon_denom->Scale(80/h_true_photon_denom->Integral());
	h_true_photon_denom->SetFillStyle(3454);
	h_true_photon_denom->SetFillColor(kRed-6);
	h_true_photon_denom->SetLineColor(kRed-6);
	h_true_photon_denom->Draw("hist same");

	h_true_proton_denom->Scale(80/h_true_proton_denom->Integral());
	h_true_proton_denom->SetFillStyle(3444);
	h_true_proton_denom->SetFillColor(kBlue-6);
	h_true_proton_denom->SetLineColor(kBlue-6);
	h_true_proton_denom->Draw("hist same");

	
	//sig_notrack->Scale(200);
	//sig_notrack->Draw("hist same");



	h_true_photon_ratio->DrawCopy("E1 same");
	h_true_photon_ratio->SetTitle("");
	h_true_proton_ratio->Draw("E1 same");

	h_true_photon_ratio->SetLineColor(kRed);
	h_true_photon_ratio->SetLineWidth(2);
	h_true_photon_ratio->SetMarkerStyle(20);
	h_true_photon_ratio->SetMarkerColor(kRed);

	h_true_proton_ratio->SetLineColor(kBlue);
	h_true_proton_ratio->SetMarkerColor(kBlue);
	h_true_proton_ratio->SetLineWidth(2);
	h_true_proton_ratio->SetMarkerStyle(20);

	h_true_photon_ratio->GetYaxis()->SetTitle("Efficiency [%]");
	h_true_photon_ratio->GetXaxis()->SetTitle("True Photon/Proton Energy [GeV]");

	h_true_photon_ratio->SetMaximum(32);
	h_true_photon_ratio->SetMinimum(0);
	h_true_photon_ratio->GetXaxis()->SetRangeUser(0,1);

	TLegend *l = new TLegend(0.13,0.79,0.89,0.89);
	l->SetLineColor(kWhite);
	l->SetLineWidth(0);
	l->SetNColumns(2);
	l->AddEntry(h_true_proton_ratio,"Efficiency: True Proton Energy","lp");
	l->AddEntry(h_true_photon_ratio,"Efficiency: True Photon Energy","lp");
	l->AddEntry(h_true_proton_denom,"#splitline{}{True Proton Spectum (au)}","fl");
	l->AddEntry(h_true_photon_denom,"#splitline{}{True Photon Spectum (au)}","fl");
	l->Draw();


		TLatex txt;
		txt.SetTextSize(0.05);
		txt.SetTextAlign(13);  //align at top
		txt.SetNDC();
		txt.DrawLatex(.60,.70,("#splitline{Total Efficiency:}{"+to_string_prec(finaleff,2)+"%}").c_str());
		



	c->Write();
	c->SaveAs("eff_a.pdf","pdf");

	TCanvas * c2d = new TCanvas();
	c2d->cd();

	gStyle->SetPalette(kDarkBodyRadiator);	
	h2_true_photon_proton_ratio->Draw("colz");
	h2_true_photon_proton_ratio->GetXaxis()->SetTitle("True Photon Energy [GeV]");
	h2_true_photon_proton_ratio->GetYaxis()->SetTitle("True Proton Kinetic Energy [GeV]");
	
	c2d->SaveAs("eff_b.pdf","pdf");

	


	feff->Close();

	return;



	std::cout<<"==================== Precuts - Individual  ==================="<<std::endl;
	file->setStageEntryList(0);

	for(int m=0; m< file->flow.vec_pre_cuts.size(); m++){
		std::string tmpcut = file->flow.vec_pre_cuts.at(m);
		double np = file->GetEntries(tmpcut.c_str())*MOD;
		std::cout<<" + "<<file->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/nverticies*100<<")\%"<<std::endl;
	}




	std::cout<<"==================== Precuts - One by One  ==================="<<std::endl;
	file->setStageEntryList(0);

	std::string thiscut = "1";
	for(int m=0; m< file->flow.vec_pre_cuts.size(); m++){
		thiscut += "&&"+ file->flow.vec_pre_cuts.at(m);
		double np = file->GetEntries(thiscut.c_str())*MOD;
		std::cout<<" + "<<file->flow.vec_pre_cuts.at(m)<<"\t||\t"<<np<<"\t("<<np/nverticies*100<<")\%"<<std::endl;
	}




}

//One being used right now

bdt_efficiency::bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT,bool is_ok,int plot_stage) : file(filein){


    double conversion = filein->scale_data*plot_POT/filein->pot;
    double n_starting_events = 0;
    denominator = "";

    std::cout<<"File has  "<<filein->GetEntries("1")*conversion<<" events when scaled to "<<plot_POT<<std::endl;


    for(int i=0; i<v_denomin.size();i++){
            std::cout<<" On Cut "<<v_denomin[i]<<std::endl;
            if(i==0){
                denominator = v_denomin[i];
            }else{
                denominator += "&&"+v_denomin[i];
            }
            double tmp_events =  filein->GetEntries(denominator)*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<std::endl;
            n_starting_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR is "<<n_starting_events<<std::endl;

    std::cout<<"-----------------------------------------"<<std::endl;

    std::string topocuts = "";
    double n_topo_events = 0;
    for(int i=0; i<v_topo.size();i++){
            std::cout<<" On topo: "<<v_topo[i]<<std::endl;
            if(i==0){
                topocuts = v_topo[i];
            }else{
                topocuts += "&&"+v_topo[i];
            }
            double tmp_events =  filein->GetEntries(denominator+"&&"+topocuts)*conversion;
            double tmp_events_just_this =  filein->GetEntries(denominator+"&&"+v_topo[i])*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<" which on its own is a ("<<tmp_events_just_this/n_starting_events*100.0<<"%) effect"<<std::endl;
            n_topo_events = tmp_events;
    }
    std::cout<<"So the DENOMINATOR + TOPOLOGICAL is "<<n_topo_events<<std::endl;
    std::cout<<"So total Pandora Reco Efficiency is "<<n_topo_events/n_starting_events*100.0<<"%"<<std::endl;
    
    std::string precuts = "";
    double n_precut_events = 0;
    for(int i=0; i<v_precuts.size();i++){
            std::cout<<" On precut: "<<v_precuts[i]<<std::endl;
            if(i==0){
                precuts = v_precuts[i];
            }else{
                precuts += "&&"+v_precuts[i];
            }
            double tmp_events =  filein->GetEntries(denominator+"&&"+topocuts+"&&"+precuts)*conversion;
            double tmp_events_just_this =  filein->GetEntries(denominator+"&&"+topocuts+"&&"+v_precuts[i])*conversion;
            std::cout<<"--- this file has: "<<tmp_events<<" which on its own is a ("<<tmp_events_just_this/n_topo_events*100.0<<"%) effect relative to topo"<<std::endl;
            n_precut_events = tmp_events;
    }


    std::cout<<"So the DENOMINATOR + TOPOLOGICAL + PRECUTS is "<<n_precut_events<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_starting_events*100.0<<"% relative to denom"<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_topo_events*100.0<<"% relative to topo"<<std::endl;
 


	bdt_variable true_photon("mctruth_exiting_photon_energy","(30, 0 , 0.6)","True Photon Energy [GeV]",false,"d");
	bdt_variable true_proton("Max$(mctruth_exiting_proton_energy)-0.938272","(30, 0 , 0.6)","True Proton Kinetic Energy [GeV]",false,"d");


	TH1* h_true_photon_denom = (TH1*)file->getTH1(true_photon, denominator, "photon_true_denom", 13.2e20);
	TH1* h_true_proton_denom = (TH1*)file->getTH1(true_proton, denominator, "proton_true_denom", 13.2e20);

	TH1* h_true_photon_numer;
	TH1* h_true_proton_numer;



	if(plot_stage>1)file->calcBDTEntryList(plot_stage,bdt_cuts);
   	
    file->calcBaseEntryList("tmp");
    file->setStageEntryList(plot_stage);
    double stage_entries ;
    if(plot_stage>0) stage_entries = file->GetEntries(denominator+"&&"+topocuts+"&&"+precuts)*conversion;
    if(plot_stage==0) stage_entries = file->GetEntries(denominator+"&&"+topocuts)*conversion;

    if(plot_stage==0){
			h_true_photon_numer = (TH1*)file->getTH1(true_photon, denominator+"&&"+topocuts , "photon_true_numer", 13.2e20);
			h_true_proton_numer = (TH1*)file->getTH1(true_proton, denominator+"&&"+topocuts , "proton_true_numer", 13.2e20);
    }else{
			h_true_photon_numer = (TH1*)file->getTH1(true_photon, denominator+"&&"+topocuts +"&&"+precuts, "photon_true_numer", 13.2e20);
			h_true_proton_numer = (TH1*)file->getTH1(true_proton, denominator+"&&"+topocuts +"&&"+precuts, "proton_true_numer", 13.2e20);
    }

	TH1* h_true_photon_ratio = (TH1*)h_true_photon_numer->Clone("h_true_photon_ratio");
	TH1* h_true_proton_ratio = (TH1*)h_true_proton_numer->Clone("h_true_proton_ratio");

	h_true_photon_ratio->Divide(h_true_photon_denom);
	h_true_proton_ratio->Divide(h_true_proton_denom);

    double finaleff = n_topo_events/n_starting_events*100.0;
    if(plot_stage==1)   finaleff = n_precut_events/n_starting_events*100.0;
    if(plot_stage>1)    finaleff = stage_entries/n_starting_events*100.0;
    std::cout<<"Relative to topo: "<<stage_entries/n_topo_events*100.0<<std::endl;
	//Std::plotting
	//
	TCanvas * c = new TCanvas();
	c->cd();

	h_true_photon_ratio->Scale(100);
	h_true_proton_ratio->Scale(100);
    h_true_photon_ratio->SetLineColor(kRed-6);
    h_true_proton_ratio->SetLineColor(kBlue-6);
	h_true_photon_ratio->Draw("E1");

	h_true_photon_denom->Scale(350/h_true_photon_denom->Integral());
	h_true_photon_denom->SetFillStyle(3454);
	h_true_photon_denom->SetFillColor(kRed-6);
	h_true_photon_denom->SetLineColor(kRed-6);
	h_true_photon_denom->Draw("hist same");

	h_true_proton_denom->Scale(350/h_true_proton_denom->Integral());
	h_true_proton_denom->SetFillStyle(3444);
	h_true_proton_denom->SetFillColor(kBlue-6);
	h_true_proton_denom->SetLineColor(kBlue-6);
	h_true_proton_denom->Draw("hist same");

	
	//sig_notrack->Scale(200);
	//sig_notrack->Draw("hist same");

	h_true_photon_ratio->DrawCopy("E1 same");
	h_true_photon_ratio->SetTitle("");
	h_true_proton_ratio->Draw("E1 same");

	h_true_photon_ratio->SetLineColor(kRed);
	h_true_photon_ratio->SetLineWidth(2);
	h_true_photon_ratio->SetMarkerStyle(20);
	h_true_photon_ratio->SetMarkerColor(kRed);

	h_true_proton_ratio->SetLineColor(kBlue);
	h_true_proton_ratio->SetMarkerColor(kBlue);
	h_true_proton_ratio->SetLineWidth(2);
	h_true_proton_ratio->SetMarkerStyle(20);

	h_true_photon_ratio->GetYaxis()->SetTitle("Efficiency [%]");
	h_true_photon_ratio->GetXaxis()->SetTitle("True Photon/Proton Energy [GeV]");

	h_true_photon_ratio->SetMaximum(120.0);
	h_true_photon_ratio->SetMinimum(0);
	h_true_photon_ratio->GetXaxis()->SetRangeUser(0,1);

	TLegend *l = new TLegend(0.13,0.79,0.89,0.89);
	l->SetLineColor(kWhite);
	l->SetLineWidth(0);
	l->SetNColumns(2);
	l->AddEntry(h_true_proton_ratio,"Efficiency: True Proton Energy","lp");
	l->AddEntry(h_true_photon_ratio,"Efficiency: True Photon Energy","lp");
	l->AddEntry(h_true_proton_denom,"#splitline{}{True Proton Spectum (au)}","fl");
	l->AddEntry(h_true_photon_denom,"#splitline{}{True Photon Spectum (au)}","fl");
	l->Draw();


		TLatex txt;
		txt.SetTextSize(0.05);
		txt.SetTextAlign(13);  //align at top
		txt.SetNDC();
		txt.DrawLatex(.60,.70,("#splitline{Total Efficiency:}{"+to_string_prec(finaleff,2)+"%}").c_str());
		



	c->SaveAs(("eff_"+file->tag+"_stage_"+std::to_string(plot_stage)+".pdf").c_str(),"pdf");


	TH2* h2_true_photon_proton_denom = (TH2*)file->getTH2(true_proton, true_photon, denominator,"2d proton photon denom",13.2e20);
	TCanvas * c2d = new TCanvas();
	c2d->cd();

	gStyle->SetPalette(kDarkBodyRadiator);	
	h2_true_photon_proton_denom->Draw("colz");
	h2_true_photon_proton_denom->GetXaxis()->SetTitle("True Photon Energy [GeV]");
	h2_true_photon_proton_denom->GetYaxis()->SetTitle("True Proton Kinetic Energy [GeV]");
	
	c2d->SaveAs("eff_true_2D.pdf","pdf");



}

    bdt_efficiency::bdt_efficiency(std::vector<bdt_file*> vec_files, std::string cut){
    //OK this file will simply make an effciciey curves for all files in vec_files. 

    bdt_variable true_energy("mctruth_nu_E","(50, 0 , 2)","True Neutrino Energy [GeV]",false,"d");

    TCanvas * c = new TCanvas();
	TPad *p1 = (TPad*)c->cd();

	TLegend *l = new TLegend(0.13,0.79,0.89,0.89);
	l->SetLineColor(kWhite);
	l->SetLineWidth(0);
//	l->SetNColumns(2);



    for(size_t f=0; f< vec_files.size(); f++){
        bdt_file * file = vec_files[f];
        double conversion = file->scale_data*13.2e20/file->pot;
        std::string defin_cut = file->flow.definition_cuts;
        double n_starting_events = file->GetEntries(defin_cut)*conversion;

        std::cout<<"File "<<file->tag<<" has  "<<n_starting_events<<" events when scaled to "<<13.2e20<<std::endl;
        c->cd();
    	TH1* h_true_nu_energy = (TH1*)file->getTH1(true_energy, defin_cut , "true_energy_num_"+file->tag, 13.2e20);
        c->cd();
    	TH1* h_true_nu_energy_cut = (TH1*)file->getTH1(true_energy, defin_cut+"&&"+cut , "true_energy_cut_"+file->tag, 13.2e20);
        c->cd();

        h_true_nu_energy_cut->Divide(h_true_nu_energy);
        h_true_nu_energy_cut->Draw("same hist");
        h_true_nu_energy_cut->SetMaximum(1);
        h_true_nu_energy_cut->SetMinimum(0);

	    l->AddEntry(h_true_nu_energy_cut,file->plot_name.c_str() ,"lp");
    }

    l->Draw();
    c->SaveAs("EFFICIENCY_test.pdf","pdf");


}
