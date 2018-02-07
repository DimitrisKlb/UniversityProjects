function [ A ] = median_filter( a, dim )	
	d = floor(dim/2);	
	a = padarray(a, [d d], 'replicate');
	[N1, N2] = size(a);
	A = zeros(N1, N2);
	
	for row=1+d : 1 : N1-d
		for col=1+d : 1 : N2-d		
			newVal = a(row-d:row+d, col-d:col+d);
			newVal = sort( newVal(:) );
			middle = ceil(length(newVal)/2);
			A(row, col) = newVal(middle);
		end
	end	
	A = A(1+d:end-d, 1+d:end-d);	
end	