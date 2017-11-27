
#include "significance.h"


std::pair<double, double> method_best_significance(std::string const & app_file_path, 
		double const run_pot,
		std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
		std::vector<std::string> const & signal_tree_cuts, 
		std::vector<std::pair<int, double>> const & signal_pots, 
		std::vector<std::pair<TTree *, std::string>> const & background_trees,
		std::vector<std::string> const & background_tree_cuts, 
		std::vector<std::pair<int, double>> const & background_pots,
		std::string const & method) {

	for(auto const & p : signal_trees) {
		p.first->AddFriend((p.second + "_" + method).c_str(), app_file_path.c_str());
	}    
	for(auto const & p : background_trees) {
		p.first->AddFriend((p.second + "_" + method).c_str(), app_file_path.c_str());
	}    

	double best_significance = 0;
	double best_mva_cut = DBL_MAX;

	for(double d = -1.5; d < 1.5; d += 0.1) {

		double signal = 0;

		for(size_t i = 0; i < signal_trees.size(); ++i) {
			auto const & p = signal_trees.at(i);
			signal += p.first->GetEntries((signal_tree_cuts.at(i) + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / signal_pots.at(i).second;
		}

		double background = 0;

		for(size_t i = 0; i < background_trees.size(); ++i) {
			auto const & p = background_trees.at(i);
			background += p.first->GetEntries((background_tree_cuts.at(i) + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / background_pots.at(i).second;
		}


		if(signal/sqrt(signal + background) > best_significance) {
			best_significance = signal/sqrt(signal + background);
			best_mva_cut = d;
		}

	}

	return std::pair<double, double>(best_mva_cut, best_significance);

}


void cout_efficiency(TTree * tree, std::string const & name, std::string const & tree_cut, double const best_cut, 
		std::string const & method, double const scale) {

	int const total = tree->GetEntries();
	int const pre_cut = tree->GetEntries(tree_cut.c_str());
	std::string cut;
	if(tree_cut != "") cut = tree_cut + " && ";
	int const mva_cut = tree->GetEntries((cut + name + "_" + method + ".mva > " + std::to_string(best_cut)).c_str());
	std::cout << name << ":\n"
		<< "Total Vertices: " << total;
	if(scale != 1)	    
		std::cout << " Scaled: " << total * scale;
	std::cout << "\nWith pre-cut: " << pre_cut;
	if(scale != 1)	    
		std::cout << " Scaled: " << pre_cut * scale;
	std::cout << " TE: " << double(pre_cut) / total * 100
		<< "\nWith MVA: " << mva_cut << " ";
	if(scale != 1)	    
		std::cout << " Scaled: " << mva_cut * scale;
	std::cout << " RE: " << double(mva_cut) / pre_cut * 100 << " TE: " << double(mva_cut) / total * 100 << "\n";

}


void cout_best_cut(std::pair<double, double> const & best_cut,
		double const run_pot,
		std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
		std::vector<std::string> const & signal_tree_cuts,
		std::vector<std::pair<int, double>> const & signal_pots, 
		std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		std::vector<std::string> const & background_tree_cuts,
		std::vector<std::pair<int, double>> const & background_pots,
		std::string const & method) {

	std::cout << "Method: " << method << "\n"
		<< "=============================================================\n"  
		<< "Cut: " << best_cut.first << " Significance: " << best_cut.second << " Scaled to: " << run_pot << "\n";
	for(size_t i = 0; i < signal_trees.size(); ++i) {
		cout_efficiency(signal_trees.at(i).first, signal_trees.at(i).second, signal_tree_cuts.at(i), best_cut.first, method, run_pot / signal_pots.at(i).second);
	}

	for(size_t i = 0; i < background_trees.size(); ++i) {
		cout_efficiency(background_trees.at(i).first, background_trees.at(i).second, background_tree_cuts.at(i), best_cut.first, method, run_pot / background_pots.at(i).second);
	}

	std::cout << "\n";

}


void significance(std::string const & app_file_path, 
		double const run_pot,
		std::vector<std::pair<TTree *, std::string>> const & signal_trees,
		std::vector<std::string> const & signal_tree_cuts, 
		std::vector<std::pair<int, double>> const & signal_pots, 
		std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		std::vector<std::string> const & background_tree_cuts,
		std::vector<std::pair<int, double>> const & background_pots,
		std::vector<method_struct> const methods) {

	for(method_struct const & method : methods) {
		cout_best_cut(method_best_significance(app_file_path, run_pot, 
					signal_trees, signal_tree_cuts, signal_pots, 
					background_trees, background_tree_cuts, background_pots, 
					method.str),
				run_pot,
				signal_trees, signal_tree_cuts, signal_pots, 
				background_trees, background_tree_cuts, background_pots,
				method.str);
	}

}


std::vector<double> method_best_significance_seperate(std::string const & app_file_path, 
		double const run_pot,
		std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
		std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts, 
		std::vector<std::pair<int, double>> const & signal_pots, 
		std::vector<std::pair<TTree *, std::string>> const & background_trees,
		std::vector<std::pair<std::string, std::string>> const & background_tree_cuts, 
		std::vector<std::pair<int, double>> const & background_pots,
		std::string const & method) {

	double best_significance = 0;
	double best_mva_cut = DBL_MAX;
	double best_mva_cut2 = DBL_MAX;

	//for nice plots make the 50, 25 is quicker tho
	int nsteps = 25;//50
	double cut_min = 999;
	double cut_max = -999;

	//Find the min and max  CUT values in the ttree, as they differ and it makes plotting hard/inconsistent
	//there is a -999 thing that seems to be broken in GetMinimum;
	for(size_t i = 0; i < signal_trees.size(); ++i) {
		auto const & p = signal_trees.at(i);
		double tmin = p.first->GetMinimum( (p.second + "_" + method + ".mva > -998").c_str());
		double tmax = p.first->GetMaximum( (p.second + "_" + method + ".mva").c_str());
		std::cout<<tmin<<" "<<tmax<<std::endl;
		std::cout<<tmin<<" "<<tmax<<std::endl;
		if( tmin <= cut_min) cut_min=tmin;
		if( tmax >= cut_max) cut_max=tmax;
	}

	for(size_t i = 0; i < background_trees.size(); ++i) {
		auto const & p = background_trees.at(i);
		double tmin = p.first->GetMinimum( (p.second + "_" + method + ".mva > -998").c_str());
		double tmax = p.first->GetMaximum( (p.second + "_" + method + ".mva").c_str());
		std::cout<<tmin<<" "<<tmax<<std::endl;
		if( tmin <= cut_min) cut_min=tmin;
		if( tmax >= cut_max) cut_max=tmax;
	}
	std::cout<<"While on "<<method<<" max and min cut values are :"<<cut_min<<" "<<cut_max<<" respectively"<<std::endl;



	
	double step = (cut_max-cut_min)/((double)nsteps);

	TFile * fout = new TFile("runtmva_significance_contours.root","UPDATE");	
	TH2D * h2_sig_cut = new TH2D( (method+"_significance_2D").c_str(),  (method+"_significance_2D").c_str(),nsteps, cut_min, cut_max, nsteps, cut_min, cut_max);
	std::vector<double> vec_sig;//some vectors to store TGraph info;
	std::vector<double> vec_cut;	


	
	

	for(int di=1; di<=nsteps; di++) {
		double d  = (double)(di-1.0)*step + cut_min; ;	

		double signal = 0;

		for(size_t i = 0; i < signal_trees.size(); ++i) {
			auto const & p = signal_trees.at(i);
			signal += p.first->GetEntries((signal_tree_cuts.at(i).first + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / signal_pots.at(i).second;
		}

		double background = 0;

		for(size_t i = 0; i < background_trees.size(); ++i) {
			auto const & p = background_trees.at(i);
			background += p.first->GetEntries((background_tree_cuts.at(i).first + " && " + p.second + "_" + method + ".mva > " + std::to_string(d)).c_str()) * run_pot / background_pots.at(i).second;
		}
		for(int di2=1; di2<=nsteps; di2++) {
			double d2  = (double)(di2-1.0)*step + cut_min; ;	

			double signal2 = signal;

			for(size_t i = 0; i < signal_trees.size(); ++i) {
				auto const & p = signal_trees.at(i);
				signal2 += p.first->GetEntries((signal_tree_cuts.at(i).second + " && " + p.second + "_" + method + ".mva > " + std::to_string(d2)).c_str()) * run_pot / signal_pots.at(i).second;
			}

			double background2 = background;

			for(size_t i = 0; i < background_trees.size(); ++i) {
				auto const & p = background_trees.at(i);
				background2 += p.first->GetEntries((background_tree_cuts.at(i).second + " && " + p.second + "_" + method + ".mva > " + std::to_string(d2)).c_str()) * run_pot / background_pots.at(i).second;
			}

			if(signal2/sqrt(signal2 + background2) > best_significance) {
				best_significance = signal2/sqrt(signal2 + background2);
				best_mva_cut = d;
				best_mva_cut2 = d2;
			}


			double signif = signal2/sqrt(signal2 + background2);
			if(signal2+background2 ==0){
				std::cout<<"method_best_significane_seperate || signal2+background2 == 0, so significance  = nan @ cut1: "<<d<<", cut2: "<<d2<<std::endl;
				std::cout<<"method_best_significane_seperate || signal2: "<<signal2<<" & background2: "<<background2<<std::endl;
			}
			std::cout<<di<<" "<<di2<<" "<<signif<<std::endl;
			vec_sig.push_back(signif);
			vec_cut.push_back(d2);
			h2_sig_cut->SetBinContent(di,di2, signif);
		}

	}

	h2_sig_cut->SetStats(false);
	TCanvas * c_sig_cuts =  new TCanvas( (method+"_significance_cuts_colz").c_str(), (method+"_significance_cuts_colz").c_str(), 2000,1600 );
	c_sig_cuts->Divide(2,1);
	TPad *p1 = (TPad*)c_sig_cuts->cd(1);
	p1->SetRightMargin(0.13);
	h2_sig_cut->Draw("colz");
	h2_sig_cut->GetXaxis()->SetTitle("Cut 1");
	h2_sig_cut->GetYaxis()->SetTitle("Cut 2");
	
   	std::vector<double> vec_bf_cut1 = {best_mva_cut};
   	std::vector<double> vec_bf_cut2 = {best_mva_cut2};
	TGraph *graph_bf = new TGraph(vec_bf_cut1.size(), &vec_bf_cut1[0], &vec_bf_cut2[0]);
	graph_bf->SetMarkerStyle(29);
	graph_bf->SetMarkerSize(2);
	graph_bf->SetMarkerColor(kBlack);
	graph_bf->Draw("same p");
 

	TGraph * graph_cut = new TGraph(vec_sig.size(), &vec_cut[0], &vec_sig[0]);
	graph_cut->SetTitle("1D slices");
	c_sig_cuts->cd(2);
	graph_cut->Draw("alp");

	h2_sig_cut->Write();
	//graph_cut->Write();



	c_sig_cuts->Write();
	fout->Close();

	return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};

}


void cout_efficiency_seperate(TTree * tree, std::string const & name, TTree * tree_friend,
		std::string const & tree_cut1, std::string const & tree_cut2, double const best_cut1, double const best_cut2, 
		std::string const & method, double const scale ) {

	int const total = tree->GetEntries();
	int const pre_cut = tree->GetEntries(tree_cut1.c_str()) + tree->GetEntries(tree_cut2.c_str());
	std::string cut1;
	if(tree_cut1 != "") cut1 = tree_cut1 + " && ";
	std::string cut2;
	if(tree_cut2 != "") cut2 = tree_cut2 + " && ";
	int const mva_cut = tree->GetEntries(("(" + cut1 + name + "_" + method + ".mva > " + std::to_string(best_cut1) + ") || (" + cut2 + name + "_" + method + ".mva > " + std::to_string(best_cut2) + ")").c_str());
	std::cout << name << ":\n"
		<< "Total Vertices: " << total;
	if(scale != 1)	    
		std::cout << " Scaled: " << total * scale;
	std::cout << " Vtx/Evts: " << get_vertices_per_event(tree) << "\n"
		<< "With pre-cut: " << pre_cut;
	if(scale != 1)	    
		std::cout << " Scaled: " << pre_cut * scale;
	std::cout << " TE: " << double(pre_cut) / total * 100 << " Vtx/Evts: " << get_vertices_per_event(tree, tree_cut1) << "\n"
		<< "With MVA: " << mva_cut << " ";
	if(scale != 1)	    
		std::cout << " Scaled: " << mva_cut * scale;
	std::cout << " RE: " << double(mva_cut) / pre_cut * 100 << " TE: " << double(mva_cut) / total * 100 << " Vtx/Evts: " << get_vertices_per_event(tree, tree_cut1, tree_cut2, tree_friend, best_cut1, best_cut2) << "\n";

}


void cout_best_cut_seperate(std::vector<double> const & best_cut,
		double const run_pot,
		std::vector<std::pair<TTree *, std::string>> const & signal_trees, 
		std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts,
		std::vector<std::pair<int, double>> const & signal_pots,
		std::vector<TTree*> const & signal_tree_friends, 
		std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		std::vector<std::pair<std::string, std::string>> const & background_tree_cuts,
		std::vector<std::pair<int, double>> const & background_pots,
		std::vector<TTree*> const & background_tree_friends,
		std::string const & method) {

	std::cout << "Method: " << method << "\n"
		<< "=============================================================\n"  
		<< "Cut1: " << best_cut.at(0) << " Cut2: " << best_cut.at(1) << " Significance: " << best_cut.at(2) << " Scaled to: " << run_pot << "\n";
	for(size_t i = 0; i < signal_trees.size(); ++i) {
		cout_efficiency_seperate(signal_trees.at(i).first, signal_trees.at(i).second, signal_tree_friends.at(i), signal_tree_cuts.at(i).first, signal_tree_cuts.at(i).second, best_cut.at(0), best_cut.at(1), method, run_pot / signal_pots.at(i).second);
	}

	for(size_t i = 0; i < background_trees.size(); ++i) {
		cout_efficiency_seperate(background_trees.at(i).first, background_trees.at(i).second, background_tree_friends.at(i), background_tree_cuts.at(i).first, background_tree_cuts.at(i).second, best_cut.at(0), best_cut.at(1), method, run_pot / background_pots.at(i).second);
	}

	std::cout << "\n";

}


void significance_seperate(std::string const & app_file_path, 
		double const run_pot,
		std::vector<std::pair<TTree *, std::string>> const & signal_trees,
		std::vector<std::pair<std::string, std::string>> const & signal_tree_cuts, 
		std::vector<std::pair<int, double>> const & signal_pots, 
		std::vector<std::pair<TTree *, std::string>> const & background_trees, 
		std::vector<std::pair<std::string, std::string>> const & background_tree_cuts,
		std::vector<std::pair<int, double>> const & background_pots,
		std::vector<method_struct> const methods) {

	//for each method, loop over and extract significances
	for(method_struct const & method : methods) {

		std::vector<TTree*> signal_tree_friends;
		for(auto const & p : signal_trees) {
			signal_tree_friends.push_back(p.first->AddFriend((p.second + "_" + method.str).c_str(), app_file_path.c_str())->GetTree());
		}    
		std::vector<TTree*> background_tree_friends;
		for(auto const & p : background_trees) {
			background_tree_friends.push_back(p.first->AddFriend((p.second + "_" + method.str).c_str(), app_file_path.c_str())->GetTree());
		}   
		
		
		auto best_sig_sep = method_best_significance_seperate(app_file_path, run_pot, 
					signal_trees, signal_tree_cuts, signal_pots,
					background_trees, background_tree_cuts, background_pots,
					method.str);

		cout_best_cut_seperate(best_sig_sep,
				run_pot,
				signal_trees, signal_tree_cuts, signal_pots, signal_tree_friends,
				background_trees, background_tree_cuts, background_pots, background_tree_friends,
				method.str);
	}

}
