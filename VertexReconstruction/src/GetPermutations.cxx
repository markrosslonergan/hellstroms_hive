

#include "GetPermutations.h"


void Permutations::AddParameter(std::string const & name,
				int const inc,
				double const min, 
				double const max) {

  parameter_name.push_back(name);
  parameter_min.push_back(min);
  parameter_max.push_back(max);
  parameter_inc.push_back(inc);
  parameter_inc_size.push_back((max - min) / inc);
  parameter_set.push_back(-1);

}


void Permutations::AddParameter(std::string const & name,
				double const set_value) {

  parameter_name.push_back(name);
  parameter_min.push_back(-1);
  parameter_max.push_back(-1);
  parameter_inc.push_back(-1);
  parameter_inc_size.push_back(-1);
  parameter_set.push_back(set_value);

}


void Permutations::Get() {

  bool continue_bool;
  std::vector<double> parameters = parameter_min;
  for(size_t i = 0; i < parameter_set.size(); ++i) {
    if(parameter_set.at(i) != -1) parameters.at(i) = parameter_set.at(i);     
  }
  permutation_v.push_back(parameters);
  do {
    continue_bool = false;
    for(int i = parameters.size() - 1; i >= 0; --i) {
      if(parameter_set.at(i) != -1) continue;
      if(parameters.at(i) < parameter_max.at(i)) {
	parameters.at(i) += parameter_inc_size.at(i);
	for(size_t j = parameters.size() - 1; int(j) > i; --j) {
	  if(parameter_set.at(j) == -1) parameters.at(j) = parameter_min.at(j);
	  else parameters.at(j) = parameter_set.at(j);
	}
	continue_bool = true;
	break;
      }
    } 
    if(continue_bool) permutation_v.push_back(parameters);
  } while(continue_bool);

}


std::vector<std::vector<double>> const & Permutations::GetV() {

  Get();
  return permutation_v;

}


void Permutations::WriteFile(std::string const & file_name,
			     std::pair<size_t, size_t> const & index_range) {

  TFile * file = TFile::Open(file_name.c_str(), "recreate");  
  TTree * tree = new TTree("permutation_tree", "");

  std::vector<double> parameter_values(parameter_name.size(), 0);
  for(size_t i = 0; i < parameter_values.size(); ++i) {
    tree->Branch(parameter_name.at(i).c_str(), &parameter_values.at(i), (parameter_name.at(i) + "/D").c_str());
  }

  for(size_t i = index_range.first; i <= index_range.second; ++i) {
    std::vector<double> const & permutation = permutation_v.at(i);
    for(size_t j = 0; j < parameter_values.size(); ++j) {
      parameter_values.at(j) = permutation.at(j);
    }
    tree->Fill();
  }

  tree->Write();
  file->Close();

}


void Permutations::GetFiles(std::string const & file_name, int const job_number) {

  Get();

  size_t const job_inc = permutation_v.size() / job_number;
  size_t job_mod = permutation_v.size() % job_number;
  int index = -1;
  std::vector<std::pair<size_t, size_t>> index_v;

  for(size_t i = 0; i < job_number; ++i) {
    ++index;
    size_t min = index;
    index += job_inc - 1;
    if(job_mod > 0) {
      ++index;
      --job_mod;
    }
    index_v.push_back({min, index});
  }

  for(size_t i = 0; i < index_v.size(); ++i)
    WriteFile(file_name + "_" + std::to_string(i) + ".root", index_v.at(i));

}


std::vector<std::vector<double>> const & Permutations::ReadFile(char const * file_name) {
      
  TFile * file = TFile::Open(file_name);
  if(!file) {
    std::cout << "Could not open " << file_name << "\n";
    exit(1);
  }
  TTree * tree = (TTree*)file->Get("permutation_tree");
  if(!tree) {
    std::cout << "Could not find permutation_tree\n";
    exit(1);
  }
  if(tree->GetEntries() == 0) {
    std::cout << "Empty permutation_tree\n";
    exit(1);
  }

  TObjArray * branches = tree->GetListOfBranches();
  std::vector<double> parameters(branches->GetEntries(), -10000);
  for(int i = 0; i < branches->GetEntries(); ++i) {
    tree->SetBranchAddress(branches->At(i)->GetName(), &parameters.at(i));
    parameter_name.push_back(branches->At(i)->GetName());
  }

  for(int i = 0; i < tree->GetEntries(); ++i) {
    tree->GetEntry(i);
    permutation_v.push_back(parameters);
  }

  file->Close();

  return permutation_v;

}


void Permutations::WritePermutationTreeToFile() {

  TTree * tree = new TTree("permutation_tree", "");

  std::vector<double> parameters(parameter_name.size(), -10000);
  for(int i = 0; i < parameter_name.size(); ++i) {
    tree->Branch(parameter_name.at(i).c_str(), &parameters.at(i), (parameter_name.at(i) + "/D").c_str());
  }

  for(std::vector<double> const & permutation : permutation_v) {
    for(size_t i = 0; i < parameters.size(); ++i) parameters.at(i) = permutation.at(i);
    tree->Fill();
  }

  tree->Write();
  delete tree;

}


void Permutations::Check() {

  int permutations = 1;
  for(size_t i = 0; i < parameter_set.size(); ++i) {
    if(parameter_set.at(i) != -1) continue;
    permutations *= (parameter_inc.at(i) + 1);
  }

  for(std::vector<double> const & v : permutation_v) {
    for(double const d : v) std::cout << d << " ";
    std::cout << "\n";
  }

  std::cout << "size: " << permutation_v.size() << " permutations: " << permutations << "\n";

}
