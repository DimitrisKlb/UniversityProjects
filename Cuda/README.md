# Machine learning circle fitting in CUDA
Implementation of a variation of the k-means clustering algorithm that achieves circle fitting for points in the 2D plane, for a NVIDIA GPU
(parallel) architecture, using C and CUDA

- Achieved up to 800% speedup against the serial implementation, for a specific CPU/GPU setup.
- Extensively tested multiple software architectures to further improve GPU usability and general efficiency.

# Designed Architecture

![Designed architecture](diagram.png)

# Sample Executions 

![Serial Times](serial.png)
![Parallel Times 1](parallel.png)
![Parallel Times 2](parallel2.png)

# Output Images

![Test Script](timerScript.png)
![Results 1](results.png)
![Results 2](results2.png)