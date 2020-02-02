void add_covar(){

    std::string dir1 = "/uboone/app/users/markrl/SBNfit_uBooNE/NEW_Improved_V2/whipping_star/build/bin/Jan2020_technote_v1_1g1p/autoxml/";
    std::string dir2 = "/uboone/app/users/markrl/SBNfit_uBooNE/NEW_Improved_V2/whipping_star/build/bin/Jan2020_technote_v1_1g1p/DetSys/LY/autoxml/";
    std::string dir3 = "/uboone/app/users/markrl/SBNfit_uBooNE/NEW_Improved_V2/whipping_star/build/bin/Jan2020_technote_v1_1g1p/DetSys/wireX/autoxml/";
    std::string dir4 = "/uboone/app/users/markrl/SBNfit_uBooNE/NEW_Improved_V2/whipping_star/build/bin/Jan2020_technote_v1_1g1p/DetSys/wireYZ/autoxml/";
    std::string dir5 = "/uboone/app/users/markrl/SBNfit_uBooNE/NEW_Improved_V2/whipping_star/build/bin/Jan2020_technote_v1_1g1p/DetSys/wireA/autoxml/";

    std::string outdir = "/uboone/app/users/markrl/SBNfit_uBooNE/NEW_Improved_V2/whipping_star/build/bin/Jan2020_technote_v1_1g1p/DetSys/AllDet/";

    std::vector<std::string> dirs = {dir1,dir2,dir3,dir4,dir5};

    std::vector<int> vars =  {3,4,5,6,14,15,19,20,28,31,36,37,41,42,43,44,45,46,47,55,58,59,60,62,63,64,65,66,67,68,69,71,72,73,74,76,82,83,84,85,86,88,89,90,91,92,93,97,99,102,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129};

    //Pass in N files each with a TMatrixD<double> called 
    std::string m_name = "frac_covariance";

    for(int i=0; i< vars.size(); i++){
        std::string nam = "VID"+std::to_string(vars[i])+".SBNcovar.root";
        TFile f((outdir+nam).c_str(),"recreate");
        f.cd();
        TMatrixT<double> Mout;
        for(int f=0; f< dirs.size(); f++){
            TFile fin((dirs[f]+nam).c_str(),"read");
            TMatrixT<double>  Min = *(TMatrixT<double>*)fin.Get(m_name.c_str());
            if(f=0){
                Mout = Min;
            }else{
                Mout.Add(&Min);
            }
        }
        f.cd();
        Mout.Write(m_name.c_str(), TObject::kWriteDelete);
        f.Close();
    }

