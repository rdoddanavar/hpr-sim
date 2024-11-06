import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

patches = []
lines   = []

# Patch: rocket body
xy = [
    (0, 0),
    (6, 0),
    (6, 1),
    (0, 1)
]

patches.append(Polygon(xy, facecolor='0.5', edgecolor='0.5'))

# Patch: letter "h"
xy = [
    (0, 1),
    (1, 1),
    (1, 2),
    (2, 2),
    (2, 1),
    (3, 1),
    (3, 3),
    (1, 3),
    (1, 4),
    (0, 4)
]

patches.append(Polygon(xy, facecolor='k'))

# Line: "h" -> "p"
x = [3, 3]
y = [1, 3]

lines.append((x, y))

# Patch: letter "p"
xy = [
    (3, 0),
    (4, 0),
    (4, 1),
    (5, 1),
    (5, 3),
    (3, 3)
]

patches.append(Polygon(xy, facecolor='k'))

# Line: "p" -> "r"
x = [5, 5]
y = [1, 3]

lines.append((x, y))

# Letter "r"
xy = [
    (5, 1),
    (6, 1),
    (6, 2),
    (7, 2),
    (7, 3),
    (5, 3),
    (5, 1)
]

patches.append(Polygon(xy, facecolor='k'))

# Line: "r" -> "s"
x = [7, 7]
y = [1, 3]

lines.append((x, y))

# Letter "s"
xy = [
    (6, 0),
    (9, 0),
    (9, 2),
    (8, 2),
    (9, 2),
    (9, 3),
    (7, 3),
    (7, 1),
    (8, 1),
    (6, 1)
]

patches.append(Polygon(xy, facecolor='k'))

# Letter "i"
xy = [
    ( 9, 0),
    (10, 0),
    (10, 2),
    ( 9, 2),
    (10, 2),
    (10, 3),
    ( 9, 3)
]

patches.append(Polygon(xy, facecolor='k'))

# Letter "m"
xy = [
    (10, 0),
    (11, 0),
    (11, 2),
    (11, 0),
    (12, 0),
    (12, 2),
    (12, 0),
    (13, 0),
    (13, 3),
    (10, 3)
]

patches.append(Polygon(xy, facecolor='k'))

# Rocket nosecone
xy = [
    (6, 0),
    (8, 0.5),
    (6, 1)
]

patches.append(Polygon(xy, facecolor='r'))

# Rocket fin bottom
xy = [
    (0, -0.5),
    (0.5, -0.5),
    (2, 0),
    (0, 0)
]

patches.append(Polygon(xy, facecolor='r'))

# Rocket fin top
xy = [
    (0, 1),
    (2, 1),
    (0.5, 1.5),
    (0, 1.5)
]

patches.append(Polygon(xy, facecolor='r'))

# Rocket fin middle
x = [0, 2]
y = [0.5, 0.5]

# Apply patches

(fig, ax) = plt.subplots()

for p in patches:
    ax.add_patch(p)

#ax.plot(x, y, color='r')

for l in lines:
    ax.plot(l[0], l[1], color='w', linewidth=0.5)

#ax.set_xlim(-2, 14)
#ax.set_ylim(-2, 5)
ax.axis("equal")
#ax.set_axis_off()
plt.show()
