function [ shiftedA ] = shift_2D( A )	
	half = ceil(size(A)/2);
	shiftedA = A([half(1)+1:end, 1:half(1)], [half(2)+1:end, 1:half(2)]);
end	