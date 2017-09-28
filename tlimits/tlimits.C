




void tlimits(char const * fname, std::string const method) {

  TFile * ifile = TFile::Open(fname);
  TH1 * hsp = (TH1*)ifile->Get(("hsp_"+method).c_str());
  TH1 * hspc = (TH1*)ifile->Get(("hspcosmic_"+method).c_str());
  TH1 * hbnbc = (TH1*)ifile->Get(("hbnbcosmic_"+method).c_str());
  //TH1* dh=(TH1*)infile->Get("data");

  /*
  TLimitDataSource * mydatasource = new TLimitDataSource(sh,bh,dh);
  TConfidenceLevel * myconfidence = TLimit::ComputeLimit(mydatasource,50000);

  std::cout << "  CLs    : " << myconfidence->CLs()  << std::endl;
  std::cout << "  CLsb   : " << myconfidence->CLsb() << std::endl;
  std::cout << "  CLb    : " << myconfidence->CLb()  << std::endl;
  std::cout << "< CLs >  : " << myconfidence->GetExpectedCLs_b()  << std::endl;
  std::cout << "< CLsb > : " << myconfidence->GetExpectedCLsb_b() << std::endl;
  std::cout << "< CLb >  : " << myconfidence->GetExpectedCLb_b()  << std::endl;

  delete myconfidence;
  delete mydatasource;
  */

  ifile->Close();

}
