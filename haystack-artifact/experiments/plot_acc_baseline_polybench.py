""" plot the stuff """

import utils
import pandas
import matplotlib.pyplot
import numpy
import sys

INPUTBASE = "./data/polybench_large/acc_baseline.csv"
INPUTPAPI = "./data/polybench_large/acc_papi_O2.csv"
INPUTL = "./data/polybench_large/perf_baseline.csv"

PLOTABS = "./plots/acc_absolute_polybench_large.pdf"
PLOTREL = "./plots/acc_relative_polybench_large.pdf"

PLOTABS1 = "./plots/acc_absolute1_polybench_large.pdf"
PLOTABS2 = "./plots/acc_absolute2_polybench_large.pdf"

PLOTPER1 = "./plots/acc_percent1_polybench_large.pdf"
PLOTPER2 = "./plots/acc_percent2_polybench_large.pdf"

def fix_overflow(number): 
    if number > sys.maxsize:
        while number > 0:
            number -= sys.maxsize
        number = -number
    return number

def plot_percent_L1():
    # load the data frames
    data = pandas.read_csv(INPUTBASE)
    papi = pandas.read_csv(INPUTPAPI)
    perf = pandas.read_csv(INPUTL)

    # fix the overflow problem    
    papi["PAPI_L1_DCM"] = papi["PAPI_L1_DCM"].apply(fix_overflow)

    # add the compulsory misses
    data["capacity0"] = data["capacity0"] + data["compulsory"]
    
    # compute the number of computed misses
    misses = data.groupby("kernel").aggregate({
        "capacity0" : ["sum"],
        "accesses" : ["sum"]})
    measurements = papi.groupby("kernel").aggregate({
        "PAPI_L1_DCM" : ["median", utils.ci_low, utils.ci_high]})
    timings = perf.groupby("kernel").aggregate({
        "total [ms]" : ["median"]})
    misses = pandas.concat([misses, measurements, timings], axis=1, sort=True)
    #misses = misses.sort_values(("total [ms]","median"))

    # compute the error in percent
    misses.loc[:,("capacity0","miss")] = 100 * misses["capacity0"]["sum"]/misses["accesses"]["sum"]
    misses.loc[:,("capacity0","hit")] = 100 - misses.loc[:,("capacity0","miss")]

    # error in percent
    misses.loc[:,("PAPI_L1_DCM","miss")] = 100 * misses["PAPI_L1_DCM"]["median"]/misses["accesses"]["sum"]
    misses.loc[:,("PAPI_L1_DCM","ci_low")] = 100 * misses["PAPI_L1_DCM"]["ci_low"]/misses["accesses"]["sum"]
    misses.loc[:,("PAPI_L1_DCM","ci_high")] = 100 * misses["PAPI_L1_DCM"]["ci_high"]/misses["accesses"]["sum"]
    
    misses.loc[:,("PAPI_L1_DCM","ci_low")] = misses.loc[:,("capacity0","miss")] - misses["PAPI_L1_DCM"]["ci_low"]
    misses.loc[:,("PAPI_L1_DCM","ci_high")] = misses["PAPI_L1_DCM"]["ci_high"] - misses.loc[:,("capacity0","miss")]

    # compute the relative error
    misses.loc[:,("capacity0","rel")] = 100 * abs(misses["capacity0"]["sum"] - misses["PAPI_L1_DCM"]["median"])/misses["accesses"]["sum"] 

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(misses["capacity0"]["sum"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    fig, (ax0, ax1) =  matplotlib.pyplot.subplots(2, 1, gridspec_kw = {"height_ratios":[1, 3]}, figsize=(5.8,2.4), dpi=300, edgecolor="w")

    p1 = ax1.bar(indexes, misses["capacity0"]["hit"], width, 
        bottom=misses["capacity0"]["miss"],
        #yerr=[misses["PAPI_L1_DCM"]["ci_low"], misses["PAPI_L1_DCM"]["ci_high"]], 
        color=colors[1])
    
    p2 = ax1.bar(indexes, misses["capacity0"]["miss"], width, 
        #yerr=[misses["PAPI_L1_DCM"]["ci_low"], misses["PAPI_L1_DCM"]["ci_high"]], 
        capsize=3,
        color=colors[0])
    p4 = ax1.plot(indexes, misses["PAPI_L1_DCM"]["miss"], marker="_", linestyle="", color="black")

    p3 = ax0.bar(indexes, misses["capacity0"]["rel"], width, color="pink")

    x = [p3[0].get_x()-width, p3[-1].get_x()+2*width]
    ax0.plot(x, [20] * 2, "-", c="lightgray", linewidth=0.8, zorder=-4)
    ax0.plot(x, [10] * 2, "-", c="lightgray", linewidth=0.8, zorder=-4)

    ax0.set_xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])

    # compute the geometic means
    errors = misses["capacity0"]["rel"]
    geomean = errors.prod()**(1.0/len(errors))
    value = "{:.1f}".format(geomean)
    ax1.annotate(r"$\overline{e}^{(g)}=" + value + "\%$",  xy=(0.8, 0.88),  xycoords="figure fraction",)

    index = misses["capacity0"]["rel"].keys().get_loc("doitgen")
    ax0.annotate("due to associativity",
        (p3[index].xy[0]+width, p3[index].xy[1] + p3[index].get_height()*0.8), 
        xytext=(20, -2), textcoords='offset points', 
        arrowprops=dict(arrowstyle="->", connectionstyle="arc3"))

    # add the labels
    ax0.set_ylabel("error [%]")
    ax0.set_xticks([])
    ax0.set_ylim([0,25])
    ax0.set_yticks([0,10,20])
    ax1.set_ylabel("accesses [%]")
    ax1.tick_params(axis="x", rotation=90)
    ax1.set_ylim([0,100])

    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.xticks(indexes, misses.index)
    matplotlib.pyplot.legend((p1[0], p2[0], p4[0]), 
        ("hits", "misses", "measured"), 
        loc="upper right",
        labelspacing=0.01 , 
        frameon=True,
        edgecolor="none",  
        #fancybox=False,
        framealpha=1.0,
        borderpad=0.3,
        borderaxespad=0.3,
        fontsize="medium")
    
    ax0.get_yaxis().set_label_coords(-0.08,0.5)
    ax1.get_yaxis().set_label_coords(-0.08,0.5)

    matplotlib.pyplot.savefig(PLOTPER1, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

def plot_percent_L2():
    # load the data frames
    data = pandas.read_csv(INPUTBASE)
    papi = pandas.read_csv(INPUTPAPI)
    perf = pandas.read_csv(INPUTL)

    # fix the overflow problem    
    papi["PAPI_L2_DCM"] = papi["PAPI_L2_DCM"].apply(fix_overflow)

    # add the compulsory misses
    data["capacity0"] = data["capacity0"] + data["compulsory"]
    data["capacity1"] = data["capacity1"] + data["compulsory"]

    # compute the number of computed misses
    misses = data.groupby("kernel").aggregate({
        "capacity0" : ["sum"],
        "capacity1" : ["sum"],
        "accesses" : ["sum"]})
    measurements = papi.groupby("kernel").aggregate({
        "PAPI_L2_DCM" : ["median", utils.ci_low, utils.ci_high]})
    timings = perf.groupby("kernel").aggregate({
        "total [ms]" : ["median"]})
    misses = pandas.concat([misses, measurements, timings], axis=1, sort=True)
    #misses = misses.sort_values(("total [ms]","median"))

    # compute the error in percent
    misses.loc[:,("capacity1","miss")] = 100 * misses["capacity1"]["sum"]/misses["accesses"]["sum"]
    misses.loc[:,("capacity1","hit")] = 100 - misses.loc[:,("capacity1","miss")]

    # error in percent
    misses.loc[:,("PAPI_L2_DCM","miss")] = 100 * misses["PAPI_L2_DCM"]["median"]/misses["accesses"]["sum"]
    misses.loc[:,("PAPI_L2_DCM","ci_low")] = 100 * misses["PAPI_L2_DCM"]["ci_low"]/misses["accesses"]["sum"]
    misses.loc[:,("PAPI_L2_DCM","ci_high")] = 100 * misses["PAPI_L2_DCM"]["ci_high"]/misses["accesses"]["sum"]
    
    misses.loc[:,("PAPI_L2_DCM","ci_low")] = misses.loc[:,("capacity1","miss")] - misses["PAPI_L2_DCM"]["ci_low"]
    misses.loc[:,("PAPI_L2_DCM","ci_high")] = misses["PAPI_L2_DCM"]["ci_high"] - misses.loc[:,("capacity1","miss")]

    # compute the relative error
    misses.loc[:,("capacity1","rel")] = 100 * abs(misses["capacity1"]["sum"] - misses["PAPI_L2_DCM"]["median"])/misses["accesses"]["sum"] 

    # plot the total times for large and extra large
    colors = ["#1f78b4", "#b2df8a", "#a6cee3"]
    
    indexes = numpy.arange(misses["capacity1"]["sum"].shape[0])  # the x locations for the groups
    width = 0.6  # the width of the bars

    fig, (ax0, ax1) =  matplotlib.pyplot.subplots(2, 1, gridspec_kw = {"height_ratios":[1, 3]}, figsize=(5.8,2.4), dpi=300, edgecolor="w")

    p1 = ax1.bar(indexes, misses["capacity1"]["hit"], width, 
        bottom=misses["capacity1"]["miss"],
        #yerr=[misses["PAPI_L2_DCM"]["ci_low"], misses["PAPI_L2_DCM"]["ci_high"]], 
        color=colors[1])
    
    p2 = ax1.bar(indexes, misses["capacity1"]["miss"], width, 
        #yerr=[misses["PAPI_L2_DCM"]["ci_low"], misses["PAPI_L2_DCM"]["ci_high"]], 
        capsize=3,
        color=colors[0])

    p4 = ax1.plot(indexes, misses["PAPI_L2_DCM"]["miss"], marker="_", linestyle="", color="black")

    p3 = ax0.bar(indexes, misses["capacity1"]["rel"], width, 
        color="pink")

    x = [p3[0].get_x()-width, p3[-1].get_x()+2*width]
    ax0.plot(x, [20] * 2, "-", c="lightgray", linewidth=0.8, zorder=-4)
    ax0.plot(x, [10] * 2, "-", c="lightgray", linewidth=0.8, zorder=-4)

    # compute the geometic means
    errors = misses["capacity1"]["rel"]
    geomean = errors.prod()**(1.0/len(errors))
    value = "{:.1f}".format(geomean)
    ax1.annotate(r"$\overline{e}^{(g)}=" + value + "\%$",  xy=(0.8, 0.88),  xycoords="figure fraction",)

    ax0.set_xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])

    # add the labels
    ax0.set_ylabel("error [%]")
    ax0.set_xticks([])
    ax0.set_ylim([0,25])
    ax0.set_yticks([0,10,20])
    ax1.set_ylabel("accesses [%]")
    ax1.tick_params(axis="x", rotation=90)
    ax1.set_ylim([0,100])

    matplotlib.pyplot.xlim([p1[0].get_x()-width, p1[-1].get_x()+2*width])
    matplotlib.pyplot.xticks(indexes, misses.index)
    matplotlib.pyplot.legend((p1[0], p2[0], p4[0]), 
        ("hits", "misses", "measured"), 
        loc="upper right",
        labelspacing=0.01 , 
        frameon=True,
        edgecolor="none",  
        #fancybox=False,
        framealpha=1.0,
        borderpad=0.3,
        borderaxespad=0.3,
        fontsize="medium")
    
    ax0.get_yaxis().set_label_coords(-0.08,0.5)
    ax1.get_yaxis().set_label_coords(-0.08,0.5)

    matplotlib.pyplot.savefig(PLOTPER2, bbox_inches="tight", pad_inches=0)
    matplotlib.pyplot.close()

if __name__ == "__main__":
    plot_percent_L1()
    plot_percent_L2()

