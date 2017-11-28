#include "data_mc.h"

template <typename T>
std::string to_string_prec(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out <<std::fixed<< std::setprecision(n) << a_value;
    return out.str();
}

struct bdt_variable{

	public:
		std::string name;
		std::string binning;
		std::string unit;
		bool is_track;

		bdt_variable(std::string inname, std::string inbin, std::string inunit,bool intrack) : 
			name(inname), 
			binning(inbin),
			is_track(intrack),
			unit(inunit)
	{};

};

struct bdt_file{
	public:
		std::string dir;
		std::string name;
		std::string tag;
		std::string plot_ops;
		std::string root_dir;
		std::string friend_tree_file;

		int col;
		bool is_data;
		bool is_mc;
			
		std::string leg;

		std::string friend_tree_name;

		double pot;

		TFile *f;
		TTree *tvertex;
		TTree *tpot;

		bdt_file(std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, std::string infriend, int incol, bool indata) :
			dir(indir),
			name(inname),
			tag(intag),
			root_dir(inrootdir),
			friend_tree_file(infriend),
			plot_ops(inops),
			col(incol),
			is_data(indata),
			is_mc(!indata)
	{

		std::cout<<"Loading : "<<name<<std::endl;
		f = new TFile((dir+"/"+name).c_str(), "read");	


		if(!f->IsOpen() || !f){
			std::cout<<"ERROR: didnt open file right: "<<dir<<"/"<<name<<std::endl;
			exit(EXIT_FAILURE);
		}

		std::string tnam = root_dir+"/"+"vertex_tree";
		std::string tnam_pot = root_dir+"/"+"get_pot";

		std::cout<<"Getting vertex tree"<<std::endl;
		tvertex = (TTree*)f->Get(tnam.c_str());
		std::cout<<"Got vertex tree"<<std::endl;

		double potbranch = 0;

		if(is_mc){
			leg = "l";

			std::cout<<"Getting POT tree: "<<tnam_pot<<std::endl;
			tpot = (TTree*)f->Get(tnam_pot.c_str());
			std::cout<<"Got POT tree"<<std::endl;
			std::cout<<"Setting the POT branch"<<std::endl;
			tpot->SetBranchAddress("pot",&potbranch);
			std::cout<<"Set the POT branch"<<std::endl;

			std::cout<<"--> POT is MC: ";
			tpot->GetEntry(0); 
			pot = potbranch;
			std::cout<<"--> value: "<<pot<<std::endl;
		}else{
			leg = "lp";
			std::cout<<"--> POT is data: ";
			pot = 4.95e19*2000.0/13671;
			std::cout<<"--> value: "<<pot<<std::endl;
		}

		std::cout<<"---> VERTEXCOUNT: "<<tag<<" "<<tvertex->GetEntries()*5e19/pot<<std::endl;

		std::cout<<"Getting friend trees!"<<std::endl;

		std::string method = "BDT";
		if(is_mc){
			friend_tree_name =  std::string(tvertex->AddFriend(("bnb_cosmic_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName());
		}else{
			friend_tree_name =  std::string(tvertex->AddFriend(("data_"+method).c_str(), friend_tree_file.c_str())->GetTree()->GetName());
		}

		std::cout<<"Done!"<<std::endl;

	};

		~bdt_file(){
			f->Close();
		}





};


class compare_instance{
	public:
		bdt_variable var;
		std::vector<bdt_file*> files;

		int Nfiles;

		double title_size_ratio;
		double label_size_ratio;
		double title_offset_ratioX;
		double title_offset_ratioY;

		double title_size_upper;
		double label_size_upper;
		double title_offset_upper;

		compare_instance(std::vector<bdt_file*> vecin, bdt_variable varin) : files(vecin), var(varin) {	
			Nfiles = vecin.size();

			title_size_ratio=0.125;
			label_size_ratio=0.125;
			title_offset_ratioY = 0.3 ;
			title_offset_ratioX = 1.1;

			title_size_upper=0.2;
			label_size_upper=0.07;
			title_offset_upper = 1.45;

		}

		int plot(TFile *fout){
			//These will be better soon
			std::string const pre_cut = "closest_asso_shower_dist_to_flashzcenter <= 40 && totalpe_ibg_sum > 140";
			std::string const base_selection = "passed_swtrigger == 1 && reco_asso_showers == 1";
			std::string const track_only = "reco_asso_tracks >0";
			std::string const no_track = "reco_asso_tracks == 0";
			std::string bdt_cut = "";
			double plot_POT = 5e19;

			double mva1 = 0.5;
			double mva2 = 0.5;

			std::cout<<"Starting to make "<<var.name<<" plot."<<std::endl;
			TCanvas * c = new TCanvas((var.name+"can").c_str(), (var.name+"can").c_str(),1600,1600);
			c->Divide(2,2);
			std::vector<TH1*> var_selec;
			std::vector<TH1*> var_precut;
			std::vector<TH1*> var_bdtcut;

			double ymax_sel=-999;
			double ymax_pre=-999;
			double ymax_bdt=-999;

			std::vector<double> vertex_count_sel;
			std::vector<double> vertex_count_pre;
			std::vector<double> vertex_count_bdt;


			//This is where we fill all our nice histograms
			for(auto &file: files){

				bdt_cut = "((reco_asso_tracks == 0 && " + file->friend_tree_name + ".mva > "+std::to_string(mva1)+") || (reco_asso_tracks > 0 && " + file->friend_tree_name + ".mva > "+std::to_string(mva2)+"))";
				std::cout<<"---> VERTEXCOUNT ---------------->"<<var.name<<"---"<<file->tag<<"<----------------------------------------"<<std::endl;
				std::cout<<"---> VERTEXCOUNT Base: "<<" "<<file->tvertex->GetEntries(base_selection.c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT Precut: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT BDT: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut).c_str())*5e19/file->pot<<std::endl;

				vertex_count_sel.push_back(file->tvertex->GetEntries(base_selection.c_str())*5e19/file->pot);
				vertex_count_pre.push_back(file->tvertex->GetEntries((base_selection+"&&"+pre_cut).c_str())*5e19/file->pot);
				vertex_count_bdt.push_back(file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut).c_str())*5e19/file->pot);

				std::cout<<"---> VERTEXCOUNT 0Tracks Base: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+no_track).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT 0Tracks Precut: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+no_track).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT 0Tracks BDT: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut+"&&"+no_track).c_str())*5e19/file->pot<<std::endl;

				std::cout<<"---> VERTEXCOUNT Tracks Base: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+track_only).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT Tracks Precut: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+track_only).c_str())*5e19/file->pot<<std::endl;
				std::cout<<"---> VERTEXCOUNT Tracks BDT: "<<" "<<file->tvertex->GetEntries((base_selection+"&&"+pre_cut+"&&"+bdt_cut+"&&"+track_only).c_str())*5e19/file->pot<<std::endl;


				std::string tcuts = base_selection;	
				if(var.is_track){
					tcuts = tcuts +"&&" + track_only;
				}


				c->cd(1);	
				std::string tsel = "tmpsel_"+var.name+"_"+file->tag;
				std::cout<<tsel<<std::endl;		


				file->tvertex->Draw( (var.name+">>"+tsel+ var.binning).c_str() ,tcuts.c_str(),"goff");
				std::cout<<"Done with Draw for "<<(var.name+">>"+tsel+ var.binning).c_str()<<std::endl;

				var_selec.push_back( (TH1*)gDirectory->Get(tsel.c_str()) );
				var_selec.back()->Scale(plot_POT/file->pot);
				std::cout<<"Scaled selection by :"<<plot_POT/file->pot<<std::endl;
				ymax_sel = std::max(ymax_sel, var_selec.back()->GetBinContent(var_selec.back()->GetMaximumBin()) );

				c->cd(2);	
				std::string tpre = "tmppre_"+var.name+"_"+file->tag;
				file->tvertex->Draw((var.name+">>"+tpre+var.binning).c_str() ,(tcuts+"&&"+pre_cut).c_str(),"goff");
				std::cout<<"Done with draw from "<<tpre<<std::endl;

				var_precut.push_back(  (TH1*)gDirectory->Get(tpre.c_str()));
				var_precut.back()->Scale(plot_POT/file->pot);;
				std::cout<<"Scaled precut by :"<<plot_POT/file->pot<<std::endl;
				ymax_pre = std::max(ymax_pre, var_precut.back()->GetBinContent(var_precut.back()->GetMaximumBin()) );


				c->cd(3);			

				std::string tbdt = "tmpbdt_"+var.name+"_"+file->tag;
				file->tvertex->Draw((var.name+">>"+tbdt+var.binning).c_str() ,(tcuts+"&&"+pre_cut+"&&"+bdt_cut).c_str(),"goff");
				std::cout<<"Done with draw from "<<tbdt<<std::endl;

				var_bdtcut.push_back(  (TH1*)gDirectory->Get(tbdt.c_str()));
				var_bdtcut.back()->Scale(plot_POT/file->pot);;
				std::cout<<"Scaled bdtcut by :"<<plot_POT/file->pot<<std::endl;
				ymax_bdt = std::max(ymax_bdt, var_bdtcut.back()->GetBinContent(var_bdtcut.back()->GetMaximumBin()) );



			}

			std::cout<<"---> VERTEXCOUNT ---------------->"<<var.name<<"---RATIO----------------------------------------"<<std::endl;
			std::cout<<"---> VERTEXCOUNT Ratio data/mc Base: "<<" "<<vertex_count_sel.at(0)/vertex_count_sel.at(1)<<std::endl;
			std::cout<<"---> VERTEXCOUNT Ratio data/mc Precut: "<<" "<<vertex_count_pre.at(0)/vertex_count_pre.at(1)<<std::endl;
			std::cout<<"---> VERTEXCOUNT Ratio data/mc BDT: "<<" "<<vertex_count_bdt.at(0)/vertex_count_bdt.at(1)<<std::endl;


			ymax_sel=1.25*ymax_sel;
			ymax_pre=1.25*ymax_pre;
			ymax_bdt=1.25*ymax_bdt;


			TLegend *lsel = new TLegend(0.6,0.6,0.89,0.89);lsel->SetLineColor(kBlack);lsel->SetFillStyle(0);
			TLegend *lpre = new TLegend(0.6,0.6,0.89,0.89);lpre->SetLineColor(kBlack);lpre->SetFillStyle(0);
			TLegend *lbdt = new TLegend(0.6,0.6,0.89,0.89);lbdt->SetLineColor(kBlack);lbdt->SetFillStyle(0);


			c->cd(1);

			TPad *pad1top = new TPad("pad1top", "pad1top", 0, 0.3, 1, 1.0);
			pad1top->SetBottomMargin(0); // Upper and lower plot are joined
			pad1top->Draw();             // Draw the upper pad: pad1top
			pad1top->cd();               // pad1top becomes the current pad


			for(int t=0; t<Nfiles; t++){
				var_selec.at(t)->SetTitle(("Selection: "+var.name).c_str());
				var_selec.at(t)->SetLineColor(files.at(t)->col);
				var_selec.at(t)->SetStats(0);
				var_selec.at(t)->SetLineWidth(3);
				var_selec.at(t)->SetMarkerStyle(20);

				var_selec.at(t)->Draw((files.at(t)->plot_ops+ " SAME").c_str());
				lsel->AddEntry(var_selec.at(t), (files.at(t)->tag+" : "+ to_string_prec( var_selec.at(t)->Integral(),1)).c_str()  , files.at(t)->leg.c_str());
				var_selec.at(t)->GetXaxis()->SetTitleSize(title_size_upper);
				var_selec.at(t)->GetXaxis()->SetLabelSize(label_size_upper);
				var_selec.at(t)->GetYaxis()->SetLabelSize(label_size_upper);
				var_selec.at(t)->GetYaxis()->SetTitleOffset(title_offset_upper);
			}


			c->cd(1);          // Go back to the main canvas before defining pad1bot
			TPad *pad1bot = new TPad("pad1bot", "pad1bot", 0, 0.05, 1, 0.3);
			pad1bot->SetTopMargin(0);
			pad1bot->SetBottomMargin(0.301);
			pad1bot->SetGridx(); // vertical grid
			pad1bot->Draw();
			pad1bot->cd();       // pad1bot becomes the current pad
			TH1* ratsel = (TH1*)var_selec.front()->Clone("rat_sel");
			ratsel->Divide(var_selec.back());		
			ratsel->SetLineColor(kBlack);
			ratsel->Draw();	

			ratsel->SetTitle("");
			ratsel->GetYaxis()->SetTitle("Ratio");
			ratsel->GetYaxis()->SetTitleOffset(title_offset_ratioY);
			ratsel->GetXaxis()->SetTitleOffset(title_offset_ratioX);

			ratsel->GetYaxis()->SetTitleSize(title_size_ratio);
			ratsel->GetXaxis()->SetTitleSize(title_size_ratio);
			ratsel->GetYaxis()->SetLabelSize(label_size_ratio);
			ratsel->GetXaxis()->SetTitle(var.unit.c_str());
			ratsel->GetXaxis()->SetLabelSize(label_size_ratio);

			var_selec.front()->GetYaxis()->SetRangeUser(0.1,ymax_sel);
			var_selec.front()->GetYaxis()->SetTitle("Verticies");

			pad1top->cd();
			lsel->Draw();


			//************************************************
			//Now the PRECUT
			//************************************************

			c->cd(2);	
			TPad *pad2top = new TPad("pad2top", "pad2top", 0, 0.3, 1, 1.0);
			pad2top->SetBottomMargin(0); // Upper and lower plot are joined
			pad2top->Draw();             // Draw the upper pad: pad2top
			pad2top->cd();               // pad2top becomes the current pad

			for(int t=0; t<Nfiles; t++){
				var_precut.at(t)->SetTitle(("Precut: "+var.name).c_str());
				var_precut.at(t)->SetLineColor(files.at(t)->col);
				var_precut.at(t)->SetStats(0);
				var_precut.at(t)->SetLineWidth(3);
				var_precut.at(t)->SetMarkerStyle(20);
				var_precut.at(t)->Draw((files.at(t)->plot_ops+" SAME").c_str());
				lpre->AddEntry(var_precut.at(t), (files.at(t)->tag+" : "+ to_string_prec( var_precut.at(t)->Integral() ,1)).c_str()  , files.at(t)->leg.c_str());

				var_precut.at(t)->GetXaxis()->SetTitleSize(title_size_upper);
				var_precut.at(t)->GetXaxis()->SetLabelSize(label_size_upper);
				var_precut.at(t)->GetYaxis()->SetLabelSize(label_size_upper);
				var_precut.at(t)->GetYaxis()->SetTitleOffset(title_offset_upper);

			}
			c->cd(2);          // Go back to the main canvas before defining pad1bot
			TPad *pad2bot = new TPad("pad2bot", "pad2bot", 0, 0.05, 1, 0.3);
			pad2bot->SetTopMargin(0);
			pad2bot->SetBottomMargin(0.301);
			pad2bot->SetGridx(); // vertical grid
			pad2bot->Draw();
			pad2bot->cd();       // pad2bot becomes the current pad
			TH1* ratpre = (TH1*)var_precut.front()->Clone("rat_pre");
			ratpre->Divide(var_precut.back());		
			ratpre->Draw();	
			ratpre->SetLineColor(kBlack);
			ratpre->SetTitle("");
			ratpre->GetYaxis()->SetTitle("Ratio");
			ratpre->GetXaxis()->SetTitleOffset(title_offset_ratioX);
			ratpre->GetYaxis()->SetTitleOffset(title_offset_ratioY);

			ratpre->GetYaxis()->SetTitleSize(title_size_ratio);
			ratpre->GetXaxis()->SetTitleSize(title_size_ratio);
			ratpre->GetYaxis()->SetLabelSize(label_size_ratio);
			ratpre->GetXaxis()->SetLabelSize(label_size_ratio);
			ratpre->GetXaxis()->SetTitle(var.unit.c_str());

			var_precut.front()->GetYaxis()->SetRangeUser(0.1,ymax_pre);
			var_precut.front()->GetYaxis()->SetTitle("Verticies");

			pad2top->cd();
			lpre->Draw();

			//************************************************
			//Now the BDT
			//************************************************


			c->cd(3);	
			TPad *pad3top = new TPad("pad3top", "pad3top", 0, 0.3, 1, 1.0);
			pad3top->SetBottomMargin(0); // Upper and lower plot are joined
			pad3top->Draw();             // Draw the upper pad: pad3top
			pad3top->cd();               // pad3top becomes the current pad

			for(int t=0; t<Nfiles; t++){
				var_bdtcut.at(t)->SetTitle(("BDT: "+var.name).c_str());
				var_bdtcut.at(t)->SetLineColor(files.at(t)->col);
				var_bdtcut.at(t)->SetStats(0);
				var_bdtcut.at(t)->SetLineWidth(3);
				var_bdtcut.at(t)->SetMarkerStyle(20);
				var_bdtcut.at(t)->Draw((files.at(t)->plot_ops+" SAME").c_str());
				lbdt->AddEntry(var_bdtcut.at(t), (files.at(t)->tag+" : "+ to_string_prec( var_bdtcut.at(t)->Integral(),1)).c_str()  , files.at(t)->leg.c_str());

				var_bdtcut.at(t)->GetXaxis()->SetTitleSize(title_size_upper);
				var_bdtcut.at(t)->GetXaxis()->SetLabelSize(label_size_upper);
				var_bdtcut.at(t)->GetYaxis()->SetLabelSize(label_size_upper);
				var_bdtcut.at(t)->GetYaxis()->SetTitleOffset(title_offset_upper);

			}
			c->cd(3);          // Go back to the main canvas before defining pad1bot
			TPad *pad3bot = new TPad("pad3bot", "pad3bot", 0, 0.05, 1, 0.3);
			pad3bot->SetTopMargin(0);
			pad3bot->SetBottomMargin(0.301);
			pad3bot->SetGridx(); // vertical grid
			pad3bot->Draw();
			pad3bot->cd();       // pad3bot becomes the current pad
			TH1* ratbdt = (TH1*)var_bdtcut.front()->Clone("rat_bdt");
			ratbdt->Divide(var_bdtcut.back());		
			ratbdt->Draw();	
			ratbdt->SetLineColor(kBlack);
			ratbdt->SetTitle("");
			ratbdt->GetXaxis()->SetTitleOffset(title_offset_ratioX);
			ratbdt->GetYaxis()->SetTitle("Ratio");
			ratbdt->GetYaxis()->SetTitleOffset(title_offset_ratioY);

			ratbdt->GetYaxis()->SetTitleSize(title_size_ratio);
			ratbdt->GetXaxis()->SetTitleSize(title_size_ratio);
			ratbdt->GetYaxis()->SetLabelSize(label_size_ratio);
			ratbdt->GetXaxis()->SetLabelSize(label_size_ratio);
			ratbdt->GetXaxis()->SetTitle(var.unit.c_str());


			var_bdtcut.front()->GetYaxis()->SetRangeUser(0.1,ymax_bdt);
			var_bdtcut.front()->GetYaxis()->SetTitle("Verticies");
			pad3top->cd();
			lbdt->Draw();





			fout->cd();
			std::cout<<"Writing out: "<<var.name<<std::endl;
			c->Write();



		}



};

int new_data_mc(){
	//std::string indir,std::string inname, std::string intag, std::string inops, std::string inrootdir, int incol, bool indata
	std::string DATFRIEND = "runtmva_merged_data.root";
	std::string MCFRIEND = "runtmva_merged_app.root";

	bdt_file *data = new bdt_file("../../../samples/data/", "data_5e19_2000f_merged.root","Data5e19","e1","LEEPhotonAnalysisData",DATFRIEND,kBlue-4,true );
	bdt_file *mc = new bdt_file("../../../samples/mcc82/", "runmv_bnb_cosmic.root","BNB+cosmic","hist","LEEPhoton", MCFRIEND,kRed-4,false);

	std::vector<bdt_file*> vec_files = {data,mc};

	//(std::string inname, std::string inbin, bool intrack) : 
	std::vector<bdt_variable> vars;
	vars.push_back(bdt_variable("summed_associated_helper_shower_energy","(25,0,0.5)","Energy [GeV]", false));
	vars.push_back(bdt_variable("reco_shower_dedx_plane0","(48,0,15)", "dE/dx [MeV/cm]",false));
	vars.push_back(bdt_variable("longest_asso_track_displacement","(25,0,500)","Length [cm]", true));
	vars.push_back(bdt_variable("shortest_asso_shower_to_vert_dist","(25,0,10)","Length [cm]" ,false));
	vars.push_back(bdt_variable("longest_asso_track_thetayz","(25,-1.7,1.7)","Angle [rad]",true));
	vars.push_back(bdt_variable("longest_asso_track_thetaxz","(25,-1.7,1.7)","Angle [rad]",true));
	vars.push_back(bdt_variable("reco_asso_tracks","(5,0,4)","Num Tracks",false));
	vars.push_back(bdt_variable("most_energetic_shower_bp_dist_to_tpc","(25,0,1000)","Distance [cm]",false));
	vars.push_back(bdt_variable("closest_asso_shower_dist_to_flashzcenter","(25,0,1000)","Distance [cm]",false));
	vars.push_back(bdt_variable("totalpe_ibg_sum","(25,0,2000)","Num PE",false));
	TFile * fout = new TFile("data_mc_comparason.root","recreate");

	for(auto & v: vars){
		compare_instance c(vec_files, v);
		c.plot(fout);
	}

	fout->Close();

	return 0;
}



int data_mc(){
	return 0;//obsolete
}

