import pathlib
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

#------------------------------------------------------------------------------#

patches = []

# Patch: rocket body
xy = [
    (0, 0),
    (6, 0),
    (6, 1),
    (0, 1)
]

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

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

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

# Patch: letter "p"
xy = [
    (3, 0),
    (4, 0),
    (4, 1),
    (5, 1),
    (5, 3),
    (3, 3)
]

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

# Patch: letter "p" hole
# Center is at (4, 2)
d  = 0.5 # Hole diameter
xy = [
    (4-d/2, 2-d/2),
    (4+d/2, 2-d/2),
    (4+d/2, 2+d/2),
    (4-d/2, 2+d/2)
]

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

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

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

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

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

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

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

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

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

# Rocket nosecone
xy = [
    (6, 0),
    (8.5, 0.5),
    (6, 1)
]

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

# Rocket fin bottom
xy = [
    (0, -0.5),
    (0.5, -0.5),
    (2, 0),
    (0, 0)
]

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

# Rocket fin top
xy = [
    (0, 1),
    (2, 1),
    (0.5, 1.5),
    (0, 1.5)
]

patches.append(Polygon(xy, facecolor='k', edgecolor='w'))

# Apply patches

(fig, ax) = plt.subplots()
fig.set_size_inches(6, 2)

for p in patches:
    ax.add_patch(p)

ax.set_xlim(0, 13)
ax.set_ylim(-0.5, 4)
ax.axis("equal")
ax.set_axis_off()
filePath = pathlib.Path(__file__).parent / "logo.svg"
plt.savefig(filePath, format="svg", transparent=True, bbox_inches='tight', pad_inches=0) # Must be before plt.show()
plt.show()
