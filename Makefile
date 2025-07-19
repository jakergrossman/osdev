#! make -f

BIN_DIR   ?= build-$(ARCH)
ISO       ?= $(BIN_DIR)/denton.iso

MAKEFLAGS += --no-print-directory

ARCH      ?= i686
TARGET    := $(ARCH)-elf
TAG       ?= denton-osdev-$(TARGET)
SDK       := $(TAG)_sdk.tar

QEMU      ?= qemu-system-$(ARCH)

ifneq (1,$(__OSDEV_ENV__))
RUNNER    := docker run -v .:/src --workdir /src --user=$(shell id -u):$(shell id -g) -t $(TAG)
endif

all: help

.SUFFIXES:
.PHONY: sdk setup compile sysroot iso qemu clean distclean help

sdk: $(BIN_DIR)/$(SDK) ## create cross-compilation SDK

setup: $(BIN_DIR) $(BIN_DIR)/.setup-stamp ## initialize meson build directory

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

setup: $(BIN_DIR)/.setup-stamp
	$(RUNNER) meson setup --cross-file=$(TARGET) $(BIN_DIR) --reconfigure

compile: setup $(BIN_DIR) ## compile operating system
	$(RUNNER) meson compile -C $(BIN_DIR) -v

sysroot: compile
	$(RUNNER) meson install -C $(BIN_DIR) --destdir sysroot

iso: sysroot ## create ISO image
	sh scripts/iso.sh $(BIN_DIR)/sysroot $(ISO)

qemu-%: iso ## run operating system with qemu
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
	docker build --build-arg ARCH=$(ARCH) --output type=tar,dest=$@ --progress=plain . $(DOCKERFLAGS)
	docker import $@ $(TAG)

$(BIN_DIR)/.%-stamp: $(BIN_DIR)
	touch $@

