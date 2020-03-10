rem @echo off
set ROOTPATH = D:\Work\K210-Linux0.11-VS\src_test
set C_INCLUDE_PATH = .:%ROOTPATH%\bsp\include:%ROOTPATH%\drivers\include
..\tools\bin\riscv64-unknown-elf-gcc.exe %*