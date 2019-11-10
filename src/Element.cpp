#include <boost/algorithm/minmax_element.hpp>
#include <algorithm>

#include "Element.h"
#include "Node.h"
#include "Global.h"

/*
 * TODO: set isQuadratic_  
 * */


typedef vector<double>::const_iterator   iter;
typedef vector<int>   ::iterator         iIter;

const std::vector<int> Element::gmshQuadraticElemTypes ({8,9,10,11,12,16});

// ------------------------------------------------------------
//    constructors
// ------------------------------------------------------------

Element::Element ( int index, int elemType,
                   const IntVector& connec )

  : index_(index), elemType_(elemType),
    connectivity_(connec), connectivity0_(connec), 
    done_(false), isQuadratic_(false),
    isChanged_(false), isNURBS_(false)
{
  nodePerFace_ = 3;

  if (       find ( Element::gmshQuadraticElemTypes.begin(), 
                    Element::gmshQuadraticElemTypes.end (), elemType )
             != Element::gmshQuadraticElemTypes.end () 
     ) isQuadratic_ = true;

}

Element::Element ( int index, int elemType,
                   const IntVector& connec, bool isNURBS )

  : index_(index), elemType_(elemType),
    connectivity_(connec), connectivity0_(connec), 
    done_(false), isQuadratic_(false),
    isChanged_(false), isNURBS_(isNURBS)
{
  nodePerFace_ = 3;
}

Element::Element ( int index,
                   const IntVector& connec )

  : index_(index), 
    connectivity_(connec), connectivity0_(connec)
{
  //print ( connectivity_.begin(), connectivity_.end() );
  isQuadratic_ = false;
}


Element::Element ( int index, int elemType,
                   const IntVector& connec, int bulk1, int bulk2 )

  : index_(index), elemType_(elemType),
    connectivity_(connec), connectivity0_(connec), 
    done_(false), isQuadratic_(connec.size() > 4 ? true : false),
    isChanged_(false), bulk1_(bulk1), bulk2_(bulk2)
{
  nodePerFace_ = 3;
}

// ------------------------------------------------------------
//    buildFaces
// ------------------------------------------------------------
//
// attention: the faces must be oriented properly
// so that the normals are outward.

void Element::buildFaces ()
{
  if       ( elemType_ == 4 )   // 4 node tetrahedron
  {
    buildFacesForTet4_ ();
  }
  else if  ( elemType_ == 11 )  // 10 node tetrahedron
  {
    buildFacesForTet10_ ();
  }
  else if  ( elemType_ == 5 )   // 8 node hexahedron
  {
    buildFacesForHex8_ ();
  }
  else if  ( elemType_ == 17 )  // 20 node hexahedron
  {
    buildFacesForHex20_ ();
  }
  else
  {
    cerr << "Unsupported element type\n";
    exit(1);
  }
}

// ------------------------------------------------------------
//    buildFaces0
// ------------------------------------------------------------
//
// attention: the faces must be oriented properly
// so that the normals are outward.

void Element::buildFaces0 ()
{

  if       ( elemType_ == 4 )   // 4 node tetrahedron
  {
    buildFaces0ForTet4_ ();
  }
  else if  ( elemType_ == 11 )  // 10 node tetrahedron
  {
    buildFaces0ForTet10_ ();
  }
  else if  ( elemType_ == 5 )   // 8 node hexahedron
  {
    buildFaces0ForHex8_ ();
  }
  else if  ( elemType_ == 17 )  // 20 node hexahedron
  {
    buildFaces0ForHex20_ ();
  }
  else
  {
    cerr << "Unsupported element type\n";
    exit(1);
  }
}


// ------------------------------------------------------------
//     isOnExternalBoundary
// ------------------------------------------------------------


bool Element::isOnExternalBoundary

   ( int       faceId,
     Global&   globdat ) const
{
  int         nodeCount = faces_[0].size ( );
  int         id, pos, mat;
  //int         count;

  IntVector   vertices(nodeCount);

  vertices = faces0_[faceId];

  // loop over nodes of face jf

  for ( int in = 0; in < nodeCount; in++ )
  {
      id  = vertices[in];
      pos = globdat.nodeId2Position[id];
      mat = globdat.nodeSet[pos]->getDuplicity ();

      // if this node is interfacial

      if ( mat == 0 || mat == 1 ) return true;
  }

  return false;
}

// ------------------------------------------------------------
//     isUniqueFace
// ------------------------------------------------------------


bool Element::isUniqueFace

   ( int       faceId,
     Global&   globdat ) const
{
  int         nodeCount = faces_[0].size ( );
  int         id, pos, mat;
  int         count(0);
  //int         count;

  IntVector   vertices(nodeCount);

  vertices = faces0_[faceId];

  // loop over nodes of face jf

  for ( int in = 0; in < nodeCount; in++ )
  {
      id  = vertices[in];
      pos = globdat.nodeId2Position[id];
      mat = globdat.nodeSet[pos]->getDuplicity ();

      // if this node is interfacial

      if (  mat == 1 ) return true;
  }

  return false;
}

// ------------------------------------------------------------
//     isOnInterface
// ------------------------------------------------------------


bool Element::isOnInterface 

   ( IntVector&        face,
     int&              oppVertex,
     int&              fIndex,
     const NodeSet&    nodeSet,
           Int2IntMap& position,
     const Global&     globdat ) const      
{
  int         faceCount = faces_.   size ( );
  //cout << faceCount <<  "\n";
  int         nodeCount = faces_[0].size ( );
  int         id, pos, mat;
  int         count;


  if ( !isInterfaceElement ( nodeSet, position, globdat ) )
  {
    return false;
  }

  IntVector   vertices(nodeCount);

  // loop over faces of 3D element

  for ( int jf = 0; jf < faceCount; jf++ )
  {
    vertices = faces_[jf];

    count    = 0;

    // loop over nodes of face jf

    for ( int in = 0; in < nodeCount; in++ )
    {
      id  = vertices[in];
      pos = position[id];

      mat = nodeSet[pos]->getDuplicity ();

      // if this node is interfacial

      if ( mat == 2 ) count++;
    }

    // if all nodes of face jf are interfacial
    // then this face is on INTERFACE

    if ( count == nodeCount ) 
    {
      face.resize ( count );
      face      = vertices;
      oppVertex = oppositeVertices_[jf];

      fIndex    = jf;

      return true;
    }
  }

  return false;
}


// ------------------------------------------------------------
//   isInterfaceElement
// ------------------------------------------------------------

bool Element::isInterfaceElement 

         ( const NodeSet& nodeSet,
	   Int2IntMap& position,
       const Global& globdat ) const
{
  int nodeCount  = connectivity0_.size ();
      nodeCount *= isQuadratic_ ? 0.5 : 1;

  if ( elemType_ == 17 ) nodeCount = 8;
	 
  int nodeFace   = nodePerFace_;    
      //nodeFace  *= isQuadratic_ ? 0.5 : 1;

  int index, count(0);

  for ( int in = 0; in < nodeCount; in++ )
  {
    index = connectivity0_[in];
    if ( find ( globdat.interfaceNodes.begin(),
                globdat.interfaceNodes.end(), index ) 
             != globdat.interfaceNodes.end () ) count++;
  }

  return ( count == nodeFace ) ? true : false; 
}

// ------------------------------------------------------------
//   isInterfacialFace
// ------------------------------------------------------------

bool Element::isInterfacialFace 

      ( int faceId,
       const Global& globdat ) const
{
  int nodeFace   = nodePerFace_;    
  int index, count(0);

  IntVector face = faces0_[faceId];
  int nodeCount = face.size ();

  for ( int in = 0; in < nodeCount; in++ )
  {
    index = face[in];
    if ( find ( globdat.interfaceNodes.begin(),
                globdat.interfaceNodes.end(), index ) 
             != globdat.interfaceNodes.end () ) count++;
  }

  return ( count == nodeFace ) ? true : false; 
}

// -------------------------------------------------------
//   getSortedFaces
// -------------------------------------------------------

void Element::getSortedFaces ( vector<IntVector>& faces ) const
{
  int facesCount =  faces_.size () ;
  faces.resize ( facesCount );
  faces = faces_;

  // sort faces

  for ( int jf = 0; jf < facesCount; jf++ )
  {
    sort ( faces[jf].begin(), faces[jf].end() );
  }
}

// -------------------------------------------------------
//   getSortedFaces0
// -------------------------------------------------------

void Element::getSortedFaces0 ( vector<IntVector>& faces ) const
{
  int facesCount =  faces0_.size ();
  faces.resize ( facesCount );
  faces = faces0_;

  // sort faces

  for ( int jf = 0; jf < facesCount; jf++ )
  {
    sort ( faces[jf].begin(), faces[jf].end() );
  }
}

// -------------------------------------------------------
//   getFaces
// -------------------------------------------------------

void Element::getFaces 
      
         ( vector<IntVector>& faces ) const
{
  faces.resize ( faces_.size () );
  faces = faces_;
}

// -------------------------------------------------------
//   getFFaces
// -------------------------------------------------------

void Element::getFFaces 
      
         ( vector<IntVector>& faces ) const
{
  faces.resize ( ffaces_.size () );
  faces = ffaces_;
}


// -------------------------------------------------------
//   getCornerConnectivity0
// -------------------------------------------------------
// node numbering for quadratic elements: 
// follow Gmsh rule: corner nodes midside nodes

void  Element::getCornerConnectivity0

    ( IntVector& connec ) const
{
  if ( !isQuadratic_ )
  {
    getConnectivity0 ( connec );  
  }
  else
  {
    int count = connectivity0_.size () / 2;
    connec.resize ( count );

    for ( int i = 0; i < count; i++ )
    {
      connec[i] = connectivity0_[i];
    }
  }
}

void  Element::getCornerConnectivity

    ( IntVector& connec ) const
{
  if ( !isQuadratic_ )
  {
    getConnectivity ( connec );  
  }
  else
  {
    int count = connectivity_.size () / 2;
    connec.resize ( count );

    for ( int i = 0; i < count; i++ )
    {
      connec[i] = connectivity_[i];
    }
  }
}

// -------------------------------------------------------
//   getBoundsOfFace
// -------------------------------------------------------

void Element::getBoundsOfFace

         ( pair<double,double>& xBound,
           pair<double,double>& yBound,
           pair<double,double>& zBound,
	   int                  fIndex,
	   Global&              globdat) const
{
  IntVector      vertices ( nodePerFace_ );
  vector<double> xCoord, yCoord, zCoord;

  double         x,y,z;
  int            id, pos;

  vertices = faces_[fIndex];

  for ( int in = 0; in < nodePerFace_; in++ )
  {
    id  = vertices[in];
    pos = globdat.nodeId2Position[id];

    x = globdat.nodeSet[pos]->getX();
    y = globdat.nodeSet[pos]->getY();
    z = globdat.nodeSet[pos]->getZ();

    xCoord.push_back ( x );
    yCoord.push_back ( y );
    zCoord.push_back ( z );
  }

  using boost::minmax_element;

  pair<iter, iter> xbound = std::minmax_element ( xCoord.begin(), 
		                             xCoord.end  () );

  xBound.first  = *xbound.first;
  xBound.second = *xbound.second;

  pair<iter, iter> ybound = std::minmax_element ( yCoord.begin(), 
		                             yCoord.end  () );

  yBound.first  = *ybound.first;
  yBound.second = *ybound.second;

  pair<iter, iter> zbound = std::minmax_element ( zCoord.begin(), 
		                             zCoord.end  () );

  zBound.first  = *zbound.first;
  zBound.second = *zbound.second;
}

// -------------------------------------------------------
//   getJemConnectivity
// -------------------------------------------------------

void Element::getJemConnectivity

         ( IntVector& connec )            const
{
  connec.resize ( connectivity_.size() );

  if      ( !isQuadratic_ || isNURBS_ )
  {
    getJemConnectLinear_ ( connec );
  }
  else 
  {
    if      ( elemType_ == 11 )
    {
      getJemConnectTet10_ ( connec );
    }
    else if ( elemType_ == 17 )
    {
      getJemConnectHex20_ ( connec );
    }
    else if ( elemType_ == 8 )
    {
      getJemConnectLinear_ ( connec );
    }
    else
    {
      getJemConnect2DQuadratic_ ( connec );
    }
  }
}

// -------------------------------------------------------
//   PrintElement
// -------------------------------------------------------

struct PrintElement : public unary_function<ElemPointer,void>
{
         PrintElement ( ofstream& out ) : of(out){}

  void   operator () ( const ElemPointer ep ) const
  {
    IntVector connec;
    ep->getConnectivity ( connec );

    copy ( connec.begin(), connec.end(), 
	   ostream_iterator<int> (of, " ") );

    of << endl;

  }

  ofstream&  of;
};


// ----------------------------------------------------------
//   getIndexElementContainsEdge
// --------- ------------------------------------------------

int Element::getIndexElementContainsEdge

      ( Global&                globdat,
        int                    node1,
        int                    node2 ) const
{
  IntVector   neighbors, jnodes;
  int         neiCount;
  int         jelem;
  int         res(-1000);
  IntVector::const_iterator it1, it2;
  ElemPointer jp;

  // loop over neighbors of current element and find the one contains edge (node1,node2)
  // using the modified connectivity not the original one!!!

  //cout << index_ << " " << endl;
  neighbors = globdat.elemNeighbors[globdat.elemId2Position[index_]];
  neiCount  = neighbors.size();

  //print(neighbors.begin(),neighbors.end());
  //cout << "node1,2: " << node1 << " " << node2 << endl;

  for(int je = 0; je < neiCount; je++)
  {
    jelem = globdat.elemId2Position[neighbors[je]];
    jp    = globdat.elemSet[jelem];

    if ( jp->getIndex() == index_ ) continue;

    jp->getCornerConnectivity (jnodes);
  
    //print(jnodes.begin(),jnodes.end());

    it1 = find ( jnodes.begin(), jnodes.end(), node1 );
    it2 = find ( jnodes.begin(), jnodes.end(), node2 );

    if ( it1 != jnodes.end() && it2 != jnodes.end() )
    {
       res = jp->getIndex();
       break;
    }
  }

  return res;
}


// ----------------------------------------------------------
//   getIndexElementContainsFace
// --------- ------------------------------------------------

int Element::getIndexElementContainsFace

      ( Global&                globdat,
        const IntVector&       face ) const
{
  IntVector         neighbors, jnodes, sface;
  int               neiCount;
  int               jelem;
  int               res(-1000);
  ElemPointer       jp;
  vector<IntVector> jfaces;

  //cout << index_ << " " << endl;
  neighbors = globdat.elemNeighbors[globdat.elemId2Position[index_]];
  neiCount  = neighbors.size();

  //print(neighbors.begin(),neighbors.end());

  //sface = face;
  //sort(sface.begin(),sface.end());

  for(int je = 0; je < neiCount; je++)
  {
    jelem = globdat.elemId2Position[neighbors[je]];
    jp    = globdat.elemSet[jelem];

    if ( jp->getIndex() == index_ ) continue;

    jp->getSortedFaces0 (jfaces);

    if ( find (jfaces.begin(), jfaces.end(), face) != jfaces.end() )
    {
       res = jp->getIndex();
       break;
    }
  }
  return res;
}

// ----------------------------------------------------
//    computeElementSize
// ----------------------------------------------------

double Element::computeElementSize ( Global&  globdat ) const
{
   return computeElementSizeTriangle_ ( globdat );
}

// ----------------------------------------------------------
//   buildFacesForTet4
// ----------------------------------------------------------

void Element::buildFacesForTet4_ ()
{
  faces_ .resize ( 4 );
  ffaces_.resize ( 4 );
  oppositeVertices_.resize ( 4 );

  faces_[0].resize ( 3 );
  faces_[1].resize ( 3 );
  faces_[2].resize ( 3 );
  faces_[3].resize ( 3 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[1];
  faces_[0][2] = connectivity_[3];
  oppositeVertices_[0] = connectivity_[2];

  faces_[1][0] = connectivity_[1];
  faces_[1][1] = connectivity_[2];
  faces_[1][2] = connectivity_[3];
  oppositeVertices_[1] = connectivity_[0];

  faces_[2][0] = connectivity_[0];
  faces_[2][1] = connectivity_[2];
  faces_[2][2] = connectivity_[3];
  oppositeVertices_[2] = connectivity_[1];

  faces_[3][0] = connectivity_[0];
  faces_[3][1] = connectivity_[1];
  faces_[3][2] = connectivity_[2];
  oppositeVertices_[3] = connectivity_[3];
  
  ffaces_ = faces_;

  nodePerFace_ = 3;
}


// ----------------------------------------------------------
//   buildFaces0ForTet4
// ----------------------------------------------------------

void Element::buildFaces0ForTet4_ ()
{
  //cout << "...building faces for 4 node tetrahedra";
  faces0_ .resize ( 4 );
  ffaces0_.resize ( 4 );
  oppositeVertices_.resize ( 4 );

  faces0_[0].resize ( 3 );
  faces0_[1].resize ( 3 );
  faces0_[2].resize ( 3 );
  faces0_[3].resize ( 3 );

  faces0_[0][0] = connectivity0_[0];
  faces0_[0][1] = connectivity0_[1];
  faces0_[0][2] = connectivity0_[3];
  oppositeVertices_[0] = connectivity0_[2];

  faces0_[1][0] = connectivity0_[1];
  faces0_[1][1] = connectivity0_[2];
  faces0_[1][2] = connectivity0_[3];
  oppositeVertices_[1] = connectivity0_[0];

  faces0_[2][0] = connectivity0_[0];
  faces0_[2][1] = connectivity0_[2];
  faces0_[2][2] = connectivity0_[3];
  oppositeVertices_[2] = connectivity0_[1];

  faces0_[3][0] = connectivity0_[0];
  faces0_[3][1] = connectivity0_[1];
  faces0_[3][2] = connectivity0_[2];
  oppositeVertices_[3] = connectivity0_[3];

  ffaces0_ = faces0_;

  nodePerFace_ = 3;
}

// ----------------------------------------------------------
//   buildFacesForTet10
// ----------------------------------------------------------

void Element::buildFacesForTet10_ ()
{
  faces_. resize ( 4 );
  ffaces_.resize ( 4 );
  oppositeVertices_.resize ( 4 );

  faces_[0].resize ( 3 ); faces_[1].resize ( 3 );
  faces_[2].resize ( 3 ); faces_[3].resize ( 3 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[1];
  faces_[0][2] = connectivity_[3];
  oppositeVertices_[0] = connectivity_[2];

  ffaces_[0].resize ( 6 );
  ffaces_[0][0] = connectivity_[0]; ffaces_[0][3] = connectivity_[4];
  ffaces_[0][1] = connectivity_[1]; ffaces_[0][4] = connectivity_[9];
  ffaces_[0][2] = connectivity_[3]; ffaces_[0][5] = connectivity_[7];

  faces_[1][0] = connectivity_[1];
  faces_[1][1] = connectivity_[2];
  faces_[1][2] = connectivity_[3];
  oppositeVertices_[1] = connectivity_[0];

  ffaces_[1].resize ( 6 );
  ffaces_[1][0] = connectivity_[1]; ffaces_[1][3] = connectivity_[5];
  ffaces_[1][1] = connectivity_[2]; ffaces_[1][4] = connectivity_[8];
  ffaces_[1][2] = connectivity_[3]; ffaces_[1][5] = connectivity_[9];

  faces_[2][0] = connectivity_[0];
  faces_[2][1] = connectivity_[2];
  faces_[2][2] = connectivity_[3];
  oppositeVertices_[2] = connectivity_[1];

  ffaces_[2].resize ( 6 );
  ffaces_[2][0] = connectivity_[0]; ffaces_[2][3] = connectivity_[6];
  ffaces_[2][1] = connectivity_[2]; ffaces_[2][4] = connectivity_[8];
  ffaces_[2][2] = connectivity_[3]; ffaces_[2][5] = connectivity_[7];

  faces_[3][0] = connectivity_[0];
  faces_[3][1] = connectivity_[1];
  faces_[3][2] = connectivity_[2];
  oppositeVertices_[3] = connectivity_[3];

  ffaces_[3].resize ( 6 );
  ffaces_[3][0] = connectivity_[0]; ffaces_[3][3] = connectivity_[4];
  ffaces_[3][1] = connectivity_[1]; ffaces_[3][4] = connectivity_[5];
  ffaces_[3][2] = connectivity_[2]; ffaces_[3][5] = connectivity_[6];

  nodePerFace_ = 3;
}


// ----------------------------------------------------------
//   buildFaces0ForTet10
// ----------------------------------------------------------

void Element::buildFaces0ForTet10_ ()
{
  faces0_. resize ( 4 );
  ffaces0_.resize ( 4 );
  oppositeVertices_.resize ( 4 );

  faces0_[0].resize ( 3 ); faces0_[1].resize ( 3 );
  faces0_[2].resize ( 3 ); faces0_[3].resize ( 3 );

  faces0_[0][0] = connectivity0_[0];
  faces0_[0][1] = connectivity0_[1];
  faces0_[0][2] = connectivity0_[3];
  oppositeVertices_[0] = connectivity0_[2];

  ffaces0_[0].resize ( 6 );
  ffaces0_[0][0] = connectivity0_[0]; ffaces0_[0][3] = connectivity0_[4];
  ffaces0_[0][1] = connectivity0_[1]; ffaces0_[0][4] = connectivity0_[9];
  ffaces0_[0][2] = connectivity0_[3]; ffaces0_[0][5] = connectivity0_[7];

  faces0_[1][0] = connectivity0_[1];
  faces0_[1][1] = connectivity0_[2];
  faces0_[1][2] = connectivity0_[3];
  oppositeVertices_[1] = connectivity0_[0];

  ffaces0_[1].resize ( 6 );
  ffaces0_[1][0] = connectivity0_[1]; ffaces0_[1][3] = connectivity0_[5];
  ffaces0_[1][1] = connectivity0_[2]; ffaces0_[1][4] = connectivity0_[8];
  ffaces0_[1][2] = connectivity0_[3]; ffaces0_[1][5] = connectivity0_[9];

  faces0_[2][0] = connectivity0_[0];
  faces0_[2][1] = connectivity0_[2];
  faces0_[2][2] = connectivity0_[3];
  oppositeVertices_[2] = connectivity0_[1];

  ffaces0_[2].resize ( 6 );
  ffaces0_[2][0] = connectivity0_[0]; ffaces0_[2][3] = connectivity0_[6];
  ffaces0_[2][1] = connectivity0_[2]; ffaces0_[2][4] = connectivity0_[8];
  ffaces0_[2][2] = connectivity0_[3]; ffaces0_[2][5] = connectivity0_[7];

  faces0_[3][0] = connectivity0_[0];
  faces0_[3][1] = connectivity0_[1];
  faces0_[3][2] = connectivity0_[2];
  oppositeVertices_[3] = connectivity0_[3];

  ffaces0_[3].resize ( 6 );
  ffaces0_[3][0] = connectivity0_[0]; ffaces0_[3][3] = connectivity0_[4];
  ffaces0_[3][1] = connectivity0_[1]; ffaces0_[3][4] = connectivity0_[5];
  ffaces0_[3][2] = connectivity0_[2]; ffaces0_[3][5] = connectivity0_[6];

  nodePerFace_ = 3;
}

// ----------------------------------------------------------
//   buildFacesForHex8
// ----------------------------------------------------------

void Element::buildFacesForHex8_ ()
{
  isQuadratic_ = false;

  faces_.           resize ( 6 );
  ffaces_.          resize ( 6 );
  oppositeVertices_.resize ( 6 );

  faces_[0].resize ( 4 ); faces_[1].resize ( 4 ); faces_[2].resize ( 4 );
  faces_[3].resize ( 4 ); faces_[4].resize ( 4 ); faces_[5].resize ( 4 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[3];
  faces_[0][2] = connectivity_[2];
  faces_[0][3] = connectivity_[1];
  oppositeVertices_[0] = connectivity_[4];

  faces_[1][0] = connectivity_[4];
  faces_[1][1] = connectivity_[7];
  faces_[1][2] = connectivity_[6];
  faces_[1][3] = connectivity_[5];
  oppositeVertices_[1] = connectivity_[0];

  faces_[2][0] = connectivity_[4];
  faces_[2][1] = connectivity_[0];
  faces_[2][2] = connectivity_[1];
  faces_[2][3] = connectivity_[5];
  oppositeVertices_[2] = connectivity_[7];

  faces_[3][0] = connectivity_[7];
  faces_[3][1] = connectivity_[6];
  faces_[3][2] = connectivity_[2];
  faces_[3][3] = connectivity_[3];
  oppositeVertices_[3] = connectivity_[4];

  faces_[4][0] = connectivity_[1];
  faces_[4][1] = connectivity_[5];
  faces_[4][2] = connectivity_[6];
  faces_[4][3] = connectivity_[2];
  oppositeVertices_[4] = connectivity_[0];

  faces_[5][0] = connectivity_[3];
  faces_[5][1] = connectivity_[0];
  faces_[5][2] = connectivity_[4];
  faces_[5][3] = connectivity_[7];
  oppositeVertices_[5] = connectivity_[2];

  ffaces_ = faces_;

  nodePerFace_ = 4;
}

// ----------------------------------------------------------
//   buildFaces0ForHex8
// ----------------------------------------------------------

void Element::buildFaces0ForHex8_ ()
{
  isQuadratic_ = false;

  faces0_.          resize ( 6 );
  ffaces0_.         resize ( 6 );
  oppositeVertices_.resize ( 6 );

  faces0_[0].resize ( 4 ); faces0_[1].resize ( 4 ); faces0_[2].resize ( 4 );
  faces0_[3].resize ( 4 ); faces0_[4].resize ( 4 ); faces0_[5].resize ( 4 );

  faces0_[0][0] = connectivity0_[0];
  faces0_[0][1] = connectivity0_[3];
  faces0_[0][2] = connectivity0_[2];
  faces0_[0][3] = connectivity0_[1];
  oppositeVertices_[0] = connectivity_[4];

  faces0_[1][0] = connectivity0_[4];
  faces0_[1][1] = connectivity0_[7];
  faces0_[1][2] = connectivity0_[6];
  faces0_[1][3] = connectivity0_[5];
  oppositeVertices_[1] = connectivity_[0];

  faces0_[2][0] = connectivity0_[4];
  faces0_[2][1] = connectivity0_[0];
  faces0_[2][2] = connectivity0_[1];
  faces0_[2][3] = connectivity0_[5];
  oppositeVertices_[2] = connectivity_[7];

  faces0_[3][0] = connectivity0_[7];
  faces0_[3][1] = connectivity0_[6];
  faces0_[3][2] = connectivity0_[2];
  faces0_[3][3] = connectivity0_[3];
  oppositeVertices_[3] = connectivity_[4];

  faces0_[4][0] = connectivity0_[1];
  faces0_[4][1] = connectivity0_[5];
  faces0_[4][2] = connectivity0_[6];
  faces0_[4][3] = connectivity0_[2];
  oppositeVertices_[4] = connectivity_[0];

  faces0_[5][0] = connectivity0_[3];
  faces0_[5][1] = connectivity0_[0];
  faces0_[5][2] = connectivity0_[4];
  faces0_[5][3] = connectivity0_[7];
  oppositeVertices_[5] = connectivity_[2];

  ffaces0_ = faces0_;

  nodePerFace_ = 4;
}

// ----------------------------------------------------------
//   buildFacesForHex20
// ----------------------------------------------------------

void Element::buildFacesForHex20_ ()
{
  faces_.           resize ( 6 );
  ffaces_.          resize ( 6 );
  oppositeVertices_.resize ( 6 );

  faces_[0].resize ( 4 ); faces_[1].resize ( 4 ); faces_[2].resize ( 4 );
  faces_[3].resize ( 4 ); faces_[4].resize ( 4 ); faces_[5].resize ( 4 );

  faces_[0][0] = connectivity_[0];
  faces_[0][1] = connectivity_[3];
  faces_[0][2] = connectivity_[2];
  faces_[0][3] = connectivity_[1];
  oppositeVertices_[0] = connectivity_[4];

  ffaces_[0].resize ( 8 );
  ffaces_[0][0] = connectivity_[0]; ffaces_[0][4] = connectivity_[8];
  ffaces_[0][1] = connectivity_[1]; ffaces_[0][5] = connectivity_[11];
  ffaces_[0][2] = connectivity_[2]; ffaces_[0][6] = connectivity_[13];
  ffaces_[0][3] = connectivity_[3]; ffaces_[0][7] = connectivity_[9];

  faces_[1][0] = connectivity_[4];
  faces_[1][1] = connectivity_[7];
  faces_[1][2] = connectivity_[6];
  faces_[1][3] = connectivity_[5];
  oppositeVertices_[1] = connectivity_[0];

  ffaces_[1].resize ( 8 );
  ffaces_[1][0] = connectivity_[4]; ffaces_[1][4] = connectivity_[16];
  ffaces_[1][1] = connectivity_[5]; ffaces_[1][5] = connectivity_[18];
  ffaces_[1][2] = connectivity_[6]; ffaces_[1][6] = connectivity_[19];
  ffaces_[1][3] = connectivity_[7]; ffaces_[1][7] = connectivity_[17];

  faces_[2][0] = connectivity_[4];
  faces_[2][1] = connectivity_[0];
  faces_[2][2] = connectivity_[1];
  faces_[2][3] = connectivity_[5];
  oppositeVertices_[2] = connectivity_[7];

  ffaces_[2].resize ( 8 );
  ffaces_[2][0] = connectivity_[4]; ffaces_[2][4] = connectivity_[16];
  ffaces_[2][1] = connectivity_[5]; ffaces_[2][5] = connectivity_[12];
  ffaces_[2][2] = connectivity_[1]; ffaces_[2][6] = connectivity_[8];
  ffaces_[2][3] = connectivity_[0]; ffaces_[2][7] = connectivity_[10];

  faces_[3][0] = connectivity_[7];
  faces_[3][1] = connectivity_[6];
  faces_[3][2] = connectivity_[2];
  faces_[3][3] = connectivity_[3];
  oppositeVertices_[3] = connectivity_[4];

  ffaces_[3].resize ( 8 );
  ffaces_[3][0] = connectivity_[7];  ffaces_[3][4] = connectivity_[19];
  ffaces_[3][1] = connectivity_[6];  ffaces_[3][5] = connectivity_[14];
  ffaces_[3][2] = connectivity_[2];  ffaces_[3][6] = connectivity_[13];
  ffaces_[3][3] = connectivity_[3];  ffaces_[3][7] = connectivity_[15];

  faces_[4][0] = connectivity_[1];
  faces_[4][1] = connectivity_[5];
  faces_[4][2] = connectivity_[6];
  faces_[4][3] = connectivity_[2];
  oppositeVertices_[4] = connectivity_[0];

  ffaces_[4].resize ( 8 );
  ffaces_[4][0] = connectivity_[1]; ffaces_[4][4] = connectivity_[11];
  ffaces_[4][1] = connectivity_[2]; ffaces_[4][5] = connectivity_[14];
  ffaces_[4][2] = connectivity_[6]; ffaces_[4][6] = connectivity_[18];
  ffaces_[4][3] = connectivity_[5]; ffaces_[4][7] = connectivity_[12];

  faces_[5][0] = connectivity_[3];
  faces_[5][1] = connectivity_[0];
  faces_[5][2] = connectivity_[4];
  faces_[5][3] = connectivity_[7];
  oppositeVertices_[5] = connectivity_[2];

  ffaces_[5].resize ( 8 );
  ffaces_[5][0] = connectivity_[3]; ffaces_[5][4] = connectivity_[15];
  ffaces_[5][1] = connectivity_[7]; ffaces_[5][5] = connectivity_[17];
  ffaces_[5][2] = connectivity_[4]; ffaces_[5][6] = connectivity_[10];
  ffaces_[5][3] = connectivity_[0]; ffaces_[5][7] = connectivity_[9];

  nodePerFace_ = 4;
}


// ----------------------------------------------------------
//   buildFaces0ForHex20
// ----------------------------------------------------------

void Element::buildFaces0ForHex20_ ()
{
  faces0_.           resize ( 6 );
  ffaces0_.          resize ( 6 );
  oppositeVertices_.resize ( 6 );

  faces0_[0].resize ( 4 ); faces0_[1].resize ( 4 ); faces0_[2].resize ( 4 );
  faces0_[3].resize ( 4 ); faces0_[4].resize ( 4 ); faces0_[5].resize ( 4 );

  faces0_[0][0] = connectivity0_[0];
  faces0_[0][1] = connectivity0_[3];
  faces0_[0][2] = connectivity0_[2];
  faces0_[0][3] = connectivity0_[1];
  oppositeVertices_[0] = connectivity_[4];

  ffaces0_[0].resize ( 8 );
  ffaces0_[0][0] = connectivity0_[0]; ffaces0_[0][4] = connectivity0_[8];
  ffaces0_[0][1] = connectivity0_[1]; ffaces0_[0][5] = connectivity0_[11];
  ffaces0_[0][2] = connectivity0_[2]; ffaces0_[0][6] = connectivity0_[13];
  ffaces0_[0][3] = connectivity0_[3]; ffaces0_[0][7] = connectivity0_[9];

  faces0_[1][0] = connectivity0_[4];
  faces0_[1][1] = connectivity0_[7];
  faces0_[1][2] = connectivity0_[6];
  faces0_[1][3] = connectivity0_[5];
  oppositeVertices_[1] = connectivity_[0];

  ffaces0_[1].resize ( 8 );
  ffaces0_[1][0] = connectivity0_[4]; ffaces0_[1][4] = connectivity0_[16];
  ffaces0_[1][1] = connectivity0_[5]; ffaces0_[1][5] = connectivity0_[18];
  ffaces0_[1][2] = connectivity0_[6]; ffaces0_[1][6] = connectivity0_[19];
  ffaces0_[1][3] = connectivity0_[7]; ffaces0_[1][7] = connectivity0_[17];

  faces0_[2][0] = connectivity0_[4];
  faces0_[2][1] = connectivity0_[0];
  faces0_[2][2] = connectivity0_[1];
  faces0_[2][3] = connectivity0_[5];
  oppositeVertices_[2] = connectivity0_[7];

  ffaces0_[2].resize ( 8 );
  ffaces0_[2][0] = connectivity0_[4]; ffaces0_[2][4] = connectivity0_[16];
  ffaces0_[2][1] = connectivity0_[5]; ffaces0_[2][5] = connectivity0_[12];
  ffaces0_[2][2] = connectivity0_[1]; ffaces0_[2][6] = connectivity0_[8];
  ffaces0_[2][3] = connectivity0_[0]; ffaces0_[2][7] = connectivity0_[10];

  faces0_[3][0] = connectivity0_[7];
  faces0_[3][1] = connectivity0_[6];
  faces0_[3][2] = connectivity0_[2];
  faces0_[3][3] = connectivity0_[3];
  oppositeVertices_[3] = connectivity_[4];

  ffaces0_[3].resize ( 8 );
  ffaces0_[3][0] = connectivity0_[7];  ffaces0_[3][4] = connectivity0_[19];
  ffaces0_[3][1] = connectivity0_[6];  ffaces0_[3][5] = connectivity0_[14];
  ffaces0_[3][2] = connectivity0_[2];  ffaces0_[3][6] = connectivity0_[13];
  ffaces0_[3][3] = connectivity0_[3];  ffaces0_[3][7] = connectivity0_[15];

  faces0_[4][0] = connectivity0_[1];
  faces0_[4][1] = connectivity0_[5];
  faces0_[4][2] = connectivity0_[6];
  faces0_[4][3] = connectivity0_[2];
  oppositeVertices_[4] = connectivity_[0];

  ffaces0_[4].resize ( 8 );
  ffaces0_[4][0] = connectivity0_[1]; ffaces0_[4][4] = connectivity0_[11];
  ffaces0_[4][1] = connectivity0_[2]; ffaces0_[4][5] = connectivity0_[14];
  ffaces0_[4][2] = connectivity0_[6]; ffaces0_[4][6] = connectivity0_[18];
  ffaces0_[4][3] = connectivity0_[5]; ffaces0_[4][7] = connectivity0_[12];

  faces0_[5][0] = connectivity0_[3];
  faces0_[5][1] = connectivity0_[0];
  faces0_[5][2] = connectivity0_[4];
  faces0_[5][3] = connectivity0_[7];
  oppositeVertices_[5] = connectivity_[2];

  ffaces0_[5].resize ( 8 );
  ffaces0_[5][0] = connectivity0_[3]; ffaces0_[5][4] = connectivity0_[15];
  ffaces0_[5][1] = connectivity0_[7]; ffaces0_[5][5] = connectivity0_[17];
  ffaces0_[5][2] = connectivity0_[4]; ffaces0_[5][6] = connectivity0_[10];
  ffaces0_[5][3] = connectivity0_[0]; ffaces0_[5][7] = connectivity0_[9];

  nodePerFace_ = 4;
}

// ----------------------------------------------------
//    getJemConnect2DQuadratic_
// ----------------------------------------------------

// Jemjive format: nodes (corner and midside) are continuous

void Element::getJemConnect2DQuadratic_

     ( IntVector& connect )            const
{

  int  halfNodeCount = connect.size () / 2;

  //print(connectivity_.begin(),connectivity_.end());

  for ( int i = 0; i < halfNodeCount; i++ )
  {
    connect[2*i]   = connectivity_[i];
    connect[2*i+1] = connectivity_[halfNodeCount+i];
  }

  //print(connect.begin(),connect.end());
}


// ----------------------------------------------------
//    getJemConnectTet10_
// ----------------------------------------------------


void Element::getJemConnectTet10_

     ( IntVector& connect )            const
{
  connect[0] = connectivity_[0]; connect[3] = connectivity_[9];
  connect[1] = connectivity_[7]; connect[4] = connectivity_[1];
  connect[2] = connectivity_[3]; connect[5] = connectivity_[4];

  connect[6] = connectivity_[6];
  connect[7] = connectivity_[8];
  connect[8] = connectivity_[5];

  connect[9] = connectivity_[2];
}


// ----------------------------------------------------
//    getJemConnectHex20_
// ----------------------------------------------------

void Element::getJemConnectHex20_

     ( IntVector& connect )            const
{
  connect[0]  = connectivity_[0];  connect[4] = connectivity_[5];
  connect[1]  = connectivity_[10]; connect[5] = connectivity_[12];
  connect[2]  = connectivity_[4];  connect[6] = connectivity_[1];
  connect[3]  = connectivity_[16]; connect[7] = connectivity_[8];

  connect[8]  = connectivity_[9];  connect[10] = connectivity_[18];
  connect[9]  = connectivity_[17]; connect[11] = connectivity_[11];

  connect[12] = connectivity_[3];  connect[16] = connectivity_[6];
  connect[13] = connectivity_[15]; connect[17] = connectivity_[14];
  connect[14] = connectivity_[7];  connect[18] = connectivity_[2];
  connect[15] = connectivity_[19]; connect[19] = connectivity_[13];
}

// ----------------------------------------------------
//    computeElementSizeTriangle_
// ----------------------------------------------------

double Element::computeElementSizeTriangle_ ( Global&  globdat ) const
{
  int id1 = connectivity0_[0]; 
  int id2 = connectivity0_[1]; 
  int id3 = connectivity0_[2]; 
  
  NodePointer node1, node2, node3;

  node1 = globdat.nodeSet[globdat.nodeId2Position[id1]];
  node2 = globdat.nodeSet[globdat.nodeId2Position[id2]];
  node3 = globdat.nodeSet[globdat.nodeId2Position[id3]];

  double x1 = node1->getX ( ); double y1 = node1->getY ( ); double z1 = node1->getZ ();
  double x2 = node2->getX ( ); double y2 = node2->getY ( ); double z2 = node2->getZ ();
  double x3 = node3->getX ( ); double y3 = node3->getY ( ); double z3 = node3->getZ ();
  
  double a = sqrt ( pow ( x2 - x1, 2 ) + pow ( y2 - y1, 2 ) + pow ( z2 - z1, 2 ) );
  double b = sqrt ( pow ( x3 - x2, 2 ) + pow ( y3 - y2, 2 ) + pow ( z3 - z2, 2 ) );
  double c = sqrt ( pow ( x1 - x3, 2 ) + pow ( y1 - y3, 2 ) + pow ( z1 - z3, 2 ) );


  double s = 0.5 * (a + b + c);
  double A =  sqrt ( s * (s-a) * (s-b) * (s-c) );

  return A;//2.*A/(min(min(a,b),c));

}


