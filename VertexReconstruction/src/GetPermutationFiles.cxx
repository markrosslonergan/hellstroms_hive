

#include <iostream>
#include <time.h>

#include "GetPermutations.h"


int main(int const argc, char const * argv[]) {

  Permutations permutations;
  permutations.AddParameter("start_prox", 4);
  permutations.AddParameter("shower_prox", 11, 4, 48);
  permutations.AddParameter("max_bp_dist", 11, 4, 48);
  permutations.AddParameter("cpoa_vert_prox", 13);
  permutations.AddParameter("cpoa_trackend_prox", 10);
  permutations.Get();

  /*
  permutations.SetParameterNames({"start_prox", "shower_prox", "max_bp_dist", "cpoa_vert_prox", "cpoa_trackend_prox"});
  permutations.AddPermutation({4, 4, 4, 13, 10});
  permutations.AddPermutation({4, 4, 24, 13, 10});
  */

  permutations.GetFiles("permutations", 10);
  permutations.Check();
  
  return 0;

}
