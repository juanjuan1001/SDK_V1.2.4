// *INDENT-OFF*
#include "app_config.h"
#include "maskrom_stubs.ld"

//============ About RAM ================
UPDATA_SIZE     = 0x80;
UPDATA_BEG      = _MASK_EXPORT_MEM_BEGIN - UPDATA_SIZE;

RAM_LIMIT_L     = _RAM_LIMIT_L;
RAM_LIMIT_H     = UPDATA_BEG;
PHY_RAM_SIZE    = RAM_LIMIT_H - RAM_LIMIT_L;

//from mask export
ISR_BASE       = _IRQ_MEM_ADDR;
ROM_RAM_SIZE   = _MASK_EXPORT_MEM_SIZE;
ROM_RAM_BEG    = _MASK_EXPORT_MEM_BEGIN;

RAM0_BEG 		= RAM_LIMIT_L;
RAM0_END 		= RAM_LIMIT_H;
RAM0_SIZE 		= RAM0_END - RAM0_BEG;

MEMORY
{
	code0(rx)    	  : ORIGIN = 0x4000100, LENGTH = 512k
	ram0(rwx)         : ORIGIN = RAM0_BEG,  LENGTH = RAM0_SIZE
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

//================== Section Info Export ====================//
text_begin  = ADDR(.text);
text_size   = SIZEOF(.text);
text_end    = text_begin + text_size;
ASSERT((text_size % 4) == 0,"!!! text_size Not Align 4 Bytes !!!");

bss_begin = ADDR(.bss);
bss_size  = SIZEOF(.bss);
bss_end    = bss_begin + bss_size;
ASSERT((bss_size % 4) == 0,"!!! bss_size Not Align 4 Bytes !!!");

data_addr = ADDR(.data);
data_begin = text_begin + text_size;
data_size =  SIZEOF(.data);
data_end = data_addr + data_size;
ASSERT((data_size % 4) == 0,"!!! data_size Not Align 4 Bytes !!!");

data_code_addr = ADDR(.data_code);
data_code_begin = data_begin + data_size;
data_code_size =  SIZEOF(.data_code);
data_code_end = data_code_addr + data_code_size;
ASSERT((data_code_size % 4) == 0,"!!! data_code_size Not Align 4 Bytes !!!");

//===================== HEAP Info Export =====================//
PROVIDE(HEAP_BEGIN = _HEAP_BEGIN);
PROVIDE(HEAP_END = _HEAP_END);
_MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN;
PROVIDE(MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN);

//============================================================//
//=== report section info begin:
//============================================================//
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

//============================================================//
//=== report section info end
//============================================================//

