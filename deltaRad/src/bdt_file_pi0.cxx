#include "bdt_file.h"




bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bdt_flow inflow) :
	dir(indir),
	name(inname),
	tag(intag),
	root_dir(inrootdir),
	plot_ops(inops),
	col(incol),
	is_data("false"),
	is_mc("true"),
	flow(inflow)
{


	//This isnt the best idea but sure whynot
    // Because it doesn't work for NC pi0 selection, that's why
    
	//recomc_cols = {kRed-7, kRed+1, kYellow-7, kOrange-3, kBlue+3, kBlue,  kGreen+1,kBlue-7};
	//recomc_cols = {kRed-7, kRed+1, kGreen+1, kBlue+3};
    /* 
	recomc_names = { "NC #Delta Radiative #gamma", "BNB NC #pi^{0} #gamma", "BNB CC #pi^{0} #gamma", "BNB Other #gamma","BNB electron","BNB other","Cosmic"};
	recomc_cuts = 
		"shower_true_pdg == 22 && shower_true_parent_pdg !=111 && is_delta_rad ==1 && shower_true_origin==1",
		"shower_true_pdg == 22 && shower_true_parent_pdg == 111 && shower_true_origin==1 && ccnc ==1",
		"shower_true_pdg == 22 && shower_true_parent_pdg == 111 && shower_true_origin==1 && ccnc == 0",
		"shower_true_pdg == 22 && shower_true_parent_pdg != 111 && is_delta_rad!=1&& shower_true_origin==1",
		"shower_true_origin ==1 && abs(shower_true_pdg) ==11",
		"shower_true_origin ==1 && shower_true_pdg !=22 && abs(shower_true_pdg) !=11",
		"shower_true_origin == 2"
        */
	
    
	
    /*****************************************************
      THIS HAS ALL BEEN MOVED TO deltaRad_pi0.cxx
    *****************************************************/
    // Edited version for NC pi0 selection
    
    recomc_names = {"BNB NC #pi^{0}", "BNB CC #pi^{0} #gamma", "Cosmic", "Other"};
    recomc_names = {"BNB NC #pi^{0}", "BNB CC #pi^{0} #gamma", "BNB NC #Delta Radiative #gamma", "BNB Electron","BNB other","Cosmic"};
    recomc_names = {"BNB NC #pi^{0}", "NC BNB Background", "CC BNB Background", "Cosmic Background"};
	recomc_cuts = {
        // NC pi0 signal: two photon showers whose true parents are pi0's, all resulting from NC BNB interaction
		"true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_pdg[second_most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] == 111 && true_shower_parent_pdg[second_most_energetic_shower_index] == 111 && ccnc == 1 && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1",
		"(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22 || true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111) && ccnc == 1 && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1",
		"(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22 || true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111) && ccnc == 0 && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1",
		"true_shower_origin[most_energetic_shower_index]==2 && true_shower_origin[second_most_energetic_shower_index]==2",
         
        // CC pi0's. This should be a large background. Same as signal, but with ccnc==0 (CC interaction)     
		"true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_pdg[second_most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] == 111 && true_shower_parent_pdg[second_most_energetic_shower_index] == 111 && ccnc == 0",
        
        // Delta radiative is now considered a (very small) background
	    "true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] != 111 && is_delta_rad == 1 && true_shower_origin==1",
        // Check if either shower came from electron
		"true_shower_origin[most_energetic_shower_index] ==1 && true_shower_origin[second_most_energetic_shower_index]==1 && (abs(true_shower_pdg[most_energetic_shower_index]) ==11 || abs(true_shower_pdg[second_most_energetic_shower_index])==11)",
        // Check for cases where either associated photon shower didn't come from pi0
		"(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22 || true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111) && true_shower_origin[most_energetic_shower_index]==1 && true_shower_origin[second_most_energetic_shower_index]==1 && abs(true_shower_pdg[most_energetic_shower_index])!=11 && abs(true_shower_pdg[second_most_energetic_shower_index])!=11",
        
        // Check if either shower is cosmic in origin
		"(true_shower_origin[most_energetic_shower_index] == 2 || true_shower_origin[second_most_energetic_shower_index] == 2) && (true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22) && (true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111)",
        // Other
		"!(true_shower_pdg[most_energetic_shower_index] == 22 && true_shower_pdg[second_most_energetic_shower_index] == 22 && true_shower_parent_pdg[most_energetic_shower_index] == 111 && true_shower_parent_pdg[second_most_energetic_shower_index] == 111) && !(true_shower_origin[most_energetic_shower_index] == 2 || true_shower_origin[second_most_energetic_shower_index] == 2) && !(true_shower_pdg[most_energetic_shower_index] != 22 || true_shower_pdg[second_most_energetic_shower_index] != 22) && !(true_shower_parent_pdg[most_energetic_shower_index] != 111 || true_shower_parent_pdg[second_most_energetic_shower_index] != 111)",
        
	};
    


	/*
	recomc_cols = {kRed-7, kRed+1, kYellow-7, kOrange-3, kBlue+3, kBlue, kBlue-7, kCyan-7, kGreen+3, kGreen+1, kGreen-9, kMagenta-7};
	recomc_names = { "NC #Delta Radiative #gamma", "BNB #pi^{0} #gamma","BNB Other #gamma","BNB electron","BNB muon","BNB proton","BNB pion","BNB other","Cosmic #gamma","Cosmic Electrons", "Cosmic Muons","Cosmic Other"};
	recomc_cuts = {
		"shower_true_pdg == 22 && shower_true_parent_pdg !=111 && is_delta_rad ==1 && shower_true_origin==1",
		"shower_true_pdg == 22 && shower_true_parent_pdg == 111 && shower_true_origin==1",
		"shower_true_pdg == 22 && shower_true_parent_pdg != 111 && is_delta_rad!=1&& shower_true_origin==1",
		"shower_true_origin ==1 && abs(shower_true_pdg) ==11",
		"shower_true_origin ==1 && abs(shower_true_pdg) ==13",
		"shower_true_origin ==1 && abs(shower_true_pdg) ==2212",
		"shower_true_origin ==1 && abs(shower_true_pdg) ==211",
		"shower_true_origin ==1 && shower_true_pdg !=22 && abs(shower_true_pdg) !=11 && abs(shower_true_pdg)!=2212 && abs(shower_true_pdg)!=13 && abs(shower_true_pdg)!=211",
		"shower_true_origin== 2 && abs(shower_true_pdg)==22",
		"shower_true_origin== 2 && abs(shower_true_pdg)==11",
		"shower_true_origin== 2 && abs(shower_true_pdg)==13",
		"shower_true_origin ==2 && shower_true_pdg !=22 && abs(shower_true_pdg) !=11 && abs(shower_true_pdg)!=13"
		//			"shower_true_origin == 2 && abs(shower_true_pdg)==22",
		//			"shower_true_origin == 2 && abs(shower_true_pdg)!=11 && shower_true_pdg!=22"
	};

	*/






	scale_data =1.0;
	std::cout<<"Loading : "<<name<<std::endl;
	f = new TFile((dir+"/"+name).c_str(), "read");	


	if(!f->IsOpen() || !f){
		std::cout<<"ERROR: didnt open file right: "<<dir<<"/"<<name<<std::endl;
		exit(EXIT_FAILURE);
	}

	std::string tnam_event = root_dir+"event_tree";
	std::string tnam = root_dir+"vertex_tree";
	std::string tnam_pot = root_dir+"pot_tree";

	std::cout<<"Getting vertex tree"<<std::endl;
	tvertex = (TTree*)f->Get(tnam.c_str());
	//tevent = (TTree*)f->Get(tnam_event.c_str());
	std::cout<<"Got vertex tree"<<std::endl;

	double potbranch = 0;
    	int  numbranch = 0;

	if(is_mc){
		if(tag == "IntimeCosmics"){
			std::cout<<"Getting POT for CosmicIntime: "<<std::endl;
			double intime_modifier = 10.279;
			double N_gen_bnb = 2412400.0;
			double N_gen_cos = 1030055.0;
			double frac_job_worked = 989875.0/1013198.0;

			double pot_bnb_cosmic = 2.23809e21;
			double pot_plot = 6.6e20;

			pot = pot_plot; 
			this->scale_data = intime_modifier*N_gen_bnb/(N_gen_cos*frac_job_worked)*pot_plot/pot_bnb_cosmic;
			std::cout<<"--> value: "<<pot<<" with scale factor: "<<scale_data<<std::endl;
		}else{
			leg = "l";
			std::cout<<"Getting POT tree: "<<tnam_pot<<std::endl;
			tpot = (TTree*)f->Get(tnam_pot.c_str());
            		tpot->SetBranchAddress("number_of_events", &numbranch);
			tpot->SetBranchAddress("pot",&potbranch);

			std::cout<<"Set the POT branch"<<std::endl;
            int tmpnum = 0;
			double tmppot=0;
			for(int i=0; i<tpot->GetEntries(); i++){
				tpot->GetEntry(i);
                tmpnum += (double)numbranch;
				tmppot += potbranch;
			}
            numberofevents = tmpnum;
			pot=tmppot;
			std::cout<<"--> POT is MC: ";
			std::cout<<"--> value: "<<pot<<std::endl;
		}
	}
	if(tag == "Data5e19"){
		leg = "lp";
		std::cout<<"--> POT is data: ";
		pot = 4.36e19*189251.0/192043.0; //7131/13671;// 376954.0/382718.0;//7131/13671;
		std::cout<<"--> value: "<<pot<<std::endl;
	}
	if(tag == "BNBext"){
		leg = "lp";
		std::cout<<"--> POT is data: ";
		pot = 4.36e19*189251.0/192043.0; //7131/13671;// 376954.0/382718.0;//7131/13671;
		std::cout<<"--> value: "<<pot<<std::endl;
	}

	std::cout<<"---> VERTEXCOUNT: "<<tag<<" "<<tvertex->GetEntries()*5e19/pot<<std::endl;

	std::cout<<"Done!"<<std::endl;




};




bdt_file::bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, std::string infriend, std::string infriendtree, int incol, bool indata) :
	dir(indir),
	name(inname),
	tag(intag),
	root_dir(inrootdir),
	friend_tree_file(infriend),
	friend_tree_name(infriendtree),
	plot_ops(inops),
	col(incol),
	is_data(indata),
	is_mc(!indata)
{
	scale_data =1.0;
	std::cout<<"Loading : "<<name<<std::endl;
	f = new TFile((dir+"/"+name).c_str(), "read");	


	if(!f->IsOpen() || !f){
		std::cout<<"ERROR: didnt open file right: "<<dir<<"/"<<name<<std::endl;
		exit(EXIT_FAILURE);
	}

	std::string tnam_event = root_dir+"event_tree";
	std::string tnam = root_dir+"vertex_tree";
	std::string tnam_pot = root_dir+"pot_tree";

	std::cout<<"Getting vertex tree"<<std::endl;
	tvertex = (TTree*)f->Get(tnam.c_str());
	//tevent = (TTree*)f->Get(tnam_event.c_str());
	std::cout<<"Got vertex tree"<<std::endl;

	double potbranch = 0;

	if(is_mc){
		leg = "l";
		std::cout<<"Getting POT tree: "<<tnam_pot<<std::endl;
		tpot = (TTree*)f->Get(tnam_pot.c_str());
		tpot->SetBranchAddress("pot",&potbranch);

		std::cout<<"Set the POT branch"<<std::endl;
		double tmppot=0;
		for(int i=0; i<tpot->GetEntries(); i++){
			tpot->GetEntry(i);
			tmppot += potbranch;
		}
		pot=tmppot;
		std::cout<<"--> POT is MC: ";
		std::cout<<"--> value: "<<pot<<std::endl;
	}else{
		leg = "lp";
		std::cout<<"--> POT is data: ";
		pot = 4.95e19*7131/13671;// 376954.0/382718.0;//7131/13671;
		std::cout<<"--> value: "<<pot<<std::endl;
	}

	std::cout<<"---> VERTEXCOUNT: "<<tag<<" "<<tvertex->GetEntries()*5e19/pot<<std::endl;

	std::cout<<"Getting friend trees!"<<std::endl;

	std::string method = "BDT";
	if(is_mc){
		friend_tree_name =  std::string(tvertex->AddFriend((friend_tree_name+"_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName());
	}else{
		friend_tree_name =  std::string(tvertex->AddFriend((friend_tree_name+"_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName());
	}

	std::cout<<"Done!"<<std::endl;


};



int bdt_file::scale(double scalein){
	scale_data = scalein;
	return 0;
}
int bdt_file::setPOT(double inpot){
	pot = inpot;
	return 0;
}
TH1* bdt_file::getEventTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){

	TCanvas *ctmp = new TCanvas();
	this->tevent->Draw((var.name+">>"+nam+ var.binning).c_str() ,cuts.c_str(),"goff");
	std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;

	TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
	th1->Scale(this->scale_data*plot_POT/this->pot);
	th1->SetLineColor(col);
	th1->SetLineWidth(1);
	th1->SetStats(0);
	th1->GetXaxis()->SetTitle(var.unit.c_str());
	th1->GetYaxis()->SetTitle("Verticies");


	return th1;
}


TH1* bdt_file::getTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){

	std::cout<<"Starting to get for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;
	TCanvas *ctmp = new TCanvas();
	this->tvertex->Draw((var.name+">>"+nam+ var.binning).c_str() ,cuts.c_str(),"goff");
	std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;


	TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
	th1->Scale(this->scale_data*plot_POT/this->pot);
	std::cout<<"IS THIS: "<<this->scale_data*plot_POT/this->pot<<" "<<th1->GetSumOfWeights()<<std::endl;
	th1->SetLineColor(col);
	th1->SetLineWidth(1);
	th1->SetStats(0);
	th1->GetXaxis()->SetTitle(var.unit.c_str());
	th1->GetYaxis()->SetTitle("Vertices");
	th1->SetDirectory(0);	

	return th1;
}


std::vector<TH1*> bdt_file::getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){
	std::vector<TH1*> ans_th1s;

	std::string other = "other";
	std::string other_cuts = cuts;

	std::cout<<"getRecoMCTH1 || size of names: "<<recomc_names.size()<<" "<<recomc_cuts.size()<<" "<<recomc_cols.size()<<std::endl;

	for(int i=0; i< recomc_cuts.size(); i++){
		std::cout<<"On "<<i<<" of "<<recomc_names.at(i)<<std::endl;
		TCanvas *ctmp = new TCanvas();
		this->tvertex->Draw((var.name+">>"+nam+"_"+recomc_names.at(i)+ var.binning).c_str() , (cuts+"&&"+recomc_cuts.at(i)).c_str(),"goff");

		TH1* th1 = (TH1*)gDirectory->Get((nam+"_"+recomc_names.at(i)).c_str()) ;
		th1->Scale(this->scale_data*plot_POT/this->pot);
		th1->SetFillColor(recomc_cols.at(i));
		th1->SetLineColor(kBlack);
		th1->SetLineWidth(1);
		th1->SetStats(0);
		th1->GetXaxis()->SetTitle(var.unit.c_str());
		th1->GetYaxis()->SetTitle("Verticies");
		ans_th1s.push_back(th1);

		other_cuts = other_cuts+ " && " +"!("+recomc_cuts.at(i)+")";	

	}

	//Should fix this soon
	//recomc_cuts.push_back(other_cuts +"&& shower_true_origin != -1");
	//recomc_names.push_back(other);

	TCanvas *ctmp = new TCanvas();
	this->tvertex->Draw((var.name+">>"+nam+"_"+other+ var.binning).c_str() , other_cuts.c_str(),"goff");


	TH1* th1 = (TH1*)gDirectory->Get((nam+"_"+other).c_str()) ;
	th1->Scale(0*this->scale_data*plot_POT/this->pot);
	th1->SetLineColor(kBlack);
	th1->SetFillColor(kBlack);
	th1->SetLineWidth(1);
	th1->SetStats(0);
	th1->GetXaxis()->SetTitle(var.unit.c_str());
	th1->GetYaxis()->SetTitle("Verticies");
	//ans_th1s.push_back(th1);

	return ans_th1s;	
}

bdt_variable bdt_file::getBDTVariable(bdt_info info){

    std::cout << "In getBDTVariable with bdt_info info, bdt_info = " << info.identifier << std::endl;
	if(info.identifier =="bnb_track" || info.identifier == "bnb_notrack"){
        std::cout << "Identifier bnb_track or bnb_notrack" << std::endl;
		return bdt_variable(this->tag +"_"+info.identifier+ ".mva","(65,0.35,0.55)","BNB BDT Response",false);
	}else if(info.identifier == "cosmic_track" || info.identifier == "cosmic_notrack"){
        std::cout << "Identifier cosmic_track or cosmic_notrack" << std::endl;
		return bdt_variable(this->tag +"_"+info.identifier+ ".mva","(65,0.2,0.62)","Cosmic BDT Response",false);
	}
    // Add cases for pi0 selection
    else if(info.identifier == "pi0cosmic_track" || info.identifier == "pi0cosmic_notrack") {
		return bdt_variable(this->tag +"_"+info.identifier+ ".mva","(65, 0.2, 0.7)","Cosmic BDT Response",false);
    }       
    else if(info.identifier == "pi0bnb_track" || info.identifier == "pi0bnb_notrack") {
		return bdt_variable(this->tag +"_"+info.identifier+ ".mva","(65, 0.2, 0.7)","BNB BDT Response",false);
    }
    else {
        std::cout << "Bad identifier in bdt_variable" << std::endl;
    }       
}

bdt_variable bdt_file::getBDTVariable(std::string info){
	//std::cout<<"getBDTVariable: "<<info<<std::endl;
    std::cout << "In getBDTVariable with std::string info, bdt_info = " << info << std::endl;
	if(info =="bnb_track" || info == "bnb_notrack"){
        std::cout << "Identifier bnb_track or bnb_notrack" << std::endl;
		return bdt_variable(this->tag +"_"+info+ ".mva","(65,0.35,0.55)","BNB BDT Response",false);
	}else if(info == "cosmic_track" || info == "cosmic_notrack"){
        std::cout << "Identifier cosmic_track or cosmic_notrack" << std::endl;
		return bdt_variable(this->tag +"_"+info+ ".mva","(65,0.2,0.62)","Cosmic BDT Response",false);
	}
    // Add cases for pi0 selection
    else if(info == "pi0bnb_track" || info == "pi0bnb_notrack") {
        std::cout << "Identified pi0cosmic_track" << std::endl;
		return bdt_variable(this->tag +"_"+info+ ".mva","(65,0.2,0.7)","Cosmic BDT Response",false);
    }       
    else if(info == "pi0cosmic_track" || info == "pi0cosmic_notrack") {
        std::cout << "Identified pi0cosmic_track" << std::endl;
		return bdt_variable(this->tag +"_"+info+ ".mva","(65,0.2,0.7)","Cosmic BDT Response",false);
    }       
    else {
        std::cout << "Bad identifier in bdt_variable" << std::endl;
    }       

    /*
	if(info =="bnb_track"|| info=="bnb_notrack"){
		return bdt_variable(this->tag +"_"+info+ ".mva","(65,0.35,0.55)","BNB BDT Response",false);
	}else if(info == "cosmic_track"|| info=="cosmic_notrack"){
		return bdt_variable(this->tag +"_"+info+ ".mva","(65,0.2,0.62)","Cosmic BDT Response",false);
	}
    */

}


bdt_file::~bdt_file(){
	f->Close();
}



int bdt_file::addFriend(std::string in_friend_tree_nam, std::string in_friend_file){
	friend_files.push_back(in_friend_file);
	friend_names.push_back(in_friend_tree_nam);

	tvertex->AddFriend(friend_names.back().c_str(), friend_files.back().c_str());


	return 0;
}



std::string bdt_file::getStageCuts(int stage, double bdtvar1, double bdtvar2){

	std::string ans;
	switch(stage) {
		case 0:
			ans = flow.base_cuts;
			break;
		case 1:
			ans = flow.base_cuts + "&&"+ flow.pre_cuts;
            //std::cout << "Stage 1 cuts: " << ans << std::endl;
			break;
		case 2: {
			bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
			ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1);
            //std::cout << "Stage 2 cuts: " << ans << std::endl;
			break;
	    }

		case 3: {
			bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
			bdt_variable stage3var = this->getBDTVariable(flow.bdt_bnb_cuts);		
			ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1)+"&&"+stage3var.name +">" +std::to_string(bdtvar2);
            //std::cout << "Stage 2 var name: " << stage2var.name << std::endl;
            //std::cout << "Stage 3 var name: " << stage3var.name << std::endl;
            //std::cout << "Stage 3 cuts: " << ans << std::endl;
			break;
		}
		case 4: {
			bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
			bdt_variable stage3var = this->getBDTVariable(flow.bdt_bnb_cuts);		
            //std::cout << "Stage 2 var name: " << stage2var.name << std::endl;
            //std::cout << "Stage 3 var name: " << stage3var.name << std::endl;
			ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1)+"&&"+stage3var.name +">" +std::to_string(bdtvar2) +"&&" +flow.post_cuts;
            //std::cout << "Stage 4 cuts: " << ans << std::endl;
			break;
        }

		default:
			ans = "1";
			break;

	}	
	return ans;
}



