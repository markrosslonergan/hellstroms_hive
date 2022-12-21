#include "bdt_sig.h"



std::vector<double> scan_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot){
    return scan_significance(sig_files, bkg_files, bdt_infos, plot_pot, 0);
}

std::vector<double> scan_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot, int sig_type){
    std::cout<<"Starting to Scan Significance [Simple Linear Scan, can take a while!] "<<std::endl;
    //This just goes between some set values of all BDT's
    double best_significance = 0;
    std::vector<double> best_mva(bdt_infos.size(), DBL_MAX);

    std::cout<<"Setting stage entry lists"<<std::endl;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        sig_files.at(i)->setStageEntryList(1);
    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {
        bkg_files.at(i)->setStageEntryList(1);
    }


    //set up cut range
    int max_pts = 1;
    std::vector<std::vector<double>> bdt_scan_pts;
    auto grid = setup_bdt_cut_grid(bdt_infos, bdt_scan_pts, max_pts, sig_files, bkg_files);
    std::vector<double> maxvals = grid[0], minvals = grid[1], n_steps = grid[2], steps = grid[3];

    std::cout<<"We are going to scan between these values "<<std::endl;
    for(int i=0; i< bdt_infos.size();i++){
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<" Steps "<<steps[i]<<" (n_steps:  "<<n_steps[i]<<")"<<std::endl;
    }


    //set entrylist 
    std::cout<<"Setting Min entry lists and POT scaling"<<std::endl;
    std::vector<double> sig_pot_scale, bkg_pot_scale;
    for(size_t i = 0; i < sig_files.size(); ++i) {
	sig_files.at(i)->setStageEntryList(1+bdt_infos.size(), minvals);
        double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
	sig_pot_scale.push_back(pot_scale);
    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {
	bkg_files.at(i)->setStageEntryList(1+bdt_infos.size(), minvals);
        double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;
	bkg_pot_scale.push_back(pot_scale);
    }	



    //Calculate total signal for efficiency 
    double total_sig = 0.;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        std::string bnbcut = sig_files.at(i)->getStageCuts(1);      
        total_sig += sig_files.at(i)->GetEntries(bnbcut.c_str())*sig_pot_scale[i];	
    }

    auto sig_metric_name = significance_name(sig_type);
    std::cout<<"Starting"<<std::endl;
    std::cout<<"----------------------------------------------------"<<std::endl;
    std::string s_mod = "";
    int n_pt = 0;
    int best_pt = -9;


    for(int i=0; i < max_pts; i++){
	std::vector<double> cur_pt = grab_point(i, bdt_scan_pts);

        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t is = 0; is < sig_files.size(); ++is) {
            std::string bnbcut = sig_files.at(is)->getStageCuts(1+bdt_infos.size(), cur_pt); 
            signal += sig_files.at(is)->GetEntries(bnbcut.c_str())*sig_pot_scale[is];    
        }

        for(size_t ib = 0; ib < bkg_files.size(); ++ib) {
            std::string bnbcut = bkg_files.at(ib)->getStageCuts(1+bdt_infos.size(),cur_pt); 
            bkg.push_back(bkg_files.at(ib)->GetEntries(bnbcut)*bkg_pot_scale[ib]);			
            background += bkg.back();
        }

	double significance = calculate_significance(signal, background, sig_type, total_sig);

        if(significance > best_significance) {
            best_significance = significance;
            best_mva = cur_pt;
            best_pt = n_pt;
            s_mod = "(Current Best)";
        }

        std::cout<<"Point: "<<n_pt<<" (";
        for(auto &dd:cur_pt){
            std::cout<<dd<<",";   
        }
        std::cout<<") N_signal: "<<signal<<" N_bkg: "<<background<<" ||  " << sig_metric_name << ": " <<significance<<" "<<s_mod<<std::endl;

        s_mod = "";
        n_pt++;




    }

    std::cout<<"----------------------------------------------------"<<std::endl;
    std::cout<<"------------ Finished. Best " << sig_metric_name << " was  "<<best_significance<<" at point "<<best_pt<<" with Cuts at "<<std::endl;
    for(auto &dd: best_mva){
        std::cout<<dd<<" ";   
    }
    std::cout<<std::endl;
    std::cout<<"Done with simple significance scan"<<std::endl;

    return std::vector<double>{0,0,0};

}

std::vector<double> scan_chisquare_sys_fixed(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, TMatrixT<double>* fixed_frac_matrix, double plot_pot, std::string pdfname, double signal_scale){
    int stage = bdt_infos.size() + 1;
    bool _debug = false; //print debugging information
    if(_debug){
	std::cout << "Print fractional covariance matrix " << std::endl; 

	for(int i = 0; i != fixed_frac_matrix->GetNrows(); ++i){
	    for(int j = 0; j != fixed_frac_matrix->GetNrows(); ++j)
		std::cout << "(" << i << ", " << j << ") " <<  (*fixed_frac_matrix)(i,j) << std::endl;
	}
    }

 
    //grab POT of stacked histogram
    double current_pot = stack->getPOT();
    double pot_scale = plot_pot / current_pot; 

    //set up cut range
    int max_pts = 1;
    std::vector<std::vector<double>> bdt_scan_pts;
    auto grid = setup_bdt_cut_grid(bdt_infos, bdt_scan_pts, max_pts, stack);
    std::vector<double> maxvals = grid[0], minvals = grid[1], n_steps = grid[2], steps = grid[3];

    std::cout<<"We are going to scan between these values "<<std::endl;
    std::cout<< "Total of " << max_pts <<" grid points to scan" << std::endl;
    for(int i=0; i< bdt_infos.size();i++){
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<" Steps "<<steps[i]<<" (n_steps:  "<<n_steps[i]<<")"<<std::endl;
    }
    std::cout << "POT: " << plot_pot << " | signal scaling: " << signal_scale << std::endl;

    //set entrylist 
    std::cout<<"Setting Min entry lists"<<std::endl;
    for(auto f: stack->getFiles()) {
	f->setStageEntryList(1+bdt_infos.size(), minvals);
    }


    //set up SBNchi 
    // first, form a valid xml, for SBNchi to use
    bdt_covar covar_handle(&var, stage);
    std::string template_xml = covar_handle.GetTemplateXmls().at(0);
    std::string xml = covar_handle.PrepareXml(template_xml, "SBNchi_Helper");
    std::cout << "xml: " << xml << std::endl;

    sbn::SBNchi chi_handle(xml);	
    chi_handle.is_stat_only = false;
 

    int best_point = -1;
    double best_chi = 0;
    std::vector<double> chivec, sigvec;
    int start_time = time(nullptr);
    // iterate through all grid point
    for(int i =0; i != max_pts; ++i){

	std::vector<double> cur_pt = grab_point(i, bdt_scan_pts);

	auto full_vector = stack->getFullVector(var, stage, cur_pt, signal_scale);
	auto full_bkg_spec = stack->getFullVectorWithError(var, stage, cur_pt, true); //same dimension as full vector, but skip signal contribution
	auto full_bkg_vector = full_bkg_spec[0], full_bkg_error = full_bkg_spec[1]; 

	if(_debug &&  i%100 == 0 ){
	    std::cout << "Print full vector : ";
	    for(auto& f : full_vector){
		std::cout << f << " ";
	    } 
	    std::cout << " Total of " << std::accumulate(full_vector.begin(), full_vector.end(), 0.0) << std::endl;
	     
	    std::cout << "Print background vector : ";
	    for(auto& f : full_bkg_vector){
		std::cout << f << " ";
	    } 
	    std::cout << " Total of " << std::accumulate(full_bkg_vector.begin(), full_bkg_vector.end(), 0.0) << std::endl;

	    std::cout << "Print background error : ";
	    for(auto& f : full_bkg_error){
		std::cout << f << " ";
	    } 
	    std::cout << " Total of " << std::accumulate(full_bkg_error.begin(), full_bkg_error.end(), 0.0) << std::endl;

	    std::cout << "POT Scale " << pot_scale << std::endl;
	    std::cout << "Signal Scale " << signal_scale << std::endl;
	}

	double current_chi = calculate_chi(&chi_handle, fixed_frac_matrix, full_vector, full_bkg_vector, full_bkg_error, pot_scale);	
	chivec.push_back(current_chi);

	    
	//grab stats S/sqrt(B) ratio
	double total_bkg = std::accumulate(full_bkg_vector.begin(), full_bkg_vector.end(), 0.0);
    	double total_sig = std::accumulate(full_vector.begin(), full_vector.end(), 0.0) - total_bkg;
	sigvec.push_back(calculate_significance(total_sig, total_bkg, 0));

	if(_debug && i%100 == 0 ){
	    std::cout << "Print full vector : ";
	    for(auto& f : full_vector){
		std::cout << f << " ";
	    } 
	    std::cout << " Total of " << std::accumulate(full_vector.begin(), full_vector.end(), 0.0) << std::endl;
	     
	    std::cout << "Print background vector : ";
	    for(auto& f : full_bkg_vector){
		std::cout << f << " ";
	    } 
	    std::cout << " Total of " << std::accumulate(full_bkg_vector.begin(), full_bkg_vector.end(), 0.0) << std::endl;

	    std::cout << "Print background error : ";
	    for(auto& f : full_bkg_error){
		std::cout << f << " ";
	    } 
	    std::cout << " Total of " << std::accumulate(full_bkg_error.begin(), full_bkg_error.end(), 0.0) << std::endl;

	    std::cout << "Current chi: " << current_chi << std::endl;
	}

	if(current_chi > best_chi){
	    best_chi = current_chi;
	    best_point = i;
	    std::cout << "at Point " << i << " Corresponding cuts: (";
	    for(auto& p : cur_pt)
		std::cout <<  p << ", ";
	    std::cout << ")  ------ Current best chi " << best_chi << " P-value: " << TMath::Prob(best_chi, chi_handle.num_bins_total_compressed);
    
	    std::cout << "  signal prediction: " << total_sig << "  background prediction: " << total_bkg << " S/sqrt(B) is " << sigvec.back() << std::endl; 
	}

	if( (i+1) % 100 ==0){
	    std::cout << i << "/" << max_pts << " Running over 100 points costs " << time(nullptr) - start_time << " seconds" << std::endl;
	    start_time = time(nullptr);
	}
    }
    
    std::cout<<"----------------------------------------------------"<<std::endl;
    std::cout<<"------------ Finished. Largest chi was  "<< best_chi <<" at point "<< best_point <<" with Cuts at ( "<<std::endl;
    auto best_pt = grab_point(best_point, bdt_scan_pts);
    for(auto &dd: best_pt){
        std::cout<<dd<<" ";
    }
    std::cout<<")" << std::endl;

    std::cout<<"Done with chi2 scan with systeamtic uncertainties"<<std::endl;

    make_project_plots(bdt_scan_pts, bdt_infos, chivec, "Marginalized #chi^{2}", pdfname + "_Chi.pdf");
    make_project_plots(bdt_scan_pts, bdt_infos, sigvec, "Marginalized S/sqrt(B)", pdfname + "_Significance.pdf");
    return best_pt;
}

std::vector<double> scan_chisquare_sys_fixed(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& cuts, double plot_pot, std::string pdfname, double signal_scale){
    int stage = bdt_infos.size() + 1;

    //generates the covaraince matrix at given bdt cuts 
    std::string stage_cuts = stack->getFiles().at(0)->getGeneralStageCuts(stage, cuts, true);

    // only consider flux and xs uncertainty for now
    bdt_covar covar_handle(&var, stage, stage_cuts);
    covar_handle.GenerateReweightingCovar();
    TFile* covar_f = new TFile(var.GetCovarFile(stage).c_str(),"read");
    TMatrixT<double>* covar_matrix = (TMatrixT<double>*)covar_f->Get(var.covar_name.c_str());

    auto res = scan_chisquare_sys_fixed(stack, bdt_infos, var, covar_matrix, plot_pot, pdfname, signal_scale);
    covar_f->Close();
    return res; 
}


std::vector<double> random_scan_chisquare_sys_fixed(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, TMatrixT<double>* fixed_frac_matrix, double plot_pot, int num_scans, double signal_scale){
    int stage = bdt_infos.size() + 1;
 
    //grab POT of stacked histogram
    double current_pot = stack->getPOT();
    double pot_scale = plot_pot / current_pot; 

    //set up cut range
    int dummy_pts;
    std::vector<std::vector<double>> dummy_bdt_scan_pts;
    auto grid = setup_bdt_cut_grid(bdt_infos, dummy_bdt_scan_pts, dummy_pts, stack);
    std::vector<double> maxvals = grid[0], minvals = grid[1];

    std::cout<<"We are going to scan randomly between these values "<<std::endl;
    std::cout<< "Total of " << num_scans <<" random scans" << std::endl;
    for(int i=0; i< bdt_infos.size();i++){
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<std::endl;
    }

    //random number generator 
    unsigned seed = 1; 
    std::default_random_engine generator(seed);
    std::vector<std::uniform_real_distribution<double>> random_dists; 
    for(int i=0; i< bdt_infos.size();i++){
	random_dists.push_back(std::uniform_real_distribution<double>(minvals[i], maxvals[i]));
    }

    //set entrylist 
    std::cout<<"Setting Min entry lists and POT scaling"<<std::endl;
    for(auto f: stack->getFiles()) {
	f->setStageEntryList(1+bdt_infos.size(), minvals);
    }


    //set up SBNchi 
    // first, form a valid xml, for SBNchi to use
    bdt_covar covar_handle(&var, stage);
    std::string template_xml = covar_handle.GetTemplateXmls().at(0);
    std::string xml = covar_handle.PrepareXml(template_xml, "SBNchi_Helper");
    std::cout << "xml: " << xml << std::endl;

    sbn::SBNchi chi_handle(xml);	
    chi_handle.is_stat_only = false;



    std::vector<double> best_chi_pt, best_significance_pt; 
    double best_chi = 0, best_significance = 0;
    int start_time = time(nullptr);
    // iterate through all grid point
    for(int i =0; i != num_scans; ++i){

        std::vector<double> cur_pt; 
        for(auto& dist : random_dists){
	    cur_pt.push_back( round_to( dist(generator), 0.001 ) );
        }

	auto full_vector = stack->getFullVector(var, stage, cur_pt, signal_scale);
	auto full_bkg_spec = stack->getFullVectorWithError(var, stage, cur_pt, true); //skip signal contribution
	auto full_bkg_vector = full_bkg_spec[0], full_bkg_error = full_bkg_spec[1]; 

	double current_chi = calculate_chi(&chi_handle, fixed_frac_matrix, full_vector, full_bkg_vector, full_bkg_error, pot_scale);    
	if(current_chi > best_chi){
	    best_chi = current_chi;
	    best_chi_pt = cur_pt;
	    std::cout << "[BEST CHI] ";
	}

	    
	//grab stats S/sqrt(B) ratio
	double total_bkg = std::accumulate(full_bkg_vector.begin(), full_bkg_vector.end(), 0.0);
    	double total_sig = std::accumulate(full_vector.begin(), full_vector.end(), 0.0) - total_bkg;
	double current_sig = calculate_significance(total_sig, total_bkg, 0);

	if(current_sig > best_significance){
	    best_significance = current_sig;
	    best_significance_pt = cur_pt;
	    std::cout << "[BEST SIG] ";
	}

	// print info for every point
	if(true){
	    std::cout << "at Point " << i << " Corresponding cuts: (";
	    for(auto& p : cur_pt)
		std::cout <<  p << ", ";
	    std::cout << ")  ------ Current chi " << current_chi << " P-value: " << TMath::Prob(best_chi, chi_handle.num_bins_total_compressed) << " signal prediction: " << total_sig << "  background prediction: " << total_bkg << " S/sqrt(B) is " << current_sig << std::endl; 
	}

	if( (i+1) % 100 ==0){
	    std::cout << i << "/" << num_scans << " Running over 100 points costs " << time(nullptr) - start_time << " seconds" << std::endl;
	    start_time = time(nullptr);
	}
    }
    
    std::cout<<"----------------------------------------------------"<<std::endl;
    std::cout<<"------------ Finished. Largest chi was  "<< best_chi <<" at Cuts ( "<<std::endl;
    for(auto &dd: best_chi_pt){
        std::cout<<dd<<" ";
    }
    std::cout<<")" << std::endl;

    std::cout<<"Done with chi2 scan with systeamtic uncertainties"<<std::endl;

    return best_chi_pt;
}

std::vector<double> random_scan_chisquare_sys_fixed(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& cuts, double plot_pot, int num_scans, double signal_scale){
    int stage = bdt_infos.size() + 1;

    //generates the covaraince matrix at given bdt cuts 
    std::string stage_cuts = stack->getFiles().at(0)->getGeneralStageCuts(stage, cuts, true);

    // only consider flux and xs uncertainty for now
    bdt_covar covar_handle(&var, stage, stage_cuts);
    covar_handle.GenerateReweightingCovar();
    TFile* covar_f = new TFile(var.GetCovarFile(stage).c_str(),"read");
    TMatrixT<double>* covar_matrix = (TMatrixT<double>*)covar_f->Get(var.covar_name.c_str());

    auto res = random_scan_chisquare_sys_fixed(stack, bdt_infos, var, covar_matrix, plot_pot, num_scans, signal_scale);
    covar_f->Close();
    return res; 
}

std::vector<double> random_scan_chisquare_stat(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& external_bdt_cuts, double plot_pot, int num_scans, double signal_scale){

    int num_subchannel = stack->getNFiles();
    int num_bins = var.GetNBins();
    int total_bins = num_subchannel * num_bins;

    TMatrixT<double> stat_matrix(total_bins, total_bins);
    stat_matrix.Zero();
    auto res = random_scan_chisquare_sys_fixed(stack, bdt_infos, var, &stat_matrix, plot_pot, num_scans, signal_scale); 

    return res; 
}

std::vector<double> scan_chisquare_stat(bdt_stack* stack,  std::vector<bdt_info> bdt_infos, bdt_variable var, std::vector<double>& external_bdt_cuts, double plot_pot, std::string pdfname, double signal_scale){

    int num_subchannel = stack->getNFiles();
    int num_bins = var.GetNBins();
    int total_bins = num_subchannel * num_bins;

    TMatrixT<double> stat_matrix(total_bins, total_bins);
    stat_matrix.Zero();
    auto res = scan_chisquare_sys_fixed(stack, bdt_infos, var, &stat_matrix, plot_pot, pdfname, signal_scale);

    return res; 
}


std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_info, double plot_pot){
    return scan_significance_random(sig_files,bkg_files,bdt_info,plot_pot,0);
}

std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot, int sig_type){
    std::cout<<"Starting to Scan Significance (randomly)"<<std::endl;
    double best_significance = 0;
    double best_impact = 0;
    std::vector<double> best_mva(bdt_infos.size(), DBL_MAX);

    double sig_scale = 1.0;

    std::cout<<"Setting stage entry lists"<<std::endl;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        sig_files.at(i)->setStageEntryList(1);
        std::cout<<sig_files[i]->tag<<" is a signal file"<<std::endl;
    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {
        bkg_files.at(i)->setStageEntryList(1);
        std::cout<<bkg_files[i]->tag<<" is a BKG file"<<std::endl;
    }


    //set up cut range
    int max_pts = 1;
    std::vector<std::vector<double>> bdt_scan_pts;
    auto grid = setup_bdt_cut_grid(bdt_infos, bdt_scan_pts, max_pts, sig_files, bkg_files);
    std::vector<double> maxvals = grid[0], minvals = grid[1];


    std::cout<<"We are going to scan between these values "<<std::endl;
    for(int i=0; i< bdt_infos.size();i++){
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<std::endl;
    }


    //set entrylist 
    std::cout<<"Setting Min entry lists and POT scaling"<<std::endl;
    std::vector<double> sig_pot_scale, bkg_pot_scale;
    for(size_t i = 0; i < sig_files.size(); ++i) {
	sig_files.at(i)->setStageEntryList(1+bdt_infos.size(), minvals);
        double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
	sig_pot_scale.push_back(pot_scale);
    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {
	bkg_files.at(i)->setStageEntryList(1+bdt_infos.size(), minvals);
        double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;
	bkg_pot_scale.push_back(pot_scale);
    }	

    //Calculate total signal for efficiency 
    double total_sig = 0.;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        std::string bnbcut = sig_files.at(i)->getStageCuts(1);      
        total_sig += sig_files.at(i)->GetEntries(bnbcut.c_str())*sig_pot_scale[i];	

    }

    std::string s_mod = "";
    auto sig_metric_name = significance_name(sig_type);
    TRandom3 *rangen  = new TRandom3(0);  
    std::cout<<"Starting"<<std::endl;
    for(int t=0; t < 100000; t++){

        std::vector<double> d (bdt_infos.size(),0);
        for(int i=0; i< bdt_infos.size(); i++){
            d[i] = rangen->Uniform(minvals[i], maxvals[i]);
            d[i] = std::round(d[i]*1000.0)/1000.0;    //to make cut value only have 3 decimal point?
        }
        

        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t is = 0; is < sig_files.size(); ++is) {
            std::string bnbcut = sig_files.at(is)->getStageCuts(1+bdt_infos.size(), d); 
            signal += sig_files.at(is)->GetEntries(bnbcut.c_str())*sig_pot_scale[is];    
        }

        for(size_t ib = 0; ib < bkg_files.size(); ++ib) {
            std::string bnbcut = bkg_files.at(ib)->getStageCuts(1+bdt_infos.size(),d); 
            bkg.push_back(bkg_files.at(ib)->GetEntries(bnbcut)*bkg_pot_scale[ib]);			
            background += bkg.back();
            //std::cout<<" "<<bkg_files[i]->tag<<" "<<bkg.back();
        }
        //std::cout<<std::endl;


	double significance = calculate_significance(signal, background, sig_type, total_sig, sig_scale);
        if(significance > best_significance) {
            best_significance = significance;
            best_mva = d;
            s_mod = "(Current Best)";
        }

        for(auto &dd:d){
            std::cout<<dd<<",";   
        }
        std::cout<<") N_BKG (";
        for(auto &dd:bkg){
            std::cout<<dd<<" ";   
        }
        std::cout<<")  N_signal: "<<signal<<" (E_signal: "<<(signal/(double)total_sig)<<") N_bkg: "<<background<<" ||  " << sig_metric_name << " : " <<significance<<" "<<s_mod<<std::endl;
        s_mod = "";

    
    }

    std::cout<<"------------_FINAL Best " << sig_metric_name << ": "<<best_significance<<std::endl;

    std::cout<<"  --ccut: ";
    for(auto &dd: best_mva){
        std::cout<<dd<<" ";   
    }


    return std::vector<double>{0,0,0};

}


int scan_likelihood(std::vector<bdt_file*> stack_files, std::vector<bdt_info> bdt_infos){

    std::cout<<"Starting the (somewhat) experimental likelihood combination of BDTs"<<std::endl;
    TCanvas *ctest = new TCanvas();
    TPad *padtop = new TPad("padtop_just", "padtop_just", 0, 0.35, 1, 1.0);
    padtop->SetLogy();
    padtop->SetBottomMargin(0); // Upper and lower plot are joined
    padtop->Draw();             // Draw the upper pad: pad2top
    padtop->cd();
    THStack *stacked = new THStack("slimp","slimp");

    std::vector<double> maxvec;
    std::vector<double> minvec;

    std::vector<double> tpoint;
    for(double k=0; k<1.0; k=k+0.0025){
        tpoint.push_back(k);
    }
    std::vector<double> sig(tpoint.size(),0);
    std::vector<double> bkg(tpoint.size(),0);


    for(auto &b: bdt_infos){

        double minval = 999;
        double maxval = -999;

        for(auto &f: stack_files){
            f->setStageEntryList(1);

            std::string precut_list_name = b.identifier+"_"+f->tag+"_"+"lust";
            std::string name = f->tag +"_"+b.identifier+".mva";

            TTreeFormula * tf_tmp = new TTreeFormula(("tf_"+precut_list_name).c_str(), name.c_str(), f->tvertex);
            for(int i = 0; i < f->tvertex->GetEntries(); ++i) {
                f->tvertex->GetEntry(i);
                tf_tmp->GetNdata();
                double thisval = tf_tmp->EvalInstance();

                if(thisval>0){
                    minval = std::min(minval,thisval);                     
                }

            }
            maxval = std::max(maxval, f->tvertex->GetMaximum(name.c_str()));

        }
        maxvec.push_back(maxval);
        minvec.push_back(minval);
        std::cout<<"So Total Min "<<minvec.back()<<"  -- > "<<maxvec.back()<<std::endl;
        std::cout<<"-----"<<std::endl;
    }

    //now scale
    for(auto &f: stack_files){
        f->setStageEntryList(1);
        std::string cat_name = "1.0";
        for(int b=0; b< bdt_infos.size();b++){
            std::string name = f->tag +"_"+bdt_infos[b].identifier+".mva";
            //cat_name += "*( (" + name+"-"+std::to_string(minvec[b]) + ")/("+std::to_string(maxvec[b])+"-"+std::to_string(minvec[b])+"))";
            cat_name = "min( "+cat_name+" , (" + name+"-"+std::to_string(minvec[b]) + ")/("+std::to_string(maxvec[b])+"-"+std::to_string(minvec[b])+"))";
            //cat_name += "+(" + name+"-"+std::to_string(minvec[b]) + ")/("+std::to_string(maxvec[b])+"-"+std::to_string(minvec[b])+")";

        }
        padtop->cd();
        bdt_variable cat_var( "("+cat_name+")/1.0","(50,0,1)","Liklihood",false,"d");
        //bdt_variable cat_var("("+cat_name+")/"+std::to_string(bdt_infos.size()),"(50,0,1)","Liklihood",false,"d");
        TH1 * tmp = (TH1*)f->getTH1(cat_var , "1",f->tag+"_"+cat_var.safe_name, 13.2e20);
        padtop->cd();
        tmp->SetFillColor(f->col);
        stacked->Add(tmp);


        std::cout<<"Starting a s/sqrt(b) scan of "<<f->tag<<std::endl;
        //now for sig/sqrt b
        for(int t=0; t<tpoint.size(); t++){
            double v = f->GetEntries(cat_name+">"+std::to_string(tpoint[t]))*(13.2e20/f->pot)*f->scale_data;
            if(f->tag == "NCDeltaRadOverlay" || f->tag == "NCDeltaRadOverlayOther"){
                sig[t] +=v;
            }else{
                bkg[t] +=v;
            }
        }

    }



    padtop->cd(); // pad2top becomes the current pad

    stacked->Draw("hist");
    stacked->SetMaximum(1e4);
    stacked->SetMinimum(1e-2);

    ctest->cd();
    TPad *padbot = new TPad("padbot_just","padbot_just", 0, 0.05, 1, 0.35);
    padbot->SetTopMargin(0);
    padbot->SetBottomMargin(0.351);
    padbot->SetGridx(); // vertical grid
    padbot->Draw();
    padbot->cd();       // pad0bot becomes the current pad


    std::vector<double> signif;
    for(int t=0; t< sig.size(); t++){
        if(bkg[t]!=0){
            signif.push_back(sig[t]/sqrt(bkg[t]));
        }else{
            signif.push_back(0);
        }
        std::cout<<"Point t: "<<tpoint[t]<<" "<<sig[t]<<" "<<bkg[t]<<" "<<signif[t]<<std::endl;
    }

    TGraph * g = new TGraph(signif.size(),&tpoint[0],&signif[0]);
    g->Draw("ACP");

    ctest->SaveAs("sig_scan_liklihood.pdf","pdf");

    return 0;
}



std::vector<double> lin_scan(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt, double c1, double c2){
    std::cout<<"Starting to Scan Significance"<<std::endl;
    double best_significance = 0;
    double best_mva_cut = DBL_MAX;
    double best_mva_cut2 = DBL_MAX;

    //	double plot_pot = 6.6e20;
    double plot_pot = 13.2e20;


    std::vector<double> vec_sig;//some vectors to store TGraph info;
    std::vector<double> vec_cut;	


    double d1 = c1;
    double d2 = c2; 


    for(int i=0; i< 100; i++){

        d1 = d1*1.0001;
        //d1 = d1*0.99999;

        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t i = 0; i < sig_files.size(); ++i) {
            double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;

            std::string bnbcut = sig_files.at(i)->getStageCuts(3,d1,d2); 
            signal += sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;

        }

        for(size_t i = 0; i < bkg_files.size(); ++i) {
            double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;


            std::string bnbcut = bkg_files.at(i)->getStageCuts(3,d1,d2); 
            bkg.push_back(	bkg_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale);			

            background += bkg.back();
        }
        double significance =0;
        if(signal==0){
            significance =0;
        }else if(background !=0){
            significance = signal/sqrt(background);
        }else{
            std::cout<<"method_best_significane_seperate || signal2+background2 == 0, so significance  = nan @ cut1: "<<d1<<", cut2: "<<d2<<std::endl;
            break;
        }


        if(significance > best_significance) {
            best_significance = significance;
            best_mva_cut = d1;
            best_mva_cut2 = d2;
        }


        std::cout<<"ccut: "<<d1<<" bcut: "<<d2<<" "<<" #signal: "<<signal<<" #bkg: "<<background<<" || "<<" bnb: "<<bkg.at(0)<<" cos: "<<bkg.at(1)<<" || "<<significance<<std::endl;
        vec_sig.push_back(significance);
        vec_cut.push_back(d1);
        vec_cut.push_back(d2);
    }


    return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};

}



std::vector<double> scan_significance(TFile * fout, std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, bdt_info cosmic_focused_bdt, bdt_info bnb_focused_bdt){
    std::cout<<"Starting to Scan Significance ----- WARNING OLD OLD OBSOLETE FUNCTION----------------ERROR"<<std::endl;
    double best_significance = 0;
    double best_mva_cut = DBL_MAX;
    double best_mva_cut2 = DBL_MAX;

    double plot_pot = 13.2e20;


    //for nice plots make the 50, 25 is quicker tho
    int nsteps_cosmic = 20;//50
    double cut_min_cosmic = 999;
    double cut_max_cosmic = -999;

    int nsteps_bnb = 20;//50
    double cut_min_bnb = 999;//0.52;
    double cut_max_bnb = -999;

    std::cout<<"Setting stage entry lists"<<std::endl;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        sig_files.at(i)->setStageEntryList(1);
    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {
        bkg_files.at(i)->setStageEntryList(1);
    }

    for(size_t i = 0; i < sig_files.size(); ++i) {
        //	double tmin_cos = sig_files.at(i)->tvertex->GetMinimum( (sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name + ">0").c_str()    );
        double tmax_cos = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name.c_str()    );
        double tmax_bnb = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(bnb_focused_bdt).name.c_str()    );

        std::cout<<sig_files.at(i)->getBDTVariable(cosmic_focused_bdt).name<<" MaxCos: "<<tmax_cos<<" MaxBnb: "<<tmax_bnb<<std::endl;
        //if( tmin_cos <= cut_min_cosmic) cut_min_cosmic=tmin_cos;
        if( tmax_cos >= cut_max_cosmic) cut_max_cosmic=tmax_cos;
        if( tmax_bnb >= cut_max_bnb) cut_max_bnb=tmax_bnb;

    }
    // Normally *0.7 or *0.8
    cut_min_cosmic = cut_max_cosmic*0.8;
    cut_min_bnb = cut_max_bnb*0.8;

    // These are normally *1.0
    cut_max_cosmic =cut_max_cosmic*0.9;
    cut_max_bnb =cut_max_bnb*0.9; 

    //Zoomed in notrack
    //	cut_min_cosmic = 0.54; cut_max_cosmic = 0.58;
    //	cut_min_bnb = 0.51; cut_max_bnb = 0.552;

    //Best Fit Significance: 0.601552 0.533678 1.63658
    //Zoomed in track
    //	cut_min_cosmic = 0.69; cut_max_cosmic = 0.71;
    //	cut_min_bnb = 0.56; cut_max_bnb = 0.58;




    //Create 2 tempoary TEntryLists  at minimum
    std::vector<TEntryList*> sig_min_lists;
    std::vector<TEntryList*> bkg_min_lists;


    std::cout<<"Setting Min entry lists"<<std::endl;
    for(size_t i = 0; i < sig_files.size(); ++i) {


        std::string min_list_name  = "micam"+std::to_string(i);
        sig_files.at(i)->tvertex->Draw((">>"+min_list_name).c_str(), sig_files.at(i)->getStageCuts(3, cut_min_cosmic,cut_min_bnb).c_str() , "entrylist");
        sig_min_lists.push_back(  (TEntryList*)gDirectory->Get(min_list_name.c_str()) );
        sig_files.at(i)->tvertex->SetEntryList(sig_min_lists.back());


    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {

        std::string min_list_name  = "mibam"+std::to_string(i);
        bkg_files.at(i)->tvertex->Draw((">>"+min_list_name).c_str(), bkg_files.at(i)->getStageCuts(3, cut_min_cosmic,cut_min_bnb).c_str() , "entrylist");
        bkg_min_lists.push_back(  (TEntryList*)gDirectory->Get(min_list_name.c_str()) );
        bkg_files.at(i)->tvertex->SetEntryList(bkg_min_lists.back());


    }	


    std::cout<<"BNB sig scan from: "<<cut_min_bnb<<" to "<<cut_max_bnb<<std::endl;
    std::cout<<"COSMIC sig scan from: "<<cut_min_cosmic<<" to "<<cut_max_cosmic<<std::endl;

    double step_cosmic = (cut_max_cosmic-cut_min_cosmic)/((double)nsteps_cosmic);
    double step_bnb = (cut_max_bnb-cut_min_bnb)/((double)nsteps_bnb);


    TH2D * h2_sig_cut = new TH2D( "significance_2D",  "significance_2D",nsteps_cosmic, cut_min_cosmic, cut_max_cosmic, nsteps_bnb, cut_min_bnb, cut_max_bnb);
    std::vector<double> vec_sig;//some vectors to store TGraph info;
    std::vector<double> vec_cut;	

    // Calculate total signal for efficiency 
    double total_sig = 0.;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
        //        std::cout << "POT scale: " << pot_scale << std::endl;
        std::string bnbcut = sig_files.at(i)->getStageCuts(1,-9,-9); 
        total_sig += sig_files.at(i)->tvertex->GetEntries(bnbcut.c_str())*pot_scale;
    }

    for(int di=1; di<=nsteps_cosmic; di++) {
        double d  = (double)(di-1.0)*step_cosmic + cut_min_cosmic; ;	

        for(int di2=1; di2<=nsteps_bnb; di2++) {
            double d2  = (double)(di2-1.0)*step_bnb + cut_min_bnb ;	

            double signal = 0;
            double background = 0;
            std::vector<double> bkg;	

            for(size_t i = 0; i < sig_files.size(); ++i) {
                double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;

                std::string bnbcut = sig_files.at(i)->getStageCuts(3,d,d2); 
                signal += sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;

            }

            for(size_t i = 0; i < bkg_files.size(); ++i) {
                double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;


                std::string bnbcut = bkg_files.at(i)->getStageCuts(3,d,d2); 
                bkg.push_back(	bkg_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale);			

                background += bkg.back();
            }



            double significance =0;
            if(signal==0){
                significance =0;
            }else if(background !=0){
                //significance = signal/(signal+background)*signal/total_sig*100;
                significance = signal/sqrt(background);
            }else{
                std::cout<<"method_best_significane_seperate || signal2+background2 == 0, so significance  = nan @ cut1: "<<d<<", cut2: "<<d2<<std::endl;
                break;
            }


            if(significance > best_significance) {
                best_significance = significance;
                best_mva_cut = d;
                best_mva_cut2 = d2;
            }


            std::cout<<"ccut: "<<d<<" bcut: "<<d2<<" "<<" #signal: "<<signal<<" #bkg: "<<background<<" || "<<" bnb: "<<bkg.at(0)<<" cos: "<<bkg.at(1)<<" || "<<significance<<std::endl;
            vec_sig.push_back(significance);
            vec_cut.push_back(d2);
            h2_sig_cut->SetBinContent(di,di2, significance);
        }

    }


    h2_sig_cut->SetStats(false);
    TCanvas * c_sig_cuts =  new TCanvas( "significance_cuts_colz", "significance_cuts_colz", 2000,1600 );
    c_sig_cuts->Divide(2,1);
    TPad *p1 = (TPad*)c_sig_cuts->cd(1);
    p1->SetRightMargin(0.13);
    h2_sig_cut->Draw("colz");
    h2_sig_cut->GetXaxis()->SetTitle("Cosmic Cut");
    h2_sig_cut->GetYaxis()->SetTitle("BNB Cut");

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
    c_sig_cuts->Write();

    return std::vector<double>{best_mva_cut, best_mva_cut2, best_significance};

}


std::vector<double> super_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files){
    std::cout<<"Starting to Scan Super Significance "<<std::endl;
    double best_significance = 0;
    double best_mva;
    double plot_pot = 10.115e20;

    std::cout<<"Setting stage entry lists"<<std::endl;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        sig_files.at(i)->setStageEntryList(1);
    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {
        bkg_files.at(i)->setStageEntryList(1);
    }

    // Calculate total signal for efficiency 
    double total_sig = 0.;
    std::vector<double>minvals = {0,0,0,0};

    for(size_t i = 0; i < sig_files.size(); ++i) {
        double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
        std::cout << "POT scale: " << pot_scale << std::endl;

        std::string bnbcut = sig_files.at(i)->getStageCuts(1,minvals); 
        total_sig += sig_files.at(i)->tvertex->GetEntries(bnbcut.c_str())*pot_scale;
    }

    std::string s_mod = "";
    TRandom3 *rangen  = new TRandom3(0);  
    std::cout<<"Starting"<<std::endl;
    for(double t=0.99999; t <= 1.0; t+=(1.0-0.99999)/100.0){

        std::string s_impact = "1";

        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t i = 0; i < sig_files.size(); ++i) {
            double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;

            std::string bnbcut = "SUPER_score > "+std::to_string(t); 
            signal += sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;
        }

        for(size_t i = 0; i < bkg_files.size(); ++i) {
            double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;

            std::string bnbcut = "SUPER_score > "+std::to_string(t); 
            bkg.push_back(bkg_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale);			
            background += bkg.back();
        }

        
        double significance =0;
        if(signal==0){
            significance =0;
        }else if(background !=0){
            //significance = signal/(signal+background);
            significance = signal/sqrt(background);

        }else{
            std::cout<<"method_best_significane_seperate || signal2+background2 == 0 , sig: "<<signal<<" , so significance  = nan. Woopsie."<<std::endl;
            //break;
        }
        if(background<0.0001) break;
        if(significance > best_significance) {
            best_significance = significance;
            best_mva = t;
            s_mod = "(Current Best)";
        }

        std::cout<<"("<<t<<")  N_signal: "<<signal<<" N_bkg: "<<background<<" ||  Sigma: " <<significance<<" "<<s_mod<<std::endl;

        s_mod = "";

    
    }

    std::cout<<"------------_FINAL Best Sig: "<<best_significance<<std::endl;

    std::cout<<"  --ccut: "<<best_mva<<std::endl;


    return std::vector<double>{0,0,0};

}


std::vector<double> scan_significance_linlin(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, std::vector<double> bdt_cuts, int which_bdt, int which_bdt2){


    std::cout<<"Starting to Scan Significance LinLin Grid"<<std::endl;
    double best_significance = 0;
    double best_impact = 0;
    std::vector<double> best_mva(bdt_infos.size(), DBL_MAX);



    double plot_pot = 6.8e20;//10.115e20;

    std::cout<<"Setting stage entry lists"<<std::endl;
    for(size_t i = 0; i < sig_files.size(); ++i) {
        sig_files.at(i)->setStageEntryList(1);
        std::cout<<sig_files[i]->tag<<" is a signal file"<<std::endl;
    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {
        bkg_files.at(i)->setStageEntryList(1);
        std::cout<<bkg_files[i]->tag<<" is a BKG file"<<std::endl;
    }

    std::vector<double> in_min_vals;
    std::vector<double> in_max_vals;
    std::vector<double> n_steps;

    for(size_t b=0; b<bdt_infos.size();b++){
	//if(!(b==(size_t)which_bdt || b==(size_t)which_bdt2))bdt_cuts[b]=0.0;//
        in_min_vals.push_back(bdt_infos[b].TMVAmethod.scan_min);
        in_max_vals.push_back(bdt_infos[b].TMVAmethod.scan_max);
        n_steps.push_back(bdt_infos[b].TMVAmethod.scan_steps);
        //std::cout<<"AGHR "<<in_min_vals.back()<<" "<<in_max_vals.back()<<" "<<n_steps.back()<<std::endl;
    }

    //So if min_max val vectors are negative, we calculate it ourselves.
    double sum_of_elems = std::accumulate(in_max_vals.begin(), in_max_vals.end(), 0.0);
    std::vector<double>maxvals(bdt_infos.size(),-999);
    std::vector<double> minvals = maxvals;
    std::cout<<sum_of_elems<<std::endl;
    if(sum_of_elems >0){
        std::cout<<"Taking scanning range from xml "<<std::endl;
        maxvals = in_max_vals;
        minvals = in_min_vals;
        //n_steps = std::vector<double>(bdt_infos.size(),30.0);
    }else{
        std::cout<<"Automatically calculting scanning range"<<std::endl;
        for(size_t i = 0; i < sig_files.size(); ++i) {
            for(size_t k=0; k< bdt_infos.size(); k++){
                double tmax_1 = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(bdt_infos[k]).name.c_str()    );
                double tmax_2 = bkg_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(bdt_infos[k]).name.c_str()    );
                maxvals[k] = std::max(maxvals[k], std::max(tmax_1,tmax_2));
            }
        }

        //for(auto &v: minvals) v=v*0.2;
        for(auto &v: minvals) v=0.0;

    }

    //Create N2tempoary TEntryLists  at minimum
    std::vector<TEntryList*> sig_min_lists;
    std::vector<TEntryList*> bkg_min_lists;



    std::cout<<"Setting Min entry lists"<<std::endl;
    for(size_t i = 0; i < sig_files.size(); ++i) {

        std::string min_list_name  = "micam"+std::to_string(i);
        sig_files.at(i)->tvertex->Draw((">>"+min_list_name).c_str(), sig_files.at(i)->getStageCuts(1+bdt_infos.size(), minvals ).c_str() , "entrylist");
        sig_min_lists.push_back(  (TEntryList*)gDirectory->Get(min_list_name.c_str()) );
        sig_files.at(i)->tvertex->SetEntryList(sig_min_lists.back());

    }
    for(size_t i = 0; i < bkg_files.size(); ++i) {

        std::string min_list_name  = "mibam"+std::to_string(i);
        bkg_files.at(i)->tvertex->Draw((">>"+min_list_name).c_str(), bkg_files.at(i)->getStageCuts(1+bdt_infos.size(), minvals).c_str() , "entrylist");
        bkg_min_lists.push_back(  (TEntryList*)gDirectory->Get(min_list_name.c_str()) );
        bkg_files.at(i)->tvertex->SetEntryList(bkg_min_lists.back());
    }	



    std::vector<double> steps(bdt_infos.size(),0.0);
    for(int i=0; i< bdt_infos.size(); i++){
        steps[i] = (maxvals[i]-minvals[i])/((double)n_steps[i]);
    }

    std::cout<<"We are going to scan between these values "<<std::endl;
    for(int i=0; i< bdt_infos.size();i++){
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<" Steps "<<steps[i]<<" Nsteps "<<n_steps[i]<<std::endl;
    }
    // Calculate total signal for efficiency 
    double total_sig = 0.;

    for(size_t i = 0; i < sig_files.size(); ++i) {
        double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
        std::cout << "POT scale: " << pot_scale << std::endl;

        std::string bnbcut = sig_files.at(i)->getStageCuts(1,minvals); 
        total_sig += sig_files.at(i)->tvertex->GetEntries(bnbcut.c_str())*pot_scale;
    }

    std::string s_mod = "";
    TRandom3 *rangen  = new TRandom3(0);  
    std::cout<<"Starting"<<std::endl;
    
    TH2D hsig("hsig","hsig",n_steps[which_bdt],minvals[which_bdt],maxvals[which_bdt],n_steps[which_bdt2],minvals[which_bdt2],maxvals[which_bdt2]);
   
    int icc = 1;
    for(double cc=minvals[which_bdt]; cc< maxvals[which_bdt]; cc+=steps[which_bdt]){
    int ibb = 1;
    for(double bb=minvals[which_bdt2]; bb< maxvals[which_bdt2]; bb+=steps[which_bdt2]){

        std::vector<double> d = bdt_cuts;
        d[which_bdt]=cc;
        d[which_bdt2]=bb;

        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t i = 0; i < sig_files.size(); ++i) {
            double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;

            std::string bnbcut = sig_files.at(i)->getStageCuts(1+bdt_infos.size(), d); 
            double thiss = sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;
            signal += thiss; 

            std::cout<<" "<<sig_files[i]->tag<<" "<<thiss;
        }

        for(size_t i = 0; i < bkg_files.size(); ++i) {
            double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;


            std::string bnbcut = bkg_files.at(i)->getStageCuts(1+bdt_infos.size(),d); 
            //     std::cout<<bnbcut<<std::endl;
            bkg.push_back(bkg_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale);			

            background += bkg.back();
            std::cout<<" "<<bkg_files[i]->tag<<" "<<bkg.back();
        }
        std::cout<<std::endl;

        double significance =0;
        if(signal==0){
            significance =0;
        }else if(background !=0){
            //significance = signal/(signal+background);
            significance = signal/sqrt(background);

        }else{
            std::cout<<"method_best_significane_seperate || signal2+background2 == 0 , sig: "<<signal<<" , so significance  = nan. Woopsie."<<std::endl;
            //break;
        }

        if(significance > best_significance) {
            best_significance = significance;
            best_mva = d;
            s_mod = "(Current Best)";
        }

        for(auto &dd:d){
            std::cout<<dd<<",";   
        }
        std::cout<<") N_signal: "<<signal<<" (E_signal: "<<(signal/(double)total_sig)<<") N_bkg: "<<background<<" ||  Sigma: " <<significance<<" "<<s_mod<<std::endl;

        s_mod = "";
        
        hsig.SetBinContent(icc,ibb,significance);
        std::cout<<"Plotted "<<icc<<" "<<ibb<<" "<<significance<<std::endl; 
        ibb++;


        }
        icc++;
    }

    TCanvas *c = new TCanvas();
    c->cd();
    hsig.SetStats(0);
    hsig.Draw("colz");

    hsig.SetTitle("sig/#sqrt{bkg} Surface");
    hsig.GetXaxis()->SetTitle(bdt_infos[which_bdt].name.c_str());
    hsig.GetYaxis()->SetTitle(bdt_infos[which_bdt2].name.c_str());
    
            TGraph *g_min = new TGraph(1);
            g_min->SetPoint(0,bdt_cuts[which_bdt], bdt_cuts[which_bdt2]);
            g_min->SetLineColor(kCyan);
            g_min->SetMarkerStyle(29);
            g_min->SetMarkerSize(3);
            g_min->Draw("p");


	c->SaveAs(("LinLin_Scan_"+std::to_string(which_bdt)+"_"+std::to_string(which_bdt2)+".pdf").c_str(),"pdf");




    std::cout<<"------------_FINAL Best Sig: "<<best_significance<<std::endl;

    std::cout<<"  --ccut: ";
    for(auto &dd: best_mva){
        std::cout<<dd<<" ";   
    }


    return std::vector<double>{0,0,0};

}

std::string significance_name(int sig_type){
    if(sig_type == 0)
	return "Sigma (S/sqrt(B))";
    if(sig_type == 3)
	return "Efficiency times Purity";
    if(sig_type == 4)
	return "Efficiency";
    if(sig_type == 5)
	return "Purity";
    return "";
}

double calculate_significance(double signal, double background, int sig_type, double total_signal, double signal_scale){

    double significance =0;
    double purity, efficiency;
    if(signal==0){
        significance =0;
    }else if(background !=0){

	switch(sig_type){
	    case 0:	//stats S/B ratio
		significance = signal_scale*signal/sqrt(background);
		break;
	    case 3:     //efficiency times purity
	    {
		if(total_signal == 0){
		    std::cerr << "\nbdt_sig::calculate_significance\t||\t Orignal signal is 0, can't calculate efficiency" << std::endl;
		    throw std::runtime_error("Not set up original signal prediction");
		}
		efficiency = signal/total_signal;	
		purity = signal_scale*signal/(signal_scale*signal+background);
		significance = efficiency * purity;
		break;
	    }
	    case 4:     //efficiency 
	    {
		if(total_signal == 0){
                    std::cerr << "\nbdt_sig::calculate_significance\t||\t Orignal signal is 0, can't calculate efficiency" << std::endl;
                    throw std::runtime_error("Not set up original signal prediction");
                }
		efficiency = signal/total_signal;	
		significance = efficiency;
		break;
	    }
	    case 5:     //purity
		purity = signal_scale*signal/(signal_scale*signal+background);
                significance = purity;
		break;
	    default:
		std::cerr << "bdt_sig::calculate_significance\t||\t Don't recognize the significance type.. " << std::endl;
		break;
	}

    }else{
	std::cerr << "bdt_sig::calculate_significance\t||\t Signal is " << signal << " but backgrounds is 0... Set significance to 0..." << std::endl;
    }
   
    return significance;
}

std::vector<std::vector<double>> setup_bdt_cut_grid(const std::vector<bdt_info>& bdt_infos, std::vector<std::vector<double>>& bdt_scan_pts, int& total_pts, bdt_stack* MCstack){
 
    std::vector<bdt_file*> sig_files, bkg_files; 
    if(MCstack){
	for(auto f : MCstack->getFiles()){
	    if(f->IsSignal())
		sig_files.push_back(f);
	    else
		bkg_files.push_back(f);
	}
    }

    return setup_bdt_cut_grid(bdt_infos, bdt_scan_pts, total_pts, sig_files, bkg_files);
}


std::vector<std::vector<double>> setup_bdt_cut_grid(const std::vector<bdt_info>& bdt_infos, std::vector<std::vector<double>>& bdt_scan_pts, int& total_pts, std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files){
    std::vector<double> maxvals(bdt_infos.size(),-999);
    std::vector<double> minvals(bdt_infos.size(),999);
    std::vector<double> n_steps(bdt_infos.size(), -999);
    std::vector<double> steps(bdt_infos.size(),0.0);
    total_pts = 1;
    bdt_scan_pts.clear();

    // grab range and number of stepss for bdt score
    for(size_t b=0; b<bdt_infos.size();b++){
        minvals[b] = bdt_infos[b].TMVAmethod.scan_min;
        maxvals[b] = bdt_infos[b].TMVAmethod.scan_max;
        n_steps[b] = (int)bdt_infos[b].TMVAmethod.scan_steps;
   
	if(maxvals[b] == -99){
	    std::cout<<"Scan range not setup in xml. Automatically calculting scanning range"<<std::endl;
	    if(sig_files.empty() && bkg_files.empty()){
		std::cout << "WARNING: No bdt files nor max allowed BDT score provided, set maxval = 1.0" << std::endl;
		maxvals[b] = 1.0;
	    }

            for(size_t i = 0; i < sig_files.size(); ++i) {
            	double tmax  = sig_files.at(i)->tvertex->GetMaximum( sig_files.at(i)->getBDTVariable(bdt_infos[b]).name.c_str()    );
                maxvals[b] = std::max(maxvals[b], tmax);
            }
            for(size_t i = 0; i < bkg_files.size(); ++i) {
            	double tmax  = bkg_files.at(i)->tvertex->GetMaximum( bkg_files.at(i)->getBDTVariable(bdt_infos[b]).name.c_str()    );
                maxvals[b] = std::max(maxvals[b], tmax);
            }
        

            minvals[b] = 0.0;
	    n_steps[b] = 10;
	}


    }

    // calculate cut positions for all BDTs
    for(int i=0; i< bdt_infos.size(); i++){
	total_pts *= n_steps[i];
        steps[i] = (maxvals[i]-minvals[i])/(n_steps[i]);

        std::vector<double> tmp;
        for(int ip=0; ip<n_steps[i]; ip++){
            tmp.push_back(minvals[i]+(double)ip*steps[i]);
        }
        bdt_scan_pts.push_back(tmp);
    }

    return {maxvals, minvals, n_steps, steps};
}

std::vector<double> grab_point(int pt, const std::vector<std::vector<double>>& all_bdt_pts){
  //Input: idnex of grid point 
  //Input: 2D vector containing all points, each row represents all possible values for one axis. (one row <--> one BDT)
  //Output: Given global index, find coordinate values at given index 


	std::vector<double> current_point;
	auto indices = get_local_indices(pt, all_bdt_pts);
	for(int i = 0; i!= indices.size(); ++i)
	    current_point.push_back(all_bdt_pts[i][indices[i]]);

	return current_point;
}

std::vector<int> get_local_indices(int global_index, const std::vector<std::vector<double>>& all_bdt_pts){
  //Input: idnex of grid point 
  //Input: 2D vector containing all points, each row represents all possible values for one axis. (one row <--> one BDT)
  //Output: Given global index, find local position of it in every sub-vector

        std::vector<int> index; 

        //were going to take each number and write each digit in base K where K is that vectors length
        int f_num_dimensions = all_bdt_pts.size();
        for(int j =f_num_dimensions-1 ;j>=0; j--){
            int this_index =  global_index % all_bdt_pts[j].size();

            index.push_back(this_index);

            //in order so that each digit is written in the correct base, modify divisor here
            global_index /= all_bdt_pts[j].size();
        }

	std::reverse(index.begin(), index.end());
	return index;
     
}


void make_project_plots(const std::vector<std::vector<double>>& bdt_scan_pts, const std::vector<bdt_info>& bdt_infos, const std::vector<double>& result_vec, std::string metric_name, std::string pdfname){
     int num_bdts = bdt_infos.size();
     int total_pts = 1; 


     // grab the grid configuration 
     std::vector<double> max_val, min_val, n_step;
     for(int i = 0; i != num_bdts; ++i){
	const std::vector<double>& bdt_pt = bdt_scan_pts[i];
  	total_pts *= bdt_pt.size();
	min_val.push_back(bdt_pt[0]);
	n_step.push_back(bdt_pt.size());

	if(n_step.back() == 1){
	    max_val.push_back(std::min(1., 2*min_val.back()));
	}else{
	    max_val.push_back(bdt_pt.back() + bdt_pt[1] - bdt_pt[0]);
	}
     }

     //initialze list of histograms 
     //1D histos
     std::vector<TH1D*> hist_vec_1D; 
     std::vector<TH2D*> hist_vec_2D;
     std::vector<std::pair<int, int>> Comb_2D;

     //flag to keep track of whether givn bin is filled. may not be needed.. checking.
     std::vector<std::vector<int>> filled_1D;
     std::vector<std::vector<std::vector<int>>> filled_2D;

     for(int i = 0; i != num_bdts; ++i){
	std::string bdt_name_i = bdt_infos[i].getName();

	hist_vec_1D.push_back(new TH1D( bdt_name_i.c_str(), bdt_name_i.c_str(), n_step[i], min_val[i], max_val[i] ) );
	hist_vec_1D.back()->GetXaxis()->SetTitle((bdt_name_i + " Score").c_str());
	hist_vec_1D.back()->GetYaxis()->SetTitle(metric_name.c_str());
	filled_1D.push_back(std::vector<int>(n_step[i]+1, 0));

	for(int j = i+1; j != num_bdts; ++j){
	    std::string bdt_name_j = bdt_infos[j].getName();
	    std::string hist_name = bdt_name_i + "_" +  bdt_name_j;
	    Comb_2D.push_back({i,j });

	    hist_vec_2D.push_back(new TH2D(hist_name.c_str(), hist_name.c_str(), n_step[i], min_val[i], max_val[i], n_step[j], min_val[j], max_val[j]));
	    hist_vec_2D.back()->SetTitle(( metric_name + ";" + bdt_name_i + " Score ;" + bdt_name_j + " Score").c_str());

	    filled_2D.push_back(std::vector<std::vector<int>>(n_step[i]+1, std::vector<int>(n_step[j]+1, 0)));
	}
     }

     //fill the histograms 
    for(int i = 0 ; i != total_pts; ++i){
	auto indices = get_local_indices(i, bdt_scan_pts);


	//update 1D histogram 
	for(int j = 0; j != num_bdts; ++j){

	    int bin = indices[j]+1;

	    //check if bin has been filled
	    double bin_content;
	    if(filled_1D[j][bin]){
		bin_content = std::min(hist_vec_1D[j]->GetBinContent(bin), result_vec[i]);
	    }
	    else{
		filled_1D[j][bin] = 1;
		bin_content = result_vec[i];
	    }

	    hist_vec_1D[j]->SetBinContent( bin, bin_content);
	}

	// update 2D histos 
	for(int j = 0; j != Comb_2D.size(); ++j){
	    int bin_x = indices[Comb_2D[j].first] + 1, bin_y = indices[Comb_2D[j].second] + 1;

	    double bin_content;
	    if(filled_2D[j][bin_x][bin_y]){
		bin_content = std::min(hist_vec_2D[j]->GetBinContent(bin_x, bin_y), result_vec[i]);
	    }else{
		filled_2D[j][bin_x][bin_y] = 1;
		bin_content = result_vec[i];
	    }

	    hist_vec_2D[j]->SetBinContent(bin_x, bin_y, bin_content);
	}
    }

    //draw the histogram 
    TCanvas* c = new TCanvas("c", "c");
    c->Print( (pdfname + "[").c_str());
    gStyle->SetOptStat(0);
    c->cd();
    for(auto h : hist_vec_1D){
	c->Clear();
	h->Draw("hist");	
	c->Print(pdfname.c_str());
    }
    for(auto h : hist_vec_2D){
	c->Clear();
        h->Draw("colz");        
        c->Print(pdfname.c_str());
    }

    c->Print( (pdfname + "]").c_str());
          

    //delete pointers
    for(auto h : hist_vec_1D)
	delete h;
    for(auto h : hist_vec_2D)
	delete h;

    delete c;
    return;
}

double calculate_chi(sbn::SBNchi* chi_handle, TMatrixT<double>* frac_matrix, std::vector<double>& fulldata, std::vector<double>& full_spec, std::vector<double>& full_spec_error, double scale){

	// scale spectrum, if necessary 
	if(scale != 1){
	    std::transform(fulldata.begin(), fulldata.end(), fulldata.begin(), [&scale](double d){return scale*d;});
	    std::transform(full_spec.begin(), full_spec.end(), full_spec.begin(), [&scale](double d){return scale*d;});
	    std::transform(full_spec_error.begin(), full_spec_error.end(), full_spec_error.begin(), [&scale](double d){return scale*d;});
	}

	// collapse full vector into collapsed ones 
	std::vector<double> collapsed_data(chi_handle->num_bins_total_compressed, 0), collapsed_spec(chi_handle->num_bins_total_compressed, 0);
	chi_handle->CollapseVectorStandAlone(&fulldata, &collapsed_data);
	chi_handle->CollapseVectorStandAlone(&full_spec, &collapsed_spec);


	//calculate chi at background prediction
	//CNP chi-square
	TMatrixT<double> collapse_covar_matrix = chi_handle->CalcCovarianceMatrixCNP(*frac_matrix, full_spec, full_spec_error, collapsed_spec, collapsed_data);

	//Pearson chi-square
	if(false){
		auto full_covar_matrix = chi_handle->CalcCovarianceMatrix(frac_matrix, full_spec, full_spec_error);
		TMatrixT<double> collapse_covar_matrix(chi_handle->num_bins_total_compressed, chi_handle->num_bins_total_compressed);
		chi_handle->CollapseModes(full_covar_matrix, collapse_covar_matrix);
	}

	TMatrixT<double> inverted_matrix = chi_handle->InvertMatrix(collapse_covar_matrix);

	// print matrix to log
	if(false){
	    std::cout << "Collapsed covariance matrix: " << std::endl;
	    for(int i = 0; i != collapse_covar_matrix.GetNrows(); ++i){
		for(int j = 0;j != collapse_covar_matrix.GetNcols(); ++j)
		    std::cout << collapse_covar_matrix(i,j) << " ";
		std::cout << std::endl;
	    }
	    std::cout << "Inverted covariance matrix: " << std::endl;
	    for(int i = 0; i != inverted_matrix.GetNrows(); ++i){
		for(int j = 0;j != inverted_matrix.GetNcols(); ++j)
		    std::cout << inverted_matrix(i,j) << " ";
		std::cout << std::endl;
	    }
	}
	double current_chi = chi_handle->CalcChi(inverted_matrix, collapsed_spec, collapsed_data);

	return current_chi;
}
	
double round_to(double value, double precision)
{
    return std::round(value / precision) * precision;
}

