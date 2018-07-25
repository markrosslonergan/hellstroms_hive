#ifndef BDT_VARLIST_H
#define BDT_VARLIST_H

/******** Our includes *****/
#include "method_struct.h"
#include "void_vec.h"

#include "bdt_var.h"
#include "bdt_flow.h"
#include "bdt_file.h"
#include "bdt_info.h"
#include "bdt_flow.h"
/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"

#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"


struct variable_list{
public:
std::vector<bdt_variable>  all_vars;
std::vector<bdt_variable>  pi0_vars;
std::vector<std::string> all_precuts;
std::string analysis_tag;
variable_list(std_string analysis_tag_in);

};




#endif
