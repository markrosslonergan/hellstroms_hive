

tmva: tmva.cpp
	g++ -std=c++14 `root-config --cflags` tmva.cpp -o tmva `root-config --libs` -lTMVA -lTreePlayer