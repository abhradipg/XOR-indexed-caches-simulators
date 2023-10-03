#!/bin/bash

# collect the haystack performance data
echo "-> collecting haystack performance data (m|l|xl)"
python perf_baseline_polybench_medium.py
python perf_baseline_polybench_large.py
python perf_baseline_polybench_xlarge.py
echo "-> collecting haystack performance data (1|3 caches)"
python perf_onecache_polybench_large.py
python perf_threecaches_polybench_large.py
echo "-> collecting haystack performance data (polycache setup)"
python perf_baseline_polybench_3.2_standard.py

# collect the haystack output
echo "-> collecting haystack cache miss data (m|l|xl)"
python acc_baseline_polybench_medium.py
python acc_baseline_polybench_large.py
python acc_baseline_polybench_xlarge.py

# generate the graphs
echo "-> generating accuracy plots"
python plot_acc_baseline_polybench.py
python plot_dinero_baseline_polybench.py
echo "-> generating performance plots"
python plot_perf_baseline_polybench_main.py
python plot_perf_alternatives.py
