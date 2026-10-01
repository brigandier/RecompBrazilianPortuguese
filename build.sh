#!/usr/bin/env bash
set -e
mkdir -p ./bin
./RecompModTool ./mod.toml ./bin
echo "Created bin/translation_ptbr.nrm"
