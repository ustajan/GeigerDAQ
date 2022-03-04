#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import numpy as np
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("file", help="file of data points to visualize. NOTE:  ONLY reads in files with one column.")
parser.add_argument('-l', '--logy', action="store_true", help='use logarithmic y-axis scale')
parser.add_argument('-n', '--filter-min', default=0, type=int, help='values lower than this will be filtered')
parser.add_argument('-x', '--filter-max', default=sys.float_info.max, type=int, help='values higher than this will be filtered')
parser.add_argument('-b', '--bins', default=20, type=int, help='number of segments to use in the histogram')

args = parser.parse_args()

data = None
with open(args.file) as f:
	data = f.readlines()


times = [float(x[:-1]) for x in data]

diffs = []

for x in range(len(times) - 1):
	diffs.append(times[x + 1] - times[x])

diffs = [x for x in diffs if x < args.filter_max]
diffs = [x for x in diffs if x > args.filter_min]

print("Range:", min(diffs), max(diffs))

print(len(diffs), "data points")

a = np.array(diffs)
plt.hist(a, bins=args.bins)
if args.logy:
	plt.yscale('log')

plt.show()
