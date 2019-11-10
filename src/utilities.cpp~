#include "utilities.h"


bool	operator == ( const NodePair& np1,
                      const NodePair& np2 )
{
  bool ret1 = ( np1.node1 == np2.node1 ) && ( np1.node2 == np2.node2 );
  bool ret2 = ( np1.node1 == np2.node2 ) && ( np1.node2 == np2.node1 );

  return ret1 || ret2;
}

ostream&  operator << 
 ( ostream& os, const NodePair& np )
{
  os << "(" << np.node1 << "," << np.node2 << ")\n";

  return os;
}

int          getParaElemType ( int gmshElemType )
{
  switch ( gmshElemType )
  {
    case 2: return 5;  // 3-node triangle
    case 3: return 9;  // 4-node quad	    
  }
}



void print ( const vector<IntVector>& v )
{
  const int size = v.size ();

  for ( int i = 0; i < size; i++ )
  {
    cout << "element " << i << " : ";

    print ( v[i].begin(), v[i].end() );
  }
}

bool  isAligned ( const Point& test, 
                  const Point& p0,
		  const Point& p1 )
{
  return ( fabs( (p1.x - test.x) * (p0.y - test.y) - 
                 (p0.x - test.x) * (p1.y - test.y) )
         < 10. * numeric_limits<double>::epsilon() );
}


Segment::Segment  ( const Point& p1, 
		    const Point& p2 )
 : p1(p1), p2(p2)		    
{
  direction.x = p2.x - p1.x;
  direction.y = p2.y - p1.y;
}

bool Segment::isOn ( double x, double y )
{
  Point p(x,y);

  if ( !isAligned ( p, p1, p2 ) )
  {
    return false;
  }
  else if ( fabs ( direction.x ) > fabs ( direction.y ) )
  {
    double t  = ( x - p1.x ) / direction.x;
    return t > -2. * numeric_limits<double>::epsilon() && 
           t < 1.  + 2. * numeric_limits<double>::epsilon() ;
  }
  else
  {
    double t  = ( y - p1.y ) / direction.y;
    return t > -2. * numeric_limits<double>::epsilon() && 
           t < 1.  + 2. * numeric_limits<double>::epsilon() ;
  }
}

ostream&  operator << 
 ( ostream& os, const Segment& s )
{
  os << "(" << s.p1.x << "," << s.p1.y << "); "
     << "(" << s.p2.x << "," << s.p2.y << "); ";

  return os;
}

ostream& operator << 

  ( ostream& os, const vector<Segment>& segments )
{
  const int noSegment = segments.size();

  //for ( int is = 0; is < noSegment; ++is )
  //{
    os << segments[0];
    os << segments[1];
  //}
  return os;
}

