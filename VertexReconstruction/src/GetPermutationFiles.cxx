

#include <iostream>
#include <time.h>

#include "GetPermutations.h"


int main(int const argc, char const * argv[]) {

  Permutations permutations;
  permutations.AddParameter("start_prox", 4);
  permutations.AddParameter("shower_prox", 3, 4, 16);
  permutations.AddParameter("max_bp_dist", 3, 4, 16);
  permutations.AddParameter("cpoa_vert_prox", 13);
  permutations.AddParameter("cpoa_trackend_prox", 10);
  permutations.GetFiles("/Users/Bobby/rootfiles/permutations", 2);
  
  return 0;

}
