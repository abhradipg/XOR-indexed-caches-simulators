"""counts the data cache misses of the polybench kernels using dinero"""

import benchmark as bm
import os
import re
import sys

BENCHMARK = sys.argv[1]
POLYBENCH = "./benchmarks/polybench_large/"
BINARY = "./data/polybench_large/8way/"
OUTPUT = "./data/polybench_large/3level_8way.csv"
ITERATIONS = 1

DINERO_ARGS = "-l1-dsize 32768 -l1-dbsize 64 -l1-dassoc 8 -l1-dccc -l1-drepl l -l2-dsize 524288 -l2-dbsize 64 -l2-dassoc 8 -l2-dccc -l2-drepl l -l3-dsize 4194304 -l3-dbsize 64 -l3-dassoc 16 -l3-dccc -l3-drepl l"

class PolyBenchDineroBenchmark(bm.Benchmark):

  def prepare(self, k, u):
    name = bm.kernel_name(k)
    print("Compiling " + name + "...")

    parent_folder = os.path.dirname(k)
    polybench_c = bm.abs_path(u, "polybench.c")
    binary = BINARY + name + "euler_assoc"
    cmd = "gcc -O2 -I " + u + " -I " + parent_folder + " " + polybench_c + " " + k + " -lm -o " + binary
    bm.execute_cmd(cmd)

    scop = self.extract_scop(binary)

    return (scop, binary)


  def extract_scop(self, b):
    print("Extracting scop...")
    output = bm.execute_cmd("objdump -d " + b)
    
    sp = re.compile("callq\s+(.+)\s+<kernel_.+>\n\s+(.+):") # start pattern   
    m = sp.search(output)

    if m == None:
      print("Couldn't extract the scop for " + b)
      return None

    start = m.groups()[0]

    ep = re.compile(start + "\s<kernel_.+:[\s\S]+?^(.+):.+ret", re.MULTILINE) # start pattern   
    m = ep.search(output)

    if m == None:
      print("Couldn't extract the scop for " + b)
      return None

    end = m.groups()[0]

    return (start, end)


  def measure(self, info, u, timeout):
    (scop, binary) = info

    if not scop:
      print("Invalid scop, aborting...")
      return [float("nan")]

    (start, end) = scop
    cmd = "DINEROIV_CMDLINE=\"" + DINERO_ARGS + "\" DINEROIV_OUTPUTS=trace /usr/local/bin/qemu-x86_64 -tcg-plugin /usr/local/libexec/x86_64/user/tcg-plugin-dineroIV.so " + binary + " 2>&1 | dineroIV " + DINERO_ARGS + " -on-trigger " + start + " -off-trigger " + end + " 2>&1"
    output = bm.execute_cmd(cmd, timeout=timeout)
    
    comp = re.compile("Compulsory misses\s+(\d+)") 
    capp = re.compile("Capacity misses\s+(\d+)") 
    conp = re.compile("Conflict misses\s+(\d+)")
    defp = re.compile("Demand Fetches\s+(\d+)")
    ps = [comp, capp, conp, defp]
    ms = [p.findall(output) for p in ps]
    
    if None not in ms:
      vals = [ms[0][0], ms[1][0], ms[2][0], ms[3][0], ms[0][1], ms[1][1], ms[2][1], ms[3][1],ms[0][2], ms[1][2], ms[2][2], ms[3][2]]
      return [int(v) for v in vals]

    print("Couldn't extract the cache misses for " + b)


  def cleanup(self, info):
    (scop, binary) = info
    os.remove(binary)


  def gen_header(self, n):
    return ["kernel", "compulsory1", "capacity1", "conflict1", "demand_fetches1", "compulsory2", "capacity2", "conflict2", "demand_fetches2","compulsory3", "capacity3", "conflict3", "demand_fetches3"]
      

if __name__ == "__main__":
  b = PolyBenchDineroBenchmark()
  if BENCHMARK == "all":
    b.run(POLYBENCH, OUTPUT, ITERATIONS, None)
  else :
    b.run2(POLYBENCH, OUTPUT, ITERATIONS, None, BENCHMARK)
