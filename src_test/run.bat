@echo off
cd ..
python .\tools\kflash.py -B goE -b 1500000 -s -p COM7 .\src_test\image.bin
.\tools\plink -serial COM7 -sercfg 115200,8,1,N,N -mem