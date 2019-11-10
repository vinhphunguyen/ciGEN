l = 100.;
w = 3.0;
a = 30.; 

h = 1.;

Point(1) = {0,0,0,h};
Point(2) = {l,0,0,h};
Point(3) = {l,0.5*w,0,h};
Point(4) = {0,0.5*w,0,h};
Point(5) = {0,-0.5*w,0,h};
Point(6) = {l,-0.5*w,0,h};

Point(7) = {a,0,h};

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};
Line(5) = {2,6};
Line(6) = {6,5};
Line(7) = {5,1};

Line Loop(1) = {1,2,3,4};
Line Loop(2) = {1,5,6,7};

Plane Surface(1) = {1};
Plane Surface(2) = {2};

Transfinite Line{2,4,7,5} = 2;
Transfinite Line{1,3,6}   = 401;

Transfinite Surface{1} = {1,2,3,4};
Transfinite Surface{2} = {1,2,6,5};

Recombine Surface{1,2};

Physical Point(111) = {4}; // positive force
Physical Point(222) = {5}; // negative force

Physical Line(333)  = {2};
Physical Line(444)  = {5};

Physical Surface(888) = {1};
Physical Surface(999) = {2};
