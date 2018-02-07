close all;
N = 300;

a = imread('clock.jpg');
a = a(1:N, 1:N, 1);

%Determine System's output g for image a
g = psf(a);

figure(1);
subplot(1, 2, 1), imshow(a, []), title('Initial Image')
subplot(1, 2, 2), imshow(g, []), title('Blurred Image')
print(gcf, '-dpng', 'erwtima6b_1.png');

%Determine the Initial System's Impulse Response b and it's corresponding Frequency Response B
delta_2D = zeros(N,N);
delta_2D(ceil(end/2), ceil(end/2)) = 255;
b = psf(delta_2D);

B = fft2(b, N, N);
figure(2), imshow( shift_2D(log(abs(B)+1)) ), title('Calculated Initial System''s Frequency Response');
print(gcf, '-dpng', 'erwtima6b_2.png');


n = [ 1.0 0.1 0.01 0.001];
figure(3);
for i=1: 1 : length(n)

%Calculate the Inverse System's Frequency Response H
	Bn = B;
	Bn( find(abs(Bn) < n(i)) ) = n(i);
	H = ones(N,N)./Bn;
	%figure, imshow( shift_2D(log(abs(H)+1)) );

%Calculate the restored image r in the frequency domain
	R = G .* H;
	r = ifft2(R);

%Calculate the Mean Squared Error
	er = double(a) - double(r);
	theMSE = sumsqr( er(:) ) / N^2;
	fprintf('MSE for threshold n=%.3f is %f \n', n(i), theMSE);
	
	subplot(2, 2, i), imshow(r, []), title( sprintf('n: %.3f | MSE: %f \n', n(i), theMSE) );
end

print(gcf, '-dpng', 'erwtima6b_3.png');