#include "bdt_sig.h"



std::vector<double> scan_significance(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, double plot_pot){
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

    std::vector<double> in_min_vals;
    std::vector<double> in_max_vals;
    std::vector<int> n_steps;
    int max_pts=1;

    for(size_t b=0; b<bdt_infos.size();b++){
        in_min_vals.push_back(bdt_infos[b].TMVAmethod.scan_min);
        in_max_vals.push_back(bdt_infos[b].TMVAmethod.scan_max);
        n_steps.push_back((int)bdt_infos[b].TMVAmethod.scan_steps);
        max_pts = max_pts*n_steps.back();
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
        //n_steps = s;
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

    //Create N2tempoary TEntryLists  at minimum for all sig files
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

    std::vector<std::vector<double>> bdt_scan_pts;
    for(int i=0; i< bdt_infos.size(); i++){
        std::vector<double> tmp;
        for(int ip=0; ip<n_steps[i]; ip++){
            tmp.push_back(minvals[i]+(double)ip*steps[i]);
        }
        bdt_scan_pts.push_back(tmp);
    }


    std::cout<<"We are going to scan between these values "<<std::endl;
    for(int i=0; i< bdt_infos.size();i++){
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<" Steps "<<steps[i]<<" (n_steps:  "<<n_steps[i]<<")"<<std::endl;
    }

    //Calculate total signal for efficiency 
    double total_sig = 0.;

    for(size_t i = 0; i < sig_files.size(); ++i) {
        double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
        //    std::cout << "POT scale: " << pot_scale << std::endl;
        std::string bnbcut = sig_files.at(i)->getStageCuts(1,minvals); 
        total_sig += sig_files.at(i)->tvertex->GetEntries(bnbcut.c_str())*pot_scale;
    }

    std::cout<<"Starting"<<std::endl;
    std::cout<<"----------------------------------------------------"<<std::endl;
    std::string s_mod = "";
    int n_pt = 0;
    int best_pt = -9;


    for(int i=0; i < max_pts; i++){

        std::vector<double> cur_pt(bdt_infos.size(),0.0);

        //were going to take each number and write each digit in base K where K is that vectors length
        int divisor=1;
        int f_num_dimensions = bdt_infos.size();
        for(int j =f_num_dimensions-1 ;j>=0; j--){
            int this_index =  (i/divisor)%bdt_scan_pts[j].size();

            cur_pt[j] = bdt_scan_pts[j][this_index];
            //in order so that each digit is written in the correct base, modify divisor here
            divisor=divisor*bdt_scan_pts[j].size();
        }


        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t is = 0; is < sig_files.size(); ++is) {
            double pot_scale = (plot_pot/sig_files.at(is)->pot )*sig_files.at(is)->scale_data;
            std::string bnbcut = sig_files.at(is)->getStageCuts(1+bdt_infos.size(), cur_pt); 
            signal += sig_files.at(is)->GetEntries(bnbcut.c_str())*pot_scale;
        }

        for(size_t ib = 0; ib < bkg_files.size(); ++ib) {
            double pot_scale = (plot_pot/bkg_files.at(ib)->pot)*bkg_files.at(ib)->scale_data;
            std::string bnbcut = bkg_files.at(ib)->getStageCuts(1+bdt_infos.size(),cur_pt); 
            bkg.push_back(bkg_files.at(ib)->GetEntries(bnbcut.c_str())*pot_scale);			
            background += bkg.back();
        }

        double significance =0;
        if(signal==0){
            significance =0;
        }else if(background !=0){
            significance = signal/sqrt(background);
        }else{
            std::cout<<" Warning Backgrounds are identically 0 here, signal is "<<signal<<", so significance NAN. Woopsie. setting to Zero."<<std::endl;
        }

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
        std::cout<<") N_signal: "<<signal<<" N_bkg: "<<background<<" ||  Sigma: " <<significance<<" "<<s_mod<<std::endl;

        s_mod = "";
        n_pt++;




    }

    std::cout<<"----------------------------------------------------"<<std::endl;
    std::cout<<"------------ Finished. Best Significance was  "<<best_significance<<" at point "<<best_pt<<" with Cuts at "<<std::endl;
    for(auto &dd: best_mva){
        std::cout<<dd<<" ";   
    }
    std::cout<<std::endl;
    std::cout<<"Done with simple significance scan"<<std::endl;

    return std::vector<double>{0,0,0};

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

    std::vector<double> in_min_vals;
    std::vector<double> in_max_vals;
    std::vector<int> n_steps;
    int max_pts=1;

    for(size_t b=0; b<bdt_infos.size();b++){
        in_min_vals.push_back(bdt_infos[b].TMVAmethod.scan_min);
        in_max_vals.push_back(bdt_infos[b].TMVAmethod.scan_max);
        n_steps.push_back((int)bdt_infos[b].TMVAmethod.scan_steps);
        max_pts = max_pts*n_steps.back();
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
        //n_steps = s;
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

    //Create N2tempoary TEntryLists  at minimum for all sig files
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

    std::vector<std::vector<double>> bdt_scan_pts;
    for(int i=0; i< bdt_infos.size(); i++){
        std::vector<double> tmp;
        for(int ip=0; ip<n_steps[i]; ip++){
            tmp.push_back(minvals[i]+(double)ip*steps[i]);
        }
        bdt_scan_pts.push_back(tmp);
    }


    std::cout<<"We are going to scan between these values "<<std::endl;
    for(int i=0; i< bdt_infos.size();i++){
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<" Steps "<<steps[i]<<" (n_steps:  "<<n_steps[i]<<")"<<std::endl;
    }

    //Calculate total signal for efficiency 
    double total_sig = 0.;

    for(size_t i = 0; i < sig_files.size(); ++i) {
        double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;
        //    std::cout << "POT scale: " << pot_scale << std::endl;
        std::string bnbcut = sig_files.at(i)->getStageCuts(1,minvals); 
        total_sig += sig_files.at(i)->tvertex->GetEntries(bnbcut.c_str())*pot_scale;
    }

    std::cout<<"Starting"<<std::endl;
    std::cout<<"----------------------------------------------------"<<std::endl;
    std::string s_mod = "";
    int n_pt = 0;
    int best_pt = -9;


    for(int i=0; i < max_pts; i++){

        std::vector<double> cur_pt(bdt_infos.size(),0.0);

        //were going to take each number and write each digit in base K where K is that vectors length
        int divisor=1;
        int f_num_dimensions = bdt_infos.size();
        for(int j =f_num_dimensions-1 ;j>=0; j--){
            int this_index =  (i/divisor)%bdt_scan_pts[j].size();

            cur_pt[j] = bdt_scan_pts[j][this_index];
            //in order so that each digit is written in the correct base, modify divisor here
            divisor=divisor*bdt_scan_pts[j].size();
        }


        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t is = 0; is < sig_files.size(); ++is) {
            double pot_scale = (plot_pot/sig_files.at(is)->pot )*sig_files.at(is)->scale_data;
            std::string bnbcut = sig_files.at(is)->getStageCuts(1+bdt_infos.size(), cur_pt); 
            signal += sig_files.at(is)->GetEntries(bnbcut.c_str())*pot_scale;
        }

        for(size_t ib = 0; ib < bkg_files.size(); ++ib) {
            double pot_scale = (plot_pot/bkg_files.at(ib)->pot)*bkg_files.at(ib)->scale_data;
            std::string bnbcut = bkg_files.at(ib)->getStageCuts(1+bdt_infos.size(),cur_pt); 
            bkg.push_back(bkg_files.at(ib)->GetEntries(bnbcut.c_str())*pot_scale);			
            background += bkg.back();
        }

        double significance =0;
        double purity = 0;
        double efficiency = 0;
        if(signal==0){
            significance =0;
        }else if(background !=0 && sig_type!=3){
            significance = signal/sqrt(background);
        }else if(background !=0 && sig_type==3){
            significance = signal/(signal+background)*signal/total_sig*100;
            purity = signal/(signal+background);
            efficiency = signal/total_sig;
        }else{
            std::cout<<" Warning Backgrounds are identically 0 here, signal is "<<signal<<", so significance NAN. Woopsie. setting to Zero."<<std::endl;
        }

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
        if(sig_type==3){
            std::cout<<") N_signal: "<<signal<<" N_bkg: "<<background<<" ||  P*E: " <<significance<<" Eff: "<<efficiency<<" Purity: "<<purity<<"   @ "<<s_mod<<std::endl;
        }else{
            std::cout<<") N_signal: "<<signal<<" N_bkg: "<<background<<" ||  Sigma: " <<significance<<" "<<s_mod<<std::endl;
        }


        s_mod = "";
        n_pt++;




    }

    std::cout<<"----------------------------------------------------"<<std::endl;
    std::cout<<"------------ Finished. Best Significance was  "<<best_significance<<" at point "<<best_pt<<" with Cuts at "<<std::endl;
    for(auto &dd: best_mva){
        std::cout<<dd<<" ";   
    }
    std::cout<<std::endl;
    std::cout<<"Done with simple significance scan"<<std::endl;

    return std::vector<double>{0,0,0};

}




std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_info){
    return scan_significance_random(sig_files,bkg_files,bdt_info,0);
}

std::vector<double> scan_significance_random(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos,int sig_type){
    std::cout<<"Starting to Scan Significance (randomly)"<<std::endl;
    double best_significance = 0;
    double best_impact = 0;
    std::vector<double> best_mva(bdt_infos.size(), DBL_MAX);

    double plot_pot = 6.91e20;//5e19;// 10.115e20;

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
        n_steps = std::vector<double>(bdt_infos.size(),10.0);
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
        std::cout<<bdt_infos[i].identifier<<" Min: "<<minvals[i]<<" Max "<<maxvals[i]<<" Steps "<<steps[i]<<std::endl;
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
    for(int t=0; t < 100000; t++){

        std::vector<double> d (bdt_infos.size(),0);
        for(int i=0; i< bdt_infos.size(); i++){
            d[i] = rangen->Uniform(minvals[i], maxvals[i]);
        }
        double impact = rangen->Uniform(10,22);
        
        double s1 = rangen->Uniform(0.3,0.6);
        double s2 = rangen->Uniform(0.3,s1);

        //double impact = rangen->Uniform(0.5,0.7);
        //  std::string s_impact = "((sss_num_candidates==0) ||  Min$(sss_candidate_impact_parameter)>"+std::to_string(impact) +") ";

        //Yarp
        std::string s_impact = "1";
        //std::string s_impact = "((sss_num_candidates==0)|| Sum$(sss_candidate_impact_parameter<"+std::to_string(impact)+ "&& sss_candidate_min_dist<70.0)==0 )";

        //std::string s_impact = "((sss_num_candidates==0)|| Max$(sss_candidate_veto_score)<"+std::to_string(s1)+  " )";

        double signal = 0;
        double background = 0;
        std::vector<double> bkg;	

        for(size_t i = 0; i < sig_files.size(); ++i) {
            double pot_scale = (plot_pot/sig_files.at(i)->pot )*sig_files.at(i)->scale_data;

            std::string bnbcut = sig_files.at(i)->getStageCuts(1+bdt_infos.size(), d)+"&&"+s_impact; 
            double thiss = sig_files.at(i)->GetEntries(bnbcut.c_str())*pot_scale;
            signal += thiss; 

            std::cout<<" "<<sig_files[i]->tag<<" "<<thiss;
        }

        for(size_t i = 0; i < bkg_files.size(); ++i) {
            double pot_scale = (plot_pot/bkg_files.at(i)->pot)*bkg_files.at(i)->scale_data;


            std::string bnbcut = bkg_files.at(i)->getStageCuts(1+bdt_infos.size(),d)+"&&"+s_impact; 
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
            best_impact = impact;
            s_mod = "(Current Best)";
        }

        for(auto &dd:d){
            std::cout<<dd<<",";   
        }
        std::cout<<") w/ Impact: "<<impact<<" N_signal: "<<signal<<" (E_signal: "<<(signal/(double)total_sig)<<") N_bkg: "<<background<<" ||  Sigma: " <<significance<<" "<<s_mod<<std::endl;

        s_mod = "";

    
    }

    std::cout<<"------------_FINAL Best Sig: "<<best_significance<<std::endl;

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


std::vector<double> scan_significance_linlin(std::vector<bdt_file*> sig_files, std::vector<bdt_file*> bkg_files, std::vector<bdt_info> bdt_infos, std::vector<double> bdt_cuts, int which_bdt){
    std::cout<<"Starting to Scan Significance (randomly)"<<std::endl;
    double best_significance = 0;
    double best_impact = 0;
    std::vector<double> best_mva(bdt_infos.size(), DBL_MAX);

    double plot_pot = 5e19;//10.115e20;

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
        n_steps = std::vector<double>(bdt_infos.size(),30.0);
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
    
    int which_bdt2=4;
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
    c->SaveAs("Yes.pdf","pdf");


    std::cout<<"------------_FINAL Best Sig: "<<best_significance<<std::endl;

    std::cout<<"  --ccut: ";
    for(auto &dd: best_mva){
        std::cout<<dd<<" ";   
    }


    return std::vector<double>{0,0,0};

}


