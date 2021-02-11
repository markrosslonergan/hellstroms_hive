#ifndef BDT_EFF_H
#define BDT_EFF_H

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
#include "TGaxis.h"
int fancyFiciency(bdt_file *file,std::string, std::string additonal_defin, bdt_variable & var, std::string tag, int denom_stage, int numer_stage,std::vector<double> bdtcuts);

int nue_efficiency(bdt_file* filein, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT,std::string tag);


class bdt_efficiency{
    public:
        bdt_file* file;
        std::string denominator;
        TEntryList *event_entry_list;

        std::vector<double> verticies;

        bdt_efficiency(bdt_file*, std::string denomin,double,double);
        bdt_efficiency(bdt_file*, std::string denomin,double,double,bool);
        bdt_efficiency(bdt_file* filein, std::vector<std::string> denomin, std::vector<std::string> topoin, std::vector<std::string> precuts, double c1, double c2, double p);
        bdt_efficiency(bdt_file* filein, std::vector<std::string> denomin, std::vector<std::string> topoin, std::vector<std::string> precuts, std::vector<double>, double p);

        bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT,bool,int, std::string tag, bool pretopo, bool is0p, bdt_variable tmp_vars);


        bdt_efficiency(bdt_file* filein, std::vector<std::string> v_denomin, std::vector<std::string> v_topo, std::vector<std::string> v_precuts , std::vector<double> bdt_cuts, double plot_POT,bool is_ok,int plot_stage,std::string tag, bool pretopo);

        bdt_efficiency(std::vector<bdt_file*> vec_files, std::string cut);
        // For NC pi0 filter stuff
        bdt_efficiency(std::vector<bdt_file*> vec_files, std::vector<bdt_file*> filtered_files);


};


#endif
