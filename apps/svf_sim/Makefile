# svf_sim - JTAG Waveform Simulator
#
# Builds svf_sim using libxsvf sources (SVF mode only).
#
# Usage:
#   make              - build svf_sim
#   make clean        - remove build artifacts
#
# Manual build (without make):
#   gcc -o svf_sim svf_sim.c ../libxsvf/svf.c ../libxsvf/tap.c \
#       ../libxsvf/play.c ../libxsvf/statename.c ../libxsvf/memname.c \
#       -DLIBXSVF_WITHOUT_XSVF -DLIBXSVF_WITHOUT_SCAN -Wall -O2

CC = gcc
CFLAGS = -Wall -O2 -DLIBXSVF_WITHOUT_XSVF -DLIBXSVF_WITHOUT_SCAN

LIBXSVF_DIR = ../libxsvf

LIBXSVF_SRCS = \
	$(LIBXSVF_DIR)/svf.c \
	$(LIBXSVF_DIR)/tap.c \
	$(LIBXSVF_DIR)/play.c \
	$(LIBXSVF_DIR)/statename.c \
	$(LIBXSVF_DIR)/memname.c

TARGET = svf_sim

all: $(TARGET)

$(TARGET): svf_sim.c $(LIBXSVF_SRCS) $(LIBXSVF_DIR)/libxsvf.h
	$(CC) $(CFLAGS) -o $@ svf_sim.c $(LIBXSVF_SRCS)

clean:
	rm -f $(TARGET) $(TARGET).exe

.PHONY: all clean
