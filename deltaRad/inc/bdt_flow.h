#ifndef BDT_FLOW_H
#define BDT_FLOW_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

/******** Root includes *****/

struct bdt_flow{

	public:

		//some convientant labels
		std::string base_cuts ;
		std::string pre_cuts;
		std::string bdt_cosmic_cuts;
		std::string bdt_bnb_cuts;

		std::string mid_cuts;
		std::string post_cuts;

		bdt_flow(){};	
		bdt_flow(std::string inbase, std::string inpre, std::string inpost, std::string incos, std::string inbnb) : base_cuts(inbase), pre_cuts(inpre), post_cuts(inpost),bdt_cosmic_cuts(incos), bdt_bnb_cuts(inbnb) {};	


};





#endif
