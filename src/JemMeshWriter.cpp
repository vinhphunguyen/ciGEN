#include "Global.h"
#include "Node.h"
#include "Element.h"


#include <boost/algorithm/string.hpp>


#include <iostream>
#include <iomanip>


// =====================================================================
//     writeJemMesh
// =====================================================================

void                     writeJemMesh 

    ( Global&     globdat,
      const char* fileName )
{
  ofstream file ( fileName, std::ios::out );

  cout << setprecision(10);   file << setprecision(10);//increase the precision
  
  cout << "Writing nodes...\n";

  file << "<Nodes>\n";

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
      file << np->getIndex() 
           << " " << np->getX() << " " << np->getY() << ";\n";
    }
    else
    {
      file << np->getIndex() << " " 
	   << np->getX() << " " 
	   << np->getY() << " "
	   << np->getZ() << ";\n";
    }
  }

  file << "</Nodes>\n";

  cout << "Writing nodes...done!\n\n";

  cout << "Writing bulk elements...\n";

  file << "<Elements>\n";

  ElemPointer ep;
  IntVector   connect, newConnect;

  for ( int ie = 0; ie < elemCount; ie ++ )
  {
    ep = globdat.elemSet[ie];

    file << ep->getIndex () << " ";

    //cout << ie << "\n";

    ep->getJemConnectivity ( connect ); 

    copy ( connect.begin(), connect.end(), ostream_iterator<int> (file, " " ) );

    //print ( connect.begin(), connect.end() );

    file << ";\n";
  }

  cout << "Writing bulk elements...done!\n\n";
  
  if ( !globdat.isNeper )
  {
    cout << "Writing boundary elements...\n";

    for ( int ie = 0; ie < bndElemCount; ie++ )
    {
      ep = globdat.bndElementSet[ie];

      // id of boundary elements numbered from the id of the last bulk element

      file << globdat.elemSet[elemCount-1]->getIndex() + ie + 1 << " ";

      ep->getJemConnectivity ( connect ); 

      //print ( connect.begin(), connect.end() );

      copy ( connect.begin(), connect.end(), ostream_iterator<int> (file, " " ) );

      file << ";\n";
    }
  }

  cout << "Writing boundary elements...done!\n\n";
      
  /*
  // id of flow elements numbered from the id of the last boundary element

  if ( globdat.isHydraulic )
  {
    cout << "Writing flow elements...\n";

    for ( int ie = 0; ie < felemCount; ie++ )
    {
      ep = globdat.flowElemSet[ie];


      file << globdat.elemSet[elemCount-1]->getIndex() + bndElemCount + ie + 1 << " ";

      ep->getJemConnectivity ( connect ); 

      copy ( connect.begin(), connect.end(), ostream_iterator<int> (file, " " ) );

      file << ";\n";
    }

    cout << "Writing flow elements...done!\n\n";
  }
  */

  // write zero-thickness interface elements
  // append the flow elements at the end of the interface elements,
  // if isHydraulic is true.


  const int   ieCount = globdat.interfaceSet.size ();
  const int   inCount = globdat.nodeSet.     size ();
  
        int   start1  = globdat.elemSet[elemCount-1]->getIndex() + bndElemCount + felemCount;
        int   start2  = start1 + ieCount;

  IntVector   connec;

  cout << "Writing interface elements in the solid mesh file...\n";

  if ( globdat.isHydraulic == false)
  {
     for ( int ie = 0; ie < ieCount; ie++ )
     {
       ep = globdat.interfaceSet[ie];

       file << start1 + ie + 1 << " ";

       ep->getConnectivity ( connec );

       copy ( connec.begin(), 
              connec.end(), 
              ostream_iterator<int> ( file, " " ) 
        );

       file << ";\n";
     }
  }
  else
  {
     ElemPointer fep;
     IntVector   fconnec;

     for ( int ie = 0; ie < ieCount; ie++ )
     {
       ep  = globdat.interfaceSet[ie];
       fep = globdat.flowElemSet[ie];

       file << start1 + ie + 1 << " ";

       ep ->getConnectivity ( connec );
       fep->getConnectivity ( fconnec );

       copy ( connec.begin(), 
              connec.end(), 
              ostream_iterator<int> ( file, " " ) 
        );
       copy ( fconnec.begin(), 
              fconnec.end(), 
              ostream_iterator<int> ( file, " " ) 
        );

       file << ";\n";
     }
  }
  
  cout << "Writing interface elements in the solid mesh file...done\n";

  file << "</Elements>\n\n";

  cout << "Writing bulk element groups...\n";

  //const int domCount = globdat.dom2Elems.size ();

  Int2IntVectMap::iterator it  = globdat.dom2Elems.begin ();
  Int2IntVectMap::iterator eit = globdat.dom2Elems.end   ();

  for ( ; it != eit; ++it )
  {
    int       domName = it->first; 
    IntVector elems   = it->second; 

    //file << "<ElementGroup name=\"" << domName << "\">\n{";
    file << "<ElementGroup name=" << globdat.physicalNames[domName] << ">\n{";
    
    for ( int ie = 0; ie < elems.size()-1; ie++ )
    {
      file << elems[ie] << ",";
    }

    file << elems[elems.size()-1] << "}\n"
         << "</ElementGroup>\n\n";
  }

  cout << "Writing bulk element groups...done!\n\n";

  if ( !globdat.isNeper )
  {
    cout << "Writing boundary element groups...\n";

    it  = globdat.dom2BndElems.begin ();
    eit = globdat.dom2BndElems.end   ();

    for ( ; it != eit; ++it )
    {
      int       domName = it->first; 
      IntVector elems   = it->second; 

      //file << "<ElementGroup name=\"" << domName << "\">\n{";
      file << "<ElementGroup name=" << globdat.physicalNames[domName] << ">\n{";
      
      for ( int ie = 0; ie < elems.size()-1; ie++ )
      {
        file << elems[ie] + globdat.elemSet[elemCount-1]->getIndex() + 1<< ",";
      }

      file << elems[elems.size()-1] + globdat.elemSet[elemCount-1]->getIndex() + 1<< "}\n" << "</ElementGroup>\n\n";
    }

    cout << "Writing boundary element groups...done!\n\n";
  }
  
  if ( ieCount )
  {
     cout << "Writing interface element groups...\n";

     file << "<ElementGroup name=\"interfaces\"" << ">\n{[";
     file << start1+1 << ":" << start2+1 << "]}\n";
     file << "</ElementGroup>\n\n";
     
     it  = globdat.mat2InterfaceElems.begin ();
     eit = globdat.mat2InterfaceElems.end   ();

     for ( ; it != eit; ++it )
     {
       int       domName = it->first; 
       IntVector elems   = it->second; 

       file << "<ElementGroup name=\"" << domName << "\">\n{";
       
       for ( int ie = 0; ie < elems.size()-1; ie++ )
       {
         file << elems[ie] + start1 + 1 << ",";
       }

       file << elems[elems.size()-1] + start1 + 1 << "}\n" << "</ElementGroup>\n\n";
     }

     cout << "Writing interface element groups...done!\n\n";
     
     cout << "Writing element database for neighouring elements... \n";

     file << "<ElementDatabase name = \"neighbours\">\n";
     file << "<Column name = \"connect\" type = \"int\">\n";
     
     for ( int ie = 0; ie < ieCount; ie++ )
     {
       ep = globdat.interfaceSet[ie];
       
       file << start1+ie+1 << " " 
            << ep->getBulk1() << " " 
            << ep->getBulk2() << ";\n";
     }

     file << "</Column>\n";
     file << "</ElementDatabase>\n\n";

     cout << "Writing element database for neighouring elements... done!\n\n";
  }

  cout << "Writing node groups...\n";

  if ( !globdat.isNeper ){

  // if only converting mesh to jem/jive, simply write the node groups
  // else, write the nodes in the group and their duplicated nodes as well
  // doing so faciliate Dirichlet boundary conditions handing in FEM model.

  if ( globdat.isConverter )
  {
    Int2IntSetMap::iterator sit  = globdat.bndNodesMap.begin ();
    Int2IntSetMap::iterator seit = globdat.bndNodesMap.end   ();

    for ( ; sit != seit; ++sit )
    {
      int       domName = sit->first; 
      IntSet    elems   = sit->second; 

      IntVector   nodes;
      copy ( elems.begin(), elems.end(), back_inserter(nodes) );
      
      int       nodeCount = nodes.size( ) - 1;

      file << "<NodeGroup name=" << globdat.physicalNames[domName]  << ">\n{";
      
      for ( int ie = 0; ie < nodeCount; ie++ )
      {
	    file << nodes[ie] << ",";
      }

      file << nodes[nodeCount] << "}\n" << "</NodeGroup>\n\n";
    }
  }
  else
  {
    IntVector dupNodes;

    int nodeCount, dupCount;

    Int2IntSetMap::iterator sit  = globdat.bndNodesMap.begin ();
    Int2IntSetMap::iterator seit = globdat.bndNodesMap.end   ();

    for ( ; sit != seit; ++sit )
    {
      int       domName = sit->first; 
      IntSet    elems   = sit->second; 

      IntVector   nodes;
      copy ( elems.begin(), elems.end(), back_inserter(nodes) );
      
      nodeCount = nodes.size( ) - 1;

      file << "<NodeGroup name=\"" << domName << "\">\n{";
      
      for ( int ie = 0; ie < nodeCount; ie++ )
      {
	    file << nodes[ie] << ","; // original node also present in dupNodes
    
        dupNodes = globdat.duplicatedNodes[nodes[ie]];

        dupCount = dupNodes.size ( );
    
        if ( dupCount != 0 )
        {
          copy ( dupNodes.begin()+1,
	             dupNodes.end  (), 
	             ostream_iterator<int> ( file, ", " ) 
	       );
        }
      }

      dupNodes = globdat.duplicatedNodes[nodes[nodeCount]];
      dupCount = dupNodes.size ( );
    
      if ( dupCount != 0 )
      {
        copy ( dupNodes.begin()+1,
	           dupNodes.end  (), 
	           ostream_iterator<int> ( file, ", " ) 
	     );
      }
      
      file << nodes[nodeCount] << "}\n" << "</NodeGroup>\n\n";
    }
  }

  const int isoNodeCount = globdat.isolatedNodes.size ();

  if ( isoNodeCount != 0 )
  {
    for ( int in = 0; in < isoNodeCount; ++in )
    {

      file << "<NodeGroup name=\"" << 999 << "\">\n{"
           << globdat.isolatedNodes[in] << "}\n"
	   << "</NodeGroup>\n\n";
    }
  }
  }

  // If the mesh is created using Neper then have to define the boundary nodes
  // manually as Neper did not do that. Assume that using Neper to build 
  // micro samples with simple geometries: rectangles in 2D and boxes in 3D.

  if ( globdat.isNeper )
  {
    NodePointer np;

    int             id;
    double          x,y;
    IntVector       lower, right, upper, left;

    for ( int in = 0; in < nodeCount; in ++ )
    {
      np = globdat.newNodeSet[in];
 
      id = np->getIndex();
      x  = np->getX    (); 
      y  = np->getY    ();

      if ( abs ( x - globdat.xMin ) < 1e-16 ) left .push_back ( id );
      if ( abs ( x - globdat.xMax ) < 1e-16 ) right.push_back ( id );
      if ( abs ( y - globdat.yMax ) < 1e-16 ) upper.push_back ( id );
      if ( abs ( y - globdat.yMin ) < 1e-16 ) lower.push_back ( id );
    }

    const int leftNodeCount  = left .size ();
    const int rightNodeCount = right.size ();
    const int upperNodeCount = upper.size ();
    const int lowerNodeCount = lower.size ();
    
    file << "<NodeGroup name=\"" << "left" << "\">\n{";

    for ( int in = 0; in < leftNodeCount-1; ++in )
    {
	  file << left[in] << ",";
    }

    file << left[leftNodeCount-1] << "}\n" << "</NodeGroup>\n\n";
    
    file << "<NodeGroup name=\"" << "right" << "\">\n{";

    for ( int in = 0; in < rightNodeCount-1; ++in )
    {
	  file << right[in] << ",";
    }

    file << right[rightNodeCount-1] << "}\n" << "</NodeGroup>\n\n";

    file << "<NodeGroup name=\"" << "upper" << "\">\n{";

    for ( int in = 0; in < upperNodeCount-1; ++in )
    {
	  file << upper[in] << ",";
    }

    file << upper[upperNodeCount-1] << "}\n" << "</NodeGroup>\n\n";

    file << "<NodeGroup name=\"" << "lower" << "\">\n{";

    for ( int in = 0; in < lowerNodeCount-1; ++in )
    {
	  file << lower[in] << ",";
    }

    file << lower[lowerNodeCount-1] << "}\n" << "</NodeGroup>\n\n";
  }

  cout << "Writing node groups...done!\n\n";
}

