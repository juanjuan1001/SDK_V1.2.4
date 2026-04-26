

if [ -f $1.bin ];
then rm $1.bin;
fi
if [ -f data.bin ];
then rm data.bin;
fi
if [ -f data_code.bin ];
then rm data_code.bin;
fi
if [ -f $1.lst ];
then rm $1.lst;
fi

${OBJDUMP} -D -address-mask=0x7ffffff -print-dbg $1.elf > $1.lst
${OBJCOPY} -O binary -j .text $1.elf text.bin
${OBJCOPY} -O binary -j .data $1.elf data.bin
${OBJCOPY} -O binary -j .data_code $1.elf data_code.bin
${OBJDUMP} -section-headers -address-mask=0x7ffffff $1.elf
${OBJSIZEDUMP} -lite -skip-zero -enable-dbg-info $1.elf | sort -k 1 > symbol_tbl.txt

cat text.bin data.bin data_code.bin > app.bin
/opt/utils/strip-ini -i isd_config.ini -o isd_config.ini
files="app.bin ${CPU}loader.* uboot* isd_config.ini"
host-client -project ${NICKNAME}$2 -f ${files} $1.elf
