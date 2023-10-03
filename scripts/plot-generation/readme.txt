To generate a plot, run its python script with "existing" or "reproduced" cmd arg.
The generated plots will be available under the artifact/plots directory.

Example:
    python3 nonwarping-vs-warping-l1-large-speedup.py existing
    python3 nonwarping-vs-warping-l1-large-speedup.py reproduced

Note: util.py and speedup.py are not runnable.

To generate all plots, run all-plots-from-existing-data.sh or all-plots-from-reproduced-data.sh.

Note: in case you run all-plots-from-reproduced-data.sh without reproducing the data, the plots that require that data won't be generated.
