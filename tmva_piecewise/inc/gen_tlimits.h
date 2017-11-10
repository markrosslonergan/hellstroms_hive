#ifndef GENTLIMITS_H
#define GENTLIMITS_H

/******** Our includes *****/
#include "method_struct.h"
#include "object_helper.h"
#include "void_vec.h"

/******** Root includes *****/


#include "TLimit.h"
#include "TLimitDataSource.h"
#include "TConfidenceLevel.h"
#include "TGraph.h"
#include "TH1.h"
#include "TCanvas.h"

std::vector<std::pair<std::string, double>> tlimits(std::string const & ifile_path,
						    method_struct const & method,
						    double const run_pot = 1,
						    double const signal_pot = 1,
						    double const background_pot = 1,
						    double const data_pot = 1);

void tlimits(std::string const & ifile_path,
	     std::vector<method_struct> const & methods,
	     double const run_pot = 1,
	     double const signal_pot = 1,
	     double const background_pot = 1,
	     double const data_pot = 1);

std::pair<double, double>  tlimits_var_method(std::string const & ifile_path,
					      method_struct const & method,
					      double const run_pot,
					      double const points,
					      double const pot_min,
					      double const pot_max,
					      double const background_pot,
					      double const data_pot);
 

void tlimits_var(std::string const & ifile_path,
		 std::vector<method_struct> const & methods,
		 double const run_pot,
		 double const points,
		 double const pot_min,
		 double const pot_max,
		 double const signal_pot,
		 double const background_pot,
		 double const data_pot);

#endif
