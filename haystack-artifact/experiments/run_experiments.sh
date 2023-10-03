#!/bin/bash

# collect the haystack performance data
echo "-> collecting haystack performance data (m|l|xl)"
python perf_baseline_polybench_medium.py
python perf_baseline_polybench_large.py
python perf_baseline_polybench_xlarge.py
echo "-> collecting haystack performance data (1|3 caches)"
python perf_onecache_polybench_large.py
python perf_threecaches_polybench_large.py
echo "-> collecting haystack performance data (no equalization)"
python perf_noequal_polybench_large.py
echo "-> collecting haystack performance data (no rasterization)"
python perf_noraster_polybench_large.py
echo "-> collecting haystack performance data (full enumeration)"
python perf_enumerateall_polybench_large.py
echo "-> collecting haystack performance data (polycache setup)"
python perf_baseline_polybench_3.2_standard.py
echo "-> collecting haystack performance data (tiled)"
python perf_baseline_polybench_tiled.py

# collect the haystack output
echo "-> collecting haystack cache miss data (m|l|xl)"
python acc_baseline_polybench_medium.py
python acc_baseline_polybench_large.py
python acc_baseline_polybench_xlarge.py
echo "-> collecting haystack cache miss data (full enumeration)"
python acc_enumerateall_polybench_large.py

# generate the graphs
echo "-> generating accuracy plots"
python plot_acc_baseline_polybench.py
python plot_dinero_baseline_polybench.py
echo "-> generating performance plots"
python plot_perf_speedup_polybench.py
python plot_perf_baseline_polybench.py
python plot_perf_alternatives.py
