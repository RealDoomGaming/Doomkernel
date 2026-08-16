#!/usr/bin/env bash
set -e

make
qemu-system-x86_64 -m 4G -drive format=raw,file=build/disk.img