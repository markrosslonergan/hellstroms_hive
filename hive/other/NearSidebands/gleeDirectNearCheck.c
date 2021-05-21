void gleeDirectNearCheck(std::string filein){
    
    TFile *f = new TFile(filein.c_str(),"read");
    if(f->IsOpen()){
            std::cout<<"Opened "<<filein<<" successfully"<<std::endl;
    }else{
            std::cout<<"ERROR! Failed to pen filein: "<< filein<<std::endl;
            return;
    }

    TTree *tvert = (TTree*)f->Get("singlephoton/vertex_tree");
    TTree *tsimple = (TTree*)f->Get("singlephoton/simple_tree");

    std::cout<<"Loaded simpletree and vertex_tree. Checking to see if same number of events. "<<std::endl;
    int diff = tvert->GetEntries()-tsimple->GetEntries();
    if(diff==0){
            std::cout<<"***Success***, simple_tree and vertex_tree have same numbers of events."<<std::endl;
    }else{
            std::cout<<"!!!ERROR!!! simple_tree and vertex_tree are different. Something is broken."<<std::endl;
            return;
    }

    std::string NEARCUT = "(simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404) &&  simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)";
    std::string FAILNEARCUT = "!(simple_1g1pMar2020_v4Nue_mva>= 0.3 && (simple_1g1pMar2020_v4BNB_mva >= 0.96273 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404) &&  simple_1g1pMar2020_v4SSVredo2wCC600parent_mva < 0.55)";

    //0.9531,0.9845,0.467404,0.7474,0.7088
    std::string FINALCUT = "(simple_1g1pMar2020_v4COSMIC_mva >= 0.9531 && simple_1g1pMar2020_v4BNB_mva>=0.9845 && simple_1g1pMar2020_v4NCPi0_mva >= 0.467404 && simple_1g1pMar2020_v4Nue_mva>= 0.7474 &&  simple_1g1pMar2020_v4SSVredo2wCC600parent_mva >= 0.7088)";


     std::cout<<"Checking if any events fail the nearsideband cuts:  "<<NEARCUT<<std::endl;
     int n_failcuts = tsimple->GetEntries((FAILNEARCUT).c_str());
     if(n_failcuts==0){
         std::cout<<"***Success***, no events in simple_tree fail the nearsideband cuts!"<<std::endl;
     }else{
         std::cout<<"!!!ERROR!!!, There are some events in simple_tree that fail the nearsideband cuts."<<std::endl;
     }


     std::cout<<"Checking if any events pass the Full Final selection cuts:  "<<FINALCUT<<std::endl;
     int n_finalcuts = tsimple->GetEntries((FINALCUT).c_str());
     if(n_finalcuts==0){
         std::cout<<"***Success***, no events in simple_tree pass the final selection cuts!"<<std::endl;
     }else{
         std::cout<<"!!!ERROR!!!, There are some events in simple_tree that pass the finalselection cuts."<<std::endl;
     }


     std::cout<<"Checking if trees are filled at all, i.e ensure trees dont have 0 events. "<<std::endl;
     int n_simple = tsimple->GetEntries();
     if(n_simple==0){
         std::cout<<"!!!ERROR!!!, There are no events in simple_tree. That is very unlikely and suggests an error somewhere."<<std::endl;
     }else{
         std::cout<<"***Success***, simple_tree has a non-zero amount of entries."<<std::endl;
     }

       

    return;
}
