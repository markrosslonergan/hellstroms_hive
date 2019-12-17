#ifndef BDT_PRECALC_H
#define BDT_PRECALC_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"

#include "TruncMean.h"
/******** Root includes *****/
#include "TFitResult.h"
#include "TF1.h"
#include "TTreeFormula.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TH2D.h"
#include "TLegend.h"
#include "THStack.h"
#include "TGraph.h"
#include "TLine.h"
#include "TMVA/Types.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Reader.h"
#include "TFriendElement.h"
#include "TGraph2D.h"
#include <Math/Functor.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <Fit/Fitter.h>
#include <TVectorD.h>
#include <TPrincipal.h>


int addPreFriends(bdt_file* filein,std::string which);

class bdt_precalc{
	std::string const bnbcorrection_dir;
	public:
	bdt_file* file;
	//TFile *friend_file_out;
	std::string friend_file_out_name;
	//pass in a bdt_file and it calculates stuff and add's it as friends!
	bdt_precalc(bdt_file* in, std::string const & dir = "../..") : file(in),
		bnbcorrection_dir(dir) {
			friend_file_out_name =file->tag+"_friends.root";

		};
	//This will actually
	~bdt_precalc(){ ;}

	int gen(std::string which );
	int genTrackInfo();
	int genNewTrackInfo();
	int genShowerInfo();
	int genPi0Info();
	int genBNBcorrectionInfo();

	};

int ncpi0_sss_precalc(const bdt_file * file);



//line between X1 and X2, point X0
double dist_line_point( std::vector<double>X1, std::vector<double> X2, std::vector<double> X0);
double cos_angle_3pts(std::vector<double> last, std::vector<double> next, std::vector<double> mid);

void line(double t, const double *p, double &x, double &y, double &z);

struct SumDistance2 {


	TGraph2D *fGraph;
	SumDistance2(TGraph2D *g) : fGraph(g) {}
	double distance2(double x,double y,double z, const double *p) {
		ROOT::Math::XYZVector xp(x,y,z);
		ROOT::Math::XYZVector x0(p[0], p[2], 0. );
		ROOT::Math::XYZVector x1(p[0] + p[1], p[2] + p[3], 1. );
		ROOT::Math::XYZVector u = (x1-x0).Unit();
		double d2 = ((xp-x0).Cross(u)).Mag2();
		return d2;
	}
	double operator() (const double *par) {
		assert(fGraph != 0);
		double * x = fGraph->GetX();
		double * y = fGraph->GetY();
		double * z = fGraph->GetZ();
		int npoints = fGraph->GetN();
		double sum = 0;
		for (int i  = 0; i < npoints; ++i) {
			double d = distance2(x[i],y[i],z[i],par);
			sum += d;
		}
		return sum;
	}
};

#endif
