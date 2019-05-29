#ifndef BDT_INFO_H
#define BDT_INFO_H

#include <vector>
#include <string>
#include <iostream>
#include "bdt_var.h"
#include "method_struct.h"
/******** Our includes *****/

/******** Root includes *****/

struct bdt_info{

	public:
	
	//some convientant labels
	std::string identifier;
	std::string name;	
	std::string binning;
	std::string base_cuts ;
 	std::string pre_cuts;
	std::string bdt_cosmic_cuts;
	std::string mid_cuts;
	std::string bdt_bnb_cuts;
	std::string post_cuts;

	std::string signal_definition;
	std::string background_definition;

    method_struct TMVAmethod;
    std::vector<bdt_variable> train_vars;
    std::vector<bdt_variable> spec_vars;

	std::string topo_name;
	
	bdt_info(){identifier = "null"; name = "null"; binning = "null";};
	
    bdt_info(std::string in_identifier, std::string in_name, std::string in_bin) : identifier(in_identifier), name(in_name), binning(in_bin), base_cuts("1"), pre_cuts("1"), mid_cuts("1"),post_cuts("1"), bdt_cosmic_cuts("1"), bdt_bnb_cuts("1"),signal_definition("1"),background_definition("1"){
	topo_name = "2#gamma1p";	
	};


    bdt_info(std::string analysis_tag, method_struct inmethod):
	TMVAmethod(inmethod), identifier(analysis_tag+inmethod.bdt_tag), name(inmethod.bdt_name),binning(inmethod.bdt_binning),base_cuts("1"), pre_cuts("1"), mid_cuts("1"),post_cuts("1"), bdt_cosmic_cuts("1"), bdt_bnb_cuts("1"),signal_definition("1"),background_definition("1") { 
	topo_name = "2#gamma1p";	
    train_vars = TMVAmethod.bdt_train_vars;
    spec_vars = TMVAmethod.bdt_spec_vars;
    }

	int setName(std::string in){ identifier = in;return 0;};
	int setBaseCuts(std::string in){ base_cuts = in; return 0;}; 
	int setPreCuts(std::string in){ pre_cuts = in; return 0;}; 
	int setMidCuts(std::string in){ mid_cuts = in; return 0;}; 
	int setPostCuts(std::string in){ post_cuts = in; return 0;}; 
	int setBdtCosmicCuts(std::string in){ bdt_cosmic_cuts = in; return 0;}; 
	int setBdtBnbCuts(std::string in){ bdt_bnb_cuts = in; return 0;}; 

	int setTopoName(std::string in){ topo_name = in; return 0;};
	
	int setSignalDefinition(std::string in){ signal_definition = in; return 0;}; 
	int setBackgroundDefinition(std::string in){ background_definition = in; return 0;}; 

};

#endif
