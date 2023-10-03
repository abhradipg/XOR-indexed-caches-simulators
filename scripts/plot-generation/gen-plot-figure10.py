#!/usr/bin/env python3
import matplotlib
matplotlib.use('Agg')
import util
import numpy as np
import matplotlib.pyplot as plt

data_path, plots_path, dataset = util.get_data_and_plots_paths()

lru_csv = "{}/miss/warping-{}-lru-l1.csv".format(data_path, dataset)
plru_csv = "{}/miss/warping-{}-plru-l1.csv".format(data_path, dataset)
qlru_csv = "{}/miss/warping-{}-qlru-l1.csv".format(data_path, dataset)
fifo_csv = "{}/miss/warping-{}-fifo-l1.csv".format(data_path, dataset)
full_assoc_lru_csv = "{}/miss/warping-{}-full-assoc-lru-l1.csv".format(data_path, dataset)
width = 6
height = 3
plot_file = "{}/figure10-{}-warping-repl-pol-miss.pdf".format(plots_path, dataset)

lru_miss_stats = util.read_our_miss_stats(lru_csv)
plru_miss_stats = util.read_our_miss_stats(plru_csv)
qlru_miss_stats = util.read_our_miss_stats(qlru_csv)
fifo_miss_stats = util.read_our_miss_stats(fifo_csv)
full_assoc_lru_miss_stats = util.read_our_miss_stats(full_assoc_lru_csv)

stats = [
    full_assoc_lru_miss_stats,
    plru_miss_stats,
    qlru_miss_stats,
    fifo_miss_stats,
]

bar_labels = [
    "Fully-associative LRU",
    "Pseudo-LRU",
    "Quad-age LRU",
    "FIFO",
]

kernels = lru_miss_stats.get_kernels()
for stat in stats:
    assert (sorted(kernels) == sorted(stat.get_kernels()))

base_miss_data = lru_miss_stats.get_l1_miss(kernels)

all_data = [stat.get_l1_miss(kernels) for stat in stats]

normalized = [[(miss/ base_miss)
               for (base_miss, miss) in zip(base_miss_data, data)]
              for data in all_data]

all_data = normalized
x_label = "kernel"
y_label = "misses / LRU misses"


n_bars_each_group = len(bar_labels)
assert(len(all_data) == len(bar_labels))

def get_offset(i):
    return (i - (n_bars_each_group / 2)) * bar_width + bar_width / 2

margin = 0.1
bar_width = (1.-2.*margin) / n_bars_each_group

fig = plt.figure(figsize=(width, height))
fig.set_tight_layout(True)
ax = fig.add_subplot(111)

xindices = np.arange(len(kernels))
for i, data in enumerate(all_data):
    xdata = xindices + get_offset(i)
    ax.bar(xdata, data,
        width=bar_width, align='center', zorder=3, label=bar_labels[i])

ax.set_xticks(xindices)
ax.set_xticklabels(kernels)
ax.set_ylabel(y_label)
ax.set_xlabel(x_label)
#ax.set_yscale('log')
ax.yaxis.grid(linestyle='-', alpha=0.5)
ax.legend(prop={'size': 9}, loc='lower right')
plt.ylim(top=1.5)
plt.xticks(rotation=90)
plt.savefig(plot_file)
