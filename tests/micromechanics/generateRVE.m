%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read a file with information of 2D circular inclusions (center and
% radius). Write a GMSH geo file for such a geometry--inclusions inside a
% square box. Case where the inclusions cut the box is allowed.
%
% Limitation: the process is not 100% automatic. You will have to run this
% file, open the generated geo file with GMSH and create a plane surface
% for the matrix, edit the geo file correspondingly. See line # 465 below.
%
% Written by:
% Vinh Phu Nguyen, nvinhphu@gmail.com, phu.nguyen@adelaide.edu.au
% The University of Adelaide, July 2015.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% (make sections)

fileID = fopen('N60_12.dat','r');
formatSpec = '%f %f %f';
sizeA = [3 Inf]; % column order, [x y radius]

A = fscanf(fileID,formatSpec,sizeA);
A = A';
fclose(fileID);

% since the inclusions are generated for DEM, they are all in contact
% scale their radius to separate them from each other

factor = 0.9;

A(:,3) = A(:,3)*factor;

numberOfInclusions = size(A,1);

th = 0:pi/50:2*pi;

% plot the inclusions

figure(1)
hold on

for i=1:numberOfInclusions
  xunit = A(i,3) * cos(th) + A(i,1);
  yunit = A(i,3) * sin(th) + A(i,2);
  plot(xunit, yunit);
end
axis equal

% plot the outer box

deltaX = -0.03;    % positive => expansion
deltaY = -0.03;    % negative => compaction
L      = 1;

% lower left and upper right points

x1   = [-deltaX -deltaY];
x3   = [L+deltaX L+deltaY];

plot([x1(1) x3(1)],[x1(2) x1(2)]);
plot([x3(1) x3(1)],[x1(2) x3(2)]);
plot([x3(1) x1(1)],[x3(2) x3(2)]);
plot([x1(1) x1(1)],[x3(2) x1(2)]);
%%

origin1     = x1;
origin2     = x3;

bottomLine = [origin1 1  0];
rightLine  = [origin2 0 -1];
topLine    = [origin2 -1 0];
leftLine   = [origin1 0 1];

cuts = []; % indices of circles cut by the box

lowerLeft   = [];
lowerRight  = [];
upperLeft   = [];
upperRight  = [];

for i=1:numberOfInclusions
  x =  A(i,1);
  y =  A(i,2);
  r =  A(i,3);
  
  center = [x y];
  circle = [center r];
  
  pts1    = intersectLineCircle(bottomLine, circle);
  pts2    = intersectLineCircle(rightLine,  circle);
  pts3    = intersectLineCircle(leftLine,   circle);
  pts4    = intersectLineCircle(topLine,    circle);
  
  % not cut the box
  
  if ( isnan(pts1(1,1)) && isnan(pts2(1,1)) && ...
      isnan(pts3(1,1)) && isnan(pts4(1,1)) )
    continue;
  end
  
  % cut by both bottom and left edges
  
  if     ( isnan(pts1(1,1))==0 && isnan(pts3(1,1))==0 )
    lowerLeft = [lowerLeft; i];
  elseif ( isnan(pts1(1,1))==0 && isnan(pts2(1,1))==0 )
    lowerRight = [lowerRight; i];
  elseif ( isnan(pts4(1,1))==0 && isnan(pts3(1,1))==0 )
    upperLeft = [upperLeft; i];
  elseif ( isnan(pts4(1,1))==0 && isnan(pts2(1,1))==0 )
    upperRight = [upperRight; i];
  else
    cuts = [cuts;i];
  end
end

%% plot only those inclusions not cut by the box
figure(2)
hold on

for i=1:numberOfInclusions
  if ismember(i,cuts) continue; end
  xunit = A(i,3) * cos(th) + A(i,1);
  yunit = A(i,3) * sin(th) + A(i,2);
  plot(xunit, yunit);
end
axis equal

% plot the outer box

plot([x1(1) x3(1)],[x1(2) x1(2)]);
plot([x3(1) x3(1)],[x1(2) x3(2)]);
plot([x3(1) x1(1)],[x3(2) x3(2)]);
plot([x1(1) x1(1)],[x3(2) x1(2)]);
%%

%%%%%%%%%%%%%%
% make a Gmsh geo file

gmshFile = fopen('N60.geo','w');

h = 0.03;
fprintf(gmshFile, 'h=%f;\n',h);
fprintf(gmshFile, 'h2=%f;\n',h*2);

for i=1:numberOfInclusions
  
  % if this inclusion cuts the box, needs special treatment...
  if ismember(i,cuts)       continue; end
  if ismember(i,lowerLeft)  continue; end
  if ismember(i,lowerRight) continue; end
  if ismember(i,upperLeft)  continue; end
  if ismember(i,upperRight) continue; end
  
  x = A(i,1);
  y = A(i,2);
  r = A(i,3);
  
  fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
  fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+1,x+r,y);
  fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+2,x,y+r);
  fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x-r,y);
  fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,x,y-r);
  
  fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+1,10*i,10*i+2);
  fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+2,10*i,10*i+3);
  fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,10*i+3,10*i,10*i+4);
  fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,10*i+4,10*i,10*i+1);
  
  fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i};\n',i,10*i,10*i+1,10*i+2,10*i+3);
  
  fprintf(gmshFile, 'Plane Surface(%i)={%i};\n',i,i);
end

%%
% treat inclusions cutting the box

topKeys   = [];
topVals   = [];
botKeys   = [];
botVals   = [];
leftKeys  = [];
leftVals  = [];
rightKeys = [];
rightVals = [];

for ii = 1:length(cuts)
  i = cuts(ii);
  x = A(i,1);
  y = A(i,2);
  r = A(i,3);
  
  center = [x y];
  circle = [center r];
  
  pts1    = intersectLineCircle(bottomLine, circle);
  pts2    = intersectLineCircle(rightLine,  circle);
  pts3    = intersectLineCircle(leftLine,   circle);
  pts4    = intersectLineCircle(topLine,    circle);
  
  % cut by the top edge
  
  if ( ~isnan(pts4(1,1)) )
    [ymax,imax] = max(pts4(:,1)); 
    [ymin,imin] = min(pts4(:,1)); 
    
    if ( y < x3(2) )                   % large part remains in the box
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+1,x+r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+2,x-r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x,y-r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts4(imin,1),pts4(imin,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts4(imax,1),pts4(imax,2));
      
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+4,10*i,10*i+2);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+2,10*i,10*i+3);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,10*i+3,10*i,10*i+1);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,10*i+1,10*i,10*i+5);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+5,10*i+4);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i};\n',i,10*i,10*i+1,10*i+2,10*i+3,100*i+15);
      
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
      
    else
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x,y-r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts4(imin,1),pts4(imin,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts4(imax,1),pts4(imax,2));
      
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+4,10*i,10*i+3);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+3,10*i,10*i+5);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+5,10*i+4);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,10*i+1,100*i+15);
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    end
    
    topKeys = [topKeys;pts4(imax,1);pts4(imin,1)];
    topVals = [topVals;10*i+5;10*i+4];
  end
  
  % cut bottom edge
  if ( ~isnan(pts1(1,1)) )
    [ymax,imax] = max(pts1(:,1)); 
    [ymin,imin] = min(pts1(:,1));  
    
    if ( y > x1(2) )                   % large part remains in the box
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+1,x+r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+2,x-r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x,y+r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts1(imin,1),pts1(imin,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts1(imax,1),pts1(imax,2));
      
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+5,10*i,10*i+1);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+1,10*i,10*i+3);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,10*i+3,10*i,10*i+2);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,10*i+2,10*i,10*i+4);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+4,10*i+5);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i};\n',i,10*i,10*i+1,10*i+2,10*i+3,100*i+15);
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    else
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x,y+r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts1(2,1),pts1(2,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts1(1,1),pts1(1,2));
      
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+4,10*i,10*i+3);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+3,10*i,10*i+5);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+5,10*i+4);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,10*i+1,100*i+15);
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    end
    
    botKeys = [botKeys;pts1(imax,1);pts1(imin,1)];
    botVals = [botVals;10*i+5;10*i+4];
  end
  
  % cut left edge
  if ( ~isnan(pts3(1,1)) )
    [ymax,imax] = max(pts3(:,2)); 
    [ymin,imin] = min(pts3(:,2));    
  
    if ( x > x1(1) )                   % large part remains in the box
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+1,x+r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+2,x,y+r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x,y-r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts3(imax,1),pts3(imax,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts3(imin,1),pts3(imin,2));
      
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+1,10*i,10*i+2);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+2,10*i,10*i+4);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,10*i+5,10*i,10*i+3);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,10*i+3,10*i,10*i+1);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+4,10*i+5);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i};\n',i,10*i,10*i+1,10*i+2,10*i+3,100*i+15);
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    else
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x+r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts3(imax,1),pts3(imax,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts3(imin,1),pts3(imin,2));
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+3,10*i,10*i+4);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+5,10*i,10*i+3);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+4,10*i+5);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,10*i+1,100*i+15);
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    end
    
    leftKeys = [leftKeys;pts3(imin,2);pts3(imax,2)];
    leftVals = [leftVals;10*i+5;10*i+4];
  end
  
  % cut right edge
  if ( ~isnan(pts2(1,1)) )
    [ymax,imax] = max(pts2(:,2)); 
    [ymin,imin] = min(pts2(:,2));
    
    if ( x < x3(1) )                   % large part remains in the box
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+1,x-r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+2,x,y+r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x,y-r);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts2(imax,1),pts2(imax,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts2(imin,1),pts2(imin,2));
      
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+4,10*i,10*i+2);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+2,10*i,10*i+1);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,10*i+1,10*i,10*i+3);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,10*i+3,10*i,10*i+5);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+5,10*i+4);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i};\n',i,10*i,10*i+1,10*i+2,10*i+3,100*i+15);
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    else
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x-r,y);
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts2(imax,1),pts2(imax,2));
      fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts2(imin,1),pts2(imin,2));
      
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+4,10*i,10*i+3);
      fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,10*i+3,10*i,10*i+5);
      
      fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+15,10*i+5,10*i+4);
      fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,10*i+1,100*i+15);
      fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    end
    
    rightKeys = [rightKeys;pts2(imin,2);pts2(imax,2)];
    rightVals = [rightVals;10*i+5;10*i+4];
  end
end

fprintf(gmshFile, 'Point(%i)={%22.4E,%22.4E,0,h};\n',  1,  x1(1),x1(2) );
fprintf(gmshFile, 'Point(%i)={%22.4E,%22.4E,0,h};\n',  2,  x3(1),x1(2) );
fprintf(gmshFile, 'Point(%i)={%22.4E,%22.4E,0,h};\n',  3,  x3(1),x3(2) );
fprintf(gmshFile, 'Point(%i)={%22.4E,%22.4E,0,h};\n\n',4,  x1(1),x3(2) );

%%
% treat inclusions cut both edges i.e. corners of the box
% four cases since there are 4 corners

for ii = 1:length(lowerRight)
  i = lowerRight(ii);
  x = A(i,1);
  y = A(i,2);
  r = A(i,3);
  
  center = [x y];
  circle = [center r];
  
  pts1    = intersectLineCircle(bottomLine, circle);
  pts2    = intersectLineCircle(rightLine,  circle);
  
  [xmax,imax] = max(pts1(:,1)); [xmin,imin] = min(pts1(:,1));
  [ymax,jmax] = max(pts2(:,2)); [ymin,jmin] = min(pts2(:,2));
  
  if ( xmax > x3(1)  )  % corner inside the inclusion
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x3(1),x1(2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts1(imin,1),pts1(imin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts2(jmax,1),pts2(jmax,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+5,10*i,10*i+4);    
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+4,2);
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,2,10*i+5);
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,100*i+35,100*i+36);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    
    botKeys   = [botKeys;pts1(imin,1)]; botVals = [botVals;10*i+4];
    rightKeys = [rightKeys;pts2(jmax,2)]; rightVals = [rightVals;10*i+5];
    
  else
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,   x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+1, x,y+r);
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+2, x-r,y);    
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,pts1(imin,1),pts1(imin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts1(imax,1),pts1(imax,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts2(jmin,1),pts2(jmin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+6,pts2(jmax,1),pts2(jmax,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+6,10*i,10*i+1);    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,  10*i+1,10*i,10*i+2);    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,  10*i+2,10*i,10*i+3);    
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+3,10*i+4);
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,  10*i+4,10*i,10*i+5); 
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,10*i+5,10*i+6);
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i,%i};\n',i,10*i,10*i+1,10*i+2,100*i+35,10*i+4,100*i+36);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    
    botKeys   = [botKeys;pts1(imin,1);pts1(imax,1)];   botVals   = [botVals;10*i+3;10*i+4];
    rightKeys = [rightKeys;pts2(jmin,2);pts2(jmax,2)]; rightVals = [rightVals;10*i+5;10*i+6];
  end
end

for ii = 1:length(lowerLeft)
  i = lowerLeft(ii);
  x = A(i,1);
  y = A(i,2);
  r = A(i,3);
  
  center = [x y];
  circle = [center r];
  
  pts1    = intersectLineCircle(bottomLine, circle);
  pts2    = intersectLineCircle(leftLine,   circle);
  
  [xmax,imax] = max(pts1(:,1)); [xmin,imin] = min(pts1(:,1));
  [ymax,jmax] = max(pts2(:,2)); [ymin,jmin] = min(pts2(:,2));
  
  if ( xmin < x1(1)  )  % corner inside the inclusion
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x1(1),x1(2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts1(imax,1),pts1(imax,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts2(jmax,1),pts2(jmax,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+4,10*i,10*i+5);    
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+5,1);
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,1,10*i+4);
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,100*i+35,100*i+36);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    
    botKeys   = [botKeys;pts1(imax,1)];   botVals  = [botVals;10*i+4];
    leftKeys  = [leftKeys;pts2(jmax,2)];  leftVals = [leftVals;10*i+5];
    
  else
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,   x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+1, x+r,y);
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+2, x,y+r);    
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,pts2(jmax,1),pts2(jmax,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts2(jmin,1),pts2(jmin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts1(imin,1),pts1(imin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+6,pts1(imax,1),pts1(imax,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,    10*i+1,10*i,10*i+2);    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,  10*i+2,10*i,10*i+3);           
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+3,10*i+4);
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,  10*i+4,10*i,10*i+5); 
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,10*i+5,10*i+6);
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,  10*i+6,10*i,10*i+1); 
    
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i,%i};\n',i,10*i,10*i+1,100*i+35,10*i+2,100*i+36,10*i+3);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    
    botKeys  = [botKeys;pts1(imin,1);pts1(imax,1)];  botVals   = [botVals;10*i+5;10*i+6];
    leftKeys = [leftKeys;pts2(jmin,2);pts2(jmax,2)]; leftVals = [leftVals;10*i+4;10*i+3];
    
  end
end

for ii = 1:length(upperLeft)
  i = upperLeft(ii);
  x = A(i,1);
  y = A(i,2);
  r = A(i,3);
  
  center = [x y];
  circle = [center r];
  
  pts1    = intersectLineCircle(topLine, circle);
  pts2    = intersectLineCircle(leftLine,circle);
  
  [xmax,imax] = max(pts1(:,1)); [xmin,imin] = min(pts1(:,1));
  [ymax,jmax] = max(pts2(:,2)); [ymin,jmin] = min(pts2(:,2));
  
  if ( xmin < x1(1)  )  % corner inside the inclusion
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x1(1),x3(2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts1(imax,1),pts1(imax,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts2(jmin,1),pts2(jmin,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+5,10*i,10*i+4);    
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+4,4);
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,4,10*i+5);
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,100*i+35,100*i+36);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    
    topKeys   = [topKeys;pts1(imax,1)];   topVals  = [topVals;10*i+4];
    leftKeys  = [leftKeys;pts2(jmin,2)];  leftVals = [leftVals;10*i+5];
  else
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,   x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+1, x+r,y);
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+2, x,y-r);    
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,pts2(jmax,1),pts2(jmax,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts2(jmin,1),pts2(jmin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts1(imin,1),pts1(imin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+6,pts1(imax,1),pts1(imax,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,    10*i+1,10*i,10*i+6);    
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+6,10*i+5);
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,  10*i+5,10*i,10*i+3);           
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,10*i+3,10*i+4);
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,  10*i+4,10*i,10*i+2);     
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,  10*i+2,10*i,10*i+1); 
    
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i,%i};\n',i,10*i,100*i+35,10*i+1,100*i+36,10*i+2,10*i+3);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n\n',i,i);
    
    topKeys  = [topKeys;pts1(imin,1);pts1(imax,1)];  topVals   = [topVals;10*i+5;10*i+6];
    leftKeys = [leftKeys;pts2(jmin,2);pts2(jmax,2)]; leftVals = [leftVals;10*i+4;10*i+3];
  end
end

for ii = 1:length(upperRight)
  i = upperRight(ii);
  x = A(i,1);
  y = A(i,2);
  r = A(i,3);
  
  center = [x y];
  circle = [center r];
  
  pts1    = intersectLineCircle(topLine,  circle);
  pts2    = intersectLineCircle(rightLine,circle);
  
  [xmax,imax] = max(pts1(:,1)); [xmin,imin] = min(pts1(:,1));
  [ymax,jmax] = max(pts2(:,2)); [ymin,jmin] = min(pts2(:,2));
  
  if ( xmin < x1(1)  )  % corner inside the inclusion
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,  x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,x3(1),x3(2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts1(imin,1),pts1(imin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts2(jmin,1),pts2(jmin,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,  10*i+4,10*i,10*i+5);    
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+5,3);
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,3,10*i+4);
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i};\n',i,10*i,10*i+35,100*i+36);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n',i,i);
    
    topKeys   = [topKeys;pts1(imin,1)];   topVals  = [topVals;10*i+4];
    rightKeys  = [rightKeys;pts2(jmin,2)];  rightVals = [rightVals;10*i+5];
  else
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i,   x,y  );
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+1, x+r,y);
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h2};\n',10*i+2, x,y-r);    
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+3,pts2(jmax,1),pts2(jmax,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+4,pts2(jmin,1),pts2(jmin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+5,pts1(imin,1),pts1(imin,2));
    fprintf(gmshFile, 'Point(%i)={%22.14E,%22.14E,0,h};\n',10*i+6,pts1(imax,1),pts1(imax,2));
    
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i,    10*i+1,10*i,10*i+6);    
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+35,10*i+6,10*i+5);
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+1,  10*i+5,10*i,10*i+3);           
    fprintf(gmshFile, 'Line(%i)={%i,%i};\n',100*i+36,10*i+3,10*i+4);
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+2,  10*i+4,10*i,10*i+2);     
    fprintf(gmshFile, 'Circle(%i)={%i,%i,%i};\n',10*i+3,  10*i+2,10*i,10*i+1); 
    
    
    fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i,%i,%i};\n',i,10*i,100*i+35,10*i+1,100*i+36,10*i+2,10*i+3);
    fprintf(gmshFile, 'Plane Surface(%i)={%i};\n',i,i);
    
    topKeys   = [topKeys;pts1(imin,1);pts1(imax,1)];  topVals   = [topVals;10*i+5;10*i+6];
    rightKeys = [rightKeys;pts2(jmin,2);pts2(jmax,2)]; rightVals = [rightVals;10*i+4;10*i+3];
  end
end

left   = [leftKeys leftVals];
right  = [rightKeys rightVals];
bottom = [botKeys botVals];
top    = [topKeys topVals];

% sort intersection points

% default values ( if not cut )

leftNodes   = [4;1];
rightNodes  = [2;3];
bottomNodes = [1;2];
topNodes    = [3;4];

% modify to take intersections into account

if (~isempty(left))
  [b,Il]  = sort(left(:,1),  'descend');
  leftNodes  = [4;left(Il,2);1];
end
if (~isempty(right))
  [b,Ir]  = sort(right(:,1), 'ascend');
  rightNodes = [2;right(Ir,2);3];
end
if (~isempty(bottom))
  [b,Ib]  = sort(bottom(:,1),'ascend');
  bottomNodes   = [1;bottom(Ib,2);2];
end
if (~isempty(top))
  [b,It]  = sort(top(:,1),   'descend');
  topNodes   = [3;top(It,2);4];
end

fprintf(gmshFile, '\n\n');

% use an array to faciliate Plane definition later

fprintf(gmshFile, 'For i In {1:%i}\n',numberOfInclusions);
fprintf(gmshFile, 'inclusions[i]=i;\n');
fprintf(gmshFile, 'fields[i]=2*i;\n');
fprintf(gmshFile, 'EndFor\n\n');

% another solution

for i=1:numberOfInclusions
  fprintf(gmshFile, 'Point{%i} In Surface {%i};\n',10*i,i);
end

% put the inclusions in a box (1,2,3,4)
% 1. write 4 corner points
% 2. write Lines for all the edges. Note that as there are intersections
% with inclusions, the bottom edge, for example, is not simply joining
% point 1 and point 2.



j = 1000;
c = 0;

for i=1:length(bottomNodes)-1
  fprintf(gmshFile, 'Line(%i)={%i,%i};\n',1000+i,bottomNodes(i),bottomNodes(i+1));
  c = c + 1;
end

fprintf(gmshFile, 'For i In {1:%i}\n',length(bottomNodes)-1);
fprintf(gmshFile, 'bottomEdge[i]=1000+i;\n');
fprintf(gmshFile, 'EndFor\n\n');

for i=1:length(topNodes)-1
  fprintf(gmshFile, 'Line(%i)={%i,%i};\n',2000+i,topNodes(i),topNodes(i+1));
  c = c + 1;
end

fprintf(gmshFile, 'For i In {1:%i}\n',length(topNodes)-1);
fprintf(gmshFile, 'topEdge[i]=2000+i;\n');
fprintf(gmshFile, 'EndFor\n\n');

for i=1:length(leftNodes)-1
  fprintf(gmshFile, 'Line(%i)={%i,%i};\n',3000+i,leftNodes(i),leftNodes(i+1));
  c = c + 1;
end

fprintf(gmshFile, 'For i In {1:%i}\n',length(leftNodes)-1);
fprintf(gmshFile, 'leftEdge[i]=3000+i;\n');
fprintf(gmshFile, 'EndFor\n\n');

for i=1:length(rightNodes)-1
  fprintf(gmshFile, 'Line(%i)={%i,%i};\n',4000+i,rightNodes(i),rightNodes(i+1));
  c = c + 1;
end

fprintf(gmshFile, 'For i In {1:%i}\n',length(rightNodes)-1);
fprintf(gmshFile, 'rightEdge[i]=4000+i;\n');
fprintf(gmshFile, 'EndFor\n\n');

% if no inclusions cut the edges...

if isempty(cuts)
  fprintf(gmshFile, 'Line Loop(%i)={%i,%i,%i,%i};\n',1000,1,2,3,4);
end

% the following is not true if there are intersections
% However it is not easy to automate this as the outer boundary for the
% matrix is now complex--both lines and circular arcs.
% Current solution: (1) define the plane surface for the matrix in GMSH GUI
% as it automatically recognises connected lines/curves. (2) Use an editor to
% edit the following line of the geo file: replace 100 by the Line Loop that you have
% just made in the previous step (1).

fprintf(gmshFile, 'Plane Surface(1000)={1000,inclusions[{1:%i}]};\n\n',60);

% physical quantities

fprintf(gmshFile, 'Physical Line(%i)={bottomEdge[{1:%i}]};\n',1,length(bottomNodes)-1); % lower edge
fprintf(gmshFile, 'Physical Line(%i)={rightEdge[{1:%i}]};\n', 2,length(rightNodes)-1 ); % right edge
fprintf(gmshFile, 'Physical Line(%i)={topEdge[{1:%i}]};\n',   3,length(topNodes)-1   ); % upper edge
fprintf(gmshFile, 'Physical Line(%i)={leftEdge[{1:%i}]};\n',  4,length(leftNodes)-1  ); % left edge

fprintf(gmshFile, 'Physical Surface(%i)={%i};\n',1,1000); % matrix
fprintf(gmshFile, 'Physical Surface(%i)={inclusions[{1:%i}]};\n\n',2,numberOfInclusions);

% inclusions: coarse elements inside


% for i=1:numberOfInclusions
%     r = A(i,3);
%
%     fprintf(gmshFile, 'Field[%i]=Attractor;\n',2*i-1);
%     fprintf(gmshFile, 'Field[%i].EdgesList={%i,%i,%i,%i};\n',2*i-1,10*i,10*i+1,10*i+2,10*i+3);
%     fprintf(gmshFile, 'Field[%i]=Threshold;\n',2*i);
%     fprintf(gmshFile, 'Field[%i].IField=1;\n',2*i);
%     fprintf(gmshFile, 'Field[%i].LcMin=h;\n',2*i);
%     fprintf(gmshFile, 'Field[%i].LcMax=h2;\n',2*i);
%     fprintf(gmshFile, 'Field[%i].DistMin=%f;\n',2*i,0.1*r);
%     fprintf(gmshFile, 'Field[%i].DistMax=%f;\n',2*i,0.3*r);
% end
%
% fprintf(gmshFile, 'Field[%i]=Min;\n',2*numberOfInclusions+1);
% fprintf(gmshFile, 'Field[%i].FieldsList={fields[{1:%i}]};\n',2*numberOfInclusions+1,numberOfInclusions);
% fprintf(gmshFile, 'Background Field=%i;\n',2*numberOfInclusions+1);
% fprintf(gmshFile, 'Mesh.CharacteristicLengthExtendFromBoundary=0;\n');



