# A Fast Analytical Model of Fully Associative Caches

## Getting Started Guide

We provide a docker container for the artifact evaluation. The container was setup and tested with Docker version 18.06.1-ce (build e68fc7a). To build the container, execute the following commands:

- tar xzf paper228.tar.gz
- cd paper228
- docker build -t paper:228 . 

The build takes a while as it loads and compiles all dependencies of the cache model. When the docker image is ready, execute the following commands to run the docker and execute the haystack unit tests:

- docker run -i -t paper:228 /bin/bash
- cd /home/haystack/build/
- make -j4
- make test

If all 24 tests pass the build is ok. To test haystack, run the following command in the /home/haystack/build folder: 

- ./src/StackDistance -I ../../experiments/benchmarks/polybench_large/utilities/ -f ../../experiments/benchmarks/polybench_large/linear-algebra/blas/gemm/gemm.c

The command executes haystack with the polybench gemm kernel. We expect the following output:

    -> start preprocessing...
    -> done (17.21ms)
    -> processing dimension 7
    -> done (6.59ms)
    -> processing dimension 6
    -> done (0.28ms)
    -> processing dimension 5
    -> done (21.57ms)
    -> processing dimension 4
    -> done (0.29ms)
    -> processing dimension 3
    -> done (34.09ms)
    -> processing dimension 2
    -> done (13.88ms)
    -> processing dimension 1
    -> done (69.06ms)
    -> processing dimension 0
    -> done (0.08ms)
    -> done (152.97ms) accumulated 453600/165434400/165434400/5282200000 (CO/CA0/CA1/TO) cache misses
    -> finished after (220.04ms)
    ==================================================
    - ComputeBetweenMap:   62.993ms
    - ComputeStackDistances:       84.307ms
    - CountBetweenMap:     78.016ms
    - CountCapacityMisses: 17.089ms
    - CountCompulsoryMisses:       0.375ms
    - applyEqualization:   0.005ms
    - applyRasterization:  1.470ms
    - countAffineDimensions:       5.213ms
    - extractAffineExpression:     3.960ms
    ==================================================

Haystack computes 453600 compulsory misses (CO), 165434400 first level cache misses (CA0), 165434400 second level cache misses (CA1), and 5282200000 memory accesses in total (TO). If the numbers match, we are ready to perform the step-by-step evaluation.

## Step-by-Step Instructions

The step-by-step instructions discuss the steps necessary to regenerate the figures of the evaluation section. The figures demonstrate the performance and the accuracy of the cache model in detail and support all important claims of the paper. The docker image contains a batch script (/home/experiments/run_experiments.sh) to recompute the evaluation data (/home/experiments/data) and to generate the plots (home/experiments/plots). As the computation of the dinero results is very time consuming (>1 week) and as the papi measurements are hardware platform specific, the docker image contains pre-computed dinero measurements

- /home/experiments/data/polybench_large/perf_din_large.csv (performance)
- /home/experiments/data/polybench_large/acc_din_full_assoc_O2.csv (misses for full associativity)
- /home/experiments/data/polybench_large/acc_din_euler_assoc_O2.csv (misses for test system associativity)

and papi results

- /home/experiments/data/polybench_large/acc_papi_O2.csv

We later discuss how to reproduce the dinero and papi results if desired.

The following steps are necessary to generate all plots of the evaluation section:

1) docker run -i -t paper:228 /bin/bash
2) cd /home/experiments
3) ./run_experiments.sh

The run_experiments.sh script runs for multiple hours (~1 day). After completion, we use the "docker cp" command to copy the results to the host. The folder /home/experiments/plots contains the generated plots and the folder /home/experiments/data the raw data. To evaluate the artifact, we suggest to compare the generated plots to the figures of the submitted paper. The accuracy plots 

- /home/experiments/plots/acc_percent1_polybench_large.pdf
- /home/experiments/plots/acc_percent2_polybench_large.pdf 
- /home/experiments/plots/acc_euler_polybench_large.pdf
- /home/experiments/plots/acc_full_polybench_large.pdf

correspond to Figure 9 and Figure 10 of the paper. The performance plots

- /home/experiments/plots/perf_baseline_polybench_large.pdf
- /home/experiments/plots/perf_sizes_polybench.pdf
- /home/experiments/plots/perf_caches_polybench_large.pdf
- /home/experiments/plots/perf_floorelim_polybench_large.pdf
- /home/experiments/plots/perf_polycache_polybench_32.pdf
- /home/experiments/plots/perf_dinero_polybench_large.pdf
- /home/experiments/plots/perf_tiled_polybench.pdf

correspond to Figure 11 - Figure 16. The results should be close to the figures in the paper but may deviate slightly depending on the performance of the system running the docker container. By default the run_experiments.sh script performs every experiment only once. To compute the confidence intervals, we can set the environment variable REPETITIONS to 10 or any other desired value. Yet the compute time will increase accordingly. We observed little variance when running the experiments as our test system was not running other tasks.

### Fast Track

As the reproduction of all figures is time consuming, we provide also an alternative script (run_experiments_main.sh) which executes much faster (~1 hour) and produces all figures except for Figure 14 and Figure 16.

### Supported Claims

The docker image reproduces all figures of the paper and thus supports all our important performance and accuracy claims. Yet the step-by-step guide uses pre-computed dinero and papi results. 

We recommend the reviewers to rely on the provided dinero and papi results. The next two sections discuss how to nevertheless reproduce the data if required.

### Run Dinero

To reproduce the dinero results execute the commands:

- cd /home/experiments
- ./run_dinero.sh

The script computes the dinero results for all polybench benchmarks. The results are printed to the console and stored in the files:

- /home/experiments/data/polybench_large/acc_din_euler_assoc_O2.csv
- /home/experiments/data/polybench_large/perf_din_large.csv

The script will run for a long time (>1 week) and overwrite the pre-computed dinero results.

### Run Papi

The papi results need to be gathered outside of the docker container on a machine with specs as close as possible to our test system (Intel Xeon Gold 6150). To measure the cache misses, we first install the papi library

- sudo apt-get install libpapi-dev (Ubuntu)
- sudo sysctl -w kernel.perf_event_paranoid=0 (enables user space access to the performance counters, not necessary on Ubuntu)

on the **host** system (not inside the docker container) and then execute the following commands

- cd paper228/experiments/
- python acc_papi_large.py

The results are printed to the console and stored to the paper228/experiments/acc_papi_O2.csv file. The python script requires python3 with the scipy and pandas packages installed.

### Structure of the Docker Container

The /home/haystack folder contains the cache model sources. The sources are stored in the /home/haystack/src and /home/haystack/include folders. We compile the cache model using cmake and set the EXPERIMENT preprocessor variable to build the binaries for the different experiments. The docker script builds the binaries automatically using the following commands:

    RUN cd haystack && \
        mkdir build && \
        cd build && \
        cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=baseline -DPOLY_BASE=/usr ../ && \
        make -j4 StackDistance && \
        cp ./src/StackDistance ../../experiments/executables/baseline && \
        cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=noequal -DPOLY_BASE=/usr ../ && \
        make -j4 StackDistance && \
        cp ./src/StackDistance ../../experiments/executables/noequal && \
        cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=noraster -DPOLY_BASE=/usr ../ && \
        make -j4 StackDistance && \
        cp ./src/StackDistance ../../experiments/executables/noraster && \
        cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=onecache -DPOLY_BASE=/usr ../ && \
        make -j4 StackDistance && \
        cp ./src/StackDistance ../../experiments/executables/onecache && \
        cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=threecaches -DPOLY_BASE=/usr ../ && \
        make -j4 StackDistance && \
        cp ./src/StackDistance ../../experiments/executables/threecaches && \
        cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=enumerateall -DPOLY_BASE=/usr ../ && \
        make -j4 StackDistance && \
        cp ./src/StackDistance ../../experiments/executables/enumerateall && \
        cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=polycache -DPOLY_BASE=/usr ../ && \
        make -j4 StackDistance && \
        cp ./src/StackDistance ../../experiments/executables/polycache && \
        cd ../../

All binaries are stored in the /home/experiments/executables folder. The binaries compute the cache misses for different cache configurations or with disabled optimizations. We generate the following binaries:

- **baseline**: the binary computes the cache misses for the L1 and L2 caches of our test system
- **polycache**: the binary computes the cache misses for the cache configuration used by PolyCache
- **threecaches/onecache**: the binaries compute the cache misses for three and one cache, respectively
- **noequal/noraster**: the binaries compute the cache misses with equalization or rasterization disabled, respectively
- **enumerateall**: the binary computes the cache misses with partial enumeration disabled meaning all points of the non-affine pieces are enumerated

The /home/experiments folder contains the scripts used to perform the experiments and to plot the figures. The names of the script define their purpose. The scripts prefixed with **acc_** measure the cache miss counts (accuracy) and the scripts prefixed with **perf_** measure the execution times (performance). The scripts prefixed with **plot_** generate the figures shown in the paper. The name of the performance and accuracy scripts furthermore contains the binary and the benchmark used for the measurement. The plot scripts generate the following figures:

- **plot_acc_baseline_polybench.py**: the script generates the accuracy plots shown by Figure 9. The script computes the miss rate in percent given the HayStack data (blue bar) and the miss rate in percent given the PAPI data (black line). To compute the miss rate, we divide the number of cache misses by the total number of memory accesses computed by HayStack. The error bars on top show the difference of the HayStack and PAPI cache misses divided by the total number of memory accesses computed by HayStack (relative error).
- **plot_dinero_baseline_polybench.py**: the script generates the dinero accuracy plots shown by Figure 10. The script also computes the miss rate and the relative error but for Dinero instead of HayStack. The two figures show the results for the L1 cache simulated with test system and full associativity, respectively.
- **plot_perf_baseline_polybench.py**: the script generates Figure 11-13 that show the influence of the problem sizes and the number of caches and Figure 16 that shows the influence of tiling. The script shows the total execution times and for some figures the stack distance and capacity miss times measured by the performance scripts.
- **plot_perf_alterantives.py**: the script generates Figures 15a and Figure 15b that compare to PolyCache and Dinero. The script computes the speedup of HayStack compared to the two alternatives using the execution times measured by the performance scripts except for the PolyCache execution times which are hard-coded in the script.
- **plot_per_speedup_polybench.py**: the script generates Figure 14 that shows the influence of the different optimizations (equalization, rasterization, and partial enumeration). The script computes the speedup of the full algorithm compared to variants with disabled optimizations.

The /home/experiments/ folder contains sub folders that are either used to store the results or that contain the benchmarks and the executables used for the experiments. We have the following sub folders:

- **/home/experiments/benchmarks**: the folder contains polybench configured with different problem sizes, a tiled version of polybench, and polybench_3.2 that is used for the PolyCache comparison
- **/home/experiments/executables**: the folder contains the haystack binaries
- **/home/experiments/data**: the folder is used to store the measurements
- **/home/experiments/plots**: the folder is used to store the figures
