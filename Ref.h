#ifndef NODE_REF_H
#define NODE_REF_H

#include "Oned_topk.h"
using namespace std;

class MyRDAG_node;

struct node_ref{
  FourTopk_second_oned* A1;
  FourTopk_second_oned* A2;
  vector<int>* list1;
  vector<int>* list2;
  map<pair<int, int>, MyRDAG_node*>* node_map;
  int plx, ply, prx, pry;
  bool from_left;
  int* weight_bits;
};

#endif
