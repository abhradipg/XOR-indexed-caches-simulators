#!/usr/bin/env python3
import util
import sys
import csv

def get_kernel_duration_ms_pairs():
    json_ms_key = "timeMSec"
    results = util.read_result_files()
    assert len(results) == 1
    result = results[0]
    name_ms_pairs = [[kernel, result[kernel][json_ms_key]]
                     for kernel in result]
    return sorted(name_ms_pairs, key=lambda x: x[0])

def write_to_csv():
    csv_file_name = util.create_csv_file_name("duration")
    with open(csv_file_name, 'w', newline='') as csv_file:
        writer = csv.writer(csv_file, delimiter=' ', quotechar='|',
                            quoting=csv.QUOTE_MINIMAL)
        writer.writerow(["KERNEL", "DURATION_MS"])
        for row in get_kernel_duration_ms_pairs():
            writer.writerow(row)

write_to_csv()
