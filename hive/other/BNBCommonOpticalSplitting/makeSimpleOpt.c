void makeSimpleOpt(){
   
    std::string dir = "/uboone/data/users/markross/Mar2020/";
    //std::string new_file = "bnb_overlay_combined_run13_Extra_v43.5.root";
    //std::string new_file = "bnb_overlay_run1_Extra_v43.5.root";
    std::string new_file = "bnb_overlay_run3_Extra_v43.5.root";

    std::string out_name = dir+"OpticalMatch_"+new_file;
    TFile *f_new =new TFile((dir+new_file).c_str());
    TFile * outfile = new TFile(out_name.c_str(),"recreate");//need to get better name for this

    TTree * new_vertex_tree = (TTree*)f_new->Get("singlephotonana/vertex_tree");

    new_vertex_tree-> SetBranchStatus("*",0);
    new_vertex_tree-> SetBranchStatus("run_number",1);
    new_vertex_tree-> SetBranchStatus("subrun_number",1);
    new_vertex_tree-> SetBranchStatus("event_number",1);
    new_vertex_tree-> SetBranchStatus("mctruth_nu_E",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_beam",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_veto",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_veto_tot",1);
    new_vertex_tree-> SetBranchStatus("m_flash_optfltr_pe_beam_tot",1);

    int new_run_number;
    int new_subrun_number;
    int new_event_number;
    double new_flash_optfltr_pe_beam;
    double new_flash_optfltr_pe_veto;
    double new_flash_optfltr_pe_veto_tot;
    double new_flash_optfltr_pe_beam_tot;
    double new_true_nu_e;

    new_vertex_tree->SetBranchAddress("run_number",    &new_run_number);
    new_vertex_tree->SetBranchAddress("subrun_number", &new_subrun_number);
    new_vertex_tree->SetBranchAddress("event_number",  &new_event_number);
    new_vertex_tree->SetBranchAddress("mctruth_nu_E",  &new_true_nu_e);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_beam",  &new_flash_optfltr_pe_beam);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_veto",  &new_flash_optfltr_pe_veto);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_beam_tot",  &new_flash_optfltr_pe_beam_tot);
    new_vertex_tree->SetBranchAddress("m_flash_optfltr_pe_veto_tot",  &new_flash_optfltr_pe_veto_tot);



    TTree* friend_tree = new TTree("opt_tree", "opt_tree");

    double m_flash_optfltr_pe_beam;
    double m_flash_optfltr_pe_veto;
    double m_flash_optfltr_pe_veto_tot;
    double m_flash_optfltr_pe_beam_tot;

    friend_tree->Branch("m_flash_optfltr_pe_beam",&m_flash_optfltr_pe_beam,"flash_optfltr_pe_beam/D");
    friend_tree->Branch("m_flash_optfltr_pe_veto",&m_flash_optfltr_pe_veto,"flash_optfltr_pe_veto/D");
    friend_tree->Branch("m_flash_optfltr_pe_beam_tot",&m_flash_optfltr_pe_beam_tot,"flash_optfltr_pe_beam_tot/D");
    friend_tree->Branch("m_flash_optfltr_pe_veto_tot",&m_flash_optfltr_pe_veto_tot,"flash_optfltr_pe_veto_tot/D");


    for (int i=0;i <new_vertex_tree->GetEntries(); i++){

        new_vertex_tree->GetEntry(i);
        m_flash_optfltr_pe_beam = new_flash_optfltr_pe_beam;
        m_flash_optfltr_pe_veto = new_flash_optfltr_pe_veto;
        m_flash_optfltr_pe_beam_tot = new_flash_optfltr_pe_beam_tot;
        m_flash_optfltr_pe_veto_tot = new_flash_optfltr_pe_veto_tot;
        friend_tree->Fill();

    }

    outfile->cd();
    friend_tree->Write();
    outfile->Close();


    return;

}
