function A = local_histogram_equalization( a, dim )	
	d = floor(dim/2);
	a = padarray(a, [d d], 'replicate');
	[N1, N2] = size(a);
	A = zeros(N1, N2);
	nhoodArea = dim*dim;
	
	for row=1+d : 1 : N1-d
		for col=1+d : 1 : N2-d		
			nhood = a(row-d:row+d, col-d:col+d);
			%neig = histeq(nhood);
			
			[thePDF, x] = imhist(nhood);			
			%thePDF = thePDF ./ nhoodArea;
			theCDF = cumsum(thePDF);
			newVal = theCDF( nhood(d+1,d+1)+1 );		
			
			A(row, col) = floor(newVal/nhoodArea*255);
		end
	end	
	A = A(1+d:end-d, 1+d:end-d);
	A = uint8(A);	
end