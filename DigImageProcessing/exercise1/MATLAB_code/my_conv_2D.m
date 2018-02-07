function [ A ] = my_conv_2D( a, h )	
a = double(a);
h = double(h);
	d = floor(size(h,1)/2);	
	a = padarray(a, [d d], 'replicate');
	[N1, N2] = size(a);
	A = zeros(N1, N2);
	
	for row=1+d : 1 : N1-d
		for col=1+d : 1 : N2-d		
			newVal = a(row-d:row+d, col-d:col+d) .* h;
			A(row, col) = sum( newVal(:) );
		end
	end	
	A = A(1+d:end-d, 1+d:end-d);	
end	