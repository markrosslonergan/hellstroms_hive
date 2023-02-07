#ifndef DUP_SUI_H
#define DUP_SUI_H

#include <vector> 
#include "TRandom3.h"
#include "TTree.h"
#include "TFile.h"
#include <string> 
#include <set> 
#include <numeric> 
#include <iostream> 
#include <algorithm>
#include <map>
#include <memory>
struct RSE{

    //simple structure to store run,subrun,events, will be expanded to pot and so firth
    vector<int> f_rse;
    RSE(int r, int s, int e){
        f_rse.resize(3);
        f_rse[0]=r;
        f_rse[1]=s;
        f_rse[2]=e;
    }

    //some comparative operators, for mapping and such. What does it mean for a RSE to be < = or > than another RSE,
    bool operator==( const RSE& rhs)
    {
        return (this->f_rse[0]==rhs.f_rse[0] &&this->f_rse[1]==rhs.f_rse[1] && this->f_rse[2]==rhs.f_rse[2] );
    }

    bool operator<(const RSE& X) const {
        if(this->f_rse[0]>X.f_rse[0]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]>X.f_rse[1]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]==X.f_rse[1] && this->f_rse[2]>=X.f_rse[2]) return false;
        return true;
    }

    bool operator>(const RSE& X) const {
        if(this->f_rse[0]<X.f_rse[0]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]<X.f_rse[1]) return false;
        if(this->f_rse[0]==X.f_rse[0] && this->f_rse[1]==X.f_rse[1] && this->f_rse[2]<=X.f_rse[2]) return false;
        return true;
    }

    int Print(){
        std::cout<<f_rse[0]<<" "<<f_rse[1]<<" "<<f_rse[2]<<std::endl;
        return 0;
    }

};


struct runlist{
    //Although a vector of RSE is fine, want some structur for counting, maps and intersection, union..etc..

    public:
        std::vector<RSE> f_rses; 

        std::map<int,bool> m_runs;
        std::map<int,bool> m_subruns;
        std::map<int,bool> m_events;

        //This map will tell us if a given RSE is in this runlist
        std::map<RSE,int> m_rses;
        //This map will tell us if a given RS is in this runlist
        std::map<RSE,int> m_rss;



        //Is this R S in this?
        int inList(int &run, int &subrun){
            RSE this_rse(run,subrun,-1); 

            if(m_runs.count(run)>0){
                if(m_rss.count(this_rse)>0){
                    return m_rss[this_rse];
                }
            }
            return 0;
        }

        //Is this r, s r in this?
        bool inList(int &run, int &subrun, const int &event){

            if(event<0) return inList(run,subrun);

            RSE this_rse(run,subrun,event); 
            if(m_runs.count(run)>0 && m_subruns.count(subrun)>0){
                return m_rses[this_rse];
            }
            return 0;
        }

        //Constructor from the vector of RSE
        runlist(std::vector<RSE> rses){
            f_rses = rses;

            for(auto &rse: rses){

                m_rses[rse] = ( m_rses.count(rse)==0 ? 1 : m_rses.count(rse)+1 );

                m_runs[rse.f_rse[0]] = true;
                m_subruns[rse.f_rse[1]] = true;
                m_events[rse.f_rse[2]] = true;

                RSE rs = rse;rs.f_rse[2]=-1;
                m_rss[rs] = true;
            }
        }



        //returns the intersection of the runlist R and the current runlist. Might be 0.
        runlist Intersection(const  runlist &  R){

            std::vector<RSE> ans;

            for(auto &e: R.f_rses){
                if(m_rses.count(e)>0) ans.push_back(e);
            }

            runlist Rans(ans);
            return Rans;
        }

        

        runlist checkDuplicates(){

            std::vector<RSE> ans;

            double dup = 0;
            for(auto &r: f_rses){
                if(m_rses[r]>1){
                    r.Print();
                    std::cout<<m_rses[r]<<std::endl;
                    dup+=1;
                }
            }

            std::cout<<"Total Duplicate Entries "<<dup<<" / "<<f_rses.size()<<std::endl;

            runlist Rans(ans);
            return Rans;

        }

        //This bit is new
        runlist getSubSet(double per){
            std::vector<RSE> ans;
            std::map<RSE,bool> tmp_rss;
            TRandom3 * rang = new TRandom3(0);

            for(auto &rse: f_rses){
                int run = rse.f_rse[0];
                int subrun = rse.f_rse[1];
                RSE rs(run,subrun,-1000);

                if( tmp_rss.count(rs)==1){//have we looked at this subrun before
                    if(tmp_rss[rs]){//is it in the SubSet
                        ans.emplace_back(run,subrun,rse.f_rse[2]);
                    }else{//its not
                        //nothing happends then. 
                    }

                }else{//Lets choose to accept or reject
                    double ran = rang->Uniform(0,1);
                    if(ran>per){
                        tmp_rss[rs] = true;
                        ans.emplace_back(run,subrun,rse.f_rse[2]);
                    }else{
                        tmp_rss[rs] = false;
                    }


                }//end of first pass

            }//end of f_rses loop

            runlist Rans(ans);

            return Rans;
        }



        //Just prints them simply
        int Print(){
            for(auto &e: f_rses){
                e.Print();
            }
            return 0;
        }


};

//Given a filename and treename (with directory) returns the simple vector of RSE
std::vector<RSE> getRunSubrunList(std::string filename, std::string treename){
    std::vector<RSE> ans;

    TFile *file = new TFile(filename.c_str(), "read");
    TTree *tree = (TTree*)file->Get(treename.c_str());

    int run_number = 0;
    int subrun_number = 0;
    int event_number = 0;

    //Set addresses, only need the 3 active here nicely
    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus("run_number",1);
    tree->SetBranchStatus("subrun_number",1);
    tree->SetBranchStatus("event_number",1);

    tree->SetBranchAddress("run_number",&run_number);
    tree->SetBranchAddress("subrun_number",&subrun_number);
    tree->SetBranchAddress("event_number",&event_number);

    //Loop over old tree, check condition is passed, if so fill the new_vtree
    for (size_t i=0;i<tree->GetEntries(); i++) {
        tree->GetEntry(i);
        ans.emplace_back(run_number,subrun_number,event_number);
    }

    file->Close();

    return ans;
}

//Given a filename and treename (with directory) returns the simple vector of RS and E but E isnt important
std::vector<RSE> getRunSubrunList_v2(std::string filename, std::string treename){
    std::vector<RSE> ans;

    TFile *file = new TFile(filename.c_str(), "read");
    TTree *tree = (TTree*)file->Get(treename.c_str());

    int run_number = 0;
    int subrun_number = 0;
    int event_number = 0;

    //Set addresses, only need the 3 active here nicely
    tree->SetBranchStatus("*",0);
    tree->SetBranchStatus("run",1);
    tree->SetBranchStatus("subrun",1);

    tree->SetBranchAddress("run",&run_number);
    tree->SetBranchAddress("subrun",&subrun_number);

    //Loop over old tree, check condition is passed, if so fill the new_vtree
    for (size_t i=0;i<tree->GetEntries(); i++) {
        tree->GetEntry(i);
        ans.emplace_back(run_number,subrun_number,event_number);
    }

    file->Close();

    return ans;
}



void RunSBNFITCloner(runlist &rlist, std::string input, std::string output, bool pos_or_neg, bool allow_duplicates,std::string indir = "singlephoton") {
    //Get old file, old tree and set top branch address
    std::string nam = input;
    std::cout<<"Reading old ttrees"<<std::endl;
    
    TFile *oldfile = new TFile(nam.c_str(), "read");

    TTree *old_vtree = (TTree*)oldfile->Get((indir+"/vertex_tree").c_str());
    TTree *old_etree = (TTree*)oldfile->Get((indir+"/eventweight_tree").c_str());
    TTree *old_stree = (TTree*)oldfile->Get((indir+"/ncdelta_slice_tree").c_str());
    TTree *old_ptree = (TTree*)oldfile->Get((indir+"/pot_tree").c_str());
    TTree *old_rtree = (TTree*)oldfile->Get((indir+"/run_subrun_tree").c_str());
    TTree *old_simtree = (TTree*)oldfile->Get((indir+"/simple_tree").c_str());

    int run_number = 0;
    int subrun_number = 0;
    int event_number = 0;

    std::cout<<"Setting brs"<<std::endl;
    //Set addresses
    old_vtree->SetBranchAddress("run_number",&run_number);
    old_vtree->SetBranchAddress("subrun_number",&subrun_number);
    old_vtree->SetBranchAddress("event_number",&event_number);
    //Activate all brances
    old_vtree->SetBranchStatus("*",1);
    old_etree->SetBranchStatus("*",1);
    old_stree->SetBranchStatus("*",1);
    old_simtree->SetBranchStatus("*",1);

    std::cout<<"Making new: "<<output<<std::endl;
    TFile *newfile = new TFile(output.c_str(),"recreate");
    TDirectory *cdtof = newfile->mkdir(indir.c_str());
    cdtof->cd();


    //Clone the old tree, but dont copy any info yet.
    TTree *new_vtree = old_vtree->CloneTree(0);
    TTree *new_etree = old_etree->CloneTree(0);
    TTree *new_stree = old_stree->CloneTree(0);
    TTree *new_simtree = old_simtree->CloneTree(0);


    std::map<RSE,int> num_c;

    //Loop over old tree, check condition is passed, if so fill the new_vtree
    for (size_t i=0;i<old_vtree->GetEntries(); i++) {
        old_vtree->GetEntry(i);
        int is_in = rlist.inList(run_number,subrun_number);//returns number
        if(  (pos_or_neg && is_in) || (!pos_or_neg && !is_in)  ){

            RSE rt(run_number, subrun_number,0);
            num_c[rt] = ( num_c.count(rt)==0 ? 1 : num_c.count(rt)+1 );       

            if(num_c[rt]==1 || allow_duplicates){        
                old_etree->GetEntry(i);
                old_stree->GetEntry(i);
                old_simtree->GetEntry(i);

                new_vtree->Fill();
                new_etree->Fill();
                new_stree->Fill();
                new_simtree->Fill();
            }
        }
    }

    newfile->cd();
    cdtof->cd();
    new_vtree->Write();
    new_etree->Write();
    new_stree->Write();
    new_simtree->Write(); 

    //Now the old fashioned pot_tree and run_subrun_tree
    TTree * new_ptree = (TTree*)old_ptree->CopyTree("1");
    cdtof->cd();
    new_ptree->Write();


    std::map<RSE,int> num_c2;


    int rs_run_number = 0;
    int rs_subrun_number = 0;

    //Set addresses
    old_rtree->SetBranchAddress("run",&rs_run_number);
    old_rtree->SetBranchAddress("subrun",&rs_subrun_number);
    //Activate all brances
    old_rtree->SetBranchStatus("*",1);

    cdtof->cd();

    //Clone the old tree, but dont copy any info yet.
    TTree *new_rtree = old_rtree->CloneTree(0);

    int t_sb = 0;
    //Loop over old tree, check condition is passed, if so fill the new_vtree
    for (size_t i=0;i<old_rtree->GetEntries(); i++) {
        old_rtree->GetEntry(i);
        int is_in = rlist.inList(rs_run_number,rs_subrun_number);//returns number
        if(  (pos_or_neg && is_in) || (!pos_or_neg && !is_in)  ){

            RSE rt(rs_run_number, rs_subrun_number,-1000);
            num_c2[rt] = ( num_c2.count(rt)==0 ? 1 : num_c2.count(rt)+1 );       

            if(num_c2[rt]==1 || allow_duplicates){        
                new_rtree->Fill();
                t_sb++;
            }
        }
    }
    std::cout<<"Total Subruns "<<t_sb<<std::endl;
    cdtof->cd();
    new_rtree->Write();


    oldfile->Close();
    newfile->Close();
}


void RunCloner(runlist &rlist, std::string input, std::string output, bool pos_or_neg, bool allow_duplicates, std::string indir = "singlephotonana", bool cut_subrun_level = false) {
    //Get old file, old tree and set top branch address
    std::string nam = input;
    std::unique_ptr<TFile> oldfile(TFile::Open(nam.c_str(), "read"));

    TTree *old_vtree = (TTree*)oldfile->Get((indir+"/vertex_tree").c_str());
    TTree *old_etree = (TTree*)oldfile->Get((indir+"/eventweight_tree").c_str());
    TTree *old_stree = (TTree*)oldfile->Get((indir+"/ncdelta_slice_tree").c_str());
    TTree *old_ptree = (TTree*)oldfile->Get((indir+"/pot_tree").c_str());
    TTree *old_rtree = (TTree*)oldfile->Get((indir+"/run_subrun_tree").c_str());
    TTree *old_ttree = (TTree*)oldfile->Get((indir+"/true_eventweight_tree").c_str());
    //TTree *old_gtree = (TTree*)oldfile->Get((indir+"/geant4_tree").c_str());
    //TTree *old_simtree = (TTree*)oldfile->Get((indir+"/simple_tree").c_str());

    int run_number = 0;
    int subrun_number = 0;
    int event_number = 0;

    //Set addresses
    old_vtree->SetBranchAddress("run_number",&run_number);
    old_vtree->SetBranchAddress("subrun_number",&subrun_number);
    old_vtree->SetBranchAddress("event_number",&event_number);
    //Activate all brances
    old_vtree->SetBranchStatus("*",1);
    old_etree->SetBranchStatus("*",1);
    old_stree->SetBranchStatus("*",1);
    old_ttree->SetBranchStatus("*",1);
   // old_gtree->SetBranchStatus("*",1);
    //old_simtree->SetBranchStatus("*",1);

    TFile *newfile = new TFile(output.c_str(),"recreate");
    TDirectory *cdtof = newfile->mkdir((indir).c_str());
    cdtof->cd();


    //Clone the old tree, but dont copy any info yet.
    TTree *new_vtree = old_vtree->CloneTree(0);
    TTree *new_etree = old_etree->CloneTree(0);
    TTree *new_stree = old_stree->CloneTree(0);
    TTree *new_ttree = old_ttree->CloneTree(0);
   // TTree *new_simtree = old_simtree->CloneTree(0);
  //  TTree *new_gtree = old_gtree->CloneTree(0);

    std::map<RSE,int> num_c;

    //Loop over old tree, check condition is passed, if so fill the new_vtree
    for (size_t i=0;i<old_vtree->GetEntries(); i++) {
        old_vtree->GetEntry(i);

        if(i%1000==0)std::cout<<"Filling vertx: "<<i<<" / "<<old_vtree->GetEntries()<<std::endl;

        int is_in = rlist.inList(run_number,subrun_number, cut_subrun_level ? -1000 : event_number  );//returns number
        
        if(  (pos_or_neg && is_in) || (!pos_or_neg && !is_in)  ){

            RSE rt(run_number, subrun_number,event_number);
            num_c[rt] = ( num_c.count(rt)==0 ? 1 : num_c.count(rt)+1 );       

            if(num_c[rt]==1 || allow_duplicates){        
                old_etree->GetEntry(i);
                old_stree->GetEntry(i);
                old_ttree->GetEntry(i);
                //old_simtree->GetEntry(i);
                //old_gtree->GetEntry(i);

                new_vtree->Fill();
                new_etree->Fill();
                new_stree->Fill();
                //new_simtree->Fill();
                new_ttree->Fill();
                //new_gtree->Fill();

            }
        }
    }

    newfile->cd();
    cdtof->cd();
    new_vtree->Write();
    new_etree->Write();
    new_stree->Write();
    new_ttree->Write();
    //new_simtree->Write();
    //new_gtree->Write();


    //Now the old fashioned pot_tree and run_subrun_tree
    TTree * new_ptree = (TTree*)old_ptree->CopyTree("1");
    cdtof->cd();
    new_ptree->Write();


    std::map<RSE,int> num_c2;


    int rs_run_number = 0;
    int rs_subrun_number = 0;

    //Set addresses
    old_rtree->SetBranchAddress("run",&rs_run_number);
    old_rtree->SetBranchAddress("subrun",&rs_subrun_number);
    //Activate all brances
    old_rtree->SetBranchStatus("*",1);

    cdtof->cd();

    //Clone the old tree, but dont copy any info yet.
    TTree *new_rtree = old_rtree->CloneTree(0);

    int t_sb = 0;
    //Loop over old tree, check condition is passed, if so fill the new_vtree
    for (size_t i=0;i<old_rtree->GetEntries(); i++) {
        old_rtree->GetEntry(i);

        if(i%1000==0)std::cout<<"Filling RSE: " <<i<<" / "<<old_rtree->GetEntries()<<std::endl;

        int is_in = rlist.inList(rs_run_number,rs_subrun_number);//returns number
        if(  (pos_or_neg && is_in) || (!pos_or_neg && !is_in)  ){

            RSE rt(rs_run_number, rs_subrun_number,-1000);
            num_c2[rt] = ( num_c2.count(rt)==0 ? 1 : num_c2.count(rt)+1 );       

            if(num_c2[rt]==1 || allow_duplicates){        
                new_rtree->Fill();
                t_sb++;
            }
        }
    }
    std::cout<<"Total Subruns "<<t_sb<<std::endl;
    cdtof->cd();
    std::cout<<"Output rubsubrun tree"<<std::endl;
    new_rtree->Write();


    std::cout<<"Close oldfile"<<std::endl;
    oldfile->Close();
    std::cout<<"Close new"<<std::endl;
    newfile->Close();
}

//actual function for duplicate checking.
void duplicate_suite_2022(std::string nam, std::string tag, std::string dir){
    
    std::vector<RSE>  rD = getRunSubrunList(nam.c_str(),"singlephotonana/vertex_tree"); 
    runlist RD(rD);

    std::cout<<tag<<std::endl;
    RD.checkDuplicates();

    //RunCloner(RD,nam.c_str(),(dir+tag+".uniq.root").c_str(),true,false);
    return;

}

//actual function for duplicate checking.
void SplitFiles2022(std::string nam, std::string tag, std::string dir, double percent){
    
    std::vector<RSE>  rD = getRunSubrunList_v2(nam.c_str(),"singlephotonana/run_subrun_tree"); 
    runlist RD(rD);

    runlist RD50 = RD.getSubSet(percent);
    
    RunCloner(RD50, nam.c_str(), (dir+tag+".incl.root").c_str(),true, false,"singlephotonana",true);
    RunCloner(RD50, nam.c_str(), (dir+tag+".comp.root").c_str(),false, false,"singlephotonana",true);

    return;

}




void copytree(){

    //OK, loop over all files input and get each's RSE's
    std::vector<RSE>  rX = getRunSubrunList("/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodX.root","singlephotonana/vertex_tree"); 
    std::vector<RSE>  rYZ = getRunSubrunList("/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodYZ.root","singlephotonana/vertex_tree"); 
    std::vector<RSE>  rA = getRunSubrunList("/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodAngle.root","singlephotonana/vertex_tree"); 
    std::vector<RSE>  rCV = getRunSubrunList("/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodCV.root","singlephotonana/vertex_tree"); 

    std::vector<RSE> rTrain = getRunSubrunList("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/Train/ncpi0_overlay_train_v33.0.root","singlephotonana/vertex_tree");
    //Check for duplciaes! SOON
    runlist RX(rX);
    runlist RYZ(rYZ);
    runlist RCV(rCV);
    runlist RA(rA);

    runlist RTrain(rTrain);

    //find overlapping subset.
    runlist RiCVX = RCV.Intersection(RX);
    runlist RiCVYZ = RCV.Intersection(RYZ);
    runlist RAll_0 = RiCVYZ.Intersection(RiCVX);
    runlist RAll  = RAll_0.Intersection(RAll_0);
    RAll.Print();

    //Make subset of each with those events.
    RunCloner(RAll,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodX.root","DONE_ncpi0_wireX_v33.0.root",true,false);
    RunCloner(RAll,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodYZ.root","DONE_ncpi0_wireYZ_v33.0.root",true,false);
    RunCloner(RAll,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodCV.root","DONE_ncpi0_wireCV_v33.0.root",true,false);
    RunCloner(RAll,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodAngle.root","DONE_ncpi0_wireA_v33.0.root",true,false);

    RunCloner(RTrain,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodX.root","NoTraing_DONE_ncpi0_wireX_v33.0.root",false,false);
    RunCloner(RTrain,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodYZ.root","NoTraing_DONE_ncpi0_wireYZ_v33.0.root",false,false);
    RunCloner(RTrain,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodCV.root","NoTraing_DONE_ncpi0_wireCV_v33.0.root",false,false);
    RunCloner(RTrain,"/uboone/data/users/wtang/detsys_v33/ncpi0_overlay_v33_wiremodAngle.root","NoTraing_DONE_ncpi0_wireA_v33.0.root",false,false);

    //Make Subset of uniq
    //RunCloner(RI,false);

}

double calcPOT(std::string file){

    TFile *f  = new TFile(file.c_str(), "read");
    TTree *v = (TTree*)f->Get("singlephotonana/vertex_tree");
    TTree *t = (TTree*)f->Get("singlephotonana/run_subrun_tree");

    int rs_run_number = 0;
    int rs_subrun_number = 0;
    double rs_pot = 0;

    double tot_pot = 0;

    //Set addresses
    t->SetBranchAddress("run",&rs_run_number);
    t->SetBranchAddress("subrun",&rs_subrun_number);
    t->SetBranchAddress("subrun_pot",&rs_pot);

    for(int i=0; i< t->GetEntries(); i++){
        t->GetEntry(i);
        tot_pot+=rs_pot;
    }

    std::cout<<"Total POT: "<<tot_pot<<" Total SubRuns: "<<t->GetEntries()<<" TotalEvents: "<<v->GetEntries()<<std::endl;

    f->Close();

    return tot_pot;
}

void spluf(){

    //Given a file.
    std::vector<RSE>  rD = getRunSubrunList("/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.root","singlephotonana/vertex_tree"); 
    runlist RD(rD);
    calcPOT("/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.root");
    runlist spluffed = RD.getSubSet(0.2);
    RunCloner(spluffed,"/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.root","/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.spluff.test.root",false,true);
    std::cout<<"A"<<std::endl;
    calcPOT("/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.spluff.test.root");

    RunCloner(spluffed,"/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.root","/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.spluff.train.root",true,true);
    std::cout<<"B"<<std::endl;
    calcPOT("/uboone/data/users/markross/Mar2020/nue_overlay_run3_v33.3.uniq.spluff.train.root");
}



void testtrain(){

    //vertexed_mcc9_v33/Test/ncdeltarad_overlay_test_run1_v33.0.root
    //vertexed_mcc9_v33/Train/ncdeltarad_overlay_train_v33.0.root


    std::vector<RSE> rTrain = getRunSubrunList("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v26/AllRun/Split/bnbext_train_v26.5.root","singlephotonana/vertex_tree");
    std::vector<RSE> rPlot = getRunSubrunList("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v26/AllRun/Split/bnbext_test_v26.5.root","singlephotonana/vertex_tree");

    /*std::vector<RSE> rTrain = getRunSubrunList("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/Train/ncdeltarad_overlay_train_v33.0.root","singlephotonana/vertex_tree");
      std::vector<RSE> rPlot = getRunSubrunList("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/Test/ncdeltarad_overlay_test_run1_v33.0.root","singlephotonana/vertex_tree");
      */
    runlist RT(rTrain);
    runlist RP(rPlot);

    //find overlapping subset.
    runlist RI = RT.Intersection(RP);
    //    RI.Print();
    std::cout<<"Train  "<<RT.f_rses.size()<<". Test "<<RP.f_rses.size()<<" Inter "<<RI.f_rses.size()<<std::endl;

}



void ccpicheck(){

    //vertexed_mcc9_v33/Test/ncdeltarad_overlay_test_run1_v33.0.root
    //vertexed_mcc10_v33/Train/ncdeltarad_overlay_train_v33.0.root

    //std::vector<RSE> rTrain = getRunSubrunList("/uboone/data/users/markross/Mar2020/ssveto_bdt_training_sample_v33.3.root","singlephotonana/vertex_tree");
    std::vector<RSE> rTrain = getRunSubrunList("/uboone/data/users/markross/Mar2020/ncpi0_bdt_training_sample_v33.3.root","singlephotonana/vertex_tree");
//    std::vector<RSE> rTrain = getRunSubrunList("/uboone/data/users/markross/Mar2020/ncdelta_bdt_training_sample_v33.3.root","singlephotonana/vertex_tree");
    std::vector<RSE> rPlot = getRunSubrunList("/uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/build/bin/Oct2020_FarSideband/1g1p_FS/sbnfit_1g1pMar2020_v4_stage_1_BNBOtherRun2Fake.root","singlephoton/vertex_tree");

    /*std::vector<RSE> rTrain = getRunSubrunList("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/Train/ncdeltarad_overlay_train_v33.0.root","singlephotonana/vertex_tree");
      std::vector<RSE> rPlot = getRunSubrunList("/pnfs/uboone/persistent/users/markross/single_photon_persistent_data/vertexed_mcc9_v33/Test/ncdeltarad_overlay_test_run1_v33.0.root","singlephotonana/vertex_tree");
      */
    runlist RT(rTrain);
    runlist RP(rPlot);

    //find overlapping subset.
    runlist RI = RT.Intersection(RP);
    //    RI.Print();
    std::cout<<"Train  "<<RT.f_rses.size()<<". Test "<<RP.f_rses.size()<<" Inter "<<RI.f_rses.size()<<std::endl;

}

#endif
