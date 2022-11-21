#ifndef FLAT_VAR_H
#define FLAT_VAR_H


#include <iostream>
#include <string>

#include "TTree.h"
#include "TTreeFormula.h"

class FlatVar{
private:
    std::string def;
    std::string name;
    int type;

    void special_character_check_helper(std::string& str);
public:
    static const int int_type = 0, vint_type = -1, vdouble_type = 1, formula_type = 2;

    FlatVar(const std::string& in_def, int in_type, const std::string& in_name = "NONE"): def(in_def),type(in_type), name(in_name){
	if(name == "NONE"){
	    name = def;
	    special_character_check_helper(name);
	}
    }

    void Print();
};

void FlatVar::special_character_check_helper(std::string& str){
    for(auto& ch : str){
	if( (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ( ch >= '0' && ch <= '9') || ch =='_') continue;

	//replace any char that's not English letters or numbers or underscores to '_'
        ch = '_';
    }
    return;
}

void FlatVar::Print(){
    std::cout << "Flat Var, def: " << def << ", name: " << name << ", type: " << (type == int_type? " int" : (type == vint_type ? "vector of ints" : (type == vdouble_type ? "vector of doubles" : "TTree formula"))) << std::endl;
    return;
}
#endif
