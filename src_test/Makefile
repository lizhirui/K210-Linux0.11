CC = ..\tools\bin\riscv64-unknown-elf-gcc.exe
#CC = ..\tools\cc.bat
CFLAGS = -Wall -O -fstrength-reduce  -nostdinc -fomit-frame-pointer -I include
#CFLAGS += -finstrument-functions
CFLAGS += -mcmodel=medany -mabi=lp64f -march=rv64imafc -fno-common -ffunction-sections -fdata-sections -fstrict-volatile-bitfields
CFLAGS += -fno-zero-initialized-in-bss -ffast-math -fno-math-errno -fsingle-precision-constant -Os -ggdb -Wall
#CFLAGS += -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-int-to-pointer-cast 
CFLAGS += -Wno-multichar -Wextra -Werror=frame-larger-than=32768 -Wno-unused-parameter -Wno-sign-compare -Wno-error=missing-braces
CFLAGS += -Wno-error=return-type -Wno-error=pointer-sign -Wno-missing-braces -Wno-strict-aliasing -Wno-implicit-fallthrough -Wno-missing-field-initializers
CFLAGS += -Wno-error=comment -Wno-error=logical-not-parentheses -Wno-error=duplicate-decl-specifier -Wno-error=parentheses
CFLAGS += -nostartfiles
CFLAGS += -static
CFLAGS += -I $(ROOTPATH)\include -I $(ROOTPATH)\bsp\include -I $(ROOTPATH)\drivers\include -I $(ROOTPATH)\riscvfunc\include -I $(ROOTPATH)\trap\include
LCFLAGS = -Wl,--gc-sections
LCFLAGS += -Wl,-static
LCFLAGS += -Wl,-EL
LCFLAGS += -Wl,--no-relax
LCFLAGS += -Wl,-T linker.ld
LCFLAGS += -Wl,-M

CFLAGS_USER = -Wall -O -fstrength-reduce -fomit-frame-pointer -I include
#CFLAGS_USER  += -finstrument-functions
CFLAGS_USER  += -mcmodel=medany -mabi=lp64f -march=rv64imafc -fno-common -ffunction-sections -fdata-sections -fstrict-volatile-bitfields
CFLAGS_USER  += -fno-zero-initialized-in-bss -ffast-math -fno-math-errno -fsingle-precision-constant -Os -ggdb -Wall
#CFLAGS_USER  += -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-int-to-pointer-cast 
CFLAGS_USER  += -Wno-multichar -Wextra -Werror=frame-larger-than=32768 -Wno-unused-parameter -Wno-sign-compare -Wno-error=missing-braces
CFLAGS_USER  += -Wno-error=return-type -Wno-error=pointer-sign -Wno-missing-braces -Wno-strict-aliasing -Wno-implicit-fallthrough -Wno-missing-field-initializers
CFLAGS_USER  += -Wno-error=comment -Wno-error=logical-not-parentheses -Wno-error=duplicate-decl-specifier -Wno-error=parentheses
#CFLAGS_USER  += -nostartfiles
CFLAGS_USER  += -static
#CFLAGS_USER  += -I $(ROOTPATH)\include -I $(ROOTPATH)\bsp\include -I $(ROOTPATH)\drivers\include -I $(ROOTPATH)\riscvfunc\include -I $(ROOTPATH)\trap\include

LCFLAGS_USER = -Wl,--gc-sections
LCFLAGS_USER += -Wl,-static
LCFLAGS_USER += -Wl,-EL
LCFLAGS_USER += -Wl,--no-relax
LCFLAGS_USER += -Wl,-T linker_user.ld
LCFLAGS_USER += -Wl,-M
ROOTPATH = D:\Work\K210-Linux0.11-VS\src_test
GCCINCLUDEFLAGS = -I $(ROOTPATH)\libgcc\include -I $(ROOTPATH)\libgcc\config\riscv
LD = ../tools/bin/riscv64-unknown-elf-ld.exe
LDFLAGS	= -s -x -nostartfiles -static --gc-sections -static -EL --no-relax -T linker.ld
AS = ../tools/bin/riscv64-unknown-elf-as.exe
OBJCOPY = ../tools/bin/riscv64-unknown-elf-objcopy.exe
OBJDUMP = ..\tools\bin\riscv64-unknown-elf-objdump.exe
REMOTE_HOST = root@192.168.0.125
REMOTE_ROOT = ~/K210-Linux0.11-VS/
#SCP = D:\Tools\Git\usr\bin\sh.exe /D/Tools/Git/usr/bin/scp
#SSH = D:\Tools\Git\usr\bin\sh.exe /D/Tools/Git/usr/bin/ssh $(REMOTE_HOST)
SCP = D:\Tools\Git\usr\bin\scp.exe
SSH = D:\Tools\Git\usr\bin\ssh.exe $(REMOTE_HOST)
INCLUDEFILE = ./include/*.h ./bsp/include/*.h ./drivers/include/*.h ./riscvfunc/include/*.h ./trap/include/*.h
CSOURCEFILE = $(wildcard *.c ./bsp/*.c ./drivers/*.c ./riscvfunc/*.c ./startup/*.c ./trap/*.c ./kernel/*.c ./kernel/blk_drv/*.c ./kernel/chr_drv/*.c ./fs/*.c ./user/test/*.c)
SSOURCEFILE = $(patsubst %.c,%.S,$(CSOURCEFILE))
SOURCEFILE = $(CSOURCEFILE) $(SSOURCEFILE)
COBJFILE = $(patsubst %.c,%.o,$(CSOURCEFILE))
SOBJFILE = $(patsubst %.S,%.o,$(SSOURCEFILE))
#OBJFILE = $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(SOURCEFILE)))
DEPFILE = $(patsubst %.S,%.d,$(patsubst %.c,%.d,$(SOURCEFILE)))
BIN2AOUT = ..\tools\bin2aout.exe

image.bin : system.bin system.txt rootfs.bin Makefile
	$(SCP) system.bin $(REMOTE_HOST):$(REMOTE_ROOT)system.bin
	$(SCP) rootfs.bin $(REMOTE_HOST):$(REMOTE_ROOT)rootfs.bin
	$(SSH) "cd $(REMOTE_ROOT);touch img.bin;dd if=system.bin of=image.bin;dd if=rootfs.bin of=image.bin bs=1024 seek=500"
	$(SCP) $(REMOTE_HOST):$(REMOTE_ROOT)image.bin image.bin
	
rootfs.bin : Makefile ./user/test/test.aout
	$(SCP) ./user/test/test.aout $(REMOTE_HOST):$(REMOTE_ROOT)test.aout
	$(SSH) "export LD_LIBRARY_PATH="/opt/glibc-2.14/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}";mkdir -p $(REMOTE_ROOT);cd $(REMOTE_ROOT);touch rootfs.bin;dd if=/dev/zero of=rootfs.bin bs=1024 count=360;/sbin/mkfs.minix -n14 rootfs.bin 360;mkdir -p mnt;mount -o loop rootfs.bin mnt;cd mnt;mkdir dev;cd dev;mknod tty0 c 4 0;cd ..;mkdir bin;cd bin;cp ../../test.aout sh;chmod 777 sh;cd ..;cd ..;df -h mnt;umount mnt;"
	$(SCP) $(REMOTE_HOST):$(REMOTE_ROOT)rootfs.bin rootfs.bin

system.txt : system.elf
	$(OBJDUMP) -d $< > $@

system.bin : system.elf
	pathconv del system.bin
	$(OBJCOPY) -O binary $< $@

system.elf : ./root_dir.o ./bsp/bsp.o ./drivers/drivers.o ./riscvfunc/riscvfunc.o ./startup/startup.o ./trap/trap.o ./kernel/kernel.o ./mm/mm.o ./fs/fs.o
	pathconv del system.elf
	$(CC) $(CFLAGS) $(LCFLAGS) -o $@ $^ ..\tools\riscv64-unknown-elf\lib\libm.a ..\tools\riscv64-unknown-elf\lib\libatomic.a ..\tools\riscv64-unknown-elf\lib\libsim.a

./user/test/test.aout : ./user/test/test.bin
	pathconv del $@
	$(BIN2AOUT) $< ./user/test/test.txt $@

./user/test/test.bin : ./user/test/test.elf
	pathconv del $@
	$(OBJCOPY) -O binary $< $@

./user/test/test.elf : ./user/test/test_main.c
	pathconv del $@
	$(CC) $(CFLAGS_USER) $(LCFLAGS_USER) -o $@ $^ ..\tools\riscv64-unknown-elf\lib\libm.a ..\tools\riscv64-unknown-elf\lib\libatomic.a ..\tools\riscv64-unknown-elf\lib\libsim.a
	$(OBJDUMP) -d ./user/test/test.elf  > ./user/test/test.txt

./user/test/test.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./user/test/*.c ./user/test/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^
	
./root_dir.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./*.c ./*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

./bsp/bsp.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./bsp/*.c ./bsp/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^
	
./drivers/drivers.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./drivers/*.c ./drivers/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

./riscvfunc/riscvfunc.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./riscvfunc/*.c ./riscvfunc/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

./startup/startup.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./startup/*.c ./startup/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

./trap/trap.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./trap/*.c ./trap/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

./kernel/kernel.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./kernel/*.c ./kernel/*.S ./kernel/blk_drv/*.c ./kernel/blk_drv/*.S ./kernel/*.S ./kernel/chr_drv/*.c ./kernel/chr_drv/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

./mm/mm.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./mm/*.c ./mm/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

./fs/fs.o : $(patsubst %.S,%.o,$(patsubst %.c,%.o,$(wildcard ./fs/*.c ./fs/*.S)))
	pathconv del $@
	$(LD) -r -o $@ $^

%.o : %.c
	pathconv del $@
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : %.S
	pathconv del $@
	$(CC) $(CFLAGS) -o $@ -c $<

%.d : %.c
	pathconv del $@
	$(CC) $(CFLAGS) -M -MF $(patsubst %.c,%.d,$<) -c $<
	depfileconvert $(ROOTPATH) $(patsubst %.c,%.d,$<)

%.d : %.S
	pathconv del $@
	$(CC) $(CFLAGS) -M -MF $(patsubst %.S,%.d,$<) -c $<
	depfileconvert $(ROOTPATH) $(patsubst %.S,%.d,$<)

include $(DEPFILE)
	
clean:
	del *.o
	del .\bsp\*.o
	del .\drivers\*.o
	del .\riscvfunc\*.o
	del .\startup\*.o
	del .\trap\*.o
	del .\kernel\*.o
	del .\kernel\blk_drv\*.o
	del .\kernel\chr_dev\*.o
	del .\mm\*.o
	del *.d
	del .\bsp\*.d
	del .\drivers\*.d
	del .\riscvfunc\*.d
	del .\startup\*.d
	del .\trap\*.d
	del .\kernel\*.d
	del .\kernel\blk_drv\*.d
	del .\kernel\chr_dev\*.d
	del .\mm\*.d
	del system.elf
	del system.bin
	del rootfs.bin
	del image.bin