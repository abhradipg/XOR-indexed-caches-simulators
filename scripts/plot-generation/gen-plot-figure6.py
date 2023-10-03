#!/usr/bin/env python3
import util
import matplotlib
matplotlib.use('Agg')
import numpy as np
import matplotlib.pyplot as plt

data_path, plots_path, dataset = util.get_data_and_plots_paths()

DURATION_KEY = "timeMSec"
WARP_KEY = "notWarpedAccessPercentages"

FIG_WIDTH = 8
FIG_HEIGHT = 5.5
HEIGHT_RATIOS = [1, 1]
PLOT_FILE = "{}/figure6-{}-speedup-and-warp-repl-pol.pdf".format(plots_path, dataset)

fifo_warp_json = "{}/json/{}/true-true-fifo-cascadelake-l1.json".format(data_path, dataset)
lru_warp_json = "{}/json/{}/true-true-lru-cascadelake-l1.json".format(data_path, dataset)
plru_warp_json = "{}/json/{}/true-true-cascadelake-l1.json".format(data_path, dataset)
qlru_warp_json = "{}/json/{}/true-true-qlru-cascadelake-l1.json".format(data_path, dataset)

fifo_nowarp_json = "{}/json/{}/false-false-fifo-cascadelake-l1.json".format(data_path, dataset)
lru_nowarp_json = "{}/json/{}/false-false-lru-cascadelake-l1.json".format(data_path, dataset)
plru_nowarp_json = "{}/json/{}/false-false-cascadelake-l1.json".format(data_path, dataset)
qlru_nowarp_json = "{}/json/{}/false-false-qlru-cascadelake-l1.json".format(data_path, dataset)


warp_json_files = [
    fifo_warp_json,
    lru_warp_json,
    plru_warp_json,
    qlru_warp_json
]

nowarp_json_files = [
    fifo_nowarp_json,
    lru_nowarp_json,
    plru_nowarp_json,
    qlru_nowarp_json
]

BAR_LABELS = ["LRU", "FIFO", "Pseudo-LRU", "Quad-age LRU"]

warp_results = util.read_json_files(warp_json_files)
nowarp_results = util.read_json_files(nowarp_json_files)
assert(len(warp_results) == len(nowarp_results))

n_results = len(warp_results)


def check_all_have_same_kernels(results):
    kernels = sorted([k for k in results[0]])
    for result in results:
        assert(sorted([k for k in result]) == kernels)

check_all_have_same_kernels(warp_results + nowarp_results)

fig, (ax1, ax2) = plt.subplots(2, sharex=True,
                               gridspec_kw={'height_ratios': HEIGHT_RATIOS})

fig.set_figheight(FIG_HEIGHT)
fig.set_figwidth(FIG_WIDTH)
fig.set_tight_layout(True)
margin = 0.1
bar_width = (1.-2.*margin) / 1.5

def get_kernel_ms(res, kernel):
    return float(res[kernel][DURATION_KEY])

def get_kernel_speedup(nowarp_res, warp_res, kernel):
    nowarp_ms = get_kernel_ms(nowarp_res, kernel)
    warp_ms = get_kernel_ms(warp_res, kernel)
    return nowarp_ms / warp_ms

def get_speedup_data():
    return [{kernel: get_kernel_speedup(nowarp_res, warp_res, kernel)
             for kernel in nowarp_res}
            for nowarp_res, warp_res in zip(nowarp_results, warp_results)]

def get_kernels_ordered_by_speedup():
    all_data = get_speedup_data()
    first_data = all_data[0]
    first_data = util.sort_dict_by_val(first_data, reverse=True)
    kernels = [k for k, _ in first_data.items()]
    return kernels

def get_warp_data():
    return [{kernel: float(result[kernel][WARP_KEY][0])
             for kernel in result}
            for result in warp_results]

def plot_duration():
    all_data = get_speedup_data()
    kernels = get_kernels_ordered_by_speedup()
    all_data = [
        [data[k] for k in kernels]
        for data in all_data
    ]
    plot_info = {
        "all_data": all_data,
        "kernels": kernels,
        "x_label": "kernel",
        "y_label":  "speedup",
        "bar_labels": BAR_LABELS,
        "logscale": True,
        "legend": True,
        "ax": ax2
    }
    util.ax_plot_grouped_bar_chart(**plot_info)

def plot_warp():
    all_data = get_warp_data()
    kernels = get_kernels_ordered_by_speedup()
    all_data = [
        [data[k] for k in kernels]
        for data in all_data
    ]
    plot_info = {
        "all_data": all_data,
        "kernels": kernels,
        "y_label":  "non-warped accesses [%]",
        "bar_labels": BAR_LABELS,
        "logscale": True,
        "legend": False,
        "ax": ax1
    }
    util.ax_plot_grouped_bar_chart(**plot_info)

plot_duration()
plot_warp()
plt.xticks(rotation=90)
#plt.show()
plt.savefig(PLOT_FILE)
