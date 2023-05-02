void ScatterBDTscore(){

    std::string filepath="/uboone/app/users/gge/hellstroms_hive/hive/working_directory/Jan2022_1g0p/new_coherent_sp/TestOptimization/";
    std::vector<std::string> filenames={"NCSingleCoherentTestTextGen", "NCDeltaRadOverlaySMTrueP", "NCDeltaRadOverlaySMTrueN",
                                        "NCPi0Coh", "NCPi0NotCoh", "CC1Pi0", "BNBOtherExtra", "NueOverlays", "Dirt", "OTPCExtra",
                                        "BNBext"};

    //std::vector<std::string> stages={"stage_4"};
    std::vector<std::string> vars={"simple_1g0pCohNeutrino2022NCPi0_mva", "Max$((trackstub_candidate_plane==2)*1g0pCohNeutrino2022PSV_score)"};
    std::vector<int> colors={kPink, kOrange+10,kTeal, kBlue, kMagenta, kYellow+1, kSpring+3, kRed-10, kViolet,kCyan-6,kViolet+1, kPink+8};
    //------- End of Input ----------

    std::vector<TGraph*> graph_vec;
    for(int i = 0 ;i != filenames.size(); ++i){
        std::string fname = filepath+"sbnfit_1g0pCohNeutrino2022_stage_4_"+filenames[i] +".root";
 	TFile* f = new TFile(fname.c_str(), "read");
	TTree* t = (TTree*)f->Get("singlephoton/simple_tree");
	t->AddFriend("singlephoton/vertex_tree");
	t->AddFriend("singlephoton/unflatten_1g0pCohNeutrino2022PSV");

	TTreeFormula* form_x = new TTreeFormula("form_x", vars[0].c_str(), t);
	TTreeFormula* form_y = new TTreeFormula("form_y", vars[1].c_str(), t);

	graph_vec.push_back(new TGraph(t->GetEntries()));

	for(int e = 0 ; e!= t->GetEntries(); ++e){
	    t->GetEntry(e);
	    form_x->GetNdata();
	    double x = form_x->EvalInstance();
	    form_y->GetNdata();
	    double y = form_y->EvalInstance();
	    graph_vec.back()->SetPoint(e, x, y);
	}

	delete form_x;
	delete form_y;
	delete t;
	delete f;
    }

   //now, draw the scattered plots 
    TCanvas* c= new TCanvas();
    c->Print("Scatter_BDT_Logy.pdf[");
    c->SetLogy(1); 

    for(int i = 0; i != filenames.size(); ++i){
 	auto g = graph_vec[i];
	c->Clear();
	g->SetMarkerSize(8);
	g->SetMarkerStyle(kDot);
	g->SetMarkerColor(colors[i]);
	g->SetTitle((filenames[i]+" NCPi0 Score vs. Max P2 PSV Score").c_str());
	g->GetXaxis()->SetTitle("NCPi0 BDT Score");
	g->GetYaxis()->SetTitle("Max PSV BDT Score on P2");
	g->Draw("AP");
	c->Update();
	c->Print("Scatter_BDT_Logy.pdf");
    }
    c->Clear();
    for(int i = 0; i != filenames.size(); ++i){
 	auto g = graph_vec[i];
	g->SetMarkerSize(2);
	if( i== 0){
	    g->SetTitle("Overlaid NCPi0 Score vs. Max P2 PSV Score");
	    g->Draw("AP");

	}
	else
	    g->Draw("P same");
	c->Update();
    }
	c->Print("Scatter_BDT_Logy.pdf");
	c->Print("Scatter_BDT_Logy.pdf]");
	return ;
}
