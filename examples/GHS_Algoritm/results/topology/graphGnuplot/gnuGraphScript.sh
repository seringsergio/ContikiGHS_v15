#!/bin/bash

echo "set terminal png
set output 'graph.png'
set xrange [0:50]
set yrange [0:50] writeback
plot 'edges.dat' with lines lc rgb 'black' lw 2 notitle, \
'edges.dat' u 1:2:(0.8) with circles linecolor rgb 'white' lw 2 fill solid border lc lt 0 notitle, \
'edges.dat' using 1:2:(sprintf('(%d, %d)', "$1", "$2")) with labels notitle" | gnuplot

xdg-open graph.png #opens the image

# 'edges.dat' using 1:2:3 with labels offset (0,0) font 'Arial Bold' notitle \
