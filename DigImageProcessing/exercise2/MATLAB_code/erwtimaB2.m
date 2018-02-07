images = {'radiograph_1.jpg', 'radiograph_2.jpg'};
cases = 4;

k = ones(length(images), cases);
wc_l = ones(length(images), cases);
wc_h = ones(length(images), cases);
figs = ones(length(images), 3);

k(1, :) = [2, 5, 8, 10];
wc_l(1, :) = [pi/4 - 0.30, pi/4 - 0.20, pi/4 - 0.20, pi/4 - 0.15];
wc_h(1, :) = [pi/4 + 0.50, pi/4 + 0.45, pi/4 + 0.40, pi/4 + 0.35];

k(2, :) = k(1, :);
wc_l(2, :) = [pi/6 - 0.35, pi/6 - 0.35, pi/6 - 0.25, pi/6 - 0.25];
wc_h(2, :) = [pi/6 + 0.10, pi/6 + 0.10, pi/6 + 0.10, pi/6 + 0.05];
	
close all;
for im=2 : 1 : length(images)
	a = im2double(imread(images{im}));
	a = a(:,:,1); %It is grayscale

	H_h = ones(size(a));
	H_l = ones(size(a));
	H   = ones(size(a));

	figs(im, 1) = figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	figs(im, 2) = figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	figs(im, 3) = figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	
	for c=1 : 1 : cases
%---------- Create the desired stop-band filter ----------
		[M,N] = size(a);
		for i=1 : 1 : M
			for j=1 : 1 : N			
				r = ( ( (i-(M/2)-1)/M*2*pi )^2 + ( (j-(N/2)-1)/N*2*pi )^2 )^(0.5);				
				H_l(i,j) = 1 / ( 1 + ( r/wc_l(im, c) )^( 2*k(im, c) ) );
				H_h(i,j) = 1 / ( 1 + ( wc_h(im, c)/r )^( 2*k(im, c) ) );
			end
		end

		H = shift_2D(H_l + H_h);

%---------- Apply the filter ----------
		A = DFT_2D_RowCol(a);
		Amagn_log = shift_2D(log(abs(A)+1));

		Y = A .* H;
		Ymagn_log = shift_2D(log(abs(Y)+1));

%---------- Display Results ----------
		y = ifft2(Y);
		
		figure(figs(im, 1));
		subplot(2,2,c), imshow(y, []), title( sprintf('k: %d', k(im,c)) );		

		figure(figs(im, 2));
		subplot(2,2,c), imshow(Ymagn_log, []), title( sprintf('k: %d', k(im,c)) );		
		
		figure(figs(im, 3));
		subplot(2,2,c), imshow(shift_2D(H), []), title( sprintf('k: %d', k(im,c)) );	
	end	
	
	figure(figs(im, 1));
	print(gcf, '-dpng', sprintf('erwtimaB2\\erwtimaB2_%d_Tdomain.png', im));
	figure(figs(im, 2));
	print(gcf, '-dpng', sprintf('erwtimaB2\\erwtimaB2_%d_Fdomain.png', im));
	figure(figs(im, 3));
	print(gcf, '-dpng', sprintf('erwtimaB2\\erwtimaB2_%d_filter.png', im));
end