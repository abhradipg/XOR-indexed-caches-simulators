#!/bin/bash

# collect dinero performance
echo "-> collecting dinero performance data"
python perf_din_large.py

# collect dinero output
echo "-> collecting dinero cache miss data"
python acc_din_euler_assoc_large.py
python acc_din_full_assoc_large.py


