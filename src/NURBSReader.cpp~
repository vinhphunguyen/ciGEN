#include "Global.h"
#include "Node.h"
#include "Element.h"

/*
 * This can be used to read a mesh of high order Bezier elements created in Matlab.
 * It can also be used to read a standard mesh created in Matlab. The file format is as follows

 * NumberOfNodess 5047 NumberOfElements 720
 * NODES
 * 1 0.000000  0
 * 2 0.166667  0
 * ELEMENTS
 * node1 node2 node3
 * node1 node2 node3
 * MATERIAL ID
 * elemID matID
 * NODE GROUPS
 *
 * VP Nguyen, nvinhphu@gmail.com
 * Written for the paper "High-order B-splines cohesive interface elements for delamination analysis"
 * while working at Ton Duc Thang university, June 2012.
 */


#include <boost/algorithm/string.hpp>


void                     readNURBSMesh 

    ( Global&     globdat,
      const char* fileName )
{
  ifstream file ( fileName, std::ios::in );

  if ( !file ) 
  {
    cout << "Unable to open mesh file!!!\n\n";
    exit(1);
  }

  int             id;
  int             length;
  int             matId;
  int             elemType, paraElemType;

  double          x,y,z(0);

  StrVector       splitLine;
  IntVector       connectivity;

  string  line;

  cout << "Reading NURBS  mesh file ...\n";
  cout << "Reading nodes...\n";

  getline ( file, line );
  boost::split ( splitLine, line, boost::is_any_of("\t ") );

  int nodeCount  = boost::lexical_cast<int> ( splitLine[1] );
  int elemCount  = boost::lexical_cast<int> ( splitLine[3] );	

  getline ( file, line );

  for ( int in = 0; in < nodeCount; in++ )
  {
    file >> id >> x >> y;

    globdat.nodeSet.push_back ( NodePointer( new Node(x,y,z,id) ) );

    globdat.nodeId2Position[id] = globdat.nodeSet.size()-1;
  }

  // checking two or three dimensional mesh

  for ( int in = 0; in < nodeCount; in++ )
  {
    if ( globdat.nodeSet[in]->getZ() != 0. )
    {
      globdat.is3D = true;
      break;
    }
  }

  cout << "Reading nodes...done!\n\n";
  cout << "Reading elements...\n";

  getline ( file, line );
  getline ( file, line );

  for ( int ie = 0; ie < elemCount; ie++ )
  {
    getline ( file, line );

    boost::split ( splitLine, line, boost::is_any_of("\t ") );

    // read connectivity 

    connectivity.clear ();

    transform ( splitLine.begin(), 
	        splitLine.end  ()-1, 
		back_inserter(connectivity), 
		Str2IntFunctor() );

    // define element type using Gmsh format
    // defined only for cases where Gmsh cannot make such meshes:
    // cross triangle mesh for example.

    length = connectivity.size();

    switch (length)
    {
       case 3: elemType = 2; break;     // three node triangle elements
       case 6: elemType = 9; break;     // six node triangle elements
    }

    globdat.elemSet.push_back ( ElemPointer ( new Element ( ie, elemType, connectivity, true ) )  );

    globdat.elemId2Position[ie] = globdat.elemSet.size() - 1;
  }

  cout << "Reading elements...done!\n\n";
  cout << "Reading materials...\n";

  // material => elements
  // element  => material (domain)
  
  getline ( file, line );
  
  for ( int ie = 0; ie < elemCount; ie++ )
  {
    getline ( file, line );

    boost::split ( splitLine, line, boost::is_any_of("\t ") );

    matId  = boost::lexical_cast<int> ( splitLine[1] );

    globdat.dom2Elems[matId].push_back ( ie );
    globdat.elem2Domain[ie] = matId;
  }

  cout << "Reading materials...done!\n\n";

  cout << "Reading node groups...\n";

  getline ( file, line );

  boost::split ( splitLine, line, boost::is_any_of("\t ") );

  int nodeGroupCount  = boost::lexical_cast<int> ( splitLine[2] );

  for (int ing = 0; ing < nodeGroupCount; ing++ )
  {
    getline ( file, line );
    getline ( file, line );
      
    boost::split ( splitLine, line, boost::is_any_of("\t ") );

    connectivity.clear ();

    transform ( splitLine.begin(), 
                splitLine.end  ()-1, 
          	back_inserter(connectivity), 
          	Str2IntFunctor() );
    
    for ( int ii = 0; ii < connectivity.size(); ii++)
    {
      globdat.boundaryNodes.insert ( connectivity[ii] );
    }

    for ( int ii = 0; ii < connectivity.size(); ii++)
    {
      globdat.bndNodesMap[ing].insert ( connectivity[ii] );
    }
  }

  cout << "Reading node groups...done!\n\n";

  file.close ();

  // check validity of input

  if ( globdat.isDomain )
  {
    Int2IntVectMap::const_iterator it;
    Int2IntVectMap::const_iterator eit = globdat.dom2Elems.end   ();

    it = globdat.dom2Elems.find ( globdat.rigidDomain );

    if ( it == eit )
    {
      cerr << "invalid number of rigid domain!!!\n";
      exit(1);
    }
  }

  if ( globdat.isInterface )
  {
  }

  if ( globdat.isNotch )
  {
    cout << "Existing notch segment is: " ;//<< globdat.segment << endl;
  }

  if ( globdat.isIgSegment )
  {
    cout << "Do not treat nodes on this segment: " << globdat.ignoredSegment << endl;
  }

  cout << endl;

  globdat.nodeElemCount = connectivity.size ();

  string elemTypeStr = "linear";

  elemType = globdat.elemSet[0]->getElemType ();

  cout << elemType << "\n";

  if ( !globdat.is3D )                      // 2D mesh
  {
    if ( elemType == 2 || elemType == 3 )
    {
      globdat.nodeICount  = globdat.isContinuum ? 4 : 2;
      globdat.isQuadratic = false;
      elemTypeStr         = "linear";
    }
    else
    {
      globdat.nodeICount  = 6;
      globdat.isQuadratic = true;
      elemTypeStr         = "quadratic";
    }
  }
  else                                     // 3D mesh
  {
    if ( elemType == 4 || elemType == 5 ) 
    {
      globdat.nodeICount  = elemType == 4 ? 6 : 8;
      globdat.isQuadratic = false; 
      elemTypeStr = "linear";
    }
    else
    {
      globdat.nodeICount  = elemType == 11 ? 12 : 16;
      globdat.isQuadratic = true;
      elemTypeStr = "quadratic";
    }

    for ( int ie = 0; ie < globdat.elemSet.size (); ie++ )
    {
      globdat.elemSet[ie]->buildFaces ();
    }
  }


}
