#!/usr/bin/env python3
import util
import csv

def get_kernel_miss_tuples():
    json_miss_key = 'missCounts'
    json_access_key = 'accessCounts'
    results = util.read_result_files()
    assert len(results) == 1
    result = results[0]
    name_miss_tuples = []
    for kernel in result:
        misses = result[kernel][json_miss_key]
        accesses = result[kernel][json_access_key]
        el = [kernel, accesses[0], misses[0]]
        if len(accesses) == 2:
            el += [accesses[1], misses[1]]
        name_miss_tuples.append(el)
    return sorted(name_miss_tuples, key=lambda x: x[0])

def write_to_csv():
    csv_file_name = util.create_csv_file_name("miss")
    with open(csv_file_name, 'w', newline='') as csv_file:
        writer = csv.writer(csv_file, delimiter=' ', quotechar='|',
                            quoting=csv.QUOTE_MINIMAL)
        kernel_miss_tuples = get_kernel_miss_tuples()
        if len(kernel_miss_tuples[0]) == 3:
            writer.writerow(["KERNEL", "L1_ACCESS", "L1_MISS"])
        else:
            assert len(kernel_miss_tuples[0]) == 5
            writer.writerow(["KERNEL",
                             "L1_ACCESS", "L1_MISS",
                             "L2_ACCESS", "L2_MISS"])
        for row in kernel_miss_tuples:
            writer.writerow(row)

write_to_csv()
