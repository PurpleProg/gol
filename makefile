# ----------------------------
# Makefile Options
# ----------------------------

NAME = GoL
ICON = icon.png
DESCRIPTION = "Conway's game of life (by PurpleProg)"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
