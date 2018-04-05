#ifndef BDT_FLOW_H
#define BDT_FLOW_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/
#include "bdt_info.h"
/******** Root includes *****/

struct bdt_flow{

	public:

		//some convientant labels
		std::string topological_cuts;
		std::string definition_cuts;

		//base cuts is by definition topo+defintion
		std::string base_cuts ;
		std::string pre_cuts;
		bdt_info bdt_cosmic_cuts;
		bdt_info bdt_bnb_cuts;

		std::string mid_cuts;
		std::string post_cuts;

		bdt_flow(){};	
		bdt_flow(std::string inbase, std::string inpre, std::string inpost, bdt_info incos, bdt_info inbnb) : base_cuts(inbase), pre_cuts(inpre), post_cuts(inpost),bdt_cosmic_cuts(incos), bdt_bnb_cuts(inbnb) {};	
		bdt_flow(std::string intopo, std::string indefin, std::string inpre, std::string inpost, bdt_info incos, bdt_info inbnb) : topological_cuts(intopo), definition_cuts(indefin), post_cuts(inpost),bdt_cosmic_cuts(incos), bdt_bnb_cuts(inbnb) {
		base_cuts  = topological_cuts +"&&"+ definition_cuts;
		};	


};





#endif
