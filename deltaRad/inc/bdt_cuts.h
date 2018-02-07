#ifndef BDT_CUTS_H
#define BDT_CUTS_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

/******** Root includes *****/

struct bdt_cuts{

	public:
	
	//some convientant labels
	std::string identifier;
	std::string name;

	std::string base_cuts ;
 	std::string pre_cuts;
	std::string bdt_cosmic_cuts;
	std::string mid_cuts;
	std::string bdt_bnb_cuts;
	std::string post_cuts;

	std::string signal_definition;
	std::string background_definition;
	
	bdt_cuts(){identifier = "null"; name = "null";};
	bdt_cuts(std::string in_identifier, std::string in_name) : identifier(in_identifier), name(in_name), base_cuts("1"), pre_cuts("1"), mid_cuts("1"),post_cuts("1"), bdt_cosmic_cuts("1"), bdt_bnb_cuts("1"),signal_definition("1"),background_definition("1"){};

	int setName(std::string in){ identifier = in;return 0;};
	int setBaseCuts(std::string in){ base_cuts = in; return 0;}; 
	int setPreCuts(std::string in){ pre_cuts = in; return 0;}; 
	int setMidCuts(std::string in){ mid_cuts = in; return 0;}; 
	int setPostCuts(std::string in){ post_cuts = in; return 0;}; 
	int setBdtCosmicCuts(std::string in){ bdt_cosmic_cuts = in; return 0;}; 
	int setBdtBnbCuts(std::string in){ bdt_bnb_cuts = in; return 0;}; 
	
	int setSignalDefinition(std::string in){ signal_definition = in; return 0;}; 
	int setBackgroundDefinition(std::string in){ background_definition = in; return 0;}; 

};





#endif
