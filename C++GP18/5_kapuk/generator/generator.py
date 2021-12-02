#!/bin/python
import random
import subprocess
import sys
import os

input_size = [2, 4, 5, 6, 8]
output_size = [1, 2, 5, 10, 20]
max_costs = [1000]
sample_sizes = [2, 4, 8, 16, 32, 64, 128, 256]
generator = os.path.splitext(sys.argv[0])[0]

if len(sys.argv) > 1:
    random.seed(sys.argv[1])

for i in input_size:
    for l in sample_sizes:
        if l > 2**i:
            continue
        for j in output_size:
            for k in max_costs:
                seed = random.randint(1, 1000000)
                base = "{}_{}_{}_{}_{}".format(i, j, k, l, seed)
                print base
                with open(base + ".txt", "w") as fin, open(base + ".refsol", "w") as fref:
                    cmd = [str(v) for v in [generator, i, j, k, l, seed]]
                    subprocess.call(cmd, stdout=fin, stderr=fref)
