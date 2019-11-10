L=2;
W=2;
//mesh size
M=1;

// -------------------
// corner points 
// -------------------

Point(1) = {0,0,0,M};
Point(2) = {L,0,0,M};
Point(3) = {L,W,0,M};
Point(4) = {0,W,0,M};
Point(5) = {0,0.5*W,0,M};
Point(6) = {L,0.5*W,0,M};



// -------------------
// outer lines
// -------------------
Line(1) = {1,2};
Line(2) = {2,6};
Line(3) = {6,3};
Line(4) = {3,4};
Line(5) = {4,5};
Line(6) = {5,1};
Line(7) = {5,6};

Line Loop(1) = {1,2,-7,6};
Line Loop(2) = {7,3,4,5};

// -------------------
//  Surfaces
// -------------------

Plane Surface(1) = {1};
Plane Surface(2) = {2};

//Assign mesh size to each line for structured meshes(this overrides any mesh size assigned to these entities before)
Transfinite Line{2,3,5,6} = 2 ;
Transfinite Line{1,7,4}   = 3;

//Transfinite surface:
//Specify points on the boundary of each surface;if not specifies program automatically detects
Transfinite Surface{1} = {1,2,6,5};
Transfinite Surface{2} = {5,6,3,4};

//To get structured mesh
Recombine Surface {1};
Recombine Surface {2};

// ----------------------
// Physcial quantities
// ----------------------

Physical Line(111) = {1}; // bottom edge
Physical Line(222) = {4}; // top edge
Physical Line(333) = {2,3}; // right edge
Physical Line(444) = {5,6}; // left edge


// Where the crack will grow and interface elements should be inserted
//Physical Line(777)  = {1};
//Assigns different groups to elements of each physical surface
//Physical Surface(888) = {1};
Physical Surface(999) = {1,2};

