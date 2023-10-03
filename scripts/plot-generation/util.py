import matplotlib
matplotlib.use('Agg')
import sys
import os
import json
import numpy as np
import csv
import pathlib
import matplotlib.pyplot as plt
from collections import OrderedDict

def get_data_and_plots_paths():
    data_type = sys.argv[1]
    plotgen_dir = pathlib.Path(__file__).parent.resolve()
    artifact_dir = plotgen_dir / ".." / ".."
    assert data_type == "existing" or data_type == "reproduced"
    data_path = artifact_dir / "data" / data_type
    plots_path = artifact_dir / "plots" / "from-{}-data".format(data_type)
    return (data_path, plots_path, sys.argv[2])

def get_file_names():
    assert len(sys.argv) > 1
    return sys.argv[1:]

def get_result_names():
    return [os.path.splitext(os.path.basename(file_name))[0]
            for file_name in get_file_names()]

def read_result_files():
    results = []
    for file_name in get_file_names():
        with open(file_name, 'r') as json_file:
            results.append(json.load(json_file))
    return results

def read_json_files(json_files):
    results = []
    for file_name in json_files:
        with open(file_name, 'r') as json_file:
            results.append(json.load(json_file))
    return results

def create_csv_file_name(addition):
    file_names = get_file_names()
    assert len(file_names) == 1
    json_file_name = file_names[0]
    csv_file_name = json_file_name.replace(".json", "-{}.csv".format(addition))
    return csv_file_name.replace("json", "csv")

def sort_dict(di):
    return OrderedDict(sorted(di.items()))

def sort_dict_by_val(di, reverse=False):
    return dict(sorted(di.items(), key=lambda item: item[1], reverse=reverse))

def mean(iterable):
    return np.mean(iterable)

def add_epsilon_to_zeros(list):
    return [var if var !=0 else var + sys.float_info.epsilon for var in list]

def get_errs(actuals, predicts):
    return [(p - a) for a, p in zip(actuals, predicts)]

def get_abs_errs(actuals, predicts):
    return [abs(a - p) for a, p in zip(actuals, predicts)]

def get_rel_errs(actuals, predicts):
    return [100 * abs((a - p) / a) for a, p in zip(actuals, predicts)]

class Stats:
    def __init__(self, stat_dict):
        self.stat_dict = stat_dict
        self.l1_misses = [val["l1_miss"] for _, val in stat_dict.items()]
        if "l2_miss" in next(iter(stat_dict.items())):
            self.l2_misses = [val["l2_miss"] for _, val in stat_dict.items()]
        self.accesses = [val["access"] for _, val in stat_dict.items()]
        self.kernels = stat_dict.keys()

    def get_kernels(self):
        return self.kernels

    def get_kernels_ordered_by_accesses(self):
        a = sorted([(k,v["access"]) for k, v in self.stat_dict.items()],
                   key=lambda x: x[1])
        return [k for k, _ in a]

    def get_kernels_ordered_by_l1_miss(self):
        a = sorted([(k,v["l1_miss"]) for k, v in self.stat_dict.items()],
                   key=lambda x: x[1])
        return [k for k, _ in a]

    def get_accesses_by_kernel_order(self, kernel_order):
        return [self.stat_dict[k]["access"] for k in kernel_order]

    def get_l1_miss_errs(self, papi_stats):
        return get_errs(actuals=papi_stats.l1_misses,
                        predicts=self.l1_misses)

    def get_l1_miss_abs_errs(self, papi_stats):
        return get_abs_errs(actuals=papi_stats.l1_misses,
                            predicts=self.l1_misses)

    def get_l1_miss_rel_errs(self, papi_stats):
        return get_rel_errs(actuals=papi_stats.l1_misses,
                            predicts=self.l1_misses)

    def get_l2_miss_abs_errs(self, papi_stats):
        return get_abs_errs(actuals=papi_stats.l2_misses,
                            predicts=self.l2_misses)

    def get_l2_miss_rel_errs(self, papi_stats):
        return get_rel_errs(actuals=papi_stats.l2_misses,
                            predicts=self.l2_misses)

    def get_l1_closeness(self, papi_stats):
        papi_l1_misses = papi_stats.l1_misses
        return [100 * (p / a) for p, a in zip(self.l1_misses, papi_l1_misses)]

    def get_l2_closeness(self, papi_stats):
        papi_l2_misses = papi_stats.l2_misses
        return [100 * (p / a) for p, a in zip(self.l2_misses, papi_l2_misses)]

    def get_l1_closeness_rel_err(self, papi_stats):
        papi_l1_misses = papi_stats.l1_misses
        return [100 * abs(p - a) / a
                for p, a in zip(self.l1_misses, papi_l1_misses)]

    def get_l2_closeness_rel_err(self, papi_stats):
        papi_l2_misses = papi_stats.l2_misses
        return [100 * abs(p - a) / a
                for p, a in zip(self.l2_misses, papi_l2_misses)]

    def get_l1_miss(self, kernels):
        return [self.stat_dict[k]["l1_miss"] for k in kernels]

    def get_l2_miss(self, kernels):
        return [self.stat_dict[k]["l2_miss"] for k in kernels]

    def get_l1_miss_percs(self):
        return [100 * (m / a) for m, a in zip(self.l1_misses, self.accesses)]

    def get_l2_miss_percs(self):
        return [100 * (m / a) for m, a in zip(self.l2_misses, self.accesses)]

    def get_l1_miss_percs_papi_only(self, accesses):
        assert(all(a == -1 for a in self.accesses))
        assert(all(a >= 0 for a in accesses))
        return [100 * (m / a) for m, a in zip(self.l1_misses, accesses)]

    def get_l2_miss_percs_papi_only(self, accesses):
        assert(all(a == -1 for a in self.accesses))
        assert(all(a >= 0 for a in accesses))
        return [100 * (m / a) for m, a in zip(self.l2_misses, accesses)]

    def get_l1_miss_percs_abs_err(self, papi_stats):
        papi_l1_misses = papi_stats.l1_misses
        papi_miss_percs = [100 * (m / a)
                           for m, a in zip(papi_l1_misses, self.accesses)]
        return [abs(p - a)
                for p, a in zip(papi_miss_percs, self.get_l1_miss_percs())]

    def get_l2_miss_percs_abs_err(self, papi_stats):
        papi_l2_misses = papi_stats.l2_misses
        papi_miss_percs = [100 * (m / a)
                           for m, a in zip(papi_l2_misses, self.accesses)]
        return [abs(p - a)
                for p, a in zip(papi_miss_percs, self.get_l2_miss_percs())]


def read_our_miss_stats(miss_csv, without_kernels=[]):
    stat_dict = {}
    with open(miss_csv, newline="") as csvfile:
        spamreader = csv.reader(csvfile, delimiter=" ", quotechar="|")
        for row in spamreader:
            if len(row) == 5:
                assert len(row) == 5
                kernel = row[0].lower()
                if kernel == "kernel" or kernel in without_kernels:
                    continue
                row = [int(el) for el in row[1:]]
                l1_access, l1_miss, l2_access, l2_miss = row
                stat_dict[kernel] = {"access": l1_access,
                                    "l1_miss": l1_miss,
                                    "l2_miss": l2_miss}
            elif len(row) == 3:
                kernel = row[0].lower()
                if kernel == "kernel" or kernel in without_kernels:
                    continue
                row = [int(el) for el in row[1:]]
                l1_access, l1_miss  = row
                stat_dict[kernel] = {"access": l1_access,
                                    "l1_miss": l1_miss}
            else:
                assert(False)
    stat_dict = sort_dict(stat_dict)
    return Stats(stat_dict)

def read_other_miss_stats(miss_csv, without_kernels=[]):
    stat_dict = {}
    with open(miss_csv, newline="") as csvfile:
        spamreader = csv.reader(csvfile, delimiter=" ", quotechar="|")
        for row in spamreader:
            kernel = row[0].lower()
            if kernel == "kernel" or kernel in without_kernels:
                continue
            row = [int(el) for el in row[1:4]]
            l1_access, l1_miss, l2_miss = row
            stat_dict[kernel] = {"access": l1_access,
                                 "l1_miss": l1_miss,
                                 "l2_miss": l2_miss}
    stat_dict = sort_dict(stat_dict)
    return Stats(stat_dict)

def read_duration_stats(duration_csv, without_kernels=[]):
    duration_dict = {}
    with open(duration_csv, newline="") as csvfile:
        spamreader = csv.reader(csvfile, delimiter=" ", quotechar="|")
        for row in spamreader:
            kernel = row[0].lower()
            if kernel == "kernel" or kernel in without_kernels:
                continue
            assert (len(row) == 2)
            duration = row[1]
            duration_dict[kernel] = float(duration)
    duration_dict = sort_dict(duration_dict)
    return DurationStats(duration_dict)

class DurationStats:
    def __init__(self, duration_dict):
        self.duration_dict = duration_dict
        ordered_by_key = sort_dict(duration_dict)
        ordered_by_val = sort_dict_by_val(duration_dict)
        self.kernels_by_name= ordered_by_key.keys()
        self.kernels_by_val = ordered_by_val.keys()

    def get_kernels_ordered_by_name(self):
        return self.kernels_by_name

    def get_kernels_ordered_by_duration(self):
        return self.kernels_by_val

    def get_durations_ms(self, kernel_order):
        return [self.duration_dict[k] for k in kernel_order]

    def get_durations_s(self, kernel_order):
        return [self.duration_dict[k] / 1000 for k in kernel_order]


def plot_grouped_bar_chart(all_data,
                           kernels,
                           fig_width,
                           fig_height,
                           x_label,
                           y_label,
                           bar_labels,
                           logscale=False,
                           symlogscale=False,
                           plot_file=None):
    n_bars_each_group = len(bar_labels)
    assert(len(all_data) == len(bar_labels))

    def get_offset(i):
        return (i - (n_bars_each_group / 2)) * bar_width + bar_width / 2

    margin = 0.1
    bar_width = (1.-2.*margin) / n_bars_each_group

    fig = plt.figure(figsize=(fig_width, fig_height))
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
    ax.yaxis.grid(linestyle='-', alpha=0.5)
    ax.legend()
    if logscale:
        plt.yscale('log')
        ax.set_yscale('log')
        y_minor = matplotlib.ticker.LogLocator(base = 10.0, subs = np.arange(1.0, 10.0) * 0.1, numticks = 10)
        ax.yaxis.set_minor_locator(y_minor)
        ax.yaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
    elif symlogscale:
        plt.yscale('symlog')
    plt.xticks(rotation=90)
    if plot_file:
        plt.savefig(plot_file)
    else:
        plt.show()

def ax_plot_grouped_bar_chart(all_data,
                              kernels,
                              y_label,
                              bar_labels,
                              ax,
                              logscale=False,
                              legend=True,
                              x_label=False):
    n_bars_each_group = len(bar_labels)
    assert(len(all_data) == len(bar_labels))

    def get_offset(i):
        return (i - (n_bars_each_group / 2)) * bar_width + bar_width / 2

    margin = 0.1
    bar_width = (1.-2.*margin) / n_bars_each_group

    xindices = np.arange(len(kernels))
    for i, data in enumerate(all_data):
        xdata = xindices + get_offset(i)
        ax.bar(xdata, data,
            width=bar_width, align='center', zorder=3, label=bar_labels[i])

    ax.set_xticks(xindices)
    ax.set_xticklabels(kernels)
    ax.set_ylabel(y_label)
    if x_label:
        ax.set_xlabel(x_label)

    ax.yaxis.grid(linestyle='-', alpha=0.5, zorder=0)
    if legend:
        ax.legend()
    if logscale:
        ax.set_yscale('log')
        y_minor = matplotlib.ticker.LogLocator(base = 10.0, subs = np.arange(1.0, 10.0) * 0.1, numticks = 10)
        ax.yaxis.set_minor_locator(y_minor)
        ax.yaxis.set_minor_formatter(matplotlib.ticker.NullFormatter())
