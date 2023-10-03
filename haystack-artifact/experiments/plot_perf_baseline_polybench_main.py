""" plot the stuff """

import utils
import pandas
import matplotlib.pyplot
import numpy

INPUTM = "./data/polybench_medium/perf_baseline.csv"
INPUTL = "./data/polybench_large/perf_baseline.csv"
INPUTXL = "./data/polybench_xlarge/perf_baseline.csv"

# caches
INPUTOC = "./data/polybench_large/perf_onecache.csv"
INPUTTC = "./data/polybench_large/perf_threecaches.csv"

# accuracy
INPUTACCM = "./data/polybench_medium/acc_baseline.csv"
INPUTACCL = "./data/polybench_large/acc_baseline.csv"
INPUTACCXL = "./data/polybench_xlarge/acc_baseline.csv"

# tiling
INPUTTILED = "./data/polybench_tiled/perf_baseline.csv"

PLOTL = "./plots/perf_baseline_polybench_large.pdf"
PLOTXL = "./plots/perf_baseline_polybench_xlarge.pdf"

PLOTCACHES = "./plots/perf_caches_polybench_large.pdf"
PLOTSIZES = "./plots/perf_sizes_polybench.pdf"
PLOTPERACCESS = "./plots/perf_peraccess_polybench.pdf"
PLOTTILED = "./plots/perf_tiled_polybench.pdf" 

def perf_plot(input1, input2, plot):
    # load the data frames
    data = pandas.read_csv(input1)
    # load the acc info
    acc = pandas.read_csv(input2)

    # compute the stack distance and cache miss times
    data.loc[:, data.columns != "kernel"] = data.drop("kernel", axis=1) / 1000
    data["compute_distance"] = data["compute_between_map [ms]"] + data["count_between_map [ms]"]
    data["compute_misses"] = data["count_capacity_misses [ms]"]

    # compute the total times
    timings = data.groupby("kernel").aggregate({
        "total [ms]" : ["median", utils.ci_low, utils.ci_high],
        "compute_distance" : ["median", utils.ci_low, utils.ci_high],
        "compute_misses" : ["median", utils.ci_low, utils.ci_high]})
    splits = acc.groupby("kernel").aggregate({
        "accesses" : ["sum"],
        "barvinok_splits" : ["sum"],
        "equalize_splits" : ["sum"],
        "raster_splits" : ["sum"],
        "enumeration_splits" : ["sum"]})
    timings = pandas.concat([timings, splits], axis=1)
    # sum up all the splits
    timings.loc[:,("enumeration_splits", "sum")] = timings["enumeration_splits"]["sum"] + timings["raster_splits"]["sum"] + timings["equalize_splits"]["sum"] + timings["barvinok_splits"]["sum"] 
    timings.loc[:,("total [ms]","ci_low")] = -(timings["total [ms]"]["ci_low"] - timings["total [ms]"]["median"])
    timings.loc[:,("total [ms]","ci_high")] = timings["total [ms]"]["ci_high"] - timings["total [ms]"]["median"]
    timings.loc[:,("compute_distance","ci_low")] = -(timings["compute_distance"]["ci_low"] - timings["compute_distance"]["median"])
    timings.loc[:,("compute_distance","ci_high")] = timings["compute_distance"]["ci_high"] - timings["compute_distance"]["median"]
    timings.loc[:,("compute_misses","ci_low")] = -(timings["compute_misses"]["ci_low"] - timings["compute_misses"]["median"])
    timings.loc[:,("compute_misses","ci_high")] = timings["compute_misses"]["ci_high"] - timings["compute_misses"]["median"]
    timings = timings.sort_values(("total [ms]","median"))
    
    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(timings["total [ms]"]["median"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    #matplotlib.pyplot.figure(num=None, figsize=(6,3), dpi=300, edgecolor="w")
    fig, ax1 =  matplotlib.pyplot.subplots(figsize=(6,2.4), dpi=300, edgecolor="w")
    ax2 = ax1.twinx()

    p3 = ax1.bar(indexes, 
            timings["total [ms]"]["median"], width, 
            yerr=[timings["total [ms]"]["ci_low"], timings["total [ms]"]["ci_high"]], color=colors[2])
    p2 = ax1.bar(indexes, 
            timings["compute_misses"]["median"], width, 
            bottom=timings["compute_distance"]["median"], 
            yerr=[timings["compute_distance"]["ci_low"], timings["compute_distance"]["ci_high"]], color=colors[0])
    p1 = ax1.bar(indexes, 
            timings["compute_distance"]["median"], width, 
            yerr=[timings["compute_misses"]["ci_low"], timings["compute_misses"]["ci_high"]], color=colors[1])
    
    p4 = ax2.plot(indexes, 
            timings["enumeration_splits"]["sum"], width, color="black", linestyle="-", linewidth=1)

    # annotate a few values
    val = timings["total [ms]"]["median"]["correlation"]
    index = timings["total [ms]"]["median"].keys().get_loc("correlation")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-20.0, 40), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    val = timings["total [ms]"]["median"]["cholesky"]
    index = timings["total [ms]"]["median"].keys().get_loc("cholesky")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-30.0, 20), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    val = timings["total [ms]"]["median"]["jacobi-1d"]
    index = timings["total [ms]"]["median"].keys().get_loc("jacobi-1d")
    ax1.annotate("{:.1f}s".format(val), (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
            xytext=(10.0, 20), textcoords='offset points', arrowprops=dict(arrowstyle="->",
                            connectionstyle="arc3"))
    
    val = timings["total [ms]"]["median"]["syr2k"]
    index = timings["total [ms]"]["median"].keys().get_loc("syr2k")
    ax1.annotate("{:.1f}s".format(val), (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
            xytext=(10.0, 20), textcoords='offset points', arrowprops=dict(arrowstyle="->",
                            connectionstyle="arc3"))

    val = timings["total [ms]"]["median"]["nussinov"]
    index = timings["total [ms]"]["median"].keys().get_loc("nussinov")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0], p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-60.0, 0), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # add the labels
    ax1.set_ylabel("execution time [s]")
    ax2.set_ylabel("#subdomains")
    ax2.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    ax2.set_yticks([0e5, 0.5e5, 1e5])
    ax1.tick_params(axis="x", rotation=90)

    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.xticks(indexes, timings.index)
    matplotlib.pyplot.legend((p3[0], p2[0], p1[0], p4[0]), 
        ("total", "capacity misses & stack distances", "stack distances", "#subdomains"), 
        labelspacing=0.1 , frameon=False, fontsize="medium")
    matplotlib.pyplot.savefig(plot, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

def plot_caches():
    # load the data frames
    data1c = pandas.read_csv(INPUTOC)
    data2c = pandas.read_csv(INPUTL)
    data3c = pandas.read_csv(INPUTTC)
    
    # compute the stack distance and cache miss times
    data1c.loc[:, data1c.columns != "kernel"] = data1c.drop("kernel", axis=1) / 1000
    data2c.loc[:, data2c.columns != "kernel"] = data2c.drop("kernel", axis=1) / 1000
    data3c.loc[:, data3c.columns != "kernel"] = data3c.drop("kernel", axis=1) / 1000
    
    data1c = data1c.loc[:, ("kernel", "total [ms]")]
    data2c = data2c.loc[:, ("kernel", "total [ms]")]
    data3c = data3c.loc[:, ("kernel", "total [ms]")]
    data1c = data1c.rename(columns={"total [ms]" : "1cache"})
    data2c = data2c.rename(columns={"total [ms]" : "2cache", "kernel" : "kernel2"})
    data3c = data3c.rename(columns={"total [ms]" : "3cache", "kernel" : "kernel3"})
    data = pandas.concat([data1c, data2c, data3c], axis=1)
    
    # compute the total times
    timings = data.groupby("kernel").aggregate({
        "1cache" : ["median", utils.ci_low, utils.ci_high],
        "2cache" : ["median", utils.ci_low, utils.ci_high],
        "3cache" : ["median", utils.ci_low, utils.ci_high]})

    timings.loc[:,("1cache","ci_low")] = -(timings["1cache"]["ci_low"] - timings["1cache"]["median"])
    timings.loc[:,("1cache","ci_high")] = timings["1cache"]["ci_high"] - timings["1cache"]["median"]
    timings.loc[:,("2cache","ci_low")] = -(timings["2cache"]["ci_low"] - timings["2cache"]["median"])
    timings.loc[:,("2cache","ci_high")] = timings["2cache"]["ci_high"] - timings["2cache"]["median"]
    timings.loc[:,("3cache","ci_low")] = -(timings["3cache"]["ci_low"] - timings["3cache"]["median"])
    timings.loc[:,("3cache","ci_high")] = timings["3cache"]["ci_high"] - timings["3cache"]["median"]

    timings = timings.sort_values(("2cache","median"))

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(timings["1cache"]["median"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    matplotlib.pyplot.figure(num=None, figsize=(6,2.2), dpi=300, edgecolor="w")
    p3 = matplotlib.pyplot.bar(indexes, 
            timings["3cache"]["median"], width, 
            yerr=[timings["3cache"]["ci_low"], timings["3cache"]["ci_high"]], color=colors[2])
    p2 = matplotlib.pyplot.bar(indexes, 
            timings["2cache"]["median"], width, 
            yerr=[timings["2cache"]["ci_low"], timings["2cache"]["ci_high"]], color=colors[0])
    p1 = matplotlib.pyplot.bar(indexes, 
            timings["1cache"]["median"], width, 
            yerr=[timings["1cache"]["ci_low"], timings["1cache"]["ci_high"]], color=colors[1])

    # annotate a few values
    val1 = timings["1cache"]["median"]["correlation"]
    val2 = timings["2cache"]["median"]["correlation"]
    val3 = timings["3cache"]["median"]["correlation"]
    index = timings["3cache"]["median"].keys().get_loc("correlation")
    matplotlib.pyplot.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val1, val2, val3), 
        (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-40.0, 40), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    val1 = timings["1cache"]["median"]["syr2k"]
    val2 = timings["2cache"]["median"]["syr2k"]
    val3 = timings["3cache"]["median"]["syr2k"]
    index = timings["3cache"]["median"].keys().get_loc("syr2k")
    matplotlib.pyplot.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val1, val2, val3), 
        (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-20.0, 40), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    val1 = timings["1cache"]["median"]["jacobi-1d"]
    val2 = timings["2cache"]["median"]["jacobi-1d"]
    val3 = timings["3cache"]["median"]["jacobi-1d"]
    index = timings["3cache"]["median"].keys().get_loc("jacobi-1d")
    matplotlib.pyplot.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val1, val2, val3), 
        (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(0.0, 40), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    val1 = timings["1cache"]["median"]["cholesky"]
    val2 = timings["2cache"]["median"]["cholesky"]
    val3 = timings["3cache"]["median"]["cholesky"]
    index = timings["3cache"]["median"].keys().get_loc("cholesky")
    matplotlib.pyplot.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val1, val2, val3), 
        (p3[index].xy[0], p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-100.0, 3), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))
    
    val1 = timings["1cache"]["median"]["nussinov"]
    val2 = timings["2cache"]["median"]["nussinov"]
    val3 = timings["3cache"]["median"]["nussinov"]
    index = timings["3cache"]["median"].keys().get_loc("nussinov")
    matplotlib.pyplot.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val1, val2, val3), 
        (p3[index].xy[0], p3[index].xy[1] + p3[index].get_height()*0.95), xycoords='data',
        xytext=(-110.0, -2), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # add the labels
    matplotlib.pyplot.ylabel("execution time [s]")
    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.xticks(indexes, timings.index, rotation=90, horizontalalignment="center", fontsize="medium")
    matplotlib.pyplot.legend((p3[0], p2[0], p1[0]), 
        ("total L1 & L2 & L3", "total L1 & L2", "total L1"), 
        labelspacing=0.1 , frameon=False, fontsize="medium")
    matplotlib.pyplot.savefig(PLOTCACHES, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

def plot_sizes():
    # load the data frames
    datam = pandas.read_csv(INPUTM)
    datal = pandas.read_csv(INPUTL)
    dataxl = pandas.read_csv(INPUTXL)
    
    # compute the stack distance and cache miss times
    datam.loc[:, datam.columns != "kernel"] = datam.drop("kernel", axis=1) / 1000
    datal.loc[:, datal.columns != "kernel"] = datal.drop("kernel", axis=1) / 1000
    dataxl.loc[:, dataxl.columns != "kernel"] = dataxl.drop("kernel", axis=1) / 1000
    
    datam = datam.loc[:, ("kernel", "total [ms]")]
    datal = datal.loc[:, ("kernel", "total [ms]")]
    dataxl = dataxl.loc[:, ("kernel", "total [ms]")]
    datam = datam.rename(columns={"total [ms]" : "medium"})
    datal = datal.rename(columns={"total [ms]" : "large", "kernel" : "kernel2"})
    dataxl = dataxl.rename(columns={"total [ms]" : "xlarge", "kernel" : "kernel3"})
    data = pandas.concat([datam, datal, dataxl], axis=1)
    
    # compute the total times
    timings = data.groupby("kernel").aggregate({
        "medium" : ["median", utils.ci_low, utils.ci_high],
        "large" : ["median", utils.ci_low, utils.ci_high],
        "xlarge" : ["median", utils.ci_low, utils.ci_high]})

    timings.loc[:,("medium","ci_low")] = -(timings["medium"]["ci_low"] - timings["medium"]["median"])
    timings.loc[:,("medium","ci_high")] = timings["medium"]["ci_high"] - timings["medium"]["median"]

    timings.loc[:,("large","ci_low")] = -(timings["large"]["ci_low"] - timings["large"]["median"])
    timings.loc[:,("large","ci_high")] = timings["large"]["ci_high"] - timings["large"]["median"]

    timings.loc[:,("xlarge","ci_low")] = -(timings["xlarge"]["ci_low"] - timings["xlarge"]["median"])
    timings.loc[:,("xlarge","ci_high")] = timings["xlarge"]["ci_high"] - timings["xlarge"]["median"]

    # load the accuracy data
    accm = pandas.read_csv(INPUTACCM)
    accl = pandas.read_csv(INPUTACCL)
    accxl = pandas.read_csv(INPUTACCXL)

    splits = accxl.groupby("kernel").aggregate({
        "barvinok_splits" : ["sum"],
        "equalize_splits" : ["sum"],
        "raster_splits" : ["sum"],
        "enumeration_splits" : ["sum"]})
    splits.loc[:,("xl", "sum")] = splits["enumeration_splits"]["sum"] + splits["raster_splits"]["sum"] + splits["equalize_splits"]["sum"] + splits["barvinok_splits"]["sum"] 
    timings = pandas.concat([timings, splits.loc[:,("xl", "sum")]], axis=1)
    
    splits = accl.groupby("kernel").aggregate({
        "barvinok_splits" : ["sum"],
        "equalize_splits" : ["sum"],
        "raster_splits" : ["sum"],
        "enumeration_splits" : ["sum"]})
    splits.loc[:,("l", "sum")] = splits["enumeration_splits"]["sum"] + splits["raster_splits"]["sum"] + splits["equalize_splits"]["sum"] + splits["barvinok_splits"]["sum"] 
    timings = pandas.concat([timings, splits.loc[:,("l", "sum")]], axis=1)
    
    splits = accm.groupby("kernel").aggregate({
        "barvinok_splits" : ["sum"],
        "equalize_splits" : ["sum"],
        "raster_splits" : ["sum"],
        "enumeration_splits" : ["sum"]})
    splits.loc[:,("m", "sum")] = splits["enumeration_splits"]["sum"] + splits["raster_splits"]["sum"] + splits["equalize_splits"]["sum"] + splits["barvinok_splits"]["sum"] 
    timings = pandas.concat([timings, splits.loc[:,("m", "sum")]], axis=1)

    timings = timings.sort_values(("large","median"))

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(timings["medium"]["median"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    fig, ax1 =  matplotlib.pyplot.subplots(figsize=(6,2.4), dpi=300, edgecolor="w")
    ax2 = ax1.twinx()
    #matplotlib.pyplot.figure(num=None, figsize=(6,3), dpi=300, edgecolor="w")
    
    p3 = ax1.bar(indexes, 
            timings["xlarge"]["median"], width, 
            yerr=[timings["xlarge"]["ci_low"], timings["xlarge"]["ci_high"]], color=colors[2])
    p2 = ax1.bar(indexes, 
            timings["large"]["median"], width, 
            yerr=[timings["large"]["ci_low"], timings["large"]["ci_high"]], color=colors[0])
    p1 = ax1.bar(indexes, 
            timings["medium"]["median"], width, 
            yerr=[timings["medium"]["ci_low"], timings["medium"]["ci_high"]], color=colors[1])
    
    p4 = ax2.plot(indexes, 
            timings["xl"]["sum"], width, color="black", linestyle="-", linewidth=1)
    p5 = ax2.plot(indexes, 
            timings["l"]["sum"], width,  color="black", linestyle="--", linewidth=1)
    p6 = ax2.plot(indexes, 
            timings["m"]["sum"], width,  color="black", linestyle=":", linewidth=1)

    # annotate a few values
    val1 = timings["xlarge"]["median"]["correlation"]
    val2 = timings["large"]["median"]["correlation"]
    val3 = timings["medium"]["median"]["correlation"]
    index = timings["xlarge"]["median"].keys().get_loc("correlation")
    ax1.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val3, val2, val1),  
        (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-60.0, 30), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    val1 = timings["xlarge"]["median"]["jacobi-1d"]
    val2 = timings["large"]["median"]["jacobi-1d"]
    val3 = timings["medium"]["median"]["jacobi-1d"]
    index = timings["xlarge"]["median"].keys().get_loc("jacobi-1d")
    ax1.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val3, val2, val1), 
            (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
            xytext=(-5.0, 30), textcoords='offset points', arrowprops=dict(arrowstyle="->",
                            connectionstyle="arc3"))
    
    val1 = timings["xlarge"]["median"]["syr2k"]
    val2 = timings["large"]["median"]["syr2k"]
    val3 = timings["medium"]["median"]["syr2k"]
    index = timings["xlarge"]["median"].keys().get_loc("syr2k")
    ax1.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val3, val2, val1), 
            (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
            xytext=(-20.0, 30), textcoords='offset points', arrowprops=dict(arrowstyle="->",
                            connectionstyle="arc3"))

    val1 = timings["xlarge"]["median"]["cholesky"]
    val2 = timings["large"]["median"]["cholesky"]
    val3 = timings["medium"]["median"]["cholesky"]    
    index = timings["xlarge"]["median"].keys().get_loc("cholesky")
    ax1.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val3, val2, val1), 
            (p3[index].xy[0] + width/2, p3[index].xy[1] + p3[index].get_height()*0.7), xycoords='data',
            xytext=(-110.0, 0), textcoords='offset points', arrowprops=dict(arrowstyle="->",
                            connectionstyle="arc3"))

    val1 = timings["xlarge"]["median"]["nussinov"]
    val2 = timings["large"]["median"]["nussinov"]
    val3 = timings["medium"]["median"]["nussinov"]
    index = timings["xlarge"]["median"].keys().get_loc("nussinov")
    ax1.annotate("{:.1f}|{:.1f}|{:.1f}s".format(val3, val2, val1),  
        (p3[index].xy[0], p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-120.0, 0.0), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # add the labels
    ax1.set_ylabel("execution time [s]")
    ax2.set_ylabel("#subdomains")
    ax2.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    ax1.tick_params(axis="x", rotation=90)

    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.xticks(indexes, timings.index)
    matplotlib.pyplot.legend((p3[0], p2[0], p1[0], p4[0], p5[0], p6[0]), 
        ("total XL", "total L", "total M", 
        "#subdomains XL", "#subdomains L", "#subdomains M"), 
        labelspacing=0.1 , frameon=False, fontsize="medium")
    matplotlib.pyplot.savefig(PLOTSIZES, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

def plot_tiled():
    # load the data frames
    databs = pandas.read_csv(INPUTL)
    data = pandas.read_csv(INPUTTILED)

    # compute the stack distance and cache miss times
    data.loc[:, data.columns != "kernel"] = data.drop("kernel", axis=1) / 1000
    data["compute_distance"] = data["compute_between_map [ms]"] + data["count_between_map [ms]"]
    data["compute_misses"] = data["count_capacity_misses [ms]"]

    # rename the total time
    databs = databs.rename(columns={"total [ms]" : "order"})

    # compute the total times
    timings = data.groupby("kernel").aggregate({
        "total [ms]" : ["median", utils.ci_low, utils.ci_high],
        "compute_distance" : ["median", utils.ci_low, utils.ci_high],
        "compute_misses" : ["median", utils.ci_low, utils.ci_high]})
    
    # sum up all the splits
    timings.loc[:,("total [ms]","ci_low")] = -(timings["total [ms]"]["ci_low"] - timings["total [ms]"]["median"])
    timings.loc[:,("total [ms]","ci_high")] = timings["total [ms]"]["ci_high"] - timings["total [ms]"]["median"]
    timings.loc[:,("compute_distance","ci_low")] = -(timings["compute_distance"]["ci_low"] - timings["compute_distance"]["median"])
    timings.loc[:,("compute_distance","ci_high")] = timings["compute_distance"]["ci_high"] - timings["compute_distance"]["median"]
    timings.loc[:,("compute_misses","ci_low")] = -(timings["compute_misses"]["ci_low"] - timings["compute_misses"]["median"])
    timings.loc[:,("compute_misses","ci_high")] = timings["compute_misses"]["ci_high"] - timings["compute_misses"]["median"]
    
    timingsbs = databs.groupby("kernel").aggregate({
        "order" : ["median"]})
    timings = pandas.concat([timings, timingsbs], axis=1, sort=True)
    timings = timings.sort_values(("total [ms]","median"))

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(timings["total [ms]"]["median"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    #matplotlib.pyplot.figure(num=None, figsize=(6,3), dpi=300, edgecolor="w")
    fig, ax1 =  matplotlib.pyplot.subplots(figsize=(6,2.2), dpi=300, edgecolor="w")

    p3 = ax1.bar(indexes, 
            timings["total [ms]"]["median"], width, 
            yerr=[timings["total [ms]"]["ci_low"], timings["total [ms]"]["ci_high"]], color=colors[2])
    p2 = ax1.bar(indexes, 
            timings["compute_misses"]["median"], width, 
            bottom=timings["compute_distance"]["median"], 
            yerr=[timings["compute_distance"]["ci_low"], timings["compute_distance"]["ci_high"]], color=colors[0])
    p1 = ax1.bar(indexes, 
            timings["compute_distance"]["median"], width, 
            yerr=[timings["compute_misses"]["ci_low"], timings["compute_misses"]["ci_high"]], color=colors[1])
    
    # annotate the timeout
    for kernel in timings.index.values:
        if pandas.isnull(timings["total [ms]"]["median"][kernel]):
            index = timings["total [ms]"]["median"].keys().get_loc(kernel)
            ax1.annotate(" >1h", (indexes[index], 0), 
                verticalalignment="bottom",
                horizontalalignment="center",
                rotation=90)

    # mark untiled kernels
    matplotlib.pyplot.xticks(indexes, timings.index)
    nottiled = ["nussinov", "durbin", "jacobi-1d", "seidel-2d"]
    for kernel in timings.index.values:
        if kernel in nottiled:
            index = timings["total [ms]"]["median"].keys().get_loc(kernel)
            ticks = ax1.xaxis.get_major_ticks()
            ticks[index].label.set_color("lightgray")
            
    # p4 = ax2.plot(indexes, 
    #         timings["enumeration_splits"]["sum"], width, color="black", linestyle="-", linewidth=1)

    # annotate a few values
    val = timings["total [ms]"]["median"]["mvt"]
    index = timings["total [ms]"]["median"].keys().get_loc("mvt")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0]+width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-11.0, 20), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # annotate a few values
    val = timings["total [ms]"]["median"]["correlation"]
    index = timings["total [ms]"]["median"].keys().get_loc("correlation")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0]+width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-13.0, 20), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # annotate a few values
    val = timings["total [ms]"]["median"]["jacobi-2d"]
    index = timings["total [ms]"]["median"].keys().get_loc("jacobi-2d")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0]+width/2, p3[index].xy[1] + p3[index].get_height()), xycoords='data',
        xytext=(-13.0, 20), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # annotate a few values
    val = timings["total [ms]"]["median"]["lu"]
    index = timings["total [ms]"]["median"].keys().get_loc("lu")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0], p3[index].xy[1] + p3[index].get_height()*0.8), xycoords='data',
        xytext=(-60.0, -2), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))


    # annotate a few values
    val = timings["total [ms]"]["median"]["adi"]
    index = timings["total [ms]"]["median"].keys().get_loc("adi")
    ax1.annotate("{:.1f}s".format(val), 
        (p3[index].xy[0], p3[index].xy[1] + p3[index].get_height()*0.9), xycoords='data',
        xytext=(-60.0, -2), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # add the labels
    ax1.set_ylabel("execution time [s]")
    ax1.tick_params(axis="x", rotation=90)

    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.legend((p3[0], p2[0], p1[0]), 
        ("total", "capacity misses & stack distances", "stack distances"), 
        labelspacing=0.1 , frameon=False, fontsize="medium")
    matplotlib.pyplot.savefig(PLOTTILED, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

if __name__ == "__main__":
    perf_plot(INPUTL, INPUTACCL, PLOTL)
    plot_caches()
    plot_sizes()
    #plot_tiled()



    
