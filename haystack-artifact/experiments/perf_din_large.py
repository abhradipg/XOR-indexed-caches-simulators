""" run polybench on the large data set """

import os
import csv
import utils
import time
import re
import benchmark as bm
import pandas as pd

REPETITIONS = int(os.environ["REPETITIONS"])

POLYBENCH = "./benchmarks/polybench_large/"
INCLUDE = "./benchmarks/polybench_large/utilities"
EXECUTABLE = "./executables/"

OUTPUT = "./data/polybench_large/perf_din_large.csv"

class DineroBenchmark(bm.Benchmark):

  def prepare(self, k, u):
    name = bm.kernel_name(k)
    print("Compiling " + name + "...")

    parent_folder = os.path.dirname(k)
    polybench_c = bm.abs_path(u, "polybench.c")
    binary = EXECUTABLE + name + "dinero_timing"
    cmd = "gcc -O2 -I " + u + " -I " + parent_folder + " " + polybench_c + " " + k + " -lm -o " + binary
    bm.execute_cmd(cmd)

    scop = self.extract_scop(binary)

    return (binary, scop)


  def extract_scop(self, b):
    print("Extracting scop...")
    output = bm.execute_cmd("objdump -d " + b)
    
    sp = re.compile("callq\s+(.+)\s+<kernel_.+>\n\s+(.+):") # scop pattern   
    m = sp.search(output)
    if m:
      start = m.groups()[0]
      end = m.groups()[1]
      return (start, end)

    print("Couldn't extract the scop for " + b)
    return None


  def measure(self, info, u, timeout):
    (b, scop) = info
    if scop:
      (start, end) = scop

      env = "DINEROIV_CMDLINE=\"-l1-dsize 32768 -l1-dbsize 64 -l1-dassoc 8 -l1-dccc -l2-dsize 1048576 -l2-dbsize 64 -l2-dassoc 16 -l2-dccc -on-trigger " + start + " -off-trigger " + end + "\" DINEROIV_OUTPUTS=dinero " 
      cmd = env + "/usr/local/bin/qemu-x86_64 -tcg-plugin /usr/local/libexec/x86_64/user/tcg-plugin-dineroIV.so " + b

      time1 = time.time()
      bm.execute_cmd(cmd, timeout=timeout)
      time2 = time.time()

      return [time2 - time1]
  
    return [float("nan")]


  def cleanup(self, info):
    (b, _) = info
    os.remove(b)


  def gen_header(self, n):
    return ["kernel", "duration"]


if __name__ == "__main__":
    b = DineroBenchmark()
    b.run(POLYBENCH, OUTPUT, REPETITIONS, None)
