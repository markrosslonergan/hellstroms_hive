#ifndef BDT_SIG_H
#define BDT_SIG_H

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <utility>
#include <numeric>
#include <random>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"
#include  "bdt_covar.h"
#include  "bdt_spec.h"
#include  "SBNchi.h"

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TMatrixDfwd.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"


	/* Function: Scan the linear grid of cuts for all BDTs, find the set of cuts with highest statistical significance (signal/Sqrt(background))
 	 * Note: The [min, max] range for cuts, and number of steps are configured in the xml
 	 * Note: return value is NOT useful
 	 */
	std::vector<double> scan_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot);

	// Added by A. Mogan, 10/1/19, for running sig with efficiency times purity as significance metric
	/* Comment by Guanqun:
 	 * Function: Scan the linear grid of cuts for all BDTs, find the set of cuts with highest significance for signal 
 	 * Note: See calculate_significance() function for possible values for sig_type
 	 * Note: The [min, max] range for cuts, and number of steps are configured in the xml
 	 * Note: return value is NOT useful
 	 */
	std::vector<double> scan_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot, int sig_type);


        /* Function: scan linear grid of cuts for all BDTs, find set of cuts with highest Chi-square, taking into consideration of systematic uncertainty 
 	 * Return: cut position with the highest chi-square
 	 * Argument: stack - marks the MC predictions of all subchannels
 	 * Argument: var - varaible to perform sensitivity calculation with 
 	 * Argument: external_bdt_cuts: This marks a selection, with which  a fractional covariance matrix will be constructed, and then used for chi2 sensitivity evaluation for all grid points    
 	 * Argument: plot_pot - POT at which sensitivity is evaluated with 
 	 * Argument: signal_scale  - scaling factor applied only to signal files
 	 */
	std::vector<double> scan_chisquare_sys_fixed(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& external_bdt_cuts, double plot_pot, std::string pdfname = "CHIS_SYS" , double signal_scale=1.0);
	std::vector<double> scan_chisquare_sys_iterative(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& external_bdt_cuts, double plot_pot, std::string pdfname = "CHIS_SYS", double signal_scale = 1.0, int num_iter = 5, bool stats_only = false);
	std::vector<double> scan_chisquare_stat(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& external_bdt_cuts, double plot_pot, std::string pdfname = "CHIS_STAT", double signal_scale=1.0);



        /* Function: similar to function "scan_significance_sys_fixed", but instead of scaning linear grid setup by BDTinfos in the xmls, randomly choose cut positions within ranges.  
 	 * Return: cut position with the highest chi-square
 	 * Note: result of each randomly chosen point will be printed out.
 	 */
	std::vector<double> random_scan_chisquare_sys_fixed(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, TMatrixT<double>* fixed_frac_matrix, double plot_pot,  int num_scans = 5000, double signal_scale=1.0 );
	std::vector<double> random_scan_chisquare_sys_fixed(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& external_bdt_cuts, double plot_pot, int num_scans = 5000, double signal_scale=1.0);
	std::vector<double> random_scan_chisquare_stat(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& external_bdt_cuts, double plot_pot, int num_scans = 5000, double signal_scale=1.0);
	
	/* Function: Scan the grid of cuts for all BDTs, by randomly choosing cuts within [min, max] cut range, choose set of cuts with max significance 
 	 * Note: See calculate_significance() function for possible values for sig_type
 	 * Note: The [min, max] range is configured in the xml
 	 * Note: sig_type argument is not used; return value is NOT useful
 	 */
	std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot, int sig_type);
	std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot);


	/* Function: Given a set of cuts `bdt_cuts`, vary the BDT of order `which_bdt` and the 5th BDT while keeping cuts of other BDTs fixed, calculate the corresponding signal/Sqrt(background) 
 	 */ 
	//std::vector<double> scan_significance_linlin(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, std::vector<double> bdt_cuts, int which_bdt);
	std::vector<double> scan_significance_linlin(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, std::vector<double> bdt_cuts, int which_bdt, int which_bdt2);



	/* Function: Scan possible values for a certain quantity (that Guanqun, doesn't understand what it actually is....), and calculate corresponding signal/Sqrt(background) ratio at each point
 	 * Note: return value is NOT useful
 	 */
	int scan_likelihood(std::vector<bdt_file*> stack_files, std::vector<bdt_info> bdt_infos);


	/* Function: Scan different values of `SUPER_score` and calculate signal/Sqrt(background) significance; cut with best significance will be printed out
 	 * Note: the return value of this function is vector of zeros 
 	 * What's a super_score??
 	 */
	std::vector<double> super_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files);


	/* Probably an old function by now..
	 * What it does: Do a exponential scan for the first BDT, starting from cut value c1, while keeping cut value for second BDT at c2, find the set of cut with highest signal/Sqrt(background)
 	 * Note: return value is {best cut for first BDT, c2, best s/sqrt(B) value}
 	 */
	std::vector<double> lin_scan(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, double c1, double c2);


	/* Obsolete function..
	 * Function: Scan the linear grid of cuts for the first two BDTs, find the set of cuts with highest statistical significance (signal/Sqrt(background))
 	 * Note: return value is {best cut for first BDT, best cut for second BDT, best s/sqrt(B) value}
 	 */
	std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_cut, bdt_info bnb_cut);

	//############################################################
	//----------- BELOW ARE INTENDED FOR INTERNAL FUNCTION -------
	//############################################################

	/* Function: Return the name of the significance type */
	std::string significance_name(int sig_type);

	/* Function: Calculate the significance level 
 	 * Note: sig_type can take {0, 3, 4, 5} values
 	 * 	 0:  significance is signal/sqrt(background)
 	 * 	 3:  significance will be evaluated with efficency*purity | argument total_signal needs to be setup
 	 * 	 4:  significance will be evaluated with signal efficiency  | argument total_signal needs to be setup
 	 * 	 5:  significance will be evaluated with signal purity
 	 * Note: signal_scale is a flat factor that will be applied to signal during significance calculation
 	 */
	double calculate_significance(double signal, double background, int sig_type, double total_signal= 0, double signal_scale = 1);


 	void make_project_plots(const std::vector<std::vector<double>>& bdt_scan_pts, const std::vector<bdt_info>& bdt_infos, const std::vector<double>& result_vec, std::string yaxis_name, std::string pdfname);

	/* Function: calculate the grid for bdt score cut 
 	 * Returns: {maxvals, minvals, n_steps, steps}
 	 * - maxvals (minvals): max(min) score boundary for all BDTs 
 	 * - n_steps: number of steps for each BDT range
 	 * - steps: step size for each BDT cut
 	 * Note: arguments bdt_scan_pts will be updated and filled with all cut positions for all BDTs
 	 * Note: argument total_pts will be filled, as total number of grid points.
 	 */
        std::vector<std::vector<double>> setup_bdt_cut_grid(const std::vector<bdt_info>& bdt_infos, std::vector<std::vector<double>>& bdt_scan_pts, int& total_pts, std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files);
        std::vector<std::vector<double>> setup_bdt_cut_grid(const std::vector<bdt_info>& bdt_infos, std::vector<std::vector<double>>& bdt_scan_pts, int& total_pts, bdt_stack* stack);


	/* Given global index, find local position of it in every sub-vector, and return value for this index
 	 */
	std::vector<int> get_local_indices(int global_index, const std::vector<std::vector<double>>& bdt_scan_pts);
	std::vector<double> grab_point(int global_index, const std::vector<std::vector<double>>& bdt_scan_pts);


	/* Function: Given fractional covariance matrix, observed spectrum, and predicted spectrum (and correspodning MC instrinsic erorr), calculate the chi-square. 
 	 * Argument: scale --> overall scaling to input spectra		
 	 * Return: chi-square value
 	 */
	double calculate_chi(sbn::SBNchi* chi_handle, TMatrixT<double>* frac_matrix, std::vector<double>& fulldata, std::vector<double>& full_spec, std::vector<double>& full_spec_error, double scale=1.0);


	/* Round given value to certain decimal points
	 * Argument: precision = 1 --> round to integer 
	 * 	     precision = 0.1 --> round to 1st decimal point 
	 * 	     precision = 0.01 --> round to 2 decimal points 
	 * 	     ...
	 */
	double round_to(double value, double precision = 1.0);


        /* Function: scan grid of cuts for all BDTs, find set of cuts with highest Chi-square, taking into consideration of systematic uncertainty 
 	 * Return: cut position with the highest chi-square
 	 * Note: fixed fractional covariance matrix is used to evaluate chi2 values for all grid points
 	 * Argument: stack - marks the MC predictions of all subchannels
 	 * Argument: chi_handle - SBNchi pointers, required for covariance matrix inversion and chi-calculation
 	 * Argument: var - varaible to perform sensitivity calculation with 
 	 * Argument: bdt_scan_pts - 2D vector, where each 1D vector represents different cut potisionts for each BDT
 	 * Argument: max_pts - total number of different BDT cut combinations
 	 * Argument: fixed_frac_matrix - input fractional covariance matrix 
 	 * Argument: plot_pot - POT at which sensitivity is evaluated with 
 	 * Argumnet: stage - stage of the analysis, usually should be Num_BDT + 1
 	 * Argument: signal_scale  - scaling factor applied only to signal files
 	 */
	std::vector<std::vector<double>> chisquare_grid_scan_sys_fixed(sbn::SBNchi* chi_handle, bdt_stack* stack,  bdt_variable var, std::vector<std::vector<double>>& bdt_scan_pts, int max_pts, TMatrixT<double>* fixed_frac_matrix, int stage, double plot_pot, double signal_scale);
#endif
