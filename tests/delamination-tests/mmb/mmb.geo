L=100;
b=1.5;
a0=30;

h = 1;

Point(1)={0,-b,0,h};
Point(2)={L,-b,0,h};
Point(3)={0,b,0,h};
Point(4)={L,b,0,h};
Point(5)={0,0,0,h};
Point(6)={L,0,0,h};
Point(7)={0.5*L,b,0,h};
Point(8)={0.5*L,-b,0,h};
Point(9)={0.5*L,0,0,h};

Line(1)={1,8};
Line(2)={8,2};
Line(3)={5,9};
Line(10)={9,6};
Line(4)={3,7};
Line(5)={7,4};
Line(6)={5,1};
Line(7)={3,5};
Line(8)={2,6};
Line(9)={6,4};
Line(12)={9,7};
Line(13)={8,9};

Line Loop(1) = {3,-13,-1,-6};
Line Loop(2) = {3,12,-4,7};
Line Loop(3) = {10,-8,-2,13};
Line Loop(4) = {10,9,-5,-12};


Plane Surface(1) = {1};
Plane Surface(2) = {2};
Plane Surface(3) = {3};
Plane Surface(4) = {4};

Transfinite Line{1,3,2,4,5,10}=201;
Transfinite Line{6,7,8,9,12,13}=2;

Transfinite Surface{1} = {1,8,9,5};
Transfinite Surface{2} = {5,9,7,3};
Transfinite Surface{3} = {8,2,6,9};
Transfinite Surface{4} = {9,6,4,7};

Recombine Surface{1,2,3,4};


Physical Point(111) = {1}; // fixed
Physical Point(222) = {2}; // fixed
Physical Point(333) = {7}; // point load
Physical Point(444) = {4}; // point load


Physical Surface(999) = {1,3};
Physical Surface(888) = {2,4};
