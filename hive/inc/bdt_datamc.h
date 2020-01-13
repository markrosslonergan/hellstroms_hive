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

/******** Root includes *****/

#include "TTreeFormula.h"
#include "TText.h"
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

        bool isSpectator = false;
        bool is_bdt_variable;
        bool do_subtraction; 
        int plot_stage;
        std::vector<bool> subtraction_vec;

        bdt_datamc(bdt_file* datafilein, bdt_stack* stackin) : data_file(datafilein), mc_stack(stackin) {tag = "null";is_bdt_variable=false; do_subtraction=false;plot_stage=-1;stack_mode=false;};
        bdt_datamc(bdt_file* datafilein, bdt_stack* stackin, std::string tagin) : data_file(datafilein), mc_stack(stackin), tag(tagin) {is_bdt_variable = false; do_subtraction=false;plot_stage=-1;stack_mode=false; };
        bdt_datamc(bdt_file* datafilein, bdt_stack* stackin, std::string tagin, bdt_info infoin) : data_file(datafilein), mc_stack(stackin), tag(tagin) {do_subtraction=false;plot_stage=-1;stack_mode=false;};

        int setPlotStage(int s){
            plot_stage =s;
            return s;
        }

        int setSubtractionVector(std::vector<bool> trac){
            subtraction_vec = trac;
            do_subtraction=true;
            return 0;
        }
        std::vector<bdt_variable> GetSelectVars(std::string vector, std::vector<bdt_variable> vars);
        int plot2D(TFile *ftest, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts);
        int plotStacks(TFile *ftest, std::vector<bdt_variable> vars, double c1, double c2);
        int plotStacks(TFile*f,std::vector<bdt_variable> vars, std::vector<double> cuts);

        int plotStacks(TFile *ftest, bdt_variable var,double c1, double c2, bdt_info whichbdt);
        int plotStacks(TFile*f, bdt_variable var,double,double);

        int plotBDTStacks(TFile*f, bdt_info,double,double);
        int plotBDTStacks(bdt_info info, std::vector<double> bdt_cuts);

        int SetSpectator();

        int printPassingDataEvents(std::string outfilename, int stage, double c1, double c2);
        int printPassingDataEvents(std::string outfilename, int stage, std::vector<double> cuts);
        int printPassingPi0DataEvents(std::string outfilename, int stage, std::vector<double> cuts);


};


#endif
