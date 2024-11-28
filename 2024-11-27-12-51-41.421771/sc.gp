# LOG_FILE = /home/yoyo/fxmark-f2fs-custom/logs/2024-11-27-12-51-41.421771/fxmark.log
# SYSTEM = Linux saturn02 6.10.0+ #6 SMP PREEMPT_DYNAMIC Wed Nov 27 11:47:03 UTC 2024 x86_64 x86_64 x86_64 GNU/Linux
# DISK_SIZE = 64G
# DURATION = 60s
# DIRECTIO = bufferedio,directio
# MEDIA_TYPES = ssd,hdd,ramdisk,nvme,mem
# FS_TYPES = ext4,xfs,btrfs,f2fs
# BENCH_TYPES = DWAL,DWOL,DWOM,DWSL,DWTL,DRBH,DRBM,DRBL
# NCORES = 1,2,4,8,16,24,32
# CORE_SEQ = 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
# MODEL_NAME = Intel(R) Xeon(R) Gold 6226R CPU @ 2.90GHz
# PHYSICAL_CHIPS = 2
# CORE_PER_CHIP = 16
# SMT_LEVEL = 2
# NUM_TEST_CONF = 1

set term pdfcairo size 4.6in,2.3in font ',10'
set_out='set output "`if test -z $OUT; then echo sc.pdf; else echo $OUT; fi`"'
eval set_out
set multiplot layout 1,2

set title 'nvme:DWOM:directio'
set xlabel '# cores'
set ylabel 'M ops/sec'
plot [0:][0:] 'nvme:f2fs:DWOM:directio.dat' using 1:2 title 'f2fs' with lp ps 0.5

unset multiplot
set output
