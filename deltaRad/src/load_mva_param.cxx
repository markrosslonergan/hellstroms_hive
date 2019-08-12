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

    std::cout<<"########################### Topology ###########################"<<std::endl;

    TiXmlElement *pTopoCut;
    pTopoCut = doc.FirstChildElement("topology");
    std::string topo_def;
    std::string topo_name;
    while(pTopoCut )
    {

        topo_def = pTopoCut->Attribute("def");
        topo_name = pTopoCut->Attribute("name");

        std::cout<<"Loading Topology "<<topo_name<<" with definition "<<topo_def<<std::endl;
        pTopoCut = pTopoCut->NextSiblingElement("topology");
    }



    TiXmlElement *pPreCut;
    pPreCut = doc.FirstChildElement("precut");

    std::vector<std::string> precuts;

    std::cout<<"########################### Precuts ###########################"<<std::endl;
    while(pPreCut )
    {

        std::string cut_def = pPreCut->Attribute("def");
        std::string cut_name = pPreCut->Attribute("name");

        std::cout<<"Loading Precut number "<<precuts.size()<<" "<<cut_name<<std::endl;
        std::cout<<"--- Define: "<<cut_def<<std::endl;
        precuts.push_back(cut_def);
        pPreCut = pPreCut->NextSiblingElement("precut");

    }
    std::cout<<"################################################################"<<std::endl;





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
                    // temp_struct.bdt_train_vars = bdt_train_vars;
                    // temp_struct.bdt_spec_vars = bdt_spec_vars;
                    temp_struct.precuts = precuts;

                    vec_methods.push_back(temp_struct);		

                    if(isVerbose) std::cout<<" MVALoader::MVALoader || Loading a method: "<<mva_type<<"::"<<method_type<<" with params: "<<param_string<<std::endl;


                }
                pMethod = pMethod->NextSiblingElement("method");

            }//-->end method loop	


            TiXmlElement *pMVAfile = pMVA->FirstChildElement("file");
            while(pMVAfile){
                //filename, foldername, traning_cut, training_fraction
                vec_methods.back().filename = pMVAfile->Attribute("filename"); 
                vec_methods.back().foldername = pMVAfile->Attribute("foldername"); 
                vec_methods.back().training_cut = pMVAfile->Attribute("training_cut"); 
                vec_methods.back().training_fraction= strtof(pMVAfile->Attribute("training_fraction"),NULL); 

                pMVAfile = pMVA->NextSiblingElement("file");
            }//end file


            TiXmlElement *pMVAscan = pMVA->FirstChildElement("scan");
            if(!pMVAscan){
                vec_methods.back().scan_max = -99; 
                vec_methods.back().scan_min = -99; 
                vec_methods.back().scan_steps = -99; 
            }else{
                while(pMVAscan){
                    vec_methods.back().scan_max = strtof(pMVAscan->Attribute("max"),NULL); 
                    vec_methods.back().scan_min = strtof(pMVAscan->Attribute("min"),NULL); 
                    vec_methods.back().scan_steps = strtof(pMVAscan->Attribute("steps"),NULL); 
                    std::cout<<"Scan params "<<vec_methods.back().scan_steps<<" "<<vec_methods.back().scan_min<<" "<<vec_methods.back().scan_max<<std::endl;   
                    pMVAscan = pMVA->NextSiblingElement("scan");
                }//end scan
            }

            vec_methods.back().topological_definition = topo_def;
            vec_methods.back().topological_name = topo_name;

        }
pMVA = pMVA->NextSiblingElement("mva");

    }//--> end mva loop

    std::cout<<"####################### Variables ########################################"<<std::endl;


    TiXmlElement *pVar = doc.FirstChildElement("var");
    std::vector<bdt_variable> bdt_all_vars;

    std::vector<bdt_variable> bdt_train_vars;
    std::vector<bdt_variable> bdt_spec_vars;

    while(pVar){
        std::string var_def = pVar->Attribute("def");
        std::string var_binning = pVar->Attribute("binning");
        std::string var_unit = pVar->Attribute("unit");
        std::string var_type = pVar->Attribute("type");
        //std::string var_spectator = pVar->Attribute("spectator");
        const char* var_logplot = pVar->Attribute("logplot");
        bool var_logplot_bool;
        if (var_logplot ==NULL || var_logplot =="false"){
            var_logplot_bool= false;
        }else{
            var_logplot_bool= false;
        }

        bool is_spec = false;
        //if(var_spectator=="true") is_spec = true;

        bdt_variable t(var_def,var_binning,var_unit,"false",var_type);
        t.is_logplot = var_logplot_bool;

        bdt_all_vars.push_back(t);

        if(is_spec){
            bdt_spec_vars.push_back(t );            
            std::cout<<"Adding Spectator Variable "<<var_def<<" with binning: "<<var_binning<<std::endl;
        }else{
            bdt_train_vars.push_back(t);            
            std::cout<<"Adding Training Variable  "<<var_def<<" with binning: "<<var_binning<<std::endl;
        }

        std::string var_train_string = pVar->Attribute("training");
        std::vector<int> var_train_int;
        std::cout<<"NWO: Variable training string is "<<var_train_string<<std::endl;
        for (auto && c : var_train_string) {
            var_train_int.push_back((int)c - '0');
            std::cout<<var_train_int.back()<<std::endl;
        }

        //Loop over vec_methods
        for(int k=0; k< var_train_int.size(); k++){
            //Check to see if this variable is for training on this particular method
            vec_methods[var_train_int[k]].bdt_train_vars.push_back(t);
            std::cout<<"NWO: Adding "<<var_def<<" as training to method "<<vec_methods[var_train_int[k]].bdt_name<<std::endl;
        }

        pVar = pVar->NextSiblingElement("var");
    }


    for(auto &v: vec_methods){
        v.bdt_all_vars = bdt_all_vars;
    }

    //type_instance.DestroyInstance();
};



std::vector<method_struct> MVALoader::GetMethods(){
    return vec_methods; 
}
