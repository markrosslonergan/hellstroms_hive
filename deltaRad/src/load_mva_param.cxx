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

	while(pMVA && (std::string)pMVA->Attribute("use") == "yes")
	{

		std::string mva_type = pMVA->Attribute("type");
		//for each type, find all methods to be used


		//use TMVA instance to get the right EMVA type
		TMVA::Types::EMVA tmva_type = type_instance.GetMethodType(mva_type.c_str());


		TiXmlElement *pMethod = pMVA->FirstChildElement("method");
		while(pMethod && (std::string)pMethod->Attribute("use") == "yes"){


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


			method_struct temp_struct = { tmva_type , method_type, param_string};
			vec_methods.push_back(temp_struct);		
	
			if(isVerbose) std::cout<<" MVALoader::MVALoader || Loading a method: "<<mva_type<<"::"<<method_type<<" with params: "<<param_string<<std::endl;
			


			pMethod = pMethod->NextSiblingElement("method");

		}//-->end method loop	



		pMVA = pMVA->NextSiblingElement("mva");

	}//--> end mva loop


	//type_instance.DestroyInstance();
};



std::vector<method_struct> MVALoader::GetMethods(){
	return vec_methods; 
}
