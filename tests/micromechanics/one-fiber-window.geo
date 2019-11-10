a = 1;
r = 0.25;

h  = 0.03;
h1 = 0.1*h;
h2 = 0.4*h;

// -------------------
// corner points 
// -------------------

Point(1) = {0,0,0,h};
Point(2) = {a,0,0,h};
Point(3) = {a,a,0,h};
Point(4) = {0.,a,0,h};

// -------------------
// inner circle's points
// -------------------

Point(5) = {0.5*a,0.5*a,0,0.06};
Point(6) = {0.5*a+r,0.5*a,0,h2};
Point(7) = {0.5*a,0.5*a+r,0,h2};
Point(8) = {0.5*a-r,0.5*a,0,h2};
Point(9) = {0.5*a,0.5*a-r,0,h2};

// points of the outer zone

Point(10) = {-0.05,-0.05,0,h};
Point(11) = {1.05,-0.05,0,h};
Point(12) = {1.05,1.05,0,h};
Point(13) = {-0.05,1.05,0,h};


// -------------------
// outer lines
// -------------------

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};

// hole

Circle(5) = {6,5,7};
Circle(6) = {7,5,8};
Circle(7) = {8,5,9};
Circle(8) = {9,5,6};

// outer lines (window)

Line(9) = {10,11};
Line(10) = {11,12};
Line(11) = {12,13};
Line(12) = {13,10};


Line Loop(100) = {1,2,3,4};
Line Loop(200) = {5,6,7,8};

Line Loop(300) = {9,10,11,12};

// -------------------
//  Surfaces
// -------------------

Plane Surface(100) = {100,200}; // matrix
Plane Surface(200) = {200};     // aggregate
Plane Surface(300) = {100,300}; // window

// ----------------------
// Physcial quantities
// ----------------------

Physical Surface(1) = {100};
Physical Surface(2) = {200};
Physical Surface(3) = {300};

// lower, right, upper, left edges
// opposite edges = same direction

Physical Line(1) = {9};
Physical Line(2) = {10};
Physical Line(3) = {11};
Physical Line(4) = {12};

// four corner nodes
// left bottom, anti-clockwise

//Physical Point(10) = {36};
//Physical Point(20) = {54};
//Physical Point(30) = {4};
//Physical Point(40) = {18};


