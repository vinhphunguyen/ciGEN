#ifndef NODE_H
#define NODE_H

#include "typedefs.h"

// =====================================================================
//     class NODE
// =====================================================================

class Node
{
  public:

                     Node ( double x, double y, double z, int index );

		     Node ( const Node& aNode );

    double           getX           () const { return x_; }		     
    double           getY           () const { return y_; }		     
    double           getZ           () const { return z_; }		     
    double           getIndex       () const { return index_; }
    bool             getIsInterface () const { return interface_;}
    bool             getIsOnBoundary() const { return onBoundary_;}
    bool             getDone        () const { return done_;}
    bool             getIsRigid     () const { return isRigid_;}

    int              getDuplicity   () const { return duplicity_;}


    void             setDuplicity   ( int dupl ) { duplicity_ = dupl; }

    void             setIsInterface ( bool interface ) { interface_ = interface; }

    void             setIsOnBoundary ( bool bound ) { onBoundary_ = bound; }

    void             setDone ( bool done ){ done_ = done;}

    void             setIsRigid ( bool rig )
    {
      isRigid_ = rig;

      if ( interface_ ) isRigid_ = false;
    }

  public:

    IntVector        support;

  private:

    double           x_;
    double           y_;
    double           z_;

    int              index_;
    int              duplicity_;

    bool             interface_;
    bool             onBoundary_;
    bool             done_;
    bool             isRigid_;
};

ostream&  operator << ( ostream& os, const Node& aNode );

struct XCoordComp 
{
  bool operator () ( const NodePointer n1, const NodePointer n2 )
  {
    return n1->getX() < n2->getX();  
  }
};

struct YCoordComp 
{
  bool operator () ( const NodePointer n1, const NodePointer n2 )
  {
    return n1->getY() < n2->getY();  
  }
};

struct ZCoordComp 
{
  bool operator () ( const NodePointer n1, const NodePointer n2 )
  {
    return n1->getZ() < n2->getZ();  
  }
};

#endif
