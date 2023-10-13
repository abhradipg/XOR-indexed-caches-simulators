"""helper module"""

import os
import sys
import subprocess
import utils
import numpy as np
import pandas as pd
from argparse import *
from abc import ABC, abstractmethod

class Benchmark(ABC):

  def prepare(self, k, u):
    return k


  @abstractmethod
  def measure(self, k, u, timeout):
    pass


  def cleanup(self, k):
    pass


  def gen_header(self, n):
    return ["name"]


  def run(self, polybench, output, iterations, timeout):  
    ks = get_kernels(polybench)
    print("Measuring ", ks)

    utilities = abs_path(polybench, "utilities")
    columns = self.gen_header(iterations)
    result = []

    for (i, k) in enumerate(ks):
      name = kernel_name(k)
      tmp = self.prepare(k, utilities)

      print("Measuring " + name + "...")
      sys.stdout.flush()
      for j in range(iterations):
        s = self.measure(tmp, utilities, timeout)
        print([name] + s)
        result.append([name] + s)

      self.cleanup(tmp)

    df = pd.DataFrame(data=result, columns=columns)
    if output:
      df.to_csv(output, index=False)
    else:
      return df

  def run2(self, polybench, output, iterations, timeout, benchmark):
    ks = [abs_path(polybench, benchmark.strip())]
    print("Measuring ", ks)

    utilities = abs_path(polybench, "utilities")
    columns = self.gen_header(iterations)
    result = []

    for (i, k) in enumerate(ks):
      name = kernel_name(k)
      tmp = self.prepare(k, utilities)

      print("Measuring " + name + "...")
      sys.stdout.flush()
      for j in range(iterations):
        s = self.measure(tmp, utilities, timeout)
        print([name] + s)
        result.append([name] + s)

      self.cleanup(tmp)

    df = pd.DataFrame(data=result, columns=columns)
    if output:
      df.to_csv(output, mode='a', index=False)
    else:
      return df

def abs_path(*paths):
  return os.path.abspath(os.path.join(*paths))


def get_kernels(polybench, fk=None, tk=None):
  list_path = abs_path(polybench, "utilities/benchmark_list")

  with open(list_path) as f:
    kernels = f.readlines()

  if fk and tk:
    kernels = kernels[fk:tk]
  elif fk:
    kernels = kernels[fk:]
  elif tk:
    kernels = kernels[:tk]

  kernels = [abs_path(polybench, k.strip()) for k in kernels]

  return kernels


def kernel_name(k):
  return os.path.splitext(os.path.basename(os.path.normpath(k)))[0]


def execute_cmd(cmd, timeout=None):
  print(" -> executing: " + cmd)
  try:
    return subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True, timeout=timeout, universal_newlines=True)
  except subprocess.CalledProcessError as e:
    print(e.output)
    exit(-1)
  else:
    return None
