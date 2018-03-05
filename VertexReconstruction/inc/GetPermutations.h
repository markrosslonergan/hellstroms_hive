

#ifndef GETPERMUTATIONS_H
#define GETPERMUTATIONS_H

#include <iostream>
#include <vector>


class Permutations {
  
 public:

  void AddParameter(std::string const & name,
		    int const inc,
		    double const min, 
		    double const max);

  void AddParameter(std::string const & name,
		    double const set_value);

  std::vector<std::vector<double>> const & Get();  
  void Check();
  std::string GetString(size_t const i);

 private:

  std::vector<std::string> parameter_name;
  std::vector<double> parameter_set;
  std::vector<double> parameter_min;
  std::vector<double> parameter_max;
  std::vector<double> parameter_inc;
  std::vector<double> parameter_inc_size;
  std::vector<std::vector<double>> permutation_v;

};

#endif
