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

 	static std::string math_op_character;
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


 	// constructors 
	bdt_variable(){}
	bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack);
	bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack, std::string intype); 
        bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack, std::string intype,int in_id);


	//---------- Member Functions ------------
	
	// Func: return bin edges of the variable
        std::string GetBinEdges() const;
	int GetNBins() const {return n_bins;}

	// Func: return the unit of variable (usually used for x axis title)	
	std::string GetVarUnit() const {return unit;}

	// Func: return varaible definition 
   	std::string GetVarDef() const; 
	std::string GetVarSimpleDef() const; //Note: add "simple_" in front of BDT score variables

	// Func: return additional cut associated with this variable
	std::string GetAdditionalCut() const;

	// Func: same as GetAdditionalCut() function, return additional cut associated with this variable
	// Note: add "simple_" in front of BDT score variables
	std::string GetSimpleAdditionalCut() const;


	// Func: return the unique hash of the variable, see function get_unique_jenkins_hash() for how hash is defined.
	std::string GetID();
	
	// Func: return fileid tag for the variable, with associated systematic string 
	std::string GetCovarFileID(int stage);

	// Func: return fileid tag for the variable, without associated systematic string 
	std::string GetBareFileID(int stage);
	std::string GetCovarFileID_FluxXS(int stage);
	std::string GetCovarFileID_Det(int stage);

        // Add covariance matrix associated with this variable
        int addCovar(std::string name, std::string dir, std::string file);

	// Func: update associated covariance matrix file, covar matrix, and matrix type("frac" or "full") for the variable
	void UpdateCovarFileName(const std::string& fname, const std::string& fdirs="");
	void UpdateCovarName(const std::string& mname);
	void UpdateCovarType(const std::string& in_type);

	// Func: return full path of the associated covariance matrix file
	std::string GetCovarFile(int s);

	bool flux_xs_sys_only() const{
	    return covar_sys == FLUXXS;
	}

	bool detector_sys_only() const{
	    return covar_sys == DET;
	}

	bool full_sys() const{
	    return covar_sys == FULL;
	}



	//---- Internal Functions  ----

	// Func: Decode binning information, and generate vector of bin edges 
	void decode_bins();
	
	// Func: Calculate and set unique jenkins hash for the variables 
	// Note: information used: variable name, variable bin edges, and additional cut associated with the variable
	void get_unique_jenkins_hash();


	// Func: given a variable definition or math formula, for every appearance of BDT score variable that ends with "_mva", add "simple_" to the front.
	// Note: this is added to make sure that sbnfit files can be read correctly since BDT score variables are saved with "simple_" prefix in sbnfit files.
	// Guanqun: something I hacked together don't like this
	std::string add_simple_to_formula(const std::string& input_formula) const;
	//---- End of private internal function ----
};


#endif
