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
    std::cout<<"######################## BDT's to Train on ########################################"<<std::endl;





    TiXmlElement *pMVA; 


    //Grab the first element. Note very little error checking here! make sure they exist.
    pMVA = doc.FirstChildElement("mva");
    if(!pMVA) {
        std::cerr<<"ERROR: MVALoader::MVALoader || XMl contains no mva's! "<<whichxml<<std::endl;
        exit(EXIT_FAILURE);
    }

    TMVA::Types  type_instance = TMVA::Types::Instance();
    int n_bdt = 0;
    while(pMVA )
    {
        if( (std::string)pMVA->Attribute("use") == "yes"){
            std::string mva_type = pMVA->Attribute("type");
            //for each type, find all methods to be used

            std::string bdt_tag = pMVA->Attribute("tag");
            std::string bdt_name = pMVA->Attribute("name");
            std::string bdt_binning = pMVA->Attribute("binning");

            std::cout<<"starting BDT number "<<n_bdt<<" with TAG: "<<bdt_tag<<" name: "<<bdt_name<<"  with binning "<<bdt_binning<<std::endl;

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
        n_bdt++;
        pMVA = pMVA->NextSiblingElement("mva");

    }//--> end mva loop


    std::cout<<"#######################  BDT_Files  ########################################"<<std::endl;
    std::cout<<"(Print out will be below when constructing bdt_file class)"<<std::endl;

    TiXmlElement *pBDTfile; 


    //Grab the first element. Note very little error checking here! make sure they exist.
    pBDTfile = doc.FirstChildElement("bdtfile");
    if(!pBDTfile) {
        std::cerr<<"ERROR: MVALoader::MVALoader || XMl contains no BDT_files! "<<whichxml<<std::endl;
        exit(EXIT_FAILURE);
    }
    n_bdt_files = 0;

    while(pBDTfile)
    {
        const char* t_tag = pBDTfile->Attribute("tag");
        if(t_tag==NULL){std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has no `tag` attribute! "<<std::endl; exit(EXIT_FAILURE);}
        bdt_tags.push_back(t_tag);

        const char* t_filename = pBDTfile->Attribute("filename");
        if(t_filename==NULL){std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has no `filename` attribute! "<<std::endl; exit(EXIT_FAILURE);}
        bdt_filenames.push_back(t_filename);

        const char* t_hist_styles = pBDTfile->Attribute("hist_style");
        if(t_hist_styles==NULL){ bdt_hist_styles.push_back("hist");}else{
            bdt_hist_styles.push_back(t_hist_styles);
        }

        const char* t_fillstyles = pBDTfile->Attribute("fillstyle");
        if(t_fillstyles==NULL){ bdt_fillstyles.push_back(1001);}else{
            bdt_fillstyles.push_back((int)strtod(t_fillstyles));
        }


        const char* t_dirs = pBDTfile->Attribute("dirs");
        if(t_dirs==NULL){std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has no `dirs` attribute! "<<std::endl; exit(EXIT_FAILURE);}
        bdt_dirs.push_back(t_dirs);

        const char* t_col = pBDTfile->Attribute("col");
        if(t_col==NULL){std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has no `col` attribute! "<<std::endl; exit(EXIT_FAILURE);}
        std::string s_col = t_col;
        s_col.erase(std::remove(s_col.begin(), s_col.end(), '('), s_col.end());
        s_col.erase(std::remove(s_col.begin(), s_col.end(), ')'), s_col.end());
        std::vector<double> v_col; 
        size_t pos = 0;
        std::string delim = ",";
        std::string token;
        while ((pos = s_col.find(delim)) != std::string::npos) {
            token = s_col.substr(0, pos);
            v_col.push_back(std::stod(token));
            s_col.erase(0, pos + delim.length());
        }
        v_col.push_back(std::stod(s_col));
        //for(auto &d:v_col)std::cout<<d<<std::endl;
        bdt_cols.push_back(new TColor(TColor::GetFreeColorIndex(),v_col[0],v_col[1],v_col[2]) );

        const char* t_scales = pBDTfile->Attribute("scale");
        if(t_scales==NULL){
            bdt_scales.push_back(1.0);
        }else{
            bdt_scales.push_back(atof(t_scales));
        }

        const char* t_signals = pBDTfile->Attribute("signal");
        if(t_signals=="true"){
            bdt_is_signal.push_back(true);
        }else{
            bdt_is_signal.push_back(false);
        }

        const char* t_plotname = pBDTfile->Attribute("plotname");
        if(t_plotname==NULL){
            bdt_plotnames.push_back(bdt_tags.back());
        }else{
            bdt_plotnames.push_back(t_plotname);
        }


        TiXmlElement *pDefinition = pBDTfile->FirstChildElement("definition");
        std::vector<std::string> this_denom; 
        while(pDefinition){
            TiXmlElement *pCut = pDefinition->FirstChildElement("cut");
            while(pCut){
                this_denom.push_back(std::string(pCut->GetText()));
                pCut = pCut->NextSiblingElement("cut");
            }
            pDefinition = pDefinition->NextSiblingElement("definition");
        }//-->end definition
        bdt_definitions.push_back(this_denom);

        //next lets check if its the Signal Training
        TiXmlElement *pTrain = pBDTfile->FirstChildElement("training");
        std::vector<std::string> this_tcut; 
        while(pTrain){

            TiXmlElement *pTCut = pTrain->FirstChildElement("cut");
            while(pTCut){
                this_tcut.push_back(std::string(pTCut->GetText()));
                pTCut = pTCut->NextSiblingElement("cut");
            }

            pTrain = pTrain->NextSiblingElement("training");
        }//-->end training cuts           
        bdt_training_cuts.push_back(this_tcut);

        //So, some book-keeping if its data!
        bool is_data = false;
        TiXmlElement *pData = pBDTfile->FirstChildElement("data");

            bdt_is_onbeam_data.push_back(false);
            bdt_is_offbeam_data.push_back(false);
            bdt_onbeam_pot.push_back(-999);
            bdt_offbeam_spills.push_back(-999);
            bdt_onbeam_spills.push_back(-999);


        while(pData){

            const char* t_use = pData->Attribute("use");
            if(t_use=="no"){break;}

            const char* t_type = pData->Attribute("type");
            if(t_type==NULL){std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has been designated data, but not given a `type` attribute! wither OnBeam or OffBeam!! "<<std::endl; exit(EXIT_FAILURE);}
            std::string s_type = t_type;
            if(s_type == "OnBeam"){
                bdt_is_onbeam_data.back() = true;
                is_data = true;
            }else if(s_type == "OffBeam"){
                bdt_is_offbeam_data.back() = true;
                is_data = true;
            }else{
                std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has been designated data, but its `type` attribute is neither OnBeam or OffBeam!! "<<t_type<<" "<<std::endl; exit(EXIT_FAILURE);
            }
            bdt_onbeam_pot.back() = 0;
            bdt_offbeam_spills.back() = 0;
            bdt_onbeam_spills.back() = 0;

            TiXmlElement *pTor = pData->FirstChildElement("tor860_wcut");
            while(pTor){
                std::string tor = std::string(pTor->GetText());
                bdt_onbeam_pot.back() = stod(tor);    
                pTor = pTor->NextSiblingElement("tor860_wcut");
            }

            TiXmlElement *pSpills = pData->FirstChildElement("E1DCNT_wcut");
            while(pSpills){
                std::string tor = std::string(pSpills->GetText());
                bdt_onbeam_spills.back() = stod(tor);    
                pSpills = pSpills->NextSiblingElement("E1DCNT_wcut");
            }
            if(bdt_onbeam_spills.back()==0 && bdt_is_offbeam_data.back()==true){
                std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has been designated "<<t_type<<" data, but no ON beam Spills has been given for normalization! Use Zarko's tool "<<std::endl; exit(EXIT_FAILURE);
            }


            TiXmlElement *pExt = pData->FirstChildElement("EXT");
            while(pExt){
                std::string tor = std::string(pExt->GetText());
                bdt_offbeam_spills.back() = stod(tor);    
                pExt = pExt->NextSiblingElement("EXT");
            }
            if(bdt_offbeam_spills.back()==0 && bdt_is_offbeam_data.back()==true){
                std::cerr<<"ERROR: MVALoader::MVALoader || bdt_file has been designated "<<t_type<<" data, but no OFF beam Spills has been given for normalization! Use Zarko's tool "<<std::endl; exit(EXIT_FAILURE);
            }

            pData = pData->NextSiblingElement("data");
        }//-->end data


        pBDTfile = pBDTfile->NextSiblingElement("bdtfile");
        n_bdt_files++;

    }//--> end bdt_file


    std::cout<<"####################### Variables ########################################"<<std::endl;


    TiXmlElement *pVar = doc.FirstChildElement("var");
    std::vector<bdt_variable> bdt_all_vars;

    std::vector<bdt_variable> bdt_train_vars;
    std::vector<bdt_variable> bdt_spec_vars;
    int n_var = 0;
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

        std::cout<<"Variable Number "<<n_var<<" is "<<var_unit<<" with definiton: "<<var_def<<std::endl;

        bdt_all_vars.push_back(t);

        if(is_spec){
            bdt_spec_vars.push_back(t );            
            //std::cout<<" -- adding as pectator Variable "<<var_def<<" with binning: "<<var_binning<<std::endl;
        }else{
            bdt_train_vars.push_back(t);            
            //std::cout<<" -- adding Training Variable  "<<var_def<<" with binning: "<<var_binning<<std::endl;
        }

        std::string var_train_string = pVar->Attribute("training");
        std::vector<int> var_train_int;
        std::cout<<" -- Variable training string is "<<var_train_string<<std::endl;
        for (auto && c : var_train_string) {
            var_train_int.push_back((int)c - '0');
        }

        //Loop over vec_methods
        for(int k=0; k< var_train_int.size(); k++){
            //Check to see if this variable is for training on this particular method
            vec_methods[var_train_int[k]].bdt_train_vars.push_back(t);
            std::cout<<" -- so adding "<<var_def<<" as training to method "<<vec_methods[var_train_int[k]].bdt_name<<std::endl;
        }

        n_var++;
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
