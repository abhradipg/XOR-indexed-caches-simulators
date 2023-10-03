#!/usr/bin/env python3
import csv
import util
import sys

data_type = sys.argv[1]
assert data_type == "reproduced"

data_path, plots_path, dataset = util.get_data_and_plots_paths()

INPUT = "{}/haystack/dinero-{}-l1-l2-miss.csv".format(data_path, dataset)
OUTPUT = "{}/miss/dinero-{}-l1-l2.csv".format(data_path, dataset)

rows = {}
with open(INPUT, newline='') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',', quotechar='|')
    for row in spamreader:
        if row[0] == "kernel":
            continue
        access = int(row[4])
        l1_miss = sum([int(x) for x in row[1:4]])
        l2_miss = sum([int(x) for x in row[5:8]])
        rows[row[0]] = [access, l1_miss, l2_miss]

rows = util.sort_dict(rows)

with open(OUTPUT, 'w', newline='') as csv_file:
    writer = csv.writer(csv_file, delimiter=' ', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    writer.writerow(["KERNEL", "ACCESS", "L1_MISS", "L2_MISS"])
    for kernel in rows.keys():
        writer.writerow([kernel] + rows[kernel])
