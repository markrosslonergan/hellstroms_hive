


#include "object_helper.hpp"
#include "TLimit.h"
#include "TLimitDataSource.h"
#include "TConfidenceLevel.h"



void tlimits(std::string const & ifile_path,
	     method_struct const & method) {

  object_helper oh;
  
  TH1 * signal_hist = oh.GetObject<TH1>(ifile_path, "signal_" + method.str + "_mva");
  TH1 * background_hist = oh.GetObject<TH1>(ifile_path, "background_" + method.str + "_mva");
  TH1 * data_hist = oh.GetObject<TH1>(ifile_path, "data_" + method.str + "_mva");

  TLimitDataSource * mydatasource = new TLimitDataSource(signal_hist, background_hist, data_hist);
  TConfidenceLevel * myconfidence = TLimit::ComputeLimit(mydatasource, 50000);
  std::cout << "  CLs    : " << myconfidence->CLs()  << std::endl;
  std::cout << "  CLsb   : " << myconfidence->CLsb() << std::endl;
  std::cout << "  CLb    : " << myconfidence->CLb()  << std::endl;
  std::cout << "< CLs >  : " << myconfidence->GetExpectedCLs_b()  << std::endl;
  std::cout << "< CLsb > : " << myconfidence->GetExpectedCLsb_b() << std::endl;
  std::cout << "< CLb >  : " << myconfidence->GetExpectedCLb_b()  << std::endl;

  delete myconfidence;
  delete mydatasource;

}



void tlimits(std::string const & ifile_path,
	     std::vector<method_struct> const & methods) {
  
  for(method_struct const & method : methods) {
    std::cout << "METHOD: " << method.str << "\n";
    tlimits(ifile_path, method);
    std::cout << "\n";
  }

}
