

#include <iostream>
#include <time.h>

#include "EvaluateVertexQuality.h"


int main(int const argc, char const * argv[]) {

  time_t start = time(0);

  if(argc < 2) {
    std::cout << "Input: vertex_quality_tree files\n";
    return 1;
  }

  EvaluateVertexQuality evq("vertex_quality_tree_signal",
			    "permutation_tree",
			    {argv + 1, argv + argc});
  evq.SetOutputFile("RunEvaluateVertexQuality.root");

  evq.AddParameterToDraw({"correct_shower_total / true_shower_total", "", "tpc_volume_contained == 1",
	"completeness", "", "Completeness"});
  evq.AddParameterToDraw({"correct_shower_total / reco_shower_total", "", "tpc_volume_contained == 1",
	"cleanliness", "", "Cleanliness"});

  evq.Run();
  
  std::cout << "Wall time: " << difftime(time(0), start) << "\n";  

  return 0;

}
