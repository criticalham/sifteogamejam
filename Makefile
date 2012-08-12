APP = exploration

include $(SDK_DIR)/Makefile.rules
APP = exploration

include $(SDK_DIR)/Makefile.defs

OBJS = $(ASSETS).gen.o mapgen.o game.o gamecube.o main.o
ASSETDEPS += *.png $(ASSETS).lua

include $(SDK_DIR)/Makefile.rules
