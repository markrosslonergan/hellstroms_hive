


#include "tmva/Types.h"



struct method_struct {

  TMVA::Types::EMVA const type;
  std::string const str;
  std::string const option;

  method_struct(TMVA::Types::EMVA const a, std::string const & b, std::string const & c) :
    type(a),
    str(b),
    option(c) {}

};
