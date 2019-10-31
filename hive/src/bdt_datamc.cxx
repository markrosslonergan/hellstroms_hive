#include "bdt_datamc.h"

int bdt_datamc::plotStacks(TFile *ftest, bdt_variable var,double c1, double c2){
    is_bdt_variable = false;
    bdt_info nullinfo;
    return this->plotStacks(ftest, var, c1,c2,nullinfo);
}

int bdt_datamc::plotBDTStacks(TFile *ftest, bdt_info whichbdt,double c1, double c2){
    is_bdt_variable = true;

    double tmin =9999;
    double tmax = -9999;

    /*
       for(auto &f: mc_stack->stack){
       std::cout<<"TAG: "<<f->tag<<" "<<tmax<<" "<<tmin<<std::endl;

       TH1 * tmp = f->getTH1((f->tag +"_"+whichbdt.identifier+ ".mva") ,(f->tag +"_"+whichbdt.identifier+ ".mva > 0")  , "tmpBDtstack_"+data_file->tag+"_"+whichbdt.name,1,0);

       tmax = std::max( tmax, f->tvertex->GetMaximum( (f->tag +"_"+whichbdt.identifier+ ".mva").c_str()   ));
       if(f->tag!="NCDeltaRadCosmics") tmin = std::min( tmin, tmp->GetBinCenter(tmp->FindFirstBinAbove(0.05)));
       std::cout<<"TAG: "<<f->tag<<" "<<tmax<<" "<<tmin<<std::endl;
       delete tmp;
       }

       std::string  binning = "(20,"+std::to_string(tmin*0.975)+","+std::to_string(tmax*1.025)+")";
       */
    std::string  binning = whichbdt.binning;

    bdt_variable dvar = data_file->getBDTVariable(whichbdt, binning);
    dvar.is_logplot = true;

    /*std::vector<bdt_variable> tmp_v = {dvar};

      return this->plotStacks(ftest, tmp_v ,c1,c2);
      */

    //run on old one
    return this->plotStacks(ftest, dvar,c1,c2,whichbdt);
}

int bdt_datamc::printPassingDataEvents(std::string outfilename, int stage, std::vector<double> cuts){

    //data_file->calcCosmicBDTEntryList(c1, c2);
    //data_file->calcBNBBDTEntryList(c1, c2);
    //data_file->setStageEntryList(3);

    //   std::string fake = "fake_bnbbdt_list_"+std::to_string(c1)+"_"+std::to_string(c2)+"_" +data_file->tag;

    // data_file=  mc_stack->stack[0];
    std::string fake = "";
    data_file->tvertex->Draw((">>"+fake).c_str(), data_file->getStageCuts(stage,cuts).c_str() , "entrylist");
    TEntryList * fake_list = (TEntryList*)gDirectory->Get(fake.c_str());


    int n_run_number = 0;
    int n_subrun_number = 0;
    int n_event_number = 0;
    double n_vertex_z =0;
    double n_vertex_y =0;
    double n_vertex_x =0;
    double n_reco_shower_energy_max = 0;


    data_file->tvertex->SetBranchAddress("run_number",    &n_run_number);
    data_file->tvertex->SetBranchAddress("subrun_number", &n_subrun_number);
    data_file->tvertex->SetBranchAddress("event_number",  &n_event_number);
    data_file->tvertex->SetBranchAddress("reco_vertex_z", &n_vertex_z);
    data_file->tvertex->SetBranchAddress("reco_vertex_y", &n_vertex_y);
    data_file->tvertex->SetBranchAddress("reco_vertex_x", &n_vertex_x);
    data_file->tvertex->SetBranchAddress("reco_shower_energy_max", &n_reco_shower_energy_max);



    std::cout<<"Starting printPassingDataEvents() for "<<data_file->name<<std::endl;

    for(int i=0;i < fake_list->GetN(); i++ ){
        data_file->tvertex->GetEntry( fake_list->GetEntry(i));
        std::cout<<i<<" "<<fake_list->GetEntry(i)<<" "<<n_run_number<<" "<<n_subrun_number<<" "<<n_event_number<<" ("<<n_vertex_x<<", "<<n_vertex_y<<", "<<n_vertex_z<< ")"<<" and shower energy = "<<n_reco_shower_energy_max<<std::endl;
    }
    std::cout<<"End printPassingDataEvents()  for "<<data_file->name<<std::endl;


    return 0;
}


int bdt_datamc::printPassingDataEvents(std::string outfilename, int stage, double c1, double c2){

    data_file->calcCosmicBDTEntryList(c1, c2);
    data_file->calcBNBBDTEntryList(c1, c2);
    data_file->setStageEntryList(3);

    std::string fake = "fake_bnbbdt_list_"+std::to_string(c1)+"_"+std::to_string(c2)+"_" +data_file->tag;

    data_file->tvertex->Draw((">>"+fake).c_str(), data_file->getStageCuts(3,c1,c2).c_str() , "entrylist");
    TEntryList * fake_list = (TEntryList*)gDirectory->Get(fake.c_str());


    int n_run_number = 0;
    int n_subrun_number = 0;
    int n_event_number = 0;
    double n_vertex_z =0;

    data_file->tvertex->SetBranchAddress("run_number",    &n_run_number);
    data_file->tvertex->SetBranchAddress("subrun_number", &n_subrun_number);
    data_file->tvertex->SetBranchAddress("event_number",  &n_event_number);
    data_file->tvertex->SetBranchAddress("reco_vertex_z", &n_vertex_z);

    std::cout<<"Starting printPassingDataEvents() for "<<data_file->name<<std::endl;

    for(int i=0;i < fake_list->GetN(); i++ ){
        data_file->tvertex->GetEntry( fake_list->GetEntry(i));
        std::cout<<i<<" "<<fake_list->GetEntry(i)<<" "<<n_run_number<<" "<<n_subrun_number<<" "<<n_event_number<<" "<<n_vertex_z<<std::endl;
    }
    std::cout<<"End printPassingDataEvents() for "<<data_file->name<<std::endl;


    return 0;
}

int bdt_datamc::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, double c1, double c2){
    return this->plotStacks(ftest,vars,{c1,c2});
}

std::vector<bdt_variable> bdt_datamc::GetSelectVars(std::string vector, std::vector<bdt_variable> vars){
    std::vector<bdt_variable> select_vars = {};
    //first parse string as a vector

    std::vector<int> vect;
    std::stringstream ss(vector);

    //for each character in the string add the ints
    for (int i; ss >> i;) {
        vect.push_back(i);    
        if (ss.peek() == ',')
            ss.ignore();
    }

    //then for each number, add that variable to the vars list
    for (std::size_t i = 0; i < vect.size(); i++){
        select_vars.push_back(vars[vect[i]]);
    }

    for(auto vars: select_vars){
        std::cout<<"added vars to list "<<vars.safe_unit<<std::endl;

    }

    return select_vars;
}

int bdt_datamc::plot2D(TFile *ftest, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts){
    if (vars.size() < 2){
        std::cout<<"need min 2 vars to make 2D plots"<<std::endl;
        return 0;
    } 

    // NEW ONE
    double plot_pot=data_file->pot;
    if(stack_mode) plot_pot = stack_pot;

    std::cout<<"DATAMC PLOT POT "<<plot_pot<<std::endl;

    double title_size_ratio=0.1;
    double label_size_ratio=0.1;
    double title_offset_ratioY = 0.3 ;
    double title_offset_ratioX = 1.1;

    double title_size_upper=0.15;
    double label_size_upper=0.05;
    double title_offset_upper = 1.45;


    ftest->cd();

    std::vector<std::string> stage_names = {"Topological Selection","Pre-Selection Cuts","Cosmic BDT Cut","BNB BDT cut","NCPi0 BDT Cut","NUE BDT Cut","tmp"};
    //Loop over all stages

    int s_min = 0;
    int s_max = bdt_cuts.size()+2;
    if(plot_stage >=0){
        s_min = plot_stage;
        s_max = plot_stage+1;
    }


    //for each stage
    for(int s = s_min; s< s_max; s++){

        std::cout<<"On stage: "<<s<<std::endl;
        //First set the files at this stage
        for(auto &f: mc_stack->stack){
            std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
            if(s>1) f->calcBDTEntryList(s,bdt_cuts);
            std::cout<<"Setting up EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
            f->setStageEntryList(s);
        }	

        std::cout<<"Done with computations on TTrees and bdt_stacks"<<std::endl;

        if(s>1) data_file->calcBDTEntryList(s,bdt_cuts);

        data_file->setStageEntryList(s);

        //And all variables in the vector var
        //make pairs of all combos
        for(int i = 0; i < vars.size(); i++){
            bdt_variable var1 = vars[i];

            for(int j = 0; j < vars.size(); j++){

                //only want to plot different variables, but also not duplicate i.e. 12 and 21
                if (i!= j && i < j){
                    bdt_variable var2= vars[j];


                    std::cout<<"Starting on variable "<<var1.name<<std::endl;

                    //make file for data
                    TCanvas *cobs = new TCanvas(("can_"+var1.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)).c_str(),1800,1600);
                    cobs->cd();

                    TPad *pad = new TPad(("pad_"+stage_names.at(s)).c_str(), ("pad_"+stage_names.at(s)).c_str(), 0, 0, 1, 1.0);
                    pad->Draw();
                    pad->cd();

                    //THStack *stk = (THStack*)mc_stack->getEntryStack(var,s);
                    //TH1 * tsum = (TH1*)mc_stack->getEntrySum(var,s);
                    TH2 * d0 = (TH2*)data_file->getTH2(var1,var2, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[s])+"_"+data_file->tag+"_"+var1.safe_unit+"_"+var2.safe_unit, plot_pot);

                    pad->cd();

                    d0->Draw("COLZ");
                    d0 ->SetTitle((data_file->tag + ", stage " + std::to_string(s)).c_str());
                    d0->GetYaxis()->SetTitleSize(0.05);
                    d0->GetYaxis()->SetTitleOffset(0.9);
                    d0->GetXaxis()->SetTitleSize(0.05);
                    d0->GetXaxis()->SetTitleOffset(0.9);
                    pad->SetRightMargin(0.15);
                    
                    std::cout<<"Writing pdf."<<std::endl;
                    cobs->Write();
                    cobs->SaveAs(("var2D/"+tag+"_"+data_file->tag+"_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)+".pdf").c_str(),"pdf");


                    delete cobs;
                    delete d0;


                    //now repeat for all of the MC files

               for(auto &f: mc_stack->stack){

                    std::cout<<"Stack "<<f->tag<<" level "<<s<<std::endl;
                    TCanvas *cobsmc = new TCanvas(("can_"+var1.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)).c_str(),1800,1600);
                    cobsmc->cd();

                    TPad *padmc = new TPad(("pad_"+stage_names.at(s)).c_str(), ("pad_"+stage_names.at(s)).c_str(), 0, 0, 1, 1.0);
                    padmc->Draw();
                    padmc->cd();

                                
                    TH2 * mc = (TH2*)f->getTH2(var1,var2, "1", std::to_string(s)+"_mc_"+std::to_string(bdt_cuts[s])+"_"+f->tag+"_"+var1.safe_unit+"_"+var2.safe_unit, plot_pot);
                    padmc->cd();

                    mc->Draw("COLZ");
                    mc ->SetTitle((f->tag + ", stage " + std::to_string(s)).c_str());
                    mc->GetYaxis()->SetTitleSize(0.05);
                    mc->GetYaxis()->SetTitleOffset(0.9);
                    mc->GetXaxis()->SetTitleSize(0.05);
                    mc->GetXaxis()->SetTitleOffset(0.9);
                    padmc->SetRightMargin(0.15);
                    std::cout<<"Writing pdf."<<std::endl;
                    cobsmc->Write();
                    cobsmc->SaveAs(("var2D/"+tag+"_"+f->tag+"_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)+".pdf").c_str(),"pdf");


                    delete cobsmc;
                    delete mc;

                    }//for each item in the mc stack

                }//if different variables and haven't already used the combo

            }//var2
        }//var1
    }//stage

    return 0;
}



int bdt_datamc::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts){


    // NEW ONE
    double plot_pot=data_file->pot;
    if(stack_mode) plot_pot = stack_pot;

    std::cout<<"DATAMC PLOT POT "<<plot_pot<<std::endl;

    double title_size_ratio=0.1;
    double label_size_ratio=0.1;
    double title_offset_ratioY = 0.3 ;
    double title_offset_ratioX = 1.1;

    double title_size_upper=0.15;
    double label_size_upper=0.05;
    double title_offset_upper = 1.45;


    ftest->cd();

    std::vector<std::string> stage_names = {"Topological Selection","Pre-Selection Cuts","Cosmic BDT Cut","BNB BDT cut","NCPi0 BDT Cut","NUE BDT Cut","tmp"};
    //Loop over all stages

    int s_min = 0;
    int s_max = bdt_cuts.size()+2;
    if(plot_stage >=0){
        s_min = plot_stage;
        s_max = plot_stage+1;
    }

    for(int s = s_min; s< s_max; s++){


        std::cout<<"On stage: "<<s<<std::endl;
        //First set the files at this stage
        for(auto &f: mc_stack->stack){
            std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
            if(s>1) f->calcBDTEntryList(s,bdt_cuts);
            std::cout<<"Setting up EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
            f->setStageEntryList(s);
        }	

        std::cout<<"Done with computations on TTrees and bdt_stacks"<<std::endl;

        if(s>1) data_file->calcBDTEntryList(s,bdt_cuts);

        data_file->setStageEntryList(s);

        //And all variables in the vector var
        for(auto &var: vars){


            //var.is_logplot = true;


            std::cout<<"Starting on variable "<<var.name<<std::endl;
            TCanvas *cobs = new TCanvas(("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(),1800,1600);
            cobs->cd();

            if(false&&do_subtraction){
                std::cout<<"Setting do Subtraction inside bdt_stack "<<std::endl;
                mc_stack->setSubtractionVector(subtraction_vec);
            }

            THStack *stk = (THStack*)mc_stack->getEntryStack(var,s);
            TH1 * tsum = (TH1*)mc_stack->getEntrySum(var,s);
            TH1 * d0 = (TH1*)data_file->getTH1(var, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[s])+"_"+data_file->tag+"_"+var.safe_name, plot_pot);

            //Check Covar for plotting
            if(var.has_covar){
                TFile *covar_f = new TFile(var.covar_file.c_str(),"read");
                TMatrixD * covar_m = (TMatrixD*)covar_f->Get(var.covar_name.c_str());

                for(int c=0; c< tsum->GetNbinsX()+1;c++){
                    tsum->SetBinError(c+1, sqrt((*covar_m)(c,c)));
                    //tsum->SetBinError(c+1, 0.0001);
                }
                covar_f->Close();
            }
            cobs->cd();



            //            std::vector<double> ks_sum = data_file->getVector(var,s);

            double rmin = 0.5;
            double rmax = 1.5;
            int data_rebin = 1;
            if(s==0 || s == 1){
                rmin=0; rmax = 1.99;
            }//else if(s>2){ data_rebin = 2;}else if(s==3){data_rebin=2;};


            //tsum->Rebin(data_rebin);
            d0->Rebin(data_rebin);

            if(false &&do_subtraction){
                std::cout<<"Actually doing the subtracting"<<std::endl;
                for(int i=0; i< subtraction_vec.size();i++)
                    if(subtraction_vec[i]){
                        std::cout<<"Subtracting: "<<i<<std::endl;
                        mc_stack->vec_hists[i]->Rebin(data_rebin);
                        d0->Add((mc_stack->vec_hists[i]),-1.0);
                        tsum->Add((mc_stack->vec_hists[i]),-1.0);
                    }
            }


            std::cout<<"2 "<<std::endl;
            tsum->SetMarkerSize(0);
            d0->SetMarkerSize(2);
            gStyle->SetEndErrorSize(10);

            cobs->cd();
            TPad *pad0top = new TPad(("pad0top_"+stage_names.at(s)).c_str(), ("pad0top_"+stage_names.at(s)).c_str(), 0, 0.35, 1, 1.0);

            if(is_bdt_variable ) 
                pad0top->SetLogy();
            pad0top->SetBottomMargin(0); // Upper and lower plot are joined
            pad0top->Draw();             // Draw the upper pad: pad2top
            pad0top->cd();               // pad2top becomes the current pad

            //      double rmin = 0.5;
            //   	double rmax = 1.699;
            //	double rmin = 0;
            //	double rmax = 2.99;
            //	int data_rebin = 1;
            if(s==0 || s == 1){
                rmin=0; rmax = 1.99;
            }//else if(s==2){ data_rebin = 2;}else if(s==3){data_rebin=2;};

            //double max_modifier = 1.65;
            double max_modifier = 2.0;
            if (s==1){
                max_modifier = 1.85;
            }
            if (s==2){
                max_modifier = 2;
            }
            //if(s==3){
            //    max_modifier=4.3;
            // }

            if (s==3){
                max_modifier = (stack_mode ? 2.0 : 1.85);
            }


            if(var.is_logplot == true){
                //if(true){
                pad0top->SetLogy();
                max_modifier=100.0;
            }
            //     double max_modifier = 1.7;
            double min_val = 0.01;
            if(is_bdt_variable) {
                max_modifier = 15.0;
                //  max_modifier = 25.0;
                min_val = 0.1;
            }

            d0->SetMarkerStyle(20);
            d0->SetMarkerSize(3);
            d0->SetLineColor(kBlack);

            stk->SetMinimum(0.0001);
            stk->Draw("hist");
            stk->SetTitle("");
            //stk->SetTitle(stage_names.at(s).c_str());
            stk->GetXaxis()->SetTitle(var.unit.c_str());
            stk->GetYaxis()->SetTitle("Events");
            stk->GetYaxis()->SetTitleSize(0.05);
            stk->GetYaxis()->SetTitleOffset(0.9);
            stk->SetMaximum(std::max(tsum->GetMaximum(), (stack_mode ? -1 :d0->GetMaximum()))*max_modifier);
            //stk->SetMaximum(500);
            stk->SetMinimum(min_val);
            tsum->SetLineWidth(3);
            tsum->DrawCopy("Same E2");
            TH1 *tmp_tsum = (TH1*)tsum->Clone(("tmp_tsum"+std::to_string(s)).c_str());
            TH1 *tmp_tsum2 = (TH1*)tsum->Clone(("tmp_tsum2"+std::to_string(s)).c_str());

            tmp_tsum2->SetFillStyle(0);
            tmp_tsum2->DrawCopy("same hist");

            tsum->SetFillStyle(0);//vec_th1s.at(s)->Draw("hist same");
            TLegend *l0 = new TLegend(0.11,0.65,0.89,0.89);
            l0->SetNColumns(2);
            double NeventsStack = 0;
            int which_signal = 0;
            int n=0;
            for(auto &f: mc_stack->stack){

                double Nevents = f->GetEntries()*(plot_pot/f->pot)*f->scale_data;
                NeventsStack+=Nevents;
                auto h1 = new TH1F(("tmp"+stage_names.at(s)+var.safe_name+f->tag).c_str(),"TLegend Example",200,-10,10);
                h1->SetFillColor(f->col);
                h1->SetFillStyle(f->fillstyle);
                //if(mc_stack->signal_on_top[n]){
                //    h1->SetLineColor(f->col);
                //    h1->SetLineWidth(3);
                //}else{
                h1->SetLineColor(kBlack);
                //}
                std::string string_events = to_string_prec(Nevents,2);
                if(do_subtraction){
                    if(subtraction_vec[n]) string_events+=" Subtracted";
                }
                std::string leg_type = "f";   

                //if(mc_stack->signal_on_top[n]) leg_type = "l";
                //l0->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+string_events+"}").c_str(),"f");
                l0->AddEntry(h1,(f->plot_name+" "+string_events).c_str(),leg_type.c_str());
                //l0->AddEntry(h1,(f->plot_name).c_str(),"f");

                if(mc_stack->signal_on_top[n]) which_signal = n;
                n++;
            }
            //l0->AddEntry(tmp_tsum,"Flux & XS Systematics Error","f");
            l0->AddEntry(tmp_tsum,"MC Stats-Only Error","f");
            //			d0->Draw("same E1");

            std::cout<<"Binned KS-test: "<<var.name<<" "<<tsum->KolmogorovTest(d0)<<std::endl;
            std::cout<<"Binned Chi-test standard: "<<var.name<<" "<<tsum->Chi2Test(d0,"CHI2")<<std::endl;
            std::cout<<"Binned Chi-test: "<<var.name<<" "<<tsum->Chi2Test(d0,"UW CHI2")<<std::endl;
            std::cout<<"Binned Chi-test (rev): "<<var.name<<" "<<d0->Chi2Test(tsum,"UW CHI2")<<std::endl;

            double mychi =0;
            int ndof = 0;
            for(int p=0; p<d0->GetNbinsX();p++){
                double da = d0->GetBinContent(p+1);
                double bk = tsum->GetBinContent(p+1);

                if (da == 0 || bk ==0){
                    std::cout<<"ERROR mychi, for bin "<<p<<" n_data= "<<da<<" and n_mc= "<<bk<<std::endl;

                } else{

                    double da_err = sqrt(d0->GetBinContent(p+1));
                    double bk_err = tsum->GetBinError(p+1);
                    //std::cout<<da<<" "<<bk<<" "<<da_err<<" "<<bk_err<<std::endl;
                    double tk = pow(da-bk,2)/(da_err*da_err+bk_err*bk_err);
                    if(tk==tk){
                        mychi+=tk;
                        ndof++;
                    }
                }
            }
            std::cout<<"MyChi: "<<var.name<<" "<<mychi<<std::endl;

            //stk->SetMaximum( std::max(tsum->GetMaximum(), d0->GetMaximum()*max_modifier));

            double NdatEvents = data_file->GetEntries()*(plot_pot/data_file->pot )*data_file->scale_data;

            d0->SetBinErrorOption(TH1::kPoisson);
            if(!stack_mode) d0->Draw("same E1 E0");

            /////// Print resolution for diphoton mass ////////
            // First, find variables containing the string of interest
            std::string massSearch("Invariant");
            std::size_t found = var.unit.find(massSearch);

            // Fit Gaussian to that variable
            /*
            if (found != std::string::npos) {
                std::cout << "[BLARG] Getting diphoton width for " << var.unit << " stage " << std::to_string(s) << std::endl;
                TF1 *gausfit_data = new TF1("gausfit_data", "gaus");
                TF1 *gausfit_mc = new TF1("gausfit_mc", "gaus");
                TH1 *tmp_hist = stk->GetHistogram();
                double lowFit, highFit;
                double mass_data = 0., mass_err_data = 0;
                double mass_res_data = 0., mass_res_err_data = 0.;
                double mass_mc = 0., mass_err_mc = 0;
                double mass_res_mc = 0., mass_res_err_mc = 0.;
                // Fit range should be similar for data and MC
                lowFit = d0->GetXaxis()->GetBinLowEdge(1);
                highFit = d0->GetXaxis()->GetBinLowEdge(d0->GetNbinsX()+1);
                d0->Fit(gausfit_data, "lv", "", lowFit, highFit);
                tmp_hist->Fit(gausfit_data, "q", "", lowFit, highFit);
                std::cout << "[BLARG] tmp max = " << tmp_hist->GetMaximum() << std::endl;
                mass_data = gausfit_data->GetParameter(1);
                mass_err_data = gausfit_data->GetParError(1);
                mass_res_data = gausfit_data->GetParameter(2);
                mass_res_err_data = gausfit_data->GetParError(2);
                mass_mc = gausfit_mc->GetParameter(1);
                mass_err_mc = gausfit_mc->GetParError(1);
                mass_res_mc = gausfit_mc->GetParameter(2);
                mass_res_err_mc = gausfit_mc->GetParError(2);
                std::cout << "[BLARG] Data mass: " << mass_data << " +/- " << mass_err_data << std::endl;
                std::cout << "[BLARG] Data mass resolution: " << mass_res_data << " +/- " << mass_res_err_data << std::endl;
                std::cout << "[BLARG] MC mass: " << mass_mc << " +/- " << mass_err_mc << std::endl;
                std::cout << "[BLARG] MC mass resolution: " << mass_res_mc << " +/- " << mass_res_err_mc << std::endl;
                gausfit_data->SetLineColor(kRed);
                gausfit_mc->SetLineColor(kAzure+1);
                gausfit_data->Draw("same");
                gausfit_mc->Draw("same");
            }
            */


            // l0->AddEntry(d0,(data_file->plot_name).c_str(),"lp");	
            //l0->AddEntry(d0,("#splitline{"+data_file->plot_name+"}{"+to_string_prec(NdatEvents,2)+"}").c_str(),"lp");	
            if(!stack_mode) l0->AddEntry(d0,(data_file->plot_name+" "+to_string_prec(NdatEvents,2)).c_str(),"lp");	


            l0->Draw();
            l0->SetLineWidth(0);
            l0->SetLineColor(0);
            //l0->SetFillStyle(0); //comment in for transparent
            l0->SetTextSize(0.04);

            //  TLatex latex;
            // latex.SetTextSize(0.06);
            //  latex.SetTextAlign(13);  //align at top
            //  latex.SetNDC();
            //  latex.DrawLatex(.7,.71,data_file->topo_name.c_str());
            TLatex pottex;
            pottex.SetTextSize(0.06);
            pottex.SetTextAlign(13);  //align at top
            pottex.SetNDC();

            //double pot_unit = stack_mode ? 1e20 : 1e19;
            //std::string pot_unit_s = stack_mode ? "e20" : "e19";
            double pot_unit = 1e20;
            std::string pot_unit_s = "e20";
            std::string pot_draw = data_file->topo_name+" "+to_string_prec(plot_pot/pot_unit,1)+ pot_unit_s+" POT";

            pottex.DrawLatex(.60,.60, pot_draw.c_str());

            // Draw stage name. Added by A. Mogan 10/14/19
            TText *stage = drawPrelim(0.88, 0.92, stage_names.at(s) );
            stage->SetTextAlign(31); // Right-adjusted 
            stage->Draw();

            TText *pre; 
            if (isSpectator) {
                pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton - In Progress");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton - In Progress  [Spectator Variable]");
            }else {
                pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation ");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton In Progress [Training Variable]");

            }
            pre->Draw();

            /* TText *spec;
               if (isSpectator) {
               TText *spec = drawPrelim(0.82, 0.52, "Spectator Variable");
               spec->Draw("same");
               }
               */
            //cobs->cd(k+1);	
            cobs->cd();
            TPad *pad0bot = new TPad(("padbot_"+stage_names.at(s)).c_str(),("padbot_"+stage_names.at(s)).c_str(), 0, 0.05, 1, 0.35);
            pad0bot->SetTopMargin(0);
            pad0bot->SetBottomMargin(0.351);
            pad0bot->SetGridx(); // vertical grid
            pad0bot->Draw();
            pad0bot->cd();       // pad0bot becomes the current pad




            //tsum->Rebin(data_rebin);
            TH1* rat_denom = (TH1*)tsum->Clone(("ratio_denom_"+stage_names.at(s)).c_str());
            for(int i=0; i<rat_denom->GetNbinsX(); i++){
                rat_denom->SetBinError(i,0.0);
            }	

            TH1* ratunit = (TH1*)tsum->Clone(("ratio_unit_"+stage_names.at(s)).c_str());
            ratunit->Divide(rat_denom);		

            TH1 * signal_hist = mc_stack->vec_hists[which_signal];
            TH1* rat_signal = (TH1*)signal_hist->Clone(("ratio_signal_"+stage_names.at(s)).c_str());
            //            rat_signal->Add(tsum);
            //          rat_signal->Divide(tsum);
            for(int b=0; b< rat_signal->GetNbinsX()+1; b++){
                double val = (signal_hist->GetBinContent(b)+tsum->GetBinContent(b))/tsum->GetBinContent(b);
                std::cout<<b<<" "<<val<<" "<<tsum->GetBinContent(b)<<" "<<signal_hist->GetBinContent(b)<<std::endl;
                if(val !=val) val = 0;
                rat_signal->SetBinContent(b,val);
            }


            ratunit->SetFillColor(kGray+1);
            ratunit->SetMarkerStyle(0);
            ratunit->SetMarkerSize(0);
            //   ratunit->SetFillStyle(3001);
            ratunit->SetFillStyle(3354);
            //gStyle->SetHatchesLineWidth(1);
            //gStyle->SetHatchesSpacing(1);

            ratunit->Draw("E2");	

            rat_signal->SetFillStyle(0);
            rat_signal->SetLineColor(mc_stack->stack[which_signal]->col);
            rat_signal->SetLineWidth(2);
            //rat_signal->Draw("hist same");
            ratunit->DrawCopy("E2 same");	


            TLine *line = new TLine(ratunit->GetXaxis()->GetXmin(),1.0,ratunit->GetXaxis()->GetXmax(),1.0 );
            line->Draw("same");
            ratunit->SetLineColor(kBlack);
            ratunit->SetTitle("");
            //ratunit->GetYaxis()->SetTitle("Data/(MC+EXT)");
            ratunit->GetYaxis()->SetTitle(  (stack_mode ? "Signal +CV/CV" : "Data/(MC+Cosmic)"));
            ratunit->GetXaxis()->SetTitleOffset(title_offset_ratioX);
            ratunit->GetYaxis()->SetTitleOffset(title_offset_ratioY);
            ratunit->SetMinimum(rmin);	
            ratunit->SetMaximum(rmax);//ratunit->GetMaximum()*1.1);
            ratunit->GetYaxis()->SetTitleSize(title_size_ratio);
            ratunit->GetXaxis()->SetTitleSize(title_size_ratio);
            ratunit->GetYaxis()->SetLabelSize(label_size_ratio);
            ratunit->GetXaxis()->SetLabelSize(label_size_ratio);
            ratunit->GetXaxis()->SetTitle(var.unit.c_str());
            ratunit->GetYaxis()->SetNdivisions(505);

            TH1* ratpre = (TH1*)d0->Clone(("ratio_"+stage_names.at(s)).c_str());
            ratpre->Divide(rat_denom);		


            std::vector<double> x;
            std::vector<double> y;

            std::vector<double> err_x_left;
            std::vector<double> err_x_right;
            std::vector<double> err_y_high;
            std::vector<double> err_y_low;


            for(int b=1; b<d0->GetNbinsX()+1;b++){
                double is_zero = rat_denom->GetBinContent(b);
                if(is_zero!=0.0){
                    y.push_back(d0->GetBinContent(b)/is_zero);
                    x.push_back(d0->GetBinCenter(b));
                    err_x_left.push_back(d0->GetBinWidth(b)/2.0);
                    err_x_right.push_back(d0->GetBinWidth(b)/2.0);
                    err_y_high.push_back((d0->GetBinErrorUp(b))/is_zero);
                    err_y_low.push_back((d0->GetBinErrorLow(b))/is_zero);

                }

            }

            //TGraphAsymmErrors * gr = new TGraphAsymmErrors(x.size(),&x[0],&y[0],&err_x_left[0],&err_x_right[0],&err_y_high[0],&err_y_low[0]);
            TGraphAsymmErrors * gr = new TGraphAsymmErrors(x.size(),&x[0],&y[0],&err_x_left[0],&err_x_right[0],&err_y_low[0],&err_y_high[0]);

            //gr->Divide(d0,tsum,"pois");
            //gr->Divide(d0,rat_denom,"pois");

            gr->SetLineWidth(1);


            ratpre->SetLineColor(kBlack);
            ratpre->SetTitle("");


            //gr->Divide(d0,tsum,"pois");
            //gr->Divide(d0,rat_denom,"pois");

            gr->SetLineWidth(1);

            ratpre->SetFillColor(kGray+1);
            ratpre->SetMarkerStyle(20);
            ratpre->SetMarkerSize(ratpre->GetMarkerSize()*0.7);

            ratpre->SetFillStyle(3144);
            if(!stack_mode){
                ratpre->Draw("same P E0 hist");	
                gr->Draw("E1 same");
            }

            //std::string mean = "(Ratio: "+to_string_prec(NdatEvents/NeventsStack,2)+"/"+to_string_prec(d0->Integral()/tsum->Integral() ,2)+")" ;
            std::string mean = "(Data/MC: "+to_string_prec(NdatEvents/NeventsStack,2)+")";//+"/"+to_string_prec(d0->Integral()/tsum->Integral() ,2)+")" ;
            std::string ks = "(KS: "+to_string_prec(tsum->KolmogorovTest(d0),4) + ")     (#chi^{2}/n#it{DOF}: "+to_string_prec(mychi,2) + "/"+to_string_prec(ndof) +")";

            std::string combined = mean + "     " +ks;
            //std::string mean = "Ratio: Normalized" ;
            TLatex *t = new TLatex(0.11,0.02,combined.c_str());
            //   TLatex *t = new TLatex(0.11,0.41,ks.c_str());
            t->SetNDC();
            t->SetTextColor(kRed-7);
            //t->SetTextFont(43);
            t->SetTextSize(0.10);
            if(!stack_mode)t->Draw("same");

            //var_precut.front()->GetYaxis()->SetRangeUser(0.1,ymax_pre);
            //var_precut.front()->GetYaxis()->SetTitle("Events");



            std::cout<<"Writing pdf."<<std::endl;
            cobs->Write();
            if(stack_mode){
                cobs->SaveAs(("stack/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".pdf").c_str(),"pdf");
            }else{
                cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".pdf").c_str(),"pdf");
            }
            //cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".png").c_str(),"png");




            delete cobs;
            delete stk;
            delete tsum;
            delete d0;
            delete ratunit;
            delete ratpre;
            delete rat_denom;			


            }
        }

        return 0;
    }




    int bdt_datamc::plotBDTStacks(bdt_info info, std::vector<double> bdt_cuts){
        // NEW ONE (for BDT only) stop measure
        double plot_pot=data_file->pot;
        std::cout<<"DATAMC PLOT POT "<<plot_pot<<std::endl;



        double title_size_ratio=0.1;
        double label_size_ratio=0.1;
        double title_offset_ratioY = 0.3 ;
        double title_offset_ratioX = 1.1;

        double title_size_upper=0.15;
        double label_size_upper=0.05;
        double title_offset_upper = 1.45;

        is_bdt_variable = true;

        std::vector<std::string> stage_names = {"Topological Selection","Pre-Selection Cuts","Cosmic BDT Cut","BNB BDT cut","NCPI","NUE","tmp"};
        for(int i= stage_names.size(); i< bdt_cuts.size(); i++){
            stage_names.push_back("Stage: "+std::to_string(i));
        }


        //Loop over all stages, currently just looking at 1
        //for(int s = 1; s< bdt_cuts.size()+2; s++){

        {int s=1;
            std::cout<<"On stage: "<<s<<std::endl;
            //First set the files at this stage
            for(auto &f: mc_stack->stack){
                std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
                //              ********
                //  will add a generic calc enerty lists here
                if(s>1){
                    f->calcBDTEntryList(s,bdt_cuts);
                }
                std::cout<<"Setting up EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
                f->setStageEntryList(s);
            }	

            std::cout<<"Done with computations on TTrees and bdt_stacks"<<std::endl;



            //generic calc enrtylist here too
            if(s>1){
                data_file->calcBDTEntryList(s,bdt_cuts);
            }
            data_file->setStageEntryList(s);

            std::string scuts="";
            for(auto &s: bdt_cuts) scuts +="_"+std::to_string(s);

            //One variable.


            std::cout<<"Starting on variable "<<info.identifier<<std::endl;
            TCanvas *cobs = new TCanvas(("can_"+info.identifier+"_stage_"+std::to_string(s)).c_str(),("can_"+info.identifier+"_stage_"+std::to_string(s)).c_str(),1800,1600);
            cobs->cd();

            if(false&&do_subtraction){
                std::cout<<"Setting do Subtraction inside bdt_stack "<<std::endl;
                mc_stack->setSubtractionVector(subtraction_vec);
            }

            //gonna write these
            THStack *stk = (THStack*)mc_stack->getBDTEntryStack(info);
            TH1 * tsum = (TH1*)mc_stack->getBDTEntrySum(info);

            bdt_variable dvar = data_file->getBDTVariable(info);
            dvar.is_logplot = true;
            TH1 * d0 = (TH1*)data_file->getTH1(dvar, "1", scuts+"_"+data_file->tag+"_"+dvar.safe_name, plot_pot);

            double rmin = 0.5;
            double rmax = 1.699;
            int data_rebin = 1;
            if(s==0 || s == 1){
                rmin=0.0; rmax = 1.999;
            }//else if(s==2){ data_rebin = 2;}else if(s==3){data_rebin=2;};


            double max_modifier = stack_mode ? 1.4 : 1.9;
            double min_val = 0.01;
            if(is_bdt_variable) {
                max_modifier = 50.0;
                //min_val = 0.01;
                min_val = 0.1; // Changed from 0.01 to 0.1 by A. Mogan, 10/22/19, for collab meeting
            }
            d0->Rebin(data_rebin);

            if(false &&do_subtraction){
                std::cout<<"Actually doing the subtracting"<<std::endl;
                for(int i=0; i< subtraction_vec.size();i++)
                    if(subtraction_vec[i]){
                        std::cout<<"Subtracting: "<<i<<std::endl;
                        mc_stack->vec_hists[i]->Rebin(data_rebin);
                        d0->Add((mc_stack->vec_hists[i]),-1.0);
                        tsum->Add((mc_stack->vec_hists[i]),-1.0);
                    }
            }


            std::cout<<"2 "<<std::endl;
            tsum->SetMarkerSize(0);
            d0->SetMarkerSize(2);
            gStyle->SetEndErrorSize(10);

            cobs->cd();
            TPad *pad0top = new TPad(("pad0top_"+stage_names.at(s)).c_str(), ("pad0top_"+stage_names.at(s)).c_str(), 0, 0.35, 1, 1.0);

            if(is_bdt_variable ) pad0top->SetLogy();
            pad0top->SetBottomMargin(0); // Upper and lower plot are joined
            pad0top->Draw();             // Draw the upper pad: pad2top
            pad0top->cd();               // pad2top becomes the current pad


            d0->SetMarkerStyle(20);
            d0->SetMarkerSize(2);
            d0->SetLineColor(kBlack);

            stk->Draw("hist");
            stk->SetTitle("");
            //stk->SetTitle(stage_names.at(s).c_str());
            stk->GetXaxis()->SetTitle(dvar.unit.c_str());
            stk->GetYaxis()->SetTitle("Events");
            stk->GetYaxis()->SetTitleSize(0.05);
            stk->GetYaxis()->SetTitleOffset(0.9);
            stk->SetMaximum( std::max(tsum->GetMaximum(), (stack_mode ? -1 : d0->GetMaximum()) )*max_modifier);

            stk->SetMinimum(min_val);
            tsum->DrawCopy("Same E2");
            TH1 *tmp_tsum = (TH1*)tsum->Clone(("tmp_tsum"+std::to_string(s)).c_str());
            tsum->SetFillStyle(0);//vec_th1s.at(s)->Draw("hist same");

            TLegend *l0 = new TLegend(0.11,0.65,0.89,0.89);
            l0->SetNColumns(2);
            double NeventsStack = 0;


            int n=0;
            for(auto &f: mc_stack->stack){

                double Nevents = f->GetEntries()*(plot_pot/f->pot)*f->scale_data;
                NeventsStack+=Nevents;
                auto h1 = new TH1F(("tmp"+stage_names.at(s)+dvar.safe_name+f->tag).c_str(),"TLegend Example",200,-10,10);
                h1->SetFillColor(f->col);
                h1->SetFillStyle(f->fillstyle);
                h1->SetLineColor(kBlack);
                std::string string_events = to_string_prec(Nevents,2);
                if(do_subtraction){
                    if(subtraction_vec[n]) string_events+=" Subtracted";
                }
                //l0->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+string_events+"}").c_str(),"f");
                l0->AddEntry(h1,(f->plot_name+" "+string_events).c_str(),"f");
                //l0->AddEntry(h1,(f->plot_name).c_str(),"f");
                n++;
            }


            l0->AddEntry(tmp_tsum,"MC Stats Only Error","f");

            std::cout<<"KSTEST: "<<dvar.name<<" "<<tsum->KolmogorovTest(d0)<<std::endl;


            stk->SetMaximum( std::max(tsum->GetMaximum(), (stack_mode ? -1 : d0->GetMaximum()) )*max_modifier);

            double NdatEvents = data_file->GetEntries()*(plot_pot/data_file->pot )*data_file->scale_data;

            d0->SetBinErrorOption(TH1::kPoisson);
            d0->Draw("same E1 E0");

            // l0->AddEntry(d0,(data_file->plot_name).c_str(),"lp");	
            //l0->AddEntry(d0,("#splitline{"+data_file->plot_name+"}{"+to_string_prec(NdatEvents,2)+"}").c_str(),"lp");	
            l0->AddEntry(d0,(data_file->plot_name+" "+to_string_prec(NdatEvents,2)).c_str(),"lp");	


            l0->Draw();
            l0->SetLineWidth(0);
            l0->SetLineColor(0);
            l0->SetFillStyle(0);
            l0->SetTextSize(0.04);

            //    tsum->DrawCopy("Same E2");

            //  TLatex latex;
            // latex.SetTextSize(0.06);
            //  latex.SetTextAlign(13);  //align at top
            //  latex.SetNDC();
            //  latex.DrawLatex(.7,.71,data_file->topo_name.c_str());
            TLatex pottex;
            pottex.SetTextSize(0.06);
            pottex.SetTextAlign(13);  //align at top
            pottex.SetNDC();

            //double pot_unit = stack_mode ? 1e20 : 1e19;
            //std::string pot_unit_s = stack_mode ? "e20" : "e19";
            double pot_unit = 1e20;
            std::string pot_unit_s = "e20";
            std::string pot_draw = data_file->topo_name+" "+to_string_prec(plot_pot/pot_unit,1)+ pot_unit_s+" POT";

            pottex.DrawLatex(.60,.64, pot_draw.c_str());

            TText *pre; 
            if (isSpectator) {
                pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton - In Progress");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton - In Progress  [Spectator Variable]");
            }else {
                pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton - In Progress");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton In Progress [Training Variable]");

            }
            pre->Draw();

            /* TText *spec;
               if (isSpectator) {
               TText *spec = drawPrelim(0.82, 0.52, "Spectator Variable");
               spec->Draw("same");
               }
               */
            //cobs->cd(k+1);	
            cobs->cd();
            TPad *pad0bot = new TPad(("padbot_"+stage_names.at(s)).c_str(),("padbot_"+stage_names.at(s)).c_str(), 0, 0.05, 1, 0.35);
            pad0bot->SetTopMargin(0);
            pad0bot->SetBottomMargin(0.351);
            pad0bot->SetGridx(); // vertical grid
            pad0bot->Draw();
            pad0bot->cd();       // pad0bot becomes the current pad




            //tsum->Rebin(data_rebin);
            TH1* rat_denom = (TH1*)tsum->Clone(("ratio_denom_"+stage_names.at(s)).c_str());
            for(int i=0; i<rat_denom->GetNbinsX(); i++){
                rat_denom->SetBinError(i,0.0);
            }	

            TH1* ratunit = (TH1*)tsum->Clone(("ratio_unit_"+stage_names.at(s)).c_str());
            ratunit->Divide(rat_denom);		




            ratunit->SetFillColor(kGray+1);
            ratunit->SetMarkerStyle(0);
            ratunit->SetMarkerSize(0);
            //    ratunit->SetFillStyle(3001);
            ratunit->SetFillStyle(3354);
            //gStyle->SetHatchesLineWidth(1);
            //gStyle->SetHatchesSpacing(1);

            ratunit->Draw("E2");	

            TLine *line = new TLine(ratunit->GetXaxis()->GetXmin(),1.0,ratunit->GetXaxis()->GetXmax(),1.0 );
            line->Draw("same");
            ratunit->SetLineColor(kBlack);
            ratunit->SetTitle("");
            //ratunit->GetYaxis()->SetTitle("Data/(MC+EXT)");
            ratunit->GetYaxis()->SetTitle(  (stack_mode ? "Signal/MC+Cosmic" : "Data/(MC+Cosmic)"));
            ratunit->GetXaxis()->SetTitleOffset(title_offset_ratioX);
            ratunit->GetYaxis()->SetTitleOffset(title_offset_ratioY);
            ratunit->SetMinimum(rmin);	
            ratunit->SetMaximum(rmax);//ratunit->GetMaximum()*1.1);
            ratunit->GetYaxis()->SetTitleSize(title_size_ratio);
            ratunit->GetXaxis()->SetTitleSize(title_size_ratio);
            ratunit->GetYaxis()->SetLabelSize(label_size_ratio);
            ratunit->GetXaxis()->SetLabelSize(label_size_ratio);
            ratunit->GetXaxis()->SetTitle(dvar.unit.c_str());
            ratunit->GetYaxis()->SetNdivisions(505);

            TH1* ratpre = (TH1*)d0->Clone(("ratio_"+stage_names.at(s)).c_str());


            std::vector<double> x;
            std::vector<double> y;

            std::vector<double> err_x_left;
            std::vector<double> err_x_right;
            std::vector<double> err_y_high;
            std::vector<double> err_y_low;


            for(int b=1; b<d0->GetNbinsX()+1;b++){
                double is_zero = rat_denom->GetBinContent(b);
                if(is_zero!=0.0){
                    y.push_back(d0->GetBinContent(b)/is_zero);
                    x.push_back(d0->GetBinCenter(b));
                    err_x_left.push_back(d0->GetBinWidth(b)/2.0);
                    err_x_right.push_back(d0->GetBinWidth(b)/2.0);
                    err_y_high.push_back((d0->GetBinErrorUp(b))/is_zero);
                    err_y_low.push_back((d0->GetBinErrorLow(b))/is_zero);

                }

            }

            //TGraphAsymmErrors * gr = new TGraphAsymmErrors(x.size(),&x[0],&y[0],&err_x_left[0],&err_x_right[0],&err_y_high[0],&err_y_low[0]);
            TGraphAsymmErrors * gr = new TGraphAsymmErrors(x.size(),&x[0],&y[0],&err_x_left[0],&err_x_right[0],&err_y_low[0],&err_y_high[0]);

            //gr->Divide(d0,tsum,"pois");
            //gr->Divide(d0,rat_denom,"pois");

            gr->SetLineWidth(1);
            ratpre->Divide(rat_denom);		

            ratpre->SetFillColor(kGray+1);
            ratpre->SetMarkerStyle(20);
            ratpre->SetMarkerSize(ratpre->GetMarkerSize()*0.7);

            ratpre->SetFillStyle(3144);
            //ratpre->SetFillColor(kGray + 3);
            ratpre->Draw("same P0 hist");	
            gr->Draw("E1 same");

            ratpre->SetLineColor(kBlack);
            ratpre->SetTitle("");

            double mychi =0;
            int ndof = 0;
            for(int p=0; p<d0->GetNbinsX();p++){
                double da = d0->GetBinContent(p+1);
                double bk = tsum->GetBinContent(p+1);

                if (da == 0 || bk ==0){
                    std::cout<<"ERROR mychi, for bin "<<p<<" n_data= "<<da<<" and n_mc= "<<bk<<std::endl;

                } else{

                    double da_err = sqrt(d0->GetBinContent(p+1));
                    double bk_err = tsum->GetBinError(p+1);
                    //std::cout<<da<<" "<<bk<<" "<<da_err<<" "<<bk_err<<std::endl;
                    double tk = pow(da-bk,2)/(da_err*da_err+bk_err*bk_err);
                    if(tk==tk){
                        mychi+=tk;
                        ndof++;
                    }
                }
            }


            std::string ks = "(KS: "+to_string_prec(tsum->KolmogorovTest(d0)) + ")     (#chi^{2}/n#it{DOF}: "+to_string_prec(mychi,2) + "/"+to_string_prec(ndof) +")";
            std::string mean = "Ratio: "+to_string_prec(NdatEvents/NeventsStack,2)+" / "+to_string_prec(d0->Integral()/tsum->Integral() ,2); ;
            //std::string mean = "Ratio: Normalized" ;
            TLatex *t = new TLatex(0.11,0.41,ks.c_str());
            t->SetNDC();
            t->SetTextColor(kRed-7);
            //t->SetTextFont(43);
            t->SetTextSize(0.10);
            t->Draw("same");



            std::cout<<"Writing pdf."<<std::endl;
            cobs->Write();
            cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+info.identifier+"_"+dvar.safe_unit+"_stage_"+std::to_string(s)+".pdf").c_str(),"pdf");
            //cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".png").c_str(),"png");


            delete cobs;
            delete stk;
            delete tsum;
            delete d0;
            delete ratunit;
            delete ratpre;
            delete rat_denom;			


        }

        return 0;
    }





    int bdt_datamc::plotStacks(TFile *ftest, bdt_variable var,double c1, double c2, bdt_info whichbdt){

        //TCanvas *cobs = new TCanvas("","",1800,1600);
        //cobs->Divide(2,2,0.0025,0.0000001);

        double plot_pot=data_file->pot;

        double title_size_ratio=0.1;
        double label_size_ratio=0.1;
        double title_offset_ratioY = 0.3 ;
        double title_offset_ratioX = 1.1;

        double title_size_upper=0.15;
        double label_size_upper=0.05;
        //double label_size_upper=0.1;
        double title_offset_upper = 1.45;


        ftest->cd();

        THStack* s0;// mc_stack->getStack(var,0,-9,-9);
        THStack* s1;// = mc_stack->getStack(var,1,-9,-9);
        THStack* s2;// = mc_stack->getStack(var,2,c1,-9);
        THStack* s3;// = mc_stack->getStack(var,3,c1, c2);

        TH1* sh0;// = mc_stack->getSum(var,0,-9,-9);
        TH1* sh1;// = mc_stack->getSum(var,1,-9,-9);
        TH1* sh2;// = mc_stack->getSum(var,2,c1,-9);
        TH1* sh3;// = mc_stack->getSum(var,3,c1, c2);


        if(is_bdt_variable){
            s0 = mc_stack->getBDTStack(whichbdt,var.binning,0,-9,-9);
            s1 = mc_stack->getBDTStack(whichbdt,var.binning,1,-9,-9);
            s2 = mc_stack->getBDTStack(whichbdt,var.binning,2,c1,-9);
            s3 = mc_stack->getBDTStack(whichbdt,var.binning,3,c1, c2);

            sh0 = mc_stack->getBDTSum(whichbdt,var.binning,0,-9,-9);
            sh1 = mc_stack->getBDTSum(whichbdt,var.binning,1,-9,-9);
            sh2 = mc_stack->getBDTSum(whichbdt,var.binning,2,c1,-9);
            sh3 = mc_stack->getBDTSum(whichbdt,var.binning,3,c1, c2);


        }else{
            s0 = mc_stack->getStack(var,0,-9,-9);
            s1 = mc_stack->getStack(var,1,-9,-9);
            s2 = mc_stack->getStack(var,2,c1,-9);
            s3 = mc_stack->getStack(var,3,c1, c2);

            std::cout<<"Gotten all stacks."<<std::endl;
            sh0 = mc_stack->getSum(var,0,-9,-9);
            sh1 = mc_stack->getSum(var,1,-9,-9);
            sh2 = mc_stack->getSum(var,2,c1,-9);
            sh3 = mc_stack->getSum(var,3,c1, c2);
        }


        sh0->SetMarkerSize(0);
        sh1->SetMarkerSize(0);
        sh2->SetMarkerSize(0);
        sh3->SetMarkerSize(0);

        std::string dat_cut_0 =	data_file->getStageCuts(0, -9, -9);
        std::string dat_cut_1 =	data_file->getStageCuts(1, -9,-9);
        std::string dat_cut_2 =	data_file->getStageCuts(2, c1, -9);
        std::string dat_cut_3 =	data_file->getStageCuts(3, c1, c2);

        std::cout<<dat_cut_1<<" CUTPRE"<<std::endl;
        TH1 * d0 = data_file->getTH1(var, dat_cut_0, "d0_"+data_file->tag+"_"+var.safe_name, plot_pot);
        TH1 * d1 = data_file->getTH1(var, dat_cut_1, "d1_"+data_file->tag+"_"+var.safe_name, plot_pot);
        TH1 * d2 = data_file->getTH1(var, dat_cut_2, "d2_"+data_file->tag+"_"+var.safe_name, plot_pot);
        TH1 * d3 = data_file->getTH1(var, dat_cut_3, "d3_"+data_file->tag+"_"+var.safe_name, plot_pot);

        std::cout<<"Gotten all data hists."<<std::endl;


        std::vector<THStack*> vec_stacks = {s0,s1,s2,s3};	
        std::vector<TH1*> vec_th1s = {sh0,sh1,sh2,sh3};	
        std::vector<std::string> data_cuts = {dat_cut_0, dat_cut_1, dat_cut_2, dat_cut_3};
        std::vector<TH1*> data_th1s = {d0,d1,d2,d3};
        std::vector<std::string> stage_name = {"Topological Selection","Pre-Selection Cuts","Cosmic BDT Cut","BNB BDT Cut"};
        //std::vector<std::string> stage_name = {"Topological Selection","","Cosmic BDT Cut","BNB BDT Cut"};


        for(int k = 1; k<2; k++){
            std::cout << "[PLOTBDTSTACKS:] On stage " << k << std::endl;
            TCanvas *cobs = new TCanvas("","",900,800);
            //cobs->cd(k+1);
            cobs->cd();
            TPad *pad0top = new TPad(("pad0top_"+stage_name.at(k)).c_str(), ("pad0top_"+stage_name.at(k)).c_str(), 0, 0.35, 1, 1.0);


            if(var.is_logplot || is_bdt_variable) pad0top->SetLogy();
            pad0top->SetBottomMargin(0); // Upper and lower plot are joined
            pad0top->Draw();             // Draw the upper pad: pad2top
            pad0top->cd();               // pad2top becomes the current pad

            //double rmin = 0;
            //double rmax = 2.99;
            double rmin = 0.5;
            double rmax = 1.699;
            int data_rebin = 1;
            if(k==0 || k == 1){
                rmin=0.0; rmax = 1.999;

            };//else if(k==2){ data_rebin = 2;}else if(k==3){data_rebin=2;};


            double max_modifier = 1.9;
            //double min_val = 0.01;
            double min_val = 0.01;
            if(is_bdt_variable || var.is_logplot) {
                //max_modifier = 500.0;
                 max_modifier = 50.0;
                min_val = 0.1;
            }

            vec_stacks.at(k)->SetMaximum(vec_th1s.at(k)->GetMaximum()*1.4);
            vec_stacks.at(k)->SetMinimum(0.00001);
            vec_stacks.at(k)->Draw("hist");
            //vec_stacks.at(k)->SetTitle(stage_name.at(k).c_str());
            vec_stacks.at(k)->SetTitle("");
            vec_stacks.at(k)->GetXaxis()->SetTitle(var.unit.c_str());
            vec_stacks.at(k)->GetYaxis()->SetTitle("Events");
            // vec_stacks.at(k)->GetYaxis()->SetTitleSize(0.05);
            vec_stacks.at(k)->GetYaxis()->SetTitleSize(0.1);
            vec_stacks.at(k)->GetYaxis()->SetTitleOffset(0.9);
            vec_stacks.at(k)->GetYaxis()->SetLabelSize(label_size_upper);
            vec_stacks.at(k)->SetMaximum( std::max(vec_th1s.at(k)->GetMaximum(), data_th1s.at(k)->GetMaximum())*max_modifier);
            vec_stacks.at(k)->SetMinimum(min_val);
            vec_th1s.at(k)->DrawCopy("Same E2"); vec_th1s.at(k)->SetFillStyle(0);//vec_th1s.at(k)->Draw("hist same");


            TLegend *l0 = new TLegend(0.11,0.62,0.89,0.89);
            l0->SetNColumns(2);
            double NeventsStack = 0;

            for(auto &f: mc_stack->stack){
                double Nevents = f->GetEntries(f->getStageCuts(k,c1,c2).c_str())*(plot_pot/f->pot )*f->scale_data;
                NeventsStack+=Nevents;
                auto h1 = new TH1F(("tmp"+stage_name.at(k)+var.safe_name+f->tag).c_str(),"TLegend Example",200,-10,10);
                h1->SetFillColor(f->col);
                h1->SetFillStyle(f->fillstyle);
                h1->SetLineColor(kBlack);
                //l0->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+to_string_prec(Nevents,2)+"}").c_str(),"f");
                l0->AddEntry(h1,(f->plot_name+" "+to_string_prec(Nevents,2)).c_str(),"f");

                //l0->AddEntry(h1,(f->plot_name.c_str()),"f");
            }

            data_th1s.at(k)->Rebin(data_rebin);
            data_th1s.at(k)->SetMarkerStyle(20);
            data_th1s.at(k)->SetLineColor(kBlack);
            data_th1s.at(k)->Draw("same E1");

            vec_stacks.at(k)->SetMaximum( std::max(vec_th1s.at(k)->GetMaximum(), data_th1s.at(k)->GetMaximum()*max_modifier));

            double NdatEvents = data_file->GetEntries(data_cuts.at(k).c_str())*(plot_pot/data_file->pot )*data_file->scale_data;

            l0->AddEntry(data_th1s.at(k),(data_file->plot_name+"_"+to_string_prec(NdatEvents,2)).c_str(),"lp");	
            //l0->AddEntry(data_th1s.at(k),("#splitline{"+data_file->plot_name+"}{"+to_string_prec(NdatEvents,2)+"}").c_str(),"lp");	

            l0->Draw();
            l0->SetLineWidth(0);
            l0->SetLineColor(0);
            l0->SetFillStyle(0);
            l0->SetTextSize(0.04);

            //  TLatex latex;
            // latex.SetTextSize(0.06);
            //  latex.SetTextAlign(13);  //align at top
            //  latex.SetNDC();
            //  latex.DrawLatex(.7,.71,data_file->topo_name.c_str());
            TLatex pottex;
            pottex.SetTextSize(0.06);
            pottex.SetTextAlign(13);  //align at top
            pottex.SetNDC();
            std::string pot_draw = data_file->topo_name+" "+to_string_prec(plot_pot/1e19,1)+"e19 POT";

            pottex.DrawLatex(.7,.96, pot_draw.c_str());

            TText *pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation");

            pre->Draw();

            // TText *pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation - In Progress");
            // pre->Draw();

            //cobs->cd(k+1);	
            cobs->cd();
            TPad *pad0bot = new TPad(("padbot_"+stage_name.at(k)).c_str(),("padbot_"+stage_name.at(k)).c_str(), 0, 0.05, 1, 0.35);
            pad0bot->SetTopMargin(0);
            pad0bot->SetBottomMargin(0.351);
            pad0bot->SetGridx(); // vertical grid
            pad0bot->Draw();
            pad0bot->cd();       // pad0bot becomes the current pad
            //        TText *pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation - In Progress");


            vec_th1s.at(k)->Rebin(data_rebin);
            TH1* rat_denom = (TH1*)vec_th1s.at(k)->Clone(("ratio_denom_"+stage_name.at(k)).c_str());
            for(int i=0; i<rat_denom->GetNbinsX(); i++){
                rat_denom->SetBinError(i,0.0);
            }	

            TH1* ratunit = (TH1*)vec_th1s.at(k)->Clone(("ratio_unit_"+stage_name.at(k)).c_str());
            ratunit->Divide(rat_denom);		

            ratunit->SetFillColor(kGray+3);
            ratunit->SetMarkerStyle(0);
            ratunit->SetMarkerSize(0);
            ratunit->SetFillStyle(3001);
            ratunit->Draw("E2");	

            TLine *line = new TLine(ratunit->GetXaxis()->GetXmin(),1.0,ratunit->GetXaxis()->GetXmax(),1.0 );
            line->Draw("same");
            ratunit->SetLineColor(kBlack);
            ratunit->SetTitle("");
            //ratunit->GetYaxis()->SetTitle("Data/(MC+EXT)");
            ratunit->GetYaxis()->SetTitle(  (stack_mode ? "Signal/MC+Cosmic" : "Data/(MC+Cosmic)"));
            ratunit->GetXaxis()->SetTitleOffset(title_offset_ratioX);
            ratunit->GetYaxis()->SetTitleOffset(title_offset_ratioY);
            ratunit->SetMinimum(rmin);	
            ratunit->SetMaximum(rmax);//ratunit->GetMaximum()*1.1);
            ratunit->GetYaxis()->SetTitleSize(title_size_ratio);
            ratunit->GetXaxis()->SetTitleSize(title_size_ratio);
            ratunit->GetYaxis()->SetLabelSize(label_size_ratio);
            ratunit->GetXaxis()->SetLabelSize(label_size_ratio);
            ratunit->GetXaxis()->SetTitle(var.unit.c_str());
            ratunit->GetYaxis()->SetNdivisions(505);

            TH1* ratpre = (TH1*)data_th1s.at(k)->Clone(("ratio_"+stage_name.at(k)).c_str());
            ratpre->Divide(rat_denom);		

            ratpre->SetFillColor(kGray+1);
            ratpre->SetMarkerStyle(20);
            ratpre->SetMarkerSize(ratpre->GetMarkerSize()*0.7);

            // ratpre->SetFillStyle(3144);
            ratpre->SetFillColor(kGray + 3);
            ratpre->SetFillStyle(3354);
            gStyle->SetHatchesLineWidth(2);
            gStyle->SetHatchesSpacing(1);

            ratpre->Draw("E1 same");	

            ratpre->SetLineColor(kBlack);
            ratpre->SetTitle("");


            double mychi =0;
            int ndof = 0;
            for(int p=0; p<data_th1s.at(k)->GetNbinsX();p++){
                double da = data_th1s.at(k)->GetBinContent(p+1);
                double bk = vec_th1s.at(k)->GetBinContent(p+1);

                if (da == 0 || bk ==0){
                    std::cout<<"ERROR mychi, for bin "<<p<<" n_data= "<<da<<" and n_mc= "<<bk<<std::endl;

                } else{

                    double da_err = sqrt(data_th1s.at(k)->GetBinContent(p+1));
                    double bk_err = vec_th1s.at(k)->GetBinError(p+1);
                    //std::cout<<da<<" "<<bk<<" "<<da_err<<" "<<bk_err<<std::endl;
                    double tk = pow(da-bk,2)/(da_err*da_err+bk_err*bk_err);
                    if(tk==tk){
                        mychi+=tk;
                        ndof++;
                    }
                }
            }


            std::string mean = "Ratio: "+to_string_prec(NdatEvents/NeventsStack,2) ;
            std::string ks = "(KS: "+to_string_prec(vec_th1s.at(k)->KolmogorovTest(data_th1s.at(k))) + ")     (#chi^{2}/n#it{DOF}: "+to_string_prec(mychi,2) + "/"+to_string_prec(ndof) +")";

            TLatex *t = new TLatex(0.11,0.41,ks.c_str());
            t->SetNDC();
            t->SetTextColor(kRed-7);
            //t->SetTextFont(43);
            t->SetTextSize(0.10);
            //t->Draw("same");

            //var_precut.front()->GetYaxis()->SetRangeUser(0.1,ymax_pre);
            //var_precut.front()->GetYaxis()->SetTitle("Verticies");



            std::cout<<"Writing pdf."<<std::endl;
            cobs->Write();
            if(is_bdt_variable){
                cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_BDTVAR_"+whichbdt.identifier+"_stage_"+std::to_string(k)+".pdf").c_str(),"pdf");
            }else{
                cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(k)+".pdf").c_str(),"pdf");
            }

            //if(is_bdt_variable) return 0;
        }


        return 0;
    }

    int bdt_datamc::SetSpectator() {
        this->isSpectator = true;
        return 0;
    }

int bdt_datamc::printPassingPi0DataEvents(std::string outfilename, int stage, std::vector<double> cuts){

    //data_file->calcCosmicBDTEntryList(c1, c2);
    //data_file->calcBNBBDTEntryList(c1, c2);
    //data_file->setStageEntryList(3);

    //   std::string fake = "fake_bnbbdt_list_"+std::to_string(c1)+"_"+std::to_string(c2)+"_" +data_file->tag;

    // data_file=  mc_stack->stack[0];
    std::string fake = "";
    data_file->tvertex->Draw((">>"+fake).c_str(), data_file->getStageCuts(stage,cuts).c_str() , "entrylist");
    TEntryList * fake_list = (TEntryList*)gDirectory->Get(fake.c_str());

    int n_run_number = 0;
    int n_subrun_number = 0;
    int n_event_number = 0;
    double n_vertex_x = 0;
    double n_vertex_y = 0;
    double n_vertex_z = 0;
    std::vector<unsigned long> *i_shr = 0;
    std::vector<unsigned long> *i_trk = 0;
    std::vector<double> *reco_shower_energy_max = 0;
    std::vector<double> *reco_shower_dirx = 0;
    std::vector<double> *reco_shower_diry = 0;
    std::vector<double> *reco_shower_dirz = 0;
    std::vector<double> *reco_track_displacement = 0;
    std::vector<double> *reco_track_proton_kinetic_energy = 0;

    // Necessary for vectors, for some reason
    TBranch *bi_shr = 0;
    TBranch *bi_trk = 0;
    TBranch *breco_shower_energy_max = 0;
    TBranch *bsim_shower_energy = 0;
    TBranch *breco_shower_dirx = 0;
    TBranch *breco_shower_diry = 0;
    TBranch *breco_shower_dirz = 0;
    TBranch *breco_track_displacement = 0;
    TBranch *breco_track_proton_kinetic_energy = 0;
    
    data_file->tvertex->SetBranchAddress("run_number",    &n_run_number);
    data_file->tvertex->SetBranchAddress("subrun_number", &n_subrun_number);
    data_file->tvertex->SetBranchAddress("event_number",  &n_event_number);
    data_file->tvertex->SetBranchAddress("reco_vertex_x", &n_vertex_x);
    data_file->tvertex->SetBranchAddress("reco_vertex_y", &n_vertex_y);
    data_file->tvertex->SetBranchAddress("reco_vertex_z", &n_vertex_z);

    data_file->tvertex->SetBranchAddress("i_shr", &i_shr, &bi_shr);
    data_file->tvertex->SetBranchAddress("i_trk", &i_trk, &bi_trk);
    data_file->tvertex->SetBranchAddress("reco_shower_energy_max", &reco_shower_energy_max, &breco_shower_energy_max);
    data_file->tvertex->SetBranchAddress("reco_shower_dirx", &reco_shower_dirx, &breco_shower_dirx);
    data_file->tvertex->SetBranchAddress("reco_shower_diry", &reco_shower_diry, &breco_shower_diry);
    data_file->tvertex->SetBranchAddress("reco_shower_dirz", &reco_shower_dirz, &breco_shower_dirz);
    data_file->tvertex->SetBranchAddress("reco_track_displacement", &reco_track_displacement, &breco_track_displacement);
    data_file->tvertex->SetBranchAddress("reco_track_proton_kinetic_energy", &reco_track_proton_kinetic_energy, &breco_track_proton_kinetic_energy);

    std::cout<<"Starting printPassingPi0DataEvents() for "<<data_file->name<<std::endl;

    for(int i=0;i < fake_list->GetN(); i++ ){
        data_file->tvertex->GetEntry( fake_list->GetEntry(i));

        // Shower kinematics
        double E1 = reco_shower_energy_max->at(i_shr->at(0));
        double E2 = reco_shower_energy_max->at(i_shr->at(1));
        // Correction factors from output of energy correction script in hive/other/
        E1 = 1.24607*E1 + 4.11138;
        E2 = 1.24607*E2 + 4.11138;

        double opAng = TMath::ACos(reco_shower_dirx->at(0)*reco_shower_dirx->at(1)+
                       reco_shower_diry->at(0)*reco_shower_diry->at(1)+
                       reco_shower_dirz->at(0)*reco_shower_dirz->at(1)) ;
        double invMass = sqrt(2*E1*E2*(1 - opAng) );

        // Track kinematics

        // Print kinematics
        /*
        std::cout<<i<<" "<<fake_list->GetEntry(i)<<" "<<n_run_number <<" "
                        <<n_subrun_number<<" "
                        << n_event_number <<" ("
                        <<n_vertex_x<<", "<<n_vertex_y<<", "<<n_vertex_z<< ")"<<" " 
                        << E1 << " " 
                        << E2 << " "
                        << opAng << " " 
                        << invMass << " " 
                        << reco_track_displacement->at(i_trk->at(0)) << " " 
                        << reco_track_proton_kinetic_energy->at(i_trk->at(0)) << " " << std::endl;
        */
        std::cout <<  opAng*57.2958 << std::endl;
    }
    std::cout<<"End printPassingDataEvents()  for "<<data_file->name<<std::endl;


    return 0;
}
