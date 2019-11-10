// Gmsh project created on Thu Jul 31 17:41:18 2014
Point(1) = {0, 0, 0, 1.0};
Point(2) = {1, 0, 0, 1.0};
Point(3) = {1, 1, 0, 1.0};
Point(4) = {0, 1, 0, 1.0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Line Loop(5) = {3, 4, 1, 2};
Plane Surface(6) = {5};
Extrude {0, 0, 0.5} {
  Surface{6};
}
Surface Loop(29) = {19, 6, 15, 28, 23, 27};
Volume(30) = {29};
Translate {0, 0, 0.5} {
  Duplicata { Volume{1}; }
}
Physical Volume(58) = {31};
Physical Volume(59) = {1};

Transfinite Line {3, 4, 14, 9, 18, 3, 8, 13, 11, 10, 22, 1, 2, 44, 39, 38, 46, 41, 40, 49, 54} = 3 Using Progression 1;
Transfinite Surface {47};
Transfinite Surface {42};
Transfinite Surface {37};
Transfinite Surface {52};
Transfinite Surface {57};
Transfinite Surface {28};
Recombine Surface {47, 42, 37, 52, 57, 28};
Transfinite Volume{31} = {6, 32, 36, 10, 5, 31, 40, 14};
Transfinite Surface {19};
Transfinite Surface {15};
Transfinite Surface {6};
Transfinite Surface {23};
Transfinite Surface {27};
Recombine Surface {19, 15, 28, 23, 6, 27};
Transfinite Volume{1} = {4, 6, 10, 1, 3, 5, 14, 2};
