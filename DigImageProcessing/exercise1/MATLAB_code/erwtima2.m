a = imread('pepper.jpg');

a_quant = zeros(size(a), 'uint8');
L1 = [4 6];
L2 = [4 6];
L3 = [4 6];

close all;
%figure('Position', [100 0 500 1000]);
%subplot(length(L1)+1, 1, 1), imshow(a), title('Initial Image'); 
figure, imshow(a), print(gcf, '-dpng', sprintf('erwtima2_0.png'));
for i=1 : 1 : length(L1)
	a_quant(:, :, 1) = my_image_quantizer(a(:, :, 1), L1(i));
	a_quant(:, :, 2) = my_image_quantizer(a(:, :, 2), L2(i));
	a_quant(:, :, 3) = my_image_quantizer(a(:, :, 3), L3(i));
	
	SNR1 = sumsqr( a_quant(:, :, 1)) / sumsqr( a(:, :, 1)-a_quant(:, :, 1) );
	SNR1 = 10*log10(SNR1);
	SNR2 = sumsqr( a_quant(:, :, 2)) / sumsqr( a(:, :, 2)-a_quant(:, :, 2) );
	SNR2 = 10*log10(SNR2);
	SNR3 = sumsqr( a_quant(:, :, 3)) / sumsqr( a(:, :, 3)-a_quant(:, :, 3) );
	SNR3 = 10*log10(SNR3);
	
	%subplot(length(L1)+1, 1, i+1), imshow(a_quant);
	%title( sprintf('Quantized Image: R|G|B Levels: %d | %d | %d \n SNRs(db): %.2f | %.2f | %.2f', L1(i), L2(i), L3(i), SNR1, SNR2, SNR3) ); 
	figure, imshow(a_quant), print(gcf, '-dpng', sprintf('erwtima2_%d.png',i));
	fprintf('Quantized Image: R|G|B Levels: %d | %d | %d \n SNRs(db): %.2f | %.2f | %.2f \n', L1(i), L2(i), L3(i), SNR1, SNR2, SNR3) 
end
%print(gcf, '-dpng', 'erwtima2.png');

%Output the 3 color components of the image
%figure, imshow(a(:, :, 1)), print(gcf, '-dpng', sprintf('erwtima2_R.png',i));
%figure, imshow(a(:, :, 2)), print(gcf, '-dpng', sprintf('erwtima2_G.png',i));
%figure, imshow(a(:, :, 3)), print(gcf, '-dpng', sprintf('erwtima2_B.png',i));

%Test Case 
L1 = 4;
L2 = 16;
L3 = 4;

a_quant(:, :, 1) = my_image_quantizer(a(:, :, 1), L1);
a_quant(:, :, 2) = my_image_quantizer(a(:, :, 2), L2);
a_quant(:, :, 3) = my_image_quantizer(a(:, :, 3), L3);

SNR1 = sumsqr( a_quant(:, :, 1)) / sumsqr( a(:, :, 1)-a_quant(:, :, 1) );
SNR1 = 10*log10(SNR1);
SNR2 = sumsqr( a_quant(:, :, 2)) / sumsqr( a(:, :, 2)-a_quant(:, :, 2) );
SNR2 = 10*log10(SNR2);
SNR3 = sumsqr( a_quant(:, :, 3)) / sumsqr( a(:, :, 3)-a_quant(:, :, 3) );
SNR3 = 10*log10(SNR3);

figure, imshow(a_quant), print(gcf, '-dpng', sprintf('erwtima2_test.png'));
fprintf('Quantized Image: R|G|B Levels: %d | %d | %d \n SNRs(db): %.2f | %.2f | %.2f \n', L1, L2, L3, SNR1, SNR2, SNR3) 
