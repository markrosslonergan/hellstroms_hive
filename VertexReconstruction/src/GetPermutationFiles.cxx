

#include <iostream>
#include <time.h>

#include "GetPermutations.h"


int main(int const argc, char const * argv[]) {

  Permutations permutations;

  permutations.AddParameter("start_prox", 4);
  permutations.AddParameter("shower_prox", 9, 10, 100);
  permutations.AddParameter("max_bp_dist", 9, 10, 100);
  permutations.AddParameter("cpoa_vert_prox", 5, 5, 30);
  permutations.AddParameter("cpoa_trackend_prox", 5, 5, 30);

  permutations.Get();

  /*
  permutations.SetParameterNames({"start_prox", "shower_prox", "max_bp_dist", "cpoa_vert_prox", "cpoa_trackend_prox"});
  permutations.AddPermutation({4, 4, 4, 13, 10});
  permutations.AddPermutation({4, 4, 24, 13, 10});
  */

  permutations.GetFiles("permutations", 360);
  permutations.Check();
  
  return 0;

}
