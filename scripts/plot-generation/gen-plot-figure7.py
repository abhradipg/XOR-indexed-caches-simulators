#!/usr/bin/env python3

import util
import sys
import pathlib
import os

draw_arrows_mark = "DRAW_ARROWS_WILL_BE_INSERTED_HERE"
data_file_name_mark = "DATA_FILE_NAME_WILL_BE_INSERTED_HERE"
smallar_dataset_mark = "SMALLERDATASET"
larger_dataset_mark = "LARGERDATASET"

data_type = sys.argv[1]
data_path, plots_path, dataset1 = util.get_data_and_plots_paths()
dataset2 = "xlarge" if data_type == "existing" else sys.argv[3]

plotgen_dir = pathlib.Path(__file__).parent.resolve()

tex_src = "{}/scatter-plot.tex".format(plotgen_dir)
data_out = "{}/figure7-{}-{}-plot-data.txt".format(plots_path, dataset1, dataset2)
tex_out = "{}/figure7-{}-{}-plot.tex".format(plots_path, dataset1, dataset2)

dataset_symbols = {
    "small": "S",
    "medium": "M",
    "large": "L",
    "xlarge": "XL",
}

dataset_sym1 = dataset_symbols[dataset1]
dataset_sym2 = dataset_symbols[dataset2]

duration_files = [
    "{}/duration/nonwarping-{}-plru-l1.csv".format(data_path, dataset1),
    "{}/duration/nonwarping-{}-plru-l1.csv".format(data_path, dataset2),
    "{}/duration/warping-{}-plru-l1.csv".format(data_path, dataset1),
    "{}/duration/warping-{}-plru-l1.csv".format(data_path, dataset2),
]

nowarp_large = util.read_duration_stats(duration_files[0])
nowarp_xlarge = util.read_duration_stats(duration_files[1])
warp_large = util.read_duration_stats(duration_files[2])
warp_xlarge = util.read_duration_stats(duration_files[3])
stat_list = [nowarp_large, nowarp_xlarge, warp_large, warp_xlarge]

kernels = nowarp_large.get_kernels_ordered_by_name()
for s in stat_list:
    assert(s.get_kernels_ordered_by_name() == kernels)

nowarp_large_t = nowarp_large.get_durations_ms(kernels)
nowarp_xlarge_t = nowarp_xlarge.get_durations_ms(kernels)
warp_large_t = warp_large.get_durations_ms(kernels)
warp_xlarge_t = warp_xlarge.get_durations_ms(kernels)

with open(data_out, "w") as f:
    print("kernel nowarp warp", file=f)
    for k, l1, l2, xl1, xl2 in zip(kernels,
                                nowarp_large_t, warp_large_t,
                                nowarp_xlarge_t, warp_xlarge_t):
        k1 = k + "_" + dataset_sym1
        k2 = k + "_" + dataset_sym2
        print("{} {} {}".format(k1, l1, l2), file=f)
        print("{} {} {}".format(k2, xl1, xl2), file=f)

with open(tex_src, "r") as src:
    with open(tex_out, "w") as dest:
        for line in src:
            if draw_arrows_mark in line:
                for k, l1, l2, xl1, xl2 in zip(kernels,
                                            nowarp_large_t, warp_large_t,
                                            nowarp_xlarge_t, warp_xlarge_t):
                    fmt = r"\draw [->, \arrowthickness] ({}, {}) -- ({}, {});"
                    arrow_line = fmt.format(l1, l2, xl1, xl2)
                    dest.write(arrow_line + "\n")
            else:
                new_line = line
                if data_file_name_mark in new_line:
                    new_line = new_line.replace(data_file_name_mark, os.path.basename(data_out))
                new_line = new_line.replace(smallar_dataset_mark, dataset_sym1)
                new_line = new_line.replace(larger_dataset_mark, dataset_sym2)
                dest.write(new_line)
