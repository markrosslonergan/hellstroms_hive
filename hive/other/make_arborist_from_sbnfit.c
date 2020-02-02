void make_arborist_from_sbnfit(){

    std::string s1 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/SBNfit/v1_technote/2g1p/sbnfit_pigLEE_Run1_C1_stage_1_Dirt.root";
    std::string a1 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/SBNfit/v1_technote/2g1p/arborist_pigLEE_Dirt_run1_stage1_v33.0.root";

    std::string s2 = "/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/SBNfit/v1_technote/2g1p/sbnfit_pigLEE_Run1_C1_stage_2_Dirt.root";
    std::string a2 = "test.root";



    // ############ Stage 2 ##############
    TFile *f_sbnfit_stage_1   = new TFile(s1.c_str(),"read");// This is parent
    TTree *t_vertex_stage_1 = (TTree*)f_sbnfit_stage_1->Get("singlephoton/vertex_tree");//Will get run/subrun/event from here
    int run1 = 0;
    int subrun1 = 0;
    int event1 = 0;
    t_vertex_stage_1->SetBranchAddress("run_number",&run1);
    t_vertex_stage_1->SetBranchAddress("subrun_number",&subrun1);
    t_vertex_stage_1->SetBranchAddress("event_number",&event1);


    TFile *f_arborist_stage_1 = new TFile(a1.c_str(),"read");//This is parent arbor
    TTree *t_mcweight_stage_1 = (TTree*)f_arborist_stage_1->Get("arborist/eventweight_tree");// This is what we extract
    
    std::map<std::string,std::vector<double>> *inwei=0;
    t_mcweight_stage_1->SetBranchAddress("mcweight",&inwei);


    // ############ Stage 2 ##############
    TFile *f_sbnfit_stage_2   = new TFile(s2.c_str(),"read");// This is daughter, to read which run/subrun/event to save
    TTree *t_vertex_stage_2 = (TTree*)f_sbnfit_stage_2->Get("singlephoton/vertex_tree");//
    int run2 = 0;
    int subrun2 = 0;
    int event2 = 0;
    t_vertex_stage_2->SetBranchAddress("run_number",&run2);
    t_vertex_stage_2->SetBranchAddress("subrun_number",&subrun2);
    t_vertex_stage_2->SetBranchAddress("event_number",&event2);

   
    TFile *f_arborist_stage_2 = new TFile(a2.c_str(),"recreate");//This is daughter to WRITE
    f_arborist_stage_2->cd();
    TDirectory *cdtof = f_arborist_stage_2->mkdir("arborist");
    cdtof->cd();    
    TTree *t_mcweight_stage_2 = new TTree("eventweight_tree","eventweight_tree"); // This is what we will write
    std::map<std::string,std::vector<double>> *outwei=0;
    t_mcweight_stage_2->Branch("mcweight",&outwei);

 
    //When we find a match, remove it from the searchable indicies
    std::vector<size_t> original_index;
    for(int i=0; i< t_vertex_stage_1->GetEntries(); i++) original_index.push_back(i);

    for(int i=0; i< t_vertex_stage_2->GetEntries(); i++){
        t_vertex_stage_2->GetEntry(i);
        //Get run2,subrun2,event2
        std::cout<<"On "<<i<<"/"<<t_vertex_stage_2->GetEntries()<<" || "<<run2<<" "<<subrun2<<" "<<event2<<" ScanSize: "<<original_index.size()<<std::endl;
        bool is_found = false;

        //Find that run,subrun,event in parent
        for(int o=0; o< original_index.size(); o++){
            size_t j = original_index[o];
            t_vertex_stage_1->GetEntry(j);
            //Get run/subrun/event

            //If match, break
            if(run1==run2 && subrun1==subrun2 && event1==event2){
                is_found = true;
                t_mcweight_stage_1->GetEntry(j);//Sets inwei     
                original_index.erase(original_index.begin()+o);
                break;
            }

        }
        if(!is_found){
            //massive error
            std::cout<<"ERROR!!!! Didnt find : "<<run2<<" "<<subrun2<<" "<<event2<<std::endl;
            return;
        }

        outwei = inwei;
        t_mcweight_stage_2->Fill();
    }


    cdtof->cd();    
    t_mcweight_stage_2->Write();
    f_arborist_stage_2->Close();



    return;
}
