# Makefile for Dynaserve CLI

CC = cc
CFLAGS ?= -Wall -Wextra -O2
SRC = src/main.c src/help.c src/version.c src/update.c src/login.c src/utils.c
OUT = dynaserve

# Version can be set via make VERSION=1.0.0 or from git tag
VERSION ?= $(shell git describe --tags --abbrev=0 2>/dev/null || echo "0.0.0")

.PHONY: all clean install

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -Isrc -DVERSION=\"$(VERSION)\" $(SRC) -o $(OUT)
	@echo "Dynaserve CLI built successfully (v$(VERSION))"

clean:
	rm -f $(OUT) src/*.o

install: $(OUT)
	# Copy binary to /usr/local/bin (sudo may be required)
	sudo cp $(OUT) /usr/local/bin/
	@echo "Dynaserve CLI installed at /usr/local/bin/$(OUT) with version $(VERSION)"