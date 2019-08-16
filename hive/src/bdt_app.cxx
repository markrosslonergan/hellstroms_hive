#include "bdt_app.h"


bdt_app_tree_struct::bdt_app_tree_struct(std::string const & tree_name, bool const extra) {
    tree = new TTree(tree_name.c_str(), "");
    tree->Branch("mva", &mva, "mva/D");
}

bdt_app_tree_struct::~bdt_app_tree_struct() {
    delete tree;
}


void bdt_app_update_formula(std::vector<TTreeFormula*> & tfv, std::vector<float *> & rvv) {

    for(size_t i = 0; i < tfv.size(); ++i) {
        *rvv.at(i) = tfv.at(i)->EvalInstance();
    }
}

void bdt_app_update(void_vec const & tvv, std::vector<float *> & rvv) {
    //OBSOLETE not called anymore

    for(size_t i = 0; i < tvv.size(); ++i) {
        if(tvv.get_type(i) == typeid(int).name()) {
            *rvv.at(i) = *tvv.at<int>(i);
        }
        else if(tvv.get_type(i) == typeid(double).name()) {
            *rvv.at(i) = *tvv.at<double>(i);
        }
    }
}

void bdt_topological_update(std::vector<double*> *topv){
    //OBSOLETE not called anymore


}


int bdt_app_tree(std::string identifier, TTree * tree, bdt_flow flow, std::string otree_name, std::vector<bdt_variable> vars, std::vector<method_struct> const & methods) {

    TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

    void_vec tree_var_v;
    std::vector<float *> reader_var_v;
    std::vector<TTreeFormula*> tree_formulas_v;

    for(bdt_variable &p : vars) {
        /*
           if(p.type == "d") {

           double * dp = new double(-1);
           tree_var_v.push_back(dp);
           tree->SetBranchAddress(p.name.c_str(), dp);
           }
           else if(p.type == "i") {
           int * ip = new int(-1);
           tree_var_v.push_back(ip);
           tree->SetBranchAddress(p.name.c_str(), ip);
           }
           else {
           std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ <<"\n"
           << "ERROR: invalid type: " << p.type << "\n";
           }
           */	
        tree_formulas_v.push_back(new TTreeFormula(p.safe_name.c_str(), p.name.c_str() ,tree));
        reader_var_v.push_back(new float(-1));
        reader->AddVariable(p.name.c_str(), reader_var_v.back());
    }


    //TTreeFormula * tf = new TTreeFormula("tf", cut.c_str(), tree);
    TTreeFormula * tf_topological = new TTreeFormula("tf_top", flow.topological_cuts.c_str(), tree);
    TTreeFormula * tf_definition = new TTreeFormula("tf_def", flow.definition_cuts.c_str(), tree);

    for(method_struct const & method : methods) {
        reader->BookMVA(method.str.c_str(), ("BDTxmls_"+identifier+"/weights/"+identifier+"_"+method.str+".weights.xml").c_str());
        bdt_app_tree_struct ts(otree_name, false);

        int N = tree->GetEntries();
        std::cout << "Beginning loop for " << identifier << std::endl;
        std::cout << "############################################" << std::endl;
        for(int i = 0; i < N; ++i) {
            tree->GetEntry(i);
            ts.mva = -999;
            if(i%25000==0){           std::cout<<i<<"/"<<N<<std::endl;          }

            tf_topological->GetNdata();
            tf_definition->GetNdata();


            if(tf_topological->EvalInstance()) {
                bdt_app_update_formula(tree_formulas_v, reader_var_v);

                if(tf_definition->EvalInstance()) {
                    ts.mva = reader->EvaluateMVA(method.str.c_str());
                    //std::cout << "Found EvaluateMVA(method.str.c_str()) = " << ts.mva << std::endl;
                }
            }
            ts.tree->Fill();
        }
        ts.tree->Write();
    }


    delete reader;
    delete tf_topological;
    delete tf_definition;
    for(float * f : reader_var_v) delete f;
    for(auto * t : tree_formulas_v) delete t;


}

// Separate training and plotting variables
int bdt_app_tree(std::string identifier, TTree * tree, bdt_flow flow, std::string otree_name, std::vector<bdt_variable> train_vars, std::vector<bdt_variable> plot_vars, std::vector<method_struct> const & methods) {

    TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

    void_vec tree_var_v;
    std::vector<float *> reader_var_v;
    std::vector<TTreeFormula*> tree_formulas_v;

    for(bdt_variable &p : train_vars) {
        /*
           if(p.type == "d") {

           double * dp = new double(-1);
           tree_var_v.push_back(dp);
           tree->SetBranchAddress(p.name.c_str(), dp);
           }
           else if(p.type == "i") {
           int * ip = new int(-1);
           tree_var_v.push_back(ip);
           tree->SetBranchAddress(p.name.c_str(), ip);
           }
           else {
           std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ <<"\n"
           << "ERROR: invalid type: " << p.type << "\n";
           }
           */	
        tree_formulas_v.push_back(new TTreeFormula(p.safe_name.c_str(), p.name.c_str() ,tree));
        reader_var_v.push_back(new float(-1));
        reader->AddVariable(p.name.c_str(), reader_var_v.back());
    }

    for (bdt_variable &k: plot_vars) {
        tree_formulas_v.push_back(new TTreeFormula(k.safe_name.c_str(), k.name.c_str() ,tree));
        reader_var_v.push_back(new float(-1));
        reader->AddSpectator(k.name.c_str(), reader_var_v.back() );
    }


    //TTreeFormula * tf = new TTreeFormula("tf", cut.c_str(), tree);
    TTreeFormula * tf_topological = new TTreeFormula("tf_top", flow.topological_cuts.c_str(), tree);
    TTreeFormula * tf_definition = new TTreeFormula("tf_def", flow.definition_cuts.c_str(), tree);

    for(method_struct const & method : methods) {
        reader->BookMVA(method.str.c_str(), ("BDTxmls_"+identifier+"/weights/"+identifier+"_"+method.str+".weights.xml").c_str());
        bdt_app_tree_struct ts(otree_name, false);

        int N = tree->GetEntries();
        std::cout << "Beginning loop for " << identifier << std::endl;
        std::cout << "############################################" << std::endl;
        for(int i = 0; i < N; ++i) {
            tree->GetEntry(i);
            ts.mva = -999;
            if(i%25000==0){           std::cout<<i<<"/"<<N<<std::endl;          }

            tf_topological->GetNdata();
            tf_definition->GetNdata();


            if(tf_topological->EvalInstance()) {
                bdt_app_update_formula(tree_formulas_v, reader_var_v);

                if(tf_definition->EvalInstance()) {
                    ts.mva = reader->EvaluateMVA(method.str.c_str());
                    //std::cout << "Found EvaluateMVA(method.str.c_str()) = " << ts.mva << std::endl;
                }
            }
            ts.tree->Fill();
        }
        ts.tree->Write();
    }


    delete reader;
    delete tf_topological;
    delete tf_definition;
    for(float * f : reader_var_v) delete f;
    for(auto * t : tree_formulas_v) delete t;


}

int bdt_app(bdt_info ininfo, std::vector<bdt_file*> & infiles){
    std::vector<method_struct> t_methods = {ininfo.TMVAmethod};
    return bdt_app(ininfo, infiles, ininfo.train_vars, t_methods);
}


int bdt_app(bdt_info info, std::vector<bdt_file*> & files, std::vector<bdt_variable> vars, std::vector<method_struct> & method){

    std::string identifier = info.identifier;
    std::cout<<"Beginning bTTree *dt application stage for "<<identifier<<std::endl;

    for(size_t i = 0; i < files.size(); i++) {
        std::cout<<"On file: "<<i<<"/"<<files.size()<<" :  "<<files.at(i)->tag<<std::endl;

        TFile * app_ofile = TFile::Open((identifier+"_"+files[i]->tag+"_app"+".root").c_str(), "recreate");


        std::cout<<"Resetting the branch addresses"<<std::endl;
        files.at(i)->tvertex->ResetBranchAddresses();

        std::string bdt_response_friend_tree_name = files.at(i)->tag+"_"+info.identifier;
        bdt_app_tree(identifier, files.at(i)->tvertex, files.at(i)->flow, bdt_response_friend_tree_name , vars, method);
        bdt_app_tree(identifier, files.at(i)->tvertex, files.at(i)->flow, bdt_response_friend_tree_name , vars, method);

        app_ofile->Close();
        delete app_ofile;

    }



    return 0;
}



int bdt_app(bdt_info info, std::vector<bdt_file*> files, std::vector<bdt_variable> train_vars, std::vector<bdt_variable> plot_vars, std::vector<method_struct> & method){

    std::string identifier = info.identifier;
    std::cout<<"Beginning bdt application stage for "<<identifier<<std::endl;

    for(size_t i = 0; i < files.size(); i++) {

        std::cout<<"On file: "<<i<<"/"<<files.size()<<" :  "<<files[i]->tag<<std::endl;

        TFile * app_ofile = TFile::Open((identifier+"_"+files[i]->tag+"_app"+".root").c_str(), "recreate");

        std::cout<<"Resetting the branch addresses"<<std::endl;
        files.at(i)->tvertex->ResetBranchAddresses();

        std::string bdt_response_friend_tree_name = files.at(i)->tag+"_"+info.identifier;
        bdt_app_tree(identifier, files.at(i)->tvertex, files.at(i)->flow, bdt_response_friend_tree_name , train_vars, plot_vars, method);
        bdt_app_tree(identifier, files.at(i)->tvertex, files.at(i)->flow, bdt_response_friend_tree_name , train_vars, plot_vars, method);

        app_ofile->Close();
        delete app_ofile;

    }



    return 0;
}


int bdt_XGapp(bdt_info info, std::vector<bdt_file*> files){
    method_struct bdt_method = info.TMVAmethod;
    std::vector<bdt_variable> vars = info.train_vars;

    for(auto &f : files){
        convertToLibSVM(info, f);
    }
  
  // create the booster
    BoosterHandle booster;
    XGBoosterCreate(0, 0, &booster);
    safe_xgboost(XGBoosterLoadModel(booster,(info.identifier+".XGBoost.mod").c_str()));

    for(auto &f: files){
        TFile * app_ofile = TFile::Open((info.identifier+"_"+f->tag+"_app"+".root").c_str(), "recreate");
        std::string bdt_response_friend_tree_name = f->tag+"_"+info.identifier;
        app_ofile->cd();
        TTree * tree = new TTree(bdt_response_friend_tree_name.c_str(), "");
        double mva;
        tree->Branch("mva", &mva, "mva/D");


        DMatrixHandle dfile;
        safe_xgboost(XGDMatrixCreateFromFile((info.identifier+"_"+f->tag+".libSVM.dat").c_str(), 0, &dfile));

        bst_ulong out_len = 0;
        const float* out_result = NULL;

        XGBoosterPredict(booster, dfile, 0, 0, &out_len, &out_result);



        size_t have_filled = 0;
        size_t working_index = f->precut_list->GetEntry(0);
        for (int i = 0; i < out_len; ++i) {

            while(have_filled < working_index){
                    mva = -999;
                    tree->Fill();
                    have_filled++;  
            }
            if(have_filled == working_index){

                std::cout<<have_filled<<" "<<working_index<<" "<<i<<" "<<out_result[i]<<std::endl;      
                mva = out_result[i];
                tree->Fill();
                working_index = f->precut_list->Next();
                have_filled++;
            }else{
                std::cerr<<"AGHR, something went wrong in XGBoost_app"<<std::endl;
            }   
        }
        while(have_filled < f->tvertex->GetEntries()){
                     mva = -999;
                    tree->Fill();
                    have_filled++;  
        }


        safe_xgboost(XGDMatrixFree(dfile));

        tree->Write();
        std::cout<<" "<<tree->GetEntries()<<std::endl;
        app_ofile->Close();

    }


    return 0;
}

