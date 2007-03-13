echo "Merging file"
hadd tauall.root $1/out_singletau_*
#hadd tauall.root ScanOut_0_0/0_0_0/out_singletau_*
echo "producing Tau Benchmark Plot"
root -l -b makeTauPlot.C
