/**
 * This file is a part of the interface element generator program.
 *
 * V.P.Nguyen, 
 * TU Delft 2009
 * Cardiff University, 2013
 *
 */

#ifndef ELEMENT_H
#define ELEMENT_H

#include "typedefs.h"

class Global;

// =====================================================================
//     class ELEMENT
// =====================================================================

// For quadratic elements:
// the connectivity follows Gmsh's format !!!

class Element
{
  public:

                         Element

	( int index, int elemType, 
	  const IntVector& connec );

                         Element

	( int index, int elemType, 
	  const IntVector& connec, bool isNURBS );

			 Element

	( int index, 
	  const IntVector& connec );

                         Element

	( int index, int elemType, 
	  const IntVector& connec, int bulk1, int bulk2 );

    // get the new connectivity (full)

    inline void		 getConnectivity

        ( IntVector& connec ) const;

    // get the original connectivity (full)
    
    inline void		 getConnectivity0

        ( IntVector& connec ) const;

    // get indices of corner nodes of unmodified mesh
    // for linear elements == getConnectivity0
    // for quadratic elements: 

    void		 getCornerConnectivity0

        ( IntVector& connec ) const;

    void		 getCornerConnectivity

        ( IntVector& connec ) const;

    inline void          setConnectivity

        ( const IntVector& newConnec );

    inline int           getIndex    () const;
    inline int           getElemType () const;
    inline bool          getDone     () const;
    inline bool          getChanged  () const;
    inline int           getBulk1    () const;
    inline int           getBulk2    () const;

    inline void          setElemType ( int type );
    inline void          setDone     ( bool done ); 

    inline int           getOppVertex (int faceID) const;

    inline void          changeConnectivity 
      
        ( int oldId, int newId );

    // 3D elements: build face datastructures

    void                 buildFaces0 ( );
    void                 buildFaces  ( );

    // 3D elements: get all faces and the nodes in each face are sorted
    // to facilitate equality comparison to find commond faces
    // Face with vertices only, not full face

    void                 getSortedFaces0
      
         ( vector<IntVector>& faces ) const;

    // The same as getSortedFaces0 but for modified mesh
    
    void                 getSortedFaces
      
         ( vector<IntVector>& faces ) const;
    
    // Get unsorted faces (vertices only)

    void                 getFaces 
      
         ( vector<IntVector>& faces ) const;
    
    void                 getFFaces 
      
         ( vector<IntVector>& faces ) const;
    
    inline void          getFullFace 

         ( IntVector& fface,
	   int        index ) const;

    // return all nodes of the face indexed index
    // used for quadratic 3D elements

    inline void          getFullFace0 

         ( IntVector& fface,
	   int        index ) const;

   // check if a 3D element has a face on the interface or not
   // if so, return the indices of nodes of that face 
    
   bool                  isOnInterface 
      
         ( IntVector&        face,    
	   int&              oppVertex,
	   int&              fIndex,
	   const NodeSet&    nodeSet,
	         Int2IntMap& position ) const;

   // check if a 3D element has a face on the interface or not 

   bool                  isInterfaceElement 

         ( const NodeSet&    nodeSet,
	         Int2IntMap& position )  const;

   // return the coordinate bounds of a face  of a 3D element.

   void                  getBoundsOfFace

         ( pair<double,double>& xBound,
           pair<double,double>& yBound,
           pair<double,double>& zBound,
	   int                  fIndex,
	         Global&        globdat ) const;

   bool                  isOnExternalBoundary

         ( int                   faceId, 
           Global&               globdat ) const;


   void                  getJemConnectivity

         ( IntVector& connec )            const;

   inline void           setNURBS ( );

   // for an edge (node1,node2), find the index of element
   // also contains this edge: useful for discontinuous Galerkin methods.

   int                   getIndexElementContainsEdge

      ( Global&                globdat,
        int                    node1,
        int                    node2 ) const;

   int                   getIndexElementContainsFace

      ( Global&                globdat,
        const IntVector&       face ) const;
   
   double                computeElementSize 
      
      ( Global&                globdat ) const;

  private:

   void                  buildFacesForTet4_   ( );
   void                  buildFaces0ForTet4_  ( );
   void                  buildFacesForTet10_  ( );
   void                  buildFaces0ForTet10_ ( );
   void                  buildFacesForHex8_   ( );
   void                  buildFaces0ForHex8_  ( );
   void                  buildFacesForHex20_  ( );
   void                  buildFaces0ForHex20_ ( );


   inline void           getJemConnectLinear_

         ( IntVector& connec )            const; 

   void                  getJemConnect2DQuadratic_

         ( IntVector& connec )            const; 

   void                  getJemConnectTet10_

         ( IntVector& connec )            const; 


   void                  getJemConnectHex20_

         ( IntVector& connec )            const; 

   double                computeElementSizeTriangle_( Global& globdat ) const;

  private:

    int                  index_;
    int                  elemType_;   // used with Paraview format

    IntVector            connectivity_;     // modified connectivity array
    IntVector            connectivity0_;    // original connectivity array

    bool                 done_;
    bool                 isQuadratic_;
    bool                 isChanged_;
    bool                 isNURBS_;
    int                  bulk1_;
    int                  bulk2_;

    // 3D data

    vector<IntVector>    faces_;             // store the corner vertex indices of faces
    vector<IntVector>    faces0_;            // store the corner vertex indices of faces
    vector<IntVector>    ffaces_;            // store the vertex indices of faces (modified mesh)
    vector<IntVector>    ffaces0_;           // store the vertex indices of faces (original mesh)
    IntVector            oppositeVertices_; // store the opposite vertices of  all faces

    int                  nodePerFace_;
};

// ==========================================================
//   related stuff
// ==========================================================

struct PrintElement;

// ==========================================================
//   implementation of inline functions
// ==========================================================

inline int Element::getIndex () const
{
  return index_;
}

inline int Element::getElemType () const
{
  return elemType_;
}

inline int Element::getBulk1 () const
{
  return bulk1_;
}

inline int Element::getBulk2 () const
{
  return bulk2_;
}

inline void  Element::setElemType ( int type )
{
  elemType_ = type;
}

inline  void Element::setDone ( bool done ) 
{
  done_ = done;
}

inline bool Element::getDone () const 
{ 
  return done_;
}

inline bool Element::getChanged () const 
{ 
  return isChanged_;
}

inline void Element::getConnectivity

   ( IntVector& connec ) const
{
  connec.resize ( connectivity_.size() );
  connec = connectivity_;	
}

inline void Element::getConnectivity0

   ( IntVector& connec ) const
{
  connec.resize ( connectivity0_.size() );
  connec = connectivity0_;	
}


inline void Element::setConnectivity

        ( const IntVector& newConnec )
{
  connectivity_ = newConnec;
}

inline void Element::changeConnectivity ( int oldId, int newId )
{
  replace ( connectivity_.begin(),
	    connectivity_.end  (),
	    oldId, newId );

  if ( !isChanged_ && newId != oldId )
  {
    isChanged_ = true;
  }
}

inline void Element::getFullFace

   ( IntVector& fface,
     int        index ) const
{
  fface.resize ( ffaces_[0].size() );
  fface = ffaces_[index];	
}

inline void Element::getFullFace0

   ( IntVector& fface,
     int        index ) const
{
  fface.resize ( ffaces0_[0].size() );
  fface = ffaces0_[index];	
}

inline void Element::getJemConnectLinear_

   ( IntVector& connec ) const
{
  connec.resize ( connectivity_.size() );
  connec = connectivity_;	
}

inline void Element::setNURBS ()
{
  isNURBS_ = true;
}

inline int Element::getOppVertex (int faceID) const
{
  return oppositeVertices_[faceID];
}

#endif
