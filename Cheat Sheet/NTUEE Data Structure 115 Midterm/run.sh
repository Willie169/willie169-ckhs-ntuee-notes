#!/usr/bin/env bash

for f in ./*.py; do
  test -f "$f" && black "$f"
done
. ~/.bashrc
lualatexci 'NTUEE Data Structure 115 Midterm.tex'
