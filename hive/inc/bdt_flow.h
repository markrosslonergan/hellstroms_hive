#ifndef BDT_FLOW_H
#define BDT_FLOW_H

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>
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

		std::string mid_cuts;
		std::string post_cuts;

		std::vector<std::string> vec_pre_cuts;
                std::vector<bdt_info> bdt_vector;
		std::vector<double> bdt_cuts; 
		std::vector<std::string> stage_cuts;

		bdt_flow(){};

		bdt_flow(std::string intop, std::string indef, std::string inpre, std::string inpost, bdt_info incos, bdt_info inbnb) : topological_cuts(intop),  definition_cuts(indef), pre_cuts(inpre), post_cuts(inpost) {
			bdt_vector.push_back(incos);
			bdt_vector.push_back(inbnb);

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
			set_stage_cuts();

		};	


		bdt_flow(std::string intop, std::string indef, std::vector<std::string> invecpre, std::string inpost, bdt_info incos, bdt_info inbnb) : topological_cuts(intop),  definition_cuts(indef), vec_pre_cuts(invecpre), post_cuts(inpost){

			bdt_vector.push_back(incos);
			bdt_vector.push_back(inbnb);
			get_precut();
			set_stage_cuts();

		};	


		bdt_flow(std::string intop, std::string indef, std::vector<std::string> invecpre, std::string inpost, std::vector<bdt_info> inbdtinfos) : topological_cuts(intop),  definition_cuts(indef), vec_pre_cuts(invecpre), post_cuts(inpost), bdt_vector(inbdtinfos) {

			get_precut();
			set_stage_cuts();

		};	

		bdt_flow(std::string intop, std::string indef, std::vector<std::string> invecpre, std::string inpost, std::vector<bdt_info> inbdtinfos, const std::vector<double>& inbdtcuts) : topological_cuts(intop),  definition_cuts(indef), vec_pre_cuts(invecpre), post_cuts(inpost), bdt_vector(inbdtinfos) , bdt_cuts(inbdtcuts){

			get_precut();
			set_stage_cuts();
		};	


		/* Get number of stages in this analysis flow */
		int GetNumStage() const;
	
		/* Get the plot name for stages */
		std::vector<std::string> GetStageNames() const;


		/* Get related cuts for given stage, file specific 
 		 * stage < -1: return "1"
 		 * stage -1: defintion cut
 		 * stage 0: topological cut + definition cut 
 		 * stage 1: stage 0 cut + precuts
 		 * stage 2/3/4/..:  stage 1 cut + bdtcuts
 		 */
		std::string GetStageCuts(int stage) const;
		std::string GetStageCuts(int stage, const std::vector<double>& external_cuts) const;

		/* Get related cuts for given stage, not file specific 
 		 * stage 0: topological cut 
 		 * stage 1: stage 0 cut + precuts
 		 * stage 2/3/4/..:  stage 1 cut + bdtcuts
 		 */
		std::string GetGeneralStageCuts(int stage) const;
		std::string GetGeneralStageCuts(int stage, const std::vector<double>& external_bdt_cuts, bool for_sbnfit=false) const;

		//---- internal function ------

	 	/* Form the whole precut string */
                void get_precut();

		/* Fill the stage_cuts vector with cuts of each individual stage */ 
		void set_stage_cuts();


		/* Return cuts for negative stages .. 
 		 * Stage = -1: returns definition cut, which is bdt_file specific 
 		 * Stage < -1: returns "1" 
 		 */ 
		std::string file_negative_stage_handle(int stage) const;
};

#endif
