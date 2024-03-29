h = 1;

nx = 31;
ny = 31;

Point(1) = {0,0,0,h};
Point(2) = {48,44,0,h};
Point(3) = {48,60,0,h};
Point(4) = {0,44,0,h};

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};

Line Loop(1) = {1,2,3,4};

Plane Surface(1) = {1};

Transfinite Line{1,3} = nx;
Transfinite Line{2,4} = ny;

Transfinite Surface{1} = {1,2,3,4};
Recombine Surface{1};

Physical Point(333) = {3};

Physical Line(111) = {4};
Physical Line(222) = {2};

Physical Surface(888) = {1};

