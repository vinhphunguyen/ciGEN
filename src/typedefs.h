#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <limits>
#include <math.h>
#include <boost/shared_ptr.hpp>

using namespace std;

class Node;
class Element;

typedef vector<string>             StrVector;
typedef vector<int>                IntVector;
typedef set<int>                   IntSet;
typedef map<int, IntVector>        Int2IntVectMap;
typedef map<int,int>               Int2IntMap;
typedef map<int,IntSet>            Int2IntSetMap;
typedef map<int, string>           Int2StringMap;
typedef boost::shared_ptr<Node>    NodePointer;
typedef boost::shared_ptr<Element> ElemPointer;
typedef vector<NodePointer>        NodeSet;
typedef vector<ElemPointer>        ElemSet;

#endif
