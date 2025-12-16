set terminal pngcairo size 1200,800 enhanced font "Arial,12"
set output "graph1.png"

set grid
set xlabel "x"
set ylabel "y"
set title "f(x) = sin(x)"
set key top right box

set xrange [-5:5]
set yrange [5:-5]

set style line 1 lc rgb "blue" lw 2
set style line 2 lc rgb "red" lw 2
set style line 3 lc rgb "green" lw 2

f(x) = sin(x)
df(x) = cos(x)
t(x) = x - 1.66667 * (x)**3

plot f(x) with lines ls 1 title "f(x)", \
     df(x) with lines ls 2 title "f'(x)", \
     t(x) with lines ls 3 title "Ряд Тейлора T(x)"

set output