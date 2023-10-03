import matplotlib
matplotlib.use('Agg')
import util
import matplotlib.pyplot as plt
import numpy as np
import sys

FASTER_COLOR = "tab:green"
SLOWER_COLOR = "tab:orange"

def get_kernel_speedup_pairs(duration_csv1, duration_csv2):
    stats1 = util.read_duration_stats(duration_csv1)
    stats2 = util.read_duration_stats(duration_csv2)
    kernels1 = stats1.get_kernels_ordered_by_name()
    kernels2 = stats2.get_kernels_ordered_by_name()
    if kernels1 != kernels2:
        print("Some kernels are missing, generating the plot without them!")
    kernels = [k for k in kernels1 if k in kernels2]
    speedups = [(kernel, float(ms1) / float(ms2))
                for kernel, ms1, ms2 in zip(kernels,
                                            stats1.get_durations_ms(kernels),
                                            stats2.get_durations_ms(kernels))
                if float(ms1) > 0 and float(ms2) > 0]
    if len(speedups) == 0:
        print("There are no common kernels for \n\t{} and \n\t{}".format(duration_csv1, duration_csv2))
        sys.exit("Exiting without generating a plot!")
    return sorted(speedups, key=lambda x: x[1], reverse=True)

def save_speedup_plot(duration_csv1, duration_csv2, plot_file, width=6, height=3, limit_top=True, limit_bot=True):
    speedup_pairs = get_kernel_speedup_pairs(duration_csv1, duration_csv2)
    names = [name for name, _ in speedup_pairs]
    values = [val for _, val in speedup_pairs]
    indices = [i for i in range(len(values))]
    colors = [SLOWER_COLOR if val < 1.0
              else FASTER_COLOR for val in values]
    min_val = min(values)
    max_val = max(values)
    lim_min = 1
    while lim_min > min_val:
        lim_min = lim_min / 10
    lim_max = 1
    while lim_max < max_val:
        lim_max = lim_max * 10
    fig = plt.figure(figsize=(width, height))
    fig.set_tight_layout(True)
    ax = fig.add_subplot(111)
    ax.vlines(ymin=1, ymax=values, x=indices, color=colors, linewidth=6)
    ax.set_xlabel("kernel")
    ax.set_ylabel("speedup")
    ax.grid(linestyle='-', alpha=0.5)
    ax.set_xticks(indices)
    ax.set_xticklabels(names)
    plt.xticks(rotation=90)
    plt.yscale('log')
    ax.tick_params(right=True, which='both')
    if limit_bot:
        plt.ylim(bottom=lim_min)
    if limit_top:
        plt.ylim(top=lim_max)
    plt.savefig(plot_file)