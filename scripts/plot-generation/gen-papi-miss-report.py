#!/usr/bin/env python3
import csv
import numpy
import util
import sys

data_type = sys.argv[1]
assert data_type == "reproduced"

data_path, plots_path, dataset = util.get_data_and_plots_paths()

INPUT = "{}/papi/{}-o2.csv".format(data_path, dataset)
OUTPUT = "{}/miss/papi-{}-o2.csv".format(data_path, dataset)

# This function below is taken from the HayStack artifact (haystack-artifact/experiments/utils.py).
def fix_overflow(number):
    if number > sys.maxsize:
        while number > 0:
            number -= sys.maxsize
        number = -number
    return number

rows = {}
with open(INPUT, newline="") as csvfile:
    spamreader = csv.reader(csvfile, delimiter=" ", quotechar="|")
    for row in spamreader:
        kernel, l1_miss, l2_miss = row[0], row[1], row[2]
        if kernel.lower() == "kernel":
            continue
        if kernel not in rows:
            rows[kernel] = [[], []]
        rows[kernel][0].append(fix_overflow(int(l1_miss)))
        rows[kernel][1].append(fix_overflow(int(l2_miss)))

l1_median = {}
l2_median = {}

for kernel, result_rows in rows.items():
    l1_median[kernel] = int(round(numpy.median(result_rows[0])))
    l2_median[kernel] = int(round(numpy.median(result_rows[1])))

l1_median = util.sort_dict(l1_median)
l2_median = util.sort_dict(l2_median)

def create_csv_row(kernel):
    return [kernel, -1, l1_median[kernel], l2_median[kernel]]

with open(OUTPUT, "w", newline="") as csv_file:
    writer = csv.writer(csv_file, delimiter=" ", quotechar="|", quoting=csv.QUOTE_MINIMAL)
    writer.writerow(["KERNEL", "ACCESS", "L1_MISS", "L2_MISS"])
    for kernel in l1_median.keys():
        writer.writerow(create_csv_row(kernel))
