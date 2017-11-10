#ifndef MERGE_H
#define MERGE_H




/******** Our includes *****/
#include "method_struct.h"
#include "void_vec.h"
#include "object_helper.h"

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TTree.h"



void set_up_branch(std::string const & branch_name,
		   std::string const & type,
		   void_vec & branches1,
		   void_vec & branches2,
		   void_vec & branches_merged,
		   TTree * tree1,
		   TTree * tree2,
		   TTree * tree_merged);


TTree * merge_trees(TTree * tree1,
		    TTree * tree2,
		    std::string const & tree_merged_name,
		    std::vector<std::pair<std::string, std::string>> const & branches,
		    TTree * cut_tree,
		    std::string const & all_cut,
		    std::string const & cut1, 
		    std::string const & cut2);

void merge(std::string const & ofile_path,
	   std::string const & file_path1,
	   std::string const & file_path2,
	   std::vector<std::pair<TTree *, std::string>> const & trees,
	   std::vector<method_struct> const & methods,
	   std::vector<std::pair<std::string, std::string>> const & branches,
	   std::string const & all_cut,
	   std::string const & cut1,
	   std::string const & cut2);
#endif
