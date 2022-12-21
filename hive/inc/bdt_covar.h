#ifndef BDT_COVAR_H
#define BDT_COVAR_H

/*** stl ***/
#include <iostream>
#include <vector>
#include <string> 
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <sys/stat.h> // for use of mkdir() function
#include <cstring>    // for use of strerror

/*** root include ***/
#include "TFile.h"
#include "TMatrixT.h"

/*** local header ***/
#include "global_func.h"
#include "bdt_var.h"
#include "SBNcovariance.h"

class bdt_covar{
private: 
    
    // Note: hardcoded path using "HIVEDIR" enviromental variable 
    // See set_template_path() function
    bool set_template = false;
    std::string fluxxs_template_xml; 
    std::string det_template_xml;


    std::string file_dir="SBNcovar/";
    std::vector<std::string> detsys = {"WiremodX","WiremodYZ","WiremodAngleYZ","WiremodAngleXZ","lightYieldAttn","lightYieldDown","lightYieldRay","SCE","Recomb2"};
    //std::vector<std::string> detsys = {"WiremodAngleXZ"};

    int stage;
    std::string base_cut;
    bdt_variable* pvar; 


    //----- internal helper function  -------

    void merge_covar(const std::vector<std::string>& files_to_merge, const std::string& target_file);


    /* create covariance matrix using given xml configuration
     * Note: generated covariance matrix will be under separate file_dir(`SBNcovar`) subdir, with name tag.SBNcovar.root
     */
    void generate_covar(const std::string& xml, const std::string& tag, bool run_det=false);

    /* find the fullpath for fluxxs and detector template xmls */
    void set_template_path();

    /* copy template xml into a new xml, and replace any words provided in `search` by words in `replace`
     * The new xml will be located into designated `dir` directory 
     */
    void copy_and_replace(const std::string& input_xml, const std::string& output_xml, const std::vector<std::string>& search, const std::vector<std::string>& replace, const std::string& dir);

public:


    bdt_covar(bdt_variable* in_var, int s): pvar(in_var), stage(s), base_cut("1"){ 
    }   

    bdt_covar(bdt_variable* in_var, int s, const std::string& cut): pvar(in_var), stage(s), base_cut(cut){   
    }

    /* Function: returns the default template xml for reweightable systematics */
    std::vector<std::string> GetTemplateXmls();

    /* given template xml for reweighable/detector systematic, genereate covariance matrix
     * Note: in the case where no xml is provided, default template xml will be used, which is located under /hive/xml/SBNfit_Integration_XMLS_Coherent/..
     * Note: vararible definition and binning, and name of the covariance file is determined by the bdt_variable 'pvar'
     * Note: any file-specific thing such as defintion, file-specific weight etc. should be set in the template xml
     */
    void GenerateReweightingCovar();
    void GenerateReweightingCovar(const std::string& xml_template);
    void GenerateDetectorCovar();
    void GenerateDetectorCovar(const std::string& xml_template);

    /* Merge the fluxXS and detector covariance matrix */
    void MergeCovar();

    std::string LocalDir() const {return file_dir; }
   
    /* copy template xml into local xml and set up the binning, definition and eventweight in the xml
     * Note: local xml will be copied into separate `xml` subdir
     */
    std::string PrepareXml(const std::string& xml_template, const std::string& tag);
};
#endif
