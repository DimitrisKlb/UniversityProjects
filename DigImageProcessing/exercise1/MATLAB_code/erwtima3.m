a = im2double(imread('factory.jpg'));
a = a(:,:,1); %It is grayscale

sobelH = [-1 -2 -1 ;  0 0 0 ;  1 2 1];
sobelV = [-1  0  1 ; -2 0 2 ; -1 0 1];

prewittH = [-1 -1 -1 ;  0 0 0 ;  1 1 1];
prewittV = [-1  0  1 ; -1 0 1 ; -1 0 1];


%---------- Edge Detection with Sobel's Mask ----------
Fr = 3;
Fc = 1;
close all;
figure('Position', [100 100 1800 800]);
photo = 1;

gH = my_conv_2D( a, sobelH );
gV = my_conv_2D( a, sobelV );	
gSob = sqrt(gH.^2 + gV.^2);
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gSob), title('Sobel'); 

T = 0.2;
gT = gSob;
gT( find(gSob>T) ) = 1;
gT( find(gSob<T) ) = 0;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gT), title( sprintf('Sobel with Global Threshold \nT=%.3f', T) ); 

T = 0.3;
gT = gSob;
gT( find(gSob>T) ) = 1;
gT( find(gSob<T) ) = 0;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gT), title( sprintf('Sobel with Global Threshold \nT=%.3f', T) ); 
%print(gcf, '-dpng', 'erwtima3_1sob.png');

%---------- Edge Detection with Prewitt's Mask ----------
Fr = 3;
Fc = 1;

figure('Position', [100 100 1800 800]);
photo = 1;

gH = my_conv_2D( a, prewittH );
gV = my_conv_2D( a, prewittV );	
gPre = sqrt(gH.^2 + gV.^2);
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gPre), title('Prewitt'); 

T = 0.2;
gT = gPre;
gT( find(gPre<T) ) = 0;
gT( find(gPre>T) ) = 1;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gT), title( sprintf('Prewitt with Global Threshold \nT=%.3f', T) );

T = 0.3;
gT = gPre;
gT( find(gPre<T) ) = 0;
gT( find(gPre>T) ) = 1;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gT), title( sprintf('Prewitt with Global Threshold \nT=%.3f', T) );
%print(gcf, '-dpng', 'erwtima3_1pre.png');

%---------- Global Threshold Selection using Histogram Info ----------
Fr = 2;
Fc = 2;
figure('Position', [100 100 1800 800]);
photo = 1;

subplot(Fr, Fc, photo), photo=photo+1;
imhist(gSob), title( sprintf('Sobel''s Mask Histogram') );

T = 0.34;
gT = gSob;
gT( find(gSob<T) ) = 0;
gT( find(gSob>T) ) = 1;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gT), title( sprintf('Sobel with Global Threshold \nT=%.3f', T) );



subplot(Fr, Fc, photo), photo=photo+1;
imhist(gPre), title( sprintf('Prewitt''s Mask Histogram') );

T = 0.25;
gT = gPre;
gT( find(gPre<T) ) = 0;
gT( find(gPre>T) ) = 1;
subplot(Fr, Fc, photo), photo=photo+1;
imshow(gT), title( sprintf('Prewitt with Global Threshold \nT=%.3f', T) );
%print(gcf, '-dpng', 'erwtima3_2.png');




