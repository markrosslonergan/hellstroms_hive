#ifndef LOADMVAPARAM_H
#define LOADMVAPARAM_H

#include "tinyxml.h"

#include "method_struct.h"


#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "TMVA/Types.h"



class MVALoader {

	protected:

	std::vector<method_struct> vec_methods;
	public:
	
	bool isVerbose;
	std::string whichxml;	

	
	MVALoader(std::string,bool);
	MVALoader(std::string);

	std::vector<method_struct> GetMethods();

};


#endif
