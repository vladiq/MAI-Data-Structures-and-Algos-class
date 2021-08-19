#!/usr/bin/python3

import sys
import random

n = int(sys.argv[1])

print(n)
for _ in range(n):
    print(random.randint(1, 3), end=' ')
