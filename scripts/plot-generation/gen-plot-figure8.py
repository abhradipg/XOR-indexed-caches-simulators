#!/usr/bin/env python3

import util
import speedup

data_path, plots_path, dataset = util.get_data_and_plots_paths()

haystack_csv = "{}/duration/haystack-{}-l1.csv".format(data_path, dataset)
warping_csv = "{}/duration/warping-{}-full-assoc-lru-l1.csv".format(data_path, dataset)
plot_file = "{}/figure8-{}-haystack-vs-warping-l1-speedup.pdf".format(plots_path, dataset)
width = 6
height = 3.6

speedup.save_speedup_plot(haystack_csv,
                          warping_csv,
                          plot_file,
                          width,
                          height,
                          limit_top=False)
