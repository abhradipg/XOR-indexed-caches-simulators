"""counts the data cache misses of the polybench kernels using papi"""

import benchmark as bm
import os

TIMEOUT = 10800
POLYBENCH = "./benchmarks/polybench_large/"
BINARY = "./data/polybench_large/"
OUTPUT = "acc_papi_O2.csv"
ITERATIONS = 1

class PolyBenchPAPIBenchmark(bm.Benchmark):

  def prepare(self, k, u):
    name = bm.kernel_name(k)
    print("Compiling " + name + "...")

    parent_folder = os.path.dirname(k)
    polybench_c = bm.abs_path(u, "polybench.c")
    binary = BINARY + name
    cmd = "gcc -O2 -I " + u + " -I " + parent_folder + " " + polybench_c + " " + k + " -lm -DPOLYBENCH_PAPI -lpapi -o " + binary
    bm.execute_cmd(cmd)

    return binary


  def measure(self, b, u, timeout):
    output = bm.execute_cmd("./" + b, timeout=timeout)
    return [int(s) for s in output.split()]


  def cleanup(self, b):
    os.remove(b)


  def gen_header(self, n):
    return ["kernel", "PAPI_L1_DCM", "PAPI_L2_DCM", "PAPI_L1_LDM", "PAPI_L2_LDM"]
      

if __name__ == "__main__":
  b = PolyBenchPAPIBenchmark()
  b.run(POLYBENCH, OUTPUT, ITERATIONS, TIMEOUT)
