/* Investigate the difference in samples before and after NCpi0 BDT and PSV BDT cut, to see how to define unblinding strategy
 */
void Unblind_Invest(){

    std::string filepath="/uboone/app/users/gge/hellstroms_hive/hive/working_directory/Jan2022_1g0p/new_coherent_sp/TestOptimization/";
    std::vector<std::string> filenames={"NCSingleCoherentTestTextGen", "NCDeltaRadOverlaySMTrueP", "NCDeltaRadOverlaySMTrueN",
					"NCPi0Coh", "NCPi0NotCoh", "CC1Pi0", "BNBOtherExtra", "NueOverlays", "Dirt", "OTPCExtra",
					"BNBext"};

    std::vector<std::string> stages={"stage_4", "stage_5"};


    for(int i = 0 ; i != filenames.size(); ++i){
	for(int s = 0; s != stages.size(); ++s){
	    std::string curr_file = filepath + "sbnfit_1g0pCohNeutrino2022_" + stages[s] + "_" + filenames[i] + ".root";
	    std::cout << "\nCurrently on file " << curr_file << std::endl;

	    TFile* tf = new TFile(curr_file.c_str(), "read");
  	    TTree* tt = (TTree*)tf->Get("singlephoton/vertex_tree");
	    tt->AddFriend("singlephoton/simple_tree");
	    tt->AddFriend("singlephoton/unflatten_1g0pCohNeutrino2022PSV");

	    // add branch address
	    double simple_pot_weight;
	    double NCpi0_BDT_score;
	    double max_PSV_P2;
	    int mctruth_num_exiting_protons;
            tt->SetBranchAddress("mctruth_num_exiting_protons", &mctruth_num_exiting_protons);
	    tt->SetBranchAddress("simple_pot_weight", &simple_pot_weight);
	    tt->SetBranchAddress("simple_1g0pCohNeutrino2022NCPi0_mva", &NCpi0_BDT_score);
	    TTreeFormula* tform = new TTreeFormula("form", "Max$((trackstub_candidate_plane==2)*1g0pCohNeutrino2022PSV_score)", tt);
	 
	    double total_count = 0, pass_PSV_count = 0, proton_count = 0;
	    for(int ie = 0 ; ie != tt->GetEntries(); ++ie){
		tt->GetEntry(ie);
		tform->GetNdata();
		max_PSV_P2 = tform->EvalInstance();

		total_count += simple_pot_weight;
		if(max_PSV_P2 <= 0.065)
		    pass_PSV_count += simple_pot_weight;

	        //count number of proton events 
		//if(filenames[i] != "BNBext" && mctruth_num_exiting_protons > 0 ) proton_count += simple_pot_weight;
		if(filenames[i] != "BNBext" && mctruth_num_exiting_protons > 0 && max_PSV_P2 <= 0.065) proton_count += simple_pot_weight;
	    }

	    std::cout << "At " << stages[s] << ", file " << filenames[i] << " has in total " << total_count << " events; " << pass_PSV_count << " events pass PSV cut; number of proton events passing PSV cut " << proton_count << ", fraction: " << proton_count/pass_PSV_count*100 << std::endl;

	}
    }
    return;
}
