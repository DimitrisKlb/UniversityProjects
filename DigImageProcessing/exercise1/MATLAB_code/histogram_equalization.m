function A = histogram_equalization( a )
	[N1, N2] = size(a);
	[thePDF, x] = imhist(a);
	
	thePDF = thePDF ./ (N1*N2);
	theCDF = cumsum(thePDF);
	A = theCDF(a+1);	
	A = uint8(floor(A * 255));

end