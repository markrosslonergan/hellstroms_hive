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
		int GetNumStage() const{
		    return bdt_vector.size() + 2;
 		}

		/* Get related cuts for given stage, file specific 
 		 * stage 0: topological cut + definition cut 
 		 * stage 1: stage 0 cut + precuts
 		 * stage 2/3/4/..:  stage 1 cut + bdtcuts
 		 */
		std::string GetStageCuts(int stage) const{

		    std::cout << "bdt_flow: Get file-specific stage cut.. " << std::endl;

		    if(stage == -1){
			std::cout << "bdt_flow: requested for stage -1, returning definition cut ... " << std::endl;
			return definition_cuts;
		    }
		    return definition_cuts + " && " + GetGeneralStageCuts(stage);
		}

		/* Get related cuts for given stage, not file specific 
 		 * stage 0: topological cut 
 		 * stage 1: stage 0 cut + precuts
 		 * stage 2/3/4/..:  stage 1 cut + bdtcuts
 		 */
		std::string GetGeneralStageCuts(int stage) const{
		    std::cout << "bdt_flow: Get general stage cut.. " << std::endl;

		    if(stage < 0){
			std::cerr << "bdt_flow: INVALID stage number : " << stage << std::endl;
			throw std::runtime_error("");
		    }else if(stage + 1 > stage_cuts.size()){
			std::cerr << "bdt_flow: WARNING stage number too large..  stage number: " << stage << " total number of stages " << stage_cuts.size() << std::endl;
			std::cerr << "bdt_flow: WARNING check if you set the bdt cut scores .." << std::endl;
			std::cerr << "bdt_flow: Will grab cuts up to stage " << std::min(stage+1, (int)stage_cuts.size()) -1 << std::endl;
		    }


		    std::string current_cut = topological_cuts;
                    for(int i = 1; i != std::min(stage+1, (int)stage_cuts.size()); ++i){
                        current_cut += " && " + stage_cuts[i];
                    }
		    return current_cut;
		}


		//---- internal function ------

                void get_precut(){
		    pre_cuts = vec_pre_cuts.front();
                    for(int i=1; i<vec_pre_cuts.size(); i++){
                         pre_cuts = pre_cuts + "&&"+ vec_pre_cuts.at(i);
                    }
		    return;
		}

		void set_stage_cuts(){
		    base_cuts = topological_cuts+"&&"+definition_cuts;
		    stage_cuts.push_back(base_cuts);
		    stage_cuts.push_back(pre_cuts);

		    for(int i = 0; i != std::min(bdt_vector.size(), bdt_cuts.size()); ++i){
		        stage_cuts.push_back(bdt_vector[i].identifier+"_mva >="+std::to_string(bdt_cuts[i]));
		    }

		    //print help info
		    std::cout << "bdt_flow: finish setting up stage cuts.. " << std::endl;
		    std::string previous_cut = "1";
		    for(int i = 0; i != stage_cuts.size(); ++i){
			std::cout << "stage " << i << " | cuts " << previous_cut << " && " << stage_cuts[i] << std::endl; 
			previous_cut += " && " + stage_cuts[i];
		    }
		    return;
		}
};

#endif
