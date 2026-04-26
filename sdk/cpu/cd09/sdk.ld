
  __local_irq_enable = ABSOLUTE(0x3f43f00);
  __local_irq_disable = ABSOLUTE(0x3f43f04);
  request_irq_rom = ABSOLUTE(0x3f43f08);
  p33_buf = ABSOLUTE(0x3f43f0c);
  p33_xor_1byte = ABSOLUTE(0x3f43f10);
  p33_and_1byte = ABSOLUTE(0x3f43f14);
  p33_or_1byte = ABSOLUTE(0x3f43f18);
  p33_tx_1byte = ABSOLUTE(0x3f43f1c);
  p33_rx_1byte = ABSOLUTE(0x3f43f20);
  P33_CON_SET = ABSOLUTE(0x3f43f24);
  memcpy = ABSOLUTE(0x3f43f28);
  memmove = ABSOLUTE(0x3f43f2c);
  memcmp = ABSOLUTE(0x3f43f30);
  memset = ABSOLUTE(0x3f43f34);
  strcmp = ABSOLUTE(0x3f43f38);
  strlen = ABSOLUTE(0x3f43f3c);
  strchr = ABSOLUTE(0x3f43f40);
  __iumoddi3 = ABSOLUTE(0x3f43f44);
  __divdf3 = ABSOLUTE(0x3f43f48);
  ic_pll_init = ABSOLUTE(0x3f43f4c);
  sfc_suspend = ABSOLUTE(0x3f43f50);
  sfc_resume = ABSOLUTE(0x3f43f54);
  sfc_drop_cache = ABSOLUTE(0x3f43f58);
  IcuCmdKst = ABSOLUTE(0x3f43f5c);
  IcuCmdSend = ABSOLUTE(0x3f43f60);
  IcuEnable = ABSOLUTE(0x3f43f64);
  IcuDisable = ABSOLUTE(0x3f43f68);
  IcuInitial = ABSOLUTE(0x3f43f6c);
  IcuLockRegion = ABSOLUTE(0x3f43f70);
  IcuFlushinvAll = ABSOLUTE(0x3f43f74);
  chip_crc16 = ABSOLUTE(0x3f43f78);
  CrcDecode = ABSOLUTE(0x3f43f7c);
  mask_init = ABSOLUTE(0x3f43f80);
  wdt_clear = ABSOLUTE(0x3f43f84);
  nvram_set_boot_state = ABSOLUTE(0x3f43f88);

  flash_poweron = ABSOLUTE(0x3f43f90);
  flash_poweroff = ABSOLUTE(0x3f43f94);
  the_debug_isr = ABSOLUTE(0x3f43f98);
  doe = ABSOLUTE(0x3f43f9c);
  LZ4_setStreamDecode = ABSOLUTE(0x3f43fa0);
  LZ4_decompress_safe_continue = ABSOLUTE(0x3f43fa4);
  LZ4_decompress_generic = ABSOLUTE(0x3f43fa8);
  LZ4_decompress_safe_usingDict = ABSOLUTE(0x3f43fac);
  dec_isd_cfg_ini = ABSOLUTE(0x3f43fb0);
  msfc_norflash_init = ABSOLUTE(0x3f43fb4);
  msfc_norflash_ioctl = ABSOLUTE(0x3f43fb8);
  msfc_norflash_entry_sleep = ABSOLUTE(0x3f43fbc);
  msfc_norflash_exit_sleep = ABSOLUTE(0x3f43fc0);
  msfc_norflash_enter_4byte_addr = ABSOLUTE(0x3f43fc4);
  msfc_get_norflash_uuid = ABSOLUTE(0x3f43fc8);
  msfc_norflash_erase = ABSOLUTE(0x3f43fcc);
  msfc_norflash_write = ABSOLUTE(0x3f43fd0);
  msfc_norflash_read = ABSOLUTE(0x3f43fd4);
  msfc_enter_spi_code = ABSOLUTE(0x3f405d8);
  msfc_exit_spi_code = ABSOLUTE(0x3f4063a);
  spi_cs = ABSOLUTE(0x3f405ee);
  spi_writebyte = ABSOLUTE(0x3f4061c);
  spi_readbyte = ABSOLUTE(0x3f406a2);
  norflash_send_addr = ABSOLUTE(0x3f40678);
  norflash_wait_ok = ABSOLUTE(0x3f406cc);
  norflash_write_enable = ABSOLUTE(0x3f40668);
  cbuf_read = ABSOLUTE(0x3f43fd8);
  cbuf_write = ABSOLUTE(0x3f43fdc);
  cbuf_is_write_able = ABSOLUTE(0x3f43fe0);
  cbuf_write_alloc = ABSOLUTE(0x3f43fe4);
  cbuf_write_updata = ABSOLUTE(0x3f43fe8);
  cbuf_read_alloc = ABSOLUTE(0x3f43fec);
  cbuf_read_updata = ABSOLUTE(0x3f43ff0);
  cbuf_init = ABSOLUTE(0x3f43ff4);
  cbuf_clear = ABSOLUTE(0x3f43ff8);
  cbuf_rewrite = ABSOLUTE(0x3f43ffc);
  g_boot_soft_flag = ABSOLUTE(0x3f09d00);
  efuse_page0 = ABSOLUTE(0x3f09d28);
  efuse_page1 = ABSOLUTE(0x3f09d30);
  efuse_page2 = ABSOLUTE(0x3f09d34);
  _IRQ_MEM_ADDR = ABSOLUTE(0x3f09f00);
  _RAM_LIMIT_L = ABSOLUTE(0x3f00000);
  _MASK_USED_MEM_END = ABSOLUTE(0x3f004f4);
  _MASK_EXPORT_MEM_BEGIN = ABSOLUTE(0x3f09d00);
  _MASK_EXPORT_MEM_SIZE = ABSOLUTE(0xf4);


UPDATA_SIZE = 0x80;
UPDATA_BEG = _MASK_EXPORT_MEM_BEGIN - UPDATA_SIZE;

RAM_LIMIT_L = _RAM_LIMIT_L;
RAM_LIMIT_H = UPDATA_BEG;
PHY_RAM_SIZE = RAM_LIMIT_H - RAM_LIMIT_L;


ISR_BASE = _IRQ_MEM_ADDR;
ROM_RAM_SIZE = _MASK_EXPORT_MEM_SIZE;
ROM_RAM_BEG = _MASK_EXPORT_MEM_BEGIN;

RAM0_BEG = RAM_LIMIT_L;
RAM0_END = RAM_LIMIT_H;
RAM0_SIZE = RAM0_END - RAM0_BEG;

MEMORY
{
 code0(rx) : ORIGIN = 0x4000100, LENGTH = 512k
 ram0(rwx) : ORIGIN = RAM0_BEG, LENGTH = RAM0_SIZE
}

ENTRY(_start)

SECTIONS
{
    . = ORIGIN(ram0);
    .boot_info ALIGN(4) : SUBALIGN(4)
    {
        *(.boot_info)
        . = ALIGN(4);
    } > ram0

    .irq_stack ALIGN(4) : SUBALIGN(4)
    {
        *(.stack_magic)
         _cpu0_sstack_begin = .;
        PROVIDE(cpu0_sstack_begin = .);
        *(.stack)
        _cpu0_sstack_end = .;
        PROVIDE(cpu0_sstack_end = .);
     _stack_end = . ;
  *(.stack_magic0)
        . = ALIGN(4);
    } > ram0

 .data ALIGN(4) : SUBALIGN(4)
 {
        *(.data_magic)
        *(.data*)
        *(*.data)
        . = ALIGN(4);
 } > ram0

    .noinit_bss ALIGN(4) : SUBALIGN(4)
    {
        *(.noinit)
        . = ALIGN(4);
    } > ram0

 .bss ALIGN(4) : SUBALIGN(4)
    {
        *(.bss)
        *(*.data.bss)
        *(.volatile_ram)
  *(.non_volatile_ram)
        . = ALIGN(4);
    } > ram0

 .data_code ALIGN(4) : SUBALIGN(4)
 {
        data_code_pc_limit_begin = .;
        *(.common*)
  *(.flushinv_icache)
        *(.cache)
        *(*.text.cache.L1)
        *(.port_wkup.text.cache.L2)
        *(.volatile_ram_code)


        *(.fat_data_code)
        . = ALIGN(4);
     data_code_pc_limit_end = .;
    } > ram0

 _HEAP_BEGIN = . ;
 _HEAP_END = RAM0_END;

    . = ORIGIN(code0);
    .text ALIGN(4) : SUBALIGN(4)
    {
        KEEP(*(.entry_text))
        *(.startup.text)
  *(.text)
  *(*.text)
        _SPI_CODE_START = . ;
        *(.vm_sfc.text.cache.L2)
        _SPI_CODE_END = . ;
        *(*.text.cache.L2)
        *(*.text.cache.L2.*)
  *(*.text.const)

  __VERSION_BEGIN = .;
        KEEP(*(.sys.version))
        __VERSION_END = .;

  . = ALIGN(4);
        hsb_critical_handler_begin = .;
        KEEP(*(.hsb_critical_txt))
        hsb_critical_handler_end = .;

  . = ALIGN(4);
        lsb_critical_handler_begin = .;
        KEEP(*(.lsb_critical_txt))
        lsb_critical_handler_end = .;

  . = ALIGN(4);
     lp_target_begin = .;
     PROVIDE(lp_target_begin = .);
     KEEP(*(.lp_target))
     lp_target_end = .;
     PROVIDE(lp_target_end = .);

        . = ALIGN(4);
        PROVIDE(device_node_begin = .);
        KEEP(*(.device))
        PROVIDE(device_node_end = .);

        . = ALIGN(4);
  *(.LOG_TAG_CONST*)
        *(.rodata*)
        . = ALIGN(4);
   } > code0
}


text_begin = ADDR(.text);
text_size = SIZEOF(.text);
text_end = text_begin + text_size;
ASSERT((text_size % 4) == 0,"!!! text_size Not Align 4 Bytes !!!");

bss_begin = ADDR(.bss);
bss_size = SIZEOF(.bss);
bss_end = bss_begin + bss_size;
ASSERT((bss_size % 4) == 0,"!!! bss_size Not Align 4 Bytes !!!");

data_addr = ADDR(.data);
data_begin = text_begin + text_size;
data_size = SIZEOF(.data);
data_end = data_addr + data_size;
ASSERT((data_size % 4) == 0,"!!! data_size Not Align 4 Bytes !!!");

data_code_addr = ADDR(.data_code);
data_code_begin = data_begin + data_size;
data_code_size = SIZEOF(.data_code);
data_code_end = data_code_addr + data_code_size;
ASSERT((data_code_size % 4) == 0,"!!! data_code_size Not Align 4 Bytes !!!");


PROVIDE(HEAP_BEGIN = _HEAP_BEGIN);
PROVIDE(HEAP_END = _HEAP_END);
_MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN;
PROVIDE(MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN);




report_text_beign = ADDR(.text);
report_text_size = SIZEOF(.text);
report_text_end = report_text_beign + report_text_size;

report_irq_stack_begin = ADDR(.irq_stack);
report_irq_stack_size = SIZEOF(.irq_stack);
report_irq_stack_end = report_irq_stack_begin + report_irq_stack_size;

report_data_begin = ADDR(.data);
report_data_size = SIZEOF(.data);
report_data_end = report_data_begin + report_data_size;

report_bss_begin = ADDR(.bss);
report_bss_size = SIZEOF(.bss);
report_bss_end = report_bss_begin + report_bss_size;

report_data_code_begin = ADDR(.data_code);
report_data_code_size = SIZEOF(.data_code);
report_data_code_end = report_data_code_begin + report_data_code_size;

report_heap_beign = _HEAP_BEGIN;
report_heap_size = _HEAP_END - _HEAP_BEGIN;
report_heap_end = _HEAP_END;
