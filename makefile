# ----------------------------
# Makefile Options
# ----------------------------

NAME = gol
ICON = icon.png
DESCRIPTION = "Conway's game of life"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
