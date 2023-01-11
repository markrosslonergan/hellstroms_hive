#ifndef BDT_VAR_H
#define BDT_VAR_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

/******** Our includes *****/
#include "global_func.h"

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"


struct bdt_variable{

	public:
		std::string name;
        int id;
		int cat;
        std::string safe_name;
		std::string binning;
		std::string unit;
		std::string safe_unit;
		bool is_track;
		std::string type;
        bool is_logplot;
        bool has_covar;
        bool is_spectator;

        std::string additional_cut;
        std::string unique_hash;
 
        std::string covar_name;
        std::string covar_dir;
        std::string covar_file_prefix;
        std::string covar_file;
        std::string covar_sys;
        std::string covar_legend_name;
        std::string covar_type;

        double plot_min;
        double plot_max;

        int n_bins;
        std::vector<double> edges;
        std::vector<double> low_edges;

        int addCovar(std::string name, std::string dir, std::string file){
            has_covar=true;
            covar_name = name;
	    covar_dir = dir;
            covar_file = file;
            return 0;
        }

		bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack, std::string intype) : bdt_variable(inname,inbin,inunit,intrack,intype,-1){}; 
		
        bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack, std::string intype,int in_id) : 
			name(inname), 
			binning(inbin),
			unit(inunit),
			is_track(intrack),
			type(intype),
            id(in_id),
            is_logplot(false)
		{
            plot_min =-999;
            plot_max =-999;
			safe_name = name;
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '('), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ')'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '\\'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '/'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '['), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ']'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '+'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '-'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '*'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '.'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ' '), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ','), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '|'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), ':'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '#'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '{'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '}'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '^'), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '='), safe_name.end());
			safe_name.erase(std::remove(safe_name.begin(), safe_name.end(), '$'), safe_name.end());
	
           	safe_unit = unit;
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), ' '), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '('), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), ')'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '\\'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '/'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '['), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), ']'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '+'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '-'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '*'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '|'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), ':'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '#'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '{'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '}'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '^'), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '='), safe_unit.end());
			safe_unit.erase(std::remove(safe_unit.begin(), safe_unit.end(), '$'), safe_unit.end());

            has_covar = false;

	    decode_bins();
            /*std::cout<<"Nbin "<<n_bins<<std::endl;
            std::cout<<"edges "<<std::endl;
            for(auto &v: edges) std::cout<<v<<std::endl;

            std::cout<<"Low edges "<<std::endl;
            for(auto &v: low_edges) std::cout<<v<<std::endl;
            */
           //low_edges = {0,0.15,0.225,0.3,0.375,0.45,0.6}; 
           //low_edges = {0.1,  0.2, 0.25, 0.3, 0.35, 0.4 ,0.45, 0.5, 0.55, 0.6, 0.7};
           // low_edges = {0, 0.075, 0.15, 0.225, 0.3, 0.375, 0.45, 0.525, 0.6, 0.675,  0.9};           
		}


		bdt_variable(){}

		bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack) : 
			name(inname), 
			binning(inbin),
			is_track(intrack),
			unit(inunit)
	{ 
                plot_min =-999;
            plot_max =-999;
            cat = 0;
	     decode_bins();
	}

	//---- function that I'd like to use only internally ----

	void decode_bins(){
            edges.clear();
	    low_edges.clear();
            n_bins = -1;

	    std::string temp_binning = binning;            
            bool alt_mode = temp_binning.find("alt")!=std::string::npos;
            if(alt_mode){
                std::cout<<"applying custom binning with config: "<<std::endl;
                int pl = temp_binning.find("alt");
                temp_binning.erase(pl,pl+3);
            }


            temp_binning.erase(std::remove(temp_binning.begin(), temp_binning.end(), '('), temp_binning.end());
            temp_binning.erase(std::remove(temp_binning.begin(), temp_binning.end(), ')'), temp_binning.end());

            size_t pos = 0;
            std::string delim = ",";
            std::string token;

            while ((pos = temp_binning.find(delim)) != std::string::npos) {
                token = temp_binning.substr(0, pos);
                if(n_bins<0 &&!alt_mode) n_bins = (int)std::stod(token);
                edges.push_back(std::stod(token));
                temp_binning.erase(0, pos + delim.length());
            }
            edges.push_back(std::stod(temp_binning));

            if(alt_mode){
                n_bins = edges.size()-1;
                low_edges = edges;
                edges = {(double)n_bins, low_edges.front(), low_edges.back()};
            }else{
                double elow = edges[1];
                double ehigh = edges[2];
                double ediff = ehigh-elow;
                double estep = ediff/(double)n_bins;
                for(int i=0; i<=n_bins;i++){
                    low_edges.push_back(elow+i*estep);
                }
            }

	    return;
	}

	//---- End of private internal function ----
	

	void GetUniqueJenkinsHash(){
	    std::string long_id =  name + "_" + GetBinEdges() + "_" + additional_cut;  
	    std::replace(long_id.begin(), long_id.end(), ' ', '_');
        //lets ignore simples 
        std::string rmo = "_mva";
        std::string sim = "simple_";
        size_t index = long_id.find(rmo);
        if(index!=std::string::npos && long_id.find(sim)==std::string::npos){
           long_id.insert(0,sim); 
        }
        
	    unique_hash = std::to_string(jenkins_hash(long_id));
	    return;
  	}

	std::string GetID(){
	   if(unique_hash.empty())
	      GetUniqueJenkinsHash();
	   return unique_hash;
	}

	std::string GetBareFileID(int stage){
	// get fileid for the variable, with syst string
	
	   std::string temp_fileid = this->GetCovarFileID(stage);
		
	   auto pos = temp_fileid.rfind(covar_sys);
	   temp_fileid = temp_fileid.substr(0, pos - 1);
	   
	   return temp_fileid;
	}

	std::string GetCovarFileID_FluxXS(int stage){
	   std::string bare_fileid = GetBareFileID(stage);
	   return bare_fileid + "_" + FLUXXS;
	}
	std::string GetCovarFileID_Det(int stage){
	   std::string bare_fileid = GetBareFileID(stage);
	   return bare_fileid + "_" + DET;
	}

	std::string GetCovarFileID(int stage){
	     if(covar_file_prefix.empty()){
		covar_file_prefix = "VarCovar_Stage_"+ std::to_string(stage) + "_" + this->GetID(); 
        
        	covar_file_prefix += "_"+covar_sys; 
    	     }

	    return covar_file_prefix;
	}

	void UpdateCovarFileName(const std::string& fname, std::string fdirs=""){
	    covar_dir = fdirs;
	    covar_file = fname;
	    return;
	}
	void UpdateCovarName(const std::string& mname){
	    covar_name= mname;
	    return;
	}

	void UpdateCovarType(const std::string& in_type){
	    covar_type = in_type;
	    return;
	}

	std::string GetCovarFile(int s){
	    if(covar_file.empty()){
	        covar_file = GetCovarFileID(s) + ".SBNcovar.root";
	    }
	    return covar_dir +  covar_file;
 	}

        std::string GetBinEdges() const {
	    std::string Binning;
            for(auto e : low_edges){
                Binning += std::to_string(e) + " ";
            }
	    Binning.pop_back();
	    return Binning;
	}

   	std::string GetVarDef() const {
	    return name;
        }

	std::string GetVarSimpleDef() const{
	    if(name.find("_mva") != std::string::npos && name.find("simple_") == std::string::npos)
		return "simple_"+name;

	    return name;
        }

	std::string GetAdditionalCut() const{
            return additional_cut;
  	}

	int GetNBins() const {return n_bins;}

	bool flux_xs_sys_only() const{
	    return covar_sys == FLUXXS;
	}

	bool detector_sys_only() const{
	    return covar_sys == DET;
	}

	bool full_sys() const{
	    return covar_sys == FULL;
	}

};





#endif
