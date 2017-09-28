


#include "void_vec.hpp"
#include "app_tree_struct.hpp"



void update(void_vec const & tvv, std::vector<float *> & rvv) {
  
  for(size_t i = 0; i < tvv.size(); ++i) {
    if(tvv.get_type(i) == typeid(int).name()) {
      *rvv.at(i) = *tvv.at<int>(i);
      }
    else if(tvv.get_type(i) == typeid(double).name()) {
      *rvv.at(i) = *tvv.at<double>(i);
    }
  }
  
}



void app_tree(std::string const & identifier,
	      TTree * tree,
	      std::string const & otree_name, 
	      std::vector<std::pair<std::string, std::string>> const & variables,
	      std::vector<method_struct> const & methods) {
  
  TMVA::Reader * reader = new TMVA::Reader("!Color:!Silent");

  void_vec tree_var_v;
  std::vector<float *> reader_var_v;
  for(std::pair<std::string, std::string> const & p : variables) {
    if(p.second == "d") {
      double * dp = new double(-1);
      tree_var_v.push_back(dp);
      tree->SetBranchAddress(p.first.c_str(), dp);
      }
    else if(p.second == "i") {
      int * ip = new int(-1);
      tree_var_v.push_back(ip);
      tree->SetBranchAddress(p.first.c_str(), ip);
      }
    else {
      std::cout << __LINE__ << " " << __PRETTY_FUNCTION__ <<"\n"
		<< "ERROR: invalid type: " << p.second << "\n";
    }
    reader_var_v.push_back(new float(-1));
    reader->AddVariable(p.first.c_str(), reader_var_v.back());
  }
  
  for(method_struct const & method : methods) {
    reader->BookMVA(method.str.c_str(), ("dataset/weights/"+identifier+"_training_"+method.str+".weights.xml").c_str());
    app_tree_struct ts(otree_name+"_"+method.str, false);
    for(int i = 0; i < tree->GetEntries(); ++i) {
      tree->GetEntry(i);
	update(tree_var_v, reader_var_v);
	ts.mva = -999;
	ts.mva = reader->EvaluateMVA(method.str.c_str());
	ts.tree->Fill();
    }
    ts.tree->Write();
  }
  
  delete reader;
  for(float * f : reader_var_v) delete f;
  
}



void app(std::string const & identifier, 
	 std::vector<std::pair<TTree *, std::string>> const & trees, 
	 std::vector<std::pair<std::string, std::string>> const & variables,
	 std::vector<method_struct> const & methods) {
  
  TFile * app_ofile = TFile::Open((identifier+"_app.root").c_str(), "recreate");
  for(auto const & p : trees) app_tree(identifier, p.first, p.second, variables, methods);
  app_ofile->Close();
  
}
