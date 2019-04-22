#ifndef METHODSTRUCT_H
#define METHODSTRUCT_H

#include "TMVA/Types.h"
#include "bdt_var.h"


struct method_struct {

	TMVA::Types::EMVA  type;
	std::string  str;
	std::string option;

    std::string bdt_name;
    std::string bdt_binning;
    std::string bdt_tag;

    std::vector<bdt_variable> bdt_all_vars;
    std::vector<bdt_variable> bdt_train_vars;
    std::vector<bdt_variable> bdt_spec_vars;

    std::string filename;
    std::string foldername;
    std::string training_cut;
    double training_fraction;

    std::string topological_name;
    std::string topological_definition;

    std::vector<std::string> precuts;

    method_struct(){
    }

	method_struct(TMVA::Types::EMVA const a, std::string const & b, std::string const & c) :
		type(a),
		str(b),
		option(c) {}

};

#endif
