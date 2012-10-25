# Draw picture for WRITE40 experiment
set title "NVWRITE_40Bytes Performance"
set xlabel "Run"
set ylabel "Performance (us)"
set yrange[500000:1000000]
plot "~/tpmdos/tpm-data2/gnuplot-write705-garfield" using 3:4, "~/tpmdos/tpm-data2/gnuplot-write705-garfield" using 3:7