#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>

#define INPUT_FILE_ENDING "dat"
#define MAX_CIRCLES_DEFAULT 5
#define MAX_CIRCLES_MAX 10
#define CLUSTER_SIZE_DEFAULT 10
#define CLUSTER_SIZE_MAX 100
#define ITERATIONS_MAX_DEFAULT 100
#define OVERFIT_PENALTY 0.001
#define CIRCLE_CENTER_LOW  -1.0
#define CIRCLE_CENTER_HIGH  1.0
#define CIRCLE_RADIUS_LOW   0.1
#define CIRCLE_RADIUS_HIGH  1.0

typedef struct Point {
	double x;
	double y;
} POINT; 

typedef struct Circle {
	POINT center;
	double radius;
} CIRCLE;

typedef struct CircleFitData {
	int N;
	double Mx, My;
	double Suu, Suuu;
	double Svv, Svvv;
	double Suv, Suvv, Svuu;
} CIRCLE_FIT_DATA;

__device__ const CIRCLE_FIT_DATA CFD_0 = {0, 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

void usage(const char* program_name);
void file_error(const char *error_msg, FILE *fd);
void cudaCheckErrors(cudaError_t error);
int outputCircles(FILE* stream, CIRCLE* circles, int* returnedCircles, int eventsNum, int maxCircles, int eventLow, double* LADs);

//Implementation of CUDA's atomicAdd() for double data, since it is officially implemented by CUDA 
//Still, the code is given as a suggestion in the documentation of CUDA
__device__ double atomicAdd(double* address, double val); 

/****************************** CUDA Kernels ******************************/

//One thread per point, one block per speculated number of circles per column, one column per event in a cluster
__global__ void fitCirclesKernel(POINT *d_points, CIRCLE *d_circles, int* d_pointsCumSums, double *d_LDSs, int maxCircles, curandState_t* statesRNG){
//Shared Variables
	extern __shared__ int sharedMem[];	
	CIRCLE *C = (CIRCLE*)sharedMem;
	CIRCLE_FIT_DATA *CFD = (CIRCLE_FIT_DATA*)&C[maxCircles];
	int *somethingChanged = (int*)&CFD[maxCircles];
	float *LDS = (float*)&somethingChanged[1];
	
//Local Variables
	int pointsNum = d_pointsCumSums[0];
	int circlesNum = blockIdx.x+1;
	int pointID = threadIdx.x;	
	POINT thePoint;
	
	double distance;
	double minDist;
	int belongsToCircle, newClosestCircle;
	double u, v;
	int i, iterations;
	int iterationsMax = ITERATIONS_MAX_DEFAULT;
	
//Identify the correct starting point of the memory, for each event
	if(blockIdx.y != 0){
		d_points += d_pointsCumSums[blockIdx.y-1];
		pointsNum = d_pointsCumSums[blockIdx.y] - d_pointsCumSums[blockIdx.y-1]; 
	}	
	d_circles += (blockIdx.y*maxCircles + blockIdx.x) * maxCircles;		
	d_LDSs += blockIdx.y*maxCircles + blockIdx.x;
	thePoint = d_points[pointID];
	
//Proceed with the Kernel Execution only for the appropriate amount of threads	
	if(pointID < pointsNum){		
	
	/***** KCC - Create the initial Circles *****/	
	
	//Random Circle Initialization		
		if(pointID < circlesNum){
			int idRNG, min, max;
			idRNG = blockIdx.y*maxCircles*maxCircles + blockIdx.x*maxCircles + pointID;	
			
			min = CIRCLE_CENTER_LOW;
			max = CIRCLE_CENTER_HIGH;
			C[pointID].center.x = curand_uniform_double(&statesRNG[idRNG]) * (max - min) + (min);
			C[pointID].center.y = curand_uniform_double(&statesRNG[idRNG]) * (max - min) + (min);
		
			min = CIRCLE_RADIUS_LOW;
			max = CIRCLE_RADIUS_HIGH;
			C[pointID].radius = curand_uniform_double(&statesRNG[idRNG]) * (max - min) + (min);			
		}
		__syncthreads();
		
		belongsToCircle = -1;
		iterations = 0;
		do{
			*somethingChanged = 0;
	/***** KCC - ITERATIVE STEP 1: Assing every Point to it's closet Circle *****/
		
		//Calculate distance from every Circle, select min	
			minDist = 100000.0;
			newClosestCircle = -1;
			for(i=0; i<circlesNum; i++){
				distance = sqrt( pow((thePoint.x - C[i].center.x), 2) + pow((thePoint.y - C[i].center.y), 2) );
				distance = fabs( distance - C[i].radius );
				if(distance < minDist){
					minDist = distance;
					newClosestCircle = i;
				}	
			}	
			if(newClosestCircle != belongsToCircle){
				belongsToCircle = newClosestCircle;
				*somethingChanged = 1;
			}
		
	/***** KCC - ITERATIVE STEP 2: Fit every Circle to it's Points using least squares *****/	
		
		//Set every CFD variable of every Circle to 0
			if(pointID < circlesNum){
				CFD[pointID] = CFD_0;		
			}
			__syncthreads();
			
		//Calculate the means of the Points coords, in every Circle
			atomicAdd(&CFD[belongsToCircle].Mx, thePoint.x);
			atomicAdd(&CFD[belongsToCircle].My, thePoint.y);
			atomicAdd(&CFD[belongsToCircle].N, 1);	
			__syncthreads();
			
			if(pointID < circlesNum){
				CFD[pointID].Mx /= CFD[pointID].N;
				CFD[pointID].My /= CFD[pointID].N;
			}
			__syncthreads();
			u = thePoint.x - CFD[belongsToCircle].Mx;
			v = thePoint.y - CFD[belongsToCircle].My;
		
		//Create the 2x2 linear system, used to find the best fitted Circle (using least squares method)
			atomicAdd(&CFD[belongsToCircle].Suu,  u*u);
			atomicAdd(&CFD[belongsToCircle].Suuu, u*u*u);
			atomicAdd(&CFD[belongsToCircle].Svv,  v*v);
			atomicAdd(&CFD[belongsToCircle].Svvv, v*v*v);
			atomicAdd(&CFD[belongsToCircle].Suv,  u*v);
			atomicAdd(&CFD[belongsToCircle].Suvv, u*v*v);
			atomicAdd(&CFD[belongsToCircle].Svuu, v*u*u);
		
		//Solve the the 2x2 linear system, by using the closed type for A_inv
			__syncthreads();	
			if(pointID < circlesNum){		
				double det = CFD[pointID].Suu*CFD[pointID].Svv - CFD[pointID].Suv*CFD[pointID].Suv;	
				
				double b[2];
				b[0] = (CFD[pointID].Suuu + CFD[pointID].Suvv)/2;
				b[1] = (CFD[pointID].Svvv + CFD[pointID].Svuu)/2;
					
				double u_c = ( CFD[pointID].Svv*b[0] - CFD[pointID].Suv*b[1])/det;
				double v_c = (-CFD[pointID].Suv*b[0] + CFD[pointID].Suu*b[1])/det;		
				double r = u_c*u_c + v_c*v_c + (CFD[pointID].Suu + CFD[pointID].Svv)/CFD[pointID].N;
				
				C[pointID].center.x = u_c + CFD[pointID].Mx;
				C[pointID].center.y = v_c + CFD[pointID].My;	
				C[pointID].radius = sqrt(r);
			}
			__syncthreads();
		
	//Iterate until either max number of iterations is reached, 
	//or no change happened in assignment of Points to Circles from the previous iteration
			iterations++;
		}while( (iterations < iterationsMax) && (*somethingChanged == 1) );			
		
	/***** Calculate Each Solution's LDS *****/
		
	//Each thread-point will calculate it's minimum distance from all circles
		if(pointID == 0){			
			*LDS = OVERFIT_PENALTY*circlesNum*circlesNum; // Add the overfitting penalty
		}
		__syncthreads();
		
		minDist = 100000.0;
		for(i=0; i<circlesNum; i++){
			distance = sqrt( pow((thePoint.x - C[i].center.x), 2) + pow((thePoint.y - C[i].center.y), 2) );
			distance = fabs( distance - C[i].radius );
			if(distance < minDist){
				minDist = distance;			
			}	
		}	
		atomicAdd(&LDS[0], minDist);		
		__syncthreads();	
		
		if(pointID == 0){
			*d_LDSs = *LDS;
		}	
		__syncthreads();
	
	//Write-back the computed circles			
		if(pointID < circlesNum){
			d_circles[pointID].center.x  = C[pointID].center.x;
			d_circles[pointID].center.y  = C[pointID].center.y;
			d_circles[pointID].radius  = C[pointID].radius;
		}			
		
	}		
}

//One thread per point, one block per event, to find the minimum LDS
//and return the corresponding solution and it's computed LAD
__global__ void computeSolutionKernel(POINT *d_points, CIRCLE *d_circles, int* d_pointsCumSums, double *d_LDSs, double *d_LADs, int* d_returnedCirclesNum, int maxCircles){
//Shared Variables
	extern __shared__ int sharedMem[];	
	float *LAD = (float*)sharedMem;

//Local Variables
	int pointsNum;
	int pointID, eventID;	
	POINT thePoint;
	int bestSolution, returnedCirclesNum;
	double minLDS, deviation, LADp;
	int i, t;
	CIRCLE *circles_input;
	double cX, cY, cR;
	
//Identify the correct starting point of the memory, for each event	
	pointID = threadIdx.x;
	eventID = blockIdx.x;	
	pointsNum = d_pointsCumSums[0];
	if(eventID != 0){
		d_points += d_pointsCumSums[eventID-1];
		pointsNum = d_pointsCumSums[eventID] - d_pointsCumSums[eventID-1]; 
	}		
	d_circles += eventID * maxCircles * maxCircles;		
	d_LDSs += eventID * maxCircles;
	d_returnedCirclesNum += eventID;
	d_LADs += eventID;
	thePoint = d_points[pointID];

//Find the smallest LDS and the returnedCirclesNum of that solution	
	if(pointID == 0){
		bestSolution = 0;
		minLDS = d_LDSs[0];
		for(i=0; i<maxCircles; i++){
			if(d_LDSs[i] < minLDS){
				minLDS = d_LDSs[i];
				bestSolution = i;
			}		
		}
		returnedCirclesNum = bestSolution+1;
		circles_input = d_circles + bestSolution*maxCircles;
	}
	
//Write back the best solution
	if(pointID == 0){
		if(bestSolution != 0){ //If (bestSolution == 0) your don't need to move the results
			t=0;
			for(i=0; i<returnedCirclesNum; i++){
				cX = circles_input[i].center.x;
				cY = circles_input[i].center.y;
				cR = circles_input[i].radius;
				
				if( isnan(cX) == 0 && isnan(cY) == 0 && isnan(cR) == 0){
					d_circles[t].center.x = cX;
					d_circles[t].center.y = cY;
					d_circles[t].radius = cR;
					t++;	
				}
			}
		}			
		*d_returnedCirclesNum = t;
	}
	__syncthreads();
	returnedCirclesNum = *d_returnedCirclesNum;
	
//Calculate and write the LAD metric	
	if(pointID < pointsNum){
		if(pointID == 0){
			*LAD = 0;
		}
		__syncthreads();
		
		LADp = 100000.0;
		for(i=0; i<returnedCirclesNum; i++){
			cX = d_circles[i].center.x;
			cY = d_circles[i].center.y;
			cR = d_circles[i].radius;
			
			deviation = pow((thePoint.x - cX), 2) + pow((thePoint.y - cY), 2) - pow(cR, 2);
			deviation = pow(deviation, 2);
			if(deviation < LADp){
				LADp = deviation;	
			}
		}
		atomicAdd(&LAD[0], LADp);		
		__syncthreads();	
		
		if(pointID == 0){
			*d_LADs = *LAD;
		}
	}				
}
	
__global__ void initRNG(int seed, curandState_t* statesRNG){
	curand_init(seed, blockIdx.x, 0, &statesRNG[blockIdx.x]);
}

/******************************    Main    ******************************/
int main(int argc, char** argv){
//CPU (host) variables	
	char *ext;
	FILE *fd = NULL;
	int i, event, clusterPart, begin;
	int sharedMemorySize;
	curandState_t* statesRNG;
	struct timeval t1, t2;
	
	int eventsNum, pointsNum;
	int pointsNumMax, pointsArraySize;
	double pointX, pointY;
	double errorPercentage = 0.0;
	int maxCircles;
	int clusterSize;
	
	POINT *points;
	CIRCLE *circles;
	double *LADs;
	int *returnedCirclesNum;	
	int *pointsCumSums;
	
//Device variables
	POINT *d_points;
	CIRCLE *d_circles;
	double *d_LADs;
	int *d_returnedCirclesNum;
	int *d_pointsCumSums;
	double *d_LDSs;

//Commmand line arguments Validation
	if(argc < 2){
		printf("Input was file not specified\n");
		usage(argv[0]);
		exit(1);
	}
	ext = strrchr(argv[1], '.');
	if(ext==NULL || strcmp(ext+1, INPUT_FILE_ENDING) != 0){
		printf("Unrecognized input file ending \n");
		usage(argv[0]);
		exit(1);
	}
	
	maxCircles = MAX_CIRCLES_DEFAULT;
	if(argc > 2){
		char *temp;			
		maxCircles = (int)strtol(argv[2], &temp, 0);		
		if(*temp != '\0' || maxCircles<1){
			printf("Second argument [%s] is not an positive integer number \n", argv[2]);
			usage(argv[0]);
			exit(1);
		}
		if(maxCircles > MAX_CIRCLES_MAX){
			printf("Value [%d] for max_circles_size is too big \n", maxCircles);
			printf("Setting to the max accepted value: %d\n", MAX_CIRCLES_MAX);
			maxCircles = MAX_CIRCLES_MAX;
		}
	}		
	clusterSize = CLUSTER_SIZE_DEFAULT;
	if(argc > 3){
		char *temp;			
		clusterSize = (int)strtol(argv[3], &temp, 0);		
		if(*temp != '\0' || clusterSize<1){
			printf("Third argument [%s] is not an positive integer number \n", argv[3]);
			usage(argv[0]);
			exit(1);
		}
		if(clusterSize > CLUSTER_SIZE_MAX){
			printf("Value [%d] for cluster_size is too big \n", clusterSize);
			printf("Setting to the max accepted value: %d\n", CLUSTER_SIZE_MAX);
			clusterSize = CLUSTER_SIZE_MAX;
		}
	}

	gettimeofday(&t1, NULL); //Begin Timing
	
//Input file (.dat) Parsing
	if((fd = fopen(argv[1], "r")) == NULL){
		file_error("Error while opening specified input file", fd);
		exit(1);
	}
	
	eventsNum = pointsNum = 0;
	pointX = pointY = 0.0;
	
	fscanf(fd, "%d", &eventsNum);
	if(ferror(fd) != 0){
		file_error("Error while reading from the input file", fd);
		exit(1);
	}	
	
//Allocate Host - Device Memory with static size
	circles = (CIRCLE *)malloc(clusterSize*maxCircles*maxCircles*sizeof(CIRCLE));
	LADs = (double *)malloc(clusterSize*sizeof(double));
	returnedCirclesNum = (int *)malloc(clusterSize*sizeof(int));
	pointsCumSums = (int *)malloc(clusterSize*sizeof(int));
	
	cudaCheckErrors(cudaMalloc((void**) &d_circles, clusterSize*maxCircles*maxCircles*sizeof(CIRCLE)));		
	cudaCheckErrors(cudaMalloc((void**) &d_LADs, clusterSize*sizeof(double)));	
	cudaCheckErrors(cudaMalloc((void**) &d_returnedCirclesNum, clusterSize*sizeof(int)));
	cudaCheckErrors(cudaMalloc((void**) &d_pointsCumSums, clusterSize*sizeof(int)));
	cudaCheckErrors(cudaMalloc((void**) &d_LDSs, clusterSize*maxCircles*sizeof(double)));
	
//Initialize the RNG states for every thread that will randomly initialize a circle
	int N = maxCircles*maxCircles*clusterSize;
	cudaCheckErrors(cudaMalloc((void**) &statesRNG, N*sizeof(curandState_t)));
	initRNG<<<1, N>>>(time(0), statesRNG);

//Start Loop
	
	clusterPart = 0;
	for(event=0; event<eventsNum; event++){	
	//Read an event
		fscanf(fd, "%d", &pointsNum);
		if(ferror(fd) != 0){
			file_error("Error while reading from the input file", fd);
			exit(1);
		}
		
	//Allocate-Initialize Host Memory with dynamic size		
		pointsCumSums[clusterPart] = pointsNum;
		if(clusterPart == 0){ //First event of the cluster
			pointsCumSums[clusterPart] = pointsNum;
			begin = 0;
			pointsNumMax = pointsNum;
			points = (POINT *)malloc(pointsNum*sizeof(POINT));
		}else{		
			pointsCumSums[clusterPart] = pointsCumSums[clusterPart-1] + pointsNum;
			begin = pointsCumSums[clusterPart-1];
			if(pointsNum > pointsNumMax){
				pointsNumMax = pointsNum;
			}
			points = (POINT *)realloc(points, pointsCumSums[clusterPart]*sizeof(POINT));
		}				
		for(i=begin; i<pointsCumSums[clusterPart]; i++){
			fscanf(fd, "%lf", &pointX);
			if(ferror(fd) != 0){
				file_error("Error while reading from the input file", fd);
				exit(1);
			}
			fscanf(fd, "%lf", &pointY);
			if(ferror(fd) != 0){
				file_error("Error while reading from the input file", fd);
				exit(1);
			}
			points[i].x = pointX;
			points[i].y = pointY;
		}
		sharedMemorySize = maxCircles*( sizeof(CIRCLE) + sizeof(CIRCLE_FIT_DATA) ) + sizeof(int) + sizeof(float);
		
		clusterPart++;
	// The cluster is full; Proceed with the Kernel call	
		if( (clusterPart == clusterSize) || (event == eventsNum-1) ){
			
		//Allocate-Initialize Device Memory that has dynamic size
			pointsArraySize = pointsCumSums[clusterPart-1]*sizeof(POINT);
			cudaCheckErrors(cudaMalloc((void**) &d_points, pointsArraySize));	
			cudaCheckErrors(cudaMemcpy(d_points, points, pointsArraySize, cudaMemcpyHostToDevice));			
			cudaCheckErrors(cudaMemcpy(d_pointsCumSums, pointsCumSums, clusterSize*sizeof(int), cudaMemcpyHostToDevice));	
			
		//Call the CUDA Kernel
			const dim3 blockSize(pointsNumMax, 1, 1);
			const dim3 gridSize1(maxCircles, clusterPart, 1);
			const dim3 gridSize2(clusterPart, 1, 1);
			
			fitCirclesKernel<<<gridSize1, blockSize, sharedMemorySize>>>(d_points, d_circles, d_pointsCumSums, d_LDSs, maxCircles, statesRNG);	
			cudaCheckErrors(cudaDeviceSynchronize()); // DEBUG Is it needed?
			computeSolutionKernel<<<gridSize2, blockSize, sizeof(float)>>>(d_points, d_circles, d_pointsCumSums, d_LDSs, d_LADs, d_returnedCirclesNum, maxCircles);
			
		//Display Results
			cudaCheckErrors(cudaMemcpy(circles, d_circles, clusterSize*maxCircles*maxCircles*sizeof(CIRCLE), cudaMemcpyDeviceToHost));
			cudaCheckErrors(cudaMemcpy(LADs, d_LADs, clusterSize*sizeof(double), cudaMemcpyDeviceToHost));
			cudaCheckErrors(cudaMemcpy(returnedCirclesNum, d_returnedCirclesNum, clusterSize*sizeof(int), cudaMemcpyDeviceToHost));
			
			errorPercentage += outputCircles(NULL, circles, returnedCirclesNum, clusterPart, maxCircles, event-clusterPart+1, LADs);
			
		//Deallocate Host - Device Memory with dynamic size
			free(points);
			cudaCheckErrors(cudaFree(d_points)); 			
			
			clusterPart = 0;
		}
	}
	gettimeofday(&t2, NULL);
	fclose(fd);	
	
//Deallocate Host - Device Memory with static size
	free(circles);
	free(LADs);
	free(returnedCirclesNum);
	free(pointsCumSums);	
	
	cudaCheckErrors(cudaFree(d_circles));
	cudaCheckErrors(cudaFree(d_LADs));
	cudaCheckErrors(cudaFree(d_returnedCirclesNum));
	cudaCheckErrors(cudaFree(d_pointsCumSums));	
	cudaCheckErrors(cudaFree(d_LDSs));
	cudaCheckErrors(cudaFree(statesRNG));

	printf("------------------------------\n");
	printf("Input file: %s - Total events: %d\n", argv[1], eventsNum);
	printf("Parallel procedure's execution time: %.3lf ms\n", ((t2.tv_sec-t1.tv_sec)*1000000.0+t2.tv_usec-t1.tv_usec)/1000.0);
	printf("Overall percentage of non-accurate solutions: %.3lf%%\n\n", errorPercentage*100/eventsNum);
	return 0;
}

void usage(const char* program_name){
	printf("Usage: %s <input_file.%s> [max_number_of_circles] [cluster_size]\n", program_name, INPUT_FILE_ENDING);
}

void file_error(const char *error_msg, FILE *fd){
	perror(error_msg);
	if(fd != NULL){
		fclose(fd);
	}	
}

void cudaCheckErrors(cudaError_t error){
	if(error != cudaSuccess){
		printf("Cuda Error: %s \n", cudaGetErrorString(error));
		exit(1);
	}
}

__device__ double atomicAdd(double* address, double val){
	unsigned long long int* address_as_ull = (unsigned long long int*)address;
	unsigned long long int old = *address_as_ull, assumed;
	do{
		assumed = old;
		old = atomicCAS(address_as_ull, assumed, 
		__double_as_longlong(val + 
		__longlong_as_double(assumed)));
	} while (assumed != old);
	
	return __longlong_as_double(old);
}

int outputCircles(FILE* stream, CIRCLE* circles, int* returnedCircles, int eventsNum, int maxCircles, int eventLow, double* LADs){
	int ev, i, p;
	char cx, cy;
	int errors = 0;
	int noPrint = 0;
	
	if(stream == NULL){
		noPrint = 1;
	}
	
	for(ev=0; ev<eventsNum; ev++){
		(noPrint) || fprintf(stream, "------------------------------\n");
		(noPrint) || fprintf(stream, "Event: %d - Circles found: %d\n", eventLow+ev+1, returnedCircles[ev]);
		for(i=0; i<returnedCircles[ev]; i++){
			p = ev*maxCircles*maxCircles + i;
			cx = (circles[p].center.x < 0) ? '\0' : ' ' ;
			cy = (circles[p].center.y < 0) ? '\0' : ' ' ;
			(noPrint) || fprintf(stream, " CenterX: %c%.10lf | CenterY: %c%.10lf | Radius: %.10lf \n", cx, circles[p].center.x, cy, circles[p].center.y, circles[p].radius);
		}		
		if( LADs[ev] <= pow(10,-10) ){
			(noPrint) || fprintf(stream, "Accurate Solution. LAD Metric < 10^(-10) \n");
		}else{
			(noPrint) || fprintf(stream, "Non-Accurate Solution. LAD Metric: %.10lf > 10^(-10) \n", LADs[ev]);
			errors++;
		}		
	}		
	
	return errors;
}