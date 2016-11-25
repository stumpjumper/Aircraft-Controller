#!/bin/sh

if [ "$1" == "" -o "$1" == "-h" -o "$1" == "--help" -o "$#" -ne 3 ]; then
  echo "usage: gnuplot_data.sh [-h|--help] <input_file> x-column y-column"
  echo "       Example: gnuplot_data.sh my.data 1 5"
  exit 0
fi
outfile=/tmp/gnuplot_data.dat
plotfile=/tmp/gnuplot_data.plt
sed -n -e 's#[:|,]# #gp' "$1" > ${outfile}
echo --- head -5 ${outfile} ----
head -5 ${outfile}
echo "plot '${outfile}' using ${2}:${3} with line" > ${plotfile}
echo "pause -1  'Hit return to continue'" >> ${plotfile}
echo --- ${plotfile} ----
cat ${plotfile}
gnuplot ${plotfile}



