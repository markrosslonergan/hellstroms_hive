#include "bdt_vertex_eff.h"

int bdt_vertex_eff::plotEfficiencies(std::string outputname){
    //bdt_file * file;
    //bdt_var plot_var;
    //std::string denominator_cut;
    //std::vector<std::string> cut_vector;


    TCanvas *cverteff = new TCanvas();
    cverteff->cd();

    //Get the denominator 
    TH1 * denom = (TH1*)file->getTH1(plot_var, denominator_cut, plot_var.safe_name+"denom", 6.6e20 , 1);
    double n_denom = denom->GetSumOfWeights();

    std::vector<int> tmp_cols= {kRed-7, kBlue-6, kGreen+1, kYellow-7, kMagenta-3, kOrange-3, kGreen+1};
    std::vector<TH1*> stage_hists;

    TLegend *l =new TLegend(0.1,0.59,0.59,0.89);
    l->SetLineWidth(0);
    l->SetFillStyle(0);

    std::string stage_cuts = denominator_cut;
    //Loop over all the sequential cuts that you would like. 
    for(size_t i=0; i< cut_vector.size(); i++){
        stage_cuts += "&&"+cut_vector[i];

        cverteff->cd();
        stage_hists.push_back( (TH1*)file->getTH1(plot_var, stage_cuts,plot_var.safe_name+"stage_"+std::to_string(i),6.6e20,1));
        double n_stage_hist = stage_hists.back()->GetSumOfWeights();

        stage_hists.back()->Divide(denom);
        stage_hists.back()->SetLineColor(tmp_cols[i]);
        stage_hists.back()->SetLineWidth(3);
        stage_hists.back()->SetMarkerStyle(2);

        cverteff->cd();
        if(i==0){
            stage_hists.back()->Draw("E1 ");
        }else{
            stage_hists.back()->Draw("same E1");
        }

        l->AddEntry(stage_hists.back(), std::to_string(i).c_str(),"l");
    
    }

    cverteff->cd();
    stage_hists.front()->GetYaxis()->SetTitle("Efficiency");
    stage_hists.front()->GetXaxis()->SetTitle(plot_var.unit.c_str());

    stage_hists.front()->SetMaximum(1);
    stage_hists.front()->SetMinimum(0);
    l->Draw();

    cverteff->SaveAs(outputname.c_str());


    return 0;    

}







