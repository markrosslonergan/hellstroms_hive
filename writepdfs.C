void writepdfs(std::string const c, std::string dir = "") {

  if(dir != "") dir += "/";

  //gROOT->SetBatch(true);
  
  TFile * f1 = TFile::Open(c.c_str());

  TIter next1(f1->GetListOfKeys());
  TKey * key1 = nullptr;

  while((key1 = (TKey*)next1())) {
   
    if(key1->ReadObj()->IsA() == TCanvas::Class()) {
      TCanvas * canvas = (TCanvas*)key1->ReadObj();
      cout << canvas->GetName() << "\n";
      canvas->SaveAs((dir+std::string(canvas->GetName())+".pdf").c_str());
    }
    
  }

}
