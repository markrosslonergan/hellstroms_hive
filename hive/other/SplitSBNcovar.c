void SplitSBNcovar(){

    ///uboone/app/users/markrl/SL7test0/hellstroms_hive/hive/xml/SBNfit_Integration_XMLS/Oct2020Selections/master_FinalSelection_1g1p_1g0p_2g1p_2g1p_Oct2020_v17.xml
    //TFile * fin = new TFile("/uboone/app/users/markrl/SBNfit_uBooNE/July2020_SL7/whipping_star/build/bin/New2020Covar/Master/ProposalB5/PropBV2_FluxXSDet.SBNcovar.root","read");
    TFile * fin = new TFile("/uboone/data/users/markross/Mar2020/SBNfit_files/Dec2020_Covar_Matrix/OTPCFix_v19_FluxXSDet.SBNcovar.root","read");
    TMatrixD * frac = (TMatrixD*)fin->Get("frac_covariance");


    TFile *fout1g1p = new TFile("Fin_FluxXSDet_1g1p.SBNcovar.root","recreate");
    fout1g1p->cd();
    TMatrixD f1g1p = frac->GetSub(0,59,0,59);
    f1g1p.Write("frac_covariance");
    fout1g1p->Close();
    int mod = 60;


    TFile *fout1g0p = new TFile("Fin_FluxXSDet_1g0p.SBNcovar.root","recreate");
    fout1g0p->cd();
    TMatrixD f1g0p = frac->GetSub(0+mod,mod+99,0+mod,mod+99);
    f1g0p.Write("frac_covariance");
    fout1g0p->Close();
    mod+=100;

    if(true){
    TFile *fout2g1p = new TFile("Fin_FluxXSDet_2g1p.SBNcovar.root","recreate");
    fout2g1p->cd();
    TMatrixD f2g1p = frac->GetSub(0+mod,mod+99,mod,mod+99);
    f2g1p.Write("frac_covariance");
    fout2g1p->Close();

    mod+=100;
    TFile *fout2g0p = new TFile("Fin_FluxXSDet_2g0p.SBNcovar.root","recreate");
    fout2g0p->cd();
    TMatrixD f2g0p = frac->GetSub(0+mod,mod+99,mod,mod+99);
    f2g0p.Write("frac_covariance");
    fout2g0p->Close();
    }













}
