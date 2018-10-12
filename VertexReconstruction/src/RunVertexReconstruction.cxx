#include <iostream>
#include <time.h>
#include <vector>
#include "Storage.h"
#include "Processor.h"
#include "VRAnalyzer.h"
#include <string>
#include <getopt.h>


int main(int const argc, char * argv[]) {

  time_t start = time(0);
  std::string timestamp = std::to_string((int)start); 
  


	//All of this is just to load in command-line arguments, its not that important
	const struct option longopts[] = 
	{
		{"flash", 		required_argument, 	0, 'f'},
		{"input", 		required_argument, 	0, 'i'},
		{"output", 		required_argument, 	0, 'o'},
		{"help", 		no_argument, 	0, 'h'},
		{0,			no_argument, 		0,  0}
	};

	std::string which_flash = "MC";
	double flash_start = 3.2;
	double flash_end = 4.8;
	std::string input_file;
    std::string output_dir = "./";
	int iarg = 0; opterr=1; int index;
	while(iarg != -1)
	{
		iarg = getopt_long(argc,argv, "o:f:?:i:h", longopts, &index);

		switch(iarg)
		{
            case 'o':
                  output_dir = optarg;
                   break;
			case 'f':
				which_flash = optarg;
				if(which_flash == "MC"){
					flash_start = 3.10; 
					flash_end = 4.90; 
				}
				if(which_flash == "Data"){
					flash_start = 3.20; 
					flash_end = 5.00; 
				}
				if(which_flash == "BNBext"){
					flash_start = 3.60; 
					flash_end = 5.40; 
				}
				if(which_flash == "Overlay"){
					flash_start = 3.50; 
					flash_end = 5.30; 
				}
				break;
			case 'i':
				input_file = optarg;
				break;
			case '?':
			case 'h':
				std::cout<<"Allowed arguments:"<<std::endl;
				std::cout<<"\t-i\t--input\t\tInput File"<<std::endl;
				std::cout<<"\t-o\t--output\t\tOutput Directory"<<std::endl;
				std::cout<<"\t-f\t--flash\t\tWhat flash timings"<<std::endl;
				std::cout<<"\t\t\t\t Options are:"<<std::endl;
				std::cout<<"\t\t\t\t MC: 3.10 ->  4.90 [Default]"<<std::endl;
				std::cout<<"\t\t\t\t Data: 3.20 -> 5.00"<<std::endl;
				std::cout<<"\t\t\t\t BNBext:3.60 -> 5.40 "<<std::endl;
				std::cout<<"\t\t\t\t Overlay: 3.50 ->5.30 "<<std::endl;
				std::cout<<"\t-h\t--help\t\tThis help menu"<<std::endl;
				return 0;
		}
	}


  //Bit of code to extract the jobsub id's from the path and append to the output filename
  std::vector<char const*>  files_input = {&input_file[0]}; 

  std::string first_file = input_file;
  std::vector<std::string> broken_down_first_file;
  std::string delimiter = "/";

  size_t pos = 0;
  std::string token;
  while ((pos = first_file.find(delimiter)) != std::string::npos) {
  	token = first_file.substr(0, pos);
	broken_down_first_file.push_back(token);
    	first_file.erase(0, pos + delimiter.length());
  }

  std::string output_name  = output_dir+"RunVertexReconstruction_"+timestamp+"_"+broken_down_first_file.at(broken_down_first_file.size()-2)+"_"+broken_down_first_file.back()+".root";

  std::cout<<"Starting Vertexing on file : "<<input_file<<std::endl;
  std::cout<<"Output Filename : "<<output_name<<std::endl;
  std::cout<<"Running in : "<<which_flash<<" Mode with beam gate between "<<flash_start<<" and "<<flash_end<<std::endl;


  Processor processor("FillLightEvent/pot_tree",
		      "FillLightEvent/meta_tree",
		      "FillLightEvent/event_tree",
		      files_input);
 

  //VR
  VRAnalyzer * vrana = new VRAnalyzer("varana");
  vrana->SetVerbose(false);
  vrana->SetVBVariables(4, 30, 100, 25, 5);
  //vrana->SetVBVariables(4, 20, 50, 13, 10);
 /* vrana->SetProducers("pandoraTrack",
		      "pandoraShower",
		      "pandoraCosmicHitRemoval",
		      "simpleFlashBeam",
		      //"swtrigger");
		      "daq");
  */            
  vrana->SetProducers(
              "pandoraNu",
		      "pandoraNu",
		      "pandoraCosmicHitRemoval",
		      "simpleFlashBeam",
		      //"swtrigger");
		      "daq");


  vrana->SetFlashTimings(flash_start, flash_end);

  //vrana->RunPandora();
  vrana->GetPOT();
  vrana->RunFillTreeVariables();
  //vrana->RunVerticesPerEvent();
  

  processor.AddAnalyzer(vrana);
  processor.SetOutputFileName(output_name.c_str());
  processor.Run();

  std::cout << "Wall time: " << difftime(time(0), start) << "\n";
  
  return 0;

}
