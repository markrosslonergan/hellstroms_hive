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

	plot_name = tag;


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
			//Found in 
			double intime_modifier = 10.279;

			double N_gen_bnb = 2153450.0;
			double N_gen_cos = 991885.0;
			double frac_job_worked = 1.0;//5.0/9.0;

			double pot_bnb_cosmic = 2.172e+21;
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
			std::cout<<"--> value: "<<pot<<" NumEvents: "<<numberofevents<<std::endl;
		}
	}
	if(tag == "Data5e19"){
		leg = "lp";
		double Nworked = 191046;// pot_tree->Scan() 
		double Nsamweb = 191131;// samweb list-definition-files XXX --summary --fileinfo

		std::cout<<"--> POT is data: From Zarkos tool..";
		//4.885e+19
		//4.879e+19
		//4.932e+19
		//4.938e+19
		pot = 4.879e+19*Nworked/Nsamweb; 
		std::cout<<"--> value: "<<pot<<std::endl;
	}
	if(tag == "BNBext"){
		leg = "lp";
		double sca = 1.23;//from 1.23
		//https://microboone-docdb.fnal.gov/cgi-bin/private/ShowDocument?docid=5640

		double exta=38713062.0;
		double extb=581923.0;

		double spill=10893847.0; 

		double Nworked = 852486.0;
		double Nsamweb = 463273 + 521026;


		//data
		double NworkedD = 190991.0;
		double NsamwebD = 191131.0;
		double datanorm = 4.879e+19*NworkedD/NsamwebD; //7131/13671;// 376954.0/382718.0;//7131/13671;

		double mod = 5.0/9.0*spill/(exta);//spill/(exta+extb);


		std::cout<<"--> POT is data: From Zarkos tool..";
		pot =datanorm/mod*Nworked/Nsamweb; //7131/13671;// 376954.0/382718.0;//7131/13671;
		std::cout<<"--> value: "<<pot<<std::endl;

	}

	std::cout<<"---> VERTEXCOUNT: "<<tag<<" "<<tvertex->GetEntries()*5e19/pot<<std::endl;

	std::cout<<"Done!"<<std::endl;




};

int bdt_file::addPlotName(std::string plotin){
	plot_name = plotin;
	return 0;
}




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
	return getTH1(var, cuts,nam,plot_POT,1);

}
TH1* bdt_file::getTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT, int rebin){

	std::cout<<"Starting to get for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;
	TCanvas *ctmp = new TCanvas();
	this->tvertex->Draw((var.name+">>"+nam+ var.binning).c_str() ,cuts.c_str(),"goff");
	std::cout<<"Done with Draw for "<<(var.name+">>"+nam+ var.binning).c_str()<<std::endl;


	TH1* th1 = (TH1*)gDirectory->Get(nam.c_str()) ;
	th1->Scale(this->scale_data*plot_POT/this->pot);
	//	std::cout<<"IS THIS: "<<this->scale_data*plot_POT/this->pot<<" "<<th1->GetSumOfWeights()<<std::endl;
	if(rebin>1) th1->Rebin(rebin);
	th1->SetLineColor(col);
	th1->SetLineWidth(1);
	th1->SetStats(0);
	th1->GetXaxis()->SetTitle(var.unit.c_str());
	th1->GetYaxis()->SetTitle("Verticies");
	th1->SetDirectory(0);	

	return th1;
}


std::vector<TH1*> bdt_file::getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT){
	return getRecoMCTH1(var, cuts, nam, plot_POT,1);
}


std::vector<TH1*> bdt_file::getRecoMCTH1(bdt_variable var, std::string cuts, std::string nam, double plot_POT, int rebin){
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
		if(rebin >1 )th1->Rebin(rebin);
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
	return bdt_variable(this->tag +"_"+info.identifier+ ".mva", info.binning, info.name+" Response" ,false);
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

int bdt_file::addBDTResponses(bdt_info cosmic_bdt_info, bdt_info bnb_bdt_info,   std::vector<method_struct> TMVAmethods){

		for(auto &method: TMVAmethods){
			
				std::cout<<"Now adding TreeFriend: "<<cosmic_bdt_info.identifier<<"_app.root"<<" "<<this->tag<<std::endl;
				this->addFriend(this->tag +"_"+cosmic_bdt_info.identifier,  cosmic_bdt_info.identifier+"_app"+".root");

				std::cout<<"Now adding TreeFriend: "<<bnb_bdt_info.identifier<<"_app.root"<<" "<<this->tag<<std::endl;
				this->addFriend(this->tag +"_"+bnb_bdt_info.identifier,  bnb_bdt_info.identifier+"_app"+".root");
		}
	
	return 0;
}


std::string bdt_file::getStageCuts(int stage, double bdtvar1, double bdtvar2){

	bool verbose = false;

	std::string ans;
	switch(stage) {
		case 0:
			ans = flow.base_cuts;
			break;
		case 1:
			ans = flow.base_cuts + "&&"+ flow.pre_cuts;
			if(verbose)std::cout << "Stage 1 cuts: " << ans << std::endl;
			break;
		case 2: {
				bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
				ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1);
				if(verbose)std::cout << "Stage 2 cuts: " << ans << std::endl;
				break;
			}

		case 3: {
				bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
				bdt_variable stage3var = this->getBDTVariable(flow.bdt_bnb_cuts);		
				ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1)+"&&"+stage3var.name +">" +std::to_string(bdtvar2);
				if(verbose)std::cout << "Stage 2 var name: " << stage2var.name << std::endl;
				if(verbose)std::cout << "Stage 3 var name: " << stage3var.name << std::endl;
				if(verbose)std::cout << "Stage 3 cuts: " << ans << std::endl;
				break;
			}
		case 4: {
				bdt_variable stage2var = this->getBDTVariable(flow.bdt_cosmic_cuts);		
				bdt_variable stage3var = this->getBDTVariable(flow.bdt_bnb_cuts);		
				if(verbose)std::cout << "Stage 2 var name: " << stage2var.name << std::endl;
				if(verbose)std::cout << "Stage 3 var name: " << stage3var.name << std::endl;
				ans = flow.base_cuts + "&&" + flow.pre_cuts + "&&"+  stage2var.name + ">" +std::to_string(bdtvar1)+"&&"+stage3var.name +">" +std::to_string(bdtvar2) +"&&" +flow.post_cuts;
				if(verbose)std::cout << "Stage 4 cuts: " << ans << std::endl;
				break;
			}
		default: 
			ans = "1";
			break;

	}	
	return ans;
}



