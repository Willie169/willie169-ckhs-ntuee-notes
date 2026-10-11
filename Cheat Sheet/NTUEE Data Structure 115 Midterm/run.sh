#!/usr/bin/env bash

for f in ./*.py; do
  test -f "$f" && black -l 1000 "$f"
done
. ~/.bashrc
lualatexci 'NTUEE Data Structure 115 Midterm.tex'
