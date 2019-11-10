#include "Global.h"
#include "Node.h"
#include "Element.h"


#include <boost/algorithm/string.hpp>


// =====================================================================
//     writeAbaqusMesh
// =====================================================================

/**
 * Write the nodes, the modified bulk elements to two files with extension of 
 * INP. Also write the interface elements to another INP file. 
 * Finally, write a main Abaqus INP file for a static analysis. 
 * You will have to modify this one manually before submitting it to Abaqus.
 *
 * Phu Nguyen, Monash University, July 2016.
 */

void                     writeAbaqusMesh 

    ( Global&     globdat,
      const char* fileName )
{
  StrVector filenames;
  string    nodeFile   ("");
  string    elemFile   ("");
  string    inteFile   ("");

  boost::split ( filenames, fileName, boost::is_any_of(".") );

  nodeFile  = filenames[0] + "-node.inp"; 
  elemFile  = filenames[0] + "-bulk-elems.inp"; 
  inteFile  = filenames[0] + "-int-elems.inp"; 

  ofstream file1 ( nodeFile.c_str(), std::ios::out );
  ofstream file2 ( elemFile.c_str(), std::ios::out );
  ofstream file3 ( inteFile.c_str(), std::ios::out );
  ofstream file  ( fileName,         std::ios::out );

  cout << "Writing nodes...\n";

  const int nodeCount    = globdat.newNodeSet.size   ();
  const int elemCount    = globdat.elemSet.size      ();
  const int bndElemCount = globdat.bndElementSet.size();

  NodePointer np;

  for ( int in = 0; in < nodeCount; in ++ )
  {
    np = globdat.newNodeSet[in];
 
    if ( !globdat.is3D )
    {
      file1 << np->getIndex() 
            << ", " << np->getX() << ", " << np->getY() << "\n";
    }
    else
    {
      file1 << np->getIndex() << ", " 
	        << np->getX() << ", " 
	        << np->getY() << ", "
	        << np->getZ() << "\n";
    }
  }

  cout << "Writing nodes...done!\n\n";

  cout << "Writing bulk elements...\n";

  ElemPointer ep;
  IntVector   connect, newConnect;

  for ( int ie = 0; ie < elemCount; ie ++ )
  {
    ep = globdat.elemSet[ie];

    //file2 << ep->getIndex () << ", ";
    file2 << ie + 1 << ", ";

    ep->getJemConnectivity ( connect ); 

    copy ( connect.begin(), connect.end()-1, ostream_iterator<int> (file2, ", " ) );
    file2 << connect[connect.size()-1] << "\n";
  }

  cout << "Writing bulk elements...done!\n\n";

  cout << "Writing user elements (interface elements)...\n";

  const int   ieCount = globdat.interfaceSet.size ();
  const int   inCount = globdat.nodeSet.     size ();

  IntVector   connec;
  IntVector   dupNodes;

  int         index;
  int         inter;

  for ( int ie = 0; ie < ieCount; ie++ )
  {
    ep = globdat.interfaceSet[ie];

    //file3 << ep->getIndex() << ", "; 
    file3 << elemCount + 1 + ie << ", "; 

    ep->getConnectivity ( connec );

    copy ( connec.begin(), 
	   connec.end()-1, 
	   ostream_iterator<int> ( file3, ", " ) 
	 );

    file3 << connec[connec.size()-1] << "\n";
  }

  cout << "Writing interface elements...done!\n\n";
  
  cout << "Writing Abaqus main input file...!\n\n";
  
  file << "*HEADING \n";
  file << "Generated automatically by ciGEN\n";
  file << "which is written by Phu Nguyen at Monash Univiersity.\n";
  file << "*NODE, INPUT= "    << nodeFile << "\n";
  file << "*ELEMENT, INPUT= " << elemFile << ", TYPE=CPS3\n";
  file << "*USER ELEMENT, TYPE=U1, NODE=4, COORDINATES=2, PROPERTIES=9, VARIABLES=4\n";
  file << "1, 2\n";
  file << "*ELEMENT, INPUT= " << inteFile << ", TYPE=U1\n";
  
  cout << "Writing bulk element groups...\n";


  const int domCount = globdat.dom2Elems.size ();

  auto it  = globdat.dom2Elems.begin ();
  auto eit = globdat.dom2Elems.end   ();
    
  int startElem = globdat.elemSet[0]->getIndex () - 1;

  for ( ; it != eit; ++it )
  {
    int       domName   = it->first; 
    IntVector elems     = it->second; 
    const int elemCount = elems.size();

    file << "*ELSET, " << "ELSET=" << domName << "\n";
    
    for ( int ie = 0; ie < elemCount-1; ie++ )
    {
      file << elems[ie] - startElem << ",";
      // Abaqus ELSET data line contains maximum 16 entries
      if ( ie % 16 == 0 ) file <<  "\n";
    }

    file << elems[elemCount-1] - startElem << "\n";
  }

  cout << "Writing bulk element groups...done!\n\n";

  cout << "Writing node groups...\n";

  const int nodeGrpCount = globdat.bndNodesMap.size ();

  //if ( nodeGrpCount != 1 )
  //{
    //Int2IntSetMap::iterator sit  = globdat.bndNodesMap.begin ();
    //Int2IntSetMap::iterator seit = globdat.bndNodesMap.end   ();

    auto sit  = globdat.bndNodesMap.begin ();
    auto seit = globdat.bndNodesMap.end   ();

    for ( ; sit != seit; ++sit )
    {
      int       domName = sit->first; 
      IntSet    elems   = sit->second; 

      IntVector   nodes;
      copy ( elems.begin(), elems.end(), back_inserter(nodes) );
   
      file << "*NSET, " << "NSET=" << domName << "\n";

      for ( int ie = 0; ie < nodes.size()-1; ie++ )
      {
	file << nodes[ie] << ",";
      }

      file << nodes[nodes.size()-1] << "\n";
    }
 // }

  const int isoNodeCount = globdat.isolatedNodes.size ();

  if ( isoNodeCount != 0 )
  {
    for ( int in = 0; in < isoNodeCount; ++in )
    {
      file << "*NSET, " << "NSET=" << "dd" << "\n";

      file << globdat.isolatedNodes[in] << "\n";
    }
  }
  
  file << "*SOLID SECTION, ELSET= , MATERIAL=  \n";
  file << "*MATERIAL, NAME=  \n";


  file << "*UEL PROPERTY, ELSET= \n\n";
  // this is for fixed BCs, defined in the initial load step
  // be careful with this Abaqus feature
  file << "*BOUNDARY\n\n";


  file << "****************************************\n";
  file << "** HISTORY DATA\n";
  file << "*STEP, NLGEOM=NO, INC= \n";
  file << "*STATIC, RIKS\n\n";
  // this is for moving boundary conditions such as displacement control
  file << "*BOUNDARY\n\n";


  file << "*END STEP\n";

  cout << "Writing node groups...done!\n\n";
}

