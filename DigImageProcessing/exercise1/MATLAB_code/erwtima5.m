close all;
images = {'dark_road_1.jpg'; 'dark_road_2.jpg'; 'dark_road_3.jpg'};

for i=1 : 1 : length(images)
	a1 = imread(images{i});
	a1 = a1(:,:,1); %It is grayscale

%---------- Histrogram Equalization ----------
	Fr = 2;
	Fc = 2;

	figure;
	photo = 1;

	subplot(Fr, Fc, photo), photo=photo+1;
	imshow(a1), title(sprintf('Initial Image: dark\\_road\\_%d', i));

	subplot(Fr, Fc, photo), photo=photo+1;
	imhist(a1), title(sprintf('Initial Image %d: Histogram', i));

	aT = histogram_equalization(a1);

	subplot(Fr, Fc, photo), photo=photo+1;
	imshow(aT), title(sprintf('Tsansformed Image %d', i));

	subplot(Fr, Fc, photo), photo=photo+1;
	imhist(aT), title(sprintf('Tsansformed Image %d: Histogram', i));

	print(gcf, '-dpng', sprintf('erwtima5_1%d.png', i));
	
%---------- Local Histrogram Equalization ----------
	windows = [51 101 151];
	Fr = 2;
	Fc = 2;
	figure('Position', [100 100 1800 800]);
	photo = 1;

	subplot(Fr, Fc, photo), photo=photo+1;
	imshow(aT), title('Global Histogram Eq');

	for k=1 : 1 : length(windows)
		aTL = local_histogram_equalization(a1, windows(k));
		subplot(Fr, Fc, photo), photo=photo+1;
		imshow(aTL), title(sprintf('Local Histogram Eq: n = %d', windows(k)));
	end
	print(gcf, '-dpng', sprintf('erwtima5_2%d.png', i));
end

