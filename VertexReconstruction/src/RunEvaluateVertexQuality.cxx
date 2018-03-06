

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
			    {argv + 1, argv + argc});
  evq.Run();
  
  std::cout << difftime(time(0), start) << "\n";  

  return 0;

}
