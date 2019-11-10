l = 100.;
w = 100.;

h = 1.;
t = 5.;

Point(1) = {0,0,0,h};
Point(2) = {l,0,0,h};
Point(3) = {l,w,0,h};
Point(4) = {0,w,0,h};
Point(5) = {0,0.5*w-t,0,h};
Point(6) = {l,0.5*w-t,0,h};
Point(7) = {0,0.5*w+t,0,h};
Point(8) = {l,0.5*w+t,0,h};

Line(1) = {1,2};
Line(2) = {2,6};
Line(3) = {6,8};
Line(4) = {8,3};
Line(5) = {3,4};
Line(6) = {4,7};
Line(7) = {7,5};
Line(8) = {5,1};
Line(9) = {5,6};
Line(10) = {7,8};

Line Loop(1) = {1,2,-9,8};
Line Loop(2) = {10,4,5,6};
Line Loop(3) = {9,3,-10,7};

Plane Surface(1) = {1};
Plane Surface(2) = {2};
Plane Surface(3) = {3};

Transfinite Line{2,8,6,4,7,3} = 2;
Transfinite Line{1,9,5,10}   = 2;

Transfinite Surface{1} = {1,2,6,5};
Transfinite Surface{2} = {7,8,3,4};

Recombine Surface{1,2,3};

Physical Line(333)  = {1};
Physical Line(444)  = {5};

Physical Surface(888) = {1,2};
Physical Surface(999) = {3};
