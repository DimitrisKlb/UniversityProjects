%%% HiPC Parallel Programming Challenge


function fitCircles(file, plotData)
% main function to fit circles to input points
% inputs:
%  file     = the file containing the events information
%  plotData = whether to plot the final data or not
% outputs:
%  no outputs. This function just prints the circles

  % Read the events from the input file, store in a cell array
  result = {};
  events = readEvents(file);

  % Start clock
  tic();

  for e = 1:length(events)
    % X is the point array (two-dimensional for x and y)
    X = events{e};
    N = size(X, 1);

    rand('seed', 456);

    % Constants for this event
    % max possible circles
    maxK = 5;
    % min possible circles
    minK = 2;
    % list of K's to try out
    Ks = minK:maxK;
    % These parameters affect the accuracy and/or performance of the algorithm!
    % ignore circles less than this radius
    radiusThreshold = 0.1;
    % maximum iterations for closest circle algorithm
    maxIter = 100;
    % penalty for fitting large number of circles (to avoid over fitting)
    overfitPenalty = 0.001;

    % call K-closest circles to fit K circles
    % choose the one which yields the least error with fewest circles
    errors = ones(1,maxK) * inf;
    circles = {};
    for i = Ks
      [circles{i}, errors(i)] = kcc(X, i, maxIter, radiusThreshold, overfitPenalty);
    end
    [w, iw] = min(errors);
    result{e} = circles{iw};
  end

  % stop clock
  elapsed_time = toc();

  % print circles
  for e = 1:length(events)
    circles = result{e};
    printf("Event: %d\n", e)
    for i = 1:size(circles,1)
      printf("%f %f %f\n", circles(i,1), circles(i,2), circles(i,3))
    end
  end

  % print time
  printf("Time: %f ms\n", elapsed_time*1000);

  % draw only the first event
  % draw circle and its center along with the points
  if(plotData)
    plotCircles(events{1}, result{1});
  endif
end

function plotCircles(X, circles)
% Simple helper function to plot circles
% inputs:
%   X       = input points (Nx2)
%   circles = the circles (Kx3)
% outputs:
%   none.

  scatter(X(:,1), X(:,2));
  hold on;
  for i = 1:size(circles,1)
    t = [0:0.1:2*pi];
    x = circles(i,1);
    y = circles(i,2);
    r = circles(i,3);
    circsx = r.*cos(t) + x; 
    circsy = r.*sin(t) + y; 
    plot(circsx, circsy, linestyle='-');
  end
  hold off;
  axis equal;
end

function X = readEvents(file)
% Reads all events from the input file
% inputs:
%   file = the file containing the points for the events
% outputs:
%   X = the points in 2D co-ordinate system (Nx2)

  fp = fopen(file, 'r');
  Nevents = fscanf(fp, "%d", 1);
  for e = 1:Nevents
    Npoints = fscanf(fp, "%d", 1);
    for p = 1:Npoints
      x = fscanf(fp, "%f",2);
      X{e}(p,:) = x;
    end
  end
  fclose(fp);
end
