images = {'radiograph_1.jpg', 'radiograph_2.jpg'};

k(1) = 10;
wc_l(1) = pi/4 - 0.15;
wc_h(1) = pi/4 + 0.30;

k(2) = 10;
wc_l(2) = pi/6 - 0.25;
wc_h(2) = pi/6 + 0.05;
	
close all;
for im=1 : 1 : length(images)
	a = im2double(imread(images{im}));
	a = a(:,:,1); %It is grayscale

	H_h = ones(size(a));
	H_l = ones(size(a));
	H   = ones(size(a));

%---------- Create the desired stop-band filter ----------
	[M,N] = size(a);
	for i=1 : 1 : M
		for j=1 : 1 : N			
			r = ( ( (i-(M/2)-1)/M*2*pi )^2 + ( (j-(N/2)-1)/N*2*pi )^2 )^(0.5);			
			H_l(i,j) = 1 / ( 1 + (r/wc_l(im))^(2*k(im)) );
			H_h(i,j) = 1 / ( 1 + (wc_h(im)/r)^(2*k(im)) );
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
	figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	subplot(2,1,1), imshow(a, []), title('Before');
	subplot(2,1,2), imshow(y, []), title('After');
	print(gcf, '-dpng', sprintf('erwtimaB\\erwtimaB_%d_Tdomain.png', im));

	figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	subplot(2,1,1), imshow(Amagn_log, []), title('Before'); 
	subplot(2,1,2), imshow(Ymagn_log, []), title('After');
	print(gcf, '-dpng', sprintf('erwtimaB\\erwtimaB_%d_Fdomain.png', im));

	figure('Position', [300 200 750 750]);
	imshow(shift_2D(H), []), title(sprintf('Filter''s Order: %d', k(im)));
	print(gcf, '-dpng', sprintf('erwtimaB\\erwtimaB_%d_filter.png', im));
	
	%{
	figure('units', 'normalized', 'outerposition',[0 0 1 1]);
	imshow(y, []), title( sprintf('k: %d', k(im)) );
	print(gcf, '-dpng', sprintf('compare_B%d.png', im));
	%}
end
