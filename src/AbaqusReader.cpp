#include "Global.h"
#include "Node.h"
#include "Element.h"


#include <boost/algorithm/string.hpp>


void                     readAbaqusMesh 

    ( Global&     globdat,
      const char* fileName )
{

  using boost::trim;
  using boost::erase_all;

  ifstream file ( fileName, std::ios::in );

  if ( !file ) 
  {
    cout << "Unable to open Abqus input file!!!\n\n";
    exit(1);
  }
  
  cout << "Reading Abaqus input file ...\n";

  int             id, ie;
  int             length;
  int             matId(0);
  int             elemType, paraElemType;

  double          x,y,z(0.);

  StrVector       splitLine;
  IntVector       connectivity;

  string          line, sub, type;
  bool            read(true);  
    
  getline ( file, line );
  while (read)
  {
    getline ( file, line );
    cout << line << "\n";

    if ( line.compare("*Node") == 0 ) read = false;
  }

  read = true;

  // READING NODE COORDINATES...

  while (read)
  {
    getline ( file, line );
    if ( line[1] == 'E' ) break;
    boost::split ( splitLine, line, boost::is_any_of(",") );
    length       = splitLine.size ();
   
    /*
    cout << length << "\n";
    */
    trim ( splitLine[0] );
    trim ( splitLine[1] );
    trim ( splitLine[2] );
    
    id     = boost::lexical_cast<int>    ( splitLine[0] );
    x      = boost::lexical_cast<double> ( splitLine[1] );
    y      = boost::lexical_cast<double> ( splitLine[2] );

    if ( length == 4 ){
      z      = boost::lexical_cast<double> ( splitLine[3] );
      globdat.is3D = true;
    }
    
    globdat.nodeSet.push_back ( NodePointer( new Node(x,y,z,id) ) );

    globdat.nodeId2Position[id] = globdat.nodeSet.size()-1;

  }

  cout << "Reading node coordinates done!\n";

  
  // READING ELEMENTS


  erase_all ( line, " ");
  boost::split ( splitLine, line, boost::is_any_of(",") );
  boost::split ( splitLine, splitLine[1], boost::is_any_of("=") );

  type = splitLine[1];

  if ( ( type == "CPS4" ) ||  ( type == "CPS4R" )  ||
       ( type == "CPE4" ) ||  ( type == "CPE4R" ) )
  {
       elemType = 3;
  }
  else if ( ( type == "CPS3" ) ||  ( type == "CPE3" ) )
  {
       elemType = 2;
  }

  cout << elemType << endl;

  while (read)
  {
    getline ( file, line );
    if ( line[0] == '*' ) break;
    erase_all ( line, " ");
    boost::split ( splitLine, line, boost::is_any_of(",") );
    length       = splitLine.size ();
   
    /*
    cout << length << "\n";
    cout << splitLine[0] << "\n";
    cout << splitLine[1] << "\n";
    cout << splitLine[2] << "\n";
    */
  
    ie     = boost::lexical_cast<int>    ( splitLine[0] );
    
    connectivity.clear ();

    transform ( splitLine.begin()+1, 
	            splitLine.end  (), 
        		back_inserter(connectivity), 
		        Str2IntFunctor() );

    globdat.elemSet.push_back ( ElemPointer ( new Element ( ie, elemType, connectivity ) )  );

    globdat.elemId2Position[ie] = globdat.elemSet.size() - 1;

  }
 
  cout << "Reading elements done!\n";

  // READING NODE and ELEMENT SETs

  while (read)
  {
    getline ( file, line );
    cout << line << "\n";
    if ( line[1] == 'E' ) break;
  }

  int startElem, endinElem, increment;
  
  while (read)
  {
    erase_all ( line, " ");
    boost::split ( splitLine, line, boost::is_any_of(",") );

    if ( splitLine[splitLine.size()-1] == "generate" )
    {
       getline ( file, line );
       erase_all ( line, " ");
       boost::split ( splitLine, line, boost::is_any_of(",") );
    
       // material => elements
       // element  => material (domain)

       startElem =  boost::lexical_cast<int>    ( splitLine[0] );
       endinElem =  boost::lexical_cast<int>    ( splitLine[1] );
       increment =  boost::lexical_cast<int>    ( splitLine[2] );

       for ( int id = startElem; id < endinElem; id += increment )
       {
         globdat.dom2Elems[matId].push_back ( id );
         globdat.elem2Domain[id] = matId;
       }
      
       matId++;
    }

    cout << line << "\n";
    getline ( file, line );

    if ( line[1] == 'S' ) break;
  }
  
  file.close ();

  // DEBUGGING...

  const int   elemCount = globdat.elemSet.size ();
        int   inodeCount;
  IntVector   inodes;
  ElemPointer ep;
  for ( int ie = 0; ie < elemCount; ie++ )
  {
    ep = globdat.elemSet[ie];
    ep->getConnectivity ( inodes );
    inodeCount = inodes.size ();
    print (inodes.begin(), inodes.end());  
  }
  
  // CHECKING ...

  string elemTypeStr = "linear";

  if ( globdat.elemSet.size() == 0 )
  {
    cout << "There is no solid elements defined!!!\n";
    cout << "Please double check your input mesh.\n";
    exit(-1);
  }
  
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
    //@todo: adapt to Abaqus, this is copied from Gmsh code
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
  }
}
