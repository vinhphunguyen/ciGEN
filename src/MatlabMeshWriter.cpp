#include "Global.h"
#include "Node.h"
#include "Element.h"


#include <boost/algorithm/string.hpp>


// =====================================================================
//     writeMatlabMesh
// =====================================================================

void                     writeMatlabMesh 

    ( Global&     globdat,
      const char* fileName )
{
  // get the base name: fiber.mesh => fiber

  string    filename  ( fileName );
  StrVector filenames;

  boost::split ( filenames, filename, boost::is_any_of(".") );

  string nodefilename = filenames[0] + ".nodes";
  string elemfilename = filenames[0] + ".elems";

  ofstream nFile ( nodefilename, std::ios::out );
  ofstream eFile ( elemfilename, std::ios::out );
  
  cout << "Writing Matlab nodes file...\n";

  const int nodeCount    = globdat.newNodeSet.size   ();
  const int elemCount    = globdat.elemSet.size      ();
  const int felemCount   = globdat.flowElemSet.size  ();
  const int bndElemCount = globdat.bndElementSet.size();

  NodePointer np;

  for ( int in = 0; in < nodeCount; in ++ )
  {
    np = globdat.newNodeSet[in];
 
    if ( !globdat.is3D )
    {
      nFile << np->getIndex() 
           << " " << np->getX() << " " << np->getY() << "\n";
    }
    else
    {
      nFile << np->getIndex() << " " 
	        << np->getX() << " " 
	        << np->getY() << " "
	        << np->getZ() << "\n";
    }
  }

  cout << "Writing Matlab nodes...done!\n\n";

  cout << "Writing Matlab elements...\n";

  ElemPointer ep;
  IntVector   connect, newConnect;

  for ( int ie = 0; ie < elemCount; ie ++ )
  {
    ep = globdat.elemSet[ie];

    eFile << ep->getIndex () << " ";

    ep->getJemConnectivity ( connect ); 

    copy ( connect.begin(), connect.end(), ostream_iterator<int> (eFile, " " ) );

    eFile << "\n";
  }

  cout << "Writing bulk elements...done!\n\n";
}

