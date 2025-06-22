#! make -f

BIN_DIR   ?= build
ISO       ?= $(BIN_DIR)/denton.iso

MAKEFLAGS += --no-print-directory

ARCH      ?= i686
TARGET    := $(ARCH)-elf
TAG       ?= denton-osdev-$(TARGET)
SDK       := $(TAG)_sdk.tar
QEMU      ?= # up to you

RUNNER    := docker run -v .:/src --workdir /src --user=$(shell id -u):$(shell id -g) -t $(TAG)

all: help

.SUFFIXES:
.PHONY: sdk setup compile sysroot iso qemu clean distclean help

sdk: $(BIN_DIR)/$(SDK) ## create cross-compilation SDK

setup: ## initialize meson build directory
	$(RUNNER) meson setup --cross-file=$(TARGET) $(BIN_DIR) --wipe

compile: $(BIN_DIR) ## compile operating system
	$(RUNNER) meson compile -C $(BIN_DIR)

sysroot: compile
	$(RUNNER) meson install -C $(BIN_DIR) --destdir sysroot

iso: sysroot ## create ISO image
	sh scripts/iso.sh $(BIN_DIR)/sysroot $(ISO)

qemu: iso ## run operating system with qemu
	$(QEMU) -no-reboot -no-shutdown -cdrom $(ISO)

clean: ## meson clean
	$(RUNNER) meson compile -C $(BIN_DIR) --clean

distclean: ## delete bin directory
	-rm -r $(BIN_DIR)

help: ## show this help text
	@echo "Usage: osdev.mk [subcommand]"
	@echo
	@echo "Subcommands:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
		| awk 'BEGIN {FS = ":.*?## "}; {printf "  %-16s %s\n", $$1, $$2}'

$(BIN_DIR)/$(SDK): $(BIN_DIR)
	docker build --output type=tar,dest=$@ --progress=plain .
	docker import $@ $(TAG)

$(BIN_DIR):
	mkdir -p $@
