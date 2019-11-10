/**
 * This program reads a mesh and generates interface elements 
 * that are inserted in between all element boundaries. 
 * It is also able not to create interface elements in some zones
 * if that is what users need, say for hard elastic inclusions.
 *
 * Interface elements supported:
 *
 *  (1) Two dimensions:   4 node and 6 node interface elements
 *  (2) Three dimensions: 
 *         8 node  interface element (pair of Q4 elements)
 *         16 node interface element (pair of Q8 elements)
 *         6 node  interface element (pair of T3 elements)
 *         12 node interface element (pair of T6 elements)
 *
 *  (3) Discrete interface elements (spring elements) two nodes in 2D.
 *      These elements are used in a Discrete Damage Model (Waisman, EFM 2012)
 *  
 *  (4) BSplines elements: 2D cubic elements (8 node interface elements),
 *      The mesh file is generated using a Matlab code with extension *.nurbs.
 *
 * Attention:
 *
 *  Be careful with the outward normal vector of the cohesive surface !!!
 *
 * Capacities:
 *
 *   (1) limit the interface elements in preferable domains.
 *   (2) limit the interface elements on material interfaces only
 *   (3) interface elements are inserted everywhere if needed.
 *   (4) Can differentiate interface elements along material interface
 *       and interface elements inserted in the bulk. Mat id for interface
 *       element is ONE and for bulk interface element is ZERO ...
 *   (5) Can read different mesh formats, currently Gmsh is supported
 *       but other format can be added with ease.
 *   (6) Can generate interface elements along intergranular boundaries
 *       of polycrystalline structure. This is done by considering
 *       intergranular boundary is a material interface. So, in the mesh
 *       of the polycrystal, every cell belongs to different domains.
 *   (7) Able to duplicate node along existing notches but
 *       do not add interface elements along this notches (traction-free cracks).
 *   (8) Both linear (4 node) and quadratic (6 node) interface elements  are supported.
 *   (9) Mesh converter: read Gmsh and convert it to jem/jive mesh.
 *   (10)Write to Abaqus (inp) files.
 *
 *  Algorithm used:
 *
 *    (1) Duplicate nodes that are shared by more than 2 elements
 *    (2) Modify element connectivities 
 *    (3) Write nodes(old+new) and modified elements to file *.mesh (currently only jem/jive mesh
 *        format is supported)
 *        which is then read by FEM programs to build solid elements.
 *    (4) The interface elements are then generated and written to a separate file.
 *
 * Usage:
 *
 *  ./interface-mesh-generator --help
 *
 * Author: V.P. Nguyen, 
 *         V.P.Nguyen@tudelft.nl
 *         nvinhphu@gmail.com
 *
 * Date:   30 June 2009
 */


#include "Node.h"
#include "Element.h"
#include "Global.h"
#include "utilities.h"
#include "MeshModifier.h"
#include "InterfaceBuilder.h"
#include "MeshWriter.h"
#include "MeshReader.h"
#include "InterfaceWriter.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>



// =====================================================================
//     main program
// =====================================================================
//

int main ( int argc, char* argv[] )
{
  Global   globdat;

  // reading input

  string   meshFile      ("");
  string   newMeshFile   ("");
  string   interfaceFile ("");
  string   paraviewFile  ("");

  bool     gotMeshFile  = false;
  bool     gotnMeshFile = false;
  bool     gotiMeshFile = false;
  bool     gotParaFile  = false;

  for ( size_t i = 1; i < argc; i++ )
  {
    if       ( string(argv[i]) == string("--mesh-file")      )
    {
      meshFile    = argv[++i];
      gotMeshFile = true;
    }
    else if  ( string(argv[i]) == string("--out-file")       )
    {
      newMeshFile  = argv[++i];
      gotnMeshFile = true;
  
      StrVector filenames;

      boost::split ( filenames, newMeshFile, boost::is_any_of(".") );

      if ( filenames[1] == "inp" ) globdat.outAbaqus=true; 
    }
    else if  ( string(argv[i]) == string("--interface-file") )
    {
      interfaceFile = argv[++i];
      gotiMeshFile  = true;
    }
    else if  ( string(argv[i]) == string("--paraview-file") )
    {
      paraviewFile = argv[++i];
      gotParaFile  = true;
    }
    else if  ( string(argv[i]) == string("--interface") )
    {
      globdat.isInterface   = true;
      globdat.isEveryWhere  = false;
    }
    else if  ( string(argv[i]) == string("--domain") )
    {
      globdat.isInterface   = false;
      globdat.isDomain      = true;
      globdat.isEveryWhere  = false;

      globdat.rigidDomain   = boost::lexical_cast<int> ( argv[++i] );
    }   
    else if  ( string(argv[i]) == string("--polycrystal") )
    {
      globdat.isPolycrystal = true;
      globdat.isEveryWhere  = false;
    }
    else if  ( string(argv[i]) == string("--everywhere") )
    {
      globdat.isPolycrystal = false;
      globdat.isInterface   = false;
      globdat.isDomain      = false;
      globdat.isEveryWhere  = true;
    }
    else if  ( string(argv[i]) == string("--notch") )
    {
      globdat.isNotch   = true;

      double x1 = boost::lexical_cast<double> ( argv[++i] );
      double y1 = boost::lexical_cast<double> ( argv[++i] );
      double x2 = boost::lexical_cast<double> ( argv[++i] );
      double y2 = boost::lexical_cast<double> ( argv[++i] );

      Segment s ( Point(x1,y1), Point(x2,y2) );

      globdat.segment.push_back ( s );
    }   
    else if  ( string(argv[i]) == string("--notches") )
    {
      globdat.isNotch   = true;

      double x1 = boost::lexical_cast<double> ( argv[++i] );
      double y1 = boost::lexical_cast<double> ( argv[++i] );
      double x2 = boost::lexical_cast<double> ( argv[++i] );
      double y2 = boost::lexical_cast<double> ( argv[++i] );

      double x3 = boost::lexical_cast<double> ( argv[++i] );
      double y3 = boost::lexical_cast<double> ( argv[++i] );
      double x4 = boost::lexical_cast<double> ( argv[++i] );
      double y4 = boost::lexical_cast<double> ( argv[++i] );

      Segment s1 ( Point(x1,y1), Point(x2,y2) );
      Segment s2 ( Point(x3,y3), Point(x4,y4) );

      globdat.segment.push_back ( s1 );
      globdat.segment.push_back ( s2 );
    }   
    else if  ( string(argv[i]) == string("--noInterface") )
    {
      globdat.isIgSegment   = true;

      double x1 = boost::lexical_cast<double> ( argv[++i] );
      double y1 = boost::lexical_cast<double> ( argv[++i] );
      double x2 = boost::lexical_cast<double> ( argv[++i] );
      double y2 = boost::lexical_cast<double> ( argv[++i] );

      Segment s ( Point(x1,y1), Point(x2,y2) );
      globdat.ignoredSegment = s;
    }  
    else if  ( string(argv[i]) == string("--isContinuum") )
    {
      globdat.isContinuum = boost::lexical_cast<bool> ( argv[++i] );
    }
    else if  ( string(argv[i]) == string("--converter") )
    {
      globdat.isConverter = true;
    }
    else if  ( string(argv[i]) == string("--help") )
    {
      cout << "USAGE:\n";
      cout << "  * --mesh-file      FILE         set the file containing the mesh\n";
      cout << "  * --out-file       FILE         set the file containing the modified mesh\n";
      cout << "  * --isContinuum    1 or 0       continuum interface elements or discrete elements\n";
      cout << "  * --interface-file FILE         set the file containing the interface mesh\n";
      cout << "  * --paraview-file  FILE         set the file of ParaView format\n";
      cout << "  * --interface                   generate interface elements along material interface\n";
      cout << "  * --everywhere                  generate interface elements at all interelement boundaries\n";
      cout << "  * --domain         domNum       not generate interface elements in domain number domNum\n";
      cout << "  * --polycrystal                 generate interface elements along intergranular boundaries\n";
      cout << "  * --notch          x1 y1 x2 y2  existing notch(duplicate nodes but no interface there)\n";
      cout << "  * --notches        x1 y1 x2 y2 x3 y3 ... existing notch(duplicate nodes but no interface there)\n";
      cout << "  * --noInterface    x1 y1 x2 y2  no duplicated nodes, no interface elements along this line\n";
      cout << "  * --converter                   convert Gmsh to jem/jive format (no interface elements)\n";
      cout << "  * --help                        print this help and exit\n";
      cout << endl;
      return 0 ;
    }
    else
    {
      cerr << "invalid argument, use \"--help\" for more information\n";
      return 1;
    }
  }

  if ( ! gotMeshFile )
  {
    cout << "please enter mesh file:" << flush;
    cin  >> meshFile;
  }

  if ( ! gotnMeshFile )
  {
    cout << "using default name for the output file.\n" << flush;

    StrVector spMeshFile;
    boost::split ( spMeshFile, meshFile, boost::is_any_of(".") );
    newMeshFile = spMeshFile[0] + "-interface-solid.mesh";
  }

  if ( ! gotiMeshFile )
  {
    cout << "using default name for the interface file.\n" << flush;
    StrVector spMeshFile;
    boost::split ( spMeshFile, meshFile, boost::is_any_of(".") );
    interfaceFile = spMeshFile[0] + "-interface.mesh";
  }

  if ( ! gotParaFile )
  {
    cout << "using default name for the ParaView file.\n\n" << flush;
    StrVector spMeshFile;
    boost::split ( spMeshFile, meshFile, boost::is_any_of(".") );
    paraviewFile = spMeshFile[0] + ".vtu";
  }

  // doing stuff 

  readMesh               ( globdat, meshFile.c_str()   );
  MeshModifier::    doIt ( globdat                     );
  InterfaceBuilder::doIt ( globdat                     );

  writeMesh              ( globdat, newMeshFile.c_str());
  writeInterface         ( globdat, interfaceFile.c_str() );

  return 0;
}
