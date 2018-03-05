

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


std::vector<std::vector<double>> const & Permutations::Get() {

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

  return permutation_v;
  
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
