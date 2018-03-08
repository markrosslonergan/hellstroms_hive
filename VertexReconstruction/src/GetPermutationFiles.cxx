

#include <iostream>
#include <time.h>

#include "GetPermutations.h"


int main(int const argc, char const * argv[]) {

  Permutations permutations;
  permutations.AddParameter("start_prox", 4);
  permutations.AddParameter("shower_prox", 7, 5, 40);
  permutations.AddParameter("max_bp_dist", 7, 5, 40);
  permutations.AddParameter("cpoa_vert_prox", 3, 3, 12);
  permutations.AddParameter("cpoa_trackend_prox", 3, 3, 12);
  permutations.GetFiles("permutations", 100);
  permutations.Check();
  
  return 0;

}
