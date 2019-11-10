
#include <boost/algorithm/string.hpp>


#include "MeshReader.h"
#include "Global.h"
#include "Element.h"

// =====================================================================
//     readMesh
// =====================================================================


void                     readMesh 

   ( Global&     globdat,
     const char* fileName )

{
  string    filename  ( fileName );
  StrVector filenames;

  boost::split ( filenames, filename, boost::is_any_of(".") );

  if      ( filenames[1] == "msh" )
  {
    readGmshMesh ( globdat, fileName );
  }
  else if ( filenames[1] == "nurbs" )
  {
    readNURBSMesh ( globdat, fileName );
    globdat.isNURBS = true;
  }
  else if ( filenames[1] == "matlab" )
  {
    readNURBSMesh ( globdat, fileName );
  }
  else
  {
    cout << "mesh file not yet supported!!!\n";
    exit(1);
  }

  
  // summarise the mesh a bit

  cout << "MESH SUMMARY:\n";

  cout << "Number of nodes............................... " << globdat.nodeSet.size       () << endl;
  cout << "Number of boundary nodes...................... " << globdat.boundaryNodes.size () << endl;
  cout << "Number of elements............................ " << globdat.elemSet.size       () << endl;
  cout << "Number of element groups...................... " << globdat.dom2Elems.size     () << endl;
  if (globdat.is3D){
  cout << "Three dimensional mesh is being considered\n";
  }
  
  // compute element size (smallest) for determining critical time step
  // in explicit dynamic simulations
  
  const  int       elemCount = globdat.elemSet.size ();

  double he;
  double she(1e50); // smallest he

  ElemPointer ep;

  // loop over all bulk elements

  for ( int ie = 0; ie < elemCount; ie++ )
  {
     ep    = globdat.elemSet[ie];

     he    = ep->computeElementSize ( globdat );

     she   = min(he,she);
  }

  cout << "Smallest element size......................... " << she << "\n";
  cout << endl;
}


