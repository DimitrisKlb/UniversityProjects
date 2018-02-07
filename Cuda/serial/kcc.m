%%% HiPC Parallel Programming Challenge


function [circles, err] = kcc(X, K, maxIter, radiusThreshold, overfitPenalty)
  [circles1, points1] = closestCircles(X, K, maxIter, true);
  [circles1, points1] = pruneCircles(circles1, points1, radiusThreshold);
  err1 = circleFitError(X, points1, circles1, overfitPenalty, K);
  [circles2, points2] = closestCircles(X, K, maxIter, false);
  [circles2, points2] = pruneCircles(circles2, points2, radiusThreshold);
  err2 = circleFitError(X, points2, circles2, overfitPenalty, K);
  if(err1 < err2)
    err = err1;
    circles = circles1;
  else
    err = err2;
    circles = circles2;
  end
end

function [circles, points] = closestCircles(X, K, maxIter, initializeCirclesFirst)
% K-means extension for fitting circles to the data
% inputs:
%   X       = input data points (Nx2)
%   K       = number of circles to be fitted (1x1)
%   maxIter = run until these many iterations in the worst case (1x1)
%   initializeCirclesFirst = randomly initialize circles first, instead of partitioning points (boolean)
% outputs:
%   circles = the final circles (Kx3)
%   points = the assignment of points to these circles (1xN)

  N = size(X,1);
  % TODO: Investigate better initialization techniques for better convergence?
  if(initializeCirclesFirst)
    circles = rand(K, 3);
    % circle centers are assumed to be in the range [-1.0, 1.0]
    circles(:,1:2) = 2 * circles(:,1:2) - 1;
    numChanges = 1;
    points = zeros(1,N);
  else
    points = zeros(1,N);
    idx = randperm(N);
    cIdx = 1;
    for i = 1:N
      points(idx(i)) = cIdx;
      cIdx = cIdx + 1;
      if(cIdx > K)
        cIdx = 1;
      end
    end
    circles = fitCircles(X, points, K);
  end
  numChanges = 1;
  while((numChanges > 0) && (maxIter > 0))
    [points, numChanges] = findPoints(X, circles, points);
    circles = fitCircles(X, points, K);
    maxIter = maxIter - 1;
    %figure;
    %plotCircles(X, circles, points);
  end
end

function plotCircles(X, circles, points)
% Simple helper function to plot circles
% inputs:
%   X       = input points (Nx2)
%   circles = the circles (Kx3)
%   points  = circle-point assignment (1xN)
% outputs:
%   none.

  colors = [
    0.0 0.0 1.0;
    0.0 1.0 0.0;
    1.0 0.0 0.0;
    1.0 1.0 0.0;
    1.0 0.0 1.0;
  ];
  K = size(circles,1);
  for i = 1:K
    hold on;
    scatter(X(find(points==i),1), X(find(points==i),2), 8, colors(i,:));
    hold off;
  end
  hold on;
  for i = 1:size(circles,1)
    t = [0:0.1:2*pi];
    x = circles(i,1);
    y = circles(i,2);
    r = circles(i,3);
    circsx = r.*cos(t) + x; 
    circsy = r.*sin(t) + y; 
    plot(circsx, circsy, linestyle='-', 'Color', colors(i,:));
  end
  hold off;
  axis equal;
end

function circles = fitCircles(X, points, K)
% Function to fit circles to the given set of points
% inputs:
%   X      = the inputs (Nx3)
%   points = assignment of points to circles (1xN)
%   K      = number of circles to fit (1x1)
% outputs:
%   circles = the circles (Kx3)

  circles = zeros(K, 3);
  for i = 1:K
    pts = X(find(points == i), :);
    % we are assuming that need atleast 4 points to fit a circle
    % (we can fit an exact circle to 3 non-collinear points, BTW!)
    if(size(pts,1) < 4)
      circles(i,:) = [0, 0, 100];
    else
      [cx, cy, r] = fitring(pts);
      circles(i,:) = [cx, cy, r];
    end
  end
end


function [points, numChanges] = findPoints(X, circles, prev)
% assign points to their closest circles
% inputs:
%   X       = the points (Nx2)
%   circles = the circles (Kx3)
%   prev    = previous assignment of points (1xN)
% outputs:
%   points     = the new assignment of points to circles (1xN)
%   numChanges = number of migration of points wrt previous iteration (1x1)

  points = zeros(size(prev));
  numChanges = 0;
  N = size(X, 1);
  for i = 1:N
    d = circleDist(X(i,:), circles);
    [w, iw] = min(d);
    points(i) = iw;
    if(prev(i) != points(i))
      numChanges = numChanges + 1;
    end
  end
end


function [cx, cy, R] = fitring(X)
% Fits the input data points to the equation
%   x^2 + y^2 + a(1)*x + a(2)*y + a(3) = 0
% But this function returns circle center and radius
% as output. So that one can use the equation
%   (x - cx).^2 + (y - cy).^2 = R.^2
% inputs:
%   X = input data points of dimension (Nx2)
% outputs:
%   cx = center x-coordinate of the circle
%   cy = center y-coordinate of the circle
%   R  = circle radius

  x = X(:,1);
  y = X(:,2);
  a = [X, ones(size(x))] \ [-(x.^2 + y.^2)];
  cx = -0.5 * a(1);
  cy = -0.5 * a(2);
  R  = sqrt((a(1)^2+a(2)^2)/4-a(3));
end

function [prunedC, prunedP] = pruneCircles(circles, points, radiusThreshold)
% Prune circles with very small radius and ones with less than 4 points.
% inputs:
%   circles         = the circles (Kx3)
%   points          = point assignments to these circles (1xN)
%   radiusThreshold = ignore circles less than this radius (1x1)
% outputs:
%   prunedC  = pruned circles (Kx3) (this K might be less than or equal to the input!)
%   prundedP = pruned points (1xN)

  prunedC = [];
  prunedP = zeros(size(points));
  for i = 1:size(circles,1)
    locations = find(points == i);
    if(circles(i,3) < radiusThreshold)
      continue;
    endif
    if(length(locations) < 4)
      continue;
    endif
    prunedP(locations) = i;
    prunedC = [prunedC; circles(i,:)];
  end
end

function err = circleFitError(X, points, circles, overfitPenalty, K)
% Measure the error in circle fitting
% inputs:
%   X              = dataset (Nx2)
%   points         = assignment of each point to the circle (1xN)
%   circles        = the circles (Kx3)
%   overfitPenalty = penalty used to prevent overfitting (1x1)
    %   K              = number of circles requested to be fit (1x1)
% outputs:
%   err = the error/penalty in fitting of these circles to its points

  N = length(points);
  err = 0;
  for i = 1:N
    err += min(circleDist(X(i,:), circles));
  end
  err += (overfitPenalty * K * K);
end

function d = circleDist(point, circles)
% Function to find distance between a point from all circles
% inputs:
%   point   = the point (1x2)
%   circles = all circles (Kx3)
% outputs:
%   d = the distance of this point from all circles (Kx1)

  % Least Absolute Deviations measure
  % the function used to measure distance between a point from a circle is:
  %   ((x - a)^2 + (y - b)^2 - r^2)^2
  xa = point(1) - circles(:,1);
  yb = point(2) - circles(:,2);
  r = circles(:,3);
  d = (xa.^2 + yb.^2 - r.^2).^2;
end
