#ifndef LOADMVAPARAM_H
#define LOADMVAPARAM_H

#include "tinyxml.h"

#include "method_struct.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "TColor.h"
#include "TMVA/Types.h"


class MVALoader {

	protected:

	std::vector<method_struct> vec_methods;
	public:
	
	bool isVerbose;
	std::string whichxml;	

	
	MVALoader(std::string,bool);
	MVALoader(std::string);

	std::vector<method_struct> GetMethods();
    size_t GetNFiles(){return n_bdt_files;}
    
    //BDT file info
    //bdt_file *signal_SM = new bdt_file(dir, "ncdeltarad_overlay_run1_v17.1.root", "NCDeltaRadOverlaySM", "hist","singlephoton/",  kOrange+6, signal_all);

    std::string analysis_tag;

    std::string filedir;
    size_t n_bdt_files;
    std::vector<std::string> bdt_filenames;
    std::vector<std::string> bdt_tags;
    std::vector<std::string> bdt_hist_styles;
    std::vector<std::string> bdt_dirs;
    std::vector<std::string> bdt_plotnames;
    std::vector<std::string> bdt_weight;
    std::vector<bool> bdt_is_validate_file;

    std::vector<TColor*> bdt_cols;
    std::vector<int> bdt_linecols;
    std::vector<int> bdt_linestyles;
    std::vector<int> bdt_group;//Keng

    std::vector<int> bdt_fillstyles;
    std::vector<double> bdt_scales;
    std::vector<std::vector<std::string>> bdt_definitions;
    std::vector<std::vector<std::string>> bdt_training_cuts;
    std::vector<bool> bdt_is_onbeam_data;
    std::vector<double> bdt_fixpot;
    std::vector<double> bdt_onbeam_pot;
    std::vector<double> bdt_onbeam_spills;
    std::vector<bool> bdt_is_offbeam_data;
    std::vector<double> bdt_offbeam_spills;

    std::vector<bool> bdt_is_signal;
    std::vector<bool> bdt_on_top;
    std::vector<bool> bdt_is_training_signal;

    std::map<std::string,std::string> aliasMap; 
    std::string AliasParse(std::string in);


    std::vector<int> v_eff_denom_stage;
    std::vector<int> v_eff_numer_stage;
    std::vector<std::string> v_eff_denom_cut;
    std::vector<std::string> v_eff_numer_cut;
    


    std::vector<double> bdt_cuts;

    std::vector<std::string> recomc_names;
    std::vector<std::string> recomc_defs;
    std::vector<TColor*> recomc_cols;

};

#endif
