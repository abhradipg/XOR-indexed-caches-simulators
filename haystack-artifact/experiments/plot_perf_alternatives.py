""" plot the stuff """

import utils
import pandas
import matplotlib.pyplot
import numpy

INPUT = "./data/polybench_3.2_standard/perf_baseline.csv"
INPUTL = "./data/polybench_large/perf_baseline.csv"
INPUTDINERO = "./data/polybench_large/perf_din_large.csv"

PLOT = "./plots/perf_polycache_polybench_32.pdf"
PLOTDINERO = "./plots/perf_dinero_polybench_large.pdf"

def perf_speedup():
    # load the data frames
    data = pandas.read_csv(INPUT)

    data["kernel"].replace({"jacobi-1d-imper" : "jacobi-1d"}, inplace=True)  
    data["kernel"].replace({"jacobi-2d-imper" : "jacobi-2d"}, inplace=True)  

    # compute the stack distance and cache miss times
    data.loc[:, data.columns != "kernel"] = data.drop("kernel", axis=1) / 1000
    data = data[data.kernel != "ludcmp"]

    # compute the total times
    timings = data.groupby("kernel").aggregate({
        "total [ms]" : ["median", utils.ci_low, utils.ci_high]})
    timings.insert(0, "PolyCache", 0)

    # values from the polycache paper
    timings.loc["2mm","PolyCache"] = 16
    timings.loc["3mm","PolyCache"] = 27
    timings.loc["adi","PolyCache"] = 78
    timings.loc["atax","PolyCache"] = 4
    timings.loc["bicg","PolyCache"] = 3
    timings.loc["cholesky","PolyCache"] = 52
    timings.loc["correlation","PolyCache"] = 13
    timings.loc["covariance","PolyCache"] = 38
    timings.loc["doitgen","PolyCache"] = 4
    timings.loc["durbin","PolyCache"] = 128
    timings.loc["dynprog","PolyCache"] = 9
    timings.loc["fdtd-2d","PolyCache"] = 25
    timings.loc["fdtd-apml","PolyCache"] = 2859
    timings.loc["floyd-warshall","PolyCache"] = 64
    timings.loc["gemm","PolyCache"] = 2
    timings.loc["gemver","PolyCache"] = 38
    timings.loc["gesummv","PolyCache"] = 7
    timings.loc["gramschmidt","PolyCache"] = 29
    timings.loc["jacobi-1d","PolyCache"] = 1
    timings.loc["jacobi-2d","PolyCache"] = 4
    timings.loc["lu","PolyCache"] = 5
    timings.loc["mvt","PolyCache"] = 6
    timings.loc["reg_detect","PolyCache"] = 2
    timings.loc["seidel-2d","PolyCache"] = 1
    timings.loc["symm","PolyCache"] = 323
    timings.loc["syr2k","PolyCache"] = 151
    timings.loc["syrk","PolyCache"] = 39
    timings.loc["trisolv","PolyCache"] = 3
    timings.loc["trmm","PolyCache"] = 36

    timings.loc[:,"speedup"] = timings["PolyCache"] / timings["total [ms]"]["median"]
    timings.loc[:,"ci_low"] = timings["PolyCache"] / timings["total [ms]"]["ci_low"]
    timings.loc[:,"ci_high"] = timings["PolyCache"] / timings["total [ms]"]["ci_high"]
    timings.loc[:,"ci_low"] = -(timings["ci_low"] - timings["speedup"])
    timings.loc[:,"ci_high"] = timings["ci_high"] - timings["speedup"]
    #timings = timings.sort_values(("total [ms]","median"))

    # compute the geometric mean
    geomean = timings["speedup"].prod()**(1.0/len(timings["speedup"]))
    timings.loc["  geom. mean","speedup"] = geomean

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(timings["total [ms]"]["median"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    #matplotlib.pyplot.figure(num=None, figsize=(6,3), dpi=300, edgecolor="w")
    fig, ax1 =  matplotlib.pyplot.subplots(figsize=(6,1.8), dpi=300, edgecolor="w")
    #ax2 = ax1.twinx()

    p1 = ax1.bar(indexes, 
        timings["speedup"], width, 
        yerr=[timings["ci_low"], timings["ci_high"]],
        color=colors[0])
    p1[-1].set_color(colors[1])

    # print baseline bar
    x = [p1[0].get_x()-width, p1[-1].get_x()+2*width]
    matplotlib.pyplot.plot(x, [1] * 2, "-", c="lightgray", linewidth=1, zorder=-1)

    # annotate a few values
    val = timings["speedup"]["  geom. mean"]
    index = timings["speedup"].keys().get_loc("  geom. mean")
    ax1.annotate(" {:.0f}x".format(val), 
        (p1[index].xy[0]+width/2, p1[index].xy[1] + p1[index].get_height()), xycoords='data',
        verticalalignment="bottom",
        horizontalalignment="center",
        rotation=90)
        
    ax1.annotate("1 core vs 1024 cores (PolyCache)",  xy=(0.47, 0.89),  xycoords="axes fraction",)

    # add the labels
    ax1.set_ylabel("speedup")
    ax1.set_yscale("log")

    ax1.tick_params(axis="x", rotation=90)

    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.xticks(indexes, timings.index)
    matplotlib.pyplot.savefig(PLOT, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

def perf_dinero():
    # load the data frames
    databs = pandas.read_csv(INPUTL)
    data = pandas.read_csv(INPUTDINERO)

    # compute the stack distance and cache miss times
    databs.loc[:, databs.columns != "kernel"] = databs.drop("kernel", axis=1) / 1000

    # compute the total times
    timingsbs = databs.groupby("kernel").aggregate({
        "total [ms]" : ["median", utils.ci_low, utils.ci_high]})
    timings = data.groupby("kernel").aggregate({
        "duration" : ["median", utils.ci_low, utils.ci_high]})

    timings = pandas.concat([timingsbs, timings], axis=1, sort=True)

    # TODO compute speedup before if we have multiple measurements
    timings.loc[:,"speedup"] = timings["duration"]["median"] / timings["total [ms]"]["median"]
    timings.loc[:,"ci_low"] = timings["duration"]["median"] / timings["total [ms]"]["ci_low"]
    timings.loc[:,"ci_high"] = timings["duration"]["median"] / timings["total [ms]"]["ci_high"]
    
    timings.loc[:,"ci_low"] = -(timings["ci_low"] - timings["speedup"])
    timings.loc[:,"ci_high"] = timings["ci_high"] - timings["speedup"]
    
    # compute the geometric mean
    geomean = timings["speedup"].prod()**(1.0/len(timings["speedup"]))
    timings.loc["  geom. mean","speedup"] = geomean

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(timings["total [ms]"]["median"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    #matplotlib.pyplot.figure(num=None, figsize=(6,3), dpi=300, edgecolor="w")
    fig, ax1 =  matplotlib.pyplot.subplots(figsize=(6,1.8), dpi=300, edgecolor="w")

    p1 = ax1.bar(indexes, 
        timings["speedup"], width, 
        yerr=[timings["ci_low"], timings["ci_high"]],
        color=colors[0])
    p1[-1].set_color(colors[1])

    # print baseline bar
    x = [p1[0].get_x()-width, p1[-1].get_x()+2*width]
    matplotlib.pyplot.plot(x, [1] * 2, "-", c="lightgray", linewidth=1, zorder=-1)

    # annotate a few values
    val = timings["speedup"]["  geom. mean"]
    index = timings["speedup"].keys().get_loc("  geom. mean")
    ax1.annotate(" {:.0f}x".format(val), 
        (p1[index].xy[0]+width/2, p1[index].xy[1] + p1[index].get_height()), xycoords='data',
        verticalalignment="bottom",
        horizontalalignment="center",
        rotation=90)

    # add the labels
    ax1.set_ylabel("speedup")
    ax1.set_yscale("log")
    ax1.tick_params(axis="x", rotation=90)

    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.xticks(indexes, timings.index)
    matplotlib.pyplot.savefig(PLOTDINERO, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

if __name__ == "__main__":
    perf_speedup()
    perf_dinero()


    
