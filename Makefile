# Top-level Makefile for ToC MUD on Ubuntu 24.04+
CC       := gcc
CFLAGS   := -std=gnu89 -O2 -Wall -fcommon -DROM
LDFLAGS  := -lcrypt -lm

SRC_DIR  := src
AREA_DIR := area
SRCS     := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(AREA_DIR)/*.c)
OBJS := $(filter-out $(SRC_DIR)/nicedb.o $(AREA_DIR)/resolve.o $(SRC_DIR)/webserver.o, $(SRCS:.c=.o))
TARGET   := merc

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(AREA_DIR)/%.o: $(AREA_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
