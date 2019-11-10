ri = 80;
ro = 150;
t  = 2;

h  = 1.7;
h2 = h/7;

Point(1)  = {0,0,0,h};
Point(2)  = {ri,0,0,h};
Point(3)  = {0,ri,0,h};
Point(4)  = {-ri,0,0,h};
Point(5)  = {0,-ri,0,h};

Circle(1) = {2,1,3};
Circle(2) = {3,1,4};
Circle(3) = {4,1,5};
Circle(4) = {5,1,2};

Line Loop(1) = {1,2,3,4};

Point(6)  = {ro,0,0,h};
Point(7)  = {0,ro,0,h};
Point(8)  = {-ro,0,0,h};
Point(9)  = {0,-ro,0,h};

Circle(5) = {6,1,7};
Circle(6) = {7,1,8};
Circle(7) = {8,1,9};
Circle(8) = {9,1,6};

Line Loop(2) = {5,6,7,8};

Point(20)  = {ri-t,0,0,h};
Point(30)  = {0,ri-t,0,h};
Point(40)  = {-ri+t,0,0,h};
Point(50)  = {0,-ri+t,0,h};

Circle(10) = {20,1,30};
Circle(20) = {30,1,40};
Circle(30) = {40,1,50};
Circle(40) = {50,1,20};

Line Loop(10) = {10,20,30,40};

Plane Surface(1) = {1,2};
Plane Surface(2) = {1,10};


Physical Line(111)  = {10,20,30,40}; // force edge
Physical Line(222)  = {5,6,7,8}; 

Physical Surface(888) = {1}; 
Physical Surface(999) = {2}; 
