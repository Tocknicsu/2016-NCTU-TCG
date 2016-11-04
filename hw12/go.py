#!/usr/bin/env python3
import subprocess as sp

# sp.call("make", shell=True)

x = 0 
counter = 0

while True:
    sp.call('./play_game 10000', shell=True)
    counter += 1
    if counter % 100 == 0:
        sp.call('cp data weight/weight.backup_%s'%(x), shell=True)
        x += 1
