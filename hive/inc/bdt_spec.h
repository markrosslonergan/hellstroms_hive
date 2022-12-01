#ifndef BDT_SPEC_H
#define BDT_SPEC_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm> //for sorter

/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH1D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"
#include "TObject.h"
#include "TStyle.h"

class bdt_stack{
	public:

		int num_files;
		std::string name;
		double plot_pot;
		std::vector<bdt_file*> stack;
        	std::vector<TH1*> vec_hists;

        	bool do_subtraction; 
        	std::vector<bool> subtraction_vec;
    
        	std::vector<bool> signal_on_top;


		bdt_stack(std::string in) : name(in), num_files(0){
		    //plot_pot=6.6e20;
        	    //plot_pot=13.2e20;
        	    plot_pot=10.1e20;
        	    //plot_pot = 4.8e19;
        	    //plot_pot = 0.3756e+19;
        	    do_subtraction = false;
		}

		int addToStack(bdt_file* in,bool signalish){
			stack.push_back(in);
            		signal_on_top.push_back(signalish);
			++num_files;
			return 0;
		}

		int addToStack(bdt_file* in){
			stack.push_back(in);
            		signal_on_top.push_back(false);
			++num_files;
			return 0;
		}


		inline
		std::vector<bdt_file*> getFiles() {return stack;}

		int getNFiles() const {return num_files;}
	
		inline 
		double getPOT() const {return plot_pot;}

		/* Function: returns stack of distribution for given variable, with no additional cuts applied 
 		 * Note: Cuts for different stages can be implicitly applied through var.additional_cut
 		 */
		THStack* getEntryStack(bdt_variable var);
		THStack* getEntryStack(bdt_variable var,int);  // second argument - stage number - is not used


		/* Function: Get stacked histogram for given variable
 		* Arguments: level defines which stage to plot, and c1, c2 provides the cut values for first two BDTs 
 		* Default argument: skip_signal. When it's true, distribution of signals will NOT be included in the stack hist. 
 		*/
		THStack* getStack(bdt_variable var, int level,double c1, double c2, bool skip_signal=false);


		/* Function: Get stacked histogram for given bdt score 
 		 * Arguments: level defines which stage to plot, and c1, c2 provides the cut values for first two BDTs 
 		 */
		THStack* getBDTStack(bdt_info, int level,double c1, double c2);
		THStack* getBDTStack(bdt_info,std::string bin, int level,double c1, double c2);

		/* Get the TH1 distribution of variable var for bdt file that's set as a plot_on_top in the configuration xml */
		TH1* getSignalOnTop(bdt_variable var);


		/* Function: get full vector of predictions for every stacked histogram and every bin, corresponding to POT = plot_pot
		 * getEntryFullVector: returns full vector, with no cuts applied 
		 * getFullVector: return full vector at given stage, and BDT cuts
		 * getFullVectorWithError: return full vector and corresponding intrinsic error, at given stage and BDT cuts
		 * Note: The order of prediction vector for bdt files is the same as bdtfile order in configuration xml 
		 * Note: if skip_signal is set to true, signal will be zeroed out.
		 */
		std::vector<double> getEntryFullVector(bdt_variable var, bool skip_signal = false);
		std::vector<double> getFullVector(bdt_variable var, int stage, const std::vector<double>& cuts, bool skip_signal=false, std::string additional_cut="1");
		std::vector<std::vector<double>> getFullVectorWithError(bdt_variable var, int stage, const std::vector<double>& cuts, bool skip_signal=false, std::string additional_cut="1");


		/* Function: returns sum of distribution histograms for given variable, at given stage. 
 		 * Argument: level specifies which stage.
 		 * Argument: c1, c2 are the cut values for the first two BDTs
 		 * Argument: full will be the full vector of the spectrum - prediction from different breakdown concatenated together
 		 */
		TH1* getSum(bdt_variable, int level, double c1, double c2);
		TH1* getSum(bdt_variable, int level, double c1, double c2, std::vector<double>& full);



		/* series of getEntrySum function creates histogram sum of distributions for given varible before any selection
 		 * Note: argument `level` is not never used. 
 		 */
		TH1* getEntrySum(bdt_variable);
		TH1* getEntrySum(bdt_variable,int level);
		TH1* getEntrySum(bdt_variable,int level, std::vector<double>&full);


		/* Function: Get summed distribution for given bdt score over all stacked histograms, at given stage 
                 * Arguments: level defines which stage to plot, and c1, c2 provides the cut values for first two BDTs  
                 */
		TH1* getBDTSum(bdt_info which, int level, double c1, double c2);
		TH1* getBDTSum(bdt_info which,std::string bin, int level, double c1, double c2);


		/* Function: Get summed distribution for given bdt score over all stacked histograms, with no additional cuts
		 * Difference from getBDTEntryStack: one returns THStack, while this function returns TH1
		 */
       		 TH1* getBDTEntrySum(bdt_info whichbdt);

		/* Function: Get stacked histgoram for given bdt score, with no additional cuts */
	        THStack* getBDTEntryStack(bdt_info whichbdt);



		int plotStacks(TFile*ftest, std::vector<bdt_variable> vars, double c1, double c2);
		int plotStacks(TFile*f, bdt_variable var,double,double);
		int plotBDTStacks(TFile*f, bdt_info,double,double);

        int setSubtractionVector(std::vector<bool> trac){
            subtraction_vec = trac;
            do_subtraction=true;
            return 0;
        }

		int makeSBNspec(std::string tag, bdt_variable var, double, double, std::vector<std::string> hist_names);

    // Added by A. Mogan 1/14/20 to help with normalization fits
    int clearStack() {
        stack.clear();
	num_files=0;
        return 0;
    }

};


#endif
