Table of contents
- [Getting Started Guide](#getting-started-guide)
  - [Setting up the warping Docker image and container](#setting-up-the-warping-docker-image-and-container)
  - [Setting up the haystack Docker image and container](#setting-up-the-haystack-docker-image-and-container)
    - [If you face problems while building the haystack image](#if-you-face-problems-while-building-the-haystack-image)
  - [About long-running experiments](#about-long-running-experiments)
- [Step-by-Step Instructions](#step-by-step-instructions)
  - [Reproducing all figures using the existing data](#reproducing-all-figures-using-the-existing-data)
  - [Reproducing Figures 6, 7, 9, and 10](#reproducing-figures-6-7-9-and-10)
    - [Building the project](#building-the-project)
    - [Preparing the PolyBench benchmark](#preparing-the-polybench-benchmark)
    - [Reproducing the figures](#reproducing-the-figures)
      - [Figure 6](#figure-6)
      - [Figure 7](#figure-7)
      - [Figure 9](#figure-9)
      - [Figure 10](#figure-10)
    - [Copying the reproduced figures and data to the host system](#copying-the-reproduced-figures-and-data-to-the-host-system)
    - [How and why these figures support our claims](#how-and-why-these-figures-support-our-claims)
  - [Reproducing Figure 8 and 11](#reproducing-figure-8-and-11)
    - [Reproducing the figures](#reproducing-the-figures-1)
      - [Figure 8](#figure-8)
      - [Figure 11](#figure-11)
    - [Copying the reproduced figures and data to the host system](#copying-the-reproduced-figures-and-data-to-the-host-system-1)
    - [How and why these figures support our claims](#how-and-why-these-figures-support-our-claims-1)
    - [Reproducing Figure 11 with PAPI-C and DineroIV (optional)](#reproducing-figure-11-with-papi-c-and-dineroiv-optional)
      - [Reproduce DineroIV data](#reproduce-dineroiv-data)
      - [Reproduce PAPI-C data](#reproduce-papi-c-data)
  - [Bonus: comparing the warping simulation cache miss results with the non-warping simulation ones](#bonus-comparing-the-warping-simulation-cache-miss-results-with-the-non-warping-simulation-ones)
- [Details about our source code](#details-about-our-source-code)
  - [Source code structure with brief descriptions](#source-code-structure-with-brief-descriptions)
  - [Connections between the source code and the structures presented in our paper](#connections-between-the-source-code-and-the-structures-presented-in-our-paper)
    - [Simulation tree](#simulation-tree)
    - [Cache state](#cache-state)
      - [Adding a new replacement policy](#adding-a-new-replacement-policy)
  - [Connections between the source code and the algorithms presented in our paper](#connections-between-the-source-code-and-the-algorithms-presented-in-our-paper)
    - [Algorithm 1: Non-warping cache simulation](#algorithm-1-non-warping-cache-simulation)
    - [Algorithm 2: Warping cache simulation](#algorithm-2-warping-cache-simulation)
    - [Important parameters used in the code](#important-parameters-used-in-the-code)
      - [Cache parameters](#cache-parameters)
      - [Warping parameters](#warping-parameters)

# Getting Started Guide

This is the artifact of the conference paper "Warping Cache Simulation of Polyhedral Programs" at PLDI 2022.

The artifact structure is as follows:
- `benchmark`: The [PolyBench](https://web.cse.ohio-state.edu/~pouchet.2/software/polybench/) benchmark that is used in our experiments.
- `data`: The data we obtained from our experiments (`data/existing`) which are presented in our paper as plots. The data that you will reproduce will also be available here (`data/reproduced`).
- `haystack-artifact`: The [artifact](https://dl.acm.org/do/10.1145/332599) provided by the related work [HayStack](https://dl.acm.org/doi/10.1145/3314221.3314606).
- `plots`: The plots you will generate (`plots/from-existing-data` and `plots/from-reproduced-data`).
- `scripts`: The scripts for repeating our experiments and generating our figures.
- `warping-cache-simulation`: The source code of our cache simulation tool.
- `d4-7.tar.gz`: DineroIV package, used only if the ftp server used in `haystack-artifact/Dockerfile` is down.
- `Dockerfile`: File to build our Docker image, which will be used to repeat our experiments and generate our plots.
- `README.md`: This file. We suggest using a "markdown capable" viewer to read the file.
- `paper-submission-version.pdf`: The submission version of our accepted paper.

Through this document, by figures, we refer to the plot figures presented in the "Experimental Evaluation" section of our paper.

The artifact evaluation consists of three main parts:

1. [Reproducing all figures using the existing data](#reproducing-all-figures-using-the-existing-data).
2. [Reproducing Figures 6, 7, 9, and 10](#reproducing-figure-6-7-9-and-10): Requires repeating the experiments involving our simulation tool.
3. [Reproducing Figures 8 and 11](#reproducing-figure-8-and-11): Requires repeating experiments involving both our simulation tool and related work.

For 1 and 2, you will use the Docker container (the `warping` container) we provide.
Commands for setting up the `warping` container are given in [Setting up the warping Docker image and container](#setting-up-the-warping-docker-image-and-container).

For 3, you will use both the `warping` container and the Docker container provided by the related work artifact (the `haystack` container).
Commands for setting up the `haystack` container are given in [Setting up the haystack Docker image and container](#setting-up-the-haystack-docker-image-and-container).

This artifact supports all the claims from the paper by reproducing all our experimental results.
For the artifact evaluation, we suggest comparing reproduced figures with the original ones in our paper.

Note that this artifact also includes the section [Details about our source code](#details-about-our-source-code) where we explain how our paper relates to the source code of our tool.
We provide enough information for extending or changing our the implementation.

## Setting up the warping Docker image and container
Using this container (to which we refer as the `warping` container) you will run our experiments and figure generation scripts.
Tested with Docker version 20.10.12 on Linux and Windows (with Windows Subsystem for Linux) machines.

1. Build an image from the Dockerfile inside this artifact directory (takes around 10 minutes):
```
docker build -t warping-image .
```
2. Run the `warping` container in the background from the image you built:
```
docker run -td --name warping warping-image
```
3. To start a Bash shell in the running container:
```
# you will run the warping experiments and reproduce our figures inside this shell
docker exec -it warping bash
```
4. To restart the container when it is stopped:
```
docker restart warping
```

## Setting up the haystack Docker image and container
This is required only for Figure 8 and Figure 11.
Using this container (to which we refer as the `haystack` container) you will run the related work experiments.
Tested with Docker version 20.10.12 on Linux and Windows (with WSL) machines.

**Important note**: the ftp server (ftp://ftp.cs.wisc.edu/markhill/DineroIV) used in the `haystack-artifact/Dockerfile` to download DineroIV is sometimes down.
If you face a problem regarding this while building the `haystack` container image, please see [If you face problems while building the haystack image](#if-you-face-problems-while-building-the-haystack-image).

1. Build an image from the Dockerfile inside the `haystack-artifact` directory (takes around 10 minutes):
```
cd haystack-artifact
mkdir -p experiments/executables
docker build -t haystack-image .
```
2. Run container in the background:
```
docker run -td --name haystack haystack-image
```
3. To start a Bash shell in the running container:
```
# you will run the haystack and dinero experiments inside this shell
docker exec -it haystack bash
```
4. To restart the container when it is stopped:
```
docker restart haystack
```

### If you face problems while building the haystack image
This can happen if the ftp server for DineroIV is down.
We are sorry if that happened but this is out of our control.
For this purpose, we included the DineroIV tar.gz file in our artifact which you can manually copy to the `haystack` container.

1. Comment out lines [77, 83] in `haystack-artifact/Dockerfile` (you can also manually do this):
```
# in the artifact directory of the host system
sed -i -e "77,83s/^/# /" haystack-artifact/Dockerfile
```
2. Build the image and run the container as explained before.
3. Copy the DineroIV package that we provide to the `haystack` container:
```
# in the artifact directory of the host system
docker cp d4-7.tar.gz haystack:/home
```
4. In the `haystack` container shell, extract and build DineroIV:
```
tar -xf d4-7.tar.gz && \
  cd d4-7 && \
  ./configure && \
  make && \
  cp dineroIV /usr/local/bin && \
  cd ..
```

## About long-running experiments

The figures in our paper have been generated with the large and xlarge PolyBench datasets.
Rerunning the corresponding experiments may take several hundred hours and may thus not be feasible depending on your time budget.
Through this guide, we provide commands to run our experiments with various PolyBench dataset sizes to provide a faster track for reproducing our results.
The plots reproduced via the faster track should still resemble our results.
However, please note that there are some experiments for which the resulting plots on smaller dataset sizes may look different than the original ones. This does not necessarily contradict our claims.

For example, the experiments required for generating Figure 11 involve PAPI-C cache measurements, which may vary substantially across different dataset sizes.
We observed that when we generate the same figure with smaller datasets, the related work's error for some benchmarks changes significantly, which in turn has an impact on the appearance of the figure (especially in case of the small PolyBench dataset configuration).

Another example is Figure 7.
When the plot is generated using small and medium datasets, it looks slightly different than the original one (which is generated using large and xlarge datasets).
However, the message conveyed by the figure still agrees with our claim: increasing the problem size does not increase the simulation time significantly, when we can apply our "warping" technique.

In case of doubt, we suggest repeating the experiments with larger dataset sizes.

# Step-by-Step Instructions

## Reproducing all figures using the existing data

Prerequisites:
- [Setting up the warping Docker image and container](#setting-up-the-warping-docker-image-and-container)

Inside the `warping` container shell, you can generate the plots from the data we provide as follows.

1. Run the script to generate the plots from the existing data:
```
./scripts/plot-generation/all-plots-from-existing-data.sh
```
2. Check which plot files are generated:
```
ls plots/from-existing-data
```
3. Copy the generated plot files from the container to the host system:
```
# you should run this command in the host system, inside the same directory this file resides!
docker cp warping:/home/plots/from-existing-data/. plots/from-existing-data
```
4. You can now examine the plots in your host system in `plots/from-existing-data`:
```
Figure 6  -> figure6-large-speedup-and-warp-repl-pol.pdf
Figure 7  -> figure7-large-xlarge-plot.tex (and figure7-large-xlarge-plot-data.txt)
Figure 8  -> figure8-large-haystack-vs-warping-l1-speedup.pdf
Figure 9  -> figure9-large-polycache-vs-warping-l1-l2-speedup.pdf
Figure 10 -> figure10-large-warping-repl-pol-miss.pdf
Figure 11 -> figure11-large-papi-accuracy-l1.pdf
```

Note that Figure 7 is a LaTeX file.
You need pgfplots and TikZ packages to compile it.
Alternatively, you can copy the tex and corresponding data txt file to an online LaTeX compiler, such as https://www.overleaf.com (tested), and view the figure there.

You can find more information regarding the figures in our paper in the "Experimental Evaluation" section.

## Reproducing Figures 6, 7, 9, and 10

Prerequisites:
- [Setting up the warping Docker image and container](#setting-up-the-warping-docker-image-and-container)

### Building the project

Inside the `warping` container shell, build our cache simulation tool as follows.

1. Run the script to build the project (< 1 minute):
```
./warping-cache-simulation/build-release.sh
```
2. Simulate a simple example (this should print the results in JSON format):
```
bin=cmake-build-release/src/warping-cache-simulation
src=warping-cache-simulation/example-source-files/example.c
cache_config=warping-cache-simulation/example-cache-configs/example.config
./$bin "$src" "$cache_config"
```
3. Compare the JSON output with the expected output below (duration results may vary, others not):
```
{
"source":"warping-cache-simulation/example-source-files/example.c",
"cacheLevel0":
{
"cacheSize":24,
"lineSize":8,
"associativity":3,
"nLines":3,
"nSets":1,
"replacementPolicy":"lru",
"writePolicy":"write-back-write-allocate"
},
"warping":true,
"heuristics":false,
"accessCounts": [20000],
"hitCounts": [100],
"missCounts": [19900],
"warpedAccessPercentages": [94.08],
"notWarpedAccessPercentages": [5.92],
"timeMSec":294,
"timeSec":0,
"timeMin":0
}
```

### Preparing the PolyBench benchmark
Run the following to make copies of the PolyBench benchmark for each dataset configuration (xlarge, large, medium, small):
```
./scripts/reproduction/prepare-polybench.sh
```
This creates the following directories:
- `benchmark/polybench-xlarge`
- `benchmark/polybench-large`
- `benchmark/polybench-medium`
- `benchmark/polybench-small`

The only differences between these and the original benchmark are:
- `<benchmark>/utilities/papi_counters.list`: Includes our PAPI-C counters for the experiments.
- `<benchmark>/utilities/polybench.h`: Includes 2 extra lines added by us, which are given below.
```
    # define POLYBENCH_USE_SCALAR_LB 1 // by warping cache simulation
    # define <size>_DATASET // by warping cache simulation
```
\<size\> can be EXTRALARGE/LARGE/MEDIUM/SMALL. These lines enforce scalar loop bounds and configure the dataset size.

### Reproducing the figures

The original figures presented in the paper are generated from experiments with large (and xlarge for Figure 7) PoyBench dataset sizes.
However, these experiments can take quite a long time.
For this reason, we also introduce alternative faster experiments with smaller dataset sizes (except for Figure 8 as the required data for the related work, PolyCache, is only available for the large dataset size).

We provide the expected approximate duration next to each command so that you can pick the experiment with the dataset size as you see fit.
This is a common pattern across this document.

Note that the larger the datasets, the more similar the reproduced figures will be to the original ones.

The commands below should be executed inside the `warping` container shell unless stated otherwise.

#### Figure 6
```
./scripts/reproduction/reproduce-figure.sh 6 large   # 70 hours
./scripts/reproduction/reproduce-figure.sh 6 medium  # 25 minutes
./scripts/reproduction/reproduce-figure.sh 6 small   # 3 minutes
```

#### Figure 7
```
./scripts/reproduction/reproduce-figure7.sh large xlarge  # 145 hours
./scripts/reproduction/reproduce-figure7.sh medium large  # 16 hours
./scripts/reproduction/reproduce-figure7.sh small medium  # 7 minutes
```

#### Figure 9
```
# fast version is not available (limited related work data)
./scripts/reproduction/reproduce-figure.sh 9 large   # 20 hours
```

#### Figure 10
```
./scripts/reproduction/reproduce-figure.sh 10 large   # 35 hours
./scripts/reproduction/reproduce-figure.sh 10 medium  # 20 minutes
./scripts/reproduction/reproduce-figure.sh 10 small   # 1 minute
```

### Copying the reproduced figures and data to the host system
In the host system:
```
# figures
docker cp warping:/home/plots/from-reproduced-data/. plots/from-reproduced-data

# data
docker cp warping:/home/data/reproduced/. data/reproduced
```

### How and why these figures support our claims

Figure 6 shows for each benchmark the speedup of warping simulation compared to the non-warping simulation.
The reproduced figure supports our claim that we can achieve large speedups and outperform traditional cache simulation (several orders of magnitude with larger datasets - expected to be less with smaller datasets) when we can apply warping.
We can also see that warping works for various cache replacement policies.

Figure 7 shows the change in warping and non-warping L1 simulation times between different problem sizes.
The reproduced figure supports our claim that for some benchmarks the warping simulation times are not proportional to the number of memory accesses, which is in contrast to the non-warping simulation times.

Figure 9 shows the speedup of warping simulation compared to PolyCache for each benchmark.
The reproduced figure supports our claim that PolyCache outperforms warping cache simulation on average but the relative performance varies greatly across the set of benchmarks.

Figure 10 shows the influence of the replacement policy on cache performance.
The reproduced figure supports our claim that replacement policy can have a considerable effect by showing that there are benchmarks with a substantially varying number of cache misses.
Note that different dataset sizes can result in larger differences between the replacement policies as the impact of cache parameters (such as associativity and cache size) on the cache performance may increase/decrease with different problem sizes.

More detailed information is given in the "Experimental Evaluation" section of our paper.
We recommend comparing the reproduced figures with those in our paper.

## Reproducing Figure 8 and 11

Prerequisites:
- [Setting up the warping Docker image and container](#setting-up-the-warping-docker-image-and-container)
- [Setting up the haystack Docker image and container](#setting-up-the-haystack-docker-image-and-container)
- [Building the project](#building-the-project)
- [Preparing the PolyBench benchmark](#preparing-the-polybench-benchmark)

### Reproducing the figures

As before, you can pick alternative dataset sizes according to the expected run times.

#### Figure 8
1. In the `haystack` container shell:
```
cd /home/experiments

# create script for PolyBench medium dataset
cp perf_onecache_polybench_large.py perf_onecache_polybench_medium.py
sed -i "s/large/medium/g" perf_onecache_polybench_medium.py

# create necessary directories if they are not available
mkdir -p /home/experiments/data/polybench_large
mkdir -p /home/experiments/data/polybench_medium

# reproduce HayStack performance data
python perf_onecache_polybench_large.py # few minutes
python perf_onecache_polybench_medium.py # few minutes
```

2. In the host system:
```
# copy HayStack performance data from the haystack container to the host system
docker cp haystack:/home/experiments/data/polybench_large/perf_onecache.csv data/reproduced/haystack/haystack-large-l1-duration.csv
docker cp haystack:/home/experiments/data/polybench_medium/perf_onecache.csv data/reproduced/haystack/haystack-medium-l1-duration.csv

# copy HayStack performance data from the host system to the warping container
docker cp data/reproduced/haystack/haystack-large-l1-duration.csv warping:/home/data/reproduced/haystack/haystack-large-l1-duration.csv
docker cp data/reproduced/haystack/haystack-medium-l1-duration.csv warping:/home/data/reproduced/haystack/haystack-medium-l1-duration.csv
```

3. In the `warping` container shell:
```
./scripts/reproduction/reproduce-figure8.sh large   # 7 hours
./scripts/reproduction/reproduce-figure8.sh medium  # 4 minutes
```

#### Figure 11
Reproducing this figure is the most involved one as it requires experiments with our tool, HayStack, DineroIV, and also PAPI-C measurements.
We suggest not repeating
- the PAPI-C measurements (as these depend highly on the hardware),
- the DineroIV experiments (as these can take a long time),
and instead, rely on the existing data we and HayStack provide.
The steps below do that.

For completeness, we give all the necessary information and commands to repeat these in section [Reproducing Figure 11 with PAPI-C and DineroIV (optional)](#reproducing-figure-11-with-papi-c-and-dineroiv).

Note that the existing data we provide for DineroIV is generated using the HayStack artifact.
As a proof, you can optionally generate the miss data from the HayStack artifact data by:
```
# in the host system, copy haystack data to the warping container
docker cp haystack-artifact/experiments/data/polybench_large/acc_din_euler_assoc_O2.csv warping:/home/data/reproduced/haystack/dinero-large-l1-l2-miss.csv

# in the warping container shell (just sums different types of cache misses to find the total miss)
./scripts/plot-generation/gen-dinero-miss-report.py reproduced large

# in the warping container shell, compare the output with the data we provide
diff data/reproduced/miss/dinero-large-l1-l2.csv data/existing/miss/dinero-large-l1-l2.csv
```
That is, the DineroIV miss data that we provide and HayStack provides are identical but in different forms.

That out of the way, let's move on with the reproduction of Figure 11.

1. In the `haystack` container shell:
```
cd /home/experiments

# create HayStack script for PolyBench small (optional, other required experiments with medium are fast enough)
cp acc_baseline_polybench_large.py acc_baseline_polybench_small.py
sed -i "s/large/small/g" acc_baseline_polybench_small.py

# create necessary directories if they are not available
mkdir -p /home/experiments/data/polybench_large
mkdir -p /home/experiments/data/polybench_medium
mkdir -p /home/experiments/data/polybench_small

# reproduce HayStack miss data
python acc_baseline_polybench_large.py   # few minutes
python acc_baseline_polybench_medium.py  # few minutes
python acc_baseline_polybench_small.py   # few minutes
```

2. In the host system:
```
# copy HayStack miss data from the haystack container to the host system
docker cp haystack:/home/experiments/data/polybench_large/acc_baseline.csv data/reproduced/haystack/haystack-large-l1-l2-miss.csv
docker cp haystack:/home/experiments/data/polybench_medium/acc_baseline.csv data/reproduced/haystack/haystack-medium-l1-l2-miss.csv
docker cp haystack:/home/experiments/data/polybench_small/acc_baseline.csv data/reproduced/haystack/haystack-small-l1-l2-miss.csv

# copy HayStack miss data from the host system to the warping container
docker cp data/reproduced/haystack/haystack-large-l1-l2-miss.csv warping:/home/data/reproduced/haystack/haystack-large-l1-l2-miss.csv
docker cp data/reproduced/haystack/haystack-medium-l1-l2-miss.csv warping:/home/data/reproduced/haystack/haystack-medium-l1-l2-miss.csv
docker cp data/reproduced/haystack/haystack-small-l1-l2-miss.csv warping:/home/data/reproduced/haystack/haystack-small-l1-l2-miss.csv
```

3. In the `warping` container shell:
```
./scripts/reproduction/reproduce-figure11-existing-papi-dinero.sh large   # 7 hours
./scripts/reproduction/reproduce-figure11-existing-papi-dinero.sh medium  # 5 minutes
./scripts/reproduction/reproduce-figure11-existing-papi-dinero.sh small   # 1 minute
```

### Copying the reproduced figures and data to the host system
In the host system:
```
# figures
docker cp warping:/home/plots/from-reproduced-data/. plots/from-reproduced-data

# data
docker cp warping:/home/data/reproduced/. data/reproduced
```

### How and why these figures support our claims

Figure 8 shows the speedup of warping simulation compared to HayStack for each kernel when only the L1 cache is simulated.
The reproduced figure supports our claim that HayStack is not faster than warping cache simulation on all benchmarks.
In particular, warping cache simulation outperforms HayStack on most stencil benchmarks.

Figure 11 shows the accuracy of related work and warping cache simulation relative to measurements on the actual hardware.
The reproduced figure supports our claim that considering the parameters of the actual hardware (such as replacement policy and associativity) while modeling/simulating the cache can result in more accurate results - but further work is required to investigate the other aspects of modern microarchitectures (such as memory reordering and speculative execution).
Note that reproducing this figure using the small/medium dataset sizes shows that the error of HayStack increases significantly on many benchmarks.
This is not addressed in our paper (as we were not aware of this until now) but is in favor of our claims.

### Reproducing Figure 11 with PAPI-C and DineroIV (optional)

Prerequisites:
- [Figure 11](#figure-11) (should finish all of the three steps with the dataset size you chose - this is crucial as these steps make the necessary data available in the `warping` container)

In this section, we first introduce how to reproduce DineroIV and PAPI-C data, and then how to generate Figure 11 using the reproduced data.

#### Reproduce DineroIV data

1. In the `haystack` container:
```
cd /home/experiments

# create script for PolyBench medium and small
cp acc_din_euler_assoc_large.py acc_din_euler_assoc_medium.py
cp acc_din_euler_assoc_large.py acc_din_euler_assoc_small.py
sed -i "s/large/medium/g" acc_din_euler_assoc_medium.py
sed -i "s/large/small/g" acc_din_euler_assoc_small.py

# create necessary directories if they are not available
mkdir -p /home/experiments/data/polybench_large
mkdir -p /home/experiments/data/polybench_medium
mkdir -p /home/experiments/data/polybench_small

# reproduce DineroIV miss data
python acc_din_euler_assoc_large.py   # > week
python acc_din_euler_assoc_medium.py  # 1 hour 20 minutes
python acc_din_euler_assoc_small.py   # 3 minutes
```

2. In the host:
```
# copy dinero miss data from the dinero container to the host system
docker cp haystack:/home/experiments/data/polybench_large/acc_din_euler_assoc_O2.csv data/reproduced/haystack/dinero-large-l1-l2-miss.csv
docker cp haystack:/home/experiments/data/polybench_medium/acc_din_euler_assoc_O2.csv data/reproduced/haystack/dinero-medium-l1-l2-miss.csv
docker cp haystack:/home/experiments/data/polybench_small/acc_din_euler_assoc_O2.csv data/reproduced/haystack/dinero-small-l1-l2-miss.csv

# copy dinero miss data from the host system to the warping container
docker cp data/reproduced/haystack/dinero-large-l1-l2-miss.csv warping:/home/data/reproduced/haystack/dinero-large-l1-l2-miss.csv
docker cp data/reproduced/haystack/dinero-medium-l1-l2-miss.csv warping:/home/data/reproduced/haystack/dinero-medium-l1-l2-miss.csv
docker cp data/reproduced/haystack/dinero-small-l1-l2-miss.csv warping:/home/data/reproduced/haystack/dinero-small-l1-l2-miss.csv
```

#### Reproduce PAPI-C data

The PAPI-C measurements should be repeated outside a docker container on a machine with specs similar (if not same) to our system (Intel Core i9-10980XE, Cascade
Lake).
Thus, the following commands should be run on the host system unless stated otherwise.
The commands have been tested on a Ubuntu machine.

1. Install the PAPI library:
```
sudo apt-get install libpapi-dev
sudo apt-get install papi-tools
```

2. Check if the required counters PAPI_L1_DCM and PAPI_L2_DCM are available:
```
papi_avail

# if not available, enable user space access to performance counters and try again
sudo sysctl -w kernel.perf_event_paranoid=0
```
Unless the counters are available, you cannot run the measurements!

3. Make our scripts executable, in the artifact directory:
```
chmod +x scripts/plot-generation/*.sh && \
    chmod +x scripts/plot-generation/*.py && \
    chmod +x scripts/reproduction/*.sh && \
    chmod +x scripts/reproduction/*.py
```
4. Prepare PolyBench benchmark:
```
./scripts/reproduction/prepare-polybench.sh
```

5. Compile PolyBench examples and create binaries for PAPI-C measurements (requires gcc):
```
./scripts/reproduction/compile-papi-binaries.sh large
./scripts/reproduction/compile-papi-binaries.sh medium
./scripts/reproduction/compile-papi-binaries.sh small
```

6. Run PAPI-C measurements:
```
sudo ./scripts/reproduction/run-papi.sh large   # 45 minutes
sudo ./scripts/reproduction/run-papi.sh medium  # 1 minute
sudo ./scripts/reproduction/run-papi.sh small   # 1 minute
```
The results should be available in `data/reproduced/papi`.
Please open them to verify that there are numbers and not words like FAILED (this can happen if PAPI measurement fail in your system).

7. Copy the reproduced PAPI-C data to the `warping` container:
```
docker cp data/reproduced/papi/large.csv warping:/home/data/reproduced/papi/large-o2.csv
docker cp data/reproduced/papi/medium.csv warping:/home/data/reproduced/papi/medium-o2.csv
docker cp data/reproduced/papi/small.csv warping:/home/data/reproduced/papi/small-o2.csv
```

8. In `warping` container, process the copied data and generate the plot:
```
# note that you should have already repeated the dinero and papi experiments with the dataset size you choose below
./scripts/reproduction/reproduce-figure11.sh large   # 7 hours
./scripts/reproduction/reproduce-figure11.sh medium  # 5 minutes
./scripts/reproduction/reproduce-figure11.sh small   # 1 minute
```
The figure should be available as `plots/from-reproduced-data/figure11-<size>-papi-accuracy-l1.pdf` where \<size\> is the dataset size you chose.

## Bonus: comparing the warping simulation cache miss results with the non-warping simulation ones

Prerequisites:
- [Setting up the warping Docker image and container](#setting-up-the-warping-docker-image-and-container)
- [Building the project](#building-the-project)
- [Preparing the PolyBench benchmark](#preparing-the-polybench-benchmark)

As explained in `scripts/reproduction/README.md`, you can run the PolyBench examples with our tool with both warping enabled and disabled.
Using the example commands below, you can do this for different dataset sizes, and compare the results of warping and non-warping simulation.

In the `warping` container:
```
cfgs=scripts/reproduction/cache-configs
script=scripts/reproduction/run-polybench-all.sh

# run warping-enabled simulation with the desired dataset
./$script large true true $cfgs/cascadelake-l1.config   # 7 hours
./$script medium true true $cfgs/cascadelake-l1.config  # 2 minutes
./$script small true true $cfgs/cascadelake-l1.config   # < 1 minute

# run warping-disabled simulation with the desired dataset
./$script large false false $cfgs/cascadelake-l1.config   # 9 hours
./$script medium false false $cfgs/cascadelake-l1.config  # 5 minutes
./$script small false false $cfgs/cascadelake-l1.config   # 2 minutes

# create csv reports
./scripts/reproduction/create-csv-reports.sh

# compare miss reports
csv_warp=true-true-cascadelake-l1-miss.csv
csv_no_warp=false-false-cascadelake-l1-miss.csv

diff data/reproduced/csv/large/$csv_warp data/reproduced/csv/large/$csv_no_warp
diff data/reproduced/csv/medium/$csv_warp data/reproduced/csv/medium/$csv_no_warp
diff data/reproduced/csv/small/$csv_warp data/reproduced/csv/small/$csv_no_warp
```

# Details about our source code

The source code is given in `warping-cache-simulation` directory (CMake project, C++17).
It is structured well and written in a modular fashion, leveraging common OOP concepts.
The names of the directories/files/classes/methods are self-explanatory and the code should be easy to follow.

The details about how to build and run (including command line parameters) are given in `warping-cache-simulation/README.md`.

We focus on the parts of the source code which are most relevant to our work.
We provide detailed information about:
- connections between the source code and the structures/algorithms presented in our paper,
- extending our tool with new replacement policies,
- important parameters used in the code.

## Source code structure with brief descriptions

```
warping-cache-simulation/ (root directory of the project)
├─ example-cache-configs/ (example cache config files)
├─ example-source-files/ (example programs to simulate)
├─ src/ (root directory of the source code)
│  ├─ Address/ (concrete and symbolic address implementations)
│  ├─ CacheState/ (cache state implementation, e.g. replacement policies)
│  ├─ IteratorState/ (iterator state implementation, e.g. iterator bound extraction)
│  ├─ ProgramInfo/ (polyhedral model extraction and program representation, e.g. array accesses)
│  ├─ Settings/ (simualation settings implemenation, e.g. cache/warping settings, cmd arg parser)
│  ├─ Simulation/ (core simulation implementation, e.g. simulation tree, updating simulation states)
│  ├─ Snapshot/ (cache state snapshot implementation, e.g. saving and matching cache states)
│  ├─ Util/ (utilities, e.g. middleware for ISL library, string utils, global variables)
│  └─ Warping/ (warping implementation, e.g. checks for warping, warping the simulation state)
├─ build-debug.sh (debug build script)
├─ build-release.sh (release build script))
├─ CMakeLists.txt
├─ format.sh (script to format the source code with clang-format)
└─ README.md (detailed information about how to build/run the tool, cmd args)
```

We refer to `warping-cache-simulation/src` directory as the `src` directory.

## Connections between the source code and the structures presented in our paper

### Simulation tree
The source code related to the simulation tree that is presented in our paper is under `src/Simulation/`:
- `AccessSimulationNode.[cpp,hpp]`: Simulation tree access node implementation
- `IterationSimulationNode.[cpp,hpp]`: Simulation tree loop node implementation
- `SimulationNode.[cpp,hpp]`: Simulation tree node base class
- `SimulationTree.[cpp,hpp]`: Simulation tree implementation
- `SimulationTreeBuilder.[cpp,hpp]`: Building the simulation tree from program info (scop) and simulation settings
- `NodeId.[cpp,hpp]`: Simulation node id

As explained in our paper, the simulation is run on top of the simulation tree.
We basically call the `Simulate` method of each `SimulationNode` while traversing the simulation tree.
This method processes memory accesses (if the node contains any) and updates the simulation state (cache state and simulation results) accordingly.
Each subtree is traversed possibly multiple times according to the iteration domains of simulation nodes.

### Cache state
For performance reasons, the concrete and symbolic cache states mentioned in our paper are represented as a single structure in the source code.
Namely, each cache line in the cache state contains both the symbolic and the concrete memory blocks.
For this reason, unlike our paper, we don't have different functions for updating the concrete and symbolic cache states in our source code.

The base cache state class is `CacheState` (`src/CacheState/CacheState.[hpp,cpp]`) which is extended by `FullyAssociativeCacheState`, `SetAssociativeCacheState` and `HierarchicalCacheState` classes.
There is also a factory class for creating cache states (`CacheStateFactory`).
Anything related to the cache state implementation can be found under `src/CacheState` directory.
There are currently implementations for LRU, PRU, LRU3PLRU4, and QLRU replacement policies.

#### Adding a new replacement policy
Our tool can be easily extended with new replacement policies.
You only need to implement a new class by extending the `ReplacementPolicy` class (`src/CacheState/ReplacementPolicy.hpp`).
There are a couple of virtual methods in `ReplacementPolicy` such as updating the cache state by accessing a specific cache line or checking whether the cache state contains a given memory block.
Any class extending the `ReplacementPolicy` by implementing these methods can be used as a replacement policy in our simulation tool.
Please refer to the existing replacement policy implementations under `src/CacheState` and their usages for more details.

## Connections between the source code and the algorithms presented in our paper

### Algorithm 1: Non-warping cache simulation
Algorithm 1 represented in our paper corresponds to the plain simulation where we don't apply our warping technique.
The procedure `LoopNode::Simulate` corresponds to the `IterationSimulationNode::Simulate` method.
The procedure `AccessNode::Simulate` corresponds to the `AccessSimulationNode::Simulate` method.

The `CacheState::ProcessAccesses` method call during `AccessNode::Simulate` handles updating the cache state, i.e., `UpCache` and `ClCache` in Algorithm 1.

The `IterationSimulator::Simulate` method call during `LoopNode::Simulate` handles updating the iterator state, i.e., `i <- i + this.stride` in Algorithm 1.
Note that the `IterationSimulator` here is the base class for updating the iterator state.
In the case of Algorithm 1, we use `NonWarpingIterationSimulator` as the `IterationSimulator` here.

### Algorithm 2: Warping cache simulation
Algorithm 2 represented in our paper corresponds to the simulation where we apply our warping technique.
At the first glance, the source codes of Algorithm 1 and 2 look identical but the underlying helper classes differ.

Just like in the case of Algorithm 1, the procedures `LoopNode::WarpingSimulate` and `AccessNode::WarpingSimulate` of Algorithm 2 correspond to `IterationSimulationNode::Simulate` and `AccessSimulationNode::Simulate` methods in the source code.
However, in the case of Algorithm 2, we use `WarpingIterationSimulator` as the underlying iteration simulator in `IterationSimulationNode::Simulate`.
The class `WarpingIterationSimulator` applies warping via the `WarpingManager` class (`src/Warping/WarpingManager.[cpp,hpp]`).
Anything related to warping during the simulation is handled by the `WarpingManager` including the checks given in Algorithm 2.
We now give the details about how the parts of Algorithm 2 relate to the source code.

The check `if x.contains(sym-c) then` at line 6 in Algorithm 2 is searching for matching cache states (e.g. symbolically equivalent cache states).
In the source code, this corresponds to the `WarpingManager::SaveSnapshotAndFindMatch` call during the `WarpingManager::TryWarping` method.
With this call, we take a snapshot of the cache state and save it.
After saving, we look for a matching cache state.
If a match is found, we try warping via the `WarpingManager::TryWarpingAccordingToSnapshotMatch` call.
This call includes the `IterationsToWarp` procedure used in Algorithm 2.
We now give the details of how this procedure relates to the source code.

The procedure `IterationsToWarp` corresponds to the `WarpingManager::ComputeIterationsToWarp` method which uses the `FurthestWarpingPointManager` helper class (`src/warping/FurthestWarpingPointManager.[cpp,hpp]`).
The `FurthestWarpingPointManager::ComputeIterationsToWarp` method computes the number of iterations to warp by calling other methods of the same class.
The procedures used in `IterationsToWarp` correspond to these methods as follows:
- `FurthestByDomains`: `FurthestWarpingPointManager::ComputeIterationsToWarpAccordingToConditionalConflicts`
- `FurthestByOverlap`: `FurthestWarpingPointManager::ComputeIterationsToWarpAccordingToCoefficientConflicts`
- `CacheAgrees`: `FurthestWarpingPointManager::DoesCacheBijectionComplyWithAccessBijection`

The last helper procedure presented in our paper `ConstructAccessMapping` is a part of the `BijectionVerifier::DoesCacheBijectionComplyWithAccessBijection` method.
That method calls `SnapshotMatch::GetOldLines` and `SnapshotMatch::GetNewLines` methods.
The results of these methods together represent the cache line mapping between the matching cache states.

### Important parameters used in the code
Information about the command-line parameters is already given in `warping-cache-simulation/README.md`.
Here, we give some details about how the important parameters relate to the source code.

Our tool is used as:
```
./warping-cache-simulation <SRC> <CACHE_CONFIG> -w <WARPING> -h <SMART_WARPING> -i <INCLUDE_DIRS>
```
where the command line parameters are:
- \<SRC\>: C source file of the program we are simulating
- \<CACHE_CONFIG\>: cache config file
- \<WARPING\>: true or false
- \<SMART_WARPING\>: true or false
- \<INCLUDE_DIRS\>: list of include dirs, separated by comma, default empty

We now present the details of cache and warping parameters.

#### Cache parameters
A cache config files consists details about the cache we want to simulate such as cache size, line size, associativity, replacement policy, and write policy.
You can examine/use the cache config files given under the  `scripts/reproduction/cache-configs` directory.
The files are self-explanatory.

The parameters given in the cache config file are represented by the `CacheSettings` class (`src/Settings/CacheSettings.[cpp,hpp]`).
The parsing of the cache config file is handled by the `CacheSettingsParser` class (`src/Settings/CacheSettingsParser.[cpp,hpp]`).
The `CacheStateFactory::CreateCacheState` method takes a `CacheSettings` parameter and creates the necessary cache state instance.

#### Warping parameters
The warping parameters are represented by the `WarpingSettings` class (`src/Settings/WarpingSettings.[cpp,hpp]`).
The parsing of the cache config file is handled by the `WarpingSettingsParser` class (`src/Settings/WarpingSettingsParser.[cpp,hpp]`).

The \<WARPING\> cmd arg is represented by the `tryWarping` field of `WarpingSettings`.
When this field is false, we use don't apply warping and do plain simulation (Algorithm 1 in our paper).
Otherwise, we apply warping during the simulation (Algorithm 2 in our paper).

The \<SMART_WARPING\> cmd arg is represented by the `useHeuristics` field of `WarpingSettings`.
If this field is true, we apply warping during the simulation using the `SmartWarpingManager` class (`src/Warping/SmartWarpingManager.[cpp,hpp]`).
Otherwise, we use the `VanillaWarpingManager` class (`src/Warping/VaniallaWarpingManager.[cpp,hpp]`) which does not use any heuristics while warping.
