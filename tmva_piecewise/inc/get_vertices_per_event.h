#ifndef GETVERTICESPEREVENT_H
#define GETVERTICESPEREVENT_H



/******** Standard includes *****/
#include <float.h>

/******** Our includes *****/

/******** Root includes *****/
#include "TFriendElement.h"
#include "TTree.h"
#include "TTreeFormula.h"


double get_vertices_per_event(TTree * vertex_tree, std::string const & cut1 = "", std::string const & cut2 = "", TTree * mva_tree = nullptr, double const mva1 = -DBL_MAX, double const mva2 = -DBL_MAX);

#endif
