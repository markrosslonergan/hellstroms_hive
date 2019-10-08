#include "bdt_scatter.h"


int plot_scatter(bdt_file *file,  std::vector<bdt_info> infos){

    for(int i=0; i< infos.size();i++){
        for(int j=i+1; j< infos.size();j++){


            TCanvas *c = new TCanvas();
            c->cd();

            bdt_variable vari = file->getBDTVariable(infos[i]);
            bdt_variable varj = file->getBDTVariable(infos[j]);


            TH2 * H1 = (TH2*)file->getTH2(vari,varj, vari.name+">0 && "+ varj.name+">0", file->tag+"scatter" , 1);
            c->cd();
            H1->SetMarkerColor(file->col);
            H1->Draw();

            c->SaveAs(("scatter/scatterplot"+file->tag+"_"+infos[i].name+"_"+infos[j].name+".pdf").c_str());

        }
    }

    




    return 0;



}


