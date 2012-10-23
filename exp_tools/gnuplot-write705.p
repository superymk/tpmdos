# Draw picture for WRITE40 experiment
set title "NVWRITE_40Bytes Performance"
set xlabel "Run"
set ylabel "Performance (us)"
set yrange[70000:900000]
#plot "~/tpmdos/tpm-data/gnuplot-write40-ML110" using 3:4
plot "~/tpmdos/tpm-data2/gnuplot-write705-garfield" using 3:4
#plot "~/tpmdos/tpm-data2/gnuplot-write705-garfield" using 3:7