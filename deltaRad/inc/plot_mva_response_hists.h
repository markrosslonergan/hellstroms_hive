#ifndef PLOTMVARESPONSE_H
#define PLOTMVARESPONSE_H




/******** Our includes *****/
#include "method_struct.h"
#include "void_vec.h"
#include "object_helper.h"

/******** Root includes *****/

#include "TCanvas.h"
#include "TLegend.h"
#include "TTree.h"
#include "TH1.h"



void draw_mva_response_hists(std::string const & ifile_path,
			     std::vector<std::pair<TTree *, std::string>> const & trees,
			     std::string const & method, 
			     std::string const & branch);



void plot_mva_response_hists(std::string const & ofile_path,
			     std::string const & ifile_path,
			     std::vector<std::pair<TTree *, std::string>> const & trees,
			     std::vector<method_struct> const & methods,
			     std::vector<std::pair<std::string, std::string>> const & branches);


#endif
