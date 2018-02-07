a = im2double(imread('radiograph_1.jpg'));
a = a(:,:,1); %It is grayscale
b = im2double(imread('radiograph_2.jpg'));
b = b(:,:,1); %It is grayscale

A = DFT_2D_RowCol(a);
Amagn_log = shift_2D(log(abs(A)+8));
B = DFT_2D_RowCol(b);
Bmagn_log = shift_2D(log(abs(B)+3));

close all;
figure('Position', [100 200 750 750]);
%imshow(Amagn_log, []), title('radiograph\_1.jpg: 2D-DFT (Logarithmic Display)');
imshow(Amagn_log, []);
print(gcf, '-dpng', 'erwtimaA\\erwtimaA_1.png');

figure('Position', [900 200 750 750]);
%imshow(Bmagn_log, []), title('radiograph\_2.jpg: 2D-DFT (Logarithmic Display)');
imshow(Bmagn_log, []);
print(gcf, '-dpng', 'erwtimaA\\erwtimaA_2.png');