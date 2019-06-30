#ifndef BDT_SIG_H
#define BDT_SIG_H

#include <vector>
#include <string>
#include <iostream>
#include <math.h>
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

struct contour_struct {
	TTree *tree;
	double mva;

	contour_struct( string const & tree_name);
	~contour_struct();
};

//this function helps search better foci by considering events near by.
vector<double> get_max (TH2D* grid);

//This maps out the significance versus efficiency;
void significance_eff(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info contour_bdt_info);

//assign each events according to the signal/bkg peaks that are previously determined.
vector<double> evaluate_events(bdt_file* file, bdt_info contour_bdt, int entries,  int * row, double * cos, double* bnb, vector<double> focus1, vector<double> focus2, bool draw_eff);

std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files,
	std::vector<bdt_file*> bkg_files, bdt_info cosmic_cut, bdt_info bnb_cut);

double get_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, double plot_pot, double cosmic_cut, double bnb_cut, double *best_significance,
	double *best_mva_cut , double *best_mva_cut2, std::vector<double> *vec_sig,    std::vector<double> *vec_cut);	


TH2D *fillin_grid (vector<bdt_file*> files, string hist_name, int step);

//Produce signal/sqrt(bkg) 2D histogram;

void select_events (vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, bdt_info contour_bdt, vector<double> efficiencies, int step , vector<double> adjustment);




#endif
