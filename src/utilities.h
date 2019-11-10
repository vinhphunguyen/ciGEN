#ifndef UTILITIES_H
#define UTILITIES_H

#include <boost/lexical_cast.hpp>

#include "typedefs.h"

// -------------------------------------------------------------------
//  class Str2IntFunctor
// -------------------------------------------------------------------

struct Str2IntFunctor : public unary_function<string,int>
{
  int operator () ( const string& str  ) const
  {
    return boost::lexical_cast<int> ( str );
  }
};

// -------------------------------------------------------------------
//  class NodePair
// -------------------------------------------------------------------

struct NodePair
{
  int            node1;
  int            node2;

                 NodePair ( int n1, int n2 )
     : node1(n1), node2(n2){}
};

bool      operator == ( const NodePair& np1,
                        const NodePair& np2 );

ostream&  operator << 

       ( ostream& os, const NodePair& np );


int          getParaElemType ( int gmshElemType );


// --------------------------------------------------------------------
//   print
// --------------------------------------------------------------------

template <typename Iter>
void print ( Iter first,
             Iter last,
	     const char* sep = " ",
	     ostream& os = cout )
{
  typedef typename iterator_traits<Iter>::value_type T;

  copy ( first, last, ostream_iterator<T>(os,sep) );

  os << endl;
}

// =====================================================================
//     class Segment
// =====================================================================


class Point
{
  public:
      
           Point ( double x = 0., 
	           double y = 0. )
	           : x(x), y(y){}

           Point ( const Point& p ) : x(p.x), y(p.y){}		   

    double x;
    double y;
};

bool  isAligned ( const Point& test, 
                  const Point& p0,
		  const Point& p1 );

class Segment
{
  public:

                 Segment () {}

                 Segment ( const Point& p1, 
		           const Point& p2 );  
    
    bool         isOn ( double x, double y );

    Point        p1;
    Point        p2;
    Point        direction;
};

ostream&  operator << 
 ( ostream& os, const Segment& s );

ostream& operator << 

  ( ostream& os, const vector<Segment>& segments );


#endif
