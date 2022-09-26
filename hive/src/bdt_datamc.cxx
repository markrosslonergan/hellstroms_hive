#include "bdt_datamc.h"


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

int bdt_datamc::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, double c1, double c2){
    //for redudancy, calls new function
    return this->plotStacks(ftest,vars,{c1,c2},{});
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

    double title_size_ratio=0.15;
    double label_size_ratio=0.1;
    double title_offset_ratioY = 0.3 ;
    double title_offset_ratioX = 1.1;

    double title_size_upper=0.15;
    double label_size_upper=0.10;
    double title_offset_upper = 1.5;//1.5


    ftest->cd();

    std::vector<std::string> stage_names = {"Topological Selection","Pre-Selection Cuts","", "", "", "", "Final Selection"};
    //  std::vector<std::string> stage_names = {"Topological Selection","Pre-Selection Cuts","Final Selection"};
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
                    //                    std::cout<<"flag1"<<std::endl;
                    TCanvas *cobs = new TCanvas(("can_"+var1.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)).c_str(),1700,1600);
                    cobs->cd();

                    //                    std::cout<<"flag2"<<std::endl;
                    TPad *pad = new TPad(("pad_"+stage_names.at(s)).c_str(), ("pad_"+stage_names.at(s)).c_str(), 0, 0, 1, 1.0);
                    pad->Draw();
                    pad->cd();
                    //                  std::cout<<"flag3"<<std::endl;
                    //TH1 * tsum = (TH1*)mc_stack->getEntrySum(var,s);
                    TH2 * d0 = (TH2*)data_file->getTH2(var1,var2, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[s])+"_"+data_file->tag+"_"+var1.safe_unit+"_"+var2.safe_unit, plot_pot);
                    //TH2 * d0;
                    //                std::cout<<"flag4"<<std::endl;
                    pad->cd();


                    //              std::cout<<"flag5"<<std::endl;
                    d0->Draw("COLZ");
                    d0 ->SetTitle((data_file->tag + ", stage " + std::to_string(s)).c_str());
                    //    d0->GetYaxis()->SetTitleSize(0.05);
                    d0->GetYaxis()->SetTitleSize(0.01);

                    d0->GetYaxis()->SetTitleOffset(0.9);
                    //   d0->GetXaxis()->SetTitleSize(0.05);
                    d0->GetXaxis()->SetTitleSize(0.01);
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

                        padmc->SetLogz();

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


                        std::cout<<"VAR2D::Corrr "<<f->tag<<" "<<var1.safe_unit<<" "<<var2.safe_unit<<" "<<mc->GetCorrelationFactor()<<std::endl;

                        //cobsmc->SaveAs(("var2D/"+tag+"_"+f->tag+"_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)+".png").c_str(),"png");


                        delete cobsmc;
                        delete mc;

                    }//for each item in the mc stack

                }//if different variables and haven't already used the combo

            }//var2
        }//var1
    }//stage

    return 0;
}



int bdt_datamc::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts, std::vector<bdt_info> bdt_infos){
    return  plotStacks(ftest,vars,bdt_cuts,"",bdt_infos);
}

int bdt_datamc::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts, std::string tago, std::vector<bdt_info> bdt_infos){
    // NEW (and soon to be only) ONE
    //
    bool entry_mode = false;
    double plot_pot=data_file->pot;
    if(stack_mode) plot_pot = stack_pot;

    std::cout<<"DATAMC PLOT POT "<<plot_pot<<std::endl;

    double title_size_ratio=0.10;
    double label_size_ratio=0.085;
    double title_offset_ratioY = 0.3 ;
    double title_offset_ratioX = 1.1;

    double title_size_upper=0.06;
    double label_size_upper=0.05;
    double title_offset_upper = 0.7;


    std::vector<std::string> stage_names;

    int leg_num_digits = 1;

    bool OTPC = false;


    ftest->cd();
    if (OTPC){
        stage_names = {"Topological Selection","Pre-Selection Cuts","Post-Cosmic BDT","Post-BNB BDT","Final Selection"};
        //stage_names = {"Topological Selection","Pre-Selection Cuts","Post-Cosmic BDT","Post-BNB BDT","Post-NC#pi^{0} BDT","Post-#nu_{e} BDT","Final Selection"};
        // std::cout<<"flag 2"<<std::endl;

    } else{
        stage_names = {"","","","","","","","",""};
    }
    //Loop over all stages

    int s_min = -1;
    int s_max = bdt_cuts.size()+2;
    if(plot_stage >=0){
        s_min = plot_stage;
        s_max = plot_stage+1;
    }

    {
        int s = plot_stage;



        std::cout<<"On stage: "<<s<<std::endl;
        //First set the files at this stage

        for(auto &f: mc_stack->stack){
            //std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
            //if(s>1 && false) f->calcBDTEntryList(s,bdt_cuts); //Turn off, use below
            std::cout<<"Setting up EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
            f->setStageEntryList((s ? s < 2 : s ));
        }

        std::cout<<"Done with computations on TTrees and bdt_stacks"<<std::endl;
        //if(s>1 && false) data_file->calcBDTEntryList(s,bdt_cuts);
        data_file->setStageEntryList((s ? s <2 :s ));


        //And all variables in the vector var
        for(auto &var: vars){

            std::string urk = (stack_mode? "stack" : "datamc" );
            TFile *fout = new TFile((urk+"/Ratio_"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+tago+".root").c_str(),"recreate");
            fout->cd();

            /*
               std::string isSpec;
               if (!var.is_spectator){
               isSpec = var.unit+ " is a training variable";
               }else{
               isSpec = var.unit + " is a spectator variable";
               if (isSpectator==true){
               std::cout<<isSpec<<std::endl;
               std::cout<<"set to plot train only, skipping!"<<std::endl;
               continue;
               }
               }
               std::cout<<isSpec<<std::endl;
               */


            //If we are at a later stage that 1, lets find the variable.
            if(s>1){
                std::string faster_cut ="(";
                for(int ss=0; ss<s-1; ss++){
                    //faster_cut += bdt_infos[ss].identifier+"_mva >"+std::to_string(bdt_cuts[ss]) +"&&";
                    faster_cut += bdt_infos[ss].identifier+"_mva >="+std::to_string(bdt_cuts[ss]) +"&&";
                }
                faster_cut+="1)";
                var.additional_cut  = "("+var.additional_cut+" ) && ("+ faster_cut +")";
                //var.additional_cut  = faster_cut;
                std::cout<<" var.additional_cut = "<< var.additional_cut <<std::endl;
            }

            std::cout<<" var.additional_cut = "<< var.additional_cut <<std::endl;

            //var.is_logplot = false;
            std::cout<<"Starting on variable "<<var.name<<std::endl;

            TCanvas *cobs = new TCanvas(("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(), (stack_mode? 2200 : 1801),1400); //1600
            //TCanvas *cobs = new TCanvas(("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var.safe_name+"_stage_"+std::to_string(s)).c_str(), (stack_mode? 2200 : 1801),1400); //1600
            cobs->cd();

            if(false&&do_subtraction){
                std::cout<<"Setting do Subtraction inside bdt_stack "<<std::endl;
                mc_stack->setSubtractionVector(subtraction_vec);
            }

            std::vector<double> m_fullvec;

            THStack *stk = (THStack*)mc_stack->getEntryStack(var,s); //so at this point its all in "stack"
            TH1 * tsum  = (TH1*)mc_stack->getEntrySum(var,s, m_fullvec);
            TH1* d0 = (TH1*)data_file->getTH1(var, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[s])+"_"+data_file->tag+"_"+var.safe_name, plot_pot);
            TH1 * tsum_after = (TH1*)tsum->Clone("tsumafter");


            
            

            //Check Covar for plotting
            TMatrixD * covar_collapsed = new TMatrixD(var.n_bins,var.n_bins);
            d0->SetBinErrorOption(TH1::kPoisson);

         
            std::vector<double> vec_mc_stats_error;
            std::vector<double> vec_mc;
            for(int c=0; c< tsum->GetNbinsX();c++){
                double mc_stats_error = tsum->GetBinError(c+1);
                vec_mc_stats_error.push_back(mc_stats_error);
                vec_mc.push_back(tsum->GetBinContent(c+1));
            }
            /*
               TH1 *trev = (TH1*)mc_stack->stack.at(1)->getTH1(var, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[s])+"_"+"arse+"+var.safe_name, plot_pot); 
               trev->Scale(125.0);
               trev->SetLineColor(mc_stack->stack.at(1)->col);
               trev->SetLineWidth(3); 
               */

                       
            if(var.has_covar && m_error_string !="stats"){

                TFile *covar_f = new TFile(var.covar_file.c_str(),"read");
                if(covar_f->IsZombie()){
                    std::cout<<"Error!! The covariance file failed to open: Does not exist?  "<<var.covar_file.c_str()<<std::endl;
                    exit(EXIT_FAILURE);
                }


                TMatrixD * covar_full = (TMatrixD*)covar_f->Get(var.covar_name.c_str());
                covar_collapsed->Zero();

                std::cout<<"Reading this from a covariance matrix "<<var.covar_file.c_str()<<std::endl;
                std::cout<<"Is it frac or full? "<<var.covar_type.c_str()<<std::endl;

                m_fullvec = mc_stack->getEntryFullVector(var);

                if(covar_full->GetNcols() != m_fullvec.size()){
                    std::cout<<"Aghr "<<covar_full->GetNcols()<<" "<<m_fullvec.size()<<std::endl;
                    exit(EXIT_FAILURE);
                }



                //std::vector<TMatrixT<double>> Mshapenorm = splitNormShape(*covar_full, m_fullvec);
                //(*covar_full) = (*covar_full) = Mshapenorm[2];

                this->calcCollapsedCovariance(covar_full, covar_collapsed,var);

                //Some shape/norm
                //std::vector<TMatrixT<double>> Mshapenorm = splitNormShape(*covar_collapsed, vec_mc);

                /*
                   if(true){
                   for(int p=0; p<Mshapenorm[2].GetNcols();p++){
                   std::cout<<"NORM "<<Mshapenorm[2](p,p)<<" "<<"SHAPE "<<Mshapenorm[0](p,p)<<" "<<"MIXED "<<Mshapenorm[1](p,p)<<" ALL "<<(*covar_full)(p,p)<<" || Result : "<<(*covar_full)(p,p) - Mshapenorm[2](p,p) <<std::endl;
                   }
                   (*covar_full) =  (*covar_full) - Mshapenorm[2];

                   }else{
                   for(int p=0; p<Mshapenorm[2].GetNcols();p++){
                   std::cout<<"NORM "<<Mshapenorm[2](p,p)<<" "<<"SHAPE "<<Mshapenorm[0](p,p)<<" "<<"MIXED "<<Mshapenorm[1](p,p)<<" ALL "<<(*covar_collapsed)(p,p)<<" || Result : "<<(*covar_collapsed)(p,p) - Mshapenorm[2](p,p) <<std::endl;
                   }
                   (*covar_collapsed) =  (*covar_collapsed) - Mshapenorm[2];
                   }
                   */

                //std::vector<double> fkr = {0.144464,0.0794493,0.204987};
                //std::vector<double> fkr = {0.0941,0.0823,0.2135};
                //std::vector<double> fkr = {0.145171,0.0859779,0.11318,0.0891966,0.151207,0.189539};
                //std::vector<double> fkr = {0.26,0.123,0.116,0.094,0.137,0.145};

                //with coh not coh
                std::vector<double> fkr = {0.235736,0.23907,0.304031};//1g1p before
                //fkr = {0.0954412,0.0903098,0.146111};//1g1p after

                //fkr = {0.285885,0.201206, 0.209106,0.193913,0.229783,0.226371};//1g0p before
                //fkr = {0.259299, 0.120909, 0.108522, 0.0874204, 0.12301, 0.140846};//1g0p after

                //October2020
                //fkr={1.26481, 2.44423,  4.22811,  1.28523,    1.54774,      2.10316};
                fkr = { 5.45969,         18.0884,      19.234,     53.0878,    23.4021,    10.0496,     6.35323,    3.72808,    2.22644,    1.36843};


                for(int c=0; c< tsum->GetNbinsX();c++){
                    //double dv = tsum->GetBinContent(c+1);
                    //tsum->SetBinError(c+1, sqrt((*covar_full)(c,c)*dv*dv));
                    //tsum_after->SetBinError(c+1, sqrt((*covar_m2)(c,c)));
                    //  double mc_sys_error = fkr[c]*tsum->GetBinContent(c+1);  //sqrt((*covar_collapsed)(c,c));

                    double mc_stats_error = tsum->GetBinError(c+1);
                    double mc_sys_error = sqrt((*covar_collapsed)(c,c));
                    //mc_sys_error = sqrt(fkr[c]);
                    //double mc_sys_error = sqrt(fabs((*covar_collapsed)(c,c)));
                    double tot_error = sqrt(mc_stats_error*mc_stats_error+mc_sys_error*mc_sys_error);

                    //double tot_error = sqrt(mc_sys_error*mc_sys_error);

                    //double tot_error = mc_sys_error; 
                    std::cout<<"Error Summary || Bin "<<c<<" Nmc: "<<tsum->GetBinContent(c+1)<<" Err: "<<tot_error<<" FracErr: "<<tot_error/tsum->GetBinContent(c+1)*100.0<<" SysErr: "<<mc_sys_error<<" SysFrac: "<<mc_sys_error/tsum->GetBinContent(c+1)*100.0<<" MCStat: "<<mc_stats_error<<" MCStatFrac: "<<mc_stats_error/tsum->GetBinContent(c+1)*100.0<<std::endl;
                    tsum->SetBinError(c+1, tot_error);
                }


                covar_f->Close();
            }else{
                for(int c=0; c< tsum->GetNbinsX()+1;c++){
                    //tsum->SetBinError(c+1, 0.0001);
                }

            }
            cobs->cd();



            //            std::vector<double> ks_sum = data_file->getVector(var,s);

            double rmin = 0.0;
            double rmax = 1.999;
            int data_rebin = 1;
            //if(s==0 || s == 1){
            //    rmin=0; rmax = 1.99;
            //}//else if(s==2){ data_rebin = 2;};//else if(s==3){data_rebin=2;};


            //d0->Rebin(data_rebin);

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

            bool perbin = false;

            if (perbin == true){
                for(int i = 0 ; i< mc_stack->vec_hists.size(); i++){
                    double total = 0;
                    double total_unscaled = 0;
                    auto &f = mc_stack->stack[i];
                    double scale = (plot_pot/f->pot)*f->scale_data;

                    std::cout<<"plot_pot = "<<plot_pot<<std::endl;
                    std::cout<<"f->pot = "<<f->pot<<std::endl;
                    std::cout<<"f->scale_data = "<<f->scale_data<<std::endl;

                    std::cout<<"the entries in this file (weighted) = "<<f->GetEntries()<<std::endl;
                    std::cout<<"the scaling to POT= "<<scale<<" which is x"<<1/scale<<" times POT" <<std::endl;
                    std::cout<<"the total number of entries = "<<f->GetEntries()*scale<<std::endl;


                    for(int p=0; p<mc_stack->vec_hists[i]->GetNbinsX();p++){
                        double nscaled = mc_stack->vec_hists[i]->GetBinContent(p+1);
                        double nunscaled = nscaled/scale;
                        total += nscaled;
                        total_unscaled += nunscaled;
                        std::cout<<"for hist "<<f->tag<<" in bin "<<p+1 <<" = "<<nscaled<<" which corresponds to "<<nunscaled <<" events"<<std::endl;

                    }
                    std::cout<<"for hist "<<f->tag<<" the total is  "<<total<<"/"<<total_unscaled<<std::endl;
                    std::cout<<"---------------------------"<<std::endl;

                }
            }


            tsum->SetMarkerSize(0);

            d0->SetMarkerSize(2);
            gStyle->SetEndErrorSize(10);

            cobs->cd();
            TPad *pad0top;
            if(!stack_mode) {
                pad0top= new TPad(("pad0top_"+stage_names.at(s)).c_str(), ("pad0top_"+stage_names.at(s)).c_str(), 0, 0.4, 1, 1.0);//0.4 was 0.35
                //if(is_bdt_variable /*|| var.is_logplot*/ )  pad0top->SetLogy();
                pad0top->SetBottomMargin(0); // Upper and lower plot are joined
                pad0top->Draw();             // Draw the upper pad: pad2top
                pad0top->cd();               // pad2top becomes the current pad

            }else{
                pad0top= new TPad(("pad0top_"+stage_names.at(s)).c_str(), ("pad0top_"+stage_names.at(s)).c_str(), 0, 0, 1.0, 1.0);//0.4 was 0.35
                pad0top->Draw();
                pad0top->cd();
                std::cout<<"jarp"<<std::endl; 
            }

            //      double rmin = 0.5;
            //   	double rmax = 1.699;
            //	double rmin = 0;
            //	double rmax = 2.99;
            //	int data_rebin = 1;
            if(s==0 || s == 1){
                rmin=0; rmax = 1.99;
            }//else if(s==2){ data_rebin = 2;}//else if(s==3){data_rebin=2;};

            double max_modifier = 1.65;
            if (OTPC == true){
                max_modifier = 3.25;
                //std::cout<<"flag 3"<<std::endl;
            } else{
                if (s==1){
                    // max_modifier = 1.7;
                    max_modifier = 2.0;
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

                if (s==4){
                    max_modifier =2;
                }
                if(s>4){
                    max_modifier = 2.75;
                }
            }


            //     double max_modifier = 1.7;
            double min_val = 0.01;
            if(is_bdt_variable) {
                max_modifier = 15.0;
                //  max_modifier = 25.0;
                min_val = 0.1;
            }

            if(var.is_logplot){
                pad0top->SetLogy();
                max_modifier = 500.0;
                // min_val = 0.0001;
                //max_modifier=3500.0;
            }

            bool isBDT = false;
            int max_val;
            std::string mva = "mva";
            if(var.name.size() >= mva.size() && var.name.compare(var.name.size() - mva.size(), mva.size(), mva) == 0 && var.is_logplot){
                isBDT = true;
            }
            if(isBDT){
                min_val = 1e-2;
                max_val = 1e5;

            }

            d0->SetMarkerStyle(20);
            d0->SetMarkerSize(3);
            d0->SetLineColor(kBlack);

            std::vector<bool> remove_to_merge = merge_down_vec; 

            TList *stlists = (TList*)stk->GetHists();

            int lor = 0;
            TH1* saved;
            bool mergetarget = false;
            for(const auto&& obj: *stlists){
                if(remove_to_merge[lor]){
                    saved = (TH1*)obj->Clone(("saved_clone_"+std::to_string(lor)).c_str());
                    mergetarget = true;
                    obj->Print();
                    lor++;
                    continue;
                }
                if(mergetarget){
                    ((TH1*)obj)->Add(saved);
                    mergetarget = false;
                }
                obj->Print();
                lor++;
            }
            lor=0;//and Zero
            for(const auto&& obj: *stlists){
                if(remove_to_merge[lor]){
                    ((TH1*)obj)->Scale(0);
                }
                obj->Print();
                lor++;
            }

            //Lets get the things we need
            //stk, tsum, d0, tsum
            TH1 *tmp_tsum = (TH1*)tsum->Clone(("tmp_tsum"+std::to_string(s)).c_str());
            TH1 *tmp_tsum2 = (TH1*)tsum->Clone(("tmp_tsum2"+std::to_string(s)).c_str());


            // ************* Do a lot of math stuf before scaling **************

            TLegend *l0;
            if(OTPC==true){
                // std::cout<<"flag 4"<<std::endl;
                l0 = new TLegend(0.11,0.5,0.89,0.89);}
            else{
                l0 = new TLegend(0.11,0.55,0.89,0.89);
            }
            l0->SetFillStyle(0);
            l0->SetLineWidth(0);
            l0->SetNColumns(2);
            l0->SetTextFont(43);//63 for bold
            l0->SetTextSize(stack_mode ? 53 : 45);

            double NeventsStack = 0;
            double NErrStack = 0;
            int which_signal = 0;
            bool b_signal_on_top = false;
            int n=0;
            std::vector<TH1F*> fake_legend_hists;

            double Nevents_save_for_later = 0.0;


            for(auto &f: mc_stack->stack){

                double Nentries = f->GetEntries(var.additional_cut);
		// Guanqun: print out signal events even when we plot it on top
                //if(mc_stack->signal_on_top[n]) Nentries = 0.0; //remove from calculation
                double Nevents = Nentries*(plot_pot/f->pot)*f->scale_data;
                double N_MCerr = sqrt(Nentries)*(plot_pot/f->pot)*f->scale_data;
                ;
                /*  stack->vec_hists[i];
                    for(int p=0; p<h1->GetNbinsX();p++){

                    std::cout<<"for file "<<f->tag<<" in bin "<<p+1 <<" = "<<h1->GetBinContent(p+1)<<std::endl;

                    }
                    */


                NeventsStack+=Nevents;
                NErrStack +=N_MCerr*N_MCerr; 

                auto h1 = new TH1F(("tmp"+stage_names.at(s)+var.safe_name+f->tag).c_str(),"TLegend Example",200,-10,10);
                fake_legend_hists.push_back(h1);
                h1->SetFillColor(f->col);
                h1->SetFillStyle(f->fillstyle);
                /*
                   std::cout << "EVENTS For sample " << f->tag << std::endl;
                   for (int i = 0; i < h1->GetNbinsX(); i++) {
                   std::cout << "EVENTS Entries in bin " << i << " = " << h1->GetBinContent(i) << std::endl;
                   }
                   */
                //if(mc_stack->signal_on_top[n]){
                //    h1->SetLineColor(f->col);
                //    h1->SetLineWidth(3);
                //}else{
                h1->SetLineColor(kBlack);
                //}
                std::string string_events = to_string_prec(Nevents,leg_num_digits);
                if(do_subtraction){
                    if(subtraction_vec[n]) string_events+=" Subtracted";
                }
                std::string leg_type = "f";   

                std::cout<<"Run2Run: "<<f->plot_name<<" events/1e20 POT "<<to_string_prec(Nevents/(double)data_file->pot*1e20,3)<<"  +/-  "<<to_string_prec(N_MCerr/(double)data_file->pot*1e20,3)<<" base "<<Nevents<<" +/- "<<N_MCerr<<std::endl;

                //if(mc_stack->signal_on_top[n]) leg_type = "l";
                //l0->AddEntry(h1,("#splitline{"+f->plot_name+"}{"+string_events+"}").c_str(),"f");

                //int f->plot_name.size(); 

                if(mc_stack->signal_on_top[n]){
                    which_signal = n;
                    //b_signal_on_top = true;
                }

                if(!remove_to_merge[n]){
                    string_events = to_string_prec(Nevents+Nevents_save_for_later,leg_num_digits);
                    if(b_signal_on_top){
                        l0->AddEntry(h1,(f->plot_name).c_str(),leg_type.c_str());
                    }else{
                        l0->AddEntry(h1,(f->plot_name+" "+string_events).c_str(),leg_type.c_str());
                    }

                    Nevents_save_for_later =0;
                }else{
                    Nevents_save_for_later += Nevents;

                }
                //l0->AddEntry(h1,(f->plot_name).c_str(),"f");
                n++;
            }
            NErrStack = sqrt(NErrStack);
            std::cout<<"Run2Run: StackAll  events/1e20 POT "<<to_string_prec(NeventsStack/(double)data_file->pot*1e20,3)<<"  +/-  "<<to_string_prec(NErrStack/(double)data_file->pot*1e20,2)<<std::endl;


            //This one is just for legend messing
            TH1 *leg_hack = (TH1*)tmp_tsum->Clone(("leg_tmp_tsum"+std::to_string(s)).c_str());
            leg_hack->SetLineWidth(2);


            if(var.has_covar && m_error_string!="stats"){
                if(b_signal_on_top){
                    l0->AddEntry(leg_hack,("Total Background and Error "+var.covar_legend_name).c_str() ,"fl"); // Was le
                }else{
                    l0->AddEntry(leg_hack,("Total Prediction: "+ to_string_prec(NeventsStack,leg_num_digits)).c_str(),"fl"); // Was le
                }
            }else{
                if(b_signal_on_top){
                    l0->AddEntry(leg_hack,("Total Background and Error "+var.covar_legend_name).c_str() ,"fl"); // Was le
                }else{
                    l0->AddEntry(leg_hack,("Total Prediction: "+ to_string_prec(NeventsStack,leg_num_digits)).c_str(),"fl"); // Was le
                }

            }

            double KS_tsum_d0 = tsum->KolmogorovTest(d0);
            std::cout<<"Binned KS-test: "<<var.name<<" "<<tsum->KolmogorovTest(d0)<<std::endl;
            std::cout<<"Binned Chi-test standard: "<<var.name<<" "<<tsum->Chi2Test(d0,"CHI2")<<std::endl;
            std::cout<<"Binned Chi-test: "<<var.name<<" "<<tsum->Chi2Test(d0,"UW CHI2")<<std::endl;
            std::cout<<"Binned Chi-test (rev): "<<var.name<<" "<<d0->Chi2Test(tsum,"UW CHI2")<<std::endl;

             d0->SetBinErrorOption(TH1::kPoisson);

            

             //Ok, move to asymetrical TGraph errors for a bit 
            std::vector<double> vAsyX;  
            std::vector<double> vAsyY;  
 
            std::vector<double> vAsyErr_right;
            std::vector<double> vAsyErr_left;  
            std::vector<double> vAsyErr_up;  
            std::vector<double> vAsyErr_down;  
           
            for(int id =0; id<d0->GetNbinsX()+1;id++){
                       vAsyErr_left.push_back(d0->GetBinWidth(id)/2.0);
                       vAsyErr_right.push_back(d0->GetBinWidth(id)/2.0);
                       
                       if(!div_bin){
                           vAsyErr_up.push_back((d0->GetBinErrorUp(id)));
                           vAsyErr_down.push_back((d0->GetBinErrorLow(id)));
                           vAsyX.push_back(d0->GetBinCenter(id));
                           vAsyY.push_back(d0->GetBinContent(id));

                       }else{
                            double dy = d0->GetBinWidth(id);
                            double scal = div_scale/dy;
                            
                            vAsyX.push_back(d0->GetBinCenter(id));
                            vAsyY.push_back(d0->GetBinContent(id)*scal);
                       
                            vAsyErr_up.push_back(scal*(d0->GetBinErrorUp(id)));
                            vAsyErr_down.push_back(scal*(d0->GetBinErrorLow(id)));
                       }

            }

            

            TGraphAsymmErrors * gAsyData = new TGraphAsymmErrors(vAsyX.size(),&vAsyX[0],&vAsyY[0],&vAsyErr_left[0],&vAsyErr_right[0],&vAsyErr_down[0],&vAsyErr_up[0]);
            gAsyData->SetLineWidth(1);
            gAsyData->SetMarkerStyle(20);
            gAsyData->SetMarkerSize(3);
            gAsyData->SetMarkerColor(kBlack);
            gAsyData->SetFillColor(kBlack);
            gAsyData->SetFillStyle(1001);


         


            double tot_norm_error= 0;
            double mychi =0;
            int ndof = 0;
            bool use_cnp = true;
            if(!var.has_covar || m_error_string=="stats"){


                for(int p=0; p<d0->GetNbinsX();p++){

                    double da = d0->GetBinContent(p+1);
                    double bk = tsum->GetBinContent(p+1);

                    std::cout<<"total MC+EXT in bin "<<p+1<<" = "<<tsum->GetBinContent(p+1)<<std::endl;
                    std::cout<<"total data in bin "<<p+1<<" = "<<d0->GetBinContent(p+1)<<std::endl;

                    if ( bk ==0){
                        std::cout<<"ERROR mychi, for bin "<<p<<" n_data= "<<da<<" and n_mc= "<<bk<<std::endl;

                    } else{

                        // Version 1 chi^2
                        //double da_err = sqrt(d0->GetBinContent(p+1));
                        //double bk_err = tsum->GetBinError(p+1);

                        double da_err = sqrt(tsum->GetBinContent(p+1));
                        double bk_err = tsum->GetBinError(p+1);

                        double tk = pow(da-bk,2)/(da_err*da_err+bk_err*bk_err);


                        tot_norm_error += pow(tsum->GetBinError(p+1),2)+d0->GetBinContent(p+1);
                        //std::cout<<da<<" "<<bk<<" "<<da_err<<" "<<bk_err<<" total: "<<sqrt(da_err*da_err+bk_err*bk_err)<<" chi^2 "<<tk<< std::endl;
                        if(tk==tk){
                            mychi+=tk;
                            ndof++;
                        }

                    }
                }
                tot_norm_error = sqrt(tot_norm_error);
            } else if (var.has_covar && use_cnp && m_error_string!="stats") {


                std::cout << "[CHI_CNP] Starting chi^2 CNP calculation" << std::endl;

                TMatrixT<double> Mout = *covar_collapsed;
                // Calculate middle term, sys + stat covar matrices
                // CNP + intrinsic MC error^2
                // this was the block

                for(int i =0; i<covar_collapsed->GetNcols(); i++) {
                    Mout(i,i) += ( d0->GetBinContent(i+1) >0.001 ? 3.0/(1.0/d0->GetBinContent(i+1) +  2.0/tsum->GetBinContent(i+1))  : tsum->GetBinContent(i+1)/2.0 ) + pow(vec_mc_stats_error[i],2);
                }

                if(false){//just for zeroing

                    for(int i =0; i<covar_collapsed->GetNcols(); i++) {
                        for(int k =0; k<covar_collapsed->GetNcols(); k++) {
                            if(i!=k)Mout(i,k)=0;
                        }
                    }
                }
                tot_norm_error = sqrt(calcTotalNormError(&Mout,var));


                // Invert matrix, because the formula says so
                Double_t *determ_ptr;
                Mout.Invert(determ_ptr);

                // Do the thing
                for (int ib = 0; ib<var.n_bins; ib++) {
                    for (int jb=0; jb<var.n_bins; jb++) {
                        mychi += (tsum->GetBinContent(ib+1)-d0->GetBinContent(ib+1))*(Mout)(ib,jb)*(tsum->GetBinContent(jb+1)-d0->GetBinContent(jb+1));
                    }
                }
                ndof = var.n_bins;

            } else{

                Double_t *determ_ptr;

                for(int ib=0; ib<var.n_bins; ib++){

                    //                       (*covar_collapsed)(ib,ib) += d0->GetBinContent(ib+1);//sqrt(n*n)//This is Data stats error
                    (*covar_collapsed)(ib,ib) += tsum->GetBinContent(ib+1);//sqrt(n*n)//This is MC stats error

                    if((*covar_collapsed)(ib,ib)==0){
                        std::cout<<"WARNING a 0 in the matrix "<<ib<<std::endl;
                    }
                }
                covar_collapsed->Invert(determ_ptr);

                for(int ib=0; ib<var.n_bins; ib++){
                    for(int jb=0; jb<var.n_bins; jb++){
                        double curchi   =  (tsum->GetBinContent(ib+1)-d0->GetBinContent(ib+1))*(*covar_collapsed)(ib,jb)*(tsum->GetBinContent(jb+1)-d0->GetBinContent(jb+1));
                        mychi += curchi;
                        //std::cout<<"Hi: "<<ib<<" "<<jb<<" "<<tsum->GetBinContent(ib+1)<<" "<<d0->GetBinContent(ib+1)<<" "<<(*covar_collapsed)(ib,ib)<<" "<<tsum->GetBinContent(jb+1)<<" "<<d0->GetBinContent(jb+1)<<" "<<curchi<<" "<<mychi<<std::endl;

                    } 
                }
                ndof = var.n_bins;
            }


            double NdatEvents = data_file->GetEntries(var.additional_cut)*(plot_pot/data_file->pot )*data_file->scale_data;
            std::cout<<"Run2Run: Data   events/1e20 POT "<<to_string_prec(NdatEvents/(double)data_file->pot*1e20,3)<<"  +/-  "<<to_string_prec(sqrt(NdatEvents)/(double)data_file->pot*1e20,3)<<std::endl;



            

            //*******************8 Drawing all of things below **********************
            //Now scale rest, has to happen after the assignment above of errors and of the TAssymGraph for data
             if(div_bin){
                TList *stlists = (TList*)stk->GetHists();
                for(const auto&& obj: *stlists){
                    ((TH1*)obj)->Scale(div_scale,"width");
                }

                tsum->Scale(div_scale,"width");
                tmp_tsum->Scale(div_scale,"width");
                tmp_tsum2->Scale(div_scale,"width");
                d0->Scale(div_scale,"width");
            }

            //
            stk->Draw("hist");
            stk->SetTitle("");
            //stk->SetTitle(stage_names.at(s).c_str());
            /*stk->GetXaxis()->SetTitle(var.unit.c_str());
            stk->GetYaxis()->SetTitle((!div_bin ? "Events" :  "Events / "+to_string_prec(div_scale,3)+" MeV ").c_str());
            if(!stack_mode){
                stk->GetYaxis()->SetTitleSize(title_size_upper);
                stk->GetYaxis()->SetLabelSize(label_size_upper);
                stk->GetYaxis()->SetTitleOffset(title_offset_upper);
            }*/
            std::cout<<"the max modifier is "<<max_modifier<<" and the min val is "<< min_val<<std::endl;
            std::cout<<" and input pmin "<<var.plot_min<<" and pmax "<<var.plot_max<<std::endl;
            std::cout<<" and do we have a signal on top to plot? "<<b_signal_on_top<<std::endl;
            std::cout<<" and do we want LEE? "<<plot_lee_on_top<<std::endl;

            //This is where we will add the signal on top, but not part of any maths (covar..etc..)
        
            TH1D *sig_on_top;
             
            if(b_signal_on_top){ 
                sig_on_top= (TH1D*)mc_stack->getSignalOnTop(var); //Signal on top
                stk->Add(sig_on_top);
                stk->DrawClone("hist same");

                TH1D *lee_on_top = (TH1D*)sig_on_top->Clone(("lee_sig_"+var.safe_name).c_str());
                lee_on_top->SetFillStyle(0);
                lee_on_top->SetLineStyle(9);
                lee_on_top->Scale(3.18);
                lee_on_top->Add(tsum);
                lee_on_top->SetLineColor(mc_stack->stack.at(which_signal)->col);
                lee_on_top->SetLineWidth(3);
               
                if(plot_lee_on_top){
                    lee_on_top->Draw("hist same");
                    l0->AddEntry(lee_on_top,("LEE Model (x_{MB} =3.18)"),"l");		
                }else if(!scale_signal_overlay){
                    TH1D *hfake = new TH1D("fk","",1,0,1);
                    hfake->SetLineColor(kWhite);
                    hfake->SetLineWidth(0);
                    l0->AddEntry(hfake," ","l");		
                }
            }


 





            if(var.plot_max==-999){ 
                stk->SetMaximum(std::max(tsum->GetMaximum(), (stack_mode ? -1 :d0->GetMaximum()))*max_modifier);
            }else{
                max_val = var.plot_max;
                stk->SetMaximum(var.plot_max);
            }

            if(var.plot_min==-999){ 
                stk->SetMinimum(min_val);
            }else{
                min_val=var.plot_min;
                stk->SetMinimum(var.plot_min);
            }

            if (isBDT){
                stk->SetMaximum(max_val);
                stk->SetMinimum(min_val);
            }

            //TEST
            stk->GetXaxis()->SetTitle(var.unit.c_str());
            //stk->GetYaxis()->SetTitle((!div_bin ? "Events" :  "Events / Bin Width ["+to_string_prec(div_scale,3)+" GeV]").c_str());
            stk->GetYaxis()->SetTitle((!div_bin ? "Events" :  "Events / "+to_string_prec(div_scale,3)+" GeV/c").c_str());
            //stk->GetYaxis()->SetTitle((!div_bin ? "Events" :  "Events / GeV "));
            if(!stack_mode){
                stk->GetYaxis()->SetTitleSize(title_size_upper);
                stk->GetYaxis()->SetLabelSize(label_size_upper);
                stk->GetYaxis()->SetTitleOffset(title_offset_upper);
            }

            //This is for if we want to draw the signal on with "100x" 
            TH1 * scale_signal_hist;
            
            if(scale_signal_overlay){
                 scale_signal_hist = (TH1D*)mc_stack->getSignalOnTop(var); //Signal on top
                //(TH1*)mc_stack->vec_hists[which_signal]->Clone(("signal_clone"+stage_names.at(s)).c_str());
                double scal_val =230.0;
                //double scal_val = NdatEvents/scale_signal_hist->Integral();
                scale_signal_hist->Scale(scal_val);
                //scale_signal_hist->Scale(100.0);
                scale_signal_hist->SetFillStyle(0);
                //scale_signal_hist->DrawCopy("hist same");
                scale_signal_hist->SetLineStyle(1);
                scale_signal_hist->SetLineColor(mc_stack->stack.at(which_signal)->col);
                scale_signal_hist->SetLineWidth(4);
                scale_signal_hist->Draw("same hist");
                l0->AddEntry(scale_signal_hist,("NC Coherent 1 #gamma (x"+to_string_prec(scal_val,0)+")").c_str(),"l");		
            }


            tsum->SetLineWidth(3);
            tsum->DrawCopy("Same E2");
            
            tmp_tsum2->SetFillStyle(0);
            tmp_tsum2->DrawCopy("same hist");

          

            tsum->SetFillStyle(0);//vec_th1s.at(s)->Draw("hist same");
            
//            d0->SetBinErrorOption(TH1::kPoisson);
            if(!stack_mode){
                  //d0->Draw("same E1 E0");
                  gAsyData->Draw("same E1P");
                  gAsyData->Draw("same P");
            }



            /////// Print resolution for diphoton mass ////////
            // First, find variables containing the string of interest
            std::string massSearch("Invariant");
            std::size_t found = var.unit.find(massSearch);

            // Fit Gaussian to that variable
            if (found != std::string::npos ) {
                std::cout << "[BLARG] Fitting " << var.unit << " stage " << std::to_string(s) << std::endl;
                TF1 *gausfit_data  = new TF1("gausfit_data" , "gaus", 0.05 , 0.25);
                //TF1 *gausfit_data2 = new TF1("gausfit_data2", "gaus", 0.135, 0.245);
                TF1 *gausfit_data2 = new TF1("gausfit_data2", "pol1", 0.00, 0.4);
                //TF1 *doub_gaus = new TF1("doub_gaus", "gaus(0)+gaus(3)", 0.015, 0.25);
                TF1 *doub_gaus = new TF1("doub_gaus", "gaus(0)+pol1(3)", 0.00, 0.4);
                gausfit_data ->SetLineColor(kRed);
                gausfit_data2->SetLineColor(kAzure);
                doub_gaus    ->SetLineColor(kCyan);
                doub_gaus->SetLineWidth(3);

                d0->Fit(gausfit_data, "lvrN");
                d0->Fit(gausfit_data2, "lvrN");
                Double_t par[5]; // 3 Gaus + 2 linear parameters
                gausfit_data ->GetParameters(&par[0]);
                gausfit_data2->GetParameters(&par[3]);
                doub_gaus->SetParameters(par);
                d0->Fit(doub_gaus, "lvr","same");
                
                //std::cout << "BLARG Doub 0 = " << doub_gaus->GetParameter(0) << std::endl;
                //std::cout << "BLARG Doub mean = " << doub_gaus->GetParameter(1) << std::endl;
                //std::cout << "BLARG Doub width = " << doub_gaus->GetParameter(2) << std::endl;
                double mass_data           = doub_gaus->GetParameter(1);
                double mass_err_data       = doub_gaus->GetParError(1);
                double mass_width_data     = doub_gaus->GetParameter(2);
                double mass_width_err_data = doub_gaus->GetParError(2);
                double mass_res_data = doub_gaus->GetParameter(2)/doub_gaus->GetParameter(1);
                double mass_res_data_err = sqrt(std::pow(doub_gaus->GetParError(1)/doub_gaus->GetParameter(1) ,2) +
                        std::pow(doub_gaus->GetParError(2)/doub_gaus->GetParameter(2), 2) );
                std::cout << "BLARG Linear A = " << doub_gaus->GetParameter(3) << " +/- " << doub_gaus->GetParError(3) << std::endl;
                std::cout << "BLARG Linear B = " << doub_gaus->GetParameter(4) << " +/- " << doub_gaus->GetParError(4) << std::endl;
                std::cout << "[BLARG] Data mass: " << mass_data << " +/- " << mass_err_data << std::endl;
                std::cout << "[BLARG] Data mass StdDev: " << mass_width_data << " +/- " << mass_width_err_data << std::endl;
                std::cout << "[BLARG] Data mass Res: " << mass_res_data*100. << "% +/- " << mass_res_data_err*100. << "%" << std::endl;
                //std::cout << "BLARG Doub other width? = " << doub_gaus->GetParameter(5) << std::endl;
                /*
                   TF1 *gausfit_data = new TF1("gausfit_data", "gaus", 0.05, 0.25);
                   TF1 *gausfit_mc = new TF1("gausfit_mc", "gaus");
                   TH1 *tmp_hist = stk->GetHistogram();
                   double lowFit = 0.05, highFit = 0.25;
                   double mass_data = 0., mass_err_data = 0;
                   double mass_width_data = 0., mass_width_err_data = 0.;
                   double mass_res_data = 0., mass_res_data_err = 0.;
                   double mass_mc = 0., mass_err_mc = 0;
                   double mass_width_mc = 0., mass_width_err_mc = 0.;
                // Fit range should be similar for data and MC
                //lowFit = d0->GetXaxis()->GetBinLowEdge(1);
                //highFit = d0->GetXaxis()->GetBinLowEdge(d0->GetNbinsX()+1);
                d0->Fit(gausfit_data, "lvr", "", lowFit, highFit);
                //tmp_hist->Fit(gausfit_mc, "q", "", lowFit, highFit);
                //tsum->Fit(gausfit_mc, "q", "", lowFit, highFit);
                //std::cout << "[BLARG] tmp max = " << tmp_hist->GetMaximum() << std::endl;
                mass_data = gausfit_data->GetParameter(1);
                mass_err_data = gausfit_data->GetParError(1);
                mass_width_data = gausfit_data->GetParameter(2);
                mass_width_err_data = gausfit_data->GetParError(2);
                //mass_mc = gausfit_mc->GetParameter(1);
                //mass_err_mc = gausfit_mc->GetParError(1);
                //mass_width_mc = gausfit_mc->GetParameter(2);
                //mass_width_err_mc = gausfit_mc->GetParError(2);
                // Calculate resolution: StdDev/width (w/ uncertainty)
                mass_res_data = gausfit_data->GetParameter(2)/gausfit_data->GetParameter(1);
                mass_res_data_err = sqrt(std::pow(gausfit_data->GetParError(1)/gausfit_data->GetParameter(1) ,2) +
                std::pow(gausfit_data->GetParError(2)/gausfit_data->GetParameter(2), 2) );
                std::cout << "[BLARG] Data mass: " << mass_data << " +/- " << mass_err_data << std::endl;
                std::cout << "[BLARG] Data mass StdDev: " << mass_width_data << " +/- " << mass_width_err_data << std::endl;
                std::cout << "[BLARG] Data mass Res: " << mass_res_data*100. << "% +/- " << mass_res_data_err*100. << "%" << std::endl;
                std::cout << "[BLARG] MC mass: " << mass_mc << " +/- " << mass_err_mc << std::endl;
                std::cout << "[BLARG] MC mass StdDev: " << mass_width_mc << " +/- " << mass_width_err_mc << std::endl;
                gausfit_data->SetLineColor(kRed);
                //gausfit_mc->SetLineColor(kAzure+1);
                gausfit_data->Draw("same");
                //gausfit_mc->Draw("same");
                */
            }

            // l0->AddEntry(d0,(data_file->plot_name).c_str(),"lp");	
            //l0->AddEntry(d0,("#splitline{"+data_file->plot_name+"}{"+to_string_prec(NdatEvents,2)+"}").c_str(),"lp");	
            TH1 *leg_hack2 = (TH1*)leg_hack->Clone(("leg_tmp2_tsum"+std::to_string(s)).c_str());
            std::string sterrname = "#splitline{MC Intrinsic Stat Error}{}";
            if(var.has_covar && m_error_string !="stats" ){
                sterrname = "#splitline{"+var.covar_legend_name+"}{}";
            }
            if(!stack_mode){
                l0->AddEntry(d0,(data_file->plot_name+" "+to_string_prec(NdatEvents,0)).c_str(),"lp");		
                leg_hack2->SetLineColor(kWhite);
                leg_hack2->SetLineWidth(0);
                leg_hack2->SetFillStyle(0);
                if(!b_signal_on_top) l0->AddEntry(leg_hack2,sterrname.c_str(),"l"); // Was le
            }else{
                leg_hack2->SetLineColor(kWhite);
                leg_hack2->SetLineWidth(0);
                leg_hack2->SetFillStyle(0);
                if(!b_signal_on_top){
		    if(!scale_signal_overlay)
                        l0->AddEntry(leg_hack2," ","l"); // Was le
                    l0->AddEntry(leg_hack2,sterrname.c_str(),"l"); // Was le
                    //l0->AddEntry(leg_hack2,sterrname.c_str(),"l"); // Was le
		}
            }

            double yypos = 0.38; //0.38
            double xxpos = 0.5;//0.5
            if(stack_mode)xxpos=0.55;

            l0->Draw();
            l0->SetLineWidth(0);
            l0->SetLineColor(0);
            //l0->SetFillStyle(0); //comment in for transparent
            //l0->SetTextSize(stack_mode ? 0.03 : 0.04);

            // Added by A. Mogan 9/30/20
            std::string topo_draw = data_file->topo_name;
            TLatex topotex;
            topotex.SetTextSize(stack_mode ? 0.04 : 0.06);
            topotex.SetTextAlign(13);  //align at top
            topotex.SetNDC();
            //topotex.DrawLatex(.55,.70, topo_draw.c_str());

            //  TLatex latex;
            // latex.SetTextSize(0.06);
            //  latex.SetTextAlign(13);  //align at top
            //  latex.SetNDC();
            //  latex.DrawLatex(.7,.71,data_file->topo_name.c_str());
            TLatex pottex;
            pottex.SetTextSize(stack_mode ? 0.04 : 0.06);
            pottex.SetTextAlign(13);  //align at top
            pottex.SetNDC();

            //double pot_unit = stack_mode ? 1e20 : 1e19;
            //std::string pot_unit_s = stack_mode ? "e20" : "e19";
            double pot_unit = 1e20;
            std::string pot_unit_s = "x10^{20}";
            std::string pot_draw = data_file->data_descriptor+" ("+to_string_prec(plot_pot/pot_unit,2)+ pot_unit_s+" POT)";
            if(stack_mode) pot_draw = "Simulation ("+to_string_prec(plot_pot/pot_unit,2)+ pot_unit_s+" POT)";
;
            pottex.SetNDC();

            if (OTPC == true){
                pottex.DrawLatex(.55, yypos-0.02, pot_draw.c_str());
                //      std::cout<<"flag 5"<<std::endl;

                //pottex.DrawLatex(.635,.48, pot_draw.c_str());
            } else{
                //pottex.DrawLatex(.55,.60, pot_draw.c_str());
	        //Guanqun: comment out the legend showing POT of the plots 
                pottex.DrawLatex(xxpos,yypos+0.1, pot_draw.c_str());
            }


            TLatex uboone_tex;
            uboone_tex.SetTextSize(stack_mode ? 0.04 : 0.06);
            uboone_tex.SetTextAlign(13);  //align at top
            uboone_tex.SetNDC();
            //uboone_tex.DrawLatex(0.55,0.66,("Selection "+ data_file->topo_name).c_str());
            //uboone_tex.DrawLatex(0.50,yypos+0.16,("Selection "+ data_file->topo_name).c_str());
	    //Guanqun: comment out the legend showing "MicroBooNE"
            uboone_tex.DrawLatex(xxpos,yypos+0.14,("MicroBooNE"));
            //uboone_tex.DrawLatex(xxpos,yypos+0.16,("MicroBooNE in-progress"));

            TLatex descriptor_tex;
            descriptor_tex.SetTextSize(stack_mode ? 0.04 : 0.06);
            descriptor_tex.SetTextAlign(13);  //align at top
            descriptor_tex.SetNDC();
            //descriptor_tex.DrawLatex(0.55,0.66,("Selection "+ data_file->topo_name).c_str());
            //descriptor_tex.DrawLatex(0.50,yypos+0.16,("Selection "+ data_file->topo_name).c_str());
	    //Guanqun: comment out the legend showing "xxx selections"
            //descriptor_tex.DrawLatex(xxpos,yypos+0.02,(data_file->topo_name+" Selection" ).c_str());
            descriptor_tex.DrawLatex(xxpos,yypos+0.02,(plot_stage == 0 ? "Topological Selection Stage" : (plot_stage == 1 ? "Pre-selection Stage" : (plot_stage == 5 ? "Final Selection": " "))));

            // Draw stage name. Added by A. Mogan 10/14/19
            /*   TText *stage = drawPrelim(0.88, 0.92, stage_names.at(s) );
                 stage->SetTextAlign(31); // Right-adjusted 
                 stage->SetTextSize(0.04);
                 stage->Draw();
                 */
            //   TLatex *stage = new TLatex(0.88, 0.92, stage_names.at(s).c_str());
            //   TLatex *t = new TLatex(0.11,0.41,ks.c_str());
            TLatex stage;
            stage.SetNDC();
            // t->SetTextColor(kRed-7);
            // stage->SetTextFont(43);
            // stage->SetTextSize(0.10);
            //stage.DrawLatex(0.6, 0.92, stage_names.at(s).c_str());
            stage.DrawLatex(0.5, 0.92, stage_names.at(s).c_str());

            std::string prestring = (stack_mode ? "MicroBooNE Simulation": "MicroBooNE Preliminary");

            TText *pre; 
            TText *pre2; 
            if (isSpectator) {
                pre = drawPrelim(0.55,stack_mode? yypos+0.025: yypos,prestring.c_str());
                if(stack_mode)pre2 = drawPrelim(0.44,yypos-0.02,"Preliminary");//0.6
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton - In Progress");
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton - In Progress  [Spectator Variable]");
            }else {
                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulation ");
                if(OTPC){   
                    pre = drawPrelim(0.55,yypos-0.09,prestring.c_str());
                    if(stack_mode) pre2 = drawPrelim(0.625,yypos-0.13,"Preliminary");


                    //std::cout<<"flag 6"<<std::endl;

                }else{

                    //pre = drawPrelim(0.55,stack_mode? 0.525 :0.5,prestring.c_str());
                    pre = drawPrelim(xxpos,stack_mode? yypos+0.025 : yypos,prestring.c_str());
                    if(stack_mode)pre2 = drawPrelim(xxpos ,yypos-0.02,"Preliminary");//0.6
                }

                //pre = drawPrelim(0.12,0.92,"MicroBooNE Simulaton In Progress [Training Variable]");

            }

            //Preliminarly or not?
            pre->SetTextSize(stack_mode ? 0.04 : 0.06);
            //pre->Draw();
            if(stack_mode){
                pre2->SetTextSize(stack_mode ? 0.04 : 0.06);;
            //    pre2->Draw();
            }


        
                       
            
            cobs->cd();
            TPad *pad0bot;
            if(!stack_mode){
                pad0bot = new TPad(("padbot_"+stage_names.at(s)).c_str(),("padbot_"+stage_names.at(s)).c_str(), 0, 0.05, 1, 0.4);//0.4 was 0.35
                pad0bot->SetTopMargin(0);
                pad0bot->SetBottomMargin(0.401);//0.351
                pad0bot->SetGridx(); // vertical grid
                pad0bot->Draw();
                pad0bot->cd();       // pad0bot becomes the current pad
            }

            /*
               std::cout<<"BNLAR ";
               std::vector<double> ptc = {7.7,16.9,11.6,4.3};
               double ssum = 0;
               double psum = 5.38+10.77;
               for(auto s: ptc) ssum+=s;


               for(auto &s: ptc) s = s/ssum*psum;

               for(double mm =0.95; mm < 1.3; mm+=0.01){
               double ahchi =0;
               double ahchi_stats =0;
               for(int l=0; l<tsum->GetNbinsX(); l++){
            //                    std::cout<<tsum->GetBinContent(l+1)<<" "<<ptc[l]<<" "<<tsum->GetBinError(l+1)<<std::endl;
            double err = sqrt(  pow(tsum->GetBinError(l+1) ,2) + tsum->GetBinContent(l+1));
            double err_statonly = sqrt(tsum->GetBinContent(l+1));
            ahchi += pow(tsum->GetBinContent(l+1)-mm*ptc[l],2)/pow(err,2);
            ahchi_stats += pow(tsum->GetBinContent(l+1)-ptc[l],3)/pow(err_statonly     ,2); 
            }
            std::cout<<"Result: "<<mm <<" Chi "<<ahchi<<" "<<sqrt(ahchi)<<std::endl;
            std::cout<<"Result: "<<mm <<" ChiStat "<<ahchi_stats<<" "<<sqrt(ahchi_stats)<<std::endl;
            }
            */
            //tsum->Rebin(daaa_rebin);
            TH1* rat_denom = (TH1*)tsum->Clone(("ratio_denom_"+stage_names.at(s)).c_str());
            for(int i=0; i<rat_denom->GetNbinsX(); i++){
                rat_denom->SetBinError(i,0.0);
            }	

            TH1* ratunit = (TH1*)tsum->Clone(("ratio_unit_"+stage_names.at(s)).c_str());
            ratunit->Divide(rat_denom);	
            for(int i=0; i< ratunit->GetNbinsX(); i++){
                ratunit->SetBinError(i+1, tsum->GetBinError(i+1)/tsum->GetBinContent(i+1));
            }


            /* DEPRECIATED
            TH1 * signal_hist = mc_stack->vec_hists[which_signal];
            TH1* rat_signal = (TH1*)signal_hist->Clone(("ratio_signal_"+stage_names.at(s)).c_str());
            //            rat_signal->Add(tsum);
            //          rat_signal->Divide(tsum);
            for(int b=0; b< rat_signal->GetNbinsX()+1; b++){
                double val = (signal_hist->GetBinContent(b)+tsum->GetBinContent(b))/tsum->GetBinContent(b);
                //std::cout<<b<<" "<<val<<" "<<tsum->GetBinContent(b)<<" "<<signal_hist->GetBinContent(b)<<std::endl;
                if(val !=val) val = 0;
                rat_signal->SetBinContent(b,val);
            }
            */

            //  TH1* ratunit_after = (TH1*)tsum_after->Clone(("ratio_unitafter_"+stage_names.at(s)).c_str());

            //   ratunit_after->Divide(rat_denom);		


            ratunit->SetFillColor(kGray+1);
            ratunit->SetMarkerStyle(0);
            ratunit->SetMarkerSize(0);
            //   ratunit->SetFillStyle(3001);
            ratunit->SetFillStyle(3354);
            //gStyle->SetHatchesLineWidth(1);
            //gStyle->SetHatchesSpacing(1);

            if(!stack_mode)          ratunit->Draw("E2");	

            //rat_signal->SetFillStyle(0);
            //rat_signal->SetLineColor(mc_stack->stack[which_signal]->col);
            //rat_signal->SetLineWidth(2);
            if(!stack_mode) ratunit->DrawCopy("E2 same");	

            TLine *line = new TLine(ratunit->GetXaxis()->GetXmin(),1.0,ratunit->GetXaxis()->GetXmax(),1.0 );
            if(!stack_mode)line->Draw("same");
            ratunit->SetLineColor(kBlack);
            ratunit->SetTitle("");
            //ratunit->GetYaxis()->SetTitle("Data/(MC+EXT)");
            //   ratunit->GetYaxis()->SetTitle(  (stack_mode ? "#splitline{Systematic}{Uncertainty}" : "Data/(MC+Cosmic)"));
            if(!stack_mode){
                ratunit->GetYaxis()->SetTitle("Data/Prediction");
                ratunit->GetXaxis()->SetTitle(var.unit.c_str());
                ratunit->GetXaxis()->SetTitleOffset(title_offset_ratioX);
                ratunit->GetYaxis()->SetTitleOffset(title_offset_ratioY*1.25);
                ratunit->SetMinimum(rmin);     
                ratunit->SetMaximum(rmax);//ratunit->GetMaximum()*1.1);
                ratunit->GetYaxis()->SetTitleSize(title_size_ratio);
                ratunit->GetXaxis()->SetTitleSize(title_size_ratio);
                ratunit->GetYaxis()->SetLabelSize(label_size_ratio);
                ratunit->GetXaxis()->SetLabelSize(label_size_ratio);
                ratunit->GetXaxis()->SetTitle(var.unit.c_str());

                ratunit->GetYaxis()->SetNdivisions(505);
            }
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


                    //Mark:Refer to new TFraphAsymmErrors
                    //err_y_high.push_back((d0->GetBinErrorUp(b))/is_zero);
                    //err_y_low.push_back((d0->GetBinErrorLow(b))/is_zero);
                    err_y_high.push_back((gAsyData->GetErrorYhigh(b))/is_zero);
                    err_y_low.push_back((gAsyData->GetErrorYlow(b))/is_zero);

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
                ratpre->Draw("same P hist");	
                gr->Draw("E0 same");
            }

            fout->cd();
            if(!stack_mode){
                d0->Write(("d0_"+tago).c_str());
                ratpre->Write(("ratpre_"+tago).c_str());
                ratunit->Write(("ratunit_"+tago).c_str());
                gr->Write(("graph_"+tago).c_str());



            }
            tsum->Write(("tsum_"+tago).c_str());
            stk->Write(("tstk_"+tago).c_str());

            TList *stcollists = (TList*)stk->GetHists();
            for(const auto&& obj: *stcollists){
                    TColor *color = (TColor*)gROOT->GetColor(((TH1*)obj)->GetFillColor());
                    color->Write();
            }




            std::string mean = "(Data/Pred: "+to_string_prec(NdatEvents/NeventsStack,2)+" #pm "+to_string_prec(tot_norm_error/NeventsStack,2)+")";//+"/"+to_string_prec(d0->Integral()/tsum->Integral() ,2)+")" ;
            std::string ks = "(KS: "+to_string_prec(KS_tsum_d0,3) + ")     (#chi^{2}/n_{#it{DOF}}: "+to_string_prec(mychi,2) + "/"+to_string_prec(ndof) +")    (#chi^{2} P^{val}: "+to_string_prec(TMath::Prob(mychi,ndof),3)+")";

            std::cout<<"KS CHI TEST "<<ks<<std::endl;
            // Make text file for chi^2
            // Note that this depends on e.g. "Run 1" existing in your plot_name
            // for on-beam data file
            std::string run_name;
            if      (data_file->plot_name.find("Run 1 ") != std::string::npos) run_name = "run1";
            else if (data_file->plot_name.find("Run 2 ") != std::string::npos) run_name = "run2";
            else if (data_file->plot_name.find("Run 3 ") != std::string::npos) run_name = "run3";
            else    run_name = "combined";

            std::string simple_topo_name;
            if      (data_file->topo_name.find("2#gamma1p") != std::string::npos) simple_topo_name = "2g1p";
            else if (data_file->topo_name.find("2#gamma0p") != std::string::npos) simple_topo_name = "2g0p";
            else if (data_file->topo_name.find("1#gamma1p") != std::string::npos) simple_topo_name = "1g1p";
            else if (data_file->topo_name.find("1#gamma0p") != std::string::npos) simple_topo_name = "1g0p";
            else    simple_topo_name = "unknown_topo";
            std::ofstream outfile;
            std::cout << "TESTING run_name = " << run_name << std::endl;
            outfile.open("chisq_"+ simple_topo_name+ "_"+data_file->tag+"_values_stage_"+std::to_string(s)+".txt", std::ios_base::app);
            outfile << to_string_prec(TMath::Prob(mychi, ndof),3) << std::endl;
            outfile.close();

            outfile.open("chisq_"+ simple_topo_name+ "_"+data_file->tag+"_names_stage_"+std::to_string(s)+".txt", std::ios_base::app);
            outfile << var.safe_unit << std::endl;
            outfile.close();


            // outfile.open("chisq_"+simple_topo_name+"_"+ run_name +"_stage_"+std::to_string(s)+".txt", std::ios_base::app);
            //   outfile << to_string_prec(TMath::Prob(mychi, ndof),3) << std::endl;


            std::string combined = mean + "     " +ks;
            TLatex *t = new TLatex(0.10,0.04,combined.c_str());
            t->SetNDC();
            t->SetTextColor(kRed-7);
            t->SetTextSize(0.09);
            if(!stack_mode ){
               t->Draw("same");
            }

            //var_precut.front()->GetYaxis()->SetRangeUser(0.1,ymax_pre);
            //var_precut.front()->GetYaxis()->SetTitle("Events");



            std::cout<<"Writing pdf."<<std::endl;
            cobs->Write();
            if(stack_mode){
                cobs->SaveAs(("stack/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+tago+".pdf").c_str(),"pdf");
                // cobs->SaveAs(("stack/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".root").c_str(),"root");
                // cobs->SaveAs(("stack/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".png").c_str(),"png");
            }else{
                cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+tago+".pdf").c_str(),"pdf");
            }
            //cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".png").c_str(),"png");



            delete stk;
            delete tsum;
            delete tsum_after;
            delete d0;
            delete ratunit;
            delete ratpre;
            delete rat_denom;			
            //if(!stack_mode)delete pad0top;
            delete tmp_tsum;
            delete tmp_tsum2;
            delete l0;
            delete pre;
            //  delete ratunit_after;
            //delete signal_hist;//depreciated
            //delete rat_signal;//depreciated
            delete gr;
            delete t;
            delete line;
            delete cobs;
            for(auto &h: fake_legend_hists) delete h;

            fout->Close();
        }


    }

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

        //double opAng = TMath::ACos(reco_shower_dirx->at(0)*reco_shower_dirx->at(1)+
        //        reco_shower_diry->at(0)*reco_shower_diry->at(1)+
        //        reco_shower_dirz->at(0)*reco_shower_dirz->at(1)) ;
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
           << n_vertex_x<<", "<<n_vertex_y<<", "<<n_vertex_z<< ")"<<" " 
           << E1 << " " 
           << E2 << " "
           << opAng*57.2958 << " " 
           << invMass << " " 
           << reco_track_displacement->at(i_trk->at(0)) << " " 
           << reco_track_proton_kinetic_energy->at(i_trk->at(0)) << " " << std::endl;
           */
        // Or just print run/subrun/event
        std::cout<< n_run_number <<"\t"
            <<n_subrun_number<<"\t"
            << n_event_number << std::endl;
    }
    std::cout<<"End printPassingDataEvents()  for "<<data_file->name<<std::endl;


    return 0;
}

int bdt_datamc::calcCollapsedCovariance(TMatrixD * frac_full, TMatrixD *full_coll, bdt_variable & var){

    int ctype = (var.covar_type.c_str()=="full" ? 0 : 1);
    if(ctype==0){
        std::cout<<" Running in Full-Covariance Mode "<<var.covar_type<<std::endl;
    }else{
        std::cout<<" Running in Frac-Covariance Mode "<<var.covar_type<<std::endl;
    }

    std::vector<double> full_vec = mc_stack->getEntryFullVector(var);
    TMatrixD tmp_full(frac_full->GetNrows(), frac_full->GetNcols());
    tmp_full.Zero();

    if(full_vec.size() != frac_full->GetNrows()) {
        std::cout<<"ERROR calcCollapsedCovariance: "<<full_vec.size()<<" in full_vec but frac_matrix is  "<<frac_full->GetNrows()<<" rows. Priting fullvex "<<std::endl;
        for(auto &f: full_vec) std::cout<<f<<std::endl;
        exit(EXIT_FAILURE);
    }

    if(var.n_bins != frac_full->GetNrows()/mc_stack->stack.size()){
        std::cout<<"ERROR calcCollapsedCovariance: "<<var.n_bins<<" bins in bdt_variable but input matrix is "<<frac_full->GetNrows()<<" rows and we have "<<mc_stack->stack.size()<<" bits in ou r data-mc stack"<<std::endl;
        exit(EXIT_FAILURE);
    }

    for(int i=0; i< frac_full->GetNrows();i++){
        for(int j=0; j< frac_full->GetNrows();j++){
            double pt = (*frac_full)(i,j);
            if(pt!=pt || isinf(pt)){
                if(full_vec[i] !=0 && full_vec[j]!=0){
                    //std::cout<<"We have a nan "<<pt<<" at "<<i<<" "<<j<<" "<<full_vec[i]<<" "<<full_vec[j]<<std::endl;
                }
                if(ctype==1)  pt=0.0000;
            }
            if(ctype==0){
                tmp_full(i,j) = pt;
            }else         if(ctype==1)
            {tmp_full(i,j) = pt*full_vec[i]*full_vec[j];
            }

        }
        // std::cout<<"StackCheck2 "<<i<<" "<<full_vec[i]<<" Err: "<<sqrt(tmp_full(i,i))<<" "<<(full_vec[i]>0 ? sqrt(tmp_full(i,i))/full_vec[i]*100.0 : -9 )<<std::endl;
    }
    //std::cout<<"Done"<<std::endl;
    //Going to do collapsing here, but for now just do diagonal!

    this->simpleCollapse(&tmp_full, full_coll, var);
    //fast old way
    /*
       int n_stack = mc_stack->stack.size();
       for(int i=0; i< frac_coll->GetNrows();i++){
       std::cout<<i<<std::endl; 
       for(int j = 1; j <n_stack; j++){
       int mod = var.n_bins*j;
       std::cout<<j<<" "<<mod<<" "<<i+mod<<"  / "<<tmp_full.GetNrows()<<std::endl;
       (*frac_coll)(i,i) += tmp_full(i+mod,i+mod); 
       }

       }
       */
    return 0;

}

int bdt_datamc::plotEfficiency(std::vector<bdt_variable> vars, std::vector<double> bdt_cuts, int stage_denom, int stage_numer){

    for(auto &var: vars){

        std::cout<<"Starting on variable "<<var.name<<std::endl;

        TCanvas *cobs = new TCanvas(("can_eff_"+var.safe_name+"_stage_"+std::to_string(stage_denom)).c_str(),("can_eff_"+var.safe_name+"_stage_"+std::to_string(stage_denom)).c_str(),1801,1200); //1600
        cobs->cd();


        double plot_pot=data_file->pot;

        //First get Denominator
        std::vector<TH1*> denom_stack;
        for(auto &f: mc_stack->stack){
            if(stage_denom>1) f->calcBDTEntryList(stage_denom,bdt_cuts);
            f->setStageEntryList(stage_denom);
            TH1 * h = (TH1*)f->getTH1(var, "1", std::to_string(stage_denom)+"_denom_b_"+std::to_string(bdt_cuts[stage_denom])+"_"+f->tag+"_"+var.safe_name, plot_pot);
            denom_stack.push_back(h);
        }	
        if(stage_denom>1) data_file->calcBDTEntryList(stage_denom,bdt_cuts);
        data_file->setStageEntryList(stage_denom);

        cobs->cd();
        TH1 * tsum_denom = (TH1*)mc_stack->getEntrySum(var,stage_denom);
        cobs->cd();
        TH1 * data_denom = (TH1*)data_file->getTH1(var, "1", std::to_string(stage_denom)+"_denom_"+std::to_string(bdt_cuts[stage_numer])+"_"+data_file->tag+"_"+var.safe_name, plot_pot);


        //Now Numerator
        std::vector<TH1*> numer_stack;
        for(auto &f: mc_stack->stack){
            if(stage_numer>1) f->calcBDTEntryList(stage_numer,bdt_cuts);
            f->setStageEntryList(stage_numer);
            TH1 * h = (TH1*)f->getTH1(var, "1", std::to_string(stage_numer)+"_numer_b_"+std::to_string(bdt_cuts[stage_numer])+"_"+f->tag+"_"+var.safe_name, plot_pot);
            numer_stack.push_back(h);
        }	
        if(stage_numer>1) data_file->calcBDTEntryList(stage_numer,bdt_cuts);
        data_file->setStageEntryList(stage_numer);

        cobs->cd();
        TH1 * tsum_numer = (TH1*)mc_stack->getEntrySum(var,stage_numer);
        cobs->cd();
        TH1 * data_numer = (TH1*)data_file->getTH1(var, "1", std::to_string(stage_numer)+"_numer_"+std::to_string(bdt_cuts[stage_numer])+"_"+data_file->tag+"_"+var.safe_name, plot_pot);


        cobs->cd();
        tsum_numer->Divide(tsum_denom);
        data_numer->Divide(tsum_denom);

        /*
        //make THStack of divisions
        THStack *stacked = new THStack((var.safe_name+"_stack").c_str(), (var.safe_name+"_stack").c_str());
        for(int i=0; i<denom_stack.size(); i++){
        numer_stack[i]->Divide(denom_stack[i]);
        numer_stack[i]->Scale(tsum_numer->Integral()/numer_stack[i]->Integral());
        stacked->Add(numer_stack[i]);
        }*/


        tsum_numer->SetLineColor(kRed);
        tsum_numer->SetMinimum(0);
        tsum_numer->SetMaximum( std::max(tsum_numer->GetMaximum(),data_numer->GetMaximum())*1.5);
        tsum_numer->SetFillStyle(0);
        tsum_numer->Draw("hist");
        TH1 * tsum_numer2 = (TH1*)tsum_numer->Clone("varsd");
        tsum_numer2->SetFillStyle(3354);
        tsum_numer2->DrawCopy("E2 same");
        tsum_numer->GetYaxis()->SetTitle("Efficiency");
        tsum_numer->SetTitle("");

        data_numer->SetMarkerSize(2);
        data_numer->SetMarkerStyle(20);
        data_numer->Draw("E1P same");
        //      stacked->Draw("hist same");

        TLegend *l = new TLegend(0.59,0.89,0.59,0.89);
        l->AddEntry(tsum_numer2,"MC Efficiency","fl");
        l->AddEntry(data_numer,"Data Effciency","lp");
        l->SetHeader(("Stage "+std::to_string(stage_numer)+"/"+std::to_string(stage_denom)).c_str());
        l->SetLineColor(kWhite);
        l->SetLineWidth(0);
        l->Draw();

        cobs->SaveAs(("efficiency/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(stage_numer)+"_over_"+std::to_string(stage_denom)+".pdf").c_str(),"pdf");
    }
    return 0 ; 
}



int bdt_datamc::simpleCollapse(TMatrixD * Min, TMatrixD * Mout, bdt_variable & var){
    //Ripped directly from...
    //void SBNchi::CollapseSubchannels(TMatrixT <double> & M, TMatrixT <double> & Mc)
    int num_channels = 1;
    std::vector<int> num_subchannels = {(int)mc_stack->stack.size()};
    std::vector<int> num_bins = {var.n_bins};

    std::vector<std::vector<TMatrixT<double>>> Summed(num_channels, std::vector<TMatrixT<double>>(num_channels) );	//Initialise a matrix of matricies, to ZERO.
    for(int ic = 0; ic < num_channels; ic++){
        for(int jc =0; jc < num_channels; jc++){
            Summed[ic][jc].ResizeTo(num_bins[jc],num_bins[ic]) ;// This is CORRECT, do not switch (ie Summed[0][1] = size (num_bins[1], num_bins[0])
            Summed[ic][jc] = 0.0;
        }
    }

    int mrow = 0.0;
    int mcol = 0.0;

    for(int ic = 0; ic < num_channels; ic++){ 	 //Loop over all rows
        for(int jc =0; jc < num_channels; jc++){ //Loop over all columns

            for(int m=0; m < num_subchannels[ic]; m++){
                for(int n=0; n< num_subchannels[jc]; n++){ //For each big block, loop over all subchannels summing toGether
                    Summed[ic][jc] +=  Min->GetSub(mrow+n*num_bins[jc] ,mrow + n*num_bins[jc]+num_bins[jc]-1, mcol + m*num_bins[ic], mcol+ m*num_bins[ic]+num_bins[ic]-1 );
                }
            }

            mrow += num_subchannels[jc]*num_bins[jc];//As we work our way left in columns, add on that many bins
        }//end of column loop

        mrow = 0; // as we end this row, reSet row count, but jump down 1 column
        mcol += num_subchannels[ic]*num_bins[ic];
    }//end of row loop

    ///********************************* And put them back toGether! ************************//
    Mout->Zero();
    mrow = 0;
    mcol = 0;

    //Repeat again for Contracted matrix
    for(int ic = 0; ic < num_channels; ic++){
        for(int jc =0; jc < num_channels; jc++){

            Mout->SetSub(mrow,mcol,Summed[ic][jc]);
            mrow += num_bins[jc];
        }

        mrow = 0;
        mcol +=num_bins[ic];
    }

    return 0 ;

}


double bdt_datamc::calcTotalNormError(TMatrixD * Min, bdt_variable & var){
    //Ripped directly from...
    //void SBNchi::CollapseSubchannels(TMatrixT <double> & M, TMatrixT <double> & Mc)
    int num_channels = 1;
    std::vector<int> num_subchannels = {var.n_bins};
    std::vector<int> num_bins = {1};
    TMatrixD * Mout = new TMatrixD(1,1);

    std::vector<std::vector<TMatrixT<double>>> Summed(num_channels, std::vector<TMatrixT<double>>(num_channels) );	//Initialise a matrix of matricies, to ZERO.
    for(int ic = 0; ic < num_channels; ic++){
        for(int jc =0; jc < num_channels; jc++){
            Summed[ic][jc].ResizeTo(num_bins[jc],num_bins[ic]) ;// This is CORRECT, do not switch (ie Summed[0][1] = size (num_bins[1], num_bins[0])
            Summed[ic][jc] = 0.0;
        }
    }

    int mrow = 0.0;
    int mcol = 0.0;

    for(int ic = 0; ic < num_channels; ic++){ 	 //Loop over all rows
        for(int jc =0; jc < num_channels; jc++){ //Loop over all columns

            for(int m=0; m < num_subchannels[ic]; m++){
                for(int n=0; n< num_subchannels[jc]; n++){ //For each big block, loop over all subchannels summing toGether
                    Summed[ic][jc] +=  Min->GetSub(mrow+n*num_bins[jc] ,mrow + n*num_bins[jc]+num_bins[jc]-1, mcol + m*num_bins[ic], mcol+ m*num_bins[ic]+num_bins[ic]-1 );
                }
            }

            mrow += num_subchannels[jc]*num_bins[jc];//As we work our way left in columns, add on that many bins
        }//end of column loop

        mrow = 0; // as we end this row, reSet row count, but jump down 1 column
        mcol += num_subchannels[ic]*num_bins[ic];
    }//end of row loop

    ///********************************* And put them back toGether! ************************//
    Mout->Zero();
    mrow = 0;
    mcol = 0;

    //Repeat again for Contracted matrix
    for(int ic = 0; ic < num_channels; ic++){
        for(int jc =0; jc < num_channels; jc++){

            Mout->SetSub(mrow,mcol,Summed[ic][jc]);
            mrow += num_bins[jc];
        }

        mrow = 0;
        mcol +=num_bins[ic];
    }

    return (*Mout)(0,0) ;

}



int bdt_datamc::calcChi2(TH1 *mc_hist, TH1 *data_hist) {

    //int bdt_datamc::calcChi2(std::vector<bdt_file> *stack_files, bdt_file *data_file) {
    /*

       double mychi =0;
       int ndof = 0;
       for(int p=0; p<data_hist->GetNbinsX();p++){

       double da = data_hist->GetBinContent(p+1);
       double bk = mc_hist->GetBinContent(p+1);

       for (size_t i = 0; i < stack_files.size(); i++) {
       bk+=stack_files.at(i)->GetBinContent(p+1);
       }

       if ( bk ==0){
       std::cout<<"ERROR mychi, for bin "<<p<<" n_data= "<<da<<" and n_mc= "<<bk<<std::endl;

       } else{

       double da_err = sqrt(mc_hist->GetBinContent(p+1));
       double bk_err = mc_hist->GetBinError(p+1);

       double tk = pow(da-bk,2)/(da_err*da_err+bk_err*bk_err);

       std::cout<<da<<" "<<bk<<" "<<da_err<<" "<<bk_err<<" total: "<<sqrt(da_err*da_err+bk_err*bk_err)<<" chi^2 "<<tk<< std::endl;
       if(tk==tk){
       mychi+=tk;
       ndof++;
       }
       }
       }

    // Added by A. Mogan 1/13/20 for easy reference in the scalenorm mode_option
    std::cout << "[SCALENORM]: chi^2/NDF: " << mychi << " / " << ndof << " = " << mychi/ndof << std::endl;
    */
    return 0;
}

void bdt_datamc::scaleNorm(std::vector<bdt_variable> var, std::vector<bdt_file*> stack_files, double scaleLow, double scaleHigh, double scaleStep, int stage, std::vector<double> bdt_cuts, std::string analysis_tag) {
    /*

       bdt_variable tmp_var = var[0];
       double plot_pot=data_file->pot;
       if(stack_mode) plot_pot = stack_pot;

       for (double s = scaleLow; s < scaleHigh; s+=scaleStep) {
    //TH1 *tsum = (TH1*)mc_stack->getEntrySum(tmp_var,stage);
    bdt_stack *tmp_stack = new bdt_stack(analysis_tag+"_scalenorm_"+std::to_string(s) );
    for(size_t f =0; f< stack_files.size(); ++f){
    if(stack_files[f]->is_data) continue;
    TH1 *tmp_mc = (TH1*)stack_files[f]->getTH1(tmp_var, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[stage])+"_"+stack_files[f]->tag+"_"+tmp_var.safe_name, plot_pot);
    // NOTE: Make sure signal is the first element of the stack vector
    if (f==0) tmp_mc->Scale(s);
    tmp_stack->Add(tmp_mc);
    std::cout<<"adding to stack: "<<stack_files[f]->tag<<std::endl;
    }
    TH1 *d0 = (TH1*)data_file->getTH1(tmp_var, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[stage])+"_"+data_file->tag+"_"+tmp_var.safe_name, plot_pot);
    this->calcChi2(tsum, d0);
    }
    return 0;
    }

*/
return;
}

// Added 5/12/20 by A. Mogan
/*
   TMatrixD CalcCovarianceMatrixCNP(TMatrixD *M, std::vector<double> spec, const std::vector<double> datavec ) {

   TMatrixT<double> Mout(M->GetNcols(), M->GetNcols() );

   for(int i =0; i<M->GetNcols(); i++)
   {
   for(int j =0; j<M->GetNrows(); j++)
   {
   if(std::isnan( (*M)(i,j) )){
   Mout(i,j) = 0.0;
   }else{
   Mout(i,j) = (*M)(i,j)*spec[i]*spec[j];
   }
   if(i==j) Mout(i,i) +=   ( datavec[i] >0.001 ? 3.0/(1.0/datavec[i] +  2.0/spec[i])  : spec[i]/2.0 );
   }
   }
   return Mout;
   }
   */












