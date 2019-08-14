#!/bin/bash

#./run ../samples/mcc82 train
./run ../samples/mcc82 app
./run ../samples/mcc82 merge
./run ../samples/mcc82 getresponse
./run ../samples/mcc82 tlimits
