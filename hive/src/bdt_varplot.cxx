#include "bdt_varplot.h"

int  plot_bdt_variables(bdt_file * signal_pure, bdt_file * background_pure, std::vector<bdt_variable> vars, bdt_info input_bdt_info, bool isSpectator, int stage, std::vector<double> bdtcuts){

    std::vector<std::string> title = {"Topological Selection","Pre-Selection Cuts","Cosmic","BNB","NCPi0","Nue","SSveto"};

    int j = stage;

    std::cout<<"on stage : "<<j<<std::endl;
    std::cout<<" Setting sig stage entry lists."<<std::endl;
    if(j>1){
        signal_pure->calcBDTEntryList(j,bdtcuts);
        background_pure->calcBDTEntryList(j,bdtcuts);
    }

    signal_pure->setStageEntryList(j);
    std::cout<<" Setting back stage entry lists."<<std::endl;
    background_pure->setStageEntryList(j);
    std::cout<<" Set stage entry lists."<<std::endl;

    int nv = 0;
    for(auto &v: vars){

        //std::string cut_signal = signal_pure->getStageCuts(j,-9,-9); 
        //std::string cut_background_pure = background_pure->getStageCuts(j,-9,-9); 

        //		TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_var" ,1.0);
        //		TH1* bkg = background_pure->getTH1(v,cut_background_pure.c_str(),v.safe_name+"_bkg_var" ,1.0);
        TCanvas *c_var = new TCanvas(("cvar_"+v.name+"_"+input_bdt_info.identifier).c_str(), ("cvar_"+v.name+"_"+input_bdt_info.identifier).c_str(),1200,1200);
        c_var->cd();

        std::cout<<"On variable: "<<v.name<<std::endl;

        TH1* sig = signal_pure->getTH1(v,"1",v.safe_name+"_sig_var" ,1.0);
        TH1* bkg = background_pure->getTH1(v,"1",v.safe_name+"_bkg_var" ,1.0);

        std::cout<<"Integrals: "<<sig->Integral()<<" "<<bkg->Integral()<<std::endl;

        if(!TMath::Finite(sig->Integral())){
            std::cout<<"scanning"<<std::endl;
            signal_pure->tvertex->Scan(v.name.c_str());
        }else{
            std::cout<<"So its fnite"<<std::endl;
        }



        sig->Scale(1.0/sig->Integral());			
        bkg->Scale(1.0/bkg->Integral());			
        sig->SetLineColor(signal_pure->col);
        bkg->SetLineColor(background_pure->col);
        sig->SetLineWidth(2);
        bkg->SetLineWidth(2);
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
        sig->GetYaxis()->SetTitle("Events [Area Normalized]");
        sig->GetYaxis()->SetTitleOffset(1.5);

        TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
        l->SetLineColor(kWhite);
        l->SetFillStyle(0);
        l->SetNColumns(2);


        l->AddEntry(sig, signal_pure->plot_name.c_str(),"lf");	
        l->AddEntry(bkg, background_pure->plot_name.c_str(),"lf");	
        l->Draw();

        TText *pre;
        if (isSpectator) {
            pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulaton In-Progress - Spectator Variable");
        }else {
            pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulaton In-Progress - Training Variable");

        }


        //TText *pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulation - In Progress");
        pre->Draw();

        TLatex latex;
        latex.SetTextSize(0.06);
        latex.SetTextAlign(13);  //align at top
        latex.SetNDC();
        latex.DrawLatex(.7,.71, input_bdt_info.topo_name.c_str());


        double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
        sig->SetMaximum(max_height*1.3);


        c_var->Print(("vars/"+std::to_string(nv)+"_"+input_bdt_info.identifier+"_"+v.safe_unit+"_stage_"+std::to_string(j)+".pdf").c_str(),"pdf");


        delete sig;
        delete bkg;
        delete c_var;
        nv++;
    }//end vars



    return 0;
}


int plot_bdt_variable(bdt_file * signal_pure, bdt_file * background_pure, bdt_variable v,bdt_info input_bdt_info, bool isSpectator,int stage, std::vector<double> bdtcuts){

    std::vector<bdt_variable> vars = {v};
    return plot_bdt_variables(signal_pure, background_pure, vars, input_bdt_info, isSpectator,stage, bdtcuts);
}


int  plot_bdt_variables(bdt_file * signal_pure, bdt_file *background_pure, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts, int stage){

    std::vector<std::string> title = {"Topological Selection","Pre-Selection Cuts","cos","bnb","ncpi0","nue","tm"};

    if(stage>1) {
        signal_pure->calcBDTEntryList(stage,bdt_cuts);
        background_pure->calcBDTEntryList(stage,bdt_cuts);
    }
    signal_pure->setStageEntryList(stage);
    background_pure->setStageEntryList(stage);

    for(auto &v: vars){

        //std::string cut_signal = signal_pure->getStageCuts(j,-9,-9); 
        //std::string cut_background_pure = background_pure->getStageCuts(j,-9,-9); 

        //		TH1* sig = signal_pure->getTH1(v,cut_signal.c_str(),v.safe_name+"_sig_var" ,1.0);
        //		TH1* bkg = background_pure->getTH1(v,cut_background_pure.c_str(),v.safe_name+"_bkg_var" ,1.0);
        TCanvas *c_var = new TCanvas(("cvar_"+v.name).c_str(), ("cvar_"+v.name).c_str(),1200,1200);
        c_var->cd();

        std::cout<<"On variable: "<<v.name<<std::endl;

        TH1* sig = signal_pure->getTH1(v,"1",v.safe_name+"_sig_var" ,1.0);
        TH1* bkg = background_pure->getTH1(v,"1",v.safe_name+"_bkg_var" ,1.0);

        std::cout<<"Integrals: "<<sig->Integral()<<" "<<bkg->Integral()<<std::endl;
        sig->Scale(1.0/sig->Integral());			
        bkg->Scale(1.0/bkg->Integral());			
        sig->SetLineColor(signal_pure->col);
        bkg->SetLineColor(background_pure->col);
        sig->SetLineWidth(2);
        bkg->SetLineWidth(2);
        c_var->cd();

        sig->SetFillColor(signal_pure->col);
        bkg->SetFillColor(background_pure->col);
        sig->SetFillStyle(3445);
        bkg->SetFillStyle(3454);

        if(stage!=1){
            sig->SetTitle(title.at(stage).c_str());
        }else{
            sig->SetTitle(" ");
        }

        c_var->cd();			

        sig->Draw("hist");
        sig->SetMinimum(0);
        bkg->Draw("hist same");
        //sig->GetXaxis()->SetTitle(v.unit.c_str());
        sig->GetYaxis()->SetTitle("Events [Area Normalized]");
        sig->GetYaxis()->SetTitleOffset(1.5);

        TLegend *l = new TLegend(0.11,0.75,0.89,0.89);
        l->SetLineColor(kWhite);
        l->SetFillStyle(0);
        l->SetNColumns(2);


        l->AddEntry(sig, signal_pure->plot_name.c_str(),"lf");	
        l->AddEntry(bkg, background_pure->plot_name.c_str(),"lf");	
        l->Draw();

        TText *pre;
        if (false) {
            pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulaton In-Progress - Spectator Variable");
        }else {
            pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulaton In-Progress - Training Variable");

        }


        //TText *pre = drawPrelim(0.1,0.915,0.03,"MicroBooNE Simulation - In Progress");
        pre->Draw();

        TLatex latex;
        latex.SetTextSize(0.06);
        latex.SetTextAlign(13);  //align at top
        latex.SetNDC();
        latex.DrawLatex(.7,.71, signal_pure->flow.bdt_vector[0].topo_name.c_str());


        double max_height = std::max( sig->GetMaximum(), bkg->GetMaximum());
        sig->SetMaximum(max_height*1.3);


        c_var->Print(("var/"+v.safe_unit+"_stage_"+std::to_string(stage)+".pdf").c_str(),"pdf");


        delete sig;
        delete bkg;
        delete c_var;
    }


    return 0;

}
