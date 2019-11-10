#include "Global.h"
#include "Node.h"
#include "Element.h"


#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

/*
 * 8 October 2014: when isConverted == true, write
 * element sets for boundaries (so that external force can be applied on them)
 * todo: only 2D boundary element set.
 * 22 June 2015: 3D boundary element set.
 * 
 * Typical Gmsh mesh file reads, if you name physical quantities by string not number!!!
 *      $MeshFormat
 *      2.2 0 8
 *      $EndMeshFormat
 *      $PhysicalNames
        4
        0 3 "loadPnt"
        1 4 "fixedPnts"
        2 1 "elastic"
        2 2 "damage"
        $EndPhysicalNames
 *      $Nodes
 *      80000 
 *      1 0 0 0
 *      2 1 0 0
 *       .
 *       .
 *       .
 *     $EndNodes
 *     $Elements
 *     40397
 *     1 1 2 333 1 1 9     ->  elemType     = boost::lexical_cast<int> ( splitLine[1] );
                               matId        = boost::lexical_cast<int> ( splitLine[3] ); 
                               geoId        = boost::lexical_cast<int> ( splitLine[4] ); 
 *     2 1 2 333 1 9 10
 *     3 1 2 333 1 10 11
 *     $EndElements
 *
 * Gmsh msh, elemet type code:
 *  elem type =15: 1 node point
 *
 *  elem type = 1: 2 node line element
 *  elem type = 8: 3 node line element
 *
 *  elem type = 2: 3 node triangle element
 *  elem type = 9: 6 node triangle element
 *
 *  elem type = 3: 4 node quadrilateral element
 *  elem type =10: 9 node quadrilateral element
 *  elem type =16: 8 node quadrilateral element
 *
 *  elem type = 4: 4 node tetrahedron element
 *  elem type =11: 10 node tetrahedron element
 *
 *  elem type = 5: 8 node hexahedron element
 *  elem type =12: 27 node hexahedron element
 *
 * 11 July 2015, VP Nguyen:
 * Neper (Polycrystals generator software) generates Gmsh mesh files like this
 * If Neper used Gmsh mesh format 2.0.8, then open the msh file with gmsh to
 * save it using 2.2 0 8 format.
 * 100 2 4 0 1 node1 node2 node3   => element "100" belongs to grain 1
 * 101 2 4 0 1 node1 node2 node3   => element "101" belongs to grain 1
 * 102 2 4 0 2 node1 node2 node    => element "102" belongs to grain 2
 * 103 2 4 0 2 node1 node2 node    => element "103" belongs to grain 2
 * Unfortunately, the information at column 4 (0 is the fitrst column) was not
 * used in the code. It is the geometrical entity ID.
 *
 * Support only 2.2 0 8 Gmsh ASCII format
 */

/*
 * 2 Feb 2019: now in gmsh, you have to use string to label physical groups, like this
 *          Physical Surface("elastic") = {16, 20}; // elastic
            Physical Surface("damage") = {18}; // damage
            Physical Point("loadPnt")  = {6}; // load;            
            Physical Line("fixedPnts") = {1}; // fixed;
    Not back compatible with old msh files where physical groups are labeled by integers!!!            
 */

void                     readGmshMesh 

    ( Global&     globdat,
      const char* fileName )
{
  ifstream file ( fileName, std::ios::in );

  if ( !file ) 
  {
    cout << "Unable to open mesh file!!!\n\n";
    exit(1);
  }

  int             id, idx;
  int             length;
  int             matId;
  int             geoId;    // geometrical entity id (Neper uses this)
  int             elemType, paraElemType;
  int             bieCount(0);

  double          x,y,z;
  double          x1,y1,z1;
  double          x2,y2,z2;

  StrVector       splitLine;
  IntVector       connectivity;
  IntVector       bndElemConn;

  string          line;
  string          name;

  cout << "Reading Gmsh mesh file ...\n";
  cout << "Reading physical names...\n";

  getline ( file, line );
  getline ( file, line );
  getline ( file, line );
  getline ( file, line );

  getline ( file, line );

  const int namesCount = boost::lexical_cast<int> ( line );

  for ( int in = 0; in < namesCount; in++ )
  {
    file >> id >> idx >> name;
    //getline ( file, line );
    //cout << line << "\n";
    globdat.physicalNames[idx] = name;
    //cout << name << "\n";
  }

  cout << "Reading physical names...done!\n";


  cout << "Reading nodes...\n";

  getline ( file, line );
  getline ( file, line );
  getline ( file, line );
  getline ( file, line );
  

  const int nodeCount = boost::lexical_cast<int> ( line );

  for ( int in = 0; in < nodeCount; in++ )
  {
    file >> id >> x >> y >> z;

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
          
  globdat.getBounds ();

  cout << globdat.xMin << " " << globdat.xMax << endl;
  cout << globdat.yMin << " " << globdat.yMax << endl;
  cout << globdat.zMin << " " << globdat.zMax << endl;

  cout << "Reading elements...\n";

  getline ( file, line );
  getline ( file, line );
  getline ( file, line );

  getline ( file, line ); // -> reading the line containing the number of elements

  const int elemCount = boost::lexical_cast<int> ( line );

  for ( int ie = 0; ie < elemCount; ie++ )
  {
    getline ( file, line );

    boost::split ( splitLine, line, boost::is_any_of("\t ") );

    length       = splitLine.size ();

    elemType     = boost::lexical_cast<int> ( splitLine[1] );
    matId        = boost::lexical_cast<int> ( splitLine[3] );	
    geoId        = boost::lexical_cast<int> ( splitLine[4] );	

    // Neper mesh : matId = 0, then use geoId instead

    //cout << matId << " " << geoId << endl;
    if ( matId == 0 ) matId = geoId;

    // line elements => boundary nodes
    // elemType == 1: two-node   line element
    // elemType == 8: three-node line element

    if ( elemType == 1 ) 
    {
      int no1 = boost::lexical_cast<int> ( splitLine[5] );
      int no2 = boost::lexical_cast<int> ( splitLine[6] );

      // If the mesh was created with Neper, then there is a problem her
      // The edges of the grains are written as well.
      // And they are definitely not boundary edges. 
      // Need to remove them.

      if ( globdat.isNeper )
      {
        x1 = globdat.nodeSet[globdat.nodeId2Position[no1]]->getX ( );     
        y1 = globdat.nodeSet[globdat.nodeId2Position[no1]]->getY ( );     

        x2 = globdat.nodeSet[globdat.nodeId2Position[no2]]->getX ( );     
        y2 = globdat.nodeSet[globdat.nodeId2Position[no2]]->getY ( );     
        
        bool xx1 = ( abs(x1-globdat.xMin) > 1e-12 ) && ( abs(x1-globdat.xMax) > 1e-12 ); 
        bool yy1 = ( abs(y1-globdat.yMin) > 1e-12 ) && ( abs(y1-globdat.yMax) > 1e-12 ); 

        bool xx2 = ( abs(x2-globdat.xMin) > 1e-12 ) && ( abs(x2-globdat.xMax) > 1e-12 ); 
        bool yy2 = ( abs(y2-globdat.yMin) > 1e-12 ) && ( abs(y2-globdat.yMax) > 1e-12 ); 

        if ( ( xx1 && yy1 ) || ( xx2 & yy2 ) )
        {
          //cout << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
          //cout << "One wrong boundary edge detected.\n\n";  
          continue;
        }
      }

      globdat.boundaryNodes.insert ( no1 );
      globdat.boundaryNodes.insert ( no2 );

      globdat.bndNodesMap[matId].insert ( no1 );
      globdat.bndNodesMap[matId].insert ( no2 );

      // only when this line element does not belong to
      // the internal edges, then add nodepairs.
      // because if you added nodepairs here no interface elements would be
      // generated!!!
      

      if ( globdat.internalEdges[0] != matId )
      {
        globdat.nodePairs     .push_back ( NodePair (no1, no2) );
        globdat.bndElemsDomain.push_back ( matId );
        cout << matId << "\n";
      }


      if ( globdat.isConverter )
      {
          bndElemConn.resize ( 2 );
          bndElemConn[0] = no1;
          bndElemConn[1] = no2;
    
          globdat.bndElementSet.push_back ( ElemPointer ( new Element ( bieCount, elemType, bndElemConn ) ) );
          globdat.dom2BndElems[matId].push_back ( bieCount++ );
      }

      continue;
    }

    // 3-node line elements

    if ( elemType == 8 )
    {
      int no1 = boost::lexical_cast<int> ( splitLine[5] );
      int no2 = boost::lexical_cast<int> ( splitLine[6] );
      int no3 = boost::lexical_cast<int> ( splitLine[7] ); //midside node

      globdat.boundaryNodes.insert ( no1 );
      globdat.boundaryNodes.insert ( no2 );
      globdat.boundaryNodes.insert ( no3 );

      globdat.bndNodesMap[matId].insert ( no1 );
      globdat.bndNodesMap[matId].insert ( no2 );
      globdat.bndNodesMap[matId].insert ( no3 );
      
      if ( globdat.internalEdges[0] != matId )
      {
        globdat.nodePairs     .push_back ( NodePair (no1, no2) );
        globdat.bndElemsDomain.push_back ( matId );
        cout << matId << "\n";
      }

      if ( globdat.isConverter )
      {
          bndElemConn.resize ( 3 );
          bndElemConn[0] = no1;
          bndElemConn[1] = no3;
          bndElemConn[2] = no2;

          //print ( bndElemConn.begin(), bndElemConn.end() );
    
          globdat.bndElementSet.push_back ( ElemPointer ( new Element ( bieCount,elemType,  bndElemConn ) ) );
          globdat.dom2BndElems[matId].push_back ( bieCount++ );
      }

      continue;
    }

    // node element 

    if ( elemType == 15 )
    {
      //globdat.isolatedNodes.push_back ( boost::lexical_cast<int> ( splitLine[5] ) );
      //continue;
      
      int no1 = boost::lexical_cast<int> ( splitLine[5] );

      globdat.boundaryNodes.     insert ( no1 );      
      globdat.bndNodesMap[matId].insert ( no1 );

      continue;
    }

    // for a 3D mesh, triangles or quadrangles are surface elements
    // ie. they are boundary elements not bulk elements.

    if ( globdat.is3D )
    {
      if ( elemType == 2 || elemType == 9 || // linear or quadratic triangle
     	   elemType == 3 || elemType == 16 ) // linear or quadratic quadrangle
      {
	    connectivity.clear ();

        transform ( splitLine.begin()+5, 
         	        splitLine.end  (), 
		            back_inserter(connectivity), 
		            Str2IntFunctor() );

        /* to check elements with coincident nodes such as [123 23 23]
         *
        print ( connectivity.begin(), connectivity.end() );
        IntVector copy ( connectivity );

        std::sort ( copy.begin (), copy.end () );
        auto last = std::unique ( copy.begin(), copy.end() );
        copy.erase ( last, copy.end() );

        if ( copy.size() != connectivity.size () ){
          print ( connectivity.begin(), connectivity.end() ); cout << endl;
          exit(1);
        }
        */

        globdat.bndNodesMap[matId].insert ( connectivity.begin(),
	                                          connectivity.end() );
        if ( globdat.isConverter ){
            globdat.bndElementSet.push_back ( ElemPointer ( new Element ( bieCount, elemType, connectivity ) ) );
            globdat.dom2BndElems[matId].push_back ( bieCount++ );
        }
        continue;
      }
    }

    // material => elements
    // element  => material (domain)

    globdat.dom2Elems[matId].push_back ( ie );
    globdat.elem2Domain[ie] = matId;

    // the rest are solid elements 
    // either 2D solid elements or 3D solid elements

    // read connectivity 

    connectivity.clear ();

    transform ( splitLine.begin()+5, 
	            splitLine.end  (), 
        		back_inserter(connectivity), 
		        Str2IntFunctor() );

    globdat.elemSet.push_back ( ElemPointer ( new Element ( ie, elemType, connectivity ) )  );

    globdat.elemId2Position[ie] = globdat.elemSet.size() - 1;
  }

  cout << "Reading elements...done!\n";

  file.close ();

  // check validity of input

  if ( globdat.isDomain )
  {
    Int2IntVectMap::const_iterator it;
    Int2IntVectMap::const_iterator eit = globdat.dom2Elems.end   ();

    it = globdat.dom2Elems.find ( globdat.rigidDomain[0] );

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
    cout << "Existing notch segment is: " << globdat.segment[0] << endl;
  }

  if ( globdat.isIgSegment )
  {
    cout << "Do not treat nodes on this segment: "<< globdat.ignoredSegment << endl;
  }

  cout << endl;

  globdat.nodeElemCount = connectivity.size ();

  string elemTypeStr = "linear";

  if ( globdat.elemSet.size() == 0 )
  {
    cout << "There is no solid elements defined!!!\n";
    cout << "Please double check your input mesh.\n";
    exit(-1);
  }

  elemType = globdat.elemSet[0]->getElemType ();

  if ( !globdat.is3D )
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
  else
  {
    // hex8 or tet4 elements 
    if ( elemType == 4 || elemType == 5 ) 
    {
      globdat.nodeICount  = elemType == 4 ? 6 : 8;
      globdat.isQuadratic = false; 
      elemTypeStr = "linear";
    }
    // hex20 or tet10 elements 
    else
    {
      globdat.nodeICount  = elemType == 11 ? 12 : 16;
      globdat.isQuadratic = true;
      elemTypeStr = "quadratic";
    }

    // build faces for 3D elements
  
    cout << "Building initial faces of 3D elements...\n\n"; 

    for ( int ie = 0; ie < globdat.elemSet.size (); ie++ )
    {
      globdat.elemSet[ie]->buildFaces0 ();
    }
    cout << "Building initial faces of 3D elements...done\n\n"; 
  }
  
  // check boundary elements of a 3D mesh to see whether zero area elements
  // exist
  if ( globdat.is3D )
  {
    const int elemCount    = globdat.elemSet      .size();
    const int bndElemCount = globdat.bndElementSet.size();
  
    ElemPointer ep;
    IntVector connect;

    for ( int ie = 0; ie < bndElemCount; ie++ )
    {
      ep = globdat.bndElementSet[ie];
      ep->getJemConnectivity ( connect ); 

          print ( connect.begin(), connect.end() );
        IntVector copy ( connect);

        std::sort ( copy.begin (), copy.end () );
        auto last = std::unique ( copy.begin(), copy.end() );
        copy.erase ( last, copy.end() );

        if ( copy.size() != connect.size () ){
          print ( connect.begin(), connect.end() );
          exit(1);
        }
    }
  }

  cout << "Check validity of input...done!\n\n";
}
