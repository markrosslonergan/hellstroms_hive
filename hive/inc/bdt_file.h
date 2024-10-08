#ifndef BDT_FILE_H
#define BDT_FILE_H

#include <vector>
#include <string>
#include <set>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <utility>
#include <climits>
#include <cmath>
#include <stdexcept>
/******** Our includes *****/

#include  "global_func.h"
#include  "bdt_flow.h"
#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"
#include  "method_struct.h"
#include  "duplicate_suite_2022.h"
/******** Root includes *****/

#include "TTreeFormula.h"
#include "TBranch.h"
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
#include "TMath.h"
#include "TVectorT.h"
#include "TEntryList.h"

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

std::string convertToXRootD(std::string fname_orig);
std::vector<TMatrixT<double>> splitNormShapeCopy(TMatrixT<double> & Min,std::vector<double>&vin);
int removeSubStrings(std::string &, std::string&);

TText * drawPrelim(double x, double y);
TText * drawPrelim(double x, double y,double s);
TText * drawPrelim(double x, double y,double s, std::string in);
TText * drawPrelim(double x, double y, std::string in);

class FlatVar{
private:
    std::string def;
    std::string name;
    int type;

    //address for ntuples
    int bint=-999;
    std::vector<int>* bvint=nullptr;
    std::vector<double>* bvdouble=nullptr;
    TTreeFormula* bform=nullptr;

    void special_character_check_helper(std::string& str);
public:
    static const int int_type = 0, vint_type = -1, vdouble_type = 1, formula_type = 2;

    FlatVar(const std::string& in_def, int in_type, const std::string& in_name = "NONE"): def(in_def),type(in_type), name(in_name){
        if(name == "NONE"){
            name = def;
            special_character_check_helper(name);
        }
    }

    void LinkWithTTree(TTree* tree);
    void DelinkTTree(TTree* tree);
    void WriteToFile(TFile* file);
    double Evaluate(int index=0);
    std::string GetName() const {return name;}
    std::string GetDef() const {return def;}
    bool IsInt() const {return type == int_type;}
    bool IsVector() const {return (type == vint_type || type == vdouble_type);}
    int GetLength() const;
    void Print();
};

struct bdt_file{
    public:
        std::string dir;
        std::string name;
        std::string tag;
        std::string plot_name;
        std::string plot_ops;
        std::string root_dir;

        std::string data_descriptor;

        //legacy check for things like true_eventweight
        bool is_legacy;

        std::string weight_branch;
        std::string global_weight_string;


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

        bool is_data;
        bool is_bnbext;
        bool is_mc;
 	bool is_signal;
	bool use_xrootd;

        std::string leg;

        int rebin;	

        int numberofevents;
        int numberofevents_raw;
        double pot;
        bool m_weightless;

	// numbers used to manually determine the POT of bdt file
  	bool manual_POT_norm;
 	double reference_event_count;
	double reference_pot;


        TFile *f;
        TTree *tvertex = NULL;
        std::string primary_ttree_name;

        //copy tvertex into topovertex, but with topological cut.
        TTree *topovertex = NULL;


        TTree *tevent = NULL;
        TTree *tpot = NULL;
        TTree *trs = NULL;
        TTree *teventweight = NULL;
        TTree *ttrueeventweight = NULL;
        TTree *tslice = NULL;

        std::string topological_list_name;
        TEntryList * topological_list;
        std::string precut_list_name;
        TEntryList * precut_list;
        std::string cosmicbdt_list_name;
        TEntryList * cosmicbdt_list;
        std::string bnbbdt_list_name;
        TEntryList * bnbbdt_list;

        std::vector<TEntryList*> vec_entry_lists;


        //---- Run management stuff, there is 5 Runs R1,R2,R3a , R3b,R4

	//criteria used to identify if TTree entry is an event --> for run-to-run normalization 
	std::string event_identifier;
        std::vector<double> run_fractions_plot; //fractions to plot
        std::vector<double> run_fractions_file; //fractions in file
        std::vector<std::string> run_names;
        std::vector<std::string> run_fraction_cuts;
        std::string run_weight_string;

        //a function that splits a BDT file based on string and !string
        int splitBDTfile(std::string split_string,std::string trueTAG, bdt_file* truesplit, std::string falseTAG, bdt_file *falsesplit);


	/* Generate a file where BDT scores of clusters from one event are saved in one vector 
 	 * Note: as opposite to flat file, one entry in this file corresponds to one event 
 	 */
        int MakeUnFlatTree(bdt_info & info,std::string &dir,std::string & s);

	/* Generate flat file for clusters in the event and save cluster info for every cluster
	 * Note: in the flat file, each cluster corresponds to one entry
	 * Note: for event with no clusters, one entry will be created and saved with `-1` value  
	 *
	 * Parameters:
	 * 	fout: TFile pointer of the output file, which the flat tree will be written into
	 * 	variable_list:   list of variables which will be flattened and write into output file
	 * 	treename:	 name of the output TTree
	 * 	optional_helper_variable_name: name of vector variable. Optional
	 * 	filter_cut:      cuts applied to events. Note only clusters of events passing this filter cut will be saved.
	 */
        void MakeFlatTree(TFile * fout, std::vector<FlatVar>& variable_list, const std::string& treename, const std::string& optional_helper_variable_name="DefaultNone", const std::string& filter_cut="1", bool remove_train = false);

	/* Generate a flat file for clusters
 	 * Note: unlike saving cluster info, this function evaluate if event passes given cut, and save this information for all clusters in the event 
	 * Note: for event with no clusters, one entry will be created and saved with `-1` value   <--- correponding to MakeFlatTree() function
 	 */
	void MakeFlatFriend(TFile *fout, const std::string& treename, const std::string& cut, const std::string& cut_name, const std::string& num_candidate_var);

	/* set the TTree entrylist for given stage j
	 * If external bdt cuts are given, use extenal cuts instead
	 */
        int setStageEntryList(int j, std::vector<double> bdt_cuts={});

	/* set the TTree entrylist for given stage j
 	 * for stage > 1, use the external bdt cuts given instead of bdt cuts configured in xml */
        int setStageEntryList(int j, double, double);
        int calcPrecutEntryList();
        int calcTopologicalEntryList();
        int calcCosmicBDTEntryList(double,double);
        int calcBNBBDTEntryList(double,double);

	/* calculate the entrylist that satisfies cuts at given stage */
        int calcBDTEntryList(int stage);

	/* calculate the entrylist that satisfies cuts at given stage
 	 * for stage > 1, use the external bdt cuts given */
        int calcBDTEntryList(int stage, std::vector<double> bdt_cuts);

        int scanStage(int which_stage , std::string scan_string);
        int scanStage(int which_stage, std::vector<double> bdt_cuts , std::string scan_string);

	/*grab entrylist for entries that pass topological cut and preselection cut, or generate them if not exist  */
        int calcBaseEntryList(std::string analysis_tag); 

        double data_tor860_wcut;
        double data_spills_E1DCNT_wcut;
        double ext_spills_ext;
        double N_samweb_ext;

 	/* Function: set the bdt_file as a signal input, for the purpose of calculating significance */
	void setAsSignal();

	/* Function: check whether this bdt_file is a signal input */
	bool IsSignal() const;
        int setAsMC();
        int setAsOverlay();
        int setAsOnBeamData(double in_tor860_wcut);
        int setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext, double N_samweb_ext);
        int setAsOffBeamData(double in_data_tor860_wcut, double in_data_spills_E1DCNT_wcut, double in_ext_spills_ext);

        int calcPOT();
	void setEventIdentifier(std::string cut);
        int calcPOT(std::vector<std::string> run_names, std::vector<std::string> run_cuts, std::vector<double> run_fractions);
        int makeWeightless(){
            m_weightless = true;
        }

        int setAsLegacy(){is_legacy=true; return 0;}

        int makeRunSubRunList();

        double scale_data;

        bdt_flow flow;
        bdt_variable getBDTVariable(bdt_info info);
        bdt_variable getBDTVariable(bdt_info info, std::string bin);
        //legacy code, and damned lazy too
        //bdt_variable getBDTVariable(std::string cut);

        bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, std::string addtional_weight, bdt_flow inflow);
        bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, int infillstyle, std::string addtional_weight,bdt_flow inflow);
        bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, int infillstyle, std::string addtional_weight, bdt_flow inflow, std::string inttree );
        bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, std::string addtional_weight, bdt_flow inflow, bool inuse_xrootd);
        bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, int infillstyle, std::string addtional_weight,bdt_flow inflow, bool inuse_xrootd);
        bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, int infillstyle, std::string addtional_weight, bdt_flow inflow, std::string inttree, bool inuse_xrootd);
   
        //legacy code OBSOLETE
        //bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, std::string infriend, std::string infriendtree, int incol, bool indata);	
        ~bdt_file();


        int scale(double scalein);
        int setPOT(double inpot);

        TH1* getEventTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT);
        int CheckWeights();

	/* Get number of events passing given cuts, after run-to-run configuration */
        double GetEntries(std::string cuts);
        double GetEntries();


        int getRunEfficiency();

	/* Grab TH1 distribution of "invar", with provided cuts applied */
        TH1* getTH1(std::string invar, std::string cuts, std::string nam, double plot_POT, int rebin);

	/* Grab TH1 distribution of var, with provided cuts and additional cuts embedded in bdt_variable applied */
        TH1* getTH1(bdt_variable & var, std::string cuts, std::string nam, double  plot_POT, int  rebin);
        TH1* getTH1(bdt_variable & var, std::string cuts, std::string nam, double  plot_POT);

	/* Grab 2D histogram of variable varx and vary, with provided cuts applied 
 	 */
        TH2D* getTH2(bdt_variable varx, bdt_variable vary, std::string cuts, std::string nam, double plot_POT);

        std::vector<TH1*> getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT);
        std::vector<TH1*> getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT, int rebin);

        int addFriend(const std::string& in_friend_tree_nam, const std::string& in_friend_file);
        int addBDTResponses(bdt_info cosmic_bdt_info, bdt_info bnb_bdt_info,   std::vector<method_struct> TMVAmethods);
        int addBDTResponses(bdt_info input_bdt_info);



	// not defined yet
        int makeSBNfitFile(const std::string &analysis_tag, const std::vector<bdt_info>& bdt_infos, int which_stage, const std::vector<double> & fbdtcuts, const std::string & inpu);

	/* create SBNfit file for events passing given stage 
 	 * Note: bdt scores of events are saved in simple_tree, and all trees in original bdt file and their friend trees will also be saved 
 	 */
        int makeSBNfitFile(const std::string &analysis_tag, const std::vector<bdt_info>& bdt_infos, int which_stage, const std::vector<double> & fbdtcuts, const std::string & inpu, const std::vector<bdt_variable> &vars ,const double splot_pot);
        int makeSBNfitFile(const std::string &analysis_tag, const std::vector<bdt_info>& bdt_infos, int which_stage, const std::vector<double> & fbdtcuts, const std::string & inpu, const std::vector<bdt_variable> &vars ,const double splot_pot,std::string external_cuts, std::string dir,runlist & masterRSE);


	/* create a sbnfit file with events that passing given stage
 	 * note: events are from a precalculate sbnfit file, in format of analysis_tag+"_"+this->tag+"_SSSprecalc.root"
 	 */ 
        int makePrecalcSBNfitFile(const std::string &analysis_tag, int which_stage);



        int splitAndPlot(int nsplit, bdt_variable var, double pot,int stage,std::vector<double> bdt_cuts);

        std::vector<double> getVector(bdt_variable & var, std::string  cuts);


	/* get cuts for given stage, use the bdt cuts embedded locally
 	 * This is file specific, ie, the cut will include definition cut for the file 
 	 * Note: see function GetStageCuts() in bdt_flow.h for details
 	 * Note: special case: when stage < -1, it will return "1" instead. 
 	 */
        std::string getStageCuts(int stage);


	/* get cuts for given stage, use the bdt cuts embedded locally
 	 * This is not file-specific, ie, the cut doens't include definition cut, and is applicable for all files 
 	 */
        std::string getGeneralStageCuts(int stage);
        std::string getGeneralStageCuts(int stage, const std::vector<double>& external_bdt_cuts, bool for_sbnfit = false);

	/* get cuts for given stage, use the bdt cuts provided in arguments */
        std::string getStageCuts(int stage, double bdtvar1, double bdtvar2);
        std::string getStageCuts(int stage, std::vector<double> bdt_cuts);

	/* get the plot name for all analysis stages */
	std::vector<std::string> getStageNames() const;

	/* set up quantities needed for manual POT normalization */
	void setRefPOT(double incount, double inpot);

        int writeStageFriendTree(std::string nam,double,double);
        int addPlotName(const std::string& plotin);
        int addDataDescriptor(const std::string& pin){ data_descriptor = pin;}
        int setTColor(TColor &);
        TColor f_TColor;
};
void get_joy();

#endif
