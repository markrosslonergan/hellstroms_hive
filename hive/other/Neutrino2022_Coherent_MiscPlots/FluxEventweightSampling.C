/* The goal of this macro is: 
 * given events that have eventweights, sample from them to assign eventweight to other events
 * (Always choose the event with closet true neutrino energy to grab the multiuniverse eventweight)
 *
 * Guanqun Ge, May 26 2022
 */
void FluxEventweightSampling(){

    //---- INPUT ----------
    //-----------------------
    // file with eventweights included
    std::string in_fname = "/pnfs/uboone/persistent/users/markross/Jan2022_gLEE_files/UniqDir/vertex_BNBOther_Run1_v50.0.uniq.root";
    std::string in_tdir_name = "singlephotonana", in_tree_name="true_eventweight_tree";
    std::string in_varname = "mctruth_nu_E";
    std::string in_weight_branch = "mcweight";

    // output file to attach eventweight to
    std::string out_fname = "vertex_DarkNu_Run1_BenchD_noHF_CV_v50.5.root";
    std::string out_tdir_name = "singlephotonana", out_tree_name = "true_eventweight_tree";
    //std::string out_varname = "mctruth_daughters_E[1]";
    std::string out_varname = "Sum$((mctruth_daughters_pdg == 14)*mctruth_daughters_E)";
    std::string out_weight_branch = "new_mcweight";
    std::string out_Ediff_plot = "FluxEventweightSampling_EnergyDiff_DarkNus_BenchD.pdf";

    // list of systematics to grab multi-verse weights 
    std::set<std::string> syst_names = {"expskin_FluxUnisim", "horncurrent_FluxUnisim", "kminus_PrimaryHadronNormalization", "kplus_PrimaryHadronFeynmanScaling", "kzero_PrimaryHadronSanfordWang", "nucleoninexsec_FluxUnisim", "nucleonqexsec_FluxUnisim", "nucleontotxsec_FluxUnisim", "piminus_PrimaryHadronSWCentralSplineVariation", "pioninexsec_FluxUnisim", "pionqexsec_FluxUnisim", "piontotxsec_FluxUnisim", "piplus_PrimaryHadronSWCentralSplineVariation"};

   // ------- END of INPUT ----------
   //-----------------------
  
   //Grab the trees and branches 
   gInterpreter->GenerateDictionary("map<string,vector<double> >", "map;string;vector"); 

   TFile* fin = new TFile(in_fname.c_str(), "read");
   TTree* tin = (TTree*)fin->Get((in_tdir_name+"/"+in_tree_name).c_str());
   tin->AddFriend("singlephotonana/vertex_tree");

   //turn off branch process to speed up
   tin->SetBranchStatus("*", 0);
   tin->SetBranchStatus(in_varname.c_str(), 1);
   tin->SetBranchStatus(in_weight_branch.c_str(), 1);
   TTreeFormula* tfin = new TTreeFormula("tformula_in", in_varname.c_str(), tin);
   std::map<std::string, std::vector<double>>* in_mcweights = nullptr;
   tin->SetBranchAddress(in_weight_branch.c_str(), &in_mcweights);

   TFile* fout = new TFile(out_fname.c_str(), "update");
   TDirectory* tdir_out = (TDirectory*)fout->Get(out_tdir_name.c_str());
   TTree* tvertex = (TTree*)fout->Get("singlephotonana/vertex_tree");
   TTreeFormula* tfout = new TTreeFormula("tformula_out", out_varname.c_str(), tvertex);
   TTree* tout = (TTree*)fout->Get((out_tdir_name+"/"+out_tree_name).c_str());
   if(tout == nullptr){
	std::cout << "TTree " << out_tree_name << " does not exist, create one ..." << std::endl;
   	tout = new TTree(out_tree_name.c_str(), out_tree_name.c_str());
   }

   //grab multiweight branch, if not exist create a new one
   TBranch* newBranch = nullptr;
   std::map<std::string, std::vector<double>> out_mcweights;
   std::map<std::string, std::vector<double>>* out_mcweights_ptr = nullptr;
   auto branchstatus = tout->SetBranchAddress(out_weight_branch.c_str(), &out_mcweights_ptr);
   if(branchstatus == TTree::kMissingBranch){
	std::cout << "Missing branch .. Create New one.." << out_weight_branch  << std::endl;
	newBranch = tout->Branch(out_weight_branch.c_str(), &out_mcweights);
	out_mcweights_ptr = &out_mcweights;
   }
  std::cout << "Check: " << branchstatus << " " << out_mcweights_ptr << " " << out_mcweights_ptr->size()<< std::endl;

   //--- Step 1: grab one to one map 
   int num_event = tvertex->GetEntries();
   std::vector<int> one_to_one_map(num_event, -1);
   std::vector<int>* one_to_one_map_ptr = nullptr;
   fout->GetObject("one_to_one_E_mapping", one_to_one_map_ptr);
   if(one_to_one_map_ptr == nullptr){ 

   	   //generate one to one map, always choose the one with closet energy and check the max true energy difference 
   	   // This is very slowwww!!!
	   std::cout << "Read input file for pool of events... Total of " << tin->GetEntries() << " events in the pool " << std::endl;

	   std::vector<std::pair<double,int>> E_index_vec;
	   for(int i = 0; i != tin->GetEntries(); ++i){
		if(i%10000 == 0) std::cout << "ON Entry " << i << std::endl;
		
		tin->GetEntry(i);
		tfin->GetNdata();
		double in_var_val = tfin->EvalInstance();
		E_index_vec.push_back({in_var_val,i});
	   }
	   std::sort(E_index_vec.begin(), E_index_vec.end());
	   std::cout << "Finish ordering all events according to their energy.. " << std::endl;

	   std::vector<int> order(num_event, 0);
	   std::iota(order.begin(), order.end(), 0);
	   std::cout << "There are " << num_event << " events awaiting evenntweight assignment.. " << std::endl;

	   TCanvas* c = new TCanvas();
	   c->Print((out_Ediff_plot+"[").c_str());
	   for(int dummy = 0 ; dummy < 1; ++dummy){
	       TH1D* f_absEdiff = new TH1D("f", "f", 100, 1e-7, 0.3); 
	       TH2D* h2D_Ediff = new TH2D("h2D", "h2D", 800, 0, 8, 1000, 1e-7, 1);

	       auto curr_order = order;
	       auto curr_vec = E_index_vec;

	       //random shuffle the order of events we traverse
	       //std::random_shuffle(curr_order.begin(), curr_order.end());
	       
	       //traverse through all events, and find corresponding events with closest energy
	       double max_diff = 0;
	       for(int i= 0; i != num_event; ++i){
	       //for(int i= 0; i != 100; ++i){
		   if(i %1000 == 0) std::cout << "On " << i<<"th Entry ";

		   int ie = curr_order[i];
		   tvertex->GetEntry(ie);
		   tfout->GetNdata();
		   double out_var_val = tfout->EvalInstance();

		   // grab cloest event
		   auto vec_iter = std::lower_bound(curr_vec.begin()+i, curr_vec.end(), std::pair<double, int>({out_var_val, -1}));
		   auto vec_pre_iter = vec_iter - 1;
		   if(vec_iter == curr_vec.end() || (vec_pre_iter->first != -1 && abs(vec_iter->first - out_var_val) > abs(vec_pre_iter->first - out_var_val)) )
		       vec_iter = vec_pre_iter;
		     
		   double sampled_val = vec_iter->first;
		   int sampled_index = vec_iter->second;
		   int sampled_pos =  vec_iter - curr_vec.begin(); 
		   double absolute_diff = abs(sampled_val - out_var_val); 
		   f_absEdiff->Fill(absolute_diff);
		   h2D_Ediff->Fill(out_var_val, absolute_diff);
//	std::cout << "True E " << out_var_val << " diff: " << absolute_diff << std::endl;
		   max_diff = std::max(max_diff, absolute_diff);
		   one_to_one_map[ie] = sampled_index;
		   if(i %1000 == 0) std::cout << " Input entry: " << ie << "  Max diff up to now " << max_diff << std::endl;
		
		   //remove sample events from the vector
		   *vec_iter = {-1, -1};
		   for(int j = sampled_pos - 1; j >= 0 && curr_vec[j].first != -1; --j)
		       std::swap(curr_vec[j], curr_vec[j+1]); 

	       }

	       f_absEdiff->SetTitle("Absolute Energy Difference between given event and sampled event;Absolute Energy Diff [GeV]; Entries");
	       h2D_Ediff->SetTitle("True #nu Energy vs Absolute Energy Difference between given event and sampled event;True #nu Energy [GeV]; Absolute Energy Diff [GeV]; Entries");
	       c->cd();
	       c->Clear();
	       c->SetLogy(1);
	       f_absEdiff->Draw("hist");
	       c->Print(out_Ediff_plot.c_str()); 
	       c->Clear();
	       gStyle->SetOptStat(0);
	       c->SetLogx(0);
	       c->SetLogz(1);
	       h2D_Ediff->Draw("colz");
	       c->Print(out_Ediff_plot.c_str()); 
	   }
	   c->Print((out_Ediff_plot+"]").c_str());

         // std::iota(one_to_one_map.begin(), one_to_one_map.end(), 0);

	   //write out the enery map
	   fout->cd();
	   fout->WriteObject(&one_to_one_map, "one_to_one_E_mapping");

	   delete c;
   }
   else{
	one_to_one_map = *one_to_one_map_ptr;
   }
   // ------ End of Step 1 ------------

   // ------- Step 2 --------
   // Assign the eventweights
if(true){
   for(int i = 0 ; i!= num_event; ++i){
   //for(int i = 0 ; i!= 100; ++i){
 	if(i %1000 == 0) std::cout << "On " << i<<"th Entry " << std::endl;
	tout->GetEntry(i);

	int sample_index = one_to_one_map[i];
   	tin->GetEntry(sample_index);
	std::map<std::string, std::vector<double>> local_mcweights;

 	for(auto pair : *in_mcweights){
            if(syst_names.count(pair.first) != 0){
                local_mcweights[pair.first] = pair.second;
		
		if(false && i==0){	
		    std::cout << pair.first << ": ";
		    for(auto& d:pair.second)
		   	std::cout << d << " ";
		    std::cout << std::endl; 
		}
            } 
        }

 	//std::cout << "map size: " << local_mcweights.size() << std::endl;
 	*out_mcweights_ptr = local_mcweights;
	if(newBranch)
	    newBranch->Fill();
	else
	    tout->Fill();		
   }
   fout->cd();
   tdir_out->cd(); 
   tout->Write("", TObject::kWriteDelete);
   fout->Close();
}
  return;
}
