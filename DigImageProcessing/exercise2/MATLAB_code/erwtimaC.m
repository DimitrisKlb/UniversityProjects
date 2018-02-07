images = {'radiograph_1.jpg', 'radiograph_2.jpg'};

wmegas = ones(length(images), 2, 2);
sigmas = ones(length(images), 2);

wmegas(1, :, :) = [-0.1 (pi/4+0.08); 0.11 -(pi/4+0.05)];
sigmas(1, :) = [0.25; 0.25];

wmegas(2, :, :) = [(pi/6-0.04) -0.05; -(pi/6-0.04) 0.05];
sigmas(2, :) = [0.25; 0.25];

close all;
for im=1 : 1 : length(images)
	a = im2double(imread(images{im}));
	a = a(:,:,1); %It is grayscale

	H = ones(size(a));

%---------- Create the desired stop-band filter ----------
	[M,N] = size(a);
	Nw = size(wmegas, 2);
	[w1, w2] = meshgrid(1:N, 1:M);
	for i=1 : 1 : Nw
		w1n = (w1 - N/2)/N*2*pi;
		w2n = (w2 - M/2)/M*2*pi;
		
		r = (w1n - wmegas(im, i, 1)).^2 + (w2n - wmegas(im, i, 2)).^2;
		H = H .* (1 - exp( -r/sigmas(im, i)^2 ) );
	end	

	H = shift_2D(H);

%---------- Apply the filter ----------
	A = DFT_2D_RowCol(a);
	Amagn_log = shift_2D(log(abs(A)+1));
	%A = shift_2D(A);

	Y = A .* H;
	Ymagn_log = shift_2D(log(abs(Y)+1));

%---------- Display Results ----------
	y = ifft2(Y);
	figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	subplot(2,1,1), imshow(a, []), title('Before');
	subplot(2,1,2), imshow(y, []), title('After');
	print(gcf, '-dpng', sprintf('erwtimaC\\erwtimaC_%d_Tdomain.png', im));

	figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	subplot(2,1,1), imshow(Amagn_log, []), title('Before'); 
	subplot(2,1,2), imshow(Ymagn_log, []), title('After');
	imshow(Ymagn_log, []);
	print(gcf, '-dpng', sprintf('erwtimaC\\erwtimaC_%d_Fdomain.png', im));

	figure('Position', [300 200 750 750]);
	imshow(shift_2D(H), []), title('Filter');
	print(gcf, '-dpng', sprintf('erwtimaC\\erwtimaC_%d_filter.png', im));
	
	%{
	figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	imshow(y, []), title( sprintf('s: %.3f', sigmas(im,1)) );
	print(gcf, '-dpng', sprintf('compare_C%d.png', im));
	%}
end
