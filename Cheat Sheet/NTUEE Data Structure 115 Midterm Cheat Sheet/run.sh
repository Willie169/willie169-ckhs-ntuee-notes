#!/usr/bin/env bash

for f in ./*.py; do
  test -f "$f" && black -l 1000 "$f"
done
. ~/.bashrc
[ ${1:-} = '-p' ] || lualatexci 'NTUEE Data Structure 115 Midterm.TEX'
