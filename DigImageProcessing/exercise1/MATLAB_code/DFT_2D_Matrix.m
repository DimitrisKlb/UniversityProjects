function [ A ] = DFT_2D_Matrix( a )	
	[N1,N2] = size(a);
	n1 = 0:N1-1;
	n2 = 0:N2-1;
	
	W = n1'*n2;	
	W = W .* ( 1i*(-2*pi/N1));
	W = exp(1) .^ (W);
	
	A = W*double(a)*transpose(W);
end	