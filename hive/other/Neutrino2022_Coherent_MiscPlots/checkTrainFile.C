/* This module plots the overlay fractions of the training files, to make sure the training events are good events
 *
 * Guanqun, Feb 07 2022
 */
void checkTrainFile(){
    std::string event_type = "NC pi0 events";
    std::string event_def = "reco_asso_tracks ==0 && reco_asso_showers==1 && (!mctruth_is_delta_radiative==1) && (mctruth_cc_or_nc==1 && mctruth_num_exiting_pi0==1) && (m_flash_optfltr_pe_beam > 20 && m_flash_optfltr_pe_veto < 20)";
    TFile* fin = new TFile("/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/Precut2Topo/TestTrainSplit_Jan24_2022/preTopo_vertex_NCPi0_Run123_TrainSplit_v50.0.uniq.root");
    TFile* fout = new TFile("NCpi0Train_res_v1.root", "recreate");

    TTree* vertex_tree = (TTree*)fin->Get("singlephotonana/vertex_tree");
    vertex_tree->AddFriend("singlephotonana/eventweight_tree", fin);
  
    int reco_vertex_size; 
    int reco_asso_tracks, reco_asso_showers; 
    int mctruth_is_delta_radiative, mctruth_cc_or_nc, mctruth_num_exiting_pi0;
    double m_flash_optfltr_pe_beam, m_flash_optfltr_pe_veto;
    double true_nu_posX, true_nu_posY, true_nu_posZ;
    std::vector<double> *sim_shower_matched = NULL, *sim_shower_overlay_fraction=NULL, *sim_shower_energy=NULL;
    std::vector<double> *reco_shower_energy_max=NULL;
    std::vector<int> *sim_shower_pdg=NULL; 

    vertex_tree->SetBranchAddress("MCFlux_NuPosX", &true_nu_posX);
    vertex_tree->SetBranchAddress("MCFlux_NuPosY", &true_nu_posY);
    vertex_tree->SetBranchAddress("MCFlux_NuPosZ", &true_nu_posZ);
    vertex_tree->SetBranchAddress("sim_shower_matched", &sim_shower_matched);
    vertex_tree->SetBranchAddress("sim_shower_pdg", &sim_shower_pdg);
    vertex_tree->SetBranchAddress("sim_shower_overlay_fraction", &sim_shower_overlay_fraction);
    vertex_tree->SetBranchAddress("sim_shower_energy", &sim_shower_energy);
    vertex_tree->SetBranchAddress("reco_shower_energy_max", &reco_shower_energy_max);
    vertex_tree->SetBranchAddress("reco_vertex_size", &reco_vertex_size);
    vertex_tree->SetBranchAddress("reco_asso_tracks", &reco_asso_tracks);
    vertex_tree->SetBranchAddress("reco_asso_showers", &reco_asso_showers);
    vertex_tree->SetBranchAddress("mctruth_is_delta_radiative", &mctruth_is_delta_radiative);
    vertex_tree->SetBranchAddress("mctruth_cc_or_nc", &mctruth_cc_or_nc);
    vertex_tree->SetBranchAddress("mctruth_num_exiting_pi0", &mctruth_num_exiting_pi0);
    vertex_tree->SetBranchAddress("m_flash_optfltr_pe_beam",&m_flash_optfltr_pe_beam);
    vertex_tree->SetBranchAddress("m_flash_optfltr_pe_veto", &m_flash_optfltr_pe_veto);

    TH1D* h_true_energy = new TH1D("h_true_shower_energy", "Truth Matched Shower Energy; Truth Energy [MeV]", 400, 0, 800);
    TH1D* h_reco_energy = new TH1D("h_reco_shower_energy", "Reconstructed Shower Energy; Reconstructed Energy [MeV]", 400, 0, 800);
    TH2D* h_posXY = new TH2D("h_posXY", "True vertex position in  XY; pos X; pos Y", 420, -70, 350, 400, -200, 200); 
    TH2D* h_posXZ = new TH2D("h_posXZ", "True vertex position in  XZ; pos X; pos Z", 420, -70, 350, 1400, -200, 1200); 
    TH2D* h_posYZ = new TH2D("h_posYZ", "True vertex position in  YZ; pos Y; pos Z", 400, -200, 200, 1400, -200, 1200); 
    TH2D* h_overlay_matched = new TH2D("h_overlay_matched", "Shower overlay fraction vs IsMatched; overlay fraction; matched", 100, 0, 1, 3, -1, 2);
    TH1D* h_matched_shower_pdg = new TH1D("h_matched_shower_pdg", "Matched PDG for shower; PDG; entries", 4000,0, 4000);
    TH2D* h_overlay_energymismatch = new TH2D("h_overlay_energymismatch", "Shower overlay fraction vs Energy differene; overlay fraction; truth energy - reco energy/truth energy", 100, 0, 1, 60, -10, 5);
    TH2D* h_pe = new TH2D("h_pe_2D", "flash optical filter; m_flash_optfltr_pe_beam; m_flash_optfltr_pe_veto", 500,0 ,1000, 50, 0, 100);

    for(int i = 0; i != vertex_tree->GetEntries(); ++i){
	if(i%10000 == 0) std::cout << "On entry: " << i << "/" << vertex_tree->GetEntries() << std::endl;

	// skip this entry if it doesn't satisfy the event definition
	//if(vertex_tree->Query("", event_def.c_str(), "", 1, i)->GetRowCount() == 0) continue;

	vertex_tree->GetEntry(i);
        //skip this entry if it doesn't satisfy event definition
        if(!(reco_vertex_size == 1 && reco_asso_tracks ==0 && reco_asso_showers==1 && (mctruth_is_delta_radiative==0) && (mctruth_cc_or_nc==1 && mctruth_num_exiting_pi0==1))) 
        //if(!(reco_vertex_size == 1 && reco_asso_tracks ==0 && reco_asso_showers==1 && (mctruth_is_delta_radiative==0) && !(mctruth_cc_or_nc==1 && mctruth_num_exiting_pi0==1))) 
        //if(!(reco_vertex_size == 1 && reco_asso_tracks ==0 && reco_asso_showers==1)) 
	    continue; 

	h_posXY->Fill(true_nu_posX, true_nu_posY);
	h_posXZ->Fill(true_nu_posX, true_nu_posZ);
	h_posYZ->Fill(true_nu_posY, true_nu_posZ);	
	h_pe->Fill(m_flash_optfltr_pe_beam,m_flash_optfltr_pe_veto);
	h_reco_energy->Fill(reco_shower_energy_max->at(0));
	h_overlay_matched->Fill(sim_shower_overlay_fraction->at(0),sim_shower_matched->at(0));
	if(sim_shower_matched->at(0)){
	   h_matched_shower_pdg->Fill(sim_shower_pdg->at(0));
	   h_overlay_energymismatch->Fill(sim_shower_overlay_fraction->at(0), (sim_shower_energy->at(0) - reco_shower_energy_max->at(0)/1000)/sim_shower_energy->at(0));
	   if(sim_shower_overlay_fraction->at(0) < 0.5) h_true_energy->Fill(sim_shower_energy->at(0)*1000);
	}
    }

    fout->cd();
    h_posXY->Write();
    h_posXZ->Write();
    h_posYZ->Write();
    h_pe->Write();
    h_overlay_matched->Write();
    h_matched_shower_pdg->Write();
    h_overlay_energymismatch->Write();
    h_reco_energy->Write();
    h_true_energy->Write();

    fin->Close();
    fout->Close();
    return;
}
