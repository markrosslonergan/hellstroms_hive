#ifndef BDT_DATAMC_H
#define BDT_DATAMC_H

#include <vector>
#include <string>
#include <iostream>
/******** Our includes *****/

#include  "bdt_file.h"
#include  "bdt_var.h"
#include  "bdt_info.h"
#include  "bdt_spec.h"
#include  "bdt_covar.h"
/******** Root includes *****/

#include "TTreeFormula.h"
#include "TText.h"
#include "TMath.h"
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
#include "TStyle.h"
#include "TGraphAsymmErrors.h"
#include "TF1.h"
#include "TMath.h"
#include "TSystem.h"

class bdt_datamc{
    public:


        bool stack_mode;
        double stack_pot;
        bool setStackMode(double s){
            stack_pot = s;
            stack_mode = true;
            return true;
        }

        bdt_file* data_file;
        bdt_stack *mc_stack;
        std::string tag;
        std::string additional_pdf_tag;

        std::string m_error_string;
        bool isSpectator = false;
        bool is_bdt_variable;
        bool do_subtraction; 
        int plot_stage;
        bool scale_signal_overlay;
        bool plot_lee_on_top;
        bool div_bin;
        double div_scale;
        bool isPublicPlot = false; 
        std::vector<bool> subtraction_vec;
        std::vector<bool> merge_down_vec;

        double legend_posx;

        bdt_datamc(bdt_file* datafilein, bdt_stack* stackin) : data_file(datafilein), mc_stack(stackin) {tag = "null";is_bdt_variable=false; do_subtraction=false;plot_stage=-1;stack_mode=false;m_error_string=STATS;additional_pdf_tag="";  scale_signal_overlay =false; plot_lee_on_top=false; div_bin=false; legend_posx = 0.5;};
        bdt_datamc(bdt_file* datafilein, bdt_stack* stackin, std::string tagin) : data_file(datafilein), mc_stack(stackin), tag(tagin) {is_bdt_variable = false; do_subtraction=false;plot_stage=-1;stack_mode=false; m_error_string=STATS; additional_pdf_tag="";  scale_signal_overlay =false;plot_lee_on_top = false; div_bin=false; legend_posx = 0.5;};
        bdt_datamc(bdt_file* datafilein, bdt_stack* stackin, std::string tagin, bdt_info infoin) : data_file(datafilein), mc_stack(stackin), tag(tagin) {do_subtraction=false;plot_stage=-1;stack_mode=false;m_error_string=STATS; additional_pdf_tag = ""; scale_signal_overlay =false; plot_lee_on_top=false;div_bin = false; legend_posx = 0.5;};

        int setErrorString(std::string in){m_error_string = in; return 0;}

        int setMergeDown(std::vector<bool> merin){
            merge_down_vec =merin;
            return 0;
        }

        int setLegendPos(double in){
            legend_posx = in!=-999 ? in : legend_posx;
            return 0;
        }


        int setPublicPlot(bool inval){
            isPublicPlot = inval;
            return 0;
        }


        int setPlotStage(int s){
            plot_stage =s;
            return s;
        }

        int setSubtractionVector(std::vector<bool> trac){
            subtraction_vec = trac;
            do_subtraction=true;
            return 0;
        }

        int setScaledSignal(){
            scale_signal_overlay = true;
            return 0;
        }

        int setDivBin(double val){
            div_bin = true;
            div_scale = val;
            return 0;
        }
        int setLEEonTop(){
            plot_lee_on_top = true;
            return 0;
        }


        std::vector<bdt_variable> GetSelectVars(std::string vector, std::vector<bdt_variable> vars);
        int plot2D(TFile *ftest, std::vector<bdt_variable> vars, bool scatter_plot=false);

        //THIS IS It
        /* Draw stacked distributions for provided variables 
 	 * If external bdt cuts are provided, will use these bdt cuts instead of the cuts configured in xml
 	 */
        int plotStacks(TFile *ftest, std::vector<bdt_variable> vars, double c1, double c2);
        int plotStacks(TFile*f,std::vector<bdt_variable> vars, std::vector<double> cuts = {} );
        int plotStacks(TFile *ftest, std::vector<bdt_variable> vars, std::string tago, std::vector<double> bdt_cuts={}  );

        int plotStacks(TFile *ftest, bdt_variable var,double c1, double c2, bdt_info whichbdt);
        int plotStacks(TFile*f, bdt_variable var,double,double);

        int plotBDTStacks(TFile*f, bdt_info,double,double);
        int plotBDTStacks(bdt_info info, std::vector<double> bdt_cuts);


        int SetSpectator(){
            isSpectator=true;
        }

        int printPassingDataEvents(std::string outfilename, int stage, double c1, double c2);
        int printPassingDataEvents(std::string outfilename, int stage, std::vector<double> cuts);
        int printPassingPi0DataEvents(std::string outfilename, int stage, std::vector<double> cuts);

        int calcChi2(TH1 *mc_hist, TH1 *data_hist);
        void scaleNorm(std::vector<bdt_variable> var, std::vector<bdt_file*> stack_files, double scaleLow, double scaleHigh, double scaleStep, int stage, std::vector<double> bdt_cuts, std::string analysis_tag);

	/* Given fractional/full covariance matrix and variable, calculate corresponding collapsed covariace matrix */
        int calcCollapsedCovariance(TMatrixD * frac_full, TMatrixD *frac_coll,bdt_variable & var);

	/* Collapse full covariance matrix `Min` into collapsed matrix `Mout` */
        int simpleCollapse(TMatrixD * Min, TMatrixD * Mout, bdt_variable & var);

        double calcTotalNormError(TMatrixD * Min, bdt_variable & var);
        
            int plotEfficiency(std::vector<bdt_variable> vars, std::vector<double> bdt_cuts, int stage_denom, int stage_numer);
        
        // Added 5/12/20 by A. Mogan
        //TMatrixD CalcCovarianceMatrixCNP(TMatrixD *M, std::vector<double> spec, const std::vector<double> datavec );




};


#endif
