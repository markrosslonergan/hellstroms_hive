#ifndef BDT_FILE_H
#define BDT_FILE_H

#include <vector>
#include <string>
#include <set>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <numeric>
/******** Our includes *****/

#include  "bdt_flow.h"
#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"
#include  "method_struct.h"
/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TLatex.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"
#include "TText.h"
#include "TRandom3.h"
#include "TVectorT.h"
#include "TEntryList.h"
#include "load_mva_param.h"

    template <typename T>
std::string to_string_prec(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out <<std::fixed<< std::setprecision(n) << a_value;
    return out.str();
}

template<typename T>
bool marks_compare_vec_nonsense(std::vector<T>& v1, std::vector<T>& v2)
{
        std::sort(v1.begin(), v1.end());
            std::sort(v2.begin(), v2.end());
                return v1 == v2;
}


template <typename T>
std::vector<size_t> sort_indexes(const std::vector<T> &v) {

    // initialize original index locations
    std::vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    sort(idx.begin(), idx.end(),
            [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

    return idx;
}


//TText * drawPrelim(double x, double y);
//TText * drawPrelim(double x, double y,double s);
TText * drawPrelim(double x, double y,double s, std::string in);
TText * drawPrelim(double x, double y, std::string in);


struct bdt_file{
    public:
        std::string dir;
        std::string name;
        std::string tag;
        std::string plot_name;
        std::string plot_ops;
        std::string root_dir;

        std::string weight_branch;


        TRandom3* rangen;
        std::string topo_name;

        //This is slightly deprecisated
        std::string friend_tree_file;
        std::string friend_tree_name;

        //Will allow in future for lots of friends
        std::vector<std::string> friend_files;
        std::vector<std::string> friend_names;

        //These are now passed into bdt_recomc as they should be really.
        //Still used here (in get recomcbdts) but are filled from bdt_reco not in constructor.
        std::vector<std::string> recomc_cuts;
        std::vector<std::string> recomc_names;
        std::vector<int> recomc_cols;

        int col;
        int fillstyle;
        int linestyle;
        int linecol;
		int group;//Keng

        bool is_data;
        bool is_bnbext;
        bool is_mc;
		bool is_signal;

        std::string leg;

        int rebin;	

        int numberofevents;
        int numberofevents_raw;
        double pot;

        TFile *file;
        TTree *tvertex;

        //copy tvertex into topovertex, but with topological cut.
        TTree *topovertex;


        TTree *tevent;
        TTree *tpot;
        TTree *trs;
        TTree *teventweight;
        TTree *tslice;

        std::string topological_list_name;
        TEntryList * topological_list;
        std::string precut_list_name;
        TEntryList * precut_list;
        std::string cosmicbdt_list_name;
        TEntryList * cosmicbdt_list;
        std::string bnbbdt_list_name;
        TEntryList * bnbbdt_list;

        std::vector<TEntryList*> vec_entry_lists;


        //Run management stuff, there is 5 Runs R1,R2,R3a , R3b,R4
        std::vector<double> run_fractions_plot; //fractions to plot
        std::vector<double> run_fractions_file; //fractions in file
        std::vector<std::string> run_names;
        std::vector<std::string> run_fraction_cuts;
        std::string run_weight_string;

        //a function that splits a BDT file based on string and !string
        int splitBDTfile(std::string split_string,std::string trueTAG, bdt_file* truesplit, std::string falseTAG, bdt_file *falsesplit);

        
        unsigned long jenkins_hash(std::string key);
        


        int setStageEntryList(int j);
        int setStageEntryList(int j, double, double);
        int calcPrecutEntryList();
        int calcTopologicalEntryList();
        int calcCosmicBDTEntryList(double,double);
        int calcBNBBDTEntryList(double,double);
        int calcBDTEntryList(int stage, std::vector<double> bdt_cuts);

        int scanStage(int which_stage, std::vector<double> bdt_cuts , std::string scan_string);


        int calcBaseEntryList(std::string);
        double data_tor860_wcut;
        double data_spills_E1DCNT_wcut;
        double ext_spills_ext;
        double N_samweb_ext;

        int setAsMC();
        int setAsOverlay();
        int setAsOnBeamData(double in_tor860_wcut);
        int setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext, double N_samweb_ext);
        int setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext);

        int calcPOT();

        int calcPOT(std::vector<std::string> run_names, std::vector<std::string> run_cuts, std::vector<double> run_fractions);

        int makeRunSubRunList();

        double scale_data;

        bdt_flow flow;
        bdt_variable getBDTVariable(bdt_info info);
        bdt_variable getBDTVariable(bdt_info info, std::string bin);
        //legacy code, and damned lazy too
        //bdt_variable getBDTVariable(std::string cut);
		//new
		bdt_file(size_t index,
			MVALoader XMLconfig,
			bdt_flow inflow);

        bdt_file(std::string indir,
		std::string inname, 
		std::string intag, 
		std::string inops, 
		std::string inrootdir,  
		int incol, 
		bdt_flow inflow);	
        
		bdt_file(std::string indir,
		std::string inname, 
		std::string intag, 
		std::string inops, 
		std::string inrootdir, 
		int incol, 
		int fillstyle,
		bdt_flow inflow);	

        //legacy code OBSOLETE
        //bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, std::string infriend, std::string infriendtree, int incol, bool indata);	


        int scale(double scalein);
        int setPOT(double inpot);

        TH1* getEventTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT);
        int CheckWeights();
     
        double GetEntries(std::string cuts);
        double GetEntries();
        TH1* getTH1(std::string invar, std::string cuts, std::string nam, double plot_POT, int rebin);
        TH1* getTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT, int rebin);
        TH1* getTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT);
        TH2* getTH2(bdt_variable varx, bdt_variable vary, std::string cuts, std::string nam, double plot_POT);

        std::vector<TH1*> getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT);
        std::vector<TH1*> getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT, int rebin);

        int addFriend(std::string in_friend_tree_nam, std::string in_friend_file);
        int addBDTResponses(bdt_info cosmic_bdt_info, bdt_info bnb_bdt_info,   std::vector<method_struct> TMVAmethods);
        int addBDTResponses(std::string dir, bdt_info input_bdt_info);

        ~bdt_file();

        int makeSBNfitFile(const std::string &analysis_tag, const std::vector<bdt_info>& bdt_infos, int which_stage, const std::vector<double> & fbdtcuts, const std::string & inpu);
        int makeSBNfitFile(const std::string &analysis_tag, const std::vector<bdt_info>& bdt_infos, int which_stage, const std::vector<double> & fbdtcuts, const std::string & inpu, const std::vector<bdt_variable> &vars ,double plot_pot);
        int makeSBNfitFile(const std::string &analysis_tag, const std::vector<bdt_info>& bdt_infos, int which_stage, const std::vector<double> & fbdtcuts, const std::string & inpu, const std::vector<bdt_variable> &vars ,double plot_pot,std::string external_cuts);


    int makePrecalcSBNfitFile(const std::string &analysis_tag, int which_stage, const std::vector<double> & fbdtcuts );



        std::string getStageCuts(int stage, double bdtvar1, double bdtvar2);
        std::string getStageCuts(int stage, std::vector<double> bdt_cuts);


        int writeStageFriendTree(std::string nam,double,double);
        int addPlotName(std::string plotin);
        int setTColor(TColor &);
        TColor f_TColor;
};
void get_joy();

#endif
