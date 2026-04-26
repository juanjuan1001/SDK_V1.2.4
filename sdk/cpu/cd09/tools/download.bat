

set ELF_NAME=%1%
cd /d %~dp0

set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe

%OBJDUMP% -d -print-imm-hex -print-dbg %ELF_NAME%.elf > %ELF_NAME%.lst
%OBJCOPY% -O binary -j .text %ELF_NAME%.elf %ELF_NAME%.bin
%OBJCOPY% -O binary -j .data %ELF_NAME%.elf data.bin
%OBJCOPY% -O binary -j .data_code %ELF_NAME%.elf data_code.bin
%OBJDUMP% -section-headers %ELF_NAME%.elf

@echo *******************************************************************************************************
@echo CD09 SDK
@echo *******************************************************************************************************
@echo % date %
cd / d % ~dp0

copy /b %ELF_NAME%.bin+data.bin+data_code.bin app.bin
packres.exe -n tone -o tone.cfg ump3_tone -normal
isd_download.exe -tonorflash -dev cd09 -boot 0x3f01000 -div8 -wait 300 -uboot uboot.boot -uboot_compress -app app.bin -res F_UNIC.PIX tone.cfg -key AC690XFC3E.key

::-format all
::utcrc.exe > log.txt
@REM 常用命令说明
@rem -format vm
@rem -format all
@rem -reboot 500
@rem -erase-otp-cfg

ping / n 2 127.1 > null
IF EXIST null del null
