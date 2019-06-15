#ifndef BDT_CONTOURCUT_H
#define BDT_CONTOURCUT_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/
#include "method_struct.h"
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
using namespace std;

struct contour_struct {
	TTree *tree;
	double distance;

	contour_struct( string const & tree_name);
	~contour_struct();
};

//the main code to perform contour_selection, it produce a scattering plot while selecting events inside the given contour (defined by the file ./contour_cut/curve_selection*.txt).
void contour_selection(std::vector<bdt_file*> files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, double c1, double c2, bool make_cut, vector<double> strictness);

//use this to read off information from a single column txt file;
vector<int> file_reader( string str);

//evaluate all events in the file and assign it a short distance when it is in the contour.
vector<int> select_events(bdt_file* file, int entires, int * row, double * cos, double* bnb, vector<double> strictness );

//determine whether or not an event is inside the given contour (determined by the boundary)
bool inside_contour( vector < double > bdt_responses , vector< vector<double> > boundary);
#endif
