#!/usr/bin/env python3
import csv
import util
import sys

data_type = sys.argv[1]
assert data_type == "reproduced"

data_path, plots_path, dataset = util.get_data_and_plots_paths()

INPUT = "{}/haystack/haystack-{}-l1-l2-miss.csv".format(data_path, dataset)
OUTPUT = "{}/miss/haystack-{}-l1-l2.csv".format(data_path, dataset)

kernel_results = {}
with open(INPUT, newline="") as csvfile:
    spamreader = csv.reader(csvfile, delimiter=",", quotechar="|")
    for row in spamreader:
        kernel_name = row[0].lower()
        if kernel_name == "kernel":
            continue
        if kernel_name not in kernel_results:
            kernel_results[kernel_name] = {"access": 0,
                                           "l1_miss": 0,
                                           "l2_miss": 0}
        compulsory, capacity0, capacity1, accesses = [int(x) for x in row[2:6]]
        kernel_results[kernel_name]["access"] += accesses
        kernel_results[kernel_name]["l1_miss"] += compulsory + capacity0
        kernel_results[kernel_name]["l2_miss"] += compulsory + capacity1

kernel_results = util.sort_dict(kernel_results)

def create_csv_row(kernel, kernel_result):
    return [kernel,
            kernel_result["access"],
            kernel_result["l1_miss"],
            kernel_result["l2_miss"]]

with open(OUTPUT, "w", newline="") as csv_file:
    writer = csv.writer(csv_file, delimiter=" ", quotechar="|",
                        quoting=csv.QUOTE_MINIMAL)
    writer.writerow(["KERNEL", "ACCESS", "L1_MISS", "L2_MISS"])
    for kernel, kernel_result in kernel_results.items():
        writer.writerow(create_csv_row(kernel, kernel_result))
