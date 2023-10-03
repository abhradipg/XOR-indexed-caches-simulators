import sys
import os
import json

def get_file_names():
    assert len(sys.argv) > 1
    return sys.argv[1:]

def get_result_names():
    return [os.path.splitext(os.path.basename(file_name))[0]
            for file_name in get_file_names()]

def read_result_files():
    results = []
    for file_name in get_file_names():
        with open(file_name, 'r') as json_file:
            results.append(json.load(json_file))
    return results

def create_csv_file_name(addition):
    file_names = get_file_names()
    assert len(file_names) == 1
    json_file_name = file_names[0]
    csv_file_name = json_file_name.replace(".json", "-{}.csv".format(addition))
    return csv_file_name.replace("json", "csv")
