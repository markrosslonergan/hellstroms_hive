#include "bdt_eff.h"

bdt_efficiency::bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT) : file(filein){

    double conversion = filein->scale_data*plot_POT/filein->pot;
    double n_starting_events = 0;
    denominator = "";

    std::cout << "------------ [EFF] On file " << filein->tag << " -----------------" << std::endl;

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

    std::cout << "------------ [EFF] On file " << filein->tag << " -----------------" << std::endl;

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

bdt_efficiency::bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT,bool is_ok,int plot_stage,std::string tag,bool pretopo, bool is0p) : file(filein){


    double conversion = filein->scale_data*plot_POT/filein->pot;
    double n_starting_events = 0;
    denominator = "";

    std::cout << "------------ [EFF] On file " << filein->tag << " -----------------" << std::endl;

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

    //    double max_x_range = 2.5;
    double max_x_range = 1.0;
    std::cout<<"So the DENOMINATOR + TOPOLOGICAL + PRECUTS is "<<n_precut_events<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_starting_events*100.0<<"% relative to denom"<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_topo_events*100.0<<"% relative to topo"<<std::endl;
    std::cout<<"This is "<<filein->tvertex->GetEntries((denominator+"&&"+topocuts+"&&"+precuts).c_str())<<" actuall MC events"<<std::endl;


    bdt_variable true_photon("mctruth_exiting_photon_energy","(20, 0 ,"+std::to_string(max_x_range)+")","True Photon Energy [GeV]",false,"d");
    TH1* h_true_photon_denom = (TH1*)file->getTH1(true_photon, denominator, "photon_true_denom", 13.2e20);
    TH1* h_true_photon_numer;
    TH1* h_true_proton_numer;
    bdt_variable true_proton("Max$(mctruth_exiting_proton_energy-0.938272)*(Max$(mctruth_exiting_proton_energy-0.938272)>0)","(20, 0 , "+std::to_string(max_x_range)+")","True Proton Kinetic Energy [GeV]",false,"d");
    TH1* h_true_proton_denom ;

    if(is0p == false){ 
        h_true_proton_denom = (TH1*)file->getTH1(true_proton, denominator, "proton_true_denom", 13.2e20);
    }


    if(plot_stage>1)file->calcBDTEntryList(plot_stage,bdt_cuts);

    file->calcBaseEntryList(tag);
    file->setStageEntryList(plot_stage);
    double stage_entries ;
    if(plot_stage>0) stage_entries = file->GetEntries(denominator+"&&"+topocuts+"&&"+precuts)*conversion;
    if(plot_stage==0) stage_entries = file->GetEntries(denominator+"&&"+topocuts)*conversion;
    std::cout<<plot_stage<<" "<<conversion<<" "<<stage_entries<<" topo: "<<topocuts<<std::endl;
    std::cout<<file->GetEntries(denominator)<<" "<<file->GetEntries(topocuts)<<std::endl;

    std::string recotruthmatchingcuts;

    if(is0p == false){
        recotruthmatchingcuts = " sim_shower_overlay_fraction[0] <  0.333 &&  sim_track_overlay_fraction[0] <  0.333 && sim_track_pdg[0] == 2212 && sim_shower_pdg[0] == 22 ";
    }else{
        recotruthmatchingcuts = " sim_shower_overlay_fraction[0] <  0.333 && sim_shower_pdg[0] == 22";

    }
    //    std::string recotruthmatchingcuts = "1";


    if(plot_stage==0){
        h_true_photon_numer = (TH1*)file->getTH1(true_photon, denominator+"&&"+topocuts + "&&" + recotruthmatchingcuts , "photon_true_numer", 10.1e20);
    }else{
        h_true_photon_numer = (TH1*)file->getTH1(true_photon, denominator+"&&"+topocuts +"&&"+precuts + "&&" + recotruthmatchingcuts   , "photon_true_numer", 10.1e20);
    }
    if(is0p == false){
        if(plot_stage==0){
            h_true_proton_numer = (TH1*)file->getTH1(true_proton, denominator+"&&"+topocuts+ "&&" + recotruthmatchingcuts  , "proton_true_numer", 10.1e20);
        }else{
            h_true_proton_numer = (TH1*)file->getTH1(true_proton, denominator+"&&"+topocuts +"&&"+precuts + "&&" + recotruthmatchingcuts    , "proton_true_numer",10.1e20);
        }
    }

    TH1* h_true_proton_ratio;
    if(is0p == false){
        h_true_proton_ratio = (TH1*)h_true_proton_numer->Clone("h_true_proton_ratio");
        h_true_proton_ratio->Divide(h_true_proton_denom);

    }

    TH1* h_true_photon_ratio = (TH1*)h_true_photon_numer->Clone("h_true_photon_ratio");
    h_true_photon_ratio->Divide(h_true_photon_denom);

    //double ratiop_g =  h_true_proton_ratio->Integral()/h_true_photon_ratio->Integral();


    if(is0p==false){    
        std::cout<<"the integrated eff from the photon/proton ratios are: "<<  h_true_photon_ratio->Integral() * 100/h_true_photon_numer->Integral() << "%/" <<  h_true_proton_ratio->Integral() * 100 / h_true_proton_numer->Integral()<< "%"<<std::endl;
        std::cout<<"---- where the integral of the numerator of the photon/protons are: "<<  h_true_photon_numer->Integral() << "/" <<  h_true_proton_numer->Integral()<<std::endl;
        std::cout<<"---- where the integral of denominator of the photon/protons are: "<<  h_true_photon_denom->Integral() << "/" <<  h_true_proton_denom->Integral()<<std::endl;

        std::cout<<"---- and the total eff of photons is: "<<  h_true_photon_numer->Integral()/h_true_photon_denom->Integral()*100.0<<" %"<<std::endl;
        std::cout<<"---- and the total eff of protons is: "<<  h_true_proton_numer->Integral()/h_true_proton_denom->Integral()*100.0<<" %"<<std::endl;
        std::cout<<"---- and the total eff by FILE is: "<<  file->GetEntries(denominator+"&&"+topocuts + "&&" + recotruthmatchingcuts)/file->GetEntries(denominator)*100.0<<" % "<<std::endl;
    }else{
        std::cout<<"the integrated eff from the photon ratio is: "<<  h_true_photon_ratio->Integral() * 100/h_true_photon_numer->Integral() << "%" <<std::endl;

        std::cout<<"---- where the integral of the numerator of the photon is: "<<  h_true_photon_numer->Integral()<<std::endl;
        std::cout<<"---- where the integral of denominator of the photon is : "<<  h_true_photon_denom->Integral() <<std::endl;
        std::cout<<"---- and the total eff of photons is: "<<  h_true_photon_numer->Integral()/h_true_photon_denom->Integral()*100.0<<" %"<<std::endl;
        std::cout<<"---- and the total eff by FILE is: "<<  file->GetEntries(denominator+"&&"+topocuts + "&&" + recotruthmatchingcuts)/file->GetEntries(denominator)*100.0<<" % "<<std::endl;

    }



    //h_true_photon_ratio->Scale(ratiop_g);
    //    h_true_proton_ratio->Scale(100);

    std::cout<<plot_stage<<" "<<conversion<<" "<<stage_entries<<std::endl;

    double finaleff = n_topo_events/n_starting_events*100.0;
    if(plot_stage==1)   finaleff = n_precut_events/n_starting_events*100.0;
    if(plot_stage>1)    finaleff = stage_entries/n_starting_events*100.0;
    std::cout<<"Relative to topo: "<<stage_entries/n_topo_events*100.0<<std::endl;
    //Std::plotting
    //
    TCanvas * c = new TCanvas();
    c->cd();

    h_true_photon_ratio->Scale(100);
    h_true_photon_ratio->SetLineColor(kRed-6);
    h_true_photon_ratio->Draw("E1");

    h_true_photon_denom->Scale(350/h_true_photon_denom->Integral());
    h_true_photon_denom->SetFillStyle(3454);
    h_true_photon_denom->SetFillColor(kRed-6);
    h_true_photon_denom->SetLineColor(kRed-6);
    h_true_photon_denom->Draw("hist same");

    if(is0p == false){
        h_true_proton_ratio->Scale(100);
        h_true_proton_ratio->SetLineColor(kBlue-6);
        h_true_proton_denom->Scale(350/h_true_proton_denom->Integral());
        h_true_proton_denom->SetFillStyle(3444);
        h_true_proton_denom->SetFillColor(kBlue-6);
        h_true_proton_denom->SetLineColor(kBlue-6);
        h_true_proton_denom->Draw("hist same"); 
    }


    //sig_notrack->Scale(200);
    //sig_notrack->Draw("hist same");

    h_true_photon_ratio->DrawCopy("E1 same");
    h_true_photon_ratio->SetTitle("");
    h_true_photon_ratio->SetLineColor(kRed);
    h_true_photon_ratio->SetLineWidth(2);
    h_true_photon_ratio->SetMarkerStyle(20);
    h_true_photon_ratio->SetMarkerColor(kRed);

    if(is0p == false){
        h_true_proton_ratio->Draw("E1 same");
        h_true_proton_ratio->SetLineColor(kBlue);
        h_true_proton_ratio->SetMarkerColor(kBlue);
        h_true_proton_ratio->SetLineWidth(2);
        h_true_proton_ratio->SetMarkerStyle(20);
    }

    h_true_photon_ratio->GetYaxis()->SetTitle("Efficiency [%]");
    h_true_photon_ratio->GetXaxis()->SetTitle("True Photon/Proton Energy [GeV]");

    h_true_photon_ratio->SetMaximum(90.0);
    h_true_photon_ratio->SetMinimum(0);
    h_true_photon_ratio->GetXaxis()->SetRangeUser(0,max_x_range);

    TLegend *l = new TLegend(0.13,0.79,0.89,0.89);
    l->SetLineColor(kWhite);
    l->SetLineWidth(0);
    l->SetNColumns(2);
    l->AddEntry(h_true_photon_ratio,"Efficiency: True Photon Energy","lp");
    l->AddEntry(h_true_photon_denom,"#splitline{}{True Photon Spectum (au)}","fl");

    if(is0p == false){
        l->AddEntry(h_true_proton_ratio,"Efficiency: True Proton Energy","lp");
        l->AddEntry(h_true_proton_denom,"#splitline{}{True Proton Spectum (au)}","fl");

    }
    l->Draw();


    TLatex txt;
    txt.SetTextSize(0.05);
    txt.SetTextAlign(13);  //align at top
    txt.SetNDC();
    txt.DrawLatex(.60,.70,("#splitline{Total Efficiency:}{"+to_string_prec(finaleff,2)+"%}").c_str());




    c->SaveAs(("eff_"+tag+"_"+file->tag+"_stage_"+std::to_string(plot_stage)+".pdf").c_str(),"pdf");

    if(is0p == false){


        TH2* h2_true_photon_proton_denom = (TH2*)file->getTH2(true_proton, true_photon, denominator,"2d proton photon denom",13.2e20);
        TCanvas * c2d = new TCanvas();
        c2d->cd();

        gStyle->SetPalette(kDarkBodyRadiator);	
        h2_true_photon_proton_denom->Draw("colz");
        h2_true_photon_proton_denom->GetXaxis()->SetTitle("True Photon Energy [GeV]");
        h2_true_photon_proton_denom->GetYaxis()->SetTitle("True Proton Kinetic Energy [GeV]");

        c2d->SaveAs("eff_true_2D.pdf","pdf");
    }


}

int nue_efficiency(bdt_file* filein, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT,std::string tag){


    double conversion = filein->scale_data*plot_POT/filein->pot;
    double n_starting_events = 0;
    std::string  denominator = "";

    std::cout << "------------ [EFF] On file " << filein->tag << " -----------------" << std::endl;

    std::cout<<"File has  "<<filein->GetEntries("1")*conversion<<" events when scaled to "<<plot_POT<<std::endl;

//apply topo, and nue def cuts to get denominator
  std::vector<std::string> v_denomin =   {"!mctruth_is_delta_radiative", "mctruth_cc_or_nc == 0", "fabs(mctruth_nu_pdg)==12"};
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
    std::cout<<"So the number of CC nue/nuebar events is "<<n_starting_events<<std::endl;

  
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
    std::cout<<"-----------------------------------------"<<std::endl;

    std::cout<<"So the DENOMINATOR + TOPOLOGICAL is "<<n_topo_events<<std::endl;
     std::cout<<"So the percent that pass topological stage  is "<<n_topo_events/n_starting_events*100.0<<"%"<<std::endl;

 


//then apply precuts, and BDT cuts get passing events
    /*
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

    //    double max_x_range = 2.5;
    double max_x_range = 1.0;
    std::cout<<"So the DENOMINATOR + TOPOLOGICAL + PRECUTS is "<<n_precut_events<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_starting_events*100.0<<"% relative to denom"<<std::endl;
    std::cout<<"So total Precut Efficiency is "<<n_precut_events/n_topo_events*100.0<<"% relative to topo"<<std::endl;
    std::cout<<"This is "<<filein->tvertex->GetEntries((denominator+"&&"+topocuts+"&&"+precuts).c_str())<<" actuall MC events"<<std::endl;
*/
    return 0;
}


//for proton/photon pre-topo
bdt_efficiency::bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT,bool is_ok,int plot_stage,std::string tag, bool pretopo) : file(filein){


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

    bdt_variable true_photon("mctruth_exiting_photon_energy","(30, 0 , 0.6)","True Photon Energy [GeV]",false,"d");
    bdt_variable true_proton("Max$(mctruth_exiting_proton_energy)-0.938272","(30, 0 , 0.6)","True Proton Kinetic Energy [GeV]",false,"d");

    std::cout<<"flag1"<<std::endl;

    TH1* h_true_photon_denom = (TH1*)file->getTH1(true_photon, denominator, "photon_true_denom", 13.2e20);
    TH1* h_true_proton_denom = (TH1*)file->getTH1(true_proton, denominator, "proton_true_denom", 13.2e20);


    TH1* h_true_photon_numer;
    TH1* h_true_proton_numer;


    // if(plot_stage>1)file->calcBDTEntryList(plot_stage,bdt_cuts);


    plot_stage = 0;

    //don't want entrylists since it's pre-topo??
    // file->calcBaseEntryList(tag);
    // std::cout<<"flag1.0"<<std::endl;
    // file->setStageEntryList(plot_stage);

    //std::cout<<"flag1.1"<<std::endl;
    double stage_entries ;
    stage_entries = file->GetEntries(denominator)*conversion;
    // std::cout<<plot_stage<<" "<<conversion<<" "<<stage_entries<<" topo: "<<topocuts<<std::endl;
    //std::cout<<file->GetEntries(denominator)<<" "<<file->GetEntries(topocuts)<<std::endl;

    std::string recotruthmatchingcuts_proton = " reco_vertex_size == 1 && reco_asso_tracks >0 &&sim_track_overlay_fraction[0] <  0.333 && sim_track_pdg[0] == 2212 ";
    std::string recotruthmatchingcuts_photon = "reco_vertex_size == 1 && reco_asso_showers >0 &&sim_shower_overlay_fraction[0] <  0.333 && sim_shower_pdg[0] == 22 ";


    //  std::cout<<"flag2"<<std::endl;

    h_true_photon_numer = (TH1*)file->getTH1(true_photon, denominator +"&&" + recotruthmatchingcuts_photon, "photon_true_numer", 13.2e20);
    h_true_proton_numer = (TH1*)file->getTH1(true_proton, denominator +"&&"+ recotruthmatchingcuts_proton, "proton_true_numer", 13.2e20);

    TH1* h_true_photon_ratio = (TH1*)h_true_photon_numer->Clone("h_true_photon_ratio");
    TH1* h_true_proton_ratio = (TH1*)h_true_proton_numer->Clone("h_true_proton_ratio");


    h_true_photon_ratio->Divide(h_true_photon_denom);
    h_true_proton_ratio->Divide(h_true_proton_denom);

    std::cout<<"the integrated eff from the photon/proton ratios are: "<<  h_true_photon_ratio->GetMaximum() << "/" <<  h_true_proton_ratio->GetMaximum()<< "%"<<std::endl;
    std::cout<<"---- where the integral of the numerator of the photon/protons are: "<<  h_true_photon_numer->Integral() << "/" <<  h_true_proton_numer->Integral()<<std::endl;
    std::cout<<"---- where the integral of denominator of the photon/protons are: "<<  h_true_photon_denom->Integral() << "/" <<  h_true_proton_denom->Integral()<<std::endl;

    std::cout<<"---- and the total eff of photons is: "<<  h_true_photon_numer->Integral()/h_true_photon_denom->Integral()*100.0<<" %"<<std::endl;
    std::cout<<"---- and the total eff of protons is: "<<  h_true_proton_numer->Integral()/h_true_proton_denom->Integral()*100.0<<" %"<<std::endl;





    //double ratiop_g =  h_true_proton_ratio->Integral()/h_true_photon_ratio->Integral();

    std::cout<<plot_stage<<" "<<conversion<<" "<<stage_entries<<std::endl;

    /*    double finaleff = n_topo_events/n_starting_events*100.0;
          if(plot_stage==1)   finaleff = n_precut_events/n_starting_events*100.0;
          if(plot_stage>1)    finaleff = stage_entries/n_starting_events*100.0;
          std::cout<<"Relative to topo: "<<stage_entries/n_topo_events*100.0<<std::endl;
    //Std::plotting
    //
    */


    TCanvas * c1 = new TCanvas();
    c1->cd();

    h_true_photon_ratio->Scale(100);
    h_true_photon_ratio->SetLineColor(kRed-6);
    h_true_photon_ratio->Draw("E1");

    h_true_photon_denom->Scale(350/h_true_photon_denom->Integral());
    h_true_photon_denom->SetFillStyle(3454);
    h_true_photon_denom->SetFillColor(kRed-6);
    h_true_photon_denom->SetLineColor(kRed-6);
    h_true_photon_denom->Draw("hist same");

    //sig_notrack->Scale(200);
    //sig_notrack->Draw("hist same");

    //  h_true_photon_ratio->DrawCopy("E1 same");


    h_true_photon_ratio->SetLineColor(kRed);
    h_true_photon_ratio->SetLineWidth(2);
    h_true_photon_ratio->SetMarkerStyle(20);
    h_true_photon_ratio->SetMarkerColor(kRed);

    h_true_photon_ratio->GetYaxis()->SetTitle("Efficiency [%]");
    h_true_photon_ratio->GetXaxis()->SetTitle("True Photon Energy [GeV]");

    h_true_photon_ratio->SetMaximum(100.0);
    h_true_photon_ratio->SetMinimum(0);
    h_true_photon_ratio->GetXaxis()->SetRangeUser(0,1);
    h_true_photon_ratio->SetTitle("");

    h_true_photon_ratio->DrawCopy("E1 same");


    TLegend *l = new TLegend(0.13,0.79,0.89,0.89);
    l->SetLineColor(kWhite);
    l->SetLineWidth(0);
    l->SetNColumns(2);
    l->AddEntry(h_true_photon_ratio,"Efficiency: True Photon Energy","lp");
    l->AddEntry(h_true_photon_denom,"#splitline{}{True Photon Spectum (au)}","fl");
    l->Draw();

    /*
       TLatex txt;
       txt.SetTextSize(0.05);
       txt.SetTextAlign(13);  //align at top
       txt.SetNDC();
       txt.DrawLatex(.60,.70,("#splitline{Total Efficiency:}{"+to_string_prec(finaleff,2)+"%}").c_str());


*/

    c1->SaveAs(("eff_"+tag+"_"+file->tag+"_pretopo_photon.pdf").c_str(),"pdf");

    TCanvas * c2 = new TCanvas();
    c2->cd();

    h_true_proton_ratio->Scale(100);
    h_true_proton_ratio->SetLineColor(kBlue-6);
    h_true_proton_ratio->Draw("E1");


    h_true_proton_denom->Scale(350/h_true_proton_denom->Integral());
    h_true_proton_denom->SetFillStyle(3444);
    h_true_proton_denom->SetFillColor(kBlue-6);
    h_true_proton_denom->SetLineColor(kBlue-6);
    h_true_proton_denom->Draw("hist same");

    h_true_proton_ratio->SetLineColor(kBlue);
    h_true_proton_ratio->SetMarkerColor(kBlue);
    h_true_proton_ratio->SetLineWidth(2);
    h_true_proton_ratio->SetMarkerStyle(20);

    h_true_proton_ratio->GetYaxis()->SetTitle("Efficiency [%]");
    h_true_proton_ratio->GetXaxis()->SetTitle("True Proton Energy [GeV]");

    h_true_proton_ratio->SetMaximum(100.0);
    h_true_proton_ratio->SetMinimum(0);
    h_true_proton_ratio->GetXaxis()->SetRangeUser(0,1);
    h_true_proton_ratio->SetTitle("");

    TLegend *l2 = new TLegend(0.13,0.79,0.89,0.89);
    l2->SetLineColor(kWhite);
    l2->SetLineWidth(0);
    l2->SetNColumns(2);
    l2->AddEntry(h_true_proton_ratio,"Efficiency: True Proton Energy","lp");
    l2->AddEntry(h_true_proton_denom,"#splitline{}{True Proton Spectum (au)}","fl");
    l2->Draw();


    c2->SaveAs(("eff_"+tag+"_"+file->tag+"_pretopo_proton.pdf").c_str(),"pdf");

}


bdt_efficiency::bdt_efficiency(std::vector<bdt_file*> vec_files, std::string cut){
    //OK this file will simply make an effciciey curves for all files in vec_files. 

    // we want to make this a function for Nu_E
    bdt_variable true_energy("mctruth_nu_E","(50, 0 , 2)","True Neutrino Energy [GeV]",false,"d");

    TCanvas * c = new TCanvas();
    TPad *p1 = (TPad*)c->cd();

    //might need log y?
    //p1->SetLogy();


    TLegend *l = new TLegend(0.13,0.79,0.89,0.89);
    l->SetLineColor(kWhite);
    l->SetLineWidth(0);
    //	l->SetNColumns(2);



    for(size_t f=0; f< vec_files.size(); f++){
        bdt_file * file = vec_files[f];

        //basically irrelavent now as its a ratio
        double conversion = file->scale_data*13.2e20/file->pot;

        file->tvertex->SetEntryList(NULL);

        //The "definition" cuts, i.e the denominator, and what's defined in the XML
        std::string defin_cut = file->flow.definition_cuts;
        double n_starting_events = file->GetEntries(defin_cut)*conversion;

        std::cout<<"File "<<file->tag<<" has  "<<n_starting_events<<" events when scaled to "<<13.2e20<<std::endl;


        c->cd();
        TH1* h_true_nu_energy = (TH1*)file->getTH1(true_energy, defin_cut , "true_energy_num_"+file->tag, 13.2e20);
        c->cd();
        TH1* h_true_nu_energy_cut = (TH1*)file->getTH1(true_energy, defin_cut+"&&"+cut , "true_energy_cut_"+file->tag, 13.2e20);
        c->cd();

        h_true_nu_energy_cut->Divide(h_true_nu_energy);
        h_true_nu_energy_cut->SetLineWidth(2);
        h_true_nu_energy_cut->Draw("same lp");
        h_true_nu_energy_cut->SetMaximum(1);
        h_true_nu_energy_cut->GetYaxis()->SetRangeUser(0, 0.3);
        //if log, minimum cant be 0, make it small
        h_true_nu_energy_cut->SetMinimum(0);
        h_true_nu_energy_cut->SetTitle("");
        l->AddEntry(h_true_nu_energy_cut,file->plot_name.c_str() ,"lp");
    }

    l->Draw();
    c->SaveAs("EFFICIENCY_test.pdf","pdf");


}

bdt_efficiency::bdt_efficiency(std::vector<bdt_file*> vec_files, std::vector<bdt_file*> filt_files){
    // OK this file will simply make an efficiency curve for all files in vec_files. 

    // we want to make this a function for Nu_E
    std::cout << "[EFF] Starting filter efficiency function" << std::endl;
    bdt_variable true_energy("mctruth_nu_E","(20, 0 , 2)","True Neutrino Energy [GeV]",false,"d");

    TCanvas * c = new TCanvas();
    TPad *p1 = (TPad*)c->cd();

    //might need log y?
    //p1->SetLogy();


    TLegend *l = new TLegend(0.13,0.64,0.45,0.89);
    l->SetLineColor(kWhite);
    l->SetLineWidth(2);
    l->SetNColumns(1);

    if (vec_files.size() != filt_files.size() ) {
        std::cout << "[EFF] WARNING: Uneven number of filtered and unfiltered files" << std::endl;
        std::cout << "[EFF] WARNING: " << vec_files.size() << " BDT files and " << 
          filt_files.size() << " filtered files" << std::endl;
    }

    for(size_t f=0; f< vec_files.size(); f++){
        bdt_file * unfiltered_file = vec_files[f];
        bdt_file * filtered_file = filt_files[f];

        //basically irrelavent now as its a ratio
        //double conversion = unfiltered_file->scale_data*13.2e20/file->pot;

        unfiltered_file->tvertex->SetEntryList(NULL);

        //The "definition" cuts, i.e the denominator, and what's defined in the XML
        std::string unfiltered_cut = unfiltered_file->flow.definition_cuts;
        std::string filtered_cut = filtered_file->flow.definition_cuts;
        //double n_starting_events = unfiltered_file->GetEntries(unfiltered_cut)*conversion;

        //std::cout<<"unfiltered_file "<<unfiltered_file->tag<<" has  "<<n_starting_events<<" events when scaled to "<<13.2e20<<std::endl;

        c->cd();
        TH1* h_true_nu_energy_unfiltered = (TH1*)unfiltered_file->getTH1(true_energy, unfiltered_cut , "true_energy_unfilt_"+unfiltered_file->tag, 13.2e20);
        c->cd();
        TH1* h_true_nu_energy_filtered = (TH1*)filtered_file->getTH1(true_energy, filtered_cut, "true_energy_filt_"+filtered_file->tag, 13.2e20);
        c->cd();

        h_true_nu_energy_filtered->Divide(h_true_nu_energy_unfiltered);
        h_true_nu_energy_filtered->SetLineWidth(2);
        h_true_nu_energy_filtered->Draw("same lp");
        h_true_nu_energy_filtered->SetMaximum(1);
        h_true_nu_energy_filtered->GetYaxis()->SetRangeUser(0, 0.3);
        //if log, minimum cant be 0, make it small
        h_true_nu_energy_filtered->SetMinimum(0);
        h_true_nu_energy_filtered->SetTitle("");
        l->AddEntry(h_true_nu_energy_filtered,unfiltered_file->plot_name.c_str() ,"lp");
    }

    c->cd();
    l->Draw();
    c->cd();
    c->SaveAs("EFFICIENCY2_test.pdf","pdf");


}



int fancyFiciency(bdt_file *file, std::string additional_denom, std::string additional_numer, bdt_variable & var, std::string tag, int denom_stage, int numer_stage,std::vector<double> bdtcuts){



    TCanvas * ceff = new TCanvas();
    TPad *p = (TPad*)ceff->cd();

    std::string denom_cuts = "("+file->getStageCuts(denom_stage,bdtcuts)+"&&"+additional_denom+")";
    TH1* h_spec_denom = (TH1*)file->getTH1(var, denom_cuts, "true_"+var.safe_unit+"_num_"+file->tag, 10.115e20);

    std::string numer_cuts = "("+file->getStageCuts(numer_stage,bdtcuts)+"&&"+additional_numer +"&&"+additional_numer +")";
    TH1* h_spec_numer = (TH1*)file->getTH1(var, numer_cuts, "true_"+var.safe_unit+"_num_"+file->tag, 10.115e20);

    std::cout<<"Gotten stage cuts"<<std::endl;
    p->cd();

    int col1 = kBlue -4;
    int col2 = kRed - 4;


    //h_spec_denom->SetFillStyle(3445);
    //h_spec_denom->SetFillColor(kBlue-6);
    h_spec_denom->SetLineColor(col1);
    h_spec_denom->Draw("hist");
    h_spec_denom->SetMaximum(h_spec_denom->GetMaximum()*1.4);
    h_spec_denom->SetMinimum(0);

    h_spec_denom->SetLineColor(col1);
    h_spec_denom->SetMarkerColor(col1);
    h_spec_denom->SetLineWidth(2);
    h_spec_denom->SetMarkerStyle(20);

    h_spec_numer->SetFillStyle(3454);
    h_spec_numer->SetFillColor(col1);
    h_spec_numer->SetLineColor(col1);
    h_spec_numer->Draw("hist same");
    

    h_spec_denom->GetYaxis()->SetTitle("Events");
    h_spec_denom->GetXaxis()->SetTitle(var.unit.c_str());

    h_spec_denom->SetTitle("");
    p->Draw();
    p->Update();


    std::cout<<"Ploted bits"<<std::endl;
    p->cd(); 
    TH1 * h_spec_ratio = (TH1*)h_spec_numer->Clone((var.safe_unit+"_gg_"+file->tag).c_str());
    h_spec_ratio->Divide(h_spec_denom);
    h_spec_ratio->SetMaximum( std::min(1.0, h_spec_ratio->GetMaximum()*1.3));
    h_spec_ratio->SetMinimum(0);

    float rightmax = 1.1*h_spec_ratio->GetMaximum();
    float scale = p->GetUymax()/rightmax;
    h_spec_ratio->SetLineColor(col2);
    h_spec_ratio->Scale(scale);
           
    h_spec_ratio->SetMarkerStyle(20);
    h_spec_ratio->SetMarkerSize(1);
    h_spec_ratio->SetMarkerColor(col2);
    
    h_spec_ratio->Draw("same E0 E1 P");
             
    std::cout<<"Ploted ratio"<<std::endl;
    TGaxis *axis = new TGaxis(p->GetUxmax(),p->GetUymin(),p->GetUxmax(), p->GetUymax(),0,rightmax,510,"+L");
    axis->SetLineColor(col2);
    axis->SetLabelColor(col2);
    axis->Draw();

    std::cout<<h_spec_numer->Integral()<<" "<<h_spec_denom->Integral()<<std::endl;

    TLegend *l2 = new TLegend(0.13,0.79,0.89,0.89);
    l2->SetLineColor(kWhite);
    l2->SetLineWidth(0);
    l2->SetNColumns(2);
    l2->AddEntry(h_spec_denom,("Denominator: "+std::to_string(denom_stage)).c_str(),"l");
    l2->AddEntry(h_spec_numer,("Numerator: "+std::to_string(numer_stage)).c_str(),"fl");
    l2->AddEntry(h_spec_ratio,("Efficiency : "+to_string_prec(h_spec_numer->Integral()/h_spec_denom->Integral()*100.0  ,2)+"\%").c_str(),"pl");
    l2->Draw();

    ceff->SaveAs(("fancyFiciency_"+tag+"_"+file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(numer_stage)+"_over_"+std::to_string(denom_stage)+".pdf").c_str(),"pdf");
    return 0;
}




