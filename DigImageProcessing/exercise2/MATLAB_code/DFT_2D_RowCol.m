function [ A ] = DFT_2D_RowCol( a )	
	Y = transpose(fft(transpose(a)));
	A = fft(Y);
end	