import fileinput

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap

lut = {
    0: 0,
    1: 110,
    2: 260,
    3: 340,
    4: 470,
    5: 620,
    6: 800,
    7: 870,
    8: 1020,
    9: 1140,
    10: 1250,
    11: 1370,
    12: 1470,
    13: 1550,
    14: 1670,
    15: 1800,
    16: 1900,
}


positions = []
currents = []
voltages = []
powers = []
statess = []

for line in fileinput.input():
    position, current, voltage, *states = map(int, line.split())
    positions.append(lut[position])
    currents.append(current)
    voltages.append(voltage)
    powers.append(current * voltage / 1000)
    statess.append(states)

statess = list(zip(*statess))

fig, ax = plt.subplots()

plt.plot(positions)
plt.plot(powers)
plt.plot(states)


X = [[i           for _ in range(17)] for i in range(len(statess[0]) + 1)]
Y = [[lut[j] - 50 for j in range(17)] for i in range(len(statess[0]) + 1)]

X = list(zip(*X))
Y = list(zip(*Y))

cmap = ListedColormap(["white", "green", "yellow", "red"])
c = ax.pcolorfast(X, Y, statess, cmap=cmap, alpha=0.5)

fig.tight_layout()
plt.show()
