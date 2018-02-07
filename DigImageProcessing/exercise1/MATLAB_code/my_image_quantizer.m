function [channel_quant] = my_image_quantizer( channel, levels )
	[N1,N2] = size(channel);
	W = 256/levels;
	
	centers = zeros(1, levels, 'uint8');
	channel_quant = zeros(N1, N2, 'uint8');	
	
	centers(1) = ceil(W/2);
	for i=2 : 1 : levels
		centers(i) = centers(i-1) + W;
	end	
	
	for i=1 : 1 : N1
		for k=1 : 1 : N2
			channel_quant(i,k) = centers(floor( double(channel(i,k))/W ) + 1);
		end
	end	
end