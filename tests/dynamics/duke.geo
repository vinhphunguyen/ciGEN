l = 3.;
w = l/2;
a = 0.25;

h  = 0.09;
h2 = h/6;

Point(1)  = {0,-w,0,h};
Point(2)  = {l,-w,0,h};
Point(3)  = {l,w,0,h};
Point(4)  = {0,w,0,h};
Point(5)  = {0,0,0,h2};
Point(6)  = {a,0,0,h2};
Point(7)  = {l,0,0,h2};

Line(1) = {1,2};
Line(2) = {2,7};
Line(3) = {7,3};
Line(4) = {3,4};
Line(5) = {4,5};
Line(6) = {5,1};
Line(7) = {5,6};
Line(8) = {6,7};

Line Loop(1) = {1,2,-8,-7,6};
Line Loop(2) = {7,8,3,4,5};

Plane Surface(1) = {1};
Plane Surface(2) = {2};

Recombine Surface{1,2};

Physical Line(111)  = {1}; // bottom edge
Physical Line(222)  = {4}; // top edge
Physical Line(333)  = {5,6}; // left edge

Physical Surface(888) = {1}; 
Physical Surface(999) = {2};
