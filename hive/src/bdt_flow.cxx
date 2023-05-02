#include "bdt_flow.h"


int bdt_flow::GetNumStage() const{
    return bdt_vector.size() + 2;
}


std::vector<std::string> bdt_flow::GetStageNames() const{
    std::vector<std::string> res = {"Topological Selection","Pre-Selection"};
    for(auto & bdtinfo : bdt_vector){
	//res.push_back("After " + bdtinfo.name);
	res.push_back("");
    }
    res.back() = "Semi-Final Selection";
    return res;
}



std::string bdt_flow::GetStageCuts(int stage) const{

    if(stage < 0)
	return this->file_negative_stage_handle(stage);

    return definition_cuts + " && " + GetGeneralStageCuts(stage);
}

std::string bdt_flow::GetStageCuts(int stage, const std::vector<double>& external_cuts) const{

    if(stage < 0)
	return this->file_negative_stage_handle(stage);

    return definition_cuts + " && " + GetGeneralStageCuts(stage, external_cuts);
}


std::string bdt_flow::GetGeneralStageCuts(int stage) const{

    if(stage < 0){
	std::cerr << "bdt_flow || INVALID stage number : " << stage << std::endl;
	throw std::runtime_error("");
    }else if(stage + 1 > stage_cuts.size()){
	std::cerr << "bdt_flow || WARNING stage number too large..  stage number: " << stage << " total number of stages " << stage_cuts.size() << std::endl;
	std::cerr << "bdt_flow || WARNING check if you set the bdt cut scores .." << std::endl;
	std::cerr << "bdt_flow || Will grab cuts up to stage " << std::min(stage+1, (int)stage_cuts.size()) -1 << std::endl;
    }


    std::string current_cut = "1";
    for(int i = 0; i != std::min(stage+1, (int)stage_cuts.size()); ++i){
	current_cut += " && " + stage_cuts[i];
    }
    return current_cut;
}


std::string bdt_flow::GetGeneralStageCuts(int stage, const std::vector<double>& external_bdt_cuts, bool for_sbnfit) const{

    if(stage < 0){
	std::cerr << "bdt_flow || INVALID stage number : " << stage << std::endl;
	throw std::runtime_error("");
    }else if(stage - 1 > external_bdt_cuts.size()){
	std::cerr << "bdt_flow || WARNING stage number too large..  stage number: " << stage << " total number of stages " << external_bdt_cuts.size() << std::endl;
	std::cerr << "bdt_flow || Will grab cuts up to stage " << std::min(stage, (int)stage_cuts.size() + 1) << std::endl;
    }


    if(stage == 0)
	return stage_cuts[0]; 
    if(stage == 1)
	return stage_cuts[0] + " && " + stage_cuts[1];

    std::string current_cut = stage_cuts[0] + " && " + stage_cuts[1]; 
    for(int i = 0; i != std::min(stage-1, (int)external_bdt_cuts.size()); ++i){
	if(for_sbnfit)
	    current_cut += " && simple_" + bdt_vector[i].identifier+"_mva >="+std::to_string(external_bdt_cuts[i]);
	else
	    current_cut += " && " + bdt_vector[i].identifier+"_mva >="+std::to_string(external_bdt_cuts[i]);
    }
    return current_cut;
}


//---- internal function ------

void bdt_flow::get_precut(){
    pre_cuts = vec_pre_cuts.front();
    for(int i=1; i<vec_pre_cuts.size(); i++){
	 pre_cuts = pre_cuts + "&&"+ vec_pre_cuts.at(i);
    }
    return;
}

void bdt_flow::set_stage_cuts(){
    base_cuts = topological_cuts+"&&"+definition_cuts;

    stage_cuts.clear();
    stage_cuts.push_back(topological_cuts);
    stage_cuts.push_back(pre_cuts);

    for(int i = 0; i != std::min(bdt_vector.size(), bdt_cuts.size()); ++i){
	stage_cuts.push_back(bdt_vector[i].identifier+"_mva >="+std::to_string(bdt_cuts[i]));
    }

    //print help info
    std::cout << "bdt_flow || finish setting up stage cuts.. " << std::endl;
    std::string previous_cut = definition_cuts;
    for(int i = 0; i != stage_cuts.size(); ++i){
	std::cout << "stage " << i << " | cuts " << previous_cut << " && " << stage_cuts[i] << std::endl; 
	previous_cut += " && " + stage_cuts[i];
    }
    return;
}

std::string bdt_flow::file_negative_stage_handle(int stage) const{

    if(stage < -1){
	std::cerr << "bdt_flow || WARNING: requested for invalid stage " << stage << ", return cut of \"1\". Make sure you want this" << std::endl;
	return "1";
    }
    else if(stage == -1){
	std::cout << "bdt_flow || requested for stage -1, returning definition cut ... " << std::endl;
	return definition_cuts;
    }

    return "1";
}
