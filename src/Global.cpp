#include "Global.h"
#include "Node.h"

Global::Global()
{
  isEveryWhere     = true;   
  isInterface      = false;
  isDomain         = false;
  isPolycrystal    = false;
  isNotch          = false;
  isIgSegment      = false;
  isQuadratic      = false;
  is3D             = false;
  isContinuum      = true;
  isNURBS          = false;
  isNeper          = false;
  isConverter      = false;
  isHydraulic      = false;
  isMatlab         = false;
  outAbaqus        = false;
  rigidDomain.push_back ( -10 ); 
  rigidDomain.push_back ( -20 ); 
  internalEdges.push_back ( 0 );
}


void Global::getBounds () 
{
  XCoordComp xcomp;  
  YCoordComp ycomp;  
  ZCoordComp zcomp;  

  auto result = std::minmax_element ( nodeSet.begin (), nodeSet.end (), xcomp );  

  xMin = (*result.first) ->getX();
  xMax = (*result.second)->getX();

  result = std::minmax_element ( nodeSet.begin (), nodeSet.end (), ycomp );  

  yMin = (*result.first) ->getY();
  yMax = (*result.second)->getY();

  result = std::minmax_element ( nodeSet.begin (), nodeSet.end (), zcomp );  

  zMin = (*result.first) ->getZ();
  zMax = (*result.second)->getZ();
}

