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
    std::cout << "Toplogical cuts: " << flow.topological_cuts.c_str() << std::endl;
    std::cout << "Definition cuts: " << flow.definition_cuts.c_str() << std::endl;

	for(method_struct const & method : methods) {
		reader->BookMVA(method.str.c_str(), ("BDTxmls_"+identifier+"/weights/"+identifier+"_"+method.str+".weights.xml").c_str());
		bdt_app_tree_struct ts(otree_name, false);
	
        int passed = 0;
        int N = tree->GetEntries();
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
                    passed++;
                    //std::cout << "Found EvaluateMVA(method.str.c_str()) = " << ts.mva << std::endl;
                }
            }
            ts.tree->Fill();
        }
		ts.tree->Write();
        std::cout << "Number of events passed: " << passed << std::endl;
    }
      

	delete reader;
	delete tf_topological;
	delete tf_definition;
	for(float * f : reader_var_v) delete f;
	for(auto * t : tree_formulas_v) delete t;


}


int bdt_app(bdt_info info, std::vector<bdt_file*> files, std::vector<bdt_variable> vars, std::vector<method_struct> & method){

	std::string identifier = info.identifier;
	
	TFile * app_ofile = TFile::Open((identifier+"_app"+".root").c_str(), "update");
	for(size_t i = 0; i < files.size(); ++i) {
		files.at(i)->tvertex->ResetBranchAddresses();

		std::cout<<"On file: "<<files.at(i)->tag<<std::endl;
		std::string bdt_response_friend_tree_name = files.at(i)->tag+"_"+info.identifier;
		bdt_app_tree(identifier, files.at(i)->tvertex, files.at(i)->flow, bdt_response_friend_tree_name , vars, method);
	}
	app_ofile->Close();
	
	delete app_ofile;



	return 0;
}
