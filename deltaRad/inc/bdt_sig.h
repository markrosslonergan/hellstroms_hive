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

//Objects to record.. signal range.
class parabola{
	public:
		//Must do 1
		parabola(vector<double> point1, vector<double> point2){//PASS
			
			focus1 = point1;
			focus2 = point2;
			c = to_standard(point1)[0];
		}
		//Must do 2
		void search_directrix( vector< double* > points, int total_count){
			int count = 0;
			double eff = 0.9;//CHECK this gives points that has 0.9 effciency of signal;
			vector< double > x_coor;
			while(count<total_count){
				x_coor.push_back(to_standard({points[0][count],points[1][count]})[0]);//get all x coordinates
				cout<<points[0][count]<<","<<points[1][count];
				cout<<" at standard: "<<to_standard({points[0][count],points[1][count]})[0]<<endl;
				count++;
			}
			sort(x_coor.begin(),x_coor.end());

			a = x_coor[floor(count*eff)];
			cout<<"CHECK"<<endl;
			directrix_std = a*a/c;//in the standard form;
			cout<<"CHECK"<<endl;
			directrix[0]=-tan(theta);//get the generalized form; theta cant be 90 degree..
			cout<<"CHECK"<<endl;
			directrix[1]=(focus1[1]+focus2[1])/2+directrix_std*sin(theta);
			cout<<"CHECK"<<endl;
			if(c>a){
				b = sqrt(c*c-a*a);
			}else{
				b = sqrt(a*a - c*c);	
				cout<<"We might have an elipse."<<endl;
			}
		}

		//Coordinate transformation;
		double get_rotation(){
			theta = atan2((focus1[1]-focus2[1]),(focus1[0]-focus2[0]));
			return theta;
		}

		double get_a(){
			return a;
		}

		double get_b(){
			return b;
		}

		double get_focus(){
			return c;
		}

		vector< double > to_standard( vector <double> point ){//PASS
			//Translation
			for(int i = 0; i < 2; i++){
				point[i] -= (focus1[i]+focus2[i])/2;
			}
			//rotation
			double x = point[0]; 
			double y = point[1]; 
//			cout<<"To standard coordinate "<<x<<endl;
//			cout<<"To standard coordinate "<<y<<endl;
//			cout<<endl;
			point[0] = cos(theta)*x+sin(theta)*y;
			point[1] = -sin(theta)*x+cos(theta)*y;
			return point;
		}

		vector< double > to_general( vector <double> point ){//reverse the to_standard function
			//translation;
			for(int i = 0; i < 2; i++){
				point[i] += (focus1[i]+focus2[i])/2;
			}
			//rotation
			double x = point[0]; 
			double y = point[1]; 

			point[0] = cos(theta)*x-sin(theta)*y;
			point[1] = +sin(theta)*x+cos(theta)*y;
			return point;
		}
		

		string draw_std_conic(){//expression for the conic section
			if(c>a){
				return to_string(b)+"*sqrt(x*x/("+to_string(a)+"*"+to_string(a)+")-1)";
			}else{
				return to_string(b)+"*sqrt(1-x*x/("+to_string(a)+"*"+to_string(a)+"))";
			}
		}
		
		string draw_gen_conic(){//expression for the conic section
			string ang = to_string(theta);
			string trans_x = to_string((focus1[0]+focus2[0])/2);
			string trans_y = to_string((focus1[1]+focus2[1])/2);
			string new_y = "-("+trans_y+"+cos("+ang+")*x-sin("+ang+")*y)+";
			string new_x = "("+trans_x+"+sin("+ang+")*x+cos("+ang+")*y)";

			if(c>a){
				return new_y+to_string(b)+"*sqrt("+new_x+"**2/("+to_string(a)+"*"+to_string(a)+")-1)";
			}else{
				return new_y+"-"+to_string(b)+"*sqrt("+new_x+"**2/("+to_string(a)+"*"+to_string(a)+")-1)";
			}
		}

		string draw_gen_line(){
			return to_string(directrix[0])+"*x+"+to_string(directrix[1]);//y=kx+b
		}

		string draw_std_line(){
			return "x="+to_string(directrix_std);
		}
		
		double get_eccentricity ( vector<double> std_point){
			//USE STANDARD FORM of coordinates;
			//distance to directrix;
			double d_directrix = abs(std_point[0]-directrix_std);
			//distance to focus1;
			vector<double> std_focus = to_standard(focus1);
			double d_focus1 = sqrt(pow(std_focus[0]-std_point[0],2)+pow(std_focus[1]-std_point[1],2));
			cout<<d_directrix<<" and "<< d_focus1<<endl;
			return d_directrix/d_focus1;//the ratio
		}

				
		double eccentricity;

		vector<double> directrix{0,0};//any lines parameterized with k,b
		double directrix_std;//x=???;
		//this is given by a^2/c in normal form; where a is the boundary along the segment of two foci;

	private:
		vector<double> focus1;	
		vector<double> focus2;	
		double a;
		double b;
		double c;
		double theta;//in radians;
};

//void parabola::search_directrix( vector< double > *points){
//	int count = 0;
//	double eff = 0.9;
//	vector< double > x_coor;
//	while(points[0][count]){
//		x_coor.push_back(to_standard(points[count])[0]);//get all x coordinates
//		count++;
//	}
//	sort(x_coor.begin(),x_coor.end());
//	directrix_std = x_coor[(int)count*0.9];
//};
 

//std::vector<double> lin_scan(std::vector<bdt_file*> sig_files, 
//    std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, 
//    bdt_info bnb_focused_bdt, double c1, double c2);

struct contour_struct_v2 {
	TTree *tree;
	double distance;

	contour_struct_v2( string const & tree_name);
	~contour_struct_v2();
};

vector<double> get_max (TH2D* grid);

void significance_eff(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info contour_bdt_info);
vector<double> evaluate_events(bdt_file* file, int entries,  int * row, double * cos, double* bnb, vector<double> focus1, vector<double> focus2, bool draw);

std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files,
	std::vector<bdt_file*> bkg_files, bdt_info cosmic_cut, bdt_info bnb_cut);

double get_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, double plot_pot, double cosmic_cut, double bnb_cut, double *best_significance,
	double *best_mva_cut , double *best_mva_cut2, std::vector<double> *vec_sig,    std::vector<double> *vec_cut);	


//THE FOLLOWINGS ARE GOING TO BE DUMPED;
TH2D *event_grid (vector<bdt_file*> files, vector<bdt_info> bdt_infos, string hist_name, int step);

//Produce signal/sqrt(bkg) 2D histogram;
void select_events (vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, vector<double> percent_sig, int step );

void select_events_v2 (vector<bdt_file*> sig_files, vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, vector<double> efficiencies, int step );

//find a boundary of contour for significance that satifies the wanted significance respected to the local maximum (a reasonable value that is < 10, but not the peak value);
void define_boundary (TH2D * sig_grid, int step, vector<double> strictness);

void define_boundary_v2 (TH2D * sig_grid, TH2D * bkg_grid, vector<double> strictness);

//shrinks the boundary into a finer region, used in the define_boundary function.
int shrinks_boundary(TH2D* sig_grid, int step, int fix_x, int fix_y, int const max_position );

//after boundary is found, remove unnecessary vertices of the boundary.
void tailor_boundary (vector< vector <double> > & boundary);




#endif
