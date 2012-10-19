# Draw picture for WRITE40 experiment
set title "NVWRITE_40Bytes Performance"
set xlabel "Run"
set ylabel "Performance (us)"
set yrange[300000:800000]
plot "~/tpmdos/tpm-data/gnuplot-write40-ML110" using 3:4