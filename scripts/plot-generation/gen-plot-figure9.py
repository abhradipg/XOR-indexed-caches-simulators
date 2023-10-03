#!/usr/bin/env python3

import speedup
import util

data_path, plots_path, dataset = util.get_data_and_plots_paths()

polycache_csv = "{}/duration/polycache-{}-l1-l2.csv".format(data_path, dataset)
warping_csv = "{}/duration/warping-{}-polycache-l1-l2.csv".format(data_path, dataset)
plot_file = "{}/figure9-{}-polycache-vs-warping-l1-l2-speedup.pdf".format(plots_path, dataset)
width = 6
height = 3

speedup.save_speedup_plot(polycache_csv,
                          warping_csv,
                          plot_file,
                          width,
                          height,
                          limit_top=False,
                          limit_bot=False)
