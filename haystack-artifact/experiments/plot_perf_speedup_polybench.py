""" plot the stuff """

import utils
import pandas
import matplotlib.pyplot
import matplotlib.axes
import numpy

INPUT = "./data/polybench_large/perf_baseline.csv"
INPUTNR = "./data/polybench_large/perf_noraster.csv"
INPUTNE = "./data/polybench_large/perf_noequal.csv"

INPUTOC = "./data/polybench_large/perf_onecache.csv"
INPUTTC = "./data/polybench_large/perf_threecaches.csv"

INPUTEA = "./data/polybench_large/perf_enumerateall.csv"
INPUTBS = "./data/polybench_large/perf_bernstein.csv"

# also load the accuracy numbers
INPUTACC = "./data/polybench_large/acc_baseline.csv"
INPUTACCNE = "./data/polybench_large/acc_noequal.csv"

INPUTACCEA = "./data/polybench_large/acc_enumerateall.csv"
INPUTACCBS = "./data/polybench_large/acc_bernstein.csv"

PLOTELIM = "./plots/perf_floorelim_polybench_large.pdf"
PLOTCACHES = "./plots/perf_caches_polybench_large.pdf"
PLOTENUMERATEALL = "./plots/perf_enumerateall_polybench_large.pdf"
PLOTBERNSTEIN = "./plots/perf_bernstein_polybench_large.pdf"



def plot_floorelim_speedup():
    # load the data frames
    data = pandas.read_csv(INPUT)
    datanr = pandas.read_csv(INPUTNR)
    datane = pandas.read_csv(INPUTNE)
    dataea = pandas.read_csv(INPUTEA)

    acc = pandas.read_csv(INPUTACC)
    # accne = pandas.read_csv(INPUTACCNE)
    # accea = pandas.read_csv(INPUTACCEA)

    # necessary since we gathered the data in multiple steps
    data = data.sort_values("kernel")
    dataea = dataea.sort_values("kernel")
    datanr = datanr.sort_values("kernel")
    datane = datane.sort_values("kernel")
    acc = acc.sort_values("kernel")
    # accne = accne.sort_values("kernel")
    # accea = accea.sort_values("kernel")
    data = data.reset_index(drop=True)
    dataea = dataea.reset_index(drop=True)
    datanr = datanr.reset_index(drop=True)
    datane = datane.reset_index(drop=True)
    acc = acc.reset_index(drop=True)
    # accne = accne.reset_index(drop=True)
    # accea = accea.reset_index(drop=True)

    # compute the speedups
    datane.loc[:, datane.columns == "total [ms]"] = datane["total [ms]"] / datanr["total [ms]"]
    datanr.loc[:, datanr.columns == "total [ms]"] =  datanr["total [ms]"] / data["total [ms]"]
    dataea.loc[:, dataea.columns == "total [ms]"] =  dataea["total [ms]"] / data["total [ms]"]
    
    
    # compute the total times
    timingsne = datane.groupby("kernel").aggregate({
        "total [ms]" : ["median", utils.ci_low, utils.ci_high]})
    timingsnr = datanr.groupby("kernel").aggregate({
        "total [ms]" : ["median", utils.ci_low, utils.ci_high]})
    timingsea = dataea.groupby("kernel").aggregate({
        "total [ms]" : ["median", utils.ci_low, utils.ci_high]})

    timingsne.loc[:,("total [ms]","ci_low")] = -(timingsne["total [ms]"]["ci_low"] - timingsne["total [ms]"]["median"])
    timingsne.loc[:,("total [ms]","ci_high")] = timingsne["total [ms]"]["ci_high"] - timingsne["total [ms]"]["median"]

    timingsnr.loc[:,("total [ms]","ci_low")] = -(timingsnr["total [ms]"]["ci_low"] - timingsnr["total [ms]"]["median"])
    timingsnr.loc[:,("total [ms]","ci_high")] = timingsnr["total [ms]"]["ci_high"] - timingsnr["total [ms]"]["median"]

    timingsea.loc[:,("total [ms]","ci_low")] = -(timingsea["total [ms]"]["ci_low"] - timingsea["total [ms]"]["median"])
    timingsea.loc[:,("total [ms]","ci_high")] = timingsea["total [ms]"]["ci_high"] - timingsea["total [ms]"]["median"]

    # extract the splits
    splits = acc.groupby("kernel").aggregate({
        "raster_splits" : ["sum"],
        "equalize_splits" : ["sum"],
        "enumeration_splits" : ["sum"],
        "barvinok_splits" : ["sum"]})

    # enuerate all
    product = 1
    count = 0
    for kernel in timingsea.index.values:
        if splits["enumeration_splits"]["sum"][kernel] != 0:
            product = product * timingsea["total [ms]"]["median"][kernel]
            count = count + 1
    geomean = product**(1.0/count)
    timingsea.loc["geom. mean",("total [ms]","median")] = geomean

    # elimination
    product = 1
    count = 0
    for kernel in timingsne.index.values:
        if splits["equalize_splits"]["sum"][kernel] != 0:
            product = product * timingsne["total [ms]"]["median"][kernel]
            count = count + 1
    geomean = product**(1.0/count)
    timingsne.loc["geom. mean",("total [ms]","median")] = geomean

    # rasterization 
    product = 1
    count = 0
    for kernel in timingsnr.index.values:
        if splits["raster_splits"]["sum"][kernel] != 0:
            product = product * timingsnr["total [ms]"]["median"][kernel]
            count = count + 1

    geomean = product**(1.0/count)
    timingsnr.loc["geom. mean",("total [ms]","median")] = geomean

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(timingsne["total [ms]"]["median"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    fig, (ax0, ax1, ax2) =  matplotlib.pyplot.subplots(3, 1, gridspec_kw = {"height_ratios":[1, 1, 1]}, figsize=(5.9,6.1), dpi=300, edgecolor="w")

    p0 = ax0.bar(indexes, 
            timingsne["total [ms]"]["median"], width, 
            yerr=[timingsne["total [ms]"]["ci_low"], timingsne["total [ms]"]["ci_high"]], color=colors[0])
    p1 = ax1.bar(indexes,  
            timingsnr["total [ms]"]["median"], width, 
            yerr=[timingsnr["total [ms]"]["ci_low"], timingsnr["total [ms]"]["ci_high"]], color=colors[0])
    p2 = ax2.bar(indexes,  
            timingsea["total [ms]"]["median"], width, 
            yerr=[timingsea["total [ms]"]["ci_low"], timingsea["total [ms]"]["ci_high"]], color=colors[0])

    p0[-1].set_color(colors[1])
    p1[-1].set_color(colors[1])
    p2[-1].set_color(colors[1])

    matplotlib.pyplot.xticks(indexes, timingsne.index, rotation=90, horizontalalignment="center", fontsize="medium")

    for kernel in timingsne.index.values:
        if kernel != "geom. mean" and splits["equalize_splits"]["sum"][kernel] == 0:
            index = timingsne["total [ms]"]["median"].keys().get_loc(kernel)
            p0[index].set_color("lightgray")
            #p0[index].set_edgecolor("lightgray")
    for kernel in timingsnr.index.values:
        if kernel != "geom. mean" and splits["raster_splits"]["sum"][kernel] == 0:
            index = timingsnr["total [ms]"]["median"].keys().get_loc(kernel)
            p1[index].set_color("lightgray")
            #p1[index].set_edgecolor("lightgray")
    for kernel in timingsea.index.values:
        if kernel != "geom. mean" and splits["enumeration_splits"]["sum"][kernel] == 0:
            index = timingsea["total [ms]"]["median"].keys().get_loc(kernel)
            p2[index].set_color("lightgray")
            #p1[index].set_edgecolor("lightgray")
    for kernel in timingsnr.index.values:
        if kernel != "geom. mean" and splits["equalize_splits"]["sum"][kernel] == 0 and splits["raster_splits"]["sum"][kernel] == 0 and splits["enumeration_splits"]["sum"][kernel] == 0:
            index = timingsnr["total [ms]"]["median"].keys().get_loc(kernel)
            ticks = ax2.xaxis.get_major_ticks()
            ticks[index].label.set_color("lightgray")

    ax0.annotate("equalization (without rasterization)",  xy=(0.5, 0.89),  xycoords="axes fraction", horizontalalignment="center")
    ax1.annotate("rasterization",  xy=(0.5, 0.89),  xycoords="axes fraction", horizontalalignment="center")
    ax2.annotate("partial enumeration",  xy=(0.5, 0.89),  xycoords="axes fraction", horizontalalignment="center")

    # print baseline bar
    x = [p0[0].get_x()-width, p0[-1].get_x()+2*width]
    ax0.plot(x, [1] * 2, "-", c="lightgray", linewidth=1, zorder=-1)
    ax1.plot(x, [1] * 2, "-", c="lightgray", linewidth=1, zorder=-1)
    ax2.plot(x, [1] * 2, "-", c="lightgray", linewidth=1, zorder=-1)

    # annotate a few values
    val = timingsne["total [ms]"]["median"]["geom. mean"]
    index = timingsne["total [ms]"]["median"].keys().get_loc("geom. mean")
    ax0.annotate(" {:.1f}x".format(val), 
        (p0[index].xy[0]+width/2, p0[index].xy[1] + p0[index].get_height()), xycoords='data',
        verticalalignment="bottom",
        horizontalalignment="center",
        rotation=90)
    
    # annotate a few values
    val = timingsnr["total [ms]"]["median"]["geom. mean"]
    index = timingsnr["total [ms]"]["median"].keys().get_loc("geom. mean")
    ax1.annotate(" {:.1f}x".format(val), 
        (p1[index].xy[0]+width/2, p1[index].xy[1] + p1[index].get_height()), xycoords='data',
        verticalalignment="bottom",
        horizontalalignment="center",
        rotation=90)

    # annotate a few values
    val = timingsea["total [ms]"]["median"]["geom. mean"]
    index = timingsea["total [ms]"]["median"].keys().get_loc("geom. mean")
    ax2.annotate(" {:.1f}x".format(val), 
        (p2[index].xy[0]+width/2, p2[index].xy[1] + p2[index].get_height()), xycoords='data',
        verticalalignment="bottom",
        horizontalalignment="center",
        rotation=90)

    # add the labels
    ax0.set_ylabel("speedup")
    ax0.set_yscale("log")
    ax0.set_ylim([0.8,16])
    ax0.set_xticks([])
    ax0.set_xlim([p0[0].get_x()-width, p0[-1].get_x()+2*width])
    
    ax1.set_ylabel("speedup")
    ax1.set_yscale("log")
    ax1.set_ylim([0.8,16])
    ax1.set_xticks([])
    ax1.set_xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])

    ax2.set_ylabel("speedup")
    ax2.set_yscale("log")
    ax2.set_xlim([p2[0].get_x()-width, p2[-1].get_x()+2*width])
    #ax2.set_ylim([0.8,16])


    #matplotlib.pyplot.legend((p1[0], p2[0]), ("equalization & rasterization", "equalization"), labelspacing=0.1 , frameon=False, fontsize="medium")
    matplotlib.pyplot.savefig(PLOTELIM, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()    

if __name__ == "__main__":
    plot_floorelim_speedup()
