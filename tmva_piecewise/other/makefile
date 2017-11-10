
run: run.cpp
	g++ -std=c++14 `root-config --cflags` *.cxx run.cpp -o run `root-config --libs ` -I `root-config --incdir`  -lTMVA -lTreePlayer -I inc/ 

clean:
	rm run

#g++ -std=c++14 `root-config --cflags` tmva.cpp -o tmva `root-config --libs` -lTMVA -lTreePlayer
