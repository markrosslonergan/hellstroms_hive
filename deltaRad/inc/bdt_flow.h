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

		std::vector<std::string> vec_pre_cuts;
        std::vector<bdt_info> bdt_vector;

		bdt_flow(){};

		bdt_flow(std::string intop, std::string indef, std::string inpre, std::string inpost, bdt_info incos, bdt_info inbnb) : topological_cuts(intop),  definition_cuts(indef), pre_cuts(inpre), post_cuts(inpost),bdt_cosmic_cuts(incos), bdt_bnb_cuts(inbnb) {

			base_cuts = topological_cuts+"&&"+definition_cuts;

			std::string s = pre_cuts;
			std::string delimiter = "&&";

			size_t pos = 0;
			std::string token;
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				s.erase(0, pos + delimiter.length());
				//std::cout<<token<<std::endl;
				vec_pre_cuts.push_back(token);
			}
			//grab last one
			vec_pre_cuts.push_back(s);

		};	


		bdt_flow(std::string intop, std::string indef, std::vector<std::string> invecpre, std::string inpost, bdt_info incos, bdt_info inbnb) : topological_cuts(intop),  definition_cuts(indef), vec_pre_cuts(invecpre), post_cuts(inpost),bdt_cosmic_cuts(incos), bdt_bnb_cuts(inbnb) {

			base_cuts = topological_cuts+"&&"+definition_cuts;

			pre_cuts = vec_pre_cuts.front();
			for(int i=1; i<vec_pre_cuts.size(); i++){
				pre_cuts = pre_cuts + "&&"+ vec_pre_cuts.at(i);
			}

		};	


		bdt_flow(std::string intop, std::string indef, std::vector<std::string> invecpre, std::string inpost, std::vector<bdt_info>inbdtinfos) : topological_cuts(intop),  definition_cuts(indef), vec_pre_cuts(invecpre), post_cuts(inpost), bdt_vector(inbdtinfos) {

			base_cuts = topological_cuts+"&&"+definition_cuts;

            //fake for now
            bdt_cosmic_cuts = bdt_vector[0];
            bdt_bnb_cuts = bdt_vector[1];

			pre_cuts = vec_pre_cuts.front();
			for(int i=1; i<vec_pre_cuts.size(); i++){
				pre_cuts = pre_cuts + "&&"+ vec_pre_cuts.at(i);
			}

		};	

};

#endif
