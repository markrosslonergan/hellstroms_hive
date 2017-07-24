

tmva: tmva.cpp
	g++ `root-config --cflags` tmva.cpp -o tmva `root-config --libs` -lTMVA