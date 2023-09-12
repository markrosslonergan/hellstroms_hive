#include "global_func.h"

/* Replace 'search' by 'replace' in input string 'subject'and return the new string */
std::string ReplaceString(std::string subject, const std::string& search,
        const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return subject;
}


/* Given a string, calculate its unique Jenkins hash value */
unsigned long  jenkins_hash(const std::string& key) {
    size_t length = key.size();
    size_t i = 0;
    unsigned long hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

/* Replace the nan value in input matrix by 0 (Note: in-place operation)  */
void remove_nan(TMatrixT<double>* tin){
    for(int i = 0 ; i != tin->GetNrows(); ++i){
	for(int j = 0; j != tin->GetNcols(); ++j){
	    if(std::isnan((*tin)(i,j)))
		(*tin)(i,j) = 0.0;
	}
    }
    return;
}

/* Copy given input file into a new file with new name */
void copy_file(const std::string& input_name, const std::string& output_name){
    std::ifstream infile(input_name);
    std::ofstream outfile(output_name, std::ofstream::trunc);
    std::string line;
    while(std::getline(infile, line)){
	outfile << line << std::endl;
    }
    outfile.close();
    infile.close();
    return;
}

/* Copy given input file into a new file with new name, in the same time replace occurrence of given words by other contents */
void copy_file(const std::string& input_name, const std::string& output_name, const std::vector<std::string>& search, const std::vector<std::string>& replace ){
    if(search.size() != replace.size()){
	std::cerr << "Global_Func\t\t||\t Number of target strings and replacements don't match" << std::endl;
	std::cerr << "Global_Func\t\t||\t target strings: " << search.size() << " replacements: " << replace.size() << std::endl;
	throw std::runtime_error("size not match");
    }

    std::ifstream infile(input_name);
    std::ofstream outfile(output_name, std::ofstream::trunc);
    std::string line;
    while(std::getline(infile, line)){

	for(int i = 0; i != search.size(); ++i){
	    line = ReplaceString(line, search[i], replace[i]); 
	}
	
	outfile << line << std::endl;
    }
    outfile.close();
    infile.close();
    return;
}

/* Load run.subrun list of the training samples */
runlist LoadTrainList(){
    std::vector<RSE> rses;
    std::cout<<"Adding a training flag to simple_tree, from /uboone/app/users/markrl/useful_scripts/duplicate_suite_2022/Detsys_2022/buildingMasterTrainingRSE/MasterTraining.RSE.sort.uniq"<<std::endl;
	
    std::ifstream infile;
    infile.open("/uboone/app/users/markrl/useful_scripts/duplicate_suite_2022/Detsys_2022/buildingMasterTrainingRSE/MasterTraining.RSE.sort.uniq");

    int run, subrun,event;
    while(infile >> run >> subrun >> event){
	    rses.emplace_back(run,subrun,event);
    }
    infile.close();
    std::cout<<"Loaded RSE master list: "<< std::endl;

    return runlist(rses);
}
