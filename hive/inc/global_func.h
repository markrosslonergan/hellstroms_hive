#ifndef GLOBAL_FUNC_H
#define GLOBAL_FUNC_H

#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <stdexcept>

/**** ROOT include ***/
#include "TMatrixT.h"


/* Replace 'search' by 'replace' in input string 'subject'and return the new string */
inline
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
inline
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
inline
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
inline
void copy_file(const std::string& input_name, const std::string& output_name){
    std::ifstream infile(input_name);
    std::ofstream outfile(output_name, std::ofstream::trunc);
    std::string line;
    while(std::getline(infile, line)){
	outfile << line << std::endl;
    }
    outfile.close();
    return;
}

/* Copy given input file into a new file with new name, in the same time replace occurrence of given words by other contents */
inline
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
    return;
}

#endif
