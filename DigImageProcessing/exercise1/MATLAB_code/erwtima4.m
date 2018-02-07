weights = [0 1 1 1 0; 1 2 2 2 1; 1 2 4 2 1; 1 2 2 2 1; 0 1 1 1 0];

%---------- Building Image ----------
a1 = im2double(imread('building.jpg'));
a1 = a1(:,:,1); %It is grayscale

Fr = 2;
Fc = 2;
close all;
figure('Position', [100 100 1800 800]);
photo = 1;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a1), title('Initial Image 1: Building');

a1F = median_filter(a1, 3); %Apply the 3x3 median filter
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a1F), title('Filtered Image 1: 3x3 median');

a1F = median_filter(a1, 5); %Apply the 5x5 median filter
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a1F), title('Filtered Image 1: 5x5 median');

a1F = weighted_median_filter(a1, weights); %Apply the 5x5 weighted median filter
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a1F), title('Filtered Image 1: 5x5 weighted median');

print(gcf, '-dpng', 'erwtima4_1.png');

%---------- Train Image ----------
a2 = im2double(imread('train.jpg'));
a2 = a2(:,:,1); %It is grayscale

Fr = 2;
Fc = 2;
figure('Position', [100 100 1800 800]);
photo = 1;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a2), title('Initial Image 2: Building');

a2F = median_filter(a2, 3); %Apply the 3x3 median filter
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a2F), title('Filtered Image 2: 3x3 median');

a2F = median_filter(a2, 5); %Apply the 5x5 median filter
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a2F), title('Filtered Image 2: 5x5 median');

a2F = weighted_median_filter(a2, weights); %Apply the 5x5 weighted median filter
subplot(Fr, Fc, photo), photo=photo+1;
imshow(a2F), title('Filtered Image 2: 5x5 weighted median');

print(gcf, '-dpng', 'erwtima4_2.png');






