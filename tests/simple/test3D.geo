h = 2;


Point(1) = {0,0,0,h};
Point(2) = {1,0,0,h};
Point(3) = {1,1,0,h};
Point(4) = {0,1,0,h};

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};

Line Loop(1) = {1,2,3,4};

Plane Surface(1) = {1};

Extrude {0,0,0.2} {
   Surface{1}; //Layers{1};// Recombine;
}

Physical Surface(27) = {25};
Physical Surface(28) = {17};
Physical Volume(29) = {1};
