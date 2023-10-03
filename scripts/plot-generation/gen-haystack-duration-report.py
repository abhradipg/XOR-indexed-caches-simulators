#!/usr/bin/env python3
import csv
import util
import sys

data_type = sys.argv[1]
assert data_type == "reproduced"

data_path, plots_path, dataset = util.get_data_and_plots_paths()

INPUT = "{}/haystack/haystack-{}-l1-duration.csv".format(data_path, dataset)
OUTPUT = "{}/duration/haystack-{}-l1.csv".format(data_path, dataset)

kernel_duration_ms_pairs = []

with open(INPUT, newline="") as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",", quotechar="|")
    for row in spamreader:
        kernel = row[0].lower()
        if kernel == "kernel":
            kernel_duration_ms_pairs.append(["KERNEL", "DURATION_MS"])
        else:
            kernel_duration_ms_pairs.append([kernel, row[1]])

with open(OUTPUT, "w", newline="") as csv_file:
    writer = csv.writer(csv_file, delimiter=" ", quotechar="|", quoting=csv.QUOTE_MINIMAL)
    for pair in kernel_duration_ms_pairs:
        writer.writerow(pair)
