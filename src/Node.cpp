#include "Node.h"


Node::Node ( double x, double y, double z, int index )
  : x_(x), y_(y), z_(z), index_(index), 
    duplicity_(1), interface_(false),
    onBoundary_(false), done_(false), isRigid_(false)
{
}

Node::Node ( const Node& aNode )
  : x_(aNode.getX()), y_(aNode.getY()), index_(aNode.getIndex())
{
}

ostream&  operator << ( ostream& os, const Node& aNode )
{
  return os;
}

