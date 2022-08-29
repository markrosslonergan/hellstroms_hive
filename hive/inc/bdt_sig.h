#ifndef BDT_SIG_H
#define BDT_SIG_H

#include <vector>
#include <string>
#include <iostream>
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
#include "TH2.h"
#include "TH1D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"


	/* Scan the linear grid of cuts for all BDTs, find the set of cuts with highest statistical significance (signal/Sqrt(background))
 	 * Note: The [min, max] range for cuts, and number of steps are configured in the xml
 	 * Note: return value is NOT useful
 	 */
	std::vector<double> scan_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot);

	// Added by A. Mogan, 10/1/19, for running sig with efficiency times purity as significance metric
	/* Comment by Guanqun:
 	 * Scan the linear grid of cuts for all BDTs, find the set of cuts with highest purity*efficiency for signal 
 	 * Note: The [min, max] range for cuts, and number of steps are configured in the xml
 	 * Note: return value is NOT useful
 	 */
	std::vector<double> scan_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot, int sig_type);

	
	/* Scan the grid of cuts for all BDTs, by randomly choosing cuts within [min, max] cut range, choose set of cuts with max signal/Sqrt(background) 
 	 * Note: The [min, max] range is configured in the xml
 	 * Note: sig_type argument is not used; return value is NOT useful
 	 */
	std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos,int sig_type);
	std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos);


	/* Given a set of cuts `bdt_cuts`, vary the BDT of order `which_bdt` and the 5th BDT while keeping cuts of other BDTs fixed, calculate the corresponding signal/Sqrt(background) 
 	 */ 
	std::vector<double> scan_significance_linlin(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, std::vector<double> bdt_cuts, int which_bdt);



	/* Scan possible values for a certain quantity (that Guanqun, doesn't understand what it actually is....), and calculate corresponding signal/Sqrt(background) ratio at each point
 	 * Note: return value is NOT useful
 	 */
	int scan_likelihood(std::vector<bdt_file*> stack_files, std::vector<bdt_info> bdt_infos);


	/* Scan different values of `SUPER_score` and calculate signal/Sqrt(background) significance; cut with best significance will be printed out
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
	 * Scan the linear grid of cuts for the first two BDTs, find the set of cuts with highest statistical significance (signal/Sqrt(background))
 	 * Note: return value is {best cut for first BDT, best cut for second BDT, best s/sqrt(B) value}
 	 */
	std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_cut, bdt_info bnb_cut);


#endif
