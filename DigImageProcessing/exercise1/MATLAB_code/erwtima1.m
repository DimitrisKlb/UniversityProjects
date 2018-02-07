a = imread('clock.jpg');
a = a(1:300, 1:300, 3); %It is grayscale
 
A1 = DFT_2D_RowCol(a);
A2 = DFT_2D_Matrix(a);

A1magn = shift_2D(abs(A1));
A2magn = shift_2D(abs(A2));

c = 1;
A1magn_log = shift_2D(log(abs(A1)+c));
A2magn_log = shift_2D(log(abs(A2)+c));

close all;
figure('Position', [100 200 750 750]);
subplot(2,2,1), imshow(A1magn, []), title('Method 1: Linear Display');
subplot(2,2,2), imshow(A2magn, []), title('Method 2: Linear Display');
subplot(2,2,3), imshow(A1magn_log, []), title('Method 1: Logarithmic Display');
subplot(2,2,4), imshow(A2magn_log, []), title('Method 2: Logarithmic Display');
colormap(jet); 

print(gcf, '-dpng', 'erwtima1_image.png');

[N1,N2] = size(a);
Y = [0:N1-1] - ceil(N1/2);
X = [0:N2-1] - ceil(N2/2);
figure('Position', [900 200 750 750]);
subplot(2,2,1), mesh(X, Y, A1magn), title('Method 1: Linear Display');
subplot(2,2,2), mesh(X, Y, A2magn), title('Method 2: Linear Display');
subplot(2,2,3), mesh(X, Y, A1magn_log ), title('Method 1: Logarithmic Display');
subplot(2,2,4), mesh(X, Y, A2magn_log), title('Method 2: Logarithmic Display');
print(gcf, '-dpng', 'erwtima1_mesh.png');

