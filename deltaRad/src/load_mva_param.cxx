#include "load_mva_param.h"
#include "method_struct.h"


MVALoader::MVALoader(std::string xmlname): MVALoader(xmlname,true) {

}

MVALoader::MVALoader(std::string xmlname, bool isVerbose_in) :whichxml(xmlname) {

	isVerbose = isVerbose_in;


	//Setup TiXml documents
	TiXmlDocument doc( xmlname.c_str() );
	bool loadOkay = doc.LoadFile();

	if(loadOkay){
		if(isVerbose) std::cout<<"MVALoader::MVALoader || Loaded "<<whichxml<<std::endl;
	}else{

		std::cerr<<"ERROR: MVALoader::MVALoader || Failed to load "<<whichxml<<std::endl;

		exit(EXIT_FAILURE);
	}

	TiXmlHandle hDoc(&doc);


	// we have Modes, Detectors, Channels, Covariance matricies, MC multisim data, oscillation pattern matching
	TiXmlElement *pMVA; 


	//Grab the first element. Note very little error checking here! make sure they exist.
	pMVA = doc.FirstChildElement("mva");
	if(!pMVA) {
		std::cerr<<"ERROR: MVALoader::MVALoader || XMl contains no mva's! "<<whichxml<<std::endl;
		exit(EXIT_FAILURE);
	}

	TMVA::Types  type_instance = TMVA::Types::Instance();

	while(pMVA )
	{
		if( (std::string)pMVA->Attribute("use") == "yes"){
		std::string mva_type = pMVA->Attribute("type");
		//for each type, find all methods to be used

        std::string bdt_tag = pMVA->Attribute("tag");
        std::string bdt_name = pMVA->Attribute("name");
        std::string bdt_binning = pMVA->Attribute("binning");

        std::cout<<"--starting BDT: "<<bdt_tag<<" name: "<<bdt_name<<"  with binning "<<bdt_binning<<std::endl;

		//use TMVA instance to get the right EMVA type
		TMVA::Types::EMVA tmva_type = type_instance.GetMethodType(mva_type.c_str());

		TiXmlElement *pVar = pMVA->FirstChildElement("var");
        std::vector<bdt_variable> bdt_train_vars;
        std::vector<bdt_variable> bdt_spec_vars;

        while(pVar){
            std::string var_def = pVar->Attribute("def");
            std::string var_binning = pVar->Attribute("binning");
            std::string var_unit = pVar->Attribute("unit");
            std::string var_type = pVar->Attribute("type");
            std::string var_spectator = pVar->Attribute("spectator");
            
            bool is_spec = false;
            if(var_spectator=="true") is_spec = true;

            if(is_spec) {
                bdt_spec_vars.push_back( bdt_variable(var_def,var_binning,var_unit,"false",var_type));            
                std::cout<<"Adding Train "<<var_def<<" with binning: "<<var_binning<<std::endl;
                        }else{
                bdt_train_vars.push_back( bdt_variable(var_def,var_binning,var_unit,"false",var_type));            
                std::cout<<"Adding Spectator "<<var_def<<" with binning: "<<var_binning<<std::endl;
            }
            pVar = pVar->NextSiblingElement("var");
        }


		TiXmlElement *pMethod = pMVA->FirstChildElement("method");
		while(pMethod ){
			if((std::string)pMethod->Attribute("use") == "yes"){

			std::string method_type = pMethod->Attribute("type");		


			std::vector<std::string> vec_params;	
			std::string param_string = "!H:!V";

			TiXmlElement *pParam = pMethod->FirstChildElement("param");
			while(pParam){
				vec_params.push_back( std::string(pParam->GetText()) );
				std::cout<<vec_params.back()<<std::endl;
				pParam = pParam->NextSiblingElement("param");
			}//-->end param loop


			for(std::string p: vec_params){
				param_string = param_string + ":" +p;
			}	


			method_struct temp_struct = {tmva_type , method_type, param_string};
            temp_struct.bdt_tag = bdt_tag;
            temp_struct.bdt_name = bdt_name;
            temp_struct.bdt_binning = bdt_binning;
            temp_struct.bdt_train_vars = bdt_train_vars;
            temp_struct.bdt_spec_vars = bdt_spec_vars;

			vec_methods.push_back(temp_struct);		
	
			if(isVerbose) std::cout<<" MVALoader::MVALoader || Loading a method: "<<mva_type<<"::"<<method_type<<" with params: "<<param_string<<std::endl;
			

			}
			pMethod = pMethod->NextSiblingElement("method");

		}//-->end method loop	


		}
		pMVA = pMVA->NextSiblingElement("mva");

	}//--> end mva loop


	//type_instance.DestroyInstance();
};



std::vector<method_struct> MVALoader::GetMethods(){
	return vec_methods; 
}
