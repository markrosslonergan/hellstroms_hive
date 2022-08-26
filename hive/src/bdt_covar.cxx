#include "bdt_covar.h"

//---- Definition of PUBLIC function -----

void bdt_covar::GenerateReweightingCovar(){
    if(!set_template) 
	set_template_path();

    GenerateReweightingCovar(this->fluxxs_template_xml);
    return;
}

void bdt_covar::GenerateReweightingCovar(const std::string& xml ){


    if(pvar->flux_xs_sys_only() || pvar->full_sys()){

    	std::string file_tag = pvar->GetCovarFileID(stage);
	if(pvar->full_sys()) 
	    file_tag += "_FluxXS";

        std::cout << "bdt_covar: start to generate reweighable systematic covariance matrix...." << std::endl;
        std::cout << "bdt_covar: WARNING covariance matrix will be generated locally" << std::endl;
	auto covar_xml = prepare_xml(xml, file_tag);
	generate_covar(covar_xml, file_tag);

	//if var only include flux xs error, update file name
	if(pvar->flux_xs_sys_only()){
	    pvar->UpdateCovarFileName(file_tag + ".SBNcovar.root", file_dir);
	    pvar->UpdateCovarName("frac_covariance");
            pvar->UpdateCovarType("frac");
	}
    }
    return;
}

void bdt_covar::GenerateDetectorCovar(){
    if(!set_template) 
	set_template_path();

    GenerateDetectorCovar(this->det_template_xml);
    return;
}
 
void bdt_covar::GenerateDetectorCovar(const std::string& xml){ 

    if(pvar->detector_sys_only() || pvar->full_sys()){

        std::string file_tag = pvar->GetCovarFileID(stage);
	if(pvar->full_sys()) file_tag += "_Det";

        std::cout << "bdt_covar: start to generate detector covariance matrix...." << std::endl;
        std::cout << "bdt_covar: WARNING covariance matrix will be generated locally" << std::endl;
	auto covar_base_xml = prepare_xml(xml, file_tag);
 
	//set stages
	//Guanqun: not sure what this does, so comment it out for now 
	/*
 	std::cout<<"Now lets add the Stages: "<<which_stage<<std::endl;
	std::string s_stagea ="stage_"+std::to_string((int)which_stage);
 	std::string s_stageb ="stage_"+std::to_string((int)which_stage); ;
 	std::string sedder_STAGEA = "sed  -i 's@STAGESTAGESTAGE@" + s_stagea + "@' " + covar_base_xml;
 	std::string sedder_STAGEB = "sed  -i 's@STAGEZOOMZOOM@" + s_stageb + "@' " + covar_base_xml;
	system(sedder_STAGEA.c_str());
        system(sedder_STAGEB.c_str());
	*/

	std::string covar_xml_name = covar_base_xml.substr(0, covar_base_xml.find_last_of('.'));

	std::vector<std::string> files_to_merge;
	for(auto &syst : detsys){

	    std::string covar_xml = covar_xml_name + "_" + syst + ".xml"; 
	    std::cout<<"On Det "<< syst << ", xml used: " << covar_xml << std::endl;

    	    //somehow redirecting sed output to a new file doesn't always work 
    	    //explicitly create new xml instead
    	    copy_and_replace(covar_base_xml, covar_xml, {"SYSVAR"}, {syst}, "xml");
	    /*
	    copy_file(covar_base_xml, covar_xml);
	    std::string sedder_DET = "sed  -i 's@SYSVAR@" + syst + "@' " +  covar_xml;
	    system(sedder_DET.c_str());
	   */
	    generate_covar(covar_xml ,file_tag + "_" + syst, true);
	    files_to_merge.push_back(file_dir +  file_tag + "_" + syst + ".SBNcovar.root");
	}

	merge_covar(files_to_merge, file_dir + file_tag + ".SBNcovar.root");

	// if var only has detector error, update covariance file name
	if(pvar->detector_sys_only()){
	    pvar->UpdateCovarFileName(file_tag + ".SBNcovar.root", file_dir);
	    pvar->UpdateCovarName("frac_covariance");
	    pvar->UpdateCovarType("frac");
	}
    }
    return;
}


void bdt_covar::MergeCovar(){

    if(pvar->full_sys()){
	std::string file_tag = pvar->GetCovarFileID(stage);
	std::vector<std::string> files_to_merge = {file_dir + file_tag + "_FluxXS.SBNcovar.root", file_dir + file_tag + "_Det.SBNcovar.root"};
	merge_covar(files_to_merge, file_dir + file_tag + ".SBNcovar.root");
	pvar->UpdateCovarFileName(file_tag + ".SBNcovar.root", file_dir);
	pvar->UpdateCovarName("frac_covariance");
	pvar->UpdateCovarType("frac");
    }
    return;
}

//---- END of PUBLIC function -----

void bdt_covar::merge_covar(const std::vector<std::string>& files_to_merge, const std::string& target_file){

    std::cout << "bdt_covar: Start to merge fractional covariance matrix... " << std::endl;
    std::cout << "bdt_covar: Merged covariance matrix will be saved into .. " << target_file << std::endl;

    TFile* ftarget = new TFile(target_file.c_str(), "recreate");
    TMatrixT<double> covar_matrix; 

    for(int i =0; i != files_to_merge.size(); ++i){
	std::cout << "On file: " << files_to_merge[i] << std::endl;

	TFile* fin = new TFile(files_to_merge[i].c_str(), "read");
	TMatrixT<double>* matrix_in = (TMatrixT<double>*)fin->Get("frac_covariance");
	remove_nan(matrix_in);
	if(i== 0)
	    covar_matrix = *matrix_in; 
	else 
	    covar_matrix += *matrix_in;

	fin->Close();
    }

    ftarget->cd();
    covar_matrix.Write("frac_covariance");
    ftarget->Close();

    return;
}


std::string bdt_covar::prepare_xml(const std::string& xml, const std::string& file_tag){
    std::cout << "bdt::covar|| Template xml provided: " << xml << std::endl;

    //configure name of local, copied xml 
    auto folder_pos = xml.find_last_of("/\\"), dot_pos = xml.find_last_of('.');
    std::string output_xml;
    if(folder_pos == std::string::npos)
 	output_xml = xml.substr(0, dot_pos);
    else
	output_xml = xml.substr(folder_pos+1, dot_pos - folder_pos - 1);
    output_xml = "xml/" + output_xml + "_" + file_tag + ".xml"; 
    std::cout << "bdt::covar|| Will copy into xml: " << output_xml << std::endl;


    std::vector<std::string> targets = {"VARVARVAR", "BINBINBIN", "WEIWEIWEI"};
    std::vector<std::string> replacements = {"\""+ pvar->GetVarDef() + "\"", "\""+ pvar->GetBinEdges() + "\"", "(" + base_cut + " && " + pvar->GetAdditionalCut() + ")"};

    copy_and_replace(xml, output_xml, targets, replacements, "xml");   

    return output_xml; 
}

void bdt_covar::generate_covar(const std::string& xml, const std::string& tag, bool run_det){
    

    std::cout << "bdt_covar: Generate covariance matrix under name " << tag << ".SBNcovar.root" << std::endl; 
    if(run_det){
	sbn::SBNcovariance covar(xml, false);
	covar.FormCovarianceMatrix(tag);
    }else{
	sbn::SBNcovariance covar(xml);
	covar.FormCovarianceMatrix(tag);
    }

    //move covariance matrix to designated place
    if(file_dir.size()){
    	std::cout << "bdt_covar: move covariance matrix to " << file_dir << " subdir..." << std::endl; 
  	if(system(NULL))
	    system(("mkdir -p " + file_dir).c_str());
	else
	    mkdir(file_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
   
	std::vector<std::string> files_to_move = {tag+".SBNcovar.root", tag+".SBNspec.root", "SBNfit_spectrum_plots_"+tag+".root"};
	for(auto& f : files_to_move){
	    if( rename(f.c_str(), (file_dir +f).c_str() ) ){
	    	throw std::runtime_error("Fail to move covariance matrix file " + f + " to " + file_dir + " directory");
	    }
	}
    }

    return; 
}

void bdt_covar::set_template_path(){
    std::cout << "bdt_covar || Attempt to set up template xmls for covariance matrix generation" << std::endl;
    std::cout << "bdt_covar || WARNING: this function looks for HIVEDIR enviromental variable" << std::endl;
    char* path = getenv("HIVEDIR");
    if(path==nullptr){
	std::cout << "bdt_covar || Fail to find HIVEDIR enviromental variable.. " << std::endl;
	std::cout << "bdt_covar || Paths of template xmls not set... " << std::endl;
    }else{
        std::cout << "bdt_covar || Successfully find HIVEDIR environmental variable = " << path << std::endl;
	std::string full_path(path);
	fluxxs_template_xml = full_path + "/xml/SBNfit_Integration_XMLS_Coherent/template_ReweightableSys_1g0p_coherent_stage_0.xml";
  	det_template_xml = ""; //remain to be set
	std::cout << "bdt_covar || template xmls:" << std::endl;
	std::cout << "bdt_covar || reweightable systematic xml: " << fluxxs_template_xml << std::endl;
	std::cout << "bdt_covar || detectory systematic xml: " << det_template_xml << std::endl;
    }

    set_template = true;
    return;
}

void bdt_covar::copy_and_replace(const std::string& input_xml, const std::string& output_xml, const std::vector<std::string>& search, const std::vector<std::string>& replace, const std::string& fdir){

    //check if system command exists 
    if(system(NULL)){
	std::cout << "bdt_covar::copy_and_replace || system() command exists, use system() from now on" << std::endl;

    	std::string create_dir = "mkdir -p " + fdir;
	std::cout << create_dir << std::endl;
    	system(create_dir.c_str());

	for(int i = 0 ; i != search.size(); ++i){
	    std::string sedder;
    	  
	    //replace && by &amp;&amp; to match xml style
    	    std::string replace_word = ReplaceString( replace[i],"&&","\\&amp;\\&amp;");

	    if(i == 0) sedder = "sed  's@" + search[i] + "@" + replace_word + "@g' "+ input_xml +" > "+ output_xml;
	    else sedder = "sed  -i 's@" + search[i] + "@" + replace_word + "@g' " + output_xml;
    	    std::cout<<sedder<<std::endl;
     	    system(sedder.c_str());
	}

    }else{
        // system() command doesn't always work  on different systems
        //explicitly create new xml instead

	std::cout << "bdt::copy_and_replace || system() command do not exist, use general tool.." << std::endl;

	int status = mkdir(fdir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(status == -1 && errno != EEXIST){
	    std::cout << "cannot create folder " << fdir << " error: " << strerror(errno) << std::endl;
	    throw std::runtime_error(strerror(errno));
	}

	std::vector<std::string> updated_replace;
	for(auto& replace_word : replace){
	    updated_replace.push_back(ReplaceString( replace_word,"&&","&amp;&amp;"));
	}

	copy_file(input_xml, output_xml, search, updated_replace);
    }
    
    return;
}
