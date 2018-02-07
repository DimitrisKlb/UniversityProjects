function Write_EPS(N,K,L,filename)
%
% Function Write_EPS
%
% Usage:
%       Write_EPS(N,K,L,filename)
%
% This function writes an Encapsulated PostScript file to disk, where the
% parameters are defined as:
%     - N : The height of the image
%     - K : The width of the image
%     - L : A matrix with four columns, where each line represents a line
%           segment (x0,y0)-(x1,y1), represented by [x0 y0 x1 y1].
%

% Open EPS file
FID=fopen(filename,'w');

% Write Header
fprintf(FID,'%%!PS-Adobe-3.0 EPSF-3.0\n');

% Write Bounding Box
fprintf(FID,'%%%%BoundingBox: 1 1 %i %i\n',K,N);

% Write each line
for l=1:size(L,1),
    fprintf(FID,'%f %f moveto\n',L(l,1),L(l,2));
    fprintf(FID,'%f %f lineto\n',L(l,3),L(l,4));
    fprintf(FID,'stroke\n');
end

% Write End of File
fprintf(FID,'%%%%EOF');

% Close output file
fclose(FID);