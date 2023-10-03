""" run polybench on the large data set """

import os
import csv
import utils

TIMEOUT = 10800
REPETITIONS = int(os.environ["REPETITIONS"])
POLYBENCH = "./benchmarks/polybench_tiled/"
INCLUDE = "./benchmarks/polybench_tiled/utilities"
EXECUTABLE = "./executables/baseline"
OUTPUT = "./data/polybench_tiled/perf_baseline.csv"
ERROR = "./data/polybench_tiled/err_baseline.csv"

if __name__ == "__main__":
    # get the polybench kernels
    kernels = utils.get_polybench_kernels(POLYBENCH)
    # run the kernels
    rows = []
    errors = []
    for (i, source) in enumerate(kernels):
        name = os.path.splitext(os.path.basename(os.path.normpath(source)))[0]
        print("-> analyzing " + name)
        for repetition in range(REPETITIONS):
            output = utils.run_cache_model(EXECUTABLE, source, INCLUDE, TIMEOUT)
            # parse the output
            if output:
                data = utils.parse_performance(output)
                data.insert(0, ("kernel", name))
                rows.append(data)
                errors.append([name, output])
        # write after every measurement
        if rows and len(rows) > 0:
            with open(OUTPUT, "w") as f:
                w = csv.writer(f)
                w.writerow([datum[0] for datum in rows[0]])
                for row in rows:
                    w.writerow([datum[1] for datum in row])
                f.close()
        if errors and len(errors) > 0:
            with open(ERROR, "w") as f:
                w = csv.writer(f)
                for error in errors:
                    w.writerow(error)
                f.close()

