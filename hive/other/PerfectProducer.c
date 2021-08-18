
void PerfectProducer(){



    TFile*fhive; 
    std::string tag;
    std::string id;
    double con_max;
    double top_max;
    std::string sel;
    std::string Ndat;
    double glob_scale = 0.05;//20.4761/26.9796;
    std::string ylab = "Events/ 0.05 [GeV]";

    if(1){
        fhive= new TFile("Ratio_1g0pMar2020_datamc_FinalSelection1g0p_ReconstructedShowerEnergyGeV_stage_4MergedCheck.root","read");
        tag = "MergedCheck";
        id="2";
        con_max = 49;
        top_max = 70;
        sel = "1#gamma0p";
        Ndat = "153";
        glob_scale = 0.05;
       ylab = "Events/ 0.05 [GeV]";

    }else{
        fhive = new TFile("Ratio_1g1pMar2020_v4_datamc_FinalSelection1g1p_ReconstructedShowerEnergyGeV_stage_6DetSysMoo.root","read");
        tag= "DetSysMoo";
        id="1";
        con_max = 15.9;
        top_max = 26;
        sel = "1#gamma1p";
        Ndat = "16";
        glob_scale=0.1;
        ylab = "Events/ 0.1 [GeV]";

    }


    TFile *fcons = new TFile("Constraint_DataOverlaid_output.root","read");
    TH1D *hcons_before = (TH1D*)fcons->Get(("Original_Spec;"+id).c_str());
    TH1D *hcons_after = (TH1D*)fcons->Get(("Constrained_Spec;"+id).c_str());

    std::vector<std::string> nams = {"GENIE CV NC #Delta #rightarrow N#gamma","NC 1 #pi^{0} Coherent","NC 1 #pi^{0} Non-Coherent","CC #nu_{#mu} 1 #pi^{0}","BNB Other","CC #nu_{e}/#bar{#nu_{e}} Intrinsic","Dirt (NO SHOW)","Dirt (Outside TPC)","Cosmic Data, Runs 1-3"};


    double mmod = 1;
    double title_size_ratio=0.10*mmod*0.75;
    double label_size_ratio=0.085*mmod;
    double title_offset_ratioY = 0.3*mmod ;
    double title_offset_ratioX = 1.1*mmod;

    double title_size_upper=0.06;
    double label_size_upper=0.05;
    double title_offset_upper = 0.6;



    //***************

    THStack * stack = (THStack*)fhive->Get(("tstk_"+tag).c_str());
    TH1D *sum = (TH1D*)fhive->Get(("tsum_"+tag).c_str());
    TH1D *data = (TH1D*)fhive->Get(("d0_"+tag).c_str());


    TList *stlists = (TList*)stack->GetHists();
    std::vector<TColor*> colors;

    TH1 *sig;
    int sig_col = 0;
    double integral = 0;


    hcons_before->Scale(glob_scale,"width");
    hcons_after->Scale(glob_scale,"width");


    for(const auto&& obj: *stlists){
        std::cout<<"Hist: "<<((TH1*)obj)->GetName()<<" "<<((TH1*)obj)->Integral()<<" "<<((TH1*)obj)->GetFillColor()<<std::endl;
        integral+=((TH1*)obj)->Integral();
        TColor *c = (TColor*)fhive->Get(("Color"+std::to_string(((TH1*)obj)->GetFillColor())).c_str());

        TColor *ctmp = new TColor(c->GetNumber(),c->GetRed(),c->GetGreen(),c->GetBlue() );
        ((TH1*)obj)->SetFillColor(ctmp->GetNumber());

        ((TH1*)obj)->Scale(glob_scale,"width");
        std::string nam = ((TH1*)obj)->GetName();

        if(nam.find("NCDelta")!=std::string::npos){
            sig_col = ((TH1*)obj)->GetFillColor();
            //((TH1*)obj)->SetLineStyle(9);
            sig = ((TH1*)obj);

            //((TH1*)obj)->Scale(glob_scale,"width");
        }

        ctmp->Print();
        colors.push_back(ctmp);
    }
    sum->Scale(glob_scale,"width");

    data->SetMarkerStyle(20);
    data->SetMarkerSize(2.5);
    data->SetLineColor(kBlack);
    data->SetBinErrorOption(TH1::kPoisson);


    //Ok, move to asymetrical TGraph errors for a bit 
    std::vector<double> vAsyX;  
    std::vector<double> vAsyY;  

    std::vector<double> vAsyErr_right;
    std::vector<double> vAsyErr_left;  
    std::vector<double> vAsyErr_up;  
    std::vector<double> vAsyErr_down;  

    for(int id =0; id<data->GetNbinsX()+1;id++){
        double sc = 0;
        vAsyErr_left.push_back(data->GetBinWidth(id)*sc/2.0);
        vAsyErr_right.push_back(data->GetBinWidth(id)*sc/2.0);

        double dy = data->GetBinWidth(id);
        double scal = glob_scale/dy;

        vAsyX.push_back(data->GetBinCenter(id));
        vAsyY.push_back(data->GetBinContent(id)*scal);

        vAsyErr_up.push_back(scal*(data->GetBinErrorUp(id)));
        vAsyErr_down.push_back(scal*(data->GetBinErrorLow(id)));

    }


    TGraphAsymmErrors * gAsyData = new TGraphAsymmErrors(vAsyX.size(),&vAsyX[0],&vAsyY[0],&vAsyErr_left[0],&vAsyErr_right[0],&vAsyErr_down[0],&vAsyErr_up[0]);
    gAsyData->SetLineWidth(1);
    gAsyData->SetMarkerStyle(20);
    gAsyData->SetMarkerSize(2.5);
    gAsyData->SetMarkerColor(kBlack);
    gAsyData->SetFillColor(kBlack);
    gAsyData->SetFillStyle(1001);


    //and scale data too for CV
    for(int id =1; id<data->GetNbinsX()+1;id++){
        double dy = data->GetBinWidth(id);
        data->SetBinContent(id,glob_scale*data->GetBinContent(id)/dy);
    }
    data->SetBinErrorOption(TH1::kPoisson);



    std::cout<<"Sum: "<<sum->Integral()<<", indiv "<<integral<<std::endl;

    TCanvas *c  = new TCanvas("c","c",1800,1200);
    c->cd();
    TPad* padtop= new TPad("Padt","Padt", 0, 0.4, 1.0, 1.0);//0.4 was 0.35
    padtop->SetBottomMargin(0);
    padtop->Draw();
    padtop->cd();


    gStyle->SetHatchesLineWidth(1);

    sum->Draw("hist");
    stack->Draw("hist same");

    TH1D *sum_copy = (TH1D*)sum->Clone("sum_copy");
    sum->Draw("hist same");

    //sum->SetFillColor(kGray + 3);
    //sum->SetFillStyle(3454);
    //sum->Draw("E2 same");
    sum_copy->Draw("hist same");

    // gStyle->SetHatchesLineWidth(2);
    gStyle->SetHatchesSpacing(0.9);

    sum->SetMaximum(top_max);
    sum->SetMinimum(0.0001);
    sum->SetTitle("");
    sum->GetYaxis()->SetTitle(ylab.c_str());
    sum->GetYaxis()->SetTitleSize(title_size_upper);
    sum->GetYaxis()->SetLabelSize(label_size_upper);
    sum->GetYaxis()->SetTitleOffset(title_offset_upper);


    //sum->SetFillColor(kGray+2);
    //sum->SetLineWidth(1);
    //sum->SetFillStyle(3354);

    /*
       TH1D *cons = (TH1D*)sum->Clone("cons");
       cons->SetFillColor(kBlack);
       cons->SetFillStyle(0);
       cons->SetLineStyle(2);
       cons->SetLineWidth(2);
       cons->DrawCopy("hist same");
       cons->SetFillStyle(3345);
       cons->DrawCopy("E2 same");
       */

    //Draw Scaled signal on top
    TH1D *lee_on_top = (TH1D*)sig->Clone(("lee_sig_"+tag).c_str());
    lee_on_top->SetFillStyle(0);
    lee_on_top->SetLineStyle(9);
    lee_on_top->Scale(3.18);
    lee_on_top->Add(sum);
    lee_on_top->SetLineColor(sig_col);
    lee_on_top->SetLineWidth(2);
    lee_on_top->Draw("hist same");   

    sum->DrawCopy("hist same");

    
    //data->Draw("E1P same");
    gAsyData->Draw("same E1P");
    gAsyData->Draw("same P");

    TLegend *l0;
    l0 = new TLegend(0.11,0.6,0.89,0.89);
    l0->SetFillStyle(0);
    l0->SetLineWidth(0);
    l0->SetLineColor(kWhite);
    l0->SetNColumns(2);

    int ic =0;
    for(const auto&& obj: *stlists){
        if(ic==6 || ic >8){ic++;continue;}
        l0->AddEntry((TH1*)obj, nams[ic].c_str(),"f");
        ic++;
    }
    l0->AddEntry(data,("BNB Data, Runs 1-3. Total: "+Ndat).c_str(),"lp");
    l0->AddEntry(lee_on_top,"MB LEE Model (x3.18 CV #Delta #rightarrow N#gamma)","l");

    l0->Draw();

    double yypos = 0.4;
    TLatex pottex;
    pottex.SetTextSize(0.06);
    pottex.SetTextAlign(13);  //align at top
    pottex.SetNDC();
    std::string pot_draw = "6.8x10^{20} POT";
    pottex.DrawLatex(.65,yypos+0.1, pot_draw.c_str());

    TLatex descriptor_tex;
    descriptor_tex.SetTextSize(0.06);
    descriptor_tex.SetTextAlign(13);  //align at top
    descriptor_tex.SetNDC();
    descriptor_tex.DrawLatex(0.65,yypos+0.16,("Selection "+sel).c_str());

    TLatex constex;
    constex.SetTextSize(0.06);
    constex.SetTextAlign(13);  //align at top
    constex.SetNDC();
    std::string cot_draw = "Unconstrained CV";
    constex.DrawLatex(.65,yypos+0.04, cot_draw.c_str());


    //*********************** BOTTOM***************************/
    c->cd();
    TPad*pad0bot = new TPad("padB","padB", 0, 0.0, 1, 0.4);//0.4 was 0.35
    pad0bot->SetTopMargin(0);
    pad0bot->SetBottomMargin(0.251);//0.351
    pad0bot->SetGridx(); 
    pad0bot->Draw();
    pad0bot->cd();      

    gStyle->SetOptStat(0);
    gStyle->SetHatchesLineWidth(2); 
    gStyle->SetEndErrorSize(10);

    data->Draw("hist p");
    data->SetMaximum(con_max);
    data->SetMinimum(0);
    TLegend *l1;
    l1 = new TLegend(0.6,0.7,0.89,0.89);
    l1->SetFillStyle(1001);
    //l1->SetLineWidth(0);
    //l1->SetLineColor(kWhite);


    //  hcons_before->Draw("E1 hist same"); 
    hcons_before->SetLineColor(kMagenta+3);
    hcons_before->SetLineWidth(2);
    hcons_before->SetLineStyle(kDashed);
    TH1D* hcons_before_copy = (TH1D*)hcons_before->Clone(); 
    hcons_before->SetFillColorAlpha(kMagenta -10, 0.8);

    //    hcons_after->Draw("E1 hist same"); 
    hcons_after->SetLineColor(kCyan+3); //kGreen+3
    hcons_after->SetLineWidth(2); 
    TH1D* hcons_after_copy = (TH1D*)hcons_after->Clone(); 
    hcons_after->SetFillColorAlpha(kCyan, 0.9); //kGreen-7
    hcons_after->SetFillStyle(3445);

    hcons_before->Draw("E2 same");
    hcons_before_copy->Draw("HIST SAME");
    hcons_after->Draw("E2 same");
    hcons_after_copy->Draw("HIST SAME");

    l1->AddEntry(hcons_before,"Unconstrained Backgrounds","fl");
    l1->AddEntry(hcons_after,"Constrained Backgrounds","fl");
    l1->Draw();


    //    data->DrawCopy("same E1P");
    gAsyData->Draw("same E1P");
    gAsyData->Draw("same P");



    data->SetTitle("");
    data->GetYaxis()->SetTitle(ylab.c_str());
    data->GetXaxis()->SetTitleOffset(title_offset_ratioX);
    data->GetYaxis()->SetTitleOffset(title_offset_ratioY*1.25);
    data->GetYaxis()->SetTitleSize(title_size_ratio);
    data->GetXaxis()->SetTitleSize(title_size_ratio);
    data->GetYaxis()->SetLabelSize(label_size_ratio);
    data->GetXaxis()->SetLabelSize(label_size_ratio);

    c->SaveAs(("test_"+tag+".pdf").c_str(),"pdf");
    return;


}
