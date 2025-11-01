# Cross-platform Makefile for building the ToC MUD server
UNAME_S  := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
CC       ?= clang
CFLAGS   ?=
else
CC       ?= gcc
CFLAGS   ?=
endif

CFLAGS   += -std=gnu89 -O2 -w -fcommon -DROM

ifeq ($(UNAME_S),Darwin)
LDFLAGS  := -lm
else
LDFLAGS  := -lcrypt -lm
endif

SRC_DIR  := src
AREA_DIR := area
SRCS     := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(AREA_DIR)/*.c)
OBJS := $(filter-out $(SRC_DIR)/nicedb.o $(AREA_DIR)/resolve.o $(SRC_DIR)/webserver.o, $(SRCS:.c=.o))
TARGET   := merc
AREA_LINK := $(AREA_DIR)/$(TARGET)

.PHONY: all clean

all: $(TARGET) $(AREA_LINK)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(AREA_LINK): $(TARGET)
	ln -sf ../$(TARGET) $(AREA_LINK)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(AREA_DIR)/%.o: $(AREA_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(AREA_LINK)
