GAUSSIAN BLUR + OPENMP IMPLEMENTATION
======================================

This implementation follows the project's Gaussian Blur requirement:
the image is divided into processing regions, threads calculate different
output pixels, and neighboring pixels are read from the shared input image.

1. Gaussian kernel
------------------
1 2 1
2 4 2
1 2 1

The kernel is normalized by 16.

2. Files
--------
gaussian_blur_sequential.c
    Sequential baseline.

gaussian_blur_openmp.c
    OpenMP implementation with runtime scheduling:
    static, dynamic, or guided.

generate_test_ppm.c
    Optional program to create a test PPM image.

3. Windows / MinGW compilation
------------------------------
gcc -O2 gaussian_blur_sequential.c -o gaussian_seq.exe
gcc -O2 -fopenmp gaussian_blur_openmp.c -o gaussian_omp.exe
gcc -O2 generate_test_ppm.c -o generate_test_ppm.exe

4. Test image (optional)
------------------------
.\generate_test_ppm.exe 1920 1080 input.ppm

5. Sequential run
-----------------
.\gaussian_seq.exe input.ppm output_seq.ppm

6. OpenMP runs
--------------
Static:
.\gaussian_omp.exe input.ppm out_static_2.ppm 2 static 1
.\gaussian_omp.exe input.ppm out_static_4.ppm 4 static 1
.\gaussian_omp.exe input.ppm out_static_8.ppm 8 static 1

Dynamic:
.\gaussian_omp.exe input.ppm out_dynamic_2.ppm 2 dynamic 1
.\gaussian_omp.exe input.ppm out_dynamic_4.ppm 4 dynamic 1
.\gaussian_omp.exe input.ppm out_dynamic_8.ppm 8 dynamic 1

Guided:
.\gaussian_omp.exe input.ppm out_guided_2.ppm 2 guided 1
.\gaussian_omp.exe input.ppm out_guided_4.ppm 4 guided 1
.\gaussian_omp.exe input.ppm out_guided_8.ppm 8 guided 1

For your project plan, also run 1 thread for each scheduling type if you
want a complete 1/2/4/8 table.

7. Recommended experiment
-------------------------
Use the same input image and compile flags for every run.

For each case, run the program 5 times and use the average execution time.
Avoid other heavy CPU workloads while measuring.

8. Performance formulas
-----------------------
Speedup = Sequential Time / Parallel Time

Parallel Efficiency = Speedup / Number of Threads

Throughput can be reported as:
Throughput = Number of processed pixels / Execution Time

For an image of W x H:
Number of processed pixels = W * H
//Result to be calculated
9. Result table
---------------
Algorithm: Gaussian Blur
Image Resolution: __________________
CPU: ______________________________
Compiler: _________________________

Sequential Time: __________________

Schedule | Threads | Time (s) | Speedup | Efficiency
---------|---------|-----------|---------|-----------
Static   | 1       |           |         |
Static   | 2       |           |         |
Static   | 4       |           |         |
Static   | 8       |           |         |
Dynamic  | 1       |           |         |
Dynamic  | 2       |           |         |
Dynamic  | 4       |           |         |
Dynamic  | 8       |           |         |
Guided   | 1       |           |         |
Guided   | 2       |           |         |
Guided   | 4       |           |         |
Guided   | 8       |           |         |

10. What to discuss in the report
---------------------------------
- Whether execution time decreases as thread count increases.
- Which scheduling strategy performs best.
- Whether adding more threads eventually gives little benefit.
- Gaussian blur's repeated neighborhood reads and memory traffic.
- Possible cache-locality effects.
- Scheduling overhead for dynamic/guided scheduling.
- Do not insert numerical speedup claims until measured values are available.

Input format:
    binary PPM (P6), max value 255.

Output format:
    binary PPM (P6), max value 255.
