#ifndef GLOBAL_FUNC_H
#define GLOBAL_FUNC_H

#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <stdexcept>

/**** ROOT include ***/
#include "TMatrixT.h"

/***** Internal include ***/
#include "duplicate_suite_2022.h"


// define systematic ucnertainty symbols 
#define STATS "stats"
#define FLUXXS "fluxxs"
#define DET "det" 
#define FULL "fluxxsdet"



/* Replace 'search' by 'replace' in input string 'subject'and return the new string */
std::string ReplaceString(std::string subject, const std::string& search, const std::string& replace); 

/* Given a string, calculate its unique Jenkins hash value */
unsigned long  jenkins_hash(const std::string& key);
 
/* Replace the nan value in input matrix by 0 (Note: in-place operation)  */
void remove_nan(TMatrixT<double>* tin);

/* Copy given input file into a new file with new name */
void copy_file(const std::string& input_name, const std::string& output_name);


/* Copy given input file into a new file with new name, in the same time replace occurrence of given words by other contents */
void copy_file(const std::string& input_name, const std::string& output_name, const std::vector<std::string>& search, const std::vector<std::string>& replace );

/* Load run.subrun list of the training samples and return */
runlist LoadTrainList();
#endif
