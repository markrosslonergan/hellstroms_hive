#ifndef OBJECTHELPER_H
#define OBJECTHELPER_H


/******* Standard includes ******/
#include <iostream>
#include <map>

/******** Our includes *****/
#include "method_struct.h"
#include "void_vec.h"

/******** Root includes *****/


#include "TFile.h"
#include "TTree.h"
#include "TH1.h"


template <class T>
class object_helper {



	std::map<std::string, TFile *> open_files;



	public:



	//  ~object_helper()
	//template <class T>
	~object_helper() {
		for(auto const & p : open_files) {
			p.second->Close();
			delete p.second;
		}
	}



	T * GetObject(std::string const & file_path, std::string const & object_path) {

		auto const of_it = open_files.find(file_path);
		TFile * file = nullptr;
		if(of_it == open_files.end()) {
			TDirectory * const tdir = gDirectory;
			file = TFile::Open(file_path.c_str());
			if(!file) {
				std::cout << "ERROR - could not find file:\n" << file_path << "\n";
				exit(1);
			}
			open_files.emplace(file_path, file);
			tdir->cd();
		}
		else {
			file = of_it->second;
		}
		T * object = dynamic_cast<T *>(file->Get(object_path.c_str()));
		if(!object) {
			std::cout << "ERROR - could not find object: " << object_path << " in file: " << file_path << "\n";
			exit(1);
		}
		return object;

	}




};

#endif
