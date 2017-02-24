#!/bin/bash

echo "set terminal png
set output '$1'
set xrange [-10:100]
set yrange [-100:0] writeback
plot 'edges.dat' with lines lc rgb 'black' lw 2 notitle, \
'edges.dat' u 1:2:(2.2) with circles linecolor rgb 'white' lw 2 fill solid border lc lt 0 notitle, \
'edges.dat' using 1:2:3 with labels offset (0,0) font 'Arial Bold' notitle " | gnuplot

xdg-open graph.png #opens the image
