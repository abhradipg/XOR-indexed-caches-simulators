""" tools necessary to run the cache model """

import os
import sys
import subprocess
import re
import math
from scipy.stats import norm

def confidence_interval(values, alpha):
    n = values.count()  
    z = -norm.ppf((1 - alpha)/2) 
    sorted_values = sorted(values)
    high = min(n-1, math.ceil(1 + (n + z * math.sqrt(n))/2) - 1)
    low = max(0, math.floor((n - z * math.sqrt(n))/2) - 1)
    return (sorted_values[low], sorted_values[high])

def ci_low(values):
    return confidence_interval(values, 0.95)[0]

def ci_high(values):
    return confidence_interval(values, 0.95)[1] 

def abs_path(*paths):
    return os.path.abspath(os.path.join(*paths))

def get_polybench_kernels(polybench):
    list_path = abs_path(polybench, "utilities/benchmark_list")
    with open(list_path) as f:
        kernels = f.readlines()
    kernels = [abs_path(polybench, k.strip()) for k in kernels]
    return kernels

def run_cache_model(executable, source, include, timeout):
    command = (
        executable + 
        " --verbose=true" +
        " --include-path=" + include +
        " --input-file=" + source)
    print(" -> executing: " + command)
    try:
        return subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True, timeout=timeout).decode()
    except:
        pass
    else:
        return None

def parse_performance(output):
    total = -1
    preprocess = -1
    postprocess = -1
    # timers
    compute_between_map = 0
    compute_first_map = 0
    compute_prefetch_info = 0
    extract_between_map = 0
    extract_first_map = 0
    count_between_map = 0
    count_compulsory_misses = 0
    count_capacity_misses = 0
    count_nonaffine_misses = 0
    bernstein_approximation = 0
    apply_equalization = 0
    apply_rasterization = 0
    np = re.compile("\d+\.\d+") # number pattern
    for line in output.splitlines():
        if "-> finished after " in line:
            match = np.search(line)
            if match:
                total = float(match.group(0))
        elif "-> done" in line:
            match = np.search(line)
            if match:
                if preprocess == -1:
                    preprocess = float(match.group(0))
                elif postprocess == -1:
                    postprocess = float(match.group(0))
        elif "ComputeBetweenMap" in line:
            match = np.search(line)
            if match:
                compute_between_map = float(match.group(0))
        elif "ComputeFirstMap" in line:
            match = np.search(line)
            if match:
                compute_first_map = float(match.group(0))
        elif "ComputePrefetchInfo" in line:
            match = np.search(line)
            if match:
                compute_prefetch_info = float(match.group(0))
        elif "ExtractBetweenMap" in line:
            match = np.search(line)
            if match:
                extract_between_map = float(match.group(0))
        elif "ExtractFirstMap" in line:
            match = np.search(line)
            if match:
                extract_first_map = float(match.group(0))
        elif "CountBetweenMap" in line:
            match = np.search(line)
            if match:
                count_between_map = float(match.group(0))
        elif "CountCompulsoryMisses" in line:
            match = np.search(line)
            if match:
                count_compulsory_misses = float(match.group(0))
        elif "CountCapacityMisses" in line:
            match = np.search(line)
            if match:
                count_capacity_misses = float(match.group(0))
        elif "CountNonAffineMisses" in line:
            match = np.search(line)
            if match:
                count_nonaffine_misses = float(match.group(0))
        elif "BernsteinApproximation" in line:
            match = np.search(line)
            if match:
                bernstein_approximation = float(match.group(0))
        elif "ApplyEqualization" in line:
            match = np.search(line)
            if match:
                apply_equalization = float(match.group(0))
        elif "ApplyRasterization" in line:
            match = np.search(line)
            if match:
                apply_rasterization = float(match.group(0))
    # timout -> total is minus 1!
    return [("total [ms]", total),
            ("preprocess [ms]", preprocess),
            ("postprocess [ms]", postprocess),
            ("compute_between_map [ms]", compute_between_map),
            ("compute_first_map [ms]", compute_first_map),
            ("compute_prefetch_info [ms]", compute_prefetch_info),
            ("extract_between_map [ms]", extract_between_map),
            ("extract_first_map [ms]", extract_first_map),
            ("count_between_map [ms]", count_between_map),
            ("count_compulsory_misses [ms]", count_compulsory_misses),
            ("count_capacity_misses [ms]", count_capacity_misses),
            ("count_nonaffine_misses [ms]", count_nonaffine_misses),
            ("bernstein_approximation [ms]", bernstein_approximation),
            ("apply_equalization [ms]", apply_equalization),
            ("apply_rasterization [ms]", apply_rasterization)]

def parse_output(output):
    sp = re.compile(r"\w+\([RW]\d+\)") # statement pattern
    np = re.compile(r"\d+") # number pattern
    mp = re.compile(r"counted \d+/\d+/\d+/\d+") # miss pattern
    pp = re.compile(r"\d+/\d+ prefetch") # prefetch pattern
    cp = re.compile(r"with \d+/\d+/\d+") # count pattern
    dp = re.compile(r"after \d+/\d+/\d+/\d+") # decomposition pattern
    rows = []
    for line in output.splitlines():
        if "splits" in line:
            # initialize line info
            statement = None
            compulsory = 0
            capacity0 = 0
            capacity1 = 0
            accesses = 0
            prefetch0 = 0
            prefetch1 = 0
            barvinok_count = 0
            bernstein_count = 0
            enumerate_count = 0
            barvinok_splits = 0
            equalize_splits = 0
            raster_splits = 0
            enumeration_splits = 0
            # parse the statement
            match = sp.search(line)
            if match:
                statement = match.group(0)
            # parse the misses
            match = mp.search(line)
            if match:
                misses = np.findall(match.group(0))
                compulsory = misses[0]
                capacity0 = misses[1]
                capacity1 = misses[2]
                accesses = misses[3]
            # parse the prefetch info
            match = pp.search(line)
            if match:
                prefetch = np.findall(match.group(0))
                prefetch0 = prefetch[0]
                prefetch1 = prefetch[1]
            # counting method
            match = cp.search(line)
            if match:
                methods = np.findall(match.group(0))
                barvinok_count = methods[0]
                bernstein_count = methods[1]
                enumerate_count = methods[2]
            # decomposition count
            match = dp.search(line)
            if match:
                splits = np.findall(match.group(0))
                barvinok_splits = splits[0]
                equalize_splits = splits[1]
                raster_splits = splits[2]
                enumeration_splits = splits[3]
            # compute the row
            row = [("statement",statement),
                ("compulsory",compulsory),
                ("capacity0",capacity0),
                ("capacity1",capacity1),
                ("accesses",accesses),
                ("prefetch0",prefetch0),
                ("prefetch1",prefetch1),
                ("barvinok_count",barvinok_count),
                ("bernstein_count",bernstein_count),
                ("enumerate_count",enumerate_count),
                ("barvinok_splits",barvinok_splits),
                ("equalize_splits",equalize_splits),
                ("raster_splits",raster_splits),
                ("enumeration_splits",enumeration_splits)]
            rows.append(row)
    return rows

