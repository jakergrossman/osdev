#! make -f

BIN_DIR   ?= build-$(ARCH)
ISO       ?= $(BIN_DIR)/denton.iso

MAKEFLAGS += --no-print-directory

ARCH      ?= aarch64
TARGET    := $(ARCH)-elf
IMAGE     ?= denton-osdev-$(TARGET)
TAG       ?= latest

QEMU      ?= qemu-system-$(ARCH)


RUNNER    := docker run \
		--volume=.:/src/project \
		--workdir=/src/project \
		--user=$(shell id -u):$(shell id -g) \
		-t $(IMAGE)

.SUFFIXES:
.PHONY: setup compile sysroot iso clean distclean help

all: help

help: ## show this help text
	@echo "Make targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
		| awk 'BEGIN {FS = ":.*?## "}; {printf "%-16s %s\n", $$1, $$2}'

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

setup: sdk-check $(BIN_DIR)
	$(RUNNER) meson setup --cross-file=$(TARGET) $(BIN_DIR) --reconfigure

compile: sdk-check $(BIN_DIR) ## compile operating system
	$(RUNNER) meson compile -C $(BIN_DIR) -v

sysroot: compile
	$(RUNNER) meson install -C $(BIN_DIR) --destdir sysroot

iso: sysroot ## create ISO image
	$(RUNNER) sh scripts/iso.sh $(BIN_DIR)/sysroot $(ISO)

clean: ## meson clean
	$(RUNNER) meson compile -C $(BIN_DIR) --clean

distclean: ## delete bin directory
	-rm -rf $(BIN_DIR)

sdk-check: ## check for cross-compiling docker image
	@if docker image inspect $(IMAGE) 2>&1 > /dev/null ; then \
		echo "toolchain: $(IMAGE) found" ; \
	else \
		echo "toolchain: $(IMAGE) docker image not found" && false ; \
	fi
