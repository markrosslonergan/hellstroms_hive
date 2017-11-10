#include "gen_tlimits.h"


std::vector<std::pair<std::string, double>> tlimits(std::string const & ifile_path,
		method_struct const & method,
		double const run_pot,
		double const signal_pot,
		double const background_pot,
		double const data_pot) {

	object_helper<TH1> oh;

	TH1 * signal_hist = oh.GetObject(ifile_path, "ncdelta_cosmic_" + method.str + "_mva");
	TH1 * background_hist = oh.GetObject(ifile_path, "bnb_cosmic_background_" + method.str + "_mva");
	TH1 * data_hist = oh.GetObject(ifile_path, "bnb_cosmic_" + method.str + "_mva");

	signal_hist->Scale(run_pot / signal_pot);
	background_hist->Scale(run_pot / background_pot);
	data_hist->Scale(run_pot / data_pot);

	TLimitDataSource * mydatasource = new TLimitDataSource(signal_hist, background_hist, data_hist);
	TConfidenceLevel * myconfidence = TLimit::ComputeLimit(mydatasource, 50000);  

	std::vector<std::pair<std::string, double>> results;
	results.emplace_back("  CLs    : ", myconfidence->CLs());
	results.emplace_back("  CLsb   : ", myconfidence->CLsb());
	results.emplace_back("  CLb    : ", myconfidence->CLb());
	results.emplace_back("< CLs >  : ", myconfidence->GetExpectedCLs_b());
	results.emplace_back("< CLsb > : ", myconfidence->GetExpectedCLsb_b());
	results.emplace_back("< CLb >  : ", myconfidence->GetExpectedCLb_b());

	TCanvas * canvas = new TCanvas(method.str.c_str());
	myconfidence->Draw();
	TH1 * b_hist = (TH1*)gDirectory->Get("b_hist");
	b_hist->SetName((method.str+"_"+std::string(b_hist->GetName())).c_str());
	TH1 * sb_hist = (TH1*)gDirectory->Get("sb_hist");
	sb_hist->SetName((method.str+"_"+std::string(sb_hist->GetName())+"_"+method.str).c_str());
	canvas->Write();

	delete canvas;
	delete myconfidence;
	delete mydatasource;

	return results;

}



void tlimits(std::string const & ifile_path,
		std::vector<method_struct> const & methods,
		double const run_pot,
		double const signal_pot,
		double const background_pot,
		double const data_pot) {

	TFile * file = TFile::Open("tlimits.root", "recreate");

	std::cout << "Run POT: " << run_pot << "\n"
		<< "Signal POT: " << signal_pot << "\n"
		<< "Background POT: " << background_pot << "\n"
		<< "Data POT: " << data_pot << "\n";

	std::vector< std::vector< std::pair<std::string, double> > > results;
	std::vector< std::pair < std::string, std::vector<std::pair<double, double> > > > var_results;
	for(method_struct const & method : methods) {
		results.push_back(tlimits(ifile_path, method, run_pot, signal_pot, background_pot, data_pot));
	}

	file->Close();

	for(size_t i = 0; i < methods.size(); ++i) {
		auto const & v = results.at(i);
		std::cout << methods.at(i).str << "\n";
		for(auto const & p : v) {
			std::cout << p.first << p.second << "\n";
		}
		std::cout << "\n";
	}

}



std::pair<double, double>  tlimits_var_method(std::string const & ifile_path,
		method_struct const & method,
		double const run_pot,
		double const points,
		double const pot_min,
		double const pot_max,
		double const background_pot,
		double const data_pot) {

	std::vector<double> signal_pot_v;
	//std::vector<double> CLs_v;
	std::vector<double> CLsexp_v;

	object_helper<TH1> ohb;
	TH1 * background_hist = ohb.GetObject(ifile_path, "bnb_cosmic_background_" + method.str + "_mva");
	TH1 * data_hist = ohb.GetObject(ifile_path, "bnb_cosmic_" + method.str + "_mva");
	background_hist->Scale(run_pot / background_pot);
	data_hist->Scale(run_pot / data_pot);

	std::cout << "Method: " << method.str << "\n";

	size_t closest_index = SIZE_MAX;
	double closest_diff = DBL_MAX;

	TLimitDataSource * closestdatasource = nullptr;
	TConfidenceLevel * closestconfidence = nullptr;

	for(double signal_pot = pot_min; signal_pot <= pot_max; signal_pot += (pot_max - pot_min) / points) {

		object_helper<TH1> ohs;
		TH1 * signal_hist = ohs.GetObject(ifile_path, "ncdelta_cosmic_" + method.str + "_mva");
		signal_hist->Scale(run_pot / signal_pot);

		TLimitDataSource * mydatasource = new TLimitDataSource(signal_hist, background_hist, data_hist);
		TConfidenceLevel * myconfidence = TLimit::ComputeLimit(mydatasource, 50000);

		signal_pot_v.push_back(signal_pot);
		//CLs_v.push_back(myconfidence->CLs()); 
		CLsexp_v.push_back(myconfidence->GetExpectedCLs_b());

		if(fabs(CLsexp_v.back() - 0.1) < closest_diff) {
			closest_index = CLsexp_v.size() - 1;
			closest_diff = fabs(CLsexp_v.back() - 0.1);
			delete closestdatasource;
			delete closestconfidence;
			closestdatasource = mydatasource;
			closestconfidence = myconfidence;
		}

		if(myconfidence != closestconfidence) delete myconfidence;
		if(mydatasource != closestdatasource) delete mydatasource;

	}

	/*    
	      TCanvas * canvas_CLs = new TCanvas((method.str+"_CLs").c_str());
	      TGraph * graph_CLs = new TGraph(signal_pot_v.size(), &signal_pot_v.front(), &CLs_v.front());
	      graph_CLs->Draw();
	      graph_CLs->SetTitle(method.str.c_str());
	      graph_CLs->GetXaxis()->SetTitle("Signal POT");
	      graph_CLs->GetXaxis()->CenterTitle();
	      graph_CLs->GetYaxis()->SetTitle("CLs");
	      graph_CLs->GetYaxis()->CenterTitle();
	      canvas_CLs->Write();
	      delete graph_CLs;
	      delete canvas_CLs;
	 */

	TCanvas * canvas_CLsexp = new TCanvas((method.str+"_CLsexp").c_str());
	TGraph * graph_CLsexp = new TGraph(signal_pot_v.size(), &signal_pot_v.front(), &CLsexp_v.front());
	graph_CLsexp->Draw();
	graph_CLsexp->SetTitle(method.str.c_str());
	graph_CLsexp->GetXaxis()->SetTitle("Signal POT");
	graph_CLsexp->GetXaxis()->CenterTitle();
	graph_CLsexp->GetYaxis()->SetTitle("CLsexp");
	graph_CLsexp->GetYaxis()->CenterTitle();
	canvas_CLsexp->Write();
	delete graph_CLsexp;
	delete canvas_CLsexp;  

	TCanvas * canvas = new TCanvas((method.str + "_llplot").c_str());
	closestconfidence->Draw();
	TH1 * b_hist = (TH1*)gDirectory->Get("b_hist");
	b_hist->SetName((method.str+"_"+std::string(b_hist->GetName())).c_str());
	TH1 * sb_hist = (TH1*)gDirectory->Get("sb_hist");
	sb_hist->SetName((method.str+"_"+std::string(sb_hist->GetName())+"_"+method.str).c_str());
	canvas->Write();

	delete closestconfidence;
	delete closestdatasource;

	return closest_index != SIZE_MAX ? std::pair<double, double>(CLsexp_v.at(closest_index), signal_pot_v.at(closest_index)) : std::pair<double, double>(-1, -1);

}



void tlimits_var(std::string const & ifile_path,
		std::vector<method_struct> const & methods,
		double const run_pot,
		double const points,
		double const pot_min,
		double const pot_max,
		double const signal_pot,
		double const background_pot,
		double const data_pot) {

	TFile * file = TFile::Open("tlimits_var.root", "recreate");

	std::vector<std::pair<double, double>> results;

	for(method_struct const & method : methods) {
		results.push_back(tlimits_var_method(ifile_path, method, run_pot, points, pot_min, pot_max, background_pot, data_pot));
	}

	object_helper<TH1> oh;

	for(size_t i = 0; i < methods.size(); ++i) {
		std::cout << "Method: " << methods.at(i).str << " CLsexp: " << results.at(i).first << " Signal Vertices: " << oh.GetObject(ifile_path, "ncdelta_cosmic_" + methods.at(i).str + "_mva")->Integral() * run_pot / results.at(i).second << " Background Vertices: " << oh.GetObject(ifile_path, "bnb_cosmic_background_" + methods.at(i).str + "_mva")->Integral() * run_pot / background_pot << "\n";
	}

	file->Close();
	delete file;

}
