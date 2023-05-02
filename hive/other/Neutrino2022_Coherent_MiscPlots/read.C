void read(){
   TFile* f= new TFile("/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/Precut2Topo/preTopo_vertex_Data_Open5e19_Run1_v50.02.uniq.root", "read");

    TTree* t= (TTree*)f->Get("singlephotonana/vertex_tree");
    int run, subrun, event;
    t->SetBranchAddress("run_number", &run);
    t->SetBranchAddress("subrun_number", &subrun);
    t->SetBranchAddress("event_number", &event);

    std::set<std::vector<int>> seen; 
    for(int i =0 ; i != t->GetEntries(); ++i){
	t->GetEntry(i);
	std::cout << run << " " << subrun << " " << event << std::endl;
        if(seen.find({run, subrun, event}) != seen.end()){
	    std::cout << " Find duplicates " << std::endl;
	}
	seen.insert({run, subrun, event});
    }

    f->Close();
    return;
}
