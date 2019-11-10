a = 1;
r = 0.1;

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


// -------------------
// outer lines
// -------------------

Line(1) = {1,2};
Line(2) = {2,3};
Line(3) = {3,4};
Line(4) = {4,1};

Circle(5) = {6,5,7};
Circle(6) = {7,5,8};
Circle(7) = {8,5,9};
Circle(8) = {9,5,6};


Line Loop(100) = {1,2,3,4};
Line Loop(200) = {5,6,7,8};

// -------------------
//  Surfaces
// -------------------

Plane Surface(200) = {200};     // aggregate

// ----------------------
// Physcial quantities
// ----------------------

// lower, right, upper, left edges
// opposite edges = same direction

Physical Line(1) = {1};
Physical Line(2) = {2};
Physical Line(3) = {3};
Physical Line(4) = {4};

// four corner nodes
// left bottom, anti-clockwise

//Physical Point(10) = {36};
//Physical Point(20) = {54};
//Physical Point(30) = {4};
//Physical Point(40) = {18};


Translate {0.3, 0.3, 0} {
  Surface{200};
}
Translate {-0.3, -0.3, 0} {
  Duplicata { Surface{200}; }
}

Translate {0, -0.5, 0} {
  Duplicata { Surface{200}; }
}
Translate {-0.5, -0., 0} {
  Duplicata { Surface{206}; }
}
Translate {-0.5, -0., 0} {
  Duplicata { Surface{200}; }
}
Translate {0.2, -0.13, 0} {
  Duplicata { Surface{211}; }
}
Translate {-0.18, -0.14, 0} {
  Duplicata { Surface{216}; }
}
Translate {-0.05, -0.22, 0} {
  Duplicata { Surface{200}; }
}
Translate {-0.24, -0., 0} {
  Duplicata { Surface{200}; }
}
Translate {-0.05, -0.2, 0} {
  Duplicata { Surface{206}; }
}
Translate {0, 0.07, 0} {
  Surface{206};
}
Translate {0, 0.02, 0} {
  Surface{241};
}
Translate {-0.1, -0.18, 0} {
  Duplicata { Surface{211}; }
}
Line Loop(251) = {228, 229, 230, 227};
Line Loop(252) = {218, 219, 220, 217};
Line Loop(253) = {238, 239, 240, 237};
Line Loop(254) = {233, 234, 235, 232};
Line Loop(255) = {202, 203, 204, 205};
Line Loop(256) = {213, 214, 215, 212};
Line Loop(257) = {248, 249, 250, 247};
Line Loop(258) = {223, 224, 225, 222};
Line Loop(259) = {208, 209, 210, 207};
Line Loop(260) = {243, 244, 245, 242};
Plane Surface(261) = {100, 200, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260};
Physical Surface(262) = {261};
Physical Surface(263) = {226, 216, 236, 200, 231, 201, 206, 211, 246, 221, 241};

Field[1] = Attractor;
Field[1].EdgesList={5,6,7,8}; // node 5, center of the circle is attractor

Field[2] = Threshold;
Field[2].IField=1;
Field[2].LcMin=h2;
Field[2].LcMax=h;
Field[2].DistMin=0.1*r;
Field[2].DistMax=0.3*r;

Field[3] = Attractor;
Field[3].EdgesList={237,238,239,240}; // node 5, center of the circle is attractor

Field[4] = Threshold;
Field[4].IField=3;
Field[4].LcMin=h2;
Field[4].LcMax=h;
Field[4].DistMin=0.1*r;
Field[4].DistMax=0.3*r;

Field[5] = Attractor;
Field[5].EdgesList={217,218,219,220}; // node 5, center of the circle is attractor

Field[6] = Threshold;
Field[6].IField=3;
Field[6].LcMin=h2;
Field[6].LcMax=h;
Field[6].DistMin=0.1*r;
Field[6].DistMax=0.3*r;

Field[7] = Attractor;
Field[7].EdgesList={227,228,229,230}; // node 5, center of the circle is attractor

Field[8] = Threshold;
Field[8].IField=3;
Field[8].LcMin=h2;
Field[8].LcMax=h;
Field[8].DistMin=0.1*r;
Field[8].DistMax=0.3*r;



Field[9] = Min;
Field[9].FieldsList = {2, 4,6,8};

Background Field = 9;

Mesh.CharacteristicLengthExtendFromBoundary = 0;
