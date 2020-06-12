#include "bdt_datamc.h"


//This is called in the hive.cxx. It is used var2D for reading var1,var2,var3 argument
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


//int bdt_datamc::plotStacks(TFile *ftest, bdt_variable var,double c1, double c2){
//   
//	std::cout<<"Obselete See "<<__FILE__<<__LINE__<<std::endl;
//	return 0;
//
//    is_bdt_variable = false;
//    bdt_info nullinfo;
//    return this->plotStacks(ftest, var, c1,c2,nullinfo);
//}

//int bdt_datamc::plotBDTStacks(TFile *ftest, bdt_info whichbdt,double c1, double c2){
//	std::cout<<"Obselete See "<<__FILE__<<__LINE__<<std::endl;
//	return 0;
//
//    is_bdt_variable = true;
//
//    double tmin =9999;
//    double tmax = -9999;
//
//    /*
//       for(auto &f: mc_stack->stack){
//       std::cout<<"TAG: "<<f->tag<<" "<<tmax<<" "<<tmin<<std::endl;
//
//       TH1 * tmp = f->getTH1((f->tag +"_"+whichbdt.identifier+ ".mva") ,(f->tag +"_"+whichbdt.identifier+ ".mva > 0")  , "tmpBDtstack_"+data_file->tag+"_"+whichbdt.name,1,0);
//
//       tmax = std::max( tmax, f->tvertex->GetMaximum( (f->tag +"_"+whichbdt.identifier+ ".mva").c_str()   ));
//       if(f->tag!="NCDeltaRadCosmics") tmin = std::min( tmin, tmp->GetBinCenter(tmp->FindFirstBinAbove(0.05)));
//       std::cout<<"TAG: "<<f->tag<<" "<<tmax<<" "<<tmin<<std::endl;
//       delete tmp;
//       }
//
//       std::string  binning = "(20,"+std::to_string(tmin*0.975)+","+std::to_string(tmax*1.025)+")";
//       */
//    std::string  binning = whichbdt.binning;
//
//    bdt_variable dvar = data_file->getBDTVariable(whichbdt, binning);
////    dvar.is_logplot = true;
//
//    /*std::vector<bdt_variable> tmp_v = {dvar};
//
//      return this->plotStacks(ftest, tmp_v ,c1,c2);
//      */
//
//    //run on old one
//    return this->plotStacks(ftest, dvar,c1,c2,whichbdt);
//}

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
    std::vector<double> *n_reco_shower_startx;
    std::vector<double> *n_reco_shower_starty;
    std::vector<double> *n_reco_shower_startz;
    std::vector<double> *n_reco_shower_energy_max;
    std::vector<double> *n_reco_shower_theta_yz;
    std::vector<double> *n_reco_shower_phi_yx;
    std::vector<double> *n_reco_shower_dirx;
    std::vector<double> *n_reco_shower_diry;
    std::vector<double> *n_reco_shower_dirz;

    // Necessary for vectors, for some reason
	//
    n_reco_shower_startx = 0;
    n_reco_shower_starty = 0;
    n_reco_shower_startz = 0;
    n_reco_shower_energy_max = 0;
    n_reco_shower_theta_yz = 0;
    n_reco_shower_phi_yx = 0;
    n_reco_shower_dirx = 0;
    n_reco_shower_diry = 0;
    n_reco_shower_dirz = 0;

    data_file->tvertex->SetBranchAddress("run_number",    &n_run_number);
    data_file->tvertex->SetBranchAddress("subrun_number", &n_subrun_number);
    data_file->tvertex->SetBranchAddress("event_number",  &n_event_number);
    data_file->tvertex->SetBranchAddress("reco_shower_startx", &n_reco_shower_startx);
    data_file->tvertex->SetBranchAddress("reco_shower_starty", &n_reco_shower_starty);
    data_file->tvertex->SetBranchAddress("reco_shower_startz", &n_reco_shower_startz);
    data_file->tvertex->SetBranchAddress("reco_shower_energy_max", &n_reco_shower_energy_max);
    data_file->tvertex->SetBranchAddress("reco_shower_theta_yz", &n_reco_shower_theta_yz);
    data_file->tvertex->SetBranchAddress("reco_shower_phi_yx", &n_reco_shower_phi_yx);
    data_file->tvertex->SetBranchAddress("reco_shower_dirx", &n_reco_shower_dirx);
    data_file->tvertex->SetBranchAddress("reco_shower_diry", &n_reco_shower_diry);
    data_file->tvertex->SetBranchAddress("reco_shower_dirz", &n_reco_shower_dirz);


    std::cout<<"Starting printPassingDataEvents() for "<<data_file->name<<std::endl;
	std::cout<<"Index entry# run_number subrun_number event_number reco_shower_startx ";
	std::cout<<"reco_shower_starty reco_shower_startz reco_shower_energy_max ";
	std::cout<<"reco_shower_theta_y reco_shower_phi_yx"<<std::endl;

    for(int i=0;i < fake_list->GetN(); i++ ){
        data_file->tvertex->GetEntry( fake_list->GetEntry(i));
        std::cout<<i<<" "<<fake_list->GetEntry(i)<<" "<<n_run_number<<" "<<n_subrun_number<<" ";
		std::cout<<n_event_number<<" ";
		std::cout<<n_reco_shower_startx->at(0)<<" "<<n_reco_shower_starty->at(0)<<" "<<n_reco_shower_startz->at(0)<<" ";
		std::cout<<n_reco_shower_energy_max->at(0)<<" "<<n_reco_shower_theta_yz->at(0)<<" "<<n_reco_shower_phi_yx->at(0)<<" ";
		std::cout<<n_reco_shower_dirx->at(0)<<" "<<n_reco_shower_diry->at(0)<<" "<<n_reco_shower_dirz->at(0);
		std::cout<<std::endl;
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


//int bdt_datamc::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, double c1, double c2){
//	std::cout<<"Obselete See "<<__FILE__<<__LINE__<<std::endl;
//	return 0;
//
////    return this->plotStacks(ftest,vars,{c1,c2});
//}



//This gives out 2D comparison; each slot contains information of (Data-MC)/sqrt(MC)
//It aims to identify data excess region in given two variables.
//modified based on plot2D() function;
int bdt_datamc::plot2D_DataMinusMC(TFile *ftest, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts){
    if (vars.size() < 2){
        std::cout<<"Need min 2 vars to make DataMinusMC 2D plots, try again!"<<std::endl;
        return 0;
    }

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


    for(int s = s_min; s< s_max; s++){//stage loop

        std::cout<<"On stage: "<<s<<std::endl;
        //First set the files at this stage
        for(auto &f: mc_stack->stack){
            std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<" On stage "<<s<<"."<<std::endl;
            if(s>1) f->calcBDTEntryList(s,bdt_cuts);//I think the weight branch is taken care here, Keng.
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

            for(int j = 0; j < vars.size(); j++){//only want to plot different variables, but also not duplicate i.e. 12 and 21
                if (i!= j && i < j){//Create TH2* for data, then substract mc events from that TH2*.
				//STEP 1, prepare d0, data; mc's, all non-data files;
                    bdt_variable var2= vars[j];
                    std::cout<<"Starting on variable "<<var1.unit<<" and "<<var2.unit<<std::endl;

					//Set up data histogram ,d0, from data_file
                    TH2 * d0 = (TH2*)data_file->getTH2(var1,var2, "1", std::to_string(s)+"_d0_"+std::to_string(bdt_cuts[s])+"_"+data_file->tag+"_"+var1.safe_unit+"_"+var2.safe_unit, plot_pot);
					TH2 * non_d0;//fill in non_d0 histogram from the following loop
					
					bool copy_this = true;
					for(auto &f: mc_stack->stack){//introduce MC's and substract them from d0;

						std::cout<<"Stack "<<f->tag<<" level "<<s<<std::endl;
						

						TH2* this_mc= (TH2*) f->getTH2(var1,var2, "1", std::to_string(s)+"_mc_"+std::to_string(bdt_cuts[s])+"_"+f->tag+"_"+var1.safe_unit+"_"+var2.safe_unit, plot_pot);

						if(copy_this){//first TH2 histogram is created through Clone(), note Clone() guarantees the original TH2 unchanged.
							non_d0 = (TH2*) this_mc->Clone();
							copy_this = false;
						}else{
							non_d0->Add(this_mc,1.);//add this_mc histogram.
						}
						this_mc = 0;
					}//for each item in the mc stack

				//STEP 2, do substractions;
					TH2* DataMinusMC = (TH2*)d0->Clone();//Make a new TH2 and add d0, pure data;
					DataMinusMC->Add(non_d0,-1.);//do the substraction; not done yet, need to weight the excess according to d0;
				
				//STEP 2.5, weight the excess according to # of predicted events, i.e. (#data-#MC)/sqrt(#MC);
					if(true){//true, do the weighting
						//std::cout<<"Weight the th2d"<<std::endl;
						Int_t xbin = d0->GetNbinsX();
						Int_t ybin = d0->GetNbinsY();

						for(Int_t xaxis = 1; xaxis<xbin+1; ++xaxis){//fill along x-axis
						double count_y = 0;
							for(Int_t yaxis = 1; yaxis<ybin+1; ++yaxis){//fill along y-axis
								double num_DataMinusMC = DataMinusMC->GetBinContent(xaxis,yaxis);
								double num_mc = non_d0->GetBinContent(xaxis,yaxis);
								double slot_value = (num_mc==0)?0:num_DataMinusMC/sqrt(num_mc);
//								std::cout<<"data: "<< num_DataMinusMC<<" mc: "<<num_mc;
//								std::cout<<"(DataMinusMC)/sqrt(mc) = "<< slot_value<<" "<<std::endl;
								DataMinusMC->SetBinContent(xaxis,yaxis,slot_value);
								count_y+= d0->GetBinContent(xaxis,yaxis);
//								count_y+= num_mc;
							}
						std::cout<<"Bin "<<xaxis<<" has # events "<<count_y<<std::endl;
						}
					}

				//STEP 3, Prepare side plots (top and right)
					TH1D* projected_MC = non_d0->ProjectionX();//Projected TH2
					TH1D* projected_MCy = non_d0->ProjectionY();//Projected TH2
					TH1D* projected_data = d0->ProjectionX();//Projected TH2
					TH1D* projected_datay = d0->ProjectionY();//Projected TH2

				//STEP 4, put DataMinusMC and projections on top and right together;
                    //Create a TCanvas and prepare a TPad.
                    TCanvas *cobs = new TCanvas(("can_"+var1.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)).c_str(),1800,1600);
                    cobs->cd();

					TPad *main_pad = new TPad(("mainpad_"+stage_names.at(s)).c_str(), ("mainpad_"+stage_names.at(s)).c_str(), 0, 0, 0.49, 0.49);
					main_pad->Draw();

					TPad *top_pad = new TPad(("toppad_"+stage_names.at(s)).c_str(), ("toppad_"+stage_names.at(s)).c_str(), 0, 0.51, 0.49, 1.0);
					top_pad->Draw();

					TPad *right_pad = new TPad(("rightpad_"+stage_names.at(s)).c_str(), ("rightpad_"+stage_names.at(s)).c_str(), 0.51, 0, 1.0, 0.49);
					right_pad->Draw();

					TPad *upperright_pad = new TPad(("rightpad_"+stage_names.at(s)).c_str(), ("rightpad_"+stage_names.at(s)).c_str(), 0.51, 0.51, 1.0, 1.0);
					upperright_pad->Draw();

				//STEP 4.1, draw the mainpad (bottom left)
					main_pad->cd();
                    DataMinusMC->Draw("COLZ");
                    DataMinusMC->SetTitle("[Data-MC)]/#sqrt{MC}");
                    DataMinusMC->GetXaxis()->SetTitle((var1.unit).c_str());
                    DataMinusMC->GetXaxis()->SetTitleSize(0.04);
//                    DataMinusMC->GetXaxis()->SetTitleOffset(1.2);
                    DataMinusMC->GetYaxis()->SetTitle((var2.unit).c_str());
                    DataMinusMC->GetYaxis()->SetTitleSize(0.04);
                    DataMinusMC->GetYaxis()->SetTitleOffset(1.2);
//                    main_pad->SetRightMargin(0.3);
				
				//STEP 4.2, draw the toppad (upper left)
					top_pad->cd();//draw on the top panel
					projected_MC->SetFillColor(kRed-2);
					projected_MC->SetTitle(("Spectrum of "+var1.unit).c_str());
                    projected_MC->GetYaxis()->SetTitle("Event Rate");
                    projected_MC->GetXaxis()->SetTitleOffset(1.1);
                    projected_MC->GetXaxis()->SetTitleSize(0.04);
					double max1 = 1.2*std::max(projected_MC->GetMaximum(),projected_data->GetMaximum());
					projected_MC->SetMaximum(max1);
					projected_MC->SetMinimum(0);
					projected_MC->SetStats(false);
					projected_MC->Draw("hist");

					projected_data->SetMarkerStyle(20);
					projected_data->SetMarkerSize(2);
					projected_data->Draw("same E");

				//STEP 4.3, draw the rightpad (bottom right)
					right_pad->cd();//draw on the right panel
					projected_MCy->SetFillColor(kRed-2);
					projected_MCy->SetTitle(("Spectrum of "+var2.unit).c_str());
					projected_MCy->GetYaxis()->SetTitle("Event Rate");
                    projected_MCy->GetXaxis()->SetTitleOffset(1.1);
                    projected_MCy->GetXaxis()->SetTitleSize(0.04);
					double max2 = 1.2*std::max(projected_MCy->GetMaximum(),projected_datay->GetMaximum());
					projected_MCy->SetMaximum(max2);
					projected_MCy->SetMinimum(0);
					projected_MCy->SetStats(false);
					projected_MCy->Draw("hist");

					projected_datay->SetMarkerStyle(20);
					projected_datay->SetMarkerSize(2);
					projected_datay->SetLineColor(kBlack);
//					projected_datay->SetBinErrorOption(TH1::kPoisson);
					projected_datay->Draw("same");//CHECK, dont know how to draw horizontal dot
				
				//STEP 4.4 Add POT info. to the upper right pad.
					upperright_pad->cd();
					TLatex pottex;
					pottex.SetTextSize(0.06);
					pottex.SetTextAlign(13);  //align at top
					pottex.SetNDC();

					double pot_unit = 1e20;
					std::string pot_unit_s = "e20";
					std::string pot_draw = data_file->topo_name+" "+to_string_prec(plot_pot/pot_unit,1)+ pot_unit_s+" POT";
					std::string description = "Stage " + std::to_string(s)+" "+stage_names[s];
					pottex.DrawLatex(.10,.40, description.c_str());
					pottex.DrawLatex(.20,.30, pot_draw.c_str());
					//legend
					TLegend *legend = new TLegend(0, 0.1,0.8,0.2);
					legend->SetNColumns(2);
					legend->AddEntry(projected_datay, "Data","P");
					legend->AddEntry(projected_MCy, "All MC","F");
					legend->Draw();

                    std::cout<<"Writing png and pdf."<<std::endl;
                    cobs->Write();
                    cobs->SaveAs(("var2D/"+tag+"_"+data_file->tag+"_"+var1.safe_unit+"_"+var2.safe_unit+"_DataMinusMC_stage_"+std::to_string(s)+".pdf").c_str(),"pdf");
                    cobs->SaveAs(("var2D/"+tag+"_"+data_file->tag+"_"+var1.safe_unit+"_"+var2.safe_unit+"_DataMinusMC_stage_"+std::to_string(s)+".png").c_str(),"png");

					//delete DataMinusMC;//DataMinusMC
					DataMinusMC = 0;
                    delete cobs;
                    delete d0;//data
                    delete non_d0;//MC
					delete projected_MC;
                    delete projected_MCy;
                    delete projected_data;
                    delete projected_datay;
                }//if different variables and haven't already used the combo
            }//end of var2 loop
        }//end of var1 loop
    }//end of stage loop

    return 0;
}


//ordinary 2D comparison for each samples;
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

    std::vector<std::string> stage_names = {"Topological Selection","Pre-Selection Cuts","Final Selection","BNB BDT cut","NCPi0 BDT Cut","NUE BDT Cut","tmp"};
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
                    TCanvas *cobs = new TCanvas(("can_"+var1.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)).c_str(),1700,1600);
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
					
				TH2* grouped_2dhist;
				bool first_time = true;
				TString tem_output_name;
				std::string tem_title;

               for(auto &f: mc_stack->stack){

                    std::cout<<"Stack "<<f->tag<<" level "<<s<<std::endl;
                    TCanvas *cobsmc = new TCanvas(("can_"+var1.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)).c_str(),1800,1600);
                    cobsmc->cd();

                    TPad *padmc = new TPad(("pad_"+stage_names.at(s)).c_str(), ("pad_"+stage_names.at(s)).c_str(), 0, 0, 1, 1.0);
                    padmc->Draw();
                    padmc->cd();

                                
                    TH2 * mc = (TH2*)f->getTH2(var1,var2, "1", std::to_string(s)+"_mc_"+std::to_string(bdt_cuts[s])+"_"+f->tag+"_"+var1.safe_unit+"_"+var2.safe_unit, plot_pot);
                    padmc->cd();
					
					//to group or not to group
					if( f->group != -1 ){//any group number != -1 will be considered as the same group;
						if(first_time){
							grouped_2dhist = (TH2*) mc->Clone();
							tem_output_name =  "var2D/"+tag+"_"+f->tag+"_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)+".pdf";
							first_time = false;
							tem_title = f->plot_name;
						} else{
							grouped_2dhist->Add(mc);
						}
					}
					
					//End of grouping
					
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

//                    cobsmc->SaveAs(("var2D/"+tag+"_"+f->tag+"_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)+".png").c_str(),"png");

					delete cobsmc;
                    delete mc;

                    }//for each item in the mc stack
					{//make up the grouped 2dhist;
                    TCanvas *cobsmc = new TCanvas(("can_"+var1.safe_name+"_stage_"+std::to_string(s)).c_str(),("can_"+var1.safe_unit+"_"+var2.safe_unit+"_stage_"+std::to_string(s)).c_str(),1800,1600);
                    cobsmc->cd();

                    TPad *padmc = new TPad(("pad_"+stage_names.at(s)).c_str(), ("pad_"+stage_names.at(s)).c_str(), 0, 0, 1, 1.0);
                    padmc->Draw();
                    padmc->cd();

					
                    grouped_2dhist->Draw("COLZ");
                    grouped_2dhist ->SetTitle((tem_title + ", stage " + std::to_string(s)).c_str());
                    grouped_2dhist->GetYaxis()->SetTitleSize(0.05);
                    grouped_2dhist->GetYaxis()->SetTitleOffset(0.9);
                    grouped_2dhist->GetXaxis()->SetTitleSize(0.05);
                    grouped_2dhist->GetXaxis()->SetTitleOffset(0.9);
                    padmc->SetRightMargin(0.15);
                    std::cout<<"Writing pdf."<<std::endl;
                    cobsmc->Write();
                    cobsmc->SaveAs(tem_output_name,"pdf");

					delete cobsmc;
					delete grouped_2dhist;
					}


                }//if different variables and haven't already used the combo

            }//var2
        }//var1
    }//stage

    return 0;
}



//main code for plotting the stack histogram;
int bdt_datamc::plotStacks(TFile *ftest, std::vector<bdt_variable> vars, std::vector<double> bdt_cuts, std::vector<bdt_info> bdt_infos){

	bool print_message = false;
	bool debug_message = false;
	double plot_pot=data_file->pot;
//	if(stack_mode) plot_pot = stack_pot;//always false for now;

	std::cout<<"DATAMC PLOT POT "<<plot_pot<<" from "<<data_file->tag<<std::endl;

	double title_size_ratio=0.1;
	double label_size_ratio=0.085;
	double title_offset_ratioY = 0.3 ;
	double title_offset_ratioX = 1.1;

	double title_size_upper=0.06;
	double label_size_upper=0.05;
	double title_offset_upper = 0.6;

	//ratio plot setting 
	double rmin = 0;//minimum ratio
	double rmax = 1.99;//maximum ratio
	int data_rebin = 1;//change binning of data, sometimes we want to group them when data are few

	//stack plot sttting
	double min_val = 0.01;//minimum events count
	double max_modifier = 2;//a factor to be applied according to the highest bin, for the maximum y

	//POT info display
	double pot_unit = 1e20;
	std::string pot_unit_string = "e20";

	ftest->cd();//output root file to storage histograms.
	std::vector<std::string> stage_names = {"Topological Selection","Pre-Selection Cuts"};
	for(size_t index = 0; index < bdt_infos.size(); ++index){
		stage_names.push_back(bdt_infos[index].name);//bdt_infos gives stage name
	}
	//Loop over all stages

	int s_min = 0;
	int s_max = bdt_cuts.size()+2;
	if(plot_stage >=0){
		s_min = plot_stage;
		s_max = plot_stage+1;
	}

	for(int stage = s_min; stage< s_max; stage++){//stages
	//STEP 1: Pick a stage, prepare entrylist;
		std::string current_stage = stage_names.at(stage);

		std::cout<<"On stage: "<<stage<<std::endl;
		//First set the files at this stage
		//bdt_stack mc_stack contains member vector<bdt_file* > stack;
		for(bdt_file* &f: mc_stack->stack){
			if(stage>1) f->calcBDTEntryList(stage,bdt_cuts);
			f->setStageEntryList(stage);
			if(print_message){
				if(stage>1) std::cout<<"Calculating any necessary EntryLists for "<<f->tag<<"."<<std::endl;
				std::cout<<"Setting up EntryLists for "<<f->tag<<"."<<std::endl;
			}
		}

//		std::cout<<"Done with computations on TTrees and bdt_stacks"<<std::endl;
		if(stage>1) data_file->calcBDTEntryList(stage,bdt_cuts);
		data_file->setStageEntryList(stage);

		for(auto &var: vars){//variables
		//STEP 2: work on variables one by one;
			//Following quantities need to be cleared for each variable for each stage
			//Container for MC event numbers
			std::vector<double> Events_num;
			double total_MC_events = 0;
			double total_MCbkg_events = 0;
			double NdatEvents = 0;
			//Statistical Quantity
			double mychi =0;
			int ndof = 0;


			std::cout<<"Starting on variable "<<var.name<<std::endl;
			if(print_message){
				if(var.is_logplot){std::cout<<"MAKEING A LOG PLOT"<<std::endl;}
				else{std::cout<<"NOT MAKEING A LOG PLOT"<<std::endl;}
			}

		//STEP 2.1: Prepare ROOT objects
			//2.1.A Pre-calculation for stat quantity
			//Check Covar for plotting
			TMatrixD * covar_collapsed = new TMatrixD(var.n_bins,var.n_bins);
			TString tsum_name;
			TString legend_style;

			//2.1.B TCanvas, TPad
			TCanvas *cobs = new TCanvas(("can_"+var.safe_name+"_stage_"+std::to_string(stage)).c_str(),("can_"+var.safe_name+"_stage_"+std::to_string(stage)).c_str(),(stack_mode)? 2200 : 1801,1400); //Create a canvas 1801x1400 (width x height)
			TPad *pad0top = new TPad(("pad0top_"+current_stage).c_str(), ("pad0top_"+current_stage).c_str(), 0, (stack_mode)? 0.0 : 0.4, 1, 1.0);
			TPad *pad0bot = new TPad(("padbot_"+current_stage).c_str(),("padbot_"+current_stage).c_str(), 0, 0.05, 1, 0.4);

			if(false&&do_subtraction){//not doing substraction now
				std::cout<<"Setting do Subtraction inside bdt_stack "<<std::endl;
				mc_stack->setSubtractionVector(subtraction_vec);
			}

			//2.1.C Histograms
			//upper plot
			THStack* stk = (THStack*)mc_stack->getEntryStack(var,stage);//Stack histogram (MC events);
			TH1* tsum = (TH1*)mc_stack->getEntrySum(var,stage);//Histogram of the total MC, use this to calculate stat errors;

			std::string temp_data_name = std::to_string(stage)+"_d0_"+std::to_string(bdt_cuts[stage])+"_"+data_file->tag+"_"+var.safe_name;
			TH1* d0 = (TH1*)data_file->getTH1(var, "1", temp_data_name, plot_pot);//Data points.

			TLegend *l0 = new TLegend(0.14,0.65,0.89,0.89);//Legend
//			TH1* leg_hack = (TH1*)tsum->Clone(("leg_tmp_tsum"+std::to_string(stage)).c_str());//legend?
//			std::vector<TH1F*> fake_legend_hists;
			
			//bottom plot

			TH1* ratio_data = (TH1*)d0->Clone(("ratio_"+current_stage).c_str());//ratio based on data histogram, black dot bottom plot;
			TH1 *ratio_bkgMC;//Histogram for backgroudn MC, new need to implement this;
			TH1* ratunit = (TH1*)tsum->Clone(("ratio_unit_"+current_stage).c_str());//The base of the bottom plot, shaded region; is this error bars for dividing by itself?
			TGraphAsymmErrors * ratio_data_err;//Error for the ratio points;

			
			//2.1.D Texts
			TText *title;//upper left plot title; 
			TText *stage_label;//upper right stage label,
			TLatex pottex;//POT info.
			TLatex *estimators;//Data/MC ratio, KS Test, Chi^2/(nDoF), Chi^2 p-value; yea, there is another way of drawing TLatex



		//STEP 2.2: Prepare Contents for ROOT objects, items A-E above.
			//STEP 2.2.A: Pre-calculation for stat quantity
			if(false &&do_subtraction){//substraction items from histogram
				std::cout<<"Actually doing the subtracting"<<std::endl;
				for(int i=0; i< subtraction_vec.size();i++){
					if(subtraction_vec[i]){
						std::cout<<"Subtracting: "<<i<<std::endl;
						mc_stack->vec_hists[i]->Rebin(data_rebin);
						d0->Add((mc_stack->vec_hists[i]),-1.0);
						tsum->Add((mc_stack->vec_hists[i]),-1.0);
					}
				}
			}

			Events_num.clear();//reset events# 
			for(auto &f: mc_stack->stack){//calculate # of MC events of each samples;
				double Nevents = f->GetEntries()*(plot_pot/f->pot)*f->scale_data;
				Events_num.push_back(Nevents);
				if(!f->is_signal) total_MCbkg_events += Nevents;
				if(debug_message) std::cout<<"Sample "<<std::setw(16)<<f->tag<<" has events "<<Nevents<<std::endl;
			}
			total_MC_events =  std::accumulate(Events_num.begin(),Events_num.end(),0.0);//sum up all events, note that  we need double 0.0!
			NdatEvents = data_file->GetEntries()*(plot_pot/data_file->pot )*data_file->scale_data;

			if(var.has_covar){//Determine Estimated Errors
				tsum_name = var.covar_legend_name.c_str();
				legend_style = "fl";
			}else{
				tsum_name = "All MC with Stats-Only Error "+ to_string_prec(total_MC_events,2);
				legend_style = "le";
			}
			if(debug_message) std::cout<<"\nTotal MC:"<<total_MC_events<<", total MCbkg:"<<total_MCbkg_events<<", data:"<<NdatEvents<<std::endl;

			//Calculate  mychi, ndof
			if(!var.has_covar){//this info. goes to the red text box
				for(int p=0; p<d0->GetNbinsX();p++){
					double da = d0->GetBinContent(p+1);
					double bk = tsum->GetBinContent(p+1);
					if ( bk ==0){
						std::cout<<"ERROR mychi, for bin "<<p<<" n_data= "<<da<<" and n_mc= "<<bk<<std::endl;
					} else{
						double da_err = sqrt(tsum->GetBinContent(p+1));
						double bk_err = tsum->GetBinError(p+1);
						double tk = pow(da-bk,2)/(da_err*da_err+bk_err*bk_err);
						std::cout<<da<<" "<<bk<<" "<<da_err<<" "<<bk_err<<" total: "<<sqrt(da_err*da_err+bk_err*bk_err)<<" chi^2 "<<tk<< std::endl;
						if(tk==tk){
							mychi+=tk;
							ndof++;
						}
					}
				}
			}else{//this variable has covariance matrix, so 
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
			std::cout<<"MyChi: "<<var.name<<" "<<mychi<<" "<<std::endl;


			//STEP 2.2.B: TCanvas, TPad
				//Top TPad
			if(!stack_mode) pad0top->SetBottomMargin(0); // Upper and lower plot are joined
			if(var.is_logplot){//adjust the Maximum y of the plot
				pad0top->SetLogy();
				max_modifier=1000.0;
				min_val = 0.1;
			}else{
				switch (stage){//nothing new, just a switch
					case 1:
						max_modifier=1.85;
						break;
					case 2:
						max_modifier=2;
						break;
					case 3:
						max_modifier = 1.85;
						break;
					default:
						max_modifier = 1.85;
						break;
				}
			}
				//Bottom TPad
			pad0bot->SetTopMargin(0);
			pad0bot->SetBottomMargin(0.401);
			pad0bot->SetGridx(); // vertical grid

			//STEP 2.2.C: Histograms
			//STEP 2.2.C.1: Top Plot 
				//stk [skip], tsum[adjust error bar, if systematic error is available], d0 [skip], l0[add contents]
			if(var.has_covar){//For tsum, (systematic+stat) error; covariance matrix calculation.
				TFile *covar_f = new TFile(var.covar_file.c_str(),"read");
				TMatrixD * covar_full = (TMatrixD*)covar_f->Get(var.covar_name.c_str());
				covar_collapsed->Zero();
				std::cout<<"Reading this from a covariance matrix "<<var.covar_file.c_str()<<std::endl;
				this->calcCollapsedCovariance(covar_full, covar_collapsed,var);

				for(int c=0; c< tsum->GetNbinsX();c++){
					double mc_stats_error = tsum->GetBinError(c+1);
					double mc_sys_error = sqrt((*covar_collapsed)(c,c));
					std::cout<<"Yarp: "<<mc_sys_error<<std::endl;
					double tot_error = sqrt(mc_stats_error*mc_stats_error+mc_sys_error*mc_sys_error);
					tsum->SetBinError(c+1, tot_error);
					//And add on the systematic error that is MC stats
					//               (*covar_collapsed)(c,c) += mc_stats_error*mc_stats_error;
				}
				covar_f->Close();
			}
			//modify # of events of different bdt_group
			std::multimap<int, double> grouped_files_index;
			for(size_t jndex = 0; jndex < mc_stack->stack.size(); ++jndex){//For l0, set legend contents according to bdt_file members
				bdt_file* f = mc_stack->stack[jndex];
				double Nevents = f->GetEntries()*(plot_pot/f->pot)*f->scale_data;
				int current_g = f->group;
				std::map<int,double>::iterator skip_this = grouped_files_index.find(current_g);
				if(current_g > -1 && skip_this != grouped_files_index.end()){//found the current_g in grouped_files_index
					skip_this->second+=Nevents;//add up events number, Nevents;
				std::cout<<"CHECK Group "<<current_g<<" has accumulated Events "<<skip_this->second<<std::endl;
				}else{//it is gorup -1 or group number is not previously found;
					grouped_files_index.insert(std::make_pair(current_g,Nevents));
				std::cout<<"CHECK Group "<<current_g<<" Events "<<Nevents<<std::endl;
				}
			}
			//End of skip grouped bdt_files;

			for(size_t jndex = 0; jndex < mc_stack->stack.size(); ++jndex){//For l0, set legend contents according to bdt_file members

				bdt_file* f = mc_stack->stack[jndex];
//				double Nevents = f->GetEntries()*(plot_pot/f->pot)*f->scale_data;

				auto temp_histogram = new TH1F(("tmp"+current_stage+var.safe_name+f->tag).c_str(),"TLegend Example",200,-10,10);//Ghost TH1F

				temp_histogram->SetFillColor(f->col);
				temp_histogram->SetFillStyle(f->fillstyle);
				temp_histogram->SetLineStyle(f->linestyle);
				temp_histogram->SetLineColor(kBlack);

				std::map<int,double>::iterator skip_this = grouped_files_index.find(f->group);
					if(skip_this == grouped_files_index.end()) continue;

				TString string_events = f->plot_name+" "+to_string_prec(skip_this->second,2);//legend name
				if(do_subtraction && subtraction_vec[jndex]) string_events+=" (Subtracted)";
				
				l0->AddEntry(temp_histogram,string_events,"f");
				grouped_files_index.erase(skip_this);

//				if(mc_stack->signal_on_top[n]) which_signal = n;//currently, only 1 signal sample is allowed, delete CHECK, not necessary
			}
			l0->AddEntry(tsum, tsum_name, legend_style);
			if(!stack_mode) l0->AddEntry(d0,(data_file->plot_name+" "+to_string_prec(NdatEvents,2)).c_str(),"lp");

			//STEP 2.2.C.2: Bottom Plot
				//ratio_data [ratio numerator], ratio_bkgMC [ratio denomenator, need to evaluate here], ratunit[shade region, inversely proportional to total MC number], ratio_data_err [error for ratio]
			bdt_stack pure_bkg = *mc_stack;//for ratio_bkgMC
			for(auto &f: mc_stack->stack){
				if(f->is_signal){
					pure_bkg.eraseStack(f);//take away signal sample
				}
			}
			ratio_bkgMC = (TH1*)pure_bkg.getEntrySum(var,stage);//Histogram of the bkg MC, denominator

			ratio_data->Divide(ratio_bkgMC);//Now, ratio is data/bkgMC
			
			ratio_data->SetMarkerSize(3);//ok, remember to do this, CHECK

			std::vector<double> x_pos, y_pos, err_x_left, err_x_right, err_y_high, err_y_low;
			for(int kndex=1; kndex<d0->GetNbinsX()+1;kndex++){//set ratio_data error bar manually,fluctuate # data, keep # bkgMC fix
				double is_zero = ratio_bkgMC->GetBinContent(kndex);
				if(is_zero!=0.0){
					y_pos.push_back(d0->GetBinContent(kndex)/is_zero);
					x_pos.push_back(d0->GetBinCenter(kndex));
					err_x_left.push_back(d0->GetBinWidth(kndex)/2.0);
					err_x_right.push_back(d0->GetBinWidth(kndex)/2.0);
					err_y_high.push_back((d0->GetBinErrorUp(kndex))/is_zero);
					err_y_low.push_back((d0->GetBinErrorLow(kndex))/is_zero);
				}
			}
			ratio_data_err = new TGraphAsymmErrors(x_pos.size(),&x_pos[0],&y_pos[0],&err_x_left[0],&err_x_right[0],&err_y_high[0],&err_y_low[0]);

			ratunit->Divide(ratunit);//this gives the shade, it indicates the static error based on # of MC events

			
			//STEP 2.2.D: Texts
				//Plot title, stage label, spectator [not currently in use?], POT info, Estimators, new label for ratio plot [CHECK];

//        // TText *pre = drawPrelim(0.12,0.92,"MiniBooNE Simulation - In Progress") is a function in bdt_file.h, depreciate it!
			TString title_content = (stack_mode)? "MicroBooNE Simulation" : "MicroBooNE Preliminary";//For title
			if (isSpectator) {//CHECK, to be fixed, isSpectator is not working now;
				title_content += "";//"[Spectator Variable]";
			}else {
				title_content += "";//"[Training Variable]";
			}
			title = new TText(0.6, stack_mode? 0.525: 0.5 , title_content);
			title->SetTextSize( (stack_mode ? 0.04 : 0.06 ));

			stage_label = new TText(0.88, 0.92, current_stage.c_str() );//For stage label

			TString pot_content = data_file->topo_name + " " + to_string_prec(plot_pot/pot_unit,2) + pot_unit_string + " POT";//For POT info

//			std::string pot_draw = data_file->topo_name+" "+to_string_prec(plot_pot/pot_unit,1)+ pot_unit_s+" POT";

//			TLatex *estimators;//Data/MC ratio, KS Test, Chi^2/(nDoF), Chi^2 p-value;
			std::string mean = "(Data/MC: "+to_string_prec(NdatEvents/total_MC_events,2)+")";//For estimators
			std::string mean2 = "";//"(Data/BkgMC: "+to_string_prec(NdatEvents/total_MCbkg_events,2)+")";
			std::string ks = "(KS: "+to_string_prec(tsum->KolmogorovTest(d0),3) + ")     (#chi^{2}/n#it{DOF}: "+to_string_prec(mychi,2) + "/"+to_string_prec(ndof) +")    (#chi^{2} P^{val}: "+to_string_prec(TMath::Prob(mychi,ndof),3)+")";
			std::string combined = mean + "  " + mean2 + "  " + ks;
			estimators = new TLatex(0.11,0.02,combined.c_str());

			if(print_message){
				std::cout<<"Binned KS-test: "<<var.name<<" "<<tsum->KolmogorovTest(d0)<<std::endl;
				std::cout<<"Binned Chi-test standard: "<<var.name<<" "<<tsum->Chi2Test(d0,"CHI2")<<std::endl;
				std::cout<<"Binned Chi-test: "<<var.name<<" "<<tsum->Chi2Test(d0,"UW CHI2")<<std::endl;
				std::cout<<"Binned Chi-test (rev): "<<var.name<<" "<<d0->Chi2Test(tsum,"UW CHI2")<<std::endl;
			}


			//STEP 2.3 Draw
			cobs->cd();//CHECK, go to last section
			pad0top->Draw();// Draw TPads now; draw histogram later after switch to corresponding pad cd()
			if(!stack_mode) pad0bot->Draw();


			pad0top->cd();//CHECK, still working on the top Tpad for the code below;

			stk->Draw("hist");
			tsum->DrawCopy("Same E2");//the statistic error;
			if(!stack_mode) d0->Draw("same E1 E0");
			l0->Draw();
			title->Draw();
			
			//pottex um..needs to be done here;
			pottex.SetTextSize(stack_mode? 0.04:0.06);
			pottex.SetTextAlign(13);
			pottex.SetNDC();
			pottex.DrawLatex(.60, 0.6, pot_content);

//			stage_label->Draw(); Disable Stage, PUBLIC NOTE CHECK

			if(!stack_mode){
				pad0bot->cd();       // pad0bot becomes the current pad
				ratunit->Draw("E2");
				TLine *line = new TLine(ratunit->GetXaxis()->GetXmin(),1.0,ratunit->GetXaxis()->GetXmax(),1.0 );
				line->Draw("same");
				ratio_data->Draw("same P hist");
				ratio_data_err->Draw("E0 same");
				estimators->Draw("same");
			}

			//STEP 2.4: Configure plots
				//Top Histograms: stk, tsum, d0 , l0
			stk->SetTitle("");
			stk->GetXaxis()->SetTitle(var.unit.c_str());
			stk->GetYaxis()->SetTitle("Events");

			if(!stack_mode){
				stk->GetYaxis()->SetTitleSize(0.05);
				stk->GetYaxis()->SetTitleOffset(0.9);
			}

			stk->SetMaximum( std::max(tsum->GetBinContent(tsum->GetMaximumBin()), d0->GetMaximum())*max_modifier);
			stk->SetMinimum(min_val);

			tsum->SetMarkerSize(0);
			tsum->SetLineWidth(3);
			tsum->SetFillStyle(0);//vec_th1s.at(s)->Draw("hist same");

			d0->Rebin(data_rebin);
			d0->SetMarkerSize(3);
			gStyle->SetEndErrorSize(10);
			d0->SetMarkerStyle(20);
			d0->SetLineColor(kBlack);
			d0->SetBinErrorOption(TH1::kPoisson);

			l0->SetFillStyle(0);
			l0->SetLineWidth(0);
			l0->SetNColumns(2);
			l0->SetLineColor(0);
			l0->SetTextSize((stack_mode)? 0.03 : 0.04);
			
			//Bottom Histogram: ratio_data, ratio_bkgMC, ratunit, ratio_data_err			
			ratunit->SetFillColor(kGray+1);
			ratunit->SetMarkerStyle(0);
			ratunit->SetMarkerSize(0);
			ratunit->SetFillStyle(3354);
			//gStyle->SetHatchesLineWidth(1);
			//gStyle->SetHatchesSpacing(1);
			ratunit->SetLineColor(kBlack);
			ratunit->SetTitle("");
			ratunit->SetMinimum(rmin);	
			ratunit->SetMaximum(rmax);//ratunit->GetMaximum()*1.1);
			ratunit->GetYaxis()->SetTitle(  (stack_mode ? "#splitline{Systematic}{Uncertainty}" : "Data/(Prediction)"));
			ratunit->GetYaxis()->SetTitleOffset(title_offset_ratioY*1.25);
			ratunit->GetYaxis()->SetTitleSize(title_size_ratio);
			ratunit->GetYaxis()->SetLabelSize(label_size_ratio);
			ratunit->GetYaxis()->SetNdivisions(505);
			ratunit->GetXaxis()->SetTitle(var.unit.c_str());
			ratunit->GetXaxis()->SetTitleOffset(title_offset_ratioX);
			ratunit->GetXaxis()->SetTitleSize(title_size_ratio);
			ratunit->GetXaxis()->SetLabelSize(label_size_ratio);

			ratio_data_err->SetLineWidth(1);

			ratio_data->SetLineColor(kBlack);
			ratio_data->SetTitle("");
			ratio_data->SetFillColor(kGray+1);
			ratio_data->SetMarkerStyle(20);
			ratio_data->SetMarkerSize(ratio_data->GetMarkerSize()*0.7);
			ratio_data->SetFillStyle(3144);


			//Text: title, stage lable, pottex, estimators
			title->SetTextColor(kBlack);
			title->SetNDC();

			stage_label->SetTextColor(kBlack);
			stage_label->SetNDC();
			stage_label->SetTextAlign(31); // Right-adjusted 

			estimators->SetNDC();
			estimators->SetTextColor(kRed-7);
			estimators->SetTextSize(0.10);


			//STEP 2.5 Additional Info.
			if(print_message){
				int gap=11;
				std::vector<TH1*> somehists((mc_stack->stack).size());
				std::cout<<"\n----- Summary -----"<<std::endl;
				std::cout<<std::left<<std::setw(gap)<<"Bin#";
				for(size_t lndex = 0; lndex < somehists.size(); ++lndex){
					std::cout<<std::left<<std::setw(gap)<<mc_stack->stack[lndex]->tag;
					somehists[lndex] = mc_stack->vec_hists[lndex];
				}
				std::cout<<"data"<<std::endl;
				for(int bin=1; bin< d0->GetNbinsX()+1; bin++){
					std::cout<<std::setw(gap)<<bin;
					for(size_t lndex = 0; lndex<somehists.size(); ++lndex){
						std::cout<<std::left<<std::setw(gap)<<somehists[lndex]->GetBinContent(bin);
					}
					std::cout<<d0->GetBinContent(bin)<<std::endl;
				}
				std::cout<<std::endl;
			}

			cobs->Write();

			if(stack_mode){
				cobs->SaveAs(("stack/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(stage)+".pdf").c_str(),"pdf");
			}else{
				cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(stage)+".pdf").c_str(),"pdf");
			}
			//cobs->SaveAs(("datamc/"+tag+"_"+data_file->tag+"_"+var.safe_unit+"_stage_"+std::to_string(s)+".png").c_str(),"png");

			delete stk;
			delete tsum;
//			delete tsum_after;
			delete d0;
			delete ratunit;
			delete ratio_data;
//			delete rat_denom;
//			delete pad0top;
//			delete tmp_tsum;
//			delete tmp_tsum2;
			delete l0, title;
			//  delete ratunit_after;
//			delete signal_hist;
//			delete rat_signal;
			delete ratio_data_err;
			delete estimators;
//			delete line;
			delete cobs;
//			for(auto &h: fake_legend_hists) delete h;

		}//end of variables loop
	}//end of stage loop

	return 0;
}

int bdt_datamc::calcCollapsedCovariance(TMatrixD * frac_full, TMatrixD *full_coll, bdt_variable & var){

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
                pt=0.0000;
            }

            tmp_full(i,j) = pt*full_vec[i]*full_vec[j];
            //std::cout<<"ARK: "<<i<<" "<<j<<" "<<full_vec[i]<<" "<<pt<<" "<<tmp_full(i,j)<<std::endl;
        }
    }
    std::cout<<"Done"<<std::endl;
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
        tsum_numer->SetMaximum(0);
        tsum_numer->Draw("hist");
        TH1 * tsum_numer2 = (TH1*)tsum_numer->Clone("varsd");

        tsum_numer2->DrawCopy("E2 same");
        data_numer->Draw("E1P same");
//        stacked->Draw("hist same");

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
