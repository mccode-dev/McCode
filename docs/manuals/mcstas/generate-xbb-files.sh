#!/usr/bin/env bash
# Generate .xbb bounding-box files for every PDF/JPEG/PNG figure, needed by
# tex4ht's DVI-based image pipeline (unlike pdflatex, it can't read image
# geometry directly out of these formats). Run from the manual's build
# directory, before htlatex.
set -e
find figures -type f \( -iname '*.pdf' -o -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' \) -exec extractbb {} \; 2>/dev/null || true
