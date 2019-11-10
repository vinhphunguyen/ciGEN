l  = 1.;
w1 = 0.4;
w  = 0.5;


h = 1.;

n1 = 8;
n2 = 16;
n3 = 2;

Point(1) = {0,0,0,h};
Point(2) = {l,0,0,h};
Point(3) = {l,w1,0,h};
Point(4) = {0,w1,0,h};
Point(5) = {l,w,0,h};
Point(6) = {0,w,0,h};


Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};
Line(5) = {3,5};
Line(6) = {5,6};
Line(7) = {6,4};

Line Loop(1) = {1,2,3,4};
Line Loop(2) = {-3,5,6,7};

Plane Surface(1) = {1};
Plane Surface(2) = {2};

Transfinite Line{7,5}     = n1+1;
Transfinite Line{1,3,6}   = n2+1;
Transfinite Line{2,4}     = n3+1;

Transfinite Surface{1} = {1,2,3,4};
Transfinite Surface{2} = {3,5,6,4};

Recombine Surface{1,2};

Physical Point(111) = {5}; // force


Physical Line(333)  = {1}; // bottom edge
Physical Line(444)  = {4}; // left edge

Physical Surface(888) = {1}; // substrate
Physical Surface(999) = {2};
