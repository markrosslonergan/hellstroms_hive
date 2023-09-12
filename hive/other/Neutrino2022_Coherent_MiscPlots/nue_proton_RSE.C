/****
 * specific script that read the flat PSV file and identifies run,subrun and event list for events with truth-matched proton clusters,
 * then read the vertex file, and grabs the true nue events that has exiting protons
 *
 * Guanqun, Feb 23 2022
 */

struct RSE{
public:
//members
     int run, subrun, event;
     RSE(int inrun, int insubrun, int inevent):run(inrun), subrun(insubrun), event(inevent){}


     bool operator<(const RSE& eve) const{
	return run < eve.run || (run == eve.run && subrun < eve.subrun) || (run == eve.run && subrun ==eve.subrun && event < eve.event);
     }

     std::ofstream& Write(std::ofstream& of) const{
	of << run << " " << subrun << " " << event << "\n";
	return of;
     }     
};

bool run_cut(int run_num){
   return run_num >= 4952 && run_num <= 7770;
}

void nue_proton_RSE(){

     //********* file names ***************
     //flat cluster level
     std::string flat_file = "/pnfs/uboone/scratch/users/markross/FlatTest3/FLATTEN_NueOverlays.root";
     std::string flat_tree = "PSV";

     std::string BDTapp_file = "/uboone/app/users/gge/hellstroms_hive/hive/working_directory/Jan2022_1g0p/new_coherent_sp/PSV_Test_v2/1g0pCohFeb2022PSV_NueOverlaysFLAT_app.root";
     std::string BDTapp_tree = "NueOverlaysFLAT_1g0pCohFeb2022PSV";
     std::string BDT_mva = "1g0pCohFeb2022PSV_mva";


     // vertex level
     std::string vertex_file = "/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/Precut2Topo/TestTrainSplit_Jan24_2022/preTopo_vertex_Nue_Run123_TestSplit_v50.0.root";
     std::string vertex_tree = "singlephotonana/vertex_tree";

     //output txt file with RSE
     std::string txt_file = "NUE_Eventlist_VisibleProton.txt";

     //********* file names ***************
     

     std::set<RSE> elist_protonstub; // list of events which passed preselection cut and have truth-matched proton cluster
     std::set<RSE> elist_exitproton; // subset of elist_protonstub which has truth exiting protons



     //********* grab event list with truth-matched proton stub **********************

     TFile* cluster_file = new  TFile(flat_file.c_str(), "read");
     TTree* cluster_tree = (TTree*)cluster_file->Get(flat_tree.c_str());
     cluster_tree->AddFriend(BDTapp_tree.c_str(), BDTapp_file.c_str());
     
     double drun_number, dsubrun_number, devent_number;
     int precut;
     double cluster_matched_pdg, bdt_score;

     cluster_tree->SetBranchAddress("run_number", &drun_number);
     cluster_tree->SetBranchAddress("subrun_number", &dsubrun_number);
     cluster_tree->SetBranchAddress("event_number", &devent_number);
     cluster_tree->SetBranchAddress("flat_cut", &precut);
     cluster_tree->SetBranchAddress("trackstub_candidate_pdg", &cluster_matched_pdg);
     cluster_tree->SetBranchAddress(BDT_mva.c_str(), &bdt_score);

     for(int i = 0; i!= cluster_tree->GetEntries(); ++i){
	 if(i%10000 ==0) std::cout << "On Entry " << i << "/" << cluster_tree->GetEntries() << std::endl;
         cluster_tree->GetEntry(i);

	 //only consider run1 evenrs
	 if(!run_cut(drun_number)) continue;

	 // only consider truth-matched proton cluster with low BDT score
	 if(!(precut == 1 && cluster_matched_pdg == 2212 && bdt_score < 0.5))
	     continue;

	 RSE curr_event(drun_number, dsubrun_number, devent_number);
	 if(elist_protonstub.find(curr_event) == elist_protonstub.end())
	     elist_protonstub.insert(curr_event);
     }
     cluster_file->Close();
     std::cout << "Total events with truth proton clsuters: " << elist_protonstub.size() << std::endl;
     //********* grab event list with truth-matched proton stub **********************
     
     //******** further reduce to events with truth exiting protons *****************
     TFile* event_file = new  TFile(vertex_file.c_str(), "read");
     TTree* event_tree = (TTree*)event_file->Get(vertex_tree.c_str());

     int run_number, subrun_number, event_number;
     int num_exiting_protons;
     double leading_exiting_proton_E;
     std::vector<int>* sim_shower_pdg = nullptr;
     event_tree->SetBranchAddress("mctruth_leading_exiting_proton_energy", &leading_exiting_proton_E);
     event_tree->SetBranchAddress("mctruth_num_exiting_protons", &num_exiting_protons);
     event_tree->SetBranchAddress("sim_shower_pdg", &sim_shower_pdg);
     event_tree->SetBranchAddress("run_number", &run_number);
     event_tree->SetBranchAddress("subrun_number", &subrun_number);
     event_tree->SetBranchAddress("event_number", &event_number);

     for(int i = 0; i!= event_tree->GetEntries(); ++i){
	 if(i%1000 ==0) std::cout << "On Entry " << i << "/" << event_tree->GetEntries() << std::endl;
	event_tree->GetEntry(i);

        if(sim_shower_pdg->size() == 0 || abs(sim_shower_pdg->at(0)) != 11 || num_exiting_protons == 0 || leading_exiting_proton_E - 0.938272 < 0.05)
	    continue;

	RSE curr_event(run_number, subrun_number, event_number);
	if(elist_protonstub.find(curr_event) != elist_protonstub.end()){
	    elist_exitproton.insert(curr_event);
	}
     }
     std::cout << "Total events with truth proton clsuters and truth exiting proton: " << elist_exitproton.size() << std::endl;

     std::ofstream txt_output(txt_file);
     for(auto& e : elist_exitproton)
	e.Write(txt_output);
     txt_output.close();

    return;
}
