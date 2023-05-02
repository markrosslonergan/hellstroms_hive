/***************************************************************************
 * This macro plots the distance between the exiting proton with leading energy in truth, and the shower for mis-ID'd 1g0p events
 *
 * Guanqun Ge, Feb 23 2022
 */

double two_point_index(const std::vector<double>& p1, const std::vector<double>& p2){
    return sqrt(pow(p1[0] - p2[0], 2.0) + pow(p1[1]-p2[1], 2.0) + pow(p1[2] - p2[2], 2.0));
}

void proton_distribution(){

    //*********** input vertex file *****************
    std::string fin_name = "/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/Precut2Topo/TestTrainSplit_Jan24_2022/preTopo_vertex_NCPi0_Run123_TestSplit_v50.0.uniq.root";
    std::string tree_name = "singlephotonana/vertex_tree";
    std::string pdf_name = "NCpi0_Proton_Distribution.pdf";
    //********** input file ******************

    TFile* fin = new TFile(fin_name.c_str(), "read");
    TTree* tree = (TTree*)fin->Get(tree_name.c_str());

    int reco_asso_tracks, reco_asso_showers, reco_vertex_size;
    int mctruth_num_exiting_protons,mctruth_num_daughter_particles;
    std::vector<int>* mctruth_daughters_pdg=nullptr;
    std::vector<int>* mctruth_daughters_status_code = nullptr;
    std::vector<double>* mctruth_daughters_E = nullptr;
    std::vector<double>* mctruth_daughters_startx = nullptr;
    std::vector<double>* mctruth_daughters_starty = nullptr;
    std::vector<double>* mctruth_daughters_startz = nullptr;


    // the reason why I look at sim_shower info instead of mctruth_daughter info for shower related information is that, for some event species, such as pi0 event, there is no exiting photons in the mctruth
    // ie. the photon comes from pi0 decay instead of something inside the nucleus
    std::vector<int>* sim_shower_pdg=nullptr;
    std::vector<double>* sim_shower_start_x = nullptr; //corrected photon end (shower start)
    std::vector<double>* sim_shower_start_y = nullptr;
    std::vector<double>* sim_shower_start_z = nullptr;
    std::vector<double>* sim_shower_vertex_x = nullptr;// corrected photon start 
    std::vector<double>* sim_shower_vertex_y = nullptr;
    std::vector<double>* sim_shower_vertex_z = nullptr;
    
    tree->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
    tree->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
    tree->SetBranchAddress("reco_vertex_size", &reco_vertex_size);
    tree->SetBranchAddress("mctruth_num_exiting_protons", &mctruth_num_exiting_protons);
    tree->SetBranchAddress("mctruth_num_daughter_particles", &mctruth_num_daughter_particles);
    tree->SetBranchAddress("mctruth_daughters_pdg", &mctruth_daughters_pdg);
    tree->SetBranchAddress("mctruth_daughters_E", &mctruth_daughters_E);
    tree->SetBranchAddress("mctruth_daughters_status_code", &mctruth_daughters_status_code);
    tree->SetBranchAddress("mctruth_daughters_startx", &mctruth_daughters_startx);
    tree->SetBranchAddress("mctruth_daughters_starty", &mctruth_daughters_starty);
    tree->SetBranchAddress("mctruth_daughters_startz", &mctruth_daughters_startz);
    tree->SetBranchAddress("sim_shower_pdg", &sim_shower_pdg);
    tree->SetBranchAddress("sim_shower_start_x", &sim_shower_start_x);
    tree->SetBranchAddress("sim_shower_start_y", &sim_shower_start_y);
    tree->SetBranchAddress("sim_shower_start_z", &sim_shower_start_z);
    tree->SetBranchAddress("sim_shower_vertex_x", &sim_shower_vertex_x);
    tree->SetBranchAddress("sim_shower_vertex_y", &sim_shower_vertex_y);
    tree->SetBranchAddress("sim_shower_vertex_z", &sim_shower_vertex_z);

    TH1D* h_SCB = new TH1D("h_SCB", "SCB corrected distance between true photon start, and photon; SCB corrected distance [cm]; entries", 100,0,100);
    TH1D* h_proton_dist = new TH1D("h_proton_dist", "Distance between true proton position and corrected shower start; Distance [cm]; entries", 200,0,200);

    for(int i = 0; i!=tree->GetEntries(); ++i){
	if(i%1000 == 0) std::cout << "On Entry " << i << "/" << tree->GetEntries() << std::endl;	
	tree->GetEntry(i);
	
        //only consider reco 1g0p events 
        if(!(reco_asso_tracks == 0 && reco_asso_showers == 1 && reco_vertex_size == 1))
	    continue;

	// only consider events with shoewr matched to photon, and have exiting protons
	if(!(sim_shower_pdg->at(0) == 22 && mctruth_num_exiting_protons > 0 ))
	    continue; 


	int num_proton_daughter = 0, proton_index = -1;
	int max_proton_E = 0;
   	for(int j =0; j!= mctruth_num_daughter_particles; ++j){
	    if(mctruth_daughters_pdg->at(j)!= 2212 || mctruth_daughters_status_code->at(j) != 1)
		continue;

	    ++num_proton_daughter;
	    if(mctruth_daughters_E->at(j) > max_proton_E){
		max_proton_E = mctruth_daughters_E->at(j);
		proton_index = j;
	    }
	}

	if(i % 1000==0 && num_proton_daughter > 1){
	    std::cout << "This event has " << num_proton_daughter << " exiting protons" << std::endl;
	}

	std::vector<double> photon_start = {sim_shower_vertex_x->at(0), sim_shower_vertex_y->at(0), sim_shower_vertex_z->at(0)};
	std::vector<double> shower_start = {sim_shower_start_x->at(0), sim_shower_start_y->at(0), sim_shower_start_z->at(0)};
	std::vector<double> proton_start = {mctruth_daughters_startx->at(proton_index), mctruth_daughters_starty->at(proton_index), mctruth_daughters_startz->at(proton_index)};
	double proton_photon_dist = two_point_index(photon_start, proton_start);
	double proton_shower_dist = two_point_index(shower_start, proton_start);
	h_SCB->Fill(proton_photon_dist);
	h_proton_dist->Fill(proton_shower_dist);	 
    } 

    TCanvas* c = new TCanvas("c", "c");
    c->Print((pdf_name+"[").c_str());
    gStyle->SetOptStat(0);
    h_SCB->Draw("hist");
    c->Print(pdf_name.c_str());
    c->Clear();
    h_proton_dist->Draw("hist");
    c->Print(pdf_name.c_str());
    c->Print((pdf_name+"]").c_str());
    return;
}
