#!/usr/bin/env bash
set -e

qemu-system-x86_64 -m 4G -serial stdio -no-reboot -drive format=raw,file=build/disk.img