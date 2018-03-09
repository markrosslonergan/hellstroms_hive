

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
  permutations.GetFiles("permutations", 10);
  permutations.Check();
  
  return 0;

}
