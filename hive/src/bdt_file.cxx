# include "bdt_file.h"


bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bdt_flow inflow) : bdt_file(indir, inname, intag,inops,inrootdir,incol,1001,inflow){}

bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, int infillstyle, bdt_flow inflow) :
    dir(indir),
    name(inname),
    tag(intag),
    plot_ops(inops),
    root_dir(inrootdir),
    col(incol),
    flow(inflow),
    is_data(false),
    is_bnbext(false),
    is_mc(true)
{

    plot_name = tag;

    rangen = new TRandom3();

    scale_data =1.0;
    std::cout<<"Loading : "<<name<<std::endl;
    f = new TFile((dir+"/"+name).c_str(), "read");	

    if(!f->IsOpen() || !f){
        std::cout<<"ERROR: didnt open file right: "<<dir<<"/"<<name<<std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout<<"bdt_file::bdt_file || "<<name<<" Opened correctly by root."<<std::endl;

    std::string tnam_event = root_dir+"event_tree";
    std::string tnam = root_dir+"vertex_tree";
    std::string tnam_pot = root_dir+"pot_tree";

    if(is_mc){
        std::cout<<"setting weight branch - mc"<<std::endl;
        //weight_branch = "genie_spline_weight";
        weight_branch = "genie_spline_weight*tan(atan(genie_CV_tune_weight))*(tan(atan(genie_CV_tune_weight))<10000)*(genie_CV_tune_weight>0)";
        //weight_branch = "genie_spline_weight*(genie_spline_weight<25)*tan(atan(genie_CV_tune_weight))*(tan(atan(genie_CV_tune_weight))<25)*(genie_CV_tune_weight>0)";
       // weight_branch = "genie_spline_weight";//*tan(atan(genie_CV_tune_weight))*(tan(atan(genie_CV_tune_weight))<10000)*(genie_CV_tune_weight>0)";
    } 
    if (is_data ||  is_bnbext) {
        std::cout<<"setting weight branch - on/off beam data"<<std::endl;
        weight_branch = "1";
    }
    
    fillstyle = infillstyle;
    scale_data = 1.0;

//    run_names = {"RI","RII","RIIIa","RIIIb","RIV"};
//    run_fractions_plot = {0.16638655, 0.26435986, 0.043400890, 0.21413742, 0.31171527};
//    run_fraction_cuts  = {"(run_number >= 4952 && run_number <= 7770)", "( run_number >=8317 && run_number <=  13696)", "(run_number >= 13697 && run_number <= 14116)","(run_number >= 14117 && run_number <= 18960)","(run_number >=18961 && run_number <= 23542)"};
    
    //run_names = {"RI/II/IIIa","RIIIb/IV"};
    //run_fraction_cuts  = {"( (run_number >= 4952 && run_number <= 7770) || ( run_number >= 8317 && run_number <=  13696) || (run_number >= 13697 && run_number <= 14116)) ", "( (run_number >= 14117 && run_number <= 18960) || (run_number >=18961 && run_number <=23542) )"};
    //run_fractions_plot = {0.4742,0.5258};
   
    //2g1p
    //run_names  = {"RI/II","RIII"};
    //run_fraction_cuts = {"(run_number <=13696)","(run_number >= 13697)"};
    //run_fractions_plot = {0.6964636727,0.3035363273};
 
    //2g0p
    //run_names  = {"RI/II","RIII"};
    //run_fraction_cuts = {"(run_number <=13696)","(run_number >= 13697)"};
    //run_fractions_plot = {0.677544979,0.322455021};
    
    // Just run 1 and/or 2
    //run_names = {"RI/II"};
    //run_fraction_cuts  = {"run_number<=13696"};
    //run_fractions_plot = {1.0};

    // Just run 3
    run_names = {"RIII"};
    run_fraction_cuts  = {"1"};
    run_fractions_plot = {1.0};

    //run_names = {"RIsmall"};
    //run_fraction_cuts  = {"1"};
    //run_fractions_plot = {1.0};

  //  run_names = {"RI","R3"};
  //  run_fraction_cuts  = {"run_number <= 7770 ","run_number>=13697"};
 //   run_fractions_plot = {0.5,0.5};


    std::cout<<"Getting vertex tree"<<std::endl;
    tvertex = (TTree*)f->Get(tnam.c_str());

    //tevent = (TTree*)f->Get(tnam_event.c_str());
    std::cout<<"Got vertex tree: "<<tvertex->GetEntries()<<std::endl;
    //topovertex = (TTree*)tvertex->CopyTree(flow.topological_cuts.c_str());
    //std::cout<<"Copied to topological tree: "<<topovertex->GetEntries()<<std::endl;

    run_fractions_file.resize(run_fractions_plot.size(),0);
    double combin = 0.0;
    for(int i=0; i< run_fractions_plot.size(); i++){
            run_fractions_file[i] = tvertex->GetEntries(run_fraction_cuts[i].c_str())/(double)tvertex->GetEntries();
            std::cout << "BLARG run_fractions_file[" << i << "] = " << run_fractions_file[i] << std::endl;
            std::cout<<run_fraction_cuts[i]<<std::endl;
            std::cout<<"-- of which "<<run_fractions_file[i]*100.0<<" \% are in "<<run_names[i]<<std::endl;
            combin+=run_fractions_file[i];
    }
    std::cout<<"Total is "<<combin<<std::endl;

    std::cout << "run_fractions_plot[0] = " << run_fractions_plot[0] << std::endl;
    std::cout << "run_fractions_file[0] = " << run_fractions_file[0] << std::endl;
    run_weight_string = "1.0*("+run_fraction_cuts[0]+"*"+std::to_string(run_fractions_plot[0]/run_fractions_file[0]);
    for(int i=1; i< run_fractions_plot.size(); i++){
         run_weight_string += "+" +run_fraction_cuts[i]+"*"+std::to_string(run_fractions_plot[i]/run_fractions_file[i]);
    }
    run_weight_string +=")";
    std::cout<<"Run Weight String is: \n "<<run_weight_string<<std::endl;


    std::cout<<"Getting eventweight tree"<<std::endl;
    teventweight = (TTree*)f->Get((root_dir+"eventweight_tree").c_str());
    tvertex->AddFriend(teventweight);
    std::cout<<"Got eventweight tree: "<<teventweight->GetEntries()<<std::endl;

    vec_entry_lists.resize(flow.bdt_vector.size());

    //this->CheckWeights();//make sure there aren't erroneous weights

    /*
    //This is all old school mcc8 stuff for now.
    if(tag == "IntimeCosmics"){
    std::cout<<"Getting POT for CosmicIntime: "<<std::endl;
    //Found in 
    double intime_modifier = 10.279;

    //Guarrenteed for fresh_mcc8.9
    double N_gen_bnb = 2146800.0;
    double N_gen_cos = 991914.0;

    double pot_bnb_cosmic = 2.16562e+21;
    double pot_plot = 6.6e20;

    pot = pot_plot; 
    this->scale_data = intime_modifier*N_gen_bnb/(N_gen_cos)*pot_plot/pot_bnb_cosmic;
    std::cout<<"--> value: "<<pot<<" with scale factor: "<<scale_data<<std::endl;
    }else{
    leg = "l";
    std::cout<<"Getting POT tree: "<<tnam_pot<<std::endl;
    tpot = (TTree*)f->Get(tnam_pot.c_str());
    std::cout << "tpot name: " << tnam_pot.c_str() << std::endl;
    tpot->SetBranchAddress("number_of_events", &numbranch);
    tpot->SetBranchAddress("POT",&potbranch);

    std::cout<<"Set the POT branch"<<std::endl;
    int tmpnum = 0;
    double tmppot=0;
    std::cout << "tpot entries: " << tpot->GetEntries() << std::endl;
    for(int i=0; i<tpot->GetEntries(); i++) {
    tpot->GetEntry(i);
    tmpnum += (double)numbranch;
    tmppot += potbranch;
    }
    numberofevents = tmpnum;
    pot=tmppot;
    std::cout<<"--> POT is MC: ";
    std::cout<<"--> value: "<<pot<<" NumEvents: "<<numberofevents<<std::endl;

    weight_branch = "1";
    numberofevents_raw = numberofevents;
    }
    }

    if(tag == "BNBPure" || tag == "BNBCosmics"){
    //MCC9 pot issues
    //OLD: POT is MC: --> value: 2.16562e+21 NumEvents: 2154500
    pot = 2.16562e21*(double)numberofevents/2154500.0;
    std::cout<<"REAL MCC9: --> POT is MC: ";
    std::cout<<"--> value: "<<pot<<" NumEvents: "<<numberofevents<<std::endl;
    }

    if(tag == "NCPi0" || tag=="NCPi0Cosmics"){
    //MCC9 pot issues
    //OLD: POT is MC: --> value: 2.16562e+21 NumEvents: 2154500
    // nc_fraction calculated by opening bnb_nu_overlay_combined_whatever and
    // diving the number of NC events with > 0 exiting pi0s by the total no. entries
    double nc_fraction = 0.061877;
    pot = 2.16562e21*(double)numberofevents/2154500.0/nc_fraction;
    std::cout<<"REAL MCC9: --> POT is MC: ";
    std::cout<<"--> value: "<<pot<<" NumEvents: "<<numberofevents<<std::endl;
    }

    if(tag == "NCDeltaRadCosmics" || tag == "NCDeltaRadPure" || tag == "NCDeltaRad"){
    double volCryo = 199668.427885;
    double volTPC = 101510.0;
    double volTPCActive=  86698.6;

    //numberofevents = numberofevents*volTPCActive/volTPC;	
    numberofevents = numberofevents;//*volTPCActive/volCryo;

    //in MCC9 POT is broken so lets fudge it here: OLD mcc8 1.64282e+24 NumEvents: 350189
    //pot = 1.64282e24*((double)numberofevents/350189.0);
    pot = 1.64282e24*(39954.0/350189.0);

    std::cout<<"REAL MCC9: --> POT is MC: ";
    std::cout<<"--> value: "<<pot<<" NumEvents: "<<numberofevents<<std::endl;

    tvertex->ResetBranchAddresses();
}



if(tag == "Data5e19"){
    leg = "lp";
    pot = 4.898e19; //old mcc84.393e19;// tor860_wcut
    weight_branch = "1";
    //so MCC9, we have 197772 events in file and 197833 evnts in samweb (lets ignore that) so 
    std::cout<<"--> value: "<<pot<<std::endl;
}
if(tag == "BNBext"){
    std::cout<<"Getting POT tree: "<<tnam_pot<<std::endl;
    tpot = (TTree*)f->Get(tnam_pot.c_str());
    tpot->SetBranchAddress("number_of_events", &numbranch);
    tpot->SetBranchAddress("POT",&potbranch);

    std::cout<<"Set the POT branch"<<std::endl;
    int tmpnum = 0;
    double tmppot=0;
    for(int i=0; i<tpot->GetEntries(); i++){
        tpot->GetEntry(i);
        tmpnum += (double)numbranch;
    }
    numberofevents = tmpnum;
    std::cout<<"BNBEXT number of events: "<<numberofevents<<std::endl;


    leg = "lp";
    double sca = 1.23;//from 1.23
    //https://microboone-docdb.fnal.gov/cgi-bin/private/ShowDocument?docid=5640

    //MCC9: 
    //Data samweb is 197833 events. defname: data_bnb_run1_unblind_mcc9.0_nov_reco_2d_reco2_slim
    //tor860_wcut: 4.898e+19,  E1DCNT_wcut: 11595542.0
    //
    //bnbext samweb is  200433 events, defname: data_extbnb_run1_dev_mcc9.0_nov_reco_2d_reco2_slim
    //EXT spills: 15435961.0


    //so that is
    double ext=15435961.0;//47953078.0; //External spills in each sample (EXT)
    double spill_on=11595542.0;//10702983.0;//This number in data zarko  (E1DCNT_wcut)
    double datanorm =4.898e19;// tor860_wcut run-subrunlist;

    double Noff_full = 200433.0; //this is full samweb events
    double Noff_have = numberofevents;

    //This is old MCC8 one
    //double ext=33752562+40051674;//47953078.0; //External spills in each sample (EXT)
    //double spill_on=10312906;//10702983.0;//This number in data zarko  (E1DCNT_wcut)
    //double datanorm =4.393e19;// tor860_wcut run-subrunlist;


    double mod = spill_on/ext*(Noff_full/Noff_have);


    std::cout<<"--> POT is data: From Zarkos tool..";
    //going to scale by how many events I actually have in MCC9
    pot =datanorm/mod;
    std::cout<<"--> value: "<<pot<<std::endl;

    weight_branch = "1";
}

*/




};

int bdt_file::setTColor(TColor & tin){
    f_TColor = tin;
    return f_TColor.GetNumber();

}


int bdt_file::setAsMC(){

}

int bdt_file::setAsOverlay(){
}

int bdt_file::setAsOnBeamData(double in_tor860_wcut){
    is_data = true;
    is_mc = false;
    is_bnbext = false;

    data_tor860_wcut = in_tor860_wcut;
    return 0;
}

int bdt_file::setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext){
    this->setAsOffBeamData(in_data_tor860_wcut, in_data_spills_E1DCNT_wcut, in_ext_spills_ext, -1);

    return 0;
}
int bdt_file::setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext, double in_N_samweb_ext){
    is_data = false;
    is_mc = false;
    is_bnbext = true;

    data_tor860_wcut = in_data_tor860_wcut;
    data_spills_E1DCNT_wcut = in_data_spills_E1DCNT_wcut;
    ext_spills_ext = in_ext_spills_ext;
    N_samweb_ext = in_N_samweb_ext;

    return 0;
}



int bdt_file::calcPOT(){

    std::string tnam_event = root_dir+"event_tree";
    std::string tnam = root_dir+"vertex_tree";
    std::string tnam_pot = root_dir+"pot_tree";
    std::string tnam_rs = root_dir+"run_subrun_tree";
    std::string tnam_slice = root_dir+"ncdelta_slice_tree";

    double potbranch = 0;
    int  numbranch = 0;

    tslice = (TTree*)f->Get(tnam_slice.c_str());


    if(is_mc){
        //If its MC or Overlay, lets just grab the POT from the nice POT tree
        leg = "l";


        trs = (TTree*)f->Get(tnam_rs.c_str());    
        trs->SetBranchAddress("subrun_pot",&potbranch);


        std::cout<<"bdt_file::bdt_file()\t||\tFile is either MC or OVERLAY for purposes of getting POT."<<std::endl;
        std::cout<<"bdt_file::bdt_file()\t||\tGetting POT tree: "<<tnam_pot<<" "<<std::endl;
        tpot = (TTree*)f->Get(tnam_pot.c_str());
        //tpot->SetBranchAddress("number_of_events", &numbranch);
        //tpot->SetBranchAddress("POT",&potbranch);
        std::cout<<"bdt_file::bdt_file()\t||\tBranches all setup."<<std::endl;
        int tmpnum = 0;
        double tmppot=0;
        
        std::cout<<"bdt_file::bdt_file()\t||\t There was "<<trs->GetEntries()<<" subruns merged to make this root file."<<std::endl;
        
        for(int i=0; i<trs->GetEntries(); i++) {
            trs->GetEntry(i);
            tmppot += potbranch;
        }

        numberofevents = tvertex->GetEntries();

        pot=tmppot;
        std::cout<<"bdt_file::bdt_file()\t||\t---> POT is MC/OVERLAY "<<std::endl;
        std::cout<<"--> POT: "<<pot<<" Number of Entries: "<<numberofevents<<std::endl;
        std::cout<<"--> Events scaled to 13.2e20 "<<numberofevents/pot*13.2e20<<std::endl;
        std::cout<<"--> Events scaled to 10.1e20 "<<numberofevents/pot*10.1e20<<std::endl;
        //weight_branch = "1";
        //weight_branch = "genie_spline_weight";
        weight_branch = "genie_spline_weight*tan(atan(genie_CV_tune_weight))*(tan(atan(genie_CV_tune_weight))<10000)*(genie_CV_tune_weight>0)*("+run_weight_string+")";

        //weight_branch = "genie_spline_weight*(genie_spline_weight<25)*tan(atan(genie_CV_tune_weight))*(tan(atan(genie_CV_tune_weight))<25)*(genie_CV_tune_weight>0)*("+run_weight_string+")";

        /*if(this->tag.find("NCPi0")!=std::string::npos){
            weight_branch = weight_branch +"*"+"(1.0+ (sqrt(mctruth_exiting_pi0_E*mctruth_exiting_pi0_E - 0.135*0.135)<0.3)*0.2 + (sqrt(mctruth_exiting_pi0_E*mctruth_exiting_pi0_E - 0.135*0.135)<0.175)*0.3  +  (sqrt(mctruth_exiting_pi0_E*mctruth_exiting_pi0_E - 0.135*0.135)<0.1)*0.3 )";

        }*/


        numberofevents_raw = numberofevents;

    }else if(is_data){
        //This is for Pure On beam Data. Taken as input by calling 

        std::cout<<"bdt_file::bdt_file()\t||\tFile is ON-BEAM DATA for purposes of getting POT."<<std::endl;
        tpot = (TTree*)f->Get(tnam_pot.c_str());
        tpot->SetBranchAddress("number_of_events", &numbranch);
        std::cout<<"bdt_file::bdt_file()\t||\tBranches all setup."<<std::endl;
        int tmpnum = 0;
        std::cout<<"bdt_file::bdt_file()\t||\t There was "<<tpot->GetEntries()<<" files merged to make this root file."<<std::endl;
        for(int i=0; i<tpot->GetEntries(); i++) {
            tpot->GetEntry(i);
            tmpnum += (double)numbranch;
        }
        numberofevents = tmpnum;
        numberofevents_raw = numberofevents;

        leg = "lp";
        pot = this->data_tor860_wcut; // tor860_wcut
        weight_branch = "1";
        std::cout<<"bdt_file::bdt_file()\t||\t---> POT is DATA. Setting from internal ZARKOS's tool numbers."<<std::endl;
        std::cout<<"--> POT: "<<pot<<" Number of Entries: "<<numberofevents<<std::endl;



    }else if(is_bnbext){

        std::cout<<"bdt_file::bdt_file()\t||\tFile is OFF-BEAM (BNBEXT) DATA for purposes of getting POT."<<std::endl;
        tpot = (TTree*)f->Get(tnam_pot.c_str());
        int numbranch =0;
        tpot->SetBranchAddress("number_of_events", &numbranch);
        std::cout<<"bdt_file::bdt_file()\t||\tBranches all setup."<<std::endl;
        int tmpnum = 0;
        std::cout<<"bdt_file::bdt_file()\t||\t There was "<<tpot->GetEntries()<<" files merged to make this root file."<<std::endl;
        for(int i=0; i<tpot->GetEntries(); i++) {
            tpot->GetEntry(i);
            tmpnum += (double)numbranch;
        }
        numberofevents = tmpnum;
        numberofevents_raw = numberofevents;



        leg = "lp";
        //https://microboone-docdb.fnal.gov/cgi-bin/private/ShowDocument?docid=5640

        //MCC9:  HERE FOR EXAMPLE PURPOSE ONLY
        //Data samweb is 197833 events. defname: data_bnb_run1_unblind_mcc9.0_nov_reco_2d_reco2_slim
        //tor860_wcut: 4.898e+19,  E1DCNT_wcut: 11595542.0
        //
        //bnbext samweb is  200433 events, defname: data_extbnb_run1_dev_mcc9.0_nov_reco_2d_reco2_slim
        //EXT spills: 15435961.0


        //so that is
        //   double ext=15435961.0;//47953078.0; //External spills in each sample (EXT)
        //  double spill_on = data_tor860_wcut;   //11595542.0;//10702983.0;//This number in data zarko  (E1DCNT_wcut)
        //  double datanorm =4.898e19;// tor860_wcut run-subrunlist;

        //This is old MCC8 one
        //double ext=33752562+40051674;//47953078.0; //External spills in each sample (EXT)
        //double spill_on=10312906;//10702983.0;//This number in data zarko  (E1DCNT_wcut)
        //double datanorm =4.393e19;// tor860_wcut run-subrunlist;

        //        double mod = spill_on/ext*(Noff_full/Noff_have);

        if(N_samweb_ext ==-1) N_samweb_ext = numberofevents;

        double modifier = data_spills_E1DCNT_wcut/ext_spills_ext*(N_samweb_ext/numberofevents);

        std::cout<<"--> POT is data: From Zarkos tool..";
        //going to scale by how many events I actually have in MCC9
        pot =this->data_tor860_wcut/modifier;
        std::cout<<"--> value: "<<pot<<std::endl;

        std::cout<<"bdt_file::bdt_file()\t||\t---> POT is OFF BEAM DATA "<<std::endl;
        std::cout<<"--> POT: "<<pot<<" Number of Entries: "<<numberofevents<<std::endl;
        std::cout<<"--> scaled to 5e19 number of Entries: "<<numberofevents/pot*5e19<<std::endl;

        weight_branch = "("+run_weight_string+")";
        numberofevents_raw = numberofevents;

    }



    return 0;
}

int bdt_file::makeRunSubRunList(){

    int n_run_number = 0;
    int n_subrun_number  = 0;
    int n_event_number = 0;

    this->tvertex->SetBranchAddress("run_number",    &n_run_number);
    this->tvertex->SetBranchAddress("subrun_number", &n_subrun_number);
    this->tvertex->SetBranchAddress("event_number",  &n_event_number);

    std::cout<<"Starting makeRunSubRunList() "<<std::endl;

    for(int i=0;i < this->tvertex->GetEntries(); i++ ){
        this->tvertex->GetEntry(i);
        std::cout<<n_run_number<<" "<<n_subrun_number<<std::endl;
    }
    std::cout<<"Ending makeRunSubRunList() "<<std::endl;

    this->tvertex->ResetBranchAddresses();

    return 0;
}

int bdt_file::calcPrecutEntryList(){

    //first check if a file exists with a precut entry list in it!

    std::string precut_key = this->name;
    for(auto &s: this->flow.vec_pre_cuts){
        precut_key+=s;
    }
    precut_key+=this->flow.base_cuts;


    unsigned long precut_hash = this->jenkins_hash(precut_key); 
    std::cout<<"These particular precuts have a hash of "<<precut_hash<<std::endl;
    std::string s_precut_hash = std::to_string(precut_hash);

    std::string filename = this->tag+"_entrylists.root";
    precut_list_name = "precut_list_"+this->tag;

    std::ifstream ifile(filename.c_str());
    bool does_local_exist = (bool)ifile;
    if(does_local_exist){

        std::cout<<"Entry List File already exists for "<<this->tag<<std::endl;
        TFile* fpre = new TFile(filename.c_str(),"update");	

        bool hash_right = fpre->GetListOfKeys()->Contains(s_precut_hash.c_str());
        if(hash_right){
            std::cout<<"File has correct hash"<<std::endl;
        }else{
            std::cout<<"File does not have a valid hash, regenerating!"<<std::endl;
        }


        if(fpre->GetListOfKeys()->Contains(precut_list_name.c_str()) &&hash_right  ) {

            std::cout<<"And it contains a list. Loading"<<std::endl;

            precut_list = (TEntryList*)fpre->Get(precut_list_name.c_str());
        } else{

            std::cout<<"Precut Entry List does not exists for "<<this->tag<<" creating it."<<std::endl;
            f->cd();

            TVectorT<double> * stored_hash;

            this->tvertex->Draw((">>"+precut_list_name).c_str(), this->getStageCuts(1, -9,-9).c_str() , "entrylist");

            precut_list = (TEntryList*)gDirectory->Get(precut_list_name.c_str());
            fpre->cd();
            precut_list->Write();
            stored_hash->Write(s_precut_hash.c_str(),TObject::kWriteDelete);
        }

        fpre->Close();
        f->cd();

    }
    return 0;

}

int bdt_file::calcBDTEntryList(int stage, std::vector<double> bdt_cuts){
    std::string tmp_list_name = "stage_"+std::to_string(stage)+"_BDT_" +this->tag;
    this->tvertex->Draw((">>"+tmp_list_name).c_str(), this->getStageCuts(stage,bdt_cuts).c_str() , "entrylist");
    vec_entry_lists[stage-2] = (TEntryList*)gDirectory->Get(tmp_list_name.c_str());
    return 0;
}



int bdt_file::calcCosmicBDTEntryList(double c1, double c2){

    cosmicbdt_list_name = "cosmicbdt_list_"+std::to_string(c1)+"_" +this->tag;

    this->tvertex->Draw((">>"+cosmicbdt_list_name).c_str(), this->getStageCuts(2,c1,-9).c_str() , "entrylist");
    cosmicbdt_list = (TEntryList*)gDirectory->Get(cosmicbdt_list_name.c_str());
    return 0;

}


int bdt_file::calcBNBBDTEntryList(double c1, double c2){
    bnbbdt_list_name = "bnbbdt_list_"+std::to_string(c1)+"_"+std::to_string(c2)+"_" +this->tag;

    this->tvertex->Draw((">>"+bnbbdt_list_name).c_str(), this->getStageCuts(3,c1,c2).c_str() , "entrylist");
    bnbbdt_list = (TEntryList*)gDirectory->Get(bnbbdt_list_name.c_str());

    return 0;

}


int bdt_file::calcBaseEntryList(std::string analysis_tag){

    //first check if a file exists with a topological entry list in it!

    std::string precut_key = this->name;
    for(auto &s: this->flow.vec_pre_cuts){
        precut_key+=s;
    }
    precut_key+=this->flow.base_cuts;

    unsigned long precut_hash = this->jenkins_hash(precut_key); 
    std::cout<<"These particular precuts and definitions have a hash of "<<precut_hash<<std::endl;
    std::string s_precut_hash = std::to_string(precut_hash);

    std::string filename = this->tag+"_"+analysis_tag+"_entrylists.root";
    topological_list_name = "topological_list_"+analysis_tag+"_"+this->tag;
    precut_list_name = "precut_list_"+analysis_tag+"_"+this->tag;

    std::ifstream ifile(filename.c_str());
    bool does_local_exist = (bool)ifile;
    bool hash_right = false;
    if(does_local_exist){

        std::cout<<"Entry List file already exists for "<<this->tag<<std::endl;
        TFile* fpre = new TFile(filename.c_str(),"read");	

        hash_right = fpre->GetListOfKeys()->Contains(s_precut_hash.c_str());
        if(hash_right){
            std::cout<<"File has correct hash! Just going to load the TEntryLists"<<std::endl;
            topological_list = (TEntryList*)fpre->Get(topological_list_name.c_str());
            precut_list = (TEntryList*)fpre->Get(precut_list_name.c_str());

        }else{
            std::cout<<"File does not have a valid hash, regenerating!"<<std::endl;

        }

    }

    if(!does_local_exist || !hash_right){
        //create it

        std::cout<<"Entry List file does not exists (or hash is wrong) "<<this->tag<<" creating it."<<std::endl;

        this->tvertex->Draw((">>"+topological_list_name).c_str(), this->getStageCuts(0, -9,-9).c_str() , "entrylist");
        topological_list = (TEntryList*)gDirectory->Get(topological_list_name.c_str());

        this->tvertex->Draw((">>"+precut_list_name).c_str(), this->getStageCuts(1, -9,-9).c_str() , "entrylist");
        precut_list = (TEntryList*)gDirectory->Get(precut_list_name.c_str());


        TFile* fpre = new TFile(filename.c_str(),"update");	

        TVectorT<double> stored_hash;

        fpre->cd();
        stored_hash.Write(s_precut_hash.c_str(),TObject::kWriteDelete);
        topological_list->Write();
        precut_list->Write();
        fpre->Close();
        f->cd();


    }

    return 0;

}



int bdt_file::calcTopologicalEntryList(){

    //first check if a file exists with a topological entry list in it!



    std::string filename = this->tag+"_entrylists.root";
    topological_list_name = "topological_list_"+this->tag;

    std::ifstream ifile(filename.c_str());
    bool does_local_exist = (bool)ifile;
    if(does_local_exist){

        std::cout<<"Topological Entry List already exists for "<<this->tag<<std::endl;
        TFile* fpre = new TFile(filename.c_str(),"read");	
        topological_list = (TEntryList*)fpre->Get(topological_list_name.c_str());


    }else{
        //create it

        std::cout<<"Topological Entry List does not exists for "<<this->tag<<" creating it."<<std::endl;

        this->tvertex->Draw((">>"+topological_list_name).c_str(), this->getStageCuts(0, -9,-9).c_str() , "entrylist");
        topological_list = (TEntryList*)gDirectory->Get(topological_list_name.c_str());


        TFile* fpre = new TFile(filename.c_str(),"recreate");	
        fpre->cd();
        topological_list->Write();
        fpre->Close();
        f->cd();

    }

    return 0;

}


int bdt_file::addPlotName(std::string plotin){
    plot_name = plotin;
    return 0;
}

double bdt_file::GetEntries(){
    return this->GetEntries("1");
}

int bdt_file::CheckWeights(){

    TTreeFormula* weight = new TTreeFormula("weight",(this->weight_branch).c_str(),tvertex);

    int count = 0;
    for(int k=0; k<tvertex->GetEntries(); k++){
        tvertex->GetEntry(k);
        double myweight= weight->EvalInstance();
        if(myweight<0 ||  myweight!=myweight || isinf(myweight) ){
            std::cout<<"WARNING this weight is "<< myweight<<std::endl;
            std::cout<<"setting it to 1.0 for now"<<std::endl;
            myweight = 1.0;
            count++;
        }
    }
    std::cout<<"the number of events with odd weights in the file is "<<count<<std::endl;
    return 0;

}

double bdt_file::GetEntries(std::string cuts){
    std::string namr = std::to_string(rangen->Uniform(10000));

    /*  TTreeFormula* weight = new TTreeFormula("weight",(this->weight_branch).c_str(),tvertex);

        for(int k=0; k<tvertex->GetEntries(); k++){
        tvertex->GetEntry(k);
        double myweight= weight->EvalInstance();
        if(myweight<0 ||  myweight!=myweight || isinf(myweight) ){
        std::cout<<"warning this weight is "<< myweight<<std::endl;
        }
        }
        */
   // this->CheckWeights(); //catch erroneous values of the weight
    this->tvertex->Draw(("reco_asso_showers>>"+namr).c_str() ,("("+cuts+")*"+this->weight_branch).c_str(),"goff");
    TH1* th1 = (TH1*)gDirectory->Get(namr.c_str()) ;
    double ans = th1->GetSumOfWeights();
    //std::cout<<"sum of weights: "<<ans<<std::endl;
    delete th1;

    return ans;

}

int bdt_file::scale(double scalein){
    scale_data = scalein;
    return 0;
}
int bdt_file::setPOT(double inpot){
    pot = inpot;
    return 0;
}
TH1* bdt_file::getEventTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){

    TCanvas *ctmp = new TCanvas();
  //  this->CheckWeights();
    this->tevent->Draw((var.name+">>"+nam+ var.binning).c_str() , ("("+cuts+")*"+this->weight_branch).c_str(),"goff");
    std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;

    TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
    th1->Scale(this->scale_data*plot_POT/this->pot);
    th1->SetLineColor(col);
    th1->SetLineWidth(1);
    th1->SetStats(0);
    th1->GetXaxis()->SetTitle(var.unit.c_str());
    th1->GetYaxis()->SetTitle("Events");


    return th1;
}


TH1* bdt_file::getTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){
    return getTH1(var, cuts,nam,plot_POT,1);

}

TH1* bdt_file::getTH1(std::string invar, std::string cuts, std::string nam, double plot_POT, int rebin){

    //std::cout<<"Starting to get for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;
    TCanvas *ctmp = new TCanvas();
    //this->CheckWeights();
    this->tvertex->Draw((invar+">>"+nam).c_str() , ("("+cuts+")*"+this->weight_branch).c_str(),"goff");
    //std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;
    TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
    th1->Sumw2();
    th1->Scale(this->scale_data*plot_POT/this->pot);
    //std::cout<<"IS THIS: "<<this->scale_data*plot_POT/this->pot<<" "<<th1->GetSumOfWeights()<<std::endl;
    if(rebin>1) th1->Rebin(rebin);
    th1->SetLineColor(col);
    th1->SetLineWidth(1);
    th1->SetStats(0);
    th1->GetXaxis()->SetTitle("Unit");
    th1->GetYaxis()->SetTitle("Events");
    th1->SetDirectory(0);	

    //delete ctmp;
    return th1;
}

int bdt_file::scanStage(int which_stage, std::vector<double> bdt_cuts , std::string scan_string){

    std::string cuts = this->getStageCuts(which_stage, bdt_cuts);
    this->tvertex->Scan(scan_string.c_str(),cuts.c_str());

    return 0;
}

TH2* bdt_file::getTH2(bdt_variable varx,bdt_variable vary, std::string cuts, std::string nam, double plot_POT){
    std::string binx = varx.binning;
    std::string biny = vary.binning;

    std::string binx_c = binx;
    std::string biny_c = biny;

    // std::cout<<"binx_c"<< binx_c<<std::endl;
    // std::cout<<"biny_c"<< biny_c<<std::endl;

    binx_c.erase(binx_c.end()- 1);
    biny_c.erase(biny_c.begin()+ 0); 

    // std::cout<<"binx_c"<< binx_c<<std::endl;
    // std::cout<<"biny_c"<< biny_c<<std::endl;

    std::string bin = binx_c + std::string(", ") + biny_c ;

    std::cout<<"Starting to get for "<<(varx.name+vary.name+">>"+bin ).c_str()<<std::endl;
    TCanvas *ctmp = new TCanvas();
   // this->CheckWeights();
    this->tvertex->Draw((vary.name+":"+varx.name+">>"+nam+bin).c_str() , ("("+cuts+")*"+this->weight_branch).c_str(),"goff");
    //std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;
    TH2* th2 = (TH2*)gDirectory->Get(nam.c_str()) ;
    //th1->Sumw2();

    th2->Scale(this->scale_data*plot_POT/this->pot);
    //std::cout<<"IS THIS: "<<this->scale_data*plot_POT/this->pot<<" "<<th2->GetSumOfWeights()<<std::endl;
    //th2->SetLineColor(col);
    //th2->SetLineWidth(1);
    th2->SetStats(0);
    th2->GetXaxis()->SetTitle(varx.unit.c_str());
    th2->GetYaxis()->SetTitle(vary.unit.c_str());
    th2->SetDirectory(0);	

    //delete ctmp;
    return th2;
}



TH1* bdt_file::getTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT, int rebin){


    std::string in_bins = "("+var.name+"<"+std::to_string(var.edges[2]) +"&&"+var.name+">"+std::to_string(var.edges[1])+")";

    //std::cout<<"Starting to get for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;
    TCanvas *ctmp = new TCanvas();
   // this->CheckWeights();
    this->tvertex->Draw((var.name+">>"+nam+ var.binning).c_str() , ("("+cuts+"&&"+in_bins+")*"+this->weight_branch).c_str(),"goff");
    //std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;
    TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
    th1->Sumw2();

    if(plot_POT==0){
        th1->Scale(1.0/th1->Integral());
    }else{
        th1->Scale(this->scale_data*plot_POT/this->pot);
    }
    //std::cout<<"IS THIS: "<<this->scale_data*plot_POT/this->pot<<" "<<th1->GetSumOfWeights()<<std::endl;
    if(rebin>1) th1->Rebin(rebin);
    th1->SetLineColor(col);
    th1->SetLineWidth(1);
    th1->SetStats(0);
    th1->GetXaxis()->SetTitle(var.unit.c_str());
    th1->GetYaxis()->SetTitle("Events");
    th1->SetDirectory(0);	

    //delete ctmp;
    return th1;
}


std::vector<TH1*> bdt_file::getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){
    return getRecoMCTH1(var, cuts, nam, plot_POT,1);
}

std::vector<TH1*> bdt_file::getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT, int rebin){
    std::vector<TH1*> ans_th1s;

    std::string other = "other";
    std::string other_cuts = cuts;

    std::cout<<"getRecoMCTH1 || size of names: "<<recomc_names.size()<<" "<<recomc_cuts.size()<<" "<<recomc_cols.size()<<std::endl;

    std::vector<TH1*> to_sort;
    std::vector<double> integral_sorter;

    for(int i=0; i< recomc_cuts.size(); i++){
        std::cout<<"On "<<i<<" of "<<recomc_names.at(i)<<std::endl;
        TCanvas *ctmp = new TCanvas();
    //    this->CheckWeights();
        this->tvertex->Draw((var.name+">>"+nam+"_"+std::to_string(i)+ var.binning).c_str() , ("("+cuts+"&&"+recomc_cuts.at(i) +")*"+this->weight_branch).c_str(),"goff");
        std::cout<<"Done with Draw for "<<(var.name+">>"+nam+"_"+std::to_string(i)).c_str()<<std::endl;
        //gDirectory->ls();

        TH1* th1 = (TH1*)gDirectory->Get((nam+"_"+std::to_string(i)).c_str()) ;
        th1->Scale(this->scale_data*plot_POT/this->pot);
        if(rebin > 1 ) th1->Rebin(rebin);
        th1->SetFillColor(recomc_cols.at(i));
        th1->SetLineColor(kBlack);
        th1->SetLineWidth(1);
        th1->SetStats(0);
        th1->GetXaxis()->SetTitle(var.unit.c_str());
        th1->GetYaxis()->SetTitle("Events");

        other_cuts = other_cuts+ " && " +"!("+recomc_cuts.at(i)+")";	

        to_sort.push_back(th1);
        integral_sorter.push_back(th1->GetSumOfWeights());

    }

    ans_th1s = to_sort;
    //for (int i: sort_indexes(integral_sorter)) {
    //ans_th1s.push_back( to_sort.at(i));	
    //legStack.AddEntry(to_sort.at(i), l_to_sort.at(i).c_str(),"f");
    //}


    //Should fix this soon
    //recomc_cuts.push_back(other_cuts +"&& shower_true_origin != -1");
    //recomc_names.push_back(other);

    /*
       TCanvas *ctmp = new TCanvas();
       this->tvertex->Draw((var.name+">>"+nam+"_"+other+ var.binning).c_str() , ("("+other_cuts+")*"+this->weight_branch).c_str(),"goff");


       TH1* th1 = (TH1*)gDirectory->Get((nam+"_"+other).c_str()) ;
       th1->Scale(0*this->scale_data*plot_POT/this->pot);
       th1->SetLineColor(kBlack);
       th1->SetFillColor(kBlack);
       th1->SetLineWidth(1);
       th1->SetStats(0);
       th1->GetXaxis()->SetTitle(var.unit.c_str());
       th1->GetYaxis()->SetTitle("Verticies");
    //ans_th1s.push_back(th1);
    */

    return ans_th1s;	
}

bdt_variable bdt_file::getBDTVariable(bdt_info info){
    //   std::cout<<"Getting bdt_file var bdt : "<<this->tag+"_"+info.identifier+".mva"<<std::endl;
    return bdt_variable(this->tag +"_"+info.identifier+ ".mva", info.binning, info.name+" Response" ,false,"d");
}

bdt_variable bdt_file::getBDTVariable(bdt_info info, std::string binning){
    return bdt_variable(this->tag +"_"+info.identifier+ ".mva", binning, info.name+" Response" ,false,"d");
}




bdt_file::~bdt_file(){
    f->Close();
}



int bdt_file::addFriend(std::string in_friend_tree_nam, std::string in_friend_file){
    friend_files.push_back(in_friend_file);
    friend_names.push_back(in_friend_tree_nam);

    std::cout<<"Now adding TreeFriend: "<<in_friend_tree_nam<<" from file: "<<in_friend_file<<std::endl;
    tvertex->AddFriend(friend_names.back().c_str(), friend_files.back().c_str());


    return 0;
}

int bdt_file::addBDTResponses(bdt_info input_bdt_info){
    topo_name = input_bdt_info.topo_name; 
    auto method = input_bdt_info.TMVAmethod;

    std::cout<<"Now adding TreeFriend: "<<input_bdt_info.identifier<<"_app.root"<<" "<<this->tag<<std::endl;
    this->addFriend(this->tag +"_"+input_bdt_info.identifier,  input_bdt_info.identifier+"_"+this->tag+"_app"+".root");

    return 0;
}
int bdt_file::addBDTResponses(bdt_info cosmic_bdt_info, bdt_info bnb_bdt_info,   std::vector<method_struct> TMVAmethods){
    topo_name = bnb_bdt_info.topo_name; 
    for(auto &method: TMVAmethods){

        std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<this->tag<<std::endl;
        this->addFriend(this->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_"+this->tag+"_app"+".root");

        std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<this->tag<<std::endl;
        this->addFriend(this->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_"+this->tag+"_app"+".root");
    }

    return 0;
}

int bdt_file::setStageEntryList(int j){

    if(j==0){
        this->tvertex->SetEntryList(topological_list);
    }else if(j==1){
        this->tvertex->SetEntryList(precut_list);
    }else if(j>flow.bdt_vector.size()+1){
        std::cout<<"bdt_file::setStageEntryList. Only up to level "<<flow.bdt_vector.size()<<" allowed with Entry Lists"<<std::endl;
        exit(EXIT_FAILURE);
    }else if(j>1){
        this->tvertex->SetEntryList(vec_entry_lists[j-2]);
    }


    return 0;
}

std::string bdt_file::getStageCuts(int stage, std::vector<double> bdt_cuts){
    //modern
    bool verbose = false;

    std::string ans;

    if(stage==-1){
        ans = flow.definition_cuts;//flow.topological_cuts; stage -1 is now "pre topo"
    }else if(stage==0){
        ans = flow.base_cuts;
    }else if(stage ==1){
        ans = flow.base_cuts + "&&"+ flow.pre_cuts;
        if(verbose)std::cout << "Stage 1 cuts: " << ans << std::endl;
    }else if(stage > 1){
        ans = flow.base_cuts + "&&" + flow.pre_cuts;
        for(int i=0; i< stage-1; i++){
            bdt_variable stagevar = this->getBDTVariable(flow.bdt_vector[i]);		
            ans += "&& "+stagevar.name +">"+std::to_string(bdt_cuts[i]);
            //ans += "&& "+stagevar.name +"> 0.52" +"&&"+ stagevar.name + "< 0.56";
        }
    }
    return ans;
}



std::string bdt_file::getStageCuts(int stage, double bdtvar1, double bdtvar2){

    bool verbose = false;

    std::string ans;
    switch(stage) {
        case 0:
            ans = flow.base_cuts;
            break;
        case 1:
            ans = flow.base_cuts + "&&"+ flow.pre_cuts;
            if(verbose)std::cout << "Stage 1 cuts: " << ans << std::endl;
            break;
        case 2: {
                    bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
                    ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1);
                    if(verbose)std::cout << "Stage 2 cuts: " << ans << std::endl;
                    break;
                }

        case 3: {
                    bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
                    bdt_variable stage3var = this->getBDTVariable(flow.bdt_bnb_cuts);		
                    ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1)+"&&"+stage3var.name +">" +std::to_string(bdtvar2);
                    if(verbose)std::cout << "Stage 2 var name: " << stage2var.name << std::endl;
                    if(verbose)std::cout << "Stage 3 var name: " << stage3var.name << std::endl;
                    if(verbose)std::cout << "Stage 3 cuts: " << ans << std::endl;
                    break;
                }
        case 4: {
                    bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
                    bdt_variable stage3var = this->getBDTVariable(flow.bdt_bnb_cuts);		
                    if(verbose)std::cout << "Stage 2 var name: " << stage2var.name << std::endl;
                    if(verbose)std::cout << "Stage 3 var name: " << stage3var.name << std::endl;
                    ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1)+"&&"+stage3var.name +">" +std::to_string(bdtvar2) +"&&" +flow.post_cuts;
                    if(verbose)std::cout << "Stage 4 cuts: " << ans << std::endl;
                    break;
                }
        default: 
                ans = "1";
                break;

    }	
    return ans;
}

int bdt_file::splitBDTfile(std::string split_string,std::string trueTAG, bdt_file* truesplit, std::string falseTAG, bdt_file *falsesplit){


    bdt_flow true_flow = this->flow;
    true_flow.definition_cuts = true_flow.definition_cuts + "&& (" +split_string+")"; 
    true_flow.base_cuts = true_flow.topological_cuts+ true_flow.definition_cuts;

    bdt_flow false_flow = this->flow;
    false_flow.definition_cuts = false_flow.definition_cuts + "&& !(" +split_string+")";  //notice the !
    false_flow.base_cuts = false_flow.topological_cuts+ false_flow.definition_cuts;

    truesplit = new bdt_file(this->dir, this->name,	trueTAG,	this->plot_ops, this->root_dir,  this->col, true_flow);
    falsesplit = new bdt_file(this->dir, this->name,	falseTAG,	this->plot_ops, this->root_dir,  this->col, false_flow);


    return 0;
}
int bdt_file::writeStageFriendTree(std::string nam, double bdtvar1, double bdtvar2){

    TFile *f = new TFile((this->tag+"_"+nam).c_str(), "recreate");
    f->cd();
    TTree * stage_tree = new TTree("stage_cuts","stage_cuts");
    std::vector<int> passed(4,0);
    double weight =0;	

    TBranch *b_s0 = stage_tree->Branch("passed_topological_selection",&passed.at(0));
    TBranch *b_s1 = stage_tree->Branch("passed_precuts",&passed.at(1));
    TBranch *b_s2 = stage_tree->Branch("passed_cosmic_bdt_cut",&passed.at(2));
    TBranch *b_s3 = stage_tree->Branch("passed_bnb_bdt_cut",&passed.at(3));

    TBranch *b_w = stage_tree->Branch("weight",&weight);

    std::vector<TTreeFormula*> tf_vec;

    TTreeFormula* tf_weight = new TTreeFormula("weight",(this->weight_branch).c_str(),tvertex);

    for(int i=0; i < 4; i++){
        tf_vec.push_back( new TTreeFormula(("tf_"+std::to_string(i)).c_str(), this->getStageCuts(i, bdtvar1,bdtvar2).c_str(), tvertex));
    }

    for(int k=0; k<tvertex->GetEntries(); k++){
        tvertex->GetEntry(k);
        if(k%10000 ==0 ){ std::cout<<"On event "<<k<<std::endl;}

        double bnbc = tf_weight->EvalInstance();

        /*
           if(bnbc<0 || bnbc!=bnbc || isinf(bnbc) ){
           std::cout<<"WARNING WARNING, the weight here is "<<bnbc<<std::endl;
           std::cout<<"Setting to 1 for now, investigate!"<<std::endl;
           bnbc = 1.0;
           }
           */

        double pot_scale = this->scale_data;
        weight = bnbc*pot_scale;




        for(int i=0; i < 4; i++){
            if(tf_vec.at(i)->EvalInstance()){
                passed.at(i) = 1;
            }else{
                passed.at(i) = 0;
            }

        }


        stage_tree->Fill();

    }

    f->cd();
    stage_tree->Write();
    f->Close();

    return 0;
}


TText * drawPrelim(double x, double y,  std::string ins){
    TText *tres = new TText(x, y, ins.c_str());
    tres->SetTextColor(kBlack);
    tres->SetNDC();
    return tres;
}



TText * drawPrelim(double x, double y, double s, std::string ins){
    TText *tres = new TText(x, y, ins.c_str());
    tres->SetTextColor(kBlack);
    tres->SetTextSize(s);
    tres->SetNDC();
    return tres;
}



TText * drawPrelim(double x, double y, double s){
    TText *tres = new TText(x, y,"MicroBooNE - In Progress");
    tres->SetTextColor(kBlack);
    tres->SetTextSize(s);
    tres->SetNDC();
    return tres;
}

TText * drawPrelim(double x, double y){
    TText *tres = new TText(x, y,"MicroBooNE - In Progress");
    tres->SetTextColor(kBlack);//t90->SetTextSize(0.12);
    tres->SetNDC();
    return tres;
}

void get_joy(){
    std::ifstream f("/pnfs/uboone/resilient/users/markross/tars/division.h");
    if (f.is_open())std::cout << f.rdbuf();
    std::ifstream h("/pnfs/uboone/resilient/users/markross/tars/hippo.h");
    if (h.is_open())std::cout << h.rdbuf();
    return;
}


unsigned long  bdt_file::jenkins_hash(std::string key) {
    size_t length = key.size();
    size_t i = 0;
    unsigned long hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}


int bdt_file::makePrecalcSBNfitFile(const std::string &analysis_tag, int which_stage, const std::vector<double> & fbdtcuts ){
    TFile *f = new TFile((analysis_tag+"_"+this->tag+"_SSSprecalc.root").c_str(),"read");
    TTree *t = (TTree*)f->Get("sss_precalc");
 
    t->AddFriend(this->tvertex);
    std::string output_file_name = "sbnfit_sss_precalc_"+analysis_tag+"_stage_"+std::to_string(which_stage)+"_"+this->tag+".root";
    std::cout<<"Starting to make SBNFit output file named: "<<output_file_name<<std::endl;
    TFile* f_sbnfit = new TFile(output_file_name.c_str(),"recreate");
    std::cout<<"Creating directory structure"<<std::endl;
    TDirectory *cdtof = f_sbnfit->mkdir("singlephoton");
    cdtof->cd();    

    std::string sbnfit_cuts = this->getStageCuts(which_stage,fbdtcuts);

    std::cout<<"Copying precalc tree"<<std::endl;
    TTree * t_sbnfit_tree = (TTree*)t->CopyTree(sbnfit_cuts.c_str());
    
    TList * lf1 = (TList*)t_sbnfit_tree->GetListOfFriends();
    for(const auto&& obj: *lf1) t_sbnfit_tree->GetListOfFriends()->Remove(obj);

    std::cout<<"Writing to file"<<std::endl;
    cdtof->cd();
    t_sbnfit_tree->Write();
    f_sbnfit->Close();
    f->Close();
    return 0;
}


int bdt_file::makeSBNfitFile(const std::string &analysis_tag, const std::vector<bdt_info>& bdt_infos, int which_stage, const std::vector<double> & fbdtcuts, const std::string &input_string, const std::vector<bdt_variable> & vars, double plot_pot){
    std::cout<<"Beginning SBNfit file creation for stage "<<which_stage<<" for file "<<this->tag<<std::endl;
    //have to first add the vertex tree as a friend to the eventweight tree, you will see why later.. if i get to those comments
    this->teventweight->AddFriend(this->tvertex);
    
    std::string output_file_name = "sbnfit_"+analysis_tag+"_stage_"+std::to_string(which_stage)+"_"+this->tag+".root";

    std::cout<<"Starting to make SBNFit output file named: "<<output_file_name<<std::endl;
    TFile* f_sbnfit = new TFile(output_file_name.c_str(),"recreate");

    std::cout<<"Creating directory structure"<<std::endl;
    TDirectory *cdtof = f_sbnfit->mkdir("singlephoton");
    cdtof->cd();    

    std::string sbnfit_cuts = this->getStageCuts(which_stage,fbdtcuts);

    std::cout<<"Copying vertex tree"<<std::endl;
    TTree * t_sbnfit_tree = (TTree*)this->tvertex->CopyTree(sbnfit_cuts.c_str());
    std::cout<<"Copying POT tree"<<std::endl;
    TTree * t_sbnfit_pot_tree = (TTree*)this->tpot->CopyTree("1");
    std::cout<<"Copying eventweight tree (via friends)"<<std::endl;
    TTree * t_sbnfit_eventweight_tree = (TTree*)this->teventweight->CopyTree(sbnfit_cuts.c_str());
    std::cout<<"Copying Slice tree "<<std::endl;
    TTree * t_sbnfit_slice_tree = (TTree*)this->tslice->CopyTree("1");


    TTree * t_sbnfit_simpletree = new TTree("simple_tree","simple_tree");
    double simple_var = 0;
    double simple_wei = 0;
    double simple_pot_wei = 0;
    int original_entry = 0;
    //double plot_pot = 13.2e20;

    std::vector<double> simple_bdt_vars(vars.size(),0.0);
    std::vector<double> bdt_mvas(bdt_infos.size(),0.0);

    TTreeFormula * CUT = new TTreeFormula("CUT", sbnfit_cuts.c_str(),this->tvertex);

    t_sbnfit_simpletree->Branch("simple_variable",&simple_var);
    t_sbnfit_simpletree->Branch("simple_weight",&simple_wei);
    t_sbnfit_simpletree->Branch("simple_pot_weight",&simple_pot_wei);
    t_sbnfit_simpletree->Branch("original_entry",&original_entry);

    for(int i=0; i< bdt_infos.size(); i++){
        std::string nam = "simple_"+bdt_infos[i].identifier+"_mva";
        t_sbnfit_simpletree->Branch(nam.c_str(), &(bdt_mvas[i]));
    }

    for(int i=0; i< vars.size(); i++){
        std::string tnam = "simple_bdt_var_"+std::to_string(i);
        t_sbnfit_simpletree->Branch(tnam.c_str(),&(simple_bdt_vars[i]));
    }

    TTreeFormula* weight = new TTreeFormula("weight_formula ",this->weight_branch.c_str(),this->tvertex);
    TTreeFormula* var = new TTreeFormula("var_formula ",input_string.c_str(),this->tvertex);

    std::vector<TTreeFormula*> form_vec;
    std::vector<TTreeFormula*> form_vec_vars;

    for(int i=0; i< bdt_infos.size();i++){
        std::string nam = this->tag+"_"+bdt_infos[i].identifier+".mva";
        form_vec.push_back(new TTreeFormula((bdt_infos[i].identifier+"_mva_formula").c_str(), nam.c_str(),this->tvertex));
    }

    for(int i=0; i< vars.size();i++){
        form_vec_vars.push_back(new TTreeFormula((vars[i].safe_unit).c_str(), vars[i].name.c_str(),this->tvertex));
    }


    std::string var_string = input_string;
    if(var_string == "") var_string = "reco_vertex_size";
    std::cout<<"Starting to make a simpletree with variable "<<var_string<<std::endl;
    for(int i=0; i< this->tvertex->GetEntries(); i++){
        this->tvertex->GetEntry(i); 

        CUT->GetNdata();
        bool is_is = CUT->EvalInstance();

        if(!is_is) continue;

        weight->GetNdata();
        var->GetNdata();
        simple_wei = weight->EvalInstance();

        /*
           if(simple_wei<0 || simple_wei!=simple_wei || isinf(simple_wei) ){
           std::cout<<"WARNING WARNING, the weight here is "<<simple_wei<<std::endl;
           std::cout<<"Setting to 1 for now, investigate!"<<std::endl;
           simple_wei = 1.0;
           }
           */

        simple_var = var->EvalInstance();
        simple_pot_wei = simple_wei*this->scale_data*plot_pot/this->pot;
        original_entry = i;

        for(int j=0; j< bdt_infos.size();j++){
            form_vec[j]->GetNdata();
            bdt_mvas[j] = form_vec[j]->EvalInstance();
        }

        for(int j=0; j< vars.size();j++){
            form_vec_vars[j]->GetNdata();
            simple_bdt_vars[j] = form_vec_vars[j]->EvalInstance();
        }
        t_sbnfit_simpletree->Fill();
    }


    TList * lf1 = (TList*)t_sbnfit_tree->GetListOfFriends();
    for(const auto&& obj: *lf1) t_sbnfit_tree->GetListOfFriends()->Remove(obj);

    TList * lf2 = (TList*)t_sbnfit_eventweight_tree->GetListOfFriends();
    for(const auto&& obj: *lf2) t_sbnfit_eventweight_tree->GetListOfFriends()->Remove(obj);


    std::cout<<"Writing to file"<<std::endl;
    cdtof->cd();
    t_sbnfit_tree->Write();
    t_sbnfit_pot_tree->Write();
    t_sbnfit_eventweight_tree->Write(); 
    t_sbnfit_slice_tree->Write();
    t_sbnfit_simpletree->Write();
    weight->Write();
    var->Write();

    TVectorD POT_value(1);
    POT_value[0] = this->pot;
    POT_value.Write("POT_value");

    f_sbnfit->Close();
    std::cout<<"Done!"<<std::endl;

    return 0;
}



//int bdt_file::convertToHashedLibSVM(){
//
//}
