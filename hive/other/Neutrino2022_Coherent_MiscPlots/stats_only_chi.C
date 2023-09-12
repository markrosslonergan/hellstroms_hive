void stats_only_chi(){

    std::vector<std::string> stages={"Final Selection", "Final Sel. + max PSV score < 0.5", "Final Sel. + max PSV score < 0.3"};
    std::vector<int> colors={kRed - 7, kBlue-7, kGreen-7};
    std::vector<int> line_style{ 1, 2,9};
    std::vector<double> sig_num = {1.45282, 1.34912, 1.2393};
    std::vector<double> bkg_num = {53.7504, //0.283304 + 2.51782 + 3.72333 + 29.5771 + 4.94183 + 3.48646 + 0.615469 + 1.79207 + 4.5715 + 2.24156;
				   36.6351, //0.103592 + 1.60617 + 3.58413 + 17.3462 + 3.79302 + 2.66568 + 0.477559 + 1.79207 + 3.77233 + 1.49437;
			           32.2516 //0.0868497 + 1.32915 + 3.45372 + 15.8326 + 3.09424 + 2.66568 + 0.419358 + 1.79207 + 3.57791 + 0; 
				   };
    double LEE_scale = 21.8;
    for(int i = 0; i!= stages.size(); ++i){
        std::cout << "stage: " << stages[i] << ", signal: " << sig_num[i] <<  ", bkg: " << bkg_num[i] << std::endl;
    }


    TCanvas* c = new TCanvas("c", "c");
    TLegend* leg = new TLegend(0.11, 0.6, 0.5, 0.89);
    leg->SetTextSize(0.04);
    leg->SetBorderSize(0);


    std::cout << "absolute numbers... " << std::endl;
    std::vector<TF1*> chi_absolute;
    for(int i = 0; i!= stages.size(); ++i){
	chi_absolute.push_back(new TF1(Form("f_%d", i), "x*x/[0]", 0, 50));
	chi_absolute.back()->SetParameter(0, bkg_num[i]);
        chi_absolute.back()->SetLineColor(colors[i]);
        chi_absolute.back()->SetLineStyle(line_style[i]);
	leg->AddEntry(chi_absolute.back(), stages[i].c_str(), "L");

	if(i ==0){
	   chi_absolute.back()->SetTitle("Sensitivity assuming no NC coherent 1#gamma; Number of NC coherent 1#gamma events; #Delta#chi^{2}");
	   chi_absolute.back()->Draw();
	}else
	   chi_absolute.back()->Draw("same");

	double chi_value = chi_absolute.back()->Eval(LEE_scale*sig_num[i]);
	std::cout << stages[i] << ", " << LEE_scale << "x NC coherent single-photon events has #chi^2 value of " << chi_value << " with "  << sqrt(2)*TMath::ErfInverse(1.0-TMath::Prob(chi_value, 1)) << " sigma" << std::endl; 
    }

    leg->Draw();
    c->SaveAs("Stats_only_sensitivity_absolute.pdf", "pdf");


    std::cout << "scale factor of the rate... " << std::endl;
    leg->Clear();
    c->Clear();
    std::vector<TF1*> chi_scale;
    for(int i = 0; i!= stages.size(); ++i){
	chi_scale.push_back(new TF1(Form("f_%d", i), "TMath::Power([0], 2.0)*x*x/[1]", 0, 40));
	chi_scale.back()->SetParameter(0, sig_num[i]);
	chi_scale.back()->SetParameter(1, bkg_num[i]);
        chi_scale.back()->SetLineColor(colors[i]);
        chi_scale.back()->SetLineStyle(line_style[i]);
	leg->AddEntry(chi_scale.back(), stages[i].c_str(), "L");

	if(i ==0){
	   chi_scale.back()->SetTitle("Sensitivity assuming no NC coherent 1#gamma; Scale Factor of predicted NC coherent 1#gamma rate; #Delta#chi^{2}");
	   chi_scale.back()->Draw();
	}else
	   chi_scale.back()->Draw("same");

	double chi_value = chi_scale.back()->Eval(LEE_scale);
	std::cout << stages[i] << ", " << LEE_scale << "x NC coherent single-photon events has #chi^2 value of " << chi_value << " with "  << sqrt(2)*TMath::ErfInverse(1.0-TMath::Prob(chi_value, 1)) << " sigma" << std::endl; 
	std::cout << stages[i] << ", SM-predicted NC coherent single-photon events has #chi^2 value of " << chi_scale.back()->Eval(1) << std::endl; 
    }

    leg->Draw();
    //c->SaveAs("Stats_only_sensitivity_scale_factor.pdf", "pdf");
    return 0;
}
