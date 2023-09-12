/* This macro aims to see the relation between between S/sqrt(B) and Purity*Efficiency
 * by Guanqun
 */
void signal_to_bkg(){

     double p_min  = 0, p_max = 1, p_step = 0.01;
     double ep_min  = 0.7, ep_max = 1, ep_step = 0.05;

     std::vector<double> p_vals;
     for(double p = p_min; p < p_max; p+= p_step){
	p_vals.push_back(p);
     }
     std::vector<TGraph*> graphs;    
     for(double ep = ep_max; ep > ep_min; ep-= ep_step){
	std::vector<double> x, y;
	for(auto& p : p_vals){
	    double eff_times_purity = p * ep;
	    x.push_back(sqrt(eff_times_purity/(1-p)));
	    y.push_back(eff_times_purity);
	}
	graphs.push_back(new TGraph((int)x.size(), &x[0], &y[0]));
	std::cout << "ep: " << ep << " x max: " << *std::max_element(x.begin(), x.end()) << std::endl;
     }

     TCanvas* c = new TCanvas("c", "c");
     TLegend* leg = new TLegend(0.5, 0.1, 0.9, 0.4);
     for(int i = 0 ; i!= graphs.size(); ++i){
	double ep = ep_max -  ep_step*i;
	graphs[i]->SetLineColor(i+1);
	graphs[i]->SetMarkerColor(i+1);
	leg->AddEntry(graphs[i], Form("Epsilon = %.2f", ep), "L");
	if(i == 0){
	    //graphs[i]->SetMaximum(2);
	    graphs[i]->Draw("AL");
	    graphs[i]->SetTitle("relation between S/sqrt(B) and P*#epsilon; (S/#sqrt{B})/#sqrt{N_{i,s}}; P*#epsilon");
	    graphs[i]->GetXaxis()->SetRangeUser(0, 2);
	}
	else
	    graphs[i]->Draw("same");
     }
     leg->Draw();
     c->SaveAs("Relation.pdf", "pdf");
     return;
} 
