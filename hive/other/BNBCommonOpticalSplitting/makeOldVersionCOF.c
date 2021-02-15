void makeOldVersionCOF(){


    std::cout<<"This is NOT NEEDED read run.sh."<<std::endl;


    std::string filename  = "/uboone/data/users/markross/Mar2020/bnb_overlay_run3a_Extra_v43.5.root";

    TFile oldfile(filename.c_str(),"read");
    
    TFile newfile("/uboone/data/users/markross/Mar2020/tmp/small2.root", "recreate");
    TDirectory *cdtof = newfile.mkdir("singlephotonana");
    cdtof->cd();    


    TTree *oldtree = (TTree*)oldfile.Get("singlephotonana/vertex_tree");


    TTree *pottree = (TTree*)oldfile.Get("singlephotonana/pot_tree");
    TTree *evetree = (TTree*)oldfile.Get("singlephotonana/eventweight_tree");
    TTree *ncdeltree = (TTree*)oldfile.Get("singlephotonana/ncdelta_slice_tree");
    TTree *rsree = (TTree*)oldfile.Get("singlephotonana/run_subrun_tree");

    // DeActivate only four of them
    oldtree->SetBranchStatus("*flash_opt*", 0);
    oldtree->SetBranchStatus("*photonu_weight*",0);
    // Create a new file + a clone of old tree in new file
    auto newtree = oldtree->CloneTree(-1,"fast");

    newfile.cd();
    cdtof->cd();    

    newtree->Write();
    pottree->Write();
    evetree->Write();
    ncdeltree->Write();
    rsree->Write();

    newfile.Close();


    return;
}
