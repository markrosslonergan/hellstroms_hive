#ifndef SIGNIFICANCE_H
#define SIGNIFICANCE_H


#include <float.h>
#include <iostream>



/******** Our includes *****/
#include "method_struct.h"
#include "object_helper.h"
#include "get_vertices_per_event.h"

/******** Root includes *****/


#include "TTree.h"



std::pair<double, double> method_best_significance(std::string const & app_file_path, 
						   double const run_pot,
						   std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
						   std::vector<std::string> const & signal_tree_cuts, 
						   std::vector<std::pair<int, double>> const & signal_pots, 
						   std::vector<std::pair<TTree *, std::string>> const & background_trees,
						   std::vector<std::string> const & background_tree_cuts, 
						   std::vector<std::pair<int, double>> const & background_pots,
						   std::string const & method);


void cout_efficiency(TTree * tree, std::string const & name, std::string const & tree_cut, double const best_cut, 
		     std::string const & method, double const scale = 1);

void cout_best_cut(std::pair<double, double> const & best_cut,
		   double const run_pot,
		   std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
		   std::vector<std::string> const & signal_tree_cuts,
		   std::vector<std::pair<int, double>> const & signal_pots, 
		   std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		   std::vector<std::string> const & background_tree_cuts,
		   std::vector<std::pair<int, double>> const & background_pots,
		   std::string const & method); 

void significance(std::string const & app_file_path, 
		  double const run_pot,
		  std::vector<std::pair<TTree *, std::string>> const & signal_trees,
		  std::vector<std::string> const & signal_tree_cuts, 
		  std::vector<std::pair<int, double>> const & signal_pots, 
		  std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		  std::vector<std::string> const & background_tree_cuts,
		  std::vector<std::pair<int, double>> const & background_pots,
		  std::vector<method_struct> const methods); 

std::vector<double> method_best_significance_seperate(std::string const & app_file_path, 
						      double const run_pot,
						      std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
						      std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts, 
						      std::vector<std::pair<int, double>> const & signal_pots, 
						      std::vector<std::pair<TTree *, std::string>> const & background_trees,
						      std::vector<std::pair<std::string, std::string>> const & background_tree_cuts, 
						      std::vector<std::pair<int, double>> const & background_pots,
						      std::string const & method);

void cout_efficiency_seperate(TTree * tree, std::string const & name, TTree * tree_friend,
			      std::string const & tree_cut1, std::string const & tree_cut2, double const best_cut1, double const best_cut2, 
			      std::string const & method, double const scale = 1);

void cout_best_cut_seperate(std::vector<double> const & best_cut,
			    double const run_pot,
			    std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
			    std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts,
			    std::vector<std::pair<int, double>> const & signal_pots,
			    std::vector<TTree*> const & signal_tree_friends, 
			    std::vector<std::pair<TTree *, std::string>> const & background_trees, 
			    std::vector<std::pair<std::string, std::string>> const & background_tree_cuts,
			    std::vector<std::pair<int, double>> const & background_pots,
			    std::vector<TTree*> const & background_tree_friends,
			    std::string const & method);


void significance_seperate(std::string const & app_file_path,
			   double const run_pot,
			   std::vector<std::pair<TTree *, std::string>> const & signal_trees,
			   std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts, 
			   std::vector<std::pair<int, double>> const & signal_pots, 
			   std::vector<std::pair<TTree *, std::string>> const & background_trees, 
			   std::vector<std::pair<std::string, std::string>> const & background_tree_cuts,
			   std::vector<std::pair<int, double>> const & background_pots,
			   std::vector<method_struct> const methods);


#endif
