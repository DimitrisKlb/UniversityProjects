images = {'radiograph_1.jpg', 'radiograph_2.jpg'};
cases = 4;

wmegas = ones(length(images), 2, 2);
sigmas = ones(length(images), cases, 2);
figs = ones(length(images), 3);

wmegas(1, :, :) = [-0.10 (pi/4+0.08); 0.10 -(pi/4+0.05)];
sigmas(1, :, :) = [0.15 0.15; 0.30 0.30; 0.50 0.50; 0.70 0.70 ];

wmegas(2, :, :) = [(pi/6-0.04) -0.05; -(pi/6-0.04) 0.05];
sigmas(2, :, :) = [0.15 0.15; 0.30 0.30; 0.50 0.50; 0.70 0.70 ];

close all;
for im=1 : 1 : length(images)
	a = im2double(imread(images{im}));
	a = a(:,:,1); %It is grayscale
	
	figs(im, 1) = figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	figs(im, 2) = figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	figs(im, 3) = figure('units', 'normalized', 'outerposition',[0 0 1 1]);

	for c=1 : 1 : cases
%---------- Create the desired stop-band filter ----------
		H = ones(size(a));
		[M,N] = size(a);
		Nw = size(wmegas, 2);
		[w1, w2] = meshgrid(1:N, 1:M);
		
		for i=1 : 1 : Nw
			w1n = (w1 - N/2)/N*2*pi;
			w2n = (w2 - M/2)/M*2*pi;
			
			r = (w1n - wmegas(im, i, 1)).^2 + (w2n - wmegas(im, i, 2)).^2;
			H = H .* (1 - exp( -r/sigmas(im, c, i)^2 ) );
		end	

		H = shift_2D(H);

%---------- Apply the filter ----------
		A = DFT_2D_RowCol(a);
		Amagn_log = shift_2D(log(abs(A)+1));

		Y = A .* H;
		Ymagn_log = shift_2D(log(abs(Y)+1));

%---------- Display Results ----------
		y = ifft2(Y);
		
		figure(figs(im, 1));
		subplot(2,2,c), imshow(y, []), title( sprintf('s: %.3f', sigmas(im,c,1)) );		

		figure(figs(im, 2));
		subplot(2,2,c), imshow(Ymagn_log, []), title( sprintf('s: %.3f', sigmas(im,c,1)) );		
		
		figure(figs(im, 3));
		subplot(2,2,c), imshow(shift_2D(H), []), title( sprintf('s: %.3f', sigmas(im,c,1)) );	
	end
	
	figure(figs(im, 1));
	print(gcf, '-dpng', sprintf('erwtimaC2\\erwtimaC2_%d_Tdomain.png', im));
	figure(figs(im, 2));
	print(gcf, '-dpng', sprintf('erwtimaC2\\erwtimaC2_%d_Fdomain.png', im));
	figure(figs(im, 3));
	print(gcf, '-dpng', sprintf('erwtimaC2\\erwtimaC2_%d_filter.png', im));	
end