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



//std::vector<double> lin_scan(std::vector<bdt_file*> sig_files, 
//    std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, 
//    bdt_info bnb_focused_bdt, double c1, double c2);

std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files,
	std::vector<bdt_file*> bkg_files, bdt_info cosmic_cut, bdt_info bnb_cut);

double get_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, double plot_pot, double cosmic_cut, double bnb_cut, double *best_significance,
	double *best_mva_cut , double *best_mva_cut2, std::vector<double> *vec_sig,    std::vector<double> *vec_cut);	


TH2D *event_grid (vector<bdt_file*> files, vector<bdt_info> bdt_infos, string hist_name, int step, vector<double> limits, bool bkg);



int shrinks_boundary(TH2D* sig_grid, int step, int fix_x, int fix_y, int const max_position );

void define_boundary (TH2D * sig_grid, int step, vector<double> strictness);

void tailer_boundary (vector< vector <double> > & boundary);

void select_events (vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, vector<double> percent_sig);



#endif
