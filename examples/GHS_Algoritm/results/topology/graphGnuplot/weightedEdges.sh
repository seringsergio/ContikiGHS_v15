#!/bin/bash

echo "set terminal png
set output '$1'
set style arrow 1 front head filled size screen 0.03,15 lt 1 lw 1
middlearrow(from_x, from_y, to_x, to_y) = \
sprintf('set arrow from %f,%f to %f,%f as 1 nohead;', 0.5*(from_x + to_x), 0.5*(from_y + to_y), to_x, to_y).\
sprintf('set arrow from %f,%f to %f,%f as 1', from_x, from_y, 0.5*(from_x + to_x), 0.5*(from_y + to_y))
eval(middlearrow(0.1,0.1,0.9,0.9))
set xrange [0:1]
set yrange [0:1]
plot 0 " | gnuplot

xdg-open graph.png #opens the image
