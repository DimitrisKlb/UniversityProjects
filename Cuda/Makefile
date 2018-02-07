NVCC=/usr/local/cuda-6.5/bin/nvcc
FLAGS=-arch=sm_20
SOURCE=main

defaut: $(SOURCE).cu
	$(NVCC) $(FLAGS) $(SOURCE).cu -o fitcircles -lcublas
