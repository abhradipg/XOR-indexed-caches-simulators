#!/usr/bin/env python3
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import util

data_path, plots_path, dataset = util.get_data_and_plots_paths()

FIG_HEIGHT = 5.5
FIG_WIDTH = 8
plot_file = "{}/figure11-{}-papi-accuracy-l1.pdf".format(plots_path, dataset)

WARPING_MISS_CSV = "{}/miss/warping-{}-plru-l1.csv".format(data_path, dataset)
HAYSTACK_MISS_CSV = "{}/miss/haystack-{}-l1-l2.csv".format(data_path, dataset)
DINERO_MISS_CSV = "{}/miss/dinero-{}-l1-l2.csv".format(data_path, dataset)
PAPI_MISS_CSV = "{}/miss/papi-{}-o2.csv".format(data_path, dataset)

war_stats = util.read_our_miss_stats(WARPING_MISS_CSV)
din_stats = util.read_other_miss_stats(DINERO_MISS_CSV)
hay_stats = util.read_other_miss_stats(HAYSTACK_MISS_CSV)
pap_stats = util.read_other_miss_stats(PAPI_MISS_CSV)

labels = ["DineroIV", "Warping", "HayStack"]
kernels = war_stats.get_kernels()
xindices = np.arange(len(kernels))
fig, (ax_rel, ax_abs) = plt.subplots(2, sharex=True, gridspec_kw={'height_ratios': [1, 3]})

margin = 0.1
width = (1.-2.*margin) / 3

def plot_rel_err():
    vals = [
        din_stats.get_l1_miss_rel_errs(pap_stats),
        war_stats.get_l1_miss_rel_errs(pap_stats),
        hay_stats.get_l1_miss_rel_errs(pap_stats)
    ]

    ylabel = "rel. error [%]"
    for i, v in enumerate(vals):
        xdata = xindices + margin + (i * width)
        ax_rel.bar(xdata, v, width, zorder=5, label=labels[i])
    ax_rel.set_ylabel(ylabel)
    ax_rel.yaxis.grid(linestyle='-', alpha=0.5)
    ax_rel.legend()

def plot_abs_err():
    vals = [din_stats.get_l1_miss_abs_errs(pap_stats),
            war_stats.get_l1_miss_abs_errs(pap_stats),
            hay_stats.get_l1_miss_abs_errs(pap_stats)]
    ylabel = "abs. err"
    xlabel = "kernels"
    for i, v in enumerate(vals):
        xdata = xindices + margin + (i * width)
        ax_abs.bar(xdata, v, width, zorder=5, label=labels[i])
    ax_abs.set_ylabel(ylabel)
    ax_abs.set_xlabel(xlabel)
    ax_abs.set_xticks(xindices + 1 / 3)
    ax_abs.set_xticklabels(kernels)
    ax_abs.yaxis.grid(linestyle='-', alpha=0.5)
    ax_abs.set_yscale('log')
    y_minor = matplotlib.ticker.LogLocator(base = 10.0, subs = np.arange(1.0, 10.0) * 0.1, numticks = 10)
    ax_abs.yaxis.set_minor_locator(y_minor)
    ax_abs.yaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())

def plot():
    plot_rel_err()
    plot_abs_err()
    fig.set_figheight(FIG_HEIGHT)
    fig.set_figwidth(FIG_WIDTH)
    fig.set_tight_layout(True)
    #matplotlib.pyplot.sca(ax1)
    #plt.xticks(fontsize=6, rotation=45)
    #matplotlib.pyplot.sca(ax2)
    plt.xticks(rotation=90)
    plt.savefig(plot_file)

plot()