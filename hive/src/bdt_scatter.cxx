#include "bdt_scatter.h"


int plot_scatter(bdt_file *f1, bdt_file *f2, bdt_info b1, bdt_info b2 ){


    TCanvas *c = new TCanvas();
    c->cd();


    bdt_variable var_1_1 = f1->getBDTVariable(b1);
    bdt_variable var_1_2 = f1->getBDTVariable(b2);

    bdt_variable var_2_1 = f2->getBDTVariable(b1);
    bdt_variable var_2_2 = f2->getBDTVariable(b2);


    TH2 * H1 = (TH2*)f1->getTH2(var_1_1,var_1_2, var_1_1.name+">0 && "+ var_1_2.name+">0", f1->tag+"scatter" , 1);
    c->cd();
    H1->SetMarkerColor(f1->col);
    H1->Draw();

    TH2 * H2 = (TH2*)f2->getTH2(var_2_1,var_2_2, var_2_1.name+">0 && "+ var_2_2.name+">0", f2->tag+"scatter" , 1);
    c->cd();
    H2->SetMarkerColor(f2->col);
    H2->Draw("same");



    c->SaveAs("test.pdf");



    return 0;



}


